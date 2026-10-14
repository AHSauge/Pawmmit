//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#include "Query.h"
#include "FTS.h"
#include "GenericLexer.h"
#include "git/Commit.h"
#include <QDate>
#include <QTimeZone>

namespace {

// Matches nothing, for queries that can't match any commit.
const QByteArray kNoMatch = "0";

// Selects the commits that touch a path matching the condition on p.
const QByteArray kPathCommits =
    "c.id IN (SELECT cp.commit_id FROM commits_paths cp "
    "JOIN paths p ON p.id = cp.path_id WHERE ";

// Caps how many indexed terms a wildcard expands to.
const int kMaxWildcardTerms = 1000;

bool isFtsField(Index::Field field) {
  switch (field) {
    case Index::Any:
    case Index::Author:
    case Index::Message:
    case Index::Scope:
    case Index::Addition:
    case Index::Deletion:
    case Index::Comment:
    case Index::String:
    case Index::Identifier:
      return true;
    default:
      return false;
  }
}

// Quotes a term so that FTS5 treats it as text rather than syntax.
QByteArray ftsQuote(const QByteArray &term) {
  return '"' + QByteArray(term).replace('"', "\"\"") + '"';
}

// The normalised words of a search term, split the same way as indexed text.
QByteArrayList termWords(const QString &text) {
  QByteArray utf8 = text.toUtf8();
  QByteArrayList words =
      isAscii(utf8) ? QByteArrayList{utf8} : segmentWords(utf8);
  for (QByteArray &word : words)
    word = normaliseToken(word);
  return words;
}

// Quotes a search term for FTS5. A term the segmenter splits into several
// words, like Chinese text, becomes a phrase so the words must be adjacent.
QByteArray ftsTerm(const QString &text) {
  return ftsQuote(termWords(text).join(kTokenSeparator));
}

// Restricts an FTS5 expression to the columns of the field.
QByteArray ftsFilter(Index::Field field, const QByteArray &expression) {
  QByteArrayList columns = ftsColumns(field);
  if (columns.isEmpty())
    return expression;
  return '{' + columns.join(' ') + "} : " + expression;
}

// Committer dates are compared in the committer's own time zone, so a day
// starts at midnight UTC in local time, which is time + tz_offset.
qint64 dayStart(const QDate &date) {
  return date.startOfDay(QTimeZone::utc()).toSecsSinceEpoch();
}

// Git abbreviates commit ids to at least this many hex digits.
const int kMinIdDigits = 7;

// Selects the commits matching the FTS5 expression.
QByteArray ftsCondition(const QByteArray &expression, QList<SqlParam> &params) {
  params.append(SqlText{expression});
  return "c.id IN (SELECT rowid FROM fts WHERE fts MATCH ?)";
}

// The upper bound of a range that contains all strings with the prefix.
QByteArray prefixEnd(QByteArray prefix) {
  prefix.back() = char(uchar(prefix.back()) + 1);
  return prefix;
}

class StarredQuery : public Query {
public:
  QString toString() const override { return "is:starred"; }

  QList<Index::Term> terms() const override { return QList<Index::Term>(); }

  QByteArray condition(const Index &index,
                       QList<SqlParam> &params) const override {
    QByteArrayList placeholders;
    for (const git::Commit &commit : index.repo().starredCommits()) {
      // A deep copy, since toByteArray() only wraps the id's own storage.
      git::Id id = commit.id();
      params.append(
          SqlBlob{QByteArray(id.toByteArray().constData(), id.getSize())});
      placeholders.append("?");
    }

    if (placeholders.isEmpty())
      return kNoMatch;
    return "c.oid IN (" + placeholders.join(", ") + ')';
  }
};

class TermQuery : public Query {
public:
  TermQuery(const Index::Term &term) : mTerm(term) {}

  QString toString() const override {
    return QString("%1:%2").arg(Index::fieldName(mTerm.field), mTerm.text);
  }

  QList<Index::Term> terms() const override { return {mTerm}; }

  // Plain terms also match data outside the FTS table, so they can't be
  // combined into a single MATCH.
  bool isFts() const override {
    return mTerm.field != Index::Any && isFtsField(mTerm.field);
  }

  QByteArray ftsExpression() const override {
    return ftsFilter(mTerm.field, ftsTerm(mTerm.text));
  }

  QByteArray condition(const Index &index,
                       QList<SqlParam> &params) const override {
    QByteArray result;
    switch (mTerm.field) {
      case Index::Email:
        params.append(SqlText{token()});
        return "c.email = ?";

      case Index::Path:
        params.append(SqlText{token()});
        return kPathCommits + "p.path = ?)";

      case Index::File:
        params.append(SqlText{token()});
        return kPathCommits + "p.file = ?)";

      case Index::Id:
        result = idCondition(index, 1, params);
        break;

      case Index::Date:
        result = dateCondition(params);
        break;

      case Index::Any:
        return anyCondition(index, params);

      default:
        return Query::condition(index, params);
    }

    return result.isEmpty() ? kNoMatch : result;
  }

protected:
  QByteArray token() const { return normaliseToken(mTerm.text.toUtf8()); }

  Index::Term mTerm;

private:
  // Plain terms match any of a commit's data, like they did with the old
  // index. Each alternative appends its parameters in the order it's listed.
  QByteArray anyCondition(const Index &index, QList<SqlParam> &params) const {
    QByteArray term = token();
    QByteArrayList alternatives = {ftsCondition(ftsTerm(mTerm.text), params)};

    params.append(SqlText{term});
    alternatives.append("c.email = ?");

    params.append(SqlText{term});
    params.append(SqlText{term});
    alternatives.append(kPathCommits + "p.path = ? OR p.file = ?)");

    // Short words like "add" are valid hex, so a plain term needs as many
    // digits as git abbreviates ids to before it's matched against them.
    QByteArray id = idCondition(index, kMinIdDigits, params);
    if (!id.isEmpty())
      alternatives.append(id);

    QByteArray date = dateCondition(params);
    if (!date.isEmpty())
      alternatives.append(date);

    return '(' + alternatives.join(") OR (") + ')';
  }

  // Matches commits whose id starts with the hex digits, or returns nothing
  // if the term isn't a valid id prefix.
  QByteArray idCondition(const Index &index, int minDigits,
                         QList<SqlParam> &params) const {
    QByteArray prefix = mTerm.text.toLatin1().toLower();
    int digits = git::Id::getSize(index.repo().oidType()) * 2;
    bool hex = std::all_of(prefix.begin(), prefix.end(), [](char ch) {
      return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f');
    });
    if (prefix.size() < minDigits || prefix.size() > digits || !hex)
      return QByteArray();

    QByteArray first = prefix.leftJustified(digits, '0');
    QByteArray last = prefix.leftJustified(digits, 'f');
    params.append(SqlBlob{QByteArray::fromHex(first)});
    params.append(SqlBlob{QByteArray::fromHex(last)});
    return "c.oid BETWEEN ? AND ?";
  }

  // Matches commits made on the day, or returns nothing if the term isn't a
  // date.
  QByteArray dateCondition(QList<SqlParam> &params) const {
    QDate date = QDate::fromString(mTerm.text, Index::dateFormat());
    if (!date.isValid())
      return QByteArray();

    params.append(dayStart(date));
    params.append(dayStart(date.addDays(1)));
    return "c.time + c.tz_offset >= ? AND c.time + c.tz_offset < ?";
  }
};

class DateRangeQuery : public TermQuery {
public:
  DateRangeQuery(const Index::Term &term) : TermQuery(term) {}

  QByteArray condition(const Index &index,
                       QList<SqlParam> &params) const override {
    QDate date = QDate::fromString(mTerm.text, Index::dateFormat());
    if (!date.isValid())
      return kNoMatch;

    // Neither bound includes the given day itself.
    switch (mTerm.field) {
      case Index::Before:
        params.append(dayStart(date));
        return "c.time + c.tz_offset < ?";
      case Index::After:
        params.append(dayStart(date.addDays(1)));
        return "c.time + c.tz_offset >= ?";
      default:
        return kNoMatch;
    }
  }
};

class WildcardQuery : public TermQuery {
public:
  WildcardQuery(const Index::Term &term) : TermQuery(term) {}

  bool isFts() const override { return TermQuery::isFts() && isPrefix(); }

  QByteArray ftsExpression() const override {
    return ftsFilter(mTerm.field, ftsTerm(mTerm.text.chopped(1)) + '*');
  }

  QByteArray condition(const Index &index,
                       QList<SqlParam> &params) const override {
    if (isFts())
      return Query::condition(index, params);

    // Wildcards use the same syntax as GLOB.
    switch (mTerm.field) {
      case Index::Email:
        params.append(SqlText{token()});
        return "c.email GLOB ?";

      case Index::Path:
        params.append(SqlText{token()});
        return kPathCommits + "p.path GLOB ?)";

      case Index::File:
        params.append(SqlText{token()});
        return kPathCommits + "p.file GLOB ?)";

      default:
        break;
    }

    if (!isFtsField(mTerm.field))
      return kNoMatch;

    QByteArrayList alternatives;
    QByteArray fts = ftsWildcard(index, params);
    if (!fts.isEmpty())
      alternatives.append(fts);

    // Plain wildcards also match emails, paths and file names.
    if (mTerm.field == Index::Any) {
      QByteArray pattern = token();
      params.append(SqlText{pattern});
      alternatives.append("c.email GLOB ?");

      params.append(SqlText{pattern});
      params.append(SqlText{pattern});
      alternatives.append(kPathCommits + "p.path GLOB ? OR p.file GLOB ?)");
    }

    if (alternatives.isEmpty())
      return kNoMatch;
    return '(' + alternatives.join(") OR (") + ')';
  }

private:
  // A single trailing * is a prefix query, which FTS5 handles itself.
  bool isPrefix() const {
    QByteArray pattern = token();
    return pattern.size() > 1 && pattern.indexOf('*') == pattern.size() - 1 &&
           !pattern.contains('?') && !pattern.contains('[');
  }

  // Matches the pattern against the FTS columns, or returns nothing if no
  // indexed term matches it.
  QByteArray ftsWildcard(const Index &index, QList<SqlParam> &params) const {
    if (isPrefix())
      return ftsCondition(ftsExpression(), params);

    // Other patterns are expanded into the matching indexed terms.
    QByteArrayList terms =
        index.indexedTerms(ftsColumns(mTerm.field), token(), kMaxWildcardTerms);
    if (terms.isEmpty())
      return QByteArray();

    QByteArrayList quoted;
    for (const QByteArray &term : terms)
      quoted.append(ftsQuote(term));
    return ftsCondition(ftsFilter(mTerm.field, '(' + quoted.join(" OR ") + ')'),
                        params);
  }
};

class PhraseQuery : public Query {
public:
  PhraseQuery(const QList<Index::Term> &terms) : mTerms(terms) {}

  QString toString() const override {
    QStringList terms;
    for (const Index::Term &term : mTerms)
      terms.append(term.text);
    Index::Field field = mTerms.first().field;
    return QString("%1:\"%2\"").arg(Index::fieldName(field), terms.join(" "));
  }

  QList<Index::Term> terms() const override { return mTerms; }

  bool isFts() const override { return isFtsField(mTerms.first().field); }

  // The tokeniser splits the quoted text into a phrase of tokens.
  QByteArray ftsExpression() const override {
    QByteArrayList tokens;
    for (const Index::Term &term : mTerms)
      tokens.append(termWords(term.text));
    return ftsFilter(mTerms.first().field,
                     ftsQuote(tokens.join(kTokenSeparator)));
  }

private:
  QList<Index::Term> mTerms;
};

class BooleanQuery : public Query {
public:
  enum Kind { And, Or };

  BooleanQuery(Kind kind, const QueryRef &lhs, const QueryRef &rhs)
      : mKind(kind), mLhs(lhs), mRhs(rhs) {}

  QString toString() const override {
    QString kind = (mKind == And) ? "AND" : "OR";
    return QString("%1 %2 %3").arg(mLhs->toString(), kind, mRhs->toString());
  }

  QList<Index::Term> terms() const override {
    return mLhs->terms() + mRhs->terms();
  }

  bool isFts() const override { return mLhs->isFts() && mRhs->isFts(); }

  QByteArray ftsExpression() const override {
    return '(' + mLhs->ftsExpression() + ')' + op() + '(' +
           mRhs->ftsExpression() + ')';
  }

  QByteArray condition(const Index &index,
                       QList<SqlParam> &params) const override {
    if (isFts())
      return Query::condition(index, params);

    // The left side goes first, so the parameters stay in order.
    QByteArray lhs = mLhs->condition(index, params);
    QByteArray rhs = mRhs->condition(index, params);
    return '(' + lhs + ')' + op() + '(' + rhs + ')';
  }

private:
  QByteArray op() const { return (mKind == And) ? " AND " : " OR "; }

  Kind mKind;
  QueryRef mLhs;
  QueryRef mRhs;
};

class PathspecQuery : public TermQuery {
public:
  PathspecQuery(const Index::Term &term) : TermQuery(term) {}

  // Matches everything below a directory, or paths matching a wildcard.
  QByteArray condition(const Index &index,
                       QList<SqlParam> &params) const override {
    QByteArray pattern = token();
    QByteArray prefix = pattern.endsWith('/') ? pattern : pattern + '/';
    params.append(SqlText{prefix});
    params.append(SqlText{prefixEnd(prefix)});
    params.append(SqlText{pattern});
    return kPathCommits + "(p.path >= ? AND p.path < ?) OR p.path GLOB ?)";
  }
};

bool isOperator(const Lexer::Lexeme &lexeme, const QByteArray &chars) {
  return (lexeme.token == Lexer::Operator && chars.contains(lexeme.text));
}

QByteArray getOperators(Index::Field field) {
  switch (field) {
      // Accept a limited set of special characters for emails
    case Index::Email:
      return ".+@";

    // Plain terms also match emails, so keep addresses whole
    case Index::Any:
      return "/.*?@";

    default:
      return "/.*?";
  }
}

QueryRef parse(QList<Lexer::Lexeme> &lexemes, Index::Field start = Index::Any) {
  QueryRef result;
  while (!lexemes.isEmpty()) {
    QueryRef query;
    Lexer::Lexeme lexeme = lexemes.takeFirst();

    // Parse OR operator.
    BooleanQuery::Kind kind = BooleanQuery::And;
    if (lexeme.token == Lexer::Identifier && lexeme.text == "OR") {
      // Advance to next token.
      if (lexemes.isEmpty())
        break;
      lexeme = lexemes.takeFirst();

      // Set kind.
      kind = BooleanQuery::Or;
    }

    // Parse field qualifier.
    Index::Field field = start;
    if (!lexemes.isEmpty() && isOperator(lexemes.first(), ":")) {
      QByteArray key = lexeme.text;

      // Discard : and advance to next token.
      lexemes.removeFirst();
      if (lexemes.isEmpty())
        break;
      lexeme = lexemes.takeFirst();

      // Set field.
      if (key == Index::fieldName(Index::Id)) {
        field = Index::Id;
      } else if (key == Index::fieldName(Index::Author)) {
        field = Index::Author;
      } else if (key == Index::fieldName(Index::Email)) {
        field = Index::Email;
      } else if (key == Index::fieldName(Index::Message) || key == "msg") {
        field = Index::Message;
      } else if (key == Index::fieldName(Index::Date)) {
        field = Index::Date;
      } else if (key == Index::fieldName(Index::Path)) {
        field = Index::Path;
      } else if (key == Index::fieldName(Index::File)) {
        field = Index::File;
      } else if (key == Index::fieldName(Index::Scope)) {
        field = Index::Scope;
      } else if (key == Index::fieldName(Index::Addition) || key == "added") {
        field = Index::Addition;
      } else if (key == Index::fieldName(Index::Deletion) || key == "deleted") {
        field = Index::Deletion;
      } else if (key == Index::fieldName(Index::Comment)) {
        field = Index::Comment;
      } else if (key == Index::fieldName(Index::String)) {
        field = Index::String;
      } else if (key == Index::fieldName(Index::Identifier) || key == "ident") {
        field = Index::Identifier;
      } else if (key == Index::fieldName(Index::Is)) {
        field = Index::Is;
      } else if (key == Index::fieldName(Index::Before)) {
        field = Index::Before;
      } else if (key == Index::fieldName(Index::After)) {
        field = Index::After;
      } else if (key == Index::fieldName(Index::Pathspec)) {
        field = Index::Pathspec;
      } else {
        continue;
      }
    }

    if (isOperator(lexeme, "(")) {
      query = parse(lexemes, field);
    } else if (isOperator(lexeme, ")")) {
      return result;
    } else if (lexeme.token == Lexer::Number) {
      // Parse date.
      QByteArray text = lexeme.text;
      while (!lexemes.isEmpty() && isOperator(lexemes.first(), "/")) {
        text += lexemes.takeFirst().text;
        if (lexemes.isEmpty() || lexemes.first().token != Lexer::Number)
          break;
        text += lexemes.takeFirst().text;
      }

      Index::Term term(field, text);
      if (field == Index::Before || field == Index::After) {
        query = std::make_shared<DateRangeQuery>(term);
      } else {
        query = std::make_shared<TermQuery>(term);
      }

    } else if (lexeme.token == Lexer::String) {
      // Strip off quotes.
      QByteArray unquoted = lexeme.text.mid(1);
      unquoted.chop(1);

      GenericLexer lexer;
      lexer.setSegmenting(false);
      if (lexer.lex(unquoted)) {
        QList<Index::Term> terms;
        while (lexer.hasNext()) {
          Lexer::Lexeme lexeme = lexer.next();
          if (lexeme.token == Lexer::Identifier)
            terms.append({field, lexeme.text});
        }

        if (!terms.isEmpty())
          query = std::make_shared<PhraseQuery>(terms);
      }

    } else if (lexeme.token == Lexer::Identifier) {
      const QByteArray operators = getOperators(field);
      // Parse file path and wildcards.
      QByteArray text = lexeme.text;
      while (!lexemes.isEmpty() && isOperator(lexemes.first(), operators)) {
        while (!lexemes.isEmpty() && isOperator(lexemes.first(), operators))
          text += lexemes.takeFirst().text;
        if (!lexemes.isEmpty() && lexemes.first().token == Lexer::Identifier)
          text += lexemes.takeFirst().text;
      }

      Index::Term term(field, text);
      if (field == Index::Is) {
        if (text == "starred")
          query = std::make_shared<StarredQuery>();
      } else if (field == Index::Pathspec) {
        query = std::make_shared<PathspecQuery>(term);
      } else if (text.contains('*') || text.contains('?')) {
        query = std::make_shared<WildcardQuery>(term);
      } else {
        query = std::make_shared<TermQuery>(term);
      }
    }

    // Form boolean query.
    if (query)
      result =
          !result
              ? query
              : QueryRef(std::make_shared<BooleanQuery>(kind, result, query));
  }

  return result;
}

} // namespace

QByteArray Query::condition(const Index &index, QList<SqlParam> &params) const {
  Q_UNUSED(index)

  if (!isFts())
    return kNoMatch;
  return ftsCondition(ftsExpression(), params);
}

QueryRef Query::parseQuery(const QString &query) {
  // Parse into list of terms.
  // Handle key:() and key:"" with embedded spaces.
  // Terms stay whole here, and are split when the SQL is built, since emails
  // and paths are matched whole.
  GenericLexer lexer;
  lexer.setSegmenting(false);
  QList<Lexer::Lexeme> lexemes;
  if (lexer.lex(query.toUtf8())) {
    while (lexer.hasNext())
      lexemes.append(lexer.next());
  }

  return parse(lexemes);
}

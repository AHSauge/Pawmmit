//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "FTS.h"
#include <QTextBoundaryFinder>
#include <algorithm>
#include <utility>

void CommitDocument::append(FTSColumn column, QByteArrayView token) {
  QByteArray &buf = columns[size_t(column)];
  if (buf.isEmpty() == false)
    buf += kTokenSeparator;
  buf += token;
  tokenCount++;
}

FTSColumn columnFor(Index::Field field, Subfield sub) {
  switch (field) {
    case Index::Message:
      return sub == Subfield::String ? FTSColumn::MessageString
                                     : FTSColumn::MessageIdent;

    case Index::Scope:
      switch (sub) {
        case Subfield::Comment:
          return FTSColumn::ScopeComment;

        case Subfield::String:
          return FTSColumn::ScopeString;

        case Subfield::Identifier:
        default:
          return FTSColumn::ScopeIdent;
      }

    case Index::Addition:
      switch (sub) {
        case Subfield::Comment:
          return FTSColumn::AdditionComment;

        case Subfield::String:
          return FTSColumn::AdditionString;

        case Subfield::Identifier:
        default:
          return FTSColumn::AdditionIdent;
      }

    case Index::Deletion:
      switch (sub) {
        case Subfield::Comment:
          return FTSColumn::DeletionComment;

        case Subfield::String:
          return FTSColumn::DeletionString;

        case Subfield::Identifier:
        default:
          return FTSColumn::DeletionIdent;
      }

    default:
      Q_UNREACHABLE();
  }
}

bool isAscii(QByteArrayView text) {
  return std::all_of(text.begin(), text.end(),
                     [](char c) { return uchar(c) < 0x80; });
}

QByteArray normaliseToken(QByteArrayView token) {
  if (isAscii(token) || token.isValidUtf8() == false)
    return token.toByteArray().toLower();

  // NFKC matches text that only differs in its encoding, like a decomposed "å"
  // or full-width "ＡＢＣ". It runs before folding, since it can produce upper
  // case letters, and again after, since folding can undo it.
  return QString::fromUtf8(token)
      .normalized(QString::NormalizationForm_KC)
      .toCaseFolded()
      .normalized(QString::NormalizationForm_KC)
      .toUtf8();
}

QByteArrayList segmentWords(QByteArrayView text) {
  QByteArrayList words;

  // Bytes that aren't UTF-8, like Latin-1, can't be segmented. Keep the ASCII
  // words between them, as they were before.
  if (!text.isValidUtf8()) {
    QByteArray word;
    for (char ch : text) {
      if (uchar(ch) < 0x80 && (QChar::isLetterOrNumber(uchar(ch)) || ch == '_'))
        word += ch;
      else if (!word.isEmpty())
        words.append(std::exchange(word, QByteArray()));
    }
    if (!word.isEmpty())
      words.append(word);
    return words;
  }

  QString string = QString::fromUtf8(text);
  QTextBoundaryFinder finder(QTextBoundaryFinder::Word, string);
  qsizetype start = 0;
  for (qsizetype end = finder.toNextBoundary(); end >= 0;
       end = finder.toNextBoundary()) {
    // Han text only has break opportunities rather than words, so keep every
    // segment that starts with a letter or number. Letters outside the BMP
    // are surrogate pairs.
    if (end > start) {
      char32_t first = string.at(start).unicode();
      if (QChar::isHighSurrogate(first) && start + 1 < end)
        first = QChar::surrogateToUcs4(string.at(start), string.at(start + 1));
      if (QChar::isLetterOrNumber(first))
        words.append(string.sliced(start, end - start).toUtf8());
    }
    start = end;
  }

  return words;
}

QByteArrayList ftsColumns(Index::Field field) {
  auto matches = [](auto predicate) {
    QByteArrayList columns;
    for (const char *name : kFTSColumnNames)
      if (predicate(QByteArray(name)))
        columns.append(name);
    return columns;
  };

  switch (field) {
    case Index::Author:
      return {kFTSColumnNames[size_t(FTSColumn::Author)]};

    case Index::Message:
      return matches(
          [](const QByteArray &n) { return n.startsWith("message_"); });

    case Index::Scope:
      return matches(
          [](const QByteArray &n) { return n.startsWith("scope_"); });

    case Index::Addition:
      return matches(
          [](const QByteArray &n) { return n.startsWith("addition_"); });

    case Index::Deletion:
      return matches(
          [](const QByteArray &n) { return n.startsWith("deletion_"); });

    case Index::Comment:
      return matches(
          [](const QByteArray &n) { return n.endsWith("_comment"); });

    case Index::String:
      return matches([](const QByteArray &n) { return n.endsWith("_string"); });

    case Index::Identifier:
      return matches([](const QByteArray &n) { return n.endsWith("_ident"); });

    default:
      return {};
  }
}

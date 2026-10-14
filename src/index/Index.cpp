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

#include "FTS.h"
#include "Index.h"
#include "IndexDatabase.h"
#include "GenericLexer.h"
#include "LPegLexer.h"
#include "Query.h"
#include "git/Commit.h"
#include "git/Config.h"
#include "git/Diff.h"
#include "git/Patch.h"
#include "git/Reference.h"
#include "git/RevWalk.h"
#include "git/Signature.h"
#include <sqlite3.h>
#include <QLockFile>
#include <QSettings>
#include <QtConcurrent>

#include <stdexcept>

namespace {

const QString kLogKey = "debug/indexer";

const QString kIndexDir = "index";

} // namespace

bool Index::sLoggingEnabled = false;

Index::Index(const git::Repository &repo, QObject *parent)
    : QObject(parent), mRepo(repo) {
  // Read log setting.
  sLoggingEnabled = QSettings().value(kLogKey).toBool();

  // TODO: Check version (when we have a newer schema) and TBD migrate

  // Read data from disk.
  reset();
}

bool Index::isValid() const { return mDB != nullptr; }

void Index::reset() {
  mDB = index_database::open(mRepo, true);

  // Lists the indexed terms, for expanding wildcards. A temporary table, since
  // the database itself is opened read-only.
  if (mDB)
    sqlite3_exec(mDB.get(),
                 "CREATE VIRTUAL TABLE temp.fts_vocab USING "
                 "fts5vocab(main, fts, 'col')",
                 nullptr, nullptr, nullptr);

  emit indexReset();
}

bool Index::remove() {
  mDB.reset();
  if (index_database::remove(mRepo)) {
    reset();
    return true;
  } else {
    return false;
  }
}

QList<git::Commit> Index::commits(const QString &filter) const {
  if (filter.isEmpty())
    return QList<git::Commit>();

  // Parse query.
  QueryRef query = Query::parseQuery(filter);
  if (!query)
    return QList<git::Commit>();

  return commits(*query);
}

QList<git::Commit> Index::commits(const Query &query) const {
  // Keeps the connection open even if reset() replaces it meanwhile.
  std::shared_ptr<sqlite3> db = mDB;
  if (!db)
    return QList<git::Commit>();

  QList<SqlParam> params;
  QByteArray sql = "SELECT c.oid FROM commits c WHERE " +
                   query.condition(*this, params) + " ORDER BY c.time DESC";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db.get(), sql.constData(), int(sql.size() + 1), &stmt,
                         nullptr) != SQLITE_OK) {
    qWarning() << "Failed to prepare search" << sql << ":"
               << sqlite3_errmsg(db.get());
    return QList<git::Commit>();
  }

  // The parameters outlive the statement, so SQLite needn't copy them.
  for (int i = 0; i < params.size(); ++i) {
    const SqlParam &param = params.at(i);
    if (const qint64 *value = std::get_if<qint64>(&param)) {
      sqlite3_bind_int64(stmt, i + 1, *value);
    } else if (const SqlText *text = std::get_if<SqlText>(&param)) {
      sqlite3_bind_text(stmt, i + 1, text->value.constData(),
                        text->value.size(), SQLITE_STATIC);
    } else if (const SqlBlob *blob = std::get_if<SqlBlob>(&param)) {
      sqlite3_bind_blob(stmt, i + 1, blob->value.constData(),
                        blob->value.size(), SQLITE_STATIC);
    }
  }

  QList<git::Commit> commits;
  git_oid_t type = mRepo.oidType();
  int rc;
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    const char *oid = static_cast<const char *>(sqlite3_column_blob(stmt, 0));
    int size = sqlite3_column_bytes(stmt, 0);
    // Commits that are no longer in the repository are skipped.
    if (git::Commit commit =
            mRepo.lookupCommit(git::Id(QByteArray(oid, size), type)))
      commits.append(commit);
  }

  if (rc != SQLITE_DONE)
    qWarning() << "Failed to search the index:" << sqlite3_errmsg(db.get());
  sqlite3_finalize(stmt);
  return commits;
}

QByteArrayList Index::indexedTerms(const QByteArrayList &columns,
                                   const QByteArray &pattern, int limit) const {
  std::shared_ptr<sqlite3> db = mDB;
  if (!db)
    return QByteArrayList();

  QByteArray sql = "SELECT DISTINCT term FROM temp.fts_vocab WHERE term GLOB ?";
  QByteArrayList params = {pattern};

  // The literal start of the pattern narrows the range of terms to check.
  qsizetype literal = 0;
  while (literal < pattern.size() &&
         !QByteArrayView("*?[").contains(pattern.at(literal)))
    ++literal;
  QByteArray prefix = pattern.left(literal);
  if (!prefix.isEmpty() && uchar(prefix.back()) < 0xff) {
    QByteArray end = prefix;
    end.back() = char(uchar(end.back()) + 1);
    sql += " AND term >= ? AND term < ?";
    params << prefix << end;
  }

  if (!columns.isEmpty()) {
    sql +=
        " AND col IN (?" + QByteArray(", ?").repeated(columns.size() - 1) + ')';
    params.append(columns);
  }

  sql += " ORDER BY term LIMIT " + QByteArray::number(limit);

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db.get(), sql.constData(), int(sql.size() + 1), &stmt,
                         nullptr) != SQLITE_OK) {
    qWarning() << "Failed to prepare term lookup:" << sqlite3_errmsg(db.get());
    return QByteArrayList();
  }

  for (int i = 0; i < params.size(); ++i)
    sqlite3_bind_text(stmt, i + 1, params.at(i).constData(),
                      params.at(i).size(), SQLITE_STATIC);

  QByteArrayList terms;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *term =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    terms.append(QByteArray(term, sqlite3_column_bytes(stmt, 0)));
  }
  sqlite3_finalize(stmt);
  return terms;
}

QStringList Index::suggestions(Field field, const QString &prefix,
                               int limit) const {
  QByteArray start = normaliseToken(prefix.toUtf8());
  QByteArrayList values;
  switch (field) {
    case Email:
      values = startingWith("commits", "email", start, limit);
      break;

    case Path:
      values = startingWith("paths", "path", start, limit);
      break;

    case File:
      values = startingWith("paths", "file", start, limit);
      break;

    case Any:
      values = startingWith("temp.fts_vocab", "term", start, limit) +
               startingWith("paths", "file", start, limit) +
               startingWith("paths", "path", start, limit) +
               startingWith("commits", "email", start, limit);
      std::sort(values.begin(), values.end());
      values.erase(std::unique(values.begin(), values.end()), values.end());
      if (limit >= 0 && values.size() > limit)
        values.resize(limit);
      break;

    default: {
      // Escaped, since the prefix is matched as a GLOB pattern.
      QByteArray pattern;
      for (char ch : start)
        pattern += QByteArrayView("*?[").contains(ch)
                       ? '[' + QByteArray(1, ch) + ']'
                       : QByteArray(1, ch);
      QByteArrayList columns = ftsColumns(field);
      if (!columns.isEmpty())
        values = indexedTerms(columns, pattern + '*', limit);
      break;
    }
  }

  QStringList suggestions;
  for (const QByteArray &value : values)
    suggestions.append(QString::fromUtf8(value));
  return suggestions;
}

QMap<Index::Field, QStringList> Index::fieldMap(const QString &prefix,
                                                int limit) const {
  QMap<Field, QStringList> map;
  for (Field field : {Author, Email, Message, File, Path, Scope, Addition,
                      Deletion, Comment, String, Identifier}) {
    QStringList values = suggestions(field, prefix, limit);
    if (!values.isEmpty())
      map.insert(field, values);
  }

  return map;
}

QByteArrayList Index::startingWith(const QByteArray &table,
                                   const QByteArray &column,
                                   const QByteArray &prefix, int limit) const {
  std::shared_ptr<sqlite3> db = mDB;
  if (!db)
    return QByteArrayList();

  // A range on the column, so that its index is used.
  QByteArrayList params;
  QByteArray sql = "SELECT DISTINCT " + column + " FROM " + table;
  if (!prefix.isEmpty()) {
    sql += " WHERE " + column + " >= ?";
    params.append(prefix);
    if (uchar(prefix.back()) < 0xff) {
      QByteArray end = prefix;
      end.back() = char(uchar(end.back()) + 1);
      sql += " AND " + column + " < ?";
      params.append(end);
    }
  }
  sql += " ORDER BY " + column + " LIMIT " + QByteArray::number(limit);

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db.get(), sql.constData(), int(sql.size() + 1), &stmt,
                         nullptr) != SQLITE_OK) {
    qWarning() << "Failed to prepare suggestions:" << sqlite3_errmsg(db.get());
    return QByteArrayList();
  }

  for (int i = 0; i < params.size(); ++i)
    sqlite3_bind_text(stmt, i + 1, params.at(i).constData(),
                      params.at(i).size(), SQLITE_STATIC);

  QByteArrayList values;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *value =
        reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
    values.append(QByteArray(value, sqlite3_column_bytes(stmt, 0)));
  }
  sqlite3_finalize(stmt);
  return values;
}

QString Index::dateFormat() { return "yyyy/MM/dd"; }

QByteArray Index::fieldName(Index::Field field) {
  switch (field) {
    case Index::Id:
      return "id";
    case Index::Author:
      return "author";
    case Index::Email:
      return "email";
    case Index::Message:
      return "message";
    case Index::Date:
      return "date";
    case Index::Path:
      return "path";
    case Index::File:
      return "file";
    case Index::Scope:
      return "scope";
    case Index::Addition:
      return "addition";
    case Index::Deletion:
      return "deletion";
    case Index::Any:
      return "any";
    case Index::Comment:
      return "comment";
    case Index::String:
      return "string";
    case Index::Identifier:
      return "identifier";
    case Index::Is:
      return "is";
    case Index::Before:
      return "before";
    case Index::After:
      return "after";
    case Index::Pathspec:
      return "pathspec";
  }
  throw std::runtime_error("unreachable; value=" +
                           std::to_string(static_cast<int>(field)));
}

QDir Index::indexDir(const git::Repository &repo) {
  QDir dir = repo.appDir();
  dir.mkpath(kIndexDir);
  dir.cd(kIndexDir);
  return dir;
}

bool Index::isLoggingEnabled() {
  return sLoggingEnabled || QSettings().value(kLogKey).toBool();
}

void Index::setLoggingEnabled(bool enable) {
  sLoggingEnabled = enable;
  QSettings().setValue(kLogKey, enable);
}

QDir Index::indexDir() const { return indexDir(mRepo); }

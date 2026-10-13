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

#ifndef INDEX_H
#define INDEX_H

#include "git/Repository.h"
#include <QList>
#include <QObject>
#include <QVector>

namespace git {
class Commit;
}
class Query;

extern "C" {
struct sqlite3;
}

class Index : public QObject {
  Q_OBJECT

public:
  enum Field {
    // fields
    Id,
    Author,
    Email,
    Message,
    Date,
    Path,
    File,
    Scope,
    Addition,
    Deletion,
    Any,

    // subfields
    Comment = 1 << 4,
    String = 2 << 4,
    Identifier = 3 << 4,

    // pseudo-fields
    Is,
    Before,
    After,
    Pathspec
  };

  struct Term {
    Term(Field field, const QString &text) : field(field), text(text) {}

    Field field;
    QString text;
  };

  Index(const git::Repository &repo, QObject *parent = nullptr);

  bool isValid() const;

  git::Repository repo() const { return mRepo; }

  void reset();
  bool remove();

  QList<git::Commit> commits(const QString &filter) const;
  QList<git::Commit> commits(const Query &query) const;

  // Indexed terms matching the GLOB pattern in any of the FTS columns, or in
  // all columns if none are given, in order.
  QByteArrayList indexedTerms(const QByteArrayList &columns,
                              const QByteArray &pattern, int limit) const;

  // Indexed values of the field that start with the prefix, for completion.
  // Plain terms get FTS terms, file names, paths and emails, like plain
  // searches match them. A negative limit returns all values.
  QStringList suggestions(Field field, const QString &prefix, int limit) const;

  // Suggestions for each field that has any.
  QMap<Field, QStringList> fieldMap(const QString &prefix = QString(),
                                    int limit = -1) const;

  // constants
  static int staleLockTime();
  static QString dateFormat();

  // Get the canonical name for the given field.
  static QByteArray fieldName(Field field);

  // Get the index directory for the given repository.
  static QDir indexDir(const git::Repository &repo);
  static QString lockFile(const git::Repository &repo);

  // Enable logging. The log is written to the index dir.
  static bool isLoggingEnabled();
  static void setLoggingEnabled(bool enable);

signals:
  void indexReset();

private:
  QDir indexDir() const;

  // Distinct values of the column that start with the prefix, in order.
  QByteArrayList startingWith(const QByteArray &table, const QByteArray &column,
                              const QByteArray &prefix, int limit) const;

  git::Repository mRepo;
  std::shared_ptr<sqlite3> mDB;
  static bool sLoggingEnabled;
};

#endif

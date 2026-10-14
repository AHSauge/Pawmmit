//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INDEXWRITER_H
#define INDEXWRITER_H

#include "git/Id.h"
#include <QByteArrayList>
#include <QHash>
#include <QSet>
#include <QtGlobal>
#include <memory>

extern "C" {
struct sqlite3;
struct sqlite3_stmt;
}

struct CommitDocument;

namespace git {
class Repository;
}

class IndexWriter {

public:
  IndexWriter(const git::Repository &repo);
  IndexWriter(const IndexWriter &) = delete;
  ~IndexWriter();

  IndexWriter &operator=(const IndexWriter &) = delete;

  /// @brief Whether the index database could be opened or not
  /// @return False if the database couldn't be opened
  bool isValid() const { return mValid; }

  bool write(const CommitDocument &doc);

  // Group writes into one transaction, since each transaction syncs to disk.
  bool beginBatch();
  bool commitBatch();

  /// @brief Removes commits and the paths only they touched, in one
  /// transaction. Call it outside of a batch.
  /// @param ids Commits that are no longer in the repository
  /// @return False if nothing was removed, because of an error
  bool remove(const QSet<git::Id> &ids);

private:
  // Returns the path's id, inserting the path if it's new, or -1 on failure.
  // Newly inserted paths are added to inserted.
  qint64 pathId(const QByteArray &path, QByteArrayList &inserted);

  std::shared_ptr<sqlite3> mDB = nullptr;
  bool mValid = false;
  sqlite3_stmt *mInsertCommit = nullptr;
  sqlite3_stmt *mInsertFTS = nullptr;
  sqlite3_stmt *mFindPath = nullptr;
  sqlite3_stmt *mInsertPath = nullptr;
  sqlite3_stmt *mInsertCommitPath = nullptr;
  // Most commits touch paths seen before, so their ids are cached.
  QHash<QByteArray, qint64> mPathIds;
  int mWalPages = 0;
  // Assumes 4 KiB pages until the actual page size has been read.
  qint64 mWalRestartPages = 65536;
};

#endif

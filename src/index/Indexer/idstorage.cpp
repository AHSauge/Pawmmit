//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "idstorage.h"
#include "git/Repository.h"
#include "index/IndexDatabase.h"

#include <QFile>
#include <sqlite3.h>

IdStorage::IdStorage(const git::Repository &repo) {
  std::shared_ptr<sqlite3> db = index_database::open(repo, true);
  if (!db)
    return;

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db.get(), "SELECT oid FROM commits", -1, &stmt,
                         nullptr) == SQLITE_OK) {
    git_oid_t type = repo.oidType();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      const char *oid = static_cast<const char *>(sqlite3_column_blob(stmt, 0));
      int size = sqlite3_column_bytes(stmt, 0);
      mIds.insert(git::Id(QByteArray(oid, size), type));
    }
    sqlite3_finalize(stmt);
  }
}

bool IdStorage::contains(const git::Id &id) const { return mIds.contains(id); }

//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "FTS.h"
#include "IndexDatabase.h"
#include "IndexWriter.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <sqlite3.h>

// The writer restarts the WAL from the beginning once it grows past this.
const qint64 kWalRestartBytes = 256 * 1024 * 1024;

static bool createSchema(sqlite3 *db);

IndexWriter::IndexWriter(const git::Repository &repo) {
  mDB = index_database::open(repo, false);
  if (!mDB)
    return;

  // An index from a newer Pawmmit is left alone, since that version would
  // otherwise rebuild it right back.
  int version = index_database::schemaVersion(mDB.get());
  if (version > index_database::kSchemaVersion) {
    qWarning().nospace()
        << "Index was written by a newer version of Pawmmit (schema version "
        << version << ")";
    return;
  }

  // An older index is rebuilt from scratch rather than migrated. The indexer
  // holds the lock, so the files can be deleted.
  if (version > 0 && version < index_database::kSchemaVersion) {
    qInfo() << "Rebuilding index with schema version" << version << "as"
            << index_database::kSchemaVersion;
    mDB.reset();
    if (!index_database::removeFiles(repo)) {
      qWarning() << "Failed to remove the outdated index";
      return;
    }

    mDB = index_database::open(repo, false);
    if (!mDB)
      return;
  }

  // WAL lets readers work during writes, and NORMAL only syncs at
  // checkpoints.
  sqlite3_exec(mDB.get(), "PRAGMA journal_mode = WAL", nullptr, nullptr,
               nullptr);
  sqlite3_exec(mDB.get(), "PRAGMA synchronous = NORMAL", nullptr, nullptr,
               nullptr);
  // Set the cache size to 256MiB
  sqlite3_exec(mDB.get(), "PRAGMA cache_size = -262144", nullptr, nullptr,
               nullptr);
  // Checkpoints run on their own connection so commits never wait for them.
  sqlite3_exec(mDB.get(), "PRAGMA wal_autocheckpoint = 0", nullptr, nullptr,
               nullptr);
  // Shrink the WAL file back down whenever it restarts.
  QByteArray sizeLimit =
      "PRAGMA journal_size_limit = " + QByteArray::number(kWalRestartBytes);
  sqlite3_exec(mDB.get(), sizeLimit.constData(), nullptr, nullptr, nullptr);

  // The WAL hook counts pages, so convert the restart threshold to pages.
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(mDB.get(), "PRAGMA page_size", -1, &stmt, nullptr) ==
          SQLITE_OK &&
      sqlite3_step(stmt) == SQLITE_ROW)
    mWalRestartPages = kWalRestartBytes / sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);

  sqlite3_wal_hook(
      mDB.get(),
      [](void *self, sqlite3 *, const char *, int pages) {
        static_cast<IndexWriter *>(self)->mWalPages = pages;
        return SQLITE_OK;
      },
      this);

  if (!createSchema(mDB.get()))
    return;

  QByteArray commitSQL = "INSERT INTO commits(oid, time, tz_offset, email) "
                         "VALUES(?1, ?2, ?3, ?4) "
                         "ON CONFLICT(oid) DO NOTHING RETURNING id";

  QByteArray commitPathSQL = "INSERT OR IGNORE INTO commits_paths(path_id, "
                             "commit_id) VALUES(?1, ?2)";

  QByteArray findPathSQL = "SELECT id FROM paths WHERE path = ?1";

  QByteArray pathSQL =
      "INSERT INTO paths(path, file) VALUES(?1, ?2) RETURNING id";

  QByteArray ftsSQL = "INSERT INTO fts(rowid";
  for (const char *name : kFTSColumnNames)
    ftsSQL += QByteArray(", ") + name;
  ftsSQL += ") VALUES(?1";
  for (size_t i = 0; i < kFTSColumnNames.size(); i++)
    ftsSQL += ", ?" + QByteArray::number(qsizetype(i + 2));
  ftsSQL += ")";

  auto prepare = [this](const QByteArray &sql, sqlite3_stmt **stmt) {
    if (sqlite3_prepare_v2(mDB.get(), sql.constData(), int(sql.size() + 1),
                           stmt, nullptr) == SQLITE_OK)
      return true;
    qWarning() << "Failed to prepare" << sql << ":"
               << sqlite3_errmsg(mDB.get());
    return false;
  };

  mValid = prepare(commitSQL, &mInsertCommit) &&
           prepare(commitPathSQL, &mInsertCommitPath) &&
           prepare(ftsSQL, &mInsertFTS) && prepare(findPathSQL, &mFindPath) &&
           prepare(pathSQL, &mInsertPath);
}

IndexWriter::~IndexWriter() {
  sqlite3_finalize(mInsertCommit);
  sqlite3_finalize(mInsertCommitPath);
  sqlite3_finalize(mInsertFTS);
  sqlite3_finalize(mFindPath);
  sqlite3_finalize(mInsertPath);
}

qint64 IndexWriter::pathId(const QByteArray &path, QByteArrayList &inserted) {
  auto it = mPathIds.constFind(path);
  if (it != mPathIds.constEnd())
    return *it;

  // Paths from earlier runs are already in the database.
  qint64 id = -1;
  sqlite3_bind_text(mFindPath, 1, path.constData(), path.size(), SQLITE_STATIC);
  int rc = sqlite3_step(mFindPath);
  if (rc == SQLITE_ROW)
    id = sqlite3_column_int64(mFindPath, 0);
  else if (rc != SQLITE_DONE)
    qWarning() << "Failed to look up path:" << sqlite3_errmsg(mDB.get());
  sqlite3_reset(mFindPath);

  if (rc == SQLITE_DONE) {
    QByteArray file = path.mid(path.lastIndexOf('/') + 1);
    sqlite3_bind_text(mInsertPath, 1, path.constData(), path.size(),
                      SQLITE_STATIC);
    sqlite3_bind_text(mInsertPath, 2, file.constData(), file.size(),
                      SQLITE_STATIC);
    rc = sqlite3_step(mInsertPath);
    if (rc == SQLITE_ROW) {
      id = sqlite3_column_int64(mInsertPath, 0);
      inserted.append(path);
    } else {
      qWarning() << "Failed to insert path:" << sqlite3_errmsg(mDB.get());
    }
    sqlite3_reset(mInsertPath);
  }

  if (id >= 0)
    mPathIds.insert(path, id);
  return id;
}

bool IndexWriter::write(const CommitDocument &doc) {
  // A commit's rows are written together or not at all, since a commit row
  // without its FTS row would be skipped as already indexed on later runs.
  sqlite3_exec(mDB.get(), "SAVEPOINT doc", nullptr, nullptr, nullptr);
  QByteArrayList insertedPaths;
  auto rollback = [this, &insertedPaths] {
    sqlite3_exec(mDB.get(), "ROLLBACK TO doc; RELEASE doc", nullptr, nullptr,
                 nullptr);
    // Those path rows are gone again, so their cached ids are invalid.
    for (const QByteArray &path : insertedPaths)
      mPathIds.remove(path);
    return false;
  };

  // Attempt to insert commit data
  sqlite3_bind_blob(mInsertCommit, 1, doc.id.toByteArray().constData(),
                    doc.id.getSize(), SQLITE_TRANSIENT);
  sqlite3_bind_int64(mInsertCommit, 2, doc.time);
  sqlite3_bind_int(mInsertCommit, 3, doc.tzOffset);
  sqlite3_bind_text(mInsertCommit, 4, doc.email.constData(), doc.email.size(),
                    SQLITE_STATIC);
  int rc = sqlite3_step(mInsertCommit);
  if (rc != SQLITE_ROW) {
    // SQLITE_DONE without a row means the commit is already indexed.
    if (rc != SQLITE_DONE)
      qWarning() << "Failed to insert commit:" << sqlite3_errmsg(mDB.get());
    sqlite3_reset(mInsertCommit);
    return rollback();
  }

  sqlite3_int64 id = sqlite3_column_int64(mInsertCommit, 0);
  sqlite3_reset(mInsertCommit);

  // Insert FTS data
  sqlite3_bind_int64(mInsertFTS, 1, id);
  for (size_t i = 0; i < doc.columns.size(); i++) {
    const QByteArray &col = doc.columns[i];
    sqlite3_bind_text(mInsertFTS, int(i + 2), col.constData(), col.size(),
                      SQLITE_STATIC);
  }
  rc = sqlite3_step(mInsertFTS);
  if (rc != SQLITE_DONE)
    qWarning() << "Failed to insert FTS data:" << sqlite3_errmsg(mDB.get());
  sqlite3_reset(mInsertFTS);
  if (rc != SQLITE_DONE)
    return rollback();

  // Link the commit to the paths it touches
  for (const QByteArray &path : doc.paths) {
    qint64 pathRowId = pathId(path, insertedPaths);
    if (pathRowId < 0)
      return rollback();

    sqlite3_bind_int64(mInsertCommitPath, 1, pathRowId);
    sqlite3_bind_int64(mInsertCommitPath, 2, id);
    rc = sqlite3_step(mInsertCommitPath);
    if (rc != SQLITE_DONE)
      qWarning() << "Failed to link path:" << sqlite3_errmsg(mDB.get());
    sqlite3_reset(mInsertCommitPath);
    if (rc != SQLITE_DONE)
      return rollback();
  }

  sqlite3_exec(mDB.get(), "RELEASE doc", nullptr, nullptr, nullptr);
  return true;
}

bool IndexWriter::beginBatch() {
  return sqlite3_exec(mDB.get(), "BEGIN IMMEDIATE", nullptr, nullptr,
                      nullptr) == SQLITE_OK;
}

bool IndexWriter::commitBatch() {
  if (sqlite3_exec(mDB.get(), "COMMIT", nullptr, nullptr, nullptr) !=
      SQLITE_OK) {
    qWarning() << "Failed to commit index batch:" << sqlite3_errmsg(mDB.get());
    // Drop the batch so the next one starts a new transaction. Its commits
    // aren't recorded as indexed, so the next run indexes them again.
    sqlite3_exec(mDB.get(), "ROLLBACK", nullptr, nullptr, nullptr);
    // Paths inserted during the batch are gone again too.
    mPathIds.clear();
    return false;
  }

  // The writer never pauses long enough for the checkpointer to restart the
  // WAL, so do it here while no transaction is open.
  if (mWalPages > mWalRestartPages)
    sqlite3_wal_checkpoint_v2(mDB.get(), nullptr, SQLITE_CHECKPOINT_RESTART,
                              nullptr, nullptr);
  return true;
}

bool IndexWriter::remove(const QSet<git::Id> &ids) {
  if (ids.isEmpty())
    return true;

  // Prepared here, since this runs at most once per indexer run.
  sqlite3 *db = mDB.get();
  sqlite3_stmt *commit = nullptr, *fts = nullptr, *paths = nullptr;
  auto finish = [&](bool ok) {
    if (!ok)
      qWarning() << "Failed to remove commits from the index:"
                 << sqlite3_errmsg(db);
    sqlite3_finalize(commit);
    sqlite3_finalize(fts);
    sqlite3_finalize(paths);
    sqlite3_exec(db, ok ? "COMMIT" : "ROLLBACK", nullptr, nullptr, nullptr);
    return ok;
  };

  if (sqlite3_exec(db, "BEGIN IMMEDIATE", nullptr, nullptr, nullptr) !=
      SQLITE_OK)
    return finish(false);

  if (sqlite3_prepare_v2(db, "DELETE FROM commits WHERE oid = ?1 RETURNING id",
                         -1, &commit, nullptr) != SQLITE_OK ||
      sqlite3_prepare_v2(db, "DELETE FROM fts WHERE rowid = ?1", -1, &fts,
                         nullptr) != SQLITE_OK ||
      sqlite3_prepare_v2(db, "DELETE FROM commits_paths WHERE commit_id = ?1",
                         -1, &paths, nullptr) != SQLITE_OK)
    return finish(false);

  for (const git::Id &id : ids) {
    sqlite3_bind_blob(commit, 1, id.toByteArray().constData(), id.getSize(),
                      SQLITE_TRANSIENT);
    int rc = sqlite3_step(commit);
    sqlite3_int64 row =
        (rc == SQLITE_ROW) ? sqlite3_column_int64(commit, 0) : -1;
    sqlite3_reset(commit);
    if (rc != SQLITE_ROW && rc != SQLITE_DONE)
      return finish(false);
    if (row < 0)
      continue;

    for (sqlite3_stmt *stmt : {fts, paths}) {
      sqlite3_bind_int64(stmt, 1, row);
      rc = sqlite3_step(stmt);
      sqlite3_reset(stmt);
      if (rc != SQLITE_DONE)
        return finish(false);
    }
  }

  // Paths that no remaining commit touches. Cached ids may be among them.
  if (sqlite3_exec(db,
                   "DELETE FROM paths WHERE NOT EXISTS (SELECT 1 FROM "
                   "commits_paths cp WHERE cp.path_id = paths.id)",
                   nullptr, nullptr, nullptr) != SQLITE_OK)
    return finish(false);
  mPathIds.clear();

  return finish(true);
}

// Creates the schema when the database is new, i.e. user_version is still 0.
static bool createSchema(sqlite3 *db) {
  char *err = nullptr;
  if (sqlite3_exec(db, "BEGIN IMMEDIATE", nullptr, nullptr, &err) !=
      SQLITE_OK) {
    qWarning() << "Failed to lock index database:" << err;
    sqlite3_free(err);
    return false;
  }

  // Checked under the write lock so concurrent processes create it only once.
  if (index_database::schemaVersion(db) == 0) {
    Q_INIT_RESOURCE(index);
    QFile file(":/index/schema.sql");
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning() << "Failed to read index schema";
      sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
      return false;
    }

    QByteArray schema = file.readAll();
    if (sqlite3_exec(db, schema.constData(), nullptr, nullptr, &err) !=
        SQLITE_OK) {
      qWarning() << "Failed to create index schema:" << err;
      sqlite3_free(err);
      sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
      return false;
    }
  }

  // Also catches schema.sql and kSchemaVersion getting out of step.
  int version = index_database::schemaVersion(db);
  if (version != index_database::kSchemaVersion) {
    qWarning() << "Index schema has version" << version << "instead of"
               << index_database::kSchemaVersion;
    sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
    return false;
  }

  return sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr) == SQLITE_OK;
}

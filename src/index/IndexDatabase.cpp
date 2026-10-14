//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "FTS.h"
#include "Index.h"
#include "IndexDatabase.h"
#include <sqlite3.h>
#include <QFile>
#include <QLockFile>
#include <QString>

static const QString kIndexDB = "index.db";
static const QString kIndexSHM = kIndexDB + "-shm";
static const QString kIndexWAL = kIndexDB + "-wal";
const QStringList kIndexFiles = {kIndexWAL, kIndexSHM, kIndexDB};
static const QString kLockFile = "lock";
constexpr int kStaleLockTime = 24 * 60 * 60 * 1000;

static bool registerTokeniser(sqlite3 *db);

namespace index_database {

std::shared_ptr<sqlite3> open(const git::Repository &repo, bool readOnly) {
  sqlite3 *db = nullptr;
  QString path = Index::indexDir(repo).filePath(kIndexDB);
  QByteArray dbPath = path.toUtf8();

  // Read-only connections can't create the database, and until the indexer
  // has, there's simply nothing indexed yet.
  if (readOnly && !QFile::exists(path))
    return nullptr;

  // The GUI shares its read-only connection between threads.
  int flags = readOnly ? (SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX)
                       : (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
  if (sqlite3_open_v2(dbPath.constData(), &db, flags, nullptr) != SQLITE_OK) {
    qWarning() << "Failed to open index database" << path << ":"
               << sqlite3_errmsg(db);
    sqlite3_close_v2(db);
    return nullptr;
  }

  if (!registerTokeniser(db)) {
    // Also fails if SQLite lacks FTS5, which leaves no error message.
    qWarning() << "Failed to register the index tokeniser:"
               << sqlite3_errmsg(db);
    sqlite3_close_v2(db);
    return nullptr;
  }

  sqlite3_busy_timeout(db, 5000);

  // Only the current schema can be read. Anything else is about to be
  // created or rebuilt by the indexer.
  if (readOnly && schemaVersion(db) != kSchemaVersion) {
    sqlite3_close_v2(db);
    return nullptr;
  }

  return std::shared_ptr<sqlite3>(db, sqlite3_close_v2);
}

int schemaVersion(sqlite3 *db) {
  int version = -1;
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db, "PRAGMA user_version", -1, &stmt, nullptr) ==
          SQLITE_OK &&
      sqlite3_step(stmt) == SQLITE_ROW)
    version = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return version;
}

std::unique_ptr<QLockFile> lock(const git::Repository &repo, int timeout) {
  auto lock =
      std::make_unique<QLockFile>(Index::indexDir(repo).filePath(kLockFile));
  lock->setStaleLockTime(kStaleLockTime);
  return lock->tryLock(timeout) ? std::move(lock) : nullptr;
}

bool remove(const git::Repository &repo) {
  auto fileLock = lock(repo);
  if (fileLock == nullptr)
    return false;

  return removeFiles(repo);
}

bool removeFiles(const git::Repository &repo) {
  QDir dir = Index::indexDir(repo);
  for (const QString &file : kIndexFiles) {
    if (dir.exists(file) && dir.remove(file) == false)
      return false;
  }
  return true;
}

} // namespace index_database

static int tokeniserCreate(void *, const char **, int, Fts5Tokenizer **out) {
  static int instance;
  *out = reinterpret_cast<Fts5Tokenizer *>(&instance);
  return SQLITE_OK;
}

static void tokeniserDelete(Fts5Tokenizer *) {}

static int
tokeniserTokenise(Fts5Tokenizer *, void *ctx, int, const char *text, int size,
                  int (*fts_emit)(void *, int, const char *, int, int, int)) {
  int i = 0;
  while (i < size) {
    while (i < size && text[i] == kTokenSeparator)
      i++;
    int start = i;
    while (i < size && text[i] != kTokenSeparator)
      i++;
    if (i > start) {
      int rc = fts_emit(ctx, 0, text + start, i - start, start, i);
      if (rc != SQLITE_OK)
        return rc;
    }
  }
  return SQLITE_OK;
}

static fts5_api *fts5Api(sqlite3 *db) {
  fts5_api *api = nullptr;
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db, "SELECT fts5(?1)", -1, &stmt, nullptr) ==
      SQLITE_OK) {
    sqlite3_bind_pointer(stmt, 1, &api, "fts5_api_ptr", nullptr);
    sqlite3_step(stmt);
  }
  sqlite3_finalize(stmt);
  return api;
}

static bool registerTokeniser(sqlite3 *db) {
  fts5_api *api = fts5Api(db);
  if (!api)
    return false;

  static fts5_tokenizer tokeniser = {tokeniserCreate, tokeniserDelete,
                                     tokeniserTokenise};
  return api->xCreateTokenizer(api, "pawmmit", nullptr, &tokeniser, nullptr) ==
         SQLITE_OK;
}

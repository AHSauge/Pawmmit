//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "checkpointer.h"
#include "common.h"
#include "index/IndexDatabase.h"
#include <sqlite3.h>

namespace {
const unsigned long kIntervalMs = 1000;
} // namespace

Checkpointer::Checkpointer(const git::Repository &repo) {
  mDB = index_database::open(repo, false);
  // Checkpoints are silently skipped until the connection has seen the WAL.
  if (mDB)
    sqlite3_exec(mDB.get(), "PRAGMA schema_version", nullptr, nullptr, nullptr);
}

void Checkpointer::stop() {
  QMutexLocker locker(&mMutex);
  mStop = true;
  mWake.wakeAll();
}

void Checkpointer::run() {
  QMutexLocker locker(&mMutex);
  while (!mStop) {
    mWake.wait(&mMutex, kIntervalMs);
    if (mStop)
      break;

    locker.unlock();
    log("checkpoint");
    checkpoint();
    locker.relock();
  }
  locker.unlock();

  log("final checkpoint");
  checkpoint(true);
}

bool Checkpointer::checkpoint(bool truncate) {
  if (!mDB)
    return false;

  // Passive checkpoints run alongside the writer, truncating waits for it.
  int mode = truncate ? SQLITE_CHECKPOINT_TRUNCATE : SQLITE_CHECKPOINT_PASSIVE;
  return sqlite3_wal_checkpoint_v2(mDB.get(), nullptr, mode, nullptr,
                                   nullptr) == SQLITE_OK;
}

//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef CHECKPOINTER_H
#define CHECKPOINTER_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <memory>

extern "C" {
struct sqlite3;
}

namespace git {
class Repository;
}

/// @brief Periodically copies the WAL into the index database, so commits on
/// the writer never wait for checkpoints
/// @note Only instantiate one instance of this class!
class Checkpointer : public QThread {
public:
  Checkpointer(const git::Repository &repo);

  // Stop after a final checkpoint that also truncates the WAL.
  void stop();

private:
  bool checkpoint(bool truncate = false);

private:
  std::shared_ptr<sqlite3> mDB = nullptr;
  QMutex mMutex;
  QWaitCondition mWake;
  bool mStop = false;

  void run() override;
};

#endif // CHECKPOINTER_H

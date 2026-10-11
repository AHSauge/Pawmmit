#ifndef MAP_H
#define MAP_H

#include <QRunnable>
#include "FTS.h"
#include "git/Repository.h"
#include "../WorkerQueue.h"
#include "common.h"

class LexerPool;

/// @brief Runnable mapping git commits and diffs into Intermediate objects.
/// This includes running lexing on the diff and commit
class Map : public QRunnable {
public:
  Map(const git::Repository &repo, LexerPool &lexers,
      WorkerQueue<QPair<git::Commit, git::Diff>> &inQueue,
      WorkerQueue<CommitDocument> &outQueue);

  void run() override;

private:
  LexerPool &mLexers;
  WorkerQueue<QPair<git::Commit, git::Diff>> &mInQueue;
  WorkerQueue<CommitDocument> &mOutQueue;

  quint32 mTermLimit = 1000000;
};

#endif // MAP_H

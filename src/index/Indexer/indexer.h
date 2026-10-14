//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INDEXER_H
#define INDEXER_H

#include "checkpointer.h"
#include "FTS.h"
#include "idstorage.h"
#include "Index.h"
#include "lexerpool.h"
#include "resultwriter.h"
#include "WorkerQueue.h"
#include "git/Commit.h"
#include "git/Diff.h"
#include "git/Repository.h"
#include "git/RevWalk.h"
#include "index/Lexer.h"

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QPair>
#include <QThread>
#include <QThreadPool>

void index(const Lexer::Lexeme &lexeme, CommitDocument &doc, Index::Field field,
           Subfield sub = Subfield::None);

class Indexer : public QThread, public QAbstractNativeEventFilter {
public:
  Indexer(const git::Repository &repo, bool notify, QObject *parent = nullptr);

  void run() override;
  bool nativeEventFilter(const QByteArray &type, void *message,
                         qintptr *result) override;

  // True if indexing couldn't start because the database wasn't usable.
  bool failed() const { return mFailed; }

private:
  void finish();
  void cancel();

  const git::Repository mRepo;
  bool mFailed = false;

  IdStorage mIds;
  git::RevWalk mWalker;
  LexerPool mLexers;

  // Variables to hold the threads
  QThreadPool mGrabbers;
  QThreadPool mWorkers;
  ResultWriter mResultWriter;
  Checkpointer mCheckpointer;
  // Various worker queues to synchronise data between threads
  WorkerQueue<git::Commit> mCommits = WorkerQueue<git::Commit>(256);
  WorkerQueue<QPair<git::Commit, git::Diff>> mDiffedCommits =
      WorkerQueue<QPair<git::Commit, git::Diff>>(256);
  WorkerQueue<CommitDocument> mIntermediateQueue =
      WorkerQueue<CommitDocument>(1024);
};

#endif // INDEXER_H

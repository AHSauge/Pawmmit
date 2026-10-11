#ifndef INDEXER
#define INDEXER

#include "FTS.h"
#include "idstorage.h"
#include "Index.h"
#include "lexerpool.h"
#include "resultwriter.h"
#include "WorkerQueue.h"
#include "git/Diff.h"
#include "git/RevWalk.h"
#include "index/Lexer.h"

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QThreadPool>

void index(const Lexer::Lexeme &lexeme, CommitDocument &doc, Index::Field field,
           Subfield sub = Subfield::None);

class Indexer : public QThread, public QAbstractNativeEventFilter {
public:
  Indexer(Index &index, bool notify, QObject *parent = nullptr);

  void run() override;
  bool nativeEventFilter(const QByteArray &type, void *message,
                         qintptr *result) override;

private:
  void finish();
  void cancel();

  Index &mIndex;

  IdStorage mIds;
  git::RevWalk mWalker;
  LexerPool mLexers;

  // Variables to hold the threads
  QThreadPool mGrabbers;
  QThreadPool mWorkers;
  ResultWriter mResultWriter;
  // Various worker queues to synchronise data between threads
  WorkerQueue<git::Commit> mCommits = WorkerQueue<git::Commit>(256);
  WorkerQueue<QPair<git::Commit, git::Diff>> mDiffedCommits =
      WorkerQueue<QPair<git::Commit, git::Diff>>(256);
  WorkerQueue<CommitDocument> mIntermediateQueue =
      WorkerQueue<CommitDocument>(256);
};

#endif // INDEXER

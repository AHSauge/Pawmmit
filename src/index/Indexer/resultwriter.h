//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef RESULTWRITER_H
#define RESULTWRITER_H

#include "FTS.h"
#include "index/Index.h"
#include "index/WorkerQueue.h"
#include <QThread>

/// @brief Writes results to the index
/// @note Only instantiate one instance of this class!
class ResultWriter : public QThread {
public:
  ResultWriter(Index &index, WorkerQueue<CommitDocument> &results, bool notify)
      : mNotify(notify), mIndex(index), mResults(results) {}

private:
  bool mNotify;
  Index &mIndex;
  WorkerQueue<CommitDocument> &mResults;

  void run() override;
};
#endif // RESULTWRITER_H

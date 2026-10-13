//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "resultwriter.h"
#include "common.h"

#include <QElapsedTimer>

namespace {
const int kBatchSize = 1000;
const qint64 kBatchMs = 1000;
} // namespace

void ResultWriter::run() {
  int pending = 0;
  QElapsedTimer timer;
  timer.start();

  // Commit the batch and tell the GUI when new commits became visible.
  auto flush = [&] {
    if (!mIndex.commitBatch())
      log("failed to commit batch");
    if (pending > 0 && mNotify)
      QTextStream(stdout) << "write" << Qt::endl;
    pending = 0;
    timer.restart();
  };

  // Without a transaction each write commits on its own, which is slow.
  auto begin = [&] {
    if (!mIndex.beginBatch())
      log("failed to begin batch");
  };

  begin();
  std::optional<CommitDocument> resultOpt = mResults.dequeue();
  while (!canceled && resultOpt.has_value()) {
    // Write to disk.
    log("start write");
    if (mIndex.write(*resultOpt))
      ++pending;
    log("end write");

    if (pending >= kBatchSize || timer.hasExpired(kBatchMs)) {
      flush();
      begin();
    }

    // Grab next value
    resultOpt = mResults.dequeue();
  }

  // Keep what was written, also when canceled.
  flush();
}

//
//          Copyright (c) 2017, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#include "RepositoryWatcher.h"
#include <QFileInfo>

constexpr int kDefaultDebounceMsec = 2000;

RepositoryWatcher::RepositoryWatcher(const git::Repository &repo,
                                     QObject *parent)
    : QObject(parent), mRepo(repo) {
  // The timer has to run on the main thread.
  mTimer.setInterval(kDefaultDebounceMsec);
  mTimer.setSingleShot(true);
  connect(&mTimer, &QTimer::timeout, this, [this] {
    bool other = mOtherChanges;
    mOtherChanges = false;

    // Staging in the app updates the UI itself, so skip its own index writes.
    if (!other && mOwnIndexStamp.isValid() && mOwnIndexStamp == indexStamp())
      return;

    emit mRepo.notifier()->workdirChanged();
  });

  // The app has just written the index itself.
  connect(repo.notifier(), &git::RepositoryNotifier::indexChanged, this,
          [this] { mOwnIndexStamp = indexStamp(); });
}

void RepositoryWatcher::setDebounceInterval(int msec) {
  mTimer.setInterval(msec);
}

void RepositoryWatcher::cancelPendingNotification() {
  mTimer.stop();
  mOtherChanges = false;
}

void RepositoryWatcher::scheduleNotification() {
  mOtherChanges = true;
  mTimer.start();
}

void RepositoryWatcher::scheduleIndexNotification() { mTimer.start(); }

RepositoryWatcher::IndexStamp RepositoryWatcher::indexStamp() const {
  QFileInfo info(mRepo.dir().filePath("index"));
  if (!info.exists())
    return IndexStamp();

  return {info.size(), info.lastModified().toMSecsSinceEpoch(),
          info.metadataChangeTime().toMSecsSinceEpoch()};
}

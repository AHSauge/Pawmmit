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

void RepositoryWatcher::init(const git::Repository &repo) {
  // The timer has to run on the main thread.
  mTimer.setInterval(2000);
  mTimer.setSingleShot(true);
  connect(&mTimer, &QTimer::timeout, repo.notifier(),
          &git::RepositoryNotifier::workdirChanged);
}

void RepositoryWatcher::cancelPendingNotification() { mTimer.stop(); }

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

#ifndef REPOSITORYWATCHER_H
#define REPOSITORYWATCHER_H

#include "git/Repository.h"
#include <QObject>
#include <QTimer>

class RepositoryWatcher : public QObject {
public:
  static RepositoryWatcher *create(const git::Repository &repo,
                                   QObject *parent = nullptr);

  ~RepositoryWatcher() override = default;
  void setDebounceInterval(int msec);
  void cancelPendingNotification();

protected:
  RepositoryWatcher(const git::Repository &repo, QObject *parent);

  // Something other than the index changed.
  void scheduleNotification();

  // Only the index changed. The app's own staging writes it too, so this
  // refreshes only if the file differs from what the app last wrote.
  void scheduleIndexNotification();

private:
  struct IndexStamp {
    qint64 size = -1;
    qint64 modified = 0;
    qint64 changed = 0;

    bool isValid() const { return size >= 0; }
    bool operator==(const IndexStamp &other) const {
      return size == other.size && modified == other.modified &&
             changed == other.changed;
    }
  };

  IndexStamp indexStamp() const;

  git::Repository mRepo;
  QTimer mTimer;
  bool mOtherChanges = false;
  IndexStamp mOwnIndexStamp;
};

#endif

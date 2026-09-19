//
//          Copyright (c) 2017, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: François Revol
//

#include "PathFilter.h"
#include "RepositoryWatcher.h"
#include <QFileSystemWatcher>

namespace {

const QDir::Filters kFilters =
    (QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);

} // namespace

// Only directories are watched, so edits to existing files go unnoticed, and
// `.git` is left out because its changes can't be told apart.
class QtRepositoryWatcher : public RepositoryWatcher {
public:
  QtRepositoryWatcher(const git::Repository &repo, QObject *parent)
      : RepositoryWatcher(repo, parent), mFilter(repo, false) {
    connect(&mFSWatcher, &QFileSystemWatcher::directoryChanged, this,
            &QtRepositoryWatcher::directoryChanged);
    watch(repo.workdir());
  }

private:
  void directoryChanged(const QString &path) {
    if (!mFilter.isRelevant(path))
      return;

    // Start watching new directories.
    if (QDir(path).exists())
      watch(path);

    scheduleNotification();
  }

  void watch(const QDir &dir) {
    mFSWatcher.addPath(dir.path());

    // Watch subdirs.
    for (const QString &name : dir.entryList(kFilters)) {
      QString path = dir.filePath(name);
      if (mFilter.isRelevant(path))
        watch(path);
    }
  }

  PathFilter mFilter;
  QFileSystemWatcher mFSWatcher;
};

RepositoryWatcher *RepositoryWatcher::create(const git::Repository &repo,
                                             QObject *parent) {
  return new QtRepositoryWatcher(repo, parent);
}

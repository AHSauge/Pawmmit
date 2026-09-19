//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef PATHFILTER_H
#define PATHFILTER_H

#include "git/Index.h"
#include "git/Repository.h"
#include <QDir>

// Decides which changed paths deserve a refresh. It owns its repository handle
// so it can be used from a watcher thread, but by one thread at a time.
class PathFilter {
public:
  enum class Kind {
    Irrelevant,
    Index, // The git index, which the app's own staging writes as well.
    Other,
  };

  // Backends that can't tell files apart pass false to leave out `.git`.
  explicit PathFilter(const git::Repository &repo, bool includeGitDir = true);

  // Accepts absolute or workdir-relative paths.
  Kind classify(const QString &path);
  bool isRelevant(const QString &path) {
    return classify(path) != Kind::Irrelevant;
  }

private:
  QDir mWorkdir;
  QDir mGitDir;
  bool mIncludeGitDir;
  git::Repository mRepo;
  git::Index mIndex;
};

#endif

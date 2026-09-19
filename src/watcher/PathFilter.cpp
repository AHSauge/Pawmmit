//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "PathFilter.h"

namespace {

// Whether `absolute` is `dir` or lies inside it; `relative` gets the remainder.
bool isInside(const QDir &dir, const QString &absolute, QString &relative) {
  relative = dir.relativeFilePath(absolute);
  return relative == "." || !(relative == ".." || relative.startsWith("../") ||
                              QDir::isAbsolutePath(relative));
}

// Only the git directory entries that change what the app shows.
bool isRelevantInGitDir(const QString &path) {
  if (path == "." || path == "refs" || path.startsWith("refs/"))
    return !path.endsWith(".lock");

  return path == "index" || path == "HEAD" || path == "packed-refs" ||
         path == "MERGE_HEAD" || path == "CHERRY_PICK_HEAD" ||
         path == "REVERT_HEAD";
}

} // namespace

PathFilter::PathFilter(const git::Repository &repo, bool includeGitDir)
    : mWorkdir(repo.workdir()), mGitDir(repo.dir()),
      mIncludeGitDir(includeGitDir),
      mRepo(git::Repository::open(mWorkdir.path())) {
  if (mRepo.isValid())
    mIndex = mRepo.index();
}

PathFilter::Kind PathFilter::classify(const QString &path) {
  // Without a handle of our own, over-report rather than drop changes.
  if (!mRepo.isValid())
    return Kind::Other;

  QString absolute =
      QDir::isAbsolutePath(path) ? path : mWorkdir.filePath(path);

  QString inGitDir;
  if (isInside(mGitDir, absolute, inGitDir)) {
    if (!mIncludeGitDir || !isRelevantInGitDir(inGitDir))
      return Kind::Irrelevant;

    return inGitDir == "index" ? Kind::Index : Kind::Other;
  }

  // libgit2 expects workdir-relative paths, and calls "." ignored.
  QString relative = mWorkdir.relativeFilePath(absolute);
  if (relative == "." || !mRepo.isIgnored(relative))
    return Kind::Other;

  // Ignore rules don't apply to tracked files.
  if (!mIndex.isValid())
    return Kind::Other;

  mIndex.read();
  return mIndex.isTracked(relative) ? Kind::Other : Kind::Irrelevant;
}

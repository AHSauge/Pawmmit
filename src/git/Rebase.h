//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#ifndef REBASE_H
#define REBASE_H

#include <QString>
#include <memory>

// TODO: move to cpp again, forward declaration should be enough
#include "git2/rebase.h"

struct git_rebase;
struct git_repository;

namespace git {

class Commit;

class Rebase {
public:
  bool isValid() const { return d != nullptr; }

  size_t count() const;
  size_t currentIndex() const;
  const git_rebase_operation *operation(size_t index);
  Commit commitToRebase() const;
  bool hasNext() const;
  Commit next() const;
  Commit commit(const QString &message);

  void abort();
  bool finish();

private:
  Rebase();
  Rebase(git_repository *repo, git_rebase *rebase = nullptr,
         const QString &overrideUser = QString(),
         const QString &overrideEmail = QString());

  git_repository *mRepo;
  std::shared_ptr<git_rebase> d;
  QString mOverrideUser;
  QString mOverrideEmail;

  friend class Repository;
};

} // namespace git

#endif

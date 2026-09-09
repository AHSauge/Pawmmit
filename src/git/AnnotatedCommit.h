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

#ifndef ANNOTATEDCOMMIT_H
#define ANNOTATEDCOMMIT_H

#include <memory>

struct git_annotated_commit;
struct git_repository;

namespace git {

class Commit;
class Repository;

class AnnotatedCommit {
public:
  AnnotatedCommit();

  bool isValid() const { return d != nullptr; }

  Commit commit() const;

  int analysis() const;

private:
  AnnotatedCommit(git_annotated_commit *commit, git_repository *repo);
  operator git_annotated_commit *() const;

  git_repository *repo;
  std::shared_ptr<git_annotated_commit> d;

  friend class Commit;
  friend class Branch;
  friend class Reference;
  friend class Repository;
};

} // namespace git

#endif

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

#ifndef BLAME_H
#define BLAME_H

struct git_signature;
struct git_repository;

#include "git2/blame.h"
#include <QString>
#include <memory>

namespace git {

class Id;
class Signature;

class Blame {
public:
  class Callbacks {
  public:
    virtual ~Callbacks() {}

    virtual bool progress() { return false; }
  };

  Blame();

  bool isValid() const { return d != nullptr; }

  int count() const;
  int index(int line) const;

  int line(int index) const;
  Id id(int index) const;
  QString message(int index) const;
  Signature signature(int index) const;

  bool isCommitted(int index) const;

  Blame updated(const QByteArray &buffer) const;

protected:
  Blame(git_blame *blame, git_repository *repo);

  git_repository *repo;
  std::shared_ptr<git_blame> d;

  friend class Repository;
};

} // namespace git

#endif

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

#ifndef REVWALK_H
#define REVWALK_H

#include <QSharedPointer>

struct git_revwalk;

namespace git {

class Commit;
class Reference;

class RevWalk {
public:
  RevWalk();

  bool isValid() const { return !d.isNull(); }

  bool hide(const Commit &commit);
  bool hide(const Reference &ref);

  bool push(const Commit &commit);
  bool push(const Reference &ref);

  // Return the next commit that matches the given pathspec.
  Commit next(const QString &pathspec = QString()) const;

protected:
  RevWalk(git_revwalk *walker);

  QSharedPointer<git_revwalk> d;

  friend class Commit;
  friend class Reference;
  friend class Repository;
};

} // namespace git

#endif

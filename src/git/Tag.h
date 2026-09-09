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

#ifndef TAG_H
#define TAG_H

#include "Object.h"
#include "git2/tag.h"

namespace git {

class Signature;

class Tag : public Object {
public:
  Tag();
  Tag(const Object &rhs);

  Signature tagger() const;
  QString message() const;

private:
  Tag(git_tag *tag);
  operator git_tag *() const;

  friend class TagRef;
};

} // namespace git

#endif

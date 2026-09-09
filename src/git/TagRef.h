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

#ifndef TAGREF_H
#define TAGREF_H

#include "Reference.h"

namespace git {

class Tag;

class TagRef : public Reference {
public:
  TagRef(const Reference &rhs);

  Tag tag() const;

  bool remove();

private:
  TagRef(git_reference *ref = nullptr);

  friend class Repository;
};

} // namespace git

#endif

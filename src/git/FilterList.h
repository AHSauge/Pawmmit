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

#ifndef FILTERLIST_H
#define FILTERLIST_H

#include "git2/filter.h"
#include <memory>

namespace git {

class FilterList {
public:
  bool isValid() const { return d != nullptr; }

private:
  FilterList(git_filter_list *filter = nullptr);
  operator git_filter_list *() const;

  std::shared_ptr<git_filter_list> d;

  friend class Patch;
  friend class Repository;
};

} // namespace git

#endif

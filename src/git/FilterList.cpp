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

#include "FilterList.h"

namespace git {

FilterList::FilterList(git_filter_list *filter)
    : d(filter, git_filter_list_free) {}

FilterList::operator git_filter_list *() const { return d.get(); }

} // namespace git

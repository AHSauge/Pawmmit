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

#include "Buffer.h"
#include "git2/blob.h"

namespace git {

Buffer::Buffer(const char *data, int size) : data(data), size(size) {}

bool Buffer::isBinary() const { return git_blob_data_is_binary(data, size); }

} // namespace git

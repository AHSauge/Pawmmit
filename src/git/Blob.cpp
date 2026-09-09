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

#include "Blob.h"

namespace git {

Blob::Blob() : Object() {}

Blob::Blob(const Object &rhs) : Object(rhs) {
  if (isValid() && type() != GIT_OBJECT_BLOB)
    d.clear();
}

Blob::Blob(git_blob *blob) : Object(reinterpret_cast<git_object *>(blob)) {}

Blob::operator git_blob *() const {
  return reinterpret_cast<git_blob *>(d.data());
}

bool Blob::isBinary() const { return git_blob_is_binary(*this); }

bool Blob::isBinary(const QByteArray &data) {
  return git_blob_data_is_binary(data.constData(), data.length());
}

QByteArray Blob::content() const {
  const char *content = static_cast<const char *>(git_blob_rawcontent(*this));
  return QByteArray(content, git_blob_rawsize(*this));
}

} // namespace git

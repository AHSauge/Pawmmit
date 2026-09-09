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

#include "Result.h"
#include "git2/errors.h"

namespace git {

Result::Result(int error) : mError(error) {
  const git_error *err = git_error_last();
  mErrorString = err ? err->message : QString();
}

QString Result::errorString(const QString &defaultError) const {
  return !mErrorString.isEmpty() ? mErrorString : defaultError;
}

} // namespace git

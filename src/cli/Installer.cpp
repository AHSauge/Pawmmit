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

#include "Installer.h"
#include <QDir>

Installer::Installer(const QString &name, const QString &path)
    : mName(name), mPath(path) {}

bool Installer::exists() const {
  return (!mName.isEmpty() && !mPath.isEmpty() && QDir(mPath).exists(mName));
}

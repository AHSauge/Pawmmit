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

#include "Repository.h"
#include "Account.h"

Repository::Repository(const QString &name, const QString &fullName,
                       Account *parent)
    : QObject(parent), mName(name), mFullName(fullName) {}

Account *Repository::account() const {
  return static_cast<Account *>(parent());
}

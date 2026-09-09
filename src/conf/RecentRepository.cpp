//
//          Copyright (c) 2018, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#include "RecentRepository.h"

RecentRepository::RecentRepository(const QString &gitpath, QObject *parent)
    : QObject(parent), mGitPath(gitpath) {}

QString RecentRepository::gitpath() const { return mGitPath; }

QString RecentRepository::name() const {
  return mGitPath.section('/', -mSections, -1);
}

void RecentRepository::increment() { ++mSections; }

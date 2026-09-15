//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef AMENDINFO_H
#define AMENDINFO_H

#include <QDateTime>
#include <QString>

struct ContributorInfo {
  enum class SelectedDateTimeType { Current, Manual, Original };
  QString name;
  QString email;
  QDateTime commitDate;
  SelectedDateTimeType commitDateType;
};

struct AmendInfo {
  ContributorInfo authorInfo;
  ContributorInfo committerInfo;
  QString commitMessage;
};

#endif

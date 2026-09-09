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

#ifndef RECENTREPOSITORY_H
#define RECENTREPOSITORY_H

#include <QObject>

class RecentRepository : public QObject {
  Q_OBJECT

public:
  RecentRepository(const QString &gitpath, QObject *parent = nullptr);

  QString gitpath() const;
  QString name() const;

private:
  void increment();

  QString mGitPath;
  int mSections = 1;

  friend class RecentRepositories;
};

#endif

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

#ifndef RECENTREPOSITORIES_H
#define RECENTREPOSITORIES_H

#include <QList>
#include <QObject>

class RecentRepository;

class RecentRepositories : public QObject {
  Q_OBJECT

public:
  int count() const;
  RecentRepository *repository(int index) const;

  void clear();
  void remove(int index);
  void add(QString path);

  static RecentRepositories *instance();

signals:
  void repositoryAboutToBeAdded();
  void repositoryAdded();
  void repositoryAboutToBeRemoved();
  void repositoryRemoved();

private:
  RecentRepositories(QObject *parent = nullptr);

  void load();
  void store();

  QList<RecentRepository *> mRepos;
};

#endif

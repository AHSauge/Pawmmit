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

#include "git/Repository.h"
#include <QTreeWidget>

class PluginsPanel : public QTreeWidget {
  Q_OBJECT

public:
  enum Column { Name, Kind, Description };

  PluginsPanel(const git::Repository &repo, QWidget *parent = nullptr);

  QSize sizeHint() const override;

private:
  void refresh();

  git::Repository mRepo;
};

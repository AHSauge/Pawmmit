//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Bryan Williams
//

#ifndef REPOGENERALPANEL_H
#define REPOGENERALPANEL_H

#include "git/Repository.h"
#include <QScopedPointer>
#include <QWidget>

class RepoView;

namespace Ui {
class RepoGeneralPanel;
}

class RepoGeneralPanel : public QWidget {
  Q_OBJECT

public:
  RepoGeneralPanel(RepoView *view, QWidget *parent = nullptr);
  ~RepoGeneralPanel() override;

  void init();

private:
  git::Repository mRepo;
  QScopedPointer<Ui::RepoGeneralPanel> ui;
};

#endif

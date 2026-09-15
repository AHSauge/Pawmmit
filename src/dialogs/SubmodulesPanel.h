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

#ifndef SUBMODULESPANEL_H
#define SUBMODULESPANEL_H

#include <QScopedPointer>
#include <QWidget>

class RepoView;

namespace Ui {
class SubmodulesPanel;
}

class SubmodulesPanel : public QWidget {
  Q_OBJECT

public:
  SubmodulesPanel(RepoView *view, QWidget *parent = nullptr);
  ~SubmodulesPanel() override;

private:
  QScopedPointer<Ui::SubmodulesPanel> ui;
};

#endif

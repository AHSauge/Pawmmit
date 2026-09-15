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

#ifndef LFSPANEL_H
#define LFSPANEL_H

#include <QScopedPointer>
#include <QWidget>

class RepoView;

namespace Ui {
class LfsPanel;
}

class LfsPanel : public QWidget {
  Q_OBJECT

public:
  LfsPanel(RepoView *view, QWidget *parent = nullptr);
  ~LfsPanel() override;

private:
  QScopedPointer<Ui::LfsPanel> ui;
};

#endif

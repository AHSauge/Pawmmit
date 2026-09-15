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

#ifndef MISCPANEL_H
#define MISCPANEL_H

#include <QScopedPointer>
#include <QWidget>

namespace Ui {
class MiscPanel;
}

class MiscPanel : public QWidget {
  Q_OBJECT

public:
  MiscPanel(QWidget *parent = nullptr);
  ~MiscPanel() override;

private:
  QScopedPointer<Ui::MiscPanel> ui;
};

#endif

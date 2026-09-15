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

#ifndef GENERALPANEL_H
#define GENERALPANEL_H

#include <QScopedPointer>
#include <QWidget>

namespace Ui {
class GeneralPanel;
}

class GeneralPanel : public QWidget {
  Q_OBJECT

public:
  GeneralPanel(QWidget *parent = nullptr);
  ~GeneralPanel() override;

  void init();

private:
  QScopedPointer<Ui::GeneralPanel> ui;
};

#endif

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

#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

#include "git/Config.h"
#include <QScopedPointer>
#include <QWidget>

class QComboBox;
class QPushButton;

namespace Ui {
class ToolsPanel;
}

class ToolsPanel : public QWidget {
  Q_OBJECT

public:
  ToolsPanel(QWidget *parent = nullptr);
  ~ToolsPanel() override;

private:
  void setupExternalTool(QComboBox *comboBox, QPushButton *configure,
                         const QString &type);

  git::Config mConfig;

  QScopedPointer<Ui::ToolsPanel> ui;
};

#endif

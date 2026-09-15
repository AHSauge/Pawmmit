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

#ifndef WINDOWPANEL_H
#define WINDOWPANEL_H

#include <QScopedPointer>
#include <QWidget>

namespace Ui {
class WindowPanel;
}

class WindowPanel : public QWidget {
  Q_OBJECT

public:
  WindowPanel(QWidget *parent = nullptr);
  ~WindowPanel() override;

private:
  QScopedPointer<Ui::WindowPanel> ui;
};

#endif

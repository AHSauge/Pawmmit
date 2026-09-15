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

#ifndef UPDATEPANEL_H
#define UPDATEPANEL_H

#include <QScopedPointer>
#include <QWidget>

namespace Ui {
class UpdatePanel;
}

class UpdatePanel : public QWidget {
  Q_OBJECT

public:
  UpdatePanel(QWidget *parent = nullptr);
  ~UpdatePanel() override;

private:
  QScopedPointer<Ui::UpdatePanel> ui;
};

#endif

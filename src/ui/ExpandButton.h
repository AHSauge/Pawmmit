//
//          Copyright (c) 2017, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#ifndef EXPANDBUTTON_H
#define EXPANDBUTTON_H

#include <QToolButton>

class ExpandButton : public QToolButton {
  Q_OBJECT

public:
  ExpandButton(QWidget *parent = nullptr);

  QSize sizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;
};

#endif

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

#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QIcon>
#include <QWidget>

class IconLabel : public QWidget {
public:
  IconLabel(const QIcon &icon, int width, int height,
            QWidget *parent = nullptr);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QIcon mIcon;
  int mWidth;
  int mHeight;
};

#endif

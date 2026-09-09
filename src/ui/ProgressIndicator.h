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

#ifndef PROGRESSINDICATOR
#define PROGRESSINDICATOR

#include <QWidget>

// FIXME: Implement fully fledged widget?
class ProgressIndicator : public QWidget {
public:
  static QSize size();

  static void paint(QPainter *painter, const QRect &rect, const QColor &c,
                    float fadein, int progress,
                    const QWidget *widget = nullptr);

  static void paint(QPainter *painter, const QRect &rect, const QColor &color,
                    int progress, const QWidget *widget = nullptr) {
    paint(painter, rect, color, 1.0f, progress, widget);
  }
};

#endif

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

#ifndef TERMINALPANEL_H
#define TERMINALPANEL_H

// Q_OS_UNIX is defined by qsystemdetection.h, pulled in via QtGlobal; it
// isn't predefined by the compiler itself, so this header needs a Qt
// include before it can check for it.
#include <QtGlobal>

#ifdef Q_OS_UNIX

#include <QScopedPointer>
#include <QWidget>

namespace Ui {
class TerminalPanel;
}

class TerminalPanel : public QWidget {
  Q_OBJECT

public:
  TerminalPanel(QWidget *parent = nullptr);
  ~TerminalPanel() override;

private:
  void updateInstallButton();

  QScopedPointer<Ui::TerminalPanel> ui;
};

#endif

#endif

//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Shane Gramlich
//

#ifndef THEMEDIALOG_H
#define THEMEDIALOG_H

#include <QDialog>

class ThemeDialog : public QDialog {
  Q_OBJECT

public:
  ThemeDialog(QWidget *parent = nullptr);
};

#endif

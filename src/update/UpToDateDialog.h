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

#ifndef UPTODATEDDIALOG_H
#define UPTODATEDDIALOG_H

#include <QDialog>

class UpToDateDialog : public QDialog {
  Q_OBJECT

public:
  UpToDateDialog(QWidget *parent = nullptr);
};

#endif

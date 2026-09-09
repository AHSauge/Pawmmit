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

#ifndef DELETEBRANCHDIALOG_H
#define DELETEBRANCHDIALOG_H

#include <QMessageBox>

namespace git {
class Branch;
}

class DeleteBranchDialog : public QMessageBox {
  Q_OBJECT

public:
  DeleteBranchDialog(const git::Branch &branch, QWidget *parent = nullptr);
};

#endif

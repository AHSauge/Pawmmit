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

#ifndef UPDATESUBMODULESDIALOG_H
#define UPDATESUBMODULESDIALOG_H

#include <QDialog>

class QCheckBox;
class QTableView;

namespace git {
class Repository;
class Submodule;
} // namespace git

class UpdateSubmodulesDialog : public QDialog {
  Q_OBJECT

public:
  UpdateSubmodulesDialog(const git::Repository &repo,
                         QWidget *parent = nullptr);

  QList<git::Submodule> submodules() const;
  bool recursive() const;
  bool init() const;

private:
  QTableView *mTable;
  QCheckBox *mRecursive;
  QCheckBox *mInit;
};

#endif

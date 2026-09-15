//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Bryan Williams
//

#ifndef BRANCHESPANEL_H
#define BRANCHESPANEL_H

#include "git/Repository.h"
#include <QScopedPointer>
#include <QWidget>

namespace Ui {
class BranchesPanel;
}

class BranchesPanel : public QWidget {
  Q_OBJECT

public:
  BranchesPanel(const git::Repository &repo, QWidget *parent = nullptr);
  ~BranchesPanel() override;

  void editBranch(const QString &name);

private:
  QScopedPointer<Ui::BranchesPanel> ui;
};

#endif

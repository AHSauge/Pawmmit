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

#ifndef NEWBRANCHDIALOG_H
#define NEWBRANCHDIALOG_H

#include "git/Commit.h"
#include <QDialog>
#include <QScopedPointer>

namespace git {
class Reference;
class Repository;
} // namespace git

namespace Ui {
class NewBranchDialog;
}

class NewBranchDialog : public QDialog {
  Q_OBJECT

public:
  NewBranchDialog(const git::Repository &repo,
                  const git::Commit &commit = git::Commit(),
                  QWidget *parent = nullptr);
  ~NewBranchDialog() override;

  QString name() const;
  bool checkout() const;
  git::Commit target() const;
  git::Reference upstream() const;

private:
  QScopedPointer<Ui::NewBranchDialog> ui;
};

#endif

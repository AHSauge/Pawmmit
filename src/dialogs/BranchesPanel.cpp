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

#include "BranchesPanel.h"
#include "BranchDelegate.h"
#include "BranchTableModel.h"
#include "DeleteBranchDialog.h"
#include "NewBranchDialog.h"
#include "git/Branch.h"
#include "ui/Footer.h"
#include "ui_BranchesPanel.h"
#include <QHeaderView>

BranchesPanel::BranchesPanel(const git::Repository &repo, QWidget *parent)
    : QWidget(parent), ui(new Ui::BranchesPanel) {
  ui->setupUi(this);

  ui->mTable->setModel(new BranchTableModel(repo, this));
  ui->mTable->setItemDelegate(new BranchDelegate(repo, ui->mTable));

  // Set section resize mode after model is set.
  ui->mTable->horizontalHeader()->setSectionResizeMode(
      BranchTableModel::Upstream, QHeaderView::Stretch);

  ui->mFooter->setPlusEnabled(repo.head().isValid());

  connect(ui->mFooter, &Footer::plusClicked, this, [this, repo] {
    NewBranchDialog *dialog = new NewBranchDialog(repo, git::Commit(), this);
    connect(dialog, &QDialog::accepted, this, [repo, dialog] {
      QString name = dialog->name();
      git::Commit commit = dialog->target();
      git::Branch branch = git::Repository(repo).createBranch(name, commit);

      // Start tracking.
      if (branch.isValid())
        branch.setUpstream(dialog->upstream());
    });

    dialog->open();
  });

  connect(ui->mFooter, &Footer::minusClicked, this, [this] {
    // Get all selected branches before removing any.
    QList<git::Branch> branches;
    QModelIndexList indexes = ui->mTable->selectionModel()->selectedRows();
    for (const QModelIndex &index : indexes) {
      QVariant var = index.data(BranchTableModel::BranchRole);
      branches.append(var.value<git::Branch>());
    }

    // Remove them all.
    for (const git::Branch &branch : branches) {
      Q_ASSERT(!branch.isHead());
      DeleteBranchDialog dialog(branch, this);
      dialog.exec();
    }
  });

  // Enable/disable minus button.
  auto updateMinusButton = [this] {
    QModelIndexList indexes = ui->mTable->selectionModel()->selectedRows();
    bool enabled = !indexes.isEmpty();
    for (const QModelIndex &index : indexes) {
      QVariant var = index.data(BranchTableModel::BranchRole);
      if (var.value<git::Branch>().isHead()) {
        enabled = false;
        break;
      }
    }

    ui->mFooter->setMinusEnabled(enabled);
  };

  connect(ui->mTable->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, updateMinusButton);
  connect(ui->mTable->model(), &QAbstractItemModel::modelReset, this,
          updateMinusButton);
}

BranchesPanel::~BranchesPanel() = default;

void BranchesPanel::editBranch(const QString &name) {
  QAbstractItemModel *model = ui->mTable->model();
  for (int i = 0; i < model->rowCount(); ++i) {
    QModelIndex index = model->index(i, BranchTableModel::Name);
    if (index.data().toString() == name) {
      ui->mTable->edit(index);
      return;
    }
  }
}

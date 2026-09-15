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

#include "ExternalToolsDialog.h"
#include "dialogs/ExternalToolsModel.h"
#include "git/Config.h"
#include "ui/Footer.h"
#include "ui_ExternalToolsDialog.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QTableView>

ExternalToolsDialog::ExternalToolsDialog(const QString &type, QWidget *parent)
    : QDialog(parent), ui(new Ui::ExternalToolsDialog) {
  setAttribute(Qt::WA_DeleteOnClose);

  ui->setupUi(this);

  connect(ui->mButtons, &QDialogButtonBox::accepted, this, &QDialog::close);
  connect(ui->mButtons, &QDialogButtonBox::rejected, this, &QDialog::close);

  ui->mDetectedTable->verticalHeader()->setVisible(false);
  ui->mDetectedTable->setModel(new ExternalToolsModel(type, true, this));
  ui->mDetectedTable->horizontalHeader()->setSectionResizeMode(
      ExternalToolsModel::Arguments, QHeaderView::Stretch);
  ui->mDetectedTable->resizeColumnsToContents();
  ui->mDetectedTable->selectRow(0);

  ExternalToolsModel *model = new ExternalToolsModel(type, false, this);

  ui->mUserDefinedTable->verticalHeader()->setVisible(false);
  ui->mUserDefinedTable->setModel(model);
  ui->mUserDefinedTable->sortByColumn(1, Qt::AscendingOrder);
  ui->mUserDefinedTable->horizontalHeader()->setSectionResizeMode(
      ExternalToolsModel::Arguments, QHeaderView::Stretch);
  ui->mUserDefinedTable->resizeColumnsToContents();

  QTableView *table = ui->mUserDefinedTable;
  connect(ui->mFooter, &Footer::plusClicked, [this, table, model] {
    model->add(QFileDialog::getOpenFileName(this, tr("Select Executable")));
    model->refresh();
    table->resizeColumnsToContents();
  });

  connect(ui->mFooter, &Footer::minusClicked, [table, model] {
    QModelIndexList indexes = table->selectionModel()->selectedRows(0);
    for (const QModelIndex &index : indexes)
      model->remove(index.data(Qt::DisplayRole).toString());
    model->refresh();
    table->resizeColumnsToContents();
  });

  // Enable/disable minus button.
  auto updateMinusButton = [this, table] {
    ui->mFooter->setMinusEnabled(table->selectionModel()->hasSelection());
  };

  connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this,
          updateMinusButton);
  connect(table->model(), &QAbstractItemModel::modelReset, this,
          updateMinusButton);
}

ExternalToolsDialog::~ExternalToolsDialog() = default;

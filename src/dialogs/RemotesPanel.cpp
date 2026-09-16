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

#include "RemotesPanel.h"
#include "AddRemoteDialog.h"
#include "RemoteTableModel.h"
#include "git/Remote.h"
#include "ui/Footer.h"
#include "ui_RemotesPanel.h"
#include <QMessageBox>
#include <QPushButton>

RemotesPanel::RemotesPanel(const git::Repository &repo, QWidget *parent)
    : QWidget(parent), mRepo(repo), ui(new Ui::RemotesPanel) {
  ui->setupUi(this);

  ui->mTable->setModel(new RemoteTableModel(this, repo));

  connect(ui->mFooter, &Footer::plusClicked, this, [this] { addRemote(); });

  connect(ui->mFooter, &Footer::minusClicked, this, [this] {
    QModelIndexList indexes = ui->mTable->selectionModel()->selectedRows();
    for (const QModelIndex &index : indexes) {
      QString name = index.data().toString();
      QString title = tr("Delete Remote?");
      QString text = tr("Are you sure you want to delete '%1'?");
      QMessageBox msg(QMessageBox::Warning, title, text.arg(name),
                      QMessageBox::Cancel, this);

      QPushButton *remove =
          msg.addButton(tr("Delete"), QMessageBox::AcceptRole);

      msg.exec();

      if (msg.clickedButton() == remove)
        mRepo.deleteRemote(name);
    }
  });

  connect(ui->mTable->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, [this] {
            QModelIndexList indexes =
                ui->mTable->selectionModel()->selectedRows();
            ui->mFooter->setMinusEnabled(!indexes.isEmpty());
          });
}

RemotesPanel::~RemotesPanel() = default;

void RemotesPanel::addRemote(const QString &name) {
  AddRemoteDialog *dialog = new AddRemoteDialog(name, this);
  connect(dialog, &QDialog::accepted, this, [this, dialog] {
    git::Remote remote = mRepo.addRemote(dialog->name(), dialog->url());
    if (!remote.isValid()) {
      QString text = tr("Failed to add remote '%1' - %2");
      QMessageBox::warning(
          this, tr("Error"),
          text.arg(dialog->name(), git::Repository::lastError()));
    }
  });

  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->open();
}

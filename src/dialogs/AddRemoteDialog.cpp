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

#include "AddRemoteDialog.h"
#include "ui_AddRemoteDialog.h"
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>

AddRemoteDialog::AddRemoteDialog(const QString &name, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddRemoteDialog) {
  ui->setupUi(this);

  ui->mName->setText(name);
  connect(ui->mName, &QLineEdit::textChanged, this, &AddRemoteDialog::update);
  connect(ui->mUrl, &QLineEdit::textChanged, this, &AddRemoteDialog::update);

  connect(ui->mButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(ui->mButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  // Custom-role, custom-text button: Designer's QDialogButtonBox only
  // supports standard buttons declaratively.
  mAdd =
      ui->mButtons->addButton(tr("Add Remote"), QDialogButtonBox::AcceptRole);

  update();
}

AddRemoteDialog::~AddRemoteDialog() = default;

QString AddRemoteDialog::name() const { return ui->mName->text(); }

QString AddRemoteDialog::url() const { return ui->mUrl->text(); }

void AddRemoteDialog::update(const QString &text) {
  mAdd->setEnabled(!name().isEmpty() && !url().isEmpty());
}

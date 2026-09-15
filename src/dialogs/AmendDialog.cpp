//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "AmendDialog.h"
#include "InfoBox.h"
#include "ui_AmendDialog.h"
#include <QPushButton>
#include <QTextEdit>

AmendDialog::AmendDialog(const git::Signature &author,
                         const git::Signature &committer,
                         const QString &commitMessage, QWidget *parent)
    : QDialog(parent), ui(new Ui::AmendDialog) {
  ui->setupUi(this);

  ui->mAuthorInfo->setSignature(author);
  ui->mCommitterInfo->setSignature(committer);
  ui->mCommitMessage->setPlainText(commitMessage);

  connect(ui->mOk, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->mCancel, &QPushButton::clicked, this, &QDialog::reject);
}

AmendDialog::~AmendDialog() = default;

AmendInfo AmendDialog::getInfo() const {
  AmendInfo ai;
  ai.authorInfo = ui->mAuthorInfo->getInfo();
  ai.committerInfo = ui->mCommitterInfo->getInfo();
  ai.commitMessage = ui->mCommitMessage->toPlainText();
  return ai;
}

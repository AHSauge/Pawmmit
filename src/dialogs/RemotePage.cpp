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

#include "RemotePage.h"
#include "host/Repository.h"
#include "ui_RemotePage.h"
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

RemotePage::RemotePage(Repository *repo, QWidget *parent)
    : QWizardPage(parent), ui(new Ui::RemotePage) {
  ui->setupUi(this);

  setSubTitle(repo
                  ? tr("Choose protocol to authenticate with the remote.")
                  : tr("Enter the URL of the remote repository or browse for a "
                       "local directory"));

  static_cast<QFormLayout *>(layout())->setRowVisible(ui->mProtocol, repo);
  static_cast<QFormLayout *>(layout())->setRowVisible(ui->mExampleLabel, !repo);
  ui->mBrowse->setVisible(!repo);

  if (repo) {
    ui->mProtocol->addItem("HTTPS", Repository::Https);
    ui->mProtocol->addItem("SSH", Repository::Ssh);

    // Reset URL when the protocol changes.
    using Signal = void (QComboBox::*)(int);
    auto signal = static_cast<Signal>(&QComboBox::activated);
    connect(ui->mProtocol, signal, [this, repo] {
      int protocol = ui->mProtocol->currentData().toInt();
      ui->mUrl->setText(repo->url(static_cast<Repository::Protocol>(protocol)));
    });

    ui->mUrl->setReadOnly(true);
    ui->mUrl->setText(repo->url(Repository::Https));
  } else {
    connect(ui->mBrowse, &QPushButton::clicked, [this]() {
      QString title = tr("Choose Directory");
      QFileDialog *dialog =
          new QFileDialog(this, title, ui->mUrl->text(), QString());
      dialog->setAttribute(Qt::WA_DeleteOnClose);
      dialog->setFileMode(QFileDialog::Directory);
      dialog->setOption(QFileDialog::ShowDirsOnly);
      connect(dialog, &QFileDialog::fileSelected,
              [this](const QString &file) { ui->mUrl->setText(file); });

      dialog->open();
    });
  }

  connect(ui->mUrl, &QLineEdit::textChanged, this,
          &RemotePage::completeChanged);

  // Register field.
  registerField("url", ui->mUrl);
}

RemotePage::~RemotePage() = default;

bool RemotePage::isComplete() const { return !ui->mUrl->text().isEmpty(); }

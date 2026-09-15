//
//          Copyright (c) 2018, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#include "AccountDialog.h"
#include "cred/CredentialHelper.h"
#include "host/Accounts.h"
#include "ui/ExpandButton.h"
#include "ui_AccountDialog.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

AccountDialog::AccountDialog(Account *account, QWidget *parent)
    : QDialog(parent), ui(new Ui::AccountDialog) {
  setAttribute(Qt::WA_DeleteOnClose);

  ui->setupUi(this);
  ui->gridLayout->setColumnStretch(1, 1);

  ui->mHost->addItem("GitHub", Account::GitHub);
  ui->mHost->addItem("Gitea", Account::Gitea);
  ui->mHost->addItem("Bitbucket", Account::Bitbucket);
  ui->mHost->addItem("Beanstalk", Account::Beanstalk);
  ui->mHost->addItem("GitLab", Account::GitLab);

  Account::Kind kind = account ? account->kind() : Account::GitHub;
  setKind(kind);

  ui->mUsername->setText(account ? account->username() : QString());
  connect(ui->mUsername, &QLineEdit::textChanged, this,
          &AccountDialog::updateButtons);

  ui->mPassword->setText(account ? account->password() : QString());
  connect(ui->mPassword, &QLineEdit::textChanged, this,
          &AccountDialog::updateButtons);

  auto signal = QOverload<int>::of(&QComboBox::currentIndexChanged);

  // Qt doesn't grow a top-level window on its own to fit a wrapping label's
  // heightForWidth, whether on first show or later text changes, so this
  // resize has to happen explicitly every time the help text changes.
  auto updateHelpText = [this] {
    Account::Kind kind =
        static_cast<Account::Kind>(ui->mHost->currentData().toInt());
    ui->mLabel->setText(Account::helpText(kind));
    ui->mLabel->setVisible(!ui->mLabel->text().isEmpty());

    // Force the layout to recompute geometry now, so the sizeHint() and
    // heightForWidth() queried below reflect mLabel's new text/visibility
    // rather than a stale, not-yet-recomputed cache.
    layout()->activate();
    int width = qMax(minimumWidth(), sizeHint().width());
    resize(width, layout()->heightForWidth(width));
  };
  // Manally force an update for the initial selection
  updateHelpText();
  connect(ui->mHost, signal, updateHelpText);

  ui->mUrl->setText(account ? account->url() : Account::defaultUrl(kind));
  connect(ui->mHost, signal, [this] {
    Account::Kind kind =
        static_cast<Account::Kind>(ui->mHost->currentData().toInt());
    ui->mUrl->setText(Account::defaultUrl(kind));
  });

  ui->mButtons->button(QDialogButtonBox::Ok)->setEnabled(false);
  connect(ui->mButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(ui->mButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  updateButtons();
}

AccountDialog::~AccountDialog() = default;

void AccountDialog::accept() {
  // Validate account.
  Account::Kind kind =
      static_cast<Account::Kind>(ui->mHost->currentData().toInt());
  QString username = ui->mUsername->text();
  QString url = (ui->mUrl->text() != Account::defaultUrl(kind))
                    ? ui->mUrl->text()
                    : QString();

  if (Account *account = Accounts::instance()->lookup(username, kind)) {
    QMessageBox mb(QMessageBox::Information, tr("Replace?"),
                   tr("An account of this type already exists."));
    mb.setInformativeText(
        tr("Would you like to replace the previous account?"));
    QPushButton *remove = mb.addButton(tr("Replace"), QMessageBox::AcceptRole);
    mb.addButton(tr("Cancel"), QMessageBox::RejectRole);
    mb.setDefaultButton(remove);
    mb.exec();

    if (mb.clickedButton() != remove)
      return;

    Accounts::instance()->removeAccount(account);
  }

  Account *account = Accounts::instance()->createAccount(kind, username, url);
  AccountProgress *progress = account->progress();
  connect(progress, &AccountProgress::finished, this, [this, account] {
    AccountError *error = account->error();
    if (error->isValid()) {
      QString text = error->text();
      QString title = tr("Connection Failed");
      QMessageBox msg(QMessageBox::Warning, title, text, QMessageBox::Ok);
      msg.setInformativeText(error->detailedText());
      msg.exec();

      Accounts::instance()->removeAccount(account);
      return;
    }

    // Store password.
    QUrl url;
    url.setScheme("https");
    url.setHost(account->host());

    CredentialHelper *helper = CredentialHelper::instance();
    helper->store(url.toString(), account->username(), ui->mPassword->text());

    QDialog::accept();
  });

  // Start asynchronous connection.
  account->connect(ui->mPassword->text());
}

void AccountDialog::setKind(Account::Kind kind) {
  ui->mHost->setCurrentIndex(ui->mHost->findData(kind));
}

void AccountDialog::updateButtons() {
  ui->mButtons->button(QDialogButtonBox::Ok)
      ->setEnabled(!ui->mUsername->text().isEmpty() &&
                   !ui->mPassword->text().isEmpty());
}

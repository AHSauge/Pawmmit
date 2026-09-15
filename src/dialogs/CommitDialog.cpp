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

#include "CommitDialog.h"

#include "conf/Settings.h"
#include "ui_CommitDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

CommitDialog::CommitDialog(const QString &message, Prompt::Kind kind,
                           QWidget *parent)
    : QDialog(parent), ui(new Ui::CommitDialog) {
  setAttribute(Qt::WA_DeleteOnClose);

  ui->setupUi(this);

  QString title;
  switch (kind) {
    case Prompt::Kind::Merge:
      title = tr("Merge commit message");
      break;

    case Prompt::Kind::Stash:
      title = tr("Stash commit message");
      break;

    case Prompt::Kind::Revert:
      title = tr("Revert commit message");
      break;

    case Prompt::Kind::CherryPick:
      title = tr("Cherry-pick commit message");
      break;

    case Prompt::Kind::Directories:
    case Prompt::Kind::LargeFiles:
      Q_ASSERT(false);
      break;
  }

  setWindowTitle(title);
  ui->mLabel->setText(QString("<b>%1:</b>").arg(title));

  ui->mEditor->setText(message);

  Settings *settings = Settings::instance();
  ui->mPrompt->setText(settings->promptDescription(kind));
  ui->mPrompt->setChecked(settings->prompt(kind));
  connect(ui->mPrompt, &QCheckBox::toggled, this, [kind](bool checked) {
    Settings::instance()->setPrompt(kind, checked);
  });

  connect(ui->mButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(ui->mButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  switch (kind) {
    case Prompt::Kind::Merge:
      ui->mButtons->addButton(tr("Merge"), QDialogButtonBox::AcceptRole);
      ui->mButtons->addButton(tr("Abort"), QDialogButtonBox::RejectRole);
      break;

    case Prompt::Kind::Stash:
      ui->mButtons->addButton(tr("Stash"), QDialogButtonBox::AcceptRole);
      ui->mButtons->addButton(QDialogButtonBox::Cancel);
      break;

    case Prompt::Kind::Revert:
      ui->mButtons->addButton(tr("Revert"), QDialogButtonBox::AcceptRole);
      ui->mButtons->addButton(tr("Abort"), QDialogButtonBox::RejectRole);
      break;

    case Prompt::Kind::CherryPick:
      ui->mButtons->addButton(tr("Cherry-pick"), QDialogButtonBox::AcceptRole);
      ui->mButtons->addButton(tr("Abort"), QDialogButtonBox::RejectRole);
      break;

    case Prompt::Kind::Directories:
    case Prompt::Kind::LargeFiles:
      Q_ASSERT(false);
      break;
  }

  // Start with focus on the accept button.
  ui->mButtons->setFocus();
}

CommitDialog::~CommitDialog() = default;

QString CommitDialog::message() const { return ui->mEditor->toPlainText(); }

void CommitDialog::open() {
  QDialog::open();
  ui->mEditor->clearFocus();
}

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

#include <algorithm>

#include "TagDialog.h"
#include "git/Repository.h"
#include "git/TagRef.h"
#include "git2/tag.h"
#include "ui/ExpandButton.h"
#include "ui_TagDialog.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>
#include <QTextEdit>

TagDialog::TagDialog(const git::Repository &repo, const QString &id,
                     const git::Remote &remote, QWidget *parent)
    : QDialog(parent), mRemote(remote), ui(new Ui::TagDialog) {
  setAttribute(Qt::WA_DeleteOnClose);

  ui->setupUi(this);

  QString text = tr("Add a new tag at %1").arg(id);
  ui->mLabel->setText(QString("<b>%1:</b>").arg(text));

  ui->mPush->setText(tr("Push to %1").arg(remote.name()));
  ui->formLayout->setRowVisible(ui->mPush, remote.isValid());

  connect(ui->mAnnotated, &QCheckBox::toggled, ui->mExpand,
          &ExpandButton::setChecked);

  connect(ui->mAnnotated, &QCheckBox::toggled, ui->mMessage,
          &QTextEdit::setEnabled);
  connect(ui->mExpand, &ExpandButton::toggled, [this](bool checked) {
    ui->mMessage->setVisible(checked);
    layout()->activate();
    resize(minimumSizeHint());
  });

  connect(ui->mButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(ui->mButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  QPushButton *create =
      ui->mButtons->addButton(tr("Create Tag"), QDialogButtonBox::AcceptRole);
  create->setEnabled(false);

  // filtering must be done only once, because mExistingTags does not change
  // during the use of this dialog
  mExistingTags = repo.existingTags(),
  // filter descending. V1.1 before V1.0 makes more sense, because normaly the
  // next greater number will be choosen and so it can be seen faster
      std::sort(mExistingTags.begin(), mExistingTags.end(), std::greater<>());
  mFilteredTags = mExistingTags;

  ui->mListWidget->addItems(mExistingTags);

  connect(
      ui->mListWidget, &QListWidget::itemDoubleClicked,
      [this](QListWidgetItem *item) { ui->mNameField->setText(item->text()); });

  auto updateButton = [this, repo, create] {
    QString name = ui->mNameField->text();
    int value = QString::compare(name, mOldTagname);
    mOldTagname = name;
    bool force = ui->mForce->isChecked();
    create->setEnabled(!name.isEmpty() &&
                       (force || !repo.lookupTag(name).isValid()) &&
                       (!ui->mAnnotated->isChecked() ||
                        !ui->mMessage->toPlainText().isEmpty()));

    if (value >= 0) {
      // a new character was added, so the filtered data can be filtered again
      mFilteredTags =
          mFilteredTags.filter(name, Qt::CaseSensitivity::CaseSensitive);
    } else {
      mFilteredTags =
          mExistingTags.filter(name, Qt::CaseSensitivity::CaseSensitive);
    }

    ui->mListWidget->clear();
    ui->mListWidget->addItems(mFilteredTags);
  };

  connect(ui->mNameField, &QLineEdit::textChanged, updateButton);
  connect(ui->mForce, &QCheckBox::toggled, updateButton);
  connect(ui->mAnnotated, &QCheckBox::toggled, updateButton);
  connect(ui->mMessage, &QTextEdit::textChanged, updateButton);
}

TagDialog::~TagDialog() = default;

bool TagDialog::force() const { return ui->mForce->isChecked(); }

git::Remote TagDialog::remote() const {
  if (mRemote.isValid() && ui->mPush->isChecked())
    return mRemote;
  else
    return git::Remote();
}

QString TagDialog::name() const { return ui->mNameField->text(); }

QString TagDialog::message() const { return ui->mMessage->toPlainText(); }

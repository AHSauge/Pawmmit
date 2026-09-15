//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "InfoBox.h"
#include "DateSelectionGroupWidget.h"
#include "ui_InfoBox.h"
#include <QDateTimeEdit>
#include <QLineEdit>

InfoBox::InfoBox(QWidget *parent) : QGroupBox(parent), ui(new Ui::InfoBox) {
  ui->setupUi(this);

  connect(ui->mCommitDateType, &DateSelectionGroupWidget::typeChanged, this,
          &InfoBox::dateTimeTypeChanged);
}

InfoBox::~InfoBox() = default;

void InfoBox::setSignature(const git::Signature &signature) {
  mOriginalDate = signature.date().toLocalTime();
  ui->mName->setText(signature.name());
  ui->mEmail->setText(signature.email());
  ui->mCommitDate->setDateTime(mOriginalDate);

  // objectName() is set by the promoting .ui only after construction, so
  // these child names can't be derived until now.
  ui->mCommitDateType->setObjectName(objectName() + "CommitDateType");
  ui->mCommitDate->setObjectName(objectName() + "CommitDate");

  dateTimeTypeChanged(ui->mCommitDateType->type());
}

ContributorInfo InfoBox::getInfo() const {
  ContributorInfo ci;
  ci.name = name();
  ci.email = email();
  ci.commitDate = commitDate();
  ci.commitDateType = commitDateType();
  return ci;
}

void InfoBox::dateTimeTypeChanged(ContributorInfo::SelectedDateTimeType type) {
  const bool enabled = type == ContributorInfo::SelectedDateTimeType::Manual;
  ui->mLCommitDate->setVisible(enabled);
  ui->mCommitDate->setVisible(enabled);
}

QString InfoBox::name() const { return ui->mName->text(); }

QString InfoBox::email() const { return ui->mEmail->text(); }

QDateTime InfoBox::commitDate() const {
  if (commitDateType() == ContributorInfo::SelectedDateTimeType::Original)
    return mOriginalDate;
  return ui->mCommitDate->dateTime();
}

ContributorInfo::SelectedDateTimeType InfoBox::commitDateType() const {
  return ui->mCommitDateType->type();
}

//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "DateSelectionGroupWidget.h"
#include "ui_DateSelectionGroupWidget.h"
#include <QRadioButton>

DateSelectionGroupWidget::DateSelectionGroupWidget(QWidget *parent)
    : QGroupBox(parent), ui(new Ui::DateSelectionGroupWidget) {
  ui->setupUi(this);

  connect(ui->mCurrent, &QRadioButton::clicked,
          [this] { emit typeChanged(type()); });
  connect(ui->mManual, &QRadioButton::clicked,
          [this] { emit typeChanged(type()); });
  connect(ui->mOriginal, &QRadioButton::clicked,
          [this] { emit typeChanged(type()); });
}

DateSelectionGroupWidget::~DateSelectionGroupWidget() = default;

ContributorInfo::SelectedDateTimeType DateSelectionGroupWidget::type() const {
  if (ui->mOriginal->isChecked())
    return ContributorInfo::SelectedDateTimeType::Original;
  if (ui->mManual->isChecked())
    return ContributorInfo::SelectedDateTimeType::Manual;
  return ContributorInfo::SelectedDateTimeType::Current;
}

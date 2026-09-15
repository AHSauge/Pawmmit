//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef DATESELECTIONGROUPWIDGET_H
#define DATESELECTIONGROUPWIDGET_H

#include "AmendInfo.h"
#include <QGroupBox>
#include <QScopedPointer>

namespace Ui {
class DateSelectionGroupWidget;
}

class DateSelectionGroupWidget : public QGroupBox {
  Q_OBJECT

public:
  DateSelectionGroupWidget(QWidget *parent = nullptr);
  ~DateSelectionGroupWidget() override;

  ContributorInfo::SelectedDateTimeType type() const;

signals:
  void typeChanged(ContributorInfo::SelectedDateTimeType type);

private:
  QScopedPointer<Ui::DateSelectionGroupWidget> ui;
};

#endif

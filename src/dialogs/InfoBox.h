//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INFOBOX_H
#define INFOBOX_H

#include "AmendInfo.h"
#include "git/Signature.h"
#include <QDateTime>
#include <QGroupBox>
#include <QScopedPointer>

namespace Ui {
class InfoBox;
}

class InfoBox : public QGroupBox {
  Q_OBJECT

public:
  InfoBox(QWidget *parent = nullptr);
  ~InfoBox() override;

  void setSignature(const git::Signature &signature);
  ContributorInfo getInfo() const;

private:
  void dateTimeTypeChanged(ContributorInfo::SelectedDateTimeType type);

  QString name() const;
  QString email() const;
  QDateTime commitDate() const;
  ContributorInfo::SelectedDateTimeType commitDateType() const;

  QDateTime mOriginalDate;
  QScopedPointer<Ui::InfoBox> ui;
};

#endif

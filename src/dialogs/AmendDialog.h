//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef AMENDDIALOG_H
#define AMENDDIALOG_H

#include "AmendInfo.h"
#include "git/Signature.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui {
class AmendDialog;
}

class AmendDialog : public QDialog {
  Q_OBJECT

public:
  AmendDialog(const git::Signature &author, const git::Signature &committer,
              const QString &commitMessage, QWidget *parent = nullptr);
  ~AmendDialog() override;

  AmendInfo getInfo() const;

private:
  QScopedPointer<Ui::AmendDialog> ui;
};

#endif

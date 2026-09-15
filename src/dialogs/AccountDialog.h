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

#ifndef ACCOUNTDIALOG
#define ACCOUNTDIALOG

#include "host/Account.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui {
class AccountDialog;
}

class AccountDialog : public QDialog {
  Q_OBJECT

public:
  AccountDialog(Account *account, QWidget *parent = nullptr);
  ~AccountDialog() override;

  void accept() override;

  void setKind(Account::Kind kind);

private:
  void updateButtons();

  QScopedPointer<Ui::AccountDialog> ui;
};

#endif

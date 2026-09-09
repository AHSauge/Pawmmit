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
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>

class AccountDialog : public QDialog {
  Q_OBJECT

public:
  AccountDialog(Account *account, QWidget *parent = nullptr);

  void accept() override;

  void setKind(Account::Kind kind);

private:
  void updateButtons();

  QComboBox *mHost;
  QLineEdit *mUsername;
  QLineEdit *mPassword;
  QLabel *mLabel;
  QLineEdit *mUrl;
  QDialogButtonBox *mButtons;
};

#endif

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

#include "MiscPanel.h"
#include "conf/Settings.h"
#include "ui_MiscPanel.h"
#include <QLineEdit>

MiscPanel::MiscPanel(QWidget *parent) : QWidget(parent), ui(new Ui::MiscPanel) {
  ui->setupUi(this);

  Settings *settings = Settings::instance();

  ui->mSshConfigPath->setText(
      settings->value(Setting::Id::SshConfigFilePath).toString());
  connect(ui->mSshConfigPath, &QLineEdit::textChanged, [](const QString &text) {
    Settings::instance()->setValue(Setting::Id::SshConfigFilePath, text);
  });

  ui->mSshKeyPath->setText(
      settings->value(Setting::Id::SshKeyFilePath).toString());
  connect(ui->mSshKeyPath, &QLineEdit::textChanged, [](const QString &text) {
    Settings::instance()->setValue(Setting::Id::SshKeyFilePath, text);
  });
}

MiscPanel::~MiscPanel() = default;

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

#include "TerminalPanel.h"

#ifdef Q_OS_UNIX

#include "cli/Installer.h"
#include "conf/Settings.h"
#include "ui_TerminalPanel.h"
#include <QLineEdit>
#include <QPushButton>

TerminalPanel::TerminalPanel(QWidget *parent)
    : QWidget(parent), ui(new Ui::TerminalPanel) {
  ui->setupUi(this);

  Settings *settings = Settings::instance();

  ui->mNameBox->setText(settings->value(Setting::Id::TerminalName).toString());
  connect(ui->mNameBox, &QLineEdit::textChanged, [this](const QString &text) {
    Settings::instance()->setValue(Setting::Id::TerminalName, text);
    updateInstallButton();
  });

  ui->mPathBox->setText(settings->value(Setting::Id::TerminalPath).toString());
  connect(ui->mPathBox, &QLineEdit::textChanged, [this](const QString &text) {
    Settings::instance()->setValue(Setting::Id::TerminalPath, text);
    updateInstallButton();
  });

  connect(ui->mInstallButton, &QPushButton::clicked, [this] {
    Installer installer(ui->mNameBox->text(), ui->mPathBox->text());
    if (installer.isInstalled()) {
      installer.uninstall();
    } else if (!installer.exists()) {
      installer.install();
    }

    updateInstallButton();
  });

  updateInstallButton();
}

TerminalPanel::~TerminalPanel() = default;

void TerminalPanel::updateInstallButton() {
  Installer installer(ui->mNameBox->text(), ui->mPathBox->text());
  bool installed = installer.isInstalled();
  ui->mInstallButton->setText(!installed ? tr("Install") : tr("Uninstall"));
  ui->mInstallButton->setEnabled(installed || !installer.exists());
}

#endif

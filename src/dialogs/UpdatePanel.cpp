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

#include "UpdatePanel.h"
#include "conf/Settings.h"
#include "ui_UpdatePanel.h"
#include "update/Updater.h"
#include <QCheckBox>
#include <QFormLayout>
#include <QPushButton>

UpdatePanel::UpdatePanel(QWidget *parent)
    : QWidget(parent), ui(new Ui::UpdatePanel) {
  ui->setupUi(this);

  Settings *settings = Settings::instance();

  ui->mCheck->setChecked(
      settings->value(Setting::Id::CheckForUpdatesAutomatically).toBool());
  connect(ui->mCheck, &QCheckBox::toggled, [](bool checked) {
    Settings::instance()->setValue(Setting::Id::CheckForUpdatesAutomatically,
                                   checked);
  });

  // On linux the packages get installed over the package manager. So no
  // manual download is needed.
#if !defined(Q_OS_LINUX) || defined(FLATPAK)
  ui->mDownload->setChecked(
      settings->value(Setting::Id::InstallUpdatesAutomatically).toBool());
  connect(ui->mDownload, &QCheckBox::toggled, [](bool checked) {
    Settings::instance()->setValue(Setting::Id::InstallUpdatesAutomatically,
                                   checked);
  });
#else
  ui->formLayout->setRowVisible(ui->mDownload, false);
#endif

  connect(ui->mCheckNow, &QPushButton::clicked, Updater::instance(),
          &Updater::update);
}

UpdatePanel::~UpdatePanel() = default;

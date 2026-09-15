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

#include "ToolsPanel.h"
#include "ExternalToolsDialog.h"
#include "conf/Settings.h"
#include "tools/ExternalTool.h"
#include "ui_ToolsPanel.h"
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <algorithm>

namespace {

void populateExternalTools(QComboBox *comboBox, const QString &type) {
  comboBox->clear();

  QList<ExternalTool::Info> tools = ExternalTool::readGlobalTools(type) +
                                    ExternalTool::readBuiltInTools(type);

  QStringList names;
  for (const ExternalTool::Info &tool : tools) {
    if (tool.found)
      names.append(tool.name);
  }

  std::sort(names.begin(), names.end());
  for (const QString &tool : names)
    comboBox->addItem(tool);
}

} // namespace

ToolsPanel::ToolsPanel(QWidget *parent)
    : QWidget(parent), mConfig(git::Config::global()), ui(new Ui::ToolsPanel) {
  ui->setupUi(this);

  // external editor
  ui->mEditTool->setText(mConfig.value<QString>("gui.editor"));
  connect(ui->mEditTool, &QLineEdit::textChanged, this,
          [this](const QString &text) {
            if (text.isEmpty()) {
              mConfig.remove("gui.editor");
            } else {
              mConfig.setValue("gui.editor", text);
            }
          });

  // external diff/merge
  setupExternalTool(ui->mDiffTool, ui->mDiffConfigure, "diff");
  setupExternalTool(ui->mMergeTool, ui->mMergeConfigure, "merge");

  // backup files
  ui->mBackup->setChecked(mConfig.value<bool>("mergetool.keepBackup"));
  connect(ui->mBackup, &QCheckBox::toggled, this, [this](bool checked) {
    mConfig.setValue("mergetool.keepBackup", checked);
  });

  ui->mTerminalCommand->setText(
      Settings::instance()->value(Setting::Id::TerminalCommand).toString());
  connect(ui->mTerminalCommand, &QLineEdit::textChanged,
          [](const QString &text) {
            Settings::instance()->setValue(Setting::Id::TerminalCommand, text);
          });

  connect(ui->mFileManagerCommand, &QLineEdit::textChanged,
          [](const QString &text) {
            Settings::instance()->setValue(Setting::Id::FilemanagerCommand,
                                           text);
          });
  ui->mFileManagerCommand->setText(
      Settings::instance()->value(Setting::Id::FilemanagerCommand).toString());
}

ToolsPanel::~ToolsPanel() = default;

void ToolsPanel::setupExternalTool(QComboBox *comboBox, QPushButton *configure,
                                   const QString &type) {
  // Fill combo box with git config entries.
  populateExternalTools(comboBox, type);

  // Read tool from git config.
  QString key = QString("%1.tool").arg(type);
  QString name = mConfig.value<QString>(key);
  comboBox->setCurrentIndex(comboBox->findText(name));

  // React to combo box selections.
  auto signal = QOverload<int>::of(&QComboBox::currentIndexChanged);
  connect(comboBox, signal, this, [this, key, comboBox](int index) {
    mConfig.setValue(key, comboBox->currentText());
  });

  connect(configure, &QPushButton::clicked, this, [this, comboBox, type] {
    ExternalToolsDialog *dialog = new ExternalToolsDialog(type, this);

    // Update combo box when external tools dialog closes.
    connect(dialog, &QDialog::finished, this, [comboBox, type] {
      QString name = comboBox->currentText();
      populateExternalTools(comboBox, type);
      comboBox->setCurrentIndex(comboBox->findText(name));
    });

    dialog->open();
  });
}

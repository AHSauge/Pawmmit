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

#include "EditorPanel.h"
#include "conf/Settings.h"
#include "ui_EditorPanel.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QSpinBox>

EditorPanel::EditorPanel(QWidget *parent)
    : QWidget(parent), ui(new Ui::EditorPanel) {
  ui->setupUi(this);

  auto spin = QOverload<int>::of(&QSpinBox::valueChanged);
  auto combo = QOverload<int>::of(&QComboBox::currentIndexChanged);

  Settings *settings = Settings::instance();

  ui->mFont->setCurrentText(
      settings->value(Setting::Id::FontFamily).toString());
  connect(ui->mFont, &QFontComboBox::currentTextChanged,
          [](const QString &text) {
            Settings::instance()->setValue(Setting::Id::FontFamily, text);
          });

  ui->mFontSize->setValue(settings->value(Setting::Id::FontSize).toInt());
  connect(ui->mFontSize, spin, [](int i) {
    Settings::instance()->setValue(Setting::Id::FontSize, i);
  });

  ui->mShowWhitespace->setChecked(
      settings->value(Setting::Id::ShowWhitespaceInEditor).toBool());
  connect(ui->mShowWhitespace, &QCheckBox::toggled, [](bool checked) {
    Settings::instance()->setValue(Setting::Id::ShowWhitespaceInEditor,
                                   checked);
  });

  ui->mIndent->setCurrentIndex(
      settings->value(Setting::Id::UseTabsForIndent).toBool() ? 0 : 1);
  connect(ui->mIndent, combo, [](int i) {
    Settings::instance()->setValue(Setting::Id::UseTabsForIndent, i == 0);
  });

  ui->mIndentWidth->setValue(settings->value(Setting::Id::IndentWidth).toInt());
  connect(ui->mIndentWidth, spin, [](int i) {
    Settings::instance()->setValue(Setting::Id::IndentWidth, i);
  });

  ui->mTabWidth->setValue(settings->value(Setting::Id::TabWidth).toInt());
  connect(ui->mTabWidth, spin, [](int i) {
    Settings::instance()->setValue(Setting::Id::TabWidth, i);
  });

  ui->mBlameHeatMap->setChecked(
      settings->value(Setting::Id::ShowHeatmapInBlameMargin).toBool());
  connect(ui->mBlameHeatMap, &QCheckBox::toggled, [](bool checked) {
    Settings::instance()->setValue(Setting::Id::ShowHeatmapInBlameMargin,
                                   checked);
  });
}

EditorPanel::~EditorPanel() = default;

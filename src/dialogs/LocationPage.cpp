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

#include "LocationPage.h"
#include "ui_LocationPage.h"
#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QSettings>
#include <QUrl>
#include <QWizard>

namespace {
const QString kPathKey = "repo/path";
}

LocationPage::LocationPage(bool init, QWidget *parent)
    : QWizardPage(parent), mInit(init), ui(new Ui::LocationPage) {
  ui->setupUi(this);

  setButtonText(init ? QWizard::FinishButton : QWizard::NextButton,
                init ? tr("Initialize") : tr("Clone"));

  connect(ui->mName, &QLineEdit::textChanged, this,
          &LocationPage::updateFullPath);
  connect(ui->mPath, &QLineEdit::textChanged, this,
          &LocationPage::updateFullPath);

  connect(ui->mBrowse, &QPushButton::clicked, [this]() {
    QString title = tr("Choose Directory");
    QFileDialog *dialog =
        new QFileDialog(this, title, ui->mPath->text(), QString());
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly);
    connect(dialog, &QFileDialog::fileSelected,
            [this](const QString &file) { ui->mPath->setText(file); });

    dialog->open();
  });

  // Register fields.
  registerField("name", ui->mName);
  registerField("path", ui->mPath);
  registerField("bare", ui->mBare);
}

LocationPage::~LocationPage() = default;

bool LocationPage::isComplete() const {
  QString path = ui->mPath->text();
  return (!ui->mName->text().isEmpty() && !path.isEmpty() &&
          QDir(path).exists());
}

int LocationPage::nextId() const { return mInit ? -1 : QWizardPage::nextId(); }

void LocationPage::initializePage() {
  QUrl url(field("url").toString());
  QString name = QFileInfo(url.path()).fileName();
  ui->mName->setText(name.endsWith(".git") ? name.chopped(4) : name);
  ui->mPath->setText(QSettings().value(kPathKey, QDir::homePath()).toString());
}

void LocationPage::updateFullPath() {
  QString name = ui->mName->text();
  QString path = ui->mPath->text();
  bool valid = !path.isEmpty() && !name.isEmpty();
  ui->mFullPath->setText(valid ? QDir(path).filePath(name) : QString());
  ui->mFullPath->setCursorPosition(0);

  emit completeChanged();
}

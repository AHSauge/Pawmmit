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

#include "AboutDialog.h"
#include "IconLabel.h"
#include "conf/Settings.h"
#include "ui_AboutDialog.h"
#include "version.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QLabel>
#include <QLocale>
#include <QPointer>
#include <QTabBar>
#include <QTextBrowser>

namespace {

const QString kIssueTracker =
    QStringLiteral("https://github.com/Pawmmit/Pawmmit/issues");

const QString kUrl =
    "https://stackoverflow.com/questions/tagged/pawmmit?sort=frequent";

const QString kSubtitleFmt = "<h4 style='margin-top: 0px; color: gray'>%2</h4>";

const QString kTextFmt =
    "<p style='white-space: nowrap'><b style='font-size: large'>%1 v%2</b> "
    "- %3 - %4<br>Copyright © 2021-" +
    QString::number(CURR_YEAR) +
    " Pawmmit and Gittyup contributors"
    "<br>Copyright © 2016-2020 Scientific Toolworks, Inc. and "
    "contributors</p><p>Licensed under the "
    "<a href='https://www.gnu.org/licenses/gpl-3.0.html'>GNU General Public "
    "License, version 3</a> or, at your option, any later version. Portions "
    "were originally published under the MIT license.</p><p> If you have a "
    "question that might benefit the "
    "community, consider asking it on <a href='%5'>Stack Overflow</a> by "
    "including 'pawmmit' in the tags. Otherwise, contact us at "
    "<a href='%6'>%6</a>.";

const QString kStyleSheet = "h3 {"
                            "  text-decoration: underline"
                            "}"
                            "h4 {"
                            "  color: #696969"
                            "}";

const Qt::TextInteractionFlags kTextFlags =
    Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;

} // namespace

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog) {
  QString name = QCoreApplication::applicationName();
  QString version = QCoreApplication::applicationVersion();

  setAttribute(Qt::WA_DeleteOnClose);

  ui->setupUi(this);
  setWindowTitle(tr("About %1").arg(name));

#if defined(Q_OS_MAC)
  ui->rightLayout->setAlignment(ui->mTabs, Qt::AlignCenter);
#endif

  QIcon icon(":/Pawmmit.iconset/icon_128x128.png");
  ui->mIconLabel->setIcon(icon, 128, 128);

  QIcon title(":/logo-type_light@2x.png");
  ui->mTitleLabel->setIcon(title, 163, 38);

  ui->mSubtitle->setText(
      kSubtitleFmt.arg(tr("Claw your way into your git history")));

  QString revision = PAWMMIT_BUILD_REVISION;
  QDateTime dateTime = QDateTime::fromString(PAWMMIT_BUILD_DATE, Qt::ISODate);
  QString date =
      dateTime.date().toString(QLocale().dateFormat(QLocale::LongFormat));
  ui->mLabel->setText(
      kTextFmt.arg(name, version, date, revision, kUrl, kIssueTracker));
  ui->mLabel->setTextInteractionFlags(kTextFlags);
  connect(ui->mLabel, &QLabel::linkActivated, QDesktopServices::openUrl);

  ui->mTabs->setTabData(ui->mTabs->addTab(tr("Changelog")), "changelog.html");
  ui->mTabs->setTabData(ui->mTabs->addTab(tr("Acknowledgments")),
                       "acknowledgments.html");
  ui->mTabs->setTabData(ui->mTabs->addTab(tr("Privacy")), "privacy.html");

  ui->mBrowser->document()->setDocumentMargin(12);
  ui->mBrowser->document()->setDefaultStyleSheet(kStyleSheet);

  connect(ui->mTabs, &QTabBar::currentChanged, this, [this](int index) {
    QString url =
        Settings::docDir().filePath(ui->mTabs->tabData(index).toString());
    ui->mBrowser->setSource(QUrl::fromLocalFile(url));
  });

  // Load the initial content.
  emit ui->mTabs->currentChanged(ui->mTabs->currentIndex());
}

AboutDialog::~AboutDialog() = default;

void AboutDialog::openSharedInstance(Index index) {
  static QPointer<AboutDialog> dialog;
  if (dialog) {
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
  } else {
    dialog = new AboutDialog;
    dialog->show();
  }

  dialog->setCurrentIndex(index);
}

void AboutDialog::setCurrentIndex(Index index) {
  ui->mTabs->setCurrentIndex(index);
}

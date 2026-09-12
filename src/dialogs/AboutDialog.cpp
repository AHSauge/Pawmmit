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
#include "version.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QPointer>
#include <QTabBar>
#include <QTextBrowser>
#include <QVBoxLayout>

#ifdef Q_OS_MAC
#define TAB_BAR_ALIGNMENT Qt::AlignCenter
#else
#define TAB_BAR_ALIGNMENT Qt::Alignment()
#endif

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

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
  QString name = QCoreApplication::applicationName();
  QString version = QCoreApplication::applicationVersion();

  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("About %1").arg(name));

  QIcon icon(":/Pawmmit.iconset/icon_128x128.png");
  IconLabel *iconLabel = new IconLabel(icon, 128, 128, this);

  QIcon title(":/logo-type_light@2x.png");
  IconLabel *titleLabel = new IconLabel(title, 163, 38, this);

  QString subtitleText =
      kSubtitleFmt.arg(tr("Claw your way into your git history"));
  QLabel *subtitle = new QLabel(subtitleText, this);
  subtitle->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *left = new QVBoxLayout;
  left->addWidget(iconLabel);
  left->addWidget(titleLabel);
  left->addWidget(subtitle);
  left->addStretch();

  QString revision = PAWMMIT_BUILD_REVISION;
  QDateTime dateTime = QDateTime::fromString(PAWMMIT_BUILD_DATE, Qt::ISODate);
  QString date =
      dateTime.date().toString(QLocale().dateFormat(QLocale::LongFormat));
  QString text =
      kTextFmt.arg(name, version, date, revision, kUrl, kIssueTracker);
  QLabel *label = new QLabel(text, this);
  label->setWordWrap(true);
  label->setTextInteractionFlags(kTextFlags);
  connect(label, &QLabel::linkActivated, QDesktopServices::openUrl);

  mTabs = new QTabBar(this);
  mTabs->setTabData(mTabs->addTab(tr("Changelog")), "changelog.html");
  mTabs->setTabData(mTabs->addTab(tr("Acknowledgments")),
                    "acknowledgments.html");
  mTabs->setTabData(mTabs->addTab(tr("Privacy")), "privacy.html");

  QTextBrowser *browser = new QTextBrowser(this);
  browser->setOpenLinks(false);
  browser->document()->setDocumentMargin(12);
  browser->document()->setDefaultStyleSheet(kStyleSheet);

  connect(mTabs, &QTabBar::currentChanged, this, [this, browser](int index) {
    QString url = Settings::docDir().filePath(mTabs->tabData(index).toString());
    browser->setSource(QUrl::fromLocalFile(url));
  });

  // Load the initial content.
  emit mTabs->currentChanged(mTabs->currentIndex());

  QVBoxLayout *right = new QVBoxLayout;
  right->setSpacing(0);
  right->addWidget(label);
  right->addSpacing(12);
  right->addWidget(mTabs, 0, TAB_BAR_ALIGNMENT);
  right->addWidget(browser);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->addLayout(left);
  layout->addSpacing(8);
  layout->addLayout(right);
}

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
  mTabs->setCurrentIndex(index);
}

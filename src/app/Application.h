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

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Theme.h"
#include <QApplication>
#include <QTemporaryDir>

class QNetworkAccessManager;
class QNetworkReply;
class QSslError;
class QTranslator;
class QUrlQuery;

class Application : public QApplication {
  Q_OBJECT

public:
  Application(int &argc, char **argv, bool haltOnParseError = false);

  void autoUpdate();
  bool restoreWindows();
  bool runSingleInstance();

  static bool isInTest();
  static void setInTest();

  static Theme *theme();

  // Load the translations for language, replacing any loaded before.
  static void setLanguage(const QString &language);

protected:
  bool event(QEvent *event) override;

private:
  void registerService();
  void loadTranslations(const QString &language);
  void handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

  QString mPathspec = QString();
  std::unique_ptr<Theme> mTheme;
  std::unique_ptr<QTemporaryDir> mTempSettingsDir;
  QStringList mPositionalArguments;
  QTranslator *mTranslator = nullptr;
  QTranslator *mQtTranslator = nullptr;
  bool mTranslationDisabled = false;

  static bool mIsInTest;
};

#ifdef Q_OS_LINUX
class DBusPawmmit : public QObject {
  Q_OBJECT

public:
  DBusPawmmit(QObject *parent = nullptr);

public slots:
  Q_SCRIPTABLE void openRepository(const QString &repo);
  Q_SCRIPTABLE void openAndFocusRepository(const QString &repo);
  Q_SCRIPTABLE void setFocus();
};
#endif

#endif

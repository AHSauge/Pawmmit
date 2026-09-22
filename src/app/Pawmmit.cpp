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

#include "Application.h"
#include "dialogs/StartDialog.h"
#include "git/Config.h"
#include <QGuiApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
  // Match the OS scale factor exactly; rounding it mis-sizes the UI at
  // fractional scaling (125%, 150%, 175%).
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

  Application app(argc, argv, true);

  // Check if only one running instance is allowed and already running
  if (app.runSingleInstance())
    return 0;

  if (!git::Config::global().isValid()) {
    QMessageBox::warning(
        nullptr, PAWMMIT_NAME,
        QObject::tr("Your global GIT configuration is invalid, Pawmmit won't "
                    "run properly until this is fixed"));
  }

  // Restore windows before checking for updates so that
  // the update dialog pops up on top of the other windows.
  //
  // If there's nothing to restore, show the repository chooser instead of an
  // empty window. Dismissing it without opening anything quits the app, via
  // Qt's default quitOnLastWindowClosed behavior.
  if (!app.restoreWindows())
    StartDialog::openSharedInstance();

  // Check for updates.
  app.autoUpdate();

  return app.exec();
}

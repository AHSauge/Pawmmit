//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Shane Gramlich
//

#include "Test.h"
#include <QMainWindow>

using namespace Test;
using namespace QTest;

class TestSample : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void sample();
  void cleanupTestCase();

private:
  int inputDelay = 99;
  int closeDelay = 9999;

  ScratchRepository *mRepo = nullptr;
  QMainWindow *mWindow = nullptr;
};

void TestSample::initTestCase() {
  mRepo = new ScratchRepository;
  mWindow = new QMainWindow;
  // UiComponent *ui = new UiComponent(mRepo->repo(), mWindow);
  // ui->show();
  // QVERIFY(qWaitForWindowActive(ui));
}

void TestSample::sample() {
  // keyClick(ui, 'Qt::Key_Return', Qt::NoModifier, inputDelay);
}

void TestSample::cleanupTestCase() {
  qWait(closeDelay);
  mWindow->close();
  delete mRepo;
}

TEST_MAIN(TestSample)

#include "sample.moc"

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

#include "Test.h"

using namespace QTest;

class TestSanity : public QObject {
  Q_OBJECT

private slots:
  void sanity();
};

void TestSanity::sanity() {
  QCOMPARE(QCoreApplication::applicationName(), QString(GITTYUP_NAME));
  QCOMPARE(QCoreApplication::applicationVersion(), QString(GITTYUP_VERSION));
}

TEST_MAIN(TestSanity)

#include "sanity.moc"

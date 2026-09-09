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
#include "git/Diff.h"

using namespace QTest;

class TestDiff : public QObject {
public:
  TestDiff(){};
private slots:
  void testContainsPath1() {
    // /src/testfile.txt, /src/testfile.txt1 - path: /src/testfile.txt --> only
    // /src/testfile.txt is shown
    QString str("/src/testfile.txt");
    QString occurence("/src/testfile.txt");
    QVERIFY(containsPath(str, occurence));

    occurence = "/src/testfile.txt1";
    QVERIFY(!containsPath(str, occurence));
  }

  void testContainsPath2() {
    // /src/testfile.txt, /src/testfile.txt1 - path: /src --> testfile.txt and
    // testfile1.txt is shown
    QString str("/src");
    QString occurence("/src/testfile.txt");
    QVERIFY(containsPath(str, occurence));

    occurence = "/src/testfile.txt1";
    QVERIFY(containsPath(str, occurence));
  }

  void testContainsPath3() {
    // /src/test/test.txt11, /src/testfile.txt, /src/testfile.txt1 - path:
    // /src/test --> only /src/test/testtest.txt11 is shown
    QString str("/src/test");
    QString occurence("/src/test/test.txt11");
    QVERIFY(containsPath(str, occurence));

    occurence = "/src/testfile.txt";
    QVERIFY(!containsPath(str, occurence));

    occurence = "/src/testfile.txt1";
    QVERIFY(!containsPath(str, occurence));
  }
};

TEST_MAIN(TestDiff)

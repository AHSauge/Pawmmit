//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Test.h"
#include "git/Commit.h"
#include "index/Index.h"
#include "index/Query.h"
#include <QProcess>

class TestSearchIndex : public QObject {
  Q_OBJECT

private slots:
  void multiLineComment();

private:
  QList<git::Commit> search(const Index &index, const QString &query);
};

void TestSearchIndex::multiLineComment() {
  Test::ScratchRepository repo;
  QFile file(repo->workdir().filePath("main.cpp"));
  QVERIFY(file.open(QFile::WriteOnly));
  file.write("int a = 0;\n"
             "/*\n"
             " * zebrafish\n"
             " */\n"
             "int quokka = 1;\n");
  file.close();

  repo->index().setStaged({"main.cpp"}, true);
  QVERIFY(repo->commit("Add main", git::AnnotatedCommit()).isValid());

  QProcess indexer;
  indexer.start(INDEXER_PATH, {repo->workdir().path()});
  QVERIFY(indexer.waitForFinished(60000));
  QCOMPARE(indexer.exitCode(), 0);

  // The middle line of the block comment is only recognized as a comment
  // when it's lexed together with the lines around it.
  Index index(*repo.operator->());
  QCOMPARE(search(index, "comment:zebrafish").size(), 1);
  QCOMPARE(search(index, "comment:quokka").size(), 0);
  QCOMPARE(search(index, "addition:quokka").size(), 1);
}

QList<git::Commit> TestSearchIndex::search(const Index &index,
                                           const QString &query) {
  QueryRef ref = Query::parseQuery(query);
  return ref ? ref->commits(&index) : QList<git::Commit>();
}

TEST_MAIN(TestSearchIndex)

#include "search_index.moc"

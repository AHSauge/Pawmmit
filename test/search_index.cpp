//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Test.h"
#include "git/Commit.h"
#include "git/Index.h"
#include "git/Signature.h"
#include "index/Index.h"
#include "index/Query.h"
#include "dialogs/SearchPanel.h"
#include "ui/IndexCompleter.h"
#include "ui/MainWindow.h"
#include "ui/RepoView.h"
#include <QPushButton>
#include <QLineEdit>
#include <QProcess>
#include <QTimeZone>

class TestSearchIndex : public QObject {
  Q_OBJECT

private slots:
  void multiLineComment();
  void queries();
  void completer();
  void remove();
  void removeButton();

private:
  void runIndexer(git::Repository &repo);
  git::Commit commitFile(git::Repository &repo, const QString &path,
                         const QByteArray &content, const QString &message,
                         const git::Signature &author);
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

  // Forward the indexer's errors so that failures show up in the test log.
  QProcess indexer;
  indexer.setProcessChannelMode(QProcess::ForwardedErrorChannel);
  indexer.start(INDEXER_PATH, {repo->workdir().path()});
  QVERIFY(indexer.waitForFinished(60000));
  QCOMPARE(indexer.exitStatus(), QProcess::NormalExit);
  QCOMPARE(indexer.exitCode(), 0);

  // The middle line of the block comment is only recognized as a comment
  // when it's lexed together with the lines around it.
  Index index(*repo.operator->());
  QCOMPARE(search(index, "comment:zebrafish").size(), 1);
  QCOMPARE(search(index, "comment:quokka").size(), 0);
  QCOMPARE(search(index, "addition:quokka").size(), 1);
}

void TestSearchIndex::queries() {
  Test::ScratchRepository scratch;
  git::Repository repo = scratch;

  // 00:30 local time is still the previous day in UTC.
  QDateTime aliceDate(QDate(2024, 3, 11), QTime(0, 30),
                      QTimeZone::fromSecondsAheadOfUtc(2 * 60 * 60));
  git::Commit alice = commitFile(
      repo, "src/Alpha.cpp", "int zebra = 1; // walrus\n", "Add alpha feature",
      repo.signature("Alice Smith", "alice@example.com", aliceDate));
  QVERIFY(alice.isValid());

  QDateTime bobDate(QDate(2024, 5, 20), QTime(12, 0), QTimeZone::utc());
  git::Commit bob = commitFile(
      repo, "docs/readme.md", "penguin habitat\n", "Document penguin habitat",
      repo.signature("Bob Jones", "bob@example.com", bobDate));
  QVERIFY(bob.isValid());

  repo.setCommitStarred(alice.id(), true);
  runIndexer(repo);

  Index index(repo);
  auto matches = [&](const QString &query) {
    QList<git::Commit> commits = search(index, query);
    QStringList ids;
    for (const git::Commit &commit : commits)
      ids.append(commit == alice ? "alice" : commit == bob ? "bob" : "other");
    return ids;
  };

  // FTS terms, fields and boolean combinations, newest first.
  QCOMPARE(matches("zebra"), QStringList({"alice"}));
  QCOMPARE(matches("author:alice"), QStringList({"alice"}));
  QCOMPARE(matches("comment:walrus"), QStringList({"alice"}));
  QCOMPARE(matches("addition:walrus"), QStringList({"alice"}));
  QCOMPARE(matches("zebra OR penguin"), QStringList({"bob", "alice"}));
  QCOMPARE(matches("zebra penguin"), QStringList());
  QCOMPARE(matches("zebra author:alice"), QStringList({"alice"}));

  // Plain terms also match data outside the FTS table.
  QCOMPARE(matches("readme.md"), QStringList({"bob"}));
  QCOMPARE(matches("src/Alpha.cpp"), QStringList({"alice"}));
  QCOMPARE(matches("bob@example.com"), QStringList({"bob"}));
  QCOMPARE(matches("2024/03/11"), QStringList({"alice"}));
  QCOMPARE(matches(alice.id().toString().left(7)), QStringList({"alice"}));
  QCOMPARE(matches(alice.id().toString().left(6)), QStringList());
  QCOMPARE(matches("read*"), QStringList({"bob"}));
  QCOMPARE(matches("src/*.cpp"), QStringList({"alice"}));
  QCOMPARE(matches("zebra OR readme.md"), QStringList({"bob", "alice"}));
  QCOMPARE(matches("zebra readme.md"), QStringList());

  // Phrases keep their word order.
  QCOMPARE(matches("msg:\"penguin habitat\""), QStringList({"bob"}));
  QCOMPARE(matches("msg:\"habitat penguin\""), QStringList());

  // Prefixes use FTS5 directly, other wildcards expand to indexed terms.
  QCOMPARE(matches("zeb*"), QStringList({"alice"}));
  QCOMPARE(matches("z?bra"), QStringList({"alice"}));
  QCOMPARE(matches("q?okka"), QStringList());

  // Columns of the commits table, also combined with FTS terms.
  QCOMPARE(matches("email:bob@example.com"), QStringList({"bob"}));
  QCOMPARE(matches("zebra OR email:bob@example.com"),
           QStringList({"bob", "alice"}));
  QCOMPARE(matches("id:" + alice.id().toString().left(7)),
           QStringList({"alice"}));
  QCOMPARE(matches("is:starred"), QStringList({"alice"}));

  // Dates are in the committer's time zone.
  QCOMPARE(matches("date:2024/03/11"), QStringList({"alice"}));
  QCOMPARE(matches("date:2024/03/10"), QStringList());
  QCOMPARE(matches("before:2024/04/01"), QStringList({"alice"}));
  QCOMPARE(matches("after:2024/04/01"), QStringList({"bob"}));

  // Paths are matched without regard to case.
  QCOMPARE(matches("path:src/Alpha.cpp"), QStringList({"alice"}));
  QCOMPARE(matches("file:readme.md"), QStringList({"bob"}));
  QCOMPARE(matches("path:src/*.cpp"), QStringList({"alice"}));
  QCOMPARE(matches("pathspec:src"), QStringList({"alice"}));
  QCOMPARE(matches("pathspec:docs/*.md"), QStringList({"bob"}));

  // Suggestions come from the same data that searches match.
  auto suggest = [&](Index::Field field, const QString &prefix) {
    return index.suggestions(field, prefix, 100);
  };
  QCOMPARE(suggest(Index::Any, "zeb"), QStringList({"zebra"}));
  QCOMPARE(suggest(Index::Any, "Read"), QStringList({"readme.md"}));
  QCOMPARE(suggest(Index::Any, "src/"), QStringList({"src/alpha.cpp"}));
  QCOMPARE(suggest(Index::Any, "bob"), QStringList({"bob", "bob@example.com"}));
  QCOMPARE(suggest(Index::Email, "ali"), QStringList({"alice@example.com"}));
  QCOMPARE(suggest(Index::Author, "s"), QStringList({"smith"}));
  QCOMPARE(suggest(Index::Comment, "wal"), QStringList({"walrus"}));
  QCOMPARE(suggest(Index::Message, "zeb"), QStringList());
  QCOMPARE(suggest(Index::File, "a"), QStringList({"alpha.cpp"}));
  QCOMPARE(
      suggest(Index::Any, "a"),
      QStringList({"add", "alice", "alice@example.com", "alpha", "alpha.cpp"}));
  QCOMPARE(index.suggestions(Index::Any, "a", 2).size(), 2);
}

void TestSearchIndex::completer() {
  // The suggestions must be in place before the completer filters them.
  QLineEdit lineEdit;
  QStringList prefixes;
  IndexCompleter::Source source = [&prefixes](const QString &prefix) {
    prefixes.append(prefix);
    return QStringList({"zeal", "zebra"});
  };
  IndexCompleter *completer = new IndexCompleter(source, &lineEdit);
  lineEdit.setCompleter(completer);
  lineEdit.show();
  QVERIFY(QTest::qWaitForWindowExposed(&lineEdit));

  QTest::keyClicks(&lineEdit, "foo ze");
  QCOMPARE(prefixes.last(), QString("ze"));
  QCOMPARE(completer->completionCount(), 2);

  QTest::keyClicks(&lineEdit, "b");
  QCOMPARE(completer->completionCount(), 1);
  QCOMPARE(completer->currentCompletion(), QString("foo zebra"));
}

void TestSearchIndex::remove() {
  Test::ScratchRepository scratch;
  git::Repository repo = scratch;
  QVERIFY(commitFile(repo, "a.txt", "zebra\n", "Add a",
                     repo.signature("A", "a@example.com",
                                    QDateTime::currentDateTime()))
              .isValid());
  runIndexer(repo);

  QDir dir = Index::indexDir(repo);
  Index index(repo);
  QVERIFY(index.isValid());
  qInfo() << "before:" << dir.entryList(QDir::Files);

  QVERIFY(index.remove());
  qInfo() << "after:" << dir.entryList(QDir::Files);
  QVERIFY(!dir.exists("index.db"));
  QVERIFY(!index.isValid());
}

void TestSearchIndex::removeButton() {
  Test::ScratchRepository scratch;
  git::Repository repo = scratch;
  QVERIFY(commitFile(repo, "a.txt", "zebra\n", "Add a",
                     repo.signature("A", "a@example.com",
                                    QDateTime::currentDateTime()))
              .isValid());
  runIndexer(repo);

  // The same steps as clicking Remove in the search settings.
  QDir dir = Index::indexDir(repo);
  MainWindow window(repo);
  RepoView *view = window.currentView();
  QVERIFY(view->index()->isValid());
  SearchPanel panel(view);
  QPushButton *button = panel.findChild<QPushButton *>("mRemove");
  QVERIFY(button && button->isEnabled());
  qInfo() << "before:" << dir.entryList(QDir::Files);
  button->click();
  qInfo() << "after:" << dir.entryList(QDir::Files);
  QVERIFY(!dir.exists("index.db"));
}

void TestSearchIndex::runIndexer(git::Repository &repo) {
  // Forward the indexer's errors so that failures show up in the test log.
  QProcess indexer;
  indexer.setProcessChannelMode(QProcess::ForwardedErrorChannel);
  indexer.start(INDEXER_PATH, {repo.workdir().path()});
  QVERIFY(indexer.waitForFinished(60000));
  QCOMPARE(indexer.exitStatus(), QProcess::NormalExit);
  QCOMPARE(indexer.exitCode(), 0);
}

git::Commit TestSearchIndex::commitFile(git::Repository &repo,
                                        const QString &path,
                                        const QByteArray &content,
                                        const QString &message,
                                        const git::Signature &author) {
  QFileInfo info(repo.workdir().filePath(path));
  QDir().mkpath(info.path());
  QFile file(info.filePath());
  if (!file.open(QFile::WriteOnly))
    return git::Commit();
  file.write(content);
  file.close();

  repo.index().setStaged({path}, true);
  return repo.commit(author, author, message);
}

QList<git::Commit> TestSearchIndex::search(const Index &index,
                                           const QString &query) {
  QueryRef ref = Query::parseQuery(query);
  return ref ? ref->commits(&index) : QList<git::Commit>();
}

TEST_MAIN(TestSearchIndex)

#include "search_index.moc"

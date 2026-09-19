#include "Test.h"

#include "watcher/PathFilter.h"

#include <memory>

using namespace Test;

namespace {

bool writeFile(const QDir &workdir, const QString &path,
               const QByteArray &data = "x") {
  QFile file(workdir.filePath(path));
  return file.open(QFile::WriteOnly | QFile::Truncate) &&
         file.write(data) == data.size();
}

} // namespace

class TestPathFilter : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void isRelevant_data();
  void isRelevant();
  void classify_data();
  void classify();
  void withoutGitDir();
  void noticesLaterTracking();

private:
  ScratchRepository mRepo;
  QDir mWorkdir;
  std::unique_ptr<PathFilter> mFilter;
};

void TestPathFilter::initTestCase() {
  mWorkdir = mRepo->workdir();
  QVERIFY(mWorkdir.mkpath("sub/dir"));

  QVERIFY(writeFile(mWorkdir, ".gitignore", "*.ign\n"));
  for (const char *name : {"plain.txt", "tracked.ign", "untracked.ign",
                           "sub/dir/tracked.ign", "sub/dir/untracked.ign"})
    QVERIFY(writeFile(mWorkdir, name));

  QVERIFY(forceAdd(mWorkdir.path(), "tracked.ign", "x"));
  QVERIFY(forceAdd(mWorkdir.path(), "sub/dir/tracked.ign", "x"));

  mFilter = std::make_unique<PathFilter>(mRepo);
}

void TestPathFilter::isRelevant_data() {
  QTest::addColumn<QString>("path");
  QTest::addColumn<bool>("relevant");

  QTest::newRow("not ignored") << "plain.txt" << true;
  QTest::newRow("ignored, untracked") << "untracked.ign" << false;
  QTest::newRow("ignored, tracked") << "tracked.ign" << true;
  QTest::newRow("ignored, untracked, in subdirectory")
      << "sub/dir/untracked.ign" << false;
  QTest::newRow("ignored, tracked, in subdirectory")
      << "sub/dir/tracked.ign" << true;
  QTest::newRow("ignored, missing") << "gone.ign" << false;
  QTest::newRow("workdir, relative") << "." << true;
  QTest::newRow("workdir, absolute") << mWorkdir.path() << true;
  QTest::newRow("subdirectory") << "sub" << true;
  QTest::newRow("git directory") << ".git" << true;
  QTest::newRow("git index") << ".git/index" << true;
  QTest::newRow("git HEAD") << ".git/HEAD" << true;
  QTest::newRow("git packed refs") << ".git/packed-refs" << true;
  QTest::newRow("git merge head") << ".git/MERGE_HEAD" << true;
  QTest::newRow("git refs directory") << ".git/refs/heads" << true;
  QTest::newRow("git branch") << ".git/refs/heads/feature/x" << true;
  QTest::newRow("git tag") << ".git/refs/tags/v1" << true;
  QTest::newRow("git index lock") << ".git/index.lock" << false;
  QTest::newRow("git branch lock") << ".git/refs/heads/main.lock" << false;
  QTest::newRow("git objects") << ".git/objects" << false;
  QTest::newRow("git object") << ".git/objects/ab/cdef" << false;
  QTest::newRow("git commit message") << ".git/COMMIT_EDITMSG" << false;
  QTest::newRow("git hooks") << ".git/hooks/pre-commit" << false;
  QTest::newRow("absolute, git index")
      << mWorkdir.filePath(".git/index") << true;
  QTest::newRow("absolute, git object")
      << mWorkdir.filePath(".git/objects/ab/cdef") << false;
  QTest::newRow("absolute, not ignored")
      << mWorkdir.filePath("plain.txt") << true;
  QTest::newRow("absolute, ignored, untracked")
      << mWorkdir.filePath("untracked.ign") << false;
  QTest::newRow("absolute, ignored, tracked")
      << mWorkdir.filePath("tracked.ign") << true;
}

void TestPathFilter::isRelevant() {
  QFETCH(QString, path);
  QFETCH(bool, relevant);

  QCOMPARE(mFilter->isRelevant(path), relevant);
}

void TestPathFilter::classify_data() {
  QTest::addColumn<QString>("path");
  QTest::addColumn<int>("kind");

  using Kind = PathFilter::Kind;
  QTest::newRow("git index") << ".git/index" << int(Kind::Index);
  QTest::newRow("absolute, git index")
      << mWorkdir.filePath(".git/index") << int(Kind::Index);
  QTest::newRow("git HEAD") << ".git/HEAD" << int(Kind::Other);
  QTest::newRow("git branch") << ".git/refs/heads/x" << int(Kind::Other);
  QTest::newRow("git index lock") << ".git/index.lock" << int(Kind::Irrelevant);
  QTest::newRow("file") << "plain.txt" << int(Kind::Other);
  QTest::newRow("ignored, tracked") << "tracked.ign" << int(Kind::Other);
  QTest::newRow("ignored, untracked")
      << "untracked.ign" << int(Kind::Irrelevant);
}

void TestPathFilter::classify() {
  QFETCH(QString, path);
  QFETCH(int, kind);

  QCOMPARE(int(mFilter->classify(path)), kind);
}

void TestPathFilter::withoutGitDir() {
  PathFilter filter(mRepo, false);
  using Kind = PathFilter::Kind;

  QCOMPARE(filter.classify(".git"), Kind::Irrelevant);
  QCOMPARE(filter.classify(".git/index"), Kind::Irrelevant);
  QCOMPARE(filter.classify(".git/refs/heads/x"), Kind::Irrelevant);
  QCOMPARE(filter.classify("plain.txt"), Kind::Other);
  QCOMPARE(filter.classify("tracked.ign"), Kind::Other);
  QCOMPARE(filter.classify("untracked.ign"), Kind::Irrelevant);
}

void TestPathFilter::noticesLaterTracking() {
  QVERIFY(writeFile(mWorkdir, "later.ign"));
  QVERIFY(!mFilter->isRelevant("later.ign"));

  QVERIFY(forceAdd(mWorkdir.path(), "later.ign", "x"));
  QVERIFY(mFilter->isRelevant("later.ign"));
}

TEST_MAIN(TestPathFilter)
#include "PathFilter.moc"

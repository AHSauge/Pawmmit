#include "Test.h"

#include "watcher/RepositoryWatcher.h"

#include "git2/blob.h"
#include "git2/refs.h"
#include <QSignalSpy>
#include <filesystem>
#include <memory>

using namespace Test;

namespace {

const int kDebounceMs = 100;

// Late notifications from the previous change must land before the spy is
// cleared, so this has to comfortably exceed the debounce.
const int kSettleMs = 250;

// Long enough for a spurious notification to show up.
const int kQuietMs = 3 * kDebounceMs;

// Timeouts are generous so a starved CI runner can't cause false failures.
const int kAttemptMs = 1000;
const int kAttempts = 15;
const int kSignalMs = 15000;

struct Row {
  const char *name;
  const char *dir;
};

// Directories that exist before the watcher starts.
const Row kRows[] = {
    {"root", ""},
    {"visible", "src"},
    {"hidden", ".github"},
    {"under hidden", ".github/workflows"},
    {"hidden under visible", "src/.cache"},
};

bool writeFile(const QString &path, const QByteArray &data = QByteArray()) {
  static int counter = 0;
  QFile file(path);
  if (!file.open(QFile::WriteOnly | QFile::Truncate))
    return false;

  file.write(data.isEmpty() ? QByteArray::number(++counter) : data);
  return true;
}

// Overwrites the target the way an atomic save does.
bool replaceFile(const QString &from, const QString &to) {
  std::error_code ec;
  std::filesystem::rename(from.toStdString(), to.toStdString(), ec);
  return !ec;
}

// Changes the git directory like a command run outside the app would, except
// for the "own" kinds, which go through the app's repository like staging does.
bool changeGitDir(const QString &workdir, git::Repository *app,
                  const QString &kind) {
  if (kind == "index")
    return forceAdd(workdir, "staged.txt", "x");

  if (kind == "own index") {
    app->index().add("own.txt", "x");
    return true;
  }

  if (kind == "own then external index") {
    app->index().add("own.txt", "y");
    return forceAdd(workdir, "external.txt", "x");
  }

  if (kind == "commit message") {
    QFile file(QDir(workdir).filePath(".git/COMMIT_EDITMSG"));
    return file.open(QFile::WriteOnly) && file.write("x") == 1;
  }

  git_repository *repo = nullptr;
  if (git_repository_open(&repo, workdir.toUtf8()))
    return false;

  git_reference *ref = nullptr;
  git_oid oid;
  int error = -1;
  if (kind == "branch") {
    error = git_reference_symbolic_create(&ref, repo, "refs/heads/topic",
                                          "refs/heads/unborn", 1, nullptr);
  } else if (kind == "nested branch") {
    error = git_reference_symbolic_create(&ref, repo, "refs/heads/feature/deep",
                                          "refs/heads/unborn", 1, nullptr);
  } else if (kind == "HEAD") {
    error = git_repository_set_head(repo, "refs/heads/topic");
  } else if (kind == "object") {
    error = git_blob_create_from_buffer(&oid, repo, "x", 1);
  }

  git_reference_free(ref);
  git_repository_free(repo);
  return !error;
}

void settle(QSignalSpy &spy) {
  QTest::qWait(kSettleMs);
  spy.clear();
}

// The watcher installs its watches on a thread, so retry a root-level change
// until one is reported; by then every existing directory is watched too.
bool waitUntilWatching(QSignalSpy &spy, const QDir &workdir) {
  // Give the thread a head start; the retries below cover a slow one.
  QTest::qWait(kDebounceMs);

  for (int i = 0; i < kAttempts; ++i) {
    if (!writeFile(workdir.filePath("canary")))
      return false;

    if (spy.wait(kAttemptMs)) {
      settle(spy);
      return true;
    }
  }

  return false;
}

} // namespace

class TestRepositoryWatcher : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void existingDirectory_data();
  void existingDirectory();
  void newHiddenDirectory();
  void atomicReplace_data();
  void atomicReplace();
  void trackedFileMatchingIgnoreRule();
  void untrackedFileMatchingIgnoreRule();
  void gitDirectoryChange_data();
  void gitDirectoryChange();

private:
  // Order matters: the watcher must be destroyed before the repository.
  std::unique_ptr<ScratchRepository> mRepo;
  std::unique_ptr<RepositoryWatcher> mWatcher;
  std::unique_ptr<QSignalSpy> mSpy;
  QDir mWorkdir;
  QTemporaryDir mOutside;
};

void TestRepositoryWatcher::initTestCase() {
  mRepo = std::make_unique<ScratchRepository>();
  mWorkdir = (*mRepo)->workdir();
  for (const Row &row : kRows) {
    if (*row.dir)
      QVERIFY(mWorkdir.mkpath(row.dir));
  }

  // A rename target, ignore rules, and a tracked file that matches one.
  QVERIFY(writeFile(mWorkdir.filePath(".gitignore"), "*.tmp\n*.ign\n"));
  QVERIFY(writeFile(mWorkdir.filePath("atomic")));
  const QByteArray tracked = "tracked\n";
  QVERIFY(writeFile(mWorkdir.filePath("tracked.ign"), tracked));
  QVERIFY(forceAdd(mWorkdir.path(), "tracked.ign", tracked));
  (*mRepo)->index().read();
  QVERIFY((*mRepo)->index().isTracked("tracked.ign"));

  // A file for the app to stage through its own repository.
  QVERIFY(writeFile(mWorkdir.filePath("own.txt")));

  mWatcher.reset(RepositoryWatcher::create(*mRepo));
  mWatcher->setDebounceInterval(kDebounceMs);
  mSpy = std::make_unique<QSignalSpy>((*mRepo)->notifier(),
                                      &git::RepositoryNotifier::workdirChanged);
  QVERIFY2(waitUntilWatching(*mSpy, mWorkdir),
           "watcher never reported a root-level change");
}

void TestRepositoryWatcher::existingDirectory_data() {
  QTest::addColumn<QString>("dir");

  for (const Row &row : kRows)
    QTest::newRow(row.name) << QString(row.dir);
}

void TestRepositoryWatcher::existingDirectory() {
  QFETCH(QString, dir);

  QVERIFY(writeFile(QDir(mWorkdir.filePath(dir)).filePath("file")));
  QVERIFY2(
      mSpy->wait(kSignalMs),
      qPrintable(QString("no notification for a change in '%1'").arg(dir)));
  settle(*mSpy);
}

void TestRepositoryWatcher::newHiddenDirectory() {
  QVERIFY(mWorkdir.mkdir(".late"));
  QVERIFY2(mSpy->wait(kSignalMs), "no notification for the new directory");
  settle(*mSpy);

  QVERIFY(writeFile(mWorkdir.filePath(".late/file")));
  QVERIFY2(mSpy->wait(kSignalMs), "no notification for a change in '.late'");
}

void TestRepositoryWatcher::atomicReplace_data() {
  QTest::addColumn<QString>("from");

  QTest::newRow("temp outside repository") << mOutside.filePath("atomic.tmp");
  QTest::newRow("gitignored temp") << mWorkdir.filePath("atomic.tmp");
  QTest::newRow("plain rename") << mWorkdir.filePath("atomic.new");
}

void TestRepositoryWatcher::atomicReplace() {
  QFETCH(QString, from);

  QVERIFY(writeFile(from));
  settle(*mSpy);

  QVERIFY(replaceFile(from, mWorkdir.filePath("atomic")));
  QVERIFY2(mSpy->wait(kSignalMs),
           "no notification for a rename onto an existing file");
  settle(*mSpy);
}

void TestRepositoryWatcher::trackedFileMatchingIgnoreRule() {
  QVERIFY(writeFile(mWorkdir.filePath("tracked.ign")));
  QVERIFY2(mSpy->wait(kSignalMs),
           "no notification for a tracked file that matches an ignore rule");
  settle(*mSpy);
}

void TestRepositoryWatcher::untrackedFileMatchingIgnoreRule() {
#ifdef Q_OS_MAC
  QSKIP("FSEvents reports whole directories, so ignored files can't be told "
        "apart");
#endif

  QVERIFY(writeFile(mWorkdir.filePath("untracked.ign")));
  QVERIFY2(!mSpy->wait(kQuietMs),
           "notification for an ignored file that isn't tracked");
}

void TestRepositoryWatcher::gitDirectoryChange_data() {
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
  QSKIP("Changes to .git aren't picked up reliably on this platform yet");
#endif

  QTest::addColumn<QString>("kind");
  QTest::addColumn<bool>("relevant");

  // Later rows build on earlier ones: "HEAD" points at the "branch" ref.
  QTest::newRow("index") << "index" << true;
  QTest::newRow("branch") << "branch" << true;
  QTest::newRow("nested branch") << "nested branch" << true;
  QTest::newRow("HEAD") << "HEAD" << true;
  QTest::newRow("object") << "object" << false;
  QTest::newRow("commit message") << "commit message" << false;

  // The app stages through its own repository and updates the UI itself.
  QTest::newRow("own index") << "own index" << false;
  QTest::newRow("own then external index") << "own then external index" << true;
}

void TestRepositoryWatcher::gitDirectoryChange() {
  QFETCH(QString, kind);
  QFETCH(bool, relevant);

  QVERIFY(changeGitDir(mWorkdir.path(), (*mRepo).operator->(), kind));
  if (relevant) {
    QVERIFY2(mSpy->wait(kSignalMs),
             qPrintable(QString("no notification for '%1'").arg(kind)));
  } else {
    QVERIFY2(!mSpy->wait(kQuietMs),
             qPrintable(QString("notification for '%1'").arg(kind)));
  }

  settle(*mSpy);
}

TEST_MAIN(TestRepositoryWatcher)
#include "RepositoryWatcher.moc"

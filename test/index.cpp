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

#include "qtsupport.h"
#include "Test.h"
#include "git/Config.h"
#include "ui/DoubleTreeWidget.h"
#include "ui/MainWindow.h"
#include "ui/RepoView.h"
#include "ui/TreeView.h"
#include "ui/TreeProxy.h"
#include "conf/Settings.h"
#include <QFile>
#include <QTextEdit>
#include <QTextStream>

using namespace Test;
using namespace QTest;

static void disableListView(TreeView &treeView, RepoView &repoView) {
  auto treeProxy = dynamic_cast<TreeProxy *>(treeView.model());
  QVERIFY(treeProxy);

  auto diffTreeModel = dynamic_cast<DiffTreeModel *>(treeProxy->sourceModel());
  QVERIFY(diffTreeModel);

  diffTreeModel->enableListView(false);
  Settings::instance()->setValue(Setting::Id::ShowChangedFilesAsList, false);
  repoView.refresh();
}

class TestIndex : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void stageAddition();
  void stageDeletion();
  void stageDirectory();
  void stagedState();
  void stagedStateIgnoresFileModeWhenDisabled();
  void stagedStateFromStatus();
  void cleanupTestCase();

private:
  ScratchRepository mRepo;
  MainWindow *mWindow = nullptr;
};

void TestIndex::initTestCase() {
  mWindow = new MainWindow(mRepo);
  mWindow->show();
  QVERIFY(qWaitForWindowActive(mWindow));
}

void TestIndex::stageAddition() {
  // Add file and refresh.
  QFile file(mRepo->workdir().filePath("test"));
  QVERIFY(file.open(QFile::WriteOnly));
  QTextStream(&file) << "This is a test." << Qt::endl;

  RepoView *view = mWindow->currentView();
  refresh(view);

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);

  auto unstagedFiles = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(unstagedFiles);

  disableListView(*unstagedFiles, *view);

  auto stagedFiles = doubleTree->findChild<TreeView *>("Staged");
  QVERIFY(stagedFiles);

  QAbstractItemModel *unstagedModel = unstagedFiles->model();
  QCOMPARE(unstagedModel->rowCount(), 1);

  // Check that it starts unstaged.
  QModelIndex unstagedIndex = unstagedModel->index(0, 0);
  QVERIFY(!unstagedIndex.data(Qt::CheckStateRole).toBool());

  // Click on the check box.
  mouseClick(unstagedFiles->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(),
             unstagedFiles->checkRect(unstagedIndex).center());

  QAbstractItemModel *stagedModel = stagedFiles->model();
  QCOMPARE(stagedModel->rowCount(), 1);

  // Check that it's staged now.
  QModelIndex stagedIndex = stagedModel->index(0, 0);
  QVERIFY(stagedIndex.data(Qt::CheckStateRole).toBool());

  // Commit and refresh.
  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);

  editor->setText("addition");
  view->commit();
  refresh(view, false);
}

void TestIndex::stageDeletion() {
  // Remove file and refresh.
  mRepo->workdir().remove("test");

  RepoView *view = mWindow->currentView();
  refresh(view);

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);

  auto unstagedFiles = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(unstagedFiles);

  auto stagedFiles = doubleTree->findChild<TreeView *>("Staged");
  QVERIFY(stagedFiles);

  QAbstractItemModel *unstagedModel = unstagedFiles->model();
  QCOMPARE(unstagedModel->rowCount(), 1);

  // Check that it starts unstaged.
  QModelIndex unstagedIndex = unstagedModel->index(0, 0);
  QVERIFY(!unstagedIndex.data(Qt::CheckStateRole).toBool());

  // Click on the check box.
  mouseClick(unstagedFiles->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(),
             unstagedFiles->checkRect(unstagedIndex).center());

  QAbstractItemModel *stagedModel = stagedFiles->model();
  QCOMPARE(stagedModel->rowCount(), 1);

  // Check that it's staged now.
  QModelIndex stagedIndex = stagedModel->index(0, 0);
  QVERIFY(stagedIndex.data(Qt::CheckStateRole).toBool());

  // Commit and refresh.
  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);

  editor->setText("deletion");
  view->commit();
  refresh(view, false);
}

void TestIndex::stageDirectory() {
  QDir dir = mRepo->workdir();
  dir.mkdir("dir");
  QVERIFY(dir.cd("dir"));

  QFile file1(dir.filePath("test1"));
  QVERIFY(file1.open(QFile::WriteOnly));
  QTextStream(&file1) << "This is a test." << Qt::endl;

  QFile file2(dir.filePath("test2"));
  QVERIFY(file2.open(QFile::WriteOnly));
  QTextStream(&file2) << "This is a test." << Qt::endl;

  RepoView *view = mWindow->currentView();
  refresh(view);

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);

  auto unstagedFiles = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(unstagedFiles);

  disableListView(*unstagedFiles, *view);

  auto stagedFiles = doubleTree->findChild<TreeView *>("Staged");
  QVERIFY(stagedFiles);

  QAbstractItemModel *unstagedModel = unstagedFiles->model();
  QCOMPARE(unstagedModel->rowCount(), 1);

  // Check that it starts unstaged.
  QModelIndex unstagedIndex = unstagedModel->index(0, 0);
  QCOMPARE(unstagedModel->rowCount(unstagedIndex), 2);
  QVERIFY(!unstagedIndex.data(Qt::CheckStateRole).toBool());

  QVERIFY(!unstagedModel->index(0, 0, unstagedIndex)
               .data(Qt::CheckStateRole)
               .toBool());
  QVERIFY(!unstagedModel->index(1, 0, unstagedIndex)
               .data(Qt::CheckStateRole)
               .toBool());

  // Click on the check box.
  mouseClick(unstagedFiles->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(),
             unstagedFiles->checkRect(unstagedIndex).center());

  // Check for two staged files.
  QAbstractItemModel *stagedModel = stagedFiles->model();
  QCOMPARE(stagedModel->rowCount(), 1);

  QModelIndex stagedIndex = stagedModel->index(0, 0);
  QCOMPARE(stagedModel->rowCount(stagedIndex), 2);

  // Check that they're staged now.
  QVERIFY(
      stagedModel->index(0, 0, stagedIndex).data(Qt::CheckStateRole).toBool());
  QVERIFY(
      stagedModel->index(1, 0, stagedIndex).data(Qt::CheckStateRole).toBool());
}

static bool writeFile(const QDir &dir, const QString &name,
                      const QByteArray &data) {
  QFile file(dir.filePath(name));
  return file.open(QFile::WriteOnly) && file.write(data) == data.size();
}

void TestIndex::stagedState() {
  ScratchRepository repo;
  QDir workdir = repo->workdir();
  QVERIFY(writeFile(workdir, "a", "a\n"));
  QVERIFY(writeFile(workdir, "b", "b\n"));
  repo->index().setStaged({"a", "b"}, true);
  QVERIFY(repo->commit("Add files").isValid());

  QVERIFY(writeFile(workdir, "untracked", "new\n"));
  QCOMPARE(repo->index().isStaged("untracked"), git::Index::Unstaged);

  QVERIFY(writeFile(workdir, "a", "a changed\n"));
  QCOMPARE(repo->index().isStaged("a"), git::Index::Unstaged);

  repo->index().setStaged({"a"}, true);
  QCOMPARE(repo->index().isStaged("a"), git::Index::Staged);

  QVERIFY(writeFile(workdir, "a", "a changed again\n"));
  QCOMPARE(repo->index().isStaged("a"), git::Index::PartiallyStaged);

  QVERIFY(workdir.remove("b"));
  QCOMPARE(repo->index().isStaged("b"), git::Index::Unstaged);

  repo->index().setStaged({"b"}, true);
  QCOMPARE(repo->index().isStaged("b"), git::Index::Staged);
}

void TestIndex::stagedStateIgnoresFileModeWhenDisabled() {
#ifdef Q_OS_WIN
  QSKIP("Windows has no executable bit");
#endif
  ScratchRepository scratch;
  QDir workdir = scratch->workdir();
  QVERIFY(writeFile(workdir, "script", "echo\n"));
  scratch->index().setStaged({"script"}, true);
  QVERIFY(scratch->commit("Add script").isValid());
  scratch->gitConfig().setValue("core.filemode", false);

  // Reopen so the repository picks up the changed setting.
  git::Repository repo = git::Repository::open(workdir.path());
  QVERIFY(repo.isValid());

  QFile script(workdir.filePath("script"));
  QVERIFY(script.setPermissions(script.permissions() | QFile::ExeOwner));
  QVERIFY(writeFile(workdir, "script", "echo changed\n"));
  repo.index().setStaged({"script"}, true);
  QCOMPARE(repo.index().isStaged("script"), git::Index::Staged);

  git::Index index = repo.status(repo.index(), nullptr, false).index();
  QCOMPARE(index.isStaged("script"), git::Index::Staged);
}

void TestIndex::stagedStateFromStatus() {
  ScratchRepository repo;
  QDir workdir = repo->workdir();
  QVERIFY(writeFile(workdir, "a", "a\n"));
  QVERIFY(writeFile(workdir, "b", "b\n"));
  QVERIFY(writeFile(workdir, "c", "c\n"));
  repo->index().setStaged({"a", "b", "c"}, true);
  QVERIFY(repo->commit("Add files").isValid());

  QVERIFY(writeFile(workdir, "untracked", "new\n"));
  QVERIFY(writeFile(workdir, "a", "a changed\n"));
  QVERIFY(writeFile(workdir, "b", "b changed\n"));
  repo->index().setStaged({"b"}, true);
  QVERIFY(writeFile(workdir, "c", "c changed\n"));
  repo->index().setStaged({"c"}, true);
  QVERIFY(writeFile(workdir, "c", "c changed again\n"));

  git::Index index = repo->status(repo->index(), nullptr, false).index();
  QCOMPARE(index.isStaged("untracked"), git::Index::Unstaged);
  QCOMPARE(index.isStaged("a"), git::Index::Unstaged);
  QCOMPARE(index.isStaged("b"), git::Index::Staged);
  QCOMPARE(index.isStaged("c"), git::Index::PartiallyStaged);

  // The states are a snapshot of the status, not recomputed per query.
  repo->index().setStaged({"c"}, true);
  QCOMPARE(index.isStaged("c"), git::Index::PartiallyStaged);
  QCOMPARE(repo->index().isStaged("c"), git::Index::Staged);
}

void TestIndex::cleanupTestCase() { mWindow->close(); }

TEST_MAIN(TestIndex)

#include "index.moc"

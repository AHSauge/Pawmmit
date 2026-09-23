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

#include "qtsupport.h"
#include "Test.h"
#include "ui/MainWindow.h"
#include "ui/DetailView.h"
#include "ui/DiffView/DiffView.h"
#include "ui/DoubleTreeWidget.h"
#include "ui/RepoView.h"
#include "ui/StateBanner.h"
#include "ui/TreeView.h"
#include "ui/CommitList.h"
#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalSpy>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>

using namespace Test;
using namespace QTest;

class TestMerge : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void firstCommit();
  void secondCommit();
  void thirdCommit();
  void mergeConflict();
  void resolve();
  void cleanupTestCase();

private:
  int inputDelay = 0;
  int closeDelay = 0;

  ScratchRepository mRepo;
  MainWindow *mWindow = nullptr;
  QString mMainBranch;
};

void TestMerge::initTestCase() {
  mMainBranch = mRepo->unbornHeadName();
  mWindow = new MainWindow(mRepo);
  mWindow->show();
  QVERIFY(qWaitForWindowExposed(mWindow));
}

void TestMerge::firstCommit() {
  // Add file and refresh.
  QFile file(mRepo->workdir().filePath("test"));
  QVERIFY(file.open(QFile::WriteOnly));
  QTextStream(&file) << "This will be a test." << Qt::endl;

  RepoView *view = mWindow->currentView();
  refresh(view);

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);

  auto files = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(files);

  QAbstractItemModel *model = files->model();
  QCOMPARE(model->rowCount(), 1);

  // Click on the check box.
  QModelIndex index = model->index(0, 0);
  mouseClick(files->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(),
             files->checkRect(index).center());

  // Commit and refresh.
  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);

  editor->setText("base commit");
  view->commit();
  refresh(view, false);
}

void TestMerge::secondCommit() {
  RepoView *view = mWindow->currentView();
  git::Branch branch = mRepo->createBranch("branch2", mRepo->head().target());
  QVERIFY(branch.isValid());

  view->checkout(branch);
  QCOMPARE(mRepo->head().name(), QString("branch2"));

  QFile file(mRepo->workdir().filePath("test"));
  QVERIFY(file.open(QFile::WriteOnly));
  QTextStream(&file) << "This is a conflict." << Qt::endl;

  refresh(view);

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);

  auto files = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(files);

  QAbstractItemModel *model = files->model();
  QCOMPARE(model->rowCount(), 1);

  // Click on the check box.
  QModelIndex index = model->index(0, 0);
  mouseClick(files->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(),
             files->checkRect(index).center());

  // Commit and refresh.
  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);

  editor->setText("conflicting commit b");
  view->commit();
  refresh(view, false);
}

void TestMerge::thirdCommit() {
  RepoView *view = mWindow->currentView();
  git::Reference ref =
      mRepo->lookupRef(QString("refs/heads/%1").arg(mMainBranch));
  QVERIFY(ref);

  view->checkout(ref);
  QCOMPARE(mRepo->head().name(), mMainBranch);

  QFile file(mRepo->workdir().filePath("test"));
  QVERIFY(file.open(QFile::WriteOnly));
  QTextStream(&file) << "This is a test." << Qt::endl;

  refresh(view);

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);

  auto files = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(files);

  QAbstractItemModel *model = files->model();
  QCOMPARE(model->rowCount(), 1);

  // Click on the check box.
  QModelIndex index = model->index(0, 0);
  mouseClick(files->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(),
             files->checkRect(index).center());

  // Commit and refresh.
  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);

  editor->setText("conflicting commit a");
  view->commit();
  refresh(view, false);
}

void TestMerge::mergeConflict() {
  RepoView *view = mWindow->currentView();
  git::Reference master =
      mRepo->lookupRef(QString("refs/heads/%1").arg(mMainBranch));
  QVERIFY(master);

  git::Reference branch2 = mRepo->lookupRef("refs/heads/branch2");
  QVERIFY(branch2);

  QCOMPARE(mRepo->head().name(), mMainBranch);

  view->merge(RepoView::Merge, branch2);

  // Diff is in a conflicted state
  git::Diff diff = mRepo->diffIndexToWorkdir();
  QVERIFY(diff.isConflicted());

  // Wait for the commit editor to report the conflict.
  DetailView *detailView = view->findChild<DetailView *>();
  QVERIFY(detailView);
  auto hasConflictStatus = [detailView] {
    for (QLabel *label : detailView->findChildren<QLabel *>()) {
      if (label->text().contains("unresolved conflict"))
        return true;
    }
    return false;
  };
  QTRY_VERIFY_WITH_TIMEOUT(hasConflictStatus(), 10000);

  // Commit is not available while conflicts remain.
  QPushButton *commit = nullptr;
  for (QPushButton *button : detailView->findChildren<QPushButton *>()) {
    if (button->text() == "Commit Merge")
      commit = button;
  }
  QVERIFY(commit);

  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);
  editor->clear();
  editor->setText("merge commit");
  QVERIFY(!commit->isEnabled());
  QVERIFY(commit->toolTip().contains("Resolve the remaining conflicts"));

  // The banner says so too, wherever the user is looking.
  StateBanner *banner = view->findChild<StateBanner *>();
  QVERIFY(banner);
  QTRY_VERIFY(banner->isVisible());
  QCOMPARE(banner->message(),
           QString("Merging branch2 into %1. 1 file has conflicts. Keep one "
                   "version or edit it, then stage it to mark it resolved. "
                   "Finally, click Commit Merge to finish the merge.")
               .arg(mMainBranch));

  // Its main action leads to the conflict, even from another commit.
  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);
  auto files = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(files);
  CommitList *commitList = view->findChild<CommitList *>();
  QVERIFY(commitList);
  commitList->setCurrentIndex(commitList->model()->index(1, 0));
  QTRY_VERIFY(!editor->isVisible());

  QPushButton *show = nullptr;
  for (QPushButton *button : banner->findChildren<QPushButton *>()) {
    if (button->isVisibleTo(banner) && button->text() == "Show Conflicts")
      show = button;
  }
  QVERIFY(show);
  show->click();
  QTRY_VERIFY(editor->isVisible());
  QTRY_COMPARE(files->currentIndex().data(Qt::DisplayRole).toString(),
               QString("test"));

  // It still works when the uncommitted changes are already shown.
  QSignalSpy dispatched(commitList, &CommitList::diffSelected);
  show->click();
  QCOMPARE(dispatched.count(), 1);
  QCOMPARE(dispatched.first().at(1).toString(), QString("test"));

  // Staging a file that still has conflict markers asks first. Cancel before
  // checking, as a failed check would leave the modal dialog open.
  QString prompt;
  QTimer::singleShot(0, [&prompt] {
    auto *box = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
    if (!box)
      return;
    prompt = box->text();
    box->button(QMessageBox::Cancel)->click();
  });
  mRepo->index().setStaged({"test"}, true);
  QCOMPARE(prompt, QString("'test' still contains conflict markers (<<<<<<<, "
                           "=======, >>>>>>>)."));
  QVERIFY(mRepo->index().hasConflicts());

  // Aborting asks first, and cancelling leaves the merge alone.
  view->promptToAbort();
  QMessageBox *confirm = nullptr;
  QTRY_VERIFY((confirm = view->findChild<QMessageBox *>()));
  QString question = confirm->text();
  confirm->button(QMessageBox::Cancel)->click();
  QCOMPARE(question, QString("Are you sure you want to abort the merge?"));
  QCOMPARE(mRepo->state(), GIT_REPOSITORY_STATE_MERGE);
}

void TestMerge::resolve() {
  RepoView *view = mWindow->currentView();
  DiffView *diffView = view->findChild<DiffView *>();

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);

  auto files = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(files);

  // Wait for refresh
  QAbstractItemModel *model = files->model();
  QTRY_VERIFY_WITH_TIMEOUT(model->rowCount() >= 1, 10000);

  files->selectionModel()->select(files->model()->index(0, 0),
                                  QItemSelectionModel::Select);

  // The diff loads asynchronously, so the buttons don't exist right away.
  QToolButton *theirs = nullptr;
  QTRY_VERIFY_WITH_TIMEOUT(
      (theirs = diffView->findChild<QToolButton *>("ConflictTheirs")), 10000);

  QToolButton *ours =
      diffView->widget()->findChild<QToolButton *>("ConflictOurs");
  QVERIFY(ours);

  // Named by branch, not by the ambiguous "ours"/"theirs" pronouns. "Ours"
  // isn't inverted during a plain merge, unlike during a rebase: it's still
  // the branch merged into (master), not the branch merged in.
  QCOMPARE(ours->text(), QString("Keep %1").arg(mMainBranch));
  QCOMPARE(theirs->text(), QString("Take branch2"));

  mouseClick(theirs, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(),
             inputDelay);

  QToolButton *undo =
      diffView->widget()->findChild<QToolButton *>("ConflictUndo");
  QVERIFY(undo);
  mouseClick(undo, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(),
             inputDelay);

  mouseClick(ours, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(),
             inputDelay);

  QToolButton *save =
      diffView->widget()->findChild<QToolButton *>("ConflictSave");
  QVERIFY(save);
  mouseClick(save, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(),
             inputDelay);

  DetailView *detailView = view->findChild<DetailView *>();
  QPushButton *stageAll = nullptr;
  QTRY_VERIFY_WITH_TIMEOUT(
      (stageAll = detailView->findChild<QPushButton *>("StageAll")), 10000);
  mouseClick(stageAll, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(),
             inputDelay);

  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);

  // With the conflicts gone, the banner points to the commit message.
  StateBanner *banner = view->findChild<StateBanner *>();
  QTRY_COMPARE(banner->message(),
               QString("Merging branch2 into %1. No conflicts left. Check the "
                       "changes, then click Commit Merge to finish the merge.")
                   .arg(mMainBranch));

  // Buttons added to a visible banner are shown on the next event loop turn.
  QPushButton *show = nullptr;
  auto findShow = [&] {
    for (QPushButton *button : banner->findChildren<QPushButton *>()) {
      if (button->isVisibleTo(banner) && button->text() == "Show Changes")
        show = button;
    }
    return show != nullptr;
  };
  QTRY_VERIFY(findShow());

  // The offscreen window is never active, so check the window's focus widget.
  files->setFocus();
  QCOMPARE(editor->window()->focusWidget(), files);
  show->click();
  QTRY_COMPARE(editor->window()->focusWidget(), editor);

  // Commit and refresh.

  editor->setText("conflicts resolved");
  view->commit();
  refresh(view, false);

  // Diff is not in a conflicted state
  git::Diff diff = mRepo->diffIndexToWorkdir();
  QVERIFY(!diff.isConflicted());

  QTRY_VERIFY(!view->findChild<StateBanner *>()->isVisible());
}

void TestMerge::cleanupTestCase() {
  qWait(closeDelay);
  mWindow->close();
}

TEST_MAIN(TestMerge)

#include "merge.moc"

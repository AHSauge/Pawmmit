#include "Test.h"
#include "ui/MainWindow.h"
#include "ui/DoubleTreeWidget.h"
#include "ui/MenuBar.h"
#include "ui/RepoView.h"
#include "ui/SearchField.h"
#include "ui/ToolBar.h"
#include "ui/TreeView.h"
#include <QAbstractButton>
#include <QFile>
#include <QTextEdit>
#include <QSettings>
#include <QTemporaryDir>

using namespace Test;
using namespace QTest;

class TestButtonTips : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void everyControlHasAName();
  void tipsShowTheDefaultHotkey();
  void tipsFollowARebinding();
  void logTipFollowsVisibility();
  void everyIconOnlyButtonHasAName();
  void commitEditorTipsShowHotkeys();
  void commitTipExplainsWhatIsMissing();
  void cleanupTestCase();

private:
  QAbstractButton *button(const QString &name) const {
    for (QAbstractButton *button :
         mWindow->toolBar()->findChildren<QAbstractButton *>()) {
      if (button->accessibleName() == name)
        return button;
    }

    return nullptr;
  }

  // Any button in the window, wherever it lives.
  QAbstractButton *anyButton(const QString &text) const {
    for (QAbstractButton *button : mWindow->findChildren<QAbstractButton *>()) {
      if (button->text() == text || button->accessibleName() == text)
        return button;
    }

    return nullptr;
  }

  static QString keys(const QString &portable) {
    return QKeySequence(portable).toString(QKeySequence::NativeText);
  }

  // Keep the test away from the user's real settings.
  QTemporaryDir mDir;
  ScratchRepository mRepo;
  MainWindow *mWindow = nullptr;
};

void TestButtonTips::initTestCase() {
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, mDir.path());

  mWindow = new MainWindow(mRepo);
  mWindow->show();
  QVERIFY(qWaitForWindowExposed(mWindow));
}

void TestButtonTips::everyControlHasAName() {
  int count = 0;
  for (QAbstractButton *button :
       mWindow->toolBar()->findChildren<QAbstractButton *>()) {
    // Qt's own overflow button isn't ours to name.
    if (button->objectName().startsWith("qt_"))
      continue;

    ++count;
    QVERIFY2(!button->accessibleName().isEmpty(),
             qPrintable(QString("%1 '%2' has no accessible name")
                            .arg(button->metaObject()->className(),
                                 button->toolTip())));
  }

  QVERIFY(count > 10);
  QVERIFY(!mWindow->toolBar()->searchField()->accessibleName().isEmpty());
}

void TestButtonTips::tipsShowTheDefaultHotkey() {
  QAbstractButton *fetch = button("Fetch");
  QVERIFY(fetch);
  QCOMPARE(fetch->toolTip(),
           QString("Fetch (%1)").arg(keys("Ctrl+Shift+Alt+F")));

  // Nothing is bound to the terminal by default.
  QAbstractButton *terminal = button("Open Terminal");
  QVERIFY(terminal);
  QCOMPARE(terminal->toolTip(), QString("Open Terminal"));
}

void TestButtonTips::tipsFollowARebinding() {
  QAbstractButton *fetch = button("Fetch");
  QVERIFY(fetch);

  Hotkeys::fetch.setKeys(QKeySequence("Ctrl+Alt+Y"));
  QCOMPARE(fetch->toolTip(), QString("Fetch (%1)").arg(keys("Ctrl+Alt+Y")));

  Hotkeys::fetch.setKeys(QKeySequence());
  QCOMPARE(fetch->toolTip(), QString("Fetch"));

  // The name is what a screen reader announces, so it never carries a hotkey.
  QCOMPARE(fetch->accessibleName(), QString("Fetch"));
}

void TestButtonTips::logTipFollowsVisibility() {
  RepoView *view = mWindow->currentView();
  QVERIFY(view);

  view->setLogVisible(false);
  QAbstractButton *show = button("Show Log");
  QVERIFY(show);

  view->setLogVisible(true);
  QAbstractButton *hide = button("Hide Log");
  QVERIFY(hide);
  QVERIFY(hide->toolTip().startsWith("Hide Log"));
}

void TestButtonTips::everyIconOnlyButtonHasAName() {
  int checked = 0;
  for (QAbstractButton *button : mWindow->findChildren<QAbstractButton *>()) {
    // Qt's own line edit icons and tab scroll arrows aren't ours to name.
    QString type = button->metaObject()->className();
    if (type == "QLineEditIconButton" || button->objectName().startsWith("qt_"))
      continue;

    if (!button->text().isEmpty())
      continue;

    ++checked;
    QVERIFY2(
        !button->accessibleName().isEmpty(),
        qPrintable(
            QString("%1 in %2 has no accessible name")
                .arg(type,
                     button->parentWidget()
                         ? button->parentWidget()->metaObject()->className()
                         : "?")));
  }

  QVERIFY(checked > 15);
}

void TestButtonTips::commitEditorTipsShowHotkeys() {
  QAbstractButton *stage = anyButton("Stage All");
  QVERIFY(stage);
  QCOMPARE(stage->toolTip(), QString("Stage All (%1)").arg(keys("Ctrl++")));

  QAbstractButton *unstage = anyButton("Unstage All");
  QVERIFY(unstage);
  QCOMPARE(unstage->toolTip(), QString("Unstage All (%1)").arg(keys("Ctrl+-")));

  QAbstractButton *commit = anyButton("Commit");
  QVERIFY(commit);
  QVERIFY(commit->toolTip().startsWith(
      QString("Commit (%1)").arg(keys("Ctrl+Shift+C"))));

  QAbstractButton *copy = anyButton("Copy Commit ID");
  QVERIFY(copy);
}

void TestButtonTips::commitTipExplainsWhatIsMissing() {
  RepoView *view = mWindow->currentView();
  QVERIFY(view);

  QFile file(mRepo->workdir().filePath("test"));
  QVERIFY(file.open(QFile::WriteOnly));
  file.write("This will be a test.\n");
  file.close();
  refresh(view);

  auto doubleTree = view->findChild<DoubleTreeWidget *>();
  QVERIFY(doubleTree);
  auto files = doubleTree->findChild<TreeView *>("Unstaged");
  QVERIFY(files);
  QTRY_COMPARE_WITH_TIMEOUT(files->model()->rowCount(), 1, 10000);

  QAbstractButton *commit = anyButton("Commit");
  QVERIFY(commit);
  QString base = QString("Commit (%1)").arg(keys("Ctrl+Shift+C"));

  QTRY_COMPARE(commit->toolTip(), base + "\nStage the files you want to commit"
                                         "\nEnter a commit message");
  QVERIFY(!commit->isEnabled());

  // Staging a file suggests a message, so there is nothing left to say.
  QModelIndex index = files->model()->index(0, 0);
  mouseClick(files->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(),
             files->checkRect(index).center());
  QTRY_COMPARE(commit->toolTip(), base);
  QVERIFY(commit->isEnabled());

  QTextEdit *editor = view->findChild<QTextEdit *>("MessageEditor");
  QVERIFY(editor);
  editor->clear();
  QTRY_COMPARE(commit->toolTip(), base + "\nEnter a commit message");
  QVERIFY(!commit->isEnabled());

  editor->setText("base commit");
  QTRY_COMPARE(commit->toolTip(), base);
  QVERIFY(commit->isEnabled());
}

void TestButtonTips::cleanupTestCase() { mWindow->close(); }

TEST_MAIN(TestButtonTips)

#include "button_tips.moc"

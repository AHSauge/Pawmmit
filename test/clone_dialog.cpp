#include "Test.h"
#include "dialogs/CloneDialog.h"
#include <QLineEdit>
#include <QMainWindow>
#include <QWizardPage>

using namespace Test;
using namespace QTest;

class TestCloneDialog : public QObject {
  Q_OBJECT

private slots:
  void typingNameKeepsGeometry();
};

void TestCloneDialog::typingNameKeepsGeometry() {
  QMainWindow window;
  window.show();
  QVERIFY(qWaitForWindowExposed(&window));

  CloneDialog *dialog = new CloneDialog(CloneDialog::Init, &window);
  dialog->show();
  QVERIFY(qWaitForWindowExposed(dialog));

  QLineEdit *path = dialog->findChild<QLineEdit *>("mPath");
  QLineEdit *name = dialog->findChild<QLineEdit *>("mName");
  QVERIFY(path && name);

  QSize dialogSize = dialog->size();
  QSize pageSize = dialog->currentPage()->size();
  QPoint namePos = name->mapTo(dialog, QPoint());

  // A long path makes the location preview as wide as it can get.
  path->setText(QString("/a/rather/long/directory/name").repeated(4));
  keyClicks(name, "repo");
  QCoreApplication::processEvents();

  QCOMPARE(dialog->size(), dialogSize);
  QCOMPARE(dialog->currentPage()->size(), pageSize);
  QCOMPARE(name->mapTo(dialog, QPoint()), namePos);
  delete dialog;
}

TEST_MAIN(TestCloneDialog)

#include "clone_dialog.moc"

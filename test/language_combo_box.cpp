#include "Test.h"
#include "conf/Settings.h"
#include "dialogs/LanguageComboBox.h"
#include <QComboBox>
#include <QSettings>
#include <QTemporaryDir>

using namespace Test;
using namespace QTest;

class TestLanguageComboBox : public QObject {
  Q_OBJECT

private slots:
  void initTestCase();
  void listsNativeNamesWithSystemFirst();
  void selectingALanguageStoresIt();
  void startsOnTheStoredLanguage();

private:
  // Keep the test away from the user's real settings.
  QTemporaryDir mDir;
};

void TestLanguageComboBox::initTestCase() {
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, mDir.path());
}

void TestLanguageComboBox::listsNativeNamesWithSystemFirst() {
  QComboBox box;
  setupLanguageComboBox(&box);

  QVERIFY(box.count() > 1);
  // Nothing is stored yet, so the system default is what's selected.
  QCOMPARE(box.currentIndex(), 0);

  QStringList codes;
  QStringList names;
  for (int i = 1; i < box.count(); ++i) {
    codes.append(box.itemData(i).toString());
    names.append(box.itemText(i));

    // A raw code such as "pt_BR" is not a name.
    QVERIFY2(box.itemText(i) != box.itemData(i).toString(),
             qPrintable(box.itemData(i).toString()));
  }

  QStringList sorted = codes;
  sorted.sort();
  QCOMPARE(codes, sorted);
  QCOMPARE(names.removeDuplicates(), 0);
  QCOMPARE(box.itemText(box.findData("en")), QString("English"));

  // Taken from Qt, which spells Russian in lower case.
  QCOMPARE(box.itemText(box.findData("de")), QString("Deutsch"));
  QCOMPARE(box.itemText(box.findData("ru")),
           QString("\u0420\u0443\u0441\u0441\u043a\u0438\u0439"));
}

void TestLanguageComboBox::selectingALanguageStoresIt() {
  QComboBox box;
  setupLanguageComboBox(&box);

  int index = box.findData("de");
  QVERIFY(index >= 0);
  box.setCurrentIndex(index);

  QCOMPARE(Settings::instance()->value(Setting::Id::Language).toString(),
           QString("de"));
}

void TestLanguageComboBox::startsOnTheStoredLanguage() {
  Settings::instance()->setValue(Setting::Id::Language, "sv");

  QComboBox box;
  setupLanguageComboBox(&box);

  QCOMPARE(box.currentData().toString(), QString("sv"));
}

TEST_MAIN(TestLanguageComboBox)

#include "language_combo_box.moc"

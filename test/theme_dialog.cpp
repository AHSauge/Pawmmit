#include "Test.h"
#include "dialogs/ThemeDialog.h"
#include <QComboBox>
#include <QLabel>
#include <QTranslator>

using namespace Test;
using namespace QTest;

namespace {

class FakeTranslator : public QTranslator {
public:
  QString translate(const char *context, const char *sourceText,
                    const char *disambiguation = nullptr,
                    int n = -1) const override {
    QByteArray source(sourceText);
    if (source == "Dark Theme")
      return "Dunkles Thema";
    if (source == "System default")
      return "Systemstandard";
    return QString();
  }

  bool isEmpty() const override { return false; }
};

} // namespace

class TestThemeDialog : public QObject {
  Q_OBJECT

private slots:
  void retranslatesWhenTheLanguageChanges();
};

void TestThemeDialog::retranslatesWhenTheLanguageChanges() {
  ThemeDialog dialog;
  QComboBox *language = dialog.findChild<QComboBox *>();
  QVERIFY(language);

  // Qt doesn't broadcast this to widgets itself; Application does.
  auto changeLanguage = [&dialog] {
    QEvent event(QEvent::LanguageChange);
    QCoreApplication::sendEvent(&dialog, &event);
  };

  auto titles = [&dialog] {
    QStringList titles;
    for (QLabel *label : dialog.findChildren<QLabel *>("title"))
      titles.append(label->text());
    return titles;
  };

  QVERIFY(titles().contains("Dark Theme"));
  QCOMPARE(language->itemText(0), QString("System default"));

  FakeTranslator translator;
  QCoreApplication::installTranslator(&translator);
  changeLanguage();
  QVERIFY(titles().contains("Dunkles Thema"));
  QCOMPARE(language->itemText(0), QString("Systemstandard"));

  QCoreApplication::removeTranslator(&translator);
  changeLanguage();
  QVERIFY(titles().contains("Dark Theme"));
  QCOMPARE(language->itemText(0), QString("System default"));
}

TEST_MAIN(TestThemeDialog)

#include "theme_dialog.moc"

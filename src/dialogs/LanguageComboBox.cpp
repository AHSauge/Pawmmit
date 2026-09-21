//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "LanguageComboBox.h"
#include "conf/Settings.h"
#include "languages.h"
#include <QComboBox>
#include <QCoreApplication>
#include <QEvent>
#include <QHash>
#include <QLocale>
#include <QStringList>

namespace {

// Qt's native names are used except where they mislead, e.g. "pt" resolves to
// Brazil there while our Portuguese translation is for Portugal.
QString nativeName(const QString &code) {
  static const QHash<QString, QString> overrides = {
      {"en", QStringLiteral("English")},
      {"es", QStringLiteral("Espa\u00f1ol")},
      {"pt", QStringLiteral("Portugu\u00eas")},
      {"pt_BR", QStringLiteral("Portugu\u00eas (Brasil)")},
  };

  QString name = overrides.value(code);
  if (name.isEmpty()) {
    name = QLocale(code).nativeLanguageName();
    if (!name.isEmpty())
      name[0] = name.at(0).toUpper();
  }

  return name.isEmpty() ? code : name;
}

QString systemDefaultName() {
  return QCoreApplication::translate("LanguageComboBox", "System default");
}

// Retranslates the "System default" entry when the language changes.
class RetranslateFilter : public QObject {
public:
  using QObject::QObject;

  bool eventFilter(QObject *watched, QEvent *event) override {
    if (event->type() == QEvent::LanguageChange)
      static_cast<QComboBox *>(watched)->setItemText(0, systemDefaultName());
    return QObject::eventFilter(watched, event);
  }
};

} // namespace

void setupLanguageComboBox(QComboBox *comboBox) {
  QStringList codes;
  for (const char *value : Languages::languages) {
    QString code = QString::fromLatin1(value);
    if (code != Languages::system)
      codes.append(code);
  }
  codes.sort();

  comboBox->addItem(systemDefaultName(), Languages::system);
  comboBox->installEventFilter(new RetranslateFilter(comboBox));
  for (const QString &code : codes)
    comboBox->addItem(nativeName(code), code);

  QString current =
      Settings::instance()->value(Setting::Id::Language).toString();
  int index = comboBox->findData(current);
  if (index >= 0)
    comboBox->setCurrentIndex(index);

  QObject::connect(
      comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), comboBox,
      [comboBox] {
        Settings::instance()->setValue(Setting::Id::Language,
                                       comboBox->currentData().toString());
      });
}

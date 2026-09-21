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

#include "ThemeDialog.h"
#include "app/Application.h"
#include "conf/Settings.h"
#include "LanguageComboBox.h"
#include <QComboBox>
#include <QCoreApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtMath>

namespace {

class ThemeButton : public QPushButton {
  Q_OBJECT

public:
  enum class Theme { Default, Dark, System };

  ThemeButton(const char *title, const QIcon &icon, const char *description,
              const Theme &theme, QWidget *parent = nullptr)
      : QPushButton(parent), mTitleKey(title), mDescriptionKey(description),
        mTheme(theme) {
    setStyleSheet("ThemeButton #description {"
                  "  font-size: 12px;"
                  "  font-style: italics;"
                  "  margin: 0px 0px 16px 0px"
                  "}"

                  "ThemeButton #title {"
                  "  font-weight: bold;"
                  "  font-size: 16px;"
                  "  margin: 13px 0px 20px 8px"
                  "}");

    QSize iconSize = QSize(245, 196);
    setFixedHeight(iconSize.height() + 80);
    setFixedWidth(iconSize.width() + 35);
    setFocusPolicy(Qt::StrongFocus);

    mTitle = new QLabel(this);
    mTitle->setObjectName("title");

    setIcon(icon);
    setIconSize(iconSize);

    mDescription = new QLabel(this);
    setTexts();
    mDescription->setAlignment(Qt::AlignHCenter);
    mDescription->setWordWrap(true);
    mDescription->setObjectName("description");

    connect(this, &QPushButton::clicked, [this] {
      switch (mTheme) {
        case Theme::System:
          Settings::instance()->setValue(Setting::Id::ColorTheme, "System");
          break;
        case Theme::Dark:
          Settings::instance()->setValue(Setting::Id::ColorTheme, "Dark");
          break;
        case Theme::Default:
          Settings::instance()->setValue(Setting::Id::ColorTheme, "Default");
          break;
      }

      window()->close();
    });
  }

  void resizeEvent(QResizeEvent *event) override { placeLabels(); }

  void changeEvent(QEvent *event) override {
    if (event->type() == QEvent::LanguageChange) {
      setTexts();
      mTitle->adjustSize();
      placeLabels();
    }

    QPushButton::changeEvent(event);
  }

private:
  void setTexts() {
    mTitle->setText(QCoreApplication::translate("ThemeDialog", mTitleKey));
    mDescription->setText(
        QCoreApplication::translate("ThemeDialog", mDescriptionKey));
  }

  void placeLabels() {
    // Wrap long descriptions instead of cutting them off.
    int margin = 2;
    int width = this->width() - 2 * margin;
    int height = mDescription->heightForWidth(width);
    mDescription->resize(width, height);
    mDescription->move(margin, this->height() - height - 5);
    mTitle->move(rect().left() + 5, rect().top());
  }

  QLabel *mTitle;
  QLabel *mDescription;
  const char *mTitleKey;
  const char *mDescriptionKey;
  Theme mTheme;
};

// A globe, which reads as "language" whatever language the labels are in.
class GlobeIcon : public QWidget {
public:
  GlobeIcon(QWidget *parent = nullptr) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  }

  QSize sizeHint() const override {
    int size = fontMetrics().height() + 4;
    return QSize(size, size);
  }

  void paintEvent(QPaintEvent *event) override {
    qreal size = qMin(width(), height());
    qreal radius = (size - 2) / 2.0;
    QPointF center = rect().center() + QPointF(0.5, 0.5);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(palette().color(QPalette::WindowText),
                        qMax<qreal>(1.0, size / 14.0)));

    painter.drawEllipse(center, radius, radius);
    painter.drawEllipse(center, radius * 0.45, radius);
    painter.drawLine(QPointF(center.x(), center.y() - radius),
                     QPointF(center.x(), center.y() + radius));

    for (qreal offset : {-0.5, 0.0, 0.5}) {
      qreal y = center.y() + offset * radius;
      qreal half = radius * qSqrt(1.0 - offset * offset);
      painter.drawLine(QPointF(center.x() - half, y),
                       QPointF(center.x() + half, y));
    }
  }
};

} // namespace

ThemeDialog::ThemeDialog(QWidget *parent) : QDialog(parent) {
  ThemeButton *native = new ThemeButton(
      QT_TRANSLATE_NOOP("ThemeDialog", "Default Theme"), QIcon(":/native.png"),
      QT_TRANSLATE_NOOP("ThemeDialog", "A consistent bright theme"),
      ThemeButton::Theme::Default);

  ThemeButton *dark = new ThemeButton(
      QT_TRANSLATE_NOOP("ThemeDialog", "Dark Theme"), QIcon(":/dark.png"),
      QT_TRANSLATE_NOOP("ThemeDialog",
                        "A consistent look optimal for reducing eye strain"),
      ThemeButton::Theme::Dark);

  ThemeButton *system = new ThemeButton(
      QT_TRANSLATE_NOOP("ThemeDialog", "System Theme"), QIcon(":/system.png"),
      QT_TRANSLATE_NOOP("ThemeDialog",
                        "A flexible look matching system colors"),
      ThemeButton::Theme::System);

  QHBoxLayout *themeButtons = new QHBoxLayout;
  themeButtons->addWidget(native);
  themeButtons->addWidget(dark);
  themeButtons->addSpacing(20);
  themeButtons->addWidget(system);

  mGlobe = new GlobeIcon(this);
  mLanguage = new QComboBox(this);
  setupLanguageComboBox(mLanguage);

  // The dialog shows its own language, so apply the choice right away.
  connect(mLanguage, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this] {
            Application::setLanguage(mLanguage->currentData().toString());
          });

  QHBoxLayout *languageRow = new QHBoxLayout;
  languageRow->addStretch();
  languageRow->addWidget(mGlobe);
  languageRow->addWidget(mLanguage);
  languageRow->addStretch();

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addLayout(themeButtons);
  layout->addSpacing(12);
  layout->addLayout(languageRow);

  retranslate();
}

void ThemeDialog::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    retranslate();

  QDialog::changeEvent(event);
}

void ThemeDialog::retranslate() {
  setWindowTitle(tr("Pick a theme and language for Pawmmit"));

  QString language = tr("Language");
  mGlobe->setToolTip(language);
  mGlobe->setAccessibleName(language);
  mLanguage->setToolTip(language);
  mLanguage->setAccessibleName(language);
}

#include "ThemeDialog.moc"

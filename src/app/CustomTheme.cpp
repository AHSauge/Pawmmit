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

#include "CustomTheme.h"
#include "conf/ConfFile.h"
#include "conf/Settings.h"
#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QProxyStyle>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStyleOptionButton>
#include <stdexcept>

#if defined(Q_OS_WIN)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <QWindow>
#include <windows.h>
#include <dwmapi.h>
#endif

namespace {

// Palette roles standing in for style sheet tokens a theme doesn't define.
const QMap<QString, QString> kTokenFallbacks = {
    {"chrome", "palette(window)"},
    {"sidebar", "palette(window)"},
    {"content", "palette(base)"},
    {"subtle", "palette(alternate-base)"},
    {"hunk_header", "palette(alternate-base)"},
    {"hover", "palette(dark)"},
    {"divider", "palette(dark)"},
    {"control_border", "palette(shadow)"},
    {"text", "palette(text)"},
    {"text_secondary", "palette(bright-text)"},
    {"text_muted", "palette(mid)"},
    {"accent", "palette(highlight)"},
    {"accent_subtle", "palette(highlight)"},
    {"accent_text", "palette(link)"},
    {"on_accent", "palette(highlighted-text)"}};

void setPaletteColors(QPalette &palette, QPalette::ColorRole role,
                      const QVariant &variant) {
  if (variant.canConvert<QColor>()) {
    palette.setColor(role, variant.value<QColor>());
    return;
  }

  QVariantMap map = variant.toMap();
  for (const QString &key : map.keys()) {
    QColor color(map.value(key).toString());
    if (!color.isValid()) {
      Q_ASSERT(false);
      continue;
    }

    if (key == "default") {
      palette.setColor(role, color);
    } else if (key == "active") {
      palette.setColor(QPalette::Active, role, color);
    } else if (key == "inactive") {
      palette.setColor(QPalette::Inactive, role, color);
    } else if (key == "disabled") {
      palette.setColor(QPalette::Disabled, role, color);
    }
  }
}

class CustomStyle : public QProxyStyle {
public:
  CustomStyle(const CustomTheme *theme)
      : QProxyStyle("fusion"), mTheme(theme) {}

  void drawPrimitive(PrimitiveElement elem, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget) const override {
    switch (elem) {
      case PE_IndicatorCheckBox: {
        QVariantMap checkbox = mTheme->checkbox();
        QColor fill = checkbox.value("fill").value<QColor>();
        QColor outline = checkbox.value("outline").value<QColor>();
        QColor mark = checkbox.value("text").value<QColor>();
        QColor checked = checkbox.value("checked").value<QColor>();
        if (!checked.isValid())
          checked = option->palette.color(QPalette::Highlight);

        bool on = option->state & State_On;
        bool partial = option->state & State_NoChange;
        bool hover = option->state & State_MouseOver;

        qreal side = qMin(option->rect.width(), option->rect.height()) - 2;
        QRectF box(0, 0, side, side);
        box.moveCenter(QRectF(option->rect).center());
        box.adjust(0.5, 0.5, -0.5, -0.5);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        if (!(option->state & State_Enabled))
          painter->setOpacity(0.5);

        painter->setPen(QPen(on || partial || hover ? checked : outline, 1));
        painter->setBrush(on || partial ? checked : fill);
        painter->drawRoundedRect(box, 3, 3);

        QPen pen(mark, qMax(1.5, side / 8));
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        qreal x = box.left(), y = box.top(), w = box.width();
        if (on) {
          QPainterPath path;
          path.moveTo(x + w * 0.26, y + w * 0.52);
          path.lineTo(x + w * 0.43, y + w * 0.69);
          path.lineTo(x + w * 0.75, y + w * 0.33);
          painter->drawPath(path);
        } else if (partial) {
          painter->drawLine(QPointF(x + w * 0.28, y + w * 0.5),
                            QPointF(x + w * 0.72, y + w * 0.5));
        }
        painter->restore();
        break;
      }

      case PE_IndicatorTabClose:
        CustomTheme::drawCloseButton(option, painter);
        break;

      default:
        baseStyle()->drawPrimitive(elem, option, painter, widget);
        break;
    }
  }

  void polish(QWidget *widget) override {
    baseStyle()->polish(widget);
    if (QWindow *window = widget->windowHandle())
      mTheme->polishWindow(window);
  }

  void polish(QPalette &palette) override {
    baseStyle()->polish(palette);
    mTheme->polish(palette);
  }

private:
  const CustomTheme *mTheme;
};

} // namespace

CustomTheme::CustomTheme(const QString &name) : mName(name) {
  mDir = Settings::themesDir();
  if (!QFileInfo(mDir.filePath(QString("%1.lua").arg(name))).exists())
    mDir = userDir();

  QByteArray file = mDir.filePath(QString("%1.lua").arg(name)).toUtf8();
  mMap = ConfFile(file).parse("theme");
}

QDir CustomTheme::dir() const { return mDir; }

QString CustomTheme::name() const { return mName; }

QStyle *CustomTheme::style() const { return new CustomStyle(this); }

QString CustomTheme::styleSheet() const {
  QString text = "QLineEdit {"
                 "  border: 1px solid @control_border;"
                 "  border-radius: 5px;"
                 "  padding: 2px 4px;"
                 "  background: @content"
                 "}"
                 "QLineEdit:focus {"
                 "  border-color: @accent"
                 "}"

                 "QToolButton {"
                 "  border: none;"
                 "  border-radius: 5px"
                 "}"
                 "QToolButton:hover {"
                 "  background: @hover"
                 "}"
                 "QToolButton:pressed {"
                 "  background: %1"
                 "}"

                 "QScrollBar:vertical {"
                 "  background: transparent;"
                 "  width: 10px;"
                 "  margin: 0"
                 "}"
                 "QScrollBar:horizontal {"
                 "  background: transparent;"
                 "  height: 10px;"
                 "  margin: 0"
                 "}"
                 "QScrollBar::handle {"
                 "  background: @control_border;"
                 "  border-radius: 3px;"
                 "  margin: 2px"
                 "}"
                 "QScrollBar::handle:hover {"
                 "  background: @text_muted"
                 "}"
                 "QScrollBar::handle:vertical {"
                 "  min-height: 32px"
                 "}"
                 "QScrollBar::handle:horizontal {"
                 "  min-width: 32px"
                 "}"
                 "QScrollBar::add-line, QScrollBar::sub-line {"
                 "  width: 0;"
                 "  height: 0"
                 "}"
                 "QScrollBar::add-page, QScrollBar::sub-page {"
                 "  background: none"
                 "}"

                 "QStatusBar {"
                 "  background: @chrome;"
                 "  border-top: 1px solid @divider;"
                 "  color: @text_secondary"
                 "}"
                 "QStatusBar::item {"
                 "  border: none"
                 "}"

                 "QPushButton {"
                 "  border: 1px solid @control_border;"
                 "  border-radius: 5px;"
                 "  padding: 3px 12px;"
                 "  background: palette(button);"
                 "  color: palette(button-text)"
                 "}"
                 "QPushButton:hover {"
                 "  background: @hover"
                 "}"
                 "QPushButton:pressed {"
                 "  background: %1"
                 "}"
                 "QPushButton:checked {"
                 "  background: @accent_subtle;"
                 "  border-color: @accent;"
                 "  color: @accent_text"
                 "}"
                 "QPushButton:default {"
                 "  border-color: @accent"
                 "}"
                 "QPushButton#CommitButton:enabled,"
                 "QPushButton#CommitButton:enabled:hover {"
                 "  background: @accent;"
                 "  border-color: @accent;"
                 "  color: @on_accent;"
                 "  font-weight: 600"
                 "}"
                 "QPushButton:disabled {"
                 "  color: @text_muted;"
                 "  border-color: @divider"
                 "}"

                 "_FileWidget--Header {"
                 "  background: @subtle;"
                 "  border-bottom: 1px solid @divider"
                 "}"
                 "#HunkHeader {"
                 "  background: @hunk_header"
                 "}"

                 "SideBar QTreeView, SideBar Footer {"
                 "  background: @sidebar"
                 "}"

                 "ContentWidget TreeView {"
                 "  border: 1px solid @divider"
                 "}"

                 "CommitList {"
                 "  border: none;"
                 "  border-top: 1px solid @divider;"
                 "  border-right: 1px solid @divider"
                 "}"

                 "DetailView QTextEdit {"
                 "  border: 1px solid @control_border;"
                 "  border-radius: 6px;"
                 "  background: @content"
                 "}"
                 "DetailView QTextEdit#MessageLabel {"
                 "  background: palette(window);"
                 "  border: 1px solid palette(window);"
                 "  color: palette(window-text)"
                 "}"
                 "DetailView .QFrame {"
                 "  border-top: 1px solid @divider"
                 "}"

                 "DiffView {"
                 "  background: @content"
                 "}"
                 "DiffView FileWidget {"
                 "  background: @content"
                 "}"
                 "DiffView HunkWidget {"
                 "  border-top: 1px solid @divider"
                 "}"
                 "DiffView HunkWidget QLabel {"
                 "  color: @text_muted"
                 "}"

                 "FindWidget {"
                 "  background: palette(highlight)"
                 "}"
                 "FindWidget QToolButton {"
                 "  border: none;"
                 "  border-radius: 4px"
                 "}"
                 "FindWidget QToolButton:pressed {"
                 "  background: rgba(0, 0, 0, 20%)"
                 "}"
                 "FindWidget QLineEdit {"
                 "  border: none"
                 "}"

                 "Footer {"
                 "  background: @chrome;"
                 "  border: none;"
                 "  border-top: 1px solid @divider"
                 "}"
                 "Footer QToolButton {"
                 "  border: none"
                 "}"

                 "LogView {"
                 "  selection-background-color: @accent_subtle"
                 "}"

                 "MenuBar {"
                 "  background: %2;"
                 "  color: %3"
                 "}"

                 "QComboBox QListView {"
                 "  background: palette(base);"
                 "}"

                 "QToolTip {"
                 "  color: palette(text);"
                 "  background: palette(highlight);"
                 "  border: 1px solid palette(highlight)"
                 "}"

                 "TabBar::tab {"
                 "  border: none;"
                 "  border-right: 1px solid @divider;"
                 "  background: %5;"
                 "  color: %6;"
                 "}"
                 "TabBar::tab:selected {"
                 "  background: %7;"
                 "  color: @text"
                 "}"

                 "ToolBar {"
                 "  background: @chrome;"
                 "  border: none;"
                 "  border-bottom: 1px solid @divider"
                 "}"
                 "ToolBar QToolButton {"
                 "  border: none"
                 "}"
                 "ToolBar QToolButton:enabled:active:checked {"
                 "  background: %4"
                 "}"

                 "TreeWidget QColumnView {"
                 "  border-top: 1px solid palette(window);"
                 "  border-right: 1px solid palette(base);"
                 "  border-bottom: 1px solid palette(window)"
                 "}"

                 "CommitDetail QToolButton,"
                 "HunkWidget QToolButton {"
                 "  border: none;"
                 "  border-radius: 5px"
                 "}"

                 "CommitToolBar QToolButton {"
                 "  background: none"
                 "}"

                 "QTableView QPushButton {"
                 "  margin: 2px;"
                 "  padding: 6px"
                 "}"

                 "QWidget {"
                 "  %8"
                 "}";

  QVariantMap button = mMap.value("button").toMap();
  QVariantMap menubar = mMap.value("menubar").toMap();
  QVariantMap tabbar = mMap.value("tabbar").toMap();

  QString tabbarBase = tabbar.value("base").toString();
  if (tabbarBase.isEmpty())
    tabbarBase = "palette(dark)";

  QString tabbarText = tabbar.value("text").toString();
  if (tabbarText.isEmpty())
    tabbarText = "palette(text)";

  QString tabbarSelected = tabbar.value("selected").toString();
  if (tabbarSelected.isEmpty())
    tabbarSelected = "palette(window)";

  QString font;
  QVariantMap fontMap = mMap.value("font").toMap();

  QString fontValue = fontMap.value("family").toString();
  if (!fontValue.isEmpty())
    font += QString("font-family: \"%1\";").arg(fontValue);

  QVariant fontVariantValue = fontMap.value("size");
  bool ok;
  double fontNumValue = fontVariantValue.toDouble(&ok);
  if (ok) {
    font += QString("font-size: %1pt;").arg(fontNumValue);
  } else {
    fontValue = fontVariantValue.toString();
    if (!fontValue.isEmpty())
      font += QString("font-size: %1;").arg(fontValue);
  }

  fontValue = fontMap.value("weight").toString();
  if (!fontValue.isEmpty())
    font += QString("font-weight: %1;").arg(fontValue);

  fontValue = fontMap.value("style").toString();
  if (!fontValue.isEmpty())
    font += QString("font-style: %1;").arg(fontValue);

  text = text.arg(
      button.value("background").toMap().value("pressed").toString(),
      menubar.value("background").toString(), menubar.value("text").toString(),
      button.value("background").toMap().value("checked").toString(),
      tabbarBase, tabbarText, tabbarSelected, font);

  // Replace @name tokens with the theme's value, or a palette fallback.
  QVariantMap tokens = mMap.value("tokens").toMap();
  static const QRegularExpression tokenRe("@([a-z_]+)");
  QString resolved;
  qsizetype last = 0;
  QRegularExpressionMatchIterator it = tokenRe.globalMatch(text);
  while (it.hasNext()) {
    QRegularExpressionMatch match = it.next();
    QString name = match.captured(1);
    QString value = tokens.value(name).toString();
    if (value.isEmpty())
      value = kTokenFallbacks.value(name, match.captured(0));
    resolved += text.mid(last, match.capturedStart() - last) + value;
    last = match.capturedEnd();
  }
  return resolved + text.mid(last);
}

void CustomTheme::polish(QPalette &palette) const {
  QVariantMap map = mMap.value("palette").toMap();
  setPaletteColors(palette, QPalette::Light, map.value("light"));
  setPaletteColors(palette, QPalette::Midlight, map.value("midlight"));
  setPaletteColors(palette, QPalette::Mid, map.value("middark"));
  setPaletteColors(palette, QPalette::Dark, map.value("dark"));
  setPaletteColors(palette, QPalette::Shadow, map.value("shadow"));

  QVariantMap widget = mMap.value("widget").toMap();
  setPaletteColors(palette, QPalette::Text, widget.value("text"));
  setPaletteColors(palette, QPalette::BrightText, widget.value("bright_text"));
  setPaletteColors(palette, QPalette::Base, widget.value("background"));
  setPaletteColors(palette, QPalette::AlternateBase, widget.value("alternate"));
  setPaletteColors(palette, QPalette::Highlight, widget.value("highlight"));
  setPaletteColors(palette, QPalette::HighlightedText,
                   widget.value("highlighted_text"));

  // Derive placeholder text from the theme's text, so it never inherits a
  // colour meant for the opposite scheme (light vs dark mode)
  for (auto group :
       {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
    QColor placeholder = palette.color(group, QPalette::Text);
    placeholder.setAlpha(128);
    palette.setColor(group, QPalette::PlaceholderText, placeholder);
  }

  QVariantMap window = mMap.value("window").toMap();
  setPaletteColors(palette, QPalette::WindowText, window.value("text"));
  setPaletteColors(palette, QPalette::Window, window.value("background"));

  QVariantMap button = mMap.value("button").toMap();
  setPaletteColors(palette, QPalette::ButtonText, button.value("text"));
  setPaletteColors(palette, QPalette::Button, button.value("background"));

  QVariantMap link = mMap.value("link").toMap();
  setPaletteColors(palette, QPalette::Link, link.value("link"));
  setPaletteColors(palette, QPalette::LinkVisited, link.value("link_visited"));

  QVariantMap tooltip = mMap.value("tooltip").toMap();
  setPaletteColors(palette, QPalette::ToolTipText, tooltip.value("text"));
  setPaletteColors(palette, QPalette::ToolTipBase, tooltip.value("background"));
}

QColor CustomTheme::badge(BadgeRole role, BadgeState state) {
  QString roleKey;
  switch (role) {
    case BadgeRole::Foreground:
      roleKey = "foreground";
      break;

    case BadgeRole::Background:
      roleKey = "background";
      break;

    case BadgeRole::Border:
      roleKey = "border";
      break;
  }

  // Themes without badge borders draw the border in the background colour.
  QVariantMap badge = mMap.value("badge").toMap();
  if (role == BadgeRole::Border && !badge.contains(roleKey))
    return this->badge(BadgeRole::Background, state);

  QString stateKey;
  switch (state) {
    case BadgeState::Normal:
      stateKey = "normal";
      break;

    case BadgeState::Selected:
      stateKey = "selected";
      break;

    case BadgeState::Conflicted:
      stateKey = "conflicted";
      break;

    case BadgeState::Head:
      stateKey = "head";
      break;

    case BadgeState::Notification:
      stateKey = "notification";
      break;
    case BadgeState::Modified:
      stateKey = "modified";
      break;
    case BadgeState::Added:
      stateKey = "added";
      break;
    case BadgeState::Deleted:
      stateKey = "deleted";
      break;
    case BadgeState::Untracked:
      stateKey = "untracked";
      break;
    case BadgeState::Renamed:
      stateKey = "renamed";
      break;
  }

  QVariant variant = badge.value(roleKey);
  if (variant.canConvert<QColor>())
    return variant.value<QColor>();

  QVariantMap map = variant.toMap();
  if (!map.contains(stateKey))
    stateKey = "normal";

  return map.value(stateKey).value<QColor>();
}

QList<QColor> CustomTheme::branchTopologyEdges() {
  QVariantMap edge = mMap.value("graph").toMap();

  QList<QColor> colors;
  for (int i = 0; i < 15; i++) {
    QString name = QString("edge%1").arg(i + 1);
    colors.append(edge.value(name).toString());
  }

  return colors;
}

QColor CustomTheme::buttonChecked() {
  QVariantMap button = mMap.value("button").toMap();
  QVariantMap buttonText = button.value("text").toMap();
  if (buttonText.contains("checked"))
    return buttonText.value("checked").value<QColor>();
  return buttonText.value("default").value<QColor>();
}

QVariantMap CustomTheme::checkbox() const {
  return mMap.value("checkbox").toMap();
}

QPalette CustomTheme::commitList() {
  QVariantMap commitList = mMap.value("commits").toMap();

  QPalette palette;
  setPaletteColors(palette, QPalette::Text, commitList.value("text"));
  setPaletteColors(palette, QPalette::BrightText,
                   commitList.value("bright_text"));
  setPaletteColors(palette, QPalette::Base, commitList.value("background"));
  setPaletteColors(palette, QPalette::AlternateBase,
                   commitList.value("alternate"));
  setPaletteColors(palette, QPalette::Highlight, commitList.value("highlight"));
  setPaletteColors(palette, QPalette::HighlightedText,
                   commitList.value("highlighted_text"));
  setPaletteColors(palette, QPalette::WindowText,
                   commitList.value("highlighted_bright_text"));
  setPaletteColors(palette, QPalette::PlaceholderText,
                   commitList.value("muted_text"));
  return palette;
}

QColor CustomTheme::commitEditor(CommitEditor color) {
  QVariantMap commitEditor = mMap.value("commiteditor").toMap();

  switch (color) {
    case CommitEditor::SpellError:
      return commitEditor.value("spellerror").value<QColor>();
    case CommitEditor::SpellIgnore:
      return commitEditor.value("spellignore").value<QColor>();
    case CommitEditor::LengthWarning:
      return commitEditor.value("lengthwarning").value<QColor>();
  }
  throw std::runtime_error("unreachable; value=" +
                           std::to_string(static_cast<int>(color)));
}

QColor CustomTheme::diff(Diff color) {
  QVariantMap diff = mMap.value("diff").toMap();

  switch (color) {
    case Diff::Ours:
      return diff.value("ours").value<QColor>();
    case Diff::Theirs:
      return diff.value("theirs").value<QColor>();
    case Diff::Addition:
      return diff.value("addition").value<QColor>();
    case Diff::Deletion:
      return diff.value("deletion").value<QColor>();
    case Diff::WordAddition:
      return diff.value("word_addition").value<QColor>();
    case Diff::WordDeletion:
      return diff.value("word_deletion").value<QColor>();
    case Diff::Plus:
      return diff.value("plus").value<QColor>();
    case Diff::Minus:
      return diff.value("minus").value<QColor>();
    case Diff::Note:
      return diff.value("note").value<QColor>();
    case Diff::Warning:
      return diff.value("warning").value<QColor>();
    case Diff::Error:
      return diff.value("error").value<QColor>();
  }
  throw std::runtime_error("unreachable; value=" +
                           std::to_string(static_cast<int>(color)));
}

QColor CustomTheme::heatMap(HeatMap color) {
  QVariantMap heatmap = mMap.value("blame").toMap();

  switch (color) {
    case HeatMap::Hot:
      return QColor(heatmap.value("hot").toString());
    case HeatMap::Cold:
      return QColor(heatmap.value("cold").toString());
  }
  throw std::runtime_error("unreachable; value=" +
                           std::to_string(static_cast<int>(color)));
}

QColor CustomTheme::remoteComment(Comment color) {
  QVariantMap comment = mMap.value("comment").toMap();

  switch (color) {
    case Comment::Background:
      return QColor(comment.value("background").toString());
    case Comment::Body:
      return QColor(comment.value("body").toString());
    case Comment::Author:
      return QColor(comment.value("author").toString());
    case Comment::Timestamp:
      return QColor(comment.value("timestamp").toString());
  }
  throw std::runtime_error("unreachable; value=" +
                           std::to_string(static_cast<int>(color)));
}

QColor CustomTheme::star() {
  return mMap.value("star").toMap().value("fill").value<QColor>();
}

QVariantMap CustomTheme::editorStyleProperties() const {
  return mMap.value("property").toMap();
}

#if defined(Q_OS_WIN)
void CustomTheme::polishWindow(QWindow *window) const {
  QVariant variant = mMap.value("titlebar").toMap().value("background");
  if (!variant.canConvert<QColor>())
    return;

  // Attribute ids are spelled out so older Windows SDKs still build.
  const DWORD kUseImmersiveDarkMode = 20;
  const DWORD kCaptionColor = 35;
  const DWORD kTextColor = 36;

  // Windows 10 ignores the caption and text colours; it still honours the
  // dark mode flag.
  HWND hwnd = reinterpret_cast<HWND>(window->winId());
  QColor background = variant.value<QColor>();
  BOOL dark = background.lightnessF() < 0.5;
  DwmSetWindowAttribute(hwnd, kUseImmersiveDarkMode, &dark, sizeof(dark));

  COLORREF caption =
      RGB(background.red(), background.green(), background.blue());
  DwmSetWindowAttribute(hwnd, kCaptionColor, &caption, sizeof(caption));

  QVariant text = mMap.value("window").toMap().value("text");
  if (text.canConvert<QColor>()) {
    QColor color = text.value<QColor>();
    COLORREF textColor = RGB(color.red(), color.green(), color.blue());
    DwmSetWindowAttribute(hwnd, kTextColor, &textColor, sizeof(textColor));
  }
}
#elif !defined(Q_OS_MAC)
void CustomTheme::polishWindow(QWindow *window) const {
  // The window manager draws the title bar here.
  Q_UNUSED(window)
}
#endif

void CustomTheme::drawCloseButton(const QStyleOption *option,
                                  QPainter *painter) {
  qreal in = 3.5;
  qreal out = 8.0;
  QRect rect = option->rect;
  qreal x = rect.x() + (rect.width() / 2);
  qreal y = rect.y() + (rect.height() / 2);

  painter->save();
  painter->setRenderHints(QPainter::Antialiasing);

  // Draw background.
  if (option->state & QStyle::State_MouseOver) {
    painter->save();
    painter->setPen(Qt::NoPen);
    bool selected = (option->state & QStyle::State_Selected);
    painter->setBrush(QColor(selected ? QPalette().color(QPalette::Highlight)
                                      : QPalette().color(QPalette::Base)));
    QRectF background(x - out, y - out, 2 * out, 2 * out);
    painter->drawRoundedRect(background, 2.0, 2.0);
    painter->restore();
  }

  // Draw x.
  painter->setPen(QPen(QPalette().color(QPalette::WindowText), 1.5));
  painter->drawLine(QPointF(x - in, y - in), QPointF(x + in, y + in));
  painter->drawLine(QPointF(x - in, y + in), QPointF(x + in, y - in));
  painter->restore();
}

QDir CustomTheme::userDir(bool create, bool *exists) {
  QDir dir = Settings::userDir();

  if (create)
    dir.mkpath("themes");

  bool tmp = dir.cd("themes");
  if (exists)
    *exists = tmp;

  return dir;
}

bool CustomTheme::isValid(const QString &name) {
  QDir confDir = Settings::themesDir();
  if (QFileInfo(confDir.filePath(QString("%1.lua").arg(name))).exists())
    return true;

  bool exists = false;
  QDir dir = userDir(false, &exists);
  return exists &&
         QFileInfo(dir.filePath(QString("%1.lua").arg(name))).exists();
}
QColor CustomTheme::notice(Notice role) {
  QVariantMap notice = mMap.value("notice").toMap();

  switch (role) {
    case Notice::Background:
      return QColor(notice.value("background").toString());
    case Notice::Foreground:
      return QColor(notice.value("foreground").toString());
  }
  throw std::runtime_error("unreachable; value=" +
                           std::to_string(static_cast<int>(role)));
}

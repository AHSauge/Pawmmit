//
//          Copyright (c) 2020
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Martin Marmsoler
//

#ifndef STATEPUSHBUTTON_H
#define STATEPUSHBUTTON_H

#include <QPushButton>

/*!
 */
class StatePushButton : public QPushButton {
  Q_OBJECT

public:
  StatePushButton(QString textChecked, QString textUnchecked,
                  QWidget *parent = nullptr);
  void setState(bool checked);
  bool toggleState();
  bool checked();

private:
  bool m_checked{false};
  QString m_textChecked{""};
  QString m_textUnchecked{""};
};
#endif // STATEPUSHBUTTON_H

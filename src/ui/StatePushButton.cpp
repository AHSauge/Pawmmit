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

#include "StatePushButton.h"

StatePushButton::StatePushButton(QString textChecked, QString textUnchecked,
                                 QWidget *parent)
    : QPushButton(parent), m_textChecked(textChecked),
      m_textUnchecked(textUnchecked) {
  setState(m_checked);
}

void StatePushButton::setState(bool checked) {
  // for initial set this makes problems. No really
  // need of this condition, because it will not occur
  // often
  //	if (m_checked == checked)
  //		return;

  m_checked = checked;

  if (m_checked)
    setText(m_textChecked);
  else
    setText(m_textUnchecked);
}

bool StatePushButton::toggleState() {
  setState(!m_checked);
  return m_checked;
}

bool StatePushButton::checked() { return m_checked; }

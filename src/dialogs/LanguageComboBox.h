//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef LANGUAGECOMBOBOX_H
#define LANGUAGECOMBOBOX_H

class QComboBox;

// Lists the available languages by their native names and keeps the language
// setting in sync with the selection.
void setupLanguageComboBox(QComboBox *comboBox);

#endif

// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Michael WERLE
//

#ifndef RENAMEBRANCHDIALOG_H
#define RENAMEBRANCHDIALOG_H

#include "git/Branch.h"
#include <QDialog>

class QLineEdit;

namespace git {
class Reference;
class Repository;
} // namespace git

class RenameBranchDialog : public QDialog {
  Q_OBJECT

public:
  RenameBranchDialog(const git::Repository &repo, const git::Branch &branch,
                     QWidget *parent = nullptr);

  QString name() const;

private:
  QLineEdit *mName;
};

#endif

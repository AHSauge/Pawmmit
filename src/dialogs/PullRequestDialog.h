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

#ifndef PULLREQUESTDIALOG_H
#define PULLREQUESTDIALOG_H

#include <QDialog>

class RepoView;
class QLineEdit;
class QTextEdit;

namespace git {
class Commit;
}

class PullRequestDialog : public QDialog {
  Q_OBJECT

public:
  PullRequestDialog(RepoView *view);

private:
  QLineEdit *mTitle;
  QTextEdit *mBody;

  void setCommit(const git::Commit &commit);
};

#endif

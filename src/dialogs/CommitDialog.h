//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

#include "conf/Setting.h"
#include <QDialog>

class QTextEdit;

class CommitDialog : public QDialog {
  Q_OBJECT

public:
  CommitDialog(const QString &message, Prompt::Kind kind,
               QWidget *parent = nullptr);

  QString message() const;

  void open() override;

private:
  QTextEdit *mEditor;
};

#endif

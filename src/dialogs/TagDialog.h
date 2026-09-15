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

#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include <QDialog>
#include "git/Remote.h"
#include <QScopedPointer>

namespace git {
class Repository;
}

namespace Ui {
class TagDialog;
}

class TagDialog : public QDialog {
  Q_OBJECT

public:
  TagDialog(const git::Repository &repo, const QString &id,
            const git::Remote &remote = git::Remote(),
            QWidget *parent = nullptr);
  ~TagDialog() override;

  bool force() const;
  git::Remote remote() const;
  QString name() const;
  QString message() const;

private:
  git::Remote mRemote;
  QStringList mExistingTags;
  QStringList mFilteredTags;
  QString mOldTagname;

  QScopedPointer<Ui::TagDialog> ui;
};

#endif

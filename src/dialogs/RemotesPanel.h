//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Bryan Williams
//

#ifndef REMOTESPANEL_H
#define REMOTESPANEL_H

#include "git/Repository.h"
#include <QScopedPointer>
#include <QWidget>

namespace Ui {
class RemotesPanel;
}

class RemotesPanel : public QWidget {
  Q_OBJECT

public:
  RemotesPanel(const git::Repository &repo, QWidget *parent = nullptr);
  ~RemotesPanel() override;

  void addRemote(const QString &name = QString());

private:
  git::Repository mRepo;
  QScopedPointer<Ui::RemotesPanel> ui;
};

#endif

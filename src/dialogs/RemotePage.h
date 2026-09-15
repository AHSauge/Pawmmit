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

#ifndef REMOTEPAGE_H
#define REMOTEPAGE_H

#include <QScopedPointer>
#include <QWizardPage>

class Repository;

namespace Ui {
class RemotePage;
}

class RemotePage : public QWizardPage {
  Q_OBJECT

public:
  RemotePage(Repository *repo, QWidget *parent = nullptr);
  ~RemotePage() override;

  bool isComplete() const override;

private:
  QScopedPointer<Ui::RemotePage> ui;
};

#endif

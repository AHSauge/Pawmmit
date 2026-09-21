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

#ifndef LOCATIONPAGE_H
#define LOCATIONPAGE_H

#include <QScopedPointer>
#include <QWizardPage>

namespace Ui {
class LocationPage;
}

class LocationPage : public QWizardPage {
  Q_OBJECT

public:
  LocationPage(bool init, QWidget *parent = nullptr);
  ~LocationPage() override;

  bool isComplete() const override;
  int nextId() const override;
  void initializePage() override;

private:
  void updateFullPath();

  bool mInit;
  QScopedPointer<Ui::LocationPage> ui;
};

#endif

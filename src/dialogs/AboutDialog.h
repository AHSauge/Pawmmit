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

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog {
  Q_OBJECT

public:
  enum Index { Changelog, Acknowledgments, Privacy };

  AboutDialog(QWidget *parent = nullptr);
  ~AboutDialog() override;

  static void openSharedInstance(Index index = Changelog);

private:
  void setCurrentIndex(Index index);

  QScopedPointer<Ui::AboutDialog> ui;
};

#endif

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

#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QScopedPointer>

class MainWindow;

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog {
  Q_OBJECT

public:
  StartDialog(QWidget *parent = nullptr);
  ~StartDialog() override;

  void accept() override;

  static StartDialog *openSharedInstance();

protected:
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;

private:
  void updateButtons();

  void edit(const QModelIndex &index = QModelIndex());
  void remove();

  MainWindow *openWindow(const QString &repo);

  QAction *mClone;
  QAction *mOpen;
  QAction *mInit;

  QScopedPointer<Ui::StartDialog> ui;
};

#endif

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

#ifndef CHECKOUTDIALOG_H
#define CHECKOUTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

class QPushButton;

namespace git {
class Reference;
class Repository;
} // namespace git

namespace Ui {
class CheckoutDialog;
}

class CheckoutDialog : public QDialog {
  Q_OBJECT

public:
  CheckoutDialog(const git::Repository &repo, const git::Reference &ref,
                 QWidget *parent = nullptr);
  ~CheckoutDialog() override;

  git::Reference reference() const;
  bool detach() const { return mDetach; }

private:
  void update(const git::Reference &ref);

  bool mDetach = false;

  QPushButton *mCheckout;

  QScopedPointer<Ui::CheckoutDialog> ui;
};

#endif

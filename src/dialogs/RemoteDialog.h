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

#ifndef REMOTEDIALOG_H
#define REMOTEDIALOG_H

#include <QDialog>
#include "git/Repository.h"

class ReferenceList;
class RepoView;
class QCheckBox;
class QComboBox;
class QLineEdit;

namespace git {
class Reference;
class Remote;
} // namespace git

class RemoteDialog : public QDialog {
  Q_OBJECT

public:
  enum Kind { Fetch, Pull, Push };

  RemoteDialog(Kind kind, RepoView *parent);

private:
  QComboBox *mRemotes;
  ReferenceList *mRefs = nullptr;
  QComboBox *mAction = nullptr;
  QCheckBox *mTags = nullptr;
  QCheckBox *mSetUpstream = nullptr;
  QCheckBox *mForce = nullptr;
  QLineEdit *mRemoteRef = nullptr;
};

#endif

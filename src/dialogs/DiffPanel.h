//
//          Copyright (c) 2017, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#ifndef DIFFPANEL_H
#define DIFFPANEL_H

#include "git/Config.h"
#include <QWidget>

class QHBoxLayout;

class DiffPanel : public QWidget {
  Q_OBJECT

public:
  DiffPanel(const git::Repository &repo, QWidget *parent = nullptr);

private:
  git::Config mConfig;
};

#endif

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

#ifndef SHOWTOOL_H
#define SHOWTOOL_H

#include "ExternalTool.h"

class ShowTool : public ExternalTool {
  Q_OBJECT

public:
  static bool openFileManager(QString path);

  ShowTool(const QString &file, QObject *parent = nullptr);

  Kind kind() const override;
  QString name() const override;

  bool start() override;
};

#endif

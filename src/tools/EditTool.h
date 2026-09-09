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

#ifndef EDITTOOL_H
#define EDITTOOL_H

#include "ExternalTool.h"

class EditTool : public ExternalTool {
  Q_OBJECT

public:
  EditTool(const QString &file, QObject *parent = nullptr);

  bool isValid() const override;

  Kind kind() const override;
  QString name() const override;

  bool start() override;
};

#endif

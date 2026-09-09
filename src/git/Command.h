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

#ifndef COMMAND_H
#define COMMAND_H

#include <QString>

class QProcessEnvironment;

namespace git {

class Command {
public:
  static QString bashPath();
  static QString substitute(const QProcessEnvironment &env,
                            const QString &command);
};

} // namespace git

#endif

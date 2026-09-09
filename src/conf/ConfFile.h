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

#ifndef CONFFILE_H
#define CONFFILE_H

#include <QString>
#include <QVariant>

class ConfFile {
public:
  ConfFile(const QString &filename);
  virtual ~ConfFile();

  // Table is the name of a new global table that the script
  // is expected to modify. If the table name is empty, then
  // the script should return a single anonymous table.
  QVariantMap parse(const QString &table = QString());

private:
  QString mFilename;
};

#endif

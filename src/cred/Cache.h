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

#ifndef CACHE_H
#define CACHE_H

#include "CredentialHelper.h"
#include <QMap>

class Cache : public CredentialHelper {
public:
  Cache();

  bool get(const QString &url, QString &username, QString &password) override;

  bool store(const QString &url, const QString &username,
             const QString &password) override;

private:
  QMap<QString, QMap<QString, QString>> mCache;
};

#endif

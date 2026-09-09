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

#ifndef SUBMODULE_H
#define SUBMODULE_H

#include "Remote.h"
#include "Result.h"
#include "git2/submodule.h"
#include <QSharedPointer>

namespace git {

class Id;
class Repository;

class Submodule {
public:
  Submodule();

  bool isValid() const { return !d.isNull(); }
  explicit operator bool() const { return isValid(); }

  bool isInitialized() const;
  void initialize() const;
  void deinitialize() const;

  QString name() const;
  QString path() const;

  QString url() const;
  void setUrl(const QString &url);

  QString branch() const;
  void setBranch(const QString &branch);

  Id headId() const;
  Id indexId() const;
  Id workdirId() const;

  Result update(Remote::Callbacks *callbacks, bool init = false,
                bool checkout_force = false);

  Repository open() const;

private:
  Submodule(git_submodule *submodule);
  operator git_submodule *() const;

  QSharedPointer<git_submodule> d;

  friend class Index;
  friend class Repository;
};

} // namespace git

Q_DECLARE_METATYPE(git::Submodule);

#endif

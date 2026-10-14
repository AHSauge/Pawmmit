//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef IDSTORAGE_H
#define IDSTORAGE_H

#include "git/Id.h"

#include <QSet>

namespace git {
class Repository;
}

/// @brief Commit IDs that are already in the the index database
/// @note Only read after construction, so it needs no locking
class IdStorage {
public:
  IdStorage(const git::Repository &repo);

  bool contains(const git::Id &id) const;
  const QSet<git::Id> &ids() const { return mIds; }

private:
  QSet<git::Id> mIds;
};

#endif // IDSTORAGE_H

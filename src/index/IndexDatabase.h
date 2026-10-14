//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INDEXDATABASE_H
#define INDEXDATABASE_H

#include <memory>

extern "C" {
struct sqlite3;
}

namespace git {
class Repository;
}

class QLockFile;

namespace index_database {
// Returns nullptr if the database couldn't be opened, or if a read-only
// connection is requested before the database exists.
std::shared_ptr<sqlite3> open(const git::Repository &repo, bool readOnly);

/// @brief Attempts to lock the database for write/erase operations
/// @param repo Which repository to touch
/// @param timeout How long to wait for the lock itself
/// @return nullptr if locking was unsuccessful
std::unique_ptr<QLockFile> lock(const git::Repository &repo, int timeout = 0);

/// @brief Attempt to remove the index database
/// @param repo Which repository to touch
/// @return True of the removal was sucessful
bool remove(const git::Repository &repo);
}; // namespace index_database

#endif

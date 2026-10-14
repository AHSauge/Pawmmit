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
// Must match PRAGMA user_version in schema.sql. Indexes with an older version
// are rebuilt from scratch rather than migrated.
constexpr int kSchemaVersion = 1;

// Returns nullptr if the database couldn't be opened. Read-only connections
// also get nullptr until the indexer has created the current schema.
std::shared_ptr<sqlite3> open(const git::Repository &repo, bool readOnly);

// The database's user_version: 0 before the schema exists, -1 on failure.
int schemaVersion(sqlite3 *db);

/// @brief Attempts to lock the database for write/erase operations
/// @param repo Which repository to touch
/// @param timeout How long to wait for the lock itself
/// @return nullptr if locking was unsuccessful
std::unique_ptr<QLockFile> lock(const git::Repository &repo, int timeout = 0);

/// @brief Attempt to remove the index database
/// @param repo Which repository to touch
/// @return True if the removal was successful
bool remove(const git::Repository &repo);

/// @brief Deletes the database files. The caller must hold the lock, and must
/// have closed its own connections.
/// @return True if the files are gone
bool removeFiles(const git::Repository &repo);
}; // namespace index_database

#endif

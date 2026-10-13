//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INDEXDATABASE_H
#define INDEXDATABASE_H

#include <QString>
#include <memory>

extern "C" {
struct sqlite3;
}

namespace git {
class Repository;
}

namespace index_database {
// Returns nullptr if the database couldn't be opened.
std::shared_ptr<sqlite3> open(const git::Repository &repo, bool readOnly);
}; // namespace index_database

#endif

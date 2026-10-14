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

#ifndef QUERY_H
#define QUERY_H

#include "Index.h"
#include <memory>
#include <variant>

using QueryRef = std::shared_ptr<class Query>;

// Parameters of the generated SQL, bound in the order they were added.
struct SqlText {
  QByteArray value;
};
struct SqlBlob {
  QByteArray value;
};
using SqlParam = std::variant<qint64, SqlText, SqlBlob>;

class Query {
public:
  virtual ~Query() {}

  virtual QString toString() const = 0;
  virtual QList<Index::Term> terms() const = 0;

  // True if the query only matches through the FTS table, so that it can be
  // combined with other such queries into a single MATCH.
  virtual bool isFts() const { return false; }

  // The FTS5 query expression. Only valid when isFts() is true.
  virtual QByteArray ftsExpression() const { return QByteArray(); }

  // A condition on the commits table, aliased as c. Its parameters are
  // appended in the order their placeholders appear.
  virtual QByteArray condition(const Index &index,
                               QList<SqlParam> &params) const;

  QList<git::Commit> commits(const Index *index) const {
    return index->commits(*this);
  }

  static QueryRef parseQuery(const QString &query);
};

#endif

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

using QueryRef = std::shared_ptr<class Query>;

class Query {
public:
  virtual ~Query() {}

  virtual QString toString() const = 0;
  virtual QList<Index::Term> terms() const = 0;
  virtual QList<git::Commit> commits(const Index *index) const = 0;

  static QueryRef parseQuery(const QString &query);
};

#endif

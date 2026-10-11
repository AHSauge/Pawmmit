//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef FTS_H
#define FTS_H

#include "Index.h"
#include "git/Id.h"
#include <array>

/// @brief Token separator for the indexer
constexpr char kTokenSeparator = ' ';

enum class Subfield : quint8 { None, Identifier, Comment, String };

// This needs to map _exactly_ to the fts table
enum class FTSColumn : quint8 {
  Author,
  MessageIdent,
  MessageString,
  ScopeIdent,
  ScopeComment,
  ScopeString,
  AdditionIdent,
  AdditionComment,
  AdditionString,
  DeletionIdent,
  DeletionComment,
  DeletionString,
  Count
};

constexpr std::array<const char *, size_t(FTSColumn::Count)> kFTSColumnNames = {
    "author",          "message_ident",  "message_string",   "scope_ident",
    "scope_comment",   "scope_string",   "addition_ident",   "addition_comment",
    "addition_string", "deletion_ident", "deletion_comment", "deletion_string",
};
static_assert(kFTSColumnNames.back() != nullptr);

struct CommitDocument {
  git::Id id;
  qint64 time = 0;
  qint32 tzOffset = 0;
  QByteArray email;
  QByteArrayList paths;
  quint32 tokenCount = 0;
  std::array<QByteArray, size_t(FTSColumn::Count)> columns;

  void append(FTSColumn column, QByteArrayView token);
};

FTSColumn columnFor(Index::Field field, Subfield sub);
QByteArray normaliseToken(QByteArrayView token);

#endif

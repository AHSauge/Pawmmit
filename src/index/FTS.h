//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef FTS_H
#define FTS_H

#include "Index.h"
#include "git/Id.h"
#include <array>
#include <cstddef>
#include <QByteArrayList>
#include <QByteArrayView>

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
bool isAscii(QByteArrayView text);
QByteArray normaliseToken(QByteArrayView token);
QByteArrayList ftsColumns(Index::Field field);

/// @brief Splits text into words by Unicode's rules, for indexing and searching
/// @param text UTF-8 text. Invalid UTF-8 only yields its ASCII words.
/// @return The words. Chinese and Japanese text, written without spaces, comes
/// back one character per word.
QByteArrayList segmentWords(QByteArrayView text);

#endif

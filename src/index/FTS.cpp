//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "FTS.h"
#include <algorithm>

void CommitDocument::append(FTSColumn column, QByteArrayView token) {
  QByteArray &buf = columns[size_t(column)];
  if (buf.isEmpty() == false)
    buf += kTokenSeparator;
  buf += token;
  tokenCount++;
}

FTSColumn columnFor(Index::Field field, Subfield sub) {
  switch (field) {
    case Index::Message:
      return sub == Subfield::String ? FTSColumn::MessageString
                                     : FTSColumn::MessageIdent;

    case Index::Scope:
      switch (sub) {
        case Subfield::Comment:
          return FTSColumn::ScopeComment;

        case Subfield::String:
          return FTSColumn::ScopeString;

        case Subfield::Identifier:
        default:
          return FTSColumn::ScopeIdent;
      }

    case Index::Addition:
      switch (sub) {
        case Subfield::Comment:
          return FTSColumn::AdditionComment;

        case Subfield::String:
          return FTSColumn::AdditionString;

        case Subfield::Identifier:
        default:
          return FTSColumn::AdditionIdent;
      }

    case Index::Deletion:
      switch (sub) {
        case Subfield::Comment:
          return FTSColumn::DeletionComment;

        case Subfield::String:
          return FTSColumn::DeletionString;

        case Subfield::Identifier:
        default:
          return FTSColumn::DeletionIdent;
      }

    default:
      Q_UNREACHABLE();
  }
}

QByteArray normaliseToken(QByteArrayView token) {
  bool ascii = std::all_of(token.begin(), token.end(),
                           [](char c) { return uchar(c) < 0x80; });
  if (ascii || token.isValidUtf8() == false)
    return token.toByteArray().toLower();
  return QString::fromUtf8(token).toCaseFolded().toUtf8();
}

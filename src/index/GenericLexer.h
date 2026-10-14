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

#ifndef GENERICLEXER_H
#define GENERICLEXER_H

#include "Lexer.h"
#include <QByteArrayList>

class GenericLexer : public Lexer {
public:
  GenericLexer(QObject *parent = nullptr);

  QByteArray name() const override { return "generic"; }
  bool lex(const QByteArray &buffer) override;
  bool hasNext() override;
  Lexeme next() override;

  /// @brief Sets whether text with non-ASCII characters is split into words
  /// @param segmenting True to split it with segmentWords(), the default.
  /// False returns it whole, for search queries that split terms themselves.
  void setSegmenting(bool segmenting) { mSegmenting = segmenting; }

private:
  int mIndex;
  QByteArray mBuffer;
  bool mSegmenting = true;
  QByteArrayList mPending;
};

#endif

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

class GenericLexer : public Lexer {
public:
  GenericLexer(QObject *parent = nullptr);

  QByteArray name() const override { return "generic"; }
  bool lex(const QByteArray &buffer) override;
  bool hasNext() override;
  Lexeme next() override;

private:
  int mIndex;
  QByteArray mBuffer;
};

#endif

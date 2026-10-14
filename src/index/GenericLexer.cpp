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

#include "GenericLexer.h"
#include "FTS.h"

namespace {

const QByteArray kOperators = ":/.()*?+@";

char safeAt(const QByteArray &buffer, int i) {
  return (i < buffer.length()) ? buffer.at(i) : 0;
}

bool isDigit(char ch) { return (ch >= '0' && ch <= '9'); }

bool isAlpha(char ch) {
  return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

} // namespace

GenericLexer::GenericLexer(QObject *parent) : Lexer(parent) {}

bool GenericLexer::lex(const QByteArray &buffer) {
  mIndex = 0;
  mBuffer = buffer;
  mPending.clear();

  return true;
}

bool GenericLexer::hasNext() {
  return !mPending.isEmpty() || (mIndex < mBuffer.length());
}

Lexer::Lexeme GenericLexer::next() {
  // Words split from an earlier run of non-ASCII text come first.
  if (!mPending.isEmpty())
    return {Identifier, mPending.takeFirst()};

  // Whitespace isn't a valid state itself. It indicates a
  // string of underscores that might become an identifier.
  int startPos = 0;
  bool nonAscii = false;
  Token state = Nothing;
  for (; mIndex <= mBuffer.length(); ++mIndex) {
    char ch = safeAt(mBuffer, mIndex);
    bool alpha = isAlpha(ch);
    bool digit = isDigit(ch);
    bool high = uchar(ch) >= 0x80;
    switch (state) {
      case Nothing:
        if (kOperators.contains(ch)) {
          ++mIndex; // advance
          return {Operator, QByteArray(1, ch)};
        } else if (ch == '_' || alpha || high) {
          startPos = mIndex;
          state = (alpha || high) ? Identifier : Whitespace;
          nonAscii = high;
        } else if (digit) {
          // FIXME: Support -/+?
          startPos = mIndex;
          state = Number;
        } else if (ch == '"') {
          startPos = mIndex;
          state = String;
        }
        break;

      case Whitespace:
        if (alpha || digit || high) {
          state = Identifier;
          nonAscii = high;
        } else if (ch != '_') {
          state = Nothing;
        }
        break;

      case Identifier: {
        if (high) {
          nonAscii = true;
          break;
        }

        char nextCh = safeAt(mBuffer, mIndex + 1);
        bool nextLetter = isAlpha(nextCh) || uchar(nextCh) >= 0x80;
        bool compound = ((ch == '\'' && nextLetter) ||
                         (ch == '-' && (nextLetter || isDigit(nextCh))));
        if (ch != '_' && !alpha && !digit && !compound) {
          QByteArray text = mBuffer.mid(startPos, mIndex - startPos);
          if (!nonAscii)
            return {Identifier, text};

          // Runs with non-ASCII characters are split by Unicode's word rules.
          QByteArrayList words = segmentWords(text);
          if (!words.isEmpty()) {
            if (!mSegmenting)
              return {Identifier, text};
            mPending = words;
            return {Identifier, mPending.takeFirst()};
          }

          // Only symbols or punctuation, so look at this character again.
          state = Nothing;
          nonAscii = false;
          --mIndex;
        }
        break;
      }

      case Number:
        // FIXME: Support .?
        if (!alpha && !digit)
          return {Number, mBuffer.mid(startPos, mIndex - startPos)};
        break;

      case String:
        if (ch == '"') {
          ++mIndex; // advance
          return {String, mBuffer.mid(startPos, mIndex - startPos)};
        }
        break;

      case Comment:      // fall through
      case Keyword:      // fall through
      case Operator:     // fall through
      case Error:        // fall through
      case Preprocessor: // fall through
      case Constant:     // fall through
      case Variable:     // fall through
      case Function:     // fall through
      case Class:        // fall through
      case Type:         // fall through
      case Label:        // fall through
      case Regex:        // fall through
      case Embedded:     // fall through
        Q_ASSERT(false);
    }
  }

  return {Nothing, QByteArray()};
}

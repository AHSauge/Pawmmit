#ifndef LEXERPOOL_H
#define LEXERPOOL_H

#include <QByteArray>
#include <QMultiMap>
#include <QMutex>

class Lexer;

class LexerPool {
public:
  LexerPool();
  ~LexerPool() { qDeleteAll(mLexers); }

  Lexer *acquire(const QByteArray &name);
  void release(Lexer *lexer);

private:
  QByteArray mHome;
  QMutex mMutex;
  QMultiMap<QByteArray, Lexer *> mLexers;
};

#endif // LEXERPOOL_H

#include "map.h"
#include "lexerpool.h"
#include "git/Index.h"
#include "git/Patch.h"
#include "git/Config.h"
#include "git/Signature.h"
#include "../GenericLexer.h"
#include "indexer.h"

#include <QRegularExpression>

namespace {
const QRegularExpression kWsRe("\\s+");

// Some Scintillua lexers rescan the text from its start for every line they
// check, which gets quadratically slower as runs grow.
const int kMaxRunLines = 128;
} // namespace

Map::Map(const git::Repository &repo, LexerPool &lexers,
         WorkerQueue<QPair<git::Commit, git::Diff>> &inQueue,
         WorkerQueue<Intermediate> &outQueue)
    : mLexers(lexers), mInQueue(inQueue), mOutQueue(outQueue) {
  git::Config config = repo.appConfig();
  mTermLimit = config.value<int>("index.termlimit", mTermLimit);
}

void Map::run() {
  std::optional<QPair<git::Commit, git::Diff>> commitOpt = mInQueue.dequeue();
  while (!canceled && commitOpt.has_value()) {
    auto commitPair = commitOpt.value();
    auto commit = commitPair.first;
    auto diff = commitPair.second;

    log("map: %1", commit.id());

    quint32 filePos = 0;
    quint32 hunkPos = 0;

    Intermediate result;
    result.id = commit.id();

    // Index id.
    result.fields[Index::Id][commit.id().toString().toUtf8()].append(0);

    // Index committer date.
    QDateTime time = commit.committer().date();
    QByteArray date = time.date().toString(Index::dateFormat()).toUtf8();
    result.fields[Index::Date][date].append(0);

    // Index author name and email.
    git::Signature author = commit.author();
    QByteArray email = author.email().toUtf8().toLower();
    result.fields[Index::Email][email].append(0);

    quint32 namePos = 0;
    for (const QString &name : author.name().split(kWsRe)) {
      QByteArray key = name.toUtf8().toLower();
      result.fields[Index::Author][key].append(namePos++);
    }

    // Index message.
    GenericLexer generic;
    quint32 messagePos = 0;
    generic.lex(commit.message().toUtf8());
    while (generic.hasNext())
      index(generic.next(), result.fields, Index::Message, messagePos);

    // Index diff.
    quint32 diffPos = 0;
    int patches = diff.count();
    for (int pidx = 0; pidx < patches; ++pidx) {
      // Truncate commits after term limit.
      if (canceled || diffPos > mTermLimit)
        break;

      // Skip binary deltas.
      if (diff.isBinary(pidx))
        continue;

      // Generate patch.
      git::Patch patch = diff.patch(pidx);
      if (!patch.isValid())
        continue;

      // Index file name and path.
      QFileInfo info(patch.name().toLower());
      log(QString("\tFilepath: %1").arg(info.filePath()));
      result.fields[Index::Path][info.filePath().toUtf8()].append(filePos);
      result.fields[Index::File][info.fileName().toUtf8()].append(filePos++);

      // Look up lexer.
      QByteArray name = Settings::instance()->lexer(patch.name()).toUtf8();
      Lexer *lexer = (name == "null") ? &generic : mLexers.acquire(name);

      int hunks = patch.count();
      for (int hidx = 0; hidx < hunks; ++hidx) {
        if (canceled || diffPos > mTermLimit)
          break;

        // Index hunk header.
        QByteArray header = patch.header(hidx);
        if (lexer->lex(header)) {
          while (lexer->hasNext())
            index(lexer->next(), result.fields, Index::Scope, hunkPos);
        }

        // Index content. Consecutive lines with the same origin are lexed
        // together, except with the generic lexer, where an unmatched quote
        // would drop the rest of the run.
        int lines = patch.lineCount(hidx);
        int line = 0;
        while (line < lines) {
          if (canceled || diffPos > mTermLimit)
            break;

          char origin = patch.lineOrigin(hidx, line);
          Index::Field field;
          switch (origin) {
            case GIT_DIFF_LINE_ADDITION:
              field = Index::Addition;
              break;
            case GIT_DIFF_LINE_DELETION:
              field = Index::Deletion;
              break;
            default:
              ++line;
              continue;
          }

          int first = line;
          QByteArray text = patch.lineContent(hidx, line++);
          while (lexer != &generic && line < lines &&
                 line - first < kMaxRunLines &&
                 patch.lineOrigin(hidx, line) == origin)
            text.append(patch.lineContent(hidx, line++));

          if (lexer->lex(text)) {
            while (!canceled && lexer->hasNext())
              index(lexer->next(), result.fields, field, diffPos);
          }
        }
      }

      // Return lexer to the pool.
      if (lexer != &generic)
        mLexers.release(lexer);
    }

    mOutQueue.enqueue(std::move(result));

    // Grab next value
    commitOpt = mInQueue.dequeue();
  }
}

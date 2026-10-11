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
         WorkerQueue<CommitDocument> &outQueue)
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

    CommitDocument result;
    result.id = commit.id();

    // Index committer date.
    git_time when = commit.committer().gitDate();
    result.time = when.time;
    result.tzOffset = when.offset * 60;

    // Index author name and email.
    git::Signature author = commit.author();
    QByteArray email = normaliseToken(author.email().toUtf8());
    result.email = email;

    for (const QString &name : author.name().split(kWsRe, Qt::SkipEmptyParts)) {
      result.append(FTSColumn::Author, normaliseToken(name.toUtf8()));
    }

    // Index message.
    GenericLexer generic;
    generic.lex(commit.message().toUtf8());
    while (generic.hasNext())
      index(generic.next(), result, Index::Message);

    // Index diff.
    int patches = diff.count();
    for (int pidx = 0; pidx < patches; ++pidx) {
      // Truncate commits after term limit.
      if (canceled || result.tokenCount > mTermLimit)
        break;

      // Index file name and path.
      auto filename = diff.name(pidx).toUtf8();
      log(QString("\tFilepath: %1").arg(filename));
      result.paths.append(normaliseToken(filename));

      // Skip binary deltas.
      if (diff.isBinary(pidx))
        continue;

      // Generate patch.
      git::Patch patch = diff.patch(pidx);
      if (!patch.isValid())
        continue;

      // Look up lexer.
      QByteArray name = Settings::instance()->lexer(patch.name()).toUtf8();
      Lexer *lexer = (name == "null") ? &generic : mLexers.acquire(name);

      int hunks = patch.count();
      for (int hidx = 0; hidx < hunks; ++hidx) {
        if (canceled || result.tokenCount > mTermLimit)
          break;

        // Index hunk header.
        QByteArray header = patch.header(hidx);
        if (lexer->lex(header)) {
          while (lexer->hasNext())
            index(lexer->next(), result, Index::Scope);
        }

        // Index content. Consecutive lines with the same origin are lexed
        // together, except with the generic lexer, where an unmatched quote
        // would drop the rest of the run.
        int lines = patch.lineCount(hidx);
        int line = 0;
        while (line < lines) {
          if (canceled || result.tokenCount > mTermLimit)
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
              index(lexer->next(), result, field);
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

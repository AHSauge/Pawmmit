#ifndef COMMON_H
#define COMMON_H

#include "git/Id.h"

#include <atomic>

class QFile;
class QString;

// global cancel flag
extern std::atomic_bool canceled;

void setLogFile(QFile *file);
void log(const QString &text);
void log(const QString &fmt, const git::Id &id);

#endif // COMMON_H

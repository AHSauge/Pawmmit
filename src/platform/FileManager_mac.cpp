//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "FileManager.h"
#include <QProcess>

namespace platform {

QString defaultFileManagerCommand() { return "open \"%1\""; }

bool revealInFileManager(const QString &file, const QString &command) {
  Q_UNUSED(command)
  return QProcess::startDetached("/usr/bin/open", {"-R", file});
}

} // namespace platform

//
//          Copyright (c) 2026
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Alf Henrik Sauge
//
// This file contains application wide constants

#include <cstdint>

/// @brief Number of bytes to read to determine if a file is binary or not
const std::size_t kMaxReadBinary = 64 * 1024;

/// @brief New (untracked) files larger than this are not rendered
/// automatically. Number of lines for such cases aren't readily available, so
/// instead we use the file size itself
const std::size_t kMaxAutoLoadDiffSize = 1024 * 1024; // 1 MiB

/// @brief Diffs that touch more than this many lines (additions + deletions)
/// are not rendered automatically, regardless of the file's total size. This
/// allows small changes in a large file to be visualised
const std::size_t kMaxAutoLoadDiffLines = 10000;

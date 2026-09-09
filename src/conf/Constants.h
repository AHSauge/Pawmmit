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

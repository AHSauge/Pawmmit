//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef LUALIBS_H
#define LUALIBS_H

struct lua_State;

// Open base, package, string, table and math, with require() limited to Lua
// files so that scripts can't load native code.
void openRestrictedLibs(lua_State *L);

// Open an io library limited to io.lines and a read-only io.open.
void openReadOnlyIo(lua_State *L);

#endif

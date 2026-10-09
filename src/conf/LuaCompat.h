//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef LUACOMPAT_H
#define LUACOMPAT_H

#include <cassert>

extern "C" {
#include <lua.h>
}

#if LUA_VERSION_NUM < 504
// Backport changes needed to support Lua 5.3

#define LUA_GNAME "_G"

inline void *lua_newuserdatauv(lua_State *L, size_t sz, int nuv) {
  assert(nuv == 0 && "user values not supported by this Lua version");
  return lua_newuserdata(L, sz);
}

#endif

#endif

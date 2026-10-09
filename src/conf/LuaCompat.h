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
#include <lauxlib.h>
#include <lualib.h>

#if LUA_VERSION_NUM < 504
// Backport changes needed to support Lua 5.3

#define LUA_GNAME "_G"

inline void *lua_newuserdatauv(lua_State *L, size_t sz, int nuv) {
  assert(nuv == 0 && "user values not supported by this Lua version");
  return lua_newuserdata(L, sz);
}

#endif

#if LUA_VERSION_NUM == 501
// Compatibility with LuaJIT
#include <luajit.h>
#include <compat-5.3.h>

#define LUA_LOADED_TABLE "_LOADED"
#define LUA_UTF8LIBNAME "utf8"
#define luaopen_utf8 luaopen_compat53_utf8

extern int luaopen_utf8(lua_State *L);

#endif
}
#endif

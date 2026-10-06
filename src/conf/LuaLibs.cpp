//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "LuaLibs.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

void openRestrictedLibs(lua_State *L) {
  luaL_requiref(L, LUA_GNAME, luaopen_base, 1);
  luaL_requiref(L, LUA_LOADLIBNAME, luaopen_package, 1);
  luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
  luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
  luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
  lua_pop(L, 5);

  // An empty cpath leaves the C library searchers nothing to find.
  lua_getglobal(L, LUA_LOADLIBNAME);
  lua_pushliteral(L, "");
  lua_setfield(L, -2, "cpath");
  lua_pushnil(L);
  lua_setfield(L, -2, "loadlib");
  lua_pop(L, 1); // package
}

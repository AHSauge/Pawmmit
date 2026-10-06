//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "LuaLibs.h"
#include <cstring>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace {

int ioOpen(lua_State *L) {
  luaL_checkstring(L, 1);
  const char *mode = luaL_optstring(L, 2, "r");
  if (strcmp(mode, "r") != 0 && strcmp(mode, "rb") != 0)
    return luaL_argerror(L, 2, "only read modes are allowed");

  int top = lua_gettop(L);
  lua_pushvalue(L, lua_upvalueindex(1)); // io.open
  lua_pushvalue(L, 1);
  lua_pushstring(L, mode);
  lua_call(L, 2, LUA_MULTRET);
  return lua_gettop(L) - top;
}

} // namespace

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

void openReadOnlyIo(lua_State *L) {
  luaL_requiref(L, LUA_IOLIBNAME, luaopen_io, 0);
  lua_createtable(L, 0, 2);
  lua_getfield(L, -2, "lines");
  lua_setfield(L, -2, "lines");
  lua_getfield(L, -2, "open");
  lua_pushcclosure(L, &ioOpen, 1);
  lua_setfield(L, -2, "open");

  // Replace the full library that require("io") would otherwise return.
  luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
  lua_pushvalue(L, -2);
  lua_setfield(L, -2, LUA_IOLIBNAME);
  lua_pop(L, 1); // loaded table

  lua_setglobal(L, LUA_IOLIBNAME);
  lua_pop(L, 1); // full library
}

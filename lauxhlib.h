/**
 *  Copyright (C) 2016 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 *
 *  lauxhlib.h
 *  lauxhlib - lua auxiliary header library
 *
 *  Created by Masatoshi Teruya on 16/02/28.
 *  Copyright 2016 Masatoshi Teruya. All rights reserved.
 *
 */

#ifndef lauxhlib_h
#define lauxhlib_h

#include <errno.h>
#include <fcntl.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* stringify */

static inline const char *lauxh_tolstr(lua_State *L, int idx, size_t *len)
{
    int type = 0;

    if (luaL_callmeta(L, idx, "__tostring")) {
        lua_replace(L, idx);
    }

    type = lua_type(L, idx);
    switch (type) {
    case LUA_TNUMBER:
    case LUA_TSTRING:
        lua_pushvalue(L, idx);
        break;

    case LUA_TNONE:
        lua_pushliteral(L, "");
        break;

    case LUA_TNIL:
        lua_pushliteral(L, "nil");
        break;

    case LUA_TBOOLEAN:
        if (lua_toboolean(L, idx)) {
            lua_pushliteral(L, "true");
        } else {
            lua_pushliteral(L, "false");
        }
        break;

    // case LUA_TTABLE:
    // case LUA_TFUNCTION:
    // case LUA_TTHREAD:
    // case LUA_TUSERDATA:
    // case LUA_TLIGHTUSERDATA:
    default: {
        char b[BUFSIZ];
        size_t n = snprintf(b, BUFSIZ, "%s: %p", lua_typename(L, type),
                            lua_topointer(L, idx));
        lua_pushlstring(L, b, n);
    } break;
    }

    return lua_tolstring(L, -1, len);
}

#define lauxh_tostr(L, idx)          lauxh_tolstr((L), (idx), NULL)
#define lauxh_tolstring(L, idx, len) lauxh_tolstr((L), (idx), (len))
#define lauxh_tostring(L, idx)       lauxh_tolstr((L), (idx), NULL)

/* reference */

static inline int lauxh_isref(int ref)
{
    return ref >= 0;
}

static inline int lauxh_ref(lua_State *L)
{
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

static inline int lauxh_refat(lua_State *L, int idx)
{
    lua_pushvalue(L, idx);
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

static inline void lauxh_pushref(lua_State *L, int ref)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

static inline int lauxh_unref(lua_State *L, int ref)
{
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    return LUA_NOREF;
}

/* table */

static inline void lauxh_gettblof(lua_State *L, const char *k, int idx)
{
    lua_pushstring(L, k);
    lua_rawget(L, idx);
}

static inline void lauxh_pushnil2tblat(lua_State *L, const char *k, int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushnil(L);
    lua_rawset(L, at);
}
#define lauxh_pushnil2tbl(L, k) lauxh_pushnil2tblat(L, k, -1)

static inline void lauxh_pushfn2tblat(lua_State *L, const char *k,
                                      lua_CFunction v, int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushcfunction(L, v);
    lua_rawset(L, at);
}
#define lauxh_pushfn2tbl(L, k, v) lauxh_pushfn2tblat(L, k, v, -1)

static inline void lauxh_pushstr2tblat(lua_State *L, const char *k,
                                       const char *v, int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushstring(L, v);
    lua_rawset(L, at);
}
#define lauxh_pushstr2tbl(L, k, v) lauxh_pushstr2tblat(L, k, v, -1)

static inline void lauxh_pushlstr2tblat(lua_State *L, const char *k,
                                        const char *v, size_t l, int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushlstring(L, v, l);
    lua_rawset(L, at);
}
#define lauxh_pushlstr2tbl(L, k, v, l) lauxh_pushlstr2tblat(L, k, v, l, -1)

static inline void lauxh_pushnum2tblat(lua_State *L, const char *k,
                                       lua_Number v, int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushnumber(L, v);
    lua_rawset(L, at);
}
#define lauxh_pushnum2tbl(L, k, v) lauxh_pushnum2tblat(L, k, v, -1)

static inline void lauxh_pushint2tblat(lua_State *L, const char *k,
                                       lua_Integer v, int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushinteger(L, v);
    lua_rawset(L, at);
}
#define lauxh_pushint2tbl(L, k, v) lauxh_pushint2tblat(L, k, v, -1)

static inline void lauxh_pushbool2tblat(lua_State *L, const char *k, int v,
                                        int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushboolean(L, v);
    lua_rawset(L, at);
}
#define lauxh_pushbool2tbl(L, k, v) lauxh_pushbool2tblat(L, k, v, -1)

/* table as array */

static inline void lauxh_gettblat(lua_State *L, int idx, int at)
{
    lua_rawgeti(L, at, idx);
}

static inline void lauxh_pushnil2arrat(lua_State *L, int idx, int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushnil(L);
    lua_rawseti(L, at, idx);
}
#define lauxh_pushnil2arr(L, idx) lauxh_pushnil2arrat(L, idx, -1)

static inline void lauxh_pushfn2arrat(lua_State *L, int idx, lua_CFunction v,
                                      int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushcfunction(L, v);
    lua_rawseti(L, at, idx);
}
#define lauxh_pushfn2arr(L, idx, v) lauxh_pushfn2arrat(L, idx, v, -1)

static inline void lauxh_pushstr2arrat(lua_State *L, int idx, const char *v,
                                       int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushstring(L, v);
    lua_rawseti(L, at, idx);
}
#define lauxh_pushstr2arr(L, idx, v) lauxh_pushstr2arrat(L, idx, v, -1)

static inline void lauxh_pushlstr2arrat(lua_State *L, int idx, const char *v,
                                        size_t l, int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushlstring(L, v, l);
    lua_rawseti(L, at, idx);
}
#define lauxh_pushlstr2arr(L, idx, v, l) lauxh_pushlstr2arrat(L, idx, v, l, -1)

static inline void lauxh_pushnum2arrat(lua_State *L, int idx, lua_Number v,
                                       int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushnumber(L, v);
    lua_rawseti(L, at, idx);
}
#define lauxh_pushnum2arr(L, idx, v) lauxh_pushnum2arrat(L, idx, v, -1)

static inline void lauxh_pushint2arrat(lua_State *L, int idx, lua_Integer v,
                                       int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushinteger(L, v);
    lua_rawseti(L, at, idx);
}
#define lauxh_pushint2arr(L, idx, v) lauxh_pushint2arrat(L, idx, v, -1)

static inline void lauxh_pushbool2arrat(lua_State *L, int idx, int v, int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushboolean(L, v);
    lua_rawseti(L, at, idx);
}
#define lauxh_pushbool2arr(L, idx, v) lauxh_pushbool2arrat(L, idx, v, -1)

#if LUA_VERSION_NUM >= 502
# define lauxh_rawlen(L, idx) lua_rawlen(L, idx)
#else
# define lauxh_rawlen(L, idx) lua_objlen(L, idx)
#endif

// get global
static inline void lauxh_getglobal(lua_State *L, const char *k)
{
#if defined(LUA_RIDX_GLOBALS)
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    lua_getfield(L, -1, k);

#else
    lua_getfield(L, LUA_GLOBALSINDEX, k);

#endif
}

/* metatable */

static inline void lauxh_setmetatable(lua_State *L, const char *tname)
{
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
}

static inline int lauxh_ismetatableof(lua_State *L, int idx, const char *tname)
{
    int rc = 0;

    // idx value is wrapped by metatable
    if (lua_getmetatable(L, idx)) {
        // get metatable from registry
        lua_pushstring(L, tname);
        lua_rawget(L, LUA_REGISTRYINDEX);
        // compare
        rc = lua_rawequal(L, -1, -2);
        lua_pop(L, 2);
    }

    return rc;
}

/* value check */

#if LUA_VERSION_NUM >= 502
# define lauxh_equal(L, idx1, idx2) lua_compare(L, idx1, idx2, LUA_OPEQ)
#else
# define lauxh_equal(L, idx1, idx2) lua_equal(L, idx1, idx2)
#endif

/* typecheck */

static inline int lauxh_isnil(lua_State *L, int idx)
{
    return lua_type(L, idx) <= LUA_TNIL;
}

static inline int lauxh_isstr(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TSTRING;
}
#define lauxh_isstring(L, idx) lauxh_isstr((L), (idx))

static inline int lauxh_isnum(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TNUMBER;
}
#define lauxh_isnumber(L, idx) lauxh_isnum((L), idx)

static inline int lauxh_isbool(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TBOOLEAN;
}
#define lauxh_isboolean(L, idx) lauxh_isbool((L), (idx))

static inline int lauxh_istable(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TTABLE;
}

static inline int lauxh_isfunc(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TFUNCTION;
}
#define lauxh_isfunction(L, idx) lauxh_isfunc((L), (idx))

static inline int lauxh_iscfunc(lua_State *L, int idx)
{
    return lua_iscfunction(L, idx);
}
#define lauxh_iscfunction(L, idx) lauxh_iscfunc((L), (idx))

static inline int lauxh_isthread(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TTHREAD;
}

static inline int lauxh_isuserdata(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TUSERDATA;
}

static inline int lauxh_ispointer(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TLIGHTUSERDATA;
}

static inline int lauxh_isint(lua_State *L, int idx)
{
#if LUA_VERSION_NUM >= 503
    return lua_isinteger(L, idx);
#else
    return lauxh_isnum(L, idx) &&
           (lua_Number)lua_tointeger(L, idx) == lua_tonumber(L, idx);
#endif
}
#define lauxh_isinteger(L, idx) lauxh_isint((L), (idx))

static inline int lauxh_isfile(lua_State *L, int idx)
{
    return lauxh_ismetatableof(L, idx, LUA_FILEHANDLE);
}

/* check argument */
static inline int lauxh_argerror(lua_State *L, int idx, const char *fmt, ...)
{
    char buf[255];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, 255, fmt, ap);
    va_end(ap);

    if (idx < 0) {
        idx = lua_gettop(L) + idx + 1;
    }

    return luaL_argerror(L, idx, buf);
}

#define lauxh_argcheck(L, cond, idx, fmt, ...)                                 \
 do {                                                                          \
  if (!(cond)) {                                                               \
   lauxh_argerror((L), (idx), (fmt), ##__VA_ARGS__);                           \
  }                                                                            \
 } while (0)

/* string argument */

static inline const char *lauxh_checklstr(lua_State *L, int idx, size_t *len)
{
    luaL_checktype(L, idx, LUA_TSTRING);
    return lua_tolstring(L, idx, len);
}
#define lauxh_checklstring(L, idx, len) lauxh_checklstr((L), (idx), (len))

static inline const char *lauxh_optlstr(lua_State *L, int idx, const char *def,
                                        size_t *len)
{
    if (lauxh_isnil(L, idx)) {
        if (len && def) {
            *len = strlen(def);
        }
        return def;
    }
    return lauxh_checklstr(L, idx, len);
}
#define lauxh_optlstring(L, idx, def, len)                                     \
 lauxh_optlstr((L), (idx), (def), (len))

static inline const char *lauxh_checkstr(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TSTRING);
    return lua_tostring(L, idx);
}
#define lauxh_checkstring(L, idx) lauxh_checkstr((L), (idx))

static inline const char *lauxh_optstr(lua_State *L, int idx, const char *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkstr(L, idx);
}
#define lauxh_optstring(L, idx, def) lauxh_optstr((L), (idx), (def))

/* number/integer argument */

static inline lua_Number lauxh_checknum(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TNUMBER);
    return lua_tonumber(L, idx);
}
#define lauxh_checknumber(L, idx) lauxh_checknum((L), (idx))

static inline lua_Number lauxh_optnum(lua_State *L, int idx, lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum(L, idx);
}
#define lauxh_optnumber(L, idx, def) lauxh_optnum((L), (idx), (def))

static inline lua_Number lauxh_checkunsigned(lua_State *L, int idx)
{
    lua_Number v = 0;

    lauxh_argcheck(L, lauxh_isnum(L, idx), idx, "unsigned expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tonumber(L, idx);
    lauxh_argcheck(L, v >= 0, idx,
                   "unsigned expected, got an out of range value");

    return v;
}

static inline lua_Number lauxh_optunsigned(lua_State *L, int idx,
                                           lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkunsigned(L, idx);
}

static inline lua_Integer lauxh_checkinteger(lua_State *L, int idx)
{
    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "integer expected, got %s",
                   luaL_typename(L, idx));
    return lua_tointeger(L, idx);
}

static inline lua_Integer lauxh_optinteger(lua_State *L, int idx,
                                           lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkinteger(L, idx);
}

static inline lua_Integer lauxh_checkuinteger(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx,
                   "unsigned integer expected, got %s", luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= 0, idx,
                   "unsigned integer expected, got an out of range value");

    return v;
}

static inline lua_Integer lauxh_optuinteger(lua_State *L, int idx,
                                            lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuinteger(L, idx);
}

static inline lua_Integer lauxh_checkpinteger(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx,
                   "positive integer expected, got %s", luaL_typename(L, idx));
    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v > 0, idx, "positive integer expected, got %d", v);
    return v;
}

static inline lua_Integer lauxh_optpinteger(lua_State *L, int idx,
                                            lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkpinteger(L, idx);
}

static inline int8_t lauxh_checkint8(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "int8_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= INT8_MIN && v <= INT8_MAX, idx,
                   "int8_t expected, got an out of range value");

    return (uint8_t)v;
}

static inline int8_t lauxh_optint8(lua_State *L, int idx, int8_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint8(L, idx);
}

static inline uint8_t lauxh_checkuint8(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "uint8_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= 0 && v <= UINT8_MAX, idx,
                   "uint8_t expected, got an out of range value");

    return (uint8_t)v;
}

static inline uint8_t lauxh_optuint8(lua_State *L, int idx, uint8_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint8(L, idx);
}

static inline int16_t lauxh_checkint16(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "int16_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= INT16_MIN && v <= INT16_MAX, idx,
                   "int16_t expected, got an out of range value");

    return (int16_t)v;
}

static inline int16_t lauxh_optint16(lua_State *L, int idx, int16_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint16(L, idx);
}

static inline uint16_t lauxh_checkuint16(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "uint16_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= 0 && v <= UINT16_MAX, idx,
                   "uint16_t expected, got an out of range value");

    return (uint16_t)v;
}

static inline uint16_t lauxh_optuint16(lua_State *L, int idx, uint16_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint16(L, idx);
}

static inline int32_t lauxh_checkint32(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "int32_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= INT32_MIN && v <= INT32_MAX, idx,
                   "int32_t expected, got an out of range value");

    return (int32_t)v;
}

static inline int32_t lauxh_optint32(lua_State *L, int idx, int32_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint32(L, idx);
}

static inline uint32_t lauxh_checkuint32(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "uint32_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= 0 && v <= UINT32_MAX, idx,
                   "uint32_t expected, got an out of range value");

    return (uint32_t)v;
}

static inline uint32_t lauxh_optuint32(lua_State *L, int idx, uint32_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint32(L, idx);
}

static inline int64_t lauxh_checkint64(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "int64_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= INT64_MIN && v <= INT64_MAX, idx,
                   "int64_t expected, got an out of range value");

    return (int64_t)v;
}

static inline int64_t lauxh_optint64(lua_State *L, int idx, int64_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint64(L, idx);
}

static inline uint64_t lauxh_checkuint64(lua_State *L, int idx)
{
    lua_Integer v = 0;

    lauxh_argcheck(L, lauxh_isint(L, idx), idx, "uint64_t expected, got %s",
                   luaL_typename(L, idx));

    v = lua_tointeger(L, idx);
    lauxh_argcheck(L, v >= 0 && (uint64_t)v <= UINT64_MAX, idx,
                   "uint64_t expected, got an out of range value");

    return (uint64_t)v;
}

static inline uint64_t lauxh_optuint64(lua_State *L, int idx, uint64_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint64(L, idx);
}

/* boolean argument */

static inline int lauxh_checkboolean(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TBOOLEAN);
    return lua_toboolean(L, idx);
}

static inline int lauxh_optboolean(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkboolean(L, idx);
}

/* table argument */

static inline void lauxh_checktable(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TTABLE);
}

static inline void lauxh_checktableof(lua_State *L, int idx, const char *k)
{
    lua_pushstring(L, k);
    lua_rawget(L, idx);
    lauxh_checktable(L, -1);
}

static inline const char *lauxh_checklstringof(lua_State *L, int idx,
                                               const char *k, size_t *len)
{
    const char *v = NULL;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_checklstr(L, -1, len);
    lua_pop(L, 1);

    return v;
}

static inline const char *lauxh_optlstringof(lua_State *L, int idx,
                                             const char *k, const char *def,
                                             size_t *len)
{
    const char *v = NULL;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_optlstr(L, -1, def, len);
    lua_pop(L, 1);

    return v;
}

static inline const char *lauxh_checkstringof(lua_State *L, int idx,
                                              const char *k)
{
    const char *v = NULL;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_checkstr(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline const char *lauxh_optstringof(lua_State *L, int idx,
                                            const char *k, const char *def)
{
    const char *v = NULL;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_optstr(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline lua_Number lauxh_checknumberof(lua_State *L, int idx,
                                             const char *k)
{
    lua_Number v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_checknum(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline lua_Number lauxh_optnumberof(lua_State *L, int idx, const char *k,
                                           lua_Number def)
{
    lua_Number v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_optnum(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline lua_Integer lauxh_checkintegerof(lua_State *L, int idx,
                                               const char *k)
{
    lua_Integer v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_checkinteger(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline lua_Integer lauxh_optintegerof(lua_State *L, int idx,
                                             const char *k, lua_Integer def)
{
    lua_Integer v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_optinteger(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_checkbooleanof(lua_State *L, int idx, const char *k)
{
    int v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_checkboolean(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_optbooleanof(lua_State *L, int idx, const char *k,
                                     int def)
{
    int v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_optboolean(L, -1, def);
    lua_pop(L, 1);

    return v;
}

// table as array

static inline void lauxh_checktableat(lua_State *L, int idx, int row)
{
    lua_rawgeti(L, idx, row);
    lauxh_checktable(L, -1);
}

static inline const char *lauxh_checklstringat(lua_State *L, int idx, int row,
                                               size_t *len)
{
    const char *v = NULL;

    lua_rawgeti(L, idx, row);
    v = lauxh_checklstr(L, -1, len);
    lua_pop(L, 1);

    return v;
}

static inline const char *lauxh_optlstringat(lua_State *L, int idx, int row,
                                             const char *def, size_t *len)
{
    const char *v = NULL;

    lua_rawgeti(L, idx, row);
    v = lauxh_optlstr(L, -1, def, len);
    lua_pop(L, 1);

    return v;
}

static inline const char *lauxh_checkstringat(lua_State *L, int idx, int row)
{
    const char *v = NULL;

    lua_rawgeti(L, idx, row);
    v = lauxh_checkstr(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline const char *lauxh_optstringat(lua_State *L, int idx, int row,
                                            const char *def)
{
    const char *v = NULL;

    lua_rawgeti(L, idx, row);
    v = lauxh_optstr(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline lua_Number lauxh_checknumberat(lua_State *L, int idx, int row)
{
    lua_Number v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_checknum(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline lua_Number lauxh_optnumberat(lua_State *L, int idx, int row,
                                           lua_Number def)
{
    lua_Number v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_optnum(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline lua_Integer lauxh_checkintegerat(lua_State *L, int idx, int row)
{
    lua_Integer v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_checkinteger(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline lua_Integer lauxh_optintegerat(lua_State *L, int idx, int row,
                                             lua_Integer def)
{
    lua_Integer v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_optinteger(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_checkbooleanat(lua_State *L, int idx, int row)
{
    int v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_checkboolean(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_optbooleanat(lua_State *L, int idx, int row, int def)
{
    int v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_optboolean(L, -1, def);
    lua_pop(L, 1);

    return v;
}

/* thread argument */

static inline lua_State *lauxh_checkthread(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TTHREAD);
    return lua_tothread(L, idx);
}

/* function argument */

static inline void lauxh_checkfunction(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TFUNCTION);
}

/* cfunction argument */

static inline lua_CFunction lauxh_checkcfunction(lua_State *L, int idx)
{
    lauxh_argcheck(L, lua_iscfunction(L, idx), idx,
                   "cfunction expected, got %s", luaL_typename(L, idx));

    return lua_tocfunction(L, idx);
}

/* lightuserdata argument */

static inline const void *lauxh_checkpointer(lua_State *L, int idx)
{
    luaL_checktype(L, idx, LUA_TLIGHTUSERDATA);
    return lua_topointer(L, idx);
}

/* userdata argument */

static inline void *lauxh_checkudata(lua_State *L, int idx, const char *tname)
{
    return luaL_checkudata(L, idx, tname);
}

static inline void *lauxh_optudata(lua_State *L, int idx, const char *tname,
                                   void *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }

    return lauxh_checkudata(L, idx, tname);
}

static inline int lauxh_isuserdataof(lua_State *L, int idx, const char *tname)
{
    // there is userdata wrapped by metatable
    return lauxh_isuserdata(L, idx) && lauxh_ismetatableof(L, idx, tname);
}

/* file argument */

static inline FILE **lauxh_checkfilep(lua_State *L, int idx)
{
#if LUA_VERSION_NUM >= 502
    luaL_Stream *stream = luaL_checkudata(L, idx, LUA_FILEHANDLE);
    return &stream->f;

#else
    return (FILE **)luaL_checkudata(L, idx, LUA_FILEHANDLE);
#endif
}

static inline FILE *lauxh_checkfile(lua_State *L, int idx)
{
    return *lauxh_checkfilep(L, idx);
}

static inline int lauxh_fileno(lua_State *L, int idx)
{
    FILE *f = lauxh_checkfile(L, idx);
    if (f) {
        return fileno(f);
    }
    return -1;
}

static inline FILE *lauxh_tofile(lua_State *L, int fd, const char *mode,
                                 const char *fname)
{
    int top    = lua_gettop(L);
    FILE *fp   = NULL;
    FILE **lfp = NULL;
    int narg   = 0;

    // push a function to create a file handle
    if (fname) {
        static int tofile_open_ref = LUA_NOREF;

        if (tofile_open_ref == LUA_NOREF) {
            luaL_loadstring(L, "return io.open(...)");
            tofile_open_ref = lauxh_ref(L);
        }
        lauxh_pushref(L, tofile_open_ref);
        lua_pushstring(L, fname);
        lua_pushstring(L, mode);
        narg = 2;
    } else {
        static int tofile_tmpfile_ref = LUA_NOREF;

        if (tofile_tmpfile_ref == LUA_NOREF) {
            luaL_loadstring(L, "return io.tmpfile()");
            tofile_tmpfile_ref = lauxh_ref(L);
        }
        lauxh_pushref(L, tofile_tmpfile_ref);
    }

    // call function
    if (lua_pcall(L, narg, LUA_MULTRET, 0) != 0) {
        // runtime error
        lua_pushnil(L);
        lua_insert(L, top + 1);
        return NULL;
    } else if (lua_gettop(L) - top != 1) {
        // got error
        return NULL;
    }

    // create a new stream associated with the existing file descriptor
    if ((fp = fdopen(fd, mode)) == NULL) {
        lua_settop(L, top);
        lua_pushnil(L);
        if (fname) {
            lua_pushfstring(L, "%s: %s", fname, strerror(errno));
        } else {
            lua_pushstring(L, strerror(errno));
        }
        lua_pushinteger(L, errno);
        return NULL;
    }

    // replace a stream of a file handle with a new stream
    lfp = lauxh_checkfilep(L, -1);
    fclose(*lfp);
    *lfp = fp;

    return fp;
}

/* flag arguments */

static inline lua_Integer lauxh_optflags(lua_State *L, int idx)
{
    const int argc  = lua_gettop(L);
    lua_Integer flg = 0;

    for (; idx <= argc; idx++) {
        flg |= lauxh_optinteger(L, idx, 0);
    }

    return flg;
}

/* traceback */

static inline void lauxh_traceback(lua_State *L, lua_State *src,
                                   const char *msg, int lv)
{
#if LUA_VERSION_NUM >= 502
    // push src stack trace to dst state
    luaL_traceback((L) ? L : src, src, msg, lv);

#else
    lua_Debug ar;
    luaL_Buffer b;

    luaL_buffinit(src, &b);
    if (msg) {
        luaL_addstring(&b, msg);
        luaL_addchar(&b, '\n');
    }

    // get stackinfo
    luaL_addstring(&b, "stack traceback:");
    while (lua_getstack(src, lv++, &ar)) {
        lua_getinfo(src, "Sln", &ar);
        if (ar.currentline <= 0) {
            lua_pushfstring(src, "\n\t%s: in ", ar.short_src);
        } else {
            lua_pushfstring(src, "\n\t%s:%d: in ", ar.short_src,
                            ar.currentline);
        }
        luaL_addvalue(&b);

        // is there a name from code?
        if (*ar.namewhat != '\0') {
            lua_pushfstring(src, "%s '%s'", ar.namewhat, ar.name);
        }
        // main
        else if (*ar.what == 'm') {
            lua_pushliteral(src, "main chunk");
        }
        // not C function, use <file:line>
        else if (*ar.what == 'C') {
            lua_pushliteral(src, "?");
        } else {
            lua_pushfstring(src, "function <%s:%d>", ar.short_src,
                            ar.linedefined);
        }
        luaL_addvalue(&b);

        if (*ar.what == 't') {
            luaL_addstring(&b, "\n\t(...tail calls...)");
        }
    }

    luaL_pushresult(&b);
    if (L) {
        lua_xmove(src, L, 1);
    }
#endif
}

/* copy a value to different state */

static inline int lauxh_xcopy(lua_State *from, lua_State *to, int idx,
                              const int allow_nil)
{
    switch (lua_type(from, idx)) {
    case LUA_TBOOLEAN:
        lua_pushboolean(to, lua_toboolean(from, idx));
        return LUA_TBOOLEAN;

    case LUA_TLIGHTUSERDATA:
        lua_pushlightuserdata(to, lua_touserdata(from, idx));
        return LUA_TLIGHTUSERDATA;

    case LUA_TNUMBER:
        lua_pushnumber(to, lua_tonumber(from, idx));
        return LUA_TNUMBER;

    case LUA_TSTRING: {
        size_t len      = 0;
        const char *str = lua_tolstring(from, idx, &len);
        lua_pushlstring(to, str, len);
        return LUA_TSTRING;
    }

    case LUA_TTABLE:
        lua_newtable(to);
        // to positive number
        if (idx < 0) {
            idx = lua_gettop(from) + idx + 1;
        }
        lua_pushnil(from);
        while (lua_next(from, idx)) {
            if (lauxh_xcopy(from, to, -2, 0) != LUA_TNONE) {
                if (lauxh_xcopy(from, to, -1, 0) != LUA_TNONE) {
                    lua_rawset(to, -3);
                } else {
                    lua_pop(to, 1);
                }
            }
            lua_pop(from, 1);
        }
        return LUA_TTABLE;

    case LUA_TNIL:
        if (allow_nil) {
            lua_pushnil(to);
            return LUA_TNIL;
        }

    // ignore unsupported values
    // LUA_TNONE
    // LUA_TFUNCTION
    // LUA_TUSERDATA
    // LUA_TTHREAD
    default:
        return LUA_TNONE;
    }
}

#if LUA_VERSION_NUM >= 504
static inline int lauxh_resume(lua_State *L, lua_State *from, int narg)
{
    int nres = 0;
    return lua_resume(L, from, narg, &nres);
}

#elif LUA_VERSION_NUM >= 502
# define lauxh_resume(L, from, narg) lua_resume(L, from, narg)
#else
# define lauxh_resume(L, from, narg) lua_resume(L, narg)
#endif

#endif

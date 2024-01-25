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
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* stringify */

static inline const char *lauxh_tolstr(lua_State *L, int idx, size_t *len)
{
    int type = lua_type(L, idx);

    if (luaL_callmeta(L, idx, "__tostring")) {
        if (!lua_isstring(L, -1)) {
            luaL_error(L, "\"__tostring\" metamethod must return a string");
        }
    } else {
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
        default:
            lua_pushfstring(L, "%s: %p", lua_typename(L, type),
                            lua_topointer(L, idx));
        }
    }

    return lua_tolstring(L, -1, len);
}

#define lauxh_tostr(L, idx)          lauxh_tolstr((L), (idx), NULL)
#define lauxh_tolstring(L, idx, len) lauxh_tolstr((L), (idx), (len))
#define lauxh_tostring(L, idx)       lauxh_tolstr((L), (idx), NULL)

/* reference */

static inline int lauxh_isref(int ref)
{
    return ref != LUA_NOREF && ref != LUA_REFNIL;
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
    return lua_isnoneornil(L, idx);
}

static inline int lauxh_isstr(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TSTRING;
}
#define lauxh_isstring(L, idx) lauxh_isstr((L), (idx))

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

static inline int lauxh_isfile(lua_State *L, int idx)
{
    return lauxh_ismetatableof(L, idx, LUA_FILEHANDLE);
}

static inline int lauxh_iscallable(lua_State *L, int idx)
{
    int t = LUA_TNIL;

    if (lauxh_isfunc(L, idx)) {
        return 1;
    } else if (lua_getmetatable(L, idx)) {
        lua_pushliteral(L, "__call");
        lua_rawget(L, -2);
        t = lua_type(L, -1);
        lua_pop(L, 2);
    }
    return t == LUA_TFUNCTION;
}

static inline int lauxh_isnum(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TNUMBER;
}
#define lauxh_isnumber(L, idx) lauxh_isnum((L), idx)

static inline int lauxh_isnum_ge(lua_State *L, int idx, lua_Number n)
{
    if (lauxh_isnum(L, idx)) {
        return lua_tonumber(L, idx) >= n;
    }
    return 0;
}

static inline int lauxh_isnum_le(lua_State *L, int idx, lua_Number n)
{
    if (lauxh_isnum(L, idx)) {
        return lua_tonumber(L, idx) <= n;
    }
    return 0;
}

static inline int lauxh_isnum_in_range(lua_State *L, int idx, lua_Number min,
                                       lua_Number max)
{
    if (lauxh_isnum(L, idx)) {
        lua_Number v = lua_tonumber(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

static inline lua_Number lauxh_isfinite(lua_State *L, int idx)
{
    if (lauxh_isnum(L, idx)) {
        return isfinite(lua_tonumber(L, idx));
    }
    return 0;
}

static inline int lauxh_isfinite_ge(lua_State *L, int idx, lua_Number n)
{
    if (lauxh_isfinite(L, idx)) {
        return lua_tonumber(L, idx) >= n;
    }
    return 0;
}

static inline int lauxh_isfinite_le(lua_State *L, int idx, lua_Number n)
{
    if (lauxh_isfinite(L, idx)) {
        return lua_tonumber(L, idx) <= n;
    }
    return 0;
}

static inline int lauxh_isfinite_in_range(lua_State *L, int idx, lua_Number min,
                                          lua_Number max)
{
    if (lauxh_isfinite(L, idx)) {
        lua_Number v = lua_tonumber(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

static inline lua_Number lauxh_isunsigned(lua_State *L, int idx)
{
    if (lauxh_isnum(L, idx)) {
        return lua_tonumber(L, idx) >= 0;
    }
    return 0;
}

static inline int lauxh_isunsigned_ge(lua_State *L, int idx, lua_Number n)
{
    if (lauxh_isunsigned(L, idx)) {
        return lua_tonumber(L, idx) >= n;
    }
    return 0;
}

static inline int lauxh_isunsigned_le(lua_State *L, int idx, lua_Number n)
{
    if (lauxh_isunsigned(L, idx)) {
        return lua_tonumber(L, idx) <= n;
    }
    return 0;
}

static inline int lauxh_isunsigned_in_range(lua_State *L, int idx,
                                            lua_Number min, lua_Number max)
{
    if (lauxh_isunsigned(L, idx)) {
        lua_Number v = lua_tonumber(L, idx);
        return v >= min && v <= max;
    }
    return 0;
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

static inline int lauxh_isint_ge(lua_State *L, int idx, lua_Integer n)
{
    if (lauxh_isint(L, idx)) {
        return lua_tointeger(L, idx) >= n;
    }
    return 0;
}

static inline int lauxh_isint_le(lua_State *L, int idx, lua_Integer n)
{
    if (lauxh_isint(L, idx)) {
        return lua_tointeger(L, idx) <= n;
    }
    return 0;
}

static inline int lauxh_isint_in_range(lua_State *L, int idx, int64_t min,
                                       int64_t max)
{
    if (lauxh_isint(L, idx)) {
        int64_t v = (int64_t)lua_tointeger(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

#define lauxh_isint8(L, idx)                                                   \
    lauxh_isint_in_range((L), (idx), INT8_MIN, INT8_MAX)
#define lauxh_isint16(L, idx)                                                  \
    lauxh_isint_in_range((L), (idx), INT16_MIN, INT16_MAX)
#define lauxh_isint32(L, idx)                                                  \
    lauxh_isint_in_range((L), (idx), INT32_MIN, INT32_MAX)
#define lauxh_isint64(L, idx)                                                  \
    lauxh_isint_in_range((L), (idx), INT64_MIN, INT64_MAX)

static inline lua_Integer lauxh_isuint(lua_State *L, int idx)
{
    if (lauxh_isint(L, idx)) {
        return lua_tointeger(L, idx) >= 0;
    }
    return 0;
}

static inline int lauxh_isuint_ge(lua_State *L, int idx, uint64_t n)
{
    if (lauxh_isuint(L, idx)) {
        return (uint64_t)lua_tointeger(L, idx) >= n;
    }
    return 0;
}

static inline int lauxh_isuint_le(lua_State *L, int idx, uint64_t n)
{
    if (lauxh_isuint(L, idx)) {
        return (uint64_t)lua_tointeger(L, idx) <= n;
    }
    return 0;
}

static inline int lauxh_isuint_in_range(lua_State *L, int idx, uint64_t min,
                                        uint64_t max)
{
    if (lauxh_isuint(L, idx)) {
        uint64_t v = (uint64_t)lua_tointeger(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

#define lauxh_isuint8(L, idx)  lauxh_isuint_in_range((L), (idx), 0, UINT8_MAX)
#define lauxh_isuint16(L, idx) lauxh_isuint_in_range((L), (idx), 0, UINT16_MAX)
#define lauxh_isuint32(L, idx) lauxh_isuint_in_range((L), (idx), 0, UINT32_MAX)
#define lauxh_isuint64(L, idx) lauxh_isuint_in_range((L), (idx), 0, UINT64_MAX)

static inline lua_Integer lauxh_ispint(lua_State *L, int idx)
{
    if (lauxh_isint(L, idx)) {
        return lua_tointeger(L, idx) > 0;
    }
    return 0;
}

static inline int lauxh_ispint_ge(lua_State *L, int idx, uint64_t n)
{
    if (lauxh_ispint(L, idx)) {
        return (uint64_t)lua_tointeger(L, idx) >= n;
    }
    return 0;
}

static inline int lauxh_ispint_le(lua_State *L, int idx, uint64_t n)
{
    if (lauxh_ispint(L, idx)) {
        return (uint64_t)lua_tointeger(L, idx) <= n;
    }
    return 0;
}

static inline int lauxh_ispint_in_range(lua_State *L, int idx, uint64_t min,
                                        uint64_t max)
{
    if (lauxh_ispint(L, idx)) {
        uint64_t v = (uint64_t)lua_tointeger(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

#define lauxh_ispint8(L, idx)  lauxh_ispint_in_range((L), (idx), 0, UINT8_MAX)
#define lauxh_ispint16(L, idx) lauxh_ispint_in_range((L), (idx), 0, UINT16_MAX)
#define lauxh_ispint32(L, idx) lauxh_ispint_in_range((L), (idx), 0, UINT32_MAX)
#define lauxh_ispint64(L, idx) lauxh_ispint_in_range((L), (idx), 0, UINT64_MAX)

/* check argument */
#if defined(LAUXHLIB_USED_IN_LUA)

static const char *LAUXH_ARGERR_NAME = NULL;
static int LAUXH_ARGERR_INDEX        = 0;
static int LAUXH_ARGERR_STACK        = 1;

# define lauxh_push_argerror_init()                                            \
     do {                                                                      \
         LAUXH_ARGERR_NAME  = NULL;                                            \
         LAUXH_ARGERR_INDEX = 0;                                               \
         LAUXH_ARGERR_STACK = 1;                                               \
     } while (0)

static inline int lauxh_push_argerror(lua_State *L, int arg, const char *extra)
{
    //
    // NOTE: porting from lua source code
    // https://github.com/lua/lua/blob/master/lauxlib.c
    //
    lua_Debug ar     = {0};
    int stack        = LAUXH_ARGERR_STACK;
    const char *name = NULL;

    LAUXH_ARGERR_STACK = 1;
    if (LAUXH_ARGERR_NAME) {
        name = lua_pushfstring(L, "'%s'", LAUXH_ARGERR_NAME);
    } else if (LAUXH_ARGERR_INDEX > 0) {
        name = lua_pushfstring(L, "#%d", LAUXH_ARGERR_INDEX);
    } else {
        name = lua_pushfstring(L, "#%d", arg);
    }
    LAUXH_ARGERR_NAME  = NULL;
    LAUXH_ARGERR_INDEX = 0;

    if (!lua_getstack(L, stack, &ar)) { // no stack frame?
        return luaL_error(L, "bad argument %s (%s)", name, extra);
    }

    lua_getinfo(L, "n", &ar);
    return luaL_error(L, "bad argument %s to '%s' (%s)", name,
                      (ar.name == NULL) ? "?" : ar.name, extra);
}

static inline void lauxh_checktype(lua_State *L, int arg, int t)
{
    //
    // NOTE: porting from lua source code
    // https://github.com/lua/lua/blob/master/lauxlib.c
    //
    if (lua_type(L, arg) != t) {
        const char *tname    = lua_typename(L, t);
        const char *extramsg = NULL;
        const char *typearg; // name for the type of the actual argument

        if (luaL_getmetafield(L, arg, "__name") == LUA_TSTRING) {
            typearg = lua_tostring(L, -1); // use the given type name
        } else if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
            typearg = "light userdata"; // special name for messages
        } else {
            typearg = luaL_typename(L, arg); // standard name
        }

        extramsg = lua_pushfstring(L, "%s expected, got %s", tname, typearg);
        lauxh_push_argerror(L, arg, extramsg);
    }
    lauxh_push_argerror_init();
}

#else

# define lauxh_push_argerror_init()
# define lauxh_push_argerror luaL_argerror
# define lauxh_checktype     luaL_checktype

#endif

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

    return lauxh_push_argerror(L, idx, buf);
}

#define lauxh_argcheck(L, cond, idx, fmt, ...)                                 \
    do {                                                                       \
        if (!(cond)) {                                                         \
            lauxh_argerror((L), (idx), (fmt), ##__VA_ARGS__);                  \
        }                                                                      \
        lauxh_push_argerror_init();                                            \
    } while (0)

/* any argument */

static inline void lauxh_checkcallable(lua_State *L, int idx)
{
    lauxh_argcheck(L, lauxh_iscallable(L, idx), idx,
                   "callable object expected, got %s", luaL_typename(L, idx));
}

static inline int lauxh_optcallable(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    lauxh_checkcallable(L, idx);
    return idx;
}

/* string argument */

static inline const char *lauxh_checklstr(lua_State *L, int idx, size_t *len)
{
    lauxh_checktype(L, idx, LUA_TSTRING);
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
    lauxh_checktype(L, idx, LUA_TSTRING);
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

#define CHECK_NUMTYPE(L, idx, tname, isnumfn)                                  \
    do {                                                                       \
        if (!isnumfn((L), (idx))) {                                            \
            int t = lua_type((L), (idx));                                      \
            if (t != LUA_TNUMBER) {                                            \
                lauxh_argerror((L), (idx), tname " expected, got %s",          \
                               lua_typename((L), t));                          \
            }                                                                  \
            lauxh_argerror((L), (idx),                                         \
                           tname " expected, got an out of range value");      \
        }                                                                      \
        lauxh_push_argerror_init();                                            \
    } while (0)

static inline lua_Number lauxh_checknum(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "number", lauxh_isnum);
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

static inline lua_Integer lauxh_checkint(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "integer", lauxh_isint);
    return lua_tointeger(L, idx);
}
#define lauxh_checkinteger(L, idx) lauxh_checkint((L), (idx))

static inline lua_Integer lauxh_optint(lua_State *L, int idx, lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint(L, idx);
}
#define lauxh_optinteger(L, idx, def) lauxh_optint((L), (idx), (def))

static inline lua_Number lauxh_checkfinite(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "finite number", lauxh_isfinite);
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_optfinite(lua_State *L, int idx, lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfinite(L, idx);
}

static inline lua_Number lauxh_checkunsigned(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "unsigned number", lauxh_isunsigned);
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_optunsigned(lua_State *L, int idx,
                                           lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkunsigned(L, idx);
}

static inline lua_Integer lauxh_checkuint(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "unsigned integer", lauxh_isuint);
    return lua_tointeger(L, idx);
}
#define lauxh_checkuinteger(L, idx) lauxh_checkuint((L), (idx))

static inline lua_Integer lauxh_optuint(lua_State *L, int idx, lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint(L, idx);
}
#define lauxh_optuinteger(L, idx, def) lauxh_optuint((L), (idx), (def))

static inline lua_Integer lauxh_checkpint(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "positive integer", lauxh_ispint);
    return lua_tointeger(L, idx);
}
#define lauxh_checkpinteger(L, idx) lauxh_checkpint((L), (idx))

static inline lua_Integer lauxh_optpint(lua_State *L, int idx, lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkpint(L, idx);
}
#define lauxh_optpinteger(L, idx, def) lauxh_optpint((L), (idx), (def))

static inline int8_t lauxh_checkint8(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "int8_t", lauxh_isint8);
    return (int8_t)lua_tointeger(L, idx);
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
    CHECK_NUMTYPE(L, idx, "uint8_t", lauxh_isuint8);
    return (uint8_t)lua_tointeger(L, idx);
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
    CHECK_NUMTYPE(L, idx, "int16_t", lauxh_isint16);
    return (int16_t)lua_tointeger(L, idx);
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
    CHECK_NUMTYPE(L, idx, "uint16_t", lauxh_isuint16);
    return (uint16_t)lua_tointeger(L, idx);
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
    CHECK_NUMTYPE(L, idx, "int32_t", lauxh_isint32);
    return (int32_t)lua_tointeger(L, idx);
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
    CHECK_NUMTYPE(L, idx, "uint32_t", lauxh_isuint32);
    return (uint32_t)lua_tointeger(L, idx);
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
    CHECK_NUMTYPE(L, idx, "int64_t", lauxh_isint64);
    return (int64_t)lua_tointeger(L, idx);
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
    CHECK_NUMTYPE(L, idx, "uint64_t", lauxh_isuint64);
    return (uint64_t)lua_tointeger(L, idx);
}

static inline uint64_t lauxh_optuint64(lua_State *L, int idx, uint64_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint64(L, idx);
}

#undef CHECK_NUMTYPE

#define CHECK_NUMTYPE_GLE(L, idx, tname, cmpname, isnumfn, n, nfmt)            \
    do {                                                                       \
        if (!isnumfn((L), (idx), (n))) {                                       \
            int t = lua_type((L), (idx));                                      \
            lua_pushfstring((L),                                               \
                            tname " " cmpname " than or equal to " nfmt        \
                                  " expected, ",                               \
                            (n));                                              \
            if (t != LUA_TNUMBER) {                                            \
                lua_pushfstring((L), "got %s", lua_typename((L), t));          \
            } else {                                                           \
                lua_pushliteral((L), "got an out of range value");             \
            }                                                                  \
            lua_concat((L), 2);                                                \
            lauxh_push_argerror((L), (idx), lua_tostring((L), -1));            \
        }                                                                      \
        lauxh_push_argerror_init();                                            \
    } while (0)

static inline lua_Number lauxh_checknum_ge(lua_State *L, int idx, lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "number", "greater", lauxh_isnum_ge, n, "%f");
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_checknum_le(lua_State *L, int idx, lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "number", "less", lauxh_isnum_le, n, "%f");
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_optnum_ge(lua_State *L, int idx, lua_Number n,
                                         lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum_ge(L, idx, n);
}

static inline lua_Number lauxh_optnum_le(lua_State *L, int idx, lua_Number n,
                                         lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum_le(L, idx, n);
}

static inline lua_Integer lauxh_checkint_ge(lua_State *L, int idx,
                                            lua_Integer n)
{
    CHECK_NUMTYPE_GLE(L, idx, "integer", "greater", lauxh_isint_ge, n, "%d");
    return lua_tointeger(L, idx);
}

static inline lua_Integer lauxh_checkint_le(lua_State *L, int idx,
                                            lua_Integer n)
{
    CHECK_NUMTYPE_GLE(L, idx, "integer", "less", lauxh_isint_le, n, "%d");
    return lua_tointeger(L, idx);
}

static inline lua_Integer lauxh_optint_ge(lua_State *L, int idx, lua_Integer n,
                                          lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint_ge(L, idx, n);
}

static inline lua_Integer lauxh_optint_le(lua_State *L, int idx, lua_Integer n,
                                          lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint_le(L, idx, n);
}

static inline lua_Number lauxh_checkfinite_ge(lua_State *L, int idx,
                                              lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "finite number", "greater", lauxh_isfinite_ge, n,
                      "%f");
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_checkfinite_le(lua_State *L, int idx,
                                              lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "finite number", "less", lauxh_isfinite_le, n,
                      "%f");
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_optfinite_ge(lua_State *L, int idx, lua_Number n,
                                            lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfinite_ge(L, idx, n);
}

static inline lua_Number lauxh_optfinite_le(lua_State *L, int idx, lua_Number n,
                                            lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfinite_le(L, idx, n);
}

#undef CHECK_NUMTYPE_GLE

#define CHECK_NUMTYPE_RANGE(L, idx, tname, isnumfn, min, max, mfmt)            \
    do {                                                                       \
        if (!isnumfn((L), (idx), (min), (max))) {                              \
            int t = lua_type((L), (idx));                                      \
            lua_pushfstring((L),                                               \
                            tname " from " mfmt " to " mfmt " expected, ",     \
                            (min), (max));                                     \
            if (t != LUA_TNUMBER) {                                            \
                lua_pushfstring((L), "got %s", lua_typename((L), t));          \
            } else {                                                           \
                lua_pushliteral((L), "got an out of range value");             \
            }                                                                  \
            lua_concat((L), 2);                                                \
            lauxh_push_argerror((L), (idx), lua_tostring((L), -1));            \
        }                                                                      \
        lauxh_push_argerror_init();                                            \
    } while (0)

static inline lua_Number lauxh_checknum_in_range(lua_State *L, int idx,
                                                 lua_Number min, lua_Number max)
{
    CHECK_NUMTYPE_RANGE(L, idx, "number", lauxh_isnum_in_range, min, max, "%f");
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_optnum_in_range(lua_State *L, int idx,
                                               lua_Number min, lua_Number max,
                                               lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum_in_range(L, idx, min, max);
}

static inline lua_Integer lauxh_checkint_in_range(lua_State *L, int idx,
                                                  lua_Integer min,
                                                  lua_Integer max)
{
    CHECK_NUMTYPE_RANGE(L, idx, "integer", lauxh_isint_in_range, min, max,
                        "%d");
    return lua_tointeger(L, idx);
}

static inline lua_Integer lauxh_optint_in_range(lua_State *L, int idx,
                                                lua_Integer min,
                                                lua_Integer max,
                                                lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint_in_range(L, idx, min, max);
}

static inline lua_Number lauxh_checkfinite_in_range(lua_State *L, int idx,
                                                    lua_Number min,
                                                    lua_Number max)
{
    CHECK_NUMTYPE_RANGE(L, idx, "finite number", lauxh_isfinite_in_range, min,
                        max, "%f");
    return lua_tonumber(L, idx);
}

static inline lua_Number lauxh_optfinite_in_range(lua_State *L, int idx,
                                                  lua_Number min,
                                                  lua_Number max,
                                                  lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfinite_in_range(L, idx, min, max);
}

#undef CHECK_NUMTYPE_RANGE

/* treat integer arguments as bit flags  */

static inline uint64_t lauxh_optflags(lua_State *L, int idx)
{
    const int argc = lua_gettop(L);
    uint64_t flg   = 0;

    for (; idx <= argc; idx++) {
        flg |= lauxh_optuint64(L, idx, 0);
    }

    return flg;
}

/* boolean argument */

static inline int lauxh_checkbool(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TBOOLEAN);
    return lua_toboolean(L, idx);
}
#define lauxh_checkboolean(L, idx) lauxh_checkbool((L), (idx))

static inline int lauxh_optbool(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkbool(L, idx);
}
#define lauxh_optboolean(L, idx, def) lauxh_optbool((L), (idx), (def))

/* table argument */

static inline void lauxh_checktable(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TTABLE);
}

static inline int lauxh_opttable(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    lauxh_checktable(L, idx);
    return idx;
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
    v = lauxh_checkint(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline lua_Integer lauxh_optintegerof(lua_State *L, int idx,
                                             const char *k, lua_Integer def)
{
    lua_Integer v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_optint(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_checkbooleanof(lua_State *L, int idx, const char *k)
{
    int v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_checkbool(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_optbooleanof(lua_State *L, int idx, const char *k,
                                     int def)
{
    int v = 0;

    lua_pushstring(L, k);
    lua_rawget(L, idx);
    v = lauxh_optbool(L, -1, def);
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
    v = lauxh_checkint(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline lua_Integer lauxh_optintegerat(lua_State *L, int idx, int row,
                                             lua_Integer def)
{
    lua_Integer v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_optint(L, -1, def);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_checkbooleanat(lua_State *L, int idx, int row)
{
    int v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_checkbool(L, -1);
    lua_pop(L, 1);

    return v;
}

static inline int lauxh_optbooleanat(lua_State *L, int idx, int row, int def)
{
    int v = 0;

    lua_rawgeti(L, idx, row);
    v = lauxh_optbool(L, -1, def);
    lua_pop(L, 1);

    return v;
}

/* thread argument */

static inline lua_State *lauxh_checkthread(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TTHREAD);
    return lua_tothread(L, idx);
}

static inline lua_State *lauxh_optthread(lua_State *L, int idx, lua_State *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkthread(L, idx);
}

/* function argument */

static inline void lauxh_checkfunc(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TFUNCTION);
}
#define lauxh_checkfunction(L, idx) lauxh_checkfunc((L), (idx))

static inline int lauxh_optfunc(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    lauxh_checkfunc(L, idx);
    return idx;
}

/* cfunction argument */

static inline lua_CFunction lauxh_checkcfunc(lua_State *L, int idx)
{
    lauxh_argcheck(L, lua_iscfunction(L, idx), idx,
                   "cfunction expected, got %s", luaL_typename(L, idx));

    return lua_tocfunction(L, idx);
}
#define lauxh_checkcfunction(L, idx) lauxh_checkcfunc((L), (idx))

static inline lua_CFunction lauxh_optcfunc(lua_State *L, int idx,
                                           lua_CFunction def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkcfunc(L, idx);
}

/* lightuserdata argument */

static inline const void *lauxh_checkpointer(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TLIGHTUSERDATA);
    return lua_topointer(L, idx);
}

static inline const void *lauxh_optpointer(lua_State *L, int idx,
                                           const void *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkpointer(L, idx);
}

/* userdata argument */

static inline const void *lauxh_checkuserdata(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TUSERDATA);
    return lua_topointer(L, idx);
}

static inline const void *lauxh_optuserdata(lua_State *L, int idx,
                                            const void *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuserdata(L, idx);
}

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

static inline FILE *lauxh_optfile(lua_State *L, int idx, FILE *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfile(L, idx);
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

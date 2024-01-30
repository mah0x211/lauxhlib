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

/**
 * NOTE: for string conversions.
 */

/**
 * @brief converts the value at the specified index to a string.
 *
 * @note this function calls the `__tostring` metamethod.
 * @param L lua state
 * @param idx index of the value
 * @param[out] len length of the string
 * @return const char*
 */
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

/**
 * @brief converts the value at the specified index to a string. it is
 * equivalent to `lauxh_tolstr(L, idx, NULL)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return const char*
 */
#define lauxh_tostr(L, idx) lauxh_tolstr((L), (idx), NULL)

/**
 * @brief converts the value at the specified index to a string. it is
 * equivalent to `lauxh_tolstr(L, idx, len)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @param[out] len length of the string
 * @return const char*
 */
#define lauxh_tolstring(L, idx, len) lauxh_tolstr((L), (idx), (len))

/**
 * @brief converts the value at the specified index to a string. it is
 * equivalent to `lauxh_tostr(L, idx, NULL)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return const char*
 */
#define lauxh_tostring(L, idx) lauxh_tolstr((L), (idx), NULL)

/**
 * NOTE: for the reference management.
 */

/**
 * @brief determine whether the specified reference is valid.
 *
 * @param ref reference
 * @return int 1 if valid, otherwise 0.
 */
static inline int lauxh_isref(int ref)
{
    return ref != LUA_NOREF && ref != LUA_REFNIL;
}

/**
 * @brief create a reference of the value at the top of the stack in the
 * `LUA_REGISTRYINDEX`, and remove the value from the stack. and return the
 * reference.
 *
 * @param L lua state
 * @return int reference
 */
static inline int lauxh_ref(lua_State *L)
{
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

/**
 * @brief create a reference of the value at the specified index in the
 * `LUA_REGISTRYINDEX`, and return the reference.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int reference
 */
static inline int lauxh_refat(lua_State *L, int idx)
{
    lua_pushvalue(L, idx);
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

/**
 * @brief push the value associated with the specified reference onto the
 * stack. it is equivalent to `lua_rawgeti(L, LUA_REGISTRYINDEX, ref)`.
 *
 * @param L lua state
 * @param ref reference
 */
static inline void lauxh_pushref(lua_State *L, int ref)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

/**
 * @brief remove the reference from the `LUA_REGISTRYINDEX`.
 *
 * @param L lua state
 * @param ref reference
 * @return int LUA_NOREF
 */
static inline int lauxh_unref(lua_State *L, int ref)
{
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    return LUA_NOREF;
}

/**
 * NOTE: for the table manipulation.
 */

/**
 * @brief get the value associated with the specified key from the table at the
 * specified index. similar to lua_gettable().
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param idx index of the table
 */
static inline void lauxh_gettblof(lua_State *L, const char *k, int idx)
{
    lua_pushstring(L, k);
    lua_rawget(L, idx);
}

/**
 * @brief push the nil value to the specified key of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param idx index of the table
 */
static inline void lauxh_pushnil2tblat(lua_State *L, const char *k, int at)
{
    if (at < 0) {
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushnil(L);
    lua_rawset(L, at);
}

/**
 * @brief push the nil value to the specified key of the table at the top of the
 * stack. it is same as a `lauxh_pushnil2tblat(L, k, -1)`.
 *
 * @param L lua state
 * @param k key string
 * @param v function
 * @param idx index of the table
 */
#define lauxh_pushnil2tbl(L, k) lauxh_pushnil2tblat(L, k, -1)

/**
 * @brief push the nil value to the specified array index of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param at index of the table
 */
static inline void lauxh_pushnil2arrat(lua_State *L, int idx, int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushnil(L);
    lua_rawseti(L, at, idx);
}

/**
 * @brief push the nil value to the specified array index of the table at the
 * top of the stack. it is equivalent to `lauxh_pushnil2arrat(L, idx, -1)`.
 *
 * @param L lua state
 * @param idx array index
 */
#define lauxh_pushnil2arr(L, idx) lauxh_pushnil2arrat(L, idx, -1)

/**
 * @brief push the function to the specified key of the table at the specified
 * index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param v function
 * @param at index of the table
 */
static inline void lauxh_pushfn2tblat(lua_State *L, const char *k,
                                      lua_CFunction v, int at)
{
    if (at < 0) {
        // adjust the index of the table
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushcfunction(L, v);
    lua_rawset(L, at);
}

/**
 * @brief push the function to the specified key of the table at the top of the
 * stack. it is equivalent to `lauxh_pushfn2tblat(L, k, v, -1)`.
 *
 * @param L lua state
 * @param k key string
 * @param v function
 */
#define lauxh_pushfn2tbl(L, k, v) lauxh_pushfn2tblat(L, k, v, -1)

/**
 * @brief push the function to the specified array index of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param v function
 * @param at index of the table
 */
static inline void lauxh_pushfn2arrat(lua_State *L, int idx, lua_CFunction v,
                                      int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushcfunction(L, v);
    lua_rawseti(L, at, idx);
}

/**
 * @brief push the function to the specified array index of the table at the
 * top of the stack. it is equivalent to `lauxh_pushfn2arrat(L, idx, v, -1)`.
 *
 * @param L lua state
 * @param idx array index
 * @param v function
 */
#define lauxh_pushfn2arr(L, idx, v) lauxh_pushfn2arrat(L, idx, v, -1)

/**
 * @brief push the string to the specified key of the table at the specified
 * index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param v string
 * @param at index of the table
 */
static inline void lauxh_pushstr2tblat(lua_State *L, const char *k,
                                       const char *v, int at)
{
    if (at < 0) {
        // adjust the index of the table
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushstring(L, v);
    lua_rawset(L, at);
}

/**
 * @brief push the string to the specified key of the table at the top of the
 * stack. it is equivalent to `lauxh_pushstr2tblat(L, k, v, -1)`.
 *
 * @param L lua state
 * @param k key string
 * @param v string
 */
#define lauxh_pushstr2tbl(L, k, v) lauxh_pushstr2tblat(L, k, v, -1)

/**
 * @brief push the string to the specified array index of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param v string
 * @param at index of the table
 */
static inline void lauxh_pushstr2arrat(lua_State *L, int idx, const char *v,
                                       int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushstring(L, v);
    lua_rawseti(L, at, idx);
}

/**
 * @brief push the string to the specified array index of the table at the top
 * of the stack. it is equivalent to `lauxh_pushstr2arrat(L, idx, v, -1)`.
 *
 * @param L lua state
 * @param idx array index
 * @param v string
 */
#define lauxh_pushstr2arr(L, idx, v) lauxh_pushstr2arrat(L, idx, v, -1)

/**
 * @brief push the string with length to the specified key of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param v string
 * @param l length of the string
 * @param at index of the table
 */
static inline void lauxh_pushlstr2tblat(lua_State *L, const char *k,
                                        const char *v, size_t l, int at)
{
    if (at < 0) {
        // adjust the index of the table
        at -= 2;
    }
    lua_pushstring(L, k);
    lua_pushlstring(L, v, l);
    lua_rawset(L, at);
}

/**
 * @brief push the string with length to the specified key of the table at the
 * top of the stack.
 *
 * @param L lua state
 * @param k key string
 * @param v string
 * @param l length of the string
 */
#define lauxh_pushlstr2tbl(L, k, v, l) lauxh_pushlstr2tblat(L, k, v, l, -1)

/**
 * @brief push the string with length to the specified array index of the table
 * at the specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param v string
 * @param l length of the string
 * @param at index of the table
 */
static inline void lauxh_pushlstr2arrat(lua_State *L, int idx, const char *v,
                                        size_t l, int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushlstring(L, v, l);
    lua_rawseti(L, at, idx);
}

/**
 * @brief push the string with length to the specified array index of the table
 * at the top of the stack. it is equivalent to `lauxh_pushlstr2arrat(L, idx, v,
 * l, -1)`.
 *
 * @param L lua state
 * @param idx array index
 * @param v string
 * @param l length of the string
 */
#define lauxh_pushlstr2arr(L, idx, v, l) lauxh_pushlstr2arrat(L, idx, v, l, -1)

/**
 * @brief push the number to the specified key of the table at the specified
 * index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param v number
 * @param at index of the table
 */
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

/**
 * @brief push the number to the specified key of the table at the top of the
 * stack.
 *
 * @param L lua state
 * @param k key string
 * @param v number
 */
#define lauxh_pushnum2tbl(L, k, v) lauxh_pushnum2tblat(L, k, v, -1)

/**
 * @brief push the number to the specified array index of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param v number
 * @param at index of the table
 */
static inline void lauxh_pushnum2arrat(lua_State *L, int idx, lua_Number v,
                                       int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushnumber(L, v);
    lua_rawseti(L, at, idx);
}

/**
 * @brief push the number to the specified array index of the table at the top
 * of the stack. it is equivalent to `lauxh_pushnum2arrat(L, idx, v, -1)`.
 *
 * @param L lua state
 * @param idx array index
 * @param v number
 */
#define lauxh_pushnum2arr(L, idx, v) lauxh_pushnum2arrat(L, idx, v, -1)

/**
 * @brief push the integer to the specified key of the table at the specified
 * index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param v integer
 * @param at index of the table
 */
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

/**
 * @brief push the integer to the specified key of the table at the top of the
 * stack.
 *
 * @param L lua state
 * @param k key string
 * @param v integer
 */
#define lauxh_pushint2tbl(L, k, v) lauxh_pushint2tblat(L, k, v, -1)

/**
 * @brief push the integer to the specified array index of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param v integer
 * @param at index of the table
 */
static inline void lauxh_pushint2arrat(lua_State *L, int idx, lua_Integer v,
                                       int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushinteger(L, v);
    lua_rawseti(L, at, idx);
}

/**
 * @brief push the integer to the specified array index of the table at the top
 * of the stack. it is equivalent to `lauxh_pushint2arrat(L, idx, v, -1)`.
 *
 * @param L lua state
 * @param idx array index
 * @param v integer
 */
#define lauxh_pushint2arr(L, idx, v) lauxh_pushint2arrat(L, idx, v, -1)

/**
 * @brief push the boolean to the specified key of the table at the specified
 * index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param k key string
 * @param v boolean
 * @param at index of the table
 */
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

/**
 * @brief push the boolean to the specified key of the table at the top of the
 * stack. it is equivalent to `lauxh_pushbool2tblat(L, k, v, -1)`.
 *
 * @param L lua state
 * @param k key string
 * @param v boolean
 */
#define lauxh_pushbool2tbl(L, k, v) lauxh_pushbool2tblat(L, k, v, -1)

/**
 * @brief push the boolean to the specified array index of the table at the
 * specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param v boolean
 * @param at index of the table
 */
static inline void lauxh_pushbool2arrat(lua_State *L, int idx, int v, int at)
{
    if (at < 0) {
        at--;
    }
    lua_pushboolean(L, v);
    lua_rawseti(L, at, idx);
}

/**
 * @brief push the boolean to the specified array index of the table at the top
 * of the stack. it is equivalent to `lauxh_pushbool2arrat(L, idx, v, -1)`.
 *
 * @param L lua state
 * @param idx array index
 * @param v boolean
 */
#define lauxh_pushbool2arr(L, idx, v) lauxh_pushbool2arrat(L, idx, v, -1)

/**
 * @brief get the value associated with the specified array index for the table
 * at the specified index.
 *
 * @note this function does not call the metamethod.
 * @param L lua state
 * @param idx array index
 * @param at index of the table
 */
static inline void lauxh_gettblat(lua_State *L, int idx, int at)
{
    lua_rawgeti(L, at, idx);
}

/**
 * @brief get the length of the table at the specified index.
 *
 * @param L lua state
 * @param idx index of the table
 */
#if LUA_VERSION_NUM >= 502
# define lauxh_rawlen(L, idx) lua_rawlen(L, idx)
#else
# define lauxh_rawlen(L, idx) lua_objlen(L, idx)
#endif

/**
 * NOTE: helper functions and macros
 */

/**
 * @brief push the value associated with the specified key from the global scope
 * onto the top of the stack. if the value is not found, then place the `nil`
 * value.
 *
 * @param L lua state
 * @param k key string
 */
static inline void lauxh_getglobal(lua_State *L, const char *k)
{
#if defined(LUA_RIDX_GLOBALS)
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    lua_getfield(L, -1, k);

#else
    lua_getfield(L, LUA_GLOBALSINDEX, k);

#endif
}

/**
 * NOTE: for metatable handling.
 */

/**
 * @brief set the metatable to the value at the top of the stack.
 *
 * @param L lua state
 * @param tname metatable name
 */
static inline void lauxh_setmetatable(lua_State *L, const char *tname)
{
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
}

/**
 * NOTE: for the comparison.
 */

/**
 * @brief determine whether the value at the specified index has a metatable of
 * the specified name.
 *
 * @param L lua state
 * @param idx index of the value
 * @param tname metatable name
 * @return int 1 if true, otherwise 0.
 */
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

#if LUA_VERSION_NUM >= 502
/**
 * @brief compare the values of the specified indices. it is equivalent to
 * `lua_compare(L, idx1, idx2, LUA_OPEQ)`.
 *
 * @param L lua state
 * @param idx1 index of the value
 * @param idx2 index of the value
 * @return int 1 if true, otherwise 0.
 */
# define lauxh_equal(L, idx1, idx2) lua_compare(L, idx1, idx2, LUA_OPEQ)

#else
/**
 * @brief compare the values of the specified indices.  it is equivalent to
 * `lua_equal(L, idx1, idx2)`.
 *
 * @param L lua state
 * @param idx1 index of the value
 * @param idx2 index of the value
 * @return int 1 if true, otherwise 0.
 */
# define lauxh_equal(L, idx1, idx2) lua_equal(L, idx1, idx2)
#endif

/**
 * NOTE: for the type checking.
 */

/**
 * @brief determine whether the value at the specified index is `LUA_TNIL` or
 * `LUA_TNONE`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isnil(lua_State *L, int idx)
{
    return lua_isnoneornil(L, idx);
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TSSTRING`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isstr(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TSTRING;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TSTRING`. it is equivalent to `lauxh_isstr(L, idx)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
#define lauxh_isstring(L, idx) lauxh_isstr((L), (idx))

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TBOLLEAN`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isbool(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TBOOLEAN;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TBOLLEAN`. it is equivalent to `lauxh_isbool(L, idx)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
#define lauxh_isboolean(L, idx) lauxh_isbool((L), (idx))

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TTABLE`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_istable(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TTABLE;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TFUNCTION`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isfunc(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TFUNCTION;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TFUNCTION`. it is equivalent to `lauxh_isfunc(L, idx)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
#define lauxh_isfunction(L, idx) lauxh_isfunc((L), (idx))

/**
 * @brief determine whether the type of the value at the specified index is the
 * `lua_CFunction` type. it is equivalent to `lua_iscfunction(L, idx)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_iscfunc(lua_State *L, int idx)
{
    return lua_iscfunction(L, idx);
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `lua_CFunction`. it is equivalent to `lauxh_iscfunc(L, idx)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
#define lauxh_iscfunction(L, idx) lauxh_iscfunc((L), (idx))

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TTHREAD`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isthread(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TTHREAD;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TUSERDATA`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isuserdata(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TUSERDATA;
}

/**
 * @brief determine whether the value at the specified index is a userdata that
 * has the metatable of the specified name.
 *
 * @param L lua state
 * @param idx index of the value
 * @param tname metatable name
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isuserdataof(lua_State *L, int idx, const char *tname)
{
    return lauxh_isuserdata(L, idx) && lauxh_ismetatableof(L, idx, tname);
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TLIGHTUSERDATA`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_ispointer(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TLIGHTUSERDATA;
}

/**
 * @brief determine whether the value at the specified index has a metatable of
 * the `LUA_FILEHANDLE`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isfile(lua_State *L, int idx)
{
    return lauxh_ismetatableof(L, idx, LUA_FILEHANDLE);
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * function or it has a metatable that contains the
 * `__call` metamethod.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_iscallable(lua_State *L, int idx)
{
    if (lauxh_isfunc(L, idx)) {
        return 1;
    } else if (lua_getmetatable(L, idx)) {
        int isfunc = 0;
        lua_pushliteral(L, "__call");
        lua_rawget(L, -2);
        isfunc = lauxh_isfunc(L, -1);
        lua_pop(L, 2);
        return isfunc;
    }
    return 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * number type.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isnum(lua_State *L, int idx)
{
    return lua_type(L, idx) == LUA_TNUMBER;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * number type. it is equivalent to `lauxh_isnum(L, idx)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
#define lauxh_isnumber(L, idx) lauxh_isnum((L), idx)

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TNUMBER` and is greater than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isnum_ge(lua_State *L, int idx, lua_Number n)
{
    return lauxh_isnum(L, idx) && lua_tonumber(L, idx) >= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TNUMBER` and is less than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isnum_le(lua_State *L, int idx, lua_Number n)
{
    return lauxh_isnum(L, idx) && lua_tonumber(L, idx) <= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TNUMBER` and is in the specified range of numbers.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isnum_in_range(lua_State *L, int idx, lua_Number min,
                                       lua_Number max)
{
    if (!isfinite(min) || !isfinite(max) || min > max) {
        luaL_error(L,
                   "min and max must be finite numbers, and min %f must be "
                   "less than or equal to max %f",
                   min, max);
    }

    if (lauxh_isnum(L, idx)) {
        lua_Number v = lua_tonumber(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * `LUA_TNUMBER` and is a finite number with `isfinite()` function.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isfinite(lua_State *L, int idx)
{
    return lauxh_isnum(L, idx) && isfinite(lua_tonumber(L, idx));
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * finite number and is greater than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isfinite_ge(lua_State *L, int idx, lua_Number n)
{
    return lauxh_isfinite(L, idx) && lua_tonumber(L, idx) >= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * finite number and is less than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isfinite_le(lua_State *L, int idx, lua_Number n)
{
    return lauxh_isfinite(L, idx) && lua_tonumber(L, idx) <= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * finite number and is in the specified range of numbers.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isfinite_in_range(lua_State *L, int idx, lua_Number min,
                                          lua_Number max)
{
    if (!isfinite(min) || !isfinite(max) || min > max) {
        luaL_error(L,
                   "min and max must be finite numbers, and min %f must be "
                   "less than or equal to max %f",
                   min, max);
    }
    if (lauxh_isfinite(L, idx)) {
        lua_Number v = lua_tonumber(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned number.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 *
 * @note this function does not call the metamethod.
 */
static inline lua_Number lauxh_isunsigned(lua_State *L, int idx)
{
    return lauxh_isnum(L, idx) && lua_tonumber(L, idx) >= 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned number and is greater than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isunsigned_ge(lua_State *L, int idx, lua_Number n)
{
    return lauxh_isunsigned(L, idx) && lua_tonumber(L, idx) >= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned number and is less than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isunsigned_le(lua_State *L, int idx, lua_Number n)
{
    return lauxh_isunsigned(L, idx) && lua_tonumber(L, idx) <= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned number and is in the specified range of numbers.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isunsigned_in_range(lua_State *L, int idx,
                                            lua_Number min, lua_Number max)
{
    if (!isfinite(min) || !isfinite(max) || min > max) {
        luaL_error(L,
                   "min and max must be finite numbers, and min %f must be "
                   "less than or equal to max %f",
                   min, max);
    }
    if (lauxh_isunsigned(L, idx)) {
        lua_Number v = lua_tonumber(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isint(lua_State *L, int idx)
{
#if LUA_VERSION_NUM >= 503
    return lua_isinteger(L, idx);
#else
    return lauxh_isnum(L, idx) &&
           (lua_Number)lua_tointeger(L, idx) == lua_tonumber(L, idx);
#endif
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer. it is equivalent to `lauxh_isint(L, idx)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isinteger(L, idx) lauxh_isint((L), (idx))

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer and is greater than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isint_ge(lua_State *L, int idx, lua_Integer n)
{
    return lauxh_isint(L, idx) && lua_tointeger(L, idx) >= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer and is less than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isint_le(lua_State *L, int idx, lua_Integer n)
{
    return lauxh_isint(L, idx) && lua_tointeger(L, idx) <= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer and is in the specified range of numbers.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isint_in_range(lua_State *L, int idx, lua_Integer min,
                                       lua_Integer max)
{
    if (min > max) {
        luaL_error(L, "min %d must be less than or equal to max %d", min, max);
    }
    if (lauxh_isint(L, idx)) {
        lua_Integer v = lua_tointeger(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer and is in the range of the `int8_t` type. it is equivalent to
 * `lauxh_isint_in_range(L, idx, INT8_MIN, INT8_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
#define lauxh_isint8(L, idx)                                                   \
    lauxh_isint_in_range((L), (idx), INT8_MIN, INT8_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer and is in the range of the `int16_t` type. it is equivalent to
 * `lauxh_isint_in_range(L, idx, INT16_MIN, INT16_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isint16(L, idx)                                                  \
    lauxh_isint_in_range((L), (idx), INT16_MIN, INT16_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer and is in the range of the `int32_t` type. it is equivalent to
 * `lauxh_isint_in_range(L, idx, INT32_MIN, INT32_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isint32(L, idx)                                                  \
    lauxh_isint_in_range((L), (idx), INT32_MIN, INT32_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * integer and is in the range of the `int64_t` type. it is equivalent to the
 * `lauxh_isint_in_range(L, idx, INT64_MIN, INT64_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isint64(L, idx)                                                  \
    lauxh_isint_in_range((L), (idx), INT64_MIN, INT64_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_isuint(lua_State *L, int idx)
{
    return lauxh_isint(L, idx) && lua_tointeger(L, idx) >= 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer and is greater than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 */
static inline int lauxh_isuint_ge(lua_State *L, int idx, uintmax_t n)
{
    return lauxh_isuint(L, idx) && (uintmax_t)lua_tointeger(L, idx) >= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer and is less than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 */
static inline int lauxh_isuint_le(lua_State *L, int idx, uintmax_t n)
{
    return lauxh_isuint(L, idx) && (uintmax_t)lua_tointeger(L, idx) <= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer and is in the specified range of numbers.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 */
static inline int lauxh_isuint_in_range(lua_State *L, int idx, uintmax_t min,
                                        uintmax_t max)
{
    if (min > max) {
        luaL_error(L, "min %d must be less than or equal to max %d", min, max);
    }
    if (lauxh_isuint(L, idx)) {
        uintmax_t v = (uintmax_t)lua_tointeger(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer and is in the range of the `uint8_t` type. it is equivalent
 * to the `lauxh_isuint_in_range(L, idx, 0, UINT8_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isuint8(L, idx) lauxh_isuint_in_range((L), (idx), 0, UINT8_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer and is in the range of the `uint16_t` type. it is equivalent
 * to the `lauxh_isuint_in_range(L, idx, 0, UINT16_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isuint16(L, idx) lauxh_isuint_in_range((L), (idx), 0, UINT16_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer and is in the range of the `uint32_t` type. it is equivalent
 * to the `lauxh_isuint_in_range(L, idx, 0, UINT32_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isuint32(L, idx) lauxh_isuint_in_range((L), (idx), 0, UINT32_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * unsigned integer and is in the range of the `uint64_t` type. it is equivalent
 * to the `lauxh_isuint_in_range(L, idx, 0, UINT64_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_isuint64(L, idx) lauxh_isuint_in_range((L), (idx), 0, UINT64_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer.
 *
 * @param L lua state
 * @param idx index of the value
 */
static inline int lauxh_ispint(lua_State *L, int idx)
{
    return lauxh_isint(L, idx) && lua_tointeger(L, idx) > 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer and is greater than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 */
static inline int lauxh_ispint_ge(lua_State *L, int idx, uintmax_t n)
{
    return lauxh_ispint(L, idx) && (uintmax_t)lua_tointeger(L, idx) >= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer and is less than or equal to the specified number.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 */
static inline int lauxh_ispint_le(lua_State *L, int idx, uintmax_t n)
{
    return lauxh_ispint(L, idx) && (uintmax_t)lua_tointeger(L, idx) <= n;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer and is in the specified range of numbers.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 */
static inline int lauxh_ispint_in_range(lua_State *L, int idx, uintmax_t min,
                                        uintmax_t max)
{
    if (min > max) {
        luaL_error(L, "min %d must be less than or equal to max %d", min, max);
    }
    if (lauxh_ispint(L, idx)) {
        uintmax_t v = (uintmax_t)lua_tointeger(L, idx);
        return v >= min && v <= max;
    }
    return 0;
}

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer and is in the range of the `uint8_t` type. it is equivalent
 * to the `lauxh_ispint_in_range(L, idx, 0, UINT8_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_ispint8(L, idx) lauxh_ispint_in_range((L), (idx), 0, UINT8_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer and is in the range of the `uint16_t` type. it is equivalent
 * to the `lauxh_ispint_in_range(L, idx, 0, UINT16_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_ispint16(L, idx) lauxh_ispint_in_range((L), (idx), 0, UINT16_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer and is in the range of the `uint32_t` type. it is equivalent
 * to the `lauxh_ispint_in_range(L, idx, 0, UINT32_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_ispint32(L, idx) lauxh_ispint_in_range((L), (idx), 0, UINT32_MAX)

/**
 * @brief determine whether the type of the value at the specified index is the
 * positive integer and is in the range of the `uint64_t` type. it is equivalent
 * to the `lauxh_ispint_in_range(L, idx, 0, UINT64_MAX)`.
 *
 * @param L lua state
 * @param idx index of the value
 */
#define lauxh_ispint64(L, idx) lauxh_ispint_in_range((L), (idx), 0, UINT64_MAX)

/**
 * NOTE: for the value checking.
 */

/**
 * @warning DO NOT USE THIS MACRO DIRECTLY.
 */
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

/**
 * @brief raises an error reporting a problem with the argument at the specified
 * index.
 *
 * @param L lua state
 * @param arg index of the argument
 * @param extra extra error message
 * @return int 1 if true, otherwise 0.
 */
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

/**
 * @brief raises an error reporting a problem if the argument at the specified
 * index is not the specified type.
 *
 * @param L lua state
 * @param arg index of the argument
 * @param t type of the argument (LUA_TXXX)
 */
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

/**
 * @brief raises an error reporting a problem.
 *
 * @param L lua state
 * @param idx index of the argument
 * @param fmt format string for the extra error message
 * @param ... arguments for the format string
 */
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

/**
 * @brief if the condition is not true, raises an error reporting the problem
 * with an error message.
 *
 * @param L lua state
 * @param cond condition
 * @param idx index of the argument
 * @param fmt format string for the extra error message
 * @param ... arguments for the format string
 */
#define lauxh_argcheck(L, cond, idx, fmt, ...)                                 \
    do {                                                                       \
        if (!(cond)) {                                                         \
            lauxh_argerror((L), (idx), (fmt), ##__VA_ARGS__);                  \
        }                                                                      \
        lauxh_push_argerror_init();                                            \
    } while (0)

/**
 * @brief checks whether the value at the specified index is luaL_Stream*
 * (userdata that has the metatable of the `LUA_FILEHANDLE`) and returns a
 * pointer of the FILE*; if not, raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @return FILE**
 */
static inline FILE **lauxh_checkfilep(lua_State *L, int idx)
{
#if LUA_VERSION_NUM >= 502
    luaL_Stream *stream = luaL_checkudata(L, idx, LUA_FILEHANDLE);
    return &stream->f;

#else
    return (FILE **)luaL_checkudata(L, idx, LUA_FILEHANDLE);
#endif
}

/**
 * @brief same as `lauxh_checkfilep()`, but returns a pointer of the FILE.
 *
 * @param L lua state
 * @param idx index of the value
 * @return FILE* pointer to the FILE
 */
static inline FILE *lauxh_checkfile(lua_State *L, int idx)
{
    return *lauxh_checkfilep(L, idx);
}

/**
 * @brief checks whether the value at the specified index is luaL_Stream*
 * (userdata that has the metatable of the `LUA_FILEHANDLE`) and returns a
 * pointer of the FILE*; if it is nil, returns the specified default value,
 * otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return FILE*
 */
static inline FILE *lauxh_optfile(lua_State *L, int idx, FILE *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfile(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a callable object.
 * if not, raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 */
static inline void lauxh_checkcallable(lua_State *L, int idx)
{
    lauxh_argcheck(L, lauxh_iscallable(L, idx), idx,
                   "callable object expected, got %s", luaL_typename(L, idx));
}

/**
 * @brief checks whether the value at the specified index is a callable object
 * and returns its index; if it is nil, returns the specified default value,
 * otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int index of the callable object
 */
static inline int lauxh_optcallable(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    lauxh_checkcallable(L, idx);
    return idx;
}

/**
 * @brief checks whether the value at the specified index is a function; if not,
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 */
static inline void lauxh_checkfunc(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TFUNCTION);
}

/**
 * @brief alias of the `lauxh_checkfunc()`.
 */
#define lauxh_checkfunction(L, idx) lauxh_checkfunc((L), (idx))

/**
 * @brief checks whether the value at the specified index is a function and
 * returns its index; if it is nil, returns the specified default value,
 * otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int index of the function
 */
static inline int lauxh_optfunc(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    lauxh_checkfunc(L, idx);
    return idx;
}

/**
 * @brief checks whether the value at the specified index is a lua_CFunction and
 * returns it; if it is not a lua_CFunction, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return lua_CFunction
 */
static inline lua_CFunction lauxh_checkcfunc(lua_State *L, int idx)
{
    lauxh_argcheck(L, lua_iscfunction(L, idx), idx,
                   "cfunction expected, got %s", luaL_typename(L, idx));
    return lua_tocfunction(L, idx);
}

/**
 * @brief alias of the `lauxh_checkcfunc()`.
 */
#define lauxh_checkcfunction(L, idx) lauxh_checkcfunc((L), (idx))

/**
 * @brief checks whether the value at the specified index is a lua_CFunction and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_CFunction
 */
static inline lua_CFunction lauxh_optcfunc(lua_State *L, int idx,
                                           lua_CFunction def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkcfunc(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a thread and
 * returns it; if it is not a thread, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 */
static inline lua_State *lauxh_checkthread(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TTHREAD);
    return lua_tothread(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a thread and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_State*
 */
static inline lua_State *lauxh_optthread(lua_State *L, int idx, lua_State *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkthread(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a light userdata
 * and returns it; if it is not a light userdata, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return const void* pointer to the light userdata
 */
static inline const void *lauxh_checkpointer(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TLIGHTUSERDATA);
    return lua_topointer(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a light userdata
 * and returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return const void* pointer to the light userdata
 */
static inline const void *lauxh_optpointer(lua_State *L, int idx,
                                           const void *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkpointer(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a userdata and
 * returns it; if it is not a userdata, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return void* pointer to the userdata
 */
static inline void *lauxh_checkuserdata(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TUSERDATA);
    return lua_touserdata(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a userdata and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return void* pointer to the userdata
 */
static inline void *lauxh_optuserdata(lua_State *L, int idx, void *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuserdata(L, idx);
}

/**
 * @brief checks whether the value at the specified index is a userdata that has
 * the metatable of the specified name and returns it; if not, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param tname metatable name
 * @return void* pointer to userdata
 */
static inline void *lauxh_checkudata(lua_State *L, int idx, const char *tname)
{
    return luaL_checkudata(L, idx, tname);
}

/**
 * @brief checks whether the value at the specified index is a userdata that has
 * the metatable of the specified name and returns it; if it is nil, returns the
 * specifed default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param tname metatable name
 * @param def default value
 * @return void* pointer to userdata
 */
static inline void *lauxh_optudata(lua_State *L, int idx, const char *tname,
                                   void *def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }

    return lauxh_checkudata(L, idx, tname);
}

/**
 * @brief checks whether the value at the specified index is a boolean and
 * returns it; if it is not a boolean, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_checkbool(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TBOOLEAN);
    return lua_toboolean(L, idx);
}

/**
 * @brief alias of the `lauxh_checkbool()`.
 */
#define lauxh_checkboolean(L, idx) lauxh_checkbool((L), (idx))

/**
 * @brief checks whether the value at the specified index is a boolean and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int 1 if true, otherwise 0.
 */
static inline int lauxh_optbool(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkbool(L, idx);
}

/**
 * @brief alias of the `lauxh_optbool()`.
 */
#define lauxh_optboolean(L, idx, def) lauxh_optbool((L), (idx), (def))

/**
 * @brief checks whether the value at the specified index is a string and
 * returns it; if it is not a string, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param[out] len length of the string
 * @return const char* pointer to the string
 */
static inline const char *lauxh_checklstr(lua_State *L, int idx, size_t *len)
{
    lauxh_checktype(L, idx, LUA_TSTRING);
    return lua_tolstring(L, idx, len);
}

/**
 * @brief alias of the `lauxh_checklstr()`.
 */
#define lauxh_checklstring(L, idx, len) lauxh_checklstr((L), (idx), (len))

/**
 * @brief equivalent to `lauxh_checklstr(L, idx, NULL)`.
 */
#define lauxh_checkstr(L, idx) lauxh_checklstr((L), (idx), NULL)

/**
 * @brief alias of the `lauxh_checkstr()`.
 */
#define lauxh_checkstring(L, idx) lauxh_checkstr((L), (idx))

/**
 * @brief checks whether the value at the specified index is a string and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @param[out] len length of the string
 * @return const char* pointer to the string
 */
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

/**
 * @brief alias of the `lauxh_optlstr()`.
 */
#define lauxh_optlstring(L, idx, def, len)                                     \
    lauxh_optlstr((L), (idx), (def), (len))

/**
 * @brief equivalent to `lauxh_optlstr(L, idx, def, NULL)`.
 */
#define lauxh_optstr(L, idx, def) lauxh_optlstr((L), (idx), (def), NULL)

/**
 * @brief alias of the `lauxh_optstr()`.
 */
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

/**
 * @brief checks whether the value at the specified index is the number and
 * returns it; if it is not a number, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return lua_Number
 */
static inline lua_Number lauxh_checknum(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "number", lauxh_isnum);
    return lua_tonumber(L, idx);
}

/**
 * @brief alias of the `lauxh_checknum()`.
 */
#define lauxh_checknumber(L, idx) lauxh_checknum((L), (idx))

/**
 * @brief checks whether the value at the specified index is the number and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optnum(lua_State *L, int idx, lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum(L, idx);
}

/**
 * @brief alias of the `lauxh_optnum()`.
 */
#define lauxh_optnumber(L, idx, def) lauxh_optnum((L), (idx), (def))

/**
 * @brief checks whether the value at the specified index is the integer and
 * returns it; if it is not a integer, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkint(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "integer", lauxh_isint);
    return lua_tointeger(L, idx);
}

/**
 * @brief alias of the `lauxh_checkint()`.
 */
#define lauxh_checkinteger(L, idx) lauxh_checkint((L), (idx))

/**
 * @brief checks whether the value at the specified index is the integer and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optint(lua_State *L, int idx, lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint(L, idx);
}

/**
 * @brief alias of the `lauxh_optint()`.
 */
#define lauxh_optinteger(L, idx, def) lauxh_optint((L), (idx), (def))

/**
 * @brief checks whether the value at the specified index is the finite number
 * and returns it; if it is not a finite number, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return lua_Integer
 */
static inline lua_Number lauxh_checkfinite(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "finite number", lauxh_isfinite);
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the value at the specified index is the finite number
 * and returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Number lauxh_optfinite(lua_State *L, int idx, lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfinite(L, idx);
}

/**
 * @brief checks whether the value at the specified index is the unsigned
 * number and returns it; if it is not a unsigned number, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return lua_Number
 */
static inline lua_Number lauxh_checkunsigned(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "unsigned number", lauxh_isunsigned);
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the value at the specified index is the unsigned number
 * and returns it; if it is nil, returns the specified default value, otherwise
 * raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optunsigned(lua_State *L, int idx,
                                           lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkunsigned(L, idx);
}

/**
 * @brief checks whether the value at the specified index is the unsigned
 * integer and returns it; if it is not a unsigned integer, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkuint(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "unsigned integer", lauxh_isuint);
    return lua_tointeger(L, idx);
}

/**
 * @brief alias of the `lauxh_checkuint()`.
 */
#define lauxh_checkuinteger(L, idx) lauxh_checkuint((L), (idx))

/**
 * @brief checks whether the value at the specified index is the unsigned
 * integer and returns it; if it is nil, returns the specified default value,
 * otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optuint(lua_State *L, int idx, lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint(L, idx);
}

/**
 * @brief alias of the `lauxh_optuint()`.
 */
#define lauxh_optuinteger(L, idx, def) lauxh_optuint((L), (idx), (def))

/**
 * @brief checks whether the value at the specified index is the positive
 * integer and returns it; if it is not a positive integer, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkpint(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "positive integer", lauxh_ispint);
    return lua_tointeger(L, idx);
}

/**
 * @brief alias of the `lauxh_checkpint()`.
 */
#define lauxh_checkpinteger(L, idx) lauxh_checkpint((L), (idx))

/**
 * @brief checks whether the value at the specified index is the positive
 * integer and returns it; if it is nil, returns the specified default value,
 * otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optpint(lua_State *L, int idx, lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkpint(L, idx);
}

/**
 * @brief alias of the `lauxh_optpint()`.
 */
#define lauxh_optpinteger(L, idx, def) lauxh_optpint((L), (idx), (def))

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int8_t` type and returns it; if it is not in the range of the `int8_t` type,
 * raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int8_t
 */
static inline int8_t lauxh_checkint8(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "int8_t", lauxh_isint8);
    return (int8_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int8_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int8_t
 */
static inline int8_t lauxh_optint8(lua_State *L, int idx, int8_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint8(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint8_t` type and returns it; if it is not in the range of the `uint8_t`
 * type, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return uint8_t
 */
static inline uint8_t lauxh_checkuint8(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "uint8_t", lauxh_isuint8);
    return (uint8_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint8_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return uint8_t
 */
static inline uint8_t lauxh_optuint8(lua_State *L, int idx, uint8_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint8(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int16_t` type and returns it; if it is not in the range of the `int16_t`
 * type, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int16_t
 */
static inline int16_t lauxh_checkint16(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "int16_t", lauxh_isint16);
    return (int16_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int16_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int16_t
 */
static inline int16_t lauxh_optint16(lua_State *L, int idx, int16_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint16(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint16_t` type and returns it; if it is not in the range of the `uint16_t`
 * type, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return uint16_t
 */
static inline uint16_t lauxh_checkuint16(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "uint16_t", lauxh_isuint16);
    return (uint16_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint16_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return uint16_t
 */
static inline uint16_t lauxh_optuint16(lua_State *L, int idx, uint16_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint16(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int32_t` type and returns it; if it is not in the range of the `int32_t`
 * type, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int32_t
 */
static inline int32_t lauxh_checkint32(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "int32_t", lauxh_isint32);
    return (int32_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int32_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int32_t
 */
static inline int32_t lauxh_optint32(lua_State *L, int idx, int32_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint32(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint32_t` type and returns it; if it is not in the range of the `uint32_t`
 * type, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return uint32_t
 */
static inline uint32_t lauxh_checkuint32(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "uint32_t", lauxh_isuint32);
    return (uint32_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint32_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return uint32_t
 */
static inline uint32_t lauxh_optuint32(lua_State *L, int idx, uint32_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkuint32(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int64_t` type and returns it; if it is not in the range of the `int64_t`
 * type, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return int64_t
 */
static inline int64_t lauxh_checkint64(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "int64_t", lauxh_isint64);
    return (int64_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `int64_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int64_t
 */
static inline int64_t lauxh_optint64(lua_State *L, int idx, int64_t def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint64(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint64_t` type and returns it; if it is not in the range of the `uint64_t`
 * type, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @return uint64_t
 */
static inline uint64_t lauxh_checkuint64(lua_State *L, int idx)
{
    CHECK_NUMTYPE(L, idx, "uint64_t", lauxh_isuint64);
    return (uint64_t)lua_tointeger(L, idx);
}

/**
 * @brief checks whether the value at the specified index is in the range of the
 * `uint64_t` type and returns it; if it is nil, returns the specified default
 * value, otherwise raises an error.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return uint64_t
 */
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

/**
 * @brief checks whether the type of the value at the specified index is the
 * number and is greater than or equal to the specified number and returns it;
 * if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return lua_Number
 */
static inline lua_Number lauxh_checknum_ge(lua_State *L, int idx, lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "number", "greater", lauxh_isnum_ge, n, "%f");
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * number and is less than or equal to the specified number and returns it;
 * if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @return lua_Number
 */
static inline lua_Number lauxh_checknum_le(lua_State *L, int idx, lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "number", "less", lauxh_isnum_le, n, "%f");
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * number and is greater than or equal to the specified number and returns it;
 * if it is nil, returns the specified default value, otherwise raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optnum_ge(lua_State *L, int idx, lua_Number n,
                                         lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum_ge(L, idx, n);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * number and is less than or equal to the specified number and returns it;
 * if it is nil, returns the specified default value, otherwise raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optnum_le(lua_State *L, int idx, lua_Number n,
                                         lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum_le(L, idx, n);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * integer and is greater than or equal to the specified number and returns it;
 * if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkint_ge(lua_State *L, int idx,
                                            lua_Integer n)
{
    CHECK_NUMTYPE_GLE(L, idx, "integer", "greater", lauxh_isint_ge, n, "%d");
    return lua_tointeger(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * integer and is less than or equal to the specified number and returns it;
 * if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkint_le(lua_State *L, int idx,
                                            lua_Integer n)
{
    CHECK_NUMTYPE_GLE(L, idx, "integer", "less", lauxh_isint_le, n, "%d");
    return lua_tointeger(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * integer and is greater than or equal to the specified number and returns it;
 * if it is nil, returns the specified default value, otherwise raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optint_ge(lua_State *L, int idx, lua_Integer n,
                                          lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint_ge(L, idx, n);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * integer and is less than or equal to the specified number and returns it;
 * if it is nil, returns the specified default value, otherwise raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optint_le(lua_State *L, int idx, lua_Integer n,
                                          lua_Integer def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint_le(L, idx, n);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * finite number and is greater than or equal to the specified number and
 * returns it; if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @return lua_Number
 */
static inline lua_Number lauxh_checkfinite_ge(lua_State *L, int idx,
                                              lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "finite number", "greater", lauxh_isfinite_ge, n,
                      "%f");
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * finite number and is less than or equal to the specified number and returns
 * it; if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @return lua_Number
 */
static inline lua_Number lauxh_checkfinite_le(lua_State *L, int idx,
                                              lua_Number n)
{
    CHECK_NUMTYPE_GLE(L, idx, "finite number", "less", lauxh_isfinite_le, n,
                      "%f");
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * finite number and is greater than or equal to the specified number and
 * returns it; if it is nil, returns the specified default value, otherwise
 * raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is greater than or equal to this number
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optfinite_ge(lua_State *L, int idx, lua_Number n,
                                            lua_Number def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkfinite_ge(L, idx, n);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * finite number and is less than or equal to the specified number and returns
 * it; if it is nil, returns the specified default value, otherwise raises an
 * error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param n value is less than or equal to this number
 * @param def default value
 * @return lua_Number
 */
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

/**
 * @brief checks whether the type of the value at the specified index is the
 * number and is in the range of the specified numbers and returns it; if not,
 * raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @return lua_Number
 */
static inline lua_Number lauxh_checknum_in_range(lua_State *L, int idx,
                                                 lua_Number min, lua_Number max)
{
    CHECK_NUMTYPE_RANGE(L, idx, "number", lauxh_isnum_in_range, min, max, "%f");
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * number and is in the range of the specified numbers and returns it; if it is
 * nil, returns the specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optnum_in_range(lua_State *L, int idx,
                                               lua_Number min, lua_Number max,
                                               lua_Number def)
{
    if (!isfinite(min) || !isfinite(max) || min > max) {
        luaL_error(L,
                   "min and max must be finite numbers, and min %f must be "
                   "less than or equal to max %f",
                   min, max);
    }
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checknum_in_range(L, idx, min, max);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * integer and is in the range of the specified numbers and returns it; if not,
 * raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkint_in_range(lua_State *L, int idx,
                                                  lua_Integer min,
                                                  lua_Integer max)
{
    CHECK_NUMTYPE_RANGE(L, idx, "integer", lauxh_isint_in_range, min, max,
                        "%d");
    return lua_tointeger(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * integer and is in the range of the specified numbers and returns it; if it is
 * nil, returns the specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optint_in_range(lua_State *L, int idx,
                                                lua_Integer min,
                                                lua_Integer max,
                                                lua_Integer def)
{
    if (min > max) {
        luaL_error(L, "min %d must be less than or equal to max %d", min, max);
    }
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    return lauxh_checkint_in_range(L, idx, min, max);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * finite number and is in the range of the specified numbers and returns it; if
 * not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @return lua_Number
 */
static inline lua_Number lauxh_checkfinite_in_range(lua_State *L, int idx,
                                                    lua_Number min,
                                                    lua_Number max)
{
    CHECK_NUMTYPE_RANGE(L, idx, "finite number", lauxh_isfinite_in_range, min,
                        max, "%f");
    return lua_tonumber(L, idx);
}

/**
 * @brief checks whether the type of the value at the specified index is the
 * finite number and is in the range of the specified numbers and returns it; if
 * it is nil, returns the specified default value, otherwise raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param min value is greater than or equal to this number
 * @param max value is less than or equal to this number
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optfinite_in_range(lua_State *L, int idx,
                                                  lua_Number min,
                                                  lua_Number max,
                                                  lua_Number def)
{
    if (!isfinite(min) || !isfinite(max) || min > max) {
        luaL_error(L,
                   "min and max must be finite numbers, and min %f must be "
                   "less than or equal to max %f",
                   min, max);
    }
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

/**
 * @brief checks whether the value at the specified index is the table; if not,
 * raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 */
static inline void lauxh_checktable(lua_State *L, int idx)
{
    lauxh_checktype(L, idx, LUA_TTABLE);
}

/**
 * @brief checks whether the value at the specified index is the table and
 * returns it index; if it is nil, returns the specified default value,
 * otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the value
 * @param def default value
 * @return int index of the table
 */
static inline int lauxh_opttable(lua_State *L, int idx, int def)
{
    if (lauxh_isnil(L, idx)) {
        return def;
    }
    lauxh_checktable(L, idx);
    return idx;
}

/**
 * NOTE: for table accessors
 */

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the table; if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 */
static inline void lauxh_checktableof(lua_State *L, int idx, const char *k)
{
    lua_pushstring(L, k);
    lua_rawget(L, idx);
    lauxh_checktable(L, -1);
}

#define CHECK_VALUE_OF_KEY_IN_TABLE(tblidx, key, vtype, checkfn, ...)          \
    do {                                                                       \
        vtype v;                                                               \
        lua_pushstring(L, (key));                                              \
        lua_rawget(L, (tblidx));                                               \
        v = checkfn(L, -1, ##__VA_ARGS__);                                     \
        lua_pop(L, 1);                                                         \
        return v;                                                              \
    } while (0)

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the string and returns it; if not, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @param[out] len length of the value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_checklstringof(lua_State *L, int idx,
                                               const char *k, size_t *len)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, const char *, lauxh_checklstr, len);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the string and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @param def default value
 * @param[out] len length of the value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_optlstringof(lua_State *L, int idx,
                                             const char *k, const char *def,
                                             size_t *len)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, const char *, lauxh_optlstr, def, len);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the string and returns it; if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_checkstringof(lua_State *L, int idx,
                                              const char *k)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, const char *, lauxh_checkstr);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the string and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @param def default value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_optstringof(lua_State *L, int idx,
                                            const char *k, const char *def)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, const char *, lauxh_optstr, def);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the number and returns it; if not, raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @return lua_Number
 */
static inline lua_Number lauxh_checknumberof(lua_State *L, int idx,
                                             const char *k)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, lua_Number, lauxh_checknum);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the number and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optnumberof(lua_State *L, int idx, const char *k,
                                           lua_Number def)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, lua_Number, lauxh_optnum, def);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the integer and returns it; if not, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkintegerof(lua_State *L, int idx,
                                               const char *k)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, lua_Integer, lauxh_checkint);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the integer and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optintegerof(lua_State *L, int idx,
                                             const char *k, lua_Integer def)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, lua_Integer, lauxh_optint, def);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the boolean and returns it; if not, raises an error
 * report.
 *
 * @param L  lua state
 * @param idx index of the table
 * @param k key of the value
 * @return int 1 if true, otherwise 0
 */
static inline int lauxh_checkbooleanof(lua_State *L, int idx, const char *k)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, int, lauxh_checkbool);
}

/**
 * @brief checks whether the value at the specified key of the table at the
 * specified index is the boolean and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param k key of the value
 * @param def default value
 * @return int 1 if true, otherwise 0
 */
static inline int lauxh_optbooleanof(lua_State *L, int idx, const char *k,
                                     int def)
{
    CHECK_VALUE_OF_KEY_IN_TABLE(idx, k, int, lauxh_optbool, def);
}

#undef CHECK_VALUE_OF_KEY_IN_TABLE

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the table; if not, raises an error report.
 *
 * @note the value is placed on the top of the stack.
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 */
static inline void lauxh_checktableat(lua_State *L, int idx, int row)
{
    lua_rawgeti(L, idx, row);
    lauxh_checktable(L, -1);
}

#define CHECK_VALUE_OF_IDX_IN_TABLE(tblidx, idx, vtype, checkfn, ...)          \
    do {                                                                       \
        vtype v;                                                               \
        lua_rawgeti(L, (tblidx), (idx));                                       \
        v = checkfn(L, -1, ##__VA_ARGS__);                                     \
        lua_pop(L, 1);                                                         \
        return v;                                                              \
    } while (0)

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the string and returns it; if not, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @param[out] len length of the value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_checklstringat(lua_State *L, int idx, int row,
                                               size_t *len)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, const char *, lauxh_checklstr, len);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the string and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @param def default value
 * @param[out] len length of the value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_optlstringat(lua_State *L, int idx, int row,
                                             const char *def, size_t *len)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, const char *, lauxh_optlstr, def,
                                len);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the string and returns it; if not, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_checkstringat(lua_State *L, int idx, int row)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, const char *, lauxh_checkstr);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the string and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @param def default value
 * @return const char* pointer to the string
 */
static inline const char *lauxh_optstringat(lua_State *L, int idx, int row,
                                            const char *def)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, const char *, lauxh_optstr, def);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the number and returns it; if not, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @return lua_Number
 */
static inline lua_Number lauxh_checknumberat(lua_State *L, int idx, int row)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, lua_Number, lauxh_checknum);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the number and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @param def default value
 * @return lua_Number
 */
static inline lua_Number lauxh_optnumberat(lua_State *L, int idx, int row,
                                           lua_Number def)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, lua_Number, lauxh_optnum, def);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the integer and returns it; if not, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_checkintegerat(lua_State *L, int idx, int row)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, lua_Integer, lauxh_checkint);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the integer and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @param def default value
 * @return lua_Integer
 */
static inline lua_Integer lauxh_optintegerat(lua_State *L, int idx, int row,
                                             lua_Integer def)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, lua_Integer, lauxh_optint, def);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the boolean and returns it; if not, raises an error
 * report.
 *
 * @param L  lua state
 * @param idx index of the table
 * @param row array index of the value
 * @return int 1 if true, otherwise 0
 */
static inline int lauxh_checkbooleanat(lua_State *L, int idx, int row)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, int, lauxh_checkbool);
}

/**
 * @brief checks whether the value at the specified array index of the table at
 * the specified index is the boolean and returns it; if it is nil, returns the
 * specified default value, otherwise raises an error report.
 *
 * @param L lua state
 * @param idx index of the table
 * @param row array index of the value
 * @param def default value
 * @return int 1 if true, otherwise 0
 */
static inline int lauxh_optbooleanat(lua_State *L, int idx, int row, int def)
{
    CHECK_VALUE_OF_IDX_IN_TABLE(idx, row, int, lauxh_optbool, def);
}

#undef CHECK_VALUE_OF_IDX_IN_TABLE

/**
 * NOTE: helper functions
 */

/**
 * @brief get a file descriptor from the file handle at the specified index. if
 * FILE* is NULL, returns -1, or if it is not a file handle, raises an error
 * report.
 *
 * @param L lua state
 * @param idx index of the file handle
 * @return int file descriptor
 */
static inline int lauxh_fileno(lua_State *L, int idx)
{
    FILE *f = lauxh_checkfile(L, idx);
    if (f) {
        return fileno(f);
    }
    return -1;
}

/**
 * @brief create a new file handle from the file descriptor and push it to the
 * top of the stack, and returns the FILE*. if failed, places nil, error message
 * and errno on the top of the stack and returns NULL.
 *
 * @param L lua state
 * @param fd file descriptor
 * @param mode mode string passed to fdopen()
 * @param fname file name if not NULL, create a file in fname
 * @return FILE*
 */
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

/**
 * @brief create a traceback for the stack `src` and push it to stack `L`.
 *
 * @param L lua state if NULL, then push the traceback to the `src` stack.
 * @param src lua state
 * @param msg error message appended to the beginning of the traceback.
 * @param lv which level to start the traceback.
 */
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

/**
 * @brief copy a value at the specified index of the state `from` to the state
 * `to` and returns the type of the copied value. if the value is not supported,
 * returns LUA_TNONE.
 *
 * @note supported types are: LUA_TBOOLEAN, LUA_TLIGHTUSERDATA, LUA_TNUMBER,
 * LUA_TSTRING, LUA_TTABLE and LUA_TNIL.
 * @param from source lua state
 * @param to destination lua state
 * @param idx index of the value
 * @param allow_nil if true, push nil to the `to` state if the value is nil.
 * @return int type of the value
 */
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

/**
 * NOTE: for backword compatibility
 */

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

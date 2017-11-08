/*
 *  Copyright (C) 2016 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
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

#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <lauxlib.h>
#include <lualib.h>


/* reference */

static inline int lauxh_isref( int ref )
{
    return ref >= 0;
}


static inline int lauxh_ref( lua_State *L )
{
    return luaL_ref( L, LUA_REGISTRYINDEX );
}


static inline int lauxh_refat( lua_State *L, int idx )
{
    lua_pushvalue( L, idx );
    return luaL_ref( L, LUA_REGISTRYINDEX );
}


static inline void lauxh_pushref( lua_State *L, int ref )
{
    lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
}


static inline int lauxh_unref( lua_State *L, int ref )
{
    luaL_unref( L, LUA_REGISTRYINDEX, ref );
    return LUA_NOREF;
}



/* table */

static inline void lauxh_gettblof( lua_State *L, const char *k, int idx )
{
    lua_pushstring( L, k );
    lua_rawget( L, idx );
}


static inline void lauxh_pushnil2tblat( lua_State *L, const char *k, int at )
{
    lua_pushstring( L, k );
    lua_pushnil( L );
    lua_rawset( L, at );
}
#define lauxh_pushnil2tbl(L, k) lauxh_pushnil2tblat(L, k, -3)


static inline void lauxh_pushfn2tblat( lua_State *L, const char *k,
                                       lua_CFunction v, int at )
{
    lua_pushstring( L, k );
    lua_pushcfunction( L, v );
    lua_rawset( L, at );
}
#define lauxh_pushfn2tbl(L, k, v) lauxh_pushfn2tblat(L, k, v, -3)


static inline void lauxh_pushstr2tblat( lua_State *L, const char *k,
                                        const char *v, int at )
{
    lua_pushstring( L, k );
    lua_pushstring( L, v );
    lua_rawset( L, at );
}
#define lauxh_pushstr2tbl(L, k, v) lauxh_pushstr2tblat(L, k, v, -3)


static inline void lauxh_pushlstr2tblat( lua_State *L, const char *k,
                                         const char *v, size_t l, int at )
{
    lua_pushstring( L, k );
    lua_pushlstring( L, v, l );
    lua_rawset( L, at );
}
#define lauxh_pushlstr2tbl(L, k, v, l) lauxh_pushlstr2tblat(L, k, v, l, -3)


static inline void lauxh_pushnum2tblat( lua_State *L, const char *k,
                                        lua_Number v, int at )
{
    lua_pushstring( L, k );
    lua_pushnumber( L, v );
    lua_rawset( L, at );
}
#define lauxh_pushnum2tbl(L, k, v) lauxh_pushnum2tblat(L, k, v, -3)


static inline void lauxh_pushint2tblat( lua_State *L, const char *k,
                                        lua_Integer v, int at )
{
    lua_pushstring( L, k );
    lua_pushinteger( L, v );
    lua_rawset( L, at );
}
#define lauxh_pushint2tbl(L, k, v) lauxh_pushint2tblat(L, k, v, -3)


static inline void lauxh_pushbool2tblat( lua_State *L, const char *k, int v,
                                         int at )
{
    lua_pushstring( L, k );
    lua_pushboolean( L, v );
    lua_rawset( L, at );
}
#define lauxh_pushbool2tbl(L, k, v) lauxh_pushbool2tblat(L, k, v, -3)


/* table as array */

static inline void lauxh_gettblat( lua_State *L, int idx, int at )
{
    lua_rawgeti( L, at, idx );
}


static inline void lauxh_pushnil2arrat( lua_State *L, int idx, int at )
{
    lua_pushnil( L );
    lua_rawseti( L, at, idx );
}
#define lauxh_pushnil2arr(L, idx) lauxh_pushnil2arrat(L, idx, -2)


static inline void lauxh_pushfn2arrat( lua_State *L, int idx, lua_CFunction v,
                                       int at )
{
    lua_pushcfunction( L, v );
    lua_rawseti( L, at, idx );
}
#define lauxh_pushfn2arr(L, idx, v) lauxh_pushfn2arrat(L, idx, v, -2)


static inline void lauxh_pushstr2arrat( lua_State *L, int idx, const char *v,
                                        int at )
{
    lua_pushstring( L, v );
    lua_rawseti( L, at, idx );
}
#define lauxh_pushstr2arr(L, idx, v) lauxh_pushstr2arrat(L, idx, v, -2)


static inline void lauxh_pushlstr2arrat( lua_State *L, int idx, const char *v,
                                         size_t l, int at )
{
    lua_pushlstring( L, v, l );
    lua_rawseti( L, at, idx );
}
#define lauxh_pushlstr2arr(L, idx, v, l) lauxh_pushlstr2arrat(L, idx, v, l, -2)


static inline void lauxh_pushnum2arrat( lua_State *L, int idx, lua_Number v,
                                        int at )
{
    lua_pushnumber( L, v );
    lua_rawseti( L, at, idx );
}
#define lauxh_pushnum2arr(L, idx, v) lauxh_pushnum2arrat(L, idx, v, -2)


static inline void lauxh_pushint2arrat( lua_State *L, int idx, lua_Integer v,
                                        int at )
{
    lua_pushinteger( L, v );
    lua_rawseti( L, at, idx );
}
#define lauxh_pushint2arr(L, idx, v) lauxh_pushint2arrat(L, idx, v, -2)


static inline void lauxh_pushbool2arrat( lua_State *L, int idx, int v, int at )
{
    lua_pushboolean( L, v );
    lua_rawseti( L, at, idx );
}
#define lauxh_pushbool2arr(L, idx, v) lauxh_pushbool2arrat(L, idx, v, -2)


#if LUA_VERSION_NUM >= 502
#   define lauxh_rawlen( L, idx )   lua_rawlen( L, idx )
#else
#   define lauxh_rawlen( L, idx )   lua_objlen( L, idx )
#endif


// get global
static inline void lauxh_getglobal( lua_State *L, const char *k )
{
#if defined(LUA_RIDX_GLOBALS)
    lua_rawgeti( L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS );
    lua_getfield( L, -1, k );

#else
    lua_getfield( L, LUA_GLOBALSINDEX, k );

#endif
}


/* metatable */

static inline void lauxh_setmetatable( lua_State *L, const char *tname )
{
    luaL_getmetatable( L, tname );
    lua_setmetatable( L, -2 );
}


/* value check */

#if LUA_VERSION_NUM >= 502
#   define lauxh_equal( L, idx1, idx2 ) lua_compare( L, idx1, idx2, LUA_OPEQ )
#else
#   define lauxh_equal( L, idx1, idx2 ) lua_equal( L, idx1, idx2 )
#endif


/* type */

static inline const char *lauxh_typenameat( lua_State *L, int idx )
{
    return lua_typename( L, lua_type( L, idx ) );
}


/* typecheck */

static inline int lauxh_isnil( lua_State *L, int idx )
{
    return lua_type( L, idx ) <= LUA_TNIL;
}

static inline int lauxh_isstring( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TSTRING;
}

static inline int lauxh_isnumber( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TNUMBER;
}

static inline int lauxh_isboolean( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TBOOLEAN;
}

static inline int lauxh_istable( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TTABLE;
}

static inline int lauxh_isfunction( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TFUNCTION;
}

static inline int lauxh_iscfunction( lua_State *L, int idx )
{
    return lua_iscfunction( L, idx );
}

static inline int lauxh_isthread( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TTHREAD;
}

static inline int lauxh_isuserdata( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TUSERDATA;
}

static inline int lauxh_ispointer( lua_State *L, int idx )
{
    return lua_type( L, idx ) == LUA_TLIGHTUSERDATA;
}

static inline int lauxh_isinteger( lua_State *L, int idx )
{
    return lauxh_isnumber( L, idx ) &&
           (lua_Number)lua_tointeger( L, idx ) == lua_tonumber( L, idx );
}


/* check argument */
#define lauxh_argerror(L, idx, fmt, ... ) do{ \
    char _errstr[255]; \
    snprintf( _errstr, 255, (fmt), ##__VA_ARGS__ ); \
    luaL_argerror((L), (idx), _errstr); \
}while(0)


#define lauxh_argcheck(L, cond, idx, fmt, ... ) do{ \
    if(!(cond)){ \
        lauxh_argerror( (L), (idx), (fmt), ##__VA_ARGS__ ); \
    }\
}while(0)


/* string argument */

static inline const char *lauxh_checklstring( lua_State *L, int idx,
                                              size_t *len )
{
    luaL_checktype( L, idx, LUA_TSTRING );
    return lua_tolstring( L, idx, len );
}


static inline const char *lauxh_optlstring( lua_State *L, int idx,
                                            const char *def, size_t *len )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checklstring( L, idx, len );
}


static inline const char *lauxh_checkstring( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TSTRING );
    return lua_tostring( L, idx );
}


static inline const char *lauxh_optstring( lua_State *L, int idx,
                                           const char *def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkstring( L, idx );
}



/* number/integer argument */

static inline lua_Number lauxh_checknumber( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TNUMBER );
    return lua_tonumber( L, idx );
}


static inline lua_Number lauxh_optnumber( lua_State *L, int idx,
                                          lua_Number def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checknumber( L, idx );
}


static inline lua_Integer lauxh_checkinteger( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger(L, idx), idx,
                    "integer expected, got number" );

    return (lua_Integer)v;
}


static inline lua_Integer lauxh_optinteger( lua_State *L, int idx,
                                            lua_Integer def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkinteger( L, idx );
}


static inline int8_t lauxh_checkint8( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= INT8_MIN && v <= INT8_MAX,
                    idx, "int8_t expected, got an out of range value" );

    return (uint8_t)v;
}


static inline int8_t lauxh_optint8( lua_State *L, int idx, int8_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkint8( L, idx );
}


static inline uint8_t lauxh_checkuint8( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= 0 && v <= UINT8_MAX,
                    idx, "uint8_t expected, got an out of range value" );

    return (uint8_t)v;
}


static inline uint8_t lauxh_optuint8( lua_State *L, int idx, uint8_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkuint8( L, idx );
}


static inline int16_t lauxh_checkint16( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= INT16_MIN && v <= INT16_MAX,
                    idx, "int16_t expected, got an out of range value" );

    return (int16_t)v;
}


static inline int16_t lauxh_optint16( lua_State *L, int idx, int16_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkint16( L, idx );
}


static inline uint16_t lauxh_checkuint16( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= 0 && v <= UINT16_MAX,
                    idx, "uint16_t expected, got an out of range value" );

    return (uint16_t)v;
}


static inline uint16_t lauxh_optuint16( lua_State *L, int idx, uint16_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkuint16( L, idx );
}


static inline int32_t lauxh_checkint32( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= INT32_MIN && v <= INT32_MAX,
                    idx, "int32_t expected, got an out of range value" );

    return (int32_t)v;
}


static inline int32_t lauxh_optint32( lua_State *L, int idx, int32_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkint32( L, idx );
}


static inline uint32_t lauxh_checkuint32( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= 0 && v <= UINT32_MAX,
                    idx, "uint32_t expected, got an out of range value" );

    return (uint32_t)v;
}


static inline uint32_t lauxh_optuint32( lua_State *L, int idx, uint32_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkuint32( L, idx );
}


static inline int64_t lauxh_checkint64( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= INT64_MIN && v <= INT64_MAX,
                    idx, "int64_t expected, got an out of range value" );

    return (int64_t)v;
}


static inline int64_t lauxh_optint64( lua_State *L, int idx, int64_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkint64( L, idx );
}


static inline uint64_t lauxh_checkuint64( lua_State *L, int idx )
{
    lua_Number v = lauxh_checknumber( L, idx );

    lauxh_argcheck( L, v == (lua_Number)lua_tointeger( L, idx ) &&
                    v >= 0 && v <= UINT64_MAX,
                    idx, "uint64_t expected, got an out of range value" );

    return (uint64_t)v;
}


static inline uint64_t lauxh_optuint64( lua_State *L, int idx, uint64_t def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }
    return lauxh_checkuint64( L, idx );
}



/* boolean argument */

static inline int lauxh_checkboolean( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TBOOLEAN );
    return lua_toboolean( L, idx );
}


static inline int lauxh_optboolean( lua_State *L, int idx, int def )
{
    if( lauxh_isnil( L, idx ) ){
        return def != 0;
    }
    return lauxh_checkboolean( L, idx );
}



/* table argument */

static inline void lauxh_checktable( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTABLE );
}


static inline void lauxh_checktableof( lua_State *L, int idx, const char *k )
{
    lua_pushstring( L, k );
    lua_rawget( L, idx );
    lauxh_checktable( L, -1 );
}


static inline const char *lauxh_checklstringof( lua_State *L, int idx,
                                                const char *k, size_t *len )
{
    const char *v = NULL;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_checklstring( L, -1, len );
    lua_pop( L, 1 );

    return v;
}


static inline const char *lauxh_optlstringof( lua_State *L, int idx,
                                              const char *k, const char *def,
                                              size_t *len )
{
    const char *v = NULL;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_optlstring( L, -1, def, len );
    lua_pop( L, 1 );

    return v;
}


static inline const char *lauxh_checkstringof( lua_State *L, int idx,
                                               const char *k )
{
    const char *v = NULL;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_checkstring( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline const char *lauxh_optstringof( lua_State *L, int idx,
                                             const char *k, const char *def )
{
    const char *v = NULL;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_optstring( L, -1, def );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Number lauxh_checknumberof( lua_State *L, int idx,
                                              const char *k )
{
    lua_Number v = 0;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_checknumber( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Number lauxh_optnumberof( lua_State *L, int idx,
                                            const char *k, lua_Number def )
{
    lua_Number v = 0;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_optnumber( L, -1, def );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Integer lauxh_checkintegerof( lua_State *L, int idx,
                                                const char *k )
{
    lua_Integer v = 0;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_checkinteger( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Integer lauxh_optintegerof( lua_State *L, int idx,
                                              const char *k, lua_Integer def )
{
    lua_Integer v = 0;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_optinteger( L, -1, def );
    lua_pop( L, 1 );

    return v;
}


static inline int lauxh_checkbooleanof( lua_State *L, int idx, const char *k )
{
    int v = 0;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_checkboolean( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline int lauxh_optbooleanof( lua_State *L, int idx, const char *k, int def )
{
    int v = 0;

    lua_pushstring( L, k );
    lua_rawget( L, idx );
    v = lauxh_optboolean( L, -1, def );
    lua_pop( L, 1 );

    return v;
}



// table as array

static inline void lauxh_checktableat( lua_State *L, int idx, int row )
{
    lua_rawgeti( L, idx, row );
    lauxh_checktable( L, -1 );
}


static inline const char *lauxh_checklstringat( lua_State *L, int idx, int row,
                                                size_t *len )
{
    const char *v = NULL;

    lua_rawgeti( L, idx, row );
    v = lauxh_checklstring( L, -1, len );
    lua_pop( L, 1 );

    return v;
}


static inline const char *lauxh_optlstringat( lua_State *L, int idx, int row,
                                              const char *def, size_t *len )
{
    const char *v = NULL;

    lua_rawgeti( L, idx, row );
    v = lauxh_optlstring( L, -1, def, len );
    lua_pop( L, 1 );

    return v;
}


static inline const char *lauxh_checkstringat( lua_State *L, int idx, int row )
{
    const char *v = NULL;

    lua_rawgeti( L, idx, row );
    v = lauxh_checkstring( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline const char *lauxh_optstringat( lua_State *L, int idx, int row,
                                             const char *def )
{
    const char *v = NULL;

    lua_rawgeti( L, idx, row );
    v = lauxh_optstring( L, -1, def );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Number lauxh_checknumberat( lua_State *L, int idx, int row )
{
    lua_Number v = 0;

    lua_rawgeti( L, idx, row );
    v = lauxh_checknumber( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Number lauxh_optnumberat( lua_State *L, int idx, int row,
                                            lua_Number def )
{
    lua_Number v = 0;

    lua_rawgeti( L, idx, row );
    v = lauxh_optnumber( L, -1, def );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Integer lauxh_checkintegerat( lua_State *L, int idx, int row )
{
    lua_Integer v = 0;

    lua_rawgeti( L, idx, row );
    v = lauxh_checkinteger( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline lua_Integer lauxh_optintegerat( lua_State *L, int idx, int row,
                                              lua_Integer def )
{
    lua_Integer v = 0;

    lua_rawgeti( L, idx, row );
    v = lauxh_optinteger( L, -1, def );
    lua_pop( L, 1 );

    return v;
}


static inline int lauxh_checkbooleanat( lua_State *L, int idx, int row )
{
    int v = 0;

    lua_rawgeti( L, idx, row );
    v = lauxh_checkboolean( L, -1 );
    lua_pop( L, 1 );

    return v;
}


static inline int lauxh_optbooleanat( lua_State *L, int idx, int row, int def )
{
    int v = 0;

    lua_rawgeti( L, idx, row );
    v = lauxh_optboolean( L, -1, def );
    lua_pop( L, 1 );

    return v;
}


/* thread argument */

static inline lua_State *lauxh_checkthread( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TTHREAD );
    return lua_tothread( L, idx );
}



/* function argument */

static inline void lauxh_checkfunction( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TFUNCTION );
}



/* cfunction argument */

static inline lua_CFunction lauxh_checkcfunction( lua_State *L, int idx )
{
    lauxh_argcheck( L, lua_iscfunction( L, idx ), idx,
                    "cfunction expected, got %s", lauxh_typenameat( L, idx ) );

    return lua_tocfunction( L, idx );
}



/* lightuserdata argument */

static inline const void *lauxh_checkpointer( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TLIGHTUSERDATA );
    return lua_topointer( L, idx );
}



/* userdata argument */

static inline void *lauxh_checkudata( lua_State *L, int idx, const char *tname )
{
    return luaL_checkudata( L, idx, tname );
}


static inline void *lauxh_optudata( lua_State *L, int idx, const char *tname,
                                    void *def )
{
    if( lauxh_isnil( L, idx ) ){
        return def;
    }

    return lauxh_checkudata( L, idx, tname );
}


static inline int lauxh_isuserdataof( lua_State *L, int idx, const char *tname )
{
    int rc = 0;

    // there is userdata wrapped by metatable
    if( lauxh_isuserdata( L, idx ) && lua_getmetatable( L, idx ) ){
        // get metatable from registry
        lua_pushstring( L, tname );
        lua_rawget( L, LUA_REGISTRYINDEX );
        // compare
        rc = lua_rawequal( L, -1, -2 );

        lua_pop( L, 2 );
    }

    return rc;
}


/* file argument */

static inline FILE *lauxh_checkfile( lua_State *L, int idx )
{
#if LUA_VERSION_NUM >= 502
    luaL_Stream *stream = luaL_checkudata(L, idx, LUA_FILEHANDLE );
    return stream->f;

#else
    return *(FILE**)luaL_checkudata(L, idx, LUA_FILEHANDLE );
#endif
}



/* flag arguments */

static inline lua_Integer lauxh_optflags( lua_State *L, int idx )
{
    const int argc = lua_gettop( L );
    lua_Integer flg = 0;

    for(; idx <= argc; idx++ ){
        flg |= lauxh_optinteger( L, idx, 0 );
    }

    return flg;
}



/* traceback */

static inline int lauxh_traceback( lua_State *L, lua_State *src,
                                   const char *msg, int lv )
{
#if LUA_VERSION_NUM >= 502
    // push src stack trace to dst state
    luaL_traceback( ( L ) ? L : src, src, msg, lv );
    return 1;

#else
    // avoid warnings for unused variables
    (void)lv;

    // get debug module
    lua_pushliteral( src, "debug" );
    lua_rawget( src, LUA_GLOBALSINDEX );
    if( lauxh_istable( src, -1 ) )
    {
        // get traceback function
        lua_pushliteral( src, "traceback" );
        lua_rawget( src, -2 );
        if( lauxh_isfunction( src, -1 ) )
        {
            int argc = 0;

            // remove debug module table
            lua_replace( src, -2 );

            // add msg argument to debug.traceback
            if( msg ){
                lua_pushstring( src, msg );
                argc = 1;
            }

            // call debug.traceback function
            if( lua_pcall( src, argc, 1, 0 ) == 0 )
            {
                if( L ){
                    lua_xmove( src, L, 1 );
                }
                return 1;
            }

            // faillure
            return -1;
        }
    }

    return 0;
#endif
}



/* copy a value to different state */

static inline int lauxh_xcopy( lua_State *from, lua_State *to, int idx,
                               const int allow_nil )
{
    switch( lua_type( from, idx ) )
    {
        case LUA_TBOOLEAN:
            lua_pushboolean( to, lua_toboolean( from, idx ) );
            return LUA_TBOOLEAN;

        case LUA_TLIGHTUSERDATA:
            lua_pushlightuserdata( to, lua_touserdata( from, idx ) );
            return LUA_TLIGHTUSERDATA;

        case LUA_TNUMBER:
            lua_pushnumber( to, lua_tonumber( from, idx ) );
            return LUA_TNUMBER;

        case LUA_TSTRING: {
            size_t len = 0;
            const char *str = lua_tolstring( from, idx, &len );
            lua_pushlstring( to, str, len );
            return LUA_TSTRING;
        }

        case LUA_TTABLE:
            lua_newtable( to );
            // to positive number
            if( idx < 0 ){
                idx = lua_gettop( from ) + idx + 1;
            }
            lua_pushnil( from );
            while( lua_next( from, idx ) )
            {
                if( lauxh_xcopy( from, to, -2, 0 ) != LUA_TNONE )
                {
                    if( lauxh_xcopy( from, to, -1, 0 ) != LUA_TNONE ){
                        lua_rawset( to, -3 );
                    }
                    else {
                        lua_pop( to, 1 );
                    }
                }
                lua_pop( from, 1 );
            }
            return LUA_TTABLE;

        case LUA_TNIL:
            if( allow_nil ){
                lua_pushnil( to );
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


#if LUA_VERSION_NUM >= 502
#   define lauxh_resume( L, from, narg )    lua_resume( L, from, narg )
#else
#   define lauxh_resume( L, from, narg )    lua_resume( L, narg )
#endif


static inline void lauxh_pushbuffer( lua_State *L, size_t n )
{
#if LUA_VERSION_NUM >= 502
    luaL_Buffer B;

    luaL_buffinitsize( L, &B, (size_t)n );
    luaL_addsize( &B, (size_t)n );
    luaL_pushresult( &B );

#else
    const char *buf = malloc( n );

    if( !buf ){
        luaL_error( L, strerror( errno ) );
    }
    lua_pushlstring( L, buf, n );
    free( (void*)buf );

#endif
}


#endif

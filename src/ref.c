/**
 *  Copyright (C) 2022 Masatoshi Fukunaga
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
 */

#define LAUXHLIB_USED_IN_LUA
#include "lauxhlib.h"

#define LAUXHLIB_REF_MT "lauxhlib.ref"

static int get_lua(lua_State *L)
{
    int *ref = (int *)lauxh_checkudata(L, 1, LAUXHLIB_REF_MT);
    lua_settop(L, 1);
    lauxh_pushref(L, *ref);
    return 1;
}

static int unref_lua(lua_State *L)
{
    int *ref = (int *)lauxh_checkudata(L, 1, LAUXHLIB_REF_MT);
    if (lauxh_isref(*ref)) {
        *ref = lauxh_unref(L, *ref);
        lua_pushboolean(L, 1);
        return 1;
    }
    lua_pushboolean(L, 0);
    return 1;
}

static int tostring_lua(lua_State *L)
{
    int *ref = (int *)lauxh_checkudata(L, 1, LAUXHLIB_REF_MT);
    lua_pushfstring(L, LAUXHLIB_REF_MT ": %d", *ref);
    return 1;
}

static int gc_lua(lua_State *L)
{
    int *ref = (int *)lauxh_checkudata(L, 1, LAUXHLIB_REF_MT);
    lauxh_unref(L, *ref);
    return 0;
}

static int ref_lua(lua_State *L)
{
    int ref = LUA_NOREF;

    lua_settop(L, 1);
    ref                                       = lauxh_ref(L);
    *((int *)lua_newuserdata(L, sizeof(int))) = ref;
    lauxh_setmetatable(L, LAUXHLIB_REF_MT);
    return 1;
}

static void create_mt(lua_State *L)
{
    struct luaL_Reg mmethod[] = {
        {"__gc",       gc_lua      },
        {"__tostring", tostring_lua},
        {NULL,         NULL        }
    };
    struct luaL_Reg method[] = {
        {"unref", unref_lua},
        {"get",   get_lua  },
        {NULL,    NULL     }
    };

    luaL_newmetatable(L, LAUXHLIB_REF_MT);
    for (struct luaL_Reg *ptr = mmethod; ptr->name; ptr++) {
        lauxh_pushfn2tbl(L, ptr->name, ptr->func);
    }
    lua_newtable(L);
    for (struct luaL_Reg *ptr = method; ptr->name; ptr++) {
        lauxh_pushfn2tbl(L, ptr->name, ptr->func);
    }
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

LUALIB_API int luaopen_lauxhlib_ref(lua_State *L)
{
    create_mt(L);
    lua_pushcfunction(L, ref_lua);
    return 1;
}

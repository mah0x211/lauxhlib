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
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// lua
#define LAUXHLIB_USED_IN_LUA
#include "lauxhlib.h"

static int fileno_lua(lua_State *L)
{
    lua_settop(L, 1);
    lua_pushinteger(L, lauxh_fileno(L, 1));
    return 1;
}

static int tofile_lua(lua_State *L)
{
    int top           = lua_gettop(L);
    int fd            = lauxh_checkint(L, 1);
    const char *mode  = lauxh_optstr(L, 2, "r");
    const char *fname = lauxh_optstr(L, 3, NULL);

    switch (top) {
    case 1:
    case 2:
    case 3:
        break;
    default:
        top = 3;
        lua_settop(L, 3);
    }

    if (lauxh_tofile(L, fd, mode, fname)) {
        return 1;
    }
    return lua_gettop(L) - top;
}

LUALIB_API int luaopen_lauxhlib_file(lua_State *L)
{
    struct luaL_Reg method[] = {
        {"tofile", tofile_lua},
        {"fileno", fileno_lua},
        {NULL,     NULL      }
    };

    lua_newtable(L);
    for (struct luaL_Reg *ptr = method; ptr->name; ptr++) {
        lauxh_pushfn2tbl(L, ptr->name, ptr->func);
    }

    return 1;
}

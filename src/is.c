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

#define RET_BOOLEAN(fn, ...)                                                   \
    do {                                                                       \
        lua_pushboolean(L, fn(L, 1, ##__VA_ARGS__));                           \
        return 1;                                                              \
    } while (0)

static int pint64_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_ispint64);
}

static int pint32_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_ispint32);
}

static int pint16_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_ispint16);
}

static int pint8_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_ispint8);
}

static int uint64_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isuint64);
}

static int uint32_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isuint32);
}

static int uint16_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isuint16);
}

static int uint8_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isuint8);
}

static int int64_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isint64);
}

static int int32_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isint32);
}

static int int16_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isint16);
}

static int int8_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isint8);
}

#define is_numtypeof(L, numtype, typename)                                     \
    do {                                                                       \
        if (lua_isnoneornil(L, 2)) {                                           \
            if (lua_isnoneornil(L, 3)) {                                       \
                RET_BOOLEAN(lauxh_is##typename);                               \
            }                                                                  \
            numtype max = lauxh_check##typename(L, 3);                         \
            RET_BOOLEAN(lauxh_is##typename##_le, max);                         \
        } else if (lua_isnoneornil(L, 3)) {                                    \
            numtype min = lauxh_check##typename(L, 2);                         \
            RET_BOOLEAN(lauxh_is##typename##_ge, min);                         \
        }                                                                      \
        numtype min = lauxh_check##typename(L, 2);                             \
        numtype max = lauxh_check##typename(L, 3);                             \
        RET_BOOLEAN(lauxh_is##typename##_in_range, min, max);                  \
    } while (0)

static int pint_lua(lua_State *L)
{
    is_numtypeof(L, uint64_t, pint);
}

static int uint_lua(lua_State *L)
{
    is_numtypeof(L, uint64_t, uint);
}

static int int_lua(lua_State *L)
{
    is_numtypeof(L, lua_Integer, int);
}

static int unsigned_lua(lua_State *L)
{
    is_numtypeof(L, lua_Number, unsigned);
}

static int finite_lua(lua_State *L)
{
    is_numtypeof(L, lua_Number, finite);
}

static int num_lua(lua_State *L)
{
    is_numtypeof(L, lua_Number, num);
}

#undef is_numtypeof

static int callable_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_iscallable);
}

static int file_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isfile);
}

static int thread_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isthread);
}

static int userdata_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isuserdata);
}

static int cfunc_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_iscfunc);
}

static int func_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isfunc);
}

static int table_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_istable);
}

static int str_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isstr);
}

static int pointer_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_ispointer);
}

static int bool_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isbool);
}

static int none_lua(lua_State *L)
{
    RET_BOOLEAN(lauxh_isnil);
}

#undef RET_BOOLEAN

LUALIB_API int luaopen_lauxhlib_is(lua_State *L)
{
    struct luaL_Reg funcs[] = {
        {"none",     none_lua    },
        {"bool",     bool_lua    },
        {"pointer",  pointer_lua },
        {"str",      str_lua     },
        {"table",    table_lua   },
        {"func",     func_lua    },
        {"cfunc",    cfunc_lua   },
        {"userdata", userdata_lua},
        {"thread",   thread_lua  },
        {"file",     file_lua    },
        {"callable", callable_lua},
        {"num",      num_lua     },
        {"unsigned", unsigned_lua},
        {"finite",   finite_lua  },
        {"int",      int_lua     },
        {"uint",     uint_lua    },
        {"pint",     pint_lua    },
        {"int8",     int8_lua    },
        {"int16",    int16_lua   },
        {"int32",    int32_lua   },
        {"int64",    int64_lua   },
        {"uint8",    uint8_lua   },
        {"uint16",   uint16_lua  },
        {"uint32",   uint32_lua  },
        {"uint64",   uint64_lua  },
        {"pint8",    pint8_lua   },
        {"pint16",   pint16_lua  },
        {"pint32",   pint32_lua  },
        {"pint64",   pint64_lua  },
        {NULL,       NULL        }
    };

    lua_newtable(L);
    for (struct luaL_Reg *ptr = funcs; ptr->name; ptr++) {
        lauxh_pushfn2tbl(L, ptr->name, ptr->func);
    }

    return 1;
}

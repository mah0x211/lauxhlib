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

#define CHECK_ERROPTS(name_idx, stacklv_idx)                                   \
    do {                                                                       \
        typeof(LAUXH_ARGERR_NAME) name = LAUXH_ARGERR_NAME;                    \
        typeof(LAUXH_ARGERR_INDEX) idx = LAUXH_ARGERR_INDEX;                   \
        switch (lua_type(L, (name_idx))) {                                     \
        case LUA_TNONE:                                                        \
        case LUA_TNIL:                                                         \
            break;                                                             \
        case LUA_TSTRING:                                                      \
            name = lauxh_checkstring(L, (name_idx));                           \
            break;                                                             \
        default:                                                               \
            idx = lauxh_checkpint(L, (name_idx));                              \
        }                                                                      \
        LAUXH_ARGERR_STACK = lauxh_optuint(L, (stacklv_idx), 1);               \
        LAUXH_ARGERR_NAME  = name;                                             \
        LAUXH_ARGERR_INDEX = idx;                                              \
    } while (0)

#define CHECK_VALUE(checkfn)                                                   \
    do {                                                                       \
        CHECK_ERROPTS(2, 3);                                                   \
        checkfn(L, 1);                                                         \
        lua_settop(L, 1);                                                      \
        return 1;                                                              \
    } while (0)

static int none_lua(lua_State *L)
{
    if (!lua_isnoneornil(L, 1)) {
        CHECK_ERROPTS(2, 3);
        lauxh_checktype(L, 1, LUA_TNIL);
    }
    lua_settop(L, 0);
    lua_pushnil(L);
    return 1;
}

static int flags_lua(lua_State *L)
{
    uint64_t flags = lauxh_optflags(L, 1);
    lua_settop(L, 0);
    lua_pushinteger(L, flags);
    return 1;
}

static int callable_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkcallable);
}

static int file_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkfile);
}

static int uint64_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkuint64);
}

static int uint32_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkuint32);
}

static int uint16_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkuint16);
}

static int uint8_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkuint8);
}

static int int64_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkint64);
}

static int int32_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkint32);
}

static int int16_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkint16);
}

static int int8_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkint8);
}

static int pint_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkpint);
}

static int uint_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkuint);
}

static int unsigned_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkunsigned);
}

#define check_numtypeof(L, typename, numtype)                                  \
    do {                                                                       \
        if (lua_isnoneornil(L, 2)) {                                           \
            /* without min argument */                                         \
            if (lua_isnoneornil(L, 3)) {                                       \
                /* without min and max argument */                             \
                CHECK_ERROPTS(4, 5);                                           \
                lauxh_check##typename(L, 1);                                   \
                lua_settop(L, 1);                                              \
                return 1;                                                      \
            }                                                                  \
            /* with max argument */                                            \
            numtype max = lauxh_check##typename(L, 3);                         \
            CHECK_ERROPTS(4, 5);                                               \
            lauxh_check##typename##_le(L, 1, max);                             \
            lua_settop(L, 1);                                                  \
            return 1;                                                          \
        } else if (lua_isnoneornil(L, 3)) {                                    \
            /* with min argument */                                            \
            numtype min = lauxh_check##typename(L, 2);                         \
            CHECK_ERROPTS(4, 5);                                               \
            lauxh_check##typename##_ge(L, 1, min);                             \
            lua_settop(L, 1);                                                  \
            return 1;                                                          \
        }                                                                      \
        /* with min and max argument */                                        \
        numtype min = lauxh_check##typename(L, 2);                             \
        numtype max = lauxh_check##typename(L, 3);                             \
        CHECK_ERROPTS(4, 5);                                                   \
        lauxh_check##typename##_in_range(L, 1, min, max);                      \
        lua_settop(L, 1);                                                      \
        return 1;                                                              \
    } while (0)

static int finite_lua(lua_State *L)
{
    check_numtypeof(L, finite, lua_Number);
}

static int int_lua(lua_State *L)
{
    check_numtypeof(L, int, lua_Integer);
}

static int num_lua(lua_State *L)
{
    check_numtypeof(L, num, lua_Number);
}

#undef check_numtypeof

static int thread_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkthread);
}

static int userdata_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkuserdata);
}

static int cfunc_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkcfunc);
}

static int func_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkfunc);
}

static int table_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checktable);
}

static int str_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkstr);
}

static int pointer_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkpointer);
}

static int bool_lua(lua_State *L)
{
    CHECK_VALUE(lauxh_checkbool);
}

#undef CHECK_VALUE
#undef CHECK_ERROPTS

#ifdef __cplusplus
extern "C" {
#endif

LUALIB_API int luaopen_lauxhlib_check(lua_State *L)
{
    struct luaL_Reg method[] = {
        {"none",     none_lua    },
        {"bool",     bool_lua    },
        {"pointer",  pointer_lua },
        {"num",      num_lua     },
        {"str",      str_lua     },
        {"table",    table_lua   },
        {"func",     func_lua    },
        {"cfunc",    cfunc_lua   },
        {"userdata", userdata_lua},
        {"thread",   thread_lua  },
        {"finite",   finite_lua  },
        {"unsigned", unsigned_lua},
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
        {"file",     file_lua    },
        {"callable", callable_lua},
        {"flags",    flags_lua   },
        {NULL,       NULL        }
    };

    lua_newtable(L);
    for (struct luaL_Reg *ptr = method; ptr->name; ptr++) {
        lauxh_pushfn2tbl(L, ptr->name, ptr->func);
    }

    return 1;
}

#ifdef __cplusplus
}
#endif

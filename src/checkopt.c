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

#define CHECKARGS()                                                            \
    do {                                                                       \
        const char *name   = lauxh_optstr(L, 3, NULL);                         \
        int stack          = lauxh_optuint(L, 4, 1);                           \
        LAUXH_ARGERR_NAME  = name;                                             \
        LAUXH_ARGERR_STACK = stack;                                            \
    } while (0)

#define CHECKOPT(checkfn)                                                      \
    do {                                                                       \
        int top = lua_gettop(L);                                               \
        if (top == 0 || (top == 1 && lauxh_isnil(L, 1))) {                     \
            /* return nil if no argument */                                    \
            lua_pushnil(L);                                                    \
            return 1;                                                          \
        } else if (top == 1) {                                                 \
            /* check value if default value is not specified */                \
            checkfn(L, 1);                                                     \
            lua_settop(L, 1);                                                  \
            return 1;                                                          \
        } else if (!lauxh_isnil(L, 2)) {                                       \
            /* check default value */                                          \
            checkfn(L, 2);                                                     \
        }                                                                      \
                                                                               \
        CHECKARGS();                                                           \
        if (lauxh_isnil(L, 1)) {                                               \
            /* use second argument as default value */                         \
            lua_settop(L, 2);                                                  \
        } else {                                                               \
            checkfn(L, 1);                                                     \
            lua_settop(L, 1);                                                  \
        }                                                                      \
        return 1;                                                              \
    } while (0)

static int callable_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkcallable);
}

static int file_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkfile);
}

static int uint64_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkuint64);
}

static int uint32_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkuint32);
}

static int uint16_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkuint16);
}

static int uint8_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkuint8);
}

static int int64_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkint64);
}

static int int32_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkint32);
}

static int int16_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkint16);
}

static int int8_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkint8);
}

static int pint_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkpint);
}

static int uint_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkuint);
}

static int int_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkint);
}

static int unsigned_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkunsigned);
}

static int finite_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkfinite);
}

static int thread_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkthread);
}

static int userdata_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkuserdata);
}

static int cfunc_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkcfunc);
}

static int func_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkfunc);
}

static int table_lua(lua_State *L)
{
    CHECKOPT(lauxh_checktable);
}

static int str_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkstr);
}

static int num_lua(lua_State *L)
{
    CHECKOPT(lauxh_checknum);
}

static int pointer_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkpointer);
}

static int bool_lua(lua_State *L)
{
    CHECKOPT(lauxh_checkbool);
}

#undef CHECKOPT

LUALIB_API int luaopen_lauxhlib_checkopt(lua_State *L)
{
    struct luaL_Reg method[] = {
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
        {NULL,       NULL        }
    };

    lua_newtable(L);
    for (struct luaL_Reg *ptr = method; ptr->name; ptr++) {
        lauxh_pushfn2tbl(L, ptr->name, ptr->func);
    }

    return 1;
}

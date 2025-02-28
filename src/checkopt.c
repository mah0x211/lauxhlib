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
        switch (lua_type(L, name_idx)) {                                       \
        case LUA_TNONE:                                                        \
        case LUA_TNIL:                                                         \
            break;                                                             \
        case LUA_TSTRING:                                                      \
            name = lauxh_checkstring(L, name_idx);                             \
            break;                                                             \
        default:                                                               \
            idx = lauxh_checkpint(L, name_idx);                                \
        }                                                                      \
        LAUXH_ARGERR_STACK = lauxh_optuint(L, stacklv_idx, 1);                 \
        LAUXH_ARGERR_NAME  = name;                                             \
        LAUXH_ARGERR_INDEX = idx;                                              \
    } while (0)

static int file_lua(lua_State *L)
{
    FILE *optfp = NULL;
    FILE *fp    = NULL;

    if (!lua_isnoneornil(L, 2)) {
        optfp = lauxh_checkfile(L, 2);
    }

    CHECK_ERROPTS(3, 4);
    fp = lauxh_optfile(L, 1, optfp);
    if (!fp) {
        lua_pushnil(L);
    } else if (fp == optfp) {
        lua_settop(L, 2);
    } else {
        lua_settop(L, 1);
    }
    return 1;
}

#define CHECK_OPT_IDXVAL(checkdeffn, checkoptfn)                               \
    do {                                                                       \
        if (lua_isnoneornil(L, 2)) {                                           \
            /* without default value */                                        \
            CHECK_ERROPTS(3, 4);                                               \
            lua_settop(L, 1);                                                  \
            checkoptfn(L, 1, 0);                                               \
            return 1;                                                          \
        }                                                                      \
        /* with default value */                                               \
        checkdeffn(L, 2);                                                      \
        CHECK_ERROPTS(3, 4);                                                   \
        lua_settop(L, checkoptfn(L, 1, 2));                                    \
        return 1;                                                              \
    } while (0)

static int table_lua(lua_State *L)
{
    CHECK_OPT_IDXVAL(lauxh_checktable, lauxh_opttable);
}

static int callable_lua(lua_State *L)
{
    CHECK_OPT_IDXVAL(lauxh_checkcallable, lauxh_optcallable);
}

static int func_lua(lua_State *L)
{
    CHECK_OPT_IDXVAL(lauxh_checkfunc, lauxh_optfunc);
}

#undef CHECK_OPT_IDXVAL

#define CHECK_OPT_VAL_EX(name_idx, stacklv_idx, defval, checkdeffn,            \
                         checkoptfn)                                           \
    do {                                                                       \
        if (lua_isnoneornil(L, 2)) {                                           \
            /* without default value */                                        \
            CHECK_ERROPTS((name_idx), (stacklv_idx));                          \
            checkoptfn(L, 1, (defval));                                        \
            lua_settop(L, 1);                                                  \
            return 1;                                                          \
        }                                                                      \
        /* with default value */                                               \
        defval = checkdeffn(L, 2);                                             \
        if (lua_isnoneornil(L, 1)) {                                           \
            /* just return default value */                                    \
            lua_settop(L, 2);                                                  \
        }                                                                      \
        CHECK_ERROPTS((name_idx), (stacklv_idx));                              \
        if (checkoptfn(L, 1, (defval)) == (defval)) {                          \
            /* with no-value or same value of default value */                 \
            lua_settop(L, 2);                                                  \
        } else {                                                               \
            lua_settop(L, 1);                                                  \
        }                                                                      \
        return 1;                                                              \
    } while (0)

#define CHECK_OPT_VAL(defval, checkdeffn, checkoptfn)                          \
    CHECK_OPT_VAL_EX(3, 4, defval, checkdeffn, checkoptfn)

static int uint64_lua(lua_State *L)
{
    uint64_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkuint64, lauxh_optuint64);
}

static int int64_lua(lua_State *L)
{
    int64_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkint64, lauxh_optint64);
}

static int uint32_lua(lua_State *L)
{
    uint32_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkuint32, lauxh_optuint32);
}

static int int32_lua(lua_State *L)
{
    int32_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkint32, lauxh_optint32);
}

static int uint16_lua(lua_State *L)
{
    uint16_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkuint16, lauxh_optuint16);
}

static int int16_lua(lua_State *L)
{
    int16_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkint16, lauxh_optint16);
}

static int uint8_lua(lua_State *L)
{
    uint8_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkuint8, lauxh_optuint8);
}

static int int8_lua(lua_State *L)
{
    int8_t def = 0;
    CHECK_OPT_VAL(def, lauxh_checkint8, lauxh_optint8);
}

static int pint_lua(lua_State *L)
{
    lua_Integer def = 0;
    CHECK_OPT_VAL(def, lauxh_checkpint, lauxh_optpint);
}

static int thread_lua(lua_State *L)
{
    lua_State *def = NULL;
    CHECK_OPT_VAL(def, lauxh_checkthread, lauxh_optthread);
}

static int cfunc_lua(lua_State *L)
{
    lua_CFunction def = NULL;
    CHECK_OPT_VAL(def, lauxh_checkcfunc, lauxh_optcfunc);
}

static int str_lua(lua_State *L)
{
    const char *def = NULL;
    CHECK_OPT_VAL(def, lauxh_checkstr, lauxh_optstr);
}

static int userdata_lua(lua_State *L)
{
    void *def = NULL;
    CHECK_OPT_VAL(def, lauxh_checkuserdata, lauxh_optuserdata);
}

static int pointer_lua(lua_State *L)
{
    const void *def = NULL;
    CHECK_OPT_VAL(def, lauxh_checkpointer, lauxh_optpointer);
}

static int bool_lua(lua_State *L)
{
    int def = 0;
    CHECK_OPT_VAL(def, lauxh_checkbool, lauxh_optbool);
}

static int uint_lua(lua_State *L)
{
    lua_Integer def = 0;
    CHECK_OPT_VAL(def, lauxh_checkuint, lauxh_optuint);
}

static int unsigned_lua(lua_State *L)
{
    lua_Number def = 0;
    CHECK_OPT_VAL(def, lauxh_checkunsigned, lauxh_optunsigned);
}

#undef CHECK_OPT_VAL

#define CHECK_OPT_RANGEVAL_GLE(defval, checkdeffn, checkoptfn, n)              \
    do {                                                                       \
        if (lua_isnoneornil(L, 2)) {                                           \
            /* without default value */                                        \
            CHECK_ERROPTS(5, 6);                                               \
            checkoptfn(L, 1, (n), (defval));                                   \
            lua_settop(L, 1);                                                  \
            return 1;                                                          \
        }                                                                      \
        /* with default value */                                               \
        defval = checkdeffn(L, 2);                                             \
        CHECK_ERROPTS(5, 6);                                                   \
        if (checkoptfn(L, 1, (n), (defval)) == (defval)) {                     \
            /* with no-value or same value of default value */                 \
            lua_settop(L, 2);                                                  \
        } else {                                                               \
            lua_settop(L, 1);                                                  \
        }                                                                      \
        return 1;                                                              \
    } while (0)

#define CHECK_OPT_RANGEVAL(def, checkdeffn, checkoptfn)                        \
    do {                                                                       \
        if (lua_isnoneornil(L, 4)) {                                           \
            if (lua_isnoneornil(L, 3)) {                                       \
                /* without min and max argument */                             \
                CHECK_OPT_VAL_EX(5, 6, def, checkdeffn, checkoptfn);           \
            }                                                                  \
            typeof(def) min = checkdeffn(L, 3);                                \
            CHECK_OPT_RANGEVAL_GLE(def, checkdeffn, checkoptfn##_ge, min);     \
        } else if (lua_isnoneornil(L, 3)) {                                    \
            if (lua_isnoneornil(L, 4)) {                                       \
                /* without min and max argument */                             \
                CHECK_OPT_VAL_EX(5, 6, def, checkdeffn, checkoptfn);           \
            }                                                                  \
            typeof(def) max = checkdeffn(L, 4);                                \
            CHECK_OPT_RANGEVAL_GLE(def, checkdeffn, checkoptfn##_le, max);     \
        }                                                                      \
        /* with min and max argument */                                        \
        typeof(def) min = checkdeffn(L, 3);                                    \
        typeof(def) max = checkdeffn(L, 4);                                    \
        if (lua_isnoneornil(L, 2)) {                                           \
            /* without default value */                                        \
            CHECK_ERROPTS(5, 6);                                               \
            checkoptfn##_in_range(L, 1, min, max, (def));                      \
            lua_settop(L, 1);                                                  \
            return 1;                                                          \
        }                                                                      \
        /* with default value */                                               \
        def = checkdeffn(L, 2);                                                \
        CHECK_ERROPTS(5, 6);                                                   \
        if (checkoptfn##_in_range(L, 1, min, max, (def)) == (def)) {           \
            /* with no-value or same value of default value */                 \
            lua_settop(L, 2);                                                  \
        } else {                                                               \
            lua_settop(L, 1);                                                  \
        }                                                                      \
        return 1;                                                              \
    } while (0)

static int int_lua(lua_State *L)
{
    lua_Integer def = 0;
    CHECK_OPT_RANGEVAL(def, lauxh_checkint, lauxh_optint);
}

static int finite_lua(lua_State *L)
{
    lua_Number def = 0;
    CHECK_OPT_RANGEVAL(def, lauxh_checkfinite, lauxh_optfinite);
}

static int num_lua(lua_State *L)
{
    lua_Number def = 0;
    CHECK_OPT_RANGEVAL(def, lauxh_checknum, lauxh_optnum);
}

#undef CHECK_OPT_RANGEVAL_GLE
#undef CHECK_OPT_RANGEVAL
#undef CHECK_OPT_VAL_EX
#undef CHECK_ERROPTS

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

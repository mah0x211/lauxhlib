#include "lauxhlib.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int cfunction(lua_State *L)
{
    (void)L;
    return 0;
}

static int test_array(lua_State *L)
{
    size_t len = 0;

    lua_newtable(L);

    // string
    lauxh_pushstr2arr(L, 2, "string");
    assert(lauxh_checkstringat(L, 1, 2));
    assert(lauxh_checklstringat(L, 1, 2, &len));
    assert(len == 6);
    // optstring
    len = 0;
    assert(lauxh_optstringat(L, 1, 2, NULL));
    assert(lauxh_optlstringat(L, 1, 2, NULL, &len));
    assert(len == 6);
    len = 0;
    lauxh_pushnil2arr(L, 2);
    assert(lauxh_optlstringat(L, 1, 2, NULL, &len) == NULL);
    assert(len == 0);

    // number
    lauxh_pushnum2arr(L, 2, 1.1);
    assert(lauxh_checknumberat(L, 1, 2) == 1.1);
    // optnumber
    assert(lauxh_optnumberat(L, 1, 2, 0) == 1.1);
    lauxh_pushnil2arr(L, 2);
    assert(lauxh_optnumberat(L, 1, 2, 0) == 0);

    // integer
    lauxh_pushint2arr(L, 2, 1);
    assert(lauxh_checkintegerat(L, 1, 2) == 1);
    // optnumber
    assert(lauxh_optintegerat(L, 1, 2, 0) == 1);
    lauxh_pushnil2arr(L, 2);
    assert(lauxh_optintegerat(L, 1, 2, 0) == 0);

    // boolean
    lauxh_pushbool2arr(L, 2, 1);
    assert(lauxh_checkbooleanat(L, 1, 2) == 1);
    // optboolean
    lauxh_pushnil2arr(L, 2);
    assert(lauxh_optbooleanat(L, 1, 2, 0) == 0);

    // push table
    lua_newtable(L);
    lauxh_pushbool2arr(L, 1, 1);
    lauxh_pushstr2arr(L, 2, "string");
    lua_rawseti(L, -2, 2);

    // table
    lauxh_checktableat(L, 1, 2);
    assert(lauxh_rawlen(L, -1) == 2);

    return 0;
}

static int test_arrayat(lua_State *L)
{
    size_t len = 0;
    int at     = 0;

    lua_newtable(L);
    at = lua_gettop(L);
    lua_pushnil(L);
    lua_pushnil(L);

    // string
    len = 0;
    lauxh_pushstr2arrat(L, 2, "string", at);
    assert(lauxh_checkstringat(L, at, 2));
    assert(lauxh_checklstringat(L, at, 2, &len));
    assert(len == 6);
    // optstring
    len = 0;
    assert(lauxh_optstringat(L, at, 2, NULL));
    assert(lauxh_optlstringat(L, at, 2, NULL, &len));
    assert(len == 6);
    len = 0;
    lauxh_pushnil2arrat(L, 2, at);
    assert(lauxh_optlstringat(L, at, 2, NULL, &len) == NULL);
    assert(len == 0);

    // number
    lauxh_pushnum2arrat(L, 2, 1.1, at);
    assert(lauxh_checknumberat(L, at, 2) == 1.1);
    // optnumber
    assert(lauxh_optnumberat(L, at, 2, 0) == 1.1);
    lauxh_pushnil2arrat(L, 2, at);
    assert(lauxh_optnumberat(L, at, 2, 0) == 0);

    // integer
    lauxh_pushint2arrat(L, 2, 1, at);
    assert(lauxh_checkintegerat(L, at, 2) == 1);
    // optnumber
    assert(lauxh_optintegerat(L, at, 2, 0) == 1);
    lauxh_pushnil2arrat(L, 2, at);
    assert(lauxh_optintegerat(L, at, 2, 0) == 0);

    // boolean
    lauxh_pushbool2arrat(L, 2, 1, at);
    assert(lauxh_checkbooleanat(L, at, 2) == 1);
    // optboolean
    lauxh_pushnil2arrat(L, 2, at);
    assert(lauxh_optbooleanat(L, at, 2, 0) == 0);

    // push table
    lua_newtable(L);
    lauxh_pushbool2arr(L, 1, 1);
    lauxh_pushstr2arr(L, 2, "string");
    lua_rawseti(L, at, 2);

    assert(lua_gettop(L) == 3);
    assert(lua_type(L, 1) == LUA_TTABLE);
    assert(lua_type(L, 2) == LUA_TNIL);
    assert(lua_type(L, 3) == LUA_TNIL);

    // table
    lauxh_checktableat(L, at, 2);
    assert(lauxh_rawlen(L, -1) == 2);
    assert(lauxh_checkbooleanat(L, -1, 1) == 1);
    lauxh_checkstringat(L, -1, 2);

    return 0;
}

static int test_table(lua_State *L)
{
    size_t len = 0;

    lua_newtable(L);

    // string
    lauxh_pushstr2tbl(L, "key", "string");
    assert(lauxh_checkstringof(L, 1, "key"));
    assert(lauxh_checklstringof(L, 1, "key", &len));
    assert(len == 6);
    // optstring
    len = 0;
    assert(lauxh_optstringof(L, 1, "key", NULL));
    assert(lauxh_optlstringof(L, 1, "key", NULL, &len));
    assert(len == 6);
    len = 0;
    lauxh_pushnil2tbl(L, "key");
    assert(lauxh_optlstringof(L, 1, "key", NULL, &len) == NULL);
    assert(len == 0);

    // number
    lauxh_pushnum2tbl(L, "key", 1.1);
    assert(lauxh_checknumberof(L, 1, "key") == 1.1);
    // optnumber
    assert(lauxh_optnumberof(L, 1, "key", 0) == 1.1);
    lauxh_pushnil2tbl(L, "key");
    assert(lauxh_optnumberof(L, 1, "key", 0) == 0);

    // integer
    lauxh_pushint2tbl(L, "key", 1);
    assert(lauxh_checkintegerof(L, 1, "key") == 1);
    // optnumber
    assert(lauxh_optintegerof(L, 1, "key", 0) == 1);
    lauxh_pushnil2tbl(L, "key");
    assert(lauxh_optintegerof(L, 1, "key", 0) == 0);

    // boolean
    lauxh_pushbool2tbl(L, "key", 1);
    assert(lauxh_checkbooleanof(L, 1, "key") == 1);
    // optboolean
    lauxh_pushnil2tbl(L, "key");
    assert(lauxh_optbooleanof(L, 1, "key", 0) == 0);

    // push table
    lua_pushstring(L, "key");
    lua_newtable(L);
    lauxh_pushbool2arr(L, 1, 1);
    lua_rawset(L, -3);

    // table
    lauxh_checktableof(L, 1, "key");
    assert(lauxh_rawlen(L, -1) == 1);

    return 0;
}

static int test_tableat(lua_State *L)
{
    size_t len = 0;
    int at     = 0;

    lua_newtable(L);
    at = lua_gettop(L);
    lua_pushnil(L);
    lua_pushnil(L);

    // string
    lauxh_pushstr2tblat(L, "key", "string", at);
    assert(lauxh_checkstringof(L, at, "key"));
    assert(lauxh_checklstringof(L, at, "key", &len));
    assert(len == 6);
    // optstring
    len = 0;
    assert(lauxh_optstringof(L, at, "key", NULL));
    assert(lauxh_optlstringof(L, at, "key", NULL, &len));
    assert(len == 6);
    len = 0;
    lauxh_pushnil2tblat(L, "key", at);
    assert(lauxh_optlstringof(L, at, "key", NULL, &len) == NULL);
    assert(len == 0);

    // number
    lauxh_pushnum2tblat(L, "key", 1.1, at);
    assert(lauxh_checknumberof(L, at, "key") == 1.1);
    // optnumber
    assert(lauxh_optnumberof(L, at, "key", 0) == 1.1);
    lauxh_pushnil2tblat(L, "key", at);
    assert(lauxh_optnumberof(L, at, "key", 0) == 0);

    // integer
    lauxh_pushint2tblat(L, "key", 1, at);
    assert(lauxh_checkintegerof(L, at, "key") == 1);
    // optnumber
    assert(lauxh_optintegerof(L, at, "key", 0) == 1);
    lauxh_pushnil2tblat(L, "key", at);
    assert(lauxh_optintegerof(L, at, "key", 0) == 0);

    // boolean
    lauxh_pushbool2tblat(L, "key", 1, at);
    assert(lauxh_checkbooleanof(L, at, "key") == 1);
    // optboolean
    lauxh_pushnil2tblat(L, "key", at);
    assert(lauxh_optbooleanof(L, at, "key", 0) == 0);

    // push table
    lua_pushstring(L, "key");
    lua_newtable(L);
    lauxh_pushbool2arr(L, 1, 1);
    lua_rawset(L, at);

    assert(lua_gettop(L) == 3);
    assert(lua_type(L, 1) == LUA_TTABLE);
    assert(lua_type(L, 2) == LUA_TNIL);
    assert(lua_type(L, 3) == LUA_TNIL);

    // table
    lauxh_checktableof(L, at, "key");
    assert(lauxh_rawlen(L, -1) == 1);
    lauxh_checkbooleanat(L, -1, 1);

    return 0;
}

static int test_arguments(lua_State *L)
{
    void *ptr  = NULL;
    size_t len = 0;

    // string
    len = 0;
    lua_pushstring(L, "string");
    assert(lauxh_checkstring(L, -1));
    assert(lauxh_optstring(L, -1, NULL));
    assert(lauxh_checklstring(L, -1, &len));
    assert(len == 6);
    len = 0;
    assert(lauxh_optlstring(L, -1, NULL, &len));
    assert(len == 6);
    // optstring
    len = 0;
    lua_settop(L, 0);
    lua_pushnil(L);
    assert(lauxh_optstring(L, -1, NULL) == NULL);
    assert(lauxh_optlstring(L, -1, NULL, &len) == NULL);
    assert(len == 0);

    // number
    lua_settop(L, 0);
    lua_pushnumber(L, 1.1);
    assert(lauxh_checknumber(L, -1) == 1.1);
    // optnumber
    lua_settop(L, 0);
    lua_pushnumber(L, 1.1);
    assert(lauxh_optnumber(L, -1, 0) == 1.1);
    lua_pushnil(L);
    assert(lauxh_optnumber(L, -1, 0) == 0);

    // unsigned
    lua_settop(L, 0);
    lua_pushnumber(L, 1.1);
    assert(lauxh_checkunsigned(L, -1) == 1.1);
    // optunsigned
    lua_settop(L, 0);
    lua_pushnumber(L, 123);
    assert(lauxh_optunsigned(L, -1, 0) == 123);
    lua_pushnil(L);
    assert(lauxh_optunsigned(L, -1, 0) == 0);

    // integer
    lua_settop(L, 0);
    lua_pushinteger(L, 1);
    assert(lauxh_checkinteger(L, -1) == 1);
    // optinteger
    lua_settop(L, 0);
    lua_pushinteger(L, 1);
    assert(lauxh_optinteger(L, -1, 2) == 1);
    lua_pushnil(L);
    assert(lauxh_optinteger(L, -1, 2) == 2);

    // uinteger
    lua_settop(L, 0);
    lua_pushinteger(L, 12);
    assert(lauxh_checkuinteger(L, -1) == 12);
    // optuinteger
    lua_settop(L, 0);
    lua_pushinteger(L, 12);
    assert(lauxh_optuinteger(L, -1, 0) == 12);
    lua_pushnil(L);
    assert(lauxh_optuinteger(L, -1, 0) == 0);

    // // 64 bit integer
    // lua_settop( L, 0 );
    // lua_pushnumber( L, 96946020001494 );
    // printf( "lauxh_checkuint64 %llu\n", lauxh_optuint64( L, -1, 0 ) );
    // assert( lauxh_checkuint64( L, -1 ) == 1 );

    // char
    lua_settop(L, 0);
    lua_pushinteger(L, 1);
    assert(lauxh_checkint8(L, -1) == 1);

    // optint8
    lua_settop(L, 0);
    lua_pushinteger(L, 1);
    assert(lauxh_optint8(L, -1, 2) == 1);
    lua_pushnil(L);
    assert(lauxh_optint8(L, -1, 2) == 2);

    // boolean
    lua_settop(L, 0);
    lua_pushboolean(L, 1);
    assert(lauxh_checkboolean(L, -1) == 1);
    // optboolean
    lua_pushnil(L);
    assert(lauxh_optboolean(L, -1, 0) == 0);

    // table
    lua_settop(L, 0);
    lua_newtable(L);
    lauxh_checktable(L, -1);

    // thread
    lua_settop(L, 0);
    ptr = (void *)lua_newthread(L);
    assert((uintptr_t)lauxh_checkthread(L, -1) == (uintptr_t)ptr);

    // lightuserdata
    lua_settop(L, 0);
    ptr = malloc(0);
    lua_pushlightuserdata(L, ptr);
    assert((uintptr_t)lauxh_checkpointer(L, -1) == (uintptr_t)ptr);
    free(ptr);

    // cfunction
    lua_settop(L, 0);
    lua_pushcfunction(L, cfunction);
    assert((uintptr_t)lauxh_checkcfunction(L, -1) == (uintptr_t)cfunction);

    // function
    lua_settop(L, 0);
    assert(luaL_loadstring(L, "function fn()end") == 0);
    lauxh_checkfunction(L, -1);

    // flags
    lua_settop(L, 0);
    lua_pushinteger(L, 0x1);
    lua_pushinteger(L, 0x2);
    lua_pushinteger(L, 0x4);
    lua_pushinteger(L, 0x8);
    assert(lauxh_optflags(L, 1) == 0xF);

    return 0;
}

static int test_reference(lua_State *L)
{
    int ref = LUA_NOREF;

    assert(!lauxh_isref(LUA_NOREF));
    assert(!lauxh_isref(LUA_REFNIL));

    lua_pushstring(L, "string");
    ref = lauxh_ref(L);
    assert(lauxh_isref(ref));

    ref = lauxh_unref(L, ref);
    assert(ref == LUA_NOREF);

    lua_pushstring(L, "string");
    ref = lauxh_refat(L, -1);
    assert(lauxh_isref(ref));

    lauxh_pushref(L, ref);
    assert(lauxh_isstring(L, -1));

    ref = lauxh_unref(L, ref);
    assert(ref == LUA_NOREF);

    return 0;
}

static int test_is(lua_State *L)
{
    void *ptr = NULL;

    lua_pushnil(L);
    assert(lauxh_isnil(L, -1));

    // string
    lua_settop(L, 0);
    lua_pushstring(L, "string");
    assert(lauxh_isstring(L, -1));

    // number
    lua_settop(L, 0);
    lua_pushnumber(L, 1.1);
    assert(lauxh_isnumber(L, -1));

    // boolean true
    lua_settop(L, 0);
    lua_pushboolean(L, 1);
    assert(lauxh_isboolean(L, -1));

    // boolean false
    lua_settop(L, 0);
    lua_pushboolean(L, 0);
    assert(lauxh_isboolean(L, -1));

    // table
    lua_settop(L, 0);
    lua_newtable(L);
    assert(lauxh_istable(L, -1));

    // thread
    lua_settop(L, 0);
    lua_newthread(L);
    assert(lauxh_isthread(L, -1));

    // userdata
    lua_settop(L, 0);
    lua_newuserdata(L, sizeof(1));
    assert(lauxh_isuserdata(L, -1));

    // lightuserdata
    lua_settop(L, 0);
    ptr = malloc(0);
    lua_pushlightuserdata(L, ptr);
    assert(lauxh_ispointer(L, -1));
    free(ptr);

    // integer
    lua_settop(L, 0);
    lua_pushinteger(L, 1);
    assert(lauxh_isinteger(L, -1));

    // floating-point number is not integer
    lua_settop(L, 0);
    lua_pushnumber(L, 1.1);
    assert(!lauxh_isinteger(L, -1));

    // cfunction
    lua_settop(L, 0);
    lua_pushcfunction(L, cfunction);
    assert(lauxh_iscfunction(L, -1));

    // function
    lua_settop(L, 0);
    assert(luaL_loadstring(L, "function fn()end") == 0);
    assert(lauxh_isfunction(L, -1));

    // file
    lua_settop(L, 0);
    assert(luaL_loadstring(L, "return io.tmpfile()") == 0);
    assert(lua_pcall(L, 0, 1, 0) == 0);
    assert(lauxh_isfile(L, -1));

    // metatable
    luaL_newmetatable(L, "LAUXHLIB_A_MT");
    luaL_newmetatable(L, "LAUXHLIB_B_MT");
    lua_settop(L, 0);
    lua_newtable(L);
    lauxh_setmetatable(L, "LAUXHLIB_A_MT");
    assert(lauxh_ismetatableof(L, -1, "LAUXHLIB_A_MT") == 1);
    assert(lauxh_ismetatableof(L, -1, "LAUXHLIB_B_MT") == 0);

    lauxh_setmetatable(L, "LAUXHLIB_B_MT");
    assert(lauxh_ismetatableof(L, -1, "LAUXHLIB_A_MT") == 0);
    assert(lauxh_ismetatableof(L, -1, "LAUXHLIB_B_MT") == 1);

    return 0;
}

static int test_userdata(lua_State *L)
{
    const char *tname = "myuserdata";
    void *udata       = NULL;

    // userdata
    lua_settop(L, 0);

    // create metatable
    luaL_newmetatable(L, tname);
    lua_pop(L, 1);

    udata = lua_newuserdata(L, sizeof(1));
    assert(lauxh_isuserdata(L, -1));
    assert(lauxh_isuserdataof(L, -1, tname) == 0);

    lauxh_setmetatable(L, tname);
    assert(lauxh_isuserdataof(L, -1, tname));

    return 0;
}

static int test_file(lua_State *L)
{
    FILE *tmp       = NULL;
    FILE *fp        = NULL;
    int fd          = -1;
    const char *str = NULL;

    lua_settop(L, 0);

    // checkfile
    assert(luaL_loadstring(L, "return io.tmpfile()") == 0);
    assert(lua_pcall(L, 0, 1, 0) == 0);
    tmp = lauxh_checkfile(L, -1);
    assert(tmp != NULL);
    fd = fileno(tmp);
    assert(fd != -1);
    write(fd, "hello", 5);

    // fileno
    assert(lauxh_fileno(L, -1) == fd);

    // tofile
    fp = lauxh_tofile(L, fd, "r+", NULL);
    assert(fp != NULL);
    assert(lauxh_isfile(L, -1) == 1);
    assert(fileno(fp) == fd);
    // confirm it can be read file contents
    assert(luaL_loadstring(L, "local f = ...; assert(f:seek('set', 0)); return "
                              "assert(f:read('*a'))") == 0);
    lua_pushvalue(L, -2);
    assert(lua_pcall(L, 1, LUA_MULTRET, 0) == 0);
    str = lauxh_checkstring(L, -1);
    assert(str != NULL);
    assert(strcmp(str, "hello") == 0);

    return 0;
}

static int test_traceback(lua_State *L)
{
    const char src[] = "\nfunction foo() local a; a = a + 1; end\nfoo();";
    lua_State *th    = NULL;
    const char *msg  = NULL;

    lua_settop(L, 0);
    th = lua_newthread(L);
    assert(th != NULL);
    assert(luaL_loadbuffer(th, src, sizeof(src) - 1, "test_traceback") == 0);
    assert(lauxh_resume(th, L, 0) == LUA_ERRRUN);

    msg = lua_tostring(th, -1);
    lauxh_traceback(L, th, msg, 0);
    assert(lua_type(L, -1) == LUA_TSTRING);

    return 0;
}

static int test_xcopy(lua_State *L)
{
    lua_State *dest = luaL_newstate();

    assert(dest != NULL);

    // nil
    lua_settop(L, 0);
    lua_settop(dest, 0);
    lua_pushnil(L);
    lua_pushnil(dest);
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TNIL && lua_gettop(dest) == 2 &&
           lauxh_equal(dest, 1, 2) == 1);

    lua_settop(L, 0);
    lua_settop(dest, 0);
    lua_pushnil(L);
    lua_pushnil(dest);
    assert(lauxh_xcopy(L, dest, -1, 0) == LUA_TNONE && lua_gettop(dest) == 1);

    // string
    lua_settop(L, 0);
    lua_settop(dest, 0);
    lua_pushstring(L, "string");
    lua_pushstring(dest, "string");
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TSTRING &&
           lua_gettop(dest) == 2 && lauxh_equal(dest, 1, 2) == 1);

    // number
    lua_settop(L, 0);
    lua_settop(dest, 0);
    lua_pushnumber(L, 1.1);
    lua_pushnumber(dest, 1.1);
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TNUMBER &&
           lua_gettop(dest) == 2 && lauxh_equal(dest, 1, 2) == 1);

    // boolean true
    lua_settop(L, 0);
    lua_settop(dest, 0);
    lua_pushboolean(L, 1);
    lua_pushboolean(dest, 1);
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TBOOLEAN &&
           lua_gettop(dest) == 2 && lauxh_equal(dest, 1, 2) == 1);

    // boolean false
    lua_settop(L, 0);
    lua_settop(dest, 0);
    lua_pushboolean(L, 0);
    lua_pushboolean(dest, 0);
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TBOOLEAN &&
           lua_gettop(dest) == 2 && lauxh_equal(dest, 1, 2) == 1);

    // function
    lua_settop(L, 0);
    lua_settop(dest, 0);
    assert(luaL_loadstring(L, "function fn()end") == 0);
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TNONE && lua_gettop(dest) == 0);

    // thread
    lua_settop(L, 0);
    lua_newthread(L);
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TNONE && lua_gettop(dest) == 0);

    // userdata
    lua_settop(L, 0);
    lua_newuserdata(L, sizeof(1));
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TNONE && lua_gettop(dest) == 0);

    // lightuserdata
    lua_settop(L, 0);
    {
        void *ptr = malloc(0);
        lua_pushlightuserdata(L, ptr);
        assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TLIGHTUSERDATA &&
               lua_gettop(dest) == 1 && lauxh_ispointer(dest, -1) == 1);
        free(ptr);
    }

    // table
    lua_settop(L, 0);
    lua_settop(dest, 0);
    lua_newtable(L);
    lauxh_pushstr2tbl(L, "hello", "world");
    assert(lauxh_xcopy(L, dest, -1, 1) == LUA_TTABLE && lua_gettop(dest) == 1);
    lauxh_gettblof(dest, "hello", -2);
    lua_pushstring(dest, "world");
    assert(lauxh_equal(dest, -1, -2) == 1);

    lua_close(dest);

    return 0;
}

static int test_resume(lua_State *L)
{
    const char src[] = "\nfunction foo() return 1, 'a', {}; end\nreturn foo();";
    lua_State *th    = NULL;

    lua_settop(L, 0);
    th = lua_newthread(L);
    assert(th != NULL);
    assert(luaL_loadbuffer(th, src, sizeof(src) - 1, "test_resume") == 0);
    assert(lauxh_resume(th, L, 0) == 0);
    assert(lua_gettop(th) == 3);
    assert(lua_type(th, -1) == LUA_TTABLE);
    assert(lua_type(th, -2) == LUA_TSTRING);
    assert(lua_type(th, -3) == LUA_TNUMBER);

    return 0;
}

static int test_argerror_func(lua_State *L)
{
    return lauxh_argerror(L, 1, "error from test_argerror_func");
}

static int test_argcheck_func(lua_State *L)
{
    lauxh_argcheck(L, 0, 1, "error from test_argcheck_func");
    return 0;
}

static int test_argerror(lua_State *L)
{
    // argerror
    lua_settop(L, 0);
    lua_pushcfunction(L, test_argerror_func);
    assert(lua_pcall(L, 0, 0, 0) == LUA_ERRRUN);
    assert(strstr(lua_tostring(L, -1), "error from test_argerror_func") !=
           NULL);

    // argcheck
    lua_settop(L, 0);
    lua_pushcfunction(L, test_argcheck_func);
    assert(lua_pcall(L, 0, 0, 0) == LUA_ERRRUN);
    assert(strstr(lua_tostring(L, -1), "error from test_argcheck_func") !=
           NULL);

    return 0;
}

static int test_tolstring(lua_State *L)
{
    lua_settop(L, 0);

#define assert_tostring(s)                                                     \
 do {                                                                          \
  const char *str = lauxh_tostring(L, 1);                                      \
  assert(strcmp(str, (s)) == 0);                                               \
  assert(lua_gettop(L) == 2);                                                  \
  lua_settop(L, 0);                                                            \
 } while (0)

    // nil
    lua_pushnil(L);
    assert_tostring("nil");

    // string
    lua_pushstring(L, "hello");
    assert_tostring("hello");

    // number
    lua_pushnumber(L, 1.1);
    assert_tostring("1.1");

    // integer
    lua_pushinteger(L, 9876);
    assert_tostring("9876");

    // boolean true
    lua_pushboolean(L, 1);
    assert_tostring("true");

    // boolean false
    lua_pushboolean(L, 0);
    assert_tostring("false");

#define assert_pointer_tolstring(fmt)                                          \
 do {                                                                          \
  char buf[BUFSIZ] = {0};                                                      \
  size_t blen      = 0;                                                        \
  blen             = snprintf(buf, BUFSIZ, fmt, lua_topointer(L, -1));         \
  buf[blen]        = 0;                                                        \
  assert_tostring(buf);                                                        \
 } while (0)

    // table
    lua_newtable(L);
    assert_pointer_tolstring("table: %p");

    // function
    assert(luaL_loadstring(L, "function fn()end") == 0);
    assert_pointer_tolstring("function: %p");

    // thread
    lua_newthread(L);
    assert_pointer_tolstring("thread: %p");

    // userdata
    lua_newuserdata(L, sizeof(1));
    assert_pointer_tolstring("userdata: %p");

    // lightuserdata
    lua_pushlightuserdata(L, L);
    assert_pointer_tolstring("userdata: %p");

    // __tostring metatable
    assert(luaL_loadstring(L,
                           "return setmetatable({}, { __tostring = function() "
                           "return 'tostring_metamethod' end })") == 0);
    assert(lua_pcall(L, 0, 1, 0) == 0);
    assert_tostring("tostring_metamethod");

#undef assert_pointer_tolstring
#undef assert_tostring

    return 0;
}

LUALIB_API int luaopen_lauxhlib(lua_State *L)
{
    struct luaL_Reg method[] = {
        {"test_is",        test_is       },
        {"test_reference", test_reference},
        {"test_arguments", test_arguments},
        {"test_table",     test_table    },
        {"test_tableat",   test_tableat  },
        {"test_array",     test_array    },
        {"test_arrayat",   test_arrayat  },
        {"test_userdata",  test_userdata },
        {"test_file",      test_file     },
        {"test_traceback", test_traceback},
        {"test_xcopy",     test_xcopy    },
        {"test_resume",    test_resume   },
        {"test_argerror",  test_argerror },
        {"test_tolstring", test_tolstring},
        {NULL,             NULL          }
    };
    struct luaL_Reg *ptr = method;

    lua_settop(L, 0);
    while (ptr->name) {
        printf("%s\n", ptr->name);
        ptr->func(L);
        lua_settop(L, 0);
        ptr++;
    }

    return 0;
}

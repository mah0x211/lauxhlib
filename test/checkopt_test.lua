local pcall = pcall
local clock = os.clock
local assert = require('assert')

local function printf(...)
    print(string.format(...))
end

local testfuncs = {}
local testcase = setmetatable({}, {
    __newindex = function(_, name, func)
        assert.is_string(name)
        assert.is_function(func)
        if testfuncs[name] then
            error(string.format('testcase.%s already defined', name), 2)
        end

        local case = {
            name = name,
            func = func,
        }
        testfuncs[#testfuncs + 1] = case
        testfuncs[name] = case
    end,
})

local checkopt = require('lauxhlib.checkopt')

local FILE = assert(io.tmpfile())
local STR = 'str'
local ZERO = 0
local INT = 1
local INTMAX = 0x7FFFFFFFFFFFFC00
local FLOAT = 1.1
local INF = 1 / 0
local NAN = 0 / 0
local TBL = {}
local FUNC = function()
end
local CFUNC = clock
local THREAD = coroutine.create(FUNC)

function testcase.checkopt_bool()
    -- test that return argument
    for _, v in ipairs({
        true,
        false,
    }) do
        assert.equal(v, checkopt.bool(v))
        assert.equal(v, checkopt.bool(nil, v))
    end
    assert.is_true(checkopt.bool(true, false))
    assert.is_false(checkopt.bool(false, true))
    assert.is_nil(checkopt.bool(nil))
    assert.is_nil(checkopt.bool())

    -- test that throws an error
    for _, v in ipairs({
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.bool, v)
        assert.match(err, '#1 .+[(]boolean expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.bool, nil, v)
        assert.match(err, '#2 .+[(]boolean expected, ', false)
    end
end

function testcase.checkopt_pointer()
    -- TODO
end

function testcase.checkopt_num()
    -- test that return argument
    for _, v in ipairs({
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
    }) do
        assert.equal(v, checkopt.num(v))
        assert.equal(v, checkopt.num(nil, v))
    end
    assert.equal(123, checkopt.num(123, 456))
    assert.is_nil(checkopt.num(nil))
    assert.is_nil(checkopt.num())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.num, v)
        assert.match(err, '#1 .+[(]number expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.num, nil, v)
        assert.match(err, '#2 .+[(]number expected, ', false)
    end
end

function testcase.checkopt_str()
    -- test that return argument
    for _, v in ipairs({
        STR,
    }) do
        assert.equal(v, checkopt.str(v))
        assert.equal(v, checkopt.str(nil, v))
    end
    assert.equal('hello', checkopt.str('hello', 'world'))
    assert.is_nil(checkopt.str(nil))
    assert.is_nil(checkopt.str())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.str, v)
        assert.match(err, '#1 .+[(]string expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.str, nil, v)
        assert.match(err, '#2 .+[(]string expected, ', false)
    end
end

function testcase.checkopt_table()
    -- test that return argument
    for _, v in ipairs({
        TBL,
    }) do
        assert.equal(v, checkopt.table(v))
        assert.equal(v, checkopt.table(nil, v))
    end
    local newtbl = {}
    assert.equal(newtbl, checkopt.table(newtbl, {
        'foo',
    }))
    assert.is_nil(checkopt.table(nil))
    assert.is_nil(checkopt.table())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        FUNC,
        CFUNC,
        THREAD,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.table, v)
        assert.match(err, '#1 .+[(]table expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.table, nil, v)
        assert.match(err, '#2 .+[(]table expected, ', false)
    end
end

function testcase.checkopt_func()
    -- test that return argument
    for _, v in ipairs({
        FUNC,
        CFUNC,
    }) do
        assert.equal(v, checkopt.func(v))
        assert.equal(v, checkopt.func(nil, v))
    end
    assert.equal(os.time, checkopt.func(os.time, FUNC))
    assert.is_nil(checkopt.func(nil))
    assert.is_nil(checkopt.func())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        THREAD,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.func, v)
        assert.match(err, '#1 .+[(]function expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.func, nil, v)
        assert.match(err, '#2 .+[(]function expected, ', false)
    end
end

function testcase.checkopt_cfunc()
    -- test that return argument
    for _, v in ipairs({
        CFUNC,
    }) do
        assert.equal(v, checkopt.cfunc(v))
        assert.equal(v, checkopt.cfunc(nil, v))
    end
    assert.equal(os.time, checkopt.cfunc(os.time, CFUNC))
    assert.is_nil(checkopt.cfunc(nil))
    assert.is_nil(checkopt.cfunc())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        THREAD,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.cfunc, v)
        assert.match(err, '#1 .+[(]cfunction expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.cfunc, nil, v)
        assert.match(err, '#2 .+[(]cfunction expected, ', false)
    end
end

function testcase.checkopt_thread()
    -- test that return argument
    for _, v in ipairs({
        THREAD,
    }) do
        assert.equal(v, checkopt.thread(v))
        assert.equal(v, checkopt.thread(nil, v))
    end
    local newth = coroutine.create(function()
    end)
    assert.equal(newth, checkopt.thread(newth, THREAD))
    assert.is_nil(checkopt.thread(nil))
    assert.is_nil(checkopt.thread())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.thread, v)
        assert.match(err, '#1 .+[(]thread expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.thread, nil, v)
        assert.match(err, '#2 .+[(]thread expected, ', false)
    end
end

function testcase.checkopt_userdata()
    -- test that return argument
    for _, v in ipairs({
        FILE,
    }) do
        assert.equal(v, checkopt.userdata(v))
        assert.equal(v, checkopt.userdata(nil, v))
    end
    local f = assert(io.tmpfile())
    assert.equal(f, checkopt.userdata(f, FILE))
    assert.is_nil(checkopt.userdata(nil))
    assert.is_nil(checkopt.userdata())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.userdata, v)
        assert.match(err, '#1 .+[(]userdata expected, ', false)

        -- test that throws an error if default is invalid
        err = assert.throws(checkopt.userdata, nil, v)
        assert.match(err, '#2 .+[(]userdata expected, ', false)
    end
end

function testcase.checkopt_finite()
    -- test that return argument
    for _, v in ipairs({
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
    }) do
        assert.equal(v, checkopt.finite(v))
        assert.equal(v, checkopt.finite(nil, v))
    end
    assert.equal(123, checkopt.finite(123, 456))
    assert.is_nil(checkopt.finite(nil))
    assert.is_nil(checkopt.finite())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -INF,
        INF,
        -NAN,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.finite, v)
        assert.match(err, '#1 .+[(]finite number expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.finite, nil, v)
        assert.match(err, '#2 .+[(]finite number expected, ', false)
    end
end

function testcase.checkopt_unsigned()
    -- test that return argument
    for _, v in ipairs({
        ZERO,
        INT,
        FLOAT,
        INF,
    }) do
        assert.equal(v, checkopt.unsigned(v))
        assert.equal(v, checkopt.unsigned(nil, v))
    end
    assert.equal(123, checkopt.unsigned(123, 456))
    assert.is_nil(checkopt.unsigned(nil))
    assert.is_nil(checkopt.unsigned())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -INT,
        -FLOAT,
        -INF,
        -NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.unsigned, v)
        assert.match(err, '#1 .+[(]unsigned number expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.unsigned, nil, v)
        assert.match(err, '#2 .+[(]unsigned number expected, ', false)
    end
end

function testcase.checkopt_int()
    -- test that return argument
    for _, v in ipairs({
        -INT,
        ZERO,
        INTMAX,
    }) do
        assert.equal(v, checkopt.int(v))
        assert.equal(v, checkopt.int(nil, v))
    end
    assert.equal(123, checkopt.int(123, 456))
    assert.is_nil(checkopt.int(nil))
    assert.is_nil(checkopt.int())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.int, v)
        assert.match(err, '#1 .+[(]integer expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.int, nil, v)
        assert.match(err, '#2 .+[(]integer expected, ', false)
    end
end

function testcase.checkopt_uint()
    -- test that return argument
    for _, v in ipairs({
        ZERO,
        INTMAX,
    }) do
        assert.equal(v, checkopt.uint(v))
        assert.equal(v, checkopt.uint(nil, v))
    end
    assert.equal(123, checkopt.uint(123, 456))
    assert.is_nil(checkopt.uint(nil))
    assert.is_nil(checkopt.uint())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -INT,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.uint, v)
        assert.match(err, '#1 .+[(]unsigned integer expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.uint, nil, v)
        assert.match(err, '#2 .+[(]unsigned integer expected, ', false)
    end
end

function testcase.checkopt_pint()
    -- test that return argument
    for _, v in ipairs({
        INT,
    }) do
        assert.equal(v, checkopt.pint(v))
        assert.equal(v, checkopt.pint(nil, v))
    end
    assert.equal(123, checkopt.pint(123, 456))
    assert.is_nil(checkopt.pint(nil))
    assert.is_nil(checkopt.pint())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.pint, v)
        assert.match(err, '#1 .+[(]positive integer expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.pint, nil, v)
        assert.match(err, '#2 .+[(]positive integer expected, ', false)
    end
end

function testcase.checkopt_int8()
    -- test that return argument
    for _, v in ipairs({
        -0x80,
        0,
        0x7F,
    }) do
        assert.equal(v, checkopt.int8(v))
        assert.equal(v, checkopt.int8(nil, v))
    end
    assert.equal(123, checkopt.int8(123, 127))
    assert.is_nil(checkopt.int8(nil))
    assert.is_nil(checkopt.int8())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -0x81,
        0x80,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.int8, v)
        assert.match(err, '#1 .+[(]int8_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.int8, nil, v)
        assert.match(err, '#2 .+[(]int8_t expected, ', false)
    end
end

function testcase.checkopt_int16()
    -- test that return argument
    for _, v in ipairs({
        -0x8000,
        0,
        0x7FFF,
    }) do
        assert.equal(v, checkopt.int16(v))
        assert.equal(v, checkopt.int16(nil, v))
    end
    assert.equal(123, checkopt.int16(123, 456))
    assert.is_nil(checkopt.int16(nil))
    assert.is_nil(checkopt.int16())

    -- test that throws an error
    for _, v in ipairs({
        -0x8001,
        0x8000,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.int16, v)
        assert.match(err, '#1 .+[(]int16_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.int16, nil, v)
        assert.match(err, '#2 .+[(]int16_t expected, ', false)
    end
end

function testcase.checkopt_int32()
    -- test that return argument
    for _, v in ipairs({
        -0x80000000,
        0,
        0x7FFFFFFF,
    }) do
        assert.equal(v, checkopt.int32(v))
        assert.equal(v, checkopt.int32(nil, v))
    end
    assert.equal(123, checkopt.int32(123, 456))
    assert.is_nil(checkopt.int32(nil))
    assert.is_nil(checkopt.int32())

    -- test that throws an error
    for _, v in ipairs({
        -0x80000001,
        0x80000000,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.int32, v)
        assert.match(err, '#1 .+[(]int32_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.int32, nil, v)
        assert.match(err, '#2 .+[(]int32_t expected, ', false)
    end
end

function testcase.checkopt_int64()
    -- test that return argument
    for _, v in ipairs({
        -0x20000000000000,
        0,
        0x7FFFFFFFFFFFFC00,
    }) do
        assert.equal(v, checkopt.int64(v))
        assert.equal(v, checkopt.int64(nil, v))
    end
    assert.equal(123, checkopt.int64(123, 456))
    assert.is_nil(checkopt.int64(nil))
    assert.is_nil(checkopt.int64())

    -- test that throws an error
    for _, v in ipairs({
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.int64, v)
        assert.match(err, '#1 .+[(]int64_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.int64, nil, v)
        assert.match(err, '#2 .+[(]int64_t expected, ', false)
    end
end

function testcase.checkopt_uint8()
    -- test that return argument
    for _, v in ipairs({
        0,
        0xFF,
    }) do
        assert.equal(v, checkopt.uint8(v))
        assert.equal(v, checkopt.uint8(nil, v))
    end
    assert.equal(123, checkopt.uint8(123, 255))
    assert.is_nil(checkopt.uint8(nil))
    assert.is_nil(checkopt.uint8())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        FILE,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
        -1,
        0x100,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.uint8, v)
        assert.match(err, '#1 .+[(]uint8_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.uint8, nil, v)
        assert.match(err, '#2 .+[(]uint8_t expected, ', false)
    end
end

function testcase.checkopt_uint16()
    -- test that return argument
    for _, v in ipairs({
        0,
        0xFFFF,
    }) do
        assert.equal(v, checkopt.uint16(v))
        assert.equal(v, checkopt.uint16(nil, v))
    end
    assert.equal(123, checkopt.uint16(123, 456))
    assert.is_nil(checkopt.uint16(nil))
    assert.is_nil(checkopt.uint16())

    -- test that throws an error
    for _, v in ipairs({
        -1,
        0x10000,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.uint16, v)
        assert.match(err, '#1 .+[(]uint16_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.uint16, nil, v)
        assert.match(err, '#2 .+[(]uint16_t expected, ', false)
    end
end

function testcase.checkopt_uint32()
    -- test that return argument
    for _, v in ipairs({
        0,
        0xFFFFFFFF,
    }) do
        assert.equal(v, checkopt.uint32(v))
        assert.equal(v, checkopt.uint32(nil, v))
    end
    assert.equal(123, checkopt.uint32(123, 456))
    assert.is_nil(checkopt.uint32(nil))
    assert.is_nil(checkopt.uint32())

    -- test that throws an error
    for _, v in ipairs({
        -1,
        0x1FFFFFFFF,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.uint32, v)
        assert.match(err, '#1 .+[(]uint32_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.uint32, nil, v)
        assert.match(err, '#2 .+[(]uint32_t expected, ', false)
    end
end

function testcase.checkopt_uint64()
    -- test that return argument
    for _, v in ipairs({
        0,
        0x7FFFFFFFFFFFFC00,
    }) do
        assert.equal(v, checkopt.uint64(v))
        assert.equal(v, checkopt.uint64(nil, v))
    end
    assert.equal(123, checkopt.uint64(123, 456))
    assert.is_nil(checkopt.uint64(nil))
    assert.is_nil(checkopt.uint64())

    -- test that throws an error
    for _, v in ipairs({
        -1,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.uint64, v)
        assert.match(err, '#1 .+[(]uint64_t expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.uint64, nil, v)
        assert.match(err, '#2 .+[(]uint64_t expected, ', false)
    end
end

function testcase.checkopt_file()
    -- test that return argument
    for _, v in ipairs({
        FILE,
    }) do
        assert.equal(v, checkopt.file(v))
        assert.equal(v, checkopt.file(nil, v))
    end
    local f = assert(io.tmpfile())
    assert.equal(f, checkopt.file(f, FILE))
    assert.is_nil(checkopt.file(nil))
    assert.is_nil(checkopt.file())

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
        STR,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
        TBL,
        FUNC,
        THREAD,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.file, v)
        assert.match(err, '#1 .+[(]FILE%* expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.file, nil, v)
        assert.match(err, '#2 .+[(]FILE%* expected, ', false)
    end
end

function testcase.checkopt_callable()
    -- test that return true
    local obj = setmetatable({}, {
        __metatable = 1,
        __call = FUNC,
    })
    for _, v in ipairs({
        FUNC,
        CFUNC,
        obj,
    }) do
        assert.equal(v, checkopt.callable(v))
        assert.equal(v, checkopt.callable(nil, v))
    end
    assert.equal(obj, checkopt.callable(obj, FUNC))
    assert.is_nil(checkopt.callable(nil))
    assert.is_nil(checkopt.callable())

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        THREAD,
        setmetatable({}, {
            __call = INT,
        }),
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(checkopt.callable, v)
        assert.match(err, '#1 .+[(]callable object expected, ', false)

        -- test that throws an error if default value is invalid
        err = assert.throws(checkopt.callable, nil, v)
        assert.match(err, '#2 .+[(]callable object expected, ', false)
    end
end

function testcase.check_with_argname()
    -- test that call without argument name
    local err = assert.throws(checkopt.int, true)
    assert.match(err, "bad argument #1 .+[(]integer expected, ", false)

    -- test that call with argument name
    err = assert.throws(checkopt.int, true, nil, 'hello')
    assert.match(err, "bad argument 'hello' .+[(]integer expected, ", false)
end

function testcase.check_with_stacklv()
    local child_caller = function(lv)
        checkopt.int(true, nil, nil, lv)
    end
    local base_caller = function(lv)
        child_caller(lv)
    end

    -- test that call without stack level
    local err = assert.throws(function()
        base_caller()
    end)
    assert.match(err, "bad argument #1 to 'child_caller'")

    -- test that stack level 0 is equal to check.none
    err = assert.throws(function()
        base_caller(0)
    end)
    assert.match(err, "bad argument #1 to 'int'")

    -- test that stack level 1 is equal to child_caller
    err = assert.throws(function()
        base_caller(1)
    end)
    assert.match(err, "bad argument #1 to 'child_caller'")

    -- test that stack level 2 is equal to base_caller
    err = assert.throws(function()
        base_caller(2)
    end)
    assert.match(err, "bad argument #1 to 'base_caller'")
end

-- run test cases
do
    local errors = {}
    for _, case in ipairs(testfuncs) do
        local t = clock()
        local ok, err = pcall(case.func)
        t = clock() - t
        if ok then
            printf('testcase.%s ... ok (%f sec)', case.name, t)
        else
            err = string.gsub(err, '\n', {
                ['\n'] = '\n  > ',
            })
            local msg = string.format('testcase.%s ... failed (%f sec)\n  > %s',
                                      case.name, t, err)
            errors[#errors + 1] = err
            print(msg)
        end
    end

    if #errors > 0 then
        error(table.concat(errors, '\n'))
    end
end

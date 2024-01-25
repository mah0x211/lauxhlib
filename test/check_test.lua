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

local check = require('lauxhlib.check')

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

function testcase.check_none()
    -- test that return argument
    assert.is_nil(check.none(nil))
    assert.is_nil(check.none())

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
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
    }) do
        local err = assert.throws(check.none, v)
        assert.match(err, '(nil expected, ')
    end
end

function testcase.check_bool()
    -- test that return argument
    assert.is_true(check.bool(true))
    assert.is_false(check.bool(false))

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
        local err = assert.throws(check.bool, v)
        assert.match(err, '(boolean expected, ')
    end
    local err = assert.throws(check.bool)
    assert.match(err, '(boolean expected, ')
    err = assert.throws(check.bool, nil)
    assert.match(err, '(boolean expected, ')
end

function testcase.check_pointer()
    -- TODO
end

function testcase.check_num()
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
        assert.equal(v, check.num(v))
    end

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
        local err = assert.throws(check.num, v)
        assert.match(err, '(number expected, ')
    end
    local err = assert.throws(check.num)
    assert.match(err, '(number expected, ')
    err = assert.throws(check.num, nil)
    assert.match(err, '(number expected, ')

    -- test that check number with min
    assert.equal(check.num(1, 0), 1)

    -- test that check number with max
    assert.equal(check.num(1, nil, 2), 1)

    -- test that check number with min and max
    assert.equal(check.num(1, 0, 2), 1)

    -- test that throws an error if value is less than min
    err = assert.throws(check.num, 1, 2.1)
    assert.match(err, '(number greater than or equal to 2.1 expected,')

    -- test that throws an error if value is greater than max
    err = assert.throws(check.num, 1, nil, 0.1)
    assert.match(err, '(number less than or equal to 0.1 expected,')

    -- test that throws an error if value is not from min to max
    err = assert.throws(check.num, 4, -2.1, 3.2)
    assert.match(err, '(number from -2.1 to 3.2 expected,')
end

function testcase.check_str()
    -- test that return argument
    assert.equal(STR, check.str(STR))

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
        local err = assert.throws(check.str, v)
        assert.match(err, '(string expected, ')
    end
    local err = assert.throws(check.str)
    assert.match(err, '(string expected, ')
    err = assert.throws(check.str, nil)
    assert.match(err, '(string expected, ')
end

function testcase.check_table()
    -- test that return argument
    assert.equal(TBL, check.table(TBL))

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
        local err = assert.throws(check.table, v)
        assert.match(err, '(table expected, ')
    end
end

function testcase.check_func()
    -- test that return argument
    assert.equal(FUNC, check.func(FUNC))
    assert.equal(CFUNC, check.func(CFUNC))

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
        local err = assert.throws(check.func, v)
        assert.match(err, '(function expected, ')
    end
end

function testcase.check_cfunc()
    -- test that return argument
    assert.equal(CFUNC, check.cfunc(CFUNC))

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
        local err = assert.throws(check.cfunc, v)
        assert.match(err, '(cfunction expected, ')
    end
end

function testcase.check_thread()
    -- test that return argument
    assert.equal(THREAD, check.thread(THREAD))

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
        local err = assert.throws(check.thread, v)
        assert.match(err, '(thread expected, ')
    end
end

function testcase.check_userdata()
    -- test that return argument
    assert.equal(FILE, check.userdata(FILE))

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
        local err = assert.throws(check.userdata, v)
        assert.match(err, '(userdata expected, ')
    end
end

function testcase.check_finite()
    -- test that return argument
    for _, v in ipairs({
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
    }) do
        assert.equal(v, check.finite(v))
    end

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
        local err = assert.throws(check.finite, v)
        assert.match(err, '(finite number expected, ')
    end

    -- test that check finite-number with min
    assert.equal(check.finite(1, 0), 1)

    -- test that check finite-number with max
    assert.equal(check.finite(1, nil, 2), 1)

    -- test that check finite-number with min and max
    assert.equal(check.finite(1, 0, 2), 1)

    -- test that throws an error if value is less than min
    local err = assert.throws(check.finite, 1, 2.1)
    assert.match(err, '(finite number greater than or equal to 2.1 expected,')

    -- test that throws an error if value is greater than max
    err = assert.throws(check.finite, 1, nil, 0.1)
    assert.match(err, '(finite number less than or equal to 0.1 expected,')

    -- test that throws an error if value is not from min to max
    err = assert.throws(check.finite, 4, -2.1, 3.2)
    assert.match(err, '(finite number from -2.1 to 3.2 expected,')
end

function testcase.check_unsigned()
    -- test that return argument
    for _, v in ipairs({
        ZERO,
        INT,
        FLOAT,
        INF,
    }) do
        assert.equal(v, check.unsigned(v))
    end

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
        local err = assert.throws(check.unsigned, v)
        assert.match(err, '(unsigned number expected, ')
    end
end

function testcase.check_int()
    -- test that return argument
    for _, v in ipairs({
        -INT,
        ZERO,
        INTMAX,
    }) do
        assert.equal(v, check.int(v))
    end

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
        local err = assert.throws(check.int, v)
        assert.match(err, '(integer expected, ')
    end

    -- test that check integer with min
    assert.equal(check.int(1, 0), 1)

    -- test that check integer with max
    assert.equal(check.int(1, nil, 2), 1)

    -- test that check integer with min and max
    assert.equal(check.int(1, 0, 2), 1)

    -- test that throws an error if value is less than min
    local err = assert.throws(check.int, 1, 2)
    assert.match(err, '(integer greater than or equal to 2 expected,')

    -- test that throws an error if value is greater than max
    err = assert.throws(check.int, 1, nil, 0)
    assert.match(err, '(integer less than or equal to 0 expected,')

    -- test that throws an error if value is not from min to max
    err = assert.throws(check.int, 4, -2, 3)
    assert.match(err, '(integer from -2 to 3 expected,')
end

function testcase.check_uint()
    -- test that return argument
    for _, v in ipairs({
        ZERO,
        INTMAX,
    }) do
        assert.equal(v, check.uint(v))
    end

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
        local err = assert.throws(check.uint, v)
        assert.match(err, '(unsigned integer expected, ')
    end
end

function testcase.check_pint()
    -- test that return argument
    for _, v in ipairs({
        INT,
    }) do
        assert.equal(v, check.pint(v))
    end

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
        local err = assert.throws(check.pint, v)
        assert.match(err, '(positive integer expected, ')
    end
end

function testcase.check_int8()
    -- test that return argument
    for _, v in ipairs({
        -0x80,
        0,
        0x7F,
    }) do
        assert.equal(v, check.int8(v))
    end

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
        local err = assert.throws(check.int8, v)
        assert.match(err, '(int8_t expected, ')
    end
end

function testcase.check_uint8()
    -- test that return argument
    for _, v in ipairs({
        0,
        0xFF,
    }) do
        assert.equal(v, check.uint8(v))
    end

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
        local err = assert.throws(check.uint8, v)
        assert.match(err, '(uint8_t expected, ')
    end
end

function testcase.check_int16()
    -- test that return argument
    for _, v in ipairs({
        -0x8000,
        0,
        0x7FFF,
    }) do
        assert.equal(v, check.int16(v))
    end

    -- test that throws an error
    for _, v in ipairs({
        -0x8001,
        0x8000,
        INF,
        NAN,
    }) do
        local err = assert.throws(check.int16, v)
        assert.match(err, '(int16_t expected, ')
    end
end

function testcase.check_uint16()
    -- test that return argument
    for _, v in ipairs({
        0,
        0xFFFF,
    }) do
        assert.equal(v, check.uint16(v))
    end

    -- test that throws an error
    for _, v in ipairs({
        -1,
        0x10000,
        INF,
        NAN,
    }) do
        local err = assert.throws(check.uint16, v)
        assert.match(err, '(uint16_t expected, ')
    end
end

function testcase.check_int32()
    -- test that return argument
    for _, v in ipairs({
        -0x80000000,
        0,
        0x7FFFFFFF,
    }) do
        assert.equal(v, check.int32(v))
    end

    -- test that throws an error
    for _, v in ipairs({
        -0x80000001,
        0x80000000,
        INF,
        NAN,
    }) do
        local err = assert.throws(check.int32, v)
        assert.match(err, '(int32_t expected, ')
    end
end

function testcase.check_uint32()
    -- test that return argument
    for _, v in ipairs({
        0,
        0xFFFFFFFF,
    }) do
        assert.equal(v, check.uint32(v))
    end

    -- test that throws an error
    for _, v in ipairs({
        -1,
        0x1FFFFFFFF,
        INF,
        NAN,
    }) do
        local err = assert.throws(check.uint32, v)
        assert.match(err, '(uint32_t expected, ')
    end
end

function testcase.check_int64()
    -- test that return argument
    for _, v in ipairs({
        -0x20000000000000,
        0,
        0x7FFFFFFFFFFFFC00,
    }) do
        assert.equal(v, check.int64(v))
    end

    -- test that throws an error
    for _, v in ipairs({
        INF,
        NAN,
    }) do
        local err = assert.throws(check.int64, v)
        assert.match(err, '(int64_t expected, ')
    end
end

function testcase.check_uint64()
    -- test that return argument
    for _, v in ipairs({
        0,
        0x7FFFFFFFFFFFFC00,
    }) do
        assert.equal(v, check.uint64(v))
    end

    -- test that throws an error
    for _, v in ipairs({
        -1,
        INF,
        NAN,
    }) do
        local err = assert.throws(check.uint64, v)
        assert.match(err, '(uint64_t expected, ')
    end
end

function testcase.check_file()
    -- test that return argument
    assert.equal(FILE, check.file(FILE))

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
        local err = assert.throws(check.file, v)
        assert.match(err, '(FILE* expected, ')
    end
end

function testcase.check_callable()
    -- test that return true
    local obj = setmetatable({}, {
        __metatable = 1,
        __call = FUNC,
    })
    assert.equal(FUNC, check.callable(FUNC))
    assert.equal(CFUNC, check.callable(CFUNC))
    assert.equal(obj, check.callable(obj))

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
        local err = assert.throws(check.callable, v)
        assert.match(err, '(callable object expected, ')
    end
end

function testcase.check_flags()
    -- test that return argument
    -- 0x8000 | 0x9 | 0x1E000000000000 | 0x2001C0
    assert.equal(0x1E0000002081C9,
                 check.flags(0x8000, 0x9, 0x1E000000000000, 0x2001C0))
    assert.equal(0, check.flags(nil, 0, nil, nil, 0))
    assert.equal(0, check.flags())

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
        FLOAT,
        INF,
        NAN,
    }) do
        -- test that throws an error if value is invalid
        local err = assert.throws(check.flags, 100, v)
        assert.match(err, '#2 .+[(]uint64_t expected, ', false)
    end
end

function testcase.check_with_argname()
    -- test that call without argument name
    local err = assert.throws(check.none, true)
    assert.match(err, "bad argument #1 .+[(]nil expected, ", false)

    -- test that call with argument name
    err = assert.throws(check.none, true, 'hello')
    assert.match(err, "bad argument 'hello' .+[(]nil expected, ", false)

    -- test that call with argument name as positive integer
    err = assert.throws(check.none, true, 125)
    assert.match(err, "bad argument #125 .+[(]nil expected, ", false)
end

function testcase.check_with_stacklv()
    local child_caller = function(lv)
        check.none(true, nil, lv)
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
    assert.match(err, "bad argument #1 to 'none'")

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

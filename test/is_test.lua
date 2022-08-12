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

local is = require('lauxhlib.is')

local FILE = assert(io.tmpfile())
local STR = 'str'
local ZERO = 0
local INT = 1
local FLOAT = 1.1
local INF = 1 / 0
local NAN = 0 / 0
local TBL = {}
local FUNC = function()
end
local CFUNC = clock
local THREAD = coroutine.create(FUNC)

function testcase.is_nil()
    -- test that return true
    assert.is_true(is.none(nil))
    assert.is_true(is.none())

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.none(v))
    end
end

function testcase.is_bool()
    -- test that return true
    assert.is_true(is.bool(true))
    assert.is_true(is.bool(false))

    -- test that return false
    for _, v in ipairs({
        FILE,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.bool(v))
    end
end

function testcase.is_pointer()
    -- TODO
end

function testcase.is_num()
    -- test that return true
    assert.is_true(is.num(-INT))
    assert.is_true(is.num(-FLOAT))
    assert.is_true(is.num(ZERO))
    assert.is_true(is.num(INT))
    assert.is_true(is.num(FLOAT))
    assert.is_true(is.num(INF))
    assert.is_true(is.num(NAN))

    -- test that return false
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
        assert.is_false(is.num(v))
    end
end

function testcase.is_str()
    -- test that return true
    assert.is_true(is.str(STR))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.str(v))
    end
end

function testcase.is_table()
    -- test that return true
    assert.is_true(is.table(TBL))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
        STR,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.table(v))
    end
end

function testcase.is_func()
    -- test that return true
    assert.is_true(is.func(FUNC))
    assert.is_true(is.func(CFUNC))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
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
    }) do
        assert.is_false(is.func(v))
    end
end

function testcase.is_cfunc()
    -- test that return true
    assert.is_true(is.cfunc(CFUNC))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        FUNC,
        THREAD,
    }) do
        assert.is_false(is.cfunc(v))
    end
end

function testcase.is_userdata()
    -- test that return true
    assert.is_true(is.userdata(FILE))

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
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.userdata(v))
    end
end

function testcase.is_thread()
    -- test that return true
    assert.is_true(is.thread(THREAD))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        ZERO,
        INT,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        FUNC,
        CFUNC,
    }) do
        assert.is_false(is.thread(v))
    end
end

function testcase.is_finite()
    -- test that return true
    assert.is_true(is.finite(-FLOAT))
    assert.is_true(is.finite(-INT))
    assert.is_true(is.finite(ZERO))
    assert.is_true(is.finite(INT))
    assert.is_true(is.finite(FLOAT))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -INF,
        INF,
        -NAN,
        NAN,
        STR,
        TBL,
        FUNC,
        THREAD,
    }) do
        assert.is_false(is.finite(v))
    end
end

function testcase.is_unsigned()
    -- test that return true
    assert.is_true(is.unsigned(ZERO))
    assert.is_true(is.unsigned(INT))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        NAN,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.unsigned(v))
    end
end

function testcase.is_int()
    -- test that return true
    assert.is_true(is.int(-INT))
    assert.is_true(is.int(ZERO))
    assert.is_true(is.int(INT))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.int(v))
    end
end

function testcase.is_uint()
    -- test that return true
    assert.is_true(is.uint(ZERO))
    assert.is_true(is.uint(INT))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.uint(v))
    end
end

function testcase.is_pint()
    -- test that return true
    assert.is_true(is.pint(INT))

    -- test that return false
    for _, v in ipairs({
        true,
        false,
        FILE,
        -FLOAT,
        -INT,
        ZERO,
        FLOAT,
        INF,
        NAN,
        STR,
        TBL,
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.pint(v))
    end
end

function testcase.is_int8()
    -- test that return true
    for _, v in ipairs({
        -0x80,
        0,
        0x7F,
    }) do
        assert.is_true(is.int8(v))
    end

    -- test that return false
    for _, v in ipairs({
        -0x81,
        0x80,
        INF,
        NAN,
    }) do
        assert.is_false(is.int8(v))
    end
end

function testcase.is_uint8()
    -- test that return true
    for _, v in ipairs({
        0,
        0xFF,
    }) do
        assert.is_true(is.uint8(v))
    end

    -- test that return false
    for _, v in ipairs({
        -1,
        0x100,
        INF,
        NAN,
    }) do
        assert.is_false(is.uint8(v))
    end
end

function testcase.is_int16()
    -- test that return true
    for _, v in ipairs({
        -0x8000,
        0,
        0x7FFF,
    }) do
        assert.is_true(is.int16(v))
    end

    -- test that return false
    for _, v in ipairs({
        -0x8001,
        0x8000,
        INF,
        NAN,
    }) do
        assert.is_false(is.int16(v))
    end
end

function testcase.is_uint16()
    -- test that return true
    for _, v in ipairs({
        0,
        0xFFFF,
    }) do
        assert.is_true(is.uint16(v))
    end

    -- test that return false
    for _, v in ipairs({
        -1,
        0x10000,
        INF,
        NAN,
    }) do
        assert.is_false(is.uint16(v))
    end
end

function testcase.is_int32()
    -- test that return true
    for _, v in ipairs({
        -0x80000000,
        0,
        0x7FFFFFFF,
    }) do
        assert.is_true(is.int32(v))
    end

    -- test that return false
    for _, v in ipairs({
        -0x80000001,
        0x80000000,
        INF,
        NAN,
    }) do
        assert.is_false(is.int32(v))
    end
end

function testcase.is_uint32()
    -- test that return true
    for _, v in ipairs({
        0,
        0xFFFFFFFF,
    }) do
        assert.is_true(is.uint32(v))
    end

    -- test that return false
    for _, v in ipairs({
        -1,
        0x1FFFFFFFF,
        INF,
        NAN,
    }) do
        assert.is_false(is.uint32(v))
    end
end

function testcase.is_int64()
    -- test that return true
    for _, v in ipairs({
        -0x20000000000000,
        0,
        0x7FFFFFFFFFFFFC00,
    }) do
        assert.is_true(is.int64(v))
    end

    -- test that return false
    for _, v in ipairs({
        INF,
        NAN,
    }) do
        assert.is_false(is.int64(v))
    end
end

function testcase.is_uint64()
    -- test that return true
    for _, v in ipairs({
        0,
        0x7FFFFFFFFFFFFC00,
    }) do
        assert.is_true(is.uint64(v))
    end

    -- test that return false
    for _, v in ipairs({
        -1,
        INF,
        NAN,
    }) do
        assert.is_false(is.uint64(v))
    end
end

function testcase.is_file()
    -- test that return true
    assert.is_true(is.file(FILE))

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
        FUNC,
        CFUNC,
        THREAD,
    }) do
        assert.is_false(is.file(v))
    end
end

function testcase.is_callable()
    -- test that return true
    assert.is_true(is.callable(FUNC))
    assert.is_true(is.callable(CFUNC))
    assert.is_true(is.callable(setmetatable({}, {
        __metatable = 1,
        __call = FUNC,
    })))

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
        assert.is_false(is.callable(v))
    end
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

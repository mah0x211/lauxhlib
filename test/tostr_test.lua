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

local tostr = require('lauxhlib.tostr')

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

function testcase.tostr()
    -- test that return false
    for _, v in ipairs({
        {
            arg = true,
            eq = 'true',
        },
        {
            arg = false,
            eq = 'false',
        },
        {
            arg = FILE,
            eq = 'file ',
        },
        {
            arg = -INT,
            eq = '-1',
        },
        {
            arg = ZERO,
            eq = '0',
        },
        {
            arg = INT,
            eq = '1',
        },
        {
            arg = FLOAT,
            eq = '1.1',
        },
        {
            arg = INF,
            eq = 'inf',
        },
        {
            arg = -INF,
            eq = '-inf',
        },
        {
            arg = NAN,
            eq = 'nan',
        },
        {
            arg = STR,
            eq = 'str',
        },
        {
            arg = TBL,
            eq = '^table: ',
        },
        {
            arg = FUNC,
            eq = '^function: ',
        },
        {
            arg = CFUNC,
            eq = '^function: ',
        },
        {
            arg = THREAD,
            eq = '^thread: ',
        },
    }) do
        local s = tostr(v.arg)
        assert.match(s, v.eq, false)
    end
    assert.equal(tostr(nil), 'nil')
    assert.equal(tostr(), '')
end

-- run test cases
do
    for _, case in ipairs(testfuncs) do
        local t = clock()
        local ok, err = pcall(case.func)
        t = clock() - t
        if ok then
            printf('testcase.%s ... ok (%f sec)', case.name, t)
        else
            printf('testcase.%s ... failed (%f sec)', case.name, t)
            print('  > ' .. string.gsub(err, '\n', {
                ['\n'] = '\n  > ',
            }))
        end
    end
end

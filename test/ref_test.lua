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

local ref = require('lauxhlib.ref')

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

function testcase.ref_getref_unref()
    -- test that create reference
    local refs = {}
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
        local refv = assert(ref(v))
        assert.match(refv, '^lauxhlib.ref: ', false)
        refs[#refs + 1] = {
            ref = refv,
            val = v,
        }
    end

    -- test that get referenced value
    for _, v in ipairs(refs) do
        assert.equal(v.ref:get(), v.val)
    end

    -- test that unref
    for _, v in ipairs(refs) do
        assert.is_true(v.ref:unref())
        assert.is_false(v.ref:unref())
        assert.is_nil(v.ref:get())
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

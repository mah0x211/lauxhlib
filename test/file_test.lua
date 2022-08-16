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

local file = require('lauxhlib.file')

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

function testcase.fileno()
    -- test that return fileno
    assert.equal(file.fileno(io.stdin), 0)
    assert.equal(file.fileno(io.stdout), 1)
    assert.equal(file.fileno(io.stderr), 2)
    assert.greater(file.fileno(FILE), 2)

    -- test that return -1 if file is closed
    local f = assert(io.tmpfile())
    f:close()
    assert.equal(file.fileno(f), -1)

    -- test that throws an error
    for _, v in ipairs({
        true,
        false,
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
        local err = assert.throws(file.fileno, v)
        assert.match(err, '(FILE* expected, ')
    end
end

function testcase.tofile()
    -- test that create tmpfile from file descriptor
    local fd = file.fileno(FILE)
    local f, err = assert(file.tofile(fd))
    assert.is_file(f, 0)
    assert.is_nil(err)
    assert.equal(file.fileno(f), fd)

    -- test that create file from file descriptor
    f, err = assert(file.tofile(fd, 'w+', './hello.txt', 'ignored argument'))
    assert.is_file(f, 0)
    assert.is_nil(err)
    assert.equal(file.fileno(f), fd)
    assert(io.open('./hello.txt'))
    os.remove('./hello.txt')

    -- test that bad file descriptor error
    f, err = file.tofile(-1)
    assert.is_nil(f)
    assert.match(err, 'Bad file descriptor')

    -- test that invalid argument error
    f, err = file.tofile(fd, 'foo')
    assert.is_nil(f)
    assert.match(err, 'Invalid argument')
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

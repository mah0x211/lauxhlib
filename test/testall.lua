local pcall = pcall
local dofile = dofile
_G.package.cpath = '.libs/?.so;' .. _G.package.cpath

local errors = {}
for _, pathname in ipairs({
    'test/unittest.lua',
    'test/check_test.lua',
    'test/checkopt_test.lua',
    'test/file_test.lua',
    'test/is_test.lua',
    'test/ref_test.lua',
    'test/tostr_test.lua',
}) do
    print(string.rep('-', 70))
    print(pathname)
    print(string.rep('-', 70))
    local ok, err = pcall(dofile, pathname)
    if not ok then
        print('')
        print('FAILED:')
        print('  ' .. err)
        errors[#errors + 1] = {
            name = pathname,
            err = err,
        }
    end
    print('')
end

if #errors > 0 then
    print('TEST DOES NOT PASSED')
    for _, v in ipairs(errors) do
        print(string.rep('=', 70))
        print(v.name)
        print('  > ' .. v.err)
    end
    print('')
    os.exit(-1)
end
print('ALL TEST PASSED')


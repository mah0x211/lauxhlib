local pcall = pcall
local dofile = dofile
_G.package.cpath = '.libs/?.so;' .. _G.package.cpath

local test_passed = true
for _, pathname in ipairs({
    'test/unittest.lua',
}) do
    print(string.rep('-', 70))
    print(pathname)
    print(string.rep('-', 70))
    local ok, err = pcall(dofile, pathname)
    if not ok then
        test_passed = false
        print('FAILED:')
        print('  ' .. err)
    end
    print('')
end

if not test_passed then
    os.exit(-1)
end


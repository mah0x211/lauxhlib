rockspec_format = "3.0"
package = "lauxhlib"
version = "0.6.3-1"
source = {
    url = "git+https://github.com/mah0x211/lauxhlib.git",
    tag = "v0.6.3",
}
description = {
    summary = "A helper module for developing Lua modules in C language.",
    homepage = "https://github.com/mah0x211/lauxhlib",
    license = "MIT/X11",
    maintainer = "Masatoshi Fukunaga",
}
dependencies = {
    "lua >= 5.1",
}
build_dependencies = {
    "luarocks-build-hooks >= 0.7.0",
}
build = {
    type = "hooks",
    before_build = {
        "$(extra-vars)",
    },
    after_build = {
        "$(header-symlink)",
    },
    extra_variables = {
        CFLAGS = "-Wall -Wno-trigraphs -Wmissing-field-initializers -Wreturn-type -Wmissing-braces -Wparentheses -Wno-switch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wunused-value -Wuninitialized -Wunknown-pragmas -Wshadow -Wsign-compare",
    },
    conditional_variables = {
        LAUXHLIB_COVERAGE = {
            CFLAGS = "--coverage",
            LIBFLAG = "--coverage",
        },
    },
    modules = {
        ["lauxhlib.check"] = "src/check.c",
        ["lauxhlib.checkopt"] = "src/checkopt.c",
        ["lauxhlib.file"] = "src/file.c",
        ["lauxhlib.is"] = "src/is.c",
        ["lauxhlib.ref"] = "src/ref.c",
        ["lauxhlib.tostring"] = "src/tostring.c",
    },
    install = {
        conf = {
            ["lauxhlib.h"] = "src/lauxhlib.h",
        },
    },
}

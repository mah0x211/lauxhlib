package = "lauxhlib"
version = "scm-1"
source = {
    url = "https://github.com/mah0x211/lauxhlib.git"
}
description = {
    summary = "lauxhlib test",
    homepage = "https://github.com/mah0x211/lauxhlib",
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1"
}
build = {
    type = "make",
    build_variables = {
        PACKAGE         = "lauxhlib",
        CFLAGS          = "$(CFLAGS)",
        WARNINGS        = "-Wall -Wno-trigraphs -Wmissing-field-initializers -Wreturn-type -Wmissing-braces -Wparentheses -Wno-switch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wunused-value -Wuninitialized -Wunknown-pragmas -Wshadow -Wsign-compare",
        CPPFLAGS        = "-I$(LUA_INCDIR)",
        LDFLAGS         = "$(LIBFLAG)",
        LIB_EXTENSION   = "$(LIB_EXTENSION)"
    },
    install_variables = {
        PACKAGE         = "lauxhlib",
        LIB_EXTENSION   = "$(LIB_EXTENSION)"
    }
}

package = "lauxhlib"
version = "0.1.0-1"
source = {
   url = "git+https://github.com/mah0x211/lauxhlib.git",
   tag = "v0.1.0"
}
description = {
   summary = "A helper module for developing Lua modules in C language.",
   homepage = "https://github.com/mah0x211/lauxhlib",
   license = "MIT/X11",
   maintainer = "Masatoshi Fukunaga"
}
dependencies = {
   "lua >= 5.1"
}
build = {
   type = "make",
   build_variables = {
      CFLAGS = "$(CFLAGS)",
      CPPFLAGS = "-I$(LUA_INCDIR)",
      LDFLAGS = "$(LIBFLAG)",
      LIB_EXTENSION = "$(LIB_EXTENSION)",
      PACKAGE = "lauxhlib",
      WARNINGS = "-Wall -Wno-trigraphs -Wmissing-field-initializers -Wreturn-type -Wmissing-braces -Wparentheses -Wno-switch -Wunused-function -Wunused-label -Wunused-parameter -Wunused-variable -Wunused-value -Wuninitialized -Wunknown-pragmas -Wshadow -Wsign-compare"
   },
   install_variables = {
      CONFDIR = "$(CONFDIR)",
      LIB_EXTENSION = "$(LIB_EXTENSION)",
      LUA_INCDIR = "$(LUA_INCDIR)",
      PACKAGE = "lauxhlib"
   }
}

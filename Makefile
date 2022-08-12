TARGET=$(PACKAGE).$(LIB_EXTENSION)
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
GCDAS=$(OBJ:.o=.gcda)
INSTALL?=install

ifdef LAUXHLIB_COVERAGE
COVFRAGS="--coverage"
endif

.EXPORT_ALL_VARIABLES:

LUA_CPATH:=./?.so;$(LUA_CPATH)

.PHONY: all install

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(COVFRAGS) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) $(PLATFORM_LDFLAGS) $(COVFRAGS)

install:
	rm -f $(OBJ) $(GCDAS)
	lua ./test/test.lua
	rm -f $(TARGET)
	$(INSTALL) src/lauxhlib.h $(CONFDIR)
	rm -f $(LUA_INCDIR)/lauxhlib.h
	ln -s $(CONFDIR)/lauxhlib.h $(LUA_INCDIR)

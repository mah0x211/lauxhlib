SRCS=$(wildcard src/*.c)
SOBJ=$(SRCS:.c=.$(LIB_EXTENSION))
GCDAS=$(SOBJ:.so=.gcda)
INSTALL?=install

ifdef LAUXHLIB_COVERAGE
COVFRAGS=--coverage
endif

.EXPORT_ALL_VARIABLES:

LUA_CPATH:=./?.so;$(LUA_CPATH)

.PHONY: all install

all: $(SOBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(COVFRAGS) $(CPPFLAGS) -o $@ -c $<

%.$(LIB_EXTENSION): %.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) $(PLATFORM_LDFLAGS) $(COVFRAGS)

install: $(SOBJ)
	$(INSTALL) -d $(INST_LIBDIR)
	$(INSTALL) $(SOBJ) $(INST_LIBDIR)
	$(INSTALL) src/lauxhlib.h $(CONFDIR)
	rm -f $(LUA_INCDIR)/lauxhlib.h
	-[ -w $(LUA_INCDIR) ] && ln -s $(CONFDIR)/lauxhlib.h $(LUA_INCDIR)
	rm -f $(SOBJ) $(GCDAS)

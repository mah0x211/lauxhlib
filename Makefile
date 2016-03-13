TARGET=$(PACKAGE).$(LIB_EXTENSION)
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)


all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) $(PLATFORM_LDFLAGS)

install:
	lua ./test/test.lua
	rm -f $(OBJ) $(TARGET)

CC = cc

#CFLAGS  = -g3 -ggdb
CFLAGS  = -O2

CFLAGS += -pipe -fdiagnostics-color -Wno-unknown-warning-option -Wpedantic
CFLAGS += -Wall -Werror-implicit-function-declaration -Wno-format-truncation
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wswitch-enum
CFLAGS += -Wuninitialized -Wformat-security -Wformat-overflow=2 -Wextra
CFLAGS += -I/usr/X11R6/include -I/usr/local/include
LDADD   = -L /usr/X11R6/lib -L/usr/local/lib -lm -lX11 -lXrandr

BIN   = ctempd
OBJS  = ctempd.o

INSTALL ?= install -p

PREFIX ?= /usr/local
BIND ?= $(PREFIX)/bin
MAN ?= $(PREFIX)/man
SHARE ?= $(PREFIX)/share

all: $(BIN)

install: all
	$(INSTALL) -d -m 755 -o root $(MAN)/man1
	$(INSTALL) -m 755 -o root $(BIN) $(BIND)
	$(INSTALL) -m 644 -o root $(BIN).1 $(MAN)/man1

uninstall:
	rm -f $(MAN)/man1/$(BIN).1
	rm -f $(BIND)/$(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDADD)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(BIN) $(OBJS)


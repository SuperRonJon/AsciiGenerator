CC 			?= gcc
CFLAGS		?= -Wall -Wextra -Werror -O2
DBGFLAGS	?= -Wall -Wextra -Werror -g

INSTALL		:= install
INSTALLBIN	:= $(INSTALL) -m 755

BUILDDIR	?= build
SRCDIR		?= src
PREFIX		?= /usr/local
BINDIR		:= $(PREFIX)/bin

ifeq ($(OS),WINDOWS_NT)
	EXE_EXT := .exe
else
	EXE_EXT :=
endif

PROGRAM		:= asciigen
TARGET		:= $(BUILDDIR)/$(PROGRAM)$(EXE_EXT)
DBGTARGET	:= $(BUILDDIR)/debug$(EXE_EXT)

SRCS		:= $(SRCDIR)/asciigen.c
OBJS		:= $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

.PHONY: all clean debug $(PROGRAM) install

$(PROGRAM): $(TARGET)

debug: $(DBGTARGET)

all: $(TARGET) $(DBGTARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -s $^ -lm -o $@

$(DBGTARGET): $(SRCS) | $(BUILDDIR)
	$(CC) $(DBGFLAGS) $^ -lm -o $@

$(BUILDDIR)/%.o: $(SRCS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

install: $(TARGET)
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALLBIN) $(TARGET) $(DESTDIR)$(BINDIR)/$(PROGRAM)$(EXE_EXT)

clean: 
	rm -f $(TARGET) $(DBGTARGET) $(OBJS)

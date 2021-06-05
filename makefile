OPSYS ?= linux
CC ?= cc
LD := $(CC)
ifneq ($(OPSYS),macos)
	ifeq ($(OPSYS),openbsd)
		LIBS = -lm
	else
		LIBS = -lm -ldl
	endif
endif
LIBSRASPI = -lm -ldl -lwiringPi
INCS =  
CFLAGS ?= $(INCS) -Wall -Wextra -D_FORTIFY_SOURCE=2
ifeq ($(DEBUG),1)
	CFLAGS += -O0 -g
else
	CFLAGS += -O3 -flto
endif
ifeq ($(CC),c++)
	CFLAGS += -std=c++98 -fno-exceptions -fno-rtti -Weffc++
else
	CFLAGS += -std=c17
endif
ifneq ($(DEBUG),1)
	LDFLAGS := -flto
	ifeq ($(OPSYS),macos)
		LDFLAGS += -Wl,-S,-x
	else
		LDFLAGS += -s
	endif
endif
PREFIX = /usr/local
bindir = $(PREFIX)/bin
DESTDIR = 
INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -m755

EISL = eisl
EDLIS = edlis

FILES = library

EISL_OBJS = main.o \
	function.o \
	extension.o \
	syntax.o \
	data.o \
	gbc.o \
	cell.o \
	error.o \
	bignum.o \
	compute.o \
	edit.o

all: eisl edlis

eisl:
ifeq  ($(shell uname -n),raspberrypi)
eisl1: $(EISL_OBJS) $(EISL)
$(EISL): $(EISL_OBJS)
	$(CC) $(CFLAGS) $(EISL_OBJS) -o $(EISL) $(LIBSRASPI) 
else
eisl2: $(EISL_OBJS) $(EISL)
$(EISL): $(EISL_OBJS)
	$(LD) $(LDFLAGS) $(EISL_OBJS) -o $(EISL) $(LIBS) 
endif



%.o: %.c eisl.h
	$(CC) $(CFLAGS) -c $< -o $@

edlis : edlis.o
	$(LD) $(LDFLAGS) edlis.o -o edlis
edlis.o : edlis.c edlis.h
	$(CC) $(CFLAGS) -c edlis.c


install: $(EISL) $(EDLIS)
	$(INSTALL_PROGRAM) $(EISL) $(DESTDIR)$(bindir)/$(EISL)
	$(INSTALL_PROGRAM) $(EDLIS) $(DESTDIR)$(bindir)/$(EDLIS)

uninstall:
	rm $(DESTDIR)$(bindir)/$(EISL)
	rm $(DESTDIR)$(bindir)/$(EDLIS)



.PHONY: clean
clean:
	rm -f *.o
	rm eisl
	rm edlis

.PHONY: check
check:
	cppcheck --enable=all --std=c11 --library=posix .

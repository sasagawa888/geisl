CC := gcc
NVCC := nvcc
LIBS = -lm -ldl -lcublas
INCS =  
CFLAGS ?= $(INCS) -O3 
PREFIX = /usr/local
bindir = $(PREFIX)/bin
DESTDIR = 
INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -Dm755

EISL = geisl
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
	edit.o \
	gpgpu.o \
	kernel.o

all: geisl

geisl:
geisl2: $(EISL_OBJS) $(EISL)
$(EISL): $(EISL_OBJS)
	$(NVCC) $(CFLAGS) $(EISL_OBJS) -o $(EISL) $(LIBS) 


kernel.o : 
	nvcc -c kernel.cu -o kernel.o

gpgpu.o :
	nvcc -c gpgpu.c -o gpgpu.o

extension.o :
	nvcc -c extension.c -o extension.o

%.o: %.c eisl.h
	$(CC) $(CFLAGS) -c $< -o $@


install: $(EISL) 
	$(INSTALL_PROGRAM) $(EISL) $(DESTDIR)$(bindir)/$(EISL)


uninstall:
	rm $(DESTDIR)$(bindir)/$(EISL)


.PHONY: clean
clean: -lm
	rm -f *.o
	rm geisl



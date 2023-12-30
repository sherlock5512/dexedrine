# Define the variables for the compiler and the flags
CC ?= gcc
CFLAGS += -Wall -g -std=c99 -pedantic

# Define the libraries to link
LIBS = -lsystemd -lbsd

prefix ?= /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
dexpath = $(bindir)/dexedrine

.PHONY: all clean install

# Define the default rule to build the program
all: dexedrine dexedrine.service

# Define the rule to compile the source file
dexedrine: dexedrine.c dexedrine.h
	$(CC) $(CFLAGS) dexedrine.c -o dexedrine $(LIBS)

dexedrine.service: dexedrine.service.template
	sed --posix "s@%DEXPATH%@$(dexpath)@" dexedrine.service.template > dexedrine.service

# Define the rule to clean the generated files
clean:
	-rm -f dexedrine
	-rm -f dexedrine.service

# Define how to install

install: all
	mkdir -p $(DESTDIR)$(bindir)
	mkdir -p $(DESTDIR)/usr/lib/systemd/user
	install dexedrine $(DESTDIR)$(dexpath)
	install dexedrine.service $(DESTDIR)/usr/lib/systemd/user/dexedrine.service

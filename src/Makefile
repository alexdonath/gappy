###########################################################################
# last changed Time-stamp: <2017-08-17 11:21:16 adonath>
###########################################################################

# Installation targets
#
prefix = $(CURDIR)
bindir = $(prefix)/../bin

# Compiler details
#
CC = g++
# compile-time flags
CFLAGS = -ansi -g -O3 -funroll-loops
WARN = -Wall -Wpedantic
# define any directories containing header files
INCLUDES = -I/usr/include
# library paths
LFLAGS = -L/usr/lib64/ -L/usr/lib/
# define any libraries to link into executable:
LIBS = 
# source files
SOURCES = gaps.cpp utils.cpp options.cpp files.cpp
# object files 
OBJS = ${SOURCES:.cpp=.o}

# Package information
#
MAIN 	= gappy
VERSION	= 0.0.2

# Other tools
#
INSTALL = /usr/bin/install -c

.PHONY: all clean install uninstall

all:	$(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(WARN) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(WARN) $(CFLAGS) $(INCLUDES) -c $<  -o $@

install:
	$(INSTALL) -d $(bindir)
	for file in $(MAIN); do \
	   $(INSTALL) -m 0755 $$file $(bindir)/ ;\
	done

uninstall:
	-rm -rf $(bindir)
	-rm -f $(OBJS) *~ $(MAIN)

clean:
	-rm -f $(OBJS) *~ $(MAIN)

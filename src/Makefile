###########################################################################
# last changed Time-stamp: <2017-05-29 16:29:18 adonath>
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
WARN = -Wall -pedantic-errors
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
VERSION	= 0.0.1

# Other tools
#
INSTALL = /usr/bin/install -c

.PHONY: all clean install uninstall

all:	$(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

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

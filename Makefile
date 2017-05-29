###########################################################################
# last changed Time-stamp: <2017-05-29 16:31:15 adonath>
###########################################################################
#
# 'make'            build gappy binary
# 'make install'    install gappy binary in bin/
# 'make uninstall'  uninstall gappy
# 'make clean'      remove all .o and executable files

# Installation targets
#
prefix = $(CURDIR)
bindir = $(prefix)/bin

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
OBJS = $(SOURCES:.cpp=.o)

# Package information
#
MAIN 	= gappy
VERSION	= 0.0.1

# Other tools
#
INSTALL = /usr/bin/install -c


###

.PHONY: all clean install uninstall

#all: Compile gappy binary
all:
	@echo Building $(MAIN) version $(VERSION).
	$(MAKE) -C src/ all
	@echo Done building $(MAIN) version $(VERSION).

# this is a suffix replacement rule for building .o's from .cpp's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .cpp file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

# install: installs the binary in $(bindir)/
install:
	@echo Installing gappy in ./bin/
	$(MAKE) -C src/ install
	@echo Done installing.

uninstall:
	@echo Uninstalling gappy.
	$(MAKE) -C src/ uninstall
	@echo Done uninstalling.

clean:
	@echo Remove unecessary files.
	$(MAKE) -C src/ clean
	@echo Done removing.

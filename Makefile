###########################################################################
# last changed Time-stamp: <2016-10-21 14:54:25 adonath>
###########################################################################
#
# 'make'            build executable file 'gappy'
# 'make clean'      removes all .o and executable files

# define the CPP compiler
CC = g++
# define compile-time flags
CFLAGS = -ansi -g -O3 -funroll-loops
WARN = -Wall -pedantic-errors
# define any directories containing header files
INCLUDES = -I/usr/include
# define library paths
LFLAGS = -L/usr/lib64/ -L/usr/lib/
# define any libraries to link into executable:
LIBS = 
# define the CPP source files
SOURCES = gaps.cpp utils.cpp options.cpp files.cpp
# define the CPP object files 
OBJS = $(SOURCES:.cpp=.o)
# define the executable file 
MAIN = gappy
VERSION	 = 1.0

###

.PHONY: depend clean

all: 	$(MAIN)
	@echo  done building $(MAIN).

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .cpp's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .cpp file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
clean:
	$(RM) $(OBJS) *~ $(MAIN)

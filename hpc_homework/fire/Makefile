# name of the binary
PROGRAM   = Fire
# source files
SRCS      = firestarter.c
# object files from source files
OBJS      = $(SRCS:.c=.o)

# which compiler to use
CC        = mpicc
# flags for compilation and linking 
CFLAGS    = -Wall
LFLAGS    = -o $(PROGRAM) -lm

# valid file suffixes 
.SUFFIXES: .c .o .cpp

# command to build .o files from .c files
.c.o: 
	$(CC) -c $(CFLAGS) $<

# command to link object files into binary
$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) 

# other dependencies (based on #includes)

clean:
	/bin/rm -f $(OBJS) $(PROGRAM) *~ *#


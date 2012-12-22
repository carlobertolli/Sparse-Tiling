ST_INSTALL_PATH = ../support

SUPPORT = $(ST_INSTALL_PATH)/lib
INCLUDE = $(ST_INSTALL_PATH)/include

CC		= gcc

LFLAGS	= -lmetis
EXEC	= testPartitioningColoring

CFLAGS	= -Wall -pedantic -std=c99  

all: library testPartitioning
	
library:
	make -f $(ST_INSTALL_PATH)/Makefile

testPartitioning:
	$(CC) $(CFLAGS) -I$(INCLUDE) testColoringPartitioning.c $(SUPPORT)/libst_op2.a $(LFLAGS)
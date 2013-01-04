ST_INSTALL_PATH = ../support

SUPPORT = $(ST_INSTALL_PATH)/lib
INCLUDE = $(ST_INSTALL_PATH)/include

CC		= gcc

LFLAGS	= -lmetis
EXEC	= testPartitioningColoring

CFLAGS	= -Wall -pedantic -std=c99  

all: library testInvert testPartitioning testTile testSmallGraph
	
library:
	make -f $(ST_INSTALL_PATH)/Makefile

testInvert:
	$(CC) $(CFLAGS) -I$(INCLUDE) testInvert.c $(SUPPORT)/libst_op2.a $(LFLAGS) -o invert

testPartitioning:
	$(CC) $(CFLAGS) -I$(INCLUDE) testColoringPartitioning.c $(SUPPORT)/libst_op2.a $(LFLAGS) -o partition_color
	
testTile:
	$(CC) $(CFLAGS) -I$(INCLUDE) testTile.c $(SUPPORT)/libst_op2.a $(LFLAGS) -o tiling

testTile:
	$(CC) $(CFLAGS) -I$(INCLUDE) testTile.c $(SUPPORT)/libst_op2.a $(LFLAGS) -o test_1

testSmallGraph:
	$(CC) $(CFLAGS) -I$(INCLUDE) testSmallGraph.c $(SUPPORT)/libst_op2.a $(LFLAGS) -o test_2
	
clean:
	rm -rf invert partition_color test_1 test_2 *~
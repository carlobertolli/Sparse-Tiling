/*
 * Contain the definition of the tile data structure and functions to manipulate it.
 */

#ifndef _TILE_H_
#define _TILE_H_

#include <stdio.h>

#define MAXLOOPS			10 // actually, there is no specific reason to specify but simplifying code 

#define TILEOP_OK				1
#define TILEOP_TOOMANYLOOPS		0
#define TILEOP_TOOMANYELEMS		-1

typedef struct {
  int ID;	                 //Unique identifier of the tile
  int nloops;              //total number of parallel loops crossed by the tile
  int* element[MAXLOOPS];  //iteration set, for each par loop
  int size[MAXLOOPS];	     //size of the iteration set, for each par loop
  
  int curSize[MAXLOOPS];   //current number of elements added per loop
  
  char* loopname[MAXLOOPS];
} tile_t;


/*
 * Create an empty tile that can span across multiple parloops 
 *
 * input:
 * nloops	: number of loops crossed by the tile
 * sizes	: size of the data set 
 */
tile_t* createTile ();

/*
 * Add an element el to the iteration set of the parloop loop 
 */
int addElement (tile_t* tile, int loop, int el);

/*
 * Add a par loop to the tile. 
 * It is important to specify the correct (upper bound of the) size of the iteration set iterated by the loop.  
 */
int addLoop (tile_t* tile, int size, char* setName);

/*
 * Run an entire tile
 */
int runTile (tile_t* tile);

/*
 * Destroy a tile
 */
void freeTile (tile_t* tile);

#endif

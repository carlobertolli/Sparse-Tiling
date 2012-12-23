/*
 * Contain the definition of the tile data structure and functions to manipulate it.
 */

#ifndef _TILE_H_
#define _TILE_H_

#include <stdio.h>

typedef struct {
	int nloops;			//total number of parallel loops crossed by the tile 
	int** element;	//iteration set, for each par loop
	int* size;			//size of the iteration set, for each par loop

} tile_t;


/*
 * Create an empty tile that can span across multiple parloops 
 */
tile_t* createTile ();

/*
 * Add an element el to the iteration set of the parloop loop 
 */
void addELement (tile_t tile, int loop, int el);

/*
 * Destroy a tile
 */
void freeTile (tile_t* tile);

#endif

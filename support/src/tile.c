/*
 *  tile.c
 */

#include <stdlib.h>

#include "tile.h"

tile_t* createTile ()
{
	tile_t* tile = (tile_t*) malloc ( sizeof(tile_t) );
	tile->nloops = 0;
	
	return tile;
}

void addELement (tile_t tile, int loop, int el)
{
	
}

void freeTile (tile_t* tile)
{
	for ( int i = 0; i < tile->nloops; i++ )
		free (tile->element[i]);
	
	free (tile->element);
	free (tile->size);
	free (tile);
}


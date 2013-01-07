/*
 *  tile.c
 */

#include <stdlib.h>

#include "tile.h"


tile_t* createTile ()
{
  static int tileID = 0; // tile identifier, initially set to 0 and incremented each time a tile is created
  
  tile_t* tile = (tile_t*) malloc ( sizeof(tile_t) );
  tile->nloops = 0;
  tile->ID = tileID++;
  
  for (int i = 0; i < MAXLOOPS; i++)
    tile->curSize[i] = 0;
  
  return tile;
}

int addElement (tile_t* tile, int loop, int el)
{
  if (tile->curSize[loop] + 1 > tile->size[loop])
    return TILEOP_TOOMANYELEMS;
  
  tile->element[loop][tile->curSize[loop]] = el; //add the element to the proper (loop, offset) in the tile 
  tile->curSize[loop]++;
  
  return TILEOP_OK;
}

int addLoop (tile_t* tile, int size, char* setName)
{
  if (tile->nloops + 1 == MAXLOOPS)
    return TILEOP_TOOMANYLOOPS;
  
  tile->loopname[tile->nloops] = setName;
  tile->size[tile->nloops] = size;
  tile->element[tile->nloops] = (int*) malloc (size * sizeof(int)); 
  
  tile->nloops++;
  return TILEOP_OK;
}

int runTile (tile_t* tile)
{
  printf ("\nExecuting Tile %d\n", tile->ID);
  for (int i = 0; i < tile->nloops; i++)
  {
    printf("Executing Loop %d over %s\n\tExecuted element: ", i, tile->loopname[i]);
    for (int j = 0; j < tile->curSize[i]; j++)
      printf("%d ", tile->element[i][j] );
    printf("\n");
    
  }
  
  return 0;
}

void freeTile (tile_t* tile)
{
  for ( int i = 0; i < tile->nloops; i++ )
    free (tile->element[i]);
  
  free (tile);
}

void printTile (tile_t* tile)
{
  printf("Tile ID: %d\n", tile->ID);
  printf("Number of loops crossed : %d\n", tile->nloops);
  
  if (! tile->nloops )
  {
    printf("Empty tile!\n");
    return;
  }
  
  for (int i = 0; i < tile->nloops; i++)
  {
    printf("Loop %d - %s - size %d - space allocated %d (elements):\n\t", i, tile->loopname[i], tile->curSize[i], tile->size[i]);
    for (int j = 0; j < tile->curSize[i]; j++)	
      printf("%d ", tile->element[i][j]);
    printf("\n");
  }
  
  printf("Tile printed!\n\n");
}

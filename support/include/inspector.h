/*
 *  inspector.h
 *  
 * Defines data structure and prototypes to build an inspector
 *
 */

#ifndef _INSPECTOR_H_
#define _INSPECTOR_H_

#include "tile.h"

typedef struct {
	int size;					//size of the base set
	int* col_prev;		//color of the base set for the two previous parloops. 
	int* col_current; //current color for each element of the base set
	
	int ntiles;				//number of tiles for this inspector
	tile_t** tiles;		//tiles of the inspector
	
	int* p2v;					//mapping from partitions to vertices
	int* partSize;		//initial size of the tiles. The size is ntiles
} inspector_t;

/*
 * Initialize a new inspector with a certain number of tiles
 *
 * input:
 * partSize : partition size requested
 * baseset	: size of the baseset
 *
 * output:
 * inspector: a new inspector
 */
inspector_t* initInspector (int baseset, int partSize);

/*
 * Destroy an inspector
 */
void freeInspector (inspector_t* insp);

/*
 * Inspect two subsequent parallel loops and builds the tiles of the inspector
 * TODO: need to add a back and forth variable? 
 */
void scanParLoop (inspector_t* insp, int fstSetSize, int* fstMap, int fstMapSize, int sndSetSize, int* sndMap, int sndMapSize);

/*
 * Add a parallel loop to the inspector (ie. the inspector) 
 * TODO: Should be unnecessary: it is enaugh to keep a global variable, initialize to 1 by initInspector,
 * and decrement it each time a scanParLoop is invoked. At the beginning of scanParLoop, the variable is tested:
 * if it is 0, then a new coloring on the baseset is computed
 */
//void addParLoop (inspector_t* insp)

/*
 * This function can be called just once after the inspector initialization.
 * Take the vertex set, partition and color it. 
 *
 * input:
 * insp				: an initialized inspector
 * vertices		: #vertices
 * e2v				: standard map e2v
 * mapsize		: size of the mapping e2v
 * 
 * output:
 * partitions	: vertex -> partitionID
 * colors			: partitionID -> color 
 */
void partitionAndColor (inspector_t* insp, int vertices, int* e2v, int mapsize);

#endif
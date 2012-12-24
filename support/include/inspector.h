/*
 *  inspector.h
 *  
 * Defines data structure and prototypes to build an inspector
 *
 */

#ifndef _INSPECTOR_H_
#define _INSPECTOR_H_

#include "tile.h"

#define INSPOP_OK				1
#define	INSPOP_MAXLOOP	0

/* This struct contains information about a specific par loop
 *
 */
typedef struct {
	char* loopname;		//name/identified of the parloop
	int setSize;			//size of the iteration set
	int* indMap;			//indirect map to the base set
	int mapSize;			//size of indMap
	
} loop_t;

/* The Inspector data structure.
 *
 */
typedef struct {
	int size;					//size of the base set
	int* col_prev;		//color of p2v for the two previous parloops. 
	int* col_current; //current color for each element of p2v
	
	int ntiles;				//number of tiles for this inspector
	tile_t** tiles;		//tiles of the inspector
	
	int nloops;				//number of loops crossed 
	loop_t** loops;		//loops crossed 
	int loopCounter;	//count the number of loops currently add to the inspector
	
	int* p2v;					//mapping from partitions to vertices
	int* v2v;					//mapping from v to v in p2v 
	int* partSize;		//initial size of the tiles. The size is ntiles

} inspector_t;



/*
 * Initialize a new inspector with a certain number of tiles
 *
 * input:
 * partSize : partition size requested
 * baseset	: size of the baseset
 * loops		: loops crossed by the inspector
 *
 * output:
 * inspector: a new inspector
 */
inspector_t* initInspector (int baseset, int partSize, int loops);

/*
 * Destroy an inspector
 */
void freeInspector (inspector_t* insp);

/*
 * Inspect two subsequent parallel loops and builds the tiles of the inspector
 * TODO: need to add a back and forth variable? 
 */
void scanParLoops (inspector_t* insp, int fstSetSize, int* fstMap, int fstMapEl, int fstMapSize, int sndSetSize, int* sndMap, int sndMapSize, int sndMapEl);

/*
 * Add a parallel loop to the inspector 
 * This way the inspector is aware of the characteristics of the parloops is going to tile.
 * Notion of parloop equivalence: two parloops are considered equivalent by the tiling inspector iff:
 * - they iterate over the same iteration set
 * - they use the same indirection mapping to access the base set
 *
 * IMPORTANT: the sequence of par loops in the OP2 code MUST match the sequence of addParLoop invokations (i.e. same ordering)  
 * IMPORTANT: all calls to addParLoop must be made before any call to scanParLoops
 *
 * input:
 * loopname				: unique identifier for the parallel loop
 * setSize				: size of the iteration set
 * indirectionMap : indirect map used by the loop to access the base set
 * mapSize				: size of indirectionMap
 *
 */
int addParLoop (inspector_t* insp, char* loopname, int setSize, int* indirectionMap, int mapSize );

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
int partitionAndColor (inspector_t* insp, int vertices, int* e2v, int mapsize);

#endif
/*
 *  inspector.h
 *  
 * Defines data structure and prototypes to build an inspector
 *
 */

#ifndef _INSPECTOR_H_
#define _INSPECTOR_H_

#define DEBUG 0

#include "tile.h"

#define INSPOP_OK 1
#define	INSPOP_MAXLOOP	0
#define INSPOP_WRONGPAR -1
#define INSPOP_NOTENAUGHLOOP -2
#define INSPOP_WRONGCOLOR -3

#define LOOPNAMELENGTH 32
#define DEBUGMSGLENGTH 128

/* This struct contains information about a specific par loop
 *
 */
typedef struct {
  char* loopname; //name/identifier of the parloop
  int setSize;    //size of the iteration set
  int* indMap;    //indirect map to the renumbered base set
  int mapSize;    //size of indMap
  
#ifdef VTK_ON
  int* setColor;  //array of colors, one for each element of the set
#endif
  
  char debug[DEBUGMSGLENGTH];
  
} loop_t;

/* The Inspector data structure.
 *
 */
typedef struct {
  int size;         //size of the base set
  int* v2pOrig;     //v2p mapping given by metis 
  int* colOrig;     //current color for each element of v2p
  
  int ncolors;      //total number of colors determined by the inspector
  int* p2c;         //mapping from partitions to colors
  
  int ntiles;       //number of tiles for this inspector
  tile_t** tiles;   //tiles of the inspector
  
  int nloops;       //number of loops crossed 
  loop_t** loops;   //loops crossed 
  int loopCounter;  //count the number of loops currently add to the inspector
  
  int* p2v;         //mapping from partitions to vertices
  int* v2v;         //mapping from v to v in p2v 
  int* partSize;    //initial size of the tiles. The size is ntiles
  
  int incidence;    //maximum incidence of the mesh
  
  char debug[DEBUGMSGLENGTH*2 + LOOPNAMELENGTH];
  
} inspector_t;



/*
 * Initialize a new inspector with a certain number of tiles
 *
 * input:
 * partSize   : partition size requested
 * baseset    : size of the baseset
 * loops      : loops crossed by the inspector
 *
 * output:
 * inspector  : a new inspector
 */
inspector_t* initInspector (int baseset, int partSize, int loops);

/*
 * Destroy an inspector
 */
void freeInspector (inspector_t* insp);

/*
 * Inspect a sequence of parallel loops and compute the coloring.
 * Parallel loop have been previously added to the inspector by means of addParLoop
 * 
 * input:
 * insp           : inspector
 * baseSetIndex   : starting point of the coloring [0, insp->nloops - 1)
 * 
 */
int runInspector (inspector_t* insp, int baseSetIndex);

/*
 * Add a parallel loop to the inspector 
 * This way the inspector get to know the characteristics of the parloops is going to tile.
 *
 * IMPORTANT: the sequence of par loops in the OP2 code MUST match the sequence of addParLoop invokations (i.e. same ordering)  
 * IMPORTANT: all calls to addParLoop must be made before the call to runInspector
 *
 * input:
 * insp           : inspector
 * loopname       : string identifying the parallel loop
 * setSize        : size of the iteration set
 * indirectionMap : indirect map used by the loop to access the base set
 * mapSize        : size of indirectionMap
 *
 */
int addParLoop (inspector_t* insp, char* loopname, int setSize, int* indirectionMap, int mapSize);

/*
 * This function can be called just once after the inspector initialization.
 * Take the vertex set, partition and color it. 
 *
 * input:
 * insp       : an initialized inspector
 * vertices   : #vertices
 * e2v        : standard map e2v
 * mapsize    : size of the mapping e2v
 * 
 * output:
 * partitions : vertex -> partitionID
 * colors     : partitionID -> color 
 */
int partitionAndColor (inspector_t* insp, int vertices, int* e2v, int mapsize);

#endif

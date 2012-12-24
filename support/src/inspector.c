/*
 *  inspector.c
 *  
 */

#include <string.h>

#include <metis.h>

#include "inspector.h"
#include "invert.h"

#define MAX(A, B)	(A > B) ? A : B
#define MIN(A, B)	(A < B) ? A : B

/* input:
 * vertices		: #vertices
 * nparts			: number of partitions requested to metis
 * xadj				: vertices offsets in adjncy - standard CSR format
 * adjncy			: v2v mapping - standard CSR format
 * 
 * output:
 * *part			: part[i] == partition of vertex 1, computed by metis
 */
int metisPartition (int vertices, int _nparts, idx_t* xadj, idx_t* adjncy, int** part)
{
	//input
	idx_t nvtxs	 = (idx_t) vertices;	// #vertices in the graph
	idx_t ncon	 = 1;					// #balancing constraint
	idx_t nparts = (idx_t) _nparts;		// #graph partitions
	
	idx_t options[METIS_NOPTIONS];
	
	METIS_SetDefaultOptions(options);
	options[METIS_OPTION_NUMBERING] = 0;		  
	options[METIS_OPTION_CONTIG] = 1;
	
	//output
	idx_t objval;	// edge-cut 
	idx_t* _part = (idx_t*) malloc( nvtxs * sizeof (idx_t)); //partition array 
	
	//computes the partitioning
	int result = METIS_PartGraphKway( &nvtxs, &ncon, xadj, adjncy, NULL, NULL, NULL, &nparts, NULL, NULL, options, &objval, _part );
	
	*part = (int*) _part;
	return result;
}


void printInspector (inspector_t* insp)
{
	if ( !insp )
	{
		printf("insp is NULL\n");
		return;
	}

	printf("size of the base set: \t%d\n", insp->size);
	printf("number of tiles: \t%d\n", insp->ntiles);
	
	printf("previous coloring set:\n\t");
	if ( insp->col_prev )
		for (int i = 0; i < insp->size; i++ )
			printf ("%d ", insp->col_prev[i]);
	else 
		printf("no previous coloring set");
	printf("\n\n");
	
	printf("current coloring set:\n\t");
	if ( insp->col_current )
		for (int i = 0; i < insp->size; i++ )
			printf ("%d ", insp->col_current[i]);
	else 
		printf("no current coloring set");
	printf("\n\n");

	printf("Initial distribution of the base set among tiles:\n");
	for (int b = 0; b < insp->ntiles; b++ )
	{
		int offset = ( b ) ? insp->partSize[b - 1] : 0;
		for (int j = 0; j < insp->partSize[b]; j++ )
			printf("\t%d ", insp->p2v[b*offset + j]);
		printf("\n");
	}
	
	printf("LOOPS (%d crossed / %d allowed):\n", insp->loopCounter, insp->nloops);
	for (int i = 0; i < insp->loopCounter; i++)
	{
		printf("\tLoop %d (setSize: %d, mapSize: %d)\n", i, insp->loops[i]->setSize, insp->loops[i]->mapSize);
		printf("\tIndirect map (to the *renumbered* base set: \n\t\t");
		for (int j = 0; j < insp->loops[i]->mapSize; j++ )
			printf("%d ", insp->loops[i]->indMap[j]);
		printf("\n");
	}
	
	printf("Inspector printed!\n");
}

inspector_t* initInspector (int baseset, int partSize, int nloops)
{
	inspector_t* insp = (inspector_t*) malloc (sizeof(inspector_t));
	
	insp->col_prev = NULL;
	insp->col_current = NULL;
	
	insp->v2v = NULL;
	
	insp->size = baseset;
	
	//init tiles
	insp->ntiles = (baseset % partSize) ? baseset / partSize + 1 : baseset / partSize;
	insp->partSize = (int*) malloc (insp->ntiles * sizeof(int));
	insp->tiles = (tile_t**) malloc (insp->ntiles * sizeof(tile_t*));
	
	for ( int b = 0; b < insp->ntiles; b++ )
		insp->tiles[b] = createTile ();
	
	//init loops
	insp->loopCounter = 0;
	insp->nloops = nloops;
	insp->loops = (loop_t**) malloc (nloops * sizeof(loop_t*));
	
	return insp;
}

void freeInspector (inspector_t* insp)
{
	for (int b = 0; b < insp->ntiles; b++)
		freeTile (insp->tiles[b]);
	
	for (int i = 0; i < insp->loopCounter; i++)
	{
		free (insp->loops[i]->indMap);
		free (insp->loops[i]->loopname);
		free (insp->loops[i]);
	}

	free (insp->loops);
	free (insp->col_prev);
	free (insp->col_current);
	free (insp->v2v);
	free (insp->partSize);
	free (insp->p2v);
	free (insp);
}

void scanParLoops (inspector_t* insp, int fstSetSize, int* fstMap, int fstMapEl, int fstMapSize, int sndSetSize, int* sndMap, int sndMapSize, int sndMapEl)
{
	//int* baseSetColors = insp->col_current;
	
	//color the first loop
	/*for (int i = 0; i < fstMapSize; i += fstMapEl)
	{
		
	}*/
	
}

int addParLoop (inspector_t* insp, char* loopname, int setSize, int* indirectionMap, int mapSize)
{
	if ( insp->loopCounter >= insp->nloops )
		return INSPOP_MAXLOOP;
	
	//create a base set mapping from the "original OP2 mapping x2y" to a new one which reflects the renumbering of the base set in p2v
	if ( ! insp->v2v )
	{
		int* mappingFunction = (int*) malloc (setSize * sizeof(int));
		baseMapping (insp->p2v, insp->size, mappingFunction);
		insp->v2v = mappingFunction;
	}
	
	//create a new mapping for the parloop, from the original OP2 indirectionMap to the new one which reflects the new position of vertices in p2v 
	int* renumberedMap = (int*) malloc (mapSize * sizeof(int));
	newMapping (indirectionMap, mapSize, insp->v2v, renumberedMap );
	
	// store parloop parameters into insp
	insp->loops[insp->loopCounter] = (loop_t*) malloc (sizeof(loop_t));
	
	insp->loops[insp->loopCounter]->loopname = strdup (loopname);
	insp->loops[insp->loopCounter]->setSize	 = setSize;
	insp->loops[insp->loopCounter]->indMap	 = renumberedMap;
	insp->loops[insp->loopCounter]->mapSize	 = mapSize;
	
	// add the parloop to each tile of the inspector
	for (int i = 0; i < insp->ntiles; i++)
		addLoop (insp->tiles[i], setSize, loopname); // IMPORTANT: an upper bound to the tile iteration set size is specified here (the whole set)
	
	insp->loopCounter++;
	return INSPOP_OK;
}

int partitionAndColor (inspector_t* insp, int vertices, int* e2v, int mapsize)
{	
	// invert the mapping, v2e is needed to compute coloring
	int* v2e 	= (int*) malloc ( mapsize * sizeof(int) );
	int* v2e_offset	= (int*) calloc ( vertices + 1, sizeof(int) ); 
	int* adjncy = (int*) malloc ( mapsize * sizeof(int) );
	
	//invert mapping, i.e. creates v2e mapping, and call metis to compute the partitioning
	invertMapping ( e2v, mapsize, vertices, 2, 1, v2e, adjncy, v2e_offset );
	
	int* v2p;
	metisPartition (vertices, insp->ntiles, (idx_t*) v2e_offset, (idx_t*) adjncy, &v2p); //TODO: 2 is just a fixed value for the example..
	
	// compute the mapping p2v as it is needed to determine a coloring scheme
	int* p2v = (int*) malloc ( vertices * sizeof(int) );
	int* p2v_offset = (int*) calloc ( insp->ntiles + 1, sizeof(int) );

	invertMapping (v2p, vertices, insp->ntiles, 1, 1, p2v, NULL, p2v_offset);
	
	// add the p2v mapping and the partition sizes to the inspector
	insp->p2v = p2v;
	for ( int b = 0; b < insp->ntiles; b++ )
		insp->partSize[b] = p2v_offset[b + 1] - p2v_offset[b];
	
	// init colors
	int* colors = (int*) malloc (insp->ntiles * sizeof (int));
	for ( int b = 0; b < insp->ntiles; b++ ) 
		colors[b] = -1;

	int repeat = 1;
	int ncolor = 0;
	int ncolors = 0;
	int prev_offset, next_offset;
	int totSize = v2e_offset[vertices];
	
	// allocate and zero out 
	int* work = (int*) malloc ( totSize * sizeof(int) );
	
	prev_offset = 0; 
	next_offset = 0;
	
	// coloring algorithm
	while ( repeat )
	{
		repeat = 0;
		
		// zero out color arrays
		for ( int e = 0; e < totSize; e++ )
			work[e] = 0;
		
		// starts trying to color all blocks
		for ( int b = 0; b < insp->ntiles; b++ )
		{
			prev_offset = next_offset;
			
			//adjusts offsets of partitions
			if ( prev_offset + insp->partSize[b] >= vertices ) // last partition can be smaller than partition size
				next_offset = vertices;
			else
				next_offset = prev_offset + insp->partSize[b];
			
			if ( colors[b] == -1 )
			{
				unsigned int mask = 0;
				
				for ( int e = prev_offset; e < next_offset; e++ ) 
				{
					int v = p2v[e];
					
					for ( int j = 0; j < (v2e_offset[v + 1] - v2e_offset[v]); j++ )  
						mask |= work[v2e[v2e_offset[v] + j]-1]; // set bits of mask
				}
				
#if (DEBUG > 0) 
				printf ("WORK LOADED: [ ");
				for ( int i = 0; i < totSize; i++ )
					printf ("%d ", work[i] );
				printf ("]\n");
#endif
				
				int color = ffs( ~mask ) - 1; // find first bit not set
				if ( color == -1 )
				{ //run out of colors on this pass
					repeat = 1;
				}
				else
				{
					colors[b] = ncolor + color;
					mask = 1 << color;
					ncolors = MAX( ncolors, ncolor + color + 1 );
					
					for ( int e = prev_offset; e < next_offset; e++ )
					{
						int v = p2v[e];		
						for ( int j = 0; j < (v2e_offset[v + 1] - v2e_offset[v]); j++ )  
							work[v2e[v2e_offset[v] + j] - 1] |= mask; 
					}
				}
			}
		}
		
		ncolor += 32; // increment base level
	}
	
	// assign color to each vertex of the base set
	insp->col_current = (int*) malloc (insp->size * sizeof(int));
	int* offset = (int*) malloc ((insp->ntiles + 1) * sizeof(int));
	offset[0] = 0;
	
	for (int i = 1; i <= insp->ntiles; i++ ) 
		offset[i] = insp->partSize[i - 1] + offset[i - 1];
	
	for (int b = 0; b < insp->ntiles; b++ )
	{
		for (int j = offset[b]; j < offset[b + 1]; j++ ) 
			insp->col_current[j] = colors[b];
	}
	
	free (offset);
	free (work);
	free (p2v_offset);
	free (v2e_offset);
	free (adjncy);
	free (v2e);
		
	return INSPOP_OK;
}

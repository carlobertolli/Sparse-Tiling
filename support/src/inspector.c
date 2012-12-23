/*
 *  inspector.c
 *  
 */

#include <metis.h>

#include "inspector.h"
#include "invert.h"

#define MAX(A, B)	(A > B) ? A : B

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


inspector_t* initInspector (int baseset, int partSize)
{
	inspector_t* insp = (inspector_t*) malloc (sizeof(inspector_t));
	
	insp->col_prev = NULL;
	insp->col_current = NULL;
	
	insp->size = baseset;
	insp->ntiles = (baseset % partSize) ? baseset / partSize + 1 : baseset / partSize;
	insp->partSize = (int*) malloc (insp->ntiles * sizeof(int));
	
	insp->tiles = (tile_t**) malloc (insp->ntiles * sizeof(tile_t*));
	
	for ( int b = 0; b < insp->ntiles; b++ )
		insp->tiles[b] = createTile ();
	
	return insp;
}

void freeInspector (inspector_t* insp)
{
	for ( int b = 0; b < insp->ntiles; b++ )
		freeTile (insp->tiles[b]);
	
	free (insp->col_prev);
	free (insp->col_current);
	free (insp->partSize);
	free (insp->p2v);
	free (insp);
}

void scanParLoop (inspector_t* insp, int fstSetSize, int* fstMap, int fstMapSize, int sndSetSize, int* sndMap, int sndMapSize)
{
	
}


void partitionAndColor (inspector_t* insp, int vertices, int* e2v, int mapsize)
{
	// allocate output arrays
	//int* partitions = (int*) calloc (vertices, sizeof (int));
	int* colors = (int*) malloc (insp->ntiles * sizeof (int));
	
	// obtain number of partitions needed TODO: fix size
	//int numberOfPartitions = vertices / partitionSize + vertices % partitionSize;
	
	// invert the mapping, v2e is needed to compute coloring
	int* v2e 	= (int*) malloc ( mapsize * sizeof(int) );
	int* v2e_offset	= (int*) calloc ( vertices + 1, sizeof(int) ); 
	int* adjncy = (int*) malloc ( mapsize * sizeof(int) );
	
	//invert mapping, i.e. creates v2e mapping, and call metis to compute the partitioning
	invertMapping ( e2v, mapsize, vertices, 2, 1, v2e, adjncy, v2e_offset );
	
	int* v2p;
	metisPartition (vertices, 2, (idx_t*) v2e_offset, (idx_t*) adjncy, &v2p); //TODO: 2 is just a fixed value for the example..
	
	// compute the mapping p2v as it is needed to determine a coloring scheme
	int* p2v;
	int* p2v_offset;
	invertMapping (v2p, vertices, vertices, 1, 1, p2v, NULL, p2v_offset);
	
	// add the p2v mapping and the partition sizes to the inspector
	insp->p2v = p2v;
	for ( int b = 0; b < insp->ntiles; b++ )
		insp->partSize[b] = p2v_offset[b + 1] - p2v_offset[b];
	
	// init colors
	for ( int b = 0; b < insp->ntiles; b++ ) 
		colors[b] = -1;
	
	int repeat = 1;
	int ncolor = 0;
	int ncolors = 0;
	int prev_offset, next_offset;
	int to_size = v2e_offset[vertices];
	
	// allocate and zero out 
	int* work = (int*) malloc ( to_size * sizeof(int) );
	
	prev_offset = 0; 
	next_offset = 0;
	
	// coloring algorithm
	while ( repeat )
	{
		repeat = 0;
		
		// zero out color arrays
		for ( int e = 0; e < to_size; e++ )
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
				for ( int i = 0; i < to_size; i++ )
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
	
	free (work);
	//TODO: Free a lot of stuff..
	
	//*_partitions = partitions;
	insp->col_current = colors;
}

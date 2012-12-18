
#include <stdlib.h>
#include <strings.h>

#include <metis.h>


#define DEBUG		1

#define MAX(A, B)	(A > B) ? A : B

//#if (DEBUG > 0)
#include <stdio.h>
//#endif


/*
 * input:
 * x2y			: mapping from x to y
 * size_x2y		: size of the mapping x2y
 * ny			: size of the destination set y
 * onex_ny		: number of y elements for each x elements (eg x2y == e2v -> onex_ny = 2)
 * x_zero		: == true -> x numbering starts from 0 
 *
 * output:
 * y2x			: mapping from y to x
 * y2adj		: mapping from y to adjacent y elements (can be passed NULL)
 * offset		: offset of the x elements for each y element in y2x (size == ny + 1)
 */
void invertMapping ( int* x2y, int size_x2y, int ny, int onex_ny, int x_zero, int* y2x, int* y2adj, int* offset  )
{
	offset[0] = 0;
	x_zero = ( x_zero ) ? 1 : 0; //takes into account x numbering starting from either 0 or 1
	
	//compute the offsets in y2x
	for ( int i = 0; i < size_x2y; i++ )
		offset[x2y[i] + x_zero] ++;
	
	for ( int i = 1; i < ny + 1; i++ )
		offset[i + x_zero] += offset[i - 1  + x_zero];
	
	/*
	 #if (DEBUG > 1) 
	 for ( int i = 0; i < ny + 1; i++ )
	 printf ("Y element %d : %d\n", i + 1 - x_zero, offset[i] );
	 #endif
	 */
	
	int* inserted = (int*) calloc ( ny, sizeof(int) ); //relative offset when inserting into y2x and adjncy
	
	if ( y2adj ) // compute both y2x and adjncy
	{
		for ( int i = 0; i < size_x2y; i += onex_ny )
		{
			for ( int j = 0; j < onex_ny; j++ )
			{
				// compute y2x
				y2x[offset [x2y[i + j] - 1 + x_zero] + inserted[x2y[i + j] - 1 + x_zero]] = i / onex_ny;
				// compute y2adj
				y2adj[offset [x2y[i + j] - 1 + x_zero] + inserted[x2y[i + j] - 1 + x_zero]] = x2y[i + onex_ny - 1 - j];
				inserted[x2y[i + j] - 1 + x_zero] ++;
			}
		}
	}
	else 
	{
		for ( int i = 0; i < size_x2y; i += onex_ny )
		{
			for ( int j = 0; j < onex_ny; j++ )
			{
				// compute y2x
				y2x[offset [x2y[i + j] - 1 + x_zero] + inserted[x2y[i + j] - 1 + x_zero]] = i / onex_ny;
				inserted[x2y[i + j] - 1 + x_zero] ++;
			}
		}
	}
	
	free (inserted);
}

/*
 * Invert the mapping, from e2v to v2e
 *
 * input: 
 * e2v			: mapping from edges to vertices
 * nvertices	: number of vertices
 * map_entries	: number of entries in e2v
 * 
 * output:
 * v2e			: mapping from vertices to edges
 * offset		: edges offset in v2e    
 * adjncy		: mapping from vertices to vertices
 * vertex_zero	: == 0 means vertex numbering starts from 1
 */
void invertMappingEV ( int* e2v, int nvertices, int map_entries, int* v2e, int* offset, int* adjncy, int vertex_zero  )
{
	offset[0] = 0;
	vertex_zero = ( vertex_zero ) ? 1 : 0; //takes into account vertices numbering starting either from 0 or 1
	 
	//offset[i] counts the number of incident edges for each vertex i-1. 
	for ( int i = 0; i < map_entries; i++ )
		offset[e2v[i] + vertex_zero] ++;
	
	for ( int i = 1; i < nvertices + 1; i++ )
		offset[i + vertex_zero] += offset[i - 1  + vertex_zero];
	
#if (DEBUG > 1) 
	for ( int i = 0; i < nvertices + 1; i++ )
		printf ("VERTEX %d : %d\n", i+1-vertex_zero, offset[i] );
#endif	
	
	int* inserted = (int*) calloc ( nvertices, sizeof(int) ); //relative offset when inserting into v2e and adjncy
	for ( int i = 0; i < map_entries; i += 2 )
	{
		int v1 = e2v[i];
		int v2 = e2v[i + 1];
		int edge = i/2 + 1;
		
		//computes v2e
		v2e [offset[v1 - 1 + vertex_zero] + inserted[v1 - 1 + vertex_zero]] = edge;
		v2e [offset[v2 - 1 + vertex_zero] + inserted[v2 - 1 + vertex_zero]] = edge;	
		
		//computes adjncy
		adjncy [offset[v1 - 1 + vertex_zero] + inserted[v1 - 1 + vertex_zero]] = v2;
		adjncy [offset[v2 - 1 + vertex_zero] + inserted[v2 - 1 + vertex_zero]] = v1;
		
		inserted[v1 - 1 + vertex_zero]++;
		inserted[v2 - 1 + vertex_zero]++;
	}
	
	free (inserted);
}


int metisPartition ( int nvertices, int _nparts, int* xadj, int* adjncy, int** part  )
{
	//input
	idx_t nvtxs	 = (idx_t) nvertices; // #vertices in the graph
	idx_t ncon	 = 1;		  // #balancing constraint
	idx_t nparts = (idx_t) _nparts;	  // #graph partitions

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

/*
 * input:
 * nvertices		: #vertices
 * v2e				: map v2e
 * partitionSize	: size requested for vertex partitions 
 * 
 * output:
 * partitions		: vertex -> partitionID
 * colors			: partitionID -> color 
 */
void partitionAndColor ( int nvertices, int partitionSize, ve_map* v2e, int** _partitions, int** _colors )
{
    // obtain number of partitions needed TODO: fix size
    int numberOfPartitions = nvertices / partitionSize + nvertices % partitionSize;
	
    // allocate output arrays
    int* partitions = (int*) calloc (nvertices, sizeof (int));
    int* colors = (int*) malloc (numberOfPartitions * sizeof (int));
	
    // assign vertices to partitions using block partitioning
    //int partitionID = 0;
    int addedElements = 0;
    for ( int i = 0; i < nvertices; i++ )
    {
		/*
		 addedElements++;
		 if ( addedElements == partitionSize )
		 partitionID++;
		 partitions[i] = partitionID;
		 */
		partitions[i] = addedElements++ / partitionSize;   
    }
	
    // init colors
    for ( int b = 0; b < numberOfPartitions; b++ ) 
		colors[b] = -1;
	
    int repeat = 1;
    int ncolor = 0;
    int ncolors = 0;
    int prev_offset, next_offset;
    int to_size = v2e->offset[nvertices];
	
    // allocate and zero out 
    int* work = (int*) malloc ( to_size * sizeof(int) );
	
    prev_offset = 0; 
    next_offset = 0;
	
    while ( repeat )
    {
		repeat = 0;
		
		// zero out color arrays
		for ( int e = 0; e < to_size; e++ )
			work[e] = 0;
		
		// starts trying to color all blocks
		for ( int b = 0; b < numberOfPartitions; b++ )
		{
			prev_offset = next_offset;
			
			//adjusts offsets of partitions
			if ( prev_offset + partitionSize >= nvertices ) // last partition can be smaller than partition size
				next_offset = nvertices;
			else
				next_offset = prev_offset + partitionSize;
			
			if ( colors[b] == -1 )
			{
				unsigned int mask = 0;
				
				for ( int e = prev_offset; e < next_offset; e++ )
					for ( int j = 0; j < (v2e->offset[e + 1] - v2e->offset[e]); j++ )  
						mask |= work[v2e->map[v2e->offset[e] + j]-1]; // set bits of mask
				
#if (DEBUG > 1) 
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
						for ( int j = 0; j < (v2e->offset[e + 1] - v2e->offset[e]); j++ )  
							work[v2e->map[v2e->offset[e] + j] - 1] |= mask; 
					
				}
			}
		}
		
		ncolor += 32; // increment base level
    }
    
    *_partitions = partitions;
    *_colors = colors;
}



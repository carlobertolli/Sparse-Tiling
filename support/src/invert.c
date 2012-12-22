#include "../include/invert.h"

#define MAX(A, B)	(A > B) ? A : B


void invertMapping ( int* x2y, int size_x2y, int ny, int onex_ny, int x_zero, int* y2x, int* y2adj, int* offset  )
{
	offset[0] = 0;
	x_zero = ( x_zero ) ? 1 : 0; //takes into account x numbering starting from either 0 or 1
	
	//compute the offsets in y2x
	for ( int i = 0; i < size_x2y; i++ )
		offset[x2y[i] + x_zero] ++;
	
	for ( int i = 1; i < ny + 1; i++ )
		offset[i + x_zero] += offset[i - 1  + x_zero];
	
	
#if (DEBUG > 0) 
	 for ( int i = 0; i < ny + 1; i++ )
		 printf ("Y element %d : %d\n", i + 1 - x_zero, offset[i] );
#endif
	 
	
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



int metisPartition ( int nvertices, int _nparts, idx_t* xadj, idx_t* adjncy, int** part  )
{
	//input
	idx_t nvtxs	 = (idx_t) nvertices;	// #vertices in the graph
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



void partitionAndColor ( int nvertices, int partitionSize, int* v2e, int* offset, int** _partitions, int** _colors )
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
    int to_size = offset[nvertices];
	
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
					for ( int j = 0; j < (offset[e + 1] - offset[e]); j++ )  
						mask |= work[v2e[offset[e] + j]-1]; // set bits of mask
				
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
						for ( int j = 0; j < (offset[e + 1] - offset[e]); j++ )  
							work[v2e[offset[e] + j] - 1] |= mask; 
					
				}
			}
		}
		
		ncolor += 32; // increment base level
    }
    
    *_partitions = partitions;
    *_colors = colors;
}



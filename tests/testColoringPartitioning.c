#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "invert.h"

int main ()
{
	// set for this example
	int nvertices	= 15;
	int n_edges		= 22;
	int map_entries	= n_edges * 2;
	int part_size	= 2;
	
	//input
	//int e2v[] = {1,2 , 1,3 , 1,4 , 2,3 , 2,7 , 4,3 , 4,8 , 4,5 , 3,5 , 3,6 , 3,7 , 7,10 , 7,6 , 6,10 , 10,9 , 6,9 , 5,6 , 5,9 , 8,5 };
	int e2v[] = { 0,1 , 1,2 , 2,3 , 3,4 , 0,5 , 5,10, 1,6 , 6,11 , 2,7 , 7,12 , 3,8 , 8,13 , 4,9 , 9,14 , 5,6 , 6,7 , 7,8 , 8,9 , 10,11 , 11,12 , 12,13, 13,14 };
	
	
	//output
	int* v2e 	= (int*) malloc ( map_entries * sizeof(int) );
	int* offset	= (int*) calloc ( nvertices + 1, sizeof(int) ); 
	
	int* adjncy = (int*) malloc ( map_entries * sizeof(int) );
	
	//invert mapping, i.e. creates v2e mapping
	invertMapping ( e2v, map_entries, nvertices, 2, 1, v2e, adjncy, offset );
	
	printf ( "v2e : [" );
	for ( int i = 0; i < map_entries; i++ )
	{
		printf ( " %d ", v2e[ i ] );
	}
	printf (" ]\n");
	
	//call METIS to partition the graph
	int* metPartitions;
	int result = metisPartition ( nvertices, 2, (idx_t*) offset, (idx_t*) adjncy, &metPartitions );
	
	if ( result != METIS_OK ) 
	{
		printf("Something went wrong when partitioning\n");
		return -1;
	}

	for ( int i = 0; i < nvertices; i++ )
		printf ("vertex %d : partition %d\n", i, metPartitions[i]);
	printf ("\n");

	//compute coloring
	int n_partitions = nvertices / part_size + nvertices % part_size;
	
	int* partitions; 
	int* colors;	 
	
	partitionAndColor ( nvertices, 2, v2e, offset, &partitions, &colors );

	printf ("COLORS: \n");
	for ( int i = 0; i < n_partitions; i++ )
	    printf ("Partition: %d, Color: %d\n", partitions[i * 2], colors[i]);
	printf ("\n");

	return 0;
}

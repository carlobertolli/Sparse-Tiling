#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "test.h"

int main ()
{
	// set for this example
	int vertices	= 15;
	int edges		= 22;
	int mapEntries	= edges * 2;
	int partSize	= 2;
	
	//input
	int e2v[] = { 0,1 , 1,2 , 2,3 , 3,4 , 0,5 , 5,10, 1,6 , 6,11 , 2,7 , 7,12 , 3,8 , 8,13 , 4,9 , 9,14 , 5,6 , 6,7 , 7,8 , 8,9 , 10,11 , 11,12 , 12,13, 13,14 };
	
	//output
	int* v2e 	= (int*) malloc ( mapEntries * sizeof(int) );
	int* offset	= (int*) calloc ( vertices + 1, sizeof(int) ); 
	
	int* adjncy = (int*) malloc ( mapEntries * sizeof(int) );
	
	//invert mapping, i.e. creates v2e mapping
	invertMapping ( e2v, mapEntries, vertices, 2, 1, v2e, adjncy, offset );
	
	printf ( "v2e : [" );
	for ( int i = 0; i < mapEntries; i++ )
	{
		printf ( " %d ", v2e[ i ] );
	}
	printf (" ]\n");
	
	//call METIS to partition the graph
	int* metPartitions;
	int result = metisPartition ( vertices, partSize, (idx_t*) offset, (idx_t*) adjncy, &metPartitions );
	
	if ( result != METIS_OK ) 
	{
		printf("Something went wrong during partitioning\n");
		return -1;
	}
	
	for ( int i = 0; i < vertices; i++ )
		printf ("vertex %d : partition %d\n", i, metPartitions[i]);
	printf ("\n");
		
	return 0;
}

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
	int partSize	= 5;
	
	//input
	int e2v[] = { 0,1 , 1,2 , 2,3 , 3,4 , 0,5 , 5,10, 1,6 , 6,11 , 2,7 , 7,12 , 3,8 , 8,13 , 4,9 , 9,14 , 5,6 , 6,7 , 7,8 , 8,9 , 10,11 , 11,12 , 12,13, 13,14 };
	
	//compute coloring
	inspector_t* insp = initInspector (vertices, partSize, 1);
		
	partitionAndColor (insp, vertices, e2v, mapEntries);

	printInspector (insp);
	/*
	printf ("TILES %d, VERTEX %d, COLORS: \n", insp->ntiles, vertices);
	for ( int i = 0; i < insp->ntiles; i++ )
	{
		partSize = insp->partSize[i];
		printf("Tile %d, size: %d\n", i, partSize);
		for ( int j = 0; j < partSize; j++ )
			printf ("\tVertex: %d, Color: %d\n", insp->p2v[i*partSize + j], insp->col_current[i*partSize + j]);
	}
	printf ("\n");
	*/
	freeInspector (insp);
	
	return 0;
}

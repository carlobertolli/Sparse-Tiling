/*
 *  testTile.c
 *  
 *
 *  Created by Fabio Luporini on 12/24/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "test.h"

int main ()
{
	// set for this example
	int vertices	= 15;
	int edges		= 22;
    int cells       = 8;
    
	int edgeMapEntries	= edges * 2;
	int cellsMapEntries = cells * 4;
    
    int partSize	= 5;
	
    int nloops      = 3;
    int seqSize     = 3;
    
	//input
	int e2v[] = { 0,1 , 1,2 , 2,3 , 3,4 , 0,5 , 5,10, 1,6 , 6,11 , 2,7 , 7,12 , 3,8 , 8,13 , 4,9 , 9,14 , 5,6 , 6,7 , 7,8 , 8,9 , 10,11 , 11,12 , 12,13, 13,14 };
	int c2v[] = { 0,1,5,6 , 1,2,6,7 , 2,3,7,8 , 3,4,8,9 , 5,6,10,11 , 6,7,11,12 , 7,8,12,13 , 8,9,13,14 };
    
	//compute coloring
	inspector_t* insp = initInspector (vertices, partSize, nloops);

	partitionAndColor ( insp, vertices, e2v, edgeMapEntries );
	
	printf("Partitioned and colored...!\n");
	printInspector ( insp );
	
	printf("Adding a first loop over edges..");
	addParLoop ( insp, "edges", edges, e2v, edgeMapEntries );
	printf("Done!\n");
	printInspector ( insp );
	
    printf("Adding a second loop over cells..");
	addParLoop ( insp, "cells", cells, c2v, cellsMapEntries );
	printf("Done!\n");
	printInspector ( insp );
	
    printf("Adding a third loop over edges, like the first one..");
	addParLoop ( insp, "edges", edges, e2v, edgeMapEntries );
	printf("Done!\n");
	printInspector ( insp );
	
    
	//scanning the ficticious loop on edges, using e2v to reference the vertex (base) base
	printf("Running the inspector..");
	int* sequence = (int*) malloc ( nloops * sizeof(int));
	sequence[0] = 0;
    sequence[1] = 1;
    sequence[2] = 0;
	int res = runInspector (insp, 0, sequence, seqSize);
	printf("Done!\n");
	
	if ( res != INSPOP_OK )
		printf("...Problems occured when the inspector run\n");
	else 
	{
		printInspector (insp);	
		printf("\n");
		
		for (int i = 0; i < insp->ntiles; i++)
			printTile (insp->tiles[i]);
			
	}

	freeInspector (insp);
	
	free (sequence);
	
	return 0;
}

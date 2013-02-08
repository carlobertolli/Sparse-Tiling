/*
 *  testTile.c
 *  
 *
 *  Created by Fabio Luporini on 12/24/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 *
 *  GRAPH:
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "test.h"

int main ()
{
  // sets for this example
  int vertices = 13;
  int edges = 26;
  int cells = 14;
  
  int edgeMapEntries	= edges * 2;
  int cellsMapEntries = cells * 3;
  
  int partSize	= 5;
  
  int nloops = 3;
  
  //input
  int e2v[] = { 0,1 , 1,2 , 0,3 , 1,3 , 2,3 , 3,5 , 2,5 , 3,4 , 4,5 , 3,6 , 4,6 , 3,7 , 0,7 , 6,7 , 6,8 , 4,8 , 4,9 , 8,9 , 9,10 , 4,10 , 10,11 , 5,11 , 11,12 , 2,12 , 5,12 , 5,10 };
  int c2v[] = { 0,1,3 , 1,2,3 , 2,3,5 , 2,5,12 , 5,11,12 , 0,3,7 , 3,6,7 , 3,4,6 , 3,4,5 , 4,5,10 , 5,10,11 , 4,6,8 , 4,8,9 , 4,9,10 };
  
  printf("***** TESTING INSPECTOR *****\n");
  
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
  
  int res = runInspector (insp, 0);
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
  
  printf("***** TESTING EXECUTOR *****\n");
  
  executor_t* exec = initExecutor (insp);
		
  printExecutor (exec);
  
  execute (exec, ALLCOLORS);
  
  freeInspector (insp);
  freeExecutor (exec);
  
  return 0;
}

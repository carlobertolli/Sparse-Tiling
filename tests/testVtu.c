/*
 *  testVtu.c
 *
 *
 *  Created by Fabio Luporini on 02/13/13.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 *
 *  GRAPH:
 *
 *  0 --  1 --  2 --  3 --  4
 *  |     |     |     |     |
 *  5 --  6 --  7 --  8 --  9
 *  |     |     |     |     |
 * 10 -- 11 -- 12 -- 13 -- 14
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
  int edges			= 22;
  int cells     = 8;
  
  int edgeMapEntries	= edges * 2;
  int cellsMapEntries = cells * 4;
  
  int partSize	= 5;
  
  int nloops    = 3;
  
  //input
  int e2v[] = { 0,1 , 1,2 , 2,3 , 3,4 , 0,5 , 5,10, 1,6 , 6,11 , 2,7 , 7,12 , 3,8 , 8,13 , 4,9 , 9,14 , 5,6 , 6,7 , 7,8 , 8,9 , 10,11 , 11,12 , 12,13, 13,14 };
  int c2v[] = { 0,1,5,6 , 1,2,6,7 , 2,3,7,8 , 3,4,8,9 , 5,6,10,11 , 6,7,11,12 , 7,8,12,13 , 8,9,13,14 };
  
  printf("***** TESTING INSPECTOR *****\n");
  
  //compute coloring
  inspector_t* insp = initInspector (vertices, partSize, nloops);
  partitionAndColor ( insp, vertices, e2v, edgeMapEntries );
  
  printf("Partitioned and colored...!\n");
  
  vtu_mesh_t* mesh = createVtuMesh (vertices, edges, cells);
  printVtuFile (insp, mesh);
  freeVtuMesh (mesh);
  
  printf("Printed out vtu file...!\n");
  
  
}
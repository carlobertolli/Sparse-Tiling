/*
 *  executor.c
 *  
 *
 *  Created by Fabio Luporini on 1/2/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "invert.h"
#include "executor.h"

executor_t* initExecutor (inspector_t* insp)
{
  executor_t* exec = (executor_t*) malloc (sizeof(executor_t));
  exec->c2p = (int*) malloc (insp->ntiles * sizeof(int));
  exec->offset = (int*) calloc (insp->ncolors + 1, sizeof(int));
  
  exec->ncolors = insp->ncolors;
  exec->ntiles = insp->ntiles;
  exec->tiles = insp->tiles;
  
  exec->computedColors = 0;
  
  //get the inverse mapping from colors to partitions
  invertMapping (insp->p2c, insp->ntiles, insp->ncolors, 1, 1, exec->c2p, NULL, exec->offset, NULL);
  
  return exec;
}

int execute (executor_t* exec, int ncolors)
{
  if (exec->computedColors + 1 > exec->ncolors)
    return EXEC_NOMORECOLORS;
  
  if (ncolors != SINGLECOLOR && ncolors != ALLCOLORS)
    return EXEC_ERR;
  
  int colorsToCompute = (ncolors == ALLCOLORS) ? exec->ncolors : 1;
  
  //for each colour, it executes all tiles 
  for (int i = 0; i < colorsToCompute; i++)
  {
    int curColor = exec->computedColors;
    for (int j = exec->offset[curColor]; j < exec->offset[curColor + 1]; j++ )
      runTile (exec->tiles[exec->c2p[ j ]]);
    
    exec->computedColors++;
  }
		
  return EXEC_OK;
}

void freeExecutor (executor_t* exec)
{
  free (exec->c2p);
  free (exec->offset);
  free (exec);
}

void printExecutor (executor_t* exec)
{
  if ( !exec )
  {
    printf("exec is NULL\n");
    return;
  }
  
  printf("- EXECUTOR -\n\n");
  printf("Number of tiles: \t%d\n", exec->ntiles);
  printf("Number of colors: \t%d\n", exec->ncolors);
  
  printf("\nColors to tiles mapping:\n");
  for (int i = 0; i < exec->ncolors; i++) {
    printf("\tColor %d: ", i);
    for (int j = 0; j < (exec->offset[i + 1] - exec->offset[i]); j++)
      printf("%d ", exec->c2p[exec->offset[i] + j]);
    printf("\n");
  }
}

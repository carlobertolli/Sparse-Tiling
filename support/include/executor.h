/*
 *  executor.h
 *  
 *
 *  Created by Fabio Luporini on 1/2/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "inspector.h"

#define SINGLECOLOR	1
#define ALLCOLORS 2

#define EXEC_OK 1
#define EXEC_NOMORECOLORS 0
#define EXEC_ERR -1

typedef struct {
  tile_t** tiles;
  
  int ncolors;
  int ntiles;
  int* c2p;
  int* offset;
  
  int computedColors; //keep track of the number of colors computed [0, ncolors]
  
} executor_t;

/*
 * Initialize an executor on the basis of the information provided by insp.
 */
executor_t* initExecutor (inspector_t* insp);

/*
 * Execute ALL tiles belonging to a color. This is done for either a single color or for all computed colors  (can be SINGLECOLOR or ALLCOLORS) 
 */
int execute (executor_t* exec, int ncolors);

void freeExecutor ();
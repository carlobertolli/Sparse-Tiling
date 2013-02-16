/*
 *  test.h
 *
 * This header file is just for testing purposes
 */

#include <metis.h>

#include "executor.h"
#include "inspector.h"
#include "invert.h"
#include "plotmesh.h"

int metisPartition ( int nvertices, int _nparts, idx_t* xadj, idx_t* adjncy, int** part  );
void printInspector (inspector_t* insp);
void printTile (tile_t* tile);
void printExecutor (executor_t* exec);

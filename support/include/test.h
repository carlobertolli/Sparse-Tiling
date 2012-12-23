/*
 *  test.h
 *
 * This header file is just for testing purposes
 */

#include <metis.h>

#include "inspector.h"
#include "invert.h"

int metisPartition ( int nvertices, int _nparts, idx_t* xadj, idx_t* adjncy, int** part  );


#ifndef _INVERT_H_
#define _INVERT_H_

#define DEBUG	0

#include <stdlib.h>
#include <strings.h>

#include <metis.h>

#if (DEBUG > 0)
#include <stdio.h>
#endif

/*
 * input:
 * x2y			: mapping from x to y
 * size_x2y		: size of the mapping x2y
 * ny			: size of the destination set y
 * onex_ny		: number of y elements for each x elements (eg x2y == e2v -> onex_ny = 2)
 * x_zero		: == true -> x numbering starts from 0 
 *
 * output:
 * y2x			: mapping from y to x
 * y2adj		: mapping from y to adjacent y elements (can be passed NULL)
 * offset		: offset of the x elements for each y element in y2x (size == ny + 1)
 */
void invertMapping (int* x2y, int size_x2y, int ny, int onex_ny, int x_zero, int* y2x, int* y2adj, int* offset);


/* input:
 * nvertices	: #vertices
 * nparts		: number of partitions requested to metis
 * xadj			: vertices offsets in adjncy - standard CSR format
 * adjncy		: v2v mapping - standard CSR format
 * 
 * output:
 * *part		: part[i] == partition of vertex 1, computed by metis
 */
int metisPartition (int nvertices, int nparts, idx_t* xadj, idx_t* adjncy, int** part);

/*
 * input:
 * nvertices		: #vertices
 * v2e				: map v2e
 * partitionSize	: size requested for vertex partitions 
 * 
 * output:
 * partitions		: vertex -> partitionID
 * colors			: partitionID -> color 
 */
void partitionAndColor (int nvertices, int partitionSize, int* v2e, int* offset, int** _partitions, int** _colors);

#endif
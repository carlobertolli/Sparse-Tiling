#ifndef _INVERT_H_
#define _INVERT_H_

#define DEBUG	0

#include <stdlib.h>
#include <strings.h>

#if (DEBUG > 0)
#include <stdio.h>
#endif

/*
 * Given a mapping from a set X to a set Y, invert it by creating the inverse mapping from Y to X 
 *
 * input:
 * x2y			: mapping from x to y
 * size_x2y	: size of the mapping x2y
 * ny				: size of the destination set y
 * onex_ny	: number of y elements for each x elements (eg x2y == e2v -> onex_ny = 2)
 * x_zero		: == true -> x numbering starts from 0 
 *
 * output:
 * y2x			: mapping from y to x
 * y2adj		: mapping from y to adjacent y elements (can be passed NULL)
 * offset		: offset of the x elements for each y element in y2x (size == ny + 1)
 */
void invertMapping (int* x2y, int size_x2y, int ny, int onex_ny, int x_zero, int* y2x, int* y2adj, int* offset);

/* Given a set X, #X = N, creates the mapping f(X) = {i -> j : X[j] = i, i \in [1, N]}
 * 
 * input:
 * x		: input set
 * size	: size of x
 *
 * output:
 * fx		:	output set
 */
void createMapping (int* x, int size, int* fx);



#endif
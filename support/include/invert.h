#ifndef _INVERT_H_
#define _INVERT_H_

#include <stdlib.h>
#include <strings.h>

#if (DEBUG > 0)
#include <stdio.h>
#endif

#define MAX(A, B)	(A > B) ? A : B
#define MIN(A, B)	(A < B) ? A : B

/*
 * Given a mapping from a set X to a set Y, invert it by creating the inverse mapping from Y to X 
 *
 * input:
 * x2y : mapping from x to y
 * size_x2y : size of the mapping x2y
 * ny : size of the destination set y
 * onex_ny : number of y elements for each x elements (eg x2y == e2v -> onex_ny = 2)
 * x_zero : == true -> x numbering starts from 0 
 *
 * output:
 * y2x : mapping from y to x
 * y2adj : mapping from y to adjacent y elements (can be passed NULL)
 * offset : offset of the x elements for each y element in y2x (size == ny + 1)
 * maxIncidence : maximum incidence over an y element
 */
void invertMapping (int* x2y, int size_x2y, int ny, int onex_ny, int x_zero, int* y2x, int* y2adj, int* offset, int* maxIncidence);

/* Given a set X, #X = N, creates the "base mapping" f(X) = {i -> j : X[j] = i, i \in [1, N]} (that is, a mapping from x to itself)
 * 
 * input:
 * x		  : input set
 * size	: size of x
 *
 * output:
 * fx	 	:	output set
 */
void baseMapping (int* x, int size, int* fx);

/* Given a mapping x2y, 
 *
 * input:
 * x2y	 : mapping from x to y
 * size	: size of the mapping x2y
 * fy		 : new mapping for the y set
 *
 * output:
 * x2fy : new mapping x2y 
 */
void newCodomain (int* x2y, int size, int* fy, int* x2fy);

/* Given a mapping x2y, 
 *
 * input:
 * x2y	 : mapping from x to y
 * size	: size of the mapping x2y
 * fx		 : new mapping for the x set
 *
 * output:
 * fx2y : new mapping x2y 
 */
void newDomain (int* x2y, int size, int* fx, int* fx2y);



#endif

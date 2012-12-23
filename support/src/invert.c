#include "invert.h"

void invertMapping ( int* x2y, int size_x2y, int ny, int onex_ny, int x_zero, int* y2x, int* y2adj, int* offset  )
{
	offset[0] = 0;
	x_zero = ( x_zero ) ? 1 : 0; //takes into account x numbering starting from either 0 or 1
	
	//compute the offsets in y2x
	for ( int i = 0; i < size_x2y; i++ )
		offset[x2y[i] + x_zero] ++;
	
	for ( int i = 1; i < ny + 1; i++ )
		offset[i + x_zero] += offset[i - 1  + x_zero];
	
	
#if (DEBUG > 0) 
	 for ( int i = 0; i < ny + 1; i++ )
		 printf ("Y element %d : %d\n", i + 1 - x_zero, offset[i] );
#endif
	 
	
	int* inserted = (int*) calloc ( ny, sizeof(int) ); //relative offset when inserting into y2x and adjncy
	
	if ( y2adj ) // compute both y2x and adjncy
	{
		for ( int i = 0; i < size_x2y; i += onex_ny )
		{
			for ( int j = 0; j < onex_ny; j++ )
			{
				// compute y2x
				y2x[offset [x2y[i + j] - 1 + x_zero] + inserted[x2y[i + j] - 1 + x_zero]] = i / onex_ny;
				// compute y2adj
				y2adj[offset [x2y[i + j] - 1 + x_zero] + inserted[x2y[i + j] - 1 + x_zero]] = x2y[i + onex_ny - 1 - j];
				inserted[x2y[i + j] - 1 + x_zero] ++;
			}
		}
	}
	else 
	{
		for ( int i = 0; i < size_x2y; i += onex_ny )
		{
			for ( int j = 0; j < onex_ny; j++ )
			{
				// compute y2x
				y2x[offset [x2y[i + j] - 1 + x_zero] + inserted[x2y[i + j] - 1 + x_zero]] = i / onex_ny;
				inserted[x2y[i + j] - 1 + x_zero] ++;
			}
		}
	}
	
	free (inserted);
}

void createMapping (int* x, int size, int* fx)
{
	
	for ( int i = 0; i < size; i++ )
		fx[x[i]] = i;
}




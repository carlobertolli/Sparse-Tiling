#include "invert.h"

void invertMapping (int* x2y, int size_x2y, int ny, int onex_ny, int x_zero, int* y2x, int* y2adj, int* offset, int* maxIncidence)
{
  offset[0] = 0;
  int incidence = 0;
   
  //takes into account x numbering starting from either 0 or 1
  x_zero = ( x_zero ) ? 1 : 0; 
  
  //compute the offsets in y2x
  for ( int i = 0; i < size_x2y; i++ )
    offset[x2y[i] + x_zero] ++;
  
  for ( int i = 1; i < ny; i++ ) 
  {
    offset[i + x_zero] += offset[i - 1  + x_zero];
    incidence = MAX (incidence, offset[i + x_zero] - offset[i - 1  + x_zero]);
  }
  
#if (DEBUG > 0) 
	 for ( int i = 0; i < ny + 1; i++ )
    printf ("Y element %d : %d\n", i + 1 - x_zero, offset[i] );
#endif
	 
  
  int* inserted = (int*) calloc ( ny + 1, sizeof(int) ); //relative offset when inserting into y2x and adjncy
  
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
  
  //return
  if (maxIncidence)
    *maxIncidence = incidence;
}


void baseMapping (int* x, int size, int* fx)
{
  
  for ( int i = 0; i < size; i++ )
    fx[x[i]] = i;
}


void newCodomain (int* x2y, int size, int* fy, int* x2fy)
{
  for (int i = 0; i < size; i++)
    x2fy[i] = fy[x2y[i]];
}

void newDomain (int* x2y, int size, int* fx, int* fx2y)
{
  for (int i = 0; i < size; i++)
    fx2y[fx[i]] = x2y[i];
}


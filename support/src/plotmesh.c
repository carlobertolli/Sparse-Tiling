/* Example VTK format 
 # vtk DataFile Version 1.0
 Coloring edges
 ASCII
 
 DATASET POLYDATA
 POINTS 3 int
 0.0 0.0 0.0
 1.0 0.0 0.0
 0.0 1.0 0.0
 
 LINES 3 9
 2 0 1
 2 0 2
 2 2 1
 
 CELL_DATA 3
 SCALARS pressure int
 LOOKUP_TABLE default
 0.0
 0.0
 1.0
*/

#include <stdio.h>
#include <stdlib.h>

#include "plotmesh.h"

vtu_mesh_t* createVtuMesh (int vertices, int edges, int cells, int* coordinates, int* e2v, int* c2v, dim_t dimension)
{
  vtu_mesh_t* mesh = (vtu_mesh_t*) malloc (sizeof(vtu_mesh_t));
  
  mesh->nvertices = vertices;
  mesh->nedges = edges;
  mesh->ncells = cells;
  mesh->coordinates = coordinates;
  mesh->dimension = (dimension == D2) ? 2 : 3;
  mesh->e2v = e2v;
  mesh->c2v = c2v;
  
  return mesh;
}


int printVtuFile (inspector_t* insp, vtu_mesh_t* mesh)
{
  // aliases
  int* coord = mesh->coordinates;
  int vertices = mesh->nvertices;
  int edges = mesh->nedges;
  int cells = mesh->ncells;
  int dim = mesh->dimension;
  
  for (int i = 0; i < insp->nloops; i++)
  {
    // aliases
    loop_t* loop = insp->loops[i];
    
    char filename[LOOPNAMELENGTH + VTK_DIR_MAX + 1];
    sprintf (filename, "%s/loop%d-%s.vtk", VTK_DIR, i, loop->loopname);
    
    FILE* f = fopen (filename, "w");
  
    // print header
    fprintf (f, "# vtk DataFile Version 1.0\n");
    fprintf (f, "Coloring %s\n", loop->loopname);
    fprintf (f, "ASCII\n\n");
    
    // print vertices
    fprintf (f, "DATASET POLYDATA\n");
    fprintf (f, "POINTS %d int\n", vertices);
    for (int j = 0; j < vertices; j++)
    {
      fprintf (f, "%d %d", coord[j*dim], coord[j*dim + 1]);
      if (dim == 2)
        fprintf (f, " 0\n");
      else
        fprintf (f, " %d\n", coord[j*dim + 2]);
    }
    
    // print edges or cells
    int step;
    if ( (step = loop->mapSize / loop->setSize) == 2)
    {
      //print edges
      fprintf (f, "\nLINES %d %d\n", edges, edges * (step + 1));
      for (int j = 0; j < edges; j++)
      {
        fprintf (f, "2 %d %d\n", mesh->e2v[j*step], mesh->e2v[j*step + 1]);
      }
    }
    if ( (step = loop->mapSize / loop->setSize) == 3)
    {
      //print triangular cells
      fprintf (f, "\nPOLYGONS %d %d\n", cells, cells * (step + 1));
      for (int j = 0; j < cells; j++)
      {
        fprintf (f, "3 %d %d %d\n", mesh->c2v[j*step], mesh->c2v[j*step + 1], mesh->c2v[j*step + 2]);
      }
    }
    if ( (step = loop->mapSize / loop->setSize) == 4)
    {
      //print quadrilateral edges
      fprintf (f, "\nPOLYGONS %d %d\n", cells, cells * (step + 1));
      for (int j = 0; j < cells; j++)
      {
        fprintf (f, "4 %d %d %d %d\n", mesh->c2v[j*step], mesh->c2v[j*step + 1], mesh->c2v[j*step + 2], mesh->c2v[j*step + 3]);
      }
    }
    
    
  
    fclose (f);
  }
  
  return VTK_OK;
}

void freeVtuMesh (vtu_mesh_t* mesh)
{
  free (mesh);
}


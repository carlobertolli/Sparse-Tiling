#include <stdio.h>
#include <stdlib.h>

#include "plotmesh.h"

vtu_mesh_t* createVtuMesh (int vertices, int edges, int cells)
{
  vtu_mesh_t* mesh = (vtu_mesh_t*) malloc (sizeof(vtu_mesh_t));
  
  mesh->vertices = vertices;
  mesh->edges = edges;
  mesh->cells = cells;
  
  return mesh;
}

void printVtuFile (inspector_t* insp, vtu_mesh_t* mesh)
{
  FILE* f = fopen ("GeneratedVTUFiles/prova.vtu", "w");
  
  fprintf (f, "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"BigEndian\">\n");
  fprintf (f, "  <UnstructuredGrid>\n");
  fprintf (f, "    <Piece NumberOfPoints=\"%d\" NumberOfCells=\"%d\">\n", mesh->vertices, mesh->cells);
  
  fprintf (f, "    </Piece>\n");
  fprintf (f, "  </UnstructuredGrid>\n");
  fprintf (f, "</VTKFile>\n");
  
  fclose (f);
  return;
}

void freeVtuMesh (vtu_mesh_t* mesh)
{
  free (mesh);
}
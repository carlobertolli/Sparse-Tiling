//
//  plotmesh.h
//  
//
//  Created by Fabio Luporini on 2/13/13.
//
//

#ifndef _PLOTMESH_H
#define _PLOTMESH_H

#include "inspector.h"

// return code when printing out the loops on a vtk file 
#define VTK_OK    0
#define VTK_UNDEF -1

#define VTK_DIR_MAX 20
#define VTK_DIR     "GeneratedVTUFiles"

typedef enum {
  D2,
  D3
} dim_t;

typedef struct {
  int nvertices;
  int nedges;
  int ncells;
  
  int* e2v;
  int* c2v;
  
  int dimension;    //2d or 3d mesh
  int* coordinates; //vertices' coordinates
  
} vtu_mesh_t;

vtu_mesh_t* createVtuMesh (int vertices, int edges, int cells, int* coordinates, int* e2v, int* c2v, dim_t dimension);
int printVtuFile (inspector_t* insp, vtu_mesh_t* mesh);
void freeVtuMesh (vtu_mesh_t* mesh);

#endif

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

typedef struct {
  int vertices;
  int edges;
  int cells;
  
} vtu_mesh_t;

vtu_mesh_t* createVtuMesh (int vertices, int edges, int cells);
void printVtuFile (inspector_t* insp, vtu_mesh_t* mesh);
void freeVtuMesh (vtu_mesh_t* mesh);

#endif

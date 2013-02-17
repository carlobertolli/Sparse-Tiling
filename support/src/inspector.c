/*
 *  inspector.c
 *  
 */

#include <string.h>

#include <metis.h>

#include "inspector.h"
#include "invert.h"

static int checkColor (loop_t *loop, const int *color, const int *partition, const int *verticesColor, const int *verticesPartition, const int *verticesAdjColor, const int *verticesAdjPartition, const int *incidence, int maxIncidence);

#if (DEBUG > 1)
static void printColoring (inspector_t *insp, loop_t *loop, int *entityColor, int *entityTile,
                           int *verticesSecondColor, int *verticesSecondTile, int *verticesAdjacentColor,
                           int *verticesAdjacentPartition);
#endif


/* input:
 * vertices		: #vertices
 * nparts			 : number of partitions requested to metis
 * xadj				  : vertices offsets in adjncy - standard CSR format
 * adjncy		 	: v2v mapping - standard CSR format
 * 
 * output:
 * *part			  : part[i] == partition of vertex 1, computed by metis
 */
int metisPartition (int vertices, int _nparts, idx_t* xadj, idx_t* adjncy, int** part)
{
  //input
  idx_t nvtxs	 = (idx_t) vertices;	// #vertices in the graph
  idx_t ncon	 = 1;					// #balancing constraint
  idx_t nparts = (idx_t) _nparts;		// #graph partitions
  
  idx_t options[METIS_NOPTIONS];
  
  METIS_SetDefaultOptions(options);
  options[METIS_OPTION_NUMBERING] = 0;		  
  options[METIS_OPTION_CONTIG] = 1;
  
  //output
  idx_t objval;	// edge-cut 
  idx_t* _part = (idx_t*) malloc( nvtxs * sizeof (idx_t)); //partition array 
  
  //computes the partitioning
  int result = METIS_PartGraphKway( &nvtxs, &ncon, xadj, adjncy, NULL, NULL, NULL, &nparts, NULL, NULL, options, &objval, _part );
  
  *part = (int*) _part;
  return result;
}

#if (DEBUG > 1)
static void printColoring (inspector_t *insp, loop_t *loop, int *entityColor, int *entityTile,
                           int *verticesSecondColor, int *verticesSecondTile, int *verticesAdjacentColor,
                           int *verticesAdjacentPartition)
{
  printf ("\nCOLORED ONWARDS\n");
  
  printf ("Entities Color: \n\t");
  for (int i = 0; i < loop->setSize; i++)
    printf ("%d ", entityColor[i]);
  printf ("\n");
  
  printf ("Entities Tile: \n\t");
  for (int i = 0; i < loop->setSize; i++)
    printf ("%d ", entityTile[i]);
  printf ("\n");
  
  printf ("Vertices Second Colors: \n\t");
  for (int i = 0; i < insp->size; i++)
    printf ("%d ", verticesSecondColor[i]);
  printf ("\n");
  
  printf ("Vertices Second Tile: \n\t");
  for (int i = 0; i < insp->size; i++)
    printf ("%d ", verticesSecondTile[i]);
  printf ("\n");
  
  printf ("Vertices Adjacent Colors and Partition: (Col, Part) \n\t");
  for (int i = 0; i < insp->size; i++) {
    for (int j = 0; j < insp->incidence; j++)
      printf ("(%d, %d)   ", verticesAdjacentColor[i*insp->incidence + j], verticesAdjacentPartition[i*insp->incidence + j]);
    printf("\n\t");
  }
}
#endif

void printInspector (inspector_t* insp)
{
  if ( !insp )
  {
    printf("insp is NULL\n");
    return;
  }
  
  printf("- INSPECTOR -\n\n");
  printf("Size of the base set:      %d\n", insp->size);
  printf("Number of tiles:           %d\n", insp->ntiles);
  printf("MAX incidence of the mesh: %d\n", insp->incidence);
  
  printf("\nInitial coloring set:\n\t");
  if ( insp->colOrig )
    for (int i = 0; i < insp->size; i++ )
      printf ("%d ", insp->colOrig[i]);
  else 
    printf("No current coloring set");
  printf("\n\n");
  
  printf("Initial distribution of the base set among tiles:\n");
  for (int b = 0; b < insp->ntiles; b++ )
  {
    int offset = ( b ) ? insp->partSize[b - 1] : 0;
    for (int j = 0; j < insp->partSize[b]; j++ )
      printf("\t%d ", insp->p2v[b*offset + j]);
    printf("\n");
  }
  
  if ( insp->v2v )
  {
    printf("\nRenumbered base set:\n");
    for (int i = 0; i < insp->size; i++ )
      printf ("%d ", insp->v2v[i]);
    printf("\n");
  }
  
  printf("\nLOOPS (%d crossed / %d allowed):\n", insp->loopCounter, insp->nloops);
  for (int i = 0; i < insp->loopCounter; i++)
  {
    printf("\tLoop %d (setSize: %d, mapSize: %d)\n", i, insp->loops[i]->setSize, insp->loops[i]->mapSize);
    printf("\tIndirect map (to the *renumbered* base set): \n\t\t");
    for (int j = 0; j < insp->loops[i]->mapSize; j++ )
      printf("%d ", insp->loops[i]->indMap[j]);
    printf("\n");
  }
  
  printf("\nInspector printed!\n");
}

inspector_t* initInspector (int baseset, int partSize, int nloops)
{
  inspector_t* insp = (inspector_t*) malloc (sizeof(inspector_t));
  
  insp->v2pOrig = NULL;
  insp->colOrig = NULL;
  insp->v2v = NULL;
  
  insp->size = baseset;
  insp->incidence = 0;
  
  //init tiles
  insp->ntiles = (baseset % partSize) ? baseset / partSize + 1 : baseset / partSize;
  insp->partSize = (int*) malloc (insp->ntiles * sizeof(int));
  insp->tiles = (tile_t**) malloc (insp->ntiles * sizeof(tile_t*));
  
  for ( int b = 0; b < insp->ntiles; b++ )
    insp->tiles[b] = createTile ();
  
  //init loops
  insp->loopCounter = 0;
  insp->nloops = nloops;
  insp->loops = (loop_t**) malloc (nloops * sizeof(loop_t*));
  
  return insp;
}

void freeInspector (inspector_t* insp)
{
  for (int b = 0; b < insp->ntiles; b++)
  {
    freeTile (insp->tiles[b]);
    insp->tiles[b] = NULL;
  }
  
  for (int i = 0; i < insp->loopCounter; i++)
  {
    //TODO: free (insp->loops[i]->indMap);
#ifdef VTK_ON
    free (insp->loops[i]->setColor);
#endif
    free (insp->loops[i]->loopname);
    free (insp->loops[i]);
  }
  
  free (insp->loops);
  
  free (insp->v2pOrig);
  
  free (insp->colOrig);
  free (insp->v2v);
  free (insp->partSize);
  free (insp->p2v);
  free (insp->p2c);
  
  free (insp);
  
}

static int checkColor (loop_t *loop, const int *color, const int *partition, const int *verticesColor, const int *verticesPartition, const int *verticesAdjColor, const int *verticesAdjPartition, const int *incidence, int maxIncidence)
{
  //aliases
  int setSize = loop->setSize;
  int mapSize = loop->mapSize;
  int *indMap = loop->indMap;
  
  int step = mapSize / setSize;
  for (int e = 0; e < setSize; e++)
  {
    //aliases
    int entityColor = color[e];
    int entityTile = partition[e];
    for (int i = 0; i < step; i++)
    {
      int currentVertex = indMap[e*step + i];
      
      //if entity's color and partition are equal to current adjacent vertex's color and partition, that's fine and I skip the control
      if (! (entityColor == verticesColor[currentVertex] && entityTile == verticesPartition[currentVertex]))
      {
        int currentIncidence = incidence[currentVertex];
        for (int j = 0; j < currentIncidence; j++)
        { 
          //now I'm looking for entities adjacent to the current vertex that have same color, but different partition
          //In that case, the coloring is messed up
          if (entityColor == verticesAdjColor[currentVertex*maxIncidence + j] && entityTile != verticesAdjPartition[currentVertex*maxIncidence + j])
          {
            snprintf (loop->debug, DEBUGMSGLENGTH, "(%d, %d, %d) found (%d, %d) through adjacent vertex %d",
                      e, entityColor, entityTile, verticesAdjColor[currentVertex*maxIncidence + j], verticesAdjPartition[currentVertex*maxIncidence + j], currentVertex);
            return INSPOP_WRONGCOLOR;
          }
        }
      }
    }
  }
  
  return INSPOP_OK;
}

void doMaxColor (tile_t** tiles, int* v2p, int* baseSetColors, loop_t *loop, int* color, int* partition, int loopID)
{
  //aliases
  int setSize = loop->setSize;
  int mapSize = loop->mapSize;
  int *indMap = loop->indMap;

  int step = mapSize / setSize;
  for (int e = 0; e < setSize; e++)
  {
    //find the maximum color (and relative index) among the base set elements adjacent to the set element e
    int newMaxColor, maxColor = baseSetColors[indMap[e*step]];
    int maxColorIndex = e * step;
    
    for (int i = 1; i < step; i++)
    {
      newMaxColor = MAX (maxColor, baseSetColors[indMap[e*step + i]]);
      
#if (DEBUG > 2)
      printf("\ne = %d, maxColor = %d, l1->indMap[%d] = %d, l1->indMap[%d] = %d, baseSetColors[l1->indMap[%d]] = %d, baseSetColors[l1->indMap[%d]] = %d",
             e, maxColor, e * step, indMap[e * step], e * step + i, indMap[e * step + i], e * step, baseSetColors[indMap[e * step]], e * step + i, baseSetColors[indMap[e * step + i]]);
#endif
      
      if ( newMaxColor != maxColor )
      {
        maxColor = newMaxColor;
        maxColorIndex = e * step + i;
      }
    }
    
    //add the color to the loop iteration set
    color[e] = maxColor;
    
    //add the element e to the corresponding tile (which corresponds to the partition of the base set element maxColorIndex)
    partition[e] = v2p[indMap[maxColorIndex]];
    addElement (tiles[partition[e]], loopID, e);
  }
}

void doMinColor (tile_t** tiles, int* v2p, int* baseSetColors, loop_t *loop, int* color, int* partition, int loopID)
{
  //aliases
  int setSize = loop->setSize;
  int mapSize = loop->mapSize;
  int *indMap = loop->indMap;
  
  int step = mapSize / setSize;
  for (int e = 0; e < setSize; e++)
  {
    //find the minium color (and relative index) among the base set elements adjacent to the set element e
    int newMinColor, minColor = baseSetColors[indMap[e * step]];
    int minColorIndex = e * step;
    
    for (int i = 1; i < step; i++)
    {
      newMinColor = MIN (minColor, baseSetColors[indMap[e * step + i]]);
      
#if (DEBUG > 2)
      printf("\ne = %d, minColor = %d, l1->indMap[%d] = %d, l1->indMap[%d] = %d, baseSetColors[l1->indMap[%d]] = %d",
             e, minColor, e * step, indMap[e * step], e * step + i, indMap[e * step + i], e * step + i, baseSetColors[indMap[e * step + i]]);
#endif
      
      if ( newMinColor != minColor )
      {
        minColor = newMinColor;
        minColorIndex = e * step + i;
      }
    }
    
    //add the color to the loop iteration set
    color[e] = minColor;
    
    //add the element e to the corresponding tile (which corresponds to the partition of the base set element minColorIndex)
    partition[e] = v2p[indMap[minColorIndex]];
    addElement (tiles[partition[e]], loopID, e);
  }
}


int runInspector (inspector_t* insp, int baseSetIndex)
{
  if (baseSetIndex >= insp->nloops - 1)
    return INSPOP_WRONGPAR;
  
  if (insp->loopCounter < 2 || insp->nloops != insp->loopCounter)
    return INSPOP_NOTENAUGHLOOP;
  
  //allocate working array
  int* workVerticesPartition = (int*) malloc (insp->size * sizeof(int));
  int* workVertices  = (int*) malloc (insp->size * sizeof(int)); //for vertices second color
  
  memcpy (workVertices, insp->colOrig, insp->size * sizeof(int));
  memcpy (workVerticesPartition, insp->v2pOrig, insp->size * sizeof(int));
  
  //determining maximum loop size (for coloring)
  int maxLoopSize = insp->loops[0]->setSize;
  for (int i = 0; i < insp->nloops; i++) 
    maxLoopSize = MAX (maxLoopSize, insp->loops[i]->setSize);
  
  //two working arrays for loop second color and partition, both sized to the largest iteration set that has to be worked out
  int* workLoopColor = (int*) malloc (maxLoopSize * sizeof(int));
  int* workLoopPartition = (int*) malloc (maxLoopSize * sizeof(int)); //for entities second partition
  
  //color and partition of verteces' adjacent elements after a sweep
  int* inserted = (int*) calloc (insp->size, sizeof(int));
  int* verticesAdjacentColor = (int*) malloc (insp->size * insp->incidence * sizeof(int));
  int* verticesAdjacentPartition = (int*) malloc (insp->size * insp->incidence * sizeof(int));
  for (int i = 0; i < insp->size * insp->incidence; i++)
  {
    verticesAdjacentColor[i] = -1;
    verticesAdjacentPartition[i] = -1;
  }
  
  // COLORING
  
  // A) proceed coloring - FORWARD
  for (int s = baseSetIndex + 1; s < insp->nloops; s++)
  {
    loop_t* startLoop = insp->loops[s];
    
    // 1) color the loop
    doMaxColor (insp->tiles, workVerticesPartition, workVertices, startLoop, workLoopColor, workLoopPartition, s);
    
    // 2) prepare data for the subsequent loop coloring
    //set to -1 each entry of the vertex second color array
    for (int i = 0; i < insp->size; i++)
      workVertices[i] = -1;
    
    // 3) compute the vertices second color based on workLoopColor, workPartition and workArray
    int step = startLoop->mapSize / startLoop->setSize;
    for (int e = 0; e < startLoop->setSize; e++)
    {
      int newColor = - 1;
      int entityColor = workLoopColor[e]; //the color previously assigned to the entity is initially assumed to be the maximum one
      int entityPartition = workLoopPartition[e]; 
      
      //then, we iterate over its adjacent base set elements (vertices?) 
      for (int i = 0; i < step; i++)
      {
        int currentVertex = startLoop->indMap[e * step + i];
        newColor = MAX (entityColor, workVertices[currentVertex]);
        
        if (newColor != workVertices[currentVertex])
        {
          workVertices[currentVertex] = newColor;
          workVerticesPartition[currentVertex] = entityPartition;
        }
       
        //storing the entity's color and partition in the target vertex local memory so that a vertex knows
        //by which entity ('s color and partition) is touched. This is useful for checking the correctness of coloring
        verticesAdjacentColor[currentVertex*insp->incidence + inserted[currentVertex]] = entityColor;
        verticesAdjacentPartition[currentVertex*insp->incidence + inserted[currentVertex]] = entityPartition;
        inserted[currentVertex]++; 
      }
    }
    
#if (DEBUG > 1)
    printColoring (insp, startLoop, workLoopColor, workLoopPartition, workVertices, workVerticesPartition, verticesAdjacentColor, verticesAdjacentPartition);
#endif
    
#ifdef VTK_ON
    // save loop color
    memcpy (startLoop->setColor, workLoopColor, startLoop->setSize*sizeof(int));
#endif
    
    // 4) check coloring
    int coloring = checkColor (startLoop, workLoopColor, workLoopPartition, workVertices, workVerticesPartition, verticesAdjacentColor, verticesAdjacentPartition, inserted, insp->incidence);
    if (coloring != INSPOP_OK) 
    {
      snprintf (insp->debug, DEBUGMSGLENGTH + LOOPNAMELENGTH, "Coloring loop %s resulted in messing up colors\n%s", startLoop->loopname, startLoop->debug);
      return INSPOP_WRONGCOLOR;
    }

#if (DEBUG > 1)
    for (int i = 0; i < insp->size * insp->incidence; i++)
    {
      verticesAdjacentColor[i] = -1;
      verticesAdjacentPartition[i] = -1;
    }
#endif
    
    memset (inserted, 0, insp->size * sizeof(int));
  }
    
  //reset base set values
  memcpy (workVertices, insp->colOrig, insp->size * sizeof(int));
  memcpy (workVerticesPartition, insp->v2pOrig, insp->size * sizeof(int));
  
  //3) proceed coloring - BACKWARD
  for (int s = baseSetIndex; s >= 0; s--)
  {
    loop_t* startLoop = insp->loops[s];
    
    // 1) color the loop
    doMinColor (insp->tiles, workVerticesPartition, workVertices, startLoop, workLoopColor, workLoopPartition, s);
    
    // 2) prepare data for the subsequent loop coloring
    //set to -1 each entry of the vertex second color array
    for (int i = 0; i < insp->size; i++)
      workVertices[i] = -1;
    
    // 3) compute the vertices second color based on workLoopColor, workPartition and workArray
    int step = startLoop->mapSize / startLoop->setSize;
    for (int e = 0; e < startLoop->setSize; e++)
    {
      int newColor = - 1;
      int entityColor = workLoopColor[e]; //the color previously assigned to the entity is initially assumed to be the maximum one
      int entityPartition = workLoopPartition[e];
      
      //then, we iterate over its adjacent base set elements (vertices?)
      for (int i = 0; i < step; i++)
      {
        int currentVertex = startLoop->indMap[e * step + i];
        newColor = MAX (entityColor, workVertices[currentVertex]);
        
        if (newColor != workVertices[currentVertex])
        {
          workVertices[currentVertex] = newColor;
          workVerticesPartition[currentVertex] = entityPartition;
        }
        verticesAdjacentColor[currentVertex*insp->incidence + inserted[currentVertex]] = entityColor;
        verticesAdjacentPartition[currentVertex*insp->incidence + inserted[currentVertex]] = entityPartition;
        inserted[currentVertex]++; 
      }
    }
    
#if (DEBUG > 1)
    printColoring (insp, startLoop, workLoopColor, workLoopPartition, workVertices, workVerticesPartition, verticesAdjacentColor, verticesAdjacentPartition);
#endif
    
#ifdef VTK_ON
    // save loop color
    memcpy (startLoop->setColor, workLoopColor, startLoop->setSize*sizeof(int));
#endif
    
    // 4) check coloring
    int coloring = checkColor (startLoop, workLoopColor, workLoopPartition, workVertices, workVerticesPartition, verticesAdjacentColor, verticesAdjacentPartition, inserted, insp->incidence);
    if (coloring != INSPOP_OK)
    {
      snprintf (insp->debug, DEBUGMSGLENGTH + LOOPNAMELENGTH, "Coloring loop %s resulted in messing up colors\n%s", startLoop->loopname, startLoop->debug);
      return INSPOP_WRONGCOLOR;
    }

#if (DEBUG > 1)
    for (int i = 0; i < insp->size * insp->incidence; i++)
    {
      verticesAdjacentColor[i] = -1;
      verticesAdjacentPartition[i] = -1;
    }
#endif
    
    memset (inserted, 0, insp->size * sizeof(int));
  }
  
  // free work array
  free (inserted);
  free (verticesAdjacentColor);
  free (verticesAdjacentPartition);
  free (workVertices);
  free (workVerticesPartition);
  free (workLoopPartition);
  free (workLoopColor);
  
  return INSPOP_OK;
}

int addParLoop (inspector_t* insp, char* loopname, int setSize, int* indirectionMap, int mapSize)
{
  if ( insp->loopCounter >= insp->nloops )
    return INSPOP_MAXLOOP;
  
  //create a new mapping for the parloop, from the original OP2 indirectionMap to the new one which reflects the new position of vertices in p2v
  int* renumberedMap = (int*) malloc (mapSize * sizeof(int));
  newCodomain (indirectionMap, mapSize, insp->v2v, renumberedMap );
  
  // store parloop parameters into insp
  insp->loops[insp->loopCounter] = (loop_t*) malloc (sizeof(loop_t));
  
  insp->loops[insp->loopCounter]->loopname = (char*) malloc (sizeof(char)*LOOPNAMELENGTH);
  strncpy (insp->loops[insp->loopCounter]->loopname, loopname, LOOPNAMELENGTH);
  
  insp->loops[insp->loopCounter]->setSize = setSize;
  insp->loops[insp->loopCounter]->indMap = renumberedMap; 
  insp->loops[insp->loopCounter]->mapSize = mapSize;
  //insp->loops[insp->loopCounter]->workColor = (int*) malloc (setSize * sizeof(int));
  
#ifdef VTK_ON
  insp->loops[insp->loopCounter]->setColor = (int*) malloc (setSize*sizeof(int));
#endif
  
  // add the parloop to each tile of the inspector
  for (int i = 0; i < insp->ntiles; i++)
    addLoop (insp->tiles[i], setSize, loopname); // IMPORTANT: an upper bound to the tile iteration set size is specified here (the whole set)
  
  insp->loopCounter++;
  return INSPOP_OK;
}

int partitionAndColor (inspector_t* insp, int vertices, int* e2v, int mapsize)
{	
  // invert the mapping, v2e is needed to compute coloring
  int* v2e 	= (int*) malloc ( mapsize * sizeof(int) );
  int* v2e_offset	= (int*) calloc ( vertices + 1, sizeof(int) ); 
  int* adjncy = (int*) malloc ( mapsize * sizeof(int) );
  
  //invert mapping, i.e. creates v2e mapping, and call metis to compute the partitioning
  invertMapping ( e2v, mapsize, vertices, 2, 1, v2e, adjncy, v2e_offset, &insp->incidence);

#if (DEBUG > 0)   
  for (int i = 0; i < mapsize; i++)
    printf("v2e[i] = %d\n", v2e[i]);
  for (int i = 0; i < vertices+1; i++)
    printf("v2e_offset[i] = %d\n", v2e_offset[i]);  
#endif  
  
  int* v2p;
  metisPartition (vertices, insp->ntiles, (idx_t*) v2e_offset, (idx_t*) adjncy, &v2p); //TODO: 2 is just a fixed value for the example..
  
  // compute the mapping p2v as it is needed to determine a coloring scheme
  int* p2v = (int*) malloc ( vertices * sizeof(int) );
  int* p2v_offset = (int*) calloc ( insp->ntiles + 1, sizeof(int) );
  
  invertMapping (v2p, vertices, insp->ntiles, 1, 1, p2v, NULL, p2v_offset, NULL);
  
  // add the p2v mapping and the partition sizes to the inspector
  insp->p2v = p2v;
  for ( int b = 0; b < insp->ntiles; b++ )
    insp->partSize[b] = p2v_offset[b + 1] - p2v_offset[b];
  
  // init colors
  int* colors = (int*) malloc (insp->ntiles * sizeof (int));
  for ( int b = 0; b < insp->ntiles; b++ ) 
    colors[b] = -1;
  
  int repeat = 1;
  int ncolor = 0;
  int ncolors = 0;
  int prev_offset, next_offset;
  int totSize = v2e_offset[vertices];
  
  // allocate and zero out 
  int* work = (int*) malloc (totSize * sizeof(int));
  
  prev_offset = 0; 
  next_offset = 0;
  
  // coloring algorithm
  while ( repeat )
  {
    repeat = 0;
    
    // zero out color arrays
    for ( int e = 0; e < totSize; e++ )
      work[e] = 0;
    
    // starts trying to color all blocks
    for ( int b = 0; b < insp->ntiles; b++ )
    {
      prev_offset = next_offset;
      
      //adjusts offsets of partitions
      if ( prev_offset + insp->partSize[b] >= vertices ) // last partition can be smaller than partition size
        next_offset = vertices;
      else
        next_offset = prev_offset + insp->partSize[b];
      
      if ( colors[b] == -1 )
      {
        unsigned int mask = 0;
        
        for ( int e = prev_offset; e < next_offset; e++ ) 
        {
          int v = p2v[e];
          for ( int j = 0; j < (v2e_offset[v + 1] - v2e_offset[v]); j++ )  
              mask |= work[v2e[v2e_offset[v] + j]]; // set bits of mask
        }
        
#if (DEBUG > 0) 
        printf ("WORK LOADED: [ ");
        for ( int i = 0; i < totSize; i++ )
          printf ("%d ", work[i] );
        printf ("]\n");
#endif
        
        int color = ffs( ~mask ) - 1; // find first bit not set
        if ( color == -1 )
        { //run out of colors on this pass
          repeat = 1;
        }
        else
        {
          colors[b] = ncolor + color;
          mask = 1 << color;
          ncolors = MAX( ncolors, ncolor + color + 1 );
          
          for ( int e = prev_offset; e < next_offset; e++ )
          {
            int v = p2v[e];		
            for ( int j = 0; j < (v2e_offset[v + 1] - v2e_offset[v]); j++ )  
              work[v2e[v2e_offset[v] + j]] |= mask; 
          }
        }
      }
    }
    
    ncolor += 32; // increment base level
  }
  
  //useful for executor
  insp->ncolors = ncolors;
  insp->p2c = colors; 
  
  // create a mapping from the original base set to the new positions in the renumbered base set. This is useful to renumber all mapppings in game.
  int* mappingFunction = (int*) malloc (insp->size * sizeof(int));
  baseMapping (insp->p2v, insp->size, mappingFunction);
  insp->v2v = mappingFunction;
  
  // initialize the inspector with the coloring and partitioning w.r.t. the renumbered base set
  insp->v2pOrig = (int*) malloc (insp->size * sizeof(int));
  insp->colOrig = (int*) malloc (insp->size * sizeof(int));
  
  int* offset = (int*) malloc ((insp->ntiles + 1) * sizeof(int));
  offset[0] = 0;
  
  for (int i = 1; i <= insp->ntiles; i++ )
    offset[i] = insp->partSize[i - 1] + offset[i - 1];
  
  // colors
  for (int b = 0; b < insp->ntiles; b++ )
  {
    for (int j = offset[b]; j < offset[b + 1]; j++ )
      insp->colOrig[j] = colors[b];
  }
  
  // partitions
  newDomain (v2p, insp->size, insp->v2v, insp->v2pOrig);
  
  /*
   for ( int i = 0; i < insp->size; i++ ){
   printf("%d\t", v2p[i]);
   }
   printf("\n");
   for ( int i = 0; i < insp->size; i++ ){
   printf("%d\t", insp->v2pOrig[i]);
   }
   printf("\n");
   */
  
  free (offset);
  
  free (work);
  
  free (p2v_offset);
  free (v2e_offset);
  free (adjncy);
  free (v2e);
  
  return INSPOP_OK;
}



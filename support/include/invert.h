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

/*
 * Invert the mapping, from e2v to v2e
 *
 * input: 
 * e2v			: mapping from edges to vertices
 * nvertices	: number of vertices
 * map_entries	: number of entries in e2v
 * 
 * output:
 * v2e			: mapping from vertices to edges
 * offset		: edges offset in v2e    
 * adjncy		: mapping from vertices to vertices
 * vertex_zero	: == 0 means vertex numbering starts from 1
 */
void invertMappingEV (int* e2v, int nvertices, int map_entries, int* v2e, int* offset, int* adjncy, int vertex_zero);

/*
 * 
 *
 */
int metisPartition (int nvertices, int _nparts, int* xadj, int* adjncy, int** part);

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
void partitionAndColor (int nvertices, int partitionSize, ve_map* v2e, int** _partitions, int** _colors);


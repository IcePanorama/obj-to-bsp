#ifndef _BINARY_SPACE_PARTITIONING_TREE_H_
#define _BINARY_SPACE_PARTITIONING_TREE_H_

#include "obj/file.h"

typedef struct BSPTree_s BSPTree_t;

BSPTree_t *bsp_alloc (void);
/** Returns: Zero on success, non-zero on failure. */
int bsp_process_obj (BSPTree_t *t, ObjFile_t *obj, float sp_center[static 4],
                     float sp_norm[static 4]);
void bsp_free (BSPTree_t *t);

#endif /* _BINARY_SPACE_PARTITIONING_TREE_H_ */

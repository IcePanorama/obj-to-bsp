#ifndef _BINARY_SPACE_PARTITIONING_TREE_H_
#define _BINARY_SPACE_PARTITIONING_TREE_H_

typedef struct BSPTree_s BSPTree_t;

BSPTree_t *bsp_create (void);
void bsp_free (BSPTree_t *t);

#endif /* _BINARY_SPACE_PARTITIONING_TREE_H_ */

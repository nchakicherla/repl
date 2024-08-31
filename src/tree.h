#ifndef TREE_H
#define TREE_H

#include "common.h"

typedef struct s_BNode {
	int32_t val;
	struct s_BNode *left;
	struct s_BNode *right;
} BNode;

#endif // TREE_H

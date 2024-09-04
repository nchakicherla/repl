#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

char *readFile(char *name, MemPool *pool);

#endif // FILE_H

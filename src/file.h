#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

char *readFile(char *name, MemPool *pool);

int writeFile(char *name, char *source);

#endif // FILE_H

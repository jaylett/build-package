/*
 * $Id: memory.h,v 1.2 1999/09/07 13:49:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "stdio.h"
#include "stdlib.h"
#include "stddef.h"

void *memalloc(size_t);
void *memrealloc(void *, size_t);
void memfree(void *);

#endif /* MEMORY_H */

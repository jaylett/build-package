/*
 * $Id: memory.h,v 1.3 2001/01/14 15:37:43 james Exp $
 * build-package
 * (c) Copyright James Aylett
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

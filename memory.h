/*
 * $Id: memory.h,v 1.1 1999/08/10 15:26:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#ifndef MEMORY_H
#define MEMORY_H

void *memalloc(size_t);
void *rememalloc(void *, size_t);
void memfree(void *);

#endif /* MEMORY_H */

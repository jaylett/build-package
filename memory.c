/*
 * $Id: memory.c,v 1.1 1999/09/16 16:54:14 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "memory.h"

void *memalloc(size_t size)
{
  void *res;
  res = malloc(size);
  if (res==NULL)
    fatal_error("Couldn't get memory.");
  return res;
}

void *memrealloc(void * bl, size_t size)
{
  void *res;
  if (bl==NULL)
    res = malloc(size);
  else
    res = realloc(bl, size);
  if (res==NULL)
    fatal_error("Couldn't get memory.");
  return res;
}

void memfree(void *bl)
{
  if (bl!=NULL)
    free(bl);
}

/*
 * $Id: memory.c,v 1.2 2001/01/14 15:37:43 james Exp $
 * build-package
 * (c) Copyright James Aylett
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

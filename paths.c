/*
 * $Id: paths.c,v 1.1 1999/08/10 15:26:59 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "paths.h"

/* equivalent of /bin/sh -e "mkdir -p <dirpath>" */
void mkdirs(char *dirpath)
{
  char *next, *where;
  char *temp = memalloc(strlen(dirpath)+1);
  strcpy(temp, dirpath); /* our own copy to work with */
  where = memalloc(3);
  if (temp[0] == '/')
    strcpy(where, "/");
  else
    strcpy(where, "./");
  next = strtok(temp, "/");
  while (next!=NULL)
  {
    if (next[0]!=0)
    {
      /* element to do here ... */
      struct stat s;
      char *new = makename(where, next);
      memfree(where);
      if (stat(new, &s)!=0)
      {
        if (errno==ENOENT || errno==ENOTDIR)
	{
          printf("creating missing directory %s\n", new);
          mkdir(new, 0775);
	}
        else
	  do_error("stat on %s failed", new);
      }
      where = new;
    }
    strtok(NULL, "/");
  }
  memfree(where);
}

char *makename(char *path, char *leaf)
{
  char *result = memalloc(strlen(path)+strlen(leaf)+2);
  if (result==NULL)
    do_error("memory error");
  strcpy(result, path);
  result[strlen(result)+1] = 0;
  result[strlen(result)] = '/';
  strcat(result, leaf);
  return result;
}

char *comb_paths(char *a, char *b, char *c)
{
  int i;
  char *result;
  i = strlen(a) + (b==NULL)?(0):(strlen(b)+1) + strlen(c) + 2;
  result = memalloc(i);
  if (result==NULL)
    do_error("memory error");
  strcpy(result, a);
  if (b!=NULL)
  {
    result[strlen(result)+1]=0;
    result[strlen(result)]='/';
    strcat(result, b);
  }
  result[strlen(result)+1]=0;
  result[strlen(result)]='/';
  strcat(result, c);
  return result;
}

char *comb_path(char *base, char *a, char *b)
{
  if (a==NULL)
    return trans_path(base, b);
  else
  {
    char *temp = trans_path(base, a);
    int i = strlen(temp)+strlen(b) + 2;
    char *result = memalloc(i);
    strcpy(result, temp);
    result[strlen(result)+1]=0;
    result[strlen(result)]='/';
    strcat(result, b);
    memfree(temp);
    return result;
  }
}

/* returns a memalloc()ed block */
char *trans_path(char *base, char *path)
{
  char *result;
  if (path[0]=='/')
  {
    result = memalloc(strlen(base)+strlen(path)+1);
    strcpy(result, base);
    strcat(result, path); /* already has '/' separator, don't need to add */
  }
  else
  {
    result = memalloc(strlen(path)+1);
    strcpy(result, path);
  }
  return result;
}

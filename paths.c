/*
 * $Id: paths.c,v 1.5 2002/02/01 13:06:29 james Exp $
 * build-package
 * (c) Copyright James Aylett
 *
 * build-package is released under the GPL.
 */

#include "paths.h"

void mkdirs_less_leaf(char *dirpath)
{
  char *last, *mycopy;
  mycopy = memalloc((strlen(dirpath)+1) * sizeof(char));
  strcpy(mycopy, dirpath);
  last = rindex(mycopy, '/');
  if (last!=NULL)
  {
    last[0]=0;
    mkdirs(mycopy);
  }
  memfree(mycopy);
  return;
}

/* equivalent of /bin/sh -e "mkdir -p <dirpath>" */
void mkdirs(char *dirpath)
{
  char *next, *where;
  char *temp;
  temp = memalloc(strlen(dirpath)+1);
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
/*          do_error("    creating missing directory %s", new);*/
          mkdir(new, 0775);
	}
        else
	  fatal_error("stat on %s failed", new);
      }
      where = new;
    }
    next = strtok(NULL, "/");
  }
  memfree(where);
}

/* makes a path of the form path/leaf */
char *makename(char *path, char *leaf)
{
  char *result = memalloc(strlen(path)+strlen(leaf)+2);
  strcpy(result, path);
  if (result[strlen(result)-1]!='/')
  {
    result[strlen(result)+1] = 0;
    result[strlen(result)] = '/';
  }
  strcat(result, leaf);
  return result;
}

/* makes a path of the form a/b/c, or a/c if b is NULL */
char *comb_paths(char *a, char *b, char *c)
{
  int i;
  char *result;
  /*  do_error("comb_paths(%s,%s,%s)", a, b, c);*/
  i = strlen(a) + ((b==NULL)?(0):(strlen(b)+1)) + strlen(c) + 2;
  result = memalloc(i);
  strcpy(result, a);
  if (b!=NULL)
  {
    if (result[strlen(result)-1]!='/')
    {
      result[strlen(result)+1]=0;
      result[strlen(result)]='/';
    }
    strcat(result, b);
  }
  if (result[strlen(result)-1]!='/')
  {
    result[strlen(result)+1]=0;
    result[strlen(result)]='/';
  }
  strcat(result, c);
  return result;
}

char *comb_path(char *base, char *a, char *b)
{
/*  do_error("comb_path(%s, %s, %s)", base, a, b);*/
  if (a==NULL)
    return trans_path(base, b);
  else
  {
    char *temp = trans_path(base, a);
    int i = strlen(temp)+strlen(b) + 2;
    char *result = memalloc(i);
    strcpy(result, temp);
    if (result[strlen(result)-1]!='/')
    {
      result[strlen(result)+1]=0;
      result[strlen(result)]='/';
    }
    strcat(result, b);
    memfree(temp);
/*    do_error("comb_path() = %s", result);*/
    return result;
  }
}

/* returns a memalloc()ed block */
char *trans_path(char *base, char *path)
{
  char *result;
/*  do_error("trans_path(%s, %s)", base, path);*/
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
/*  do_error("trans_path() = %s", result);*/
  return result;
}

/*
 * $Id: paths.h,v 1.3 2001/01/14 15:37:43 james Exp $
 * build-package
 * (c) Copyright James Aylett
 *
 * build-package is released under the GPL.
 */

#include "build.h"

#ifndef BUILD_MAIN
#define BUILD_MAIN

#include "main.h"

#endif /* BUILD_MAIN */

char *trans_path(char *, char *);
char *comb_path(char *, char *, char *);
char *comb_paths(char *, char *, char *);
char *makename(char *, char *);
void mkdirs(char *);
void mkdirs_less_leaf(char *);

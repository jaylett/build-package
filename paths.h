/*
 * $Id: paths.h,v 1.1 1999/08/10 15:26:59 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
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

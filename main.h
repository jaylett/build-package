/*
 * $Id: main.h,v 1.1 1999/08/10 15:26:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "build.h"

#ifndef BUILD_MAIN
#define BUILD_MAIN

#include "coopt.h"

#include "option.h"
#include "module.h"
#include "paths.h"
#include "step.h"

#define RMCOMMAND "rm -rf "
char *tmptree;
char *startdir;
int deltree = -1; /* 0 => do delete it (it'll be in tmptree) */
struct module *cli;

#else /* BUILD_MAIN */

extern char *tmptree;
extern char *startdir;
extern struct module *cli;

#endif /* BUILD_MAIN */

int main(int, char const * const *);
void do_error(char *, ...);

/*
 * $Id: main.h,v 1.2 1999/09/07 13:49:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "build.h"

#ifndef BUILD_MAIN
#define BUILD_MAIN

#include <stdarg.h>

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

char *do_getcwd(void);
void do_help(void);
void do_version(void);

#else /* BUILD_MAIN */

extern char *tmptree;
extern char *startdir;
extern struct module *cli;

#endif /* BUILD_MAIN */

int main(int, char const * const *);
void fatal_error(char *, ...);
void do_error(char *, ...);
void int_flogf(FILE *, ...);

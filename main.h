/*
 * $Id: main.h,v 1.4 2000/03/09 16:30:02 james Exp $
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

char *tmptree;
char *startdir=NULL;
struct module *cli;

char *do_getcwd(void);
void do_help(void);
void do_version(void);
int paranoid=0;

#else /* BUILD_MAIN */

extern char *tmptree;
extern char *startdir;
extern struct module *cli;
extern int paranoid;

#endif /* BUILD_MAIN */

#define RMCOMMAND "rm -rf "
int main(int, char const * const *);
void fatal_error(char *, ...);
void do_error(char *, ...);
void int_flogf(FILE *, ...);

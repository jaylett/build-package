/*
 * $Id: main.h,v 1.6 2001/01/14 15:37:43 james Exp $
 * build-package
 * (c) Copyright James Aylett
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
int ignore_cvs = FALSE;

char *do_getcwd(void);
void do_help(void);
void do_version(void);

#else /* BUILD_MAIN */

extern char *tmptree;
extern char *startdir;
extern struct module *cli;
extern int ignore_cvs;

#endif /* BUILD_MAIN */

#define RMCOMMAND "rm -rf "
int main(int, char const * const *);
void fatal_error(char *, ...);
void do_error(char *, ...);
void int_flogf(FILE *, ...);

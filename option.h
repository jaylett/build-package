/*
 * $Id: option.h,v 1.1 1999/08/10 15:26:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "build.h"

#ifndef BUILD_MAIN
#define BUILD_MAIN

#include "main.h"
#include "module.h"

char *get_option(struct module *, int);
int find_option(struct module *, char *);
void add_string(char ***, unsigned int *, char const *);

#endif /* BUILD_MAIN */

void add_option(struct module *, char const * );
char *read_option(struct module *, char *);
void add_source(struct step *, char const *);

/*
 * $Id: option.h,v 1.4 2001/01/14 15:37:43 james Exp $
 * build-package
 * (c) Copyright James Aylett
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
char *copy_string_no_trailing_spaces(char const *);

#endif /* BUILD_MAIN */

void add_option(struct module *, char const * );
char *read_option(struct module *, char *);
void add_string(char ***, unsigned int *, char const *);

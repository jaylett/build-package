/*
 * $Id: module.h,v 1.1 1999/08/10 15:26:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "build.h"

#ifndef BUILD_MAIN
#define BUILD_MAIN

#include "main.h"
#include "option.h"
#include "paths.h"
#include "step.h"

struct module **modules = NULL;
unsigned int num_modules = 0;

#else /* BUILD_MAIN */

extern struct module **modules;

#endif /* BUILD_MAIN */

struct module *find_module(char *);
struct module *new_module(char *);
void scan_modules(void);
void build_module(struct module *);

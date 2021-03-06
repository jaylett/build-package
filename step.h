/*
 * $Id: step.h,v 1.3 2001/01/14 15:37:43 james Exp $
 * build-package
 * (c) Copyright James Aylett
 *
 * build-package is released under the GPL.
 */

#include "build.h"

#ifndef BUILD_MAIN
#define BUILD_MAIN

#include "main.h"
#include "paths.h"
#include "option.h"
#include "module.h"

#endif /* BUILD_MAIN */

void add_step(struct module *, struct step *);
struct step *new_step();
void run_step(struct module *, struct step *);
void mirror_dir(char *, char *);
void copy_source(struct module *, struct step *, char *);
void add_source(struct step *, char const *);

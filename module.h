/*
 * $Id: module.h,v 1.5 2000/11/11 01:59:24 james Exp $
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

void move_up(unsigned char *, unsigned long, unsigned long *);
unsigned char *build_object(unsigned char *, unsigned long, unsigned long);
unsigned long skip_object(unsigned char *, unsigned long, unsigned long);
unsigned long object_end(unsigned char *, unsigned long, unsigned long);
unsigned long skip_wsp(unsigned char *, unsigned long, unsigned long);
unsigned long skip_to_char(unsigned char *, unsigned long, unsigned long, unsigned char, int);
unsigned long end_of_line(unsigned char *, unsigned long, unsigned long);
unsigned long skip_line(unsigned char *, unsigned long, unsigned long);
void cleandir(char *);

#else /* BUILD_MAIN */

extern struct module **modules;

#endif /* BUILD_MAIN */

struct module *find_module(char *);
struct module *new_module(char *);
void scan_modules(char *);
void build_module(struct module *, unsigned int);
unsigned char *slurp_file(char *, unsigned long *);

/*
 * $Id: build.h,v 1.2 1999/09/07 13:49:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#ifndef BUILD_H
#define BUILD_H

#define PROGRAM "build-package"
#define VERSION "0.01"
#define COPYRIGHT "(c) James Aylett 1999"

/* Need to do this vile thing to get it to build. Ugh.
 * This is for: sys_nerr, sys_errlist, symlink()
 * I think that's it. No doubt this will go away when we autoconf-ise.
 */
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include "coopt.h"
#include "memory.h"

struct module
{
  char *name;
  char **options;
  unsigned int num_options;
  struct step **steps;
  unsigned int num_steps;
};

struct step
{
  enum { STEPskip, STEPbuild, STEPcopy, STEPmodule } type;
  char **sources; /* or module name(s) */
  unsigned int num_sources;
  /* COPY, BUILD only */
  char *as;
  char *into;
  char *from;
  char *using;
  /* BUILD only */
  char *in;
};

/* Are we supporting logging or not? */
#if 0
 #define flogf fprintf
#else
 #define flogf int_flogf
#endif

#endif /* BUILD_H */

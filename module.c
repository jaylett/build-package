/*
 * $Id: module.c,v 1.1 1999/08/10 15:26:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "module.h"

/*
 * We do this the horrible way: read build.parts into core, and then
 * frob it to get rid of continuations.
 *
 * This is done by having a central routine which fixes up the next line into
 * an appropriate format, and then returns a pointer to the start of it. Once
 * this is done, we can figure out what it actually meant.
 *
 * Remember that <source> can be :<file>.
 */
void scan_modules(void)
{
  /* FIXME: write this */
}

void build_module(struct module *mod)
{
  char *temp;
  int i;
  if (mod==NULL)
    return;
  /* run the build steps */
  for (i=0; i<mod->num_steps; i++)
    run_step(mod, mod->steps[i]);
  /* make the archive */
  printf("ought to run '%s' (%s/%s%s) in %s\n",
         read_option(mod, "archive"), read_option(mod, "archiveroot"),
         mod->name, read_option(mod, "archiveext"), tmptree);
  /* FIXME: run the 'archive' option followed by mod->name'archiveext'
   * and the complete contents of 'tmptree'.
   * chdir() to 'tmptree' first.
   */
}

struct module *find_module(char *name)
{
  int i;
  if (modules==NULL)
    return NULL;
  for (i=0; i<num_modules; i++)
  {
    if (strcmp(name, modules[i]->name)==0)
      return modules[i]; /* negative => += */
  }
  return NULL;
}

/*
 * name must not point to transient data
 */
struct module *new_module(char *name)
{
  struct module ** temp  = rememalloc(modules,
                                    (++num_modules) * sizeof(struct module *));
  struct module * t;

  if (temp==NULL)
    do_error("memory error");
  modules = temp;
  modules[num_modules+1] = memalloc(sizeof(struct module));
  t = modules[num_modules+1];
  if (t==NULL)
    do_error("memory error");
  t->name=name;
  t->options=NULL;
  t->num_options=0;
  t->steps=NULL;
  t->num_steps=0;
  return t;
}

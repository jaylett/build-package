/*
 * $Id: step.c,v 1.2 1999/09/07 13:49:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "step.h"

void run_step(struct module *mod, struct step *step)
{
  int i;
  switch (step->type)
  {
    case STEPskip:
      return;
    case STEPbuild:
    case STEPcopy: /* these are actually the same option */
      if (step->using!=NULL)
      {
        if (step->in!=NULL)
	{
          char *t = trans_path(read_option(mod, "sourceroot"), step->in);
          printf("changing to directory '%s'\n", t);
	  chdir(t);
          memfree(t);
	}
        printf("running build command '%s'\n", step->using);
        system(step->using);
	chdir(startdir);
      }
      for (i=0; i<step->num_sources; i++)
      {
        printf("copying source '%s'\n", step->sources[i]);
        copy_source(mod, step, step->sources[i]);
      }
      break;
    case STEPmodule:
      if (step->sources!=NULL)
      {
	for (i=0; i<step->num_sources; i++)
	{
	  printf("building module '%s'\n", step->sources[i]);
	  build_module(find_module(step->sources[i]));
	}
      }
      break;
  }
}

struct step *new_step()
{
  struct step *temp = memalloc(sizeof(struct step));
  temp->type = STEPskip;
  temp->sources = NULL;
  temp->num_sources = 0;
  temp->as = temp->into = temp->from = temp->using = temp->in = NULL;
  return temp;
}

void add_step(struct module *module, struct step *step)
{
  struct step **temp = memrealloc(module->steps,
                                  (++module->num_steps) * sizeof(struct step *));
  module->steps = temp;
  module->steps[module->num_steps-1] = step;
}

/* mirror directory structure and link all files
 * FROM [step->from/]source
 * TO [step->into/]as
 * OR [step->info/]source
 */
void copy_source(struct module *mod, struct step *step, char *source)
{
  char *from;
  char *to;
  char *srcroot = read_option(mod, "sourceroot");

  if (srcroot==NULL)
    fatal_error("sourceroot wasn't specified. Like, *anywhere*.");

  from = comb_path(srcroot, step->from, source);
  if (step->as!=NULL)
    to = comb_paths(tmptree, step->into, step->as);
  else
    to = comb_paths(tmptree, step->into, source);

  mirror_dir(from, to);

  memfree(srcroot);
  memfree(from);
  memfree(to);
}

void mirror_dir(char *from, char *to)
{
  /* 1. do the equivalent of 'mkdir -p <to>'
   * 2. read all the elements of 'source'. For each one:
   *    - if it's a directory, create an equivalent in 'dest', append to
   *      'source' and 'dest', and call myself recursively
   *    - if it's a file, symlink() it into 'dest'
   *    - other types of fs object should be faulted
   */
  DIR *dirn;
  struct dirent *ent;
  mkdirs(to);

  if ((dirn = opendir(from))==NULL)
    fatal_error("couldn't open dir %s for mirroring", from);

  while ((ent = readdir(dirn))!=NULL)
  {
    char *f, *t;
    struct stat s;
    f = makename(from, ent->d_name);
    if (stat(f, &s)!=0)
      fatal_error("stat on %s failed", f);
    if (S_ISREG(s.st_mode))
    {
      t = makename(to, ent->d_name);
      printf("found file %s in %s; symlinking\n", ent->d_name, from);
      if (symlink(f, t)!=0)
        fatal_error("failed to symlink %s to %s", f, t);
      memfree(t);
    }
    else if (S_ISDIR(s.st_mode))
    {
      t = makename(to, ent->d_name);
      printf("found directory %s in %s; mirroring\n", ent->d_name, from);
      mirror_dir(f, t);
      memfree(t);
    }
    else
      printf("!!! non-file, non-directory: SKIPPING !!!\n");
    memfree(f);
  }

  closedir(dirn);
}

void add_source(struct step *step, char const * source)
{
  add_string(&step->sources, &step->num_sources, source);
}

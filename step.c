/*
 * $Id: step.c,v 1.3 1999/09/16 16:54:15 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "step.h"

void run_step(struct module *mod, struct step *step)
{
  int i;
/*  do_error("** run_step(): from = %p", step->from);*/
  switch (step->type)
  {
    case STEPskip:
      return;
    case STEPbuild:
    case STEPcopy: /* these are actually the same option */
      if (step->using!=NULL)
      {
	int ret;
        do_error("\nrunning build command '%s'", step->using);
        if (step->in!=NULL)
	{
          char *t = trans_path(read_option(mod, "sourceroot"), step->in);
          do_error("  changing to directory '%s'", t);
	  if (chdir(t)!=0)
	    fatal_error("couldn't chdir(): errno = %i", errno);
          memfree(t);
	}
	if ((ret = system(step->using)) < 0)
	{
	  do_error("  build command '%s' returned %i, error code %i", step->using, ret, errno);
	}
	chdir(startdir);
      }
      for (i=0; i<step->num_sources; i++)
      {
	if (step->sources[i][0]==':')
	{
	  if (step->sources[i][1]==0)
	  {
	    /* ignore - this source list has already been looked up and
	     * added, line by line
	     */
	  }
	  else
	  {
	    unsigned char *block, *ptr;
	    unsigned long used;
	    char *srclist, *srcroot;
	    do_error("\ncopying source list '%s'", step->sources[i]+1);
	    srcroot = read_option(mod, "sourceroot");
	    srclist = comb_path(srcroot, step->from, step->sources[i]+1);
	    if ((block = slurp_file(srclist, &used))==NULL)
	      fatal_error("Couldn't load source list %s", step->sources[i]+1);
	    ptr = strtok(block, "\n");
	    while (ptr!=NULL && (ptr - block)<used)
	    {
/*	      do_error("found another at %i, maximum %i", ptr-block, used);*/
	      add_source(step, ptr);
	      ptr = strtok(NULL, "\n");
	    }
	    memfree(srcroot);
	    memfree(srclist);
	    memfree(block);
	    step->sources[i][1]=0; /* don't do this again */
	  }
	}
	else
	{
	  do_error("\ncopying source '%s'", step->sources[i]);
	  copy_source(mod, step, step->sources[i]);
	}
      }
      break;
    case STEPmodule:
      if (step->sources!=NULL)
      {
	for (i=0; i<step->num_sources; i++)
	{
	  do_error("\nbuilding module '%s'", step->sources[i]);
	  build_module(find_module(step->sources[i]));
	}
      }
      break;
  }
}

struct step *new_step()
{
  struct step *temp;
  temp = memalloc(sizeof(struct step));
  temp->type = STEPskip;
  temp->sources = NULL;
  temp->num_sources = 0;
  temp->as = temp->into = temp->from = temp->using = temp->in = NULL;
  return temp;
}

void add_step(struct module *module, struct step *step)
{
  struct step **temp;
  temp= memrealloc(module->steps,
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

/* Sometimes gets called on files, not on directories
 * Hence we have to be a little clever. To be honest, this is all
 * beginning to turn into a bit of a hack :-(
 */
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
  struct stat s;
  char *srcroot;
/*  do_error("  mirror_dir(%s, %s)", from, to);*/
  
  if (stat(from, &s)!=0)
    fatal_error("stat on %s failed", from);
  if (S_ISREG(s.st_mode))
  {
    /* link the files */
/*    do_error("  mirror_dir() called on file; symlinking");*/
    mkdirs_less_leaf(to);
    if (do_symlink(from, to)!=0)
      fatal_error("failed to symlink %s to %s", from, to);
  }
  else
  {
/*    do_error("  mirror_dir() called on directory; mirroring");*/
    /* mirror the directory */
    mkdirs(to);
    if ((dirn = opendir(from))==NULL)
      fatal_error("couldn't open dir %s for mirroring", from);
    while ((ent = readdir(dirn))!=NULL)
    {
      char *f, *t;
      f = makename(from, ent->d_name);
      if (stat(f, &s)!=0)
	fatal_error("stat on %s failed", f);
      if (S_ISREG(s.st_mode))
      {
	t = makename(to, ent->d_name);
/*	do_error("    found file %s in %s; symlinking", ent->d_name, from);*/
        if (do_symlink(f, t)!=0)
	  fatal_error("failed to symlink %s to %s", f, t);
	memfree(t);
      }
      else if (S_ISDIR(s.st_mode))
      {
	if(ent->d_name[0]!='.')
	{
	  t = makename(to, ent->d_name);
/*	  do_error("    found directory %s in %s; mirroring", ent->d_name, from);*/
	  mirror_dir(f, t);
	  memfree(t);
	}
/*        else
	  do_error("    found directory %s in %s; skipping (hidden directory)", ent->d_name, from);*/
      }
      else
	do_error("!!! non-file, non-directory: SKIPPING !!!");
      memfree(f);
    }

    closedir(dirn);
  }
}

int do_symlink(const char *old, const char *new)
{
  char *temp;
  int result;
  if (old[0]=='/')
    return symlink(old, new);
  temp = makename(startdir, (char *)old);
  result = symlink(temp, new);
  memfree(temp);
  if (result<0 && errno==EEXIST)
    result=0;
  return result;
}

void add_source(struct step *step, char const * source)
{
/*  do_error("add_source(%p, %s)", step, source);*/
  add_string(&step->sources, &step->num_sources, source);
}

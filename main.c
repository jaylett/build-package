/*
 * $Id: main.c,v 1.7 2001/02/11 15:41:30 james Exp $
 * build-package
 * (c) Copyright James Aylett
 *
 * build-package is released under the GPL.
 */

#include "main.h"

int main(int argc, char const * const * argv)
{
  char *controlfile=NULL;
  char *temp;
  char **build_modules = NULL;
  unsigned int num_build_modules = 0;
  struct coopt_return ret;
  struct coopt_state state;
  struct coopt_option option[] =
  {
    { 'f', COOPT_REQUIRED_PARAM, "file" },
    { 'v', COOPT_NO_PARAM, "version" },
    { 'h', COOPT_NO_PARAM, "help" },
    { 'C', COOPT_REQUIRED_PARAM, "directory" },
    { 'i', COOPT_NO_PARAM, "ignore-cvs" },
  };

  tmptree = memalloc(256 + sizeof(RMCOMMAND));
  strcpy(tmptree, RMCOMMAND);
  tmptree += sizeof(RMCOMMAND)-1;
  cli = new_module("__cli__");

  coopt_init(&state, (struct coopt_option const *)&option,
             sizeof(option) / sizeof(struct coopt_option),
             argc-1, argv+1);

  do
  {
    ret = coopt(&state);
    if (!coopt_is_error(ret.result))
    {
      if (ret.opt!=NULL)
      {
        switch (ret.opt->short_option)
	{
	  case 'C':
	    startdir = strdup(ret.param);
	    break;
          case 'f':
            controlfile = strdup(ret.param);
            break;
          case 'i':
            ignore_cvs = TRUE;
            break;
          case 'h':
            do_help();
            exit(0);
          case 'v':
            do_version();
            exit(0);
	}
      }
      else if (ret.param!=NULL)
      {
        char *ptr = strchr(ret.param, '=');
	if (ptr!=NULL)
	{
          if (ptr > ret.param && ptr[-1]=='+')
            fatal_error("can't use += for options on the command line");
          else
	    add_option(cli, ret.param);
	}
        else
	{
          add_string(&build_modules, &num_build_modules, ret.param);
	}
      }
    }
  } while (coopt_is_okay(ret.result));

  if (coopt_is_error(ret.result))
  {
    char err[255];
    coopt_serror(err, 255, &ret, &state);
    fatal_error("fatal error in options parsing: %s", err);
  }

  if (controlfile==NULL)
    controlfile=strdup("./build.parts");

  scan_modules(controlfile);

  /*
   * Set 'startdir' to the directory containing build.parts (this isn't
   * terribly normal, but OTOH I think it's quite a useful way of doing
   * things. I imagine that this behaviour will disappear at some point,
   * however).
   */
  if (startdir==NULL)
  {
    int i = strlen(controlfile);
    do
    {
      i--;
    } while (controlfile[i]!='/' && i>0);
    if (i==0)
      startdir = do_getcwd();
    else
    {
      controlfile[i]=0; /* terminate! */
      if (controlfile[i]=='/')
	startdir = controlfile; /* absolute */
      else
      {
        /* relative */
	char *cwd = do_getcwd();
        startdir = makename(cwd, controlfile);
	memfree(cwd);
      }
    }
  }
/*  do_error("chdir(%s)", startdir);*/
  if (chdir(startdir)!=0)
    fatal_error("couldn't chdir: errno = %i", errno);

  temp = read_option(NULL, "temproot");
  sprintf(tmptree, "%s/%s.%i", (temp==NULL)?("/tmp"):(temp), PROGRAM, (int)getpid());
  memfree(temp);
/*  do_error("tmptree = %s", tmptree);*/
  if (mkdir(tmptree, 0775)!=0)
  {
    if (errno>=sys_nerr || sys_errlist[errno]==NULL)
      fatal_error("couldn't create build directory (errno = %i)", errno);
    else
      fatal_error("couldn't create build directory (err = %s)", sys_errlist[errno]);
  }

  if (num_build_modules==0 || build_modules==NULL)
  {
    char *mods, *tok;
    mods = read_option(NULL, "all");
    /* okay, so 'mods' has been created by memalloc(), so we can use strtok()
     * (eeek!)
     */
    if (mods==NULL)
      fatal_error("Couldn't read option 'all', and no modules supplied on command line.");
    tok = strtok(mods, " \t");
    while (tok!=NULL)
    {
/*      do_error("adding module to build %s", tok);*/
      add_string(&build_modules, &num_build_modules, tok);
      tok = strtok(NULL, " \t");
    }
    memfree(mods);
  }

  if (num_build_modules>0 && build_modules!=NULL)
  {
    int i;
    for (i=0; i<num_build_modules; i++) {
      do_error("\nStarting top-level module '%s'", build_modules[i]);
      build_module(find_module(build_modules[i]), TRUE);
      do_error("Done top-level module '%s'\n", build_modules[i]);
    }
  }

  return system(tmptree - sizeof(RMCOMMAND) +1);
  /* ie perform "rm -rf <tmptree>" */
}

char *do_getcwd()
{
  char *buf=NULL, *res=NULL;
  int len = 0;

  do
  {
    len+=PATH_MAX;
    buf = (char *)memrealloc((void *)buf, len);
    res = getcwd(buf, len);
  } while (res==NULL);
  return res;
}

void do_help()
{
  do_version();
  printf (
"usage: %s [options] [module] ...\n"
"\n"
"  -f FILE, --file=FILE    read FILE as a build.parts file\n"
"  -C DIRECTORY, --directory=DIRECTORY\n"
"                          root at DIRECTORY instead of parent of build.parts\n"
"  -i, --ignore-cvs        skip CVS dirs, .cvsignore files\n"
"\n"
"  -h, --help              display this help message and quit\n"
"  -V, --version           display version string and quit\n"
    , PROGRAM);
}

void do_version()
{
  printf("%s v%s %s\n", PROGRAM, VERSION, COPYRIGHT);
}

void fatal_error(char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  putc('\n', stderr);
  va_end(ap);
  exit(EXIT_FAILURE);
}

void do_error(char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  putc('\n', stderr);
  va_end(ap);
}

/* This should get optimised out ... */
void int_flogf(FILE *out, ...)
{
}

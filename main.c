/*
 * $Id: main.c,v 1.2 1999/09/07 13:49:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
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
    { 'h', COOPT_NO_PARAM, "help" }
  };

  tmptree = memalloc(256 + sizeof(RMCOMMAND));
  strcpy(tmptree, RMCOMMAND);
  tmptree += sizeof(RMCOMMAND);
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
          case 'f':
            controlfile = strdup(ret.param);
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
   *
   * Eeek! The following isn't portable - relies on GNU extension
   * to getcwd().
   */
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
  chdir(startdir);

  temp = read_option(NULL, "temproot");
  sprintf(tmptree, "%s/%s.%i", (temp==NULL)?("/tmp"):(temp), PROGRAM, (int)getpid());
  memfree(temp);
  do_error("tmptree = %s", tmptree);
  if ((deltree = mkdir(tmptree, 0775))==-1)
  {
    if (errno>=sys_nerr || sys_errlist[errno]==NULL)
      fatal_error("couldn't create build directory (errno = %i)", errno);
    else
      fatal_error("couldn't create build directory (err = %s)", sys_errlist[errno]);
  }

  if (num_build_modules>0 && build_modules!=NULL)
  {
    int i;
    for (i=0; i<num_build_modules; i++)
      build_module(find_module(build_modules[i]));
  }
  else
  {
    char *mods, *tok;
    mods = read_option(NULL, "all");
    /* okay, so 'mods' has been created by memalloc(), so we can use strtok()
     * (eeek!)
     */
    if (mods==NULL)
      fatal_error("Couldn't read option all.");
    tok = strtok(mods, " \t");
    while (tok!=NULL)
    {
      build_module(find_module(tok));
      tok = strtok(NULL, " \t");
    }
    memfree(mods);
  }

  return system(tmptree - sizeof(RMCOMMAND)); /* ie perform "rm -rf <tmptree>" */
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
  /* FIXME: something proper here */
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

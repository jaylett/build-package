/*
 * $Id: main.c,v 1.1 1999/08/10 15:26:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "main.h"

int main(int argc, char const * const * argv)
{
  char *controlfile;
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
             argc, argv);

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
            controlfile = (char *)ret.param;
            break;
          case 'h':
            do_help();
            exit;
          case 'v':
            do_version();
            exit;
	}
      }
      else
      {
        /* some sort of option thingumy */
        char *ptr = strchr(ret.param, '=');
	if (ptr!=NULL)
	{
          if (ptr[-1]=='+')
            do_error("can't use += for options on the command line");
          else
	    add_option(cli, ret.param);
	}
        else
          add_string(&build_modules, &num_build_modules, ret.param);
      }
    }
  } while (coopt_is_okay(ret.result));

  if (coopt_is_error(ret.result))
    do_error("fatal error in options parsing: %s", coopt_parse_error(ret));

  if (controlfile==NULL)
    controlfile="./build.parts";

  scan_modules();

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
      startdir = getcwd(NULL, -1);
    else
    {
      controlfile[i]=0; /* terminate! */
      if (controlfile[i]=='/')
	startdir = controlfile; /* absolute */
      else
        startdir = makename(getcwd(NULL, -1), controlfile);
        /* relative */
    }
  }
  chdir(startdir);

  temp = read_option(NULL, "temproot");
  sprintf(tmptree, "%s/%s.%i", (temp==NULL)?("/tmp"):(temp), argv[0], (int)getpid());
  memfree(temp);
  if ((deltree = mkdir(tmptree, 0775))==-1)
  {
    if (errno>=sys_nerr || sys_errlist[errno]==NULL)
      do_error("couldn't create build directory (errno = %i)", errno);
    else
      do_error("couldn't create build directory (err = %s)", sys_errlist[errno]);
  }

  if (num_build_modules>0 && build_modules!=NULL)
  {
    int i;
    for (i=0; i<num_build_modules; i++)
      build_module(find_module(build_modules[i]));
  }
  else
  {
    char *mods = read_option(NULL, "all");
    /* okay, so 'mods' has been created by memalloc(), so we can use strtok()
     * (eeek!)
     */
    char *tok = strtok(mods, " \t");
    while (tok!=NULL)
    {
      build_module(find_module(tok));
      tok = strtok(NULL, " \t");
    }
    memfree(mods);
  }

  return system(tmptree - sizeof(RMCOMMAND)); /* ie perform "rm -rf <tmptree>" */
}

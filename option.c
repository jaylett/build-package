/*
 * $Id: option.c,v 1.1 1999/08/10 15:26:58 james Exp $
 * build-package
 * (c) Copyright James Aylett 1999
 *
 * build-package is released under the GPL.
 */

#include "option.h"

/*
 * returns a block that has been memalloc()'ed
 */
char *read_option(struct module *am, char *name)
{
  int ao=0, bo=0, co=0;
  char *as=NULL, *bs=NULL, *cs=NULL;
  struct module *bm=NULL;
  char *result=NULL;
  int size;

  if (am!=NULL)
    ao = find_option(am, name);
  if ((bm = find_module("global"))!=NULL)
    bo = find_option(bm, name);
  if (cli!=NULL)
    co = find_option(cli, name);

  if (ao>0)
  {
    bo=0;
    co=0;
  }
  if (bo>0)
    co=0;

  if (ao==0 && bo==0 && co==0)
    return NULL; /* couldn't find it */

  as = get_option(am, ao);
  bs = get_option(bm, bo);
  cs = get_option(cli, co);

  size = (as==NULL)?(0):(strlen(as)) +
         (bs==NULL)?(0):(strlen(bs)) +
         (cs==NULL)?(0):(strlen(cs)) + 1;
  result = memalloc(size);
  if (result==NULL)
    do_error("memory error");
  result[0]=0;
  if (cs!=NULL)
    strcat(result, cs);
  if (bs!=NULL)
    strcat(result, bs);
  if (as!=NULL)
    strcat(result, as);
  return result;
}

int find_option(struct module *module, char *name)
{
  int i;
  if (module==NULL)
    return 0;

  for (i=0; i<module->num_options; i++)
  {
    int j=1;
    char *ptr = strchr(module->options[i], '=');
    if (ptr[-1]=='+')
      j=-1;
    if (strncmp(name, module->options[i], ptr - module->options[i] + (j==-1)?(-1):(0))==0)
      return j*(i+1); /* negative => += */
  }
  return 0;
}

char *get_option(struct module *module, int opt)
{
  char *ptr;
  if (opt==0)
    return NULL;
  if (opt<0)
    opt *= -1;
  ptr = strchr(module->options[opt-1], '=');
  if (ptr==NULL)
    do_error("internal fuckwittage");
  return ptr+1;
}

void add_string(char ***opt, unsigned int *num, char const * option)
{
  char ** temp = rememalloc(*opt, (++*num) * sizeof(char *));
  if (temp==NULL)
    do_error("memory error");
  *opt = temp;
  temp[*num-1] = memalloc((strlen(option)+1) * sizeof(char));
  if (temp[*num-1]==NULL)
    do_error("memory error");
  strcpy(temp[*num-1], option);
}

void add_source(struct step *step, char const * source)
{
  add_string(&step->sources, &step->num_sources, source);
}

void add_option(struct module *module, char const * option)
{
  add_string(&module->options, &module->num_options, option);
}

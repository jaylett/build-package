/*
 * $Id: module.c,v 1.3 1999/09/16 16:54:14 james Exp $
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
 *
 * NULs in the files will confuse things lots, so don't feed it that.
 */
void scan_modules(char *file)
{
  unsigned long current, used;
  unsigned char *block;
  struct module *module=NULL;

  if ((block = slurp_file(file, &used))==NULL)
    fatal_error("Couldn't load file %s", file);

  /* Should have loaded successfully into 'block', of length 'used'
   * Basically, we could through in a state machine, adding relevant stuff
   * as needed.
   * There is guaranteed to be space to tromp on block[used] (which is
   * initialised to NUL, but will probably be overwritten again at some point).
   */
  current=0; /* where are we? */

  while (current < used)
  {
    unsigned long line_end, next_line, line_start;

    next_line = skip_line(block, current, used);
    line_end = end_of_line(block, current, used);
    line_start = current;
    block[line_end]=0;
    current = skip_wsp(block, current, used);
    if (current<line_end)
    {
      flogf(stderr, "Processing line: from %i to %i (next line starts at %i). Starts '%c'.\n", current, line_end, next_line, block[current]);
      if (block[current]=='[')
      {
        /* New module */
        unsigned long end = skip_to_char(block, current, used, ']', 0);
        if (end==line_end)
          fatal_error("Unterminated [...] module name.");
        block[end] = 0;
        module = new_module(block+current+1);
	assert(module!=NULL);
      }
      else
      {
        unsigned long end;

        if (module==NULL)
          fatal_error("Step / option line outside module.");

        /* current is start of line, line_end is NUL-terminated end of useful bits.
         * No LFs that aren't in strings.
         */
        end = skip_to_char(block, current, used, '=', 1);
        if (block[end]=='=')
        {
          /* option */
	  do_error("Processing line: found option <%s>.", block+current);
          add_option(module, block+current);
        }
        else
        {
          /* step */
          struct step *step = new_step();
	  unsigned long next_object = skip_object(block, current, used);
          block[end]=0;
	  do_error("Processing line: found step <%s>.", block+current);
          if (strcmp(block+current, "COPY")==0)
          {
            step->type = STEPcopy;
          }
          else if (strcmp(block+current, "BUILD")==0)
          {
            step->type = STEPbuild;
          }
          else if (strcmp(block+current, "MODULE")==0)
          {
            step->type = STEPmodule;
          }
          else
          {
            do_error("Unknown step type %s (removing step).", block+current);
          }

          if (step->type != STEPskip)
          {
            enum { SOURCES, AS, INTO, FROM, USING, IN, COMMAND } state = SOURCES;
            current = next_object;

            while (current < line_end)
            {
              next_object = skip_object(block, current, used);
              end = object_end(block, current, used);
              block[end]=0;
/*	      do_error("  Processing subline: found option <%s>.", block+current);*/
              /* object runs from current to end, may be quoted, in NUL-terminated */
              switch (state)
              {
                case COMMAND:
                case SOURCES:
                  if (strcmp(block+current, "AS")==0)
                  {
                    state = AS;
                  }
                  else if (strcmp(block+current, "INTO")==0)
                  {
                    state = INTO;
                  }
                  else if (strcmp(block+current, "FROM")==0)
                  {
                    state = FROM;
                  }
                  else if (strcmp(block+current, "USING")==0)
                  {
                    state = USING;
                  }
                  else if (strcmp(block+current, "IN")==0)
                  {
		    if (step->type == STEPbuild)
		      state = IN;
		    else
		      fatal_error("IN can only be used for BUILD steps.");
                  }
                  else
                  {
                    if (state==SOURCES)
                      add_source(step, build_object(block, current, end));
                    else
                      do_error("Erroneous source '%s' after commands started.",
                               build_object(block, current, end));
                  }
                  break;
                case AS:
                  step->as = build_object(block, current, end);
/*		  do_error("  as = %s", step->as);*/
                  state=COMMAND;
                  break;
                case INTO:
                  step->into = build_object(block, current, end);
/*		  do_error("  into = %s", step->into);*/
                  state=COMMAND;
                  break;
                case FROM:
                  step->from = build_object(block, current, end);
/*		  do_error("  from = %p", step->from);*/
                  state=COMMAND;
                  break;
                case USING:
                  step->using = build_object(block, current, end);
/*		  do_error("  using = %s", step->using);*/
                  state=COMMAND;
                  break;
                case IN:
                  step->in = build_object(block, current, end);
/*		  do_error("  in = %s", step->in);*/
                  state=COMMAND;
                  break;
              }
              current = next_object;
            }
            add_step(module, step);
          }
        }
      }
    }
    current = next_line;
  }
  /* don't free block - we're using it throughout the entire system! */
}

/* Slurps the file into core. *used_ptr is filled with the length of data.
 * Immediately after the data will be a NUL. You need to free up the returned
 * block. Read in as textual data, because this is a textual file ... :-)
 *
 * Remember that fatal_error() calls exit(), which will close our open filehandles.
 */
unsigned char *slurp_file(char *file, unsigned long *used_ptr)
{
  unsigned long current, used;
  long length;
  unsigned char *block;
  FILE *fp;
  
  fp = fopen(file, "r");
  if (fp==NULL)
    return NULL;

#ifndef NO_FSEEK
  fseek(fp,0,SEEK_END);
  length=ftell(fp);
  fseek(fp,0,SEEK_SET);
  if (length<=0)
#endif
    length=20*1024;
  /* length is a suggestion as to the page size to use */

  block = (unsigned char *)memalloc((size_t)length);

  current = (unsigned long)length;
  used=0;
  while (!feof(fp) && !ferror(fp))
  {
    size_t t;
    if (used+length > current)
    {
      block = (unsigned char *)memrealloc((void *)block, (size_t)(current+(unsigned long)length));
      current+=(unsigned long)length;
    }
    t = fread((void *)(block+used), 1, (size_t)length, fp);
    used += t;
  }
  if (ferror(fp))
    fatal_error("Error while reading file %s.", file);
  fclose(fp);
  if (used >= current)
  {
    block = (unsigned char *)memrealloc((void *)block, (size_t)(current+1));
    current+=(unsigned long)length;
    block[used]=0; /* terminate with a NUL (don't increment used) */
  }

  *used_ptr = used;
  return block;
}

/*
 * Returns a pointer to the object, all quoting removed, and NUL-terminated.
 * Whenever we find a quote (" or ') we remove it and go into an appropriate quoting mode.
 * Within that mode, we unquote (/<character>), and remove the matching end quotes.
 * Also remove
 * This corrupts the buffer (but who cares?).
 */
unsigned char *build_object(unsigned char *block, unsigned long start, unsigned long end)
{
  unsigned long current = start;
  enum { NORMAL, SINGLE_QUOTED, DOUBLE_QUOTED, STOP } state = NORMAL;
  flogf(stderr, "Build object: <");
  while (current<end && state!=STOP)
  {
    switch (block[current])
    {
      case '\'':
        if (state==NORMAL)
          state=SINGLE_QUOTED;
        else if (state==SINGLE_QUOTED)
          state=NORMAL;
        if (state==NORMAL || state==SINGLE_QUOTED)
          move_up(block, current, &end);
        break;
      case '"':
        if (state==NORMAL)
          state=DOUBLE_QUOTED;
        else if (state==DOUBLE_QUOTED)
          state=NORMAL;
        if (state==NORMAL || state==DOUBLE_QUOTED)
          move_up(block, current, &end);
        break;
      case '\\':
        if (current+1<end && (state==DOUBLE_QUOTED || state==SINGLE_QUOTED))
        {
          move_up(block, current, &end);
          current++;
        }
        break;
      default:
        flogf(stderr, "%c", block[current]);
        current++;
        break;
    }
  }
  flogf(stderr, ">\n");
  return block+start;
}

/*
 * Moves the block (start+1, *end) to (start, *end-1) and decrements *end
 * This isn't very efficient, but there's no standard function to do a fast
 * move properly ...
 */
void move_up(unsigned char *block, unsigned long start, unsigned long *end)
{
  unsigned long current;
  unsigned long real_end;

  real_end = *end;

  for (current=start+1; current<real_end; current++)
  {
/*    do_error("move_up(): moving %c back one (overwriting %c", block[current], block[current-1]);*/
    block[current-1] = block[current];
  }

/*  do_error("move_up(): replacing %c with \0", block[current-1]);*/
  block[current-1]=0;
  *end = real_end-1;
}

/*
 * returns the offset increased to skip the object. Points to first element of whitespace.
 * Also stops at comments, LF.
 */
unsigned long object_end(unsigned char *block, unsigned long current, unsigned long used)
{
  enum { NORMAL, SINGLE_QUOTED, DOUBLE_QUOTED, STOP } state = NORMAL;
  while (current<used && state!=STOP)
  {
    switch (block[current])
    {
      case 0:
      case '\n':
      case '#':
        if (state==NORMAL)
          state=STOP;
        break;
      case '\'':
        if (state==NORMAL)
          state=SINGLE_QUOTED;
        else if (state==SINGLE_QUOTED)
          state=NORMAL;
        break;
      case '"':
        if (state==NORMAL)
          state=DOUBLE_QUOTED;
        else if (state==DOUBLE_QUOTED)
          state=NORMAL;
        break;
      case 32: case 9:
        if (state==NORMAL)
          state=STOP;
        break;
      case '\\':
        if (current+1<used && (state==DOUBLE_QUOTED || state==SINGLE_QUOTED))
        {
          current++;
        }
        break;
    }
    if (state!=STOP)
      current++;
  }
  return current;
}

/*
 * returns the offset increased to skip the object, and trailing whitespace.
 * Also stops at comments, LF.
 */
unsigned long skip_object(unsigned char *block, unsigned long current, unsigned long used)
{
  enum { NORMAL, SINGLE_QUOTED, DOUBLE_QUOTED, WSP, STOP } state = NORMAL;
  while (current<used && state!=STOP)
  {
    switch (block[current])
    {
      case '\n':
      case '#':
        if (state==NORMAL || state==WSP)
          state=STOP;
        break;
      case '\'':
        if (state==NORMAL)
          state=SINGLE_QUOTED;
        else if (state==SINGLE_QUOTED)
          state=NORMAL;
        else if (state==WSP)
          state=STOP;
        break;
      case '"':
        if (state==NORMAL)
          state=DOUBLE_QUOTED;
        else if (state==DOUBLE_QUOTED)
          state=NORMAL;
        else if (state==WSP)
          state=STOP;
        break;
      case 32: case 9:
        if (state==NORMAL)
          state=WSP;
        break;
      case '\\':
        if (current+1<used && (state==DOUBLE_QUOTED || state==SINGLE_QUOTED))
        {
          current++;
        }
        else if (state==WSP)
        {
	  state=STOP;
        }
        break;
       default:
        if (state==WSP)
	  state=STOP;
        break;
    }
    if (state!=STOP)
      current++;
  }
  return current;
}

/*
 * returns the offset increased to find the character, outside strings
 * Also stops at comments, LF.
 * If f is non-zero, also stops at whitespace.
 */
unsigned long skip_to_char(unsigned char *block, unsigned long current,
                           unsigned long used, unsigned char c, int f)
{
  enum { NORMAL, SINGLE_QUOTED, DOUBLE_QUOTED, STOP } state = NORMAL;
  while (current<used && state!=STOP)
  {
    switch (block[current])
    {
      case '\n':
      case '#':
        if (state==NORMAL)
          state=STOP;
        break;
      case '\'':
        if (state==NORMAL)
          state=SINGLE_QUOTED;
        else if (state==SINGLE_QUOTED)
          state=NORMAL;
        break;
      case '"':
        if (state==NORMAL)
          state=DOUBLE_QUOTED;
        else if (state==DOUBLE_QUOTED)
          state=NORMAL;
        break;
      case 32: case 9:
        if (state==NORMAL && f)
          state=STOP;
        break;
      case '\\':
        if (current+1<used && (state==DOUBLE_QUOTED || state==SINGLE_QUOTED))
        {
          current++;
        }
        break;
      default:
        if (block[current]==c && state==NORMAL)
          state=STOP;
        break;
    }
    if (state!=STOP)
      current++;
  }
  return current;
}

/*
 * returns the offset increased to skip whitespace, but not beyond the end of the line
 */
unsigned long skip_wsp(unsigned char *block, unsigned long current, unsigned long used)
{
  while (current<used && (block[current]==9 || block[current]==32))
    current++;
  return current;
}

/*
 * returns the offset increased to the start of the next line
 */
unsigned long skip_line(unsigned char *block, unsigned long current, unsigned long used)
{
  enum { NORMAL, SINGLE_QUOTED, DOUBLE_QUOTED, COMMENT, LFS, STOP } state = NORMAL;
  flogf(stderr, "Scanning line:   <");
  while (current<used && state!=STOP)
  {
    if (state==LFS)
    {
      if (block[current]!='\n')
	state=STOP;
      else
	flogf(stderr, "Skipped blank line.\n");
    }
    else
    {
      if (block[current]!='\n' || (state!=NORMAL && state!=COMMENT))
	flogf(stderr, "%c", block[current]);
      switch (block[current])
      {
       case '#':
        if (state==NORMAL)
          state=COMMENT;
        break;
       case '\n':
        if (state==NORMAL || state==COMMENT)
	{
          state=LFS;
	  flogf(stderr, ">\n");
	}
        break;
       case '\'':
        if (state==NORMAL)
          state=SINGLE_QUOTED;
        else if (state==SINGLE_QUOTED)
          state=NORMAL;
        break;
       case '"':
        if (state==NORMAL)
          state=DOUBLE_QUOTED;
        else if (state==DOUBLE_QUOTED)
          state=NORMAL;
        break;
       case '\\':
        if (current+1<used)
	{
          if (state==NORMAL && block[current+1]=='\n')
          {
	    current++;
	    flogf(stderr, "  ");
          }
	  else if (state==DOUBLE_QUOTED || state==SINGLE_QUOTED)
	  {
	    current++;
	    flogf(stderr, "%c",block[current]);
	  }
	}
        break;
      }
    }
    if (state!=STOP)
      current++;
  }
  return current;
}

/*
 * returns the offset increased to either the start of a comment,
 * or the end of a line. Within single- and double-quoted strings,
 * NOTHING stops the string except an un-quoted single or double
 * quote respectively. Quoting within strings uses '\' in the
 * obvious way.
 * A single '\' at the end of a line (must be the last thing - can't have
 * comments or anything beyond it) will cause the following line to be
 * wrapped into this one. This is done by replacing this and the subsequent
 * LF with SPC. This doesn't apply during quotes (because the LF will effectively
 * be quoted), but then the line wouldn't end even without the '\'.
 *
 * Call this before calling anything else, because it takes care of line continuations.
 */
unsigned long end_of_line(unsigned char *block, unsigned long current, unsigned long used)
{
  enum { NORMAL, SINGLE_QUOTED, DOUBLE_QUOTED, STOP } state = NORMAL;
  flogf(stderr, "Processing line: <");
  while (current<used && state!=STOP)
  {
    switch (block[current])
    {
      case '\n':
      case '#':
        if (state==NORMAL)
          state=STOP;
        break;
      case '\'':
        if (state==NORMAL)
          state=SINGLE_QUOTED;
        else if (state==SINGLE_QUOTED)
          state=NORMAL;
        flogf(stderr, "'");
        break;
      case '"':
        if (state==NORMAL)
          state=DOUBLE_QUOTED;
        else if (state==DOUBLE_QUOTED)
          state=NORMAL;
        flogf(stderr, "\"");
        break;
      case '\\':
        if (current+1<used)
        {
          if (state==NORMAL && block[current+1]=='\n')
          {
            block[current]=32;
            block[current+1]=32;
	    flogf(stderr, " ");
          }
          else if (state==DOUBLE_QUOTED || state==SINGLE_QUOTED)
          {
            current++;
	    flogf(stderr, "\\%c",block[current]);
          }
        }
        break;
      default:
        flogf(stderr, "%c", block[current]);
        break;
    }
    if (state!=STOP)
      current++;
  }
  flogf(stderr, ">\n");
  return current;
}

void build_module(struct module *mod)
{
  char *temp;
  int i;
/*  do_error("build_module(%p)", mod);*/
  if (mod==NULL || mod->done==1)
  {
/*    do_error("doesn't exist, or already done");*/
    return;
  }
  /* run the build steps */
  for (i=0; i<mod->num_steps; i++)
    run_step(mod, mod->steps[i]);
  /* make the archive */
  do_error("ought to run '%s' (%s/%s%s) in %s\n",
	   read_option(mod, "archive"), read_option(mod, "archiveroot"),
	   mod->name, read_option(mod, "archiveext"), tmptree);
  /* FIXME: run the 'archive' option followed by mod->name'archiveext'
   * and the complete contents of 'tmptree'.
   * chdir() to 'tmptree' first.
   */
  mod->done=1;
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
  struct module ** temp  = memrealloc(modules,
                                      (++num_modules) * sizeof(struct module *));
  struct module * t;
  
  flogf(stderr, "New module:      <%s>\n", name);

  modules = temp;
  modules[num_modules-1] = memalloc(sizeof(struct module));
  t = modules[num_modules-1];
  t->name=name;
  t->options=NULL;
  t->num_options=0;
  t->steps=NULL;
  t->num_steps=0;
  t->done=0;
  return t;
}

/***********************************************************************
                          arsparser.c  -  description
                          ---------------------------
    begin                : Wed Apr 17 2001
    update               : Sat Aug 28 2004
    copyright            : (C) 2001-2004 by Georg P. Loczewski
    email                : gpl@lambda-bound.com
 **********************************************************************/

/***********************************************************************
*                                                                      *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
***********************************************************************/
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include <setjmp.h>

# include "arsc.h"

extern ENV *globalEnv;
extern EXP *trueSym;
extern NAME lambda;

static void printPrimaryPrompt ();
static void printSecondaryPrompt ();
static void primeBuffer ();
static int isSeparator (int);
static void skipSpaces ();
static void skipNewlines ();
static NLIST *readNL_aux ();
static NLIST *readNL ();
static ELIST *readEL ();
static EXP *readEXP ();
static VALUE *readVAL ();
static VALUE *readINTV ();
static VALUE *readSYMV ();
static NAME readSymbol ();
static int readInteger ();
static NAME readString ();

static char buffer[1000];
static char *current;		// current buffer pointer
static FILE *ins;		// input stream

extern jmp_buf _EXH;

/* initialize ars parser */
void
parserinit (FILE * is)
{
  ins = is;
}


/* print prompt to enter expressions */
void
printPrimaryPrompt ()
{
  printf ("\nARS-Eval-> ");
  fflush (stdout);
}

/* print prompt to continue expressions */
void
printSecondaryPrompt ()
{
  printf ("> ");
  fflush (stdout);
}

/* fill the input buffer */
void
primeBuffer ()
{
  char *p;

  p = fgets (buffer, 1000, ins);
  if (p == NULL) {		// eof return by fgets
    if (ins == stdin) {
      strcpy (buffer, "(quit) ");
    }
    else {
      strcpy (buffer, "\"*eof*\" ");
    }
    p = buffer;
  }

  // initialize the current pointer
  *(p + strlen (p) - 1) = '\0';	// erase the newline character
  current = buffer;
  skipSpaces ();
}

/* test for separator between tokens */
int
isSeparator (int c)
{
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
  case EOF:
  case '\0':
  case '\'':
  case ';':
  case ')':
  case '(':
    return 1;
  }
  return 0;
}

/* ignore whithe space and comments */
void
skipSpaces ()
{
  while ((*current == ' ') || (*current == '\t'))
    current++;
  if (*current == ';') {	// ignore comments
    while (*current)
      current++;
  }
}

/* eliminate newline characters and prompt for continuation */
void
skipNewlines ()
{
  skipSpaces ();
  while (*current == '\0') {	// end of line
    printSecondaryPrompt ();	// print prompt as long as buffer is
    primeBuffer ();		// empty
  }
}

/* activate the parser in interactive mode */
EXP *
readWithPrompt ()
{
  EXP *exp;

  do {
    printPrimaryPrompt ();
    primeBuffer ();
  } while (*current == '\0');

  exp = readEXP ();

  skipSpaces ();
  if (*current) {
    printf ("unexpected characters at end of line: %c\n", *current);
    longjmp (_EXH, 2);
  }
  return exp;
}

/* activate the parser in background  mode */
EXP *
readWithoutPrompt ()
{
  EXP *exp;

  do {
    primeBuffer ();
  } while (*current == '\0');

  exp = readEXP ();

  // make sure we are at and of line
  skipSpaces ();
  if (*current) {
    printf ("unexpected characters at end of line: %c\n", *current);
    longjmp (_EXH, 2);
  }
  return exp;
}

/* read and parse an ars expression */
EXP *
readEXP ()
{
  EXP *etemp;
  NAME nm;
  NLIST *nl;
  ELIST *el;

  if (*current == '(') {
    current++;
    etemp = readEXP ();
    if ((etemp->etype == RXP) && (etemp->exp.ref == lambda)) {
      nl = readNL ();
      el = readEL ();
      return newAXP (nl, el);
    }
    else {
      return newSXP (etemp, readEL ());
    }
  }
  else if (*current == '"') {
    current++;
    return newVXP (readVAL ());
  }
  else if (*current == '\'') {
    current++;
    return newVXP (readSYMV());
  }
  else if (isdigit(*current)) {
    return newVXP (readINTV());
  }
  else {
    nm = readSymbol ();
    return newRXP (nm);
  }
}

/* read and parse a list of expressions */
ELIST *
readEL ()
{
  EXP *etemp;

  skipNewlines ();

  if (*current == ')') {
    current++;
    return NULL;
  }
  etemp = readEXP ();
  return econs (etemp, readEL ());
}

/* read and parse a list of names */
NLIST *
readNL ()
{
  NLIST *nl;

  skipNewlines ();
  if (*current == '(') {
    current++;
    nl = readNL_aux ();
    return nl;
  }
  else {
    printf ("'(' expected: got %c\n", *current);
    longjmp (_EXH, 2);
  }
}

/* auxiliary function for readNL */
NLIST *
readNL_aux ()
{
  NAME nm;

  skipNewlines ();

  if (*current == ')') {
    current++;
    return NULL;
  }
  nm = readSymbol ();
  return ncons (nm, readNL_aux ());
}

/* read a value (in ars values in expression are always strings */
VALUE *
readVAL ()
{
  return newSTRV (readString ());
}

/* read a symbol value */
VALUE *
readSYMV ()
{
  return newSYMV (readSymbol ());
}

/* read an integer number */
VALUE *
readINTV ()
{
  return newINTV (readInteger ());
}

/* read and parse the name of a variable */
int
readInteger ()
{
  char strbuf[40], *p;

  for (p = strbuf; isdigit (*current); *p++ = *current++);
  *p = '\0';
  return atoi(strbuf);
}

/* read and parse the name of a variable */
NAME
readSymbol ()
{
  char strbuf[40], *p;

  for (p = strbuf; !isSeparator (*current); *p++ = *current++);
  *p = '\0';
  return defName (strdup (strbuf));
}

/* read and parse a string (identified by double quotes) */
NAME
readString ()
{
  char strbuf[80], *p;

  for (p = strbuf; !(*current == '"'); *p++ = *current++);
  current++;
  *p = '\0';
  return defName (strdup (strbuf));
}

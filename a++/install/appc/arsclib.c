/***********************************************************************
                          arsc.c  -  description
                          ----------------------
    begin                : Wed Apr 17 2001
    update               : Sa  Aug 06 2004
    copyright            : (C) 2001 by Georg P. Loczewski
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

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <malloc.h>
#include "gc.h"
#include <string.h>
#include "arsc.h"

extern jmp_buf _EXH;

EXP *currentExp;
ENV *globalEnv;
EXP *trueSym;

char *nameTable[MAX_NAMES];
NAME numNames;
NAME setName;
NAME eof;
NAME lambda;
VALUE *setVal;
VALUE *falseValue, *trueValue;

int false;
int true;


/*****************************************************************
 *                  ADT CONSTRUCTORS                             *
 *****************************************************************/


/* newVAL - make a value (evaluated expression)  */
VALUE *
newVAL (VTYPE t)
{
  VALUE *val;

  val = (VALUE *) GC_malloc (sizeof (VALUE));
  val->type = t;
  return val;
}


/* newLISTV - represent a value list as a value   */
VALUE *
newLISTV (VLIST *vl)
{
  VALUE *val;

  if (vl) {
     val = (VALUE *) GC_malloc (sizeof (VALUE));
     val->type = LISTV;
     val->val.lv.car = vl->head;
     val->val.lv.cdr = newLISTV(vl->tail);
     return val;
  }
  else {
     return NULL;
  }
}


/* newPRIMV - make a primitive value */
VALUE *
newPRIMV (PRIM prim)
{
  VALUE *result;

  result = (VALUE *) GC_malloc (sizeof (VALUE));
  result->type = PRIMV;
  result->val.prim = prim;
  return result;
}


/* newSTRV- make a string value */
VALUE *
newSTRV(NAME nm)
{
  VALUE *result;

  result = (VALUE *) GC_malloc (sizeof (VALUE));
  result->type = STRV;
  result->val.str = nm;
  return result;
}

/* newSYMV- make a symbol value */
VALUE *
newSYMV(NAME nm)
{
  VALUE *result;

  result = (VALUE *) GC_malloc (sizeof (VALUE));
  result->type = SYMV;
  result->val.sym = nm;
  return result;
}

/* newINTV- make an integer value */
VALUE *
newINTV (int i)
{
  VALUE *result;

  result = (VALUE *) GC_malloc (sizeof (VALUE));
  result->type = INTV;
  result->val.ival = i;
  return result;
}


/* newACLV- make an ars-closure */
VALUE *
newACLV (EXP * fun, ENV * env)
{
  VALUE *result;

  result = (VALUE *) GC_malloc (sizeof (VALUE));
  result->type = ACLV;
  result->val.acl.fun = fun;
  result->val.acl.env = env;
  return result;
}

/* newCLAMV- make a c-lambda abstraction  */
VALUE *
newCLAMV (NLIST * vars, LFUN lfun, ENV *env)
{
  VALUE *cl;

  cl = (VALUE *) GC_malloc (sizeof (VALUE));
  cl->type = CLAMV;
  cl->val.clam.vars = vars;
  cl->val.clam.lfun = lfun;
  cl->val.clam.env = env;
  return cl;
}


/* ncons - make a list of names */
NLIST *
ncons (NAME nm, NLIST * nl)
{
  NLIST *newnl;

  newnl = (NLIST *) GC_malloc (sizeof (NLIST));
  newnl->head = nm;
  newnl->tail = nl;
  return newnl;
}


/* vcons - make a list of values */
VLIST *
vcons (VALUE * val, VLIST * vl)
{
  VLIST *newvl;

  newvl = (VLIST *) GC_malloc (sizeof (VLIST));
  newvl->head = val;
  newvl->tail = vl;
  return newvl;
}

/* cons - make a list of values */
VALUE *
cons (VALUE * hd, VALUE * tl)
{
  VALUE *newvl;

  newvl = (VALUE *) GC_malloc (sizeof (VALUE));
  newvl->type = LISTV;
  newvl->val.lv.car = hd;
  newvl->val.lv.cdr = tl;
  return newvl;
}


/* newENV - make an environment */
ENV *
newENV (NLIST * nl, VLIST * vl, ENV * env)
{
  ENV *newenv;

  newenv = (ENV *) GC_malloc (sizeof (ENV));
  newenv->vars = nl;
  newenv->values = vl;
  newenv->next = env;
  return newenv;
}


/*****************************************************************
 *                     NAMES OF VARIABLES                        *
 *****************************************************************/

/* initNames - place all pre-defined names into nameTable       */
void
initNames ()
{
  long i = 1;

  nameTable[i - 1] = "define";
  i++;
  nameTable[i - 1] = "incr";
  i++;
  nameTable[i - 1] = "+";
  i++;
  nameTable[i - 1] = "-";
  i++;
  nameTable[i - 1] = "*";
  i++;
  nameTable[i - 1] = "/";
  i++;
  nameTable[i - 1] = "print";
  i++;
  nameTable[i - 1] = "load";
  i++;
  nameTable[i - 1] = "equal";
  i++;
  nameTable[i - 1] = ">=";
  i++;
  nameTable[i - 1] = "<";
  i++;
  nameTable[i - 1] = "quit";
  i++;
  nameTable[i - 1] = "false";
  i++;
  nameTable[i - 1] = "true";
  numNames = i;
}



/* defName - insert new name into nameTable                     */
NAME
defName (char *nm)
{
  long i = 1;
  int found = false;

  while (i <= numNames && !found) {
    if (!strcmp (nm, nameTable[i - 1]))
      found = true;
    else
      i++;
  }
  if (found)
    return i;
  if (i > MAX_NAMES) {
    printf ("No more room for names\n");
    longjmp (_EXH, 1);
  }
  numNames = i;
  nameTable[i - 1] = nm;
  return i;
}


/* prName - print name nm  */
void
prName (NAME nm)
{
  printf ("%s", nameTable[nm - 1]);
}


/* prString - print string nm (strings are handled like symbols ) */
void
prString (NAME nm)
{
  printf ("%s", nameTable[nm - 1]);
}

/* lengthNL - return length of NLIST nl      */
long
lengthNL (NLIST * nl)
{
  long i = 0;

  while (nl != NULL) {
    i++;
    nl = nl->tail;
  }
  return i;
}

/* prNL - print list of names */
void
prNL (NLIST * nl)
{
  long i = 0;

  putchar ('(');
  while (nl != NULL) {
    i++;
    prName (nl->head);
    nl = nl->tail;
    if (nl != NULL) {
       putchar (' ');
    }
  }
  printf (")\n");
}


/*****************************************************************
 *                     VARIABLES AND ENVIRONMENTS                *
 *****************************************************************/

/* emptyEnv - return an environment with no bindings             */
ENV *
emptyEnv ()
{
  return (newENV (NULL, NULL, NULL));
}


/* defVar - bind variable nm to value val in environment env      */
void
defVar (NAME nm, VALUE * val, ENV * env)
{
  env->vars = ncons (nm, env->vars);
  env->values = vcons (val, env->values);
}

/* setVar - set variable nm to value val in env                 */
void
setVar (NAME nm, VALUE * val, ENV * env)
{
  VLIST *vl;

  vl = bindingInEnv (nm, env);
  if (vl) {
     vl->head = val;
  }
  else {
     printf("variable not defined: ");
     prName(nm);
     printf("\n");
  }
}

/* getVar - return VAL bound to nm in env                        */
VALUE *
getVar (NAME nm, ENV * env)
{
  VLIST *vl;

  vl = bindingInEnv (nm, env);
  if (vl)
     return (vl->head);
  else
     return falseValue;
}

/*  extendEnv - extend environment env by binding vars to vals   */
ENV *
extendEnv (ENV * env, NLIST * vars, VLIST * vals)
{
  return (newENV (vars, vals, env));
}


/* bindingInFrame - look up nm in one frame                      */
VLIST *
bindingInFrame (NLIST * nl, VLIST * vl, NAME nm)
{
  int found = false;

  while (nl != NULL && !found) {
    if (nl->head == nm)
      found = true;
    else {
      nl = nl->tail;
      vl = vl->tail;
    }
  }
  return vl;
}


/* bindingInEnv - look up nm in env                                   */
VLIST *
bindingInEnv (NAME nm, ENV * env)
{
  VLIST *vl;

  do {
    vl = bindingInFrame (env->vars, env->values, nm);
    env = env->next;
  } while (vl == NULL && env != NULL);
  return vl;
}


/* isBound - check if nm is bound in env                         */
int
isBound (NAME nm, ENV * env)
{
  return (bindingInEnv (nm, env) != NULL);
}

/* initGlobalEnv - initialize global environment */
void
initGlobalEnv ()
{
  PRIM op;

  false = 0;
  true = 1;
  falseValue = newVAL (SYMV);
  trueValue = newVAL (SYMV);
  falseValue->val.sym = defName("false");
  trueValue->val.sym = defName("true");
  globalEnv = emptyEnv ();
  setVal->val.ival = 0;
  defVar (setName, setVal, globalEnv);

  eof = defName (strdup ("*eof*"));
  lambda = defName (strdup ("lambda"));
}

/*****************************************************************
 *                     EVALUATION  AND VALUES                    *
 *****************************************************************/


/* apply - apply c-closure to arguments */
VALUE *
apply (VALUE * cl, VLIST * args)
{
  LFUN lfun;
  NLIST *names;
  ENV *savedenv, *newenv;

  savedenv = cl->val.clam.env;
  names = cl->val.clam.vars;
  lfun = cl->val.clam.lfun;
  if (lengthNL (names) != lengthVL (args)) {
    printf ("Wrong number of arguments to closure\n");
    printf ("names\n");
    prNL (names);
    printf ("arguments\n");
    prVL (args);
    longjmp (_EXH, 1);
  }
  newenv = extendEnv (savedenv, names, args);
  return (lfun(newenv));
}

/* prValue - print value  val                                */
void
prValue (VALUE * val)
{
  int i;
  
  if (val != NULL) {
     switch (val->type) {

     case INTV:
       printf ("%ld", val->val.ival);
       break;

     case SYMV:
       prString (val->val.sym);
       break;

     case STRV:
       prString (val->val.str);
       break;

     case LISTV:
        i = 0;
        putchar ('(');
        while (val != NULL) {
          i++;
          prValue (val->val.lv.car);
          putchar (' ');
          val = val->val.lv.cdr;
        }
        printf (")\n");
       break;

     case PRIMV:
       printf ("<primitive: ");
       prName ((int) val->val.prim + 1);
       putchar ('>');
       break;

     case ACLV:
       printf ("lambda");
       prNL(val->val.acl.fun->exp.abs.vars);
       break;

     case CLAMV:
       printf ("<c-lambda-abstraction>");
       break;

     case THUNKV:
       printf ("...");
       break;
     }
  }
}

/* lengthVL - return length of VLIST vl      */
long
lengthVL (VLIST * vl)
{
  long i = 0;

  while (vl != NULL) {
    i++;
    vl = vl->tail;
  }
  return i;
}

/* prVL - print list of values */
void
prVL (VLIST * vl)
{
  long i = 0;

  putchar ('(');
  while (vl != NULL) {
    i++;
    prValue (vl->head);
    putchar (' ');
    vl = vl->tail;
  }
  printf (")\n");
}

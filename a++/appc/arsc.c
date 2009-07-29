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

/* newVXP - make a value expression */
EXP *
newVXP (VALUE * val)
{
  EXP *e;

  e = (EXP *) GC_malloc (sizeof (EXP));
  e->etype = VXP;
  e->exp.val = val;
  return e;
}


/* newRXP - make a variable expression (reference)  */
EXP *
newRXP (NAME nm)
{
  EXP *e;

  e = (EXP *) GC_malloc (sizeof (EXP));
  e->etype = RXP;
  e->exp.ref = nm;
  return e;
}


/* newSXP - make an application expression (synthesis)  */
EXP *
newSXP (EXP * op, ELIST * el)
{
  EXP *e;

  e = (EXP *) GC_malloc (sizeof (EXP));
  e->etype = SXP;
  e->exp.syn.op = op;
  e->exp.syn.args = el;
  return e;
}


/* newAXP - make a lambda expression (abstraction)  */
EXP *
newAXP (NLIST * formals, ELIST * body)
{
  EXP *e;

  e = (EXP *) GC_malloc (sizeof (EXP));
  e->etype = AXP;
  e->exp.abs.vars = formals;
  e->exp.abs.body = body;
  return e;
}


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

/* newTHUNKV - make a frozen expression (delayed evaluation) */
VALUE *
newTHUNKV (EXP * e, ENV * env)
{
  VALUE *result;

  result = (VALUE *) GC_malloc (sizeof (VALUE));
  result->type = THUNKV;
  result->val.thunk.body = e;
  result->val.thunk.env = env;
  return result;
}


/* econs - make a list of expressions */
ELIST *
econs (EXP * e, ELIST * el)
{
  ELIST *newel;

  newel = (ELIST *) GC_malloc (sizeof (ELIST));
  newel->head = e;
  newel->tail = el;
  return newel;
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

/* initNames - place all pre-defined names into nameTable  */
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
  trueSym = newRXP (trueValue->val.sym);
  globalEnv = emptyEnv ();
  for (op = PRIMDEF;
       (long) op <= (long) PRIMQUIT; op = (PRIM) ((long) op + 1)) {
    defVar ((int) op + 1, newPRIMV (op), globalEnv);
  }
  setName = defName (strdup ("vmzero"));
  setVal = newVAL(INTV);
  setVal->val.ival = 0;
  defVar (setName, setVal, globalEnv);
  setName = defName (strdup ("vmtrue"));
  setVal = trueValue;
  defVar (setName, setVal, globalEnv);

  setName = defName (strdup ("vmfalse"));
  setVal = falseValue;
  defVar (setName, setVal, globalEnv);

  eof = defName (strdup ("*eof*"));
  lambda = defName (strdup ("lambda"));
}

/*****************************************************************
 *                     EVALUATION  AND VALUES                    *
 *****************************************************************/

/* eval - evaluate  expression e in local environment env  */
VALUE *
eval (EXP * e, ENV * env, EXP * lazy)
{
  VALUE *result, *op;
  PRIM primname;

  switch (e->etype) {

  case VXP:
    result = e->exp.val;
    break;

  case RXP:
    if (isBound (e->exp.ref, env))
      result = getVar(e->exp.ref, env);
    else {
      printf ("Undefined variable: ");
      prName (e->exp.ref);
      putchar ('\n');
      longjmp (_EXH, 1);
    }
    result = evalThunk (result);
    break;

  case SXP:
    op = evalThunk (eval (e->exp.syn.op, env, lazy));
    if (op->type == PRIMV) {
      primname = op->val.prim;
      if (primname == PRIMDEF)
	result = applySpecialForm (PRIMDEF, e->exp.syn.args, env);
      else
	result = applyPrimitive (primname,
				 evalList (e->exp.syn.args, env, lazy));
    }
    else
      result = applyARSClosure (op, 
		                evalList (e->exp.syn.args, env, lazy), 
			        lazy);
    break;

  case AXP:
    result = newACLV(e, env);
    break;
  }
  return result;
}


/* evalThunk - enforce evaluation of frozen expression                */
/* memoizing version would be faster than standard version but not as */
/* flexible and is not used here to provide maximum flexibility for   */
/* ARS ( to allow implementing special forms as normal functions      */

VALUE *
evalThunk (VALUE * val)
{
  if (val->type == THUNKV)
    return (evalThunk (eval (val->val.acl.fun,
			     val->val.acl.env, trueSym)));
  else
    return val;
}

/* applyPrimitive - apply PRIM op to arguments in VLIST vl  */
VALUE *
applyPrimitive (PRIM op, VLIST * vl)
{
  VALUE *result, *v1, *v2;
  int nargs;

  nargs = lengthVL (vl);
  switch (nargs) {
  case 0:
    if (op == PRIMQUIT) {
      longjmp (_EXH, 2);
    }
    else {
      printf ("Illegal operation: ");
      prName ((int) op + 1);
      putchar ('\n');
      longjmp (_EXH, 1);
    }
    break;
  case 1:
    v1 = evalThunk (vl->head);	/* 1st actual */
    switch (op) {
    case PRIMINCR:
      if (v1->type == INTV) {
	result = newVAL (INTV);
	result->val.ival = v1->val.ival + 1;
	return result;
      }
      else {
	printf ("Non-arithmetic arguments to ");
	prName ((int) op + 1);
	putchar ('\n');
	prValue (v1);
	longjmp (_EXH, 1);
      }
      break;
    case PRIMPRINT:
      printf("-->");
      prValue (v1);
      putchar ('\n');
      result = newVAL (SYMV);;
      result->val.sym = defName("void\n");
      break;
    case PRIMLOAD:
      prValue (v1);
      putchar ('\n');
      result = load (v1);
      break;
    default:
      printf ("undefined primitive operation\n ");
      prName ((int) op + 1);
      putchar ('\n');
      longjmp (_EXH, 2);
    }
    break;
  case 2:
    v1 = evalThunk (vl->head);	/* 1st actual */
    v2 = evalThunk (vl->tail->head);	/* 2nd actual */
    switch (op) {
      case PRIMADD:
        if ((v1->type == INTV) && (v2->type == INTV)) {
	   result = newVAL (INTV);
	   result->val.ival = v1->val.ival + v2->val.ival;
        }
        else {
           printf ("PRIMADD:\n ");
           printf ("Integer argument expected!\n ");
           longjmp (_EXH, 2);
        }
        break;
      case PRIMSUB:
        if ((v1->type == INTV) && (v2->type == INTV)) {
	   result = newINTV (v1->val.ival - v2->val.ival);
        }
        else {
           printf ("PRIMSUB:\n ");
           printf ("Integer argument expected!\n ");
           longjmp (_EXH, 2);
        }
        break;
      case PRIMMLT:
        if ((v1->type == INTV) && (v2->type == INTV)) {
	   result = newINTV (v1->val.ival * v2->val.ival);
        }
        else {
           printf ("PRIMMLT:\n ");
           printf ("Integer argument expected!\n ");
           longjmp (_EXH, 2);
        }
        break;
      case PRIMDIV:
        if ((v1->type == INTV) && (v2->type == INTV)) {
	   result = newINTV (v1->val.ival / v2->val.ival);
        }
        else {
           printf ("Integer argument expected!\n ");
           longjmp (_EXH, 2);
        }
        break;
      case PRIMGE:
        if (v1->type != v2->type) {
           result = getVar(defName("false"), globalEnv);
        }
        else if ((v1->type == INTV) &&
                 (v1->val.ival >= v2->val.ival)) {
                result = getVar(defName("true"), globalEnv);
        }
        else {
           result = getVar(defName("false"), globalEnv);
        }
        break;
      case PRIMLT:
        if (v1->type != v2->type) {
           result = getVar(defName("false"), globalEnv);
        }
        else if ((v1->type == INTV) &&
                 (v1->val.ival < v2->val.ival)) {
                result = getVar(defName("true"), globalEnv);
        }
        else {
           result = getVar(defName("false"), globalEnv);
        }
        break;
      case PRIMEQUAL:
        if (v1 == v2) {
          result = getVar(defName("true"), globalEnv);
        }
        else if (v1->type != v2->type) {
           result = getVar(defName("false"), globalEnv);
        }
        else if ((v1->type == INTV) &&
                 (v1->val.ival == v2->val.ival)) {
                result = getVar(defName("true"), globalEnv);
        }
        else if (v1->type == SYMV) {
                if (v1->val.sym == v2->val.sym) {
                   result = getVar(defName("true"), globalEnv);
                }
                else {
                   result = getVar(defName("false"), globalEnv);
                }
        }
        else if (v1->type == STRV) {
                if (v1->val.str == v2->val.str) {
                   result = getVar(defName("true"), globalEnv);
                }
                else {
                   result = getVar(defName("false"), globalEnv);
                }
        }
        else {
           result = getVar(defName("false"), globalEnv);
        }
        break;
      default:
        printf ("undefined primitive operation\n ");
        prName ((int) op + 1);
        putchar ('\n');
        longjmp (_EXH, 2);
      }
      break;
    default:
      printf ("Wrong number of arguments to ");
      prName ((int) op + 1);
      putchar ('\n');
      longjmp (_EXH, 1);
  }
  return result;
}

/* lazyp - checks if lazy evaluation is to be applied */
int
lazyp (EXP * e1, EXP * e2)
{
  if (e1->etype == RXP && e2->etype == RXP) {
    if (e1->exp.ref == e2->exp.ref)
      return false;
    else
      return true;
  }
  else
    return true;
}


/* evalList - evaluate each expression in el                     */
VLIST *
evalList (ELIST * el, ENV * env, EXP * lazy)
{
  VALUE *h;
  VLIST *t;

  if (el == NULL)
    return NULL;
  else {
    if (lazyp (el->head, lazy))
      h = newTHUNKV (el->head, env);
    else
      h = evalThunk (eval (el->head, env, lazy));

    t = evalList (el->tail, env, lazy);
    return (vcons (h, t));
  }
}

/* evalSequence - evaluate a sequence of expressions */
VALUE *
evalSequence (ELIST * el, ENV * newenv, EXP * lazy)
{
  while (el->tail != NULL) {
    evalThunk (eval (el->head, newenv, lazy));
    el = el->tail;
  }
  return (evalThunk (eval (el->head, newenv, lazy)));
}

/* applyARSClosure - apply closure to arguments */
VALUE *
applyARSClosure (VALUE * op, VLIST * args, EXP * lazy)
{
  EXP *fun;
  ELIST *body;
  NLIST *names;
  ENV *savedenv, *newenv;

  fun = op->val.acl.fun;
  savedenv = op->val.acl.env;
  names = fun->exp.abs.vars;
  body = fun->exp.abs.body;
  if (lengthNL (names) != lengthVL (args)) {
    printf ("Wrong number of arguments to closure\n");
    printf ("names\n");
    prNL (names);
    printf ("arguments\n");
    prVL (args);
    longjmp (_EXH, 1);
  }
  newenv = extendEnv (savedenv, names, args);
  return (evalSequence (body, newenv, lazy));
}

/* applySpecialForm - evaluate special form */
VALUE *
applySpecialForm (PRIM op, ELIST * args, ENV * env)
{
  VALUE *result;

  if (op == PRIMDEF) {
    result = evalThunk (eval (args->tail->head, env, args->head));
    if (isBound (args->head->exp.ref, env)) {
      setVar (args->head->exp.ref, result, env);
    }
    else {
      defVar (args->head->exp.ref, result, env);
    }
  }
  else {
    printf ("undefined control operation!\n");
    longjmp (_EXH, 2);
  }
  return result;
}

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
          prValue (evalThunk (val->val.lv.car));
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
    prValue (evalThunk (vl->head));
    putchar (' ');
    vl = vl->tail;
  }
  printf (")\n");
}
/*****************************************************************
 *                        LOAD ABSTRACTION FROM FILE             *
 *****************************************************************/

/* primitive function load (load abstractions from file) */
VALUE *
load (VALUE * val)
{
  char *file;
  FILE *fis;
  EXP *currp;
  VALUE *result;
  int ret;

  if (val->type == STRV) {
    file = nameTable[val->val.str - 1];
  }
  else {
    printf ("load: error in argument type\n");
    longjmp (_EXH, 1);
  }
  fis = fopen (file, "r");
  parserinit (fis);

  while (1) {
    currp = readWithoutPrompt ();
    result = evalThunk (eval (currp, globalEnv, trueSym));
    if ((result->type == STRV) && (result->val.str == eof)) {
      fclose (fis);
      break;
    }
    prValue (result);
    //printf (".");
  }
  parserinit (stdin);
  return result;
}

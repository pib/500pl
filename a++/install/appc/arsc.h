/**********************************************************************
                          arsc.h  -  description
                          ----------------------
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
#define MAX_NAMES        300	/* Maximum number of different names */

typedef short NAME;		/* a NAME is an index in nameTable */

typedef enum
{
  PRIMDEF,   PRIMINCR, PRIMADD,   PRIMSUB, PRIMMLT, PRIMDIV, 
  PRIMPRINT, PRIMLOAD, PRIMEQUAL, PRIMGE,  PRIMLT,  PRIMQUIT
}
PRIM;

typedef enum
{
  INTV, SYMV, STRV, LISTV, ACLV, CLAMV, PRIMV, THUNKV
}
VTYPE;

typedef enum
{
  AXP, RXP, SXP, VXP
}
EXPTYPE;

typedef struct syn
{
  struct exp *op;
  struct elist *args;
}
SYN;

typedef struct abs
{
  struct nlist *vars;
  struct elist *body;
}
ABS;

typedef struct exp
{
  EXPTYPE etype;
  union
  {
    struct value *val;
    ABS    abs;
    NAME   ref;
    SYN    syn;
  }
  exp;
}
EXP;

typedef struct elist
{
  EXP *head;
  struct elist *tail;
}
ELIST;

typedef struct nlist
{
  NAME head;
  struct nlist *tail;
}
NLIST;

typedef struct vlist
{
  struct value *head;
  struct vlist *tail;
}
VLIST;

typedef struct env
{
  NLIST *vars;
  VLIST *values;
  struct env *next;
}
ENV;

typedef struct value  * (*LFUN) (ENV *env);

typedef struct clam
{
  struct nlist *vars;
  LFUN lfun;
  ENV  *env;
}
CLAM;

typedef struct acl
{
  EXP *fun;
  ENV *env;
}
ACL;

typedef struct thunk
{
  EXP *body;
  ENV *env;
}
THUNK;

typedef struct value
{
  VTYPE type;
  union
  {
    long ival;
    NAME sym;
    NAME str;
    struct {
       struct value  *car;
       struct value  *cdr;
    } lv;
    ACL   acl;
    CLAM   clam;
    THUNK thunk;
    PRIM  prim;
  }
  val;
}
VALUE;


/*****************************************************************
 *                     ADT CONSTRUCTORS                          *
 *****************************************************************/
EXP *newVXP (VALUE * val);
EXP *newRXP (NAME nm);
EXP *newSXP (EXP * op, ELIST * el);
EXP *newAXP (NLIST * f, ELIST * b);
VALUE *newCLAMV (NLIST * vars, LFUN lfun, ENV *env);
VALUE *newVAL (VTYPE t);
VALUE *newLISTV (VLIST *vl);
VALUE *newPRIMV (PRIM op);
VALUE *newSTRV (NAME nm);
VALUE *newSYMV (NAME nm);
VALUE *newINTV (int  i);
VALUE *newACLV (EXP * e, ENV * env);
VALUE *newTHUNKV (EXP * e, ENV * env);
ENV *newENV (NLIST * nl, VLIST * vl, ENV * env);
ELIST *econs (EXP * e, ELIST * el);
NLIST *ncons (NAME nm, NLIST * nl);
VLIST *vcons (VALUE *val, VLIST * vl);
VALUE *cons (VALUE *hd, VALUE * tl);
/*****************************************************************
 *                     NAMES OF VARIABLES                        *
 *****************************************************************/
void initNames ();
NAME defName (char *nm);
void prName (NAME nm);
void prNL (NLIST * nl);
long lengthNL (NLIST * nl);
void prString (NAME nm);
/*****************************************************************
 *                     VARIABLES AND ENVIRONMENTS                *
 *****************************************************************/
ENV *emptyEnv ();
void defVar (NAME nm, VALUE *val, ENV * env);
void setVar (NAME nm, VALUE *val, ENV * env);
VALUE *getVar (NAME nm, ENV * env);
ENV *extendEnv (ENV * env, NLIST * vars, VLIST * vals);
VLIST *bindingInFrame (NLIST * nl, VLIST * vl, NAME nm);
VLIST *bindingInEnv (NAME nm, ENV * env);
int isBound (NAME nm, ENV * env);
void initGlobalEnv ();
/*****************************************************************
 *                     EVALUATION                                *
 *****************************************************************/
VALUE *eval (EXP * e, ENV * env, EXP * lazy);
VALUE *evalThunk (VALUE * val);
VLIST *evalList (ELIST * el, ENV * env, EXP * lazy);
VALUE *evalSequence (ELIST * el, ENV * newenv, EXP * lazy);
VALUE *applyARSClosure (VALUE * acl, VLIST * args, EXP * lazy);
VALUE *applyPrimitive (PRIM op, VLIST * vl);
VALUE *applySpecialForm (PRIM op, ELIST * args, ENV * env);
VALUE *apply(VALUE * cl, VLIST * args);
int lazyp (EXP * e1, EXP * e2);
void prValue (VALUE * val);
void prVL (VLIST * vl);
long lengthVL (VLIST * vl);
/*****************************************************************
 *                     MAIN CONTROL AND INPUT                    *
 *****************************************************************/
void parserinit (FILE * is);	
EXP *readWithPrompt ();
EXP *readWithoutPrompt ();
VALUE *load (VALUE * s);
int main (int argc, char *argv[]);

/***********************************************************************
                          main.c  -  description
                          ----------------------
    begin                : Wed Apr 17 22:22:30 CEST 2001
    update               : Sat Aug 28 21:12:15 CEST 2004
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <setjmp.h>
#include <malloc.h>
#include "gc.h"
#include <string.h>
#include "arsc.h"
#include "test.h"

extern ENV *globalEnv;
extern EXP *currentExp;
extern EXP *trueSym;

extern int true;

jmp_buf _EXH;

int
main (int argc, char *argv[])
{
  int ret;

  //malloc_stats ();
  initNames ();
  parserinit (stdin);
  initGlobalEnv ();
  load (newSTRV(defName("init.app")));
  if (argc > 1) {
    switch (argv[1][1]) {
    case 'i':
      ret = setjmp (_EXH);
      if (ret == 1)
	goto _LBREAK;
      if (ret == 2)
	goto _LQUIT;
      goto _LBREAK;
    case 'v':
      ret = setjmp (_EXH);
      if (ret == 1 || ret == 2)
	goto _LQUIT;
      printf ("A++ Interpreter 'arscint' version 2.0\n");
      longjmp (_EXH, 2);
    default:
      printf ("undefined option: %s\n", argv[1]);
      longjmp (_EXH, 2);
    }
  }
  else {
     ret = setjmp (_EXH);
     if (ret == 1)
        goto _LBREAK;
     if (ret == 2)
        goto _LQUIT;
     goto _LBREAK;
  }
_LBREAK:
  while (1) {
     currentExp = readWithPrompt ();
     prValue (evalThunk (eval (currentExp, globalEnv, trueSym)));
     printf ("\n\n");
  }
_LQUIT:
  //malloc_stats ();
  return EXIT_SUCCESS;
}

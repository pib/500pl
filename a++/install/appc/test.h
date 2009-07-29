/***********************************************************************
                          test.h  -  description
                          ----------------------
    begin                : Mit Apr 17 22:22:30 CEST 2001
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


void ootest();
VALUE *make_account(ENV *env);
VALUE *get_balance(ENV *env);
VALUE *deposit(ENV *env);
VALUE *withdraw(ENV *env);
VALUE *print_balance(ENV *env);
VALUE *self(ENV *env);
VALUE *konto(ENV *env);

void futest();
VALUE *car(ENV *env);
VALUE *cdr(ENV *env);
VALUE *compose_aux(ENV *env);
VALUE *compose(ENV *env);

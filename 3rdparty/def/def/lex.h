/*******************************************************************************
 *******************************************************************************
 * Copyright 2013, Cadence Design Systems
 * 
 * This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 * Distribution,  Product Version 5.8. 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *    implied. See the License for the specific language governing
 *    permissions and limitations under the License.
 * 
 * For updates, support, or to become part of the LEF/DEF Community,
 * check www.openeda.org for details.
 *******************************************************************************
 * 
 *  $Author: dell $
 *  $Revision: #7 $
 *  $Date: 2015/01/27 $
 *  $State:  $
 ******************************************************************************/

#include "defiKRDefs.hpp"


BEGIN_LEFDEF_PARSER_NAMESPACE

extern int yylex();
extern void lex_init();
extern void lex_un_init();
extern int defyyparse();

void AddStringDefine(char *token, char *string);
void AddBooleanDefine(char *token, int val);
void AddNumDefine(char *token, double val);
void yyerror(const char *s);
void defError(int msgNum, const char *s);
void defWarning(int msgNum, const char *s);
void defInfo(int msgNum, const char *s);
void* defMalloc(size_t def_size);
void* defRealloc(void *name, size_t def_size);
void defFree(void *name);

int fake_ftell();

END_LEFDEF_PARSER_NAMESPACE

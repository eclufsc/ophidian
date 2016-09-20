// *****************************************************************************
// *****************************************************************************
// Copyright 2013, Cadence Design Systems
// 
// This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
// Distribution,  Product Version 5.8. 
// 
// Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
// 
//        http://www.apache.org/licenses/LICENSE-2.0
// 
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
// 
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
// 
//  $Author: dell $
//  $Revision: #8 $
//  $Date: 2015/01/27 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "defiDebug.hpp"


#include "defrData.hpp"
#include "defrSettings.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

/*******************
 *  Debug flags:
 *  0 -
 *  1 - malloc debug
 *  2 - print each history size bump up.
 *  3 - print each call to CatchAll
 *  4 -
 *  5 -
 *  6 -
 *  7 -
 *  8 -
 *  9 -
 * 10 -
 * 11 - lexer debug
 *
 ******************************/

/* Set flag */
void defiSetDebug(int num, int value) {
  defSettings->Debug[num] = value;
}

/* Read flag */
int defiDebug(int num) {
  return defSettings->Debug[num];
}

void defiError(int check, int msgNum, const char* mess) {
  /* check is 1 if the caller function has checked totalMsgLimit, etc. */

  if (!check) {
     if ((defSettings->totalDefMsgLimit > 0) && (defData->defMsgPrinted >= defSettings->totalDefMsgLimit))
        return;
     if (defSettings->MsgLimit[msgNum-5000] > 0) {
        if (defData->msgLimit[msgNum-5000] >= defSettings->MsgLimit[msgNum-5000])
           return;    /*over the limit*/
        defData->msgLimit[msgNum-5000] = defData->msgLimit[msgNum-5000] + 1;
     }
     defData->defMsgPrinted++;
  }

  if (defSettings->ErrorLogFunction) {
    (*defSettings->ErrorLogFunction)(mess);
  } else {
    fprintf(stderr, mess);
  }
}



static char defiShift [] = {
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  ' ',  '!',  '"',  '#',  '$',  '%',  '&', '\'', 
  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/', 
  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7', 
  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?', 
  '@',  'A',  'B',  'C',  'D',  'E',  'F',  'G', 
  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O', 
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W', 
  'X',  'Y',  'Z',  '[', '\\',  ']',  '^',  '_', 
  '`',  'A',  'B',  'C',  'D',  'E',  'F',  'G', 
  'H',  'I',  'J',  'K',  'l',  'M',  'N',  'O', 
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W', 
  'X',  'Y',  'Z',  '{',  '|',  '}',  '~', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};


const char* upperCase(const char* str) {
  char* place = (char*)str;
  char* to;
  int len = strlen(str) + 1;

  if (len > defData->shiftBufLength) {
    if (defData->shiftBuf == 0) {
      len = len < 64 ? 64 : len;
      defData->shiftBuf = (char*)defMalloc(len);
      defData->shiftBufLength = len;
    } else {
      defFree(defData->shiftBuf);
      defData->shiftBuf = (char*)defMalloc(len);
      defData->shiftBufLength = len;
    }
  }

  to = defData->shiftBuf;
  while (*place) {
      int i = (int)*place;
      place++;
    *to++ = defiShift[i];
  }
  *to = '\0';

  return defData->shiftBuf;
}


END_LEFDEF_PARSER_NAMESPACE


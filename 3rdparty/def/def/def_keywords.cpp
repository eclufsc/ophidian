// *****************************************************************************
// *****************************************************************************
// Copyright 2013 - 2016, Cadence Design Systems
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
//  $Author: icftcm $
//  $Revision: #11 $
//  $Date: 2016/05/31 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

/*                                                                     */
/*   Revision:                                                         */
/*   03-15-2000 Wanda da Rosa - Add code to support 5.4, add keywords  */
/*                              for PINS + USE, SPECIALNETS + SHAPE    */
/*                              and other keywords                     */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lex.h"
#include "defiDefs.hpp"
#include "defiDebug.hpp"
#include "defrCallBacks.hpp"
#include "defrData.hpp"
#include "defrSettings.hpp"

#ifdef WIN32
#   include <direct.h>
#else /* not WIN32 */
#   include <unistd.h>
#endif /* WIN32 */


using namespace std;

BEGIN_LEFDEF_PARSER_NAMESPACE

#include "def.tab.h"

inline
int defGetKeyword(const char* name, int *result) 
{ 
    map<const char*, int, defCompareCStrings>::iterator search = defSettings->Keyword_set.find(name);

    if ( search != defSettings->Keyword_set.end()) {
        *result = search->second;
        return TRUE;
    }

    return FALSE;
}

inline
int defGetAlias(const string &name, string &result) 
{ 
    map<string, string, defCompareStrings>::iterator search = defData->def_alias_set.find(name);

    if ( search != defData->def_alias_set.end()) {
        result = search->second;
        return TRUE;
    }

    return FALSE;
}

inline
int defGetDefine(const string &name, string &result) 
{ 
    map<string, string, defCompareStrings>::iterator search = defData->def_defines_set.find(name);

    if ( search != defData->def_defines_set.end()) {
        result = search->second;
        return TRUE;
    }

    return FALSE;
}

#define yyparse	defyyparse
#define yylex	defyylex
#define yyerror	defyyerror
#define yylval	defyylval
#define yychar	defyychar
#define yydebug	defyydebug
#define yynerrs	defyynerrs

// lex.cpph starts here

/* Although in the DEF language reference manual has stated the max size is
** 2,048 characters, def still allow larger due to pcr 436497 was fixed
** before I realized the limit
*/


int defrLineNumber() {
    // Compatibility feature: in old versions the translators,  
    // the function can be called before defData initialization. 
    return defData ? (int)defData->nlines : 0; 
}

long long defrLongLineNumber() {
    // Compatibility feature: in old versions the translators,  
    // the function can be called before defData initialization. 
    return defData ? defData->nlines : (long  long) 0; 
}


extern char* defkywd(int num);

/* User defined if log file should be in append from the previous run */

/* User defined if property string value should be process */

/* Varible from lex.cpph to keep track of invalid nonEnglish character */
/* in def file */

/************Some simple file reading routines since ungetc() proves ****/
/************to be quite slow, and we don't need multiple chars of pushback */
#ifndef WIN32
#   include <unistd.h>
#endif



void reload_buffer() {
   int nb = 0;

   if (defData->first_buffer) {
      defData->first_buffer = 0;
      if (defSettings->ReadFunction) {
         if ((nb = (*defSettings->ReadFunction)(defSettings->File, defData->buffer, 4)) != 4) {
            defData->next = NULL;
            return;
         }
      } else {
         if ((nb = fread(defData->buffer, 1, 4, defSettings->File)) != 4) {
            defData->next = NULL;
            return;
         }
      }
   }

   if (nb == 0) {
      if (defSettings->ReadFunction)
         nb = (*defSettings->ReadFunction)(defSettings->File, defData->buffer, IN_BUF_SIZE);
      else
         /* This is a normal file so just read some bytes. */
         nb = fread(defData->buffer, 1, IN_BUF_SIZE, defSettings->File);
   }
    
   if (nb <= 0) {
      defData->next = NULL;
   } else {
      defData->next = defData->buffer;
      defData->last = defData->buffer + nb - 1;
   }
}   

int GETC() {
   // Remove '\r' symbols from Windows streams.
    for(;;) {
       if (defData->next > defData->last)
          reload_buffer();
       if(defData->next == NULL)
          return EOF;

       int ch =  *defData->next++;

       if (ch != '\r')
           return ch;
    }
}

void UNGETC(char ch) {
    if (defData->next <= defData->buffer) {
        defError(6111, "UNGETC: buffer access violation.");
    } else {
        *(--defData->next) = ch;
    }
}



/* The following two variables are for communicating with the parser */
                           /* even in dumb mode */
                           /* even in dumb mode */




/* Return a copy of the string allocated from the ring buffer.
 * We will keep several strings in the buffer and just reuse them.
 * This could cause problems if we need to use more strings than we
 * have in the buffer.
 */
char* ringCopy(const char* string) {
   int len = strlen(string) + 1;
   if (++defData->ringPlace >= RING_SIZE) defData->ringPlace = 0;
   if (len > defData->ringSizes[defData->ringPlace]) {
      defFree(defData->ring[defData->ringPlace]);
      defData->ring[defData->ringPlace] = (char*)defMalloc(len);
      defData->ringSizes[defData->ringPlace] = len;
   }
   strcpy(defData->ring[defData->ringPlace], string);
   return defData->ring[defData->ringPlace];
}


int DefGetTokenFromStack(char *s) {
   const char *ch;        /* utility variable */
   char *prS = NULL;          /* pointing to the previous char or s */
   char *save = s;            /* for debug printing */

   while(defData->input_level >= 0) {
      for(ch=defData->stack[defData->input_level].c_str(); *ch != 0; ch++)    /* skip white space */
         if (*ch != ' ' && *ch != '\t' && (defData->nl_token || *ch != '\n'))
            break;
      /* did we find anything?  If not, decrement level and try again */
      if (*ch == 0)
         defData->input_level--;
      else if (*ch == '\n') {
         *s++ = *ch; 
         *s = 0;
         if (defSettings->Debug[11]) 
            printf("Stack[%d] Newline token\n", defData->input_level);
         return TRUE;
      }
      else {            /* we found something */
        for(; ; ch++)  {
           if (*ch == ' ' || *ch == '\t' || *ch == '\n' || *ch == 0) {
              /* 10/10/2000 - Wanda da Rosa, pcr 341032
              ** Take out the defData->last '"', the 1st will be skip later
              */
              if (*prS == '"') {
                 *prS = '\0';
              } else
                 *s++ = '\0';
              defData->stack[defData->input_level] = ch;
              if (defSettings->Debug[11])
                 printf("Stack[%d]: <%s>, dm=%d\n", 
                        defData->input_level, save, defData->dumb_mode);
              return TRUE;
           }
           /* 10/10/2000 - Wanda da Rosa, pcr 341032
           ** Save the location of the previous s
           */
           prS = s;
           *s++ = *ch;
        }
     }
   }
   return FALSE;    /* if we get here, we ran out of input levels */
}

inline static void 
print_lines(long long lines) 
{
    if (lines % defSettings->defiDeltaNumberLines) {
        return;
    }

    if (defSettings->LineNumberFunction) {
        defSettings->LineNumberFunction((int)lines);
    } else if (defSettings->LongLineNumberFunction ) {
        defSettings->LongLineNumberFunction(lines);
    }
}

inline const char * 
lines2str(long long lines) 
{
    static char buffer[30];

#ifdef _WIN32
    sprintf(buffer, "%I64d", lines);
#else
    sprintf(buffer, "%lld", lines);
#endif 

    return buffer;
}


// Increment current position of buffer pointer. 
// Double buffer size if curPos is out of boundary.
static inline void  
IncCurPos(char **curPos, char **buffer, int *bufferSize)
{
    (*curPos)++;
    if (*curPos - *buffer < *bufferSize) {
        return;
    }

    long offset = *curPos - *buffer;

    *bufferSize *= 2;
    *buffer = (char*) realloc(*buffer, *bufferSize);
    *curPos = *buffer + offset;
}


int
DefGetToken(char **buffer, int *bufferSize)
{
    char *s = *buffer;
    int ch;
    int prCh;

    defData->ntokens++;
    defData->defInvalidChar = 0;

    if (defData->input_level >= 0){  /* if we are expanding an alias */
       if( DefGetTokenFromStack(s) ) /* try to get a token from it */
          return TRUE;               /* if we get one, return it */
    }                                /* but if not, continue */

    /* skip blanks and count lines */
    while ((ch = GETC()) != EOF) {
       if (ch == '\n') {
          print_lines(++defData->nlines);
       }
       if (ch != ' ' && ch != '\t' && (defData->nl_token || ch != '\n'))
          break;
    }

    if (ch == EOF) return FALSE;


    if (ch == '\n') {
       *s = ch; 
       IncCurPos(&s, buffer, bufferSize);

       *s = '\0';
       if (defSettings->Debug[11]) printf("Newline token\n");
          return TRUE;
    }

    /* now get the token */
    if (ch == '"') {
       do {
          /* 5/5/2008 - CCR 556818
          ** Check if the ch is a valid ascii character 0 =< ch < 128
          ** If not write out an error
          */
          /* 8/7/2008 - CCR 586175
          ** Some files may not end with \n or \0 or EOF as the defData->last character
          ** The parser allows this char instead of error out
          */
          if ((ch < -1) || (ch > 127)) {
             defData->defInvalidChar = 1;
          }

          /* 8/22/2000 - Wanda da Rosa, pcr 333334
          ** save the previous char to allow backslash quote within quote
          */
          prCh = ch;
          if (!defSettings->DisPropStrProcess) {
             /* 3/4/2008 - CCR 523879 - convert \\ to \, \" to ", \x to x */
             if (defData->parsing_property) {  /* working on property value */
                if (ch == '\\') {      /* got a \, save the next char only */
                   ch = GETC();
                   if ((ch == '\n') || (ch == EOF)) { /* senaty check */
                      *s = '\0';
                      return FALSE;
                   }
                   if (ch == '\\')
                      prCh = ' ';  /* current value is \\, for \", make sure */
                                   /* it is either \" or \\\" only */    

                   if (ch == 't' || ch == 'n' || ch == 'r') {  /* need to save the \ for \n, \t or \r */
                      *s = prCh;
                      IncCurPos(&s, buffer, bufferSize);
                   } 
                }
             }
          }

          /* 5/5/2004 - pcr 704784
          ** if name, or quote string is longer than TOKEN_SIZE, truncate it
          ** as described in the language reference manual
          ** 11/5/2010 - CCR 840697
          ** instead of truncating the string, temporary alloc a longer string
          ** to finish reading the string.
          */
          *s = ch;
          IncCurPos(&s, buffer, bufferSize);

          ch = GETC();

          if ((ch == '\n') || (ch == EOF)) { /* 7/23/2003 - pcr 606558 - do not allow \n in a string instead of ; */
             /* 5/30/2002 - Wanda da Rosa pcr 448738
             ** missing end "
             */ 
             if (((!defData->defInPropDef) && (!defData->parsing_property)) || (ch == EOF)) {
                *s = '\0';
                return FALSE;
             }
             /* 2/2/2007 - PCR 909714, allow string to go more than 1 line */
             /*            continue to parse */
          }
       }
       while ((ch != '"') || (prCh == '\\'));
       *s = '\0';
       return TRUE;
    }

    if (defData->names_case_sensitive) {
       for(; ; ch = GETC())  {

          /* 5/5/2008 - CCR 556818
          ** Check if the ch is a valid ascii character 0 =< ch < 128
          ** If not write out an error
          */
          if ((ch < -1) || (ch > 127)) {
             defData->defInvalidChar = 1;
          }

          if (ch == ' ' || ch == '\t' || ch == '\n' || ch == EOF)
             break;

          *s = ch;
          IncCurPos(&s, buffer, bufferSize);        
       }
    }
    else { /* we are case insensitive, use a different loop */
       for(; ; ch = GETC())  {

          /* 5/5/2008 - CCR 556818
          ** Check if the ch is a valid ascii character 0 =< ch < 128
          ** If not write out an error
          */
          if ((ch < -1) || (ch > 127)) {
             defData->defInvalidChar = 1;
          }

          if (ch == ' ' || ch == '\t' || ch == '\n' || ch == EOF)
             break;

          *s = (ch >= 'a' && ch <= 'z')? (ch -'a' + 'A') : ch;            
          IncCurPos(&s, buffer, bufferSize);
       }
    }
   
    /* If we got this far, the defData->last char was whitespace */
    *s = '\0';
    if (ch != EOF)   /* shouldn't ungetc an EOF */
       UNGETC((char)ch);
    return TRUE;
}

/* creates an upper case copy of an array */
void uc_array(char *source, char *dest)
{
    for(; *source != 0; )
    *dest++ = toupper(*source++);
    *dest = 0;
}

void defError(int msgNum, const char *s);

void
StoreAlias()
{
    int         tokenSize = TOKEN_SIZE;
    char        *aname = (char*)malloc(tokenSize);

    DefGetToken(&aname, &tokenSize);

    char        *line = (char*)malloc(tokenSize);

    DefGetToken(&line, &tokenSize);  

    char        *uc_line = (char*)malloc(tokenSize);

    string so_far;               /* contains alias contents as we build it */

    if (strcmp(line,"=") != 0) {
       defError(6000, "Expecting '='");
       return;
    }

    /* now keep getting lines till we get one that contains &ENDALIAS */
    for(char *p = NULL ;p == NULL;){
        int i;
        char *s = line;
        for(i=0;i<tokenSize-1;i++) {
            int ch = GETC();
            if (ch == EOF) {
                defError(6001, "End of file in &ALIAS");
                return;
            }

            *s++ = ch;
            
            if (ch == '\n') {
                print_lines(++defData->nlines);             
                break;
            }
        }

        *s = '\0';

        uc_array(line, uc_line);             /* make upper case copy */
        p = strstr(uc_line, "&ENDALIAS");    /* look for END_ALIAS */
        if (p != NULL)                       /* if we find it */
            *(line + (p - uc_line)) = 0;     /* remove it from the line */

        so_far += line;
    }

    defData->def_alias_set[aname] = so_far;

    free(aname);
    free(line);
    free(uc_line);
}

int amper_lookup(char *token);    /* forward reference to this routine */

/* The main routine called by the YACC parser to get the next token.
 *    Returns 0 if no more tokens are available.
 *    Returns an integer for keywords (see the yacc_defines.h for values)
 *    Returns the character itself for punctuation
 *    Returns NUMBER for numeric looking tokens
 *    Returns T_STRING for anything else
 * If the global "defData->dumb_mode" is > 0, it reads the next token in "dumb mode".
 * In this case, it does not do keyword lookup, or attempt to read a token
 * as a number; if the token is not punctuation, it's a T_STRING.  Each token
 * read decrements defData->dumb_mode, so you can instruct the the lexer to read the
 * next N tokens in dumb mode by setting "defData->dumb_mode" to that value.
 *
 * Newlines are in general silently ignored.  If the global defData->nl_token is
 * true, however, they are returned as the token K_NL.
 */
extern int sublex();


int yylex() {
   int v = sublex();
   if (defData->defPrintTokens) {
      if (v == 0) {
         printf("yylex NIL\n");
      } else if (v < 256) {
         printf("yylex char %c\n", v);
      } else if (v == QSTRING) {
         printf("yylex quoted string '%s'\n", yylval.string);
      } else if (v == T_STRING) {
         printf("yylex string '%s'\n", yylval.string);
      } else if (v == NUMBER) {
         printf("yylex number %f\n", yylval.dval);
      } else {
         printf("yylex keyword %s\n", defkywd(v));
      }
   }

   if ((v == 0) && (!defData->doneDesign)) {
      defError(6002, "Incomplete def file.");
      return (-1);
   }

   return v;
}

int sublex()
{
   char fc;
   double numVal;
   char*  outMsg;

   defData->pv_deftoken = (char*)realloc(defData->pv_deftoken, defData->deftokenLength);
   strcpy(defData->pv_deftoken, defData->deftoken);

   /* First, we eat all the things the parser should be unaware of.
    * This includes:
    * a) Comments
    * b) &alias definitions
    * c) &alias expansions
    */
   for(;;) {
      if(!DefGetToken(&defData->deftoken, &defData->deftokenLength)) {    /* get a raw token */
         return 0;
      }
      fc = defData->deftoken[0];
      defData->uc_token = (char*)realloc(defData->uc_token, defData->deftokenLength);

      /* defData->first, check for # comments or &alias statements.  # comments
      we ignore, and &alias statements are eaten and recorded by the
      lexer. */
      if (fc == defSettings->CommentChar) {
         // The code isn't work in correct way, no way to fix it exits 
         // but keep it for compatibility reasons. 
         int magic_count = -1;
         for(fc = GETC();; fc = GETC()) {/* so skip to the end of line */
            magic_count++;
            if ((magic_count < (int)strlen(defData->magic)) && (fc == defData->magic[magic_count])) {
              if ((int)strlen(defData->magic) == (magic_count + 1)) {
                if (defSettings->MagicCommentFoundFunction) {
                  defSettings->MagicCommentFoundFunction();
                }
              }
            }
            if (fc == EOF) return 0;
            if (fc == '\n') {
                print_lines(++defData->nlines);
                break;
            }
         }
      }
      else if (fc == '&') {
         /* begins with &.  If &alias, read contents and */
         /* store them.  Otherwise it's a define, or a macro use. */
         string alias;
         uc_array(defData->deftoken, defData->uc_token);

         if (strcmp(defData->uc_token,"&ALIAS") == 0)
            StoreAlias();    /* read and store the alias */
         else if (defGetAlias(defData->deftoken, alias))
            defData->stack[++defData->input_level] = alias;
         else
            break;    /* begins with &, but not an &alias defn. or use. */
      } else
         break;    /* does not begin with defData->commentChar or '&' */
   }

   if (defData->defInvalidChar) {
      outMsg = (char*)defMalloc(500 + strlen(defData->deftoken));
      sprintf(outMsg, "Invalid characters found in \'%s\'.\nThese characters might be using the character types other than English.\nCreate characters by specifying valid characters types.",
              defData->deftoken);
      defError(6008, outMsg);
      defFree(outMsg);
      return 0;
   }

   if (defData->doneDesign) {
      fc = EOF;
      defInfo(8000, "There are still data after the END DESIGN statement");
      return 0;    
   }

   if(fc == '\"') {
      yylval.string = ringCopy(&(defData->deftoken[1]));

      return QSTRING;
   }

   /* at this point we've read a token */
   /* printf("Token is %s\n", defData->deftoken); */
   
   // Protect the token counting variables form the decrement overflow.
   if (defData->dumb_mode >= 0) {
       defData->dumb_mode--;
   }

   if (defData->no_num >= 0) {
       defData->no_num--;
   }

   if (isdigit(fc) || fc == '.' || (fc == '-' && defData->deftoken[1] != '\0') ) {
      char *ch;
      /* 6/12/2003 - The following switching to use strtol defData->first is a fix */
      /* for FE for performance improvement. */
      /* Adding the flag "defData->parsing_property" is for pcr 594214, need to call */
      /* strtod defData->first to handle double number inside PROPERTY.  Only        */
      /* property has real number (large number) */
      if (!defData->parsing_property && !defData->real_num) {
         yylval.dval = strtol(defData->deftoken, &ch, 10); /* try string to long defData->first */
         if (defData->no_num < 0 && *ch == '\0') { /* did we use the whole string? */
            return NUMBER;
         } else {  /* failed strtol, try double */
            numVal = yylval.dval = strtod(defData->deftoken, &ch);
            if (defData->no_num < 0 && *ch == '\0') {  /* did we use the whole string? */
               /* check if the integer has exceed the limit */
               if ((numVal >= defData->lVal) && (numVal <= defData->rVal))
                  return NUMBER;    /* YES, it's really a number */
               else {
                  char* str = (char*)defMalloc(strlen(defData->deftoken)
                               +strlen(defSettings->FileName)+350);
                  sprintf(str,
                    "<Number has exceed the limit for an integer> in %s at line %s\n",
                    defSettings->FileName, lines2str(defData->nlines));
                  fflush(stdout);
                  defiError(1, 0, str);
                  defFree(str);
                  defData->errors++;
                  return NUMBER;
               }
             } else {
               yylval.string = ringCopy(defData->deftoken);  /* NO, it's a string */
               return T_STRING;
            }
         }
      } else {  /* handling PROPERTY, do strtod defData->first instead of strtol */
         numVal = yylval.dval = strtod(defData->deftoken, &ch);
         if (defData->no_num < 0 && *ch == '\0') { /* did we use the whole string? */
            /* check if the integer has exceed the limit */
            if (defData->real_num)    /* this is for PROPERTYDEF with REAL */
               return NUMBER;
            if ((numVal >= defData->lVal) && (numVal <= defData->rVal)) 
               return NUMBER;    /* YES, it's really a number */
            else {
               char* str = (char*)defMalloc(strlen(defData->deftoken)
                                +strlen(defSettings->FileName)+350);
               sprintf(str,
                 "<Number has exceed the limit for an integer> in %s at line %s\n",
                 defSettings->FileName, lines2str(defData->nlines));
               fflush(stdout);
               defiError(1, 0, str);
               defFree(str);
               defData->errors++;
               return NUMBER;
            }
         } else {  /* failed integer conversion, try floating point */
            yylval.dval = strtol(defData->deftoken, &ch, 10);
            if (defData->no_num < 0 && *ch == '\0')  /* did we use the whole string? */
               return NUMBER;
            else {
               yylval.string = ringCopy(defData->deftoken);  /* NO, it's a string */
               return T_STRING;
            }
         }
      }
   }
      
   /* if we are dumb mode, all we return is punctuation and strings & numbers*/
   /* until we see the next '+' or ';' defData->deftoken */
   if (defData->dumb_mode >= 0) {
      if (defData->deftoken[1]=='\0' && (fc=='('||fc==')'||fc=='+'||fc==';'||fc=='*')){
      
         if (fc == ';' ||  fc == '+') {
            defData->dumb_mode = 0;
            defData->no_num = 0;
         }   
         return (int)fc;
      }
      if (defData->by_is_keyword  && ((strcmp(defData->deftoken,"BY") == 0) ||
          (strcmp(defData->deftoken, "by") == 0))) {
         return K_BY; /* even in dumb mode, we must see the BY defData->deftoken */
      }
      if (defData->do_is_keyword  && ((strcmp(defData->deftoken,"DO") == 0) ||
          (strcmp(defData->deftoken, "by") == 0))) {
         return K_DO; /* even in dumb mode, we must see the DO defData->deftoken */
      }
      if (defData->new_is_keyword  && ((strcmp(defData->deftoken,"NEW") == 0) ||
          (strcmp(defData->deftoken, "new") == 0))) {
         return K_NEW; /* even in dumb mode, we must see the NEW defData->deftoken */
      }
      if (defData->nondef_is_keyword && ((strcmp(defData->deftoken, "NONDEFAULTRULE") == 0) ||
          (strcmp(defData->deftoken, "nondefaultrule") == 0))){
          return K_NONDEFAULTRULE; /* even in dumb mode, we must see the */
                                   /* NONDEFAULTRULE defData->deftoken */
      }
      if (defData->mustjoin_is_keyword && ((strcmp(defData->deftoken, "MUSTJOIN") == 0) ||
          (strcmp(defData->deftoken, "mustjoin") == 0))) {
          return K_MUSTJOIN; /* even in dumb mode, we must see the */
                             /* MUSTJOIN defData->deftoken */
      }
      if (defData->step_is_keyword  && ((strcmp(defData->deftoken,"STEP") == 0) ||
          (strcmp(defData->deftoken, "step") == 0))) {
          return K_STEP;/* even in dumb mode, we must see the STEP defData->deftoken */
      }
      if (defData->fixed_is_keyword  && ((strcmp(defData->deftoken,"FIXED") == 0) ||
          (strcmp(defData->deftoken, "fixed") == 0))) {
         return K_FIXED; /* even in dumb mode, we must see the FIXED defData->deftoken */
      }
      if (defData->cover_is_keyword  && ((strcmp(defData->deftoken,"COVER") == 0) ||
          (strcmp(defData->deftoken, "cover") == 0))) {
         return K_COVER; /* even in dumb mode, we must see the COVER defData->deftoken */
      }
      if (defData->routed_is_keyword  && ((strcmp(defData->deftoken,"ROUTED") == 0) ||
          (strcmp(defData->deftoken, "rounted") == 0))) {
         return K_ROUTED; /* even in dumb mode, we must see the */
                          /* ROUTED defData->deftoken */
      }
      
      if (defData->virtual_is_keyword && ((strcmp(defData->deftoken, "VIRTUAL") == 0 )
         || (strcmp(defData->deftoken, "virtual") == 0 ))) {
         return K_VIRTUAL;
      }
      
      if (defData->rect_is_keyword && ((strcmp(defData->deftoken, "RECT") == 0 )
         || (strcmp(defData->deftoken, "rect") == 0 ))) {
         return K_RECT;
      }
      
      if (defData->virtual_is_keyword && ((strcmp(defData->deftoken, "MASK") == 0 )
         || (strcmp(defData->deftoken, "mask") == 0 ))) {
         return K_MASK;
      }
      
      if (defData->orient_is_keyword) {
         int result;
         uc_array(defData->deftoken, defData->uc_token);

         if (defGetKeyword( defData->uc_token, &result)) {
            if (K_N == result)
                return K_N;
            if (K_W == result)
                return K_W;
            if (K_S == result)
                return K_S;
            if (K_E == result)
                return K_E;
            if (K_FN == result)
                return K_FN;
            if (K_FW == result)
                return K_FW;
            if (K_FS == result)
                return K_FS;
            if (K_FE == result)
            if (strcmp(defData->deftoken, "FE") == 0)
                return K_FE;
         }
      }
      yylval.string = ringCopy(defData->deftoken);
      return T_STRING;
   }

   /* if we get here we are in smart mode.  Parse defData->deftoken */
   /* 2/19/2004 - add _ since name can starts with '_' */
   /* 2/23/2004 - add the following characters which a name can starts with */
   /*    ! $ | : , @ ~ = < . ? { ' ^ " */
   if (isalpha(fc) || fc == '&' || fc == '_') {
      int result;

      defData->History_text.resize(0);
      uc_array(defData->deftoken, defData->uc_token);

      if (defGetKeyword(defData->uc_token, &result)) {
         if (K_HISTORY == result) {  /* history - get up to ';' */
            int n;
            int c;
            int prev;
            n = 0;
            prev = ' ';
            while (1) {
               c = GETC();
               
               if (c == EOF) {
                   defError(6015, "Unexpected end of the DEF file.");
                   break;
               }

               if (c == ';' && (prev == ' ' || prev == '\t' || prev == '\n'))
                    break;
               if (c == '\n') {
                    print_lines(++defData->nlines);
               }
               prev = c;
               defData->History_text.push_back(c);
            }
            defData->History_text.push_back('\0');
         } else if (K_BEGINEXT == result) { /* extension, get up to end */
            int nn;
            int cc;
            int foundTag = 0;
            int notEmpTag = 0;
            int begQuote = 0;
            nn = 0;
            /* First make sure there is a name after BEGINEXT within quote */
            /* BEGINEXT "name" */
            while (1) {
               cc = GETC();
               
               if (cc == EOF) {
                   defError(6015, "Unexpected end of the DEF file.");
                   break;
               }

               if (cc == '\n') {
                  if (!foundTag) {
                     defError(6003, "tag is missing for BEGINEXT");
                     break;
                  }
               } else {

                  defData->History_text.push_back(cc);
                  if (cc != ' ') {
                     if (cc == '\"') {   /* found a quote */
                        if (!begQuote)
                           begQuote = 1;
                        else if (notEmpTag) {
                           foundTag = 1;
                           break;      /* Found the quoted tag */
                        } else {
                           defError(6004, "The BEGINEXT tag is empty. Specify a value for the tag and try again.");
                           break;
                        }
                     } else if (!begQuote) {   /* anything but a quote */
                        defError(6005, "The '\"' is missing within the tag. Specify the '\"' in the tag and then try again.");
                        break;
                     }  else             /* anything but a quote and there */
                        notEmpTag = 1;   /* is already a quote */
                  }
               }
            }
            if (foundTag) {
               /* We have handle with the tag, just read the rest until */
               /* ENDEXT */
               begQuote = 0;
               while (1) {
                  cc = GETC();
               
                  if (cc == EOF) {
                      defError(6015, "Unexpected end of the DEF file.");
                      break;
                  }

                  if (cc == '\n') {
                        print_lines(++defData->nlines);
                  } else if (cc == '\"') {
                     if (!begQuote)
                        begQuote = 1;
                     else
                        begQuote = 0;
                  }

                  defData->History_text.push_back(cc);

                  int histTextSize = defData->History_text.size();

                  if (histTextSize >= 6 && memcmp(&defData->History_text[histTextSize - 6 ], "ENDEXT", 6) == 0) {
                     if (begQuote)
                        defError (6006, "The ending '\"' is missing in the tag. Specify the ending '\"' in the tag and then try again.");
                     break;
                  } else if (histTextSize >= 10 && memcmp(&defData->History_text[histTextSize - 10 ], "END DESIGN", 10) == 0) {
                     defError(6007, "The ENDEXT statement is missing in the DEF file. Include the statement and then try again.");
                     defData->nlines--;
                     break;
                  }
               }
            }
            defData->History_text.push_back('\0');
         }
         return result;        /* YES, return its value */
      } else {  /* we don't have a keyword.  */
         if (fc == '&')
         return amper_lookup(defData->deftoken);
         yylval.string = ringCopy(defData->deftoken);  /* NO, it's a string */
         return T_STRING;
      }
   } else {  /* it should be a punctuation character */
      if (defData->deftoken[1] != '\0') {
         if (strcmp(defData->deftoken, ">=") == 0) return K_GE;
         if (strcmp(defData->deftoken, "<=") == 0) return K_LE;
         if (strcmp(defData->deftoken, "<>") == 0) return K_NE;

         defError(6017, "Odd punctuation found.");
      } else if (strlen(defData->deftoken) > 2
                 || strlen(defData->deftoken) == 0) {
          defError(6017, "Odd punctuation found.");
      }
      return (int)defData->deftoken[0];
   }
}

/* We have found a defData->deftoken beginning with '&'.  If it has been previously
   defined, substitute the definition.  Otherwise return it. */
int amper_lookup(char *tkn)
{
   string   defValue;

   /* printf("Amper_lookup: %s\n", tkn); */

   /* &defines returns a T_STRING */
   if (defGetDefine(tkn, defValue)) {
      int value;
      if (defGetKeyword(defValue.c_str(), &value))
         return value;
      if (defValue.c_str()[0] == '"')
         yylval.string = ringCopy(defValue.c_str()+1);
      else
         yylval.string = ringCopy(defValue.c_str());
      return (defValue.c_str()[0] == '\"' ? QSTRING : T_STRING);
   }
   /* if none of the above, just return the defData->deftoken. */
   yylval.string = ringCopy(tkn);
   return T_STRING;
}

void defError(int msgNum, const char *s) {
   char* str;
   const char  *curToken = isgraph(defData->deftoken[0]) ? defData->deftoken
                                                              : "<unprintable>";
   const char  *pvToken = isgraph(defData->pv_deftoken[0]) ? defData->pv_deftoken
                                                           : "<unprintable>";
   int len = strlen(curToken)-1;
   int pvLen = strlen(pvToken)-1;

   if ((defSettings->totalDefMsgLimit > 0) && (defData->defMsgPrinted >= defSettings->totalDefMsgLimit))
      return;
   if (defSettings->MsgLimit[msgNum-5000] > 0) {
      if (defData->msgLimit[msgNum-5000] >= defSettings->MsgLimit[msgNum-5000])
         return;             /* over the limit */
      defData->msgLimit[msgNum-5000] = defData->msgLimit[msgNum-5000] + 1;
   }

   /* PCR 690679, probably missing space before a ';' */
   if (strcmp(s, "parse error") == 0) {
      if ((len > 1) && (defData->deftoken[len] == ';')) {
         str = (char*)defMalloc(len + strlen(s) + strlen(defSettings->FileName) + 350);
         sprintf(str, "ERROR (DEFPARS-%d): %s, file %s at line %s\nLast token was <%s>, space is missing before <;>\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines), curToken);
      } else if ((pvLen > 1) && (defData->pv_deftoken[pvLen] == ';')) {
         str = (char*)defMalloc(pvLen + strlen(s) + strlen(defSettings->FileName) + 350);
         sprintf(str, "ERROR (DEFPARS-%d): %s, file %s at line %s\nLast token was <%s>, space is missing before <;>\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines-1), pvToken);
      } else {
         str = (char*)defMalloc(len + strlen(defSettings->FileName) + 350);
         sprintf(str, "ERROR (DEFPARS-%d): Def parser has encountered an error in file %s at line %s, on token %s.\nProblem can be syntax error on the def file or an invalid parameter name.\nDouble check the syntax on the def file with the LEFDEF Reference Manual.\n",
              msgNum, defSettings->FileName, lines2str(defData->nlines), curToken);
      }
   } else if (strcmp(s, "syntax error") == 0) {
      if ((len > 1) && (defData->deftoken[len] == ';')) {
         str = (char*)defMalloc(len + strlen(s) + strlen(defSettings->FileName) + 350);
         sprintf(str, "ERROR (DEFPARS-%d): %s, file %s at line %s\nLast token was <%s>, space is missing before <;>\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines), curToken);
      } else if ((pvLen > 1) && (defData->pv_deftoken[pvLen] == ';')) {
         str = (char*)defMalloc(pvLen + strlen(s) + strlen(defSettings->FileName) + 350);
         sprintf(str, "ERROR (DEFPARS-%d): %s, file %s at line %s\nLast token was <%s>, space is missing before <;>\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines-1), pvToken);
      } else {
         str = (char*)defMalloc(len + strlen(defSettings->FileName) + 350);
         sprintf(str, "ERROR (DEFPARS-%d): Def parser has encountered an error in file %s at line %s, on token %s.\nProblem can be syntax error on the def file or an invalid parameter name.\nDouble check the syntax on the def file with the LEFDEF Reference Manual.\n",
              msgNum, defSettings->FileName, lines2str(defData->nlines), curToken);
      }
   } else {
      str = (char*)defMalloc(len + strlen(s) + strlen(defSettings->FileName) + 350);
      sprintf(str, "ERROR (DEFPARS-%d): %s Error in file %s at line %s, on token %s.\nUpdate the def file before parsing the file again.\n",
           msgNum, s, defSettings->FileName, lines2str(defData->nlines), curToken);
   }

   fflush(stdout);
   defiError(1, msgNum, str);
   defFree(str);
   defData->errors++;
}

/* yydeferror is called by bison.simple */
void yyerror(const char *s) {

   defError(defData->defMsgCnt++, s);
}

/* All info starts with 8000 */
/* All info within defInfo starts with 8500 */
void defInfo(int msgNum, const char *s) {
   int i;

   for (i = 0; i <defSettings->nDDMsgs; i++) {  /* check if info has been disable */
      if (defSettings->disableDMsgs[i] == msgNum)
         return;  /* don't print out any info since msg has been disabled */
   }

   if (defSettings->WarningLogFunction) {
      char* str = (char*)defMalloc(strlen(defData->deftoken)+strlen(s)
                                   +strlen(defSettings->FileName)+350);
      sprintf(str, "INFO (DEFPARS-%d): %s See file %s at line %s.\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines));
      (*defSettings->WarningLogFunction)(str);
      defFree(str);
   } else if (defData->defrLog) {
      fprintf(defData->defrLog, "INFO (DEFPARS-%d): %s See file %s at line %s\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines));
   } else {
      if (!defData->hasOpenedDefLogFile) {
         if ((defData->defrLog = fopen("defRWarning.log", "w")) == 0) {
            printf("WARNING(DEFPARS-8500): Unable to open the file defRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Info messages will not be printed.\n");
         } else {
            defData->hasOpenedDefLogFile = 1;
            fprintf(defData->defrLog, "Info from file: %s\n\n", defSettings->FileName);
            fprintf(defData->defrLog, "INFO (DEFPARS-%d): %s See file %s at line %s\n",
                    msgNum, s, defSettings->FileName, lines2str(defData->nlines));
         }
      } else {
         if ((defData->defrLog = fopen("defRWarning.log", "a")) == 0) {
            printf("WARNING (DEFPARS-8500): Unable to open the file defRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Info messages will not be printed.\n");
         } else {
            defData->hasOpenedDefLogFile = 1;
            fprintf(defData->defrLog, "\nInfo from file: %s\n\n", defSettings->FileName);
            fprintf(defData->defrLog, "INFO (DEFPARS-%d): %s See file %s at line %s\n",
                    msgNum, s, defSettings->FileName, lines2str(defData->nlines));
         }
      } 
   }
}

/* All warning starts with 7000 */
/* All warning within defWarning starts with 7500 */
void defWarning(int msgNum, const char *s) {
   int i;

   for (i = 0; i <defSettings->nDDMsgs; i++) {  /* check if warning has been disable */
      if (defSettings->disableDMsgs[i] == msgNum)
         return;  /* don't print out any warning since msg has been disabled */
   }

   if (defSettings->WarningLogFunction) {
      char* str = (char*)defMalloc(strlen(defData->deftoken)+strlen(s)
                                   +strlen(defSettings->FileName)+350);
      sprintf(str, "WARNING (DEFPARS-%d): %s See file %s at line %s.\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines));
      (*defSettings->WarningLogFunction)(str);
      defFree(str);
   } else if (defData->defrLog) {
      fprintf(defData->defrLog, "WARNING (DEFPARS-%d): %s See file %s at line %s\n",
              msgNum, s, defSettings->FileName, lines2str(defData->nlines));
   } else {
      if (!defData->hasOpenedDefLogFile) {
         if ((defData->defrLog = fopen("defRWarning.log", "w")) == 0) {
            printf("WARNING (DEFPARS-7500): Unable to open the file defRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Warning messages will not be printed.\n");
         } else {
            defData->hasOpenedDefLogFile = 1;
            fprintf(defData->defrLog, "Warnings from file: %s\n\n", defSettings->FileName);
            fprintf(defData->defrLog, "WARNING (DEFPARS-%d): %s See file %s at line %s\n",
                    msgNum, s, defSettings->FileName, lines2str(defData->nlines));
         }
      } else {
         if ((defData->defrLog = fopen("defRWarning.log", "a")) == 0) {
            printf("WARNING (DEFAPRS-7501): Unable to open the file defRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Warning messages will not be printed.\n");
         } else {
            defData->hasOpenedDefLogFile = 1;
            fprintf(defData->defrLog, "\nWarnings from file: %s\n\n", defSettings->FileName);
            fprintf(defData->defrLog, "WARNING (DEFPARS-%d): %s See file %s at line %s\n",
                    msgNum, s, defSettings->FileName, lines2str(defData->nlines));
         }
      } 
   }
   defData->def_warnings++;
}

void * defMalloc(size_t def_size) {
   void * mallocVar;
   if (defSettings->MallocFunction)
      mallocVar =  (*defSettings->MallocFunction)(def_size);
   else
      mallocVar = (void*)malloc(def_size);
   if (!mallocVar) {
      fprintf (stderr,
        "<Not enough memory, parser exits!> in %s at line %s\n",
        defSettings->FileName, lines2str(defData->nlines));
      exit (1);
   }
   return mallocVar;
}

void * defRealloc(void *name, size_t def_size) {
   if (defSettings->ReallocFunction)
      return (*defSettings->ReallocFunction)(name, def_size);
   else
      return (void*)realloc(name, def_size);
}


void defFree(void *name) {
   if (defSettings->FreeFunction)
      (*defSettings->FreeFunction)(name);
   else
      free(name);
}

char* addr(const char* in) {
   return (char*)in;
}

char* defkywd(int num)
{
   char* a;
   switch (num) {
      case QSTRING: a = addr("QSTRING"); break;
      case T_STRING: a = addr("T_STRING"); break;
      case SITE_PATTERN: a = addr("SITE_PATTERN"); break;
      case NUMBER: a = addr("NUMBER"); break;
      case K_ALIGN: a = addr("ALIGN"); break;
      case K_AND: a = addr("AND"); break;
      case K_ARRAY: a = addr("ARRAY"); break;
      case K_ASSERTIONS: a = addr("ASSERTIONS"); break;
      case K_BEGINEXT: a = addr("BEGINEXT"); break;
      case K_BOTTOMLEFT: a = addr("BOTTOMLEFT"); break;
      case K_BUSBITCHARS: a = addr("BUSBITCHARS"); break;
      case K_BY: a = addr("BY"); break;
      case K_CANNOTOCCUPY: a = addr("CANNOTOCCUPY"); break;
      case K_CANPLACE: a = addr("CANPLACE"); break;
      case K_CAPACITANCE: a = addr("CAPACITANCE"); break;
      case K_COMMONSCANPINS: a = addr("COMMONSCANPINS"); break;
      case K_COMPONENT: a = addr("COMPONENT"); break;
      case K_COMPONENTPIN: a = addr("COMPONENTPIN"); break;
      case K_COMPS: a = addr("COMPS"); break;
      case K_COMP_GEN: a = addr("COMP_GEN"); break;
      case K_CONSTRAINTS: a = addr("CONSTRAINTS"); break;
      case K_COVER: a = addr("COVER"); break;
      case K_CUTSIZE: a = addr("CUTSIZE"); break;
      case K_CUTSPACING: a = addr("CUTSPACING"); break;
      case K_DEFAULTCAP: a = addr("DEFAULTCAP"); break;
      case K_DEFINE: a = addr("DEFINE"); break;
      case K_DEFINES: a = addr("DEFINES"); break;
      case K_DEFINEB: a = addr("DEFINEB"); break;
      case K_DESIGN: a = addr("DESIGN"); break;
      case K_DESIGNRULEWIDTH: a = addr("DESIGNRULEWIDTH"); break;
      case K_DIAGWIDTH: a = addr("DIAGWIDTH"); break;
      case K_DIEAREA: a = addr("DIEAREA"); break;
      case K_DIFF: a = addr("DIFF"); break;
      case K_DIRECTION: a = addr("DIRECTION"); break;
      case K_DIST: a = addr("DIST"); break;
      case K_DISTANCE: a = addr("DISTANCE"); break;
      case K_DIVIDERCHAR: a = addr("DIVIDERCHAR"); break;
      case K_DO: a = addr("DO"); break;
      case K_DRIVECELL: a = addr("DRIVECELL"); break;
      case K_E: a = addr("E"); break;
      case K_EEQMASTER: a = addr("EEQMASTER"); break;
      case K_ELSE: a = addr("ELSE"); break;
      case K_ENCLOSURE: a = addr("ENCLOSURE"); break;
      case K_END: a = addr("END"); break;
      case K_ENDEXT: a = addr("ENDEXT"); break;
      case K_EQ: a = addr("EQ"); break;
      case K_EQUAL: a = addr("EQUAL"); break;
      case K_ESTCAP: a = addr("ESTCAP"); break;
      case K_FE: a = addr("FE"); break;
      case K_FALL: a = addr("FALL"); break;
      case K_FALLMAX: a = addr("FALLMAX"); break;
      case K_FALLMIN: a = addr("FALLMIN"); break;
      case K_FALSE: a = addr("FALSE"); break;
      case K_FIXED: a = addr("FIXED"); break;
      case K_FLOATING: a = addr("FLOATING"); break;
      case K_FLOORPLAN: a = addr("FLOORPLAN"); break;
      case K_FN: a = addr("FN"); break;
      case K_FOREIGN: a = addr("FOREIGN"); break;
      case K_FPC: a = addr("FPC"); break;
      case K_FROMCLOCKPIN: a = addr("FROMCLOCKPIN"); break;
      case K_FROMCOMPPIN: a = addr("FROMCOMPPIN"); break;
      case K_FROMPIN: a = addr("FROMPIN"); break;
      case K_FROMIOPIN: a = addr("FROMIOPIN"); break;
      case K_FS: a = addr("FS"); break;
      case K_FW: a = addr("FW"); break;
      case K_GCELLGRID: a = addr("GCELLGRID"); break;
      case K_GE: a = addr("GE"); break;
      case K_GT: a = addr("GT"); break;
      case K_GROUND: a = addr("GROUND"); break;
      case K_GROUNDSENSITIVITY: a = addr("GROUNDSENSITIVITY"); break;
      case K_GROUP: a = addr("GROUP"); break;
      case K_GROUPS: a = addr("GROUPS"); break;
      case K_HALO: a = addr("HALO"); break;
      case K_HARDSPACING: a = addr("HARDSPACING"); break;
      case K_HISTORY: a = addr("HISTORY"); break;
      case K_HOLDRISE: a = addr("HOLDRISE"); break;
      case K_HOLDFALL: a = addr("HOLDFALL"); break;
      case K_HORIZONTAL: a = addr("HORIZONTAL"); break;
      case K_IF: a = addr("IF"); break;
      case K_IN: a = addr("IN"); break;
      case K_INTEGER: a = addr("INTEGER"); break;
      case K_IOTIMINGS: a = addr("IOTIMINGS"); break;
      case K_LAYER: a = addr("LAYER"); break;
      case K_LAYERS: a = addr("LAYERS"); break;
      case K_LE: a = addr("LE"); break;
      case K_LT: a = addr("LT"); break;
      case K_MACRO: a = addr("MACRO"); break;
      case K_MASK: a = addr("MASK"); break;
      case K_MAX: a = addr("MAX"); break;
      case K_MAXDIST: a = addr("MAXDIST"); break;
      case K_MAXHALFPERIMETER: a = addr("MAXHALFPERIMETER"); break;
      case K_MAXX: a = addr("MAXX"); break;
      case K_MAXY: a = addr("MAXY"); break;
      case K_MICRONS: a = addr("MICRONS"); break;
      case K_MIN: a = addr("MIN"); break;
      case K_MINCUTS: a = addr("MINCUTS"); break;
      case K_MINPINS: a = addr("MINPINS"); break;
      case K_MUSTJOIN: a = addr("MUSTJOIN"); break;
      case K_N: a = addr("N"); break;
      case K_NAMESCASESENSITIVE: a = addr("NAMESCASESENSITIVE"); break;
      case K_NAMEMAPSTRING: a = addr("NAMEMAPSTRING"); break;
      case K_NE: a = addr("NE"); break;
      case K_NET: a = addr("NET"); break;
      case K_NETEXPR: a = addr("NETEXPR"); break;
      case K_NETLIST: a = addr("NETLIST"); break;
      case K_NETS: a = addr("NETS"); break;
      case K_NEW: a = addr("NEW"); break;
      case K_NONDEFAULTRULE: a = addr("NONDEFAULTRULE"); break;
      case K_NOSHIELD: a= addr("NOSHIELD"); break;
      case K_NOT: a = addr("NOT"); break;
      case K_OFF: a = addr("OFF"); break;
      case K_OFFSET: a = addr("OFFSET"); break;
      case K_ON: a = addr("ON"); break;
      case K_OR: a = addr("OR"); break;
      case K_ORDERED: a = addr("ORDERED"); break;
      case K_ORIGIN: a = addr("ORIGIN"); break;
      case K_ORIGINAL: a = addr("ORIGINAL"); break;
      case K_OUT: a = addr("OUT"); break;
      case K_PARALLEL: a = addr("PARALLEL"); break;
      case K_PARTITIONS: a = addr("PARTITIONS"); break;
      case K_PATH: a = addr("PATH"); break;
      case K_PATTERN: a = addr("PATTERN"); break;
      case K_PATTERNNAME: a = addr("PATTERNNAME"); break;
      case K_PINPROPERTIES: a = addr("PINPROPERTIES"); break;
      case K_PINS: a = addr("PINS"); break;
      case K_PLACED: a = addr("PLACED"); break;
      case K_PIN: a = addr("PIN"); break;
      case K_POLYGON: a = addr("POLYGON"); break;
      case K_PROPERTY: a = addr("PROPERTY"); break;
      case K_PROPERTYDEFINITIONS: a = addr("PROPERTYDEFINITIONS"); break;
      case K_RANGE: a = addr("RANGE"); break;
      case K_REAL: a = addr("REAL"); break;
      case K_RECT: a = addr("RECT"); break;
      case K_REENTRANTPATHS: a = addr("REREENTRANTPATHS"); break;
      case K_REGION: a = addr("REGION"); break;
      case K_REGIONS: a = addr("REGIONS"); break;
      case K_RISE: a = addr("RISE"); break;
      case K_RISEMAX: a = addr("RISEMAX"); break;
      case K_RISEMIN: a = addr("RISEMIN"); break;
      case K_ROUTED: a = addr("ROUTED"); break;
      case K_ROW: a = addr("ROW"); break;
      case K_ROWCOL: a = addr("ROWCOL"); break;
      case K_ROWS: a = addr("ROWS"); break;
      case K_S: a = addr("S"); break;
      case K_SCANCHAINS: a = addr("SCANCHAINS"); break;
      case K_SETUPRISE: a = addr("SETUPRISE"); break;
      case K_SETUPFALL: a = addr("SETUPFALL"); break;
      case K_SHAPE: a = addr("SHAPE"); break;
      case K_SITE: a = addr("SITE"); break;
      case K_SLEWRATE: a = addr("SLEWRATE"); break;
      case K_SNET: a = addr("SNET"); break;
      case K_SNETS: a = addr("SNETS"); break;
      case K_SOURCE: a = addr("SOURCE"); break;
      case K_SOFT: a = addr("SOFT"); break;
      case K_SPACING: a = addr("SPACING"); break;
      case K_SPECIAL: a = addr("SPECIAL"); break;
      case K_START: a = addr("START"); break;
      case K_START_NET: a = addr("START_NET"); break;
      case K_STEP: a = addr("STEP"); break;
      case K_STRING: a = addr("STRING"); break;
      case K_STOP: a = addr("STOP"); break;
      case K_SUBNET: a = addr("SUBNET"); break;
      case K_SUM: a = addr("SUM"); break;
      case K_SUPPLYSENSITIVITY: a = addr("SUPPLYSENSITIVITY"); break;
      case K_STYLE: a = addr("STYLE"); break;
      case K_STYLES: a = addr("STYLES"); break;
      case K_SYNTHESIZED: a = addr("SYNTHESIZED"); break;
      case K_TAPER: a = addr("TAPER"); break;
      case K_TAPERRULE: a = addr("TAPERRULE"); break;
      case K_THEN: a = addr("THEN"); break;
      case K_THRUPIN: a = addr("THRUPIN"); break;
      case K_TIMING: a = addr("TIMING"); break;
      case K_TIMINGDISABLES: a = addr("TIMINGDISABLES"); break;
      case K_TOCLOCKPIN: a = addr("TOCLOCKPIN"); break;
      case K_TOCOMPPIN: a = addr("TOCOMPPIN"); break;
      case K_TOIOPIN: a = addr("TOIOPIN"); break;
      case K_TOPIN: a = addr("TOPIN"); break;
      case K_TOPRIGHT: a = addr("TOPRIGHT"); break;
      case K_TRACKS: a = addr("TRACKS"); break;
      case K_TRUE: a = addr("TRUE"); break;
      case K_TURNOFF: a = addr("TURNOFF"); break;
      case K_VARIABLE: a = addr("VARIABLE"); break;
      case K_VIA: a = addr("VIA"); break;
      case K_VIARULE: a = addr("VIARULE"); break;
      case K_VIAS: a = addr("VIAS"); break;
      case K_VOLTAGE: a = addr("VOLTAGE"); break;
      case K_TECH: a = addr("TECH"); break;
      case K_UNITS: a = addr("UNITS"); break;
      case K_UNPLACED: a = addr("UNPLACED"); break;
      case K_USE: a = addr("USE"); break;
      case K_USER: a = addr("USER"); break;
      case K_VERSION: a = addr("VERSION"); break;
      case K_VIRTUAL: a = addr("VIRTUAL"); break;
      case K_VERTICAL: a = addr("VERTICAL"); break;
      case K_VPIN: a = addr("VPIN"); break;
      case K_W: a = addr("W"); break;
      case K_WIRECAP: a = addr("WIRECAP"); break;
      case K_WEIGHT: a = addr("WEIGHT"); break;
      case K_WIDTH: a = addr("WIDTH"); break;
      case K_WIREDLOGIC: a = addr("WIREDLOGIC"); break;
      case K_WIREEXT: a = addr("WIREEXT"); break;
      case K_XTALK: a = addr("XTALK"); break;
      case K_X: a = addr("X"); break;
      case K_Y: a = addr("Y"); break;
      default: a = addr("bogus");
   }
   return a;
}

const char* DEFCASE(const char* ch)
{
    return defData->names_case_sensitive ? ch : upperCase(ch);
}

END_LEFDEF_PARSER_NAMESPACE





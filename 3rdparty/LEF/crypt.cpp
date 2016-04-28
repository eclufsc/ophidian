// *****************************************************************************
// *****************************************************************************
// Copyright 2012 - 2013, Cadence Design Systems
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
//  $Revision: #1 $
//  $Date: 2014/02/10 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

/*
 * FILE: crypt.c
 *     Simple encryption/decryption.
 *     To build a encrypted-to-ascii/ascii-to-encrypted executable, type
 *         cc -DENCRYPT_BUILD crypt.c -o crypt
 *
 */

#include <stdio.h>
#include <stdarg.h>

#include "lefiKRDefs.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

#ifdef WIN32
#   include <io.h>
#else // not WIN32 
#   include <unistd.h>

#endif // WIN32 

#define SEED        7892393

#define BUF_SIZE_BIT    10
#define BUF_SIZE    (1 << BUF_SIZE_BIT)
#define BUF_SIZE_MASK    (BUF_SIZE - 1)

#define MAGIC_0        0xf3
#define MAGIC_1        0xe0
#define MAGIC_2        0x8a
#define MAGIC_3        0xbc

#define RAND         ((seed = seed * 1103515267 + 12347) & BUF_SIZE_MASK)

static int seed, beginning, encrypted, index;
static unsigned char buffer[BUF_SIZE];
static short map[BUF_SIZE];

FILE *
encOpenFileForRead(char *filename)
{
    beginning = 1;
    return fopen(filename, "r");
}

FILE *
encOpenFileForWrite(char    *filename,
                    int     encrypt_f)
{
    beginning = encrypt_f ? -1 : 1;
    return fopen(filename, "w");
}

int
encCloseFile(FILE *fp)
{
    if (encrypted < 0 && index > 0)
        write(fileno(fp), (char *) buffer, BUF_SIZE);
    return fclose(fp);
}

void
encClearBuf(FILE *fp)
{
    if (encrypted && index > 0)
        write(fileno(fp), (char *) buffer, BUF_SIZE);
}

/* We have already determined that the input file is encrypted (Probably
 * from a call to encIsEncrypted below).  Set the local static variables
 * so the the next buffer we read will be decrypted.
 */
void
encReadingEncrypted()
{
    int i;

    beginning = 0;
    encrypted = 1;
    index = 0;
    seed = SEED;
    for (i = 0; i < BUF_SIZE; i++)
        map[i] = i;
}

/* Need to set the encrypted flag to 1
 */
void
encWritingEncrypted()
{
    beginning = 1;
    encrypted = 1;
}

// Test a buffer (already read in). To see if it is encrypted 
int
encIsEncrypted(unsigned char *buf)
{
    int enc = ((
               buf[0] != (unsigned char) MAGIC_0 ||
               buf[1] != (unsigned char) MAGIC_1 ||
               buf[2] != (unsigned char) MAGIC_2 ||
               buf[3] != (unsigned char) MAGIC_3) ? 0 : 1);

    if (enc)
        encReadingEncrypted();

    return enc;
}

int
encFgetc(FILE *fp)
{
    int i, n, m;

    if (beginning) {
        n = fread((char *) buffer, 1, 4, fp);
        encrypted = (n == 4 && encIsEncrypted(buffer)) ? 1 : 0;
        if (encrypted == 0) {
            rewind(fp);
        }
    }
    if (encrypted) {
        if (!index) {
            for (i = 0; i < BUF_SIZE; i++) {
                n = RAND;
                m = map[i];
                map[i] = map[n];
                map[n] = m;
                buffer[i] = 0xff;
            }
            n = fread((char *) buffer, 1, BUF_SIZE, fp);
            if (!n)
                return EOF;
        }
        n = (int) ((~buffer[map[index++]]) & 0xff);
        seed += n;
        if (index >= BUF_SIZE)
            index = 0;
        return n ? n : EOF;
    } else {
        return fgetc(fp);
    }
}

int
encFputc(char   c,
         FILE   *fp)
{
    int i, n, m;

    if (beginning) {
        // encrypted = (beginning < 0) ? -1 : 0;
        seed = SEED;
        beginning = index = 0;
        if (encrypted) {
            buffer[0] = (unsigned char) MAGIC_0;
            buffer[1] = (unsigned char) MAGIC_1;
            buffer[2] = (unsigned char) MAGIC_2;
            buffer[3] = (unsigned char) MAGIC_3;
            write(fileno(fp), (char *) buffer, 4);
            for (i = 0; i < BUF_SIZE; i++)
                map[i] = i;
        }
    }
    if (encrypted) {
        if (!index) {
            for (i = 0; i < BUF_SIZE; i++) {
                n = RAND;
                m = map[i];
                map[i] = map[n];
                map[n] = m;
                buffer[i] = 0xff;
            }
        }
        seed += c;
        buffer[map[index++]] = (~((unsigned char) c)) & 0xff;
        if (index >= BUF_SIZE) {
            index = 0;
            write(fileno(fp), (char *) buffer, BUF_SIZE);
        }
        return c;
    } else {
        return fputc(c, fp);
    }
}

void
encPrint(FILE   *fp,
         char   *format,
         ...)
{
    va_list ap;
    char    *s;
    char    buf[BUF_SIZE];

    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);

    for (s = buf; *s; s++)
        encFputc(*s, fp);
    return;
}

END_LEFDEF_PARSER_NAMESPACE


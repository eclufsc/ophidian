// *****************************************************************************
// *****************************************************************************
// Copyright 2013-2014, Cadence Design Systems
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
//  $Author: arakhman $
//  $Revision: #6 $
//  $Date: 2013/08/09 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "defrData.hpp"
#include "defrSettings.hpp"

using namespace std;

BEGIN_LEFDEF_PARSER_NAMESPACE

extern void *defMalloc(size_t def_size);
extern void defFree(void *name);

defrData *defData = NULL;

defrData::defrData()
: Subnet(0),
  aOxide(0),
  assertionWarnings(0),
  bit_is_keyword(0),
  bitsNum(0),
  blockageWarnings(0),
  by_is_keyword(0),
  caseSensitiveWarnings(0),
  ch(NULL),
  componentWarnings(0),
  constraintWarnings(0),
  cover_is_keyword(0),

  defIgnoreVersion(0),
  defInPropDef(0),
  defInvalidChar(0),
  defMsg(NULL),
  defMsgCnt(5500),
  defMsgPrinted(0),
  defPrintTokens(0),
  defPropDefType('\0'),
  defRetVal(0),
  def_warnings(0),
  defaultCapWarnings(0),
  defrLog(0),
  do_is_keyword(0),
  dumb_mode(0),
  errors(0),
  fillWarnings(0),
  first_buffer(0),
  fixed_is_keyword(0),
  gcellGridWarnings(0),
  hasBlkLayerComp(0),
  hasBlkLayerSpac(0),
  hasBlkLayerTypeComp(0),
  hasBlkPlaceComp(0),
  hasBlkPlaceTypeComp(0),
  hasBusBit(0),
  hasDes(0),
  hasDivChar(0),
  hasDoStep(0),
  hasNameCase(0),
  hasOpenedDefLogFile(0),
  hasPort(0),
  hasVer(0),
  iOTimingWarnings(0),
  input_level(-1),
  last(NULL),
  magic((char*)malloc(1)),
  mask_is_keyword(0),
  mustjoin_is_keyword(0),
  names_case_sensitive(1),
  needNPCbk(0),
  needSNPCbk(0),
  netOsnet(0),
  netWarnings(0),
  new_is_keyword(0),
  next(NULL),
  nlines(1),
  nl_token(FALSE),
  no_num(0),
  nonDefaultWarnings(0),
  nondef_is_keyword(0),
  ntokens(0),
  orient_is_keyword(0),
  parsing_property(0),
  pinExtWarnings(0),
  pinWarnings(0),
  real_num(0),
  rect_is_keyword(0),
  regTypeDef(0),
  regionWarnings(0),
  ringPlace(0),
  routed_is_keyword(0),
  rowName(NULL),
  rowWarnings(0),
  sNetWarnings(0),
  save_x(0.0),
  save_y(0.0),
  scanchainWarnings(0),
  shield(FALSE),
  shieldName(NULL),
  shiftBuf(0),
  shiftBufLength(0),
  specialWire_mask(0),
  step_is_keyword(0),
  stylesWarnings(0),
  trackWarnings(0),
  unitsWarnings(0),
  versionWarnings(0),
  viaRule(0),
  viaWarnings(0),
  virtual_is_keyword(0),
  deftokenLength(TOKEN_SIZE),
  warningMsg(NULL),
  specialWire_routeStatus((char*) "ROUTED"),
  specialWire_routeStatusName((char *)""),
  specialWire_shapeType((char*)""),
  VersionNum(5.7),
  xStep(0),
  yStep(0),
  lVal(0.0),
  rVal(0.0),
  // defrReader vars
  Prop(),
  Site(),
  Component(),
  ComponentMaskShiftLayer(),
  Net(),
  PinCap(),
  CannotOccupy(),
  Canplace(),
  DieArea(),
  Pin(),
  Row(),
  Track(),
  GcellGrid(),
  Via(),
  Region(),
  Group(),
  Assertion(),
  Scanchain(),
  IOTiming(),
  FPC(),
  TimingDisable(),
  Partition(),
  PinProp(),
  Blockage(),
  Slot(),
  Fill(),
  NonDefault(),
  Styles(),
  doneDesign(0),
  NeedPathData(0),
  deftoken((char*)defMalloc(TOKEN_SIZE)),
  uc_token((char*)defMalloc(TOKEN_SIZE)),
  pv_deftoken((char*)defMalloc(TOKEN_SIZE))
{
    magic[0] = '\0';
    deftoken[0] = '\0';
    History_text.push_back('\0');

    memset(msgLimit, 0, DEF_MSGS * sizeof(int));
    memset(buffer, 0, IN_BUF_SIZE * sizeof(char));
    memset(ring, 0, RING_SIZE * sizeof(char*));
    memset(ringSizes, 0, RING_SIZE * sizeof(int));

    // lex_init
    struct stat statbuf;

    // initRingBuffer
    int i;
    ringPlace = 0;
    for (i = 0; i < RING_SIZE; i++) {
        ring[i] = (char*)defMalloc(TOKEN_SIZE);
        ringSizes[i] = TOKEN_SIZE;
    }

    nlines = 1;
    last = buffer-1;
    next = buffer;
    first_buffer = 1;

    /* 4/11/2003 - Remove file lefrRWarning.log from directory if it exist */
    /* pcr 569729 */
    if (stat("defRWarning.log", &statbuf) != -1) {
        /* file exist, remove it */
        if (!defSettings->LogFileAppend)
            remove("defRWarning.log");
    }

    lVal = strtod("-2147483648", &ch);
    rVal = strtod("2147483647", &ch);
  }

  
  defrData::~defrData()
  {
      // lex_un_init.
      /* Close the file */
      if (defrLog) {
          fclose(defrLog);
          defrLog = 0;
      }

      free(deftoken);
      free(uc_token);
      free(pv_deftoken);
      free(magic);

      // freeRingBuffer.
      int i;

      ringPlace = 0;
      for (i = 0; i < RING_SIZE; i++) {
          defFree(ring[i]);
      }
  }

void
defrData::reset()
{
    delete defData;
    defData = new defrData();
}


END_LEFDEF_PARSER_NAMESPACE


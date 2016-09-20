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
//  $Revision: #7 $
//  $Date: 2015/01/27 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include <stdlib.h>
#include "lex.h"
#include "defiDebug.hpp"
#include "defiPinProp.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//        defiPinProp
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////



defiPinProp::defiPinProp() {
  Init();
}


void defiPinProp::Init() {
  instName_ = (char*)defMalloc(16);
  pinName_ = (char*)defMalloc(16);
  pinNameSize_ = 16;
  instNameSize_ = 16;
  isPin_ = 0;
  numProps_ = 0;
  propsAllocated_ = 2;
  propNames_   = (char**)defMalloc(sizeof(char*)*2);
  propValues_  = (char**)defMalloc(sizeof(char*)*2);
  propDValues_ = (double*)defMalloc(sizeof(double)*2);
  propTypes_   = (char*)defMalloc(sizeof(char)*2);
}


defiPinProp::~defiPinProp() {
  Destroy();
}


void defiPinProp::Destroy() {
  clear();
  defFree(instName_);
  defFree(pinName_);
  defFree((char*)(propNames_));
  defFree((char*)(propValues_));
  defFree((char*)(propDValues_));
  defFree((char*)(propTypes_));
}


void defiPinProp::clear() {
  int i;
  for (i = 0; i < numProps_; i++) {
    defFree(propNames_[i]);
    defFree(propValues_[i]);
    propDValues_[i] = 0;
  }
  numProps_ = 0;
  isPin_ = 0;
}


void defiPinProp::setName(const char* inst, const char* pin) {
  int len = strlen(inst) + 1;

  if ((strcmp(inst, "PIN") == 0) || (strcmp(inst,"pin") == 0)) {
     isPin_ = 1;
     instName_[0] = '\0'; /* make sure to clear any prev inst */
  } else {
     if (instNameSize_ < len) {
       instNameSize_ = len;
       defFree(instName_);
       instName_ = (char*)defMalloc(len);
     }
     strcpy(instName_, DEFCASE(inst));
  }

  len = strlen(pin) + 1;
  if (pinNameSize_ < len) {
    pinNameSize_ = len;
    defFree(pinName_);
    pinName_ = (char*)defMalloc(len);
  }
  strcpy(pinName_, DEFCASE(pin));
}


int defiPinProp::isPin() const {
  return isPin_ ? 1 : 0;
}


const char* defiPinProp::instName() const {
  return instName_;
}


const char* defiPinProp::pinName() const {
  return pinName_;
}


void defiPinProp::addProperty(const char* name, const char* value,
                              const char type) {
  int len;
  if (numProps_ == propsAllocated_) {
    int i;
    char**  nn;
    char**  nv;
    double* nd;
    char*   nt;

    propsAllocated_ *= 2;
    nn = (char**)defMalloc(sizeof(char*)*propsAllocated_);
    nv = (char**)defMalloc(sizeof(char*)*propsAllocated_);
    nd = (double*)defMalloc(sizeof(double)*propsAllocated_);
    nt = (char*)defMalloc(sizeof(char)*propsAllocated_);
    for (i = 0; i < numProps_; i++) {
      nn[i] = propNames_[i];
      nv[i] = propValues_[i];
      nd[i] = propDValues_[i];
      nt[i] = propTypes_[i];
    }
    defFree((char*)(propNames_));
    defFree((char*)(propValues_));
    defFree((char*)(propDValues_));
    defFree((char*)(propTypes_));
    propNames_   = nn;
    propValues_  = nv;
    propDValues_ = nd;
    propTypes_   = nt;
  }
  len = strlen(name) + 1;
  propNames_[numProps_] = (char*)defMalloc(len);
  strcpy(propNames_[numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  propValues_[numProps_] = (char*)defMalloc(len);
  strcpy(propValues_[numProps_], DEFCASE(value));
  propDValues_[numProps_] = 0;
  propTypes_[numProps_] = type;
  numProps_ += 1;
}


void defiPinProp::addNumProperty(const char* name, const double d,
                                 const char* value, const char type) {
  int len;
  if (numProps_ == propsAllocated_) {
    int i;
    char**  nn;
    char**  nv;
    double* nd;
    char*   nt;

    propsAllocated_ *= 2;
    nn = (char**)defMalloc(sizeof(char*)*propsAllocated_);
    nv = (char**)defMalloc(sizeof(char*)*propsAllocated_);
    nd = (double*)defMalloc(sizeof(double)*propsAllocated_);
    nt = (char*)defMalloc(sizeof(char)*propsAllocated_);
    for (i = 0; i < numProps_; i++) {
      nn[i] = propNames_[i];
      nv[i] = propValues_[i];
      nd[i] = propDValues_[i];
      nt[i] = propTypes_[i];
    }
    defFree((char*)(propNames_));
    defFree((char*)(propValues_));
    defFree((char*)(propDValues_));
    defFree((char*)(propTypes_));
    propNames_   = nn;
    propValues_  = nv;
    propDValues_ = nd;
    propTypes_   = nt;
  }
  len = strlen(name) + 1;
  propNames_[numProps_] = (char*)defMalloc(len);
  strcpy(propNames_[numProps_], DEFCASE(name));
  len = strlen(value) + 1;
  propValues_[numProps_] = (char*)defMalloc(len);
  strcpy(propValues_[numProps_], DEFCASE(value));
  propDValues_[numProps_] = d;
  propTypes_[numProps_] = type;
  numProps_ += 1;
}


int defiPinProp::numProps() const {
  return numProps_;
}


const char* defiPinProp::propName(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6120): The index number %d specified for the PIN PROPERTY is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6120, msg);
     return 0;
  }
  return propNames_[index];
}


const char* defiPinProp::propValue(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6120): The index number %d specified for the PIN PROPERTY is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6120, msg);
     return 0;
  }
  return propValues_[index];
}


double defiPinProp::propNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6120): The index number %d specified for the PIN PROPERTY is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6120, msg);
     return 0;
  }
  return propDValues_[index];
}


const char defiPinProp::propType(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6120): The index number %d specified for the PIN PROPERTY is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6120, msg);
     return 0;
  }
  return propTypes_[index];
}


int defiPinProp::propIsNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6120): The index number %d specified for the PIN PROPERTY is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6120, msg);
     return 0;
  }
  return propDValues_[index] ? 1 : 0;
}


int defiPinProp::propIsString(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6120): The index number %d specified for the PIN PROPERTY is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6120, msg);
     return 0;
  }
  return propDValues_[index] ? 0 : 1;
}


void defiPinProp::print(FILE* f) const {
  int i;

  fprintf(f, "PinProp %s %s\n", instName(),
     pinName());

  for (i = 0; i < numProps(); i++) {
    fprintf(f, "    %s %s\n", propName(i),
                              propValue(i));
  }
}


END_LEFDEF_PARSER_NAMESPACE


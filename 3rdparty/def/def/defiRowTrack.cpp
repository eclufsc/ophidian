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
#include "defiRowTrack.hpp"
#include "defiDebug.hpp"
#include "lex.h"
#include "defiUtil.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

//////////////////////////////////////////////
//////////////////////////////////////////////
//
//   defiRow
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiRow::defiRow() {
  Init();
}


void defiRow::Init() {
  nameLength_ = 0;
  name_ = 0;
  macroLength_ = 0;
  macro_ = 0;
  orient_ = 0;
  x_ = 0.0;
  y_ = 0.0;
  xStep_ = 0.0;
  yStep_ = 0.0;
  xNum_ = 0.0;
  yNum_ = 0.0;
  hasDo_ = 0;
  hasDoStep_ = 0;
  numProps_ = 0;
  propsAllocated_ = 2;
  propNames_ = (char**)defMalloc(sizeof(char*)*2);
  propValues_ = (char**)defMalloc(sizeof(char*)*2);
  propDValues_ = (double*)defMalloc(sizeof(double)*2);
  propTypes_ = (char*)defMalloc(sizeof(char)*2);
}


defiRow::~defiRow() {
  Destroy();
}


void defiRow::Destroy() {
  clear();
  if (name_) defFree(name_);
  if (macro_) defFree(macro_);
  defFree((char*)(propNames_));
  defFree((char*)(propValues_));
  defFree((char*)(propDValues_));
  defFree((char*)(propTypes_));
}


void defiRow::clear() {
  int i;
  for (i = 0; i < numProps_; i++) {
    defFree(propNames_[i]);
    defFree(propValues_[i]);
    propDValues_[i] = 0;
  }
  hasDo_ = 0;
  hasDoStep_ = 0;
  numProps_ = 0;
}


void defiRow::setup(const char* name, const char* macro, double x, double y,
		 int orient) {
  int len = strlen(name) + 1;

  clear();

  if (len > nameLength_) {
    if (name_) defFree(name_);
    nameLength_ = len;
    name_ = (char*)defMalloc(len);
  }
  strcpy(name_, DEFCASE(name));

  len = strlen(macro) + 1;
  if (len > macroLength_) {
    if (macro_) defFree(macro_);
    macroLength_ = len;
    macro_ = (char*)defMalloc(len);
  }
  strcpy(macro_, DEFCASE(macro));

  x_ = x;
  y_ = y;
  xStep_ = 0.0;
  yStep_ = 0.0;
  xNum_ = 0.0;
  yNum_ = 0.0;
  orient_ = orient;

}


void defiRow::setDo(double x_num, double y_num,
		    double x_step, double y_step) {
  xStep_ = x_step;
  yStep_ = y_step;
  xNum_ = x_num;
  yNum_ = y_num;
  hasDo_ = 1;
}


void defiRow::setHasDoStep() {
  hasDoStep_ = 1;
}


void defiRow::addProperty(const char* name, const char* value, const char type) 
{
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
    propNames_ = nn;
    propValues_ = nv;
    propDValues_ = nd;
    propTypes_ = nt;
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


void defiRow::addNumProperty(const char* name, const double d,
                             const char* value, const char type) 
{
  int len;
  if (numProps_ == propsAllocated_) {
    int i;
    char** nn;
    char** nv;
    double* nd;
    char*  nt;
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
    propNames_ = nn;
    propValues_ = nv;
    propDValues_ = nd;
    propTypes_ = nt;
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


int defiRow::numProps() const {
  return numProps_;
}


const char* defiRow::propName(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6140): The index number %d specified for the VIA LAYER RECTANGLE is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6140, msg);
     return 0;
  }
  return propNames_[index];
}


const char* defiRow::propValue(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6140): The index number %d specified for the VIA LAYER RECTANGLE is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6140, msg);
     return 0;
  }
  return propValues_[index];
}

double defiRow::propNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6140): The index number %d specified for the VIA LAYER RECTANGLE is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6140, msg);
     return 0;
  }
  return propDValues_[index];
}

const char defiRow::propType(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6140): The index number %d specified for the VIA LAYER RECTANGLE is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6140, msg);
     return 0;
  }
  return propTypes_[index];
}

int defiRow::propIsNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6140): The index number %d specified for the VIA LAYER RECTANGLE is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6140, msg);
     return 0;
  } 
  return propDValues_[index] ? 1 : 0;
}

int defiRow::propIsString(int index) const {
  char msg[160];
  if (index < 0 || index >= numProps_) {
     sprintf (msg, "ERROR (DEFPARS-6140): The index number %d specified for the VIA LAYER RECTANGLE is invalide.\nValid index number is from 0 to %d. Specify a valid index number and then try again.",
              index, numProps_);
     defiError (0, 6140, msg);
     return 0;
  } 
  return propDValues_[index] ? 0 : 1;
}

const char* defiRow::name() const {
  return name_;
}


const char* defiRow::macro() const {
  return macro_;
}


double defiRow::x() const {
  return x_;
}


double defiRow::y() const {
  return y_;
}


double defiRow::xNum() const {
  return xNum_;
}


double defiRow::yNum() const {
  return yNum_;
}


int defiRow::orient() const {
  return orient_;
}


const char* defiRow::orientStr() const {
  return (defiOrientStr(orient_));
}


int defiRow::hasDo() const {
  return hasDo_;
}


int defiRow::hasDoStep() const {
  return hasDoStep_;
}


double defiRow::xStep() const {
  return xStep_;
}


double defiRow::yStep() const {
  return yStep_;
}


void defiRow::print(FILE* f) const {
  fprintf(f, "Row '%s' '%s' %g,%g  orient %s\n",
      name(), macro(),
      x(), y(), orientStr());
  fprintf(f, "  DO X %g STEP %g\n", xNum(),
      xStep());
  fprintf(f, "  DO Y %g STEP %g\n", yNum(),
      yStep());
}


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//   defiTrack
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiTrack::defiTrack() {
  Init();
}


void defiTrack::Init() {
  macro_ = 0;
  macroLength_ = 0;
  x_ = 0.0;
  xNum_ = 0.0;
  xStep_ = 0.0;
  layersLength_ = 0;
  numLayers_ = 0;
  layers_ = 0;
  firstTrackMask_=0;
  samemask_ = 0;
}

defiTrack::~defiTrack() {
  Destroy();
}


void defiTrack::Destroy() {
  int i;

  if (macro_) defFree(macro_);

  if (layers_) {
    for (i = 0; i < numLayers_; i++)
      if (layers_[i]) defFree(layers_[i]);
    defFree((char*)(layers_));
  }
}


void defiTrack::setup(const char* macro) {
  int i;
  int len = strlen(macro) + 1;

  if (len > macroLength_) {
    if (macro_) defFree(macro_);
    macroLength_ = len;
    macro_ = (char*)defMalloc(len);
  }
  strcpy(macro_, DEFCASE(macro));

  if (layers_) {
    for (i = 0; i < numLayers_; i++)
      if (layers_[i]) {
        defFree(layers_[i]);
        layers_[i] = 0;
      }
  }
  numLayers_ = 0;
  x_ = 0.0;
  xStep_ = 0.0;
  xNum_ = 0.0;

  firstTrackMask_=0;
  samemask_=0;
}


void defiTrack::setDo(double x, double x_num, double x_step) {
  x_ = x;
  xStep_ = x_step;
  xNum_ = x_num;
}


void defiTrack::addLayer(const char* layer) {
  char* l;
  int len;

  if (numLayers_ >= layersLength_) {
    int i;
    char** newl;
    layersLength_ = layersLength_ ? 2 * layersLength_ : 8;
    newl = (char**)defMalloc(layersLength_* sizeof(char*));
    for (i = 0; i < numLayers_; i++)
      newl[i] = layers_[i];
    if (layers_) defFree((char*)(layers_));
    layers_ = newl;
  }

  len = strlen(layer) + 1;
  l = (char*)defMalloc(len);
  strcpy(l, DEFCASE(layer));
  layers_[numLayers_++] = l;
}

void defiTrack::addMask(int colorMask, int sameMask) {
   samemask_=sameMask;
   firstTrackMask_= colorMask;
}


const char* defiTrack::macro() const {
  return macro_;
}


double defiTrack::x() const {
  return x_;
}


double defiTrack::xNum() const {
  return xNum_;
}


double defiTrack::xStep() const {
  return xStep_;
}


int defiTrack::numLayers() const {
  return numLayers_;
}


const char* defiTrack::layer(int index) const {
  if (index >= 0 && index < numLayers_) {
    return layers_[index];
  }

  return 0;
}

int defiTrack::firstTrackMask() const {
    return firstTrackMask_;
}

int defiTrack::sameMask() const {
    return samemask_;
}

void defiTrack::print(FILE* f) const {
  int i;

  fprintf(f, "Track '%s'\n", macro());
  fprintf(f, "  DO %g %g STEP %g\n",
      x(),
      xNum(),
      xStep());
  fprintf(f, "  %d layers ", numLayers());
  for (i = 0; i < numLayers(); i++) {
    fprintf(f, " '%s'", layer(i));
  }
  fprintf(f, "\n");
}


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//   defiGcellGrid
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiGcellGrid::defiGcellGrid() {
  Init();
}


void defiGcellGrid::Init() {
  macro_ = 0;
  macroLength_ = 0;
  x_ = 0;
  xNum_ = 0;
  xStep_ = 0;
}


defiGcellGrid::~defiGcellGrid() {
  Destroy();
}


void defiGcellGrid::Destroy() {
  if (macro_) defFree(macro_);
}


void defiGcellGrid::setup(const char* macro, int x, int xNum, double xStep) {
  int len = strlen(macro) + 1;
  if (len > macroLength_) {
    if (macro_) defFree(macro_);
    macroLength_ = len;
    macro_ = (char*)defMalloc(len);
  }
  strcpy(macro_, DEFCASE(macro));

  x_ = x;
  xNum_ = xNum;
  xStep_ = xStep;
}


int defiGcellGrid::x() const {
  return x_;
}


int defiGcellGrid::xNum() const {
  return xNum_;
}


double defiGcellGrid::xStep() const {
  return xStep_;
}


const char* defiGcellGrid::macro() const {
  return macro_;
}


void defiGcellGrid::print(FILE* f) const {
  fprintf(f, "GcellGrid '%s'\n", macro());
  fprintf(f, "  DO %d %d STEP %5.1f\n",
      x(),
      xNum(),
      xStep());
}


END_LEFDEF_PARSER_NAMESPACE


// *****************************************************************************
// *****************************************************************************
// Copyright 2013 - 2015, Cadence Design Systems
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
//  $Revision: #8 $
//  $Date: 2016/01/26 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include <stdlib.h>
#include "lex.h"
#include "defiVia.hpp"
#include "defiDebug.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE


//////////////////////////////////////////////
//////////////////////////////////////////////
//
//   defiVia
//
//////////////////////////////////////////////
//////////////////////////////////////////////


defiVia::defiVia() {
  Init();
}


void defiVia::Init() {
  name_ = 0;
  nameLength_ = 0;
  pattern_ = 0;
  patternLength_ = 0;
  xl_ = 0;
  yl_ = 0;
  xh_ = 0;
  yh_ = 0;
  layersLength_ = 0;
  layers_ = 0;
  viaRule_ = 0;
  viaRuleLength_ = 0;
  xSize_ = 0;
  ySize_ = 0;
  botLayer_ = 0;
  cutLayer_ = 0;
  topLayer_ = 0;
  botLayerLength_ = 0;
  cutLayerLength_ = 0;
  topLayerLength_ = 0;
  xCutSpacing_ = 0;
  yCutSpacing_ = 0;
  xBotEnc_ = 0;
  yBotEnc_ = 0;
  xTopEnc_ = 0;
  yTopEnc_ = 0;
  cutPattern_ = 0;
  cutPatternLength_ = 0;
  numLayers_ = 0;
  numPolys_ = 0;
  polygons_ = 0;
  polysAllocated_ = 0;
  polygonNames_ = 0;
  rectMask_ = 0;
  polyMask_ = 0;
  clear();
}

void defiVia::clear() {
  int i;

  hasPattern_ = 0;
  hasViaRule_ = 0;
  rows_ = 0;
  cols_ = 0;
  xOffset_ = 0;
  yOffset_ = 0;
  xBotOffset_ = 0;
  yBotOffset_ = 0;
  xTopOffset_ = 0;
  yTopOffset_ = 0;
  hasCutPattern_ = 0;
  if (polygonNames_) {
    struct defiPoints* p;
    for (i = 0; i < numPolys_; i++) {
      if (polygonNames_[i]) defFree((char*)(polygonNames_[i]));
      p = polygons_[i];
      defFree((char*)(p->x));
      defFree((char*)(p->y));
      defFree((char*)(polygons_[i]));
    }
    defFree((char*)(polygonNames_));
    defFree((char*)(polygons_));
    defFree((char*)(polyMask_));

    polygonNames_ = 0;
    polygons_ = 0;
    polyMask_ = 0;
  }
  numPolys_ = 0;
  polysAllocated_ = 0;
}

defiVia::~defiVia() {
  Destroy();
}


void defiVia::Destroy() {
  int i;

  if (name_) defFree(name_);
  if (pattern_) defFree(pattern_);

  if (layers_) {
    for (i = 0; i < numLayers_; i++)
      if (layers_[i]) defFree(layers_[i]);
    defFree((char*)(layers_));
    defFree((char*)(xl_));
    defFree((char*)(yl_));
    defFree((char*)(xh_));
    defFree((char*)(yh_));
    defFree((char*)(rectMask_));
    defFree((char*)(polyMask_));
  }
  if (viaRule_)
    defFree((char*)(viaRule_));
  if (botLayer_)
    defFree((char*)(botLayer_));
  if (cutLayer_)
    defFree((char*)(cutLayer_));
  if (topLayer_)
    defFree((char*)(topLayer_));
  if (cutPattern_)
    defFree((char*)(cutPattern_));
  clear();
}


void defiVia::setup(const char* name) {
  int i;
  int len = strlen(name) + 1;
  if (len > nameLength_) {
    nameLength_ = len;
    name_ = (char*)defRealloc(name_, len);
  }
  strcpy(name_, DEFCASE(name));
  if (pattern_) *(pattern_) = 0;
  if (layers_) {
    for (i = 0; i < numLayers_; i++) {
      if (layers_[i]) defFree(layers_[i]);
    layers_[i] = 0;
    }
  }
  numLayers_ = 0;
}


void defiVia::addPattern(const char* pattern) {
  int len = strlen(pattern) + 1;
  if (len > patternLength_) {
    patternLength_ = len;
    pattern_ = (char*)defRealloc(pattern_, len);
  }
  strcpy(pattern_, DEFCASE(pattern));
  hasPattern_ = 1;
}


void defiVia::addLayer(const char* layer, int xl, int yl, int xh, int yh, int colorMask) {
  char* l;
  int len;

  if (numLayers_ >= layersLength_) {
    int i;
    char** newl;
    int* ints;
    layersLength_ = layersLength_ ? 2 * layersLength_ : 8;

    newl = (char**)defMalloc(layersLength_ * sizeof(char*));
    for (i = 0; i < numLayers_; i++)
      newl[i] = layers_[i];
    if (layers_) defFree((char*)(layers_));
    layers_ = newl;

    ints = (int*)defMalloc(layersLength_ * sizeof(int));
    for (i = 0; i < numLayers_; i++)
      ints[i] = xl_[i];
    if (xl_) defFree((char*)(xl_));
    xl_ = ints;

    ints = (int*)defMalloc(layersLength_ * sizeof(int));
    for (i = 0; i < numLayers_; i++)
      ints[i] = yl_[i];
    if (yl_) defFree((char*)(yl_));
    yl_ = ints;

    ints = (int*)defMalloc(layersLength_ * sizeof(int));
    for (i = 0; i < numLayers_; i++)
      ints[i] = xh_[i];
    if (xh_) defFree((char*)(xh_));
    xh_ = ints;

    ints = (int*)defMalloc(layersLength_ * sizeof(int));
    for (i = 0; i < numLayers_; i++)
      ints[i] = yh_[i];
    if (yh_) defFree((char*)(yh_));
    yh_ = ints;

    ints = (int*)defMalloc(layersLength_ * sizeof(int));
    for (i = 0; i < numLayers_; i++)
        ints[i] = rectMask_[i];
    if (rectMask_) defFree((char*)(rectMask_));
    rectMask_ = ints;
  }

  len = strlen(layer) + 1;
  l = (char*)defMalloc(len);
  strcpy(l, DEFCASE(layer));
  layers_[numLayers_] = l;
  xl_[numLayers_] = xl;
  yl_[numLayers_] = yl;
  xh_[numLayers_] = xh;
  yh_[numLayers_] = yh;
  rectMask_[numLayers_] = colorMask;
  numLayers_++;
}

// 5.6
void defiVia::addPolygon(const char* layer, defiGeometries* geom, int colorMask) {
  struct defiPoints* p;
  int x, y;
  int i;

  if (numPolys_ == polysAllocated_) {
    char** newn;
    int* masks;
    struct defiPoints** poly;
    polysAllocated_ = (polysAllocated_ == 0) ?
          2 : polysAllocated_ * 2;
    newn = (char**)defMalloc(sizeof(char*) * polysAllocated_);
    poly = (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            polysAllocated_);
    masks = (int*)defMalloc(polysAllocated_ * sizeof(int));
    for (i = 0; i < numPolys_; i++) {
      newn[i] = polygonNames_[i];
      poly[i] = polygons_[i];
      masks[i] = polyMask_[i];
    }
    if (polygons_)
      defFree((char*)(polygons_));
    if (polygonNames_)
      defFree((char*)(polygonNames_));
    if (polyMask_) 
      defFree((char*)(polyMask_));
    polygonNames_ = newn;
    polygons_ = poly;
    polyMask_ = masks;
  }
  polygonNames_[numPolys_] = strdup(layer);
  p = (struct defiPoints*)defMalloc(sizeof(struct defiPoints));
  p->numPoints = geom->numPoints();
  p->x = (int*)defMalloc(sizeof(int)*p->numPoints);
  p->y = (int*)defMalloc(sizeof(int)*p->numPoints);
  for (i = 0; i < p->numPoints; i++) {
    geom->points(i, &x, &y);
    p->x[i] = x;
    p->y[i] = y;
  }
  polygons_[numPolys_] = p;
  polyMask_[numPolys_] = colorMask;
  numPolys_ += 1;
}

void defiVia::addViaRule(char* viaRuleName, int xSize, int ySize,
                         char* botLayer, char* cutLayer, char* topLayer,
                         int xSpacing, int ySpacing, int xBotEnc, int yBotEnc,
                         int xTopEnc, int yTopEnc) {
  int len;

  len = strlen(viaRuleName) + 1;
  if (len > viaRuleLength_) {
    if (viaRule_)
      defFree(viaRule_);
    viaRule_ = (char*)defMalloc(strlen(viaRuleName)+1);
  }
  strcpy(viaRule_, DEFCASE(viaRuleName));
  xSize_ = xSize;
  ySize_ = ySize;
  len = strlen(botLayer) + 1;
  if (len > botLayerLength_) {
    if (botLayer_)
      defFree(botLayer_);
    botLayer_ = (char*)defMalloc(strlen(botLayer)+1);
    botLayerLength_ = len;
  }
  strcpy(botLayer_, DEFCASE(botLayer));
  len = strlen(cutLayer) + 1;
  if (len > cutLayerLength_) {
    if (cutLayer_)
      defFree(cutLayer_);
    cutLayer_ = (char*)defMalloc(strlen(cutLayer)+1);
    cutLayerLength_ = len;
  }
  strcpy(cutLayer_, DEFCASE(cutLayer));
  len = strlen(topLayer) + 1;
  if (len > topLayerLength_) {
    if (topLayer_)
      defFree(topLayer_);
    topLayer_ = (char*)defMalloc(strlen(topLayer)+1);
    topLayerLength_ = len;
  }
  strcpy(topLayer_, DEFCASE(topLayer));
  xCutSpacing_ = xSpacing;
  yCutSpacing_ = ySpacing;
  xBotEnc_ = xBotEnc;
  yBotEnc_ = yBotEnc;
  xTopEnc_ = xTopEnc;
  yTopEnc_ = yTopEnc;
  hasViaRule_ = 1;
}

void defiVia::addRowCol(int numCutRows, int numCutCols) {
  rows_ = numCutRows;
  cols_ = numCutCols;
}

void defiVia::addOrigin(int xOffset, int yOffset) {
  xOffset_ = xOffset;
  yOffset_ = yOffset;
}

void defiVia::addOffset(int xBotOs, int yBotOs, int xTopOs, int yTopOs) {
  xBotOffset_ = xBotOs;
  yBotOffset_ = yBotOs;
  xTopOffset_ = xTopOs;
  yTopOffset_ = yTopOs;
}

void defiVia::addCutPattern(char* cutPattern) {
  int len;

  len = strlen(cutPattern) + 1;
  if (len > cutPatternLength_) {
    if (cutPattern_)
      defFree(cutPattern_);
    cutPattern_ = (char*)defMalloc(strlen(cutPattern)+1);
    cutPatternLength_ = len;
  }
  strcpy(cutPattern_, DEFCASE(cutPattern));
  hasCutPattern_ = 1;
}

int defiVia::hasPattern() const {
  return hasPattern_;
}


const char* defiVia::pattern() const {
  return pattern_;
}


const char* defiVia::name() const {
  return name_;
}


int defiVia::numLayers() const {
  return numLayers_;
}

int defiVia::rectMask(int index) const {
    if (index >= 0 && index < numLayers_) {
        return rectMask_[index];
    }

    return 0;
}

int defiVia::polyMask(int index) const {
    if (index >= 0 && index < numPolys_) {
        return polyMask_[index];
    }

    return 0;
}

void defiVia::layer(int index, char** layer, int* xl, int* yl, 
      int* xh, int* yh) const {
  if (index >= 0 && index < numLayers_) {
    if (layer) *layer = layers_[index];
    if (xl) *xl = xl_[index];
    if (yl) *yl = yl_[index];
    if (xh) *xh = xh_[index];
    if (yh) *yh = yh_[index];
  }
}

// The following code is for 5.6

int defiVia::numPolygons() const {
  return numPolys_;
}

const char* defiVia::polygonName(int index) const {
  char msg[160];
  if (index < 0 || index > numPolys_) {
     sprintf (msg, "ERROR (DEFPARS-6180): The index number %d specified for the VIA POLYGON is invalid.\nValid index is from 0 to %d. Specify a valid index number and then try again",
              index, numPolys_);
     defiError (0, 6180, msg);
     return 0;
  }
  return polygonNames_[index];
}

struct defiPoints defiVia::getPolygon(int index) const {
  return *(polygons_[index]);
}

int defiVia::hasViaRule() const {
  return hasViaRule_;
}

void defiVia::viaRule(char** viaRuleName, int* xSize, int* ySize,
                      char** botLayer, char** cutLayer, char** topLayer,
                      int* xCutSpacing, int* yCutSpacing, int* xBotEnc,
                      int* yBotEnc, int* xTopEnc, int* yTopEnc) const {
  *viaRuleName = viaRule_;
  *xSize = xSize_;
  *ySize = ySize_;
  *botLayer = botLayer_;
  *cutLayer = cutLayer_;
  *topLayer = topLayer_;
  *xCutSpacing = xCutSpacing_;
  *yCutSpacing = yCutSpacing_;
  *xBotEnc = xBotEnc_;
  *yBotEnc = yBotEnc_;
  *xTopEnc = xTopEnc_;
  *yTopEnc = yTopEnc_;
}

int defiVia::hasRowCol() const {
  if (rows_)
    return rows_; 
  else
    return cols_; 
}

void defiVia::rowCol(int* numCutRows, int* numCutCols) const {
  *numCutRows = rows_;
  *numCutCols = cols_;
}

int defiVia::hasOrigin() const {
  if (xOffset_)
    return xOffset_;
  else
    return yOffset_;
}

void defiVia::origin(int* xOffset, int* yOffset) const {
  *xOffset = xOffset_;
  *yOffset = yOffset_;
}

int defiVia::hasOffset() const {
  if (xBotOffset_)
    return xBotOffset_;
  else if (yBotOffset_)
    return yBotOffset_;
  else if (xTopOffset_)
    return xTopOffset_;
  else 
    return yTopOffset_;
}

void defiVia::offset(int* xBotOffset, int* yBotOffset, int* xTopOffset,
                     int* yTopOffset) const {
  *xBotOffset = xBotOffset_;
  *yBotOffset = yBotOffset_;
  *xTopOffset = xTopOffset_;
  *yTopOffset = yTopOffset_;
}

int defiVia::hasCutPattern() const {
  return hasCutPattern_;
}

int defiVia::hasRectMask(int index) const {
    if (index > 0 || index < numLayers_) {
        return rectMask_[index];
    }

    return 0;
}

int defiVia::hasPolyMask(int index) const {
    if (index > 0 || index < numPolys_) {
        return polyMask_[index];
    }
    
    return 0;
}

const char* defiVia::cutPattern() const {
  return cutPattern_;
}

void defiVia::print(FILE* f) const {
  int i;
  int xl, yl, xh, yh;
  char* c;
  char* vrn;
  char *bl, *cl, *tl;
  int xs, ys, xcs, ycs, xbe, ybe, xte, yte;
  int cr, cc, xo, yo, xbo, ybo, xto, yto;

  fprintf(f, "via '%s'\n", name());

  if (hasPattern())
    fprintf(f, "  pattern '%s'\n", pattern());

  for (i = 0; i < numLayers(); i++) {
    layer(i, &c, &xl, &yl, &xh, &yh);
    fprintf(f, "  layer '%s' %d,%d %d,%d\n", c, xl, yl, xh, yh);
  }

  if (hasViaRule()) {
    viaRule(&vrn, &xs, &ys, &bl, &cl, &tl, &xcs, &ycs,
                                 &xbe, &ybe, &xte, &yte);
    fprintf(f, "  viarule '%s'\n", vrn);
    fprintf(f, "    cutsize %d %d\n", xs, ys);
    fprintf(f, "    layers %s %s %s\n", bl, cl, tl);
    fprintf(f, "    cutspacing %d %d\n", xcs, ycs);
    fprintf(f, "    enclosure %d %d %d %d\n", xbe, ybe, xte, yte);
    if (hasRowCol()) {
       rowCol(&cr, &cc);
       fprintf(f, "    rowcol %d %d\n", cr, cc);
    }
    if (hasOrigin()) {
       origin(&xo, &yo);
       fprintf(f, "    origin %d %d\n", xo, yo);
    }
    if (hasOffset()) {
       offset(&xbo, &ybo, &xto, &yto);
       fprintf(f, "    offset %d %d %d %d\n", xbo, ybo, xto, yto);
    }
    if (hasCutPattern())
       fprintf(f, "    pattern '%s'\n", cutPattern());
  }
}
END_LEFDEF_PARSER_NAMESPACE


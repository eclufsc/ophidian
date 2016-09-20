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
#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "defiBlockage.hpp"
#include "defiDebug.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    defiBlockages
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////

defiBlockage::defiBlockage() {
  Init();
}


void defiBlockage::Init() {
  numPolys_ = 0;
  clear();
  layerName_ = 0;
  componentName_ = 0;
  layerNameLength_ = 0;
  componentNameLength_ = 0;
  xl_ = 0;
  yl_ = 0;
  xh_ = 0;
  yh_ = 0;
  rectsAllocated_ = 0;
  polysAllocated_ = 0;
  polygons_ = 0;
}


defiBlockage::~defiBlockage() {
  Destroy();
}


void defiBlockage::clear() {
  hasLayer_ = 0;
  hasPlacement_ = 0;
  hasComponent_ = 0;
  hasSlots_ = 0;
  hasFills_ = 0;
  hasPushdown_ = 0;
  hasExceptpgnet_ = 0;
  hasSoft_ = 0;
  maxDensity_ = -1;
  minSpacing_ = -1;
  width_ = -1;
  numRectangles_ = 0;
  mask_ = 0;
}


void defiBlockage::clearPoly() {
  struct defiPoints* p;
  int i;

  for (i = 0; i < numPolys_; i++) {
    p = polygons_[i];
    defFree((char*)(p->x));
    defFree((char*)(p->y));
    defFree((char*)(polygons_[i]));
  }
  numPolys_ = 0;
}

void defiBlockage::Destroy() {
  if (layerName_) defFree(layerName_);
  if (componentName_) defFree(componentName_);
  layerName_ = 0;
  componentName_ = 0;
  if (rectsAllocated_) {
    defFree((char*)(xl_));
    defFree((char*)(yl_));
    defFree((char*)(xh_));
    defFree((char*)(yh_));
    rectsAllocated_ = 0;
    xl_ = 0;
    yl_ = 0;
    xh_ = 0;
    yh_ = 0;
  }
  clearPoly();
  defFree((char*)(polygons_));
  polygons_ = 0;
  clear();
}


void defiBlockage::setLayer(const char* name) {
  int len = strlen(name) + 1;
  if (layerNameLength_ < len) {
    if (layerName_) defFree(layerName_);
    layerName_ = (char*)defMalloc(len);
    layerNameLength_ = len;
  }
  strcpy(layerName_, DEFCASE(name));
  hasLayer_ = 1;
}


void defiBlockage::setPlacement() {

  /* 10/29/2001 - Wanda da Rosa, new enhancement */
  hasPlacement_ = 1;
  return;
}

void defiBlockage::setComponent(const char* name) {
  int len;

  /* 10/29/2001 - Wanda da Rosa, component name is required */
  len = strlen(name) + 1;
  if (componentNameLength_ < len) {
    if (componentName_) defFree(componentName_);
    componentName_ = (char*)defMalloc(len);
    componentNameLength_ = len;
  }
  strcpy(componentName_, DEFCASE(name));
  hasComponent_ = 1;
}

void defiBlockage::setSlots() {
  hasSlots_ = 1;
}

void defiBlockage::setFills() {
  hasFills_ = 1;
}

void defiBlockage::setPushdown() {
  hasPushdown_ = 1;
}

// 5.7
void defiBlockage::setExceptpgnet() {
  hasExceptpgnet_ = 1;
}

// 5.7
void defiBlockage::setSoft() {
  hasSoft_ = 1;
}

// 5.7
void defiBlockage::setPartial(double maxDensity) {
  maxDensity_ = maxDensity;
}

void defiBlockage::setSpacing(int minSpacing) {
  minSpacing_ = minSpacing;
}

void defiBlockage::setDesignRuleWidth(int width) {
  width_ = width;
}

void defiBlockage::setMask(int colorMask) {
  mask_ = colorMask;
}

void defiBlockage::addRect(int xl, int yl, int xh, int yh) {
  if (numRectangles_ == rectsAllocated_) {
    int i;
    int max = rectsAllocated_ = (rectsAllocated_ == 0) ?
          2 : rectsAllocated_ * 2;
    int* newxl = (int*)defMalloc(sizeof(int)*max);
    int* newyl = (int*)defMalloc(sizeof(int)*max);
    int* newxh = (int*)defMalloc(sizeof(int)*max);
    int* newyh = (int*)defMalloc(sizeof(int)*max);
    for (i = 0; i < numRectangles_; i++) {
      newxl[i] = xl_[i];
      newyl[i] = yl_[i];
      newxh[i] = xh_[i];
      newyh[i] = yh_[i];
    }
    defFree((char*)(xl_));
    defFree((char*)(yl_));
    defFree((char*)(xh_));
    defFree((char*)(yh_));
    xl_ = newxl;
    yl_ = newyl;
    xh_ = newxh;
    yh_ = newyh;
  }
  xl_[numRectangles_] = xl;
  yl_[numRectangles_] = yl;
  xh_[numRectangles_] = xh;
  yh_[numRectangles_] = yh;
  numRectangles_ += 1;
}


// 5.6
void defiBlockage::addPolygon(defiGeometries* geom) {
  struct defiPoints* p;
  int x, y;
  int i;

  if (numPolys_ == polysAllocated_) {
    struct defiPoints** poly;
    polysAllocated_ = (polysAllocated_ == 0) ?
          2 : polysAllocated_ * 2;
    poly = (struct defiPoints**)defMalloc(sizeof(struct defiPoints*) *
            polysAllocated_);
    for (i = 0; i < numPolys_; i++)
      poly[i] = polygons_[i];
    if (polygons_)
      defFree((char*)(polygons_));
    polygons_ = poly;
  }
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
  numPolys_ += 1;
}


int defiBlockage::hasLayer() const {
  return hasLayer_;
}


int defiBlockage::hasPlacement() const {
  return hasPlacement_;
}


int defiBlockage::hasComponent() const {
  return hasComponent_;
}


int defiBlockage::hasSlots() const {
  return hasSlots_;
}


int defiBlockage::hasFills() const {
  return hasFills_;
}


int defiBlockage::hasPushdown() const {
  return hasPushdown_;
}

// 5.7
int defiBlockage::hasExceptpgnet() const {
  return hasExceptpgnet_;
}

// 5.7
int defiBlockage::hasSoft() const {
  return hasSoft_;
}

// 5.7
int defiBlockage::hasPartial() const {
  if (maxDensity_ == -1)
    return 0;
  return 1;
}

// 5.7
double defiBlockage::placementMaxDensity() const {
  return maxDensity_;
}

int defiBlockage::hasSpacing() const {
  if (minSpacing_ == -1)
    return 0;
  return 1;
}


int defiBlockage::hasDesignRuleWidth() const {
  if (width_ == -1)
    return 0;
  return 1;
}

int defiBlockage::hasMask() const {
    return mask_;
}

int defiBlockage::minSpacing() const {
  return minSpacing_;
}


int defiBlockage::designRuleWidth() const {
  return width_;
}

int defiBlockage::mask() const {
  return mask_;
}

const char* defiBlockage::layerName() const {
  return layerName_;
}


const char* defiBlockage::layerComponentName() const {
  return componentName_;
}


const char* defiBlockage::placementComponentName() const {
  return componentName_;
}


int defiBlockage::numRectangles() const {
  return numRectangles_;
}


int defiBlockage::xl(int index) const {
  if (index < 0 || index >= numRectangles_) {
    defiError(1, 0, "bad index for blockage xl");
    return 0;
  }
  return xl_[index];
}


int defiBlockage::yl(int index) const {
  if (index < 0 || index >= numRectangles_) {
    defiError(1, 0, "bad index for blockage yl");
    return 0;
  }
  return yl_[index];
}


int defiBlockage::xh(int index) const {
  if (index < 0 || index >= numRectangles_) {
    defiError(1, 0, "bad index for blockage xh");
    return 0;
  }
  return xh_[index];
}


int defiBlockage::yh(int index) const {
  if (index < 0 || index >= numRectangles_) {
    defiError(1, 0, "bad index for blockage yh");
    return 0;
  }
  return yh_[index];
}


// 5.6
int defiBlockage::numPolygons() const {
  return numPolys_;
}


// 5.6
struct defiPoints defiBlockage::getPolygon(int index) const {
  return *(polygons_[index]);
}


void defiBlockage::print(FILE* f) const {
  int i, j;
  struct defiPoints points;

  if (hasLayer()) {
    fprintf(f, "- LAYER %s", layerName());
    if (hasComponent())
      fprintf(f, " + COMPONENT %s", layerComponentName());
    if (hasSlots())
      fprintf(f, " + SLOTS");
    if (hasFills())
      fprintf(f, " + FILLS");
    if (hasPushdown())
      fprintf(f, " + PUSHDOWN");
    if (hasExceptpgnet())
      fprintf(f, " + EXCEPTPGNET");
    fprintf(f, "\n");
  }
  if (hasPlacement()) {
    fprintf(f, "- PLACEMENT");
    if (hasComponent())
      fprintf(f, " + COMPONENT %s", layerComponentName());
    if (hasPushdown())
      fprintf(f, " + PUSHDOWN");
    if (hasSoft())
      fprintf(f, " + SOFT");
    if (hasPartial())
      fprintf(f, " + PARTIAL %f", placementMaxDensity());
    fprintf(f, "\n");
  }

  for (i = 0; i < numRectangles(); i++) {
    fprintf(f, "   RECT %d %d %d %d\n", xl(i), yl(i), xh(i), yh(i));
  }

  for (i = 0; i < numPolygons(); i++) {
    fprintf(f, "   POLYGON ");
    points = getPolygon(i);
    for (j = 0; j < points.numPoints; j++)
      fprintf(f, "%d %d ", points.x[j], points.y[j]);
    fprintf(f,"\n");
  }
  fprintf(f,"\n");
}
END_LEFDEF_PARSER_NAMESPACE


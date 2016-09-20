//******************************************************************************
//******************************************************************************
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
//******************************************************************************
// 
//  $Author: arakhman $
//  $Revision: #12 $
//  $Date: 2016/04/22 $
//  $State:  $
//****************************************************************************

//  Error message number:
//  5000 - def reader, defrReader.cpp
//  5500 - lex.cpph, yyerror
//  6000 - def parser, error, lex.cpph, def.y (CALLBACK & CHKERR)
//  6010 - defiBlockage.cpp
//  6020 - defiComponent.cpp
//  6030 - defiFPC.cpp
//  6040 - defiFill.cpp
//  6050 - defiGroup.cpp
//  6060 - defiIOTiming.cpp
//  6070 - defiMisc.cpp
//  6080 - defiNet.cpp
//  6090 - defiNonDefault.cpp
//  6100 - defiPartition.cpp
//  6110 - defiPinCap.cpp
//  6120 - defiPinProp.cpp
//  6130 - defiRegion.cpp
//  6140 - defiRowTrack.cpp
//  6150 - defiScanchain.cpp
//  6160 - defiSlot.cpp
//  6170 - defiTimingDisable.cpp
//  6180 - defiVia.cpp
//  6500 - def parser, error, def.y
//  7000 - def parser, warning, lex.cpph
//  7500 - def parser, warning, lef.y
//  8000 - def parser, info, lex.cpph
//  9000 - def writer

%{
#include <stdlib.h>
#include <string.h>
#include "defrReader.hpp"
#include "defiUser.hpp"
#include "defrCallBacks.hpp"
#include "lex.h"

#define DEF_MAX_INT 2147483647
#define YYDEBUG 1     // this is temp fix for pcr 755132 
// TX_DIR:TRANSLATION ON


#include "defrData.hpp"
#include "defrSettings.hpp"
#include "defrCallBacks.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

extern int errors;        // from lex.cpph
extern int defInPropDef;  // from lex.cpph, tracking if insided propDef

// From def_keywords.cpp 
extern char* ringCopy(const char* string);

extern void pathIsDone(int shield, int reset, int netOsnet, int *needCbk);

// Macro to describe how we handle a callback.
// If the function was set then call it.
// If the function returns non zero then there was an error
// so call the error routine and exit.
//
#define CALLBACK(func, typ, data) \
    if (!defData->errors) {\
      if (func) { \
        if ((defData->defRetVal = (*func)(typ, data, defSettings->UserData)) == PARSE_OK) { \
        } else if (defData->defRetVal == STOP_PARSE) { \
          return defData->defRetVal; \
        } else { \
          defError(6010, "An error has been reported in callback."); \
          return defData->defRetVal; \
        } \
      } \
    }

#define CHKERR() \
    if (defData->errors > 20) {\
      defError(6011, "Too many syntax defData->errors have been reported."); \
      defData->errors = 0; \
      return 1; \
    }

#define CHKPROPTYPE(propType, propName, name) \
    if (propType == 'N') { \
       defData->warningMsg = (char*)defMalloc(strlen(propName)+strlen(name)+40); \
       sprintf(defData->warningMsg, "The PropName %s is not defined for %s.", \
               propName, name); \
       defWarning(7010, defData->warningMsg); \
       defFree(defData->warningMsg); \
    }

int validateMaskInput(int input, int warningIndex, int getWarningsIndex) 
{
    if (defData->VersionNum < 5.8 && input > 0) {
      if (warningIndex++ < getWarningsIndex) {
          defData->defMsg = (char*)defMalloc(1000);
          sprintf (defData->defMsg,
             "The MASK statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
          defError(7415, defData->defMsg);
          defFree(defData->defMsg);
          CHKERR();
          
          return 0;
        }
    }   
    
    return 1; 
}

double convert_defname2num(char *versionName)
{
    char majorNm[80];
    char minorNm[80];
    char *subMinorNm = NULL;
    char *versionNm = strdup(versionName);

    double major = 0, minor = 0, subMinor = 0;
    double version;

    sscanf(versionNm, "%[^.].%s", majorNm, minorNm);
    
    char *p1 = strchr(minorNm, '.');
    if (p1) {
       subMinorNm = p1+1;
       *p1 = '\0';
    }
    major = atof(majorNm);
    minor = atof(minorNm);
    if (subMinorNm)
       subMinor = atof(subMinorNm);

    version = major;

    if (minor > 0)
       version = major + minor/10;

    if (subMinor > 0)
       version = version + subMinor/1000;

    free(versionNm);
    return version;
}

int numIsInt (char* volt) {
    if (strchr(volt, '.'))  // a floating point
       return 0;
    else
       return 1;
}

int defValidNum(int values) {
    char *outMsg;
    switch (values) {
        case 100:
        case 200:
        case 1000:
        case 2000:
                return 1;
        case 400:
        case 800:
        case 4000:
        case 8000:
        case 10000:
        case 20000:
             if (defData->VersionNum < 5.6) {
                if (defCallbacks->UnitsCbk) {
                  if (defData->unitsWarnings++ < defSettings->UnitsWarnings) {
                    outMsg = (char*)defMalloc(1000);
                    sprintf (outMsg,
                    "An error has been found while processing the DEF file '%s'\nUnit %d is a 5.6 or later syntax. Define the DEF file as 5.6 and then try again.",
                    defSettings->FileName, values);
                    defError(6501, outMsg);
                    defFree(outMsg);
                  }
                }
                
                return 0;
             } else {
                return 1;
             }
    }
    if (defCallbacks->UnitsCbk) {
      if (defData->unitsWarnings++ < defSettings->UnitsWarnings) {
        outMsg = (char*)defMalloc(10000);
        sprintf (outMsg,
          "The value %d defined for DEF UNITS DISTANCE MICRON is invalid\n. The valid values are 100, 200, 400, 800, 1000, 2000, 4000, 8000, 10000, or 20000. Specify a valid value and then try again.", values);
        defError(6502, outMsg);
        defFree(outMsg);
        CHKERR();
      }
    }
    return 0;
}

#define FIXED 1
#define COVER 2
#define PLACED 3
#define UNPLACED 4
%}

%union {
        double dval ;
        int    integer ;
        char * string ;
        int    keyword ;  // really just a nop 
        struct LefDefParser::defpoint pt;
        LefDefParser::defTOKEN *tk;
}

%token <string>  QSTRING
%token <string>  T_STRING SITE_PATTERN
%token <dval>    NUMBER
%token <keyword> K_HISTORY K_NAMESCASESENSITIVE
%token <keyword> K_DESIGN K_VIAS K_TECH K_UNITS K_ARRAY K_FLOORPLAN
%token <keyword> K_SITE K_CANPLACE K_CANNOTOCCUPY K_DIEAREA
%token <keyword> K_PINS
%token <keyword> K_DEFAULTCAP K_MINPINS K_WIRECAP
%token <keyword> K_TRACKS K_GCELLGRID
%token <keyword> K_DO K_BY K_STEP K_LAYER K_ROW K_RECT
%token <keyword> K_COMPS K_COMP_GEN K_SOURCE K_WEIGHT K_EEQMASTER
%token <keyword> K_FIXED K_COVER K_UNPLACED K_PLACED K_FOREIGN K_REGION 
%token <keyword> K_REGIONS
%token <keyword> K_NETS K_START_NET K_MUSTJOIN K_ORIGINAL K_USE K_STYLE
%token <keyword> K_PATTERN K_PATTERNNAME K_ESTCAP K_ROUTED K_NEW 
%token <keyword> K_SNETS K_SHAPE K_WIDTH K_VOLTAGE K_SPACING K_NONDEFAULTRULE
%token <keyword> K_NONDEFAULTRULES
%token <keyword> K_N K_S K_E K_W K_FN K_FE K_FS K_FW
%token <keyword> K_GROUPS K_GROUP K_SOFT K_MAXX K_MAXY K_MAXHALFPERIMETER
%token <keyword> K_CONSTRAINTS K_NET K_PATH K_SUM K_DIFF 
%token <keyword> K_SCANCHAINS K_START K_FLOATING K_ORDERED K_STOP K_IN K_OUT
%token <keyword> K_RISEMIN K_RISEMAX K_FALLMIN K_FALLMAX K_WIREDLOGIC
%token <keyword> K_MAXDIST
%token <keyword> K_ASSERTIONS
%token <keyword> K_DISTANCE K_MICRONS
%token <keyword> K_END
%token <keyword> K_IOTIMINGS K_RISE K_FALL K_VARIABLE K_SLEWRATE K_CAPACITANCE
%token <keyword> K_DRIVECELL K_FROMPIN K_TOPIN K_PARALLEL
%token <keyword> K_TIMINGDISABLES K_THRUPIN K_MACRO
%token <keyword> K_PARTITIONS K_TURNOFF
%token <keyword> K_FROMCLOCKPIN K_FROMCOMPPIN K_FROMIOPIN
%token <keyword> K_TOCLOCKPIN K_TOCOMPPIN K_TOIOPIN
%token <keyword> K_SETUPRISE K_SETUPFALL K_HOLDRISE K_HOLDFALL
%token <keyword> K_VPIN K_SUBNET K_XTALK K_PIN K_SYNTHESIZED
%token <keyword> K_DEFINE K_DEFINES K_DEFINEB K_IF K_THEN K_ELSE K_FALSE K_TRUE 
%token <keyword> K_EQ K_NE K_LE K_LT K_GE K_GT K_OR K_AND K_NOT
%token <keyword> K_SPECIAL K_DIRECTION K_RANGE
%token <keyword> K_FPC K_HORIZONTAL K_VERTICAL K_ALIGN K_MIN K_MAX K_EQUAL
%token <keyword> K_BOTTOMLEFT K_TOPRIGHT K_ROWS K_TAPER K_TAPERRULE
%token <keyword> K_VERSION K_DIVIDERCHAR K_BUSBITCHARS
%token <keyword> K_PROPERTYDEFINITIONS K_STRING K_REAL K_INTEGER K_PROPERTY
%token <keyword> K_BEGINEXT K_ENDEXT K_NAMEMAPSTRING K_ON K_OFF K_X K_Y
%token <keyword> K_COMPONENT K_MASK K_MASKSHIFT K_COMPSMASKSHIFT K_SAMEMASK
%token <keyword> K_PINPROPERTIES K_TEST
%token <keyword> K_COMMONSCANPINS K_SNET K_COMPONENTPIN K_REENTRANTPATHS
%token <keyword> K_SHIELD K_SHIELDNET K_NOSHIELD K_VIRTUAL
%token <keyword> K_ANTENNAPINPARTIALMETALAREA K_ANTENNAPINPARTIALMETALSIDEAREA
%token <keyword> K_ANTENNAPINGATEAREA K_ANTENNAPINDIFFAREA
%token <keyword> K_ANTENNAPINMAXAREACAR K_ANTENNAPINMAXSIDEAREACAR
%token <keyword> K_ANTENNAPINPARTIALCUTAREA K_ANTENNAPINMAXCUTCAR
%token <keyword> K_SIGNAL K_POWER K_GROUND K_CLOCK K_TIEOFF K_ANALOG K_SCAN
%token <keyword> K_RESET K_RING K_STRIPE K_FOLLOWPIN K_IOWIRE K_COREWIRE
%token <keyword> K_BLOCKWIRE K_FILLWIRE K_BLOCKAGEWIRE K_PADRING K_BLOCKRING
%token <keyword> K_BLOCKAGES K_PLACEMENT K_SLOTS K_FILLS K_PUSHDOWN
%token <keyword> K_NETLIST K_DIST K_USER K_TIMING K_BALANCED K_STEINER K_TRUNK
%token <keyword> K_FIXEDBUMP K_FENCE K_FREQUENCY K_GUIDE K_MAXBITS
%token <keyword> K_PARTITION K_TYPE K_ANTENNAMODEL K_DRCFILL
%token <keyword> K_OXIDE1 K_OXIDE2 K_OXIDE3 K_OXIDE4
%token <keyword> K_CUTSIZE K_CUTSPACING K_DESIGNRULEWIDTH K_DIAGWIDTH
%token <keyword> K_ENCLOSURE K_HALO K_GROUNDSENSITIVITY
%token <keyword> K_HARDSPACING K_LAYERS K_MINCUTS K_NETEXPR 
%token <keyword> K_OFFSET K_ORIGIN K_ROWCOL K_STYLES
%token <keyword> K_POLYGON K_PORT K_SUPPLYSENSITIVITY K_VIA K_VIARULE K_WIREEXT
%token <keyword> K_EXCEPTPGNET K_FILLWIREOPC K_OPC K_PARTIAL K_ROUTEHALO
%type <pt>      pt opt_paren
%type <integer> comp_net_list subnet_opt_syn
%type <integer> orient pin_via_mask_opt
%type <integer> placement_status
%type <string>  net_type subnet_type track_start use_type shape_type source_type
%type <string>  pattern_type netsource_type
%type <tk>      path paths new_path
%type <string>  risefall opt_pin opt_pattern pin_layer_opt
%type <string>  vpin_status opt_plus track_type region_type
%type <string>  h_or_v turnoff_setup turnoff_hold
%type <integer> conn_opt partition_maxbits same_mask mask orient_pt

%%

def_file: version_stmt case_sens_stmt rules end_design
            ;

version_stmt:  // empty 
    | K_VERSION { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING ';'
      {
        defData->VersionNum = convert_defname2num($3);
        if (defData->VersionNum > CURRENT_VERSION) {
          char temp[300];
          sprintf(temp,
          "The execution has been stopped because the DEF parser %.1f does not support DEF file with version %s.\nUpdate your DEF file to version 5.8 or earlier.",
                  CURRENT_VERSION, $3);
          defError(6503, temp);
          return 1;
        }
        if (defCallbacks->VersionStrCbk) {
          CALLBACK(defCallbacks->VersionStrCbk, defrVersionStrCbkType, $3);
        } else if (defCallbacks->VersionCbk) {
            CALLBACK(defCallbacks->VersionCbk, defrVersionCbkType, defData->VersionNum);
        }
        if (defData->VersionNum > 5.3 && defData->VersionNum < 5.4)
          defData->defIgnoreVersion = 1;
        if (defData->VersionNum < 5.6)     // default to false before 5.6
          defData->names_case_sensitive = defSettings->reader_case_sensitive;
        else
          defData->names_case_sensitive = 1;
        defData->hasVer = 1;
        defData->doneDesign = 0;
    }

case_sens_stmt: // empty 
    | K_NAMESCASESENSITIVE K_ON ';'
      {
        if (defData->VersionNum < 5.6) {
          defData->names_case_sensitive = 1;
          if (defCallbacks->CaseSensitiveCbk)
            CALLBACK(defCallbacks->CaseSensitiveCbk, defrCaseSensitiveCbkType,
                     defData->names_case_sensitive); 
          defData->hasNameCase = 1;
        } else
          if (defCallbacks->CaseSensitiveCbk) // write error only if cbk is set 
             if (defData->caseSensitiveWarnings++ < defSettings->CaseSensitiveWarnings)
               defWarning(7011, "The NAMESCASESENSITIVE statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
      }
    | K_NAMESCASESENSITIVE K_OFF ';'
      {
        if (defData->VersionNum < 5.6) {
          defData->names_case_sensitive = 0;
          if (defCallbacks->CaseSensitiveCbk)
            CALLBACK(defCallbacks->CaseSensitiveCbk, defrCaseSensitiveCbkType,
                     defData->names_case_sensitive);
          defData->hasNameCase = 1;
        } else {
          if (defCallbacks->CaseSensitiveCbk) { // write error only if cbk is set 
            if (defData->caseSensitiveWarnings++ < defSettings->CaseSensitiveWarnings) {
              defError(6504, "Def parser version 5.7 and later does not support NAMESCASESENSITIVE OFF.\nEither remove this optional construct or set it to ON.");
              CHKERR();
            }
          }
        }
      }

rules: // empty 
        | rules rule
        | error 
            ;

rule: design_section | assertions_section | blockage_section | comps_section
      | constraint_section | extension_section | fill_section | comps_maskShift_section
      | floorplan_contraints_section | groups_section | iotiming_section
      | nets_section |  nondefaultrule_section | partitions_section
      | pin_props_section | regions_section | scanchains_section
      | slot_section | snets_section | styles_section | timingdisables_section
      | via_section
            ;

design_section: array_name | bus_bit_chars | canplace | cannotoccupy |
              design_name | die_area | divider_char | 
              floorplan_name | gcellgrid | history |
              pin_cap_rule | pin_rule | prop_def_section |
              row_rule | tech_name | tracks_rule | units
            ;



design_name: K_DESIGN {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING ';' 
      {
            if (defCallbacks->DesignCbk)
              CALLBACK(defCallbacks->DesignCbk, defrDesignStartCbkType, $3);
            defData->hasDes = 1;
          }

end_design: K_END K_DESIGN
          {
            defData->doneDesign = 1;
            if (defCallbacks->DesignEndCbk)
              CALLBACK(defCallbacks->DesignEndCbk, defrDesignEndCbkType, 0);
            // 11/16/2001 - pcr 408334
            // Return 1 if there is any defData->errors during parsing
            if (defData->errors)
                return 1;

            if (!defData->hasVer) {
              char temp[300];
              sprintf(temp, "No VERSION statement found, using the default value %2g.", defData->VersionNum);
              defWarning(7012, temp);            
            }
            if (!defData->hasNameCase && defData->VersionNum < 5.6)
              defWarning(7013, "The DEF file is invalid if NAMESCASESENSITIVE is undefined.\nNAMESCASESENSITIVE is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the NAMESCASESENSITIVE statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasBusBit && defData->VersionNum < 5.6)
              defWarning(7014, "The DEF file is invalid if BUSBITCHARS is undefined.\nBUSBITCHARS is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the BUSBITCHARS statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasDivChar && defData->VersionNum < 5.6)
              defWarning(7015, "The DEF file is invalid if DIVIDERCHAR is undefined.\nDIVIDERCHAR is a mandatory statement in the DEF file with version 5.6 and earlier.\nTo define the DIVIDERCHAR statement, refer to the LEF/DEF 5.5 and earlier Language Reference manual.");
            if (!defData->hasDes)
              defWarning(7016, "DESIGN is a mandatory statement in the DEF file. Ensure that it exists in the file.");
          }

tech_name: K_TECH { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING ';'
          { 
            if (defCallbacks->TechnologyCbk)
              CALLBACK(defCallbacks->TechnologyCbk, defrTechNameCbkType, $3);
          }

array_name: K_ARRAY {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING ';'
          { 
            if (defCallbacks->ArrayNameCbk)
              CALLBACK(defCallbacks->ArrayNameCbk, defrArrayNameCbkType, $3);
          }

floorplan_name: K_FLOORPLAN { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING ';'
          { 
            if (defCallbacks->FloorPlanNameCbk)
              CALLBACK(defCallbacks->FloorPlanNameCbk, defrFloorPlanNameCbkType, $3);
          }

history: K_HISTORY
          { 
            if (defCallbacks->HistoryCbk)
              CALLBACK(defCallbacks->HistoryCbk, defrHistoryCbkType, &defData->History_text[0]);
          }

prop_def_section: K_PROPERTYDEFINITIONS
          {
            defData->parsing_property = 1;
            defData->defInPropDef = 1;     // set flag as inside propertydefinitions 
            if (defCallbacks->PropDefStartCbk)
              CALLBACK(defCallbacks->PropDefStartCbk, defrPropDefStartCbkType, 0);
          }
    property_defs K_END K_PROPERTYDEFINITIONS
          { 
            if (defCallbacks->PropDefEndCbk)
              CALLBACK(defCallbacks->PropDefEndCbk, defrPropDefEndCbkType, 0);
            defData->real_num = 0;     // just want to make sure it is reset 
            defData->parsing_property = 0;
            defData->defInPropDef = 0;     // reset flag 
          }

property_defs: // empty 
        | property_defs property_def
            { }

property_def: K_DESIGN {defData->dumb_mode = 1; defData->no_num = 1; defData->Prop.clear(); }
              T_STRING property_type_and_val ';' 
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("design", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->DesignProp.setPropType($3, defData->defPropDefType);
            }
        | K_NET { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("net", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->NetProp.setPropType($3, defData->defPropDefType);
            }
        | K_SNET { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("specialnet", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->SNetProp.setPropType($3, defData->defPropDefType);
            }
        | K_REGION { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("region", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->RegionProp.setPropType($3, defData->defPropDefType);
            }
        | K_GROUP { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("group", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->GroupProp.setPropType($3, defData->defPropDefType);
            }
        | K_COMPONENT { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("component", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->CompProp.setPropType($3, defData->defPropDefType);
            }
        | K_ROW { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("row", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->RowProp.setPropType($3, defData->defPropDefType);
            }

        | K_COMPONENTPIN
          { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defCallbacks->PropCbk) {
                defData->Prop.setPropType("componentpin", $3);
                CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
              }
              defSettings->CompPinProp.setPropType($3, defData->defPropDefType);
            }
        | K_NONDEFAULTRULE
          { defData->dumb_mode = 1 ; defData->no_num = 1; defData->Prop.clear(); }
          T_STRING property_type_and_val ';'
            {
              if (defData->VersionNum < 5.6) {
                if (defData->nonDefaultWarnings++ < defSettings->NonDefaultWarnings) {
                  defData->defMsg = (char*)defMalloc(1000); 
                  sprintf (defData->defMsg,
                     "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6505, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              } else {
                if (defCallbacks->PropCbk) {
                  defData->Prop.setPropType("nondefaultrule", $3);
                  CALLBACK(defCallbacks->PropCbk, defrPropCbkType, &defData->Prop);
                }
                defSettings->NDefProp.setPropType($3, defData->defPropDefType);
              }
            }
        | error ';' { yyerrok; yyclearin;}

property_type_and_val: K_INTEGER { defData->real_num = 0 ;} opt_range opt_num_val
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropInteger();
              defData->defPropDefType = 'I';
            }
        | K_REAL { defData->real_num = 1 ;} opt_range opt_num_val
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropReal();
              defData->defPropDefType = 'R';
              defData->real_num = 0;
            }
        | K_STRING
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropString();
              defData->defPropDefType = 'S';
            }
        | K_STRING QSTRING
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropQString($2);
              defData->defPropDefType = 'Q';
            }
        | K_NAMEMAPSTRING T_STRING
            {
              if (defCallbacks->PropCbk) defData->Prop.setPropNameMapString($2);
              defData->defPropDefType = 'S';
            }

opt_num_val: // empty 
        | NUMBER
            { if (defCallbacks->PropCbk) defData->Prop.setNumber($1); }

units: K_UNITS K_DISTANCE K_MICRONS NUMBER ';'
          {
            if (defCallbacks->UnitsCbk) {
              if (defValidNum((int)$4))
                CALLBACK(defCallbacks->UnitsCbk,  defrUnitsCbkType, $4);
            }
          }

divider_char: K_DIVIDERCHAR QSTRING ';'
          {
            if (defCallbacks->DividerCbk)
              CALLBACK(defCallbacks->DividerCbk, defrDividerCbkType, $2);
            defData->hasDivChar = 1;
          }

bus_bit_chars: K_BUSBITCHARS QSTRING ';'
          { 
            if (defCallbacks->BusBitCbk)
              CALLBACK(defCallbacks->BusBitCbk, defrBusBitCbkType, $2);
            defData->hasBusBit = 1;
          }

canplace: K_CANPLACE {defData->dumb_mode = 1;defData->no_num = 1; } T_STRING NUMBER NUMBER
            orient K_DO NUMBER  K_BY NUMBER  K_STEP NUMBER NUMBER ';' 
            {
              if (defCallbacks->CanplaceCbk) {
                defData->Canplace.setName($3);
                defData->Canplace.setLocation($4,$5);
                defData->Canplace.setOrient($6);
                defData->Canplace.setDo($8,$10,$12,$13);
                CALLBACK(defCallbacks->CanplaceCbk, defrCanplaceCbkType,
                &(defData->Canplace));
              }
            }
cannotoccupy: K_CANNOTOCCUPY {defData->dumb_mode = 1;defData->no_num = 1; } T_STRING NUMBER NUMBER
            orient K_DO NUMBER  K_BY NUMBER  K_STEP NUMBER NUMBER ';' 
            {
              if (defCallbacks->CannotOccupyCbk) {
                defData->CannotOccupy.setName($3);
                defData->CannotOccupy.setLocation($4,$5);
                defData->CannotOccupy.setOrient($6);
                defData->CannotOccupy.setDo($8,$10,$12,$13);
                CALLBACK(defCallbacks->CannotOccupyCbk, defrCannotOccupyCbkType,
                        &(defData->CannotOccupy));
              }
            }

orient: K_N    {$$ = 0;}
        | K_W  {$$ = 1;}
        | K_S  {$$ = 2;}
        | K_E  {$$ = 3;}
        | K_FN {$$ = 4;}
        | K_FW {$$ = 5;}
        | K_FS {$$ = 6;}
        | K_FE {$$ = 7;}

die_area: K_DIEAREA
          {
            defData->Geometries.Reset();
          }
          firstPt nextPt otherPts ';'
          {
            if (defCallbacks->DieAreaCbk) {
               defData->DieArea.addPoint(&defData->Geometries);
               CALLBACK(defCallbacks->DieAreaCbk, defrDieAreaCbkType, &(defData->DieArea));
            }
          }

// 8/31/2001 - This is obsolete in 5.4 
pin_cap_rule: start_def_cap pin_caps end_def_cap
            { }

start_def_cap: K_DEFAULTCAP NUMBER 
        {
          if (defData->VersionNum < 5.4) {
             if (defCallbacks->DefaultCapCbk)
                CALLBACK(defCallbacks->DefaultCapCbk, defrDefaultCapCbkType, ROUND($2));
          } else {
             if (defCallbacks->DefaultCapCbk) // write error only if cbk is set 
                if (defData->defaultCapWarnings++ < defSettings->DefaultCapWarnings)
                   defWarning(7017, "The DEFAULTCAP statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }

pin_caps: // empty 
        | pin_caps pin_cap
            ;

pin_cap: K_MINPINS NUMBER K_WIRECAP NUMBER ';'
          {
            if (defData->VersionNum < 5.4) {
              if (defCallbacks->PinCapCbk) {
                defData->PinCap.setPin(ROUND($2));
                defData->PinCap.setCap($4);
                CALLBACK(defCallbacks->PinCapCbk, defrPinCapCbkType, &(defData->PinCap));
              }
            }
          }

end_def_cap: K_END K_DEFAULTCAP 
            { }

pin_rule: start_pins pins end_pins
            { }

start_pins: K_PINS NUMBER ';'
          { 
            if (defCallbacks->StartPinsCbk)
              CALLBACK(defCallbacks->StartPinsCbk, defrStartPinsCbkType, ROUND($2));
          }

pins: // empty 
        | pins pin
            ;

pin: '-' {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING '+' K_NET
         {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              defData->Pin.Setup($3, $7);
            }
            defData->hasPort = 0;
          }
        pin_options ';'
          { 
            if (defCallbacks->PinCbk)
              CALLBACK(defCallbacks->PinCbk, defrPinCbkType, &defData->Pin);
          }

pin_options: // empty 
        | pin_options pin_option

pin_option: '+' K_SPECIAL
          {
            if (defCallbacks->PinCbk)
              defData->Pin.setSpecial();
          }

        | extension_stmt
          { 
            if (defCallbacks->PinExtCbk)
              CALLBACK(defCallbacks->PinExtCbk, defrPinExtCbkType, &defData->History_text[0]);
          }

        | '+' K_DIRECTION T_STRING
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.setDirection($3);
          }

        | '+' K_NETEXPR QSTRING
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The NETEXPR statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6506, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setNetExpr($3);

            }
          }

        | '+' K_SUPPLYSENSITIVITY { defData->dumb_mode = 1; } T_STRING
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The SUPPLYSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6507, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setSupplySens($4);
            }
          }

        | '+' K_GROUNDSENSITIVITY { defData->dumb_mode = 1; } T_STRING
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The GROUNDSENSITIVITY statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6508, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.setGroundSens($4);
            }
          }

        | '+' K_USE use_type
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) defData->Pin.setUse($3);
          }
        | '+' K_PORT          // 5.7 
          {
            if (defData->VersionNum < 5.7) {
               if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                 if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                     (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                   defData->defMsg = (char*)defMalloc(10000);
                   sprintf (defData->defMsg,
                     "The PORT in PINS is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                   defError(6555, defData->defMsg);
                   defFree(defData->defMsg);
                   CHKERR();
                 }
               }
            } else {
               if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                 defData->Pin.addPort();
               defData->hasPort = 1;
            }
          }

        | '+' K_LAYER { defData->dumb_mode = 1; } T_STRING
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayer($4);
              else
                 defData->Pin.addLayer($4);
            }
          }
          pin_layer_mask_opt pin_layer_spacing_opt pt pt
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.addPortLayerPts($8.x, $8.y, $9.x, $9.y);
              else
                 defData->Pin.addLayerPts($8.x, $8.y, $9.x, $9.y);
            }
          }

        | '+' K_POLYGON { defData->dumb_mode = 1; } T_STRING
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6509, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolygon($4);
                else
                   defData->Pin.addPolygon($4);
              }
            }
            
            defData->Geometries.Reset();            
          }
          pin_poly_mask_opt pin_poly_spacing_opt firstPt nextPt nextPt otherPts
          {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolygonPts(&defData->Geometries);
                else
                   defData->Pin.addPolygonPts(&defData->Geometries);
              }
            }
          }
        | '+' K_VIA { defData->dumb_mode = 1; } T_STRING pin_via_mask_opt '(' NUMBER NUMBER ')'   // 5.7
          {
            if (defData->VersionNum < 5.7) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The PIN VIA statement is available in version 5.7 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6556, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortVia($4, (int)$7,
                                               (int)$8, $5);
                else
                   defData->Pin.addVia($4, (int)$7,
                                               (int)$8, $5);
              }
            }
          }
  
        | placement_status pt orient
          {
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
              if (defData->hasPort)
                 defData->Pin.setPortPlacement($1, $2.x, $2.y, $3);
              else
                 defData->Pin.setPlacement($1, $2.x, $2.y, $3);
            }
          }

        // The following is 5.4 syntax 
        | '+' K_ANTENNAPINPARTIALMETALAREA NUMBER pin_layer_opt
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALMETALAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6510, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialMetalArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINPARTIALMETALSIDEAREA NUMBER pin_layer_opt
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALMETALSIDEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6511, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialMetalSideArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINGATEAREA NUMBER pin_layer_opt
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINGATEAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6512, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
                defData->Pin.addAPinGateArea((int)$3, $4); 
            }
        | '+' K_ANTENNAPINDIFFAREA NUMBER pin_layer_opt
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINDIFFAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6513, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinDiffArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINMAXAREACAR NUMBER K_LAYER {defData->dumb_mode=1;} T_STRING
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6514, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxAreaCar((int)$3, $6); 
          }
        | '+' K_ANTENNAPINMAXSIDEAREACAR NUMBER K_LAYER {defData->dumb_mode=1;}
           T_STRING
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXSIDEAREACAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6515, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxSideAreaCar((int)$3, $6); 
          }
        | '+' K_ANTENNAPINPARTIALCUTAREA NUMBER pin_layer_opt
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINPARTIALCUTAREA statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6516, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinPartialCutArea((int)$3, $4); 
          }
        | '+' K_ANTENNAPINMAXCUTCAR NUMBER K_LAYER {defData->dumb_mode=1;} T_STRING
          {
            if (defData->VersionNum <= 5.3) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAPINMAXCUTCAR statement is available in version 5.4 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6517, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAPinMaxCutCar((int)$3, $6); 
          }
        | '+' K_ANTENNAMODEL pin_oxide
          {  // 5.5 syntax 
            if (defData->VersionNum < 5.5) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The ANTENNAMODEL statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6518, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
          }

pin_layer_mask_opt: // empty 
        | K_MASK NUMBER
         { 
           if (validateMaskInput((int)$2, defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerMask((int)$2);
                else
                   defData->Pin.addLayerMask((int)$2);
              }
           }
         }
         
pin_via_mask_opt: 
        // empty 
        { $$ = 0; }
        | K_MASK NUMBER
         { 
           if (validateMaskInput((int)$2, defData->pinWarnings, defSettings->PinWarnings)) {
             $$ = $2;
           }
         }
         
pin_poly_mask_opt: // empty 
        | K_MASK NUMBER
         { 
           if (validateMaskInput((int)$2, defData->pinWarnings, defSettings->PinWarnings)) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolyMask((int)$2);
                else
                   defData->Pin.addPolyMask((int)$2);
              }
           }
         }
   
         
pin_layer_spacing_opt: // empty 
        | K_SPACING NUMBER
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6519, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerSpacing((int)$2);
                else
                   defData->Pin.addLayerSpacing((int)$2);
              }
            }
          }
        | K_DESIGNRULEWIDTH NUMBER
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "DESIGNRULEWIDTH statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defData->VersionNum);
                  defError(6520, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortLayerDesignRuleWidth((int)$2);
                else
                   defData->Pin.addLayerDesignRuleWidth((int)$2);
              }
            }
          }

pin_poly_spacing_opt: // empty 
        | K_SPACING NUMBER
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "SPACING statement is a version 5.6 and later syntax.\nYour def file is defined with version %g", defData->VersionNum);
                  defError(6521, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolySpacing((int)$2);
                else
                   defData->Pin.addPolySpacing((int)$2);
              }
            }
          }
        | K_DESIGNRULEWIDTH NUMBER
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if ((defData->pinWarnings++ < defSettings->PinWarnings) &&
                    (defData->pinWarnings++ < defSettings->PinExtWarnings)) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The DESIGNRULEWIDTH statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                  defError(6520, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            } else {
              if (defCallbacks->PinCbk || defCallbacks->PinExtCbk) {
                if (defData->hasPort)
                   defData->Pin.addPortPolyDesignRuleWidth((int)$2);
                else
                   defData->Pin.addPolyDesignRuleWidth((int)$2);
              }
            }
          }

pin_oxide: K_OXIDE1
          { defData->aOxide = 1;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
        | K_OXIDE2
          { defData->aOxide = 2;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
        | K_OXIDE3
          { defData->aOxide = 3;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }
        | K_OXIDE4
          { defData->aOxide = 4;
            if (defCallbacks->PinCbk || defCallbacks->PinExtCbk)
              defData->Pin.addAntennaModel(defData->aOxide);
          }

use_type: K_SIGNAL
          { $$ = (char*)"SIGNAL"; }
        | K_POWER
          { $$ = (char*)"POWER"; }
        | K_GROUND
          { $$ = (char*)"GROUND"; }
        | K_CLOCK
          { $$ = (char*)"CLOCK"; }
        | K_TIEOFF
          { $$ = (char*)"TIEOFF"; }
        | K_ANALOG
          { $$ = (char*)"ANALOG"; }
        | K_SCAN
          { $$ = (char*)"SCAN"; }
        | K_RESET
          { $$ = (char*)"RESET"; }

pin_layer_opt:
        // empty 
          { $$ = (char*)""; }
        | K_LAYER {defData->dumb_mode=1;} T_STRING
          { $$ = $3; }

end_pins: K_END K_PINS
        { 
          if (defCallbacks->PinEndCbk)
            CALLBACK(defCallbacks->PinEndCbk, defrPinEndCbkType, 0);
        }

row_rule: K_ROW {defData->dumb_mode = 2; defData->no_num = 2; } T_STRING T_STRING NUMBER NUMBER
      orient
        {
          if (defCallbacks->RowCbk) {
            defData->rowName = $3;
            defData->Row.setup($3, $4, $5, $6, $7);
          }
        }
      row_do_option
      row_options ';'
        {
          if (defCallbacks->RowCbk) 
            CALLBACK(defCallbacks->RowCbk, defrRowCbkType, &defData->Row);
        }

row_do_option: // empty 
        {
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6523, "Invalid ROW statement defined in the DEF file. The DO statement which is required in the ROW statement is not defined.\nUpdate your DEF file with a DO statement.");
                CHKERR();
              }
            }
          }
        }
      | K_DO NUMBER K_BY NUMBER row_step_option
        {
          // 06/05/2002 - pcr 448455 
          // Check for 1 and 0 in the correct position 
          // 07/26/2002 - Commented out due to pcr 459218 
          if (defData->hasDoStep) {
            // 04/29/2004 - pcr 695535 
            // changed the testing 
            if ((($4 == 1) && (defData->yStep == 0)) ||
                (($2 == 1) && (defData->xStep == 0))) {
              // do nothing 
            } else 
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->RowCbk) {
                  if (defData->rowWarnings++ < defSettings->RowWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf(defData->defMsg,
                            "The DO statement in the ROW statement with the name %s has invalid syntax.\nThe valid syntax is \"DO numX BY 1 STEP spaceX 0 | DO 1 BY numY STEP 0 spaceY\".\nSpecify the valid syntax and try again.", defData->rowName);
                    defWarning(7018, defData->defMsg);
                    defFree(defData->defMsg);
                    }
                  }
              }
          }
          // pcr 459218 - Error if at least numX or numY does not equal 1 
          if (($2 != 1) && ($4 != 1)) {
            if (defCallbacks->RowCbk) {
              if (defData->rowWarnings++ < defSettings->RowWarnings) {
                defError(6524, "Invalid syntax specified. The valid syntax is either \"DO 1 BY num or DO num BY 1\". Specify the valid syntax and try again.");
                CHKERR();
              }
            }
          }
          if (defCallbacks->RowCbk)
            defData->Row.setDo(ROUND($2), ROUND($4), defData->xStep, defData->yStep);
        }

row_step_option: // empty 
        {
          defData->hasDoStep = 0;
        }
      | K_STEP NUMBER NUMBER
        {
          defData->hasDoStep = 1;
          defData->Row.setHasDoStep();
          defData->xStep = $2;
          defData->yStep = $3;
        }

row_options: // empty 
      | row_options row_option
      ;

row_option : '+' K_PROPERTY {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1;}
             row_prop_list
         { defData->dumb_mode = 0; defData->parsing_property = 0; }

row_prop_list : // empty 
       | row_prop_list row_prop
       ;
       
row_prop : T_STRING NUMBER
        {
          if (defCallbacks->RowCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp =  defSettings->RowProp.propType($1);
             CHKPROPTYPE(propTp, $1, "ROW");
             // For backword compatibility, also set the string value 
             sprintf(str, "%g", $2);
             defData->Row.addNumProperty($1, $2, str, propTp);
          }
        }
      | T_STRING QSTRING
        {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defSettings->RowProp.propType($1);
             CHKPROPTYPE(propTp, $1, "ROW");
             defData->Row.addProperty($1, $2, propTp);
          }
        }
      | T_STRING T_STRING
        {
          if (defCallbacks->RowCbk) {
             char propTp;
             propTp =  defSettings->RowProp.propType($1);
             CHKPROPTYPE(propTp, $1, "ROW");
             defData->Row.addProperty($1, $2, propTp);
          }
        }

tracks_rule: track_start NUMBER
        {
          if (defCallbacks->TrackCbk) {
            defData->Track.setup($1);
          }
        }
        K_DO NUMBER K_STEP NUMBER track_opts ';' 
        {
          if (($5 <= 0) && (defData->VersionNum >= 5.4)) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $5);
                defError(6525, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ($7 < 0) {
            if (defCallbacks->TrackCbk)
              if (defData->trackWarnings++ < defSettings->TrackWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in TRACK is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $7);
                defError(6526, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->TrackCbk) {
            defData->Track.setDo(ROUND($2), ROUND($5), $7);
            CALLBACK(defCallbacks->TrackCbk, defrTrackCbkType, &defData->Track);
          }
        }

track_start: K_TRACKS track_type
        {
          $$ = $2;
        }

track_type: K_X
            { $$ = (char*)"X";}
        | K_Y
            { $$ = (char*)"Y";}
            
track_opts: track_mask_statement track_layer_statement
        
track_mask_statement: // empty 
        | K_MASK NUMBER same_mask
           { 
              if (validateMaskInput((int)$2, defData->trackWarnings, defSettings->TrackWarnings)) {
                  if (defCallbacks->TrackCbk) {
                    defData->Track.addMask($2, $3);
                  }
               }
            }
            
same_mask: 
        // empty 
        { $$ = 0; }
        | K_SAMEMASK
        { $$ = 1; }
        
track_layer_statement: // empty 
        | K_LAYER { defData->dumb_mode = 1000; } track_layer track_layers
            { defData->dumb_mode = 0; }

track_layers: // empty 
        | track_layer track_layers 
            ;

track_layer: T_STRING
        {
          if (defCallbacks->TrackCbk)
            defData->Track.addLayer($1);
        }

gcellgrid: K_GCELLGRID track_type NUMBER 
     K_DO NUMBER K_STEP NUMBER ';'
        {
          if ($5 <= 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The DO number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $5);
                defError(6527, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if ($7 < 0) {
            if (defCallbacks->GcellGridCbk)
              if (defData->gcellGridWarnings++ < defSettings->GcellGridWarnings) {
                defData->defMsg = (char*)defMalloc(1000);
                sprintf (defData->defMsg,
                   "The STEP number %g in GCELLGRID is invalid.\nThe number value has to be greater than 0. Specify the valid syntax and try again.", $7);
                defError(6528, defData->defMsg);
                defFree(defData->defMsg);
              }
          }
          if (defCallbacks->GcellGridCbk) {
            defData->GcellGrid.setup($2, ROUND($3), ROUND($5), $7);
            CALLBACK(defCallbacks->GcellGridCbk, defrGcellGridCbkType, &defData->GcellGrid);
          }
        }

extension_section: K_BEGINEXT
        {
          if (defCallbacks->ExtensionCbk)
             CALLBACK(defCallbacks->ExtensionCbk, defrExtensionCbkType, &defData->History_text[0]);
        }

extension_stmt: '+' K_BEGINEXT
        { }

via_section: via via_declarations via_end
            ;
        
via: K_VIAS NUMBER ';' 
        {
          if (defCallbacks->ViaStartCbk)
            CALLBACK(defCallbacks->ViaStartCbk, defrViaStartCbkType, ROUND($2));
        }

via_declarations: // empty 
        | via_declarations via_declaration
            ;

via_declaration: '-' {defData->dumb_mode = 1;defData->no_num = 1; } T_STRING
            {
              if (defCallbacks->ViaCbk) defData->Via.setup($3);
              defData->viaRule = 0;
            }
        layer_stmts ';'
            {
              if (defCallbacks->ViaCbk)
                CALLBACK(defCallbacks->ViaCbk, defrViaCbkType, &defData->Via);
              defData->Via.clear();
            }

layer_stmts: // empty 
        | layer_stmts layer_stmt
            ;

layer_stmt: '+' K_RECT {defData->dumb_mode = 1;defData->no_num = 1; } T_STRING mask pt pt 
            { 
              if (defCallbacks->ViaCbk)
                if (validateMaskInput($5, defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addLayer($4, $6.x, $6.y, $7.x, $7.y, $5);
                }
            }
        | '+' K_POLYGON { defData->dumb_mode = 1; } T_STRING mask
            {
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf (defData->defMsg,
                       "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                    defError(6509, defData->defMsg);
                    defFree(defData->defMsg);
                    CHKERR();
                  }
                }
              }
              
              defData->Geometries.Reset();
              
            }
            firstPt nextPt nextPt otherPts
            {
              if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
                if (defCallbacks->ViaCbk)
                  if (validateMaskInput($5, defData->viaWarnings, defSettings->ViaWarnings)) {
                    defData->Via.addPolygon($4, &defData->Geometries, $5);
                  }
              }
            }
        | '+' K_PATTERNNAME {defData->dumb_mode = 1;defData->no_num = 1; } T_STRING
            {
              if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk)
                  defData->Via.addPattern($4);
              } else
                if (defCallbacks->ViaCbk)
                  if (defData->viaWarnings++ < defSettings->ViaWarnings)
                    defWarning(7019, "The PATTERNNAME statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement."); 
            }
        | '+' K_VIARULE {defData->dumb_mode = 1;defData->no_num = 1; } T_STRING
          '+' K_CUTSIZE NUMBER NUMBER
          '+' K_LAYERS {defData->dumb_mode = 3;defData->no_num = 1; } T_STRING T_STRING T_STRING
          '+' K_CUTSPACING NUMBER NUMBER
          '+' K_ENCLOSURE NUMBER NUMBER NUMBER NUMBER
            {
               defData->viaRule = 1;
               if (defData->VersionNum < 5.6) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defData->defMsg = (char*)defMalloc(1000);
                    sprintf (defData->defMsg,
                       "The VIARULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                    defError(6557, defData->defMsg);
                    defFree(defData->defMsg);
                    CHKERR();
                  }
                }
              } else {
                if (defCallbacks->ViaCbk)
                   defData->Via.addViaRule($4, (int)$7, (int)$8, $12, $13,
                                             $14, (int)$17, (int)$18, (int)$21,
                                             (int)$22, (int)$23, (int)$24); 
              }
            }
        | layer_viarule_opts
        | extension_stmt
          { 
            if (defCallbacks->ViaExtCbk)
              CALLBACK(defCallbacks->ViaExtCbk, defrViaExtCbkType, &defData->History_text[0]);
          }

layer_viarule_opts: '+' K_ROWCOL NUMBER NUMBER
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6559, "The ROWCOL statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addRowCol((int)$3, (int)$4);
            }
        | '+' K_ORIGIN NUMBER NUMBER
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6560, "The ORIGIN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOrigin((int)$3, (int)$4);
            }
        | '+' K_OFFSET NUMBER NUMBER NUMBER NUMBER
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6561, "The OFFSET statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addOffset((int)$3, (int)$4, (int)$5, (int)$6);
            }
        | '+' K_PATTERN {defData->dumb_mode = 1;defData->no_num = 1; } T_STRING 
            {
              if (!defData->viaRule) {
                if (defCallbacks->ViaCbk) {
                  if (defData->viaWarnings++ < defSettings->ViaWarnings) {
                    defError (6562, "The PATTERN statement is missing from the VIARULE statement. Ensure that it exists in the VIARULE statement.");
                    CHKERR();
                  }
                }
              } else if (defCallbacks->ViaCbk)
                 defData->Via.addCutPattern($4);
            }

firstPt: pt
          { defData->Geometries.startList($1.x, $1.y); }

nextPt: pt
          { defData->Geometries.addToList($1.x, $1.y); }

otherPts: // empty 
        | otherPts nextPt
        ;

pt: '(' NUMBER NUMBER ')'
          {
            defData->save_x = $2;
            defData->save_y = $3;
            $$.x = ROUND($2);
            $$.y = ROUND($3);
          }
        | '(' '*' NUMBER ')'
          {
            defData->save_y = $3;
            $$.x = ROUND(defData->save_x);
            $$.y = ROUND($3);
          }
        | '(' NUMBER '*' ')'
          {
            defData->save_x = $2;
            $$.x = ROUND($2);
            $$.y = ROUND(defData->save_y);
          }
        | '(' '*' '*' ')'
          {
            $$.x = ROUND(defData->save_x);
            $$.y = ROUND(defData->save_y);
          }
          
mask: // empty 
      { $$ = 0; }
      | '+' K_MASK NUMBER
      { $$ = $3; }

via_end: K_END K_VIAS
        { 
          if (defCallbacks->ViaEndCbk)
            CALLBACK(defCallbacks->ViaEndCbk, defrViaEndCbkType, 0);
        }

regions_section: regions_start regions_stmts K_END K_REGIONS
        {
          if (defCallbacks->RegionEndCbk)
            CALLBACK(defCallbacks->RegionEndCbk, defrRegionEndCbkType, 0);
        }

regions_start: K_REGIONS NUMBER ';'
        {
          if (defCallbacks->RegionStartCbk)
            CALLBACK(defCallbacks->RegionStartCbk, defrRegionStartCbkType, ROUND($2));
        }

regions_stmts: // empty 
        | regions_stmts regions_stmt
            {}

regions_stmt: '-' { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
        {
          if (defCallbacks->RegionCbk)
             defData->Region.setup($3);
          defData->regTypeDef = 0;
        }
     rect_list region_options ';'
        { CALLBACK(defCallbacks->RegionCbk, defrRegionCbkType, &defData->Region); }

rect_list :
      pt pt
        { if (defCallbacks->RegionCbk)
          defData->Region.addRect($1.x, $1.y, $2.x, $2.y); }
      | rect_list pt pt
        { if (defCallbacks->RegionCbk)
          defData->Region.addRect($2.x, $2.y, $3.x, $3.y); }
      ;

region_options: // empty 
      | region_options region_option
      ;

region_option : '+' K_PROPERTY {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
                region_prop_list
         { defData->dumb_mode = 0; defData->parsing_property = 0; }
      | '+' K_TYPE region_type      // 5.4.1 
         {
           if (defData->regTypeDef) {
              if (defCallbacks->RegionCbk) {
                if (defData->regionWarnings++ < defSettings->RegionWarnings) {
                  defError(6563, "The TYPE statement already exists. It has been defined in the REGION statement.");
                  CHKERR();
                }
              }
           }
           if (defCallbacks->RegionCbk) defData->Region.setType($3);
           defData->regTypeDef = 1;
         }
      ;

region_prop_list : // empty 
       | region_prop_list region_prop
       ;
       
region_prop : T_STRING NUMBER
        {
          if (defCallbacks->RegionCbk) {
             char propTp;
             char* str = ringCopy("                       ");
             propTp = defSettings->RegionProp.propType($1);
             CHKPROPTYPE(propTp, $1, "REGION");
             // For backword compatibility, also set the string value 
             // We will use a temporary string to store the number.
             // The string space is borrowed from the ring buffer
             // in the lexer.
             sprintf(str, "%g", $2);
             defData->Region.addNumProperty($1, $2, str, propTp);
          }
        }
      | T_STRING QSTRING
        {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defSettings->RegionProp.propType($1);
             CHKPROPTYPE(propTp, $1, "REGION");
             defData->Region.addProperty($1, $2, propTp);
          }
        }
      | T_STRING T_STRING
        {
          if (defCallbacks->RegionCbk) {
             char propTp;
             propTp = defSettings->RegionProp.propType($1);
             CHKPROPTYPE(propTp, $1, "REGION");
             defData->Region.addProperty($1, $2, propTp);
          }
        }

region_type: K_FENCE
            { $$ = (char*)"FENCE"; }
      | K_GUIDE
            { $$ = (char*)"GUIDE"; }
  
comps_maskShift_section : K_COMPSMASKSHIFT  layer_statement ';'
         {
           if (defData->VersionNum < 5.8) {
                if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                   defData->defMsg = (char*)defMalloc(10000);
                   sprintf (defData->defMsg,
                     "The MASKSHIFT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                   defError(7415, defData->defMsg);
                   defFree(defData->defMsg);
                   CHKERR();
                }
            }
            if (defCallbacks->ComponentMaskShiftLayerCbk) {
                CALLBACK(defCallbacks->ComponentMaskShiftLayerCbk, defrComponentMaskShiftLayerCbkType, &defData->ComponentMaskShiftLayer);
            }
         }
                 
comps_section: start_comps comps_rule end_comps
            ;
         
start_comps: K_COMPS NUMBER ';'
         { 
            if (defCallbacks->ComponentStartCbk)
              CALLBACK(defCallbacks->ComponentStartCbk, defrComponentStartCbkType,
                       ROUND($2));
         }
         
layer_statement : // empty 
         | layer_statement maskLayer
         ;
         
maskLayer: T_STRING
        {
            if (defCallbacks->ComponentMaskShiftLayerCbk) {
              defData->ComponentMaskShiftLayer.addMaskShiftLayer($1);
            }
        } 

comps_rule: // empty 
        | comps_rule comp
            ;

comp: comp_start comp_options ';'
         {
            if (defCallbacks->ComponentCbk)
              CALLBACK(defCallbacks->ComponentCbk, defrComponentCbkType, &defData->Component);
         }

comp_start: comp_id_and_name comp_net_list
         {
            defData->dumb_mode = 0;
            defData->no_num = 0;
         }

comp_id_and_name: '-' {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; }
       T_STRING T_STRING
         {
            if (defCallbacks->ComponentCbk)
              defData->Component.IdAndName($3, $4);
         }

comp_net_list: // empty 
        { }
        | comp_net_list '*'
            {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet("*");
            }
        | comp_net_list T_STRING
            {
              if (defCallbacks->ComponentCbk)
                defData->Component.addNet($2);
            }
            
comp_options: // empty 
        |     comp_options comp_option
            ;
    
comp_option:  comp_generate | comp_source | comp_type | weight | maskShift |
              comp_foreign | comp_region | comp_eeq | comp_halo |
              comp_routehalo | comp_property | comp_extension_stmt
            ;

comp_extension_stmt: extension_stmt
        {
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentExtCbk, defrComponentExtCbkType,
                     &defData->History_text[0]);
        }

comp_eeq: '+' K_EEQMASTER {defData->dumb_mode=1; defData->no_num = 1; } T_STRING
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setEEQ($4);
        }

comp_generate: '+' K_COMP_GEN { defData->dumb_mode = 2;  defData->no_num = 2; } T_STRING
    opt_pattern
        {
          if (defCallbacks->ComponentCbk)
             defData->Component.setGenerate($4, $5);
        }
opt_pattern :
    // empty 
      { $$ = (char*)""; }
    | T_STRING
      { $$ = $1; }

comp_source: '+' K_SOURCE source_type 
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setSource($3);
        }

source_type: K_NETLIST
            { $$ = (char*)"NETLIST"; }
        | K_DIST
            { $$ = (char*)"DIST"; }
        | K_USER
            { $$ = (char*)"USER"; }
        | K_TIMING
            { $$ = (char*)"TIMING"; }


comp_region:
        comp_region_start comp_pnt_list
        { }
        | comp_region_start T_STRING 
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setRegionName($2);
        }

comp_pnt_list: pt pt
        { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds($1.x, $1.y, 
                                                            $2.x, $2.y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        } 
    | comp_pnt_list pt pt
        { 
          // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->ComponentCbk)
               defData->Component.setRegionBounds($2.x, $2.y,
                                                            $3.x, $3.y);
          }
          else
            defWarning(7020, "The REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
        } 

comp_halo: '+' K_HALO                    // 5.7 
        {
          if (defData->VersionNum < 5.6) {
             if (defCallbacks->ComponentCbk) {
               if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The HALO statement is a version 5.6 and later syntax.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                 defError(6529, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
               }
             }
          }
        }
        halo_soft NUMBER NUMBER NUMBER NUMBER 
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setHalo((int)$5, (int)$6,
                                                 (int)$7, (int)$8);
        }

halo_soft: // 5.7 
    | K_SOFT
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->ComponentCbk) {
             if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The HALO SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6550, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defCallbacks->ComponentCbk)
             defData->Component.setHaloSoft();
        }
      }

// 5.7 
comp_routehalo: '+' K_ROUTEHALO NUMBER { defData->dumb_mode = 2; defData->no_num = 2; } T_STRING T_STRING
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->ComponentCbk) {
             if (defData->componentWarnings++ < defSettings->ComponentWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The ROUTEHALO is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6551, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defCallbacks->ComponentCbk)
             defData->Component.setRouteHalo(
                            (int)$3, $5, $6);
        }
      }

comp_property: '+' K_PROPERTY { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
      comp_prop_list
      { defData->dumb_mode = 0; defData->parsing_property = 0; }

comp_prop_list: comp_prop
    | comp_prop_list comp_prop
          ;

comp_prop: T_STRING NUMBER
        {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defSettings->CompProp.propType($1);
            CHKPROPTYPE(propTp, $1, "COMPONENT");
            sprintf(str, "%g", $2);
            defData->Component.addNumProperty($1, $2, str, propTp);
          }
        }
     | T_STRING QSTRING
        {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defSettings->CompProp.propType($1);
            CHKPROPTYPE(propTp, $1, "COMPONENT");
            defData->Component.addProperty($1, $2, propTp);
          }
        }
     | T_STRING T_STRING
        {
          if (defCallbacks->ComponentCbk) {
            char propTp;
            propTp = defSettings->CompProp.propType($1);
            CHKPROPTYPE(propTp, $1, "COMPONENT");
            defData->Component.addProperty($1, $2, propTp);
          }
        }

comp_region_start: '+' K_REGION
        { defData->dumb_mode = 1; defData->no_num = 1; }

comp_foreign: '+' K_FOREIGN { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
        opt_paren orient
        { 
          if (defData->VersionNum < 5.6) {
            if (defCallbacks->ComponentCbk) {
              defData->Component.setForeignName($4);
              defData->Component.setForeignLocation($5.x, $5.y, $6);
            }
         } else
            if (defCallbacks->ComponentCbk)
              if (defData->componentWarnings++ < defSettings->ComponentWarnings)
                defWarning(7021, "The FOREIGN statement is obsolete in version 5.6 and later.\nThe DEF parser will ignore this statement.");
         }

opt_paren:
       pt
         { $$ = $1; }
       | NUMBER NUMBER
         { $$.x = ROUND($1); $$.y = ROUND($2); }

comp_type: placement_status pt orient
        {
          if (defCallbacks->ComponentCbk) {
            defData->Component.setPlacementStatus($1);
            defData->Component.setPlacementLocation($2.x, $2.y, $3);
          }
        }
        | '+' K_UNPLACED
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setPlacementStatus(
                                         DEFI_COMPONENT_UNPLACED);
            defData->Component.setPlacementLocation(-1, -1, -1);
        }
        | '+' K_UNPLACED pt orient
        {
          if (defData->VersionNum < 5.4) {   // PCR 495463 
            if (defCallbacks->ComponentCbk) {
              defData->Component.setPlacementStatus(
                                          DEFI_COMPONENT_UNPLACED);
              defData->Component.setPlacementLocation($3.x, $3.y, $4);
            }
          } else {
            if (defData->componentWarnings++ < defSettings->ComponentWarnings)
               defWarning(7022, "In the COMPONENT UNPLACED statement, point and orient are invalid in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }

maskShift: '+' K_MASKSHIFT NUMBER
        {  
          if (defCallbacks->ComponentCbk) {
            if (validateMaskInput((int)$3, defData->componentWarnings, defSettings->ComponentWarnings)) {
                defData->Component.setMaskShift(int($3));
            }
          }
        }
        
placement_status: '+' K_FIXED 
        { $$ = DEFI_COMPONENT_FIXED; }
        | '+' K_COVER 
        { $$ = DEFI_COMPONENT_COVER; }
        | '+' K_PLACED
        { $$ = DEFI_COMPONENT_PLACED; }

weight: '+' K_WEIGHT NUMBER 
        {
          if (defCallbacks->ComponentCbk)
            defData->Component.setWeight(ROUND($3));
        }

end_comps: K_END K_COMPS
        { 
          if (defCallbacks->ComponentCbk)
            CALLBACK(defCallbacks->ComponentEndCbk, defrComponentEndCbkType, 0);
        }

nets_section:  start_nets net_rules end_nets
        ;

start_nets: K_NETS NUMBER ';'
        { 
          if (defCallbacks->NetStartCbk)
            CALLBACK(defCallbacks->NetStartCbk, defrNetStartCbkType, ROUND($2));
          defData->netOsnet = 1;
        }

net_rules: // empty 
        | net_rules one_net
            ;

one_net: net_and_connections net_options ';'
        { 
          if (defCallbacks->NetCbk)
            CALLBACK(defCallbacks->NetCbk, defrNetCbkType, &defData->Net);
        }
/*
** net_and_connections: net_start {defData->dumb_mode = DEF_MAX_INT; no_num = DEF_MAX_INT;}
**                      net_connections
** wmd -- this can be used to replace
**        | '(' K_PIN {defData->dumb_mode = 1; no_num = 1;} T_STRING conn_opt ')' (???)
*/
net_and_connections: net_start
        {defData->dumb_mode = 0; defData->no_num = 0; }

/* pcr 235555 & 236210 */
net_start: '-' {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; defData->nondef_is_keyword = TRUE; defData->mustjoin_is_keyword = TRUE;} net_name 

net_name: T_STRING
        {
          // 9/22/1999 
          // this is shared by both net and special net 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.setName($1);
          if (defCallbacks->NetNameCbk)
            CALLBACK(defCallbacks->NetNameCbk, defrNetNameCbkType, $1);
        } net_connections
        | K_MUSTJOIN '(' T_STRING {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING ')'
        {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addMustPin($3, $5, 0);
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }

net_connections: // empty 
        | net_connections net_connection 
        ;

net_connection: '(' T_STRING {defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT;}
                T_STRING conn_opt ')'
        {
          // 9/22/1999 
          // since the code is shared by both net & special net, 
          // need to check on both flags 
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin($2, $4, $5);
          // 1/14/2000 - pcr 289156 
          // reset defData->dumb_mode & defData->no_num to 3 , just in case 
          // the next statement is another net_connection 
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
        | '(' '*' {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING conn_opt ')'
        {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("*", $4, $5);
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }
        | '(' K_PIN {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING conn_opt ')'
        {
          if ((defCallbacks->NetCbk && (defData->netOsnet==1)) || (defCallbacks->SNetCbk && (defData->netOsnet==2)))
            defData->Net.addPin("PIN", $4, $5);
          defData->dumb_mode = 3;
          defData->no_num = 3;
        }

conn_opt: // empty 
          { $$ = 0; }
        | extension_stmt
        {
          if (defCallbacks->NetConnectionExtCbk)
            CALLBACK(defCallbacks->NetConnectionExtCbk, defrNetConnectionExtCbkType,
              &defData->History_text[0]);
          $$ = 0;
        }
        | '+' K_SYNTHESIZED
        { $$ = 1; }
        
        
// These are all the optional fields for a net that go after the '+' 
net_options: // empty 
        | net_options net_option
        ;

net_option: '+' net_type 
        {  
          if (defCallbacks->NetCbk) defData->Net.addWire($2, NULL);
        }
         paths
        {
          defData->by_is_keyword = FALSE;
          defData->do_is_keyword = FALSE;
          defData->new_is_keyword = FALSE;
          defData->nondef_is_keyword = FALSE;
          defData->mustjoin_is_keyword = FALSE;
          defData->step_is_keyword = FALSE;
          defData->orient_is_keyword = FALSE;
          defData->virtual_is_keyword = FALSE;
          defData->rect_is_keyword = FALSE;
          defData->mask_is_keyword = FALSE;
          defData->needNPCbk = 0;
        }

        | '+' K_SOURCE netsource_type
        { if (defCallbacks->NetCbk) defData->Net.setSource($3); }

        | '+' K_FIXEDBUMP
        {
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->NetCbk) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The FIXEDBUMP statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                 defError(6530, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
              }
            }
          }
          if (defCallbacks->NetCbk) defData->Net.setFixedbump();
        } 

        | '+' K_FREQUENCY { defData->real_num = 1; } NUMBER
        {
          if (defData->VersionNum < 5.5) {
            if (defCallbacks->NetCbk) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                 defData->defMsg = (char*)defMalloc(1000);
                 sprintf (defData->defMsg,
                    "The FREQUENCY statement is a version 5.5 and later syntax.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                 defError(6558, defData->defMsg);
                 defFree(defData->defMsg);
                 CHKERR();
              }
            }
          }
          if (defCallbacks->NetCbk) defData->Net.setFrequency($4);
          defData->real_num = 0;
        }

        | '+' K_ORIGINAL {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING
        { if (defCallbacks->NetCbk) defData->Net.setOriginal($4); }

        | '+' K_PATTERN pattern_type
        { if (defCallbacks->NetCbk) defData->Net.setPattern($3); }

        | '+' K_WEIGHT NUMBER
        { if (defCallbacks->NetCbk) defData->Net.setWeight(ROUND($3)); }

        | '+' K_XTALK NUMBER
        { if (defCallbacks->NetCbk) defData->Net.setXTalk(ROUND($3)); }

        | '+' K_ESTCAP NUMBER
        { if (defCallbacks->NetCbk) defData->Net.setCap($3); }

        | '+' K_USE use_type 
        { if (defCallbacks->NetCbk) defData->Net.setUse($3); }

        | '+' K_STYLE NUMBER
        { if (defCallbacks->NetCbk) defData->Net.setStyle((int)$3); }

        | '+' K_NONDEFAULTRULE { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
        { 
          if (defCallbacks->NetCbk && defCallbacks->NetNonDefaultRuleCbk) {
             // User wants a callback on nondefaultrule 
             CALLBACK(defCallbacks->NetNonDefaultRuleCbk,
                      defrNetNonDefaultRuleCbkType, $4);
          }
          // Still save data in the class 
          if (defCallbacks->NetCbk) defData->Net.setNonDefaultRule($4);
        }

        | vpin_stmt

        | '+' K_SHIELDNET { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
        { if (defCallbacks->NetCbk) defData->Net.addShieldNet($4); }

        | '+' K_NOSHIELD { defData->dumb_mode = 1; defData->no_num = 1; }
        { // since the parser still support 5.3 and earlier, can't 
          // move NOSHIELD in net_type 
          if (defData->VersionNum < 5.4) {   // PCR 445209 
            if (defCallbacks->NetCbk) defData->Net.addNoShield("");
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->shield = TRUE;    // save the path info in the defData->shield paths 
          } else
            if (defCallbacks->NetCbk) defData->Net.addWire("NOSHIELD", NULL);
        }
        paths
        {
          if (defData->VersionNum < 5.4) {   // PCR 445209 
            defData->shield = FALSE;
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->nondef_is_keyword = FALSE;
            defData->mustjoin_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
          } else {
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->nondef_is_keyword = FALSE;
            defData->mustjoin_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
          }
          defData->needNPCbk = 0;
        }

        | '+' K_SUBNET
        { defData->dumb_mode = 1; defData->no_num = 1;
          if (defCallbacks->NetCbk) {
            defData->Subnet = (defiSubnet*)defMalloc(sizeof(defiSubnet));
            defData->Subnet->Init();
          }
        }
        T_STRING {
          if (defCallbacks->NetCbk && defCallbacks->NetSubnetNameCbk) {
            // User wants a callback on Net subnetName 
            CALLBACK(defCallbacks->NetSubnetNameCbk, defrNetSubnetNameCbkType, $4);
          }
          // Still save the subnet name in the class 
          if (defCallbacks->NetCbk) {
            defData->Subnet->setName($4);
          }
        } 
        comp_names {
          defData->routed_is_keyword = TRUE;
          defData->fixed_is_keyword = TRUE;
          defData->cover_is_keyword = TRUE;
        } subnet_options {
          if (defCallbacks->NetCbk) {
            defData->Net.addSubnet(defData->Subnet);
            defData->Subnet = NULL;
            defData->routed_is_keyword = FALSE;
            defData->fixed_is_keyword = FALSE;
            defData->cover_is_keyword = FALSE;
          }
        }

        | '+' K_PROPERTY {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
          net_prop_list
        { defData->dumb_mode = 0; defData->parsing_property = 0; }

        | extension_stmt
        { 
          if (defCallbacks->NetExtCbk)
            CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]);
        }

net_prop_list: net_prop
      | net_prop_list net_prop
      ;

net_prop: T_STRING NUMBER
        {
          if (defCallbacks->NetCbk) {
            char propTp;
            char* str = ringCopy("                       ");
            propTp = defSettings->NetProp.propType($1);
            CHKPROPTYPE(propTp, $1, "NET");
            sprintf(str, "%g", $2);
            defData->Net.addNumProp($1, $2, str, propTp);
          }
        }
        | T_STRING QSTRING
        {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defSettings->NetProp.propType($1);
            CHKPROPTYPE(propTp, $1, "NET");
            defData->Net.addProp($1, $2, propTp);
          }
        }
        | T_STRING T_STRING
        {
          if (defCallbacks->NetCbk) {
            char propTp;
            propTp = defSettings->NetProp.propType($1);
            CHKPROPTYPE(propTp, $1, "NET");
            defData->Net.addProp($1, $2, propTp);
          }
        }

netsource_type: K_NETLIST
        { $$ = (char*)"NETLIST"; }
        | K_DIST
        { $$ = (char*)"DIST"; }
        | K_USER
        { $$ = (char*)"USER"; }
        | K_TIMING
        { $$ = (char*)"TIMING"; }
        | K_TEST
        { $$ = (char*)"TEST"; }

vpin_stmt: vpin_begin vpin_layer_opt pt pt 
        {
          // vpin_options may have to deal with orient 
          defData->orient_is_keyword = TRUE;
        }
        vpin_options
        { if (defCallbacks->NetCbk)
            defData->Net.addVpinBounds($3.x, $3.y, $4.x, $4.y);
          defData->orient_is_keyword = FALSE;
        }

vpin_begin: '+' K_VPIN {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING
        { if (defCallbacks->NetCbk) defData->Net.addVpin($4); }

vpin_layer_opt: // empty 
        | K_LAYER {defData->dumb_mode=1;} T_STRING
        { if (defCallbacks->NetCbk) defData->Net.addVpinLayer($3); }

vpin_options: // empty 
        | vpin_status pt orient
        { if (defCallbacks->NetCbk) defData->Net.addVpinLoc($1, $2.x, $2.y, $3); }

vpin_status: K_PLACED 
        { $$ = (char*)"PLACED"; }
        | K_FIXED 
        { $$ = (char*)"FIXED"; }
        | K_COVER
        { $$ = (char*)"COVER"; }

net_type: K_FIXED
        { $$ = (char*)"FIXED"; defData->dumb_mode = 1; }
        | K_COVER
        { $$ = (char*)"COVER"; defData->dumb_mode = 1; }
        | K_ROUTED
        { $$ = (char*)"ROUTED"; defData->dumb_mode = 1; }

paths:
    path   // not necessary to do partial callback for net yet
      { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      }
    | paths new_path
      { }

new_path: K_NEW { defData->dumb_mode = 1; } path
      { if (defData->NeedPathData && defCallbacks->NetCbk)
          pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needNPCbk);
      }

path:  T_STRING
      {
        if ((strcmp($1, "TAPER") == 0) || (strcmp($1, "TAPERRULE") == 0)) {
          if (defData->NeedPathData && defCallbacks->NetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6531, "The layerName which is required in path is missing. Include the layerName in the path and then try again.");
              CHKERR();
            }
          }
          // Since there is already error, the next token is insignificant 
          defData->dumb_mode = 1; defData->no_num = 1;
        } else {
          // CCR 766289 - Do not accummulate the layer information if there 
          // is not a callback set 
          if (defData->NeedPathData && defCallbacks->NetCbk)
              defData->PathObj.addLayer($1);
          defData->dumb_mode = 0; defData->no_num = 0;
        }
      }
    opt_taper_style_s  path_pt
      { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
/*
       dumb_mode = 1; by_is_keyword = TRUE; do_is_keyword = TRUE;
*/
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE; 
        defData->orient_is_keyword = TRUE; defData->virtual_is_keyword = TRUE;
        defData->mask_is_keyword = TRUE, defData->rect_is_keyword = TRUE;  }
    
       path_item_list
     
      { defData->dumb_mode = 0;   defData->virtual_is_keyword = FALSE; defData->mask_is_keyword = FALSE,
       defData->rect_is_keyword = FALSE; }
    
virtual_statement :
    K_VIRTUAL virtual_pt
    {
      if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The VIRTUAL statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
          }
    } 
  
rect_statement : 
    K_RECT rect_pts
    {
      if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The RECT statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
      }
    } 
              
    
path_item_list: // empty 
    | path_item_list path_item
    ;


path_item:
     T_STRING
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          if (strcmp($1, "TAPER") == 0)
            defData->PathObj.setTaper();
          else {
            defData->PathObj.addVia($1);
            }
        }
      }
    | K_MASK NUMBER T_STRING
      {
        if (validateMaskInput((int)$2, defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
              if (strcmp($3, "TAPER") == 0)
                defData->PathObj.setTaper();
              else {
                defData->PathObj.addViaMask($2);
                defData->PathObj.addVia($3);
                }
            }
        }
      }
    | T_STRING orient
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
            defData->PathObj.addVia($1);
            defData->PathObj.addViaRotation($2);
        }
      }
    | K_MASK NUMBER T_STRING orient
      { 
        if (validateMaskInput((int)$2, defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
                (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
                defData->PathObj.addViaMask($2);
                defData->PathObj.addVia($3);
                defData->PathObj.addViaRotation($4);
            }
        }
      }
    | K_MASK NUMBER T_STRING K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (validateMaskInput((int)$2, defData->sNetWarnings, defSettings->SNetWarnings)) {      
            if (($5 == 0) || ($7 == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask($2);
                defData->PathObj.addVia($3);
                defData->PathObj.addViaData((int)$5, (int)$7, (int)$9, (int)$10);
            }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
    | T_STRING K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (defData->VersionNum < 5.5) {
          if (defData->NeedPathData && 
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6532, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        }
        if (($3 == 0) || ($5 == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia($1);
            defData->PathObj.addViaData((int)$3, (int)$5, (int)$7, (int)$8);
        }  else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
    | T_STRING orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (defData->VersionNum < 5.5) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The VIA DO statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6532, defData->defMsg);
              CHKERR();
            }
          }
        }
        if (($4 == 0) || ($6 == 0)) {
          if (defData->NeedPathData &&
              defCallbacks->SNetCbk) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
              CHKERR();
            }
          }
        }
        if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
            defData->PathObj.addVia($1);
            defData->PathObj.addViaRotation($2);
            defData->PathObj.addViaData((int)$4, (int)$6, (int)$8, (int)$9);
        } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
          if (defData->netWarnings++ < defSettings->NetWarnings) {
            defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
            CHKERR();
          }
        }
      }
    | K_MASK NUMBER T_STRING orient K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
      {
        if (validateMaskInput((int)$2, defData->sNetWarnings, defSettings->SNetWarnings)) {
            if (($6 == 0) || ($8 == 0)) {
              if (defData->NeedPathData &&
                  defCallbacks->SNetCbk) {
                if (defData->netWarnings++ < defSettings->NetWarnings) {
                  defError(6533, "Either the numX or numY in the VIA DO statement has the value. The value specified is 0.\nUpdate your DEF file with the correct value and then try again.\n");
                  CHKERR();
                }
              }
            }
            if (defData->NeedPathData && (defCallbacks->SNetCbk && (defData->netOsnet==2))) {
                defData->PathObj.addViaMask($2); 
                defData->PathObj.addVia($3);
                defData->PathObj.addViaRotation($4);;
                defData->PathObj.addViaData((int)$6, (int)$8, (int)$10, (int)$11);
            } else if (defData->NeedPathData && (defCallbacks->NetCbk && (defData->netOsnet==1))) {
              if (defData->netWarnings++ < defSettings->NetWarnings) {
                defError(6567, "The VIA DO statement is defined in the NET statement and is invalid.\nRemove this statement from your DEF file and try again.");
                CHKERR();
              }
            }
        }
      }
   | virtual_statement
   | rect_statement 
   | K_MASK NUMBER K_RECT { defData->dumb_mode = 6; } '(' NUMBER NUMBER NUMBER NUMBER ')'
    {
      if (validateMaskInput((int)$2, defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask($2);
          defData->PathObj.addViaRect($6, $7, $8, $9);
        }
      }
    }
   | K_MASK NUMBER
    {
       if (validateMaskInput((int)$2, defData->sNetWarnings, defSettings->SNetWarnings)) {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addMask($2); 
        }
       }  
    }
    path_pt
  | path_pt 
    {
       // reset defData->dumb_mode to 1 just incase the next token is a via of the path
        // 2/5/2004 - pcr 686781
        defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
        defData->orient_is_keyword = TRUE;
    }  

      
path_pt :
     '(' NUMBER NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND($2), ROUND($3)); 
        defData->save_x = $2;
        defData->save_y = $3; 
      }
    | '(' '*' NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND($3)); 
        defData->save_y = $3;
      }
    | '(' NUMBER '*' ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND($2), ROUND(defData->save_y)); 
        defData->save_x = $2;
      }
    | '(' '*' '*' ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addPoint(ROUND(defData->save_x), ROUND(defData->save_y)); 
      }
    | '(' NUMBER NUMBER NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND($2), ROUND($3), ROUND($4)); 
        defData->save_x = $2;
        defData->save_y = $3;
      }
    | '(' '*' NUMBER NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND($3),
          ROUND($4)); 
        defData->save_y = $3;
      }
    | '(' NUMBER '*' NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND($2), ROUND(defData->save_y),
          ROUND($4)); 
        defData->save_x = $2;
      }
    | '(' '*' '*' NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addFlushPoint(ROUND(defData->save_x), ROUND(defData->save_y),
          ROUND($4)); 
      }

virtual_pt :
     '(' NUMBER NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND($2), ROUND($3)); 
        defData->save_x = $2;
        defData->save_y = $3;
      }
    | '(' '*' NUMBER ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND(defData->save_x), ROUND($3)); 
        defData->save_y = $3;
      }
    | '(' NUMBER '*' ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND($2), ROUND(defData->save_y)); 
        defData->save_x = $2;
      }
    | '(' '*' '*' ')'
      {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addVirtualPoint(ROUND(defData->save_x), ROUND(defData->save_y));
      }
 
rect_pts :
    '(' NUMBER NUMBER NUMBER NUMBER ')'
    {
        if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
          defData->PathObj.addViaRect($2, $3, $4, $5); 
        }    
    }   
    
                
opt_taper_style_s: // empty 
    | opt_taper_style_s opt_taper_style
    ;
opt_taper_style: opt_style
    | opt_taper
    ;

opt_taper: K_TAPER
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.setTaper(); }
    | K_TAPERRULE { defData->dumb_mode = 1; } T_STRING
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addTaperRule($3); }

opt_style: K_STYLE NUMBER
      { 
        if (defData->VersionNum < 5.6) {
           if (defData->NeedPathData && (defCallbacks->NetCbk ||
               defCallbacks->SNetCbk)) {
             if (defData->netWarnings++ < defSettings->NetWarnings) {
               defData->defMsg = (char*)defMalloc(1000);
               sprintf (defData->defMsg,
                  "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
               defError(6534, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else
           if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
             (defCallbacks->SNetCbk && (defData->netOsnet==2))))
             defData->PathObj.addStyle((int)$2);
      }

opt_spaths: // empty 
    | opt_spaths opt_shape_style
    ;

opt_shape_style:
    '+' K_SHAPE shape_type
      { if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
          (defCallbacks->SNetCbk && (defData->netOsnet==2))))
          defData->PathObj.addShape($3); }
    | '+' K_STYLE NUMBER
      { if (defData->VersionNum < 5.6) {
          if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2)))) {
            if (defData->netWarnings++ < defSettings->NetWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The STYLE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6534, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else {
          if (defData->NeedPathData && ((defCallbacks->NetCbk && (defData->netOsnet==1)) ||
            (defCallbacks->SNetCbk && (defData->netOsnet==2))))
            defData->PathObj.addStyle((int)$3);
        }
      }

end_nets: K_END K_NETS 
          { 
            CALLBACK(defCallbacks->NetEndCbk, defrNetEndCbkType, 0);
            defData->netOsnet = 0;
          }

shape_type: K_RING
            { $$ = (char*)"RING"; }
        | K_STRIPE
            { $$ = (char*)"STRIPE"; }
        | K_FOLLOWPIN
            { $$ = (char*)"FOLLOWPIN"; }
        | K_IOWIRE
            { $$ = (char*)"IOWIRE"; }
        | K_COREWIRE
            { $$ = (char*)"COREWIRE"; }
        | K_BLOCKWIRE
            { $$ = (char*)"BLOCKWIRE"; }
        | K_FILLWIRE
            { $$ = (char*)"FILLWIRE"; }
        | K_FILLWIREOPC                         // 5.7 
            {
              if (defData->VersionNum < 5.7) {
                 if (defData->NeedPathData) {
                   if (defData->fillWarnings++ < defSettings->FillWarnings) {
                     defData->defMsg = (char*)defMalloc(10000);
                     sprintf (defData->defMsg,
                       "The FILLWIREOPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                     defError(6552, defData->defMsg);
                     defFree(defData->defMsg);
                     CHKERR();
                  }
                }
              }
              $$ = (char*)"FILLWIREOPC";
            }
        | K_DRCFILL
            { $$ = (char*)"DRCFILL"; }
        | K_BLOCKAGEWIRE
            { $$ = (char*)"BLOCKAGEWIRE"; }
        | K_PADRING
            { $$ = (char*)"PADRING"; }
        | K_BLOCKRING
            { $$ = (char*)"BLOCKRING"; }

snets_section :  start_snets snet_rules end_snets
            ;

snet_rules: // empty 
        | snet_rules snet_rule
            ;

snet_rule: net_and_connections snet_options ';'
        { CALLBACK(defCallbacks->SNetCbk, defrSNetCbkType, &defData->Net); }

snet_options: // empty 
        | snet_options snet_option
            ;

snet_option: snet_width | snet_voltage | 
             snet_spacing | snet_other_option
             ;

snet_other_option: '+' net_type
            {
             if (defData->VersionNum >= 5.8) {
                defData->specialWire_routeStatus = $2;
             } else {
                 if (defCallbacks->SNetCbk) {   // PCR 902306 
                   defData->defMsg = (char*)defMalloc(1024);
                   sprintf(defData->defMsg, "The SPECIAL NET statement, with type %s, does not have any net statement defined.\nThe DEF parser will ignore this statemnet.", $2);
                   defWarning(7023, defData->defMsg);
                   defFree(defData->defMsg);
                 }
             }
            }
        |  '+' net_type
            {
            if (defCallbacks->SNetCbk) defData->Net.addWire($2, NULL);
            }
            spaths
            {
            // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
            if (defCallbacks->SNetWireCbk) {
               CALLBACK(defCallbacks->SNetWireCbk, defrSNetWireCbkType, &defData->Net);
               defData->Net.freeWire();
            }
            defData->by_is_keyword = FALSE;
            defData->do_is_keyword = FALSE;
            defData->new_is_keyword = FALSE;
            defData->step_is_keyword = FALSE;
            defData->orient_is_keyword = FALSE;
            defData->virtual_is_keyword = FALSE;
            defData->mask_is_keyword = FALSE;
            defData->rect_is_keyword = FALSE;
            defData->needSNPCbk = 0;
            }
 
        | '+' K_SHIELD { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
            { defData->shieldName = $4; 
              defData->specialWire_routeStatus = (char*)"SHIELD";
              defData->specialWire_routeStatusName = $4; 
            }
            shield_layer
            
        | '+' K_SHAPE shape_type
          {  
            defData->specialWire_shapeType = $3;
          }
        | '+' K_MASK NUMBER
          {
            if (validateMaskInput((int)$3, defData->sNetWarnings, defSettings->SNetWarnings)) {
                defData->specialWire_mask = $3;
            }     
          }
        | '+' K_POLYGON { defData->dumb_mode = 1; } T_STRING
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The POLYGON statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6535, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            
            defData->Geometries.Reset();
          }
          firstPt nextPt nextPt otherPts
          {
            if (defData->VersionNum >= 5.6) {  // only add if 5.6 or beyond
              if (defCallbacks->SNetCbk) {
                // defData->needSNPCbk will indicate that it has reach the max
                // memory and if user has set partialPathCBk, def parser
                // will make the callback.
                // This will improve performance
                // This construct is only in specialnet
                defData->Net.addPolygon($4, &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
                                                                defData->specialWire_routeStatusName);
                defData->specialWire_mask = 0;
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_shapeType = (char*)"";
                if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                   CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                            &defData->Net);
                   defData->Net.clearRectPolyNPath();
                   defData->Net.clearVia();
                }
              }
            }
          }

        | '+' K_RECT { defData->dumb_mode = 1; } T_STRING pt pt
          {
            if (defData->VersionNum < 5.6) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The RECT statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
            }
            if (defCallbacks->SNetCbk) {
              // defData->needSNPCbk will indicate that it has reach the max
              // memory and if user has set partialPathCBk, def parser
              // will make the callback.
              // This will improve performance
              // This construct is only in specialnet
              defData->Net.addRect($4, $5.x, $5.y, $6.x, $6.y, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType, defData->specialWire_routeStatusName);
              defData->specialWire_mask = 0;
              defData->specialWire_routeStatus = (char*)"ROUTED";
              defData->specialWire_shapeType = (char*)"";
              defData->specialWire_routeStatusName = (char*)"";
              if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                 CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                          &defData->Net);
                 defData->Net.clearRectPolyNPath();
                 defData->Net.clearVia();
              }
            }
          }
        | '+' K_VIA { defData->dumb_mode = 1; } T_STRING orient_pt
        {
          if (defData->VersionNum < 5.8) {
              if (defCallbacks->SNetCbk) {
                if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                  defData->defMsg = (char*)defMalloc(1000);
                  sprintf (defData->defMsg,
                     "The VIA statement is available in version 5.8 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
                  defError(6536, defData->defMsg);
                  defFree(defData->defMsg);
                  CHKERR();
                }
              }
          }
        }
        firstPt otherPts
        {
          if (defData->VersionNum >= 5.8 && defCallbacks->SNetCbk) {
              defData->Net.addPts($4, $5, &defData->Geometries, &defData->needSNPCbk, defData->specialWire_mask, defData->specialWire_routeStatus, defData->specialWire_shapeType,
                                                          defData->specialWire_routeStatusName);
              defData->specialWire_mask = 0;
              defData->specialWire_routeStatus = (char*)"ROUTED";
              defData->specialWire_shapeType = (char*)"";
              defData->specialWire_routeStatusName = (char*)"";
              if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
                 CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                          &defData->Net);
                 defData->Net.clearRectPolyNPath();
                 defData->Net.clearVia();
              }
            }
        }
 
        | '+' K_SOURCE source_type
            { if (defCallbacks->SNetCbk) defData->Net.setSource($3); }

        | '+' K_FIXEDBUMP
            { if (defCallbacks->SNetCbk) defData->Net.setFixedbump(); }
 
        | '+' K_FREQUENCY NUMBER
            { if (defCallbacks->SNetCbk) defData->Net.setFrequency($3); }

        | '+' K_ORIGINAL {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING
            { if (defCallbacks->SNetCbk) defData->Net.setOriginal($4); }
 
        | '+' K_PATTERN pattern_type
            { if (defCallbacks->SNetCbk) defData->Net.setPattern($3); }
 
        | '+' K_WEIGHT NUMBER
            { if (defCallbacks->SNetCbk) defData->Net.setWeight(ROUND($3)); }
 
        | '+' K_ESTCAP NUMBER
            { 
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setCap($3);
              else
                 defWarning(7024, "The ESTCAP statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }
 
        | '+' K_USE use_type
            { if (defCallbacks->SNetCbk) defData->Net.setUse($3); }
 
        | '+' K_STYLE NUMBER
            { if (defCallbacks->SNetCbk) defData->Net.setStyle((int)$3); }
 
        | '+' K_PROPERTY {defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
          snet_prop_list
            { defData->dumb_mode = 0; defData->parsing_property = 0; }
 
        | extension_stmt
          { CALLBACK(defCallbacks->NetExtCbk, defrNetExtCbkType, &defData->History_text[0]); }

orient_pt: // empty 
        { $$ = 0; }
        | K_N  {$$ = 0;}
        | K_W  {$$ = 1;}
        | K_S  {$$ = 2;}
        | K_E  {$$ = 3;}
        | K_FN {$$ = 4;}
        | K_FW {$$ = 5;}
        | K_FS {$$ = 6;}
        | K_FE {$$ = 7;}
        
shield_layer: // PCR 902306 
            {
                if (defCallbacks->SNetCbk) {
                    if (defData->VersionNum < 5.8) { 
                        defData->defMsg = (char*)defMalloc(1024);
                        sprintf(defData->defMsg, "The SPECIAL NET SHIELD statement doesn't have routing points definition.\nWill be ignored.");
                        defWarning(7025, defData->defMsg);
                        defFree(defData->defMsg);
                    } else {  // CCR 1244433
                      defData->specialWire_routeStatus = (char*)"SHIELD";
                      defData->specialWire_routeStatusName = defData->shieldName;
                    }
                }
            }
        |
            { // since the parser still supports 5.3 and earlier, 
              // can't just move SHIELD in net_type 
              if (defData->VersionNum < 5.4) { // PCR 445209 
                if (defCallbacks->SNetCbk) defData->Net.addShield(defData->shieldName);
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_routeStatusName = (char*)"";
                defData->shield = TRUE;   // save the path info in the defData->shield paths 
              } else
                if (defCallbacks->SNetCbk) defData->Net.addWire("SHIELD", defData->shieldName);
                defData->specialWire_routeStatus = (char*)"ROUTED";
                defData->specialWire_routeStatusName = (char*)"";
            }
            spaths
            {
              // 7/17/2003 - Fix for pcr 604848, add a callback for each wire
              if (defCallbacks->SNetWireCbk) {
                 CALLBACK(defCallbacks->SNetWireCbk, defrSNetWireCbkType, &defData->Net);
                 if (defData->VersionNum < 5.4)
                   defData->Net.freeShield();
                 else
                   defData->Net.freeWire();
              }
              if (defData->VersionNum < 5.4) {  // PCR 445209 
                defData->shield = FALSE;
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->nondef_is_keyword = FALSE;
                defData->mustjoin_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
              } else {
                defData->by_is_keyword = FALSE;
                defData->do_is_keyword = FALSE;
                defData->new_is_keyword = FALSE;
                defData->step_is_keyword = FALSE;
                defData->orient_is_keyword = FALSE;
                defData->virtual_is_keyword = FALSE;
                defData->mask_is_keyword = FALSE;
                defData->rect_is_keyword = FALSE;
              }
              defData->needSNPCbk = 0;
            }

snet_width: '+' K_WIDTH { defData->dumb_mode = 1; } T_STRING NUMBER
            {
              // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
              if (defData->VersionNum < 5.5)
                 if (defCallbacks->SNetCbk) defData->Net.setWidth($4, $5);
              else
                 defWarning(7026, "The WIDTH statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
            }

snet_voltage: '+' K_VOLTAGE  { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
            {
              if (numIsInt($4)) {
                 if (defCallbacks->SNetCbk) defData->Net.setVoltage(atoi($4));
              } else {
                 if (defCallbacks->SNetCbk) {
                   if (defData->sNetWarnings++ < defSettings->SNetWarnings) {
                     defData->defMsg = (char*)defMalloc(1000);
                     sprintf (defData->defMsg,
                        "The value %s for statement VOLTAGE is invalid. The value can only be integer.\nSpecify a valid value in units of millivolts", $4);
                     defError(6537, defData->defMsg);
                     defFree(defData->defMsg);
                     CHKERR();
                   }
                 }
              }
            }

snet_spacing: '+' K_SPACING { defData->dumb_mode = 1; } T_STRING NUMBER
            {
              if (defCallbacks->SNetCbk) defData->Net.setSpacing($4,$5);
            }
        opt_snet_range
            {
            }

snet_prop_list: snet_prop
      | snet_prop_list snet_prop
      ;

snet_prop: T_STRING NUMBER
            {
              if (defCallbacks->SNetCbk) {
                char propTp;
                char* str = ringCopy("                       ");
                propTp = defSettings->SNetProp.propType($1);
                CHKPROPTYPE(propTp, $1, "SPECIAL NET");
                // For backword compatibility, also set the string value 
                sprintf(str, "%g", $2);
                defData->Net.addNumProp($1, $2, str, propTp);
              }
            }
         | T_STRING QSTRING
            {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defSettings->SNetProp.propType($1);
                CHKPROPTYPE(propTp, $1, "SPECIAL NET");
                defData->Net.addProp($1, $2, propTp);
              }
            }
         | T_STRING T_STRING
            {
              if (defCallbacks->SNetCbk) {
                char propTp;
                propTp = defSettings->SNetProp.propType($1);
                CHKPROPTYPE(propTp, $1, "SPECIAL NET");
                defData->Net.addProp($1, $2, propTp);
              }
            }

opt_snet_range: // nothing 
        | K_RANGE NUMBER NUMBER
            {
              if (defCallbacks->SNetCbk) defData->Net.setRange($2,$3);
            }

opt_range: // nothing 
        | K_RANGE NUMBER NUMBER
            { defData->Prop.setRange($2, $3); }

pattern_type: K_BALANCED
            { $$ = (char*)"BALANCED"; }
        | K_STEINER
            { $$ = (char*)"STEINER"; }
        | K_TRUNK
            { $$ = (char*)"TRUNK"; }
        | K_WIREDLOGIC
            { $$ = (char*)"WIREDLOGIC"; }

spaths:
    spath
      { 
        if (defData->NeedPathData && defCallbacks->SNetCbk) {
           if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) { 
              // require a callback before proceed because defData->needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                       &defData->Net);
              defData->needSNPCbk = 0;   // reset the flag
              pathIsDone(defData->shield, 1, defData->netOsnet, &defData->needSNPCbk);
              defData->Net.clearRectPolyNPath();
              defData->Net.clearVia();
           } else
              pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needSNPCbk);
        }
      }
    | spaths snew_path
      { }

snew_path: K_NEW { defData->dumb_mode = 1; } spath
      { if (defData->NeedPathData && defCallbacks->SNetCbk) {
           if (defData->needSNPCbk && defCallbacks->SNetPartialPathCbk) {
              // require a callback before proceed because defData->needSNPCbk must be
              // set to 1 from the previous pathIsDone and user has registered
              // a callback routine.
              CALLBACK(defCallbacks->SNetPartialPathCbk, defrSNetPartialPathCbkType,
                       &defData->Net);
              defData->needSNPCbk = 0;   // reset the flag
              pathIsDone(defData->shield, 1, defData->netOsnet, &defData->needSNPCbk);
              // reset any poly or rect in special wiring statement
              defData->Net.clearRectPolyNPath();
              defData->Net.clearVia();
           } else
              pathIsDone(defData->shield, 0, defData->netOsnet, &defData->needSNPCbk);
        }
      }

spath:  T_STRING
      { if (defData->NeedPathData && defCallbacks->SNetCbk)
           defData->PathObj.addLayer($1);
        defData->dumb_mode = 0; defData->no_num = 0;
      }
    width opt_spaths path_pt
/*
      { dumb_mode = 1; new_is_keyword = TRUE; }
*/
      { defData->dumb_mode = DEF_MAX_INT; defData->by_is_keyword = TRUE; defData->do_is_keyword = TRUE;
        defData->new_is_keyword = TRUE; defData->step_is_keyword = TRUE;
         defData->orient_is_keyword = TRUE; defData->rect_is_keyword = TRUE, defData->mask_is_keyword = TRUE; 
         defData->virtual_is_keyword = TRUE;  }

    path_item_list
      { defData->dumb_mode = 0; defData->rect_is_keyword = FALSE, defData->mask_is_keyword = FALSE, defData->virtual_is_keyword = FALSE; }

width: NUMBER
      { if (defData->NeedPathData && defCallbacks->SNetCbk)
          defData->PathObj.addWidth(ROUND($1));
      }

start_snets: K_SNETS NUMBER ';'
      { 
        if (defCallbacks->SNetStartCbk)
          CALLBACK(defCallbacks->SNetStartCbk, defrSNetStartCbkType, ROUND($2));
        defData->netOsnet = 2;
      }

end_snets: K_END K_SNETS 
      { 
        if (defCallbacks->SNetEndCbk)
          CALLBACK(defCallbacks->SNetEndCbk, defrSNetEndCbkType, 0);
        defData->netOsnet = 0;
      }

groups_section: groups_start group_rules groups_end
      ;

groups_start: K_GROUPS NUMBER ';'
      {
        if (defCallbacks->GroupsStartCbk)
           CALLBACK(defCallbacks->GroupsStartCbk, defrGroupsStartCbkType, ROUND($2));
      }

group_rules: // empty 
      | group_rules group_rule
      ;

group_rule: start_group group_members group_options ';'
      {
        if (defCallbacks->GroupCbk)
           CALLBACK(defCallbacks->GroupCbk, defrGroupCbkType, &defData->Group);
      }

start_group: '-' { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING 
      {
        defData->dumb_mode = DEF_MAX_INT;
        defData->no_num = DEF_MAX_INT;
        /* dumb_mode is automatically turned off at the first
         * + in the options or at the ; at the end of the group */
        if (defCallbacks->GroupCbk) defData->Group.setup($3);
        if (defCallbacks->GroupNameCbk)
           CALLBACK(defCallbacks->GroupNameCbk, defrGroupNameCbkType, $3);
      }

group_members: 
      | group_members group_member
      {  }

group_member: T_STRING
      {
        // if (defCallbacks->GroupCbk) defData->Group.addMember($1); 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupMemberCbk, defrGroupMemberCbkType, $1);
      }

group_options: // empty 
      | group_options group_option 
      ;

group_option: '+' K_SOFT group_soft_options
      { }
      |     '+' K_PROPERTY { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
            group_prop_list
      { defData->dumb_mode = 0; defData->parsing_property = 0; }
      |     '+' K_REGION { defData->dumb_mode = 1;  defData->no_num = 1; } group_region
      { }
      | extension_stmt
      { 
        if (defCallbacks->GroupMemberCbk)
          CALLBACK(defCallbacks->GroupExtCbk, defrGroupExtCbkType, &defData->History_text[0]);
      }

group_region: pt pt
      {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5) {
          if (defCallbacks->GroupCbk)
            defData->Group.addRegionRect($1.x, $1.y, $2.x, $2.y);
        }
        else
          defWarning(7027, "The GROUP REGION pt pt statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
      | T_STRING
      { if (defCallbacks->GroupCbk)
          defData->Group.setRegionName($1);
      }

group_prop_list : // empty 
      | group_prop_list group_prop
      ;

group_prop : T_STRING NUMBER
      {
        if (defCallbacks->GroupCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->GroupProp.propType($1);
          CHKPROPTYPE(propTp, $1, "GROUP");
          sprintf(str, "%g", $2);
          defData->Group.addNumProperty($1, $2, str, propTp);
        }
      }
      | T_STRING QSTRING
      {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defSettings->GroupProp.propType($1);
          CHKPROPTYPE(propTp, $1, "GROUP");
          defData->Group.addProperty($1, $2, propTp);
        }
      }
      | T_STRING T_STRING
      {
        if (defCallbacks->GroupCbk) {
          char propTp;
          propTp = defSettings->GroupProp.propType($1);
          CHKPROPTYPE(propTp, $1, "GROUP");
          defData->Group.addProperty($1, $2, propTp);
        }
      }

group_soft_options: // empty 
      | group_soft_options group_soft_option 
      { }

group_soft_option: K_MAXX NUMBER
      {
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxX(ROUND($2));
        else
          defWarning(7028, "The GROUP SOFT MAXX statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
      | K_MAXY NUMBER
      { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setMaxY(ROUND($2));
        else
          defWarning(7029, "The GROUP SOFT MAXY statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }
      | K_MAXHALFPERIMETER NUMBER
      { 
        // 11/12/2002 - this is obsolete in 5.5, & will be ignored 
        if (defData->VersionNum < 5.5)
          if (defCallbacks->GroupCbk) defData->Group.setPerim(ROUND($2));
        else
          defWarning(7030, "The GROUP SOFT MAXHALFPERIMETER statement is obsolete in version 5.5 and later.\nThe DEF parser will ignore this statement.");
      }

groups_end: K_END K_GROUPS 
      { 
        if (defCallbacks->GroupsEndCbk)
          CALLBACK(defCallbacks->GroupsEndCbk, defrGroupsEndCbkType, 0);
      }

// 8/31/2001 - This is obsolete in 5.4 
assertions_section: assertions_start constraint_rules assertions_end
      ;

// 8/31/2001 - This is obsolete in 5.4 
constraint_section: constraints_start constraint_rules constraints_end
      ;

assertions_start: K_ASSERTIONS NUMBER ';'
      {
        if ((defData->VersionNum < 5.4) && (defCallbacks->AssertionsStartCbk)) {
          CALLBACK(defCallbacks->AssertionsStartCbk, defrAssertionsStartCbkType,
                   ROUND($2));
        } else {
          if (defCallbacks->AssertionCbk)
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->AssertionCbk)
          defData->Assertion.setAssertionMode();
      }

constraints_start: K_CONSTRAINTS NUMBER ';'
      {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintsStartCbk)) {
          CALLBACK(defCallbacks->ConstraintsStartCbk, defrConstraintsStartCbkType,
                   ROUND($2));
        } else {
          if (defCallbacks->ConstraintCbk)
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
        if (defCallbacks->ConstraintCbk)
          defData->Assertion.setConstraintMode();
      }

constraint_rules: // empty 
      | constraint_rules constraint_rule 
      ;

constraint_rule:   operand_rule 
      | wiredlogic_rule 
      {
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
      }

operand_rule: '-' operand delay_specs ';'
      { 
        if ((defData->VersionNum < 5.4) && (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)) {
          if (defData->Assertion.isConstraint()) 
            CALLBACK(defCallbacks->ConstraintCbk, defrConstraintCbkType, &defData->Assertion);
          if (defData->Assertion.isAssertion()) 
            CALLBACK(defCallbacks->AssertionCbk, defrAssertionCbkType, &defData->Assertion);
        }
   
        // reset all the flags and everything
        defData->Assertion.clear();
      }

operand: K_NET { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING 
      {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addNet($3);
      }
      | K_PATH {defData->dumb_mode = 4; defData->no_num = 4;} T_STRING T_STRING T_STRING T_STRING
      {
         if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.addPath($3, $4, $5, $6);
      }
      | K_SUM  '(' operand_list ')'
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setSum();
      }
      | K_DIFF '(' operand_list ')'
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
           defData->Assertion.setDiff();
      }

operand_list: operand 
      | operand_list operand
      { }
      | operand_list ',' operand

wiredlogic_rule: '-' K_WIREDLOGIC { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      opt_plus K_MAXDIST NUMBER ';'
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setWiredlogic($4, $7);
      }

opt_plus:
      // empty 
      { $$ = (char*)""; }
      | '+'
      { $$ = (char*)"+"; }

delay_specs: // empty 
      | delay_specs delay_spec
      ;

delay_spec: '+' K_RISEMIN NUMBER 
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMin($3);
      }
      | '+' K_RISEMAX NUMBER 
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setRiseMax($3);
      }
      | '+' K_FALLMIN NUMBER 
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMin($3);
      }
      | '+' K_FALLMAX NUMBER 
      {
        if (defCallbacks->ConstraintCbk || defCallbacks->AssertionCbk)
          defData->Assertion.setFallMax($3);
      }

constraints_end: K_END K_CONSTRAINTS
      { if ((defData->VersionNum < 5.4) && defCallbacks->ConstraintsEndCbk) {
          CALLBACK(defCallbacks->ConstraintsEndCbk, defrConstraintsEndCbkType, 0);
        } else {
          if (defCallbacks->ConstraintsEndCbk) {
            if (defData->constraintWarnings++ < defSettings->ConstraintWarnings)
              defWarning(7032, "The CONSTRAINTS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }

assertions_end: K_END K_ASSERTIONS
      { if ((defData->VersionNum < 5.4) && defCallbacks->AssertionsEndCbk) {
          CALLBACK(defCallbacks->AssertionsEndCbk, defrAssertionsEndCbkType, 0);
        } else {
          if (defCallbacks->AssertionsEndCbk) {
            if (defData->assertionWarnings++ < defSettings->AssertionWarnings)
              defWarning(7031, "The ASSERTIONS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
          }
        }
      }

scanchains_section: scanchain_start scanchain_rules scanchain_end
      ;

scanchain_start: K_SCANCHAINS NUMBER ';'
      { if (defCallbacks->ScanchainsStartCbk)
          CALLBACK(defCallbacks->ScanchainsStartCbk, defrScanchainsStartCbkType,
                   ROUND($2));
      }

scanchain_rules: // empty 
      | scanchain_rules scan_rule
      {}

scan_rule: start_scan scan_members ';' 
      { 
        if (defCallbacks->ScanchainCbk)
          CALLBACK(defCallbacks->ScanchainCbk, defrScanchainCbkType, &defData->Scanchain);
      }

start_scan: '-' {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING 
      {
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setName($3);
        defData->bit_is_keyword = TRUE;
      }

scan_members: 
      | scan_members scan_member
      ;

opt_pin :
      // empty 
      { $$ = (char*)""; }
      | T_STRING
      { $$ = $1; }

scan_member: '+' K_START {defData->dumb_mode = 2; defData->no_num = 2;} T_STRING opt_pin
      { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStart($4, $5);
      }
      | '+' K_FLOATING { defData->dumb_mode = 1; defData->no_num = 1; } floating_inst_list
      { defData->dumb_mode = 0; defData->no_num = 0; }
      | '+' K_ORDERED
      {
         defData->dumb_mode = 1;
         defData->no_num = 1;
         if (defCallbacks->ScanchainCbk)
           defData->Scanchain.addOrderedList();
      }
      ordered_inst_list
      { defData->dumb_mode = 0; defData->no_num = 0; }
      | '+' K_STOP {defData->dumb_mode = 2; defData->no_num = 2; } T_STRING opt_pin
      { if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setStop($4, $5);
      }
      | '+' K_COMMONSCANPINS { defData->dumb_mode = 10; defData->no_num = 10; } opt_common_pins
      { defData->dumb_mode = 0;  defData->no_num = 0; }
      | '+' K_PARTITION { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING  // 5.5 
      partition_maxbits
      {
        if (defData->VersionNum < 5.5) {
          if (defCallbacks->ScanchainCbk) {
            if (defData->scanchainWarnings++ < defSettings->ScanchainWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The PARTITION statement is available in version 5.5 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6538, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        }
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.setPartition($4, $5);
      }
      | extension_stmt
      {
        if (defCallbacks->ScanChainExtCbk)
          CALLBACK(defCallbacks->ScanChainExtCbk, defrScanChainExtCbkType, &defData->History_text[0]);
      }

opt_common_pins: // empty 
      { }
      | '(' T_STRING T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.setCommonIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.setCommonOut($3);
        }
      }
      | '(' T_STRING T_STRING ')' '(' T_STRING T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.setCommonIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.setCommonOut($3);
          if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
            defData->Scanchain.setCommonIn($7);
          else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
            defData->Scanchain.setCommonOut($7);
        }
      }

floating_inst_list: // empty 
      | floating_inst_list one_floating_inst
      ;

one_floating_inst: T_STRING
      {
        defData->dumb_mode = 1000;
        defData->no_num = 1000;
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addFloatingInst($1);
      }
      floating_pins
      { defData->dumb_mode = 1; defData->no_num = 1; }

floating_pins: // empty  
      { }
      | '(' T_STRING  T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.addFloatingIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.addFloatingOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            defData->bitsNum = atoi($3);
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.addFloatingIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.addFloatingOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            defData->bitsNum = atoi($3);
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
            defData->Scanchain.addFloatingIn($7);
          else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
            defData->Scanchain.addFloatingOut($7);
          else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            defData->bitsNum = atoi($7);
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')' '(' T_STRING
      T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.addFloatingIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.addFloatingOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            defData->bitsNum = atoi($3);
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
            defData->Scanchain.addFloatingIn($7);
          else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
            defData->Scanchain.addFloatingOut($7);
          else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            defData->bitsNum = atoi($7);
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
          if (strcmp($10, "IN") == 0 || strcmp($10, "in") == 0)
            defData->Scanchain.addFloatingIn($11);
          else if (strcmp($10, "OUT") == 0 || strcmp($10, "out") == 0)
            defData->Scanchain.addFloatingOut($11);
          else if (strcmp($10, "BITS") == 0 || strcmp($10, "bits") == 0) {
            defData->bitsNum = atoi($11);
            defData->Scanchain.setFloatingBits(defData->bitsNum);
          }
        }
      }
    
ordered_inst_list: // empty 
      | ordered_inst_list one_ordered_inst
      ;

one_ordered_inst: T_STRING
      { defData->dumb_mode = 1000; defData->no_num = 1000; 
        if (defCallbacks->ScanchainCbk)
          defData->Scanchain.addOrderedInst($1);
      }
      ordered_pins
      { defData->dumb_mode = 1; defData->no_num = 1; }

ordered_pins: // empty  
      { }
      | '(' T_STRING  T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.addOrderedIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.addOrderedOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            defData->bitsNum = atoi($3);
            defData->Scanchain.setOrderedBits(defData->bitsNum);
         }
        }
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.addOrderedIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.addOrderedOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            defData->bitsNum = atoi($3);
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
            defData->Scanchain.addOrderedIn($7);
          else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
            defData->Scanchain.addOrderedOut($7);
          else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            defData->bitsNum = atoi($7);
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      }
      | '(' T_STRING  T_STRING ')' '(' T_STRING  T_STRING ')' '(' T_STRING
      T_STRING ')'
      {
        if (defCallbacks->ScanchainCbk) {
          if (strcmp($2, "IN") == 0 || strcmp($2, "in") == 0)
            defData->Scanchain.addOrderedIn($3);
          else if (strcmp($2, "OUT") == 0 || strcmp($2, "out") == 0)
            defData->Scanchain.addOrderedOut($3);
          else if (strcmp($2, "BITS") == 0 || strcmp($2, "bits") == 0) {
            defData->bitsNum = atoi($3);
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp($6, "IN") == 0 || strcmp($6, "in") == 0)
            defData->Scanchain.addOrderedIn($7);
          else if (strcmp($6, "OUT") == 0 || strcmp($6, "out") == 0)
            defData->Scanchain.addOrderedOut($7);
          else if (strcmp($6, "BITS") == 0 || strcmp($6, "bits") == 0) {
            defData->bitsNum = atoi($7);
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
          if (strcmp($10, "IN") == 0 || strcmp($10, "in") == 0)
            defData->Scanchain.addOrderedIn($11);
          else if (strcmp($10, "OUT") == 0 || strcmp($10, "out") == 0)
            defData->Scanchain.addOrderedOut($11);
          else if (strcmp($10, "BITS") == 0 || strcmp($10, "bits") == 0) {
            defData->bitsNum = atoi($11);
            defData->Scanchain.setOrderedBits(defData->bitsNum);
          }
        }
      }
    
partition_maxbits: // empty 
      { $$ = -1; }
      | K_MAXBITS NUMBER
      { $$ = ROUND($2); }
    
scanchain_end: K_END K_SCANCHAINS
      { 
        if (defCallbacks->ScanchainsEndCbk)
          CALLBACK(defCallbacks->ScanchainsEndCbk, defrScanchainsEndCbkType, 0);
        defData->bit_is_keyword = FALSE;
        defData->dumb_mode = 0; defData->no_num = 0;
      }

// 8/31/2001 - This is obsolete in 5.4 
iotiming_section: iotiming_start iotiming_rules iotiming_end
      ;

iotiming_start: K_IOTIMINGS NUMBER ';'
      {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsStartCbk) {
          CALLBACK(defCallbacks->IOTimingsStartCbk, defrIOTimingsStartCbkType, ROUND($2));
        } else {
          if (defCallbacks->IOTimingsStartCbk)
            if (defData->iOTimingWarnings++ < defSettings->IOTimingWarnings)
              defWarning(7035, "The IOTIMINGS statement is obsolete in version 5.4 and later.\nThe DEF parser will ignore this statement.");
        }
      }

iotiming_rules: // empty 
      | iotiming_rules iotiming_rule
      { }

iotiming_rule: start_iotiming iotiming_members ';' 
      { 
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingCbk)
          CALLBACK(defCallbacks->IOTimingCbk, defrIOTimingCbkType, &defData->IOTiming);
      } 

start_iotiming: '-' '(' {defData->dumb_mode = 2; defData->no_num = 2; } T_STRING T_STRING ')'
      {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setName($4, $5);
      }

iotiming_members: 
      | iotiming_members iotiming_member
      ;

iotiming_member:
      '+' risefall K_VARIABLE NUMBER NUMBER
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setVariable($2, $4, $5);
      }
      | '+' risefall K_SLEWRATE NUMBER NUMBER
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setSlewRate($2, $4, $5);
      }
      | '+' K_CAPACITANCE NUMBER
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setCapacitance($3);
      }
      | '+' K_DRIVECELL {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setDriveCell($4);
      } iotiming_drivecell_opt
      // | '+' K_FROMPIN   {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING

      // | '+' K_PARALLEL NUMBER

      | extension_stmt
      {
        if (defData->VersionNum < 5.4 && defCallbacks->IoTimingsExtCbk)
          CALLBACK(defCallbacks->IoTimingsExtCbk, defrIoTimingsExtCbkType, &defData->History_text[0]);
      }

iotiming_drivecell_opt: iotiming_frompin
      K_TOPIN {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defCallbacks->IOTimingCbk) 
          defData->IOTiming.setTo($4);
      }
      iotiming_parallel

iotiming_frompin: // empty 
      | K_FROMPIN {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setFrom($3);
      }

iotiming_parallel: // empty 
      | K_PARALLEL NUMBER
      {
        if (defCallbacks->IOTimingCbk)
          defData->IOTiming.setParallel($2);
      }

risefall: K_RISE { $$ = (char*)"RISE"; } | K_FALL { $$ = (char*)"FALL"; }

iotiming_end: K_END K_IOTIMINGS
      {
        if (defData->VersionNum < 5.4 && defCallbacks->IOTimingsEndCbk)
          CALLBACK(defCallbacks->IOTimingsEndCbk, defrIOTimingsEndCbkType, 0);
      }

floorplan_contraints_section: fp_start fp_stmts K_END K_FPC
      { 
        if (defCallbacks->FPCEndCbk)
          CALLBACK(defCallbacks->FPCEndCbk, defrFPCEndCbkType, 0);
      }

fp_start: K_FPC NUMBER ';'
      {
        if (defCallbacks->FPCStartCbk)
          CALLBACK(defCallbacks->FPCStartCbk, defrFPCStartCbkType, ROUND($2));
      }

fp_stmts: // empty 
      | fp_stmts fp_stmt
      {}

fp_stmt: '-' { defData->dumb_mode = 1; defData->no_num = 1;  } T_STRING h_or_v
      { if (defCallbacks->FPCCbk) defData->FPC.setName($3, $4); }
      constraint_type constrain_what_list ';'
      { if (defCallbacks->FPCCbk) CALLBACK(defCallbacks->FPCCbk, defrFPCCbkType, &defData->FPC); }

h_or_v: K_HORIZONTAL 
      { $$ = (char*)"HORIZONTAL"; }
      | K_VERTICAL
      { $$ = (char*)"VERTICAL"; }

constraint_type: K_ALIGN
      { if (defCallbacks->FPCCbk) defData->FPC.setAlign(); }
      | K_MAX NUMBER
      { if (defCallbacks->FPCCbk) defData->FPC.setMax($2); }
      | K_MIN NUMBER
      { if (defCallbacks->FPCCbk) defData->FPC.setMin($2); }
      | K_EQUAL NUMBER
      { if (defCallbacks->FPCCbk) defData->FPC.setEqual($2); }

constrain_what_list: // empty 
      | constrain_what_list constrain_what
      ;

constrain_what: '+' K_BOTTOMLEFT
      { if (defCallbacks->FPCCbk) defData->FPC.setDoingBottomLeft(); }
      row_or_comp_list 
      |       '+' K_TOPRIGHT
      { if (defCallbacks->FPCCbk) defData->FPC.setDoingTopRight(); }
      row_or_comp_list 
      ;

row_or_comp_list: // empty 
      | row_or_comp_list row_or_comp

row_or_comp: '(' K_ROWS  {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING ')'
      { if (defCallbacks->FPCCbk) defData->FPC.addRow($4); }
      |    '(' K_COMPS {defData->dumb_mode = 1; defData->no_num = 1; } T_STRING ')'
      { if (defCallbacks->FPCCbk) defData->FPC.addComps($4); }

timingdisables_section: timingdisables_start timingdisables_rules
      timingdisables_end
      ;

timingdisables_start: K_TIMINGDISABLES NUMBER ';'
      { 
        if (defCallbacks->TimingDisablesStartCbk)
          CALLBACK(defCallbacks->TimingDisablesStartCbk, defrTimingDisablesStartCbkType,
                   ROUND($2));
      }

timingdisables_rules: // empty 
      | timingdisables_rules timingdisables_rule
      {}

timingdisables_rule: '-' K_FROMPIN { defData->dumb_mode = 2; defData->no_num = 2;  } T_STRING
      T_STRING K_TOPIN { defData->dumb_mode = 2; defData->no_num = 2;  } T_STRING T_STRING ';'
      {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setFromTo($4, $5, $8, $9);
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      }
      | '-' K_THRUPIN {defData->dumb_mode = 2; defData->no_num = 2; } T_STRING T_STRING ';'
      {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setThru($4, $5);
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                   &defData->TimingDisable);
        }
      }
      | '-' K_MACRO {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING td_macro_option ';'
      {
        if (defCallbacks->TimingDisableCbk) {
          defData->TimingDisable.setMacro($4);
          CALLBACK(defCallbacks->TimingDisableCbk, defrTimingDisableCbkType,
                &defData->TimingDisable);
        }
      }
      | '-' K_REENTRANTPATHS ';'
      { if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setReentrantPathsFlag();
      }


td_macro_option: K_FROMPIN {defData->dumb_mode = 1; defData->no_num = 1;} T_STRING K_TOPIN
      {defData->dumb_mode=1; defData->no_num = 1;} T_STRING
      {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroFromTo($3,$6);
      }
      |        K_THRUPIN {defData->dumb_mode=1; defData->no_num = 1;} T_STRING
      {
        if (defCallbacks->TimingDisableCbk)
          defData->TimingDisable.setMacroThru($3);
      }

timingdisables_end: K_END K_TIMINGDISABLES
      { 
        if (defCallbacks->TimingDisablesEndCbk)
          CALLBACK(defCallbacks->TimingDisablesEndCbk, defrTimingDisablesEndCbkType, 0);
      }


partitions_section: partitions_start partition_rules partitions_end
      ;

partitions_start: K_PARTITIONS NUMBER ';'
      {
        if (defCallbacks->PartitionsStartCbk)
          CALLBACK(defCallbacks->PartitionsStartCbk, defrPartitionsStartCbkType,
                   ROUND($2));
      }

partition_rules: // empty 
      | partition_rules partition_rule
      { }

partition_rule: start_partition partition_members ';' 
      { 
        if (defCallbacks->PartitionCbk)
          CALLBACK(defCallbacks->PartitionCbk, defrPartitionCbkType, &defData->Partition);
      }

start_partition: '-' { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING turnoff
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setName($3);
      }

turnoff: // empty 
      | K_TURNOFF turnoff_setup turnoff_hold
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.addTurnOff($2, $3);
      }

turnoff_setup: // empty 
      { $$ = (char*)" "; }
      | K_SETUPRISE
      { $$ = (char*)"R"; }
      | K_SETUPFALL
      { $$ = (char*)"F"; }

turnoff_hold: // empty 
      { $$ = (char*)" "; }
      | K_HOLDRISE
      { $$ = (char*)"R"; }
      | K_HOLDFALL
      { $$ = (char*)"F"; }

partition_members: // empty 
      | partition_members partition_member
      ;

partition_member: '+' K_FROMCLOCKPIN {defData->dumb_mode=2; defData->no_num = 2;}
      T_STRING T_STRING risefall minmaxpins
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromClockPin($4, $5);
      }
      | '+' K_FROMCOMPPIN {defData->dumb_mode=2; defData->no_num = 2; }
      T_STRING T_STRING risefallminmax2_list
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromCompPin($4, $5);
      }
      | '+' K_FROMIOPIN {defData->dumb_mode=1; defData->no_num = 1; } T_STRING
      risefallminmax1_list
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setFromIOPin($4);
      }
      | '+' K_TOCLOCKPIN {defData->dumb_mode=2; defData->no_num = 2; }
      T_STRING T_STRING risefall minmaxpins
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToClockPin($4, $5);
      }
      | '+' K_TOCOMPPIN {defData->dumb_mode=2; defData->no_num = 2; }
      T_STRING T_STRING risefallminmax2_list
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToCompPin($4, $5);
      }
      | '+' K_TOIOPIN {defData->dumb_mode=1; defData->no_num = 2; } T_STRING risefallminmax1_list
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setToIOPin($4);
      }
      | extension_stmt
      { 
        if (defCallbacks->PartitionsExtCbk)
          CALLBACK(defCallbacks->PartitionsExtCbk, defrPartitionsExtCbkType,
                   &defData->History_text[0]);
      }

minmaxpins: min_or_max_list K_PINS
      { defData->dumb_mode = DEF_MAX_INT; defData->no_num = DEF_MAX_INT; } pin_list
      { defData->dumb_mode = 0; defData->no_num = 0; }

min_or_max_list: // empty 
      | min_or_max_list min_or_max_member
      { }

min_or_max_member: K_MIN NUMBER NUMBER
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMin($2, $3);
      }
      | K_MAX NUMBER NUMBER
      {
        if (defCallbacks->PartitionCbk)
          defData->Partition.setMax($2, $3);
      }

pin_list: // empty 
      | pin_list T_STRING
      { if (defCallbacks->PartitionCbk) defData->Partition.addPin($2); }

risefallminmax1_list: // empty 
      | risefallminmax1_list risefallminmax1

risefallminmax1: K_RISEMIN NUMBER
      { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMin($2); }
      | K_FALLMIN NUMBER
      { if (defCallbacks->PartitionCbk) defData->Partition.addFallMin($2); }
      | K_RISEMAX NUMBER
      { if (defCallbacks->PartitionCbk) defData->Partition.addRiseMax($2); }
      | K_FALLMAX NUMBER
      { if (defCallbacks->PartitionCbk) defData->Partition.addFallMax($2); }

risefallminmax2_list:
      risefallminmax2
      | risefallminmax2_list risefallminmax2
      ;

risefallminmax2: K_RISEMIN NUMBER NUMBER
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMinRange($2, $3); }
      | K_FALLMIN NUMBER NUMBER
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMinRange($2, $3); }
      | K_RISEMAX NUMBER NUMBER
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addRiseMaxRange($2, $3); }
      | K_FALLMAX NUMBER NUMBER
      { if (defCallbacks->PartitionCbk)
          defData->Partition.addFallMaxRange($2, $3); }

partitions_end: K_END K_PARTITIONS
      { if (defCallbacks->PartitionsEndCbk)
          CALLBACK(defCallbacks->PartitionsEndCbk, defrPartitionsEndCbkType, 0); }

comp_names: // empty 
      | comp_names comp_name
      { }

comp_name: '(' {defData->dumb_mode=2; defData->no_num = 2; } T_STRING
      T_STRING subnet_opt_syn ')'
      {
        // note that the defData->first T_STRING could be the keyword VPIN 
        if (defCallbacks->NetCbk)
          defData->Subnet->addPin($3, $4, $5);
      }

subnet_opt_syn: // empty 
      { $$ = 0; }
      | '+' K_SYNTHESIZED
      { $$ = 1; }

subnet_options: // empty 
      | subnet_options subnet_option

subnet_option: subnet_type
      {  
        if (defCallbacks->NetCbk) defData->Subnet->addWire($1);
      }
      paths
      {  
        defData->by_is_keyword = FALSE;
        defData->do_is_keyword = FALSE;
        defData->new_is_keyword = FALSE;
        defData->step_is_keyword = FALSE;
        defData->orient_is_keyword = FALSE;
        defData->needNPCbk = 0;
      }
      | K_NONDEFAULTRULE { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      { if (defCallbacks->NetCbk) defData->Subnet->setNonDefault($3); }

subnet_type: K_FIXED
      { $$ = (char*)"FIXED"; defData->dumb_mode = 1; }
      | K_COVER
      { $$ = (char*)"COVER"; defData->dumb_mode = 1; }
      | K_ROUTED
      { $$ = (char*)"ROUTED"; defData->dumb_mode = 1; }
      | K_NOSHIELD
      { $$ = (char*)"NOSHIELD"; defData->dumb_mode = 1; }

pin_props_section: begin_pin_props pin_prop_list end_pin_props ;

begin_pin_props: K_PINPROPERTIES NUMBER opt_semi
      { if (defCallbacks->PinPropStartCbk)
          CALLBACK(defCallbacks->PinPropStartCbk, defrPinPropStartCbkType, ROUND($2)); }

opt_semi:
      // empty 
      { }
      | ';'
      { }

end_pin_props: K_END K_PINPROPERTIES
      { if (defCallbacks->PinPropEndCbk)
          CALLBACK(defCallbacks->PinPropEndCbk, defrPinPropEndCbkType, 0); }

pin_prop_list: // empty 
      | pin_prop_list pin_prop_terminal
      ;

pin_prop_terminal: '-' { defData->dumb_mode = 2; defData->no_num = 2; } T_STRING T_STRING
      { if (defCallbacks->PinPropCbk) defData->PinProp.setName($3, $4); }
      pin_prop_options ';'
      { if (defCallbacks->PinPropCbk) {
          CALLBACK(defCallbacks->PinPropCbk, defrPinPropCbkType, &defData->PinProp);
         // reset the property number
         defData->PinProp.clear();
        }
      }

pin_prop_options : // empty 
      | pin_prop_options pin_prop ;

pin_prop: '+' K_PROPERTY { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
      pin_prop_name_value_list 
      { defData->dumb_mode = 0; defData->parsing_property = 0; }

pin_prop_name_value_list : // empty 
      | pin_prop_name_value_list pin_prop_name_value
      ;

pin_prop_name_value : T_STRING NUMBER
      {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->CompPinProp.propType($1);
          CHKPROPTYPE(propTp, $1, "PINPROPERTIES");
          sprintf(str, "%g", $2);
          defData->PinProp.addNumProperty($1, $2, str, propTp);
        }
      }
 | T_STRING QSTRING
      {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defSettings->CompPinProp.propType($1);
          CHKPROPTYPE(propTp, $1, "PINPROPERTIES");
          defData->PinProp.addProperty($1, $2, propTp);
        }
      }
 | T_STRING T_STRING
      {
        if (defCallbacks->PinPropCbk) {
          char propTp;
          propTp = defSettings->CompPinProp.propType($1);
          CHKPROPTYPE(propTp, $1, "PINPROPERTIES");
          defData->PinProp.addProperty($1, $2, propTp);
        }
      }

blockage_section: blockage_start blockage_defs blockage_end ;

blockage_start: K_BLOCKAGES NUMBER ';'
      { if (defCallbacks->BlockageStartCbk)
          CALLBACK(defCallbacks->BlockageStartCbk, defrBlockageStartCbkType, ROUND($2)); }

blockage_end: K_END K_BLOCKAGES
      { if (defCallbacks->BlockageEndCbk)
          CALLBACK(defCallbacks->BlockageEndCbk, defrBlockageEndCbkType, 0); }

blockage_defs: // empty 
      | blockage_defs blockage_def
      ;

blockage_def: blockage_rule rectPoly_blockage rectPoly_blockage_rules
      ';'
      {
        if (defCallbacks->BlockageCbk) {
          CALLBACK(defCallbacks->BlockageCbk, defrBlockageCbkType, &defData->Blockage);
          defData->Blockage.clear();
        }
      }

blockage_rule: '-' K_LAYER { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING 
      {
        if (defCallbacks->BlockageCbk) {
          if (defData->Blockage.hasPlacement() != 0) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defData->Blockage.setLayer($4);
          defData->Blockage.clearPoly(); // free poly, if any
        }
        defData->hasBlkLayerComp = 0;
        defData->hasBlkLayerSpac = 0;
        defData->hasBlkLayerTypeComp = 0;
      }

      layer_blockage_rules
      // 10/29/2001 - enhancement 
      | '-' K_PLACEMENT
      {
        if (defCallbacks->BlockageCbk) {
          if (defData->Blockage.hasLayer() != 0) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6539, "Invalid BLOCKAGE statement defined in the DEF file. The BLOCKAGE statment has both the LAYER and the PLACEMENT statements defined.\nUpdate your DEF file to have either BLOCKAGE or PLACEMENT statement only.");
              CHKERR();
            }
          }
          defData->Blockage.setPlacement();
          defData->Blockage.clearPoly(); // free poly, if any
        }
        defData->hasBlkPlaceComp = 0;
        defData->hasBlkPlaceTypeComp = 0;
      }
      placement_comp_rules
      
layer_blockage_rules: // empty 
      | layer_blockage_rules layer_blockage_rule
      ;

layer_blockage_rule: '+' K_SPACING NUMBER
      {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The SPACING statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6540, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defData->hasBlkLayerSpac) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk)
            defData->Blockage.setSpacing((int)$3);
          defData->hasBlkLayerSpac = 1;
        }
      }
      | '+' K_DESIGNRULEWIDTH NUMBER
      {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else if (defData->hasBlkLayerSpac) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6541, "The SPACING statement is defined in the LAYER statement,\nbut there is already either a SPACING statement or DESIGNRULEWIDTH  statement has defined in the LAYER statement.\nUpdate your DEF file to have either SPACING statement or a DESIGNRULEWIDTH statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk)
            defData->Blockage.setDesignRuleWidth((int)$3);
          defData->hasBlkLayerSpac = 1;
        }
      }
      | mask_blockage_rule
      | comp_blockage_rule
      
mask_blockage_rule: 
      '+' K_MASK NUMBER
      {      
        if (validateMaskInput((int)$3, defData->blockageWarnings, defSettings->BlockageWarnings)) {
          defData->Blockage.setMask((int)$3);
        }
      } 

comp_blockage_rule:
      // 06/20/2001 - pcr 383204 = pcr 378102 
      '+' K_COMPONENT { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defData->hasBlkLayerComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk) {
            defData->Blockage.setComponent($4);
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
        }
      }
      // 8/30/2001 - pcr 394394 
      | '+' K_SLOTS
      {
        if (defData->hasBlkLayerComp || defData->hasBlkLayerTypeComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk) {
            defData->Blockage.setSlots();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
          if (defData->VersionNum == 5.8) {
            defData->hasBlkLayerTypeComp = 1;
          }
        }
      }
      | '+' K_FILLS
      {
        if (defData->hasBlkLayerComp || defData->hasBlkLayerTypeComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setFills();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
          if (defData->VersionNum == 5.8) {
            defData->hasBlkLayerTypeComp = 1;
          }
        }
      }
      | '+' K_PUSHDOWN
      {
        if (defData->hasBlkLayerComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setPushdown();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkLayerComp = 1;
          }
        }
      }
      | '+' K_EXCEPTPGNET              // 5.7 
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The EXCEPTPGNET is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6549, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
              }
           }
        } else {
           if (defData->hasBlkLayerComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6542, "The defined BLOCKAGES COMPONENT statement has either COMPONENT, SLOTS, FILLS, PUSHDOWN or EXCEPTPGNET defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES COMPONENT statement per layer.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setExceptpgnet();
             }
             if (defData->VersionNum < 5.8){
               defData->hasBlkLayerComp = 1;
             }
           }
        }
      }

placement_comp_rules: // empty 
      | placement_comp_rules placement_comp_rule
      ;
      
placement_comp_rule: // empty 
      // 10/29/2001 - enhancement 
      '+' K_COMPONENT { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defData->hasBlkPlaceComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setComponent($4);
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkPlaceComp = 1;
          }
        }
      }
      | '+' K_PUSHDOWN
      {
        if (defData->hasBlkPlaceComp) {
          if (defCallbacks->BlockageCbk) {
            if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
              defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
              CHKERR();
            }
          }
        } else {
          if (defCallbacks->BlockageCbk){
            defData->Blockage.setPushdown();
          }
          if (defData->VersionNum < 5.8) {
            defData->hasBlkPlaceComp = 1;
          }
        }
      }
      | '+' K_SOFT                   // 5.7
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The PLACEMENT SOFT is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6547, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defData->hasBlkPlaceComp || defData->hasBlkPlaceTypeComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setSoft();
             }
             if (defData->VersionNum < 5.8) {
               defData->hasBlkPlaceComp = 1;
             }
             if (defData->VersionNum == 5.8) {
               defData->hasBlkPlaceTypeComp = 1;
             }
           }
        }
      }
      | '+' K_PARTIAL NUMBER         // 5.7
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->BlockageCbk) {
             if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                defData->defMsg = (char*)defMalloc(10000);
                sprintf (defData->defMsg,
                  "The PARTIAL is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
                defError(6548, defData->defMsg);
                defFree(defData->defMsg);
                CHKERR();
             }
           }
        } else {
           if (defData->hasBlkPlaceComp || defData->hasBlkPlaceTypeComp) {
             if (defCallbacks->BlockageCbk) {
               if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                 defError(6543, "The defined BLOCKAGES PLACEMENT statement has either COMPONENT, PUSHDOWN, SOFT or PARTIAL defined.\nOnly one of these statements is allowed per LAYER. Updated the DEF file to define a valid BLOCKAGES PLACEMENT statement.");
                 CHKERR();
               }
             }
           } else {
             if (defCallbacks->BlockageCbk){
               defData->Blockage.setPartial($3);
             } 
             if (defData->VersionNum < 5.8) {
               defData->hasBlkPlaceComp = 1;
             }
             if (defData->VersionNum == 5.8) {
               defData->hasBlkPlaceTypeComp = 1;
             }
           }
         }
      }
           

rectPoly_blockage_rules: // empty 
      | rectPoly_blockage_rules rectPoly_blockage
      ;
  
rectPoly_blockage: K_RECT pt pt
      {
        if (defCallbacks->BlockageCbk)
          defData->Blockage.addRect($2.x, $2.y, $3.x, $3.y);
      }
      | K_POLYGON
      {
        if (defCallbacks->BlockageCbk) {
            defData->Geometries.Reset();
        }
      }
      firstPt nextPt nextPt otherPts
      {
        if (defCallbacks->BlockageCbk) {
          if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
            if (defData->Blockage.hasLayer()) {  // only in layer
              if (defCallbacks->BlockageCbk)
                defData->Blockage.addPolygon(&defData->Geometries);
            } else {
              if (defCallbacks->BlockageCbk) {
                if (defData->blockageWarnings++ < defSettings->BlockageWarnings) {
                  defError(6544, "A POLYGON statement is defined in the BLOCKAGE statement,\nbut it is not defined in the BLOCKAGE LAYER statement.\nUpdate your DEF file to either remove the POLYGON statement from the BLOCKAGE statement or\ndefine the POLYGON statement in a BLOCKAGE LAYER statement.");
                  CHKERR();
                }
              }
            }
          }
        }
      }

// 8/31/2001 - 5.4 enhancement 
slot_section: slot_start slot_defs slot_end ;

slot_start: K_SLOTS NUMBER ';'
      { if (defCallbacks->SlotStartCbk)
          CALLBACK(defCallbacks->SlotStartCbk, defrSlotStartCbkType, ROUND($2)); }

slot_end: K_END K_SLOTS
      { if (defCallbacks->SlotEndCbk)
          CALLBACK(defCallbacks->SlotEndCbk, defrSlotEndCbkType, 0); }

slot_defs: // empty 
      | slot_defs slot_def
      ;

slot_def: slot_rule geom_slot_rules ';'
      {
        if (defCallbacks->SlotCbk) {
          CALLBACK(defCallbacks->SlotCbk, defrSlotCbkType, &defData->Slot);
          defData->Slot.clear();
        }
      }

slot_rule: '-' K_LAYER { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING 
      {
        if (defCallbacks->SlotCbk) {
          defData->Slot.setLayer($4);
          defData->Slot.clearPoly();     // free poly, if any
        }
      } geom_slot

geom_slot_rules: // empty 
      | geom_slot_rules geom_slot
      ;

geom_slot: K_RECT pt pt
      {
        if (defCallbacks->SlotCbk)
          defData->Slot.addRect($2.x, $2.y, $3.x, $3.y);
      }
      | K_POLYGON
      {
          defData->Geometries.Reset();
      }
      firstPt nextPt nextPt otherPts
      {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
          if (defCallbacks->SlotCbk)
            defData->Slot.addPolygon(&defData->Geometries);
        }
      }

// 8/31/2001 -  5.4 enhancement 
fill_section: fill_start fill_defs fill_end ;

fill_start: K_FILLS NUMBER ';'
      { if (defCallbacks->FillStartCbk)
          CALLBACK(defCallbacks->FillStartCbk, defrFillStartCbkType, ROUND($2)); }

fill_end: K_END K_FILLS
      { if (defCallbacks->FillEndCbk)
          CALLBACK(defCallbacks->FillEndCbk, defrFillEndCbkType, 0); }

fill_defs: // empty 
      | fill_defs fill_def
      ;

fill_def: fill_rule geom_fill_rules ';'
      {
        if (defCallbacks->FillCbk) {
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
      }
      | '-' K_VIA { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING  // 5.7
      {
        if (defCallbacks->FillCbk) {
          defData->Fill.setVia($4);
          defData->Fill.clearPts();
          defData->Geometries.Reset();
        }
      }
      fill_via_mask_opc_opt fill_via_pt ';'

fill_rule: '-' K_LAYER { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defCallbacks->FillCbk) {
          defData->Fill.setLayer($4);
          defData->Fill.clearPoly();    // free poly, if any
        }
      } 
      fill_layer_mask_opc_opt geom_fill            // 5.7

geom_fill_rules: // empty 
      | geom_fill_rules geom_fill
      ;

geom_fill: K_RECT pt pt
      {
        if (defCallbacks->FillCbk)
          defData->Fill.addRect($2.x, $2.y, $3.x, $3.y);
      }
      | K_POLYGON
      {
        defData->Geometries.Reset();
      }
      firstPt nextPt nextPt otherPts
      {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond
          if (defCallbacks->FillCbk)
            defData->Fill.addPolygon(&defData->Geometries);
        } else {
            defData->defMsg = (char*)defMalloc(10000);
            sprintf (defData->defMsg,
              "POLYGON statement in FILLS LAYER is a version 5.6 and later syntax.\nYour def file is defined with version %g.", defData->VersionNum);
            defError(6564, defData->defMsg);
            defFree(defData->defMsg);
            CHKERR();
        }
      }

fill_layer_mask_opc_opt: // empty 
    | fill_layer_mask_opc_opt opt_mask_opc_l
    ;
opt_mask_opc_l: fill_layer_opc
    | fill_mask
    ;
        
// 5.7
fill_layer_opc: 
      '+' K_OPC
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->FillCbk) {
             if (defData->fillWarnings++ < defSettings->FillWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The LAYER OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6553, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defCallbacks->FillCbk)
             defData->Fill.setLayerOpc();
        }
      }

fill_via_pt: firstPt otherPts
    {
        if (defCallbacks->FillCbk) {
          defData->Fill.addPts(&defData->Geometries);
          CALLBACK(defCallbacks->FillCbk, defrFillCbkType, &defData->Fill);
          defData->Fill.clear();
        }
    }
      

fill_via_mask_opc_opt: // empty 
    | fill_via_mask_opc_opt opt_mask_opc
    ;
opt_mask_opc: fill_via_opc
    | fill_viaMask
    ;
    
// 5.7
fill_via_opc:
      '+' K_OPC
      {
        if (defData->VersionNum < 5.7) {
           if (defCallbacks->FillCbk) {
             if (defData->fillWarnings++ < defSettings->FillWarnings) {
               defData->defMsg = (char*)defMalloc(10000);
               sprintf (defData->defMsg,
                 "The VIA OPC is available in version 5.7 or later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
               defError(6554, defData->defMsg);
               defFree(defData->defMsg);
               CHKERR();
             }
           }
        } else {
           if (defCallbacks->FillCbk)
             defData->Fill.setViaOpc();
        }
      }
      
fill_mask:
      '+' K_MASK NUMBER
      { 
        if (validateMaskInput((int)$3, defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)$3);
             }
        }
      }

fill_viaMask:
      '+' K_MASK NUMBER
      { 
        if (validateMaskInput((int)$3, defData->fillWarnings, defSettings->FillWarnings)) {
             if (defCallbacks->FillCbk) {
                defData->Fill.setMask((int)$3);
             }
        }
      }
      
// 11/17/2003 - 5.6 enhancement 
nondefaultrule_section: nondefault_start nondefault_def nondefault_defs
    nondefault_end ;

nondefault_start: K_NONDEFAULTRULES NUMBER ';'
      { 
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->NonDefaultStartCbk) {
            if (defData->nonDefaultWarnings++ < defSettings->NonDefaultWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The NONDEFAULTRULE statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g.", defData->VersionNum);
              defError(6545, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defCallbacks->NonDefaultStartCbk)
          CALLBACK(defCallbacks->NonDefaultStartCbk, defrNonDefaultStartCbkType,
                   ROUND($2));
      }

nondefault_end: K_END K_NONDEFAULTRULES
      { if (defCallbacks->NonDefaultEndCbk)
          CALLBACK(defCallbacks->NonDefaultEndCbk, defrNonDefaultEndCbkType, 0); }

nondefault_defs: // empty 
      | nondefault_defs nondefault_def
      ;

nondefault_def: '-' { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.clear(); 
          defData->NonDefault.setName($3);
        }
      }
      nondefault_options ';'
      { if (defCallbacks->NonDefaultCbk)
          CALLBACK(defCallbacks->NonDefaultCbk, defrNonDefaultCbkType, &defData->NonDefault); }

nondefault_options: // empty  
      | nondefault_options nondefault_option
      ;

nondefault_option: '+' K_HARDSPACING
      {
        if (defCallbacks->NonDefaultCbk)
          defData->NonDefault.setHardspacing();
      }
      | '+' K_LAYER { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
        K_WIDTH NUMBER
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addLayer($4);
          defData->NonDefault.addWidth($6);
        }
      }
      nondefault_layer_options
      | '+' K_VIA { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addVia($4);
        }
      }
      | '+' K_VIARULE { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addViaRule($4);
        }
      }
      | '+' K_MINCUTS { defData->dumb_mode = 1; defData->no_num = 1; } T_STRING NUMBER
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addMinCuts($4, (int)$5);
        }
      }
      | nondefault_prop_opt
      ;

nondefault_layer_options: // empty 
      | nondefault_layer_options nondefault_layer_option

nondefault_layer_option:
      K_DIAGWIDTH NUMBER
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addDiagWidth($2);
        }
      }
      | K_SPACING NUMBER
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addSpacing($2);
        }
      }
      | K_WIREEXT NUMBER
      {
        if (defCallbacks->NonDefaultCbk) {
          defData->NonDefault.addWireExt($2);
        }
      }
      ;

nondefault_prop_opt: '+' K_PROPERTY { defData->dumb_mode = DEF_MAX_INT; defData->parsing_property = 1; }
                     nondefault_prop_list
      { defData->dumb_mode = 0; defData->parsing_property = 0; }

nondefault_prop_list: // empty 
      | nondefault_prop_list nondefault_prop
      ;

nondefault_prop: T_STRING NUMBER
      {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          char* str = ringCopy("                       ");
          propTp = defSettings->NDefProp.propType($1);
          CHKPROPTYPE(propTp, $1, "NONDEFAULTRULE");
          sprintf(str, "%g", $2);
          defData->NonDefault.addNumProperty($1, $2, str, propTp);
        }
      }
      | T_STRING QSTRING
      {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defSettings->NDefProp.propType($1);
          CHKPROPTYPE(propTp, $1, "NONDEFAULTRULE");
          defData->NonDefault.addProperty($1, $2, propTp);
        }
      }
      | T_STRING T_STRING
      {
        if (defCallbacks->NonDefaultCbk) {
          char propTp;
          propTp = defSettings->NDefProp.propType($1);
          CHKPROPTYPE(propTp, $1, "NONDEFAULTRULE");
          defData->NonDefault.addProperty($1, $2, propTp);
        }
      }

// 12/2/2003 - 5.6 enhancement 
styles_section: styles_start styles_rules styles_end ;

styles_start: K_STYLES NUMBER ';'
      {
        if (defData->VersionNum < 5.6) {
          if (defCallbacks->StylesStartCbk) {
            if (defData->stylesWarnings++ < defSettings->StylesWarnings) {
              defData->defMsg = (char*)defMalloc(1000);
              sprintf (defData->defMsg,
                 "The STYLES statement is available in version 5.6 and later.\nHowever, your DEF file is defined with version %g", defData->VersionNum);
              defError(6546, defData->defMsg);
              defFree(defData->defMsg);
              CHKERR();
            }
          }
        } else if (defCallbacks->StylesStartCbk)
          CALLBACK(defCallbacks->StylesStartCbk, defrStylesStartCbkType, ROUND($2));
      }

styles_end: K_END K_STYLES
      { if (defCallbacks->StylesEndCbk)
          CALLBACK(defCallbacks->StylesEndCbk, defrStylesEndCbkType, 0); }

styles_rules: // empty 
      | styles_rules styles_rule
      ;

styles_rule: '-' K_STYLE NUMBER
      {
        if (defCallbacks->StylesCbk) defData->Styles.setStyle((int)$3);
        defData->Geometries.Reset();
      }
      firstPt nextPt otherPts ';'
      {
        if (defData->VersionNum >= 5.6) {  // only 5.6 and beyond will call the callback
          if (defCallbacks->StylesCbk) {
            defData->Styles.setPolygon(&defData->Geometries);
            CALLBACK(defCallbacks->StylesCbk, defrStylesCbkType, &defData->Styles);
          }
        }
      }
      

%%

END_LEFDEF_PARSER_NAMESPACE

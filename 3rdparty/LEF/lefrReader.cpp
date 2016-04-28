// *****************************************************************************
// *****************************************************************************
// Copyright 2012 - 2014, Cadence Design Systems
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
//  $Revision: #4 $
//  $Date: 2014/06/05 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************
#include "lefrReader.hpp"
#include "lex.h"
#include <stdlib.h>
#include <string.h>

#include "lefiDebug.hpp"
#include "lefrData.hpp"
#include "lefrSettings.hpp"
#include "lefrCallBacks.hpp"

#define NOCBK 100
#define NOLEFMSG 4701 // 4701 = 4700 + 1, message starts on 1

//#if _DEBUG
#define ASSERT_INIT assert_lef_init(__FILE__, __LINE__)
//#else
//# define ASSERT_INIT 1
//#endif

BEGIN_LEFDEF_PARSER_NAMESPACE

extern double convert_name2num(const char *versionName);
extern bool validateMaskNumber(int num);

void
assert_lef_init(const char  *file,
                int         line)
{
    if (lefSettings && lefCallbacks) {
        return;
    }

    fprintf(stderr, "ERROR: Attempt to configure LEF parser before lefrInit() call: %s %d\n", file, line);
    exit(100);
}

void
lefiNerr(int i)
{
    sprintf(lefData->lefrErrMsg, "ERROR number %d\n", i);
    lefiError(1, 0, lefData->lefrErrMsg);
    exit(2);
}

void
lefiNwarn(int i)
{
    sprintf(lefData->lefrErrMsg, "WARNING number %d\n", i);
    lefiError(1, 0, lefData->lefrErrMsg);
    exit(2);
}

double
convert_name2num(const char *versionName)
{
    char    majorNm[80];
    char    minorNm[80];
    char    *subMinorNm = NULL;

    char    *versionNm = strdup(versionName);

    double  major = 0, minor = 0, subMinor = 0;
    double  version, versionNumber;
    char    finalVersion[80];

    sscanf(versionNm, "%[^.].%s", majorNm, minorNm);
    char    *p1 = strchr(minorNm, '.');
    if (p1) {
        subMinorNm = p1 + 1;
        *p1 = '\0';
    }
    major = atof(majorNm);
    minor = atof(minorNm);
    if (subMinorNm)
        subMinor = atof(subMinorNm);

    version = major;

    if (minor > 0)
        version = major + minor / 10;

    if (subMinor > 0)
        version = version + subMinor / 1000;

    lefFree(versionNm);

    sprintf(finalVersion, "%.4f", version);

    versionNumber = atof(finalVersion);

    return versionNumber;
}

bool
validateMaskNumber(int num)
{
    int digit = 0;
    int index = 0;

    if (num < 0) {
        return false;
    }

    while (num > 0) {
        digit = num % 10;

        if (digit > 3) {
            return false;
        }

        index++;
        num = num / 10;
    }

    if (index > 3) {
        return false;
    }

    return true;
}

// *****************************************************************************s
// Global variables
// *****************************************************************************

// 5.6 END LIBRARY is optional.
// Function to initialize global variables.
// This make sure the global variables are initialized

// User control warning to be printed by the parser
void
lefrDisableParserMsgs(int   nMsg,
                      int   *msgs)
{
    int i, j;
    int *tmp[2];

    if (nMsg <= 0)
        return;

    if (lefData->nDMsgs == 0) {  // new list 
        lefData->nDMsgs = nMsg;
        // 0 - holds the message number, 1 keep track if warning has printed 
        lefData->disableMsgs[0] = (int*) lefMalloc(sizeof(int) * nMsg);
        lefData->disableMsgs[1] = (int*) lefMalloc(sizeof(int) * nMsg);
        for (i = 0; i < nMsg; i++) {
            lefData->disableMsgs[0][i] = msgs[i];
            lefData->disableMsgs[1][i] = 0;
        }
    } else {  // add the list to the existing list 
        // 1st check if the msgId is already on the list before adding it on 
        tmp[0] = (int*) lefMalloc(sizeof(int) * (nMsg + lefData->nDMsgs));
        tmp[1] = (int*) lefMalloc(sizeof(int) * (nMsg + lefData->nDMsgs));
        for (i = 0; i < lefData->nDMsgs; i++) {  // copy the existing to the new list 
            tmp[0][i] = lefData->disableMsgs[0][i];
            tmp[1][i] = lefData->disableMsgs[1][i];
        }
        lefFree((int*) (lefData->disableMsgs[0]));
        lefFree((int*) (lefData->disableMsgs[1]));
        lefData->disableMsgs[0] = tmp[0];           // set lefData->disableMsgs to the new list 
        lefData->disableMsgs[1] = tmp[1];
        for (i = 0; i < nMsg; i++) { // merge the new list with the existing 
            for (j = 0; j < lefData->nDMsgs; j++) {
                if (lefData->disableMsgs[0][j] == msgs[i])
                    break;             // msgId already on the list 
            }
            if (j == lefData->nDMsgs) {         // msgId not on the list, add it on 
                lefData->disableMsgs[0][lefData->nDMsgs] = msgs[i];
                lefData->disableMsgs[1][lefData->nDMsgs] = 0;
                lefData->nDMsgs++;
            }
        }
    }
}

void
lefrEnableParserMsgs(int    nMsg,
                     int    *msgs)
{
    int i, j;

    if (lefData->nDMsgs == 0)
        return;                       // list is empty, nothing to remove 

    for (i = 0; i < nMsg; i++) {     // loop through the given list 
        for (j = 0; j < lefData->nDMsgs; j++) {
            if (lefData->disableMsgs[0][j] == msgs[i]) {
                lefData->disableMsgs[0][j] = -1;    // temp assign a -1 on that slot 
                break;
            }
        }
    }
    // fill up the empty slot with the lefData->next non -1 msgId 
    for (i = 0; i < lefData->nDMsgs; i++) {
        if (lefData->disableMsgs[0][i] == -1) {
            j = i + 1;
            while (j < lefData->nDMsgs) {
                if (lefData->disableMsgs[0][j] != -1) {
                    lefData->disableMsgs[0][i] = lefData->disableMsgs[0][j];
                    lefData->disableMsgs[1][i] = lefData->disableMsgs[1][j];
                    i++;
                    j++;
                }
            }
            break;     // break out the for loop, the list should all moved 
        }
    }
    // Count how many messageId left and change all -1 to 0 
    for (j = i; j < lefData->nDMsgs; j++) {
        lefData->disableMsgs[0][j] = 0;     // set to 0 
        lefData->disableMsgs[1][j] = 0;
    }
    lefData->nDMsgs = i;
}

void
lefrEnableAllMsgs()
{
    lefData->nDMsgs = 0;
    lefFree((int*) (lefData->disableMsgs[0]));
    lefFree((int*) (lefData->disableMsgs[1]));
    lefData->dAllMsgs = 0;
}

void
lefrSetTotalMsgLimit(int totNumMsgs)
{
    ASSERT_INIT;
    lefSettings->TotalMsgLimit = totNumMsgs;
}

void
lefrSetLimitPerMsg(int  msgId,
                   int  numMsg)
{
    ASSERT_INIT;
    char msgStr[10];
    if ((msgId <= 0) || (msgId >= NOLEFMSG)) {
        sprintf(msgStr, "%d", msgId);
        lefError(204, msgStr);
        return;
    }
    lefSettings->MsgLimit[msgId] = numMsg;
}

// *****************************************************************************
// Since the lef parser only keep one list of disable message ids, and does
// not have a list of enable message ids, if the API lefrDisableAllMsgs is
// called to disable all message ids, user has to call API lefrEnableAllMsgs
// to enable all message ids lefData->first, before calling lefrDisableParserMsgs &
// lefrEnableParserMsgs.
// Users cannot call lefrDisableAllMsgs and call lefrEnableParserMsgs to
// enable a small list of message ids since lefrDisableAllMsgs does not have
// a list of all message ids, hence there isn't a list for lefrEnableParserMsgs
// to work on to enable the message ids.
// *****************************************************************************
void
lefrDisableAllMsgs()
{
    lefData->nDMsgs = 0;
    lefFree((int*) (lefData->disableMsgs[0]));
    lefFree((int*) (lefData->disableMsgs[1]));
    lefData->dAllMsgs = 1;
}

// Parser control by the user.
// Reader initialization
int
lefrInit()
{
    lefrCallbacks::reset();
    lefrSettings::reset();

    return 0;
}

int
lefrReset()
{
    // obsoleted.
    return 0;
}


int 
lefrClear()
{
    delete lefData;
    lefData = NULL;

    delete lefCallbacks;
    lefCallbacks = NULL;

    delete lefSettings;
    lefSettings = NULL;

    return 0;
}


const char *
lefrFName()
{
    return lefData->lefrFileName;
}

int
lefrReleaseNResetMemory()
{
    return 0;
}

int
lefrRead(FILE           *f,
         const char     *fName,
         lefiUserData   uData)
{
    ASSERT_INIT;
    int status;

    lefrData::reset();

    lefData->versionNum = (lefSettings->VersionNum == 0.0) ?
        CURRENT_VERSION :
        lefData->versionNum = lefSettings->VersionNum;

    if (lefSettings->CaseSensitiveSet) {
        lefData->namesCaseSensitive = lefSettings->CaseSensitive;
    }

    lefData->lefrFileName = (char*) fName;
    lefData->lefrFile = f;
    lefSettings->UserData = uData;

    status = lefyyparse();

    return status;
}

void
lefrSetUnusedCallbacks(lefrVoidCbkFnType func)
{
    // Set all of the callbacks that have not been set yet to
    // the given function.
    ASSERT_INIT;

    if (lefCallbacks->ArrayBeginCbk == 0)
        lefCallbacks->ArrayBeginCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->ArrayCbk == 0)
        lefCallbacks->ArrayCbk = (lefrArrayCbkFnType) func;
    if (lefCallbacks->ArrayEndCbk == 0)
        lefCallbacks->ArrayEndCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->DividerCharCbk == 0)
        lefCallbacks->DividerCharCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->BusBitCharsCbk == 0)
        lefCallbacks->BusBitCharsCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->CaseSensitiveCbk == 0)
        lefCallbacks->CaseSensitiveCbk = (lefrIntegerCbkFnType) func;
    if (lefCallbacks->NoWireExtensionCbk == 0)
        lefCallbacks->NoWireExtensionCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->CorrectionTableCbk == 0)
        lefCallbacks->CorrectionTableCbk = (lefrCorrectionTableCbkFnType) func;
    if (lefCallbacks->DielectricCbk == 0)
        lefCallbacks->DielectricCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->EdgeRateScaleFactorCbk == 0)
        lefCallbacks->EdgeRateScaleFactorCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->EdgeRateThreshold1Cbk == 0)
        lefCallbacks->EdgeRateThreshold1Cbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->EdgeRateThreshold2Cbk == 0)
        lefCallbacks->EdgeRateThreshold2Cbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->IRDropBeginCbk == 0)
        lefCallbacks->IRDropBeginCbk = (lefrVoidCbkFnType) func;
    if (lefCallbacks->IRDropCbk == 0)
        lefCallbacks->IRDropCbk = (lefrIRDropCbkFnType) func;
    if (lefCallbacks->IRDropEndCbk == 0)
        lefCallbacks->IRDropEndCbk = (lefrVoidCbkFnType) func;
    if (lefCallbacks->LayerCbk == 0)
        lefCallbacks->LayerCbk = (lefrLayerCbkFnType) func;
    if (lefCallbacks->LibraryEndCbk == 0)
        lefCallbacks->LibraryEndCbk = (lefrVoidCbkFnType) func;
    if (lefCallbacks->MacroBeginCbk == 0)
        lefCallbacks->MacroBeginCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->MacroCbk == 0)
        lefCallbacks->MacroCbk = (lefrMacroCbkFnType) func;
    if (lefCallbacks->MacroClassTypeCbk == 0)
        lefCallbacks->MacroClassTypeCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->MacroOriginCbk == 0)
        lefCallbacks->MacroOriginCbk = (lefrMacroNumCbkFnType) func;
    if (lefCallbacks->MacroSizeCbk == 0)
        lefCallbacks->MacroSizeCbk = (lefrMacroNumCbkFnType) func;
    if (lefCallbacks->MacroFixedMaskCbk == 0)
        lefCallbacks->MacroFixedMaskCbk = (lefrIntegerCbkFnType) func;
    if (lefCallbacks->TimingCbk == 0)
        lefCallbacks->TimingCbk = (lefrTimingCbkFnType) func;
    if (lefCallbacks->MinFeatureCbk == 0)
        lefCallbacks->MinFeatureCbk = (lefrMinFeatureCbkFnType) func;
    if (lefCallbacks->NoiseMarginCbk == 0)
        lefCallbacks->NoiseMarginCbk = (lefrNoiseMarginCbkFnType) func;
    if (lefCallbacks->NoiseTableCbk == 0)
        lefCallbacks->NoiseTableCbk = (lefrNoiseTableCbkFnType) func;
    if (lefCallbacks->NonDefaultCbk == 0)
        lefCallbacks->NonDefaultCbk = (lefrNonDefaultCbkFnType) func;
    if (lefCallbacks->ObstructionCbk == 0)
        lefCallbacks->ObstructionCbk = (lefrObstructionCbkFnType) func;
    if (lefCallbacks->PinCbk == 0)
        lefCallbacks->PinCbk = (lefrPinCbkFnType) func;
    if (lefCallbacks->PropBeginCbk == 0)
        lefCallbacks->PropBeginCbk = (lefrVoidCbkFnType) func;
    if (lefCallbacks->PropCbk == 0)
        lefCallbacks->PropCbk = (lefrPropCbkFnType) func;
    if (lefCallbacks->PropEndCbk == 0)
        lefCallbacks->PropEndCbk = (lefrVoidCbkFnType) func;
    if (lefCallbacks->SiteCbk == 0)
        lefCallbacks->SiteCbk = (lefrSiteCbkFnType) func;
    if (lefCallbacks->SpacingBeginCbk == 0)
        lefCallbacks->SpacingBeginCbk = (lefrVoidCbkFnType) func;
    if (lefCallbacks->SpacingCbk == 0)
        lefCallbacks->SpacingCbk = (lefrSpacingCbkFnType) func;
    if (lefCallbacks->SpacingEndCbk == 0)
        lefCallbacks->SpacingEndCbk = (lefrVoidCbkFnType) func;
    if (lefCallbacks->UnitsCbk == 0)
        lefCallbacks->UnitsCbk = (lefrUnitsCbkFnType) func;
    if ((lefCallbacks->VersionCbk == 0) && (lefCallbacks->VersionStrCbk == 0)) {
        // both version callbacks weren't set, if either one is set, it is ok
        lefCallbacks->VersionCbk = (lefrDoubleCbkFnType) func;
        lefCallbacks->VersionStrCbk = (lefrStringCbkFnType) func;
    }
    if (lefCallbacks->ViaCbk == 0)
        lefCallbacks->ViaCbk = (lefrViaCbkFnType) func;
    if (lefCallbacks->ViaRuleCbk == 0)
        lefCallbacks->ViaRuleCbk = (lefrViaRuleCbkFnType) func;
    if (lefCallbacks->InputAntennaCbk == 0)
        lefCallbacks->InputAntennaCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->OutputAntennaCbk == 0)
        lefCallbacks->OutputAntennaCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->InoutAntennaCbk == 0)
        lefCallbacks->InoutAntennaCbk = (lefrDoubleCbkFnType) func;

    // NEW CALLBACK - Add a line here for each new callback routine 
    if (lefCallbacks->AntennaInputCbk == 0)
        lefCallbacks->AntennaInputCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->AntennaInoutCbk == 0)
        lefCallbacks->AntennaInoutCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->AntennaOutputCbk == 0)
        lefCallbacks->AntennaOutputCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->ManufacturingCbk == 0)
        lefCallbacks->ManufacturingCbk = (lefrDoubleCbkFnType) func;
    if (lefCallbacks->UseMinSpacingCbk == 0)
        lefCallbacks->UseMinSpacingCbk = (lefrUseMinSpacingCbkFnType) func;
    if (lefCallbacks->ClearanceMeasureCbk == 0)
        lefCallbacks->ClearanceMeasureCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->MacroClassTypeCbk == 0)
        lefCallbacks->MacroClassTypeCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->MacroOriginCbk == 0)
        lefCallbacks->MacroOriginCbk = (lefrMacroNumCbkFnType) func;
    if (lefCallbacks->MacroSizeCbk == 0)
        lefCallbacks->MacroSizeCbk = (lefrMacroNumCbkFnType) func;
    if (lefCallbacks->MacroFixedMaskCbk == 0)
        lefCallbacks->MacroFixedMaskCbk = (lefrIntegerCbkFnType) func;
    if (lefCallbacks->MacroEndCbk == 0)
        lefCallbacks->MacroEndCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->MaxStackViaCbk == 0)
        lefCallbacks->MaxStackViaCbk = (lefrMaxStackViaCbkFnType) func;
    if (lefCallbacks->ExtensionCbk == 0)
        lefCallbacks->ExtensionCbk = (lefrStringCbkFnType) func;
    if (lefCallbacks->DensityCbk == 0)
        lefCallbacks->DensityCbk = (lefrDensityCbkFnType) func;
    if (lefCallbacks->FixedMaskCbk == 0)
        lefCallbacks->FixedMaskCbk = (lefrIntegerCbkFnType) func;
}

// These count up the number of times an unset callback is called... 
static int lefrUnusedCount[NOCBK];

int
lefrCountFunc(lefrCallbackType_e    e,
              void                  *v,
              lefiUserData          d)
{
    ASSERT_INIT;
    int i = (int) e;
    if (lefiDebug(23))
        printf("count %d 0x%p 0x%p\n", (int) e, v, d);
    if (i >= 0 && i < NOCBK) {
        lefrUnusedCount[i] += 1;
        return 0;
    }
    return 1;
}

void
lefrSetRegisterUnusedCallbacks()
{
    ASSERT_INIT;
    int i;
    lefSettings->RegisterUnused = 1;
    lefrSetUnusedCallbacks(lefrCountFunc);
    for (i = 0; i < NOCBK; i++)
        lefrUnusedCount[i] = 0;
}

void
lefrPrintUnusedCallbacks(FILE *f)
{
    ASSERT_INIT;
    int i;
    int firstCB = 1;
    int trueCB = 1;

    if (lefSettings->RegisterUnused == 0) {
        fprintf(f,
                "ERROR (LEFPARS-101): lefrSetRegisterUnusedCallbacks was not called to setup this data.\n");
        return;
    }

    for (i = 0; i < NOCBK; i++) {
        if (lefrUnusedCount[i]) {
            // Do not need to print yet if i is:
            //  lefrMacroClassTypeCbkType
            //  lefrMacroOriginCbkType
            //  lefrMacroSizeCbkType
            //  lefrMacroEndCbkType
            // it will be taken care later
            if (firstCB &&
                (lefrCallbackType_e) i != lefrMacroClassTypeCbkType &&
                (lefrCallbackType_e) i != lefrMacroOriginCbkType &&
                (lefrCallbackType_e) i != lefrMacroSizeCbkType &&
                (lefrCallbackType_e) i != lefrMacroFixedMaskCbkType &&
                (lefrCallbackType_e) i != lefrMacroEndCbkType) {
                fprintf(f,
                        "WARNING (LEFPARS-201): LEF items that were present but ignored because of no callback:\n");
                firstCB = 0;
            }
            switch ((lefrCallbackType_e) i) {
            case lefrArrayBeginCbkType:
                fprintf(f, "ArrayBegin");
                break;
            case lefrArrayCbkType:
                fprintf(f, "Array");
                break;
            case lefrArrayEndCbkType:
                fprintf(f, "ArrayEnd");
                break;
            case lefrDividerCharCbkType:
                fprintf(f, "DividerChar");
                break;
            case lefrBusBitCharsCbkType:
                fprintf(f, "BusBitChars");
                break;
            case lefrNoWireExtensionCbkType:
                fprintf(f, "NoWireExtensionAtPins");
                break;
            case lefrCaseSensitiveCbkType:
                fprintf(f, "CaseSensitive");
                break;
            case lefrCorrectionTableCbkType:
                fprintf(f, "CorrectionTable");
                break;
            case lefrDielectricCbkType:
                fprintf(f, "Dielectric");
                break;
            case lefrEdgeRateScaleFactorCbkType:
                fprintf(f, "EdgeRateScaleFactor");
                break;
            case lefrEdgeRateThreshold1CbkType:
                fprintf(f, "EdgeRateThreshold1");
                break;
            case lefrEdgeRateThreshold2CbkType:
                fprintf(f, "EdgeRateThreshold2");
                break;
            case lefrIRDropBeginCbkType:
                fprintf(f, "IRDropBegin");
                break;
            case lefrIRDropCbkType:
                fprintf(f, "IRDrop");
                break;
            case lefrIRDropEndCbkType:
                fprintf(f, "IRDropEnd");
                break;
            case lefrLayerCbkType:
                fprintf(f, "Layer");
                break;
            case lefrLibraryEndCbkType:
                fprintf(f, "LibraryEnd");
                break;
            case lefrMacroBeginCbkType:
                fprintf(f, "MacroBegin");
                break;
            case lefrMacroCbkType:
                fprintf(f, "Macro");
                break;
            case lefrMinFeatureCbkType:
                fprintf(f, "MinFeature");
                break;
            case lefrNoiseMarginCbkType:
                fprintf(f, "NoiseMargin");
                break;
            case lefrNoiseTableCbkType:
                fprintf(f, "NoiseTable");
                break;
            case lefrNonDefaultCbkType:
                fprintf(f, "NonDefault");
                break;
            case lefrObstructionCbkType:
                fprintf(f, "Obstruction");
                break;
            case lefrPinCbkType:
                fprintf(f, "Pin");
                break;
            case lefrPropBeginCbkType:
                fprintf(f, "PropBegin");
                break;
            case lefrPropCbkType:
                fprintf(f, "Prop");
                break;
            case lefrPropEndCbkType:
                fprintf(f, "PropEnd");
                break;
            case lefrSiteCbkType:
                fprintf(f, "Site");
                break;
            case lefrSpacingBeginCbkType:
                fprintf(f, "SpacingBegin");
                break;
            case lefrSpacingCbkType:
                fprintf(f, "Spacing");
                break;
            case lefrSpacingEndCbkType:
                fprintf(f, "SpacingEnd");
                break;
            case lefrUnitsCbkType:
                fprintf(f, "Units");
                break;
            case lefrVersionCbkType:
                fprintf(f, "Version");
                break;
            case lefrVersionStrCbkType:
                fprintf(f, "Version");
                break;
            case lefrViaCbkType:
                fprintf(f, "Via");
                break;
            case lefrViaRuleCbkType:
                fprintf(f, "ViaRule");
                break;
            case lefrInputAntennaCbkType:
                fprintf(f, "InputAntenna");
                break;
            case lefrOutputAntennaCbkType:
                fprintf(f, "OutputAntenna");
                break;
            case lefrInoutAntennaCbkType:
                fprintf(f, "InoutAntenna");
                break;
            case lefrAntennaInputCbkType:
                fprintf(f, "AntennaInput");
                break;
            case lefrAntennaInoutCbkType:
                fprintf(f, "AntennaInout");
                break;
            case lefrAntennaOutputCbkType:
                fprintf(f, "AntennaOutput");
                break;
            case lefrManufacturingCbkType:
                fprintf(f, "Manufacturing");
                break;
            case lefrUseMinSpacingCbkType:
                fprintf(f, "UseMinSpacing");
                break;
            case lefrClearanceMeasureCbkType:
                fprintf(f, "ClearanceMeasure");
                break;
            case lefrTimingCbkType:
                fprintf(f, "Timing");
                break;
            case lefrMaxStackViaCbkType:
                fprintf(f, "MaxStackVia");
                break;
            case lefrExtensionCbkType:
                fprintf(f, "Extension");
                break;
                // 07/13/2001 - Wanda da Rosa
                // Don't need to print MacroClassType if it is not set,
                // since this is an extra CB for Ambit only.
                // Other users should not have to deal with it.
                // case lefrMacroClassTypeCbkType: fprintf(f, "MacroClassType"); break;
            case lefrMacroClassTypeCbkType:
            case lefrMacroOriginCbkType:
            case lefrMacroSizeCbkType:
            case lefrMacroFixedMaskCbkType:
            case lefrMacroEndCbkType:
                trueCB = 0;
                break;
                // NEW CALLBACK  add the print here 
            case lefrDensityCbkType:
                fprintf(f, "Density");
                break;
            case lefrFixedMaskCbkType:
                fprintf(f, "FixedMask");
                break;
            default:
                fprintf(f, "BOGUS ENTRY");
                break;
            }
            if (trueCB)
                fprintf(f, " %d\n", lefrUnusedCount[i]);
            else
                trueCB = 1;
        }
    }
}

void
lefrUnsetCallbacks()
{
    lefrCallbacks::reset();
}

// Unset callbacks functions
void
lefrUnsetAntennaInoutCbk()
{
    ASSERT_INIT;
    lefCallbacks->AntennaInoutCbk = 0;
}

void
lefrUnsetAntennaInputCbk()
{
    ASSERT_INIT;
    lefCallbacks->AntennaInputCbk = 0;
}

void
lefrUnsetAntennaOutputCbk()
{
    ASSERT_INIT;
    lefCallbacks->AntennaOutputCbk = 0;
}

void
lefrUnsetArrayBeginCbk()
{
    ASSERT_INIT;
    lefCallbacks->ArrayBeginCbk = 0;
}

void
lefrUnsetArrayCbk()
{
    ASSERT_INIT;
    lefCallbacks->ArrayCbk = 0;
}

void
lefrUnsetArrayEndCbk()
{
    ASSERT_INIT;
    lefCallbacks->ArrayEndCbk = 0;
}

void
lefrUnsetBusBitCharsCbk()
{
    ASSERT_INIT;
    lefCallbacks->BusBitCharsCbk = 0;
}

void
lefrUnsetCaseSensitiveCbk()
{
    ASSERT_INIT;
    lefCallbacks->CaseSensitiveCbk = 0;
}

void
lefrUnsetClearanceMeasureCbk()
{
    ASSERT_INIT;
    lefCallbacks->ClearanceMeasureCbk = 0;
}

void
lefrUnsetCorrectionTableCbk()
{
    ASSERT_INIT;
    lefCallbacks->CorrectionTableCbk = 0;
}

void
lefrUnsetDensityCbk()
{
    ASSERT_INIT;
    lefCallbacks->DensityCbk = 0;
}

void
lefrUnsetDielectricCbk()
{
    ASSERT_INIT;
    lefCallbacks->DielectricCbk = 0;
}

void
lefrUnsetDividerCharCbk()
{
    ASSERT_INIT;
    lefCallbacks->DividerCharCbk = 0;
}

void
lefrUnsetEdgeRateScaleFactorCbk()
{
    ASSERT_INIT;
    lefCallbacks->EdgeRateScaleFactorCbk = 0;
}

void
lefrUnsetEdgeRateThreshold1Cbk()
{
    ASSERT_INIT;
    lefCallbacks->EdgeRateThreshold1Cbk = 0;
}

void
lefrUnsetEdgeRateThreshold2Cbk()
{
    ASSERT_INIT;
    lefCallbacks->EdgeRateThreshold2Cbk = 0;
}

void
lefrUnsetExtensionCbk()
{
    ASSERT_INIT;
    lefCallbacks->ExtensionCbk = 0;
}

void
lefrUnsetFixedMaskCbk()
{
    ASSERT_INIT;
    lefCallbacks->FixedMaskCbk = 0;
}
void
lefrUnsetIRDropBeginCbk()
{
    ASSERT_INIT;
    lefCallbacks->IRDropBeginCbk = 0;
}

void
lefrUnsetIRDropCbk()
{
    ASSERT_INIT;
    lefCallbacks->IRDropCbk = 0;
}

void
lefrUnsetIRDropEndCbk()
{
    ASSERT_INIT;
    lefCallbacks->IRDropEndCbk = 0;
}

void
lefrUnsetInoutAntennaCbk()
{
    ASSERT_INIT;
    lefCallbacks->InoutAntennaCbk = 0;
}

void
lefrUnsetInputAntennaCbk()
{
    ASSERT_INIT;
    lefCallbacks->InputAntennaCbk = 0;
}

void
lefrUnsetLayerCbk()
{
    ASSERT_INIT;
    lefCallbacks->LayerCbk = 0;
}

void
lefrUnsetLibraryEndCbk()
{
    ASSERT_INIT;
    lefCallbacks->LibraryEndCbk = 0;
}

void
lefrUnsetMacroBeginCbk()
{
    ASSERT_INIT;
    lefCallbacks->MacroBeginCbk = 0;
}

void
lefrUnsetMacroCbk()
{
    ASSERT_INIT;
    lefCallbacks->MacroCbk = 0;
}

void
lefrUnsetMacroClassTypeCbk()
{
    ASSERT_INIT;
    lefCallbacks->MacroClassTypeCbk = 0;
}

void
lefrUnsetMacroEndCbk()
{
    ASSERT_INIT;
    lefCallbacks->MacroEndCbk = 0;
}

void
lefrUnsetMacroFixedMaskCbk()
{
    ASSERT_INIT;
    lefCallbacks->MacroFixedMaskCbk = 0;
}

void
lefrUnsetMacroOriginCbk()
{
    ASSERT_INIT;
    lefCallbacks->MacroOriginCbk = 0;
}

void
lefrUnsetMacroSizeCbk()
{
    ASSERT_INIT;
    lefCallbacks->MacroSizeCbk = 0;
}

void
lefrUnsetManufacturingCbk()
{
    ASSERT_INIT;
    lefCallbacks->ManufacturingCbk = 0;
}

void
lefrUnsetMaxStackViaCbk()
{
    ASSERT_INIT;
    lefCallbacks->MaxStackViaCbk = 0;
}

void
lefrUnsetMinFeatureCbk()
{
    ASSERT_INIT;
    lefCallbacks->MinFeatureCbk = 0;
}

void
lefrUnsetNoWireExtensionCbk()
{
    ASSERT_INIT;
    lefCallbacks->NoWireExtensionCbk = 0;
}

void
lefrUnsetNoiseMarginCbk()
{
    ASSERT_INIT;
    lefCallbacks->NoiseMarginCbk = 0;
}

void
lefrUnsetNoiseTableCbk()
{
    ASSERT_INIT;
    lefCallbacks->NoiseTableCbk = 0;
}

void
lefrUnsetNonDefaultCbk()
{
    ASSERT_INIT;
    lefCallbacks->NonDefaultCbk = 0;
}

void
lefrUnsetObstructionCbk()
{
    ASSERT_INIT;
    lefCallbacks->ObstructionCbk = 0;
}

void
lefrUnsetOutputAntennaCbk()
{
    ASSERT_INIT;
    lefCallbacks->OutputAntennaCbk = 0;
}

void
lefrUnsetPinCbk()
{
    ASSERT_INIT;
    lefCallbacks->PinCbk = 0;
}

void
lefrUnsetPropBeginCbk()
{
    ASSERT_INIT;
    lefCallbacks->PropBeginCbk = 0;
}

void
lefrUnsetPropCbk()
{
    ASSERT_INIT;
    lefCallbacks->PropCbk = 0;
}

void
lefrUnsetPropEndCbk()
{
    ASSERT_INIT;
    lefCallbacks->PropEndCbk = 0;
}

void
lefrUnsetSiteCbk()
{
    ASSERT_INIT;
    lefCallbacks->SiteCbk = 0;
}

void
lefrUnsetSpacingBeginCbk()
{
    ASSERT_INIT;
    lefCallbacks->SpacingBeginCbk = 0;
}

void
lefrUnsetSpacingCbk()
{
    ASSERT_INIT;
    lefCallbacks->SpacingCbk = 0;
}

void
lefrUnsetSpacingEndCbk()
{
    ASSERT_INIT;
    lefCallbacks->SpacingEndCbk = 0;
}

void
lefrUnsetTimingCbk()
{
    ASSERT_INIT;
    lefCallbacks->TimingCbk = 0;
}

void
lefrUnsetUnitsCbk()
{
    ASSERT_INIT;
    lefCallbacks->UnitsCbk = 0;
}

void
lefrUnsetUseMinSpacingCbk()
{
    ASSERT_INIT;
    lefCallbacks->UseMinSpacingCbk = 0;
}

void
lefrUnsetVersionCbk()
{
    ASSERT_INIT;
    lefCallbacks->VersionCbk = 0;
}

void
lefrUnsetVersionStrCbk()
{
    ASSERT_INIT;
    lefCallbacks->VersionStrCbk = 0;
}

void
lefrUnsetViaCbk()
{
    ASSERT_INIT;
    lefCallbacks->ViaCbk = 0;
}

void
lefrUnsetViaRuleCbk()
{
    ASSERT_INIT;
    lefCallbacks->ViaRuleCbk = 0;
}

// Setting of user data.
void
lefrSetUserData(lefiUserData d)
{
    ASSERT_INIT;
    lefSettings->UserData = d;
}

lefiUserData
lefrGetUserData()
{
    ASSERT_INIT;
    return lefSettings->UserData;
}

// Callbacks set functions.

void
lefrSetAntennaInoutCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->AntennaInoutCbk = f;
}

void
lefrSetAntennaInputCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->AntennaInputCbk = f;
}

void
lefrSetAntennaOutputCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->AntennaOutputCbk = f;
}

void
lefrSetArrayBeginCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ArrayBeginCbk = f;
}

void
lefrSetArrayCbk(lefrArrayCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ArrayCbk = f;
}

void
lefrSetArrayEndCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ArrayEndCbk = f;
}

void
lefrSetBusBitCharsCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->BusBitCharsCbk = f;
}

void
lefrSetCaseSensitiveCbk(lefrIntegerCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->CaseSensitiveCbk = f;
}

void
lefrSetClearanceMeasureCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ClearanceMeasureCbk = f;
}

void
lefrSetCorrectionTableCbk(lefrCorrectionTableCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->CorrectionTableCbk = f;
}

void
lefrSetDensityCbk(lefrDensityCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->DensityCbk = f;
}

void
lefrSetDielectricCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->DielectricCbk = f;
}

void
lefrSetDividerCharCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->DividerCharCbk = f;
}

void
lefrSetEdgeRateScaleFactorCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->EdgeRateScaleFactorCbk = f;
}

void
lefrSetEdgeRateThreshold1Cbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->EdgeRateThreshold1Cbk = f;
}

void
lefrSetEdgeRateThreshold2Cbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->EdgeRateThreshold2Cbk = f;
}

void
lefrSetExtensionCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ExtensionCbk = f;
}

void
lefrSetFixedMaskCbk(lefrIntegerCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->FixedMaskCbk = f;
}

void
lefrSetIRDropBeginCbk(lefrVoidCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->IRDropBeginCbk = f;
}

void
lefrSetIRDropCbk(lefrIRDropCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->IRDropCbk = f;
}

void
lefrSetIRDropEndCbk(lefrVoidCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->IRDropEndCbk = f;
}

void
lefrSetInoutAntennaCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->InoutAntennaCbk = f;
}

void
lefrSetInputAntennaCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->InputAntennaCbk = f;
}

void
lefrSetLayerCbk(lefrLayerCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->LayerCbk = f;
}

void
lefrSetLibraryEndCbk(lefrVoidCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->LibraryEndCbk = f;
}

void
lefrSetMacroBeginCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MacroBeginCbk = f;
}

void
lefrSetMacroCbk(lefrMacroCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MacroCbk = f;
}

void
lefrSetMacroClassTypeCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MacroClassTypeCbk = f;
}

void
lefrSetMacroEndCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MacroEndCbk = f;
}

void
lefrSetMacroFixedMaskCbk(lefrIntegerCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MacroFixedMaskCbk = f;
}

void
lefrSetMacroOriginCbk(lefrMacroNumCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MacroOriginCbk = f;
}

void
lefrSetMacroSizeCbk(lefrMacroNumCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MacroSizeCbk = f;
}

void
lefrSetManufacturingCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ManufacturingCbk = f;
}

void
lefrSetMaxStackViaCbk(lefrMaxStackViaCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MaxStackViaCbk = f;
}

void
lefrSetMinFeatureCbk(lefrMinFeatureCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->MinFeatureCbk = f;
}

void
lefrSetNoWireExtensionCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->NoWireExtensionCbk = f;
}

void
lefrSetNoiseMarginCbk(lefrNoiseMarginCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->NoiseMarginCbk = f;
}

void
lefrSetNoiseTableCbk(lefrNoiseTableCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->NoiseTableCbk = f;
}

void
lefrSetNonDefaultCbk(lefrNonDefaultCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->NonDefaultCbk = f;
}

void
lefrSetObstructionCbk(lefrObstructionCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ObstructionCbk = f;
}

void
lefrSetOutputAntennaCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->OutputAntennaCbk = f;
}

void
lefrSetPinCbk(lefrPinCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->PinCbk = f;
}

void
lefrSetPropBeginCbk(lefrVoidCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->PropBeginCbk = f;
}

void
lefrSetPropCbk(lefrPropCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->PropCbk = f;
}

void
lefrSetPropEndCbk(lefrVoidCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->PropEndCbk = f;
}

void
lefrSetSiteCbk(lefrSiteCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->SiteCbk = f;
}

void
lefrSetSpacingBeginCbk(lefrVoidCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->SpacingBeginCbk = f;
}

void
lefrSetSpacingCbk(lefrSpacingCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->SpacingCbk = f;
}

void
lefrSetSpacingEndCbk(lefrVoidCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->SpacingEndCbk = f;
}

void
lefrSetTimingCbk(lefrTimingCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->TimingCbk = f;
}

void
lefrSetUnitsCbk(lefrUnitsCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->UnitsCbk = f;
}

void
lefrSetUseMinSpacingCbk(lefrUseMinSpacingCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->UseMinSpacingCbk = f;
}

void
lefrSetVersionCbk(lefrDoubleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->VersionCbk = f;
}

void
lefrSetVersionStrCbk(lefrStringCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->VersionStrCbk = f;
}

void
lefrSetViaCbk(lefrViaCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ViaCbk = f;
}

void
lefrSetViaRuleCbk(lefrViaRuleCbkFnType f)
{
    ASSERT_INIT;
    lefCallbacks->ViaRuleCbk = f;
}

int
lefrLineNumber()
{
    // Compatibility feature: in old versions the translators,  
    // the function can be called before lefData initialization. 
    return lefData ? lefData->lef_nlines : 0; 
}

void
lefrSetLogFunction(LEFI_LOG_FUNCTION f)
{
    ASSERT_INIT;
    lefSettings->ErrorLogFunction = f;
}

void
lefrSetWarningLogFunction(LEFI_WARNING_LOG_FUNCTION f)
{
    ASSERT_INIT;
    lefSettings->WarningLogFunction = f;
}

void
lefrSetMallocFunction(LEFI_MALLOC_FUNCTION f)
{
    ASSERT_INIT;
    lefSettings->MallocFunction = f;
}

void
lefrSetReallocFunction(LEFI_REALLOC_FUNCTION f)
{
    ASSERT_INIT;
    lefSettings->ReallocFunction = f;
}

void
lefrSetFreeFunction(LEFI_FREE_FUNCTION f)
{
    ASSERT_INIT;
    lefSettings->FreeFunction = f;
}

void
lefrSetLineNumberFunction(LEFI_LINE_NUMBER_FUNCTION f)
{
    ASSERT_INIT;
    lefSettings->LineNumberFunction = f;
}

void
lefrSetDeltaNumberLines(int numLines)
{
    ASSERT_INIT;
    lefSettings->DeltaNumberLines = numLines;
}

// from the lexer

void
lefrSetShiftCase()
{
    ASSERT_INIT;
    lefSettings->ShiftCase = 1;
}

void
lefrSetCommentChar(char c)
{
    ASSERT_INIT;
    lefSettings->CommentChar = c;
}

void
lefrSetCaseSensitivity(int caseSense)
{
    ASSERT_INIT;
    lefSettings->CaseSensitive = caseSense;
    lefSettings->CaseSensitiveSet = TRUE;
}

void
lefrSetRelaxMode()
{
    ASSERT_INIT;
    lefSettings->RelaxMode = TRUE;
}

void
lefrUnsetRelaxMode()
{
    ASSERT_INIT;
    lefSettings->RelaxMode = FALSE;
}

void
lefrSetVersionValue(const char *version)
{
    ASSERT_INIT;
    lefSettings->VersionNum = convert_name2num(version);
}

void
lefrSetOpenLogFileAppend()
{
    ASSERT_INIT;
    lefSettings->LogFileAppend = TRUE;
}

void
lefrUnsetOpenLogFileAppend()
{
    ASSERT_INIT;
    lefSettings->LogFileAppend = FALSE;
}

void
lefrSetReadFunction(LEFI_READ_FUNCTION f)
{
    ASSERT_INIT;
    lefSettings->ReadFunction = f;
}

void
lefrUnsetReadFunction()
{
    ASSERT_INIT;
    lefSettings->ReadFunction = 0;
}

// Set the maximum number of warnings
//
// *****************************************************************************

void
lefrSetAntennaInoutWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->AntennaInoutWarnings = warn;
}

void
lefrSetAntennaInputWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->AntennaInputWarnings = warn;
}

void
lefrSetAntennaOutputWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->AntennaOutputWarnings = warn;
}

void
lefrSetArrayWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->ArrayWarnings = warn;
}

void
lefrSetCaseSensitiveWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->CaseSensitiveWarnings = warn;
}

void
lefrSetCorrectionTableWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->CorrectionTableWarnings = warn;
}

void
lefrSetDielectricWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->DielectricWarnings = warn;
}

void
lefrSetEdgeRateThreshold1Warnings(int warn)
{
    ASSERT_INIT;
    lefSettings->EdgeRateThreshold1Warnings = warn;
}

void
lefrSetEdgeRateThreshold2Warnings(int warn)
{
    ASSERT_INIT;
    lefSettings->EdgeRateThreshold2Warnings = warn;
}

void
lefrSetEdgeRateScaleFactorWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->EdgeRateScaleFactorWarnings = warn;
}

void
lefrSetInoutAntennaWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->InoutAntennaWarnings = warn;
}

void
lefrSetInputAntennaWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->InputAntennaWarnings = warn;
}

void
lefrSetIRDropWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->IRDropWarnings = warn;
}

void
lefrSetLayerWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->LayerWarnings = warn;
}

void
lefrSetMacroWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->MacroWarnings = warn;
}

void
lefrSetMaxStackViaWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->MaxStackViaWarnings = warn;
}

void
lefrSetMinFeatureWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->MinFeatureWarnings = warn;
}

void
lefrSetNoiseMarginWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->NoiseMarginWarnings = warn;
}

void
lefrSetNoiseTableWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->NoiseTableWarnings = warn;
}

void
lefrSetNonDefaultWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->NonDefaultWarnings = warn;
}

void
lefrSetNoWireExtensionWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->NoWireExtensionWarnings = warn;
}

void
lefrSetOutputAntennaWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->OutputAntennaWarnings = warn;
}

void
lefrSetPinWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->PinWarnings = warn;
}

void
lefrSetSiteWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->SiteWarnings = warn;
}

void
lefrSetSpacingWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->SpacingWarnings = warn;
}

void
lefrSetTimingWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->TimingWarnings = warn;
}

void
lefrSetUnitsWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->UnitsWarnings = warn;
}

void
lefrSetUseMinSpacingWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->UseMinSpacingWarnings = warn;
}

void
lefrSetViaRuleWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->ViaRuleWarnings = warn;
}

void
lefrSetViaWarnings(int warn)
{
    ASSERT_INIT;
    lefSettings->ViaWarnings = warn;
}

void
lefrDisablePropStrProcess()
{
    ASSERT_INIT;
    lefSettings->DisPropStrProcess = 1;
}

END_LEFDEF_PARSER_NAMESPACE


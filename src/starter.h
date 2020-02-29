//
//  STARTER.H
//

//==========================================================================================================
// NOTICE:
//
// The Astron code base was released for the fun and enjoyment of developers.  The Astron game or any game 
// or application derived from the code base may not be sold for profit without the specific written consent 
// of the original developer (Neal Coleman, info@ncblast.com).  And I'm not likely to grant that 
// consent - this code is for fun.  Feel free to use the code and exchange it with other developers. 
// This boilerplate text must remain with the code.
//
//===========================================================================================================

#ifndef _STARTER2_H_
#define _STARTER2_H_

#include <PalmOS.h>
#include <UIPublic.h>
#include <PalmTypes.h>
#include <PalmCompatibility.h>

#include "startRsc.h"
#include "defs.h"
#include "dbver1.h"  // contains structure of saved records


//************ HERE'S WHERE WE DEFINE IF LOAD IS TESTING OR RELEASE **************
//#define THIS_IS_A_RELEASE_LOAD
//************ HERE'S WHERE WE DEFINE IF LOAD IS TESTING OR RELEASE **************



//-----------------------------------------------------------------------
// PRC section macro -- see coolfg.def
//-----------------------------------------------------------------------
#define SEC1  __attribute__ ((section ("sec1fns")))
#define SEC2  __attribute__ ((section ("sec2fns")))
#define SEC3  __attribute__ ((section ("sec3fns")))
#define SEC4  __attribute__ ((section ("sec4fns")))
#define SEC5  __attribute__ ((section ("sec5fns")))
#define SEC6  __attribute__ ((section ("sec6fns")))


//----------------------------------------------------------------------
// Vars
//----------------------------------------------------------------------


// Game
UInt16       gt;
appStateEnum appState;
UInt32       MinTicksPerFrame;
UInt32       MaxTicksBeforeText;

// Screen
UInt8        ScreenDepth;

// Password
Byte    pwWord[4];
//char    fmtUserName[9];
Byte    numTimesRun;
Byte    nagTimerVal;
ULong   installTimGetSeconds;
Boolean tenDayAdditionOn;
ULong   lastTimRan;
Boolean timeModifiedFraudFlag;
UInt16  numTimesRestarted;

// must be global because debug screen can launch from blast.c
UInt16 lastForm;

// records
mainGameRecordStructV1 mainGameRec1Default;
mainGameRecordStructV1 mainGameRec1;   
mainGameRecordStructV2 mainGameRec2; 
mainGameRecordStructV3 mainGameRec3Default;
mainGameRecordStructV3 mainGameRec3;
mainGameRecordStructV4 mainGameRec4Default;
mainGameRecordStructV4 mainGameRec4;


//----------------------------------------------------------------------
// Procs
//----------------------------------------------------------------------

Boolean FormEventHandlerGameScreen(EventPtr eventP) SEC5;
Boolean FormEventHandlerRegisScreen(EventPtr eventP) SEC4;
Boolean FormEventHandlerSettingsScreen(EventPtr eventP) SEC4;
Boolean FormEventHandlerDebugScreen(EventPtr eventP) SEC2;
Boolean FormEventHandlerInstructionsScreen(EventPtr eventP) SEC2;
Boolean FormEventHandlerGreyscaleOpsScreen(EventPtr eventP) SEC4;


Boolean HdlDebugMenuEvent(UInt16 command) SEC2;
void    HdlUserSaysStart() SEC4;

void    UtilConfigHardKeys()  SEC4;
void    UtilRestoreHardKeys() SEC4;
void    UtilFixV1HardKeyBug() SEC5;

Boolean AppRestoreState() SEC4;
void    AppSetScreenDepthVar() SEC4;

#endif

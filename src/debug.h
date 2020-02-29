//
//  debug.h 
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

#ifndef _DEBUG_H_
#define _DEBUG_H_


#include <PalmTypes.h>
#include <PalmCompatibility.h>
#include <TimeMgr.h>               // Time ticks

#include "sprite.h"   // for sprite *
#include "starter.h"



//----------------------------------------------------------------------------------
//  Enums
//----------------------------------------------------------------------------------

typedef enum
{
  noDebugText=0,
  funcEnterOnly,
  timeToo,       
  allDebugText
} debugLevelEnum;

//----------------------------------------------------------------------------------
//  Vars
//----------------------------------------------------------------------------------

char debugStr[100];
char debugStr2[100];
char dstr[8];

// Frame Timing
#define MAXTIMEDINCRS 16
#define MAXTIMEDINTRAMEAS 10

UInt32  DebugStopwatch;
UInt16  DebugNumFrames;
Int32   DebugTotalTime;
UInt16  DebugTimedFrames[MAXTIMEDINCRS];
Int32   DebugTimedIntraFrames[MAXTIMEDINTRAMEAS];


#define NUMVARMEAS 100
UInt8   DebugCurrectVarMeas;
Int32   DebugVarTiming[NUMVARMEAS];
Boolean DebugVarTimingOverTop;

UInt8   debugHighTime;

#define MAXBESTFRAMETIME 7





//----------------------------------------------------------------------------------
//  Debug flags
//----------------------------------------------------------------------------------

Boolean SKIPINTROWAVES;     // Just put bugs into formation and start gameplay
Boolean SKIPREGPLAY;        // Just do the intro's
Boolean AUTOPLAY;           // Simulate user's play (for testing)
Boolean TITLEBAROFF;
Boolean DEBUGTEXT;           // Allow onscreen debug text to be turned on/off
Boolean USERIMMUNE;         // User not hurt by bullets or collisions
Boolean HOLDFORMATION;     // Bugs do not dive after intro
Boolean TIMEFRAMES;         // Time ticks per frame (and display)
Boolean ALLOWDEBUGMENU;       // Allow debug menu to be displayed
Boolean IGNORETRACBEAM;      // Useful during auto play to ignore trac beam
Boolean DEBUGCOLLTIMINGON;   // used in collision detection timing
 

debugLevelEnum DEBUGLEVEL; // level of detail 
debugLevelEnum OLDDEBUGLEVEL; // for overriding 





//********************************** START OF TESTING MACROS *************************** 
//********************************** START OF TESTING MACROS *************************** 
//********************************** START OF TESTING MACROS *************************** 
//********************************** START OF TESTING MACROS *************************** 
//********************************** START OF TESTING MACROS *************************** 
#ifndef THIS_IS_A_RELEASE_LOAD


//----------------------------------------------------------------------------------
// Debug Procs
//----------------------------------------------------------------------------------
void DebugClearTimingVars();
void DebugDisplayTimingText();
void DebugUpdateVarTiming();
void DebugRunGraphicsTest();
void DebugRunCollisionTest();
//Byte DebugCalcCollRegion(sprite *) SEC3;
void DebugFindHighTime(UInt8 ticks);
void DebugCheckCollRegion() SEC3;
void DebugTestRepaints() SEC3;



//----------------------------------------------------------------------------------
//  Error asserts -- visible to player/tester
//----------------------------------------------------------------------------------

#define ERROR_ASSERT(s) debugPutsError(s);StrIToA(dstr,__LINE__); \
                        FrmCustomAlert(nealErrAlertId,__FILE__,dstr,s);

//----------------------------------------------------------------------------------
//  Timing macros
//----------------------------------------------------------------------------------
#define debugStartStopwatch() DebugStopwatch=TimGetTicks()


#define debugDumpElapTime(s)   if (DEBUGLEVEL>=timeToo) { \
     HostTraceOutputTL(appErrorClass, "%4hd [%10s:%-4hd] At:%hd Elap time=%hd", \
        gt,__FILE__,__LINE__,s,(Int)(TimGetTicks()-DebugStopwatch)); \
     DebugTimedIntraFrames[s]+=(TimGetTicks()-DebugStopwatch); \
     DebugStopwatch=TimGetTicks(); }


//----------------------------------------------------------------------------------
//  Debug macros
//
//  Reference: gnu document (file:///C|/PalmDev/doc/texi/cpp_13.html#IDX24)
//----------------------------------------------------------------------------------

//
//  Print function entry
//
#define  debugFuncEnter(s) if (DEBUGLEVEL>=funcEnterOnly) \
                HostTraceOutputTL (appErrorClass, "%4hd [%10s:%-4hd] ---%s", gt,__FILE__,__LINE__,s)

//
//  Print function entry -- combined with debugPutInt
//
#define  debugFuncEnterInt(s,v) if (DEBUGLEVEL>=funcEnterOnly) \
                HostTraceOutputTL (appErrorClass, "%4hd [%10s:%-4hd] ---%s=%hd", gt,__FILE__,__LINE__,s,(Int)v)

//
//  Print function exit
//
#define  debugFuncExit(s) if (DEBUGLEVEL>=funcEnterOnly) \
                HostTraceOutputTL (appErrorClass, "%4hd [%10s:%-4hd] <<<%s", gt,__FILE__,__LINE__,s)


//
//  Print a constant string
//
#define  debugPuts(s) if (DEBUGLEVEL>=allDebugText) \
              HostTraceOutputTL (appErrorClass, "%4hd [%10s:%-4hd] %s", gt,__FILE__,__LINE__,s)

//
//  Print 1 Int value
//
#define  debugPutInt(s,v) if (DEBUGLEVEL>=allDebugText) \
                HostTraceOutputTL (appErrorClass, "%4hd [%10s:%-4hd] %s=%hd", gt,__FILE__,__LINE__,s,(Int)v)

//
//  Print 1 UInt16 hex value
//
#define  debugPutHex(s,v) if (DEBUGLEVEL>=allDebugText) \
                HostTraceOutputTL (appErrorClass, "%4hd [%10s:%-4hd] %s=%#06x", gt,__FILE__,__LINE__,s,v)

//
//  Print 1 UInt32 hex value
//  lx = long HEX
//
#define  debugPutUInt32Hex(s,v) if (DEBUGLEVEL>=allDebugText) \
                HostTraceOutputTL (appErrorClass, "%4hd [%10s:%-4hd] %s=%#010lx", gt,__FILE__,__LINE__,s,v)

//
//  Used in rare occasions
//
#define debugPutsNoFormat(s) HostTraceOutputTL (appErrorClass, "%s",s);

//----------------------------------------------------------------------------------
//  Override the DEBUGLEVEL (used for temporary overrides)
//----------------------------------------------------------------------------------
#define debugOverrideLevel(v) OLDDEBUGLEVEL=DEBUGLEVEL;DEBUGLEVEL=v;

#define debugRestoreLevel() DEBUGLEVEL=OLDDEBUGLEVEL;

//----------------------------------------------------------------------------------
//  Error logging
//----------------------------------------------------------------------------------
#define  debugPutsError(s) HostTraceOutputTL (appErrorClass, \
                               "\n**ERROR**  %4hd [%10s:%-4hd] %s\n", gt,__FILE__,__LINE__,s)

#define  debugPutIntError(s,v) HostTraceOutputTL (appErrorClass, \
                               "\n**ERROR**  %4hd [%10s:%-4hd] %s=%hd\n", gt,__FILE__,__LINE__,s,(Int)v)

#define  debugTempOnly()  HostTraceOutputTL (appErrorClass, \
  "\n**ERROR IF LEFT IN RELEASE VERSION**  %4hd [%10s:%-4hd] \n", gt,__FILE__,__LINE__)


//----------------------------------------------------------------------------------
//  Onscreen debug text
//----------------------------------------------------------------------------------
#define  debugOnscreen(s) StrCopy( debugStr,s)


//----------------------------------------------------------------------------------
//  Got heres...
//----------------------------------------------------------------------------------
#define got_here if (DEBUGLEVEL > noDebugText) HostTraceOutputTL (appErrorClass, \
                               "%4hd [%10s:%-4hd] ..got here", gt,__FILE__,__LINE__)

#define got_here2 if (DEBUGLEVEL > noDebugText) HostTraceOutputTL (appErrorClass, \
                               "%4hd [%10s:%-4hd] ..got here", 0,__FILE__,__LINE__)


//----------------------------------------------------------------------------------
//  Skip password check
//----------------------------------------------------------------------------------
#define  debugNealSkipPWCheck  



//********************************* START OF RELEASE MACROS *************************** 
//********************************* START OF RELEASE MACROS *************************** 
//********************************* START OF RELEASE MACROS *************************** 
#else // !THIS_IS_A_RELEASE_LOAD



// NULL out procedures
#define  DebugClearTimingVars() 
#define  DebugDisplayTimingText();
#define  DebugUpdateVarTiming();
#define  DebugRunGraphicsTest();
#define  DebugRunCollisionTest();
#define  DebugFindHighTime(ticks);
//#define  DebugCalcCollRegion(s);
#define  DebugCheckCollRegion();
#define  DebugTestRepaints();

// NULL out macros
#define  ERROR_ASSERT(s)
#define  debugStartStopwatch()
#define  debugDumpElapTime(s)
#define  debugFuncEnter(s)
#define  debugFuncEnterInt(s,v)
#define  debugFuncExit(s)
#define  debugPuts(s)
#define  debugPutInt(s,v)
#define  debugPutHex(s,v)
#define  debugPutUInt32Hex(s,v)
#define  debugPutsNoFormat(s)
#define  debugOverrideLevel(v)
#define  debugRestoreLevel()
#define  debugPutsError(s)
#define  debugPutIntError(s,v)
#define  debugOnscreen(s)
#define  got_here 
#define  got_here2 

// **** MUST BE NULL -- TURNS OFF ALL PW CHECKING **** 
#define  debugNealSkipPWCheck  
// **** MUST BE NULL -- TURNS OFF ALL PW CHECKING **** 




#endif // !THIS_IS_A_RELEASE_LOAD







#endif  // debug.h

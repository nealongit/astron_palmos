//
//  MISC.H
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

#ifndef _MISC_H_
#define _MISC_H_


#include <PalmOS.h>
#include <UIPublic.h>
#include <PalmTypes.h>
#include <PalmCompatibility.h>

#include "starter.h" // SEC3 macro
#include "sprite.h" // for sprite *

//------------------------------------------------------------------------
//  Enums
//------------------------------------------------------------------------

typedef enum
{
  unrestricted,
  warningPeriod,
  nagTimerPeriod
} allowRCEnum;



//------------------------------------------------------------------------
// Struct
//------------------------------------------------------------------------
typedef struct
{
  char  name[41];
  ULong pw;
} pwTestStruct;


//-----------------------------------------------------------------------
//  Vars
//-----------------------------------------------------------------------
Int randomSeed;



//-----------------------------------------------------------------------
//   Random num gen, returns 1..n-1                   
//-----------------------------------------------------------------------
Int     randomNum(Int n) SEC4;
Boolean randomPercTest(Int p)  SEC6;

//------------------------------------------------------------------------
// Procs
//------------------------------------------------------------------------

void    debugTestManyPWs() SEC3;
Int16   UtilCharToHex(char * c) SEC3;
void    UtilSetCPULevel(UInt16) SEC3;

UInt32 Util4BitTo32Bit(Byte n) SEC3;
UInt32 Util2BitTo32Bit(Byte n) SEC3;

cpuSpeedEnum UtilDetermineCPUSpeed(Boolean onScreem) SEC3;


#endif


//
// MISC.C
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

#include <DLServer.h>  // for user name

#include "misc.h"
#include "starter.h"
#include "defs.h"
#include "debug.h"
#include "gameman.h" // for randomNum, etc
#include "sprite.h"  // for SpriteEngNumSpritesRendered
#include "debug.h"
#include "graphics.h" // for COG tests
#include "database.h"


//----------------------------------------------------------------------
//  Vars
//----------------------------------------------------------------------

// password testing
Boolean PWTESTING=false;
char * TESTTHISNAME=NULL;

Int     randomSeed =342; 
char    fmtUserName[9];


//-----------------------------------------------------------------------
// Generate random number, and re-seed for next time
//-----------------------------------------------------------------------
Int randomNum(Int n)
{
  // prevent divide by zeroes
  if (n==0) { return 0;} 
  randomSeed = SysRandom(TimGetTicks() + randomSeed - 17)%n;
  return randomSeed;
}

//--------------------------------------------------------------------
//  randomPercTest
//-------------------------------------------------------------------- 
Boolean randomPercTest(Int p)
{
  return (randomNum(100) < p);
}


//----------------------------------------------------------------------
// UtilCharToHex
//
// Given an ascii char, return it's hex value (or -1 if invalid)
// In ascii: 0=48, a=97
//----------------------------------------------------------------------
Int16 UtilCharToHex(char * c)
{

  char l;

  if (c == NULL) {
    debugFuncExit("YtilCharToHex()");
    return -1;
  } else {
    l = *c;
  }

  debugFuncEnterInt("UtilCharToHex, l",l);

  // Convert to lowercase if needed
  if ((l >=65) && (l <=90))  { l+=32; }

  // Handle non 0-9a-f
  if ((l < 48) || (l >102)) {
    debugPuts("  not hex char");
    debugFuncExit("UtilCharToHex()");
    return -1;
  } else {
    if ((l>57) && (l<97)) {
    debugPuts("  not hex char");
    debugFuncExit("UtilCharToHex()");
    return -1;
    }
  }

  // Handle 0-9
  if (l<=57) {
    debugFuncExit("UtilCharToHex()");
    return l-48;
  } else {
     // Handle a-f
    debugFuncExit("UtilCharToHex()");
    return l-97+10;
  }

   debugFuncExit("UtilCharToHex()");
}


//----------------------------------------------------------------------
// Util4BitTo32Bit
//
// Replicate a 4-bit number to 32-bit 
//----------------------------------------------------------------------
UInt32 Util4BitTo32Bit(Byte c)
{
   UInt32 a=0,b=0,n=0;
   
   // Clean up the input so left shifting does not get weird
   n=c & 0x0000000f;
   debugFuncEnterInt("Util4BitTo32Bit() n",n);
   
   a=(n<<12)|(n<<8)|(n<<4)|n;
   b=(a<<16)|a;

   debugPutUInt32Hex("n",n);
   debugPutUInt32Hex("a",a);
   debugPutUInt32Hex("b",b);

   return b;
}

//----------------------------------------------------------------------
// Util2BitTo32Bit
//
// Replicate a 2-bit number to 32-bit 
//----------------------------------------------------------------------
UInt32 Util2BitTo32Bit(Byte n)
{
   UInt32 a=0,b=0;

   debugFuncEnter("Util2BitTo32Bit()");
   
   n=n & 0x3;
   
   a=(n<<6)|(n<<4)|(n<<2)|n;     // 8-bits
   b=(a<<24)|(a<<16)|(a<<8)|a;

   return b;
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
void debugTestManyPWs()
{ 
}



//----------------------------------------------------------------------
// UtilSetCPULevel
//
// Forsee CPU-intensive events and set UtilCPULevel global to 
// avoid them.
//----------------------------------------------------------------------
void UtilSetCPULevel(UInt16 lastFrameTime)
{

   UInt8 numBugs,bugLimitMedCPU,bugLimitHighCPU;
   
   debugFuncEnter("UtilSetCPULevel");
   debugPutInt("SpriteEngNumSpritesRendered",SpriteEngNumSpritesRendered);

   // 1-bit game has more CPU to spare
   bugLimitMedCPU =  (ScreenDepth == oneBitDepth) ? 30 : 24;
   bugLimitHighCPU = (ScreenDepth == oneBitDepth) ? 27 : 21;

   // Weight the barriers more heavily
   numBugs = roundInfo.numRemainBugs + (2*roundInfo.numBarriers);
   debugPutInt("numBugs",numBugs);
   
   // Set cpu   
   UtilCPULevel = CPULevelOK;

   if (!gameInfo.twoShots) {
      if (numBugs > bugLimitMedCPU) {
	 UtilCPULevel = CPULevelMed;
      }
   } else {
      if (numBugs > bugLimitHighCPU) {
	 UtilCPULevel = CPULevelHigh;
      }
   } 
   
  
   debugPutInt("  UtilCPULevel",UtilCPULevel);
   debugFuncExit("UtilSetCPULevel()");

}


//----------------------------------------------------------------------
// UtilDetermineCPUSpeed
//
// Rough determination of CPU speed.  SysTicksPerSecond does not differentiate
// between different CPUs.
//----------------------------------------------------------------------
cpuSpeedEnum UtilDetermineCPUSpeed(Boolean onScreen)
{

   UInt32 i,startTime,stopTime,x,y;
   char msg[80]="",temp[20]="";
   cpuSpeedEnum rc;
   
   debugFuncEnter("UtilDetermineCPUSpeed()");   

   // ---------- START of CPU intensive operation ----------
   startTime=TimGetTicks();

   x=3;y=7;
   for (i=0;i<35000;i++) {
      x=y*212*35/41*5;
      y+=7;           
   }
   
   stopTime=TimGetTicks();
   // ---------- END of CPU intensive operation ----------

   // CPUs 16MHz and slower normally take 12 ticks.  20MHz CPUs take 7-8 ticks.
   // 33MHz CPUs take 5 ticks (Prism tested)
   rc = (stopTime-startTime >= 10) ? cpuIs16MHz : cpuIs20MHz;
   

   // Debug
   if (onScreen) {
      StrCopy(msg,"Ticks (7-8 for 20MHz) =");
      StrIToA(temp,(stopTime-startTime));
      StrCat(msg,temp);
      StrCopy(temp,". Current Depth=");
      StrCat(msg,temp);
      StrIToA(temp,ScreenDepth);
      StrCat(msg,temp);
      FrmCustomAlert(nealInfoAlertId,msg," "," ");
      //ERROR_ASSERT(msg);
   }
   
   debugPutInt("  CPU speed",rc);
   debugFuncExit("UtilDetermineCPUSpeed()"); 

   //********************************** debug
   //debugPutsError("fix this hack");
   //return cpuIs20MHz;
   //********************************** debug

   return rc;  
   
}



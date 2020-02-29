// 
// DEFS.H
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


#ifndef _DEFS_H_
#define _DEFS_H_

#include <PalmOS.h>
#include <UIPublic.h>
#include <PalmTypes.h>
#include <PalmCompatibility.h>


//--------------------------------------------------------------------
// Screen depth enum
//-------------------------------------------------------------------- 
typedef enum
{
   oneBitDepth=1,
   greyTwoBitDepth,       // 16MHz CPU OK
   greyFourBitDepth,      // Needs at least a 20MHz CPU for speed
   colorFourBitDepth
} gameScreenDepthEnum;

typedef enum
{
   normalGreyscaleLvl,
   highestGreyscaleLvl
} greyscaleLevelEnum;

//--------------------------------------------------------------------
// CPU speeds.  
//
// Currently needed only in 1 instance
//-------------------------------------------------------------------- 
typedef enum
{
   cpuUnknown=0,  // default
   cpuIs16MHz=16,
   cpuIs20MHz=20
} cpuSpeedEnum;


//-----------------------------------------------------------------------
//   BITMAP CONSTS    
//-----------------------------------------------------------------------

#define  MAXNUMBITMAPS   45

//                      .. 0-9 are digits
#define userShipBmp     11
#define fatBulletBmp    12
#define slimBulletBmp   13
#define explosBmp       14
#define twoShotAwardBmp 15
#define barrierBmp      16
#define starBmp         17
#define bug1_2          18
#define sweepBug2       19
#define bug1            20
#define sweepBug        21
#define mothership      22
#define dartBug         23
#define mineLayer       24
#define fullMotherShip  25
#define bigOafBmp       26
#define userShip2Bmp    27
#define waveSymbolBmp   28
#define mineBmp         29
#define numShipsBmp     30
#define tracBeamBmp     31
#define thrust1         32
#define thrust2         33
#define inFormBmp1      34
#define inFormBmp2      35
#define inFormBmp3      36
#define thrust3         37
#define thrust4         38
#define dotBulletBmp    39
#define star2Bmp        40

// goes up to ..45

//-----------------------------------------------------------------------
//  GAMEPLAY CONSTS
//-----------------------------------------------------------------------
#define MILLISECPERFRAME  60
#define MILLISECALLOWTEXT 40

// Number of ticks to wait in for user to restart. Must be greater than ExplosDurNormal
#define TICKSWAITFORRESTART 75

//-----------------------------------------------------------------------
//  SCREEN CONSTS
//-----------------------------------------------------------------------
#define xMaxBorder 155
#define xMinBorder 0
#define yMaxBorder 144
#define yMinBorder 0

#define TOPBORDER 0
#define RTBORDER  1
#define BOTBORDER 2
#define LFBORDER  3


//-----------------------------------------------------------------------
//  DATABASE/GAME VERSION CONSTS
//-----------------------------------------------------------------------

#define DATABASEv1           1           // associated with Astron v.80
#define DATABASEv2           2           // associated with Astron v.90-v.95
#define DATABASEv3           3           // associated with Astron v1.00 (sprite records were changed)
#define DATABASEv4           4           // associated with Astron v2.00 (greyscale options)
#define DATABASEver          DATABASEv4  // set to latest database version

// Randomize these (BUT MUST KEEP INCREASING) a little in case a hacker is trying to figure
// out the encoding
#define astronV1_00 0               // Astron v1.00 
#define astronV1_05 5               // Astron v1.05
#define astronV1_10 21              // Astron v1.10
#define astronV1_15 22              // Astron v1.15 (released?)
#define astronV2_00 43              // Astron v2.00
#define astronV3_00 44              // Astron v2.00

#define ASTRONVERSION astronV3_00   // set to latest game version

// What prevents these from being trampled when sprites are written?
#define mainGameRec1UniqId   100
#define gameInfoRecUniqId    101
#define roundInfoRecUniqId   102
#define blastRecUniqId       103
#define mainGameRec2UniqId   104   // Added in Astron v.90
#define mainGameRec3UniqId   105   // Added in Astron v1.00
#define mainGameRec4UniqId   106   // Added in Astron v2.00


//--------------------------------------------------------------------
//  USER CONSTS
//-------------------------------------------------------------------- 

#define MAXUSERBULLETS      3
#define MAXUSER2BULLETS     6

#define USERINITxPOS       75
#define USERINITyPOS      129


//-----------------------------------------------------------------------
//  SPRITE CLASSES   
//-----------------------------------------------------------------------

#define ExplosionSpriteClass  0  // 0x01
#define UserSpriteClass       1  // 0x02
#define UserBulletSpriteClass 2  // 0x04
#define BugSpriteClass        3  // 0x08
#define BugBulletSpriteClass  4  // 0x10
#define TracBeamSpriteClass   5  // 0x20
#define MineSpriteClass       6  // 0x40
#define MiscSpriteClass       7  // 0x80
#define BackDropSpriteClass   8  // 0x100

// Max num of sprite classes ( though this is not checked for yet...)
#define  spriteClassMax       9 // numbered 0..8


//--------------------------------------------------------------------
//  SPRITE COUNT CONSTS
//-------------------------------------------------------------------- 

#define MAXNUMBUGS          40  // max num bugs in 1 ROUND
#define MAXNUMUSERBULLETS    6  // was 10
#define MAXNUMEXPLOSIONS     5  // was 6
#define MAXNUMBUGBULLETS     6
#define MAXNUMTRACBEAMS      1
#define MAXNUMMINES          6 
#define MAXNUMMISCSPRITES    2
#define MAXNUMBACKSPRITES    9   //76 sprites so far

#define MAXNUMSPRITES       80    // max num sprites in ENTIRE GAME (should be > than sum of above)


Byte   firstBackDropSprite;
Byte   firstBackDropSpriteClass2;
Byte   backDropSpacing;

#define NUMSLOWBACKDROP      6

//--------------------------------------------------------------------
//  BUGS CONSTS
//--------------------------------------------------------------------

#define MAX_NUM_BUGGROUPS     6     // for wave structures
#define MAX_BUGS_PER_BUGROUP  12    // for wave structures


//--------------------------------------------------------------------
//  SIGNALS (sent from SpriteEng to application)
//-------------------------------------------------------------------- 

#define  SignalNoSignal                0
#define  SignalError                   1
#define  SignalBulletPoint             100
#define  SignalBugBackInForm           101
#define  SignalRemoveSprite            102
#define  SignalTracBeamPoint           103
#define  SignalMineLayPoint            104
#define  SignalUserWithinMothership    105
#define  SignalTracBeamExpired         106
#define  SignalBarrierMotDone          107
#define  SignalBugPeelingOffPoint      108
#define  SignalReplayTracBeamSnd       109
#define  SignalBackDropSprIsOffscreen  110

//--------------------------------------------------------------------
//  GAME CONSTS
//-------------------------------------------------------------------- 

#define firstExtraShipScore     7500
#define pointsForNextShip      10000


//--------------------------------------------------------------------
//  PASSWORD CONSTS
//-------------------------------------------------------------------- 

#define pwStringLen           8

#define minFreeGames          12  // Replaces minFreePlays & minFreeRestarts method (v1.0)
#define maxFreeGames          20  // Added in 1.10 (was 50). Reduced in v2.00
#define minFreeDays           12  // nag screen has this hardcoded too!

#define NagTimerManyPlays     6   // <15 days but >maxFreeGames

#define regPeriod1            15  // days 13-15
#define NagTimerVal1          1

#define regPeriod2            20  // days 16-20
#define NagTimerVal2          22
#define NagTimerVar2          14

#define NagTimerVal3          35  // days 21-
#define NagTimerVar3          12

//--------------------------------------------------------------------
//  Operating systems
//-------------------------------------------------------------------- 
#define PALMOS30 0x03003000
#define PALMOS31 0x03103000
#define PALMOS35 0x03503000


//--------------------------------------------------------------------
// Password stuff
//-------------------------------------------------------------------- 
typedef enum
{
  PwFailed=0,
  TempPwPassed,  
  DebugPwPassed,
  FullPwPassed,     // normal case of real password entered
  NoPWEntered       // user clicked OK without entering a password
} validatePwRC;  


//--------------------------------------------------------------------
// For conversion 1-bit to 8-bit
//-------------------------------------------------------------------- 
Byte ThrustSpriteXOffset;
Byte TracBeamXOffset;
Byte UserShipXMax;
Byte DigitSpacing;


//--------------------------------------------------------------------
//  Collision detection regions 
//-------------------------------------------------------------------- 
#define region2h 66
#define region3l 78
#define region1h 118 
#define region2l 130


//--------------------------------------------------------------------
//  Star speed 
//-------------------------------------------------------------------- 
Byte slowStarSpeed;
Byte fastStarSpeed;



//--------------------------------------------------------------------
//  CPU Level Restrictions
//-------------------------------------------------------------------- 

typedef enum
{
   CPULevelOK=0,
   CPULevelLow,
   CPULevelMed,       
   CPULevelHigh,     // Restrict double shots, mine layer, 
} CPULevelEnum;

CPULevelEnum UtilCPULevel;
UInt16       UtilCPURestrictions;


// Defined limits to restrict game events which tend to kill cpu (cause huge realtime hit)
#define CPULimitForMineLayer      CPULevelMed

// Define limit to restrict bug flights
#define CPULimitForBugFlight      CPULevelMed
#define CPULimitForBugsDiving     3
#define CPUTicksToDelayBugFlight  2
 
// Hasten explosion removal under tight CPU conditions
#define CPULimitForLongExplos     CPULevelMed
#define ExplosDurNormalBW         10 
#define ExplosDurNormalColor      5  // new in v1.10
#define ExplosDurCPULimit         2


#endif

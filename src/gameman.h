//
//  GAME Manager ROUTINES
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

#ifndef _GAMEMAN_H_
#define _GAMEMAN_H_

#include <PalmOS.h>
#include <UIPublic.h>
#include <PalmTypes.h>
#include <PalmCompatibility.h>

#include "wave.h"
#include "blast.h"
#include "sprite.h"
#include "behav.h"
#include "defs.h"


//-----------------------------------------------------------------------
//                    Enums
//-----------------------------------------------------------------------

// Type of round
typedef enum
{
   regularRound,           // normal game play
   bonusRound,             // bugs fly for points  not coded yet****
   blitzRound              // not coded yet****
} roundTypeEnum;


//
//   Game state
//
typedef enum
{
   before1stRound,
   setupNextIntro,
   betweenRounds,
   introWaves,
   waitForFormation,
   gamePlay,
   waitForEnemyDivingOver,
   reintroducePlayer,
   restAfterGamePlay, 
   gameOver,
   waitForGameRestart,
   blah  // test state
} gameStateEnum;


//
//  Bug state
//
typedef enum
{
   dead,              // bug is dead (used == false, also)
   inFormation,       // bug is in formation
   diving,            // bug is alive, and diving (regular game play)
   doingIntro,        // bug doing intro
   barrierShudder,    // (barriers only)... doing shudder motion
   notMovable         // (barriers only currently)
} bugStateEnum;


//
//  User state
//
typedef enum
{
  user_normal,            // normal game play -- let player control ship
  user_dead,              // user ship has been killed  (stop sending bugs)
  user_inTracBeam         // user ship being hauled up in trac beam
} userStateEnum;


//-----------------------------------------------------------------------
//                   Game and Round Structures
//-----------------------------------------------------------------------

//
//  This struct gives the variances from round-to-round
//

typedef struct
{
  Int   maxBugsInFlight;             // max num bugs flying at once (normal play)
  Int   maxBulletsInIntroWaves;      // max num bullets during intro wave
  Int   maxBulletsInNormalPlay;      // max num bullets during normal play 
  Int   maxBulletsInBlitzMode;       // max num bullets during end-of-round blitz mode
  Int   maxMinesInNormalPlay;
  Int   numBugsForBlitzMode;         // num bugs left to launch end-of-round blitz mode
  Int   numBugsCanPeelOff;           // num bugs that can peel off intro waves
  Int   maxTracBeams;
  Int   bulletSpeed;                 // bullet speed for bugs
  Int   bulletCeiling;               // bullets must fire above this Y
  Int   mineCeiling;                 
  Int   keepCourseCeiling;
  Int   aimAtPlayerVariance;         // how close to aim for player
  Int   bugXSpeedIntro;              // bug x,y speed during intro and bonus waves
  Int   bugYSpeedIntro; 
  Int   slowXSpeed;                  // bug x,y speeds during regular play
  Int   medXSpeed;
  Int   highXSpeed;
  Int   slowYSpeed;
  Int   medYSpeed;
  Int   highYSpeed;
  Int   minTicksBetwBugs;             // ticks between launching bugs during normal play
  Int   percChanceMothershipBeam;    // % chance of mothership tractor beam
  Int   percChanceShotIntroWave;     // % chance of shot -- intro wave  
  Int   percChanceShotNormalPlay;    // % chance of shot -- normal play   (blitz hardcoded to 75%)
  Int   maxTimeBetwIntroWaves;      // used to start next fly in wave if all bugs destroyed
  Int   timeBefore1stBugDive;     // time to wait after formation has formed
  Int   peelOffXSpeed;               // x,y 
  Int   peelOffYSpeed;
} roundParamStruct;



//
//  Bug record (1 per each bug in the round)
//
typedef struct
{
   Int           spriteNum;            // index into allSprites *** DO NOT MOVE, READ in RESTORE **
   Int           bugType;              // bug type (bug1, mothership, etc.)
   Int           bugGroup;             // which bug group we are 
   Int           bugGroupIndex;        // num within list
   bugStateEnum  aliveState;
   Int           numFlybacksAllowed;   // for bugs which do flybacks: num of flybacks allowed (0=none)
   Int           numHitsToKill;        // normally 1 bullet to kill
   Int           origX;                // (x,y) original position in formation
   Int           origY;
   Byte          motionIndex;          // index into motion sequence
} bugInfoStruct;


//
//  Game manager data for overall game
//
typedef struct
{
  UInt32         score;             // players score
  Int            numShips;          // number of user ships
  Int            round;             // which round
  roundTypeEnum  roundType;         // round type
  gameStateEnum  gameState;         // game state 
  gameStateEnum  savedGameState;    // saves gameState when (1) in paused, (2) user has died
  userStateEnum  userState;         // user state
  Boolean        twoShots;          // user has double fire
  UInt8          debugTimesHit;
  UInt32         nextExtraShipScore; // next score when a bonus man will be awarded

} gameInfoStruct;


typedef struct
{   
   Int             currentWave;           // current intro wave flying in
   Int             numTicks;              // game tick at which time a bug will fly
   Int             numStartBugs;          // number of bugs which start the round
   Int             numBarriers;           // num of barriers in round 
   Int             numRemainBugs;         // number of bugs left alive
   Int             numBugsDiving;         // num bugs on screen, not in formation
   Int             numBugsPeeledOff;      // num bugs peeled off in intro
   Int             numBulletsFlying;      // num bug bullets flying
   Int             numMinesFlying;        // num mines dropping
   Byte            numTracBeamsTried;
   Byte            numUserBulletsFlying;  // num user bullets flying
   Boolean         inBlitzMode;           // in end-of-round blitz mode

 
   Int             currBugGroup;
   Int             numBugGroups;
   Int             bugGroupCount[MAX_NUM_BUGGROUPS];  // count of num bugs in a particular group
   Int             bugGroup[MAX_NUM_BUGGROUPS][MAX_BUGS_PER_BUGROUP];           
                                                      // index to the bugs list (buggroup,bug)

   bugInfoStruct   bugs [ MAXNUMBUGS ]; // all the bugs

   moveStruct      ms;                        // Movestruct used to control bugs in formation
   Int             offsetFromStartPos;        // delta X offset from bugs init position
   Int             tracBeamOwnerSpriteNum;     // trac beam owner (-1 = none)
   Int             tracBeamSpriteNum;          // trac beam sprite (-1 = none)
   Int             bonus2ShotAwardSpriteNum;   // 2-shot bonus sprite (-1 = none)

} roundInfoStruct;


// .... probably could go in sprite.h
typedef struct
{
   sprite         savedSpriteStruct;
   UInt           nextRecIndex;      // used to retrieve next sprite
   ULong          nextUniqId;
   UInt           junk;    // filler
} spriteSaveStruct;


//-----------------------------------------------------------------------
//                   Vars
//-----------------------------------------------------------------------

gameInfoStruct      gameInfo;
roundInfoStruct     roundInfo;
roundParamStruct    rps;

Boolean             skipIntroFlyIns;

//-----------------------------------------------------------------------
//                    Methods
//-----------------------------------------------------------------------

void  startOfFrame() SEC5;
void  GameManagerInit() SEC3; 
void  cleanupFromPrevRound() SEC3;
void  loadNewRoundInfo() SEC4;
void  loadNewRoundPercs() SEC3;
void  loadNewRoundText() SEC3;
void  debugSkipIntroWaves() SEC5; 
void  debugDumpRoundInfoNonBugs() SEC3;
void  dummyProc(Int,Int) SEC5;

//-----------------------------------------------------------------------
//                   Game state routines
//-----------------------------------------------------------------------

void  doWave(Int) SEC4;
void  HandleGameProgress()SEC2;
void  HdlGameState_Before1stRound()SEC3;
void  HdlGameState_setupNextIntro() SEC4;
void  HdlGameState_BetweenRounds() SEC4;
void  HdlGameState_IntroWave() SEC2;
void  HdlGameState_WaitForFormation() SEC2;
void  HdlGameState_GamePlay() SEC4;
void  HdlGameState_WaitForEnemyDivingOver() SEC4;
void  HdlGameState_ReintroducePlayer() SEC4;
void  HdlGameState_blahPlay() SEC4;
void  HdlGameState_RestAfterGamePlay() SEC4;
void  HdlGameState_Paused() SEC4;
void  HdlGameState_GameOver() SEC4;
void  HdlGameState_WaitForGameRestart() SEC4;




//-----------------------------------------------------------------------
//   Game save/restore routines
//-----------------------------------------------------------------------
Boolean SaveGamemanRecs() SEC4;
Boolean RestoreGamemanRecs() SEC4;

#endif

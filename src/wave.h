//
//  WAVE.H
//
//  This file defines the characteristics of the wave -- intro flights 
//  included.  Behavior of the bugs is NOT in this file (see behav.h)
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
 
#ifndef _WAVE_H_
#define _WAVE_H_

#include "blast.h"
#include "behav.h"


//-----------------------------------------------------------------------
//                    Defines
//-----------------------------------------------------------------------

#define MAX_NUM_SUBINFOs 12
#define MAX_NUM_WAVES    8


//-----------------------------------------------------------------------
//                     Enums/Structs
//-----------------------------------------------------------------------

//
//  Define the bugs in this wave, set final position
//
typedef struct
{
   Int  bugGroup;
   Int  firstSpriteNum;
   Int  lastSpriteNum;
   Int  spriteType;     // bug1, mothership, dartBug (bitmap?)
   Int  firstMoveIndex;
   Int  initX;          // start position of row -- x coordinate
   Int  initY;          // start position of row -- y coordinate
   Int  deltaXspace;    // x spacing on the row
} roundSubInfoStruct;


//
//  Defines how the sprites fly the intro wave
//
typedef struct
{
   Int     numSprites;          // Number of sprites in this wave
   Int     initX;               // Initial x, y position to start at
   Int     initY;
   Int     theseSprites[10];    // List of indiv sprites
   Int     numMovements;        // Num of movement rules
   Int     movements[4];        // The movment rules (iterated 1 for each sprite)
} introWaveStruct;


// 
//  Defines all the waves in a round
// 
typedef struct
{
   Int                delayBetwWaves[MAX_NUM_WAVES];
  //introWaveStruct  * wavePtr[6]; ... waves[] should actually go here
  // ... wavesCount should actually go here
} roundStruct;



//-----------------------------------------------------------------------
//                    Vars
//-----------------------------------------------------------------------

roundSubInfoStruct  roundSubInfo[ MAX_NUM_SUBINFOs ];
Int                 roundSubInfoCount;

introWaveStruct     waves[ MAX_NUM_WAVES ];
Int                 wavesCount;

roundStruct         thisRound;


//-----------------------------------------------------------------------
//                     Procs
//-----------------------------------------------------------------------
void setRoundVars ( 

   Int                iwsCnt,
   introWaveStruct    *i1,
   introWaveStruct    *i2,
   introWaveStruct    *i3,
   introWaveStruct    *i4,
   introWaveStruct    *i5,
   introWaveStruct    *i6,
   introWaveStruct    *i7,
   introWaveStruct    *i8,
   roundStruct        *r
) SEC1;


void addRoundPosInfo(
   Int  bugGroup,
   Int  firstSpriteNum,
   Int  lastSpriteNum,
   Int  spriteType, 
   Int  firstMoveIndex,
   Int  initX,   
   Int  initY,        
   Int  deltaXspace
) SEC1;



//-----------------------------------------------------------------------
//                     Waves
//-----------------------------------------------------------------------

void setupRegWaveA() SEC1;
void setupRegWaveB() SEC1;
void setupRegWaveD() SEC1;
void setupRegWaveC() SEC2;
void setupRegWaveE() SEC2;
void setupRegWaveF() SEC2;
void setupRegWaveG() SEC2;

#endif

//
//  WAVE.C
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

#include "gameman.h"
#include "sprite.h"
#include "blast.h"
#include "debug.h"
#include "graphics.h"
#include "wave.h" 
#include "starter.h"

//-----------------------------------------------------------------------
//  Copy local vars to round structure
//-----------------------------------------------------------------------
roundSubInfoStruct  roundSubInfo[ MAX_NUM_SUBINFOs ];
Int                 roundSubInfoCount=0;

introWaveStruct     waves[ MAX_NUM_WAVES ];
Int                 wavesCount=0;

roundStruct         thisRound;

//-----------------------------------------------------------------------
//  Spacing
//     For 8 bugs... 160 = 24+(8*14)+24
//-----------------------------------------------------------------------

#define XINIT  24
#define YSPACE 12
#define XHALF  6    // XSPACE divided by 2

#define XSPACE 14
#define YROW1 6
#define YROW2 18
#define YROW3 30
#define YROW4 42
#define YROW5 54


//-----------------------------------------------------------------------
//  Copy local vars to round structure
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
)
{

   debugFuncEnter("setRoundVars()");

   if ((i1==0) || (i2==0) || (i3==0) || (i4==0) ||
       (i5==0) || (i6==0) || (i7==0) || (i8==0) || (r==0))
   {
      debugPuts("**** error,  NULL i1-i8 or r passed to setRoundVars() ****");
      debugFuncExit("For()");
      return;
   }

   // Handle wave structs
   wavesCount = iwsCnt;
   waves[0] = *i1; waves[1] = *i2; waves[2] = *i3; waves[3] = *i4;
   waves[4] = *i5; waves[5] = *i6; waves[6] = *i7; waves[7] = *i8;

   thisRound = *r;

   got_here;

   debugFuncExit("For()");
}


//-----------------------------------------------------------------------
//   
//-----------------------------------------------------------------------
void addRoundPosInfo(
   Int  bugGroup,
   Int  firstSpriteNum,
   Int  lastSpriteNum,
   Int  spriteType, 
   Int  firstMoveIndex,
   Int  initX,   
   Int  initY,        
   Int  deltaXspace  
)
{
  debugFuncEnterInt("addRoundPosInfo(), roundSubInfoCount",roundSubInfoCount);

  roundSubInfo[roundSubInfoCount].bugGroup = bugGroup;
  roundSubInfo[roundSubInfoCount].firstSpriteNum = firstSpriteNum;
  roundSubInfo[roundSubInfoCount].lastSpriteNum = lastSpriteNum;
  roundSubInfo[roundSubInfoCount].spriteType = spriteType;
  roundSubInfo[roundSubInfoCount].firstMoveIndex = firstMoveIndex;
  roundSubInfo[roundSubInfoCount].initX = initX;
  roundSubInfo[roundSubInfoCount].initY = initY;
  roundSubInfo[roundSubInfoCount].deltaXspace = deltaXspace;

  roundSubInfoCount++;

  if (roundSubInfoCount >= MAX_NUM_SUBINFOs) {
    debugPutsError("Too many waves defined, overflow error");
  }

  if (lastSpriteNum >= MAXNUMBUGS) {
    debugPutsError("Too many bugs defined, overflow error");
  }
  
   debugFuncExit("addRoundPosInfo()");
}


//-----------------------------------------------------------------------
//    Wave A  -- Conventional formation (4 rows)
//-----------------------------------------------------------------------
void setupRegWaveA()
{
   Int b;

   introWaveStruct g =  { 7, 70,0, {0,1,8,9,16,17,24},   1, {fltB1} };
   introWaveStruct h =  { 7, 70,0, {2,3,10,11,18,19,25}, 1, {fltB2} };
   introWaveStruct i =  { 7, 70,0, {4,5,12,13,20,21,26}, 1, {fltB3} };
   introWaveStruct j =  { 7, 70,0, {6,7,14,15,22,23,27}, 1, {fltB4} };

   // delay betw waves, wave ptr
   roundStruct     r = { {30,75,80,75,-1,-1} }; 

   // top 2 middle bugs are changeable
   b = (gameInfo.round==1) ? sweepBug : mothership;

   addRoundPosInfo (0,24,25,mineLayer, 2,XINIT+2*XSPACE,YROW1,3*XSPACE);
   addRoundPosInfo (0,26,27,b,         0,XINIT+3*XSPACE,YROW1,XSPACE);
   addRoundPosInfo (1,16,23,sweepBug,  0,XINIT,YROW2,XSPACE); 
   addRoundPosInfo (2,8,15,bug1,       2,XINIT,YROW3,XSPACE);
   addRoundPosInfo (3,0,7,dartBug,     2,XINIT,YROW4,XSPACE);
 
   debugFuncEnter( "setupRegWaveA()" );
   setRoundVars( 4,&g,&h,&i,&j,&j,&j,&j,&j, &r);
   gameInfo.roundType = regularRound;

   got_here;
   debugFuncExit("Wave()");
} 



//-----------------------------------------------------------------------
//    Wave B  -- Sprites move in from either side in 4 rows
//-----------------------------------------------------------------------
void setupRegWaveB()
{
   // delay betw waves, wave ptr
   roundStruct r={ {20,10,10,10,-1,-1} };

   introWaveStruct g= { 8, 160,10, {6,0,1,2,3,4,5,7},         1, {lfThenStop} };  // row 1
   introWaveStruct i= { 8, 0,30,   {16,17,18,19,20,21,22,23}, 1, {rtThenStop} };  // row 2
   introWaveStruct h= { 8, 160,20, {8,9,10,11,12,13,14,15},   1, {lfThenStop} };  // row 3
   introWaveStruct j= { 8, 0,40,   {24,25,26,27,28,29,30,31}, 1, {rtThenStop} };  // row 4

   addRoundPosInfo (0,0,5  ,sweepBug,    2,XINIT+XSPACE,YROW1,XSPACE);
   addRoundPosInfo (0,6,7  ,mothership,  2,XINIT,YROW1,XSPACE+6*XSPACE);
   addRoundPosInfo (1,16,23,sweepBug,    2,XINIT,YROW2,XSPACE);
   addRoundPosInfo (2,8,15 ,bug1,        2,XINIT,YROW3,XSPACE);
   addRoundPosInfo (3,24,31,dartBug,     2,XINIT,YROW4,XSPACE);

   debugFuncEnter( "setupRegWaveB()" );
   setRoundVars( 4,&g,&i,&h,&j,&j,&j,&j,&j, &r);
   gameInfo.roundType = regularRound;

   debugFuncExit("setupRegWaveB()");
} 


//-----------------------------------------------------------------------
//    Wave D  -- inverted pyramid in 4 waves
//-----------------------------------------------------------------------
void setupRegWaveD()
{
   roundStruct r ={ {10,5,5,5,5,-1} };

   introWaveStruct g = { 2, 72,0,   {26,27},          1, {drpThenStop} };
   introWaveStruct h = { 4, 64,-12, {22,23,24,25},           1, {drpThenStop} };
   introWaveStruct i = { 6, 50,-24, {16,17,18,19,20,21},     1, {drpThenStop} };
   introWaveStruct j = { 8, 36,-36, {8,9,10,11,12,13,14,15}, 1, {drpThenStop} };
   introWaveStruct k = { 8, 36,-48, {0,1,2,3,4,5,6,7},       1, {drpThenStop} };

   addRoundPosInfo (0,26,27,  dartBug,          2,XINIT+3*XSPACE,YROW5,XSPACE);
   addRoundPosInfo (1,22,25, dartBug,       2,XINIT+2*XSPACE,YROW4,XSPACE);
   addRoundPosInfo (2,16,21,bug1,      2,XINIT+XSPACE,YROW3,XSPACE);
   addRoundPosInfo (3,8,15,sweepBug,          2,XINIT,YROW2,XSPACE);
   addRoundPosInfo (4,0,7,mothership,           2,XINIT,YROW1,XSPACE);

   // debug
   debugFuncEnter( "setupRegWaveD()" );

   // Load up the van
   setRoundVars( 5,&g,&h,&i,&j,&k,&j,&j,&j, &r);
   gameInfo.roundType = regularRound;

   debugFuncExit("setupRegWaveD()");
} 

//-----------------------------------------------------------------------
//    Wave C (Conventional formation, 5 rows)
//-----------------------------------------------------------------------
void setupRegWaveC()
{
   roundStruct     r = { {30,100,100,100,100,-1} }; 
  
   introWaveStruct g =  { 7, 70,0, {0,1,8,9,16,17,24},        1, {fltF2} };
   introWaveStruct h =  { 7, 70,0, {2,3,10,11,18,19,25},      1, {fltF3} };
   introWaveStruct i =  { 7, 70,0, {4,5,12,13,20,21,26},      1, {fltF1} };
   introWaveStruct j =  { 7, 70,0, {28,29,30,31,32,33,34},    1, {fltF4} };
   introWaveStruct k =  { 8, 70,0, {6,7,14,15,22,23,27,35},   2, {fltF2,fltF3} };

   addRoundPosInfo (0,24,25,mineLayer,  2,XINIT+2*XSPACE,YROW1,3*XSPACE);
   addRoundPosInfo (0,26,27,mothership, 2,XINIT+3*XSPACE,YROW1,XSPACE);
   addRoundPosInfo (1,16,23,sweepBug,   2,XINIT,YROW2,XSPACE); 
   addRoundPosInfo (2,8,15,sweepBug,    2,XINIT,YROW3,XSPACE);
   addRoundPosInfo (3,28,35,bug1,       2,XINIT,YROW4,XSPACE);
   addRoundPosInfo (4,0,7,dartBug,      2,XINIT,YROW5,XSPACE);

   debugFuncEnter( "introWaveC()" );
   setRoundVars( 5,&g,&h,&i,&j,&k,&j,&j,&j, &r);

   debugFuncExit("Wave()");
} 

//-----------------------------------------------------------------------
//    Wave E -- Conventional formation (4 rows) with barriers
//-----------------------------------------------------------------------
void setupRegWaveE()
{
   // delay betw waves, wave ptr
   roundStruct     r = { {30,65,65,65,65,-1} }; 

   introWaveStruct g =  { 7, 70,0, {28,29,8,9,16,17,24},   2, {fltE1,fltE2} };
   introWaveStruct h =  { 7, 70,0, {30,31,10,11,18,19,25}, 1, {fltE2} };
   introWaveStruct i =  { 7, 70,0, {32,33,12,13,20,21,26}, 2, {fltE2,fltE1} };
   introWaveStruct j =  { 7, 70,0, {34,35,14,15,22,23,27}, 1, {fltE1} };
   introWaveStruct k =  { 8, 70,0, {0,1,2,3,4,5,6,7}, 1, {putInForm} };


   addRoundPosInfo (0,24,27,mineLayer, 2,XINIT+2*XSPACE,YROW1,XSPACE);
   addRoundPosInfo (1,16,23,sweepBug,  2,XINIT,YROW2,XSPACE); 
   addRoundPosInfo (2,8,15,dartBug,    2,XINIT,YROW3,XSPACE);
   addRoundPosInfo (4,28,35,dartBug,   2,XINIT,YROW4,XSPACE);
   addRoundPosInfo (3,0,7,barrierBmp,  2,XINIT,YROW5,XSPACE);

   debugFuncEnter( "setupRegWaveE()" );
   setRoundVars( 5,&g,&h,&i,&j,&k,&k,&k,&k, &r);
   gameInfo.roundType = regularRound;
   
   debugFuncExit("Wave()");
} 



//-----------------------------------------------------------------------
//    Wave F  -- 2 inverted pyramids (4 bugs at base)
//-----------------------------------------------------------------------
void setupRegWaveF()
{

   introWaveStruct g =  { 8, 70,0, {0,1,2,3,4,5,6,7},         2, {fltE1,fltE2} };
   introWaveStruct h =  { 8, 70,0, {8,9,10,11,12,13,14,15},   1, {fltE2} };
   introWaveStruct i =  { 4, 70,0, {16,17,18,19},             2, {fltE2,fltE1} };
 
   roundStruct     r = { {30,100,100,100,-1,-1} }; 

   debugFuncEnter( "setupRegWaveF()" );
   addRoundPosInfo (0,0,7,   bug1,        2,XINIT,         YROW1,XSPACE);
   addRoundPosInfo (1,8,10,  dartBug,     2,XINIT+XHALF,   YROW2,XSPACE);
   addRoundPosInfo (1,11,13, bug1,        2,XINIT+XHALF+4*XSPACE,YROW2,XSPACE);
   addRoundPosInfo (2,14,15, dartBug,     2,XINIT+XSPACE,YROW3,XSPACE);
   addRoundPosInfo (2,16,17, bug1,        2,XINIT+5*XSPACE,YROW3,XSPACE);
   addRoundPosInfo (3,18,19, dartBug,     2,XINIT+XHALF+XSPACE,YROW4,4*XSPACE);
 
   // Load up the van
   setRoundVars( 3,&g,&h,&i,&i,&i,&i,&i,&i, &r);
   gameInfo.roundType = regularRound;

   debugFuncExit("Wave()");
} 


//-----------------------------------------------------------------------
//    Wave G  -- 2 inverted pyramids + diamond (4 bugs at base)
//-----------------------------------------------------------------------
void setupRegWaveG()
{

   introWaveStruct g =  { 8, 70,0, {0,1,2,3,4,5,6,7},         2, {fltE1,fltE2} };
   introWaveStruct h =  { 8, 70,0, {8,9,10,11,12,13,14,15},   1, {fltE2} };
   introWaveStruct i =  { 8, 70,0, {16,17,18,19,20,21,22,23}, 2, {fltE2,fltE1} };
   //introWaveStruct j =  { 8, 70,0, {24,25,26,27,28,29,30,31}, 1, {fltE1} };
 
   roundStruct     r = { {30,100,100,100,-1,-1} }; 

   debugFuncEnter( "setupRegWaveG()" );
   addRoundPosInfo (0,0,7,   bug1,       2,XINIT,         YROW1,XSPACE);   // row1
   addRoundPosInfo (1,8,10,  dartBug,    2,XINIT+XHALF,   YROW2,XSPACE);
   addRoundPosInfo (1,11,13,  bug1,      2,XINIT+XHALF+4*XSPACE,YROW2,XSPACE);  // row2
   addRoundPosInfo (2,14,15, dartBug,     2,XINIT+XSPACE,YROW3,XSPACE);         // row 3
   addRoundPosInfo (2,16,17,  bug1,        2,XINIT+5*XSPACE,YROW3,XSPACE);

   addRoundPosInfo (3,18,19, dartBug,    2,XINIT+XHALF+XSPACE,YROW4,4*XSPACE);  // row 4

   addRoundPosInfo (3,20,21, sweepBug,     2,XINIT+3*XSPACE,YROW4,XSPACE);

   addRoundPosInfo (4,22,22,  bug1,        2,XINIT+XHALF+3*XSPACE,YROW3,XSPACE);
   addRoundPosInfo (4,23,23, dartBug,    2,XINIT+XHALF+3*XSPACE,YROW5,XSPACE);


   // Load up the van
   setRoundVars( 3,&g,&h,&i,&i,&i,&i,&i,&i, &r);
   gameInfo.roundType = regularRound;

   debugFuncExit("Wave()");
} 






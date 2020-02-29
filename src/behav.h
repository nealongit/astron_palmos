//
//  Sprite behavior ROUTINES
//
//  This file defines how the bugs (sprites) behave
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

#ifndef _BEHAV_H_
#define _BEHAV_H_

#include <PalmOS.h>
#include <UIPublic.h>
#include <PalmTypes.h>
#include <PalmCompatibility.h>

#include "sprite.h"
#include "starter.h" // to access SEC1 macro
#include "defs.h"



//-----------------------------------------------------------------------
// Intro movements constants
//-----------------------------------------------------------------------
#define dropThenDiagLf      2
#define dropThenDiagRt      3
#define lfThenStop          4
#define rtThenStop          5
#define drpThenStop         6

#define putInForm           7


#define fltB1  23
#define fltB2  24
#define fltB3  25
#define fltB4  26

#define fltE1  31
#define fltE2  32

#define fltF1  27
#define fltF2  28
#define fltF3  29
#define fltF4  30


//-----------------------------------------------------------------------
//                   
//-----------------------------------------------------------------------

typedef struct
{
  Int16   x;
  Int16  y;
  Int16  signal;
} flightPtStruct; 

// replace 8 later...
flightPtStruct ifb1[8];
flightPtStruct ifb2[8];
flightPtStruct ifb3[8];
flightPtStruct ifb4[8];

flightPtStruct ifc1[8];

flightPtStruct ife1[8];
flightPtStruct ife2[8];

flightPtStruct iff1[8];
flightPtStruct iff2[8];
flightPtStruct iff3[8];
flightPtStruct iff4[8];



//-----------------------------------------------------------------------
//                    Bug motion/behavior encoders
//-----------------------------------------------------------------------

void setBug1Motion         (sprite *s) SEC1;
void setDartBugMotion      (sprite *s) SEC1;
void setMothershipMotion   (sprite *s,Boolean) SEC1;
void setMineLayerMotion    (sprite *s) SEC1;
void setSweepBugMotion     (sprite *s) SEC1;
void setBigOafMotion       (sprite *s) SEC1;

void setMineMotion         (sprite *s, sprite * motherShip) SEC1;
Int setBonus2ShotMotion   (sprite * motherShip) SEC1;

void setTracBeamMotion     (sprite *s, sprite * motherShip) SEC1;

void encodeFlightpath (
   sprite            * s, 
   Int               numInLine,
   flightPtStruct  * thisFlt,
   Int,
   Int
);

//-----------------------------------------------------------------------
//                    Other motion encoders
//-----------------------------------------------------------------------
void showExplosion         (sprite * hitSprite);
void setSuccessMothershipMotion (sprite *s) SEC1;
void setBugPeelingOffMotion(sprite *s) SEC1;
void setUserInTracBeamMotion(
   sprite * userSprite,
   Int      dist
) SEC1;

void setStarMotion(sprite * s);
void setBlitzMotion(sprite * s);
void setBarrierGotHitMotion( sprite * s ) SEC1;
void setBugFlybackMotion (sprite *s) SEC1;



//-----------------------------------------------------------------------
//                    Intro motion encoders
//-----------------------------------------------------------------------


void setDropMotion(
   sprite * s, 
   Int      speed
) SEC1;

void setDropThenDiag( 
  sprite * s, 
  Int      numInLine,
  Boolean  diagRt
) SEC1;

void dirThenStop( 
  sprite * s, 
  Int      numInLine,
  Int      totalNumSprites,
  Int      targX,
  Int      targY,
  Int      deltaX,
  Int      deltaY
) SEC1;

void setIntroFlight10( 
  sprite * s, 
  Int      numInLine,
  Int      totalNumBugs,
  Int      flavor
) SEC1;

void setIntroFlight11( 
  sprite * s, 
  Int      numInLine,
  Int      totalNumBugs,
  Int      flavor
) SEC1;


//-----------------------------------------------------------------------
//                    Leg encoders
//-----------------------------------------------------------------------

moveStruct * setSpriteForDynamicMovement(
  sprite * s
)  SEC6;

void encodeMoveToSpotLeg(
  moveStruct *  ms,
  Int           leg,
  Int           duration,
  Int           sendThisEvent,
  Int           deltaX,
  Int           deltaY,
  Int           targX,
  Int           targY
) SEC6;


void encodeStaticLeg(
  moveStruct *  ms,
  Int           leg,
  Int           duration,
  Int           sendThisEvent,
  Int           deltaX,
  Int           deltaY
) SEC1;

void encodeHomeToLeg(
  moveStruct *  ms,
  Int           leg,
  Int           duration,
  Int           sendThisEvent,
  Int           deltaX,
  Int           deltaY
) SEC6;

void encodeHideFromView(
  moveStruct *  ms,
  Int           leg,
  Int           duration,
  Int           restoreX,
  Int           restoreY
) SEC6;

void encodeNowInFormation(
  moveStruct *  ms,
  Int           leg
) SEC6;


void encodeBugAllDone(
  moveStruct *  ms,
  Int           leg
) SEC6;




#endif

//
//  SPRITE ROUTINES
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

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <PalmCompatibility.h>   // Required for Palm OS 3.5

#include "debug.h"
#include "starter.h"  // for SEC2 macro only
#include "defs.h"  // for MAXNUMSPRITES

//-----------------------------------------------------------------------
//                       Defines
//-----------------------------------------------------------------------

#define startOfClassList -2
#define endOfClassList   -1



#define isActive(s)  (s->usedState == true)
#define notActive(s) (s->usedState == false)

#define AddToActiveList(s) {if (notActive(s)) {s->usedState=true; \
                            s->collisionRegion=regionUnknown; \
                            s->realX12calc=false; \
                            s->realCogXcalc=false; \
                            s->realY12calc=false; \
                            s->realCogYcalc=false; \
                            spriteList[s->spriteType].numActive++;}}

#define DelFromActiveList(s) {if(isActive(s)) {s->usedState=false; \
                            spriteList[s->spriteType].numActive--;}}


//-----------------------------------------------------------------------
//                       Enums
//-----------------------------------------------------------------------

// Action to take when sprite exceeds a viewing border
typedef enum
{
   noAction,
   wrap,
   sendSignal,
   bounce,           // send sprite in opposite direction
   nextLeg,       // NOT IMPLEMENTED
   bounceIncr2    // bounce, but add 2 to moveStruct (only for x right now)

} borderActionEnum;


typedef enum
{
   regionNone=0,
   region1__=1,
   region_2_=2,
   region12_=3,
   region__3=4,
   region1_3=5,
   region_23=6,
   regionUnknown=7      // collision region not set -- important that =7 (to trip all collision detection)
} collisionRegionEnum;

// Make this more generic later..



//-----------------------------------------------------------------------
//                     Movement Structures
//-----------------------------------------------------------------------



//
// Movement leg types 
//
#define staticLeg           1   
#define homeToLeg           2   
#define staticLegYIncr      3  // bullets going down..
#define staticLegYDecr      4  // bullets going up..
#define hideFromView        5
#define noMovement          6  // used for explosions
#define resetPos            7


//
// Individual leg element
//
   //
   //  staticLeg -- used to fly straight lines for a set duration 
   //

   //
   //  hideFromView -- hide the sprite for a specified delay (delay=duration)
   //

typedef struct{

   // Common parts
   Int legType;
   Int duration;
   Int sendThisEvent;

   union {

      struct
      {
	 Int deltaX;
	 Int deltaY;
      } staticLegStruct;
           
      struct
      {
	 Int restoreX;        // X,Y to restore when delay is done
	 Int restoreY;        
      } hideFromViewStruct; 

      struct 
      {
	 UInt8   currSequence;
	 UInt8   timeBetwSeqFrames;
	 UInt16  restoreBitmap;
      } inFormSeqStruct;
      

   } guts; // union

} moveElement;





#define MAXLEGS 8


// Defines who owns the movement structure
typedef enum
{
   sprEng=0,         // SprEng is responsible doing the countdown, and advancing to next leg
   noMove,
   userAppl0,        // User application is responsible -- normal formation
   userAppl1,        // User application is responsible -- reappearing
   userAppl2,
   userAppl3,
   userAppl4,
   userAppl5,
   userAppl6
} moveStructOwnerEnum;

//
// MoveStruct -- completely codes the movement of a sprite
//
typedef struct
{
   Int            currentLeg;
   Int            countdown;      
   Int            sendThisEvent;  // event sent when countdown=0
   moveElement    leg[MAXLEGS];
} moveStruct;


//-----------------------------------------------------------------------
//                    Generic Structures
//-----------------------------------------------------------------------

//
// Bitmap motion sequence
//
typedef struct
{
   Int currentMotionIndex;     // changes each round --  
   Int currentTickCount;       // changes each round --
   Int ticksBetwMotionFrames;  // 
   Int maxMotionBitmaps;       // count of next array (0 signifies mps not in use)
   Int motionBitmaps[8];
} motionBitmapStruct;



//-----------------------------------------------------------------------
//                Individual sprite structures
//-----------------------------------------------------------------------


//
// Individual sprite data
//
typedef struct 
{ 
   UInt16             spriteNum;      // 
   UInt16             spriteType;     // 

   //----- DO NOT MOVE x,y, READ DIRECTLY BY ASM ROUTINE ----- 
   Int16              x;         // Current x,y coordinates (must allow negative numbers)
   Int16              y;         

   UInt16              bitmap;                                  
   Boolean usedState; // TRUE=display, FALSE=don't display
   moveStruct          myMoveStruct;            // ... ALL MOVE INFO IS HERE
   moveStructOwnerEnum moveStructOwner;
   borderActionEnum    borderAction[4];      // Action to take when sprite moves across a viewing border

   // "Private" fields..
   // (dynamic) fields for collision detection
   Boolean realCogXcalc;    // Flags to see if the fields below are valid
   Boolean realCogYcalc;
   Boolean realX12calc;
   Boolean realY12calc;
   Int16   realCogX;        // Adjusted COGs
   Int16   realCogY;
   Int16   realX1;          // Adjusted x1,x2,y1,y2
   Int16   realY1;
   Int16   realX2;
   Int16   realY2;

   // Trial...
   collisionRegionEnum collisionRegion;
   

   // "Private" fields..
   // This used to be a linked list ptr, but that made save/restore complicated
   // so we store the spriteNum of the next sprite.  -1 == no more in chain
   // This index never changes (regardless of active state)
   Int16  nextSprite;  
  

}  sprite;



//
// Constructor data
//
typedef struct
{
   Boolean            used;          // UNUSED!!!
   Int                spriteNum;
   Int                spriteType;
   Int                bitmap;
   Int                x;
   Int                y;
   Int                deltaX;
   Int                deltaY;
   borderActionEnum * borderAction;
   Int                bitmapMotionSeqIndex;   
   Int                bitmapMotionSeqOffset;

}  NewSpriteInfo;


//
// Info for sprite engine
//


//

typedef enum
{
   fourCorners=0,  // Overlapping squares checking
   centerOfGrav,   // Check if COG of sprite is within 
   bothWays,
   dontCare
} collisionMethodEnum;




typedef struct
{
   UInt16                 collisionBitmap;    
   collisionMethodEnum    collisionMethod;
   UInt8                  numActive;
   Int16                  firstSprite;         // index of 1st sprite in class
   //Int16                firstActiveSprite;   // index of 1st sprite to render (-1 = none in this class)
} spriteCategoryStruct;


//-----------------------------------------------------------------------
//                     Sprite Engine Vars
//-----------------------------------------------------------------------

// Sprites organized by class..
spriteCategoryStruct   spriteList[ spriteClassMax ];

// Control the order in which the sprites are rendered (later
// classes go on "top" of earlier).
UInt16                 spriteRenderDepthList[ spriteClassMax ];

// Sprites organized by sprite number..
sprite * allSprites[ MAXNUMSPRITES ];

UInt16   SpriteEngHighestSpriteNum;

// Number of sprites drawn in last frame (OM)
UInt16    SpriteEngNumSpritesRendered;

//-----------------------------------------------------------------------
//                     Sprite Engine OMs
//-----------------------------------------------------------------------

// Debug
UInt16 OMcollisionTests;
UInt16 OMfourCornerLevel1Checks;
UInt16 OMfourCornerLevel2Checks;
UInt16 OMcogLevel1Checks;
UInt16 OMcogLevel2Checks;
UInt16 OMCogXcalculations;
UInt16 OMCogYcalculations;
UInt16 OMnotInSameRegion;
UInt16 OMRealX12calc;
UInt16 OMRealY12calc;


//-----------------------------------------------------------------------
//                     Sprite Engine Methods
//-----------------------------------------------------------------------

//inline void  SpriteCheckBorderViolations(sprite *s) SEC6;

void      SpriteEngInit() SEC2;
sprite *  SpriteEngCreateSprite( NewSpriteInfo * n ) SEC2;
void      SpriteEngDeleteSprite( sprite * s ) SEC2;
void      SpriteEngMarkSpriteInactive(sprite *) SEC2;
sprite *  SpriteEngFindFirstUsedSprite( Int thisClass ) SEC2;
sprite *  SpriteEngFindFirstUnusedSprite( Int thisClass ) SEC2;
sprite *  SpriteEngFindFirstSprite( Int thisClass, Boolean state ) SEC2;
Int16     SpriteEngInfoNumFreeSprites( Int thisClass );
sprite *  SpriteEngFindFirstUnusedSpriteOrForce( Int thisClass ) SEC2;

sprite *  SpriteEngFindThisSpriteType 
(
   Int thisClass, 
   Int thisBmp,
   sprite * starthere
) SEC2;

void    SpriteEngHandleTick_OLD() SEC5;
void    SpriteEngHandleTick() SEC5;

void    SpriteEngDumpActiveList() SEC5;
void    SpriteHandleTick() SEC5;
void SpriteEngRegisterCollisionMap(
   UInt16               thisSpriteType,
   UInt16               targSpriteType,
   collisionMethodEnum  method
) SEC6;

void    SpriteEngRegisterCollisionMapOLD(Int, UInt16, collisionMethodEnum) SEC6;
void    SpriteEngRegisterBitmapMotionSeq(Int i, motionBitmapStruct mps) SEC6;
void    SpriteEngUpdateMotionSeq() SEC6;
void    SpriteEngCheckForCollisions() SEC5;
void    SpriteDoNewMovement(sprite *s) SEC5;
inline Boolean SpriteLoadNextMovementLeg(sprite * s) SEC6;


void SpriteEngSetRenderOrder(UInt16 * renderListPtr) SEC6;


#endif

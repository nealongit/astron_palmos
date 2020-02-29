//
//  BLAST.H
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

#ifndef _BLAST_H_
#define _BLAST_H_

#include <StringMgr.h>
#include <PalmTypes.h>
#include <DebugMgr.h>

#include "defs.h"
#include "sprite.h"
#include "starter.h"


//-----------------------------------------------------------------------
//                           Game constants
//-----------------------------------------------------------------------

#define maxNumMotionSeq_CONST   8    // max num bitmap motion sequences

#define noMotionSeq            -1   // OLD?
#define bug1MotionSeq           1


//-----------------------------------------------------------------------
//             Structs
//-----------------------------------------------------------------------

typedef struct
{  
  Int8 addToX;   // can be +/-
  Int8 addToY;
} formationMoveStruct;


typedef struct
{
  UInt16  numSprites;
  UInt    firstSpriteIndex; 
  UInt32  firstSpriteUniqId;   // UInt32 = ULong
} masterSpriteSaveStruct;


//-----------------------------------------------------------------------
//                               Vars
//-----------------------------------------------------------------------

UInt8    numSprites;
sprite * userSprite;

UInt16 keyInput;

#define maxFormMoveIndex 20
#define ticksBetwMovementConst 4
formationMoveStruct formationMove[maxFormMoveIndex];
Byte formationMoveIndex;


Byte  CapturedByTracBeamXOffset;



//-----------------------------------------------------------------------
//                             Methods
//-----------------------------------------------------------------------

inline void setQuickBulletMotion( 
   sprite * s,
   UInt16   deltaY
) SEC6;

void SetKeyEtcPreferences(mainGameRecordStructV1 * pref, mainGameRecordStructV3 * prefRec3) SEC6;
void BlastHandleTick() SEC6;
sprite * GetUserSprite()  SEC6;
void CreateUserSprite() SEC6;
void CreateUserSpritePt2() SEC6;
void ResetUserSprite() SEC6;
void CreateGameSprites() SEC6;
void CreateMotionSequences() SEC6;
void UpdateMotionSequences() SEC6;
void HandleUserInput() SEC5;
void HandleSpriteEvent(sprite*,Int) SEC5;
void HandleTracBeamPoint(sprite * motherShip ) SEC6;
void HandleLayMinePoint(sprite * thisSprite ) SEC6;
void HandleUserWithinMothership() SEC6;
void HandleSpriteMovementNeeded(sprite *) SEC6;

void HandleSpriteCollision( 
  sprite * hitSprite,   Int hitSpriteClass,
  sprite * hitBySprite, Int hitByClass 
) SEC6;

void HandleUserGotHit( 
  sprite * hitBySprite, 
  Int hitByClass 
) SEC5;

void HandleBugGotHit(
   sprite * hitSprite,
   sprite * hitBySprite
) SEC5;

void HandleNonBugTargetGotHit(
   sprite * hitSprite,
   sprite * hitBySprite
) SEC6;

void UpdateScore( sprite * targetSprite) SEC5;
void HandleSpriteOverBorder(sprite*,Int) SEC6;
void HandleBulletPoint(sprite *) SEC5;
void HandleResetBugToFormation(sprite* s, Boolean) SEC6; 
void SetUserDead() SEC5;

Boolean SaveBlastRecs() SEC6;
Boolean RestoreBlastRecs() SEC6;
void  FreeSprite(sprite *s)  SEC6;
void DoCountAudits() SEC2;
void BlastAppStopDoCleanup() SEC6;


void BackDropMgrInit() SEC5;

#endif

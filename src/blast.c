//
//  BLAST.C
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

#include <PalmOS.h>
#include <UIPublic.h>
#include <PalmTypes.h> 
#include <KeyMgr.h>     // For key polling
#include <TimeMgr.h>    // For getting time ticks 
#include <StringMgr.h>
 

#include "starter.h" 
#include "debug.h" 
#include "behav.h"  
#include "sprite.h" 
#include "blast.h"
#include "gameman.h"
#include "graphics.h" 
#include "defs.h"
#include "misc.h"     // for random number
#include "startRsc.h" // for key numbering
#include "database.h"


//--------------------------------------------------------------------
//  Import the proc var's from sprite.c
//-------------------------------------------------------------------- 
extern void (*HandleSpriteCollisionFuncPtr)(sprite*,Int,sprite*,Int);
extern void (*HandleSpriteEventFuncPtr)(sprite*,Int);
extern void (*HandleSpriteMovementNeededFuncPtr)(sprite*);
extern void (*HandleSpriteExceededBorderFuncPtr)(sprite*,Int);


//--------------------------------------------------------------------
//  Vars
//-------------------------------------------------------------------- 

// Key mask from KeyMgr.h (default setting -- which is fire button on the right)
UInt16 MoveLeftKeyMask   = keyBitHard1;  // 0x08
UInt16 MoveRightKeyMask  = keyBitHard3;  // 0x20
UInt16 FireKeyMask       = keyBitHard4;  // 0x40
UInt16 userBulletBitmap  = slimBulletBmp; // slim

// Button debounce counters
Byte ButtonDebounceCtr[] = {0,0,0,0,0,0,0,0};
Byte ButtonDebounceCnt = 0;

#define fireButQuickRepeatRate 3
Byte FireDebounceCnt = fireButQuickRepeatRate;  // quick repeat 

Byte MoveAmount = 5;       // hacked from 4
Int8 UserBulletSpeed = -6;

// used to store key sample in mid-frame
//UInt16  keyInputSample =0;


UInt16 keyInput =0;


Byte  CapturedByTracBeamXOffset=2;


//--------------------------------------------------------------------
//  SAVEABLES: allSprite, userSprite
//-------------------------------------------------------------------- 

// all Bugs
sprite * allSprites[ MAXNUMSPRITES ];
UInt8    numSprites=0; 

// User sprites
sprite * userSprite; 
sprite * userThrustSprite;

//--------------------------------------------------------------------
//  Motion sequences
//-------------------------------------------------------------------- 
formationMoveStruct formationMove[maxFormMoveIndex]=
  { {-2,1},{-4,2},{-6,1},{-8,0},
    {-6,-1},{-4,-2},{-2,-1},{0,0},
    {2,1},{4,2},{6,1},{8,0},
    {6,-1},{4,-2},{2,-1},{0,0}
};


// SWEEP BUG motion sequence info
#define sweepBugTicksBetwMotConst 4  // num ticks before frame is changed
Byte    sweepBugTicksBetwMot = sweepBugTicksBetwMotConst;
#define sweepBugMotIndexMask 7
#define sweepBugMotIndexMax 8
Byte    sweepBugMotIndexes[sweepBugMotIndexMax] = {0,0,1,1,0,0,1,1};
#define sweepMotSeqMax 4
Byte    sweepBugMotBitmapSeq[sweepMotSeqMax] = {sweepBug2,sweepBug2,sweepBug,sweepBug};
Byte    sweepBugMotIndex=0;


// BUG1B motion sequence info
#define bug1TicksBetwMotConst 4     // num ticks before frame is changed
#define bug1BugMotIndexMax 8    // num separate indexes 
#define bug1BugMotIndexMask 7  
#define bug1MotSeqMax 6

Byte    bug1TicksBetwMot = bug1TicksBetwMotConst;
Byte    bug1BugMotIndexes[bug1BugMotIndexMax] = {0,0,1,1,1,1,0,0};
Byte    bug1BugMotBitmapSeq[bug1MotSeqMax] = {bug1,bug1,bug1_2,bug1_2,bug1,bug1};
Byte    bug1BugMotIndex=0;


// Thrust  motion sequence info
Byte    THRUSTINITyPOS=139;   // 139 for color, 140 for b/w

Byte    thrustTicksBetwMotConst=1;     // num ticks before frame is changed  (=3 for 1-bit)

Byte    thrustTicksBetwMot = 1;
Byte    thrustBugMotBitmapYOffset[3] = {0,1,2};  // for 1-bit only

Byte    thrustBugMotIndex=0;
Byte    thrustMotSeqMax=8; // reset to 3 for 1=bit
Byte    thrustBugMotBitmapSeq[8] = {thrust1,thrust2,thrust2,thrust1,
				    thrust3,thrust3,thrust1,thrust4};    


// Back-in-formation sequences
#define inFormTicksBetwMotConst 2     // num ticks before frame is changed
#define inFormMotSeqMax 3

Byte    inFormTicksBetwMot = inFormTicksBetwMotConst;
Byte    inFormMotBitmapSeq[inFormMotSeqMax] = {inFormBmp1,inFormBmp2,inFormBmp3};    
Byte    inFormMotIndex=0;


// FORMATION
Byte formationMoveIndex=7;
Byte ticksBetwMovement=ticksBetwMovementConst;
Byte formationMoveIndexMax=16;

Byte   firstBackDropSprite=0;
Byte    backDropSpacing=20;

//-----------------------------------------------------------------------
//  INLINE!
//-----------------------------------------------------------------------
inline void setQuickBulletMotion( 
   sprite * s,
   UInt16   deltaY
)
{
   moveStruct * ms=&s->myMoveStruct;

   // Directly write the moveStruct so that DoNewMovement does not have to load it
   ms->currentLeg=0;
   ms->countdown = 200;
   ms->sendThisEvent = SignalRemoveSprite;


   ms->leg[0].legType=(deltaY>0) ? staticLegYIncr : staticLegYDecr;
   ms->leg[0].guts.staticLegStruct.deltaX=0;
   ms->leg[0].guts.staticLegStruct.deltaY=deltaY;
}

   

//--------------------------------------------------------------------
//  
//-------------------------------------------------------------------- 
sprite * GetUserSprite()
{
   //debugFuncExit("GetUserSprite()");
  return userSprite;
}


//--------------------------------------------------------------------
//  SetKeyEtcPreferences
//-------------------------------------------------------------------- 
void SetKeyEtcPreferences(
   mainGameRecordStructV1 * pref,
   mainGameRecordStructV3 * prefRec3
)
{
   debugFuncEnterInt("SetKeyEtcPreferences(), sidePref",pref->sidePref);

   // In v 1.05 the sidePref changed from saying which side the FIREBUTTON is on
   // to which side the CONTROLS are on (so #defines were changed)
   if (pref->sidePref == ltSidepBut ) {
      // LEFT
      MoveLeftKeyMask  = keyBitHard1; 
      MoveRightKeyMask = keyBitHard2;
      FireKeyMask      = keyBitHard4;
   } else {
      if (pref->sidePref == rtSidepBut) {
	 // RIGHT
	 MoveLeftKeyMask  = keyBitHard3;
	 MoveRightKeyMask = keyBitHard4;
	 FireKeyMask      = keyBitHard1;
      } else {
	 // MIDDLE
	 MoveLeftKeyMask  = keyBitHard2;
	 MoveRightKeyMask = keyBitHard3;
	 FireKeyMask      = (keyBitHard1 | keyBitHard4);  // 2 fire buttons
      }
   }
      
   userBulletBitmap =  fatBulletBmp;
   
   if (pref->bulletSizePref == 0) {
      userBulletBitmap =  fatBulletBmp;
   } else {
      if (pref->bulletSizePref == 1) {
	 userBulletBitmap =  slimBulletBmp;
      } else {
	 userBulletBitmap =  dotBulletBmp;  // UNUSED
      }
   }
   
   
   // 250 = no repeat
   FireDebounceCnt = (prefRec3->bulletRepeatPref) ? fireButQuickRepeatRate : 250;
   

   debugFuncExit("SetKeyEtcPreferences()");
}

//--------------------------------------------------------------------
//  Direct the sprite engine to create the motion sequences
//-------------------------------------------------------------------- 
void CreateMotionSequences() 
{

  // fix!!! when we draw more sprites
  /*
  motionBitmapStruct   bug1Motion = {2,0,2,4,{16,17,18,17}};

      
   SpriteEngRegisterBitmapMotionSeq(bug1MotionSeq,bug1Motion);
  */

   //debugFuncExit("CreateMotionSequences()");
}

//--------------------------------------------------------------------
//  UpdateMotionSequences
//-------------------------------------------------------------------- 
void UpdateMotionSequences()
{
   Byte i=0;

   //UInt8 testSeq[]={thrust1,thrust2,thrust3,thrust4},x;
   
   debugFuncEnter("UpdateMotionSequences()");
   
   
   // Update formation movement
   ticksBetwMovement--;
   
   if (!ticksBetwMovement) {
      formationMoveIndex = (++formationMoveIndex)%formationMoveIndexMax;
      ticksBetwMovement = ticksBetwMovementConst;  // reset countdown
   }
   
   // Update sweep bug motion sequence (incr each one)
   sweepBugTicksBetwMot--;
   
   if (!sweepBugTicksBetwMot) {
      for (i=0;i<sweepBugMotIndexMax;i++) {
	 sweepBugMotIndexes[i] = ++sweepBugMotIndexes[i]%sweepMotSeqMax;
	 //debugPutInt("sweepBugMotIndexes[i]",sweepBugMotIndexes[i]);
      }
      sweepBugTicksBetwMot = sweepBugTicksBetwMotConst; // reset countdown
   }
   
   // Update bug1 bug motion sequence (incr each one)
   bug1TicksBetwMot--;
   
   if (!bug1TicksBetwMot) {
      for (i=0;i<bug1BugMotIndexMax;i++) {
	 bug1BugMotIndexes[i] = ++bug1BugMotIndexes[i]%bug1MotSeqMax;
	 //debugPutInt("bug1BugMotIndexes[i]",bug1BugMotIndexes[i]);
      }
      bug1TicksBetwMot = bug1TicksBetwMotConst; // reset countdown
   }
   

   // Handle thrust update
   if (--thrustTicksBetwMot == 0) {
      thrustTicksBetwMot=thrustTicksBetwMotConst;
      thrustBugMotIndex=(thrustBugMotIndex+1)%thrustMotSeqMax;
      userThrustSprite->bitmap = thrustBugMotBitmapSeq[thrustBugMotIndex];

      // 1-bit(only)  moves the thrust sprite up and down
      if (ScreenDepth == oneBitDepth) {
	 userThrustSprite->y=THRUSTINITyPOS + thrustBugMotBitmapYOffset[thrustBugMotIndex];
      }
   }
   

   // Update in-formation motion sequence
   inFormTicksBetwMot--;
   
   if (!inFormTicksBetwMot) {
      inFormMotIndex = (inFormMotIndex+1)%inFormMotSeqMax;
      inFormTicksBetwMot = inFormTicksBetwMotConst;
      debugPutInt("  inFormMotIndex",inFormMotIndex);
   }


   debugFuncExit("UpdateMotionSequences()");
} 


//--------------------------------------------------------------------
//  CreateGameSprites
//-------------------------------------------------------------------- 
void CreateGameSprites() 
{

   UInt16 renderList[] = {BackDropSpriteClass,ExplosionSpriteClass,BugBulletSpriteClass,
                          UserBulletSpriteClass,BugSpriteClass,MineSpriteClass,
			  UserSpriteClass,MiscSpriteClass,TracBeamSpriteClass};
   
   
   Int i;

   borderActionEnum   borderAction[]         = { noAction,sendSignal,sendSignal,sendSignal };
   borderActionEnum   bulletBorderAction[]   = { sendSignal,sendSignal,sendSignal,sendSignal };
   borderActionEnum   tracBeamBorderAction[] = { sendSignal,noAction,sendSignal,sendSignal };
   borderActionEnum   miscBorderAction[]     = { noAction,noAction,noAction,noAction };
   borderActionEnum   starBorderAction[]     = {sendSignal,noAction,noAction,noAction };


   // Explosion sprites (class 0)
   NewSpriteInfo explos= 
          {false,0, ExplosionSpriteClass,explosBmp,0,0,0,0,borderAction,noMotionSeq,0};

   // User's bullet sprites (class 2)
   NewSpriteInfo bullet= 
          {false,0, UserBulletSpriteClass,slimBulletBmp,0,0,0,0,bulletBorderAction,noMotionSeq,0};

   // Bug sprites (class 3) 
   NewSpriteInfo bug= 
          {false,0,BugSpriteClass,bug1,0,0,0,0,borderAction,bug1MotionSeq,0};
 
   // Bug's bullet sprites (class 4)
   NewSpriteInfo bugBullet= 
          {false,0,BugBulletSpriteClass,fatBulletBmp,0,0,0,0,bulletBorderAction,noMotionSeq,0};

   // tractor beam sprites (class 5)
   NewSpriteInfo tracBeam= 
          {false,0,TracBeamSpriteClass,tracBeamBmp,0,0,0,0,tracBeamBorderAction,noMotionSeq,0};

   // mine sprites (class 6)
   NewSpriteInfo mine= 
          {false,0,MineSpriteClass,mineBmp,0,0,0,0,bulletBorderAction,noMotionSeq,0};

   // misc sprites (class 7)
   NewSpriteInfo misc= 
          {false,0,MiscSpriteClass,mineBmp,0,0,0,0,miscBorderAction,noMotionSeq,0};

   // misc sprites (class 8)
   NewSpriteInfo star= 
          {false,0,BackDropSpriteClass,starBmp,0,0,0,0,starBorderAction,noMotionSeq,0};

   // Register all motion sequences first
   CreateMotionSequences();

   debugPutInt("1st Bug sprite, s",numSprites);

   // Create all bug sprites
   for (i=0;i < MAXNUMBUGS;i++) {

      bug.spriteNum = numSprites;

      // Overwritten in SetFinalPosition
      bug.x=0;
      bug.y=0;

      // Motion bitmap (defaulted for now)
      bug.bitmapMotionSeqIndex = 0; 

      // border action
      bug.borderAction=&borderAction[0];

      // Store ptr in the all-bug list
      allSprites[numSprites] = SpriteEngCreateSprite(&bug);
      numSprites++;

   } // i


   // Create user's bullets 
   debugPutInt("1st user bullet sprite, s",numSprites);
 
   for (i=0;i<MAXNUMUSERBULLETS;i++) {
      bullet.spriteNum=numSprites;
      allSprites[numSprites] = SpriteEngCreateSprite(&bullet);
      setSpriteForDynamicMovement(allSprites[numSprites]);
      numSprites++;
   }


   // Create explosions
   debugPutInt("1st explosion sprite, s",numSprites);

   for (i=0;i<MAXNUMEXPLOSIONS;i++) {
      explos.spriteNum=numSprites;
      allSprites[numSprites] = SpriteEngCreateSprite(&explos);
      setSpriteForDynamicMovement(allSprites[numSprites]);
      numSprites++;  
   }


   // Create bug bullets
   debugPutInt("1st bug bullet sprite, s",numSprites);

   for (i=0;i<MAXNUMBUGBULLETS;i++) {
      bugBullet.spriteNum=numSprites;
      allSprites[numSprites] = SpriteEngCreateSprite(&bugBullet);  
      setSpriteForDynamicMovement(allSprites[numSprites]);
      numSprites++;   
   }

   // Create tractor beam
   debugPutInt("1st tractor beam sprite, s",numSprites);

   for (i=0;i<MAXNUMTRACBEAMS;i++) {
      tracBeam.spriteNum=numSprites;
      allSprites[numSprites++] = SpriteEngCreateSprite(&tracBeam);     
   }
  
   
   // Create mines
   debugPutInt("1st mine sprite, s",numSprites);

   for (i=0;i<MAXNUMMINES;i++) {
      mine.spriteNum=numSprites;
      allSprites[numSprites++] = SpriteEngCreateSprite(&mine);     
   }
  
   // Create misc
   debugPutInt("1st misc sprite, s",numSprites);

   for (i=0;i<MAXNUMMISCSPRITES;i++) {
      misc.spriteNum=numSprites;
      allSprites[numSprites++] = SpriteEngCreateSprite(&misc);     
   }

   // Create backdrop sprites
   firstBackDropSprite=numSprites;
   firstBackDropSpriteClass2 = firstBackDropSprite + NUMSLOWBACKDROP;
   debugPutInt("1st star sprite, s",firstBackDropSprite);

   for (i=0;i<MAXNUMBACKSPRITES;i++) {
      star.spriteNum=numSprites;
      allSprites[numSprites] = SpriteEngCreateSprite(&star);     
      setSpriteForDynamicMovement(allSprites[numSprites]);

      // Set here for speed
      allSprites[numSprites]->borderAction[0]=sendSignal;  
      allSprites[numSprites]->borderAction[1]=sendSignal;  
      allSprites[numSprites]->borderAction[2]=sendSignal;
      allSprites[numSprites]->borderAction[3]=sendSignal; 

      numSprites++;
   }

   // Register the collision detection -- ***USER checks for collisions with 
   // bugs,trac beam ***
 
   SpriteEngRegisterCollisionMap( ExplosionSpriteClass, 0, dontCare);   
   SpriteEngRegisterCollisionMap( BugSpriteClass,       0, dontCare);  
   SpriteEngRegisterCollisionMap( TracBeamSpriteClass,  0, dontCare);
   SpriteEngRegisterCollisionMap( MiscSpriteClass,      0, dontCare);
   SpriteEngRegisterCollisionMap( BackDropSpriteClass,  0, dontCare);

   SpriteEngRegisterCollisionMap( UserBulletSpriteClass, MineSpriteClass, centerOfGrav);  // 0x48   
   SpriteEngRegisterCollisionMap( UserBulletSpriteClass, BugSpriteClass,  centerOfGrav); 
   SpriteEngRegisterCollisionMap( BugBulletSpriteClass,  UserSpriteClass, centerOfGrav);   //0x02
   SpriteEngRegisterCollisionMap( MineSpriteClass,       UserSpriteClass, centerOfGrav);
   //SpriteEngRegisterCollisionMap( MineSpriteClass,       UserBulletSpriteClass, centerOfGrav);


   SpriteEngSetRenderOrder(&renderList[0]);
   

   /*
   //SpriteEngRegisterCollisionMap( UserBulletSpriteClass,0x48,centerOfGrav);  // mines,bugs     
   SpriteEngRegisterCollisionMap( BugBulletSpriteClass,0x02,centerOfGrav);   
   SpriteEngRegisterCollisionMap( MineSpriteClass,0x02,centerOfGrav);   // user bullets
   SpriteEngRegisterCollisionMap( MiscSpriteClass,0,dontCare);
   SpriteEngRegisterCollisionMap( BackDropSpriteClass,0,dontCare);
   */

   // Set procvars 
   HandleSpriteCollisionFuncPtr        = HandleSpriteCollision;
   HandleSpriteEventFuncPtr            = HandleSpriteEvent; 
   HandleSpriteMovementNeededFuncPtr   = HandleSpriteMovementNeeded;
   HandleSpriteExceededBorderFuncPtr   = HandleSpriteOverBorder;


   debugFuncExit("CreateGameSprites()");
}


//----------------------------------------------------------------------
//  Proc called by sprite engine when it sees it is not the controller
//  of the movement.
//
//  Currently we move formation, and do some animtation
//----------------------------------------------------------------------
void HandleSpriteMovementNeeded(sprite * s)
{
  debugFuncEnterInt("HandleSpriteMovementNeeded(), s",s->spriteNum);

  //
  //   NOT CALLED... FUNCTIONALITY MOVED TO UPDATEMOTIONSEQUENCE
  //
  //



  // stop barriers from moving... HACK
  if (s->bitmap == barrierBmp) {
    debugFuncExit("HandleSpriteMovementNeeded()");
    return;
  }

  s->x = roundInfo.bugs[s->spriteNum].origX + formationMove[formationMoveIndex].addToX;
 

  // For sweep bugs, lookup new bitmap in the array
  if ((s->bitmap == sweepBug) || (s->bitmap == sweepBug2)) {
    s->bitmap = sweepBugMotBitmapSeq[sweepBugMotIndexes[roundInfo.bugs[s->spriteNum].motionIndex &
                                                         sweepBugMotIndexMask]];
  }

  // For bug1 bugs, lookup new bitmap in the array
  if ((s->bitmap == bug1) || (s->bitmap == bug1_2)) {
    s->bitmap = bug1BugMotBitmapSeq[bug1BugMotIndexes[roundInfo.bugs[s->spriteNum].motionIndex &
                                                         bug1BugMotIndexMask]];
  }

  //debugPutInt("  thrustBugMotIndex",thrustBugMotIndex);

  /*
  // Thrust animation for 1-bit
  if ((ScreenDepth == oneBitDepth) &&
      ((s->bitmap == thrust1) || (s->bitmap == thrust2)))
  {
     s->bitmap = thrustBugMotBitmapSeq[thrustBugMotIndex];
     s->y=THRUSTINITyPOS + thrustBugMotBitmapYOffset[thrustBugMotIndex];
     debugPutInt(" thrust s->y",s->y);  
  }
  */

  //debugPutInt("   index",roundInfo.bugs[s->spriteNum].motionIndex);
  //debugPutInt("   s->x",s->x);
  //debugPutInt("   s->y",s->y);
  //debugPutInt("   s->bitmap",s->bitmap);


   debugFuncExit("HandleSpriteMovementNeeded()");
}


//----------------------------------------------------------------------
// BlastHandleTick
//----------------------------------------------------------------------
void BlastHandleTick()
{

   UInt16 i=0;
   sprite * s=0;
   
   debugFuncEnter("BlastHandleTick()");

   // Loop over all bugs (numbered from zero)
   for (i=0;i<MAXNUMBUGS ;i++)
   {
      s=allSprites[i];
      
      if isActive(s) { 

	 // Check who is controlling sprite motion -- us, or the SpriteEngine
	 //
	 // ***Note that we are ALSO SKIPPING BORDER VIOLATION CHECKING
	 //    SINCE custom code should not put sprite over border!
	 // 
	 if ( s->moveStructOwner == userAppl0) 
	 {

	    // Set the region fields -- only should be in top regions
	    s->collisionRegion=region__3;

	    // stop barriers from moving... HACK
	    if (s->bitmap == barrierBmp) {
	       debugFuncExit("HandleSpriteMovementNeeded()");
	       continue;
	    } 
	       
	    // While in formation the Y parameter does not change!  The x does (except 
	    // for barriers -- above)
	    s->realCogXcalc = false;    
	    s->realX12calc = false;
	    
	    // Update x 
	    s->x = roundInfo.bugs[s->spriteNum].origX + formationMove[formationMoveIndex].addToX;    
	    
	    // For sweep bugs, lookup new bitmap in the array
	    if ((s->bitmap == sweepBug) || (s->bitmap == sweepBug2)) {
	       s->bitmap = sweepBugMotBitmapSeq[sweepBugMotIndexes[roundInfo.bugs[s->spriteNum].motionIndex &
								  sweepBugMotIndexMask]];
	       continue;
	       
	    }

	       
	    // For bug1 bugs, lookup new bitmap in the array
	    if ((s->bitmap == bug1) || (s->bitmap == bug1_2)) {
	       s->bitmap = bug1BugMotBitmapSeq[bug1BugMotIndexes[roundInfo.bugs[s->spriteNum].motionIndex &
								bug1BugMotIndexMask]];
	       
	       debugPutInt("s->bitmap",s->bitmap);
	    } 
	    
	    //debugPutInt("   index",roundInfo.bugs[s->spriteNum].motionIndex);
	    //debugPutInt("   s->x",s->x);
	    //debugPutInt("   s->y",s->y);
	    //debugPutInt("   s->bitmap",s->bitmap);

	 } 

	 else if (s->moveStructOwner == userAppl1) 
	 {
	    // Set the region fields -- only should be in top regions
	    s->collisionRegion=region__3;
	       
	    // While in formation the Y parameter does not change (except BUGFIX 
	    // below)!  The x does (except for barriers -- above)
	    s->realCogXcalc = false;    
	    s->realX12calc  = false;
	    
	    // Update x 
	    s->x = roundInfo.bugs[s->spriteNum].origX + formationMove[formationMoveIndex].addToX;    

	    s->myMoveStruct.countdown--;

	    if (!s->myMoveStruct.countdown) {
	       
	       s->myMoveStruct.leg[0].guts.inFormSeqStruct.currSequence++;
	       
	       if (s->myMoveStruct.leg[0].guts.inFormSeqStruct.currSequence !=inFormMotSeqMax) {
		  // Move to next bitmap in sequence, reset counter
		  s->bitmap = inFormMotBitmapSeq[s->myMoveStruct.leg[0].guts.inFormSeqStruct.currSequence];
		  s->myMoveStruct.countdown = inFormTicksBetwMotConst;
	       } else {
		  // Sequence complete
		  s->bitmap = s->myMoveStruct.leg[0].guts.inFormSeqStruct.restoreBitmap;
		  s->moveStructOwner = userAppl0;

		  // BUG FIX: "Immune Formation Bugs"
		  // Must reset these flags because the >in-formation< bitmap was used
		  // to calculate the X&Y collision regions... but now the real bitmap
		  // is being restored, so X&Y regions must be recalculated.  X flags
		  // are cleared above.  
		  s->realCogYcalc = false;    
		  s->realY12calc  = false;

	       }
	    }
	    
	 }
	 

      } // isActive

   } // for i 
  
   debugFuncExit("BlastHandleTick()"); 
}



//--------------------------------------------------------------------
//  Sprite engine has seen a collision
//-------------------------------------------------------------------- 
void HandleSpriteCollision( sprite * hitSprite,   Int hitSpriteClass,
                            sprite * hitBySprite, Int hitByClass )
{

   // debug
   debugFuncEnterInt("HandleSpriteCollision(),  hitByClass",hitByClass);
   debugPutInt("  hitSpriteClass",hitSpriteClass);

   // Very infrequently we may have a bullet sprite simultaneously hit
   // 2 targets, if we don't catch this then round vars get screwed up..
   if (notActive(hitBySprite) || notActive(hitSprite)) {
      debugPuts("Dual collision detected");
      debugFuncExit("HandleSpriteCollision()");
      return;
   }

   // User got hit?
   if (hitSpriteClass == UserSpriteClass)
   {
     HandleUserGotHit(hitBySprite,hitByClass);
     debugFuncExit("HandleSpriteCollision()");
     return;
   }

   // User did the hitting?
   if (hitByClass == UserSpriteClass)
   {
     // flip the order of the hit/hit-by sprites...
     HandleUserGotHit(hitSprite,hitSpriteClass);
     debugFuncExit("HandleSpriteCollision()");
     return;
   }

   // User bullet hit 2-shot award (which is impervious)
   if (hitSprite->bitmap == twoShotAwardBmp) {
     debugPuts("  Ignoring hit to 2 shot award");
     debugFuncExit("HandleSpriteCollision()");
     return;
   }

   // User bullet hit mine?
   if ((hitSpriteClass == MineSpriteClass) && 
       (hitByClass == UserBulletSpriteClass)) 
   {
      HandleNonBugTargetGotHit(hitSprite,hitBySprite);

      // Free user bullet
      roundInfo.numUserBulletsFlying--;
      FreeSprite( hitBySprite ); 
      debugFuncExit("HandleSpriteCollision()");
      return;
   }

   // User bullet hit bug?
   if ((hitSpriteClass == BugSpriteClass) && 
       (hitByClass     == UserBulletSpriteClass)) 
   {

      debugPutInt("  User bullet hit bug, spriteNum",hitSprite->spriteNum);

      // remove user bullet
      roundInfo.numUserBulletsFlying--;
      FreeSprite( hitBySprite );

      // debug
      if (hitSprite->bitmap == barrierBmp) {
	 debugPuts("  barrier got hit");
      }

      // Handle objects taking multiple hits
      roundInfo.bugs[hitSprite->spriteNum].numHitsToKill--;

      if (roundInfo.bugs[hitSprite->spriteNum].numHitsToKill !=0) {
	debugFuncExit("HandleSpriteCollision()");
	return;
      }

      // barrier, dying
      if (hitSprite->bitmap == barrierBmp) {
	 roundInfo.numBarriers--;
         showExplosion(hitSprite);
	 UpdateScore(hitSprite);
         FreeSprite( hitSprite );
         debugFuncExit("HandleSpriteCollision()");
         return;
      }

      // User bullet hitting empty mothership? 
      if ((hitSprite->bitmap == mothership) &&
          (roundInfo.tracBeamOwnerSpriteNum == hitSprite->spriteNum)) 
      {
         if (gameInfo.userState == user_inTracBeam) {
            // Too late, user already captured, ignore bullet
	    debugPuts("bullet ignored, user already captured");
	    debugFuncExit("HandleSpriteCollision()");
	    return;
	 } else {
	    // Mothership got shot while in trac beam, but before getting user do cleanup
            FreeSprite(allSprites[roundInfo.tracBeamSpriteNum]);
            roundInfo.tracBeamSpriteNum = -1;
            roundInfo.tracBeamOwnerSpriteNum = -1;
	 }
      }

      // OK, remove the bug
      showExplosion(hitSprite);
      HandleBugGotHit(hitSprite,hitBySprite);


     // ..end of user bullet hit bug

   } else {

     debugPutsError("Unknown collision");
     //ERROR_ASSERT("NEAL: Unknown collision");

   }
   debugFuncExit("HandleSpriteCollision()");
}


//--------------------------------------------------------------------
//  UpdateScore
//
// Add to score, see if player has won a new ship
//-------------------------------------------------------------------- 
void UpdateScore(
   sprite * targetSprite
)
{
  Int score=0;

  debugFuncEnter("UpdateScore()");

  // Calculate non-diving score
  switch (targetSprite->bitmap) 
  {
    case mineLayer:           score=75;break;
    case fullMotherShip:      score=75;break;
    case mothership:          score=50;break;
    case sweepBug:
    case sweepBug2:           score=60;break;
    case bug1:
    case bug1_2:              score=40;break;
    case dartBug:             score=30;break;

    // non-bugs
    case mineBmp:             score=50;break;
    case barrierBmp:          score=25;break;
  }

  // Additional points if bug was diving
  if (targetSprite->spriteType == BugSpriteClass) {
    if (roundInfo.bugs[targetSprite->spriteNum].aliveState == diving) {
      score+=50;
    }
  }

  // Update title bar
  gameInfo.score += score;  
  textMgrTitleBar.needsRedraw = true;  

  // See if player gets a bonus ship
  if (gameInfo.score > gameInfo.nextExtraShipScore) {
    gameInfo.nextExtraShipScore += pointsForNextShip;
    gameInfo.numShips++;
    SoundMgrThisSound(extraShipAwarded); 
  }

   debugFuncExit("UpdateScore()");
}


//--------------------------------------------------------------------
//  
//-------------------------------------------------------------------- 
void HandleNonBugTargetGotHit(
   sprite * hitSprite,
   sprite * hitBySprite
)
{

   // Mine..
   if ((hitSprite->spriteType == MineSpriteClass) && 
       (hitBySprite->spriteType == UserBulletSpriteClass)) 
   {
      showExplosion(hitSprite);
      roundInfo.numMinesFlying--;

      UpdateScore(hitSprite);

      // user buller already freed, free the mine sprite
      FreeSprite( hitSprite );
   }

   debugFuncExit("HandleNonBugTargetGotHit()");
}




//--------------------------------------------------------------------
//  HandleBugGotHit
//-------------------------------------------------------------------- 
void HandleBugGotHit(
   sprite * hitSprite,
   sprite * hitBySprite
)
{

   Int bg,bgi;

   debugFuncEnterInt("HandleBugGotHit(), hitSprite",hitSprite->spriteNum);

   // Score it..
   UpdateScore(hitSprite);
   SoundMgrThisSound(bugExplosion);

     // Handle the round info
     roundInfo.numRemainBugs--;
     if ((roundInfo.bugs[hitSprite->spriteNum].aliveState == diving) ||
         (roundInfo.bugs[hitSprite->spriteNum].aliveState == doingIntro))
     {
       // moving bug got hit, decr count
       roundInfo.numBugsDiving--;
     }

     // Handle user bullet hitting full mothership
     if ((hitSprite->bitmap == fullMotherShip) && 
         (roundInfo.bugs[hitSprite->spriteNum].aliveState == diving)) 
     {
        // drop the 2-shot bonus down
        roundInfo.bonus2ShotAwardSpriteNum = setBonus2ShotMotion(hitSprite);
        roundInfo.tracBeamOwnerSpriteNum = -1;        
     }

     // Handle the round info bug lists -- put the bug at end of list into this space
     bg  = roundInfo.bugs[ hitSprite->spriteNum ].bugGroup;
     bgi = roundInfo.bugs[ hitSprite->spriteNum ].bugGroupIndex;
 

     // If the bug is not the last in the list then assign the index previously held 
     // the bug to the index held by the end of list bug, then decrement count

     if ( bgi != (roundInfo.bugGroupCount[bg]-1)) {

        // Move the spriteNum
        roundInfo.bugGroup[bg][bgi] = roundInfo.bugGroup[bg][ roundInfo.bugGroupCount[bg] -1 ];

        // Also need to reset the bgi in this newly moved bug
        roundInfo.bugs[ roundInfo.bugGroup[bg][bgi] ].bugGroup      = bg;
        roundInfo.bugs[ roundInfo.bugGroup[bg][bgi] ].bugGroupIndex = bgi;

     } else {
        // debug
        debugPutInt("hit but AT END OF LINE spriteNum",hitSprite->spriteNum);
     }

     roundInfo.bugGroupCount[bg]--;

     debugPutInt("hit bug bg",bg);
     debugPutInt("hit bug bgi",bgi);
     debugPutInt("hit bugs group count",roundInfo.bugGroupCount[bg]);

     // Free bug sprite (bullet HAS ALREADY BEEN FREED)
     FreeSprite( hitSprite ); 

     debugFuncExit("HandleBugGotHit()");
}

//--------------------------------------------------------------------
//  User has been hit by an object (good, or bad)
//-------------------------------------------------------------------- 
void HandleUserGotHit( 
  sprite * hitBySprite, 
  Int hitByClass 
) 
{
   debugFuncEnterInt("HandleUserGotHit(), hitBySprite",hitBySprite->spriteNum);

   if( gameInfo.userState != user_normal ) {
     debugPuts("user state != user_normal");
     debugFuncExit("User()");
     return;
   }


   switch(hitBySprite->bitmap)
   {

      case tracBeamBmp:
	debugPuts("User caught in trac beam");

	if (IGNORETRACBEAM) {
	   break;
	}
	
        if ( (roundInfo.tracBeamSpriteNum   == -1) ||
             (roundInfo.tracBeamOwnerSpriteNum == -1)) 
        {   
          // big errors
	  debugPutsError("no mothership and/or trac beam");
	  debugFuncExit("User()");
	  return;
	}
 
        // Reset the trac beam, mothership, and player
        userSprite->x = allSprites[roundInfo.tracBeamOwnerSpriteNum]->x+CapturedByTracBeamXOffset;

        setTracBeamMotion(allSprites[roundInfo.tracBeamSpriteNum],
                          allSprites[roundInfo.tracBeamOwnerSpriteNum]);

        setSuccessMothershipMotion(allSprites[roundInfo.tracBeamOwnerSpriteNum]);

        setUserInTracBeamMotion(userSprite,
				(userSprite->y - allSprites[roundInfo.tracBeamOwnerSpriteNum]->y));

        // no thrust in trac beam...
	DelFromActiveList(userThrustSprite);
 

        // affect gameplay -- no more bugs, trac beams, etc.
        gameInfo.userState = user_inTracBeam; 
        break;

     case twoShotAwardBmp:
        debugPuts("User caught 2-shot award");
        SoundMgrThisSound(doubleShotAwarded);

	userSprite->bitmap=userShip2Bmp;

        gameInfo.twoShots=true;
	gameInfo.numShips++;
	roundInfo.bonus2ShotAwardSpriteNum=-1;

        FreeSprite(hitBySprite);
        break;

     case mineBmp:
        debugPuts("User hit by mine");
        SoundMgrThisSound(userExplosion);
	showExplosion(userSprite);  // temp

        roundInfo.numMinesFlying--;
        FreeSprite(hitBySprite);

        SetUserDead();
	break;

     case fatBulletBmp:
        debugPuts("User hit by bullet");
        SoundMgrThisSound(userExplosion);
	 
        FreeSprite(hitBySprite);
        roundInfo.numBulletsFlying--;

        SetUserDead();
	break;
        
      default:
	// case of a bug hitting user
        debugPuts("User hit by bug");
        SoundMgrThisSound(userExplosion);
        HandleBugGotHit(hitBySprite,hitBySprite); 

        SetUserDead();
     }


   debugFuncExit("User()");
}

//--------------------------------------------------------------------
//  Called when user has been destroyed
//--------------------------------------------------------------------
void SetUserDead()
{

  debugFuncEnter("SetUserDead()");

  showExplosion(userSprite);   // temporary

  if (USERIMMUNE) {   
     // debug hack to let player keep playing
     gameInfo.debugTimesHit++;
     textMgrTitleBar.needsRedraw = true; 
     debugFuncExit("SetUserDead()");
     return; 
  }  

  // This is not really safe for the thrust sprite, because it has now been releases
  // back to the pool...
  DelFromActiveList(userSprite);
  DelFromActiveList(userThrustSprite);

  // Save game state so we debugFuncExit("SetUserDead()");
  // Save game state so we return to correct state when player is reintroduced
  gameInfo.savedGameState = gameInfo.gameState;

  gameInfo.gameState = waitForEnemyDivingOver;
  gameInfo.userState = user_dead;
  gameInfo.twoShots  = false;

  // game over?
  gameInfo.numShips--;
  textMgrTitleBar.needsRedraw = true;  

  if ( gameInfo.numShips == 0) {
    // Change state
    gameInfo.gameState = gameOver;
    roundInfo.numTicks = gt + 35;
  }
	
   debugFuncExit("SetUserDead()");
}


//--------------------------------------------------------------------
//  Bug is at a point where it can fire
//-------------------------------------------------------------------- 
void HandleBulletPoint(sprite * thisSprite )
{

   sprite * s=0;
   //moveStruct * ms;
   UInt8 limit=0,percChance=100;

   debugFuncEnterInt("HandleBulletPoint(), s",thisSprite->spriteNum);


   if (thisSprite->y > rps.bulletCeiling) {
     debugPutInt("  bug lower than bulletCeiling",rps.bulletCeiling);
     debugPutInt("  bug->y",thisSprite->y);
     debugFuncExit("HandleBulletPoint()");
     return;
   }

   // Different stages of game have limits on num bullets flying
   if (gameInfo.gameState == gamePlay) {
      if (!roundInfo.inBlitzMode) {
	 // normal game play
	 limit      = rps.maxBulletsInNormalPlay;
         percChance = rps.percChanceShotNormalPlay;
      } else {
	 // blitz mode
	 limit      = rps.maxBulletsInBlitzMode;
         percChance = 75;
      } 
   } else {
      // Intro waves (probably) 
      limit      = rps.maxBulletsInIntroWaves;
      percChance = rps.percChanceShotIntroWave;
   }

   // Test..
   if ((roundInfo.numBulletsFlying >= limit) || 
       (!randomPercTest(percChance))) 
   {
      debugPutInt("   failed % chance, or too many bullets are flying ",roundInfo.numBulletsFlying);
      debugFuncExit("HandleBulletPoint()");
      return;
   } 

   
   // Get an unused sprite bullet
   s = SpriteEngFindFirstUnusedSprite( BugBulletSpriteClass );

   if (s != NULL) {
       
      // Set the sprite info -- do it here for speed
      AddToActiveList(s);
      s->y = thisSprite->y;

      if (gameInfo.gameState !=introWaves) {
         s->x = thisSprite->x+6;
      } else {
	// randomize x so bullets don't come down in columns
	s->x = thisSprite->x+3+randomNum(6);
      }

      debugPutInt("  encoding bug bullet, s",s->spriteNum);
      debugPutInt("                       x",s->x);

      // Perform quick setup for bullet motion (for speed)
      setQuickBulletMotion(s,rps.bulletSpeed);

      roundInfo.numBulletsFlying++;
   }


   debugFuncExit("HandleBulletPoint()");
}

//--------------------------------------------------------------------
//  Bug is at a point where it can lay a mine
//-------------------------------------------------------------------- 
void HandleLayMinePoint(sprite * thisSprite )
{

   sprite * s;

   debugFuncEnter("HandleLayMinePoint()");

   // Get an unused mine sprite
   s = SpriteEngFindFirstUnusedSprite( MineSpriteClass );

   if (s != NULL) {
      setMineMotion(s,thisSprite);
      roundInfo.numMinesFlying++;
      SoundMgrThisSound(layedAMine);
   }
 
   debugFuncExit("HandleLayMinePoint()");
}

//--------------------------------------------------------------------
//  User has been captured
//-------------------------------------------------------------------- 
void HandleUserWithinMothership() 
{
  Boolean storeUSERIMMUNE;

   debugFuncEnter("HandleUserWithinMothership()");

   // Turn off trac beam
   if ( roundInfo.tracBeamSpriteNum != -1) {
     FreeSprite(allSprites[roundInfo.tracBeamSpriteNum]);
     roundInfo.tracBeamSpriteNum = -1;
   } else {
     debugPutsError("trac beam disappeared");
   }

   // hack, USERIMNUNE must be false or ship floats in air
   storeUSERIMMUNE = USERIMMUNE;
   USERIMMUNE=false;

   // Turn off the user
   SetUserDead();

   USERIMMUNE=storeUSERIMMUNE;  // hack, restore USERIMMUNE

   // Set mothership in motion
   allSprites[roundInfo.tracBeamOwnerSpriteNum]->bitmap = fullMotherShip;
   setDropMotion(allSprites[roundInfo.tracBeamOwnerSpriteNum],2);

   // Don't NULL the owner, this is how we keep track of who has the user ship
   //roundInfo.tracBeamOwnerSprite = NULL;

   debugFuncExit("HandleUserWithinMothership()");
}

//--------------------------------------------------------------------
//  Bug is at a point where it can release a tractor beam
//-------------------------------------------------------------------- 
void HandleTracBeamPoint(sprite * motherShip )
{
   sprite * s;

   debugFuncEnter("HandleTracBeamPoint()");

   // Check lots of conditions
   if (( gameInfo.twoShots) ||
       ( roundInfo.tracBeamOwnerSpriteNum != -1 ) ||
       ( roundInfo.numTracBeamsTried >= rps.maxTracBeams) ||
       ( roundInfo.bonus2ShotAwardSpriteNum != -1))
   {
     debugPuts("Trac beam did not meet conditions");
     debugFuncExit("HandleTracBeamPoint()");
     return;
   }

   // Get an unused trac beam
   s = SpriteEngFindFirstUnusedSprite( TracBeamSpriteClass );

   if (s != NULL) {
     debugPuts("  trac beam away!");
     setTracBeamMotion(s,motherShip);
     SoundMgrThisSound(tracBeam);
     roundInfo.tracBeamSpriteNum = s->spriteNum;
     roundInfo.tracBeamOwnerSpriteNum = motherShip->spriteNum;
     roundInfo.numTracBeamsTried++;
   }

   debugFuncExit("HandleTracBeamPoint()");
}

//--------------------------------------------------------------------
//  PROCVAR TARGET
//
//  
//-------------------------------------------------------------------- 
void HandleSpriteEvent(sprite * s, Int event) {

   // debug
   debugFuncEnterInt("HandleSpriteEvent(), event",event);
   debugPutInt("   sprite",s->spriteNum);
   debugPutInt("   spriteType",s->spriteType);
   
   switch (s->spriteType)
   {

      case ExplosionSpriteClass:
 
               // Handle explosion events
               debugPuts("explosion expired");
               FreeSprite(s);
               break;               
 
      case UserBulletSpriteClass:  

               // Handle bullet expiration  ---- will we ever get this, border action should suff??
	       roundInfo.numUserBulletsFlying--;
               FreeSprite(s);
               break;

      case BugBulletSpriteClass:

               // Handle bullet expiration ---- will we ever get this, border action should suff??
	       ERROR_ASSERT("bullet expired, count f'd up");
               FreeSprite(s);
               break;

      case TracBeamSpriteClass:

	   if ( event == SignalReplayTracBeamSnd) {
	     // replay warble!
              SoundMgrThisSound(tracBeam);
           } else {
	     // must be expiration
               FreeSprite(s);
	       debugPuts("trac beam expired");
               roundInfo.tracBeamSpriteNum      = -1;
               roundInfo.tracBeamOwnerSpriteNum = -1;
	   }

           break;

      case BugSpriteClass:

               // Bug is at a peeling-off point in intro flight
               if (event == SignalBugPeelingOffPoint) {
                 if ((rps.numBugsCanPeelOff!=0) && randomPercTest(30))
                 {
                    setBugPeelingOffMotion(s);
                    rps.numBugsCanPeelOff--;
		 } else {
		    // Allow this to possibly be a bullet point
		    event = SignalBulletPoint;
		 }
               }

               // Handle launch bullet
               if (event == SignalBulletPoint) {
                  HandleBulletPoint(s);
               }

               // Handle bug has flown back to formation
	       if (event == SignalBugBackInForm) {
		  HandleResetBugToFormation(s,false);
	       }

               // Handle remove bug (put it back in formation)
	       if (event == SignalRemoveSprite) {
                  HandleSpriteOverBorder(s,5); // hack
	       }

               // Handle tractor beam point
               if (event == SignalTracBeamPoint) {
                  HandleTracBeamPoint(s);
               }

               // Handle mine point
               if (event == SignalMineLayPoint) {
		 if ( roundInfo.numMinesFlying < rps.maxMinesInNormalPlay) {
                    HandleLayMinePoint(s);
		 }
               }
 
               // Barrier is done a rockin'
               if (event == SignalBarrierMotDone) {
                 roundInfo.bugs[s->spriteNum].aliveState = notMovable;
                 s->moveStructOwner=noMove;  // prevent sprite engine from moving it
               }

               break;

      case UserSpriteClass:

               // Handle we-ve been beamed up!
               if (event == SignalUserWithinMothership) {
                  HandleUserWithinMothership();
               }
	       break;
	       

      case BackDropSpriteClass:
	 // only signal for this class
	 setStarMotion(s);

	 break;
	 

      default:
               // Prevent gcc warnings
               break;


   }
 


}


//--------------------------------------------------------------------
// FreeSprite
//-------------------------------------------------------------------- 
void  FreeSprite(sprite *s)
{
   debugFuncEnterInt("FreeSprite(), s",s->spriteNum);
   
   if (notActive(s)) {
      debugPutIntError("sprite already deallocated (used==false), s",s->spriteNum);
      debugFuncExit("FreeSprite()");
      return;
   }
   
   DelFromActiveList(s); 
   
   debugFuncExit("FreeSprite()");
}


//--------------------------------------------------------------------
// Create the user (player's) ship sprite
//-------------------------------------------------------------------- 
void CreateUserSprite()
{
  
   borderActionEnum   borderAction[] = { noAction, noAction, sendSignal, noAction };

   NewSpriteInfo user=  {false,0,UserSpriteClass,userShipBmp,75,130,0,0,borderAction,noMotionSeq,0};

   user.spriteNum=numSprites;
   debugFuncEnterInt("CreateUserSprite(), s",user.spriteNum);


   // grab a misc sprite for the thrust
   userThrustSprite = SpriteEngFindFirstUnusedSprite( MiscSpriteClass );
   AddToActiveList(userThrustSprite);
   userThrustSprite->bitmap = thrust1;
   userThrustSprite->moveStructOwner = userAppl0;
   debugPutInt("  thrust sprite, s",userThrustSprite->spriteNum);

   // Create user sprite.  Put user in sprite list so it can be saved at game end.. 
   userSprite = SpriteEngCreateSprite(&user); 
   allSprites[numSprites++] = userSprite;
   AddToActiveList(userSprite);  // MUST go after allSprites has been updated (or error)


   // Set user dead (will be livened later at game start)
   gameInfo.userState = user_dead;

   // collision map... trac beam, bugs
   SpriteEngRegisterCollisionMap( UserSpriteClass ,TracBeamSpriteClass,fourCorners ); 
   SpriteEngRegisterCollisionMap( UserSpriteClass ,BugSpriteClass,fourCorners ); 
 

   // NULL out the movement structure since we will control movement
   userSprite->moveStructOwner = noMove;

   // User never moves from this region
   userSprite->collisionRegion=region1__;

   // Set used flag to false (dangerous) because if the game is saved/restored when the 
   // user is dead this causes problems.  Flags turned on in ResetUserSprite()
   DelFromActiveList(userSprite);
   DelFromActiveList(userThrustSprite);


   debugFuncExit("CreateUserSprite()");
}  

//--------------------------------------------------------------------
// CreateUserSpritePt2
//
// Finish up any initiation now that ScreenDepth has been set
//-------------------------------------------------------------------- 
void CreateUserSpritePt2()
{

   debugFuncEnter("CreateUserSpritePt2()");
   

   // Default for 2-bit, 4-bit grey
   THRUSTINITyPOS = 140;

   // Set up the thrust sequence for 1-bit
   if (ScreenDepth == oneBitDepth) {
      THRUSTINITyPOS = 140;
      
      thrustTicksBetwMotConst=3;
      thrustTicksBetwMot =3;
      thrustMotSeqMax=3;
      thrustBugMotBitmapSeq[0]=thrust2;
      thrustBugMotBitmapSeq[1]=thrust2;
      thrustBugMotBitmapSeq[2]=thrust1;
   }

   if (ScreenDepth == colorFourBitDepth) {
      THRUSTINITyPOS = 139;
   }

   debugFuncExit("CreateUserSpritePt2()");
   
}


//--------------------------------------------------------------------
//  Poll the keys, move the user sprite
//-------------------------------------------------------------------- 
void HandleUserInput() 
{ 
 
   UInt16 r,oldPos;
   sprite * s1=NULL;
   sprite * s2=NULL;
   //moveStruct * ms; 
   Byte maxUser2Bullets=MAXUSER2BULLETS;
   
   // Ensure user is not dead or other
   if (gameInfo.userState != user_normal) {
     debugPuts("User input ignored, userState != user_normal"); 
     debugFuncExit("HandleUserInput()");
     return;
   }

   // Call Palm OS KeyMgr  
   keyInput |= KeyCurrentState();
   debugFuncEnterInt("HandleUserInput() ",keyInput);

    //debug
   //debugOverrideLevel(allDebugText);

   //-------------------debug auto play----------------------
   if (AUTOPLAY) {

      r=randomNum(8);
      if (r==1) { keyInput |= MoveLeftKeyMask; }
      if (r==2) { keyInput |= MoveRightKeyMask; }
      if (r==3) { keyInput =0; }
      if (r==4) { keyInput |= FireKeyMask; }

   }
   //-------------------debug auto play----------------------

   /*
   if ((keyInput & AllKeysMask) == 0 ) {
      ButtonDebounceCtr[3]=0;
      ButtonDebounceCtr[4]=0;
      ButtonDebounceCtr[5]=0;
      ButtonDebounceCtr[6]=0;
   }
   */


   oldPos = userSprite->x;

   if ((keyInput & MoveLeftKeyMask) >0) {

     //    if (ButtonDebounceCtr[3] !=0) {
     //    ButtonDebounceCtr[3]--;
     //  } else {
	 if (userSprite->x >= MoveAmount) {
            userSprite->x-=MoveAmount;
	    //      ButtonDebounceCtr[3] = ButtonDebounceCnt;
         }
	 //    }
	 //  } else {
	 //   ButtonDebounceCtr[3]=0;

	 userSprite->realX12calc  = false;
	 userSprite->realCogXcalc = false;
	 
   }

 
   if ((keyInput & MoveRightKeyMask) >0) {

     //      if (ButtonDebounceCtr[4] !=0) {
     //   ButtonDebounceCtr[4]--;
     // } else {
	 if (userSprite->x < UserShipXMax) {
            userSprite->x+=MoveAmount;
	    //     ButtonDebounceCtr[4] = ButtonDebounceCnt;
         }
	 //   }
	 // } else {
	 //  ButtonDebounceCtr[4]=0;

	 userSprite->realX12calc  = false;
	 userSprite->realCogXcalc = false;
   }


   // Before Astron v1.00 the fire button was prevented from repeating.  Now it
   // is a configurable setting

   if ((keyInput & FireKeyMask) >0) {

      ButtonDebounceCtr[6]--;

      if (ButtonDebounceCtr[6] == 0) 
      {
	 
	 // This value control the "repeatability" of the fire button
	 ButtonDebounceCtr[6] = FireDebounceCnt;   
	 
	 if (  (!gameInfo.twoShots && (roundInfo.numUserBulletsFlying < MAXUSERBULLETS)) ||
	       (gameInfo.twoShots && (roundInfo.numUserBulletsFlying < maxUser2Bullets)))
	 {
	    // Get a bullet sprite if one is available
	    s1 = SpriteEngFindFirstUnusedSprite( UserBulletSpriteClass );
	    if (s1 != NULL) 
	    {
	       AddToActiveList(s1);
	       roundInfo.numUserBulletsFlying++;
	    }
	    
	    got_here;
	    
	    
	    // Get another if user if double firing
	    if (gameInfo.twoShots && (roundInfo.numUserBulletsFlying < MAXUSER2BULLETS ))
	    {
	       s2 = SpriteEngFindFirstUnusedSprite( UserBulletSpriteClass );
	    }
	    
	    if (s1 != NULL) {
	       
	       // put bullet at tip of ship
	       if (s2 == NULL) {
		  s1->x = oldPos+4;  
		  s1->y = userSprite->y-6;   
	       } else {
		  s1->x = oldPos-2;  
		  s1->y = userSprite->y-2;
	       }
	       
	       s1->bitmap=userBulletBitmap;
	       setQuickBulletMotion(s1,UserBulletSpeed);

	       SoundMgrThisSound(userShoots);
	    } else {
	       debugPuts("no bullet");
	    }
	    
	    // possible 2nd bullet
	    if (s2 != NULL)  {
	       AddToActiveList(s2);
	       s2->x = oldPos+10; 
	       s2->y = userSprite->y-2;
	       
	       roundInfo.numUserBulletsFlying++;
	       
	       s2->bitmap=userBulletBitmap;
	       setQuickBulletMotion(s2,UserBulletSpeed);
	    }
	    
	 }  // user has shots in the bank
      }  // debounce
      
   } else {
      ButtonDebounceCtr[6]=1;
   } 


   // Launch debug screen
   if (((keyInput & keyBitPageUp) >0) && (ALLOWDEBUGMENU)) { 
      lastForm=FrmGetActiveFormID();
      FrmGotoForm(debugScreenForm);
   }


   keyInput=0;

   // hack.. for user thrust
   userThrustSprite->x = userSprite->x + ThrustSpriteXOffset;


   debugFuncExit("HandleUserInput()");
}

//--------------------------------------------------------------------
//   
//-------------------------------------------------------------------- 
void ResetUserSprite()
{

  debugFuncEnter("ResetUserSprite()");

  // may have already been done, if so return
  if ( gameInfo.userState == user_normal) {
    debugFuncExit("ResetUserSprite()");
    return; 
  }

  debugPuts("resetting user");

  // cleanup anything modified from previous play 
  AddToActiveList(userSprite); 
  userSprite->x      = USERINITxPOS;
  userSprite->y      = USERINITyPOS;
  userSprite->bitmap = userShipBmp;
  userSprite->moveStructOwner = noMove;
  gameInfo.userState = user_normal;

  AddToActiveList(userThrustSprite);  
  userThrustSprite->x      = USERINITxPOS;
  userThrustSprite->y      = THRUSTINITyPOS;
  userThrustSprite->bitmap = thrust1;
  userThrustSprite->moveStructOwner = userAppl0; // allow animation
 

   debugFuncExit("ResetUserSprite()"); 
} 

//--------------------------------------------------------------------
//   Sprite engine is telling us a bug is over the border, handle it
//-------------------------------------------------------------------- 
void HandleSpriteOverBorder(sprite* s,Int border)
{

   debugPutInt("HandleSpriteOverBorder(), border",border);
   
   // sprites may cross over 2 borders at 1 time, if so we get called 
   // twice.  Check to see if sprite is still active to combat this
   if (notActive(s)) {
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // bug bullet...
   if (s->spriteType == (Int)BugBulletSpriteClass ) {
      roundInfo.numBulletsFlying--;
      FreeSprite(s);                               
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // mine, or 2-shot award...
   if (s->spriteType == (Int)MineSpriteClass ) {
      if (s->bitmap == mineBmp) {
         roundInfo.numMinesFlying--;
      }

      if (s->spriteNum == roundInfo.bonus2ShotAwardSpriteNum) {
         roundInfo.tracBeamOwnerSpriteNum = -1;
      }

      FreeSprite(s);                             
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // user bullet...
   if ( s->spriteType == (Int)UserBulletSpriteClass )
   { 
      roundInfo.numUserBulletsFlying--;
      FreeSprite(s);                        
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // trac beam...
   if ( s->spriteType == (Int)TracBeamSpriteClass )
   {
      FreeSprite(s);                            
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // star...
   if ( s->spriteType == (Int)BackDropSpriteClass )
   {
      setStarMotion(s);                            
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // If this is a bug which does flyback then put it into flyback  
   if (( roundInfo.bugs[ s->spriteNum ].numFlybacksAllowed > 0 ) && 
       ( border == 2 )) 
   {
      debugPutInt("starting flyback, sprite",s->spriteNum);
 
      // decr flyback, encode the motion
      roundInfo.bugs[ s->spriteNum ].numFlybacksAllowed--;
      setBugFlybackMotion(s);
  
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }
   
   // DEBUG -- when we're just watching the intro waves just delete the bug
   if (SKIPREGPLAY) {
      roundInfo.numBugsDiving--;
      FreeSprite(s);
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // Handle over border situations for non-regular play elsewhere
   if ( gameInfo.roundType != regularRound ) {
      debugPutsError("fix over-the-border");
      debugFuncExit("HandleSpriteOverBorder()");
      return;
   }

   // Cleanup the rest...
   HandleResetBugToFormation(s,true);

   debugFuncExit("HandleSpriteOverBorder()");
}

//--------------------------------------------------------------------
//   HandleResetBugToFormation
//-------------------------------------------------------------------- 
void HandleResetBugToFormation(sprite* s, Boolean doReturnAnim)
{
   debugFuncEnterInt("HandleResetBugToFormation s",s->spriteNum);


   // VERY important yo reset these flags, because the Y position flags 
   // are not reset when in formation
   s->collisionRegion = regionUnknown;
   s->realX12calc  =false; 
   s->realCogXcalc =false; 
   s->realY12calc  =false; 
   s->realCogYcalc =false;

   // Decide whether to put bug in formation or execute blitz motion
   if ((roundInfo.inBlitzMode == false) || 
       (gameInfo.userState    != user_normal)) 
   {

      roundInfo.numBugsDiving--;

      // OK, at this point we have a bug that needs to be returned to the formation...
      // reset x,y and allow the userApp to move the bug 

      s->x = roundInfo.bugs[s->spriteNum].origX;  // need to add movement factor
      s->y = roundInfo.bugs[s->spriteNum].origY;
      s->moveStructOwner=userAppl0;

      if (roundInfo.bugs[s->spriteNum].bugType == barrierBmp) {
	roundInfo.bugs[s->spriteNum].aliveState = notMovable;
      } else {
        roundInfo.bugs[s->spriteNum].aliveState = inFormation;

	// Set up the in-formation sequence -- if required
	if (doReturnAnim) {
	   s->myMoveStruct.countdown = inFormTicksBetwMotConst;
	   s->myMoveStruct.leg[0].guts.inFormSeqStruct.restoreBitmap = s->bitmap;
	   s->myMoveStruct.leg[0].guts.inFormSeqStruct.currSequence = 0;
	   
	   s->bitmap = inFormMotBitmapSeq[0];
	   s->moveStructOwner = userAppl1;
	}
	
      }

      // THIS MAY ACTUALLY BE A BUG HERE... WE MAY BE INVALIDATING THE BOUNCE SIGNAL, ETC..
      // border action may have been changed due to a fly-in intro, reset it 
      s->borderAction[0]=noAction;    // no signal because sprite is sometimes hidden w/ y<0 
      s->borderAction[1]=sendSignal;
      s->borderAction[2]=sendSignal;
      s->borderAction[3]=sendSignal;
   } else {

      // New code... start blitz movement for this bug.  numBugsDiving, aliveState,
      // does not need to be changed since dive continues
      s->x = 40+randomNum(80);  
      s->y = -5;
      setBlitzMotion(s);   
   } 

   debugPutInt("  s->x",s->x);
   debugPutInt("  s->y",s->y);


   //debugPuts("done");
   debugFuncExit("HandleResetBugToFormation()");
}


//-----------------------------------------------------------------------
//  DoCountAudits
//
//  Debug-only proc to see if counts are not accurate
//-----------------------------------------------------------------------
void DoCountAudits()
{

  debugFuncExit("DoCountAudits()");
  return;

  debugFuncEnter("DoCountAudits()");


  if ((gameInfo.gameState == gamePlay) || 
      (gameInfo.gameState == introWaves) ||
      (gameInfo.gameState == waitForFormation) ||
      (gameInfo.gameState == waitForEnemyDivingOver) ||
      (gameInfo.gameState == reintroducePlayer) ||
      (gameInfo.gameState == restAfterGamePlay))
  {

  debugOverrideLevel(allDebugText);

 
  debugPuts("-----");
  debugPutInt("  num free user bullets",SpriteEngInfoNumFreeSprites(UserBulletSpriteClass));
  debugPutInt("  num free bugs",SpriteEngInfoNumFreeSprites(BugSpriteClass));
  debugPutInt("  num free bug bullets",SpriteEngInfoNumFreeSprites(BugBulletSpriteClass));
  debugPutInt("  num free mines",SpriteEngInfoNumFreeSprites(MineSpriteClass));


  if ((roundInfo.numRemainBugs + roundInfo.numBarriers) != 
      (MAXNUMBUGS - SpriteEngInfoNumFreeSprites(BugSpriteClass)) )
  {
    debugDumpRoundInfoNonBugs();
    ERROR_ASSERT("NEAL: mismatch in numRemainBugs");
  }

  if (roundInfo.numMinesFlying != (MAXNUMMINES - SpriteEngInfoNumFreeSprites(MineSpriteClass)))
  {
    // Need to take into account here that the 2-shot bonus is in the mine class
    debugDumpRoundInfoNonBugs();
    //ERROR_ASSERT("NEAL: mismatch in numMinesFlying");
  }

  if (roundInfo.numBulletsFlying != (MAXNUMBUGBULLETS - SpriteEngInfoNumFreeSprites(BugBulletSpriteClass)))
  {
    debugDumpRoundInfoNonBugs();
    ERROR_ASSERT("NEAL: mismatch in numBulletsFlying");
  }

  if (roundInfo.numUserBulletsFlying != (MAXNUMUSERBULLETS - SpriteEngInfoNumFreeSprites(UserBulletSpriteClass)))
  {
    debugDumpRoundInfoNonBugs();
    ERROR_ASSERT("NEAL: mismatch in numBulletsFlying");
  }

  if (roundInfo.numBugsDiving > (MAXNUMBUGS - SpriteEngInfoNumFreeSprites(BugSpriteClass)))
  {
    debugDumpRoundInfoNonBugs();
    ERROR_ASSERT("NEAL: mismatch in numBugsDiving");
  }

  debugRestoreLevel();

  }

   debugFuncExit("DoCountAudits()");
}

//-----------------------------------------------------------------------
//  ASSUMES RECS are NOT already present
//-----------------------------------------------------------------------
Boolean SaveBlastRecs()
{

    Int i,count=0;
    UInt prevRecIndex=999,index=9999;   
    UInt16 err,offset=0;
    UInt32 uniqId=blastRecUniqId;
    VoidHand recHdl=NULL;
    VoidPtr ptr;
    ULong prevRecUniqId=0;

    spriteSaveStruct        spriteSaveBlock; 
    masterSpriteSaveStruct  masterSpriteSaveBlock;

    debugFuncEnter("SaveBlastRecs()");

    debugPutInt("  numSprites",numSprites);

    // Loop through allSprites, and save the USED=true sprites.  Records get saved from
    // last-to-first (they are retrieved in opposite order)...
    for (i=0;i<numSprites;i++)
    {
      if (isActive(allSprites[i])) {
	count++;

	// Clear the collision flags since they may not be valid in mid-cycle
	allSprites[i]->collisionRegion = regionUnknown;
	allSprites[i]->realCogXcalc = false;
	allSprites[i]->realCogYcalc = false;
	allSprites[i]->realX12calc  = false;
	allSprites[i]->realY12calc  = false;
 

	spriteSaveBlock.savedSpriteStruct = *allSprites[i];
	spriteSaveBlock.nextRecIndex      = prevRecIndex;
	spriteSaveBlock.nextUniqId        = prevRecUniqId;

	debugPutInt("  saving sprite",spriteSaveBlock.savedSpriteStruct.spriteNum);

        offset=0;
	index=9999;

        recHdl = DmNewRecord(dbRef,&index,sizeof(spriteSaveStruct)); 
	debugPutInt("  index",index);
	prevRecIndex = index;

        ptr    = MemHandleLock(recHdl);

        err = DmWrite(ptr, offset, &spriteSaveBlock, sizeof(spriteSaveBlock));
        ErrFatalDisplayIf(err, "Could not write sprite record.");
 
	// Read the unique ID
	err = DmRecordInfo(dbRef,index,0,&prevRecUniqId,0);

	debugPutUInt32Hex("  uniqId",prevRecUniqId);

        MemPtrUnlock(ptr);
        DmReleaseRecord(dbRef, index, true);

      } else { // if used
	debugPutInt("  sprite is NULL",allSprites[i]->spriteNum);
      }
    }  // for 

    got_here;

    // Save the master sprite record...
    masterSpriteSaveBlock.numSprites       = count;
    masterSpriteSaveBlock.firstSpriteIndex = prevRecIndex; // link to 1st sprite block
    masterSpriteSaveBlock.firstSpriteUniqId = prevRecUniqId;

    debugPutInt("  1st index",masterSpriteSaveBlock.firstSpriteIndex);

    offset=0;

    recHdl = DmNewRecord(dbRef,&index,sizeof(masterSpriteSaveStruct)); 
    debugPutInt("  masterSpriteSaveBlock index",index);

    ptr    = MemHandleLock(recHdl);

    err = DmWrite(ptr, offset, &masterSpriteSaveBlock, sizeof(masterSpriteSaveStruct));
    ErrFatalDisplayIf(err, "Neal: Could not write master Blast record.");

    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);

    got_here;

    // Create, lock a new gameInfo record 
    uniqId = blastRecUniqId;
    DmSetRecordInfo(dbRef, index, NULL, &uniqId);

    debugFuncExit("SaveBlastRecs()");
    return true;


}


//-----------------------------------------------------------------------
//  BlastAppStopCleanup
//
//  Free all dynamic memory held by sprites.
//-----------------------------------------------------------------------
void BlastAppStopDoCleanup()
{

   Int i;

   debugFuncEnter("BlastAppStopCleanup()");

   for (i=0;i<numSprites;i++)
   {
       if (allSprites[i] != NULL) {
	 SpriteEngDeleteSprite(allSprites[i]);
	 allSprites[i] = NULL;
       }
   }


   debugFuncExit("BlastAppStopCleanup()");
}


//-----------------------------------------------------------------------
// Read the records from 
//-----------------------------------------------------------------------
Boolean RestoreBlastRecs()
{

    UInt index=0;
    UInt16 err,spriteNum,i;
    UInt32 uniqId=gameInfoRecUniqId;
    VoidHand recHdl=NULL;
    VoidPtr ptr;

    spriteSaveStruct        spriteSaveBlock;
    masterSpriteSaveStruct  masterSpriteSaveBlock;

    debugFuncEnter("RestoreBlastRecs()");

    // MASTER BLAST
    uniqId=blastRecUniqId;
    err = DmFindRecordByID(dbRef,uniqId,&index);

    if (err == dmErrUniqueIDNotFound) {
       debugPuts("  master Blast record NOT present");
       debugFuncExit("RestoreBlastRecs()");
       return false;
    }

    debugPutInt("  master Blast record IS present, index",index);


    recHdl = DmGetRecord(dbRef,index);
    ptr    = MemHandleLock(recHdl);
    MemMove(&masterSpriteSaveBlock,ptr,sizeof(masterSpriteSaveBlock));
    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);

    debugPutInt("  numSprites",masterSpriteSaveBlock.numSprites);
    uniqId = masterSpriteSaveBlock.firstSpriteUniqId;


    // INDIV SPRITE BLOCKS
    for (i=0;i<masterSpriteSaveBlock.numSprites;i++)
    {
       // Retrieve by uniqId
       err = DmFindRecordByID(dbRef,uniqId,&index);

       // check err
       if (err) {
	 debugPuts("Error, not all sprite records were found");
	 debugFuncExit("RestoreBlastRecs()");
	 return false;
       }

       recHdl = DmGetRecord(dbRef,index);

       ptr    = MemHandleLock(recHdl);
       MemMove(&spriteSaveBlock,ptr,sizeof(spriteSaveBlock));
       MemPtrUnlock(ptr);
       DmReleaseRecord(dbRef, index, true);

       //debugPutInt("index",index);

       // Now put the block into the allSprites struct
       spriteNum=spriteSaveBlock.savedSpriteStruct.spriteNum;

       ptr = &spriteSaveBlock.savedSpriteStruct;
       MemMove(allSprites[spriteNum],ptr,sizeof(spriteSaveBlock.savedSpriteStruct));

       // Cleanup the active list after a restore
       if isActive(allSprites[spriteNum]) {
	  spriteList[allSprites[spriteNum]->spriteType].numActive++;
       }
       
	     

       debugPutInt("  nextSprite",allSprites[spriteNum]->nextSprite);
       debugPutInt("  restored sprite",spriteNum);

       uniqId = spriteSaveBlock.nextUniqId;

    }

    debugFuncExit("RestoreBlastRecs()");
    return true;

}


//-----------------------------------------------------------------------
// BackDropMgrInit
//-----------------------------------------------------------------------
void BackDropMgrInit()
{
   
   sprite * s = NULL;
   Byte i;
   Byte initY[]={23,118,80,64,17,95,   74,105,34};
  
   debugFuncEnter("BackDropMgrInit");

   // No backdrop for 1-bit 
   if (ScreenDepth == oneBitDepth) {
      debugFuncExit("BackDropMgrInit");
      return;
   }
   
   // Backdrop sprites come in 2 classes: slow and fast
   for (i=0;i<MAXNUMBACKSPRITES;i++) 
   {
      s = SpriteEngFindFirstUnusedSprite( BackDropSpriteClass );
      
      if (s != NULL) {

	 AddToActiveList(s);
	 setStarMotion(s);

	 // Init the y to space across screen
	 s->y = initY[i] + randomNum(10); 
      }
   }

   // Set the star speeds. Stars move slower on greyscale so they can be seen
   fastStarSpeed = (ScreenDepth == colorFourBitDepth) ? 4:2;
   slowStarSpeed = (ScreenDepth == colorFourBitDepth) ? 2:1;

   debugFuncExit("BackDropMgrInit");
   
}









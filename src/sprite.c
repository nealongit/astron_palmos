//
// SPRITE ENGINE
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
 
//--------------------------------------------------------------------
//  SAVE-ABLES: spriteList
//--------------------------------------------------------------------

// Palm OS calls, types
#include <PalmOS.h>     // was Pilot.h in pre-3.5 OS
#include <UIPublic.h>   // was UICommon.h in pre-3.5 OS
#include <PalmTypes.h>   // was Common.h in pre-3.5 OS
#include <KeyMgr.h>     // For key polling
#include <TimeMgr.h>    // For getting time ticks
 
// My header files
#include "starter.h"
#include "graphics.h"
#include "sprite.h" 
#include "debug.h"
#include "blast.h" 
#include "gameman.h"
#include "defs.h"
   

// Math: 2^n function
UInt16 PowersOf2[] = {1,2,4,8,16,32,64,128,256,512,1024,2048};


// ASM vars
sprite * ASM_thisSprite;
sprite * ASM_targSprite;
UInt16 ASM_COGTestResult;
bmpStruct * ASM_thisBmpTablePtr;
bmpStruct * ASM_targBmpTablePtr;


UInt16   SpriteEngHighestSpriteNum=0;
UInt16  SpriteEngNumSpritesRendered=0;

/*
UInt16 OMcollisionTests=0;
UInt16 OMfourCornerLevel1Checks=0;
UInt16 OMfourCornerLevel2Checks=0;
UInt16 OMcogLevel1Checks=0;
UInt16 OMcogLevel2Checks=0;
UInt16 OMCogXcalculations=0;
UInt16 OMCogYcalculations=0;
UInt16 OMnotInSameRegion=0;
UInt16 OMRealX12calc=0;
UInt16 OMRealY12calc=0;
*/


//--------------------------------------------------------------------
// Prototype
//--------------------------------------------------------------------
Boolean asmSpriteEngCOGTest();


//--------------------------------------------------------------------
// Main db structure for SpriteEng vars
//--------------------------------------------------------------------
 
// Array of linked lists of all sprites
spriteCategoryStruct   spriteList[spriteClassMax];
UInt16                 spriteRenderDepthList[spriteClassMax];

// Array of motion sequence
motionBitmapStruct     bitmapMotionSeq[maxNumMotionSeq_CONST];




//--------------------------------------------------------------------
//  Procvars
//--------------------------------------------------------------------
void (*HandleSpriteCollisionFuncPtr)(sprite*,Int,sprite*,Int);
void (*HandleSpriteEventFuncPtr)(sprite*,Int);
void (*HandleSpriteMovementNeededFuncPtr)(sprite*);
void (*HandleSpriteExceededBorderFuncPtr)(sprite*,Int);

// do this to aid perl code detailer
//{
//}

//--------------------------------------------------------------------
// SpriteEngInit
//
//--------------------------------------------------------------------
void SpriteEngInit() {

   Word i;
   motionBitmapStruct noMotion = {0,0,0,0,{0}};

   // debug
   debugFuncEnter( "SpriteEngInit()" );
   

   // Null out the sprite catelog
   for (i=0;i<spriteClassMax;i++) 
   {
      spriteList[i].collisionBitmap   = 0;
      spriteList[i].collisionMethod   = dontCare;
      spriteList[i].numActive         = 0;
      spriteList[i].firstSprite       = endOfClassList;

      spriteRenderDepthList[i]=i;
   }
 

   // Null out the motion bitmaps
   for (i=0;i<maxNumMotionSeq_CONST;i++)
   {
      bitmapMotionSeq[i] = noMotion;
   }



   debugFuncExit( "SpriteEngInit()" );

}


//--------------------------------------------------------------------
// SpriteEngCreateSprite
//
// returns ptr to new sprite.
//-------------------------------------------------------------------- 
sprite * SpriteEngCreateSprite( NewSpriteInfo * n ) {


   Int i;
   sprite            * nsp;   // new sprite ptr
        
   debugFuncEnterInt("SpriteEngCreateSprite(), s",n->spriteNum); 

   // Alloc memory for the new sprite
   nsp = (sprite*)MemPtrNew(sizeof(sprite));

   if (!nsp) {
      // error
      debugPuts("error, MemPtrNew failed");
      return NULL;
   }

   // Remember highest sprite num
   if (n->spriteNum > SpriteEngHighestSpriteNum) {
      SpriteEngHighestSpriteNum = n->spriteNum;
   }
   

   // Init the sprite.  Easy fields..
   nsp->usedState  = false;
   nsp->spriteNum  = n->spriteNum;
   nsp->spriteType = n->spriteType;
   nsp->bitmap     = n->bitmap;
   nsp->nextSprite = endOfClassList;
   nsp->x          = n->x;
   nsp->y          = n->y;
         
   nsp->moveStructOwner = noMove;

   // ..border action 
   for (i=0;i<4;i++) {
     nsp->borderAction[i] = n->borderAction[i];
   }

   // Flags used during collision detection
   nsp->collisionRegion = regionUnknown;
   nsp->realCogXcalc = false;   
   nsp->realCogYcalc = false;
   nsp->realX12calc = false;
   nsp->realY12calc = false;

   
   
   // Place sprite in catalog (at head)
   if ( spriteList[n->spriteType].firstSprite == endOfClassList ) {
      // 1st sprite in list
      spriteList[n->spriteType].firstSprite = nsp->spriteNum;

   } else {
      // Bump current sprites down the list
      nsp->nextSprite = spriteList[n->spriteType].firstSprite;
      spriteList[n->spriteType].firstSprite = nsp->spriteNum;
   } 

   // Return a pointer to sprite
   return nsp;

};

//--------------------------------------------------------------------
// SpriteEngDeleteSprite
//
// Free dynamic memory held by sprite
//-------------------------------------------------------------------- 
void SpriteEngDeleteSprite( sprite * s ) {

  Err err;

  if (s != NULL) {
     debugFuncEnterInt("SpriteEngDeleteSprite, MemPtrFree   s",s->spriteNum);
     err = MemPtrFree(s);
  }

}


//--------------------------------------------------------------------
//  Store the bitmap motion sequence in the table
//--------------------------------------------------------------------
void SpriteEngRegisterBitmapMotionSeq(Int i, motionBitmapStruct mps)
{

   // debug
   debugFuncEnter( "SpriteEngRegisterBitmapMotionSeq()" );


   // Handle index errors
   if ((i<0) || (i >= maxNumMotionSeq_CONST)) 
   {
      debugPutInt("error, index exceeded max i",i);
      debugFuncExit("*()");
      return;
   } 

   // store the motion sequence
   bitmapMotionSeq[i] = mps;
   bitmapMotionSeq[i].currentTickCount = bitmapMotionSeq[i].ticksBetwMotionFrames; // set 1st timer
   

   debugFuncExit("SpriteEngRegisterBitmapMotionSeq()");
}


//--------------------------------------------------------------------
// SpriteEngFindFirstUnusedSprite
//-------------------------------------------------------------------- 
sprite *  SpriteEngFindFirstUnusedSprite(Int  thisClass)
{
   sprite * s=0;
   
      
   debugFuncEnterInt("SpriteEngFindFirstUnusedSprite(), thisClass",thisClass);
   s= SpriteEngFindFirstSprite(thisClass,false);
   
   if (s != NULL) {
    // Important to reset these or mystery collisions occur
      s->realCogXcalc = false;    
      s->realCogYcalc = false;
      s->realX12calc = false;
      s->realY12calc = false;
  }
   
   debugFuncExit("SpriteEngFindFirstUnusedSprite()");
   return s;
  
}

//--------------------------------------------------------------------
// SpriteEngSetRenderOrder
//-------------------------------------------------------------------- 
void SpriteEngSetRenderOrder(UInt16 * renderListPtr)
{
   UInt16 i;
   
   for (i=0;i<spriteClassMax;i++) {
      spriteRenderDepthList[i]=renderListPtr[i];
   }
   
   
}


//--------------------------------------------------------------------
// SpriteEngFindFirstUsedSprite
//-------------------------------------------------------------------- 
sprite *  SpriteEngFindFirstUsedSprite(Int  thisClass)
{
  debugFuncEnterInt("SpriteEngFindFirstUsedSprite(), thisClass",thisClass);
  debugFuncExit("SpriteEngFindFirstUsedSprite()");
  return SpriteEngFindFirstSprite(thisClass,true);

}

//--------------------------------------------------------------------
// SpriteEngFindFirstSprite
//-------------------------------------------------------------------- 
sprite *  SpriteEngFindFirstSprite(
  Int      thisClass,
  Boolean  state
)
{
    sprite * s;
    Int16 thisIndex = spriteList[thisClass].firstSprite;

    debugFuncEnterInt("SpriteEngFindFirstSprite(), thisClass",thisClass);


    while (thisIndex != endOfClassList)
    {
       s=allSprites[thisIndex];

       if (s->usedState == state) {
          debugFuncExit("SpriteEngFindFirstSprite() happy");
          return s;
       } else {
	  thisIndex=s->nextSprite;
       }
    }

    // No free sprite in this class
    debugFuncExit("SpriteEngFindFirstSprite() sad");
    return NULL;

}

//--------------------------------------------------------------------
// SpriteEngInfoNumUnusedSprites
//
// Info proc to count number of free (unused) sprites in a class
//-------------------------------------------------------------------- 
Int16 SpriteEngInfoNumFreeSprites(
  Int      thisClass
)
{
    sprite * s;
    Int16 thisIndex = spriteList[ thisClass ].firstSprite, count=0;

    while (thisIndex != -1)
    {
       s=allSprites[thisIndex];

       if (notActive(s)) { count++; }

       thisIndex=s->nextSprite;
    }

    debugFuncExit("Info()");
    return count;

}


//--------------------------------------------------------------------
// SpriteEngFindFirstUnusedSprite
//
// Return first unused sprite in a specific class.
//-------------------------------------------------------------------- 
sprite *  SpriteEngFindFirstUnusedSpriteOrForce(Int thisClass)
{
  sprite * s = NULL;

  debugFuncEnterInt("SpriteEngFindFirstUnusedSpriteOrForce(), thisClass",thisClass);
  s = SpriteEngFindFirstUnusedSprite(thisClass);

  if ((s == NULL) && (spriteList[thisClass].firstSprite != endOfClassList ))
  {
     // Grab 1 by force
     s = allSprites[spriteList[ thisClass ].firstSprite];
     DelFromActiveList(s);
  }

  debugFuncExit("SpriteEngFindFirstUnusedSpriteOrForce()");
  return s;

}

//--------------------------------------------------------------------
// SpriteEngFindThisSpriteType
//-------------------------------------------------------------------- 
sprite *  SpriteEngFindThisSpriteType
(
   Int      thisClass, 
   Int      thisBmp,
   sprite * startHere
)
{

    sprite * s;

    if (startHere == NULL) {
      if (spriteList[ thisClass ].firstSprite == -1) {
        // There are no sprites in this class
	debugFuncExit("SpriteEngFindThisSpriteType()");
	return NULL;
      }
       s = allSprites[spriteList[ thisClass ].firstSprite];

    } else {
       s= startHere;
    }

    // walk the sprite list...
    while (s != NULL)
    {
       if (isActive(s) && (s->bitmap == thisBmp)) {
          debugFuncExit("SpriteEngFindThisSpriteType()");
          return s;
       } else {
	 if (s->nextSprite == -1) {
           // sprite type not in this class
	   debugFuncExit("SpriteEngFindThisSpriteType()");
	   return NULL;
	 }

         s=allSprites[s->nextSprite];
       }
    }

    debugPutsError("bad exit from proc");
    debugFuncExit("SpriteEngFindThisSpriteType()");

    return NULL; // shouldn't reach here

}



//--------------------------------------------------------------------
// SpriteEngHandleTick
//
// Handle timer tick for Engine.  CONSOLIDATED TO PROVIDE BETTER SPEED
// (SpriteHandleTick folded in).
//-------------------------------------------------------------------- 
void SpriteEngHandleTick_OLD() {

   /*
   Word i;
   sprite * s;
   Int16 thisIndex;
   UInt8 numHandled=0;
   
   
   // debug
   debugFuncEnter( "SpriteEngHandleTick()" );

   // reset counter
   SpriteEngNumSpritesRendered=0;

   // Walk the list,let each sprite handle tick
   for (i=0;i<spriteClassMax;i++) {

      thisIndex = spriteList[i].firstSprite;
      numHandled = 0;

      debugPutInt("spriteList[i].numActive",spriteList[i].numActive);
      
      while ((thisIndex != -1) && 
	     (numHandled < spriteList[i].numActive)) 
      {

 	 s=allSprites[thisIndex];

	 if (isActive(s)) {
	    SpriteEngNumSpritesRendered++;
	    numHandled++;
	        
	    // Perform movement, check border movement
	    if ( s->moveStructOwner == sprEng) {
	       SpriteDoNewMovement(s); 	       
	       //SpriteCheckBorderViolations(s);
	    } 
         }

         thisIndex = s->nextSprite;
      }

   }
   */


};



//--------------------------------------------------------------------
//
// UNCALLED!!!!!!!!
//
//-------------------------------------------------------------------- 
void SpriteEngUpdateMotionSeq()
{

   Int i;

   // debug
   debugFuncEnter( "SpriteEngUpdateMotionSeq()" );

   // Null out the motion bitmaps
   for (i=0;i<maxNumMotionSeq_CONST;i++)
   {
      // See if we have a valid bitmap sequence
      if (bitmapMotionSeq[i].maxMotionBitmaps !=0) {

         bitmapMotionSeq[i].currentTickCount--;

         // If time has expired rotate the bitmap         
         if (bitmapMotionSeq[i].currentTickCount == 0) {

            // debug
            debugPutInt("motion bitmap rotated currentMotionIndex",bitmapMotionSeq[i].currentMotionIndex);

            // Reset the timer, rotate the bitmap
            bitmapMotionSeq[i].currentTickCount   = bitmapMotionSeq[i].ticksBetwMotionFrames;
            bitmapMotionSeq[i].currentMotionIndex = (++bitmapMotionSeq[i].currentMotionIndex %
                                                       bitmapMotionSeq[i].maxMotionBitmaps);
	 }
      }

   }

   debugFuncExit("SpriteEngHandleTick()");
}
  

//--------------------------------------------------------------------
//  SpriteLoadNextMovementLeg
//--------------------------------------------------------------------
inline Boolean SpriteLoadNextMovementLeg(sprite * s)
{

    moveStruct * ms;

    debugFuncEnterInt("SpriteLoadNextMovementLeg(), s",s->spriteNum);

    ms = &s->myMoveStruct;

    // Ensure that there are more legs to load..
    if ((ms->currentLeg != -1) && (ms->sendThisEvent == SignalRemoveSprite)) {
      debugPutInt("  no more legs (SignalRemoveSprite), s",s->spriteNum);  
      HandleSpriteExceededBorderFuncPtr(s, 5);
      debugFuncExit("SpriteLoadNextMovementLeg()");
      return false;
    }

    if (ms->currentLeg != MAXLEGS) {
       // note the signal from the sprite is skipped...
       ms->currentLeg++;
       ms->sendThisEvent=ms->leg[ms->currentLeg].sendThisEvent;
       ms->countdown=    ms->leg[ms->currentLeg].duration;  
       debugFuncExit("SpriteLoadNextMovementLeg()");
       return true;
    } else {
      debugPutIntError("More than MAXLEGS for this bug, check behav.c s",s->spriteNum);
      HandleSpriteExceededBorderFuncPtr(s, 5);
      debugFuncExit("SpriteLoadNextMovementLeg()");
      return false;
    }

    debugFuncExit("SpriteLoadNextMovementLeg()");
    return false;
}


//--------------------------------------------------------------------
//  SpriteEngHandleTick
//
//  Consolidated 3 functions into 1 gave great time savings.  Inlining
//  did not work well here.
//
//-------------------------------------------------------------------- 
void SpriteEngHandleTick()
{

    moveStruct    * ms ; 
    Int           l,userX,targX;
    Int16 lastX,lastY;
    //collisionRegionEnum cr;
    
    Word i;
    sprite * s;
    Int16 thisIndex;
    UInt8 numHandled=0;
   
   
   // debug
   debugFuncEnter( "SpriteEngHandleTick()" );

   // reset counter
   SpriteEngNumSpritesRendered=0;

   // Walk the list,let each sprite handle tick
   for (i=0;i<spriteClassMax;i++) {
      
      thisIndex = spriteList[i].firstSprite;
      numHandled = 0;
      
      debugPutInt("spriteList[i].numActive",spriteList[i].numActive);
      
      while ((thisIndex != -1) && 
	     (numHandled < spriteList[i].numActive)) 
      {

 	 s=allSprites[thisIndex];

	 if (isActive(s)) {

	    SpriteEngNumSpritesRendered++;
	    numHandled++;
	        

	    //
	    //  START OF SpriteDoNewMovement
	    //

	    // Perform movement, check border movement
	    if ( s->moveStructOwner == sprEng) {

	       //debugPutInt("   sprite", s->spriteNum);
	       //debugPutInt("   spriteType", s->spriteType);       
	       
	       // Load the active movement structure
	       ms = &s->myMoveStruct;
	       
	       // If NULL no movement is needed
	       if (ms == NULL) {
		  debugPutInt("error ...nil movement struct for sprite",s->spriteNum);
		  debugFuncExit("SpriteDoNewMovement()");
		  //return;
	       }
	       
	       
	       // Load the 1st leg if needed
	       if (ms->currentLeg == -1) {
		  debugPutInt("   load 1st leg for sprite",s->spriteNum);
		  
		  if (!SpriteLoadNextMovementLeg(s)) {
		     // error occured, exit
		     debugFuncExit("SpriteDoNewMovement()");
		     //return;
		  }
	       }
	       
	       // Remember old x and y position so we can see if they changed
	       lastX=s->x;
	       lastY=s->y;
	       
	       l = ms->currentLeg;
	       debugPutInt("   leg, l",l);
	       debugPutInt("   legType",ms->leg[l].legType);
	       debugPutInt("   ms->countdown",ms->countdown);
	       
	       switch (ms->leg[l].legType)
	       {
		  
		  case staticLegYIncr:
		     s->y = s->y + ms->leg[l].guts.staticLegStruct.deltaY; 
		     s->realY12calc  = false;
		     s->realCogYcalc = false; 
		     break;
		     
		  case staticLegYDecr:
		     s->y = s->y + ms->leg[l].guts.staticLegStruct.deltaY;  
		     s->realY12calc  = false;
		     s->realCogYcalc = false; 
		     break;
		     
		  case staticLeg:
		     s->x = s->x + ms->leg[l].guts.staticLegStruct.deltaX;  
		     s->y = s->y + ms->leg[l].guts.staticLegStruct.deltaY;  
		     
		     // Now see if x or y was changed...
		     s->realX12calc  = ((s->realX12calc)  && (s->x == lastX));
		     s->realCogXcalc = ((s->realCogXcalc) && (s->x == lastX));
		     s->realY12calc  = ((s->realY12calc)  && (s->y == lastY));
		     s->realCogYcalc = ((s->realCogYcalc) && (s->y == lastY));
		     break;
		     
		  case noMovement:
		     // No changes to x,y position -- typically explosions
		     break;
		     
		     
		  case hideFromView:
		     
		     // Ensure we don't trip border checking while we are hidden.  THIS IS NOT
		     // VERY ROBUST (misflying sprites will now not trip y<0 checking)
		     // SHOULD PROBABLY CHANGE SOMETIME.
		     s->borderAction[0]=noAction; 
		     s->y=-20;
		     s->x=0;
		     
		     // See if we should restore
		     if ( ms->countdown == 1) {
			s->x = ms->leg[l].guts.hideFromViewStruct.restoreX;
			s->y = ms->leg[l].guts.hideFromViewStruct.restoreY;
			
			s->realY12calc  = false;
			s->realCogYcalc = false; 
			s->realX12calc  = false;
			s->realCogXcalc = false; 
		     }
		     
		     break;
		     
		  case homeToLeg:
		     if (s->y <= rps.keepCourseCeiling) {
			
			// debug
			//ERROR_ASSERT("homeToLeg");
			//s->bitmap=9;
			
			// bad... coupling to blast.c, (GetUserSprite) hard coding of user sprite
			debugPuts("doing homeToLeg");
			userX = GetUserSprite()->x + bmpTableNew[GetUserSprite()->bitmap].centerX;
			targX = s->x + bmpTableNew[s->bitmap].centerX;
			
			if (userX+20 > targX) {
			   // move right
			   ms->leg[l].guts.staticLegStruct.deltaX = 
			      ms->leg[l].guts.staticLegStruct.deltaX;
			} else { 
			   if (userX < targX+20) {
			      // move left
			      ms->leg[l].guts.staticLegStruct.deltaX = 
				 -ms->leg[l].guts.staticLegStruct.deltaX; 
			   } else {
			      
			      // drop down 
			      ms->leg[l].guts.staticLegStruct.deltaX = 0;
			   }
			}
			
			// now set to static movement
			ms->leg[l].legType = staticLeg;
			s->y = s->y + ms->leg[l].guts.staticLegStruct.deltaY;
			
		     } else {
			debugPuts("too low to do homeToLeg");
			
			// Trick the movement code by backing up to the previous leg
			// and letting the bug fly on
			ms->currentLeg--;
			ms->countdown=200;
			s->y+=2;
		     }
		     
		     s->realY12calc  = false;
		     s->realCogYcalc = false; 
		     s->realX12calc  = false;
		     s->realCogXcalc = false; 
		     
		     break;
		     
		  default:
		     debugPutInt("error ....hit default legType",ms->leg[l].legType);
		     debugPutInt("leg number l",l);
		     
	       }
	       
	       
	       // Decr the countdown if this is not userAppl controlled movement struct
	       // and move to next leg if needed...
	       
	       ms->countdown--;
	       
	       if (ms->countdown == 0) {
		  
		  // This leg is finished, send event if available
		  if (ms->sendThisEvent != SignalNoSignal)
		  {
		     debugPutInt("leg finished, sending event",ms->sendThisEvent);   
		     HandleSpriteEventFuncPtr( s, ms->sendThisEvent );
		  } 
		  
		  // Move to next leg, check is needed because HandleSpriteEventFuncPtr
		  // may have changed "ownership" of sprite, or Free'd (used==false) the sprite
		  if ( (s->moveStructOwner == sprEng) && isActive(s) ) {

		     // Don't do keepCourseCeiling checking right now
		     if (s->y <= 300) {
		     //  if (s->y <= rps.keepCourseCeiling) {
			// BAD... coupling to gameman.c
			if (SpriteLoadNextMovementLeg(s)) {
			   debugPutInt("duration",ms->leg[ms->currentLeg].duration);
			}
		     } else {
			// bug too low to change course, keep it flying same direction
			debugPuts("  bug to low to change course");
			ms->countdown=    ms->leg[ms->currentLeg].duration;
		     }
		  }
		  
	       }
  
	       
	       // Set the region fields -- optimized for speed
	       if (s->y >= region2l) {
		  s->collisionRegion=region1__;
	       } else {
		  if (s->y >= region1h) {
		     s->collisionRegion=region12_;
		  } else {
		     if (s->y >= region3l) {
			s->collisionRegion=region_2_;
		     } else {
			if (s->y >= region2h) {
			   s->collisionRegion=region_23;
			} else {
			   s->collisionRegion=region__3;
			}
		     }
		  }
	       }   

       

	       //
	       // START OF SpriteCheckBorderViolations(s)
	       // 

	       
	       // Handle exceeding right border 
	       if ( s->x > xMaxBorder ) { 
		  debugPuts("   x>xMaxBorder");
		  
		  if (s->borderAction[RTBORDER] == wrap) {
		     s->x = (s->x % 160); 
		  } else if (s->borderAction[RTBORDER] == sendSignal) {
		     HandleSpriteExceededBorderFuncPtr(s, RTBORDER);
		  } else if (s->borderAction[RTBORDER] == bounce) {
		     // move to nest leg
		     if (SpriteLoadNextMovementLeg(s)) {
			s->x = xMaxBorder-2;
		     }
		  } else if (s->borderAction[RTBORDER] == bounceIncr2) {
		     // Skip over next leg
		     if (s->y < 90) { 
			ms->currentLeg++;
		     }
		     
		     if (SpriteLoadNextMovementLeg(s)) {
			s->x = xMaxBorder-2;
		     }
		  }

	       }
	       
	       
	       // Handle exceeding left border
	       if ( s->x < xMinBorder ) {
		  debugPuts("   x<xMinBorder");
		  
		  if (s->borderAction[LFBORDER] == wrap) {
		     s->x = (s->x + 160); 
		  } else if (s->borderAction[LFBORDER] == sendSignal) {
		     HandleSpriteExceededBorderFuncPtr(s, LFBORDER);
		  } else if (s->borderAction[LFBORDER] == bounce) {
		     // move to nest leg
		     if (SpriteLoadNextMovementLeg(s)) {
			s->x = xMinBorder;
		     }
		  } else if (s->borderAction[RTBORDER] == bounceIncr2) {
		     // Skip over next leg
		     if (s->y < 90) { 
			ms->currentLeg++;
		     }
		     
		     if (SpriteLoadNextMovementLeg(s)) {
			s->x = xMinBorder;
		     }
		  }
	       }
	       
	       
	       // Handle exceeding bottom border
	       if ( s->y > yMaxBorder ) {  
		  debugPuts("   y>yMaxBorder");
		  
		  if (s->borderAction[BOTBORDER] == wrap) {
		     s->y = (s->y % 160); 
		  } else if (s->borderAction[BOTBORDER] == sendSignal) {
		     HandleSpriteExceededBorderFuncPtr(s, BOTBORDER);
		  } else if (s->borderAction[BOTBORDER] == bounce) {
		     // move to next leg
		     if (SpriteLoadNextMovementLeg(s)) {
			s->y = yMaxBorder;
		     }
		  }
	       }
	       
	       
	       // Handle exceeding top border
	       //debugPutInt("  s->borderAction[0]",s->borderAction[0]);
	       
	       if ( s->y < yMinBorder ) { 
		  debugPuts("   y<yMinBorder");
		  
		  if (s->borderAction[TOPBORDER] == wrap) {
		     s->y = (s->y + 160); 
		  } else if (s->borderAction[TOPBORDER] == sendSignal) {
		     HandleSpriteExceededBorderFuncPtr(s, TOPBORDER);
		  } else if (s->borderAction[TOPBORDER] == bounce) {
		     // move to next leg
		     if (SpriteLoadNextMovementLeg(s)) {
			s->y = yMinBorder;
		     }
		  }
	       }
	       


	    } 
         }

         thisIndex = s->nextSprite;
      }

   }



   debugFuncExit("SpriteDoNewMovement3()");
}




//--------------------------------------------------------------------
// SpriteEngDumpActiveList
//--------------------------------------------------------------------
void    SpriteEngDumpActiveList()
{
   /*
   UInt16 i;
   sprite *s=0;
   Int16 thisIndex=0;
   
   
   for (i=0;i<spriteClassMax;i++) {
      
      thisIndex = spriteList[i].firstActiveSprite;
      
      debugPutInt("class",i);
      //debugPutInt("thisIndex",thisIndex);
      
      
      while ( thisIndex != endOfClassList ) {
	 
 	 s=allSprites[thisIndex];
	 debugPutInt("  s",thisIndex);
         thisIndex = s->nextActiveSprite;

      } // while

   } // for i
   */
}


//--------------------------------------------------------------------
// SpriteEngRegisterCollisionMap
//
// Register a collision map with the Sprite Engine  
//-------------------------------------------------------------------- 
void SpriteEngRegisterCollisionMap(
   UInt16               thisSpriteType,
   UInt16               targSpriteType,
   collisionMethodEnum  method
) 
{

   debugFuncEnter("SpriteEngRegisterCollisionMap()");

   if ( (spriteList[thisSpriteType].collisionMethod == method) ||
	(spriteList[thisSpriteType].collisionMethod == dontCare)) 
   {    
      spriteList[ thisSpriteType ].collisionMethod = method; 

      if (method != dontCare) {
	 spriteList[ thisSpriteType ].collisionBitmap |= PowersOf2[targSpriteType];
      }
      
   } else {
      ERROR_ASSERT("Error! Conflicting collision methods not allowed");
   }
   
   debugPutInt("   thisSpriteType",thisSpriteType);
   debugPutHex("   collision bitmap",spriteList[ thisSpriteType ].collisionBitmap);
   
   debugFuncExit("SpriteEngRegisterCollisionMap()");
}


//--------------------------------------------------------------------
// SpriteEngRegisterCollisionMap
//
//-------------------------------------------------------------------- 
void SpriteEngRegisterCollisionMapOLD(
   Int spriteType,
   UInt16 n,
   collisionMethodEnum method
) 
{
    spriteList[ spriteType ].collisionBitmap = n;
    spriteList[ spriteType ].collisionMethod = method; 
    debugFuncExit("SpriteEngRegisterCollisionMapOLD()");
}


//--------------------------------------------------------------------
// SpriteEngCheckForCollisions
//
// Using the collision maps see if sprites have collided
//-------------------------------------------------------------------- 
void SpriteEngCheckForCollisions()
{

   Word i,targetClass=7,cbm=0;
   sprite * thisSprite=0;
   sprite * targSprite=0;
   Int16 thisIndex,targIndex;

   Boolean t1=false,t2=false,t3=false,t4=false,bang=false;

   debugFuncEnter( "SpriteEngCheckForCollisions()" );


   // Loop over all sprite classes (i=sprite class).  
   for (i=0;i<spriteClassMax;i++) {
  
      cbm = spriteList[i].collisionBitmap;

      if ( cbm>0 ) 
      {

      // thisSprite = param #1, targetClass = param #2 of SpriteEngRegisterCollisionMap
      for (targetClass=0; targetClass<spriteClassMax; targetClass++) {

         // Check the bitmap for a match
         if ( cbm & PowersOf2[targetClass] ) 
         {
            // Yes, we must check for collisions between each sprite in i
            // and each sprite in targetClass
 	    thisIndex = spriteList[i].firstSprite;

	    // debug
	    debugPutInt("  thisClass      ",i);
	    debugPutInt("  vs. targetClass",targetClass);
	    debugPutInt("  coll method    ",spriteList[i].collisionMethod);

            // Loop over all sprites in my class (i)
            while ( thisIndex != -1 )
	    {
	       thisSprite=allSprites[thisIndex];

               // Skip unused sprites
               if (notActive(thisSprite)) {
                  thisIndex = thisSprite->nextSprite;
                  continue;
               }
              
               // Loop over all sprites in target class
	       targIndex = spriteList[targetClass].firstSprite;
               while ( targIndex != -1 ) 
               {

                  // Skip unused "this" sprites -- since a bullet may have been
		  // used in a collision on a previous round
                  if (notActive(thisSprite)) {
                      targIndex = -1;
                      continue;
                  }

                  targSprite = allSprites[targIndex];

                  // Skip unused target sprites -- since they may have been
		  // kiled on previous rounds
                  if (notActive(targSprite)) {
                      targIndex = targSprite->nextSprite;
                      continue;
                  }

		  //OMcollisionTests++;
		  

		  // Skip detection when sprites aren't even in same region
		  if ((targSprite->collisionRegion & thisSprite->collisionRegion) == 0) 
		  {
		     debugPuts("  not in same region");
		     debugPutInt("  targSprite->collisionRegion",targSprite->collisionRegion);
		     debugPutInt("  thisSprite->collisionRegion",thisSprite->collisionRegion);
		     //OMnotInSameRegion++;
		     targIndex = targSprite->nextSprite;
		     continue;
		  }
	       

                  bang=false;

                  // should we do 4 corners checking?
		  if ((spriteList[i].collisionMethod == fourCorners )||
                      (spriteList[i].collisionMethod == bothWays )) {

		     // Check y coordinates first (for speed)

		     
		     // y overlap on top side
		     t3 = ((thisSprite->y + bmpTableNew[thisSprite->bitmap].y1)   <= 
			   (targSprite->y + bmpTableNew[targSprite->bitmap].y1)) &&
			((thisSprite->y + bmpTableNew[thisSprite->bitmap].y2)   >= 
			 (targSprite->y + bmpTableNew[targSprite->bitmap].y1));
		     
		     // y overlap on bottom side
		     t4 = ((thisSprite->y + bmpTableNew[thisSprite->bitmap].y1)   <= 
			   (targSprite->y + bmpTableNew[targSprite->bitmap].y2)) &&
			((thisSprite->y + bmpTableNew[thisSprite->bitmap].y2)   >= 
			 (targSprite->y + bmpTableNew[targSprite->bitmap].y2));
		     
		     //OMfourCornerLevel1Checks++;
		     
		     // If there is a possible x hit, then check the x coordinates
		     if (t3 | t4) 
		     {
			
			// x overlap on left side
			t1 = ((thisSprite->x + bmpTableNew[thisSprite->bitmap].x1)   <= 
			      (targSprite->x + bmpTableNew[targSprite->bitmap].x1)) &&
			   ((thisSprite->x + bmpTableNew[thisSprite->bitmap].x2)   >= 
			    (targSprite->x + bmpTableNew[targSprite->bitmap].x1));
			
			// x overlap on right side
			t2 = ((thisSprite->x + bmpTableNew[thisSprite->bitmap].x1)   <= 
			      (targSprite->x + bmpTableNew[targSprite->bitmap].x2)) &&
			   ((thisSprite->x + bmpTableNew[thisSprite->bitmap].x2)   >= 
			    (targSprite->x + bmpTableNew[targSprite->bitmap].x2));
			
			//OMfourCornerLevel2Checks++;
			
			// Confirm if there is a hit
			if (t1 | t2) {
			   bang=true;
			}
			
		     }  // t1|t2             
		     
		     // debug
		     /*
		       if ((thisSprite->spriteNum == 40) && 
		       (targSprite->spriteNum >= 0)   &&
		       (targSprite->spriteNum <= 8) ) 
		       {
		       StrPrintF ( debugStr2, "b x1=%d,x2=%d bug x1=%d,x2=%d", 
		       thisSprite->x + bmpTableNew[thisSprite->bitmap].x1,
		       thisSprite->x + bmpTableNew[thisSprite->bitmap].x2,
		       targSprite->x + bmpTableNew[targSprite->bitmap].x1,
		       targSprite->x + bmpTableNew[targSprite->bitmap].x2);
		       
		       
		       debugPutInt("against ",targSprite->spriteNum);
		       debugPutInt("t1",(Int)t1);
		       debugPutInt("t2",(Int)t2);
		       debugPutInt("t3",(Int)t3);
		       debugPutInt("t4",(Int)t4);
		       }
		     */
		     
		  } // do 4 corners checking 
		  
                  // Sometimes the sprites are moving so fast that the 
                  // overlap check fails because 1 sprite is actually inside
                  // another (true especially for small, fast sprites like
                  // bullets).  Check for that condition here...
		  
		  if ((spriteList[i].collisionMethod == centerOfGrav ) ||
                      (spriteList[i].collisionMethod == bothWays )) {
		     
		     //debugPutInt("  center-of-grav test for s",thisSprite->spriteNum);
		     /*
		       ASM_thisSprite = thisSprite;
		       ASM_targSprite = targSprite;
		       ASM_thisBmpTablePtr = &bmpTableNew[thisSprite->bitmap];
		       ASM_targBmpTablePtr = &bmpTableNew[targSprite->bitmap];
		     */

 
		    if (!bang) {

		       // Do ASM checking
		      //bang = asmSpriteEngCOGTest();

		      if (!thisSprite->realCogYcalc) {
			// calc real Y c.o.g, and cache it..
			thisSprite->realCogY = thisSprite->y + bmpTableNew[thisSprite->bitmap].centerY;
			thisSprite->realCogYcalc = true;
			
			debugPutInt("setting realCogYcalc for s",thisSprite->spriteNum);
			
			//OMCogYcalculations++;
		      }

		      if (!targSprite->realY12calc) {
			// calc real Y1, Y2 and cache them..
			targSprite->realY1 = targSprite->y + bmpTableNew[targSprite->bitmap].y1;
			targSprite->realY2 = targSprite->y + bmpTableNew[targSprite->bitmap].y2;
			targSprite->realY12calc = true;

			debugPutInt("setting realY12calc for s",targSprite->spriteNum);
			//OMRealY12calc++;
		      }

		      //OMcogLevel1Checks++;

		      if ( thisSprite->realCogY <= targSprite->realY2) {
			if ( thisSprite->realCogY >= targSprite->realY1) {

			  // OK, it's in the y band, now look at x band...

		          if (!thisSprite->realCogXcalc) {
			    // calc real X c.o.g, and cache it..
			    thisSprite->realCogX = thisSprite->x + bmpTableNew[thisSprite->bitmap].centerX;
			    thisSprite->realCogXcalc = true;
			    debugPutInt("setting realCogXcalc for s",thisSprite->spriteNum);
			    //OMCogXcalculations++;
		          }

		          if (!targSprite->realX12calc) {
			     // calc real Y1, Y2 and cache them..
			     targSprite->realX1 = targSprite->x + bmpTableNew[targSprite->bitmap].x1;
			     targSprite->realX2 = targSprite->x + bmpTableNew[targSprite->bitmap].x2;
			     targSprite->realX12calc = true;
			     debugPutInt("setting realX12calc for s",targSprite->spriteNum);
			     //OMRealX12calc++;
			  }

			  //OMcogLevel2Checks++;

		         if ( thisSprite->realCogX <= targSprite->realX2) {
			   if ( thisSprite->realCogX >= targSprite->realX1) {
			     bang=true;
			   } // realX1
			 } // realX2


			} // realY1
		      } // realY2

		       // OLD non-ASM checking
		      /*
		      if (((thisSprite->y + bmpTableNew[thisSprite->bitmap].centerY)   >= 
			   (targSprite->y + bmpTableNew[targSprite->bitmap].y1)) &&
			  ((thisSprite->y + bmpTableNew[thisSprite->bitmap].centerY)   <= 
			   (targSprite->y + bmpTableNew[targSprite->bitmap].y2)))
		      {
			  if (((thisSprite->x + bmpTableNew[thisSprite->bitmap].centerX)   >= 
			       (targSprite->x + bmpTableNew[targSprite->bitmap].x1)) &&
			      ((thisSprite->x + bmpTableNew[thisSprite->bitmap].centerX)   <= 
			       (targSprite->x + bmpTableNew[targSprite->bitmap].x2)))
			  {
			      bang=true;
			  }
		      }
		      */

		    } // !bang


		  } // !-fourcorners


                  if (bang && !DEBUGCOLLTIMINGON) {          
                     // Call the user-supplied collision function
                     HandleSpriteCollisionFuncPtr( targSprite, targetClass,
                                                   thisSprite, i );
                  }

                  if (bang && DEBUGCOLLTIMINGON) { 
		     DelFromActiveList(targSprite);
		     DelFromActiveList(thisSprite);
		  }
		  
           

                  // Move to next target sprite
                  targIndex = targSprite->nextSprite;
	       } // targIndex

               // Move to next sprite in my class
               thisIndex = thisSprite->nextSprite;
	    }

          
	 } // if (cbm & 2^targetclass) ... if this target class should be checked

      } // for each target class sprite

      } // if cbm > 0
     
   } // for each sprite class

   // stats
   /*
   debugPutInt("  OMcogLevel1Checks",cogLevel1Checks);
   debugPutInt("  cogLevel2Checks",cogLevel2Checks);
   debugPutInt("  fourCornerLevel1Checks",fourCornerLevel1Checks);
   debugPutInt("  fourCornerLevel2Checks",fourCornerLevel2Checks);
   */

   debugFuncExit("SpriteEngCheckForCollisions()");
}


//--------------------------------------------------------------------
//   
// 
//
//--------------------------------------------------------------------
Boolean asmSpriteEngCOGTest()
{

  asm volatile (   
         "movea.l  %1, %%a0 ;     /* a0= ASM_thisSprite */
	  movea.l  %2, %%a1 ;     /* a1= ASM_targSprite */
	  movea.l  %3, %%a2 ;     /* a2= ASM_thisBmpTablePtr */
	  movea.l  %4, %%a3 ;     /* a3= ASM_targBmpTablePtr */

	  /* d0 = thisSprite->y + bitmap.centerY */  
          move.w  6(%%a0), %%d0  ;    /* .y        is 3rd word of struct */
          add.w  14(%%a2), %%d0  ;    /* .centerY  is 7th word of struct */
	  
	  /* d1 = targSprite->y + bitmap.y1 */  
	  move.w 6(%%a1), %%d1  ;    /*  .y        is 3rd word of struct */
	  add.w  6(%%a3), %%d1  ;    /* .y1       is 3rd word of struct */

	  cmp.w %%d1,%%d0  ;         /*      test: d0 >= d1 */
	  blt .cog99  ;   

	  /* d1 = targSprite->y + bitmap.y2 */  
          move.w  6(%%a1), %%d1  ;    /* .y        is 3rd word of struct */
          add.w  10(%%a3), %%d1  ;    /* .y2       is 5th word of struct */

	  cmp.w %%d1,%%d0  ;          /*      test: d0 <= d1 */
	  bgt .cog99  ;   

	  /* d0 = thisSprite->x + bitmap.centerX */  
          move.w  4(%%a0), %%d0  ;    /* .x        is 2nd word of struct */
          add.w  12(%%a2), %%d0  ;    /* .centerX  is 6th word of struct */
	  
	  /* d1 = targSprite->z + bitmap.x1 */  
	  move.w 4(%%a1), %%d1  ;    /*  .x        is 2nd word of struct */
	  add.w  4(%%a3), %%d1  ;    /* .x1       is 3rd word of struct */

	  cmp.w %%d1,%%d0  ;         /*      test: d0 >= d1 */
	  blt .cog99  ;   

	  /* d1 = targSprite->x + bitmap.x2 */  
          move.w  4(%%a1), %%d1  ;    /* .x        is 2nd word of struct */
          add.w   8(%%a3), %%d1  ;    /* .x2       is 4th word of struct */

	  cmp.w %%d1,%%d0  ;          /*      test: d0 <= d1 */
	  bgt .cog99  ;   


	  /* TRUE EXIT */
          move.w #1, %0  ;
	  bne .cog00  ; 

	  /* FALSE EXIT */
  .cog99: move.w #0, %0  ;
  .cog00: nop  " 
       : "=&r" (ASM_COGTestResult)    /* 0 */
       : "r"  (ASM_thisSprite),       /* 1 */
         "r"  (ASM_targSprite),        /* 2 */
         "r"  (ASM_thisBmpTablePtr),      /* 3 */
         "r"  (ASM_targBmpTablePtr)       /* 4 */
	  : "%%a0","%%a1","%%a2","%%a3","%%d0","%%d1"
   );


  // debug
  //debugFuncEnter("asmSpriteEngCOGTest()");
  //debugPutInt("  asm",ASM_COGTestResult);

  debugFuncExit("asmSpriteEngCOGTest()");
  return (Boolean)ASM_COGTestResult;

   debugFuncExit("asmSpriteEngCOGTest()");
}

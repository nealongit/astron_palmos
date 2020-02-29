//
//  Behavior
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

//----------------------------------------------------------------------
//   **** THIS MODULE CONTAINS NO SAVE-ABLES ****             
//----------------------------------------------------------------------
#include <SysUtils.h>  // for random numbers

#include "wave.h"
#include "gameman.h"
#include "sprite.h"
#include "blast.h"
#include "debug.h"
#include "graphics.h"  
#include "starter.h"
#include "behav.h"

#include "defs.h"
#include "misc.h"  // for random numbers           


//----------------------------------------------------------------------
//                
//----------------------------------------------------------------------

// screen is 20*xd by 17.5*yd
#define xd 8
#define yd 8
#define ALLDONE 255
#define NOMORE {ALLDONE,ALLDONE,ALLDONE}

// Accounts for the fact that the x,y position is in the top left 
// corner of the sprite, so left flying waves do not balance
#define xfud 10   
#define midx 74


// Star speeds defaulted to monochrome speed
Byte slowStarSpeed=2; 
Byte fastStarSpeed=4;



//----------------------------------------------------------------------
//                Internal APIs
//-----------------------------------------------------------------------
void         updateFormationMoveStruct();

moveStruct * setSpriteForDynamicMovement(sprite * s);

Boolean      chooseLeftOrRight(sprite *s);


//----------------------------------------------------------------------
//   Flight Paths 
//----------------------------------------------------------------------

// a group
flightPtStruct ifa1[] = { {5*xd,0,SignalBulletPoint},{6*xd,6*yd,SignalBulletPoint},
                          {14*xd,11*yd,SignalBulletPoint},{15*xd,13*yd,SignalBugPeelingOffPoint}, 
                          {15*xd,30*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct ifa2[] = { {15*xd,0,SignalBulletPoint},{14*xd,6*yd,SignalBulletPoint},
                          {6*xd,11*yd,SignalBulletPoint},{5*xd,13*yd,SignalBugPeelingOffPoint}, 
                          {5*xd,30*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct ifa3[] = { {0*xd,12*yd,SignalBulletPoint},{5*xd,10*yd,SignalBulletPoint},
                          {6*xd,5*yd,SignalBugPeelingOffPoint},{10*xd,3*yd,SignalBulletPoint},
                           {14*xd,5*yd,SignalBulletPoint},
                          {15*xd,10*yd,SignalBugPeelingOffPoint},{25*xd,12*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct ifa4[] = { {19*xd,12*yd,SignalBulletPoint},{15*xd,10*yd,SignalBulletPoint},
                          {14*xd,5*yd,SignalBulletPoint},{10*xd,3*yd,SignalBulletPoint},
                           {6*xd,5*yd,SignalBulletPoint},
                          {5*xd,10*yd,SignalBugPeelingOffPoint},{-30,14*yd,SignalRemoveSprite},
                           NOMORE}; 
 
// b group
flightPtStruct ifb1[] = { {midx,0*yd,SignalBulletPoint},{8*xd,6*yd,SignalBulletPoint},
                          {5*xd,10*yd,SignalBugPeelingOffPoint},{-30,16*yd,SignalRemoveSprite},
                           NOMORE}; 

flightPtStruct ifb2[] = { {midx,0*yd,SignalBulletPoint},{12*xd,6*yd,SignalBulletPoint},
                          {15*xd,10*yd,SignalBugPeelingOffPoint},{23*xd,16*yd,SignalRemoveSprite},
                           NOMORE}; 

flightPtStruct ifb3[] = { {1*xd,0*yd,SignalBulletPoint},{3*xd,6*yd,SignalBulletPoint},
                          {5*xd,8*yd,SignalBugPeelingOffPoint},{10*xd,8*yd,SignalBulletPoint},
                          {14*xd,8*yd,SignalBulletPoint},
                          {14*xd,10*yd,SignalBugPeelingOffPoint},{20*xd,20*yd,SignalBulletPoint},
                           NOMORE};

flightPtStruct ifb4[] = { {18*xd,0*yd,SignalBulletPoint},{14*xd,6*yd,SignalBulletPoint},
                          {12*xd,8*yd,SignalBulletPoint},
                           {10*xd,8*yd,SignalBulletPoint},{5*xd,8*yd,SignalBugPeelingOffPoint},
                          {3*xd,10*yd,SignalBugPeelingOffPoint},{1*xd,20*yd,SignalRemoveSprite},
                           NOMORE};

// c group
flightPtStruct ifc1[] = { {midx,0,SignalBulletPoint},{6*xd,4*yd,SignalBulletPoint},
                          {6*xd,6*yd,SignalBulletPoint},
                          {14*xd,11*yd,SignalBulletPoint},{15*xd,13*yd,SignalBugPeelingOffPoint}, 
                          {6*xd,22*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct ifc2[] = { {midx,0,SignalBulletPoint},{14*xd,4*yd,SignalBulletPoint},
                          {14*xd,6*yd,SignalBulletPoint},
                          {6*xd,11*yd,SignalBulletPoint},{6*xd,13*yd,SignalBugPeelingOffPoint}, 
                          {14*xd,22*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct ifc3[] = { {midx,0,SignalBulletPoint},{6*xd,4*yd,SignalBulletPoint},
                          {6*xd,6*yd,SignalBulletPoint},
                          {14*xd,11*yd,SignalBugPeelingOffPoint},{22*xd,12*yd,SignalRemoveSprite}, 
                           NOMORE};

flightPtStruct ifc4[] = { {midx,0,SignalBulletPoint},{13*xd,4*yd,SignalBulletPoint},
                          {13*xd,6*yd,SignalBulletPoint},
                          {6*xd,11*yd,SignalBugPeelingOffPoint},{-30,12*yd,SignalRemoveSprite}, 
                           NOMORE};

// e group
flightPtStruct ife1[] = { {midx,0*yd,SignalBulletPoint},{midx,6*yd,SignalBulletPoint},
                          {midx,8*yd,SignalBugPeelingOffPoint},
                          {-20,14*yd,SignalRemoveSprite},
                           NOMORE}; 

flightPtStruct ife2[] = { {midx,0*yd,SignalBulletPoint},{midx,6*yd,SignalBulletPoint},
                          {midx,8*yd,SignalBugPeelingOffPoint},
                          {21*xd,14*yd,SignalRemoveSprite},
                           NOMORE};


// f group -- flights have been tweeked to appear balanced
flightPtStruct iff1[] = { {0*xd,14*yd,SignalBulletPoint},{5*xd,8*yd,SignalBulletPoint},
                          {8*xd,7*yd,SignalBugPeelingOffPoint},{-20,2*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct iff2[] = { {0*xd,14*yd,SignalBulletPoint},{5*xd,10*yd,SignalBulletPoint},
                          {15*xd,4*yd,SignalBugPeelingOffPoint},{22*xd,2*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct iff3[] = { {18*xd,13*yd,SignalBulletPoint},{14*xd-xfud,9*yd,SignalBulletPoint},
                          {5*xd-xfud,4*yd,SignalBugPeelingOffPoint},
			  {-20-xfud,2*yd,SignalRemoveSprite},
                           NOMORE};

flightPtStruct iff4[] = { {18*xd,14*yd,SignalBulletPoint},{14*xd-xfud,8*yd,SignalBulletPoint},
                          {11*xd-xfud,7*yd,SignalBugPeelingOffPoint},
			  {20*xd,2*yd,SignalRemoveSprite},
                           NOMORE};


//-----------------------------------------------------------------------
// 
//-----------------------------------------------------------------------
void encodeFlightpath(
   sprite          * s, 
   Int               numInLine,
   flightPtStruct  * thisFlt,
   Int               xMaxSpeed,
   Int               yMaxSpeed
)
{
   UInt16 qTime=0, xsign,ysign,xAbsDist,yAbsDist,xspeed,yspeed,i,dur,iStart=0;
   moveStruct * ms;
   flightPtStruct * fp;

   // debug
   debugFuncEnterInt("encodeFlightpath(), s",s->spriteNum); 
   debugPutInt("  numInLine",numInLine);

   fp = thisFlt;

   ms = setSpriteForDynamicMovement(s); 
   s->borderAction[1]=sendSignal;  
   s->borderAction[2]=sendSignal; 
   s->borderAction[3]=sendSignal; 

   // Calculate how long the sprite should be off screen (queuing)
   if (numInLine != -1) {
      qTime = numInLine * 6 + 1;
      encodeHideFromView(ms,0,qTime,fp[0].x,fp[0].y);  // leg 0   
      iStart=1;
   } else {
     // This bug is not off screen-- it's diving
     debugPuts("  no hide-from-view encoded");
     encodeStaticLeg  (ms,0,1,SignalNoSignal,0,0);
     iStart=1;
   }
   

   for(i=iStart;i<MAXLEGS;i++)
   {
     if (fp[i].x == ALLDONE ) {
       // no more legs
       break;
     }

     // account for negative 
     xsign = ((fp[i].x - fp[i-1].x) > 0) ? 1 : -1;
     xAbsDist = xsign * (fp[i].x - fp[i-1].x);  

     ysign = ((fp[i].y - fp[i-1].y) > 0) ? 1 : -1;
     yAbsDist = ysign * (fp[i].y - fp[i-1].y);

     if ((xAbsDist * xMaxSpeed) > (yAbsDist * yMaxSpeed)) {
       // X speed must be faster...
       xspeed = xMaxSpeed * xsign;
       dur    = xAbsDist/xMaxSpeed;

       // prevent divide by zeroes
       if (dur==0) {
         yspeed=1*ysign;
       } else {
         yspeed = yAbsDist/dur * ysign;
       }
     } else {
       // Y speed must be faster...
       yspeed = yMaxSpeed * ysign;
       dur    = yAbsDist/yMaxSpeed;

       // prevent divide by zeroes
       if (dur==0) {
	 xspeed=1*xsign;
       } else {
         xspeed = xAbsDist/dur * xsign;
       }
     }

     //debugPutInt("xAbsDist",xAbsDist);
     //debugPutInt("yAbsDist",yAbsDist);

     encodeStaticLeg(ms,i,dur,fp[i].signal,xspeed,yspeed); 

   }

   //got_here;

   debugFuncExit("updateFormationMoveStruct()");
}





//-----------------------------------------------------------------------
//  true = go left
//-----------------------------------------------------------------------
Boolean chooseLeftOrRight(sprite *s)
{
  debugFuncEnter("chooseLeftOrRight");

  if (s->x > 130) { debugFuncExit("chooseLeftOrRight()"); return true; };
  if (s->x < 30) { debugFuncExit("chooseLeftOrRight()"); return false; }; 

  if (randomNum(2) ==0) { debugFuncExit("chooseLeftOrRight()"); return true; }
  
  debugPuts(" chooseLeftOrRight returned false");
  debugFuncExit("chooseLeftOrRight()");
  return false;

  
}


//-----------------------------------------------------------------------
//  Encode flyback
//-----------------------------------------------------------------------
void setBugFlybackMotion(
   sprite * s
)
{
   Int l,dir=1;
   moveStruct * ms;
   flightPtStruct fps[MAXLEGS] = {{0,0,SignalNoSignal},{0,0,SignalNoSignal},
                                  {0,0,SignalNoSignal},{0,0,SignalNoSignal},
                                  {0,0,SignalNoSignal},{0,0,SignalNoSignal},
                                  {0,0,SignalNoSignal},{0,0,SignalRemoveSprite}};

   // debug
   debugFuncEnter("setBugFlybackMotion()"); 

   // see where we got to before to determine the flyback direction
   ms = &s->myMoveStruct;
   l = ms->currentLeg;

   if (ms->leg[l].legType == staticLeg) {
     // see if the last delta x was + or -
     if (ms->leg[l].guts.staticLegStruct.deltaX > 0) {
       dir=1;
     } else {
       dir=-1;
     }
   }

   // Adjust y to put bug back on screen
   s->y=142;

   fps[0].x = s->x;             // used only as reference
   fps[0].y = s->y;
   fps[1].x = s->x +  3*xd*dir;
   fps[1].y = s->y;
   fps[2].x = s->x +  4*xd*dir;
   fps[2].y = s->y + -4*yd;
   fps[3].x = s->x +  xd*dir;
   fps[3].y = s->y + -5*yd;
   fps[4].x = s->x + -1*xd*dir;
   fps[4].y = s->y + -4*yd;
   fps[5].x = s->x + -6*xd*dir;          // do final dive, -6 gives it a fade
   fps[5].y = 21*yd;

   fps[6].x = ALLDONE;

   ms = setSpriteForDynamicMovement(s);
   encodeFlightpath(s,-1,fps,rps.medXSpeed,rps.medYSpeed);

   debugFuncExit("setBugFlybackMotion()");
}


//-----------------------------------------------------------------------
//  Encode Bug 1's motions
//-----------------------------------------------------------------------
void setBug1Motion(
   sprite * s
)
{
   Int dir;
   moveStruct * ms;
   flightPtStruct fps[MAXLEGS] = {{0,0,SignalNoSignal},   {0,0,SignalBulletPoint},
                                  {0,0,SignalBulletPoint},{0,0,SignalBulletPoint},
                                  {0,0,SignalBulletPoint},{0,0,SignalBulletPoint},
                                  {0,0,SignalBulletPoint},{0,0,SignalBulletPoint}};

   debugFuncEnter("setBug1Motion()"); 

   dir = ( chooseLeftOrRight(s) ) ? 1 : -1;

   fps[0].x = s->x;             // used only as reference
   fps[0].y = s->y;
   fps[1].x = s->x +  4*xd*dir;
   fps[1].y = s->y +  4*yd;
   fps[2].x = s->x +  4*xd*dir;
   fps[2].y = s->y +  6*yd;
   fps[3].x = s->x + -4*xd*dir;
   fps[3].y = s->y +  10*yd;
   fps[4].x = s->x + -4*xd*dir;
   fps[4].y = s->y +  12*yd;

   fps[5].x = s->x +  1*xd*dir;
   fps[5].y = s->y +  21*yd;

   fps[6].x = ALLDONE;

   // This bug does flybacks -- encoded later
   roundInfo.bugs[ s->spriteNum ].numFlybacksAllowed = 1;   

   ms = setSpriteForDynamicMovement(s);
   encodeFlightpath(s,-1,fps,rps.medXSpeed,rps.medYSpeed);

   // Overwrite border actions set in encodeFlightpath
   s->borderAction[1]=bounce;  
   s->borderAction[2]=sendSignal;
   s->borderAction[3]=bounce;  

   debugFuncExit("setBug1Motion()");
}


//-----------------------------------------------------------------------
//  Encode sweep bug movements 
//-----------------------------------------------------------------------
void setSweepBugMotion(
   sprite * s
)
{
   Int dir;
   flightPtStruct fps[] = { {0,0,SignalNoSignal}, 
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalRemoveSprite}, 
                            NOMORE };

   debugFuncEnter("setSweepBugMotion()"); 

   // left=true
   dir = ( chooseLeftOrRight(s) ) ? -1 : +1;

   fps[0].x = s->x;
   fps[0].y = s->y;
   fps[1].x = s->x + 6*xd*dir;
   fps[1].y = s->y + 2*yd;
   fps[2].x = s->x + 2*xd*(-dir);
   fps[2].y = 12*yd -rps.aimAtPlayerVariance;
   // aim at user 
   fps[3].x = GetUserSprite()->x -rps.aimAtPlayerVariance/2 + randomNum(rps.aimAtPlayerVariance);  
   fps[3].y = 135 + randomNum(rps.aimAtPlayerVariance);
   // drop down 
   fps[4].x = fps[3].x;
   fps[4].y = 24*yd;

   encodeFlightpath(s,-1,fps,rps.medXSpeed,rps.medYSpeed);

   // Overwrite border actions set in encodeFlightpath
   s->borderAction[1]=bounce;  
   s->borderAction[2]=sendSignal;  
   s->borderAction[3]=bounce; 

   s->bitmap = sweepBug;
  
   debugFuncExit("setSweepBugMotion()");
}


//-----------------------------------------------------------------------
//  Encode 
//-----------------------------------------------------------------------
void setBugPeelingOffMotion(
   sprite * s
)
{
   Int dir,userX,n;
   moveStruct * ms;
   flightPtStruct fps[] = { {0,0,SignalNoSignal}, {0,0,SignalNoSignal},
                             {0,0,SignalNoSignal}, NOMORE };

   debugFuncEnter("setBugPeelingOffMotion()"); 

   userX = GetUserSprite()->x;

   // do an s-shaped dive
   dir = ( s->x > userX ) ? +1 : +1;

   fps[0].x = s->x;
   fps[0].y = s->y;

   // leg 1.. fly away from user
   if (s->y > 100) {
     // handle low peel-offs  
     n=randomNum(10)+10;
     fps[1].x = s->x + n*dir;
     fps[1].y = s->y + n;
   } else {
     fps[1].x = s->x + (10 + randomNum(10))*dir;
     fps[1].y = s->y + randomNum(20)+10;
   }

   // leg 2.. fly back towards user
   fps[2].x = userX + randomNum(30)*(-dir);
   fps[2].y = 150 + randomNum(30);

   // Get a movement structure for this bug
   ms = setSpriteForDynamicMovement(s);

   s->borderAction[1]=sendSignal;
   s->borderAction[2]=sendSignal;
   s->borderAction[3]=sendSignal; 

   encodeFlightpath(s,-1,fps,rps.peelOffXSpeed,rps.peelOffYSpeed);


   debugFuncExit("setBugPeelingOffMotion()");
}



//-----------------------------------------------------------------------
//  Encode MotherShip!  -- hovers
//-----------------------------------------------------------------------
void setMothershipMotion(
   sprite * s, 
   Boolean  sendTracBeam
)
{
   Int dir,signal;
   moveStruct * ms;

   flightPtStruct fps[] = { {0,0,SignalNoSignal}, 
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalRemoveSprite}, 
                            NOMORE };

   if (!sendTracBeam) {
      debugFuncEnterInt("setMothershipMotion(), no trac beam, s",s->spriteNum); 
   } else {
      debugFuncEnterInt("setMothershipMotion(), TRAC BEAM, s",s->spriteNum); 
   }

   // left=true
   dir = ( chooseLeftOrRight(s) ) ? -1 : +1;

   // "full" motherships do not fire.. 
   signal = (s->bitmap == fullMotherShip) ? SignalNoSignal : SignalBulletPoint;

   fps[0].x = s->x;   // used only as reference
   fps[0].y = s->y;
   fps[0].signal = signal;
   fps[1].x = s->x;      // drop down
   fps[1].y = 5*yd;
   fps[1].signal = signal;
   fps[2].x = s->x +  3*xd*dir;
   fps[2].y = 7*yd;
   fps[2].signal = signal;
   fps[3].x = s->x;
   fps[3].y = 9*yd;
   fps[3].signal = signal;

   // Point 4 is the (possible) trac beam point
   fps[4].x = s->x +  2*xd*dir;
   fps[4].y = 12*yd;
   fps[4].signal = (sendTracBeam) ? SignalNoSignal : SignalBulletPoint;

   // dummy point 5
   fps[5].x = ALLDONE;  
   fps[5].y = ALLDONE;
   fps[5].signal = ALLDONE;

   // Have the flight path calculated up to this part
   encodeFlightpath(s,-1,fps,rps.slowXSpeed,rps.slowYSpeed);

   // Overwrite border actions set in encodeFlightpath
   s->borderAction[1]=bounce;  
   s->borderAction[2]=sendSignal;  
   s->borderAction[3]=bounce; 

   // Now hand encode the rest
   ms = &s->myMoveStruct; 

   if (sendTracBeam)  {
      encodeStaticLeg(ms,5,5,SignalTracBeamPoint,0,0); // pause before trac beam
      encodeStaticLeg(ms,6,85,SignalNoSignal,0,0);   // the hover ... DUR LINKED TO TRAC BEAM MOTION!!
      encodeStaticLeg(ms,7,160,SignalRemoveSprite,0,rps.slowYSpeed); // descend
   } else {
      encodeStaticLeg(ms,5,12,signal,dir*rps.slowXSpeed,rps.slowYSpeed);
      encodeStaticLeg(ms,6,160,SignalRemoveSprite,0,rps.slowYSpeed); // descend
   }

 
   debugFuncExit("setMothershipMotion()");
}


//-----------------------------------------------------------------------
//  Encode MotherShip hauling the user in then descending.  Leg 0 should
//  not complete (signal should come that user has beamed up 1st)
//-----------------------------------------------------------------------
void setSuccessMothershipMotion(
   sprite * s
)
{
   moveStruct * ms;

   debugFuncEnter("setSuccessMothershipMotion"); 

   // legs
   ms = setSpriteForDynamicMovement(s); 
   encodeStaticLeg(ms,0,80,SignalRemoveSprite,0,0);  // stationary
   encodeStaticLeg(ms,1,20,SignalRemoveSprite,0,2);   // drop after holding user

   debugFuncExit("not()");
}
 
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void setUserInTracBeamMotion(
   sprite * userSprite,
   Int      dist
)
{
   moveStruct * ms;

   debugFuncEnter("setUserInTracBeamMotion"); 

   ms = setSpriteForDynamicMovement(userSprite); 
   encodeStaticLeg(ms,0,dist-10,SignalUserWithinMothership,0,-1);  // stationary

   debugFuncExit("setUserInTracBeamMotion()");
}

//-----------------------------------------------------------------------
//  Encode Mine Layer motion
//-----------------------------------------------------------------------
void setMineLayerMotion(
   sprite * s
)
{
   Int deltaX,i,div,legs=4;
   moveStruct * ms;

   debugFuncEnter("setMineLayerMotion"); 

   // bug flies left-to-right, or right-to-left based on position
   if ( chooseLeftOrRight(s) ) {
     // left
     deltaX=-rps.slowXSpeed; div= (s->x) / legs / rps.slowXSpeed;
   } else {
     // right
     deltaX=+rps.slowXSpeed; div= (160 - s->x) / legs / rps.slowXSpeed;
   }

   debugPutInt("s->x",s->x);
   debugPutInt("deltaX",deltaX);
   debugPutInt("div",div);


   // legs
   ms = setSpriteForDynamicMovement(s); 
   encodeStaticLeg(ms,0,((rps.mineCeiling-s->y)/rps.medXSpeed),
                             SignalMineLayPoint,0,rps.medXSpeed);      // the drop

   for (i=1;i<4;i++) {
      encodeStaticLeg(ms,i,div,SignalMineLayPoint,deltaX,0);
   }

   encodeStaticLeg(ms,4,100,SignalNoSignal,deltaX,-1);     // rise back up

   debugFuncExit("setMineLayerMotion()");
}


//-----------------------------------------------------------------------
//  Encode Mine 
//-----------------------------------------------------------------------
void setMineMotion(
   sprite * s, 
   sprite * mineLayerShip
)
{
   Int deltaX=0;  // ......... hacked to just drop
   moveStruct * ms;

   debugPutInt("setMineMotion for s",s->spriteNum); 

   // Set the sprite info
   AddToActiveList(s);
   s->bitmap=mineBmp;  // VITAL SINCE THIS CLASS IS ALSO USED FOR 2 SHOT AWARD
   s->x = mineLayerShip->x-4;
   s->y = mineLayerShip->y+8;

   // Get a movement structure for this bug, do setup
   ms = setSpriteForDynamicMovement(s); 

   // legs
   encodeStaticLeg(ms,0,120,SignalRemoveSprite,deltaX,rps.slowYSpeed);    
   /* 
   encodeHomeToLeg(ms,1,10,SignalNoSignal,deltaX,rps.slowYSpeed);     
   encodeHomeToLeg(ms,2,8,SignalNoSignal,deltaX,rps.slowYSpeed);     
   encodeHomeToLeg(ms,3,8,SignalNoSignal,deltaX,rps.slowYSpeed);     
   encodeHomeToLeg(ms,4,8,SignalNoSignal,deltaX,rps.slowYSpeed);     
   encodeStaticLeg(ms,5,120,SignalNoSignal,0,rps.slowYSpeed);     
   */

   debugFuncExit("setMineMotion()");
}

//-----------------------------------------------------------------------
//  Encode Big Oaf
//-----------------------------------------------------------------------
void setBigOafMotion(
   sprite * s
)
{
   moveStruct * ms;

   debugPutInt("setBigOafMotion for s",s->spriteNum); 

   // Set the sprite info
   AddToActiveList(s);

   // legs
   ms = setSpriteForDynamicMovement(s); 
   encodeHomeToLeg(ms,0,15,SignalBulletPoint,rps.slowXSpeed,rps.slowYSpeed);     
   encodeHomeToLeg(ms,1,15,SignalBulletPoint,rps.medXSpeed,rps.slowYSpeed);    
   encodeHomeToLeg(ms,2,15,SignalBulletPoint,rps.medXSpeed,rps.slowYSpeed);    
   encodeHomeToLeg(ms,3,15,SignalBulletPoint,rps.slowXSpeed,rps.slowYSpeed);    
   encodeHomeToLeg(ms,4,15,SignalBulletPoint,rps.medXSpeed,rps.slowYSpeed);    
   encodeStaticLeg(ms,5,120,SignalNoSignal,0,rps.slowYSpeed);    

   debugFuncExit("setBigOafMotion()");
}

//-----------------------------------------------------------------------
// Encode the barrier "shudder"
//-----------------------------------------------------------------------
void setBarrierGotHitMotion(
   sprite * s
)
{
   moveStruct * ms;

   debugPutInt("---setBarrierGotHitMotion for s",s->spriteNum); 

   // legs
   ms = setSpriteForDynamicMovement(s);
   encodeStaticLeg(ms,0,1,SignalNoSignal,0,-rps.slowYSpeed);     
   encodeStaticLeg(ms,1,2,SignalNoSignal,0,0);                  // stop
   encodeStaticLeg(ms,2,1,SignalNoSignal,0,+rps.slowYSpeed);    
   encodeStaticLeg(ms,3,1,SignalBarrierMotDone,0,0);            // stop

   debugFuncExit("setBarrierGotHitMotion()");
}

//-----------------------------------------------------------------------
//  Encode Bonus 2 shot
//-----------------------------------------------------------------------
Int setBonus2ShotMotion(
   sprite * motherShip
)
{
 
   sprite *s;
   moveStruct * ms;
   Int dur;

   //char msg[40]="",temp[40]=""; // debug

   debugFuncEnter("setBonus2ShotMotion");

   // Get an unused mine sprite (force)
   s = SpriteEngFindFirstUnusedSpriteOrForce( MineSpriteClass );

   if (s == NULL) {
     debugPuts("error, force failed");
     debugFuncExit("setBonus2ShotMotion()");
     return -1;
   }

   // Set the sprite info
   AddToActiveList(s);
   s->bitmap=twoShotAwardBmp;
   s->x = motherShip->x-4;

   // Handle releasing 2-shot quite low
   if (motherShip->y+8 < USERINITyPOS) {
      dur=(USERINITyPOS - (motherShip->y+8))/rps.slowYSpeed;
      s->y = motherShip->y+8;
   } else {
     dur = 1;
     s->y = USERINITyPOS -2;
   }

   //----------------debug--------------
   /*
   StrCopy(msg,"dur=");
   StrIToA(temp,dur);
   StrCat(msg,temp);
   StrCat(msg," USERINITyPOS=");
   StrIToA(temp,USERINITyPOS);
   StrCat(msg,temp);
   StrCat(msg," s->y=");
   StrIToA(temp,s->y);
   StrCat(msg,temp);
   FrmCustomAlert(nealErrAlertId,__FILE__,dstr,msg);
   ERROR_ASSERT(msg);
   */
   //----------------debug--------------


   ms = setSpriteForDynamicMovement(s); 
   encodeStaticLeg(ms,0,dur,SignalNoSignal,0,rps.slowYSpeed);   // drop
   encodeStaticLeg(ms,1,50,SignalNoSignal,0,0);    // stop
   encodeStaticLeg(ms,2,100,SignalRemoveSprite,0,rps.slowYSpeed);  // drop
  
  
   debugFuncExit("setBonus2ShotMotion()");
   return s->spriteNum;

}

//-----------------------------------------------------------------------
//  Encode explosion
//
// This proc is a little different in that it allocates it's own sprite
//-----------------------------------------------------------------------
void showExplosion(
   sprite * hitSprite
)
{
   sprite * s;
   moveStruct * ms=0;
   UInt8 dur;
   
   debugFuncEnter("showExplosion");

   // Get an unused collision sprite (or force)
   s = SpriteEngFindFirstUnusedSpriteOrForce( ExplosionSpriteClass );

   if (s != NULL) {

      // set the attributes
      AddToActiveList(s);
      s->x = hitSprite->x;
      s->y = hitSprite->y;

      if (hitSprite != userSprite) {
	 if (UtilCPULevel <= CPULimitForLongExplos) {
	    dur = (ScreenDepth == oneBitDepth) ? ExplosDurNormalBW : ExplosDurNormalColor;
	 } else {
	    dur = ExplosDurCPULimit;
	 }
      } else {
	 // User always has long explosion
	 dur = ExplosDurNormalBW;
      }

      // Directly write the moveStruct so that DoNewMovement does not have to load it
      ms=&s->myMoveStruct;
      ms->currentLeg     = 0;
      ms->countdown      = dur;
      ms->sendThisEvent  = SignalRemoveSprite;
      ms->leg[0].legType = noMovement;

   } else {
      debugPuts("No explosion sprite available");
   }

   debugFuncExit("showExplosion()");
}

//-----------------------------------------------------------------------
//  Encode tractor beam up-and-down motion
//-----------------------------------------------------------------------
void setTracBeamMotion(
   sprite * s, 
   sprite * motherShip
)
{
   Int dur,x;
   moveStruct * ms;

   x = motherShip->x-4 + TracBeamXOffset;

   // Set the sprite info
   AddToActiveList(s);
   s->x = x;
   s->y = motherShip->y+8;
   s->borderAction[0]=noAction;  // for hiding
   s->borderAction[2]=noAction;  // lips over bottom


   // calc dur, ensure it's not zero
   dur = (136-(motherShip->y+8))/2;
   dur = (dur==0) ? 1 : dur;

   debugPutInt("dur",dur);

   // twice on the tractor beam
   ms = setSpriteForDynamicMovement(s); 
   encodeStaticLeg   (ms,0,dur,SignalReplayTracBeamSnd,0,2);   
   encodeHideFromView(ms,1,2,x,motherShip->y+10);
   encodeStaticLeg   (ms,2,dur,SignalReplayTracBeamSnd,0,2);   
   encodeHideFromView(ms,3,2,x,motherShip->y+10);
   encodeStaticLeg   (ms,4,dur,SignalTracBeamExpired,0,2); 
   encodeStaticLeg   (ms,5,2,SignalRemoveSprite,0,2);



   debugFuncExit("setTracBeamMotion()");
}

//-----------------------------------------------------------------------
//  Encode drop off the bottom of the screen
//-----------------------------------------------------------------------
void setDropMotion(
   sprite * s, 
   Int      speed
)
{
   moveStruct * ms;

   debugFuncEnter("setDropMotion()");

   // Set the sprite info.. x,y,used should already be set
   s->borderAction[2]=sendSignal; 
   ms = setSpriteForDynamicMovement(s); 

   encodeStaticLeg(ms,0,160,SignalRemoveSprite,0,speed);   
   debugFuncExit("setDropMotion()");
}


//-----------------------------------------------------------------------
//  Encode wide dart bug
//-----------------------------------------------------------------------
void setDartBugMotion(
   sprite * s
)
{
   Int deltaX, backX;
   moveStruct * ms;

   debugFuncEnter("setDartBugMotion()"); 

   // left=true
   deltaX = ( chooseLeftOrRight(s) ) ? -rps.highXSpeed : +rps.highXSpeed;
   backX  = ( deltaX < 0) ? -(deltaX +1) : -(deltaX -1);
   

   s->borderAction[1]=bounceIncr2;  
   s->borderAction[2]=sendSignal;
   s->borderAction[3]=bounceIncr2; 

   // legs
   ms = setSpriteForDynamicMovement(s); 
   encodeStaticLeg(ms,0,3,SignalBulletPoint,deltaX,rps.slowYSpeed );  // fast, shallow horiz movement,
   encodeStaticLeg(ms,1,3,SignalBulletPoint,deltaX,rps.slowYSpeed);  
   encodeStaticLeg(ms,2,3,SignalBulletPoint,deltaX,rps.slowYSpeed);  
   encodeStaticLeg(ms,3,15,SignalBulletPoint,deltaX,rps.slowYSpeed);  
   encodeStaticLeg(ms,4,160,SignalRemoveSprite,0,rps.highYSpeed);  // drop
   encodeStaticLeg(ms,5,15,SignalNoSignal,backX,rps.slowYSpeed);  // 5,6 ONLY used after drop
   encodeStaticLeg(ms,6,160,SignalRemoveSprite,0,rps.highYSpeed);  // drop

   debugFuncExit("setDartBugMotion()");
}


//-----------------------------------------------------------------------
//  Encode backdrop star motion
//-----------------------------------------------------------------------
void setStarMotion(sprite *s)
{

   Byte deltaY;
   
   debugFuncEnterInt("setStarMotion() s",s->spriteNum);

   if (s->spriteNum < firstBackDropSpriteClass2) {
      s->bitmap=starBmp;
      deltaY=slowStarSpeed;
      s->x = backDropSpacing * (s->spriteNum - firstBackDropSprite) + randomNum(20) +20; 
   } else {
      // Faster random 
      s->bitmap=star2Bmp;
      deltaY=fastStarSpeed;
      s->x = 40 + randomNum(100);
   } 

   s->y = yMinBorder +2;

   // Directly write the moveStruct so that DoNewMovement does not have to load it
   s->myMoveStruct.currentLeg=0;
   s->myMoveStruct.countdown = 200;
   s->myMoveStruct.sendThisEvent = SignalRemoveSprite;

   s->myMoveStruct.leg[0].legType=(deltaY>0) ? staticLegYIncr : staticLegYDecr;
   s->myMoveStruct.leg[0].guts.staticLegStruct.deltaX=0;
   s->myMoveStruct.leg[0].guts.staticLegStruct.deltaY=deltaY;
   
}


//-----------------------------------------------------------------------
// setBlitzMotion
//  
// Used for end-of-round all out shootout (modeled after Sweep bug)
//-----------------------------------------------------------------------
void setBlitzMotion(
   sprite * s
)
{
   Int dir;
   moveStruct * ms;
   Byte d=3;
   

   flightPtStruct fps[] = { {0,0,SignalBulletPoint}, 
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalBulletPoint},
                            {0,0,SignalRemoveSprite}, 
                            NOMORE };

   debugFuncEnter("setBlitzMotion()"); 

   // left=true
   dir = ( chooseLeftOrRight(s) ) ? -1 : +1;
   
   // can't set the x or y here because we don't know if the bug is diving from the top
   // of screen, or from formation
   fps[0].x = s->x;
   fps[0].y = s->y+yd;
   // zig
   fps[1].x = s->x + 3*xd*dir;
   fps[1].y = s->y + (3*yd);
   // zag 1
   fps[2].x = s->x + 1*xd*dir;
   fps[2].y = s->y + (7*yd);
   // zag 2
   fps[3].x = s->x + 2*xd*(-dir);
   fps[3].y = s->y + (13*yd) -rps.aimAtPlayerVariance;
   // aim at user 
   fps[4].x = GetUserSprite()->x;  
   fps[4].y = 135;
   // drop down 
   fps[5].x = fps[4].x;
   fps[5].y = 24*yd;

   // hard-coded speed (here
   encodeFlightpath(s,-1,fps,4,4);

   ms=&s->myMoveStruct;

   // Randomize the delta X
   d= (randomPercTest(50)) ? 3 : 2;  // deltaX was 4
   encodeHomeToLeg(ms,4,20,SignalBulletPoint,d,4 );  
   

   // Overwrite border actions set in encodeFlightpath
   s->borderAction[1]=bounce;  
   s->borderAction[2]=sendSignal;  
   s->borderAction[3]=bounce; 

   // Ensure sweep bug is showing diving bitmap
   if (s->bitmap == sweepBug2) {
      s->bitmap=sweepBug;
   }
   
  
   debugFuncExit("setBlitzMotion()");
}


//-----------------------------------------------------------------------
//  Movement leg encoder == Hide from view
//
//  Bug's is hidden offscreen for time period.  For q'ing.  
//-----------------------------------------------------------------------
void encodeHideFromView(
  moveStruct *  ms,
  Int           leg,
  Int           duration,
  Int           restoreX,
  Int           restoreY
)
{
   ms->leg[leg].legType                           = hideFromView;
   ms->leg[leg].duration                          = duration;
   ms->leg[leg].sendThisEvent                     = SignalNoSignal;
   ms->leg[leg].guts.hideFromViewStruct.restoreX  = restoreX;
   ms->leg[leg].guts.hideFromViewStruct.restoreY  = restoreY;

   debugFuncExit("encodeHideFromView()");
}


//-----------------------------------------------------------------------
//  Movement leg encoder == static leg
//
//  Bug moves in a straight line  
//-----------------------------------------------------------------------
void encodeStaticLeg(
  moveStruct *  ms,
  Int           leg,
  Int           duration,
  Int           sendThisEvent,
  Int           deltaX,
  Int           deltaY
)
{

   debugFuncEnter("encodeStaticLeg()");

   ms->leg[leg].legType=staticLeg;
   ms->leg[leg].duration=duration;
   ms->leg[leg].sendThisEvent=sendThisEvent;
   ms->leg[leg].guts.staticLegStruct.deltaX=deltaX;
   ms->leg[leg].guts.staticLegStruct.deltaY=deltaY;

   debugFuncExit("encodeStaticLeg()");
}

//-----------------------------------------------------------------------
//  Movement leg encoder 
//
//  Bug homes towards user 
//-----------------------------------------------------------------------
void encodeHomeToLeg(
  moveStruct *  ms,
  Int           leg,
  Int           duration,
  Int           sendThisEvent,
  Int           deltaX,
  Int           deltaY
)
{

  // HomeToLeg MUST USE STATICLEG STRUCT LIKE BELOW DUE TO CONVERSION
  // IN DONEWMOVEMENT!

   ms->leg[leg].legType=homeToLeg;
   ms->leg[leg].duration=duration;
   ms->leg[leg].sendThisEvent=sendThisEvent;
   ms->leg[leg].guts.staticLegStruct.deltaX=deltaX;
   ms->leg[leg].guts.staticLegStruct.deltaY=deltaY;

   debugFuncExit("encodeHomeToLeg()");
}


//-----------------------------------------------------------------------
//  Load the drop-then-diag-right/left intro movement
//-----------------------------------------------------------------------
void setDropThenDiag(
   sprite * s, 
   Int      numInLine,
   Boolean  diagRt
)
{
   Int deltaX, qTime,r;
   moveStruct * ms;

   // debug
   debugFuncEnter("setDropThenDiagRt()"); 
   debugPutInt("   sprite",s->spriteNum);

   // Calculate how long the sprite should be off screen (queuing)
   qTime = numInLine * 5 + 1;

   // diag left or right
   deltaX = ( diagRt ) ? rps.bugXSpeedIntro+1 : -(rps.bugXSpeedIntro+1);
   r=randomNum(10);

   // legs
   ms = setSpriteForDynamicMovement(s);
   encodeHideFromView(ms,0,qTime,80,0);      // offscreen time, restore to (80,0)
   encodeStaticLeg(ms,1,50/rps.bugYSpeedIntro,SignalNoSignal,0,rps.bugYSpeedIntro);   // drop
   encodeStaticLeg(ms,2,r,SignalBulletPoint,deltaX,rps.bugYSpeedIntro-1);   // diag to bullet
   encodeStaticLeg(ms,3,100,SignalRemoveSprite,deltaX,rps.bugYSpeedIntro-1);   // more diag


   debugFuncExit("setDropThenDiag()");
}



//-----------------------------------------------------------------------
//  utility proc
//-----------------------------------------------------------------------
moveStruct * setSpriteForDynamicMovement(sprite * s)
{ 
   Int i=0;
   moveStruct * ms;

   // Instruct sprEng to control this bug
   s->moveStructOwner = sprEng; 
   ms=&s->myMoveStruct; 

   // Set the moveStruct info
   ms->currentLeg=-1;  // have sprEng load leg 0

   // Safety check for 
   for (i=0;i<MAXLEGS;i++)
   {
       ms->leg[i].duration=1;
       ms->leg[i].sendThisEvent = SignalRemoveSprite;
   }


   debugFuncExit("setSpriteForDynamicMovement()");
   return ms;
}

 

//-----------------------------------------------------------------------
// 
//  Load the direction-then-stop intro movement
//
//-----------------------------------------------------------------------
void dirThenStop(
   sprite * s, 
   Int      numInLine,
   Int      totalNumSprites,
   Int      targX,         
   Int      targY,        
   Int      deltaX,
   Int      deltaY
)
{
   Int qTime=0, n,dur=0,r;
   Int restoreX=0, restoreY=0;
   moveStruct * ms;

   n=rps.bugXSpeedIntro; // n=speed factor

   // Handle different entrances
   switch (deltaX)
   {
     case(0):
       // drpThenStop
       debugFuncEnterInt   ("dirThenStop(), drpThenStop    sprite",s->spriteNum);
       restoreX=targX;
       restoreY=(targY % n);
       dur = (targY -restoreY) /n;
       qTime = numInLine * 5 + 1;
       s->borderAction[0]=noAction;
       break;

     case(1):
       // rtThenStop
       debugFuncEnterInt   ("dirThenStop(), rtThenStop  sprite",s->spriteNum);
       restoreY=targY;
       restoreX=(targX % n)+ (2*n);    // extra 2*n to help solve bitmap bleed 
       dur = (targX - restoreX) / n;
       qTime = (totalNumSprites - numInLine -1) * 5 + 1;  // prevents overlapping
       s->borderAction[3]=noAction;
       break;

     case(-1):
       // lfThenStop
       debugFuncEnterInt   ("dirThenStop(), lfThenStop  sprite",s->spriteNum);
       restoreY=targY;
       restoreX=160-((160-targX) % n) - (2*n);
       dur = (restoreX - targX) / n;
       qTime = numInLine * 5 + 1;
       s->borderAction[1]=noAction;
       break;
   }

   r=randomNum(dur-2)+1; // must not be zero

   debugPutInt("restoreX",restoreX);
   debugPutInt("restoreY",restoreY);
   debugPutInt("dur",dur);

   // legs
   ms = setSpriteForDynamicMovement(s); 

   encodeHideFromView (ms,0,qTime,restoreX,restoreY);        // offscreen time, restore to (x,y)

   if (dur>3) {
      encodeStaticLeg    (ms,1,r,SignalBulletPoint,deltaX*n,deltaY*n);
      encodeStaticLeg    (ms,2,dur-r,SignalBugBackInForm,deltaX*n,deltaY*n);
   } else {
      encodeStaticLeg    (ms,1,dur,SignalBugBackInForm,deltaX*n,deltaY*n);
   }

   debugFuncExit("dirThenStop()");
}



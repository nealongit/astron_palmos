//
//  debug.c
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

#include "debug.h"
#include "starter.h"
#include "sprite.h"  // for SpriteEngNumSpritesRendered
#include "graphics.h" // for DebugRunGraphicsTest, etc.
#include "gameman.h" // for DebugDisplay..

// For debug text..
#include <PalmTypes.h>
#include <Font.h> 
#include "graphics.h"
#include <Rect.h>

//----------------------------------------------------------------------
//  Save-ABLES: none
//----------------------------------------------------------------------

UInt32 DebugStopwatch=0;
UInt16 DebugNumFrames=0;
Int32  DebugTotalTime=0;
UInt16 DebugTimedFrames[MAXTIMEDINCRS];
Int32  DebugTimedIntraFrames[10] = {0,0,0,0,0,0,0,0,0,0};

UInt8   DebugCurrectVarMeas=0;
Int32   DebugVarTiming[NUMVARMEAS];
Boolean DebugVarTimingOverTop=false;

UInt8   debugHighTime=0;




#ifndef THIS_IS_A_RELEASE_LOAD

//----------------------------------------------------------------------
//                DEBUG -- for testing
//----------------------------------------------------------------------

Boolean SKIPINTROWAVES = false;    // Just put bugs into formation and start gameplay
Boolean SKIPREGPLAY    = false;    // Just do the intro's
Boolean AUTOPLAY       = false;     // Inject random user movements to simulate play
Boolean TITLEBAROFF    = false;         // Display no
Boolean USERIMMUNE     = true;
Boolean HOLDFORMATION  = false;   // true= all bugs stay in formation     
Boolean DEBUGTEXT      = false;
Boolean TIMEFRAMES     = true;    // time frames, and display to screen at end of round
Boolean ALLOWDEBUGMENU = true;    // true= allow debug menu access via page up hard button
Boolean IGNORETRACBEAM = false;    // true = ignore being hit by tractor beam (mainly for autoplay)
Boolean DEBUGCOLLTIMINGON = false;



debugLevelEnum DEBUGLEVEL = allDebugText;
debugLevelEnum OLDDEBUGLEVEL = noDebugText;

//----------------------------------------------------------------------
//               
//----------------------------------------------------------------------
void DebugClearTimingVars() 
{

   UInt16 i=0;

   for (i=0;i<MAXTIMEDINCRS;i++) {
      DebugTimedFrames[i]=0;
   }

   for (i=0;i<MAXTIMEDINTRAMEAS;i++) {
      DebugTimedIntraFrames[i]=0;
   }

   DebugStopwatch=0; // required because timer is running while on debug screen
   DebugTotalTime=0;
   DebugNumFrames=0;
   DebugCurrectVarMeas=0;
   DebugVarTimingOverTop=false;
   debugHighTime=0;
   

}



//----------------------------------------------------------------------
//               
//----------------------------------------------------------------------
void DebugDisplayTimingText()
{
   
   char t2[60]=" ";
   char t3[60]=" ";
   char t4[60]=" ";
   char t5[80]=" ";
   char t6[60]=" ";
   UInt16 n=0,nh=0,m=0;
   Int t,i=0,p=0;
   Int numZeroLen=0;
   

   UInt32 ave=0,goodFrames=0;

   // dist vars:
   UInt16 overAve=0,x=0;
   UInt16 dist[]={0,0,0,0,0};
   UInt16 tickCount[]={0,0,0,0,0,0,0};
   


   // debug for frame timing
   if (TIMEFRAMES) {

      // Display num frames
      debugPutInt("DebugNumFrames",DebugNumFrames);
      StrIToA(t2,DebugNumFrames);
      StrCopy(t3,"NumFrames=");
      StrCat(t3,t2);

      // Find Min, Max nonzero entry
      n=0;
      
      for (i=0;i<MAXTIMEDINCRS;i++) {
	 if ((DebugTimedFrames[i] != 0) && (n==0)) {
	    n=i;
	 }

	 // Count number which are good frames (well timed)
	 if (i <= MAXBESTFRAMETIME) {
	    goodFrames+=DebugTimedFrames[i];
	 }
	 
      }

      StrCat(t3,"   good= ");
      p=(goodFrames*100)/DebugNumFrames;
      StrIToA(t2,p);
      StrCat(t3,t2);
      StrCat(t3,"%");   

      TextMgrAddCenteredText(t3,1,stdFont,58,false);
      StrCopy(t3,"");



      nh=MAXTIMEDINCRS-1;
      
      
      debugPutInt("n",n);
      

      // (4)% 20,30,0,12 ....... Display per-frame total timing %
      StrCopy(t3,"(");
      StrIToA(t2,n);
      StrCat(t3,t2);
      StrCat(t3,")%");

      for (i=n;i<=nh;i++) {
	 p=(DebugTimedFrames[i]*100)/DebugNumFrames;
      
	 StrIToA(t2,p);
	 StrCat(t3,t2);
	 StrCat(t3,",");
	 debugPutInt("DebugTimedFrames[]",DebugTimedFrames);
      }
      TextMgrAddCenteredText(t3,2,stdFont,70,false);
      
      
      // Intra: 0,0,0,10 ......... Display intra-frame timing
      StrCopy(t4,"Intra: ");
      for (i=1;i<=9;i++) {
	 t=(DebugTimedIntraFrames[i]*10)/DebugNumFrames;
      
	 StrIToA(t2,t);
	 debugPutInt("DebugTimedIntraFrames[i]",DebugTimedIntraFrames[i]);
       
	 StrCat(t4,t2);
	 StrCat(t4,",");
      }
      TextMgrAddCenteredText(t4,3,stdFont,82,false); 



      // Var ticks*10 (m,ave,M) ....... Find the Min, max, ave variance    
      m = (DebugVarTimingOverTop) ? NUMVARMEAS : DebugCurrectVarMeas;
      debugPutInt("m",m);
      
      n=100;
      nh=0;
      ave=0;
      
      for (i=0;i<m;i++) {

	 if (DebugVarTiming[i]==0) {
	    numZeroLen++;
	 }

	 if (DebugVarTiming[i]<n) {
	    n= DebugVarTiming[i];
	 }
	 if (DebugVarTiming[i]>nh) {
	    nh= DebugVarTiming[i];
	 }

	 ave+=DebugVarTiming[i]*10;
      
      }

      ave = ave/m;
      

      StrCopy(t5,"Var ticks*10 (m,ave,M)= ");
      StrIToA(t2,n*10);
      StrCat(t5,t2);
      StrCat(t5,",");
      StrIToA(t2,ave);
      StrCat(t5,t2);
      StrCat(t5,",");
      StrIToA(t2,nh*10);
      StrCat(t5,t2);

      TextMgrAddCenteredText(t5,4,stdFont,96,false);  


      // Now find low distribution, calculate how many samples are .5 ticks away from
      // the average
      for (i=0;i<m;i++) {
	 n=DebugVarTiming[i]*10;
	 
	 // Below, equal ave:
	 if (n <= ave) {
	    // See how many .5 ticks over we are
	    x=(ave-n)/5;

	    //debugPutInt("ave",ave);
	    //debugPutInt("x",x);
	    //debugPutInt("n",n);
	    
	    x=(x>4) ? 4:x;  // cap it at 2.0 ticks 
	    dist[x]++;
	    
	 }
      }
    

      StrCopy(t5,"Dis: (");
      StrIToA(t2,numZeroLen);    // number of frames 0 in length
      StrCat(t5,t2);
      StrCat(t5,") ");

      StrCopy(t6,"%  : (");
      StrIToA(t2,numZeroLen*100/m);
      StrCat(t6,t2);
      StrCat(t6,") ");
      
      for (i=4;i>=0;i--) {
	 StrIToA(t2,dist[i]);
	 StrCat(t5,t2);
	 if (i>0) {StrCat(t5,",");}

	 StrIToA(t2,(dist[i]*100/m)); //%
	 StrCat(t6,t2);
	 if (i>0) {StrCat(t6,",");}
	 
	 
	 dist[i]=0; // rest for below..
      }

      StrCat(t5,"=AVE=");
      StrCat(t6,"=AVE=");
      got_here;
      

      

      // Now find high distribution, calculate how many samples are .5 ticks away from
      // the average
      for (i=0;i<m;i++) {
     
	 n=DebugVarTiming[i]*10;
	 
	 // Above ave:
	 if (n > ave) {
	    // See how many .5 ticks over we are
	    x=(n-ave)/5;

	    x=(x>4) ? 4:x;  // cap it at 2.0 ticks 
	    overAve++;
	    dist[x]++;
	 }

	 // Count the straight tick counts
	 tickCount[DebugVarTiming[i]]++;
	 

      }
     
      for (i=0;i<5;i++) {
	 StrIToA(t2,dist[i]);
	 StrCat(t5,t2);
	 if (i!=4){StrCat(t5,",");}

	 StrIToA(t2,(dist[i]*100/m)); //%
	 StrCat(t6,t2);
	 if (i!=4){StrCat(t6,",");}
	 
      }

      TextMgrAddCenteredText(t5,5,stdFont,108,false);
      TextMgrAddCenteredText(t6,6,stdFont,120,false); 

      
      // Tick Count
      StrCopy(t5,"Count @0: ");
      for (i=0;i<7;i++) {
	 StrIToA(t2,tickCount[i]);
	 StrCat(t5,t2);
	 StrCat(t5,",");
      }

      TextMgrAddCenteredText(t5,7,stdFont,132,false); 

      // Reset timing vars
      DebugClearTimingVars();  

   }

   StrCopy(t5,"NumBugs=");
   StrIToA(t6,roundInfo.numRemainBugs);
   //StrCopy(t5,"NumSprites=");
   //StrIToA(t6,SpriteEngNumSpritesRendered);
   StrCat(t5,t6);
   StrCat(t5," CPU halts=");
   StrIToA(t6,UtilCPURestrictions);
   StrCat(t5,t6);
   StrCat(t5," dep=");
   StrIToA(t6,ScreenDepth);
   StrCat(t5,t6);
   TextMgrAddCenteredText(t5,8,stdFont,144,false);


   // Put text on screen
   WinSetDrawWindow(scrnBuffer);  
   TextMgrDisplayGameText();
   WinSetDrawWindow(backBuffer);  

   
}


//----------------------------------------------------------------------
//  MUST GO BEFORE debugDumpElapTime SINCE THAT PROC ZERO'S THE TIMER!
//----------------------------------------------------------------------
void DebugUpdateVarTiming()
{
   // Store the time, incrment the counter
   DebugVarTiming[DebugCurrectVarMeas]=TimGetTicks()-DebugStopwatch;

   if (DebugCurrectVarMeas == (NUMVARMEAS-1)) {
      DebugCurrectVarMeas=0;
      DebugVarTimingOverTop=true;
   } else {
      DebugCurrectVarMeas++;
   }
   
}

/*
//----------------------------------------------------------------------
//               
//----------------------------------------------------------------------
void DebugRunGraphicsTest()
{

   Int start,stop,i,n,x,numLoops=10;
   sprite * s;
   char msg[60] = "";
   char temp[60] = "";
   
   DEBUGCOLLTIMINGON=true;
   
   // First deactivate all sprites
   for (i=0;i<=SpriteEngHighestSpriteNum;i++) {
      s=allSprites[i];
      DelFromActiveList(s);
   }

   // Create sprites
   for (i=0;i<36;i++) {
      s=allSprites[i];
      DelFromActiveList(s);
      s->x=i+10;
      s->y=i*2+40;
      s->bitmap=sweepBug;
   }

   for (i=36;i<44;i++) {
      s=allSprites[i];
      DelFromActiveList(s);
      s->x=i+10;
      s->y=i*2+50;
      s->bitmap=fatBulletBmp;
   } 

   for (i=44;i<50;i++) {
      s=allSprites[i];
      DelFromActiveList(s);
      s->x=10+2*i;
      s->y=i*2+23;
      s->bitmap=starBmp;
   } 
   
   // Loop over tests
   for (n=0;n<60;n+=10) {

      for (i=0;i<n;i++) {
	 s=allSprites[i];
	 AddToActiveList(s);
      }
      
      start = TimGetTicks();

      for (x=0;x<numLoops;x++) {
	 GraphicsClrBackBuffer();
	 Graphics4BitASMDrawAllSprites();
	 GraphicsRepaint();
      }
      
      stop = TimGetTicks();

      StrIToA(temp,n);
      StrCopy(msg,"For n=");
      StrCat(msg,temp);
      StrCat(msg," sprites, ");
      StrIToA(temp,(stop-start));
      StrCat(msg,temp);
      StrCat(msg," ticks, loops=");
      StrIToA(temp,numLoops);
      StrCat(msg,temp);

      FrmCustomAlert(nealErrAlertId,"",0,msg);
   }

   DEBUGCOLLTIMINGON = false;
   
}
*/


//----------------------------------------------------------------------
//               
//----------------------------------------------------------------------
void DebugRunCollisionTest()
{

   Int start,stop,i,x,y,numLoops=10,elapTime=0,nl,nt;
   sprite * s;
   char msg[60] = "";
   char temp[60] = "";
   
   debugFuncEnter("DebugRunCollisionTest()");
   
   DEBUGCOLLTIMINGON=true;
   

   // Loop over tests
   for (nt=0;nt<10;nt++) {

      elapTime=0;
      debugPutInt("test",nt);
      
 
      for (nl=0;nl<numLoops;nl++) {

	 // First deactivate all sprites
	 for (i=0;i<=SpriteEngHighestSpriteNum;i++) {
	    s=allSprites[i];
	    DelFromActiveList(s);
	 }
	 
	 // Create sprites
	 for (x=0;x<8;x++) {

	    //    for (y=3;y>=0;y--) {
	    for (y=0;y<4;y++) {

	       s=SpriteEngFindFirstUnusedSprite(BugSpriteClass);
	       AddToActiveList(s);
	       s->x=10+x*14;
	       s->y=6+y*12;
	       s->bitmap=sweepBug;

	       // This would already be done..
	       s->realY1 = s->y + bmpTableNew[s->bitmap].y1;
	       s->realY2 = s->y + bmpTableNew[s->bitmap].y2;
	       s->realY12calc = true;
	       //s->realCogY = s->y + bmpTableNew[s->bitmap].centerY;
	       //s->realCogYcalc = true; 

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

	    }
	 }
	 
	 // Create bullets
	 for (y=0;y<3;y++) {
	    for (x=0;x<1;x++) {
	       s=SpriteEngFindFirstUnusedSprite(UserBulletSpriteClass);
	       AddToActiveList(s);
	       s->x=86+(x*12);
	       s->y=100-(y*10)-(nt*10);
	       s->bitmap=fatBulletBmp;

	       // this would already be done...
	       s->realCogX = s->x + bmpTableNew[s->bitmap].centerX;
	       s->realCogXcalc = true;
	       //s->realX1 = s->x + bmpTableNew[s->bitmap].x1;
	       //s->realX2 = s->x + bmpTableNew[s->bitmap].x2;
	       //s->realX12calc = true;
	       

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
	       }   // end-of-region set


	    }
	 }
	 
	 
	 // Render...
	 GraphicsClrBackBuffer();
	 Graphics4BitASMDrawAllSprites();
	 GraphicsRepaint();

	 OMcollisionTests=0;
	 OMnotInSameRegion=0;
	 OMfourCornerLevel1Checks=0;
	 OMfourCornerLevel2Checks=0;
	 OMcogLevel1Checks=0;
	 OMcogLevel2Checks=0;
	 OMCogXcalculations=0;
	 OMRealX12calc=0;
	 OMCogYcalculations=0;
	 OMRealY12calc=0;

	 
	 start = TimGetTicks(); 
	 SpriteEngCheckForCollisions();
	 stop = TimGetTicks();

	 elapTime += (stop-start);

      } // numLoops
      


      StrIToA(temp,elapTime);
      StrCopy(msg,temp);
      StrCat(msg," ticks for ");
      StrIToA(temp,numLoops);
      StrCat(msg,temp);
      StrCat(msg," loops, OMs=\n");
      StrIToA(temp,OMcollisionTests);StrCat(msg,temp);StrCat(msg,"-");
      StrIToA(temp,OMnotInSameRegion);StrCat(msg,temp);StrCat(msg,"-");
      StrIToA(temp,OMfourCornerLevel1Checks);StrCat(msg,temp);StrCat(msg,"-"); // 3
      StrIToA(temp,OMfourCornerLevel2Checks);StrCat(msg,temp);StrCat(msg,"-"); // 4
      StrIToA(temp,OMcogLevel1Checks);StrCat(msg,temp);StrCat(msg,"-");  // 5
      StrIToA(temp,OMcogLevel2Checks);StrCat(msg,temp);StrCat(msg,"-");  // 6
      StrIToA(temp,OMCogXcalculations);StrCat(msg,temp);StrCat(msg,"-");
      StrIToA(temp,OMRealX12calc);StrCat(msg,temp);StrCat(msg,"-");
      StrIToA(temp,OMCogYcalculations);StrCat(msg,temp);StrCat(msg,"-");
      StrIToA(temp,OMRealY12calc);StrCat(msg,temp);

      FrmCustomAlert(nealErrAlertId,"",0,msg);

   }// numTests

   DEBUGCOLLTIMINGON=false;
   

   debugFuncExit("DebugRunCollisionTest()");

}



//----------------------------------------------------------------------
//               
//----------------------------------------------------------------------
void DebugFindHighTime(UInt8 ticks) 
{

   char msg[40]="",temp[40]="";
   
   
   if (ticks>debugHighTime) {
      debugHighTime=ticks;
      StrCopy(msg,"High time observed.. ");
      StrIToA(temp,ticks);
      StrCat(msg,temp);
      FrmCustomAlert(nealErrAlertId,"",0,msg);
   }
   
}


/*
//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
Byte DebugCalcCollRegion(sprite *s) 
{
   

   if (s->y >= region2l) {
      return region1__;
   } else {
      if (s->y >= region1h) {
	 return region12_;
      } else {
	 if (s->y >= region3l) {
	    return region_2_;
	 } else {
	    if (s->y >= region2h) {
	       return region_23;
	    } else {
	       return region__3;
	    }
	 }
      }
   }   

   
}

*/


//----------------------------------------------------------------------
// DebugCheckCOG
//
// Validate the collision flags and numbers
//----------------------------------------------------------------------
void DebugCheckCOG(sprite * thisSprite) 
{

   UInt16 calcCOGY,calcCOGX,x1,x2,y1,y2;
   char msg[40]="",temp[40]="";
   
   
   if (thisSprite->realCogYcalc) {
      // calc real Y c.o.g, and cache it..
      calcCOGY = thisSprite->y + bmpTableNew[thisSprite->bitmap].centerY;

      if (thisSprite->realCogY != calcCOGY) {
	 thisSprite->bitmap=9;
	 StrCopy(msg,"calcCOGY error, sprite=");
	 StrIToA(temp,thisSprite->spriteNum);
	 
	 StrCat(msg,temp);
	 ERROR_ASSERT(msg);
      }
   }
   
   if (thisSprite->realCogXcalc) {
      // calc real X c.o.g, and cache it..
      calcCOGX = thisSprite->x + bmpTableNew[thisSprite->bitmap].centerX;   

      if (thisSprite->realCogX != calcCOGX) {
	 thisSprite->bitmap=9;
	 StrCopy(msg,"calcCOGX error, sprite=");
	 StrIToA(temp,thisSprite->spriteNum);
	 
	 StrCat(msg,temp);
	 ERROR_ASSERT(msg);
      }
   }


   if (thisSprite->realX12calc) {
      x1 = thisSprite->x + bmpTableNew[thisSprite->bitmap].x1;
      x2 = thisSprite->x + bmpTableNew[thisSprite->bitmap].x2;

      if (x1 != thisSprite->realX1) {
	 thisSprite->bitmap=9;
	 StrCopy(msg,"x1 error, sprite=");
	 StrIToA(temp,thisSprite->spriteNum);
	 
	 StrCat(msg,temp);
	 ERROR_ASSERT(msg);
      }

      if (x2 != thisSprite->realX2) {
	 thisSprite->bitmap=9;
	 StrCopy(msg,"x2 error, sprite=");
	 StrIToA(temp,thisSprite->spriteNum);
	 
	 StrCat(msg,temp);
	 ERROR_ASSERT(msg);
      }


   }


   if (thisSprite->realY12calc) {
      // calc real Y1, Y2 and cache them..
      y1 = thisSprite->y + bmpTableNew[thisSprite->bitmap].y1;
      y2 = thisSprite->y + bmpTableNew[thisSprite->bitmap].y2;

      if (y1 != thisSprite->realY1) {
	 thisSprite->bitmap=9;
	 StrCopy(msg,"y1 error, sprite=");
	 StrIToA(temp,thisSprite->spriteNum);
	 
	 StrCat(msg,temp);
	 ERROR_ASSERT(msg);
      }

      if (y2 != thisSprite->realY2) {
	 thisSprite->bitmap=9;
	 StrCopy(msg,"y2 error, sprite=");
	 StrIToA(temp,thisSprite->spriteNum);
	 
	 StrCat(msg,temp);
	 ERROR_ASSERT(msg);
      }


   }
		      
}




//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void DebugTestRepaints() 
{

   Int time1,time2,i;
   char msg[40]="",temp[40]="";
   RectangleType rect;  

   RctSetRectangle(&rect,0,0,159,159);

   time1=TimGetTicks();
   for (i=0;i<100;i++) {
      GraphicsRepaintNative();
   }
   time2=TimGetTicks();

   StrCopy(msg,"GraphicsRepaintNative=");
   StrIToA(temp,(time2-time1));
   StrCat(msg,temp);
   ERROR_ASSERT(msg);

   time1=TimGetTicks();
   for (i=0;i<100;i++) {
      GraphicsRepaint();
   }
   time2=TimGetTicks();

   StrCopy(msg,"GraphicsRepaint=");
   StrIToA(temp,(time2-time1));
   StrCat(msg,temp);
   ERROR_ASSERT(msg);

   
}




//----------------------------------------------------------------------
// 
//----------------------------------------------------------------------
void DebugCheckCollRegion() 
{
   sprite * s;
   Byte n;
   
   for (n=0;n<numSprites;n++) 
   {
      if (isActive(allSprites[n]))
      {

	 s=allSprites[n];

	 DebugCheckCOG(s);
	 

	 //allSprites[n]->bitmap = allSprites[n]->collisionRegion;
	 
	 /*
	 if (s->realX12calc) {
	    // Get rough approx of realY1
	    s->bitmap = s->realX1/16;
	 }	 
	 */

	 /*
	 if ( (allSprites[n]->collisionRegion != UtilCalcCollRegion(allSprites[n]))
	 {
	    debugPutInt("s",allSprites[n]->spriteNum);
	    ERROR_ASSERT("bug collision region mismatch");
	 }
	 */
	 
      }
   } // for i





   ERROR_ASSERT("Done.");
   

}

      
 

#else
//----------------------------------------------------------------------
//                DEBUG -- for RELEASE
//----------------------------------------------------------------------

Boolean SKIPINTROWAVES = false;    
Boolean SKIPREGPLAY    = false;   
Boolean AUTOPLAY       = false;   
Boolean TITLEBAROFF    = false;      
Boolean USERIMMUNE     = false;
Boolean HOLDFORMATION  = false;   
Boolean DEBUGTEXT      = false;
Boolean TIMEFRAMES     = false; 
Boolean ALLOWDEBUGMENU = false; 
Boolean IGNORETRACBEAM = false;
Boolean DEBUGCOLLTIMINGON = false;


debugLevelEnum DEBUGLEVEL = noDebugText;
debugLevelEnum OLDDEBUGLEVEL = noDebugText;


#endif //THIS_IS_A_RELEASE_LOAD

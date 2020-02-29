//
//  STARTER.C
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

#include <PalmOs.h>
#include <SysEvtMgr.h>
#include <TimeMgr.h>      // Time ticks
#include <KeyMgr.h>
#include <HostControl.h>  // palm reporter 
#include <DebugMgr.h>
#include <Form.h>
#include <Font.h>         // splash screen
#include <DLServer.h>     // to retrieve user name (registration screen)
#include <UIColor.h>      // Added for color functionality
 
// My includes
#include "starter.h"
#include "setup.h"
#include "misc.h"
#include "defs.h"
#include "debug.h"
#include "sprite.h" 
#include "graphics.h"
#include "blast.h"
#include "gameman.h"
#include "database.h"

//--------------------------------------------------------------------  
// Prototypes
//--------------------------------------------------------------------

static Err  AppInit(void);
static void AppStop(void);
static void AppEventLoop(void);



//--------------------------------------------------------------------  
// Global variables
//--------------------------------------------------------------------


// key storage 
UInt32   oldKeyMask;
UInt16   keyInitDelay;
UInt16   keyPeriodP; 
UInt16   keyDoubleTapDelay;
Boolean  keyQueueAhead;
Boolean  hardKeysChanged=false;

// records
mainGameRecordStructV1 mainGameRec1;   
mainGameRecordStructV1 mainGameRec1Default = 
                          {1,4,0,0,0,1,appNewlyInstalled,0,{0,0,0,0},0,0,false};
mainGameRecordStructV2 mainGameRec2;
// rec 2 default 
mainGameRecordStructV3 mainGameRec3;
mainGameRecordStructV3 mainGameRec3Default = {1,ASTRONVERSION,0};
mainGameRecordStructV4 mainGameRec4;
mainGameRecordStructV4 mainGameRec4Default = {3,1,1,cpuUnknown,normalGreyscaleLvl,false};


// timing
UInt16 gt=0;
UInt32 MinTicksPerFrame=0;
UInt32 MaxTicksBeforeText=0;

// cpu
CPULevelEnum UtilCPULevel=CPULevelOK;
UInt16       UtilCPURestrictions=0;

// app state, etc
appStateEnum appState = splashScreen;
Boolean      allRecordMustBeRewritten=false;
ULong        lastNagTimerSeconds=0;
ULong        stopNagTimerSeconds=0;
ULong        firstNagTimerSeconds=0;
Byte         nagTimerVal=0;
Byte         origGraphicsLevel=0;


// password
Byte    pwWord[4]={0,0,0,0};
Byte    numTimesRun=0;              // mainGameRec1
ULong   installTimGetSeconds=0;     // mainGameRec1
Boolean tenDayAdditionOn=false;     // mainGameRec1

ULong   lastTimRan=0;                // mainGameRec2
Boolean timeModifiedFraudFlag=false; // mainGameRec2
UInt16  numTimesRestarted=0;         // mainGameRec2

// form trackers
UInt16 startOnThisForm;
UInt16 lastForm;

// ROM version
UInt32 romVersion;

// Splash screen animation
Byte bmpFrame=0;
ULong tickNextBmpFrame=0;
Byte ticksBetwBmpFrame = 16;

// Define the minimum OS version we support... this macro (where is it?)
// makes myMinVersion loook like 0x03000000
#define myMinVersion	sysMakeROMVersion(3,0,0,sysROMStageRelease,0)


// *** IMPORTANT VAR ***
// ScreenDepth is set to the capability of the device at startup.  Many
// UI routines must reference this variable
gameScreenDepthEnum ScreenDepth=oneBitDepth;  // default for now




//--------------------------------------------------------------------
// RomVersionCompatible!
//!
// Code is directly from Palm
//-------------------------------------------------------------------- 
Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags)
{

    // See if we're on in minimum required version of the ROM or later.
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

    if (romVersion < requiredVersion)
    {
       if ((launchFlags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
	   (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp))
       {
	  FrmAlert (romIncompatAlertId);
	  
	  // Palm OS 1.0 will continuously relaunch this app unless we switch to 
	  // another safe one.
	  if (romVersion < myMinVersion)
	  {
	     AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL);
	  }
       }
       
       debugFuncExit("RomVersionCompatible()");
       return sysErrRomIncompatible;
    }
    
    debugFuncExit("RomVersionCompatible()");
    
    return errNone;

}


/***********************************************************************
 *
 * FUNCTION:    PilotMain
 *
 * DESCRIPTION: This is the main entry point for the application.
 *
 * PARAMETERS:  cmd - word value specifying the launch code. 
 *              cmdPB - pointer to a structure that is associated with the launch code. 
 *              launchFlags -  word value providing extra information about the launch.
 *
 ***********************************************************************/
DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags)
{
   Err error;

   switch (cmd)
   {
       case sysAppLaunchCmdNormalLaunch:

            // MAKE SURE WE ARE RUNNING PALMOS 3.0 OR HIGHER! OR WE WILL CRASH
            // Sound playing requires 3.0
            error = RomVersionCompatible(myMinVersion, launchFlags);
            if (error) return (error);


#ifndef THIS_IS_A_RELEASE_LOAD
	    // Fire up palm reporter (debug logs on POSEr)
	    HostTraceInit(); 
#endif

	    error = AppInit();
	    if (error) return error;
				
	    FrmGotoForm(startOnThisForm);
	    AppEventLoop();

            // Stop event received from OS
	    AppStop();
	    break;

       default:
	    break;

   }
	
   //debugFuncExit("PilotMain()");
   return 0;


}





//--------------------------------------------------------------------
// HdlMenuEvent
//
// Handle game screen and splash screen
//-------------------------------------------------------------------- 
static Boolean HdlMenuEvent(Word command)
{
	Boolean handled = false;

	switch (command)
        {

	   case mainMenuItem2:
	     // Exit game -- taken from SetUserDead()
	     gameInfo.gameState = gameOver;
             roundInfo.numTicks = gt+2; // hasten the end
	     //appState = gameIsOver;
	     //allRecordMustBeRewritten = true; // needed to blow away old game recs (really?)

	     handled = true;
	     break;

	  case mainMenuItem1:
	  case splashMenuItem0:
	     lastForm = FrmGetActiveFormID();
	     FrmGotoForm(optionsScreenForm);
	     handled = true;
	     break;

	  case splashMenuItem1:
	  case mainMenuItem3:
	     lastForm = FrmGetActiveFormID();
	     FrmGotoForm(regScreenForm);
	     handled = true;
	     break;

	  // Game -> "Playing Tips.."
	  case splashMenuItem4:
	  case mainMenuItem4:     
	     lastForm = FrmGetActiveFormID();
             FrmGotoForm(instructionsScreenForm);
	     handled = true;
	     break;

	  // Greyscale -> "Tips.."
	  case mainMenuItem7:
	
	     if ((ScreenDepth == greyTwoBitDepth) || (ScreenDepth == greyFourBitDepth))
	     {
		got_here;
		lastForm = FrmGetActiveFormID();
		FrmGotoForm(greyscaleInfoScreenForm);

	     } else {
		FrmCustomAlert(nealInfoAlertId,
		     "This menu item is only needed in the greyscale Astron version."," "," ");
	     }
	     
	     handled = true;
	     break;

	  // Greyscale -> "Theme.."
	  case mainMenuItem5:
	     if ((ScreenDepth == greyTwoBitDepth) || (ScreenDepth == greyFourBitDepth))
	     {
		lastForm = FrmGetActiveFormID();
		FrmGotoForm(greyscaleOpsScreenForm);
	     } else {
		FrmCustomAlert(nealInfoAlertId,
		    "This menu item is only needed in the greyscale Astron version."," "," ");
	     }
	     	     
	     handled = true;
	     break;

	  // Greyscale -> "Device Speed.."
	  case mainMenuItem6:
	     if ((ScreenDepth == greyTwoBitDepth) || (ScreenDepth == greyFourBitDepth))
	     {
		lastForm = FrmGetActiveFormID();
		FrmGotoForm(setPdaCPUForm);
	     } else {
		FrmCustomAlert(nealInfoAlertId,
		     "This menu item is only needed in the greyscale Astron version."," "," ");
	     }
	     
	     handled = true;
	     break;

	  case splashMenuItem2:
	     FrmAlert(aboutAlertId);
	     handled = true;
	     break; 

	}

	debugPutInt("Exit HdlMenuEvent(), command",command);
	return handled;

}


//--------------------------------------------------------------------
// HdlDebugMenuEvent
//--------------------------------------------------------------------
Boolean HdlDebugMenuEvent(UInt16 command)
{

   Boolean handled=false;
   UInt8 i;
   


	switch (command)
        {

	   case 0:
             // user killed
             SetUserDead();   // works alone ??
	     handled = true;
	     break;

	   case 1:
             gameInfo.twoShots=!gameInfo.twoShots;
	     handled = true;
	     break;

	   case 2:
	      // next round.. lots of hacks here..
	      for (i=0;i<MAXNUMBUGS;i++) {
		 DelFromActiveList(allSprites[i]);
	      }
	      gameInfo.gameState=restAfterGamePlay;
	      roundInfo.numBulletsFlying = 0;  //hack
	      roundInfo.numMinesFlying = 0; // hack
	      roundInfo.numTicks = gt + 5;
	      handled = true;
	      break;

	   case 3:
	     numTimesRun=60;
             numTimesRestarted=60;
             allRecordMustBeRewritten=true;
             handled=true;
	     break;

	   case 4:
             USERIMMUNE=!USERIMMUNE;
	     handled = true;
	     break;

	   case 5:
             HOLDFORMATION=!HOLDFORMATION;
             roundInfo.numTicks = gt+2;
	     handled = true;
	     break;

	   case 6:
             DEBUGTEXT=!DEBUGTEXT;
	     handled = true;
	     break;

	   case 7:
	     // Start autoplay, and make user immune while we're here
             AUTOPLAY=!AUTOPLAY;
             USERIMMUNE=true;
	     handled = true;
	     break;

	   case 8:
             SKIPINTROWAVES=!SKIPINTROWAVES;
	     handled = true;
	     break;

	   case 9:
             TIMEFRAMES=!TIMEFRAMES;
	     handled = true;
	     break;

	   case 10:
	     TITLEBAROFF=!TITLEBAROFF;
	     handled = true;
	     break;

	   case 11:
	     IGNORETRACBEAM =!IGNORETRACBEAM; 
	     handled = true;
	     break;

	   case 12:
	       // DebugRunGraphicsTest();
	     UtilDetermineCPUSpeed(true);
             handled=true;
	     break;  

	   case 13:
	      DebugCheckCollRegion();  // stolen space
	      //DebugRunCollisionTest();
	     handled = true;
	     break;

	   case 14:
	     DEBUGLEVEL=noDebugText;
	     handled = true;
	     break;

	   case 15:
	     DEBUGLEVEL=funcEnterOnly;
	     handled = true;
	     break;

	   case 16:
	     DEBUGLEVEL=allDebugText;
             handled=true;
	     break;  




       } // switch

	debugFuncExit("HdlDebugMenuEvent()");
	return handled;

   debugFuncExit("HdlDebugMenuEvent()");
}
 



/***********************************************************************
 *
 * FUNCTION:    gameScreenEventHandler
 *
 * PARAMETERS:  eventP  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event has handle and should not be passed
 *              to a higher level handler.
 *
 ***********************************************************************/
Boolean FormEventHandlerGameScreen(EventPtr eventP) 
{ 
   Boolean handled = false, skipTextDisplay=false;
   FormPtr frmP;

   Int tempTime6,tempTime1,timeTilText,b;

   switch (eventP->eType) 
   {

      case nilEvent:

         // make sure the active window is the form 
         if (WinGetActiveWindow() == (WinHandle)FrmGetActiveForm()) {

	   if ((appState == gameIsActive) ||
	       (appState == gameIsOver) ||
	       (appState == gameIsStarting))
           {

 
	     // PLAYING GAME (or waiting for a restart)...
	     debugStartStopwatch();
 	     tempTime1 = TimGetTicks();

	     startOfFrame();

	     gt++;
	     debugDumpElapTime(1);

	     // Clear the back buffer -- procvar
	     GraphicsClrBackBuffer();

             // Update formation movement struct
             UpdateMotionSequences();
	     debugDumpElapTime(2);

             // Move all sprites, etc.
             SpriteEngHandleTick();
	     DebugUpdateVarTiming();

	     //DebugFindHighTime(TimGetTicks()-DebugStopwatch);
	     BlastHandleTick();
	     debugDumpElapTime(3); 

	     // Handle user input
	     HandleUserInput();
	     debugDumpElapTime(4);

	     GraphicsDrawAllSprites();   // procvar
	     debugDumpElapTime(5);

	     // Show the banner (if any), title bar, debug text.  Text display is
	     // SKIPPED if the frame is getting too long (because the conversion of
	     // the score to text is lengthy)
             skipTextDisplay=false;
	     timeTilText=TimGetTicks();

	     if ((appState == gameIsActive)   ||
                 (appState == gameIsStarting)) 

             {
	        skipTextDisplay=((timeTilText-tempTime1) >= MaxTicksBeforeText);
		debugPutHex("     timeTilText",timeTilText);
		debugPutHex("       tempTime1",tempTime1);
	     } 

	     if (!skipTextDisplay) {
	        TextMgrDisplayText();  
	     } else {
                debugPuts("   skipping TextMgrDisplayText()");
	     }

	     debugDumpElapTime(6);
 
             // Ok, now draw the screen
	     GraphicsRepaintNative();
	     debugDumpElapTime(7);

	     // Do collision detection
	     SpriteEngCheckForCollisions();
	     debugDumpElapTime(8);

	     // Game Mgr and Sound Mgr routines 
	     HandleGameProgress();
	     debugDumpElapTime(9); 

	     // Beep! beep!
	     SoundMgrPlaySounds();

	     // debug to find count discrepancies
	     //DoCountAudits();

	     tempTime6 = TimGetTicks();

	     // Set CPU level for any restrictions
	     UtilSetCPULevel(tempTime6-tempTime1);


             // ------- debug.. to time frames -------------
	     //  if (gameInfo.gameState==gamePlay)
	     // {
		 DebugTotalTime+=(tempTime6-tempTime1);

		 // Incr intra-frame count (per-tick)
		 b= (tempTime6-tempTime1 > MAXTIMEDINCRS) ? (MAXTIMEDINCRS -1) : (tempTime6-tempTime1);
		 debugPutInt("intra time",b);
		 DebugTimedFrames[b]++;
		 DebugNumFrames++;
		 //}
             // ------- debug.. to time frames -------------
	
             // Create even timed frames           
	     while (tempTime6-tempTime1 < MinTicksPerFrame) {
	       tempTime6 = TimGetTicks();
	     }


	   } else {

	     if (appState == gameIsPaused) 
             {

	        // PAUSED...
	        tempTime1 = TimGetTicks();

	        GraphicsClrBackBuffer();    // procvar
	        GraphicsDrawAllSprites();   // procvar
	        TextMgrDisplayText();
	        GraphicsRepaintNative();
	        HandleGameProgress();  // should be in PAUSED state

                // Reduce flicker
	        tempTime6 = TimGetTicks();           
	        while (tempTime6-tempTime1 < MinTicksPerFrame) {
	          tempTime6 = TimGetTicks();
	        }

	     } else {
	       ERROR_ASSERT("unkown game state");
	     }

	   } // gameState != paused
         break;


      case menuEvent:
	 return HdlMenuEvent(eventP->data.menu.itemID);

	 //case menuOpenEvent:
	 //debugPuts("GameScreen menuOpenEvent received");
	 //break;
	 

      case frmUpdateEvent: 
      case frmOpenEvent:
	 debugPuts("GameScreen frmOpenEvent event received");

	 // See splash screen for details...
	 WinSetDrawWindow(WinGetDisplayWindow());
	 GraphicsHdlNewForm(gameScreenForm); 

	 frmP = FrmGetActiveForm();	 
	 FrmDrawForm ( frmP );

	 handled = true; 
	 break;
 


      default:
	break;
	       
      } // if active window

   } // switch

   //debugFuncExit("FormEventHandlerGameScreen()");
   return handled;

}

//----------------------------------------------------------------------
// FormEventHandlerSplashScreen
//----------------------------------------------------------------------
static Boolean FormEventHandlerSplashScreen(EventPtr eventP)
{
   Boolean handled = false;
   //WinHandle win;

   RGBColorType t;
   Byte x,n;
   
   IndexedColorType ict;
   
   MemHandle hBmpImage;
   BitmapType *bitmap; 

   FormPtr frmP=0;
   char s[20]="";
   char s2[20]="";


   MenuBarType * myMenu = NULL;

   switch (eventP->eType) 
   {

      case menuEvent:
	 return HdlMenuEvent(eventP->data.menu.itemID);
         break;

	
      case frmUpdateEvent:
      case frmOpenEvent:

	 // frmOpenEvent = initial form open
	 // frmUpdateEvent = form was covered, do a redraw

	 debugFuncEnter("FormEventHandlerSplashScreen()");

	 // This line is needed to get around a bug in the 3.5 debug ROMs
	 // It should not be needed for release ROMs.  A call to WinSetDrawWindow(win)
	 // IS needed for the text writing below.
	 // See "Using UI Color Table functions" 1-25-00 on eScribe.
	 WinSetDrawWindow(WinGetDisplayWindow());
	 GraphicsHdlNewForm(splashScreenForm);

	 frmP = FrmGetActiveForm(); 
	 FrmDrawForm(frmP);

	 
	 if (ScreenDepth == colorFourBitDepth) {

	    // color
	    hBmpImage = DmGetResource(bitmapRsc,f200);
	    bitmap = MemHandleLock(hBmpImage);    
	    WinDrawBitmap(bitmap, 0, 0);  //x,y
	    MemHandleUnlock(hBmpImage);
	    DmReleaseResource(hBmpImage);
	    
	 } else {
	    // 1-bit
	    hBmpImage = DmGetResource(bitmapRsc,b103);
	    bitmap = MemHandleLock(hBmpImage);
	    WinDrawBitmap(bitmap, 0, 0);
	    MemHandleUnlock(hBmpImage);
	    DmReleaseResource(hBmpImage);
	 }

	 got_here;


	 // neon green font (for color)
	 if (ScreenDepth == colorFourBitDepth) {
	    t.r=0x00;
	    t.g=0xff;
	    t.b=0x00;
	    ict = WinRGBToIndex(&t);
	    WinSetTextColor(ict);

	    // background=black
	    t.r=0x00;
	    t.g=0x00;
	    t.b=0x00;
	    ict = WinRGBToIndex(&t);
	    WinSetBackColor(ict);

	 }
	 	 
	 FntSetFont(stdFont);
	 StrCopy(s,"Version 3.00");
	 n=StrLen(s);
	 x=80-FntCharsWidth(s,n)/2; 
	 WinDrawChars(s,n,x,58);

	 // silver/grey font (for color)
	 if (ScreenDepth == colorFourBitDepth) {
	    t.r=0xaa;
	    t.g=0xaa;
	    t.b=0xaa;
	    ict = WinRGBToIndex(&t);
	    WinSetTextColor(ict);
	 }
	 

	 FntSetFont(stdFont);
	 StrCopy(s,"www.ncblast.com");
	 n=StrLen(s);
	 x=80-FntCharsWidth(s,n)/2; 
	 WinDrawChars(s,n,x,68);

	 FntSetFont(stdFont);
	 StrCopy(s,"High Score: ");
	 StrIToA(s2,mainGameRec1.highScore);
	 StrCat(s,s2);
	 n=StrLen(s);
	 x=80-FntCharsWidth(s,n)/2; 
	 WinDrawChars(s,n,x,90);

	 StrCopy(s,"High Wave: ");
	 StrIToA(s2,mainGameRec1.highWave);
	 StrCat(s,s2);
	 n=StrLen(s);
	 x=80-FntCharsWidth(s,n)/2; 
	 WinDrawChars(s,n,x,102);
	 
	 // white font (for color)
	 if (ScreenDepth == colorFourBitDepth) {
	    t.r=0xff;
	    t.g=0xff;
	    t.b=0xff;
	    ict = WinRGBToIndex(&t);
	    WinSetTextColor(ict);
	 }
	 
	 FntSetFont(stdFont);
	 StrCopy(s,"Tap screen to start");
	 n=StrLen(s);
	 x=80-FntCharsWidth(s,n)/2; 
	 WinDrawChars(s,n,x,133);
	 

	 handled = true; 
	 break;

 
	 
      case penDownEvent:
	 if (eventP->screenY <= 160) {
	    
	    // Start game..
	    myMenu = MenuGetActiveMenu();
	    if (myMenu == NULL)
	    {
	       HdlUserSaysStart();
	       handled = true;
	    }
	 }
	 break;
	 
	
      default:
	break;

   }


   return handled;
   
}


//----------------------------------------------------------------------
// FormEventHandlerRegisScreen
//
// This used to be where we did some of the SecurityMgr code, but that's
// been ripped out.
//----------------------------------------------------------------------
Boolean FormEventHandlerRegisScreen(EventPtr eventP)
{
   Boolean handled = false;
   FormPtr frmP;

   switch (eventP->eType) 
   {

      case frmOpenEvent:
	 debugFuncEnter("FormEventHandlerRegisScreen()");

	 WinSetDrawWindow(WinGetDisplayWindow());
	 GraphicsHdlNewForm(optionsScreenForm);

	 frmP = FrmGetActiveForm();
	 FrmDrawForm ( frmP );

	 handled = true; 
	 break;

      case ctlSelectEvent:
	 // If the done button is pressed, move to the main form.
	 if ((eventP->data.ctlEnter.controlID == genericOkButtonId) || 
	     (eventP->data.ctlEnter.controlID == genericCancelButtonId))
	 {
	    // Remove the edit form and display the main form.
	    debugFuncExit("FormEventHandlerRegisScreen()");
	    FrmGotoForm(lastForm);
	    handled = true;
	 }
	 break;

      default:
	break;
   }

   //debugFuncExit("FormEventHandlerRegisScreen()");
   return handled;

} 


//----------------------------------------------------------------------
// FormEventHandlerDebugScreen
//----------------------------------------------------------------------
Boolean FormEventHandlerDebugScreen(EventPtr eventP)
{
   Boolean handled = false;
   FormPtr frmP;
   UInt16 item;

   switch (eventP->eType) 
   {

     
      case popSelectEvent:
          item=eventP->data.popSelect.selection;
          HdlDebugMenuEvent(item);
          //debugPutInt("  item",item);
          handled=true;
          break;

      case menuEvent:
	 debugFuncExit("FormEventHandlerDebugScreen()");
	 return HdlMenuEvent(eventP->data.menu.itemID);
	 break;

      case frmOpenEvent:
	 frmP = FrmGetActiveForm();
	 FrmDrawForm ( frmP );

	 DebugDisplayTimingText();
	 
	 handled = true; 
	 break;

      case ctlSelectEvent:
	 // If the done button is pressed, leave form.
	 if (eventP->data.ctlEnter.controlID == genericOkButtonId )
	 {
	    TextMgrClr(); // remove debug text
	    FrmGotoForm(lastForm);
	    handled = true;
	 }
	 break;

      default:
	break;
   }

   debugFuncExit("FormEventHandlerDebugScreen()");
   return handled;

}

//----------------------------------------------------------------------
// FormEventHandlerInstructionsScreen
//----------------------------------------------------------------------
Boolean FormEventHandlerInstructionsScreen(EventPtr eventP)
{
   Boolean handled = false;
   FormPtr frmP;

   switch (eventP->eType) 
   {

      case frmOpenEvent:

	 debugFuncEnter("FormEventHandlerInstructionsScreen()");

	 WinSetDrawWindow(WinGetDisplayWindow());
	 GraphicsHdlNewForm(instructionsScreenForm);

	 frmP = FrmGetActiveForm();
	 FrmDrawForm ( frmP );

	 handled = true; 
	 break;

      case ctlSelectEvent:
	 // If the done button is pressed, leave form.
	 if (eventP->data.ctlEnter.controlID == genericOkButtonId )
	 {
	    FrmGotoForm(lastForm);
	    handled = true;
	 }
	 break;

      default:
	break;
   }

   //debugFuncExit("FormEventHandlerInstructionsScreen()");
   return handled;

}


//----------------------------------------------------------------------
// FormEventHandlerSettingsScreen
//----------------------------------------------------------------------
Boolean FormEventHandlerSettingsScreen(EventPtr eventP)
{
   Boolean handled = false;
   FormPtr frmP;

   UInt32 itemNum=0;


   switch (eventP->eType) 
   {

      //case frmUpdateEvent: 
      case frmOpenEvent:
	 debugFuncEnter("FormEventHandlerSettingsScreen()");

	 // FIRST: Call WinSetDrawWindow to avoid a fatal error in the debug ROMs.  Must
	 // call GraphicsHdlNewForm before drawing the form since resolution and text 
	 // colors are set there.
	 WinSetDrawWindow(WinGetDisplayWindow());
	 GraphicsHdlNewForm(optionsScreenForm);

	 frmP = FrmGetActiveForm();
	 FrmDrawForm (frmP);


	 // init form...
         FrmSetControlGroupSelection(frmP,sideGroup,0+mainGameRec1.sidePref);
         FrmSetControlGroupSelection(frmP,volGroup,volOffpBut+mainGameRec1.volPref);
         FrmSetControlGroupSelection(frmP,bulletSizeGroup,fatpBut+mainGameRec1.bulletSizePref);

	 itemNum = FrmGetObjectIndex(frmP,bulletRepeatBut);
	 FrmSetControlValue(frmP,itemNum,mainGameRec3.bulletRepeatPref);


	 handled = true; 
	 break;

      case ctlSelectEvent:
	 // If the done button is pressed, save settings & return to the splash screen form.
	 if (eventP->data.ctlEnter.controlID == genericOkButtonId )
	 {
	     frmP = FrmGetActiveForm();
	     
	     // groupNum->itemNum->objectId
	     itemNum = FrmGetControlGroupSelection(frmP,sideGroup); 
	     mainGameRec1.sidePref = FrmGetObjectId(frmP,itemNum)-0;
			
	     itemNum = FrmGetControlGroupSelection(frmP,volGroup); 
             mainGameRec1.volPref = FrmGetObjectId(frmP,itemNum)-volOffpBut;

	     itemNum = FrmGetControlGroupSelection(frmP,bulletSizeGroup); 
	     mainGameRec1.bulletSizePref = FrmGetObjectId(frmP,itemNum)-fatpBut;

	     itemNum = FrmGetObjectIndex(frmP,bulletRepeatBut);
	     mainGameRec3.bulletRepeatPref = FrmGetControlValue(frmP,itemNum);
    
	     //debugPutInt("mainGameRec1.sidePref",mainGameRec1.sidePref);
	     //debugPutInt("mainGameRec1.volPref",mainGameRec1.volPref);
	     //debugPutInt("mainGameRec1.bulletSizePref",mainGameRec1.bulletSizePref);

             // save, act on pref in Db
	     allRecordMustBeRewritten = true; 
             SetKeyEtcPreferences(&mainGameRec1,&mainGameRec3);
	     SoundMgrSetSoundPreferences(&mainGameRec1);

	     FrmGotoForm(lastForm);
	     handled = true;
	 }

	 if (eventP->data.ctlEnter.controlID == genericCancelButtonId )
	 {
	     // Remove the edit form and display the main form.
	     FrmGotoForm(lastForm);
	     handled = true;
	 }
	 break;
	 	 

      default:
	break;
   }

   //debugFuncExit("FormEventHandlerSettingsScreen()");
   return handled;

}

//----------------------------------------------------------------------
// FormEventHandlerGreyscaleOpsScreen
//----------------------------------------------------------------------
Boolean FormEventHandlerGreyscaleOpsScreen(EventPtr eventP)
{
   Boolean handled = false;
   FormPtr frmP;

   UInt32 itemNum=0;
   //UInt32 bgShade32=0,ttlBar32=0;
	
   ListPtr    listPtr;
   ControlPtr ctlPtr;
   char *     charPtr1=NULL;
   char *     charPtr2=NULL;
   char *     charPtr3=NULL;
   

   switch (eventP->eType) 
   {

      //case frmUpdateEvent: 
      case frmOpenEvent:
	 debugFuncEnter("FormEventHandlerGreyscaleOpsScreen()");

	 // FIRST: Call WinSetDrawWindow to avoid a fatal error in the debug ROMs.  Must
	 // call GraphicsHdlNewForm before drawing the form since resolution and text 
	 // colors are set there.
	 WinSetDrawWindow(WinGetDisplayWindow());
	 GraphicsHdlNewForm(greyscaleOpsScreenForm);

	 frmP = FrmGetActiveForm();
	 FrmDrawForm (frmP);

	 // Greyscale theme
	 debugPutInt("mainGameRec4.greyscaleTheme",mainGameRec4.greyscaleTheme);
	 ctlPtr   = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,greyshadeThemeTrigger));
	 listPtr  = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,greyshadeThemeList));
	 charPtr1 = LstGetSelectionText(listPtr,mainGameRec4.greyscaleTheme);
	 CtlSetLabel(ctlPtr,charPtr1);

	 // Background shade
	 ctlPtr   = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,backgroundGreyShadeTrigger));
	 listPtr  = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,backgroundGreyShadeList));
	 charPtr2 = LstGetSelectionText(listPtr,mainGameRec4.backgroundGreyShade);
	 CtlSetLabel(ctlPtr,charPtr2);

	 // Starfield
	 ctlPtr   = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,backgroundStarfieldTrigger));
	 listPtr  = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,backgroundStarfieldList));
	 charPtr3 = LstGetSelectionText(listPtr,mainGameRec4.backgroundStarfield);
	 CtlSetLabel(ctlPtr,charPtr3);
	 

	 handled = true; 
	 break;

      case ctlSelectEvent:
	 // If the done button is pressed, save settings & return to the splash screen form.
	 if (eventP->data.ctlEnter.controlID == genericOkButtonId )
	 {
	     frmP = FrmGetActiveForm();
	     FrmGotoForm(lastForm);
	     handled = true;
	 }

	 if (eventP->data.ctlEnter.controlID == genericCancelButtonId )
	 {
	     // Remove the edit form and display the main form.
	     FrmGotoForm(lastForm);
	     handled = true;
	 }
	 break;
	 

      case popSelectEvent:

	 // Must return false from popup to have field selected

	 // GREYSHADE THEME
	 if (eventP->data.popSelect.controlID == greyshadeThemeTrigger)
	 {
	    itemNum=eventP->data.popSelect.selection;
	    mainGameRec4.greyscaleTheme = itemNum;
	    allRecordMustBeRewritten = true;

	    debugPutInt("  greyshade itemNum",itemNum);

	    // Rerun greyscale initialization
	    GraphicsFreeMemory();
	    SetGreyShades();         // procvar
	    RegisterGameGraphics();  // procvar

	    break;
	 }

	 // BACKGROUND GREYSHADE
	 if (eventP->data.popSelect.controlID == backgroundGreyShadeTrigger)
	 {

	    itemNum=eventP->data.popSelect.selection;
	    mainGameRec4.backgroundGreyShade = itemNum;
	    allRecordMustBeRewritten = true;

	    debugPutInt("  background itemNum",itemNum);

	    // Rerun the greyscale initialization since changing the background
	    // could: (1) make the starfield visible again when they have been 
	    // turned off, (2) require the text color to be inverted to white
	    // Kind of overkill and slow though.
	    GraphicsFreeMemory();
	    SetGreyShades();         // procvar
	    RegisterGameGraphics();  // procvar

	    break;
	 }

	 // STARFIELD
	 if (eventP->data.popSelect.controlID == backgroundStarfieldTrigger)
	 {
	    itemNum=eventP->data.popSelect.selection;
	    mainGameRec4.backgroundStarfield = itemNum;
	    allRecordMustBeRewritten = true;

	    debugPutInt("  starfield itemNum",itemNum);

	    // Rerun greyscale initialization to remap the star color char to 
	    // be the same as background.
	    GraphicsFreeMemory();
	    SetGreyShades();          // procvar
	    RegisterGameGraphics();   // procvar

	    break;
	 }
	 break;
	 
	 

      default:
	break;
   }

   //debugFuncExit("FormEventHandlerSettingsScreen()");
   return handled;

}

//----------------------------------------------------------------------
// FormEventHandlerGreyscaleDetCPUScreen
//----------------------------------------------------------------------
Boolean FormEventHandlerGreyscaleDetCPUScreen(EventPtr eventP)
{
   Boolean handled = false;
   FormPtr frmP;

   UInt32 itemNum=0;
	
   ListPtr    listPtr;
   ControlPtr ctlPtr;
   char *     charPtr1=NULL;
  

   switch (eventP->eType) 
   {

      //case frmUpdateEvent: 
      case frmOpenEvent:
	 debugFuncEnter("FormEventHandlerGreyscaleDetCPUScreen()");

	 // FIRST: Call WinSetDrawWindow to avoid a fatal error in the debug ROMs.  Must
	 // call GraphicsHdlNewForm before drawing the form since resolution and text 
	 // colors are set there.
	 WinSetDrawWindow(WinGetDisplayWindow());
	 GraphicsHdlNewForm(setPdaCPUForm);

	 frmP = FrmGetActiveForm();
	 FrmDrawForm (frmP);

	 // Graphics detail level
	 debugPutInt("mainGameRec4.graphicsLevel",mainGameRec4.graphicsLevel);
	 ctlPtr   = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,graphicLevelTrigger));
	 listPtr  = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,graphicLevelList));
	 charPtr1 = LstGetSelectionText(listPtr,mainGameRec4.graphicsLevel);
	 CtlSetLabel(ctlPtr,charPtr1);

	 // Store original graphics level to determine if it gets changed by user
	 // or algorithm
	 origGraphicsLevel = mainGameRec4.graphicsLevel;
	 
	 handled = true; 
	 break;

      case ctlSelectEvent:
	 // If the done button is pressed, save settings & return to previous form.
	 if (eventP->data.ctlEnter.controlID == genericOkButtonId )
	 {

	    // If level was changed, instruct the user
	    if (origGraphicsLevel != mainGameRec4.graphicsLevel) {
	       FrmCustomAlert(nealInfoAlertId,"Switch to another application, and then back to Astron to enable the new graphics setting."," "," ");
	    }
	    
	    frmP = FrmGetActiveForm();
	    FrmGotoForm(lastForm);
	    handled = true;
	 }

	 if (eventP->data.ctlEnter.controlID == rerunCpuDetTestButtonId )
	 {

	    // Re-run the CPU test, and set the greyscale level based on CPU speed.
	    mainGameRec4.cpuSpeed = UtilDetermineCPUSpeed(true); // debug, remove

	    mainGameRec4.cpuSpeed = UtilDetermineCPUSpeed(false);
	    
	    // 4-bit (highestGreyscaleLvl) greyscale needs at least a 20 MHz CPU
	    mainGameRec4.graphicsLevel = 
	       (mainGameRec4.cpuSpeed >= cpuIs20MHz) ? highestGreyscaleLvl : normalGreyscaleLvl;

	    FrmCustomAlert(nealInfoAlertId,"Speed test completed. Detail level set to the correct value for this CPU."," "," ");

	    allRecordMustBeRewritten = true;

	    // Reset onscreen text
	    debugPutInt("mainGameRec4.graphicsLevel",mainGameRec4.graphicsLevel);
	    frmP = FrmGetActiveForm();
	    ctlPtr   = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,graphicLevelTrigger));
	    listPtr  = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,graphicLevelList));
	    charPtr1 = LstGetSelectionText(listPtr,mainGameRec4.graphicsLevel);
	    CtlSetLabel(ctlPtr,charPtr1);

	    handled = true;
	 }
	 break;
	 

      case popSelectEvent:
	 // Must return false from popup to have field selected

	 if (eventP->data.popSelect.controlID == graphicLevelTrigger)
	 {
	    itemNum=eventP->data.popSelect.selection;

	    mainGameRec4.graphicsLevel = itemNum;
	    allRecordMustBeRewritten = true;

	    debugPutInt("  greyshade itemNum",itemNum);
	    break;
	 }

	 break;
	 
	 

      default:
	break;
   }

   //debugFuncExit("FormEventHandlerGreyscaleDetCPUScreen()");
   return handled;

}


/***********************************************************************
 *
 * FUNCTION:    AppHandleEvent
 *
 * DESCRIPTION: This routine loads form resources and set the event
 *              handler for the form loaded.
 *
 *
 ***********************************************************************/
static Boolean AppHandleEvent( EventPtr eventP)
{
   Word formId;
   FormPtr frmP;
   
   
   if (eventP->eType == frmLoadEvent)
   {
      // Load the form resource.
      formId = eventP->data.frmLoad.formID;
      frmP = FrmInitForm(formId);
      FrmSetActiveForm(frmP); 
      
      // Set the event handler for the form.  The handler of the currently
      // active form is called by FrmHandleEvent each time is receives an
      // event.
      switch (formId)
      {
	 
	 case greyscaleOpsScreenForm:
	    FrmSetEventHandler(frmP, FormEventHandlerGreyscaleOpsScreen);
	    break;

	 case setPdaCPUForm:
	    FrmSetEventHandler(frmP, FormEventHandlerGreyscaleDetCPUScreen);
	    break;

	 case optionsScreenForm:
	    FrmSetEventHandler(frmP, FormEventHandlerSettingsScreen);
	    break;
	    
	 case splashScreenForm:
	    FrmSetEventHandler(frmP, FormEventHandlerSplashScreen);
	    break;
	    
	 case regScreenForm:
	    FrmSetEventHandler(frmP, FormEventHandlerRegisScreen);
	    break;
	    
	 case gameScreenForm:
	    FrmSetEventHandler(frmP, FormEventHandlerGameScreen);
	    break;
	    	    
	 case debugScreenForm:
	    FrmSetEventHandler(frmP, FormEventHandlerDebugScreen);
	    break;
	    
	 // Generic single forms (1 OK button only)... use same handler
	 case greyscaleInfoScreenForm:
	 case instructionsScreenForm:
	    FrmSetEventHandler(frmP, FormEventHandlerInstructionsScreen);
	    break;
	    
	 default:
	    //				ErrFatalDisplay("Invalid Form Load Event");
	    break;
	    
      }
      return true;
   }
   
   return false;
 
}


/***********************************************************************
 *
 * FUNCTION:    AppEventLoop
 *
 * DESCRIPTION: This routine is the event loop for the application.  
 *
 *
 ***********************************************************************/
static void AppEventLoop(void)
{
   Word error;
   EventType event;

   do {
      EvtGetEvent(&event, 0); // 0= return immediately
              
      //if (event.eType != nilEvent)    // neal added feb 14,2001
	 if (! SysHandleEvent(&event)) 
	    if (! MenuHandleEvent(0, &event, &error))
	       if (! AppHandleEvent(&event)) {} // my proc for handling new form loads
		  
      
      FrmDispatchEvent(&event);     // my proc specific to the form handler (game, splash, etc)

   } while (event.eType != appStopEvent);

   //debugFuncExit("AppEventLoop()");
}


//------------------------------------------------------------------------
//  HdlUserSaysStart
//------------------------------------------------------------------------
void HdlUserSaysStart()
{

  debugFuncEnter("HdlUserSaysStart()");

  // >>>>This function is called 2 times:  from splash screen, and when a game is
  // finished and the user wishes to restart

  debugPuts("  immediate gameplay allowed");
  numTimesRun++;
  allRecordMustBeRewritten=true; // save numTimesRun
  UtilConfigHardKeys();
  appState = gameIsStarting;
  debugPuts("  going to gameScreenForm");
  FrmGotoForm(gameScreenForm); 
  
  debugFuncExit("HdlUserSaysStart()");

}

//------------------------------------------------------------------------
// UtilConfigHardKeys
//
// Set key vars, remember old settings
//------------------------------------------------------------------------
void UtilConfigHardKeys()
{
    UInt16 err,zero=0;

    debugFuncEnter("UtilConfigHardKeys()");

    // Don't config more than once
    if (hardKeysChanged) {
      debugPuts("   keys already configured");
      debugFuncExit("UtilConfigHardKeys()");
      return;
    }

    // Set key vars, remember old settings
    oldKeyMask = KeySetMask( ~(keyBitPageUp|keyBitPageDown|keyBitHard1|
                             keyBitHard2|keyBitHard3|keyBitHard4) );

    // Read
    err = KeyRates(false,&keyInitDelay,&keyPeriodP,&keyDoubleTapDelay,&keyQueueAhead);

    if (err) { debugPutIntError("Key rates read error, err",err); }

    // debug
    debugPutHex("   oldKeyMask",oldKeyMask);
    debugPutInt("   keyInitDelay",keyInitDelay);
    debugPutInt("   keyPeriodP",keyPeriodP);
    debugPutInt("   keyDoubleTapDelay",keyDoubleTapDelay);
    debugPutInt("   keyQueueAhead",(Int)keyQueueAhead);

    // Set
    err = KeyRates(true,&zero,        &zero,      &keyDoubleTapDelay,&keyQueueAhead);

    if (err) { 
       debugPutIntError("Key rates set error, err",err); 
    } 

    // BUG FIX (v.90)
    // This flag is needed because the keys are only set if the game is paused, or
    // if it were started.  If the splash screen is visited but not started then the
    // key rate and mask are never set (so don't set them back to junk)!
    // Set this to true even though above may fail since KeySetMask is already done
    hardKeysChanged=true;

   debugFuncExit("UtilConfigHardKeys()");
}


//------------------------------------------------------------------------
// Version 0.80 of Astron had a bug which sometimes caused keyInitDelay
// and keyPeriodP to be mis-set on game exit.  Clean that up here..
//------------------------------------------------------------------------
void UtilFixV1HardKeyBug()
{

    UInt16   err;
    UInt16   kInitDelay2;
    UInt16   kPeriodP2;
    UInt16   kDoubleTapDelay2;
    Boolean  kQueueAhead2;

    debugFuncEnter("UtilFixV1HardKeyBug()");

    // Read
    err = KeyRates(false,&kInitDelay2,&kPeriodP2,&kDoubleTapDelay2,&kQueueAhead2);

    if (err) { 
      debugPutIntError("Key rates read error, err",err); 
      debugFuncExit("fixV1HardKeyBug()");
      return;
    }

    // debug
    debugPutInt("   read kInitDelay2",kInitDelay2);
    debugPutInt("   read kPeriodP2",kPeriodP2);
    debugPutInt("   read kDoubleTapDelay2",kDoubleTapDelay2);
    debugPutInt("   read kQueueAhead2",(Int)kQueueAhead2);

    // OSs 3.0-3.5.1 are set to these values
    kInitDelay2      = 50;
    kPeriodP2        = 12;
    kDoubleTapDelay2 = 200;
    kQueueAhead2     = false;

    // Set
    err = KeyRates(true,&kInitDelay2,&kPeriodP2,&kDoubleTapDelay2,&kQueueAhead2);

    if (err) { 
       debugPutIntError("Key rates set error, err",err); 
    } 


   debugFuncExit("fixV1HardKeyBug()");
}


//------------------------------------------------------------------------
// AppInit
//------------------------------------------------------------------------
static Err AppInit(void)
{
   Word n;

   debugFuncEnter("AppInit()");

   // Account for changes in the Palm CPU speeds (num ticks
   n=SysTicksPerSecond();
   MinTicksPerFrame=n*MILLISECPERFRAME/1000;
   MaxTicksBeforeText=n*MILLISECALLOWTEXT/1000;
   debugPutInt("  SysTicksPerSecond",n);
   debugPutInt("  MinTicksPerFrame",MinTicksPerFrame);
   debugPutInt("  MaxTicksBeforeText",MaxTicksBeforeText);

     
   DebugClearTimingVars();

   // Call the init routines which must execute before database records are read
   SpriteEngInit();                               // Fire up the sprite engine..
   CreateGameSprites();                           // Create game sprites
   CreateUserSprite();                            // Create user sprite   ... needs ScreenDepth!!

   // Load all database records to Try to restore app/game.  THIS MUST GO AFTER 
   // SPRITEENGINEINIT() BECAUSE WE NOW WILL OVERWRITE VARS NEW'D IN THAT PROC. 
   AppRestoreState();
   debugPutInt("   new appState",appState);

   // Define ScreenDepth.  **ALL PROCS BEFORE NOW CANNOT REFERENCE THE VAR UNTIL THIS POINT**
   // Must go after AppRestoreState since the CPU type stored in the mainGameRec4 influences
   // the screen depth
   AppSetScreenDepthVar();

   // Finish user initiation now that ScreenDepth is set
   CreateUserSpritePt2();

   // Call the init routines which use info read from the database records
   GraphicsInit(ScreenDepth,160,160,false,true);  // Fire up the graphics   
   RegisterGameGraphics();                        // Register all bitmaps  (procvar)
   TextMgrInit(ScreenDepth);                      // Fire up Text mgr
   BackDropMgrInit();                             // Star field 
 

   // App state set (in AppRestoreState), go to the correct form..
   switch(appState) {

      case appNewlyInstalled:
	debugPuts("   1st install of game");

	// Set mainGameRec1
	numTimesRun=0;
	installTimGetSeconds              = TimGetSeconds();
        mainGameRec1.numTimesRun          = numTimesRun; // =0 (above)
        mainGameRec1.installTimGetSeconds = installTimGetSeconds; // set the install time
	mainGameRec1.tenDayAdditionOn     = false;

	// Set mainGameRec2.  This is needed because we may be installing a new version
	// of Astron, and the previous version may have expired
	timeModifiedFraudFlag = false;
	numTimesRestarted = 0;
	mainGameRec2.timeModifiedFraudFlag = timeModifiedFraudFlag;
	mainGameRec2.numTimesRestarted     = numTimesRestarted;

	allRecordMustBeRewritten = true;

	// ok, reset appState
	appState=splashScreen;


	// If this is the first install of a greyscale game give the 1-time info screen
	// else just roll to splash screen
	if ((ScreenDepth == greyTwoBitDepth) || (ScreenDepth == greyFourBitDepth))
	{
	   startOnThisForm = greyscaleInfoScreenForm;
	   lastForm = splashScreenForm;
	   
	   mainGameRec4.greyscaleInstructGiven = true;
	   allRecordMustBeRewritten = true;
   
	} else {
	   startOnThisForm = splashScreenForm;
	}


        break;

      case splashScreen: 
        startOnThisForm = splashScreenForm;
        break;

      case nagScreen:    
	// This used to be part of the registration code
        startOnThisForm = splashScreenForm;
        break;

      case gameIsPaused:
      case gameIsActive: 
        UtilConfigHardKeys();
        startOnThisForm = gameScreenForm;
        break;

      default:
        ERROR_ASSERT("weird saved state");
        debugPutIntError("weird saved state",appState);
	appState=splashScreen;
        startOnThisForm = splashScreenForm;
   }


   debugFuncExit("AppInit()");
   return 0;

}



//----------------------------------------------------------------------
// AppRestoreState
//
// Calls database routines and other procs to restore all of the game
// records which were saved from the previous game.
//
// Sets any non-recovered records to default values.  Returns TRUE if 
// a game has successfully been restored.  WILL SET APP
// STATE REGARDLESS OF ERROR/SUCCESS
//----------------------------------------------------------------------
Boolean AppRestoreState()
{
   Boolean rc;
   debugFuncEnter("AppRestoreState");

   // Default mainGameRec1 
   mainGameRec1            = mainGameRec1Default;
   appState                = mainGameRec1.appState;

   // Default mainGameRec2
   lastTimRan              = TimGetSeconds();
   timeModifiedFraudFlag   = false;

   // Default mainGameRec3, mainGameRec4
   mainGameRec3            = mainGameRec3Default;  
   mainGameRec4            = mainGameRec4Default;
   

   // Open/create database, then 
   // READ MAIN RECORDS
   if (DbMgrOpenOrCreateGameDb()) {

      // appState is overwritten here (if record is present)
      DbMgrReadMainAppRecords();

      // Perform CPU test if it has never been done (CPU intensive
      // so only done once).
      if (mainGameRec4.cpuSpeed == cpuUnknown) {

	 mainGameRec4.cpuSpeed = UtilDetermineCPUSpeed(false);

	 // Set the greyscale level based on CPU speed.  4-bit greyscale needs at least
	 // a 20 MHz CPU
	 mainGameRec4.graphicsLevel = 
	    (mainGameRec4.cpuSpeed >= cpuIs20MHz) ? highestGreyscaleLvl : normalGreyscaleLvl;
	 
      }
      
      // Process info in main records (or use default)
      SetKeyEtcPreferences(&mainGameRec1,&mainGameRec3);
      SoundMgrSetSoundPreferences(&mainGameRec1);

   } else {
      // This error should remain in released game (not debug)
      ErrDisplay("ERROR: Not enough free memory to create required databases");
      appState=splashScreen;
      debugFuncExit("AppRestoreState()");
      return false;
   }


   // Handle upgrade from an earlier Astron database version 
   if (dbVer != DATABASEver) {
     debugPutInt("  Upgrading from earlier Astron version, v",dbVer);

     switch (dbVer)
     {
       case DATABASEv1:
	  UtilFixV1HardKeyBug();
	  break;

       default:
          debugPuts("Unknown/don't care about database version");
     }
   }
   

   debugPutInt("   CURRENT astronVersion",mainGameRec3.astronVersion);

   // Handle upgrade from an earlier Astron database version OR the same database
   // version, BUT a new game version (Ex: 1.00 and 1.05 have same database version)
   if ((dbVer != DATABASEver) || (mainGameRec3.astronVersion < ASTRONVERSION)){

      // Ensure the game version is written when we leave 
      allRecordMustBeRewritten = true;

      // Don't read the sprite records, act like Astron has just been installed,
      // this will modify the install time, and allow more free play, but that is
      // fine.  This is important because sprite structs may have changed between
      // versions of the game, and reloading old records may create errors.  AppInit
      // will handle the rest
      appState=appNewlyInstalled;
      
      gt=0;
      
      debugFuncExit("AppRestoreState()");
      return true;
   }

 

   // READ GAME RECORDS (if they are present)
   if ((appState == gameIsActive) ||
       (appState == gameIsPaused))
   {

      rc = false;

      if (RestoreGamemanRecs()) {
          rc = RestoreBlastRecs();
      }

      if (!rc) {
	// Records were present, but unable to read.  Big problem because some
	// vars may be half set.  
	allRecordMustBeRewritten = true;
	appState=splashScreen;

	// Important to zero out gt because mainGameRec1 will be erroneous, and game
	// will hang in zombie state
	gt=0;

	// This should be converted to a ErrDisplay in the released copy 
	ERROR_ASSERT("ERROR,appState == gameIsSaved , but records were unrecoverable");
	//debugPutsError("ERROR,appState == gameIsSaved , but records were unrecoverable");
	debugFuncExit("AppRestoreState()");
	return false;

      } 

      // SUCCESS CASE... final, internal setup
      //debugPuts("   game restored from records");
      appState = gameIsPaused;
      loadNewRoundPercs();


   } else {
      //debugPutInt("   appState says no records present, appState",appState);

      // Cleanup gt
      gt=0;
   }


   debugFuncExit("AppRestoreState()");
   return true;


}


//----------------------------------------------------------------------
// AppSetScreenDepthVar
//
// Determine if the device will support 4-bit color, set the ScreenDepth
// variable.
//----------------------------------------------------------------------
void AppSetScreenDepthVar()
{

   Err      err;
   UInt32   depth=0;
   Boolean  supportsColor=false;

   debugFuncEnter("AppSetScreenDepthVar()");
   

   // Default for now
   ScreenDepth=oneBitDepth;

   // OSs prior to 3.5 do not support color, get the ROM version 
   FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);


   // Check if device is 4-bit COLOR enabled (PalmOS >= 3.5)
   if (romVersion >= PALMOS35)
   {

      // First: Determine if this device supports 4-bit depth (Reference pg. 884)
      err=WinScreenMode(winScreenModeGetSupportedDepths,
			NULL,   // width
			NULL,   // height
			&depth, 
			NULL);   // got color?

      debugPutUInt32Hex(">=PalmOS 3.0,depth",depth);
      
      // Second: Determine if this device supports color (Reference pg. 884)
      err=WinScreenMode(winScreenModeGetSupportsColor,
			NULL,   // width
			NULL,   // height
			NULL,
			&supportsColor);   // got color?
    

      debugPutInt("supportsColor",(Byte)supportsColor);

      // Set depth var if 4-bit color is supported (8 = 2^3)
      if ((depth & 0x08) && (supportsColor)) 
      {
	 // We are making an implicit assumption that the CPU can handle 4-bit color
	 // Astron (is at least 20MHz)
	 ScreenDepth=colorFourBitDepth;
      }

   }

   // If not 4-bit color, check for 2 or 4-bit GREYSCALE. Astron requires at
   // least PalmOS 3.0 to run (already checked for).  Let's see if
   // greyscale is around
   if (ScreenDepth == oneBitDepth)
   {
      // WinScreenMode is a PalmOS 3.5 call, 
      err=ScrDisplayMode(winScreenModeGetSupportedDepths,
			NULL,   // width
			NULL,   // height
			&depth,
			NULL);   // got color?

      debugPutUInt32Hex(">=PalmOS 3.0, depth",depth); 

      // Set depth var.  2-bit = (2^(2-1)), 4-bit = (2^(4-1))
      ScreenDepth = (depth & 0x02) ? greyTwoBitDepth  : ScreenDepth;
      ScreenDepth = (depth & 0x08) ? greyFourBitDepth : ScreenDepth;

      // 4-bit greyscale requires at least a 20MHz processor.  Almost all
      // 4-bit PDAs have a CPU of this size except old Palm and 1 Handspring model
      if (ScreenDepth == greyFourBitDepth) {
	 if (mainGameRec4.graphicsLevel == normalGreyscaleLvl)
	 {
	    // PDA is 4-bit greyscale capable, but CPU is too slow, 
	    // do 2-bit greyscale or monochrome
	    ScreenDepth = (depth & 0x02) ? greyTwoBitDepth  : oneBitDepth;
	    debugPuts("4-bit grey supported, but CPU not >= 20MHz");
	 }
      }

   }

 
   debugPutInt("Final ScreenDepth",ScreenDepth);
   debugFuncExit("AppSetScreenDepthVar()");
   
}


//----------------------------------------------------------------------
// UtilRestoreHardKeys
//----------------------------------------------------------------------
void UtilRestoreHardKeys()
{

   UInt16 err;

    debugFuncEnter("UtilRestoreHardKeys");

    if (!hardKeysChanged) {
       debugPuts("   hardKeysChanged==false, key mask and rate NOT changed");
       debugFuncExit("UtilRestoreHardKeys()");
       return;
    }

    // Unmask the hardbuttons, reset key rates, etc.
    oldKeyMask = KeySetMask( oldKeyMask );
    err = KeyRates(true,&keyInitDelay,&keyPeriodP,&keyDoubleTapDelay,&keyQueueAhead);
   
    hardKeysChanged=false;

    // Read
    err = KeyRates(false,&keyInitDelay,&keyPeriodP,&keyDoubleTapDelay,&keyQueueAhead);

    // debug...
    if (err) { debugPutIntError("Key rates read error, err",err); }
    debugPuts(" Exiting key values:");
    debugPutHex("   oldKeyMask",oldKeyMask);
    debugPutInt("   keyInitDelay",keyInitDelay);
    debugPutInt("   keyPeriodP",keyPeriodP);
    debugPutInt("   keyDoubleTapDelay",keyDoubleTapDelay);
    debugPutInt("   keyQueueAhead",(Int)keyQueueAhead);
   
    debugFuncExit("UtilRestoreHardKeys()");
}



/***********************************************************************
 *
 * FUNCTION:    AppStop
 *
 * DESCRIPTION: Save the current state of the application.
 *
 *
 *
 ***********************************************************************/
static void AppStop(void)
{
   Boolean rc=false;

   debugFuncEnterInt("AppStop(), appState",appState);

   // Restore hard buttons (if needed)
   UtilRestoreHardKeys();

   // DELETE records before upcoming writes...
   if ((allRecordMustBeRewritten) ||
       (appState == nagScreen)    ||
       (appState == gameIsActive) ||
       (appState == gameIsOver))
   {
     if ( !DbMgrDeleteAllRecords() ) {
        ErrDisplay("FATAL ERROR: Unable to delete database records");
        debugFuncExit("void()");
        return;  
     }
   }


  // WRITE GAME RECORDS
  if (appState == gameIsActive) 
  {
        //debugPuts("  Saving game records");

        rc = false;

	// Call all the module save routines...
        if (SaveGamemanRecs() ) {
          rc = SaveBlastRecs();
	}

	if (!rc) {
	  // Couldn't save all records, try to clear memory to at least save main record
	  mainGameRec1.appState = splashScreen;

          if ( !DbMgrDeleteAllRecords() ) {
             ErrDisplay("FATAL ERROR: not enough memory to create/save data");
             debugFuncExit("void()");
             return;  
          }
	}

   } else {
       // Make everything squeaky clean
       //debugPuts("  no game to save");
   }


   // WRITE MAIN RECORDS
   if ((allRecordMustBeRewritten) ||
       (appState == nagScreen)    ||
       (appState == gameIsActive) ||
       (appState == gameIsOver))
   {

     //debugPuts("  writing main app records");

     // If game is over, then next the next time the game should start on 
     // the splash screen
     appState = (appState == gameIsOver) ? splashScreen : appState;

     // write expects the recs to be created, so do a fake read first
     if ( DbMgrReadMainAppRecords() ) {
         DbMgrWriteMainAppRecords();   // user pref, etc
     } else {
         // very screwed here -- can't save anything
         ErrDisplay("FATAL ERROR -- not enough memory to create/save data");
     }

   } else {
     //debugPuts("  NOT writing main app records");
   }



   // Remaining cleanup...
   DbMgrCloseGameDb(); // all done with dB
   BlastAppStopDoCleanup();  // free memory
   GraphicsStopDoCleanup(); // reset depth, free memory


   debugPuts("****************  THE END  *****************");
   debugPuts("****************  THE END  *****************");


   debugFuncExit("AppStop()");

   FrmCloseAllForms();
   
}


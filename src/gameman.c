//
//  GAME Manager
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

#include <SysUtils.h>  // for random numbers

#include "behav.h" 
#include "wave.h"
#include "gameman.h"
#include "sprite.h"
#include "blast.h" 
#include "debug.h" 
#include "graphics.h"  
#include "starter.h"
#include "misc.h" // for AllowImmediateGamePlay()
#include "startRsc.h" // for formIDs
#include "database.h" 


//----------------------------------------------------------------------
//    SAVE-ABLES: gameInfo, roundInfo
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//                DEBUG
//----------------------------------------------------------------------

char debugStr[100] = "x"; 
char debugStr2[100] = "x"; 
char miscText[40]= "x";

char * ds1 = "HandleIntroWaves";
char * ds2 = "RegularPlay";
char * ds3 = "Before1stRound";   
char * ds4 = "betweenRounds";
char * ds5 = "restAfterGamePlay";
char * ds6 = "waitForFormation";


//----------------------------------------------------------------------
//                Internal APIs
//----------------------------------------------------------------------
void updateFormationMoveStruct();


//-----------------------------------------------------------------------
//    Structs for overall game and round  info
//-----------------------------------------------------------------------

// Also modify HdlGameState_Before1stRound if changing this...
gameInfoStruct gameInfo = {0,                  // score =0
                           3,                  // 3 initial ships 
                           1,                  // round =1  
                           regularRound,       // round type
                           before1stRound,     // game is just starting
                           before1stRound,     // dummied, we're not in PAUSED
                           user_normal,        // player is alive
                           false,               // double fire
			   false,              // game is in a save state
                           0,                  // actual times hit
                          };

 

roundInfoStruct roundInfo;  // init'd later


Int     ticksInRestState =-1;
Boolean skipIntroFlyIns = false;


//--------------------------------------------------------------------
//  startOfFrame
//
//  Called at start of every frame  
//-------------------------------------------------------------------- 
void startOfFrame()
{
   debugFuncEnter( "----------------------start_of_frame-----------------------" ); 
}


//-----------------------------------------------------------------------
// Based on the round number, load the intro wave in
//-----------------------------------------------------------------------
void loadNewRoundInfo()
{

   debugFuncEnterInt( "loadNewRoundInfo() round",gameInfo.round );

   // Load new round wave structures -- wave.c
   switch(gameInfo.round%21) 
   {
     case 1:  setupRegWaveA(); break;
     case 2:  setupRegWaveA(); break;
     case 3:  setupRegWaveB(); break;
     case 4:  setupRegWaveD(); break; 
     case 5:  setupRegWaveF(); break;
     case 6:  setupRegWaveB(); break;     
     case 7:  setupRegWaveC(); break; 
     case 8:  setupRegWaveC(); break; 
     case 9:  setupRegWaveF(); break;
     case 10: setupRegWaveD(); break;
     case 11: setupRegWaveB(); break;
     case 12: setupRegWaveE(); break;
     case 13: setupRegWaveD(); break;
     case 14: setupRegWaveC(); break;
     case 15: setupRegWaveG(); break;
     case 16: setupRegWaveE(); break;
     case 17: setupRegWaveD(); break;
     case 18: setupRegWaveC(); break;
     case 19: setupRegWaveC(); break;
     case 20: setupRegWaveG(); break;
 
     default:  setupRegWaveA();    
   }

   debugFuncExit("loadNewRoundInfo()");
}


//-----------------------------------------------------------------------
// loadNewRoundPercs~
//
// Based on the round number, set the per round percentages
//-----------------------------------------------------------------------
void loadNewRoundPercs()
{
    // see gameman.h, line 80
  roundParamStruct round1 =   { 2,     // num bugs in flight
                                2,5,5,  // bullets in intro,normal,blitz play
                                2,  // num mines in normal play
                                3,  // bugs for blitz mode start
                                0,  // bugs that can peel off
                                3,  // num trac beam attempts
                                3,   // bug bullet speed
                                95,  // bullet ceiling
                                70,  // mine ceiling
                                100,  // keep course ceiling   ..need to fix loopback sprites when chg this
                                30,   // aim at user variance
                                4,3,    // bug x,y speed in intro
                                2,3,3,  // slow,med,highXSpeed   4 is VERY FAST (for 1 bit)
                                2,3,3,  // slow,med,highYSpeed
                                8,       // min ticks between launching bugs during normal play
                                60,   // % chance mothership beam
                                50,50, // % chance bullet intro wave, normal play 
                                3,  // max ticks 'til next wave
				20, // ticks to wait after formation is formed
                                3,3    // x,y peel off speeds
                                };   


      debugFuncEnterInt( "loadNewRoundPercs(), round",gameInfo.round );

      // Adjust the game parameters based on the round
      rps = round1;

      // Default 2nd line
      TextMgrAddCenteredText("Get Ready..",1,stdFont,50,true);

      // 1
      if (gameInfo.round == 1) 
      {
        TextMgrAddCenteredText("Your mission: shoot everything",1,stdFont,50,true);
      }

      // 2..
      if (gameInfo.round >=2) 
      {
	rps.maxBugsInFlight=3;
      }

      // 3..
      if (gameInfo.round >=3)
      {
        rps.numBugsCanPeelOff =3;
        rps.bugXSpeedIntro =4;
        rps.bugYSpeedIntro =4;
      }

      // 5..
      if (gameInfo.round >=5)
      {
	rps.maxBugsInFlight =4;
	rps.maxBulletsInIntroWaves =3;
        rps.maxMinesInNormalPlay =4;
	rps.numBugsForBlitzMode =4;
        rps.aimAtPlayerVariance =20;
	rps.percChanceMothershipBeam = 40;
	rps.timeBefore1stBugDive =7;  // was 10
      }

      // 6..
      if (gameInfo.round >=6)
      {
	rps.peelOffXSpeed = 3;
	rps.peelOffYSpeed = 4;
	rps.numBugsCanPeelOff=4;
      }

      // 8..
      if (gameInfo.round >=8)
      {
	rps.percChanceMothershipBeam = 33;
        rps.bugXSpeedIntro =4;  // 5 is too fast
        rps.bugYSpeedIntro =4;
      }

      // 10..
      if (gameInfo.round >=10)
      {
        rps.aimAtPlayerVariance =10;
	rps.numBugsCanPeelOff=5;
	if (gameInfo.round == 10) {
           TextMgrAddCenteredText("Ooooo round 10!",1,stdFont,50,true);
	}
      }


      // 12
      if (gameInfo.round == 12) 
      {
        TextMgrAddCenteredText("Barriers require 5 shots to destroy",1,stdFont,50,true);
	rps.timeBefore1stBugDive =5;
      }

      // 15..
      if (gameInfo.round >=15)
      {
        // slow x&y speed of 3 is quite difficult...
        rps.slowXSpeed =3;
        rps.slowYSpeed =3;
        rps.aimAtPlayerVariance =2;
      }

      // 15-20 (and multiples therafter)
      if ((gameInfo.round > 14) && (gameInfo.round%15 <= 5))
      {
	 skipIntroFlyIns = true;
         TextMgrAddCenteredText("Warning: quick introduction",1,stdFont,50,true);
      }

      // 20..
      if (gameInfo.round >=20) 
      {
        rps.bugYSpeedIntro =5;
      }


   debugFuncExit("loadNewRoundPercs()");
}



//--------------------------------------------------------------------
//  Reset all the round structs, vars 
//-------------------------------------------------------------------- 
void cleanupFromPrevRound()
{

   Int i;

   // Also called at the start of a new game..
   debugFuncEnter("cleanupFromPrevRound()");

   for (i=0;i < MAXNUMBUGS;i++) {

      if ( allSprites[i] != NULL ) {
	 allSprites[i]->x=0;
	 allSprites[i]->y=-20;
	 allSprites[i]->realX12calc=false; 
	 allSprites[i]->realCogXcalc=false; 
	 allSprites[i]->realY12calc=false; 
	 allSprites[i]->realCogYcalc=false; 
	 DelFromActiveList(allSprites[i]);
      }

      roundInfo.bugs[i].aliveState   = dead;
   }

   for (i=0;i<MAX_NUM_BUGGROUPS;i++) {
     roundInfo.bugGroupCount[i] =0;
   }

   // Reset the round info struct
   roundInfo.currentWave      = 0;            // no intro waves flown yet
   roundInfo.numTicks         = gt;   
   roundInfo.numStartBugs     = 0;            // no bugs on display yet
   roundInfo.numBarriers      = 0;
   roundInfo.numRemainBugs    = 0;
   roundInfo.numBugsDiving    = 0;            // no bugs diving in intro or gameplay
   roundInfo.numBugsPeeledOff = 0;
   roundInfo.numBulletsFlying = 0;        // can be screwed up from previous game (documented)
   roundInfo.numMinesFlying   = 0;  
   roundInfo.numTracBeamsTried = 0;  
   roundInfo.inBlitzMode      = false;
   
   roundInfo.numBugGroups     = 0;
   roundInfo.currBugGroup     = 0;   
   roundInfo.ms.currentLeg    = 0;               // reset the formation move struct
   roundInfo.ms.sendThisEvent = SignalNoSignal;
   roundInfo.tracBeamOwnerSpriteNum =-1;  // in case full mothership left round (how?)
   roundInfo.tracBeamSpriteNum      =-1;
   roundInfo.bonus2ShotAwardSpriteNum =-1;

   roundSubInfoCount=0;
   wavesCount=0; 

   UtilCPURestrictions=0;

   // Reset colors -- error boxes can screw them up
   //GraphicsHdlNewForm(gameScreenForm);
   
   StrPrintF(debugStr2," "); 
   debugFuncExit("cleanupFromPrevRound()");
}



//--------------------------------------------------------------------
//   
//-------------------------------------------------------------------- 
void GameManagerInit()
{

   // debug
   debugFuncEnter( "GameManagerInit()" );

   // not called currently
   
   debugFuncExit("GameManagerInit()");
}


//--------------------------------------------------------------------
//   
//  Main routine for game progress.  Switch on state
//  
//-------------------------------------------------------------------- 
void HandleGameProgress() 
{

   // debug
   debugFuncEnterInt( "HandleGameProgress(), appState",appState);

   if (DEBUGTEXT) {
      StrPrintF(debugStr,"GamePlay (%d), next=%d  ",gt,(roundInfo.numTicks-gt));
      StrPrintF(debugStr2,"usBl=%d,bgBl=%d,mn=%d,dv=%d,left=%d",
               roundInfo.numUserBulletsFlying, roundInfo.numBulletsFlying,  \
               roundInfo.numMinesFlying, roundInfo.numBugsDiving, roundInfo.numRemainBugs );
   }
 
   if (appState == gameIsPaused) {
      HdlGameState_Paused(); 
   } else {

      switch (gameInfo.gameState)
      {
	 case gamePlay:
	    HdlGameState_GamePlay();
	    break;
	    
	 case before1stRound:
	    HdlGameState_Before1stRound();
	    break;
	    
	 case betweenRounds:
	    HdlGameState_BetweenRounds();
	    break;
	    
	 case setupNextIntro:
	    HdlGameState_setupNextIntro();
	    break;     
	    
	 case introWaves:
	    HdlGameState_IntroWave();      
	    break;
	    
	 case waitForFormation:
	    HdlGameState_WaitForFormation();
	    break;
	    
	    // gamePlay goes here
	    
	 case waitForEnemyDivingOver:
	    HdlGameState_WaitForEnemyDivingOver();
	    break;
	    
	 case reintroducePlayer:
	    HdlGameState_ReintroducePlayer();
	    break;
	    
	 case restAfterGamePlay:
	    HdlGameState_RestAfterGamePlay();
	    break;
	    
	 case gameOver:
	    HdlGameState_GameOver();
	    break;
	    
	 case waitForGameRestart:
	    HdlGameState_WaitForGameRestart();
	    break;
	    
	 case blah:
	    HdlGameState_blahPlay(); 
	    break;
	    
	 default:
	    debugPutIntError("unknown game state",gameInfo.gameState);
	    break; 
      } 

   } 

   debugFuncExit("HandleGameProgress()");
}



//--------------------------------------------------------------------
//
//  This proc responsible for loading all the structures to start
//  a new game round.
//-------------------------------------------------------------------- 
void HdlGameState_setupNextIntro()
{ 
   
   Int bg,sn,bgi,i,numInLine,mi=0;

   debugFuncEnter( "HdlGameState_setupNextIntro()" );

   cleanupFromPrevRound();  // Cleanup from the past round
   loadNewRoundPercs();    // set the percentages for the new round
   loadNewRoundInfo();     // load new data structs which define the new round      

   got_here;
   //loadNewRoundText();

    textMgrTitleBar.needsRedraw = true;  // update the round/wave display
   
   // Build the rest of the roundInfo
   for (i=0;i<roundSubInfoCount;i++) {

      bg = roundSubInfo[i].bugGroup;
      numInLine=0;

      // Calculate and store num bugs in group... handle the case of multiple
      // roundSubInfo defining 1 bugGroup
      bgi = roundInfo.bugGroupCount[bg];

      if (bgi==0) {
         roundInfo.numBugGroups++;
      } 

      roundInfo.bugGroupCount[bg] += ( roundSubInfo[i].lastSpriteNum - 
                                       roundSubInfo[i].firstSpriteNum +1);

      mi = roundSubInfo[i].firstMoveIndex;

      // "Skin" the bugs
      for (sn=roundSubInfo[i].firstSpriteNum;sn<=roundSubInfo[i].lastSpriteNum;sn++) {

         // Create a struct for each bug
         roundInfo.bugs[sn].spriteNum           = sn;    // index into allSprites[]
         roundInfo.bugs[sn].bugType             = roundSubInfo[i].spriteType;  // bug1, bug2, etc.
         roundInfo.bugs[sn].bugGroup            = bg;
         roundInfo.bugs[sn].bugGroupIndex       = bgi; 
         roundInfo.bugs[sn].aliveState          = inFormation; 
         roundInfo.bugs[sn].numFlybacksAllowed  = 0;
         roundInfo.bugs[sn].origX               = roundSubInfo[i].initX + 
                                                      (roundSubInfo[i].deltaXspace*numInLine);
         roundInfo.bugs[sn].origY               = roundSubInfo[i].initY;

	 roundInfo.bugs[sn].motionIndex         = mi;
	 mi = ++mi%8;

         // Handle different bug quirks
         switch (roundInfo.bugs[sn].bugType)
	 {
	    case bigOafBmp:  roundInfo.bugs[sn].numHitsToKill = 2;
                             break; 

	    case barrierBmp: roundInfo.bugs[sn].numHitsToKill = 5; 
	                     roundInfo.numBarriers++;
                             roundInfo.bugGroupCount[bg] =0;
                             break;  

	    default:         roundInfo.bugs[sn].numHitsToKill = 1; 
                             break;
	 }

	 // debug
         debugPutInt("sn",sn);
	 debugPutInt("  origX",roundInfo.bugs[sn].origX);
	 debugPutInt("  origY",roundInfo.bugs[sn].origY);
	 debugPutInt("  bg",bg);
	 debugPutInt("  bgi",bgi);
         debugPutInt("  i",i);
	 debugPutInt("  1st  sprite",roundSubInfo[i].firstSpriteNum);
	 debugPutInt("  last sprite",roundSubInfo[i].lastSpriteNum);

         // Modify the sprite itself also.. 

         //       ******* NOTE: we are modifiying a sprite without ********
	 // setting used=true, this means the sprite will not be saved if the user
	 // quits the game before intro waves are complete THEREFORE, any code here
	 // must be replicated in RestoreGamemanRecs

         allSprites[sn]->x=roundInfo.bugs[sn].origX;       // overwritten in doWave
         allSprites[sn]->y=roundInfo.bugs[sn].origY;       // overwritten in doWave
         allSprites[sn]->moveStructOwner=sprEng;           // overwritten in doWave
         allSprites[sn]->bitmap=roundSubInfo[i].spriteType;

         // Store the index (spriteNum) from the bug group to the individual bug
         roundInfo.bugGroup[bg][bgi] = sn;

         bgi++;
         numInLine++;
	 roundInfo.numStartBugs++;

         // Handle vectors of only 1 bug..
	 if (roundSubInfo[i].firstSpriteNum == roundSubInfo[i].lastSpriteNum) {
            break;
	 }

      } // sn
 
   } // bg
 
   // Start with 2nd to last group (bugs dive off 2 lower levels first this way)
   roundInfo.currBugGroup   = (roundInfo.numBugGroups >0) ? roundInfo.numBugGroups-1 : 0; 
                     
   roundInfo.numRemainBugs  = roundInfo.numStartBugs - roundInfo.numBarriers;  // no enemies are dead yet


   debugDumpRoundInfoNonBugs();

   // Goto next game state
   appState = gameIsActive;

   TextMgrCreateRoundBanner();  // moved here so banner only created once

   gameInfo.gameState=betweenRounds;
   roundInfo.numTicks =  gt+30;

   debugFuncExit("HdlGameState_setupNextIntro()");
}

//-----------------------------------------------------------------------
//  debugDumpRoundInfoNonBugs
//-----------------------------------------------------------------------
void debugDumpRoundInfoNonBugs()
{
  Int i,j;

    debugFuncEnter("debugDumpRoundInfoNonBugs()");
    debugPutInt("   roundInfo.currentWave",roundInfo.currentWave);
    debugPutInt("   roundInfo.numStartBugs",roundInfo.numStartBugs);
    debugPutInt("   roundInfo.numBarriers",roundInfo.numBarriers);
    debugPutInt("   roundInfo.numRemainBugs",roundInfo.numRemainBugs);
    debugPutInt("   roundInfo.numBugsDiving",roundInfo.numBugsDiving);
    debugPutInt("   roundInfo.numBugsPeeledOff",roundInfo.numBugsPeeledOff);
    debugPutInt("   roundInfo.numBulletsFlying",roundInfo.numBulletsFlying);
    debugPutInt("   roundInfo.numMinesFlying",roundInfo.numMinesFlying);
    debugPutInt("   roundInfo.currBugGroup",roundInfo.currBugGroup);
    debugPutInt("   roundInfo.numBugGroups",roundInfo.numBugGroups);

    for (i=0;i<MAX_NUM_BUGGROUPS;i++)
    {
      debugPutInt("   roundInfo.bugGroupCount[i]",roundInfo.bugGroupCount[i]);

      for(j=0;j<roundInfo.bugGroupCount[i];j++)
      {
         debugPutInt("      spriteNum",roundInfo.bugGroup[i][j]);
      }

    }


   debugFuncExit("debugDumpRoundInfoNonBugs()");
}


//-----------------------------------------------------------------------
//  GameState == Before1stRound
//
// User is just starting/restarting the game
//-----------------------------------------------------------------------
void HdlGameState_Before1stRound()
{
   UInt8 delay=10;

   // debug
   debugFuncEnter("HdlGameState_Before1stRound()");
   debugOnscreen("Before1stRound");

   // Cleanup any previous gameInfo
   gameInfo.score =0;   
   gameInfo.numShips =3;
   gameInfo.round=1;   //7=max sprites, 12=barriers

   // Debug
   if (gameInfo.round !=1) {
      debugPutIntError("reset gameInfo.round",gameInfo.round);
   }

   gameInfo.roundType=regularRound;     
   gameInfo.userState=user_dead; // needed for ResetUserSprite();
   gameInfo.twoShots=false;
   gameInfo.debugTimesHit=0;  
   gameInfo.nextExtraShipScore=firstExtraShipScore;

   textMgrTitleBar.needsRedraw = true;  // refresh

   
   // Handle bad gt's from saved games, etc
   if (gt>delay) {
     debugPutIntError("Caught a bad gt at start of game, gt",gt);
     gt=0;
   }

   // cleanup any previous roundInfo
   cleanupFromPrevRound();

   // User cleanup
   ResetUserSprite();

   // goto 1st round
   if (gt==delay) {
      roundInfo.numTicks = gt + 20;
      gameInfo.gameState=setupNextIntro;
   }  

   debugFuncExit("HdlGameState_Before1stRound()");
}

//-----------------------------------------------------------------------
//  GameState == BetweenRounds
//
//  Display upcoming round number
//-----------------------------------------------------------------------
void HdlGameState_BetweenRounds()
{
 
   // debug
   debugFuncEnter("HdlGameState_betweenRounds()");

   got_here;

   if (gt == roundInfo.numTicks) {
      TextMgrClr(); // remove the banner
      SoundMgrThisSound(newRound);

      //DEBUG
      if ( SKIPINTROWAVES || skipIntroFlyIns ) {
         debugSkipIntroWaves();
         gameInfo.gameState=gamePlay;
	 skipIntroFlyIns=false;
         debugFuncExit("HdlGameState_BetweenRounds()");
         return;
      }

      gameInfo.gameState=introWaves;
      roundInfo.numTicks = gt + 20;
   }
   


   debugFuncExit("HdlGameState_BetweenRounds()");
}

//-----------------------------------------------------------------------
// GameState == RestAfterGamePlay
//
// Delay when round is over to provide rest to user. 
//-----------------------------------------------------------------------
void HdlGameState_RestAfterGamePlay()
{

   sprite * s;

   debugFuncEnter("restAfterGamePlay()");
   debugOnscreen("restAfterGamePlay");

   debugDumpRoundInfoNonBugs();

   // Start/increment guard timer
   ticksInRestState = (ticksInRestState==-1) ? 1 : ticksInRestState+1;

   // Gaurd against waiting to long because of stuck counters
   if ((ticksInRestState > (yMaxBorder/rps.bulletSpeed + 5)) &&
       (gt >= roundInfo.numTicks))
   {

      // cleanup weird negative count errors
      if (roundInfo.numBulletsFlying < 0) {
	ERROR_ASSERT("NEAL: numBulletsFlying <0");
	debugPutIntError("roundInfo.numBulletsFlying <0, ",roundInfo.numBulletsFlying);
	roundInfo.numBulletsFlying=0;
      }

      if (roundInfo.numMinesFlying < 0) {
	ERROR_ASSERT("NEAL: numMinesFlying <0");
	debugPutIntError("roundInfo.numMinesFlying <0, ",roundInfo.numMinesFlying);
	roundInfo.numMinesFlying=0;
      }

      if (roundInfo.numBulletsFlying != 0) {
	ERROR_ASSERT("NEAL: numBulletsFlying !=0");
	debugPutIntError("roundInfo.numBulletsFlying !=0, ",roundInfo.numBulletsFlying);

	// Cleanup
	s = SpriteEngFindFirstUsedSprite( BugBulletSpriteClass );
	while ( s != NULL )
	{
	    debugPutIntError("  hung bullet",s->spriteNum);
	    DelFromActiveList(s);
            s = SpriteEngFindFirstUsedSprite( BugBulletSpriteClass );
        }

	roundInfo.numBulletsFlying=0;
      }


      if (roundInfo.numMinesFlying != 0) {
	ERROR_ASSERT("NEAL: numMinesFlying is hung");
	debugPutIntError("roundInfo.numMinesFlying !=0, ",roundInfo.numMinesFlying);

	// Cleanup 
	s = SpriteEngFindFirstUsedSprite( MineSpriteClass );
	while ( s != NULL )
	{
	    debugPutIntError("  hung mine",s->spriteNum);
	    DelFromActiveList(s);
            s = SpriteEngFindFirstUsedSprite( MineSpriteClass );
        }

	roundInfo.numMinesFlying=0;
      }

   }

   // Ensure all bullets are gone
   if ((gt >= roundInfo.numTicks) && 
       (roundInfo.numBulletsFlying == 0) && 
       (roundInfo.numMinesFlying == 0))
   {
      ticksInRestState=-1;

      gameInfo.round++;                    
      //debugPutsError("restore round incr");

      gameInfo.gameState=setupNextIntro;    // goto next round
      roundInfo.numTicks = gt + 10;

   }


   debugFuncExit("HdlGameState_RestAfterGamePlay()");
}

//-----------------------------------------------------------------------
// GameState == GameOver
//-----------------------------------------------------------------------
void HdlGameState_GameOver()
{
   Boolean hiScore=false,hiRound=false;


   sprite * s = NULL;

   // debug
   debugFuncEnter("HdlGameState_GameOver()");
   //debugOnscreen("gameOver");


   // Cleanup vars
   appState = gameIsOver;
   gameInfo.userState = user_dead;
   allRecordMustBeRewritten=true;  // Ensure restart number is saved

   // remove trac beam
   if ( roundInfo.tracBeamSpriteNum != -1) {
     FreeSprite(allSprites[roundInfo.tracBeamSpriteNum]);
     roundInfo.tracBeamSpriteNum = -1;
   }

   // blow away any remaining bugs, bullets, mines
   if (gt >= roundInfo.numTicks) {

     s=SpriteEngFindFirstUsedSprite(MineSpriteClass);

     if (s==NULL) {
       s=SpriteEngFindFirstUsedSprite(TracBeamSpriteClass);
     }

     if (s==NULL) {
       s=SpriteEngFindFirstUsedSprite(MiscSpriteClass);
     }

     if (s==NULL) {
       s=SpriteEngFindFirstUsedSprite(UserBulletSpriteClass);
     }

     if (s==NULL) {
        s=SpriteEngFindFirstUsedSprite(BugBulletSpriteClass);
     }

     if (s==NULL) {
       s=SpriteEngFindFirstUsedSprite(UserSpriteClass);
     }

     if (s==NULL) {
        s=SpriteEngFindFirstUsedSprite(BugSpriteClass);
     }

     if (s != NULL) {
       showExplosion(s);
       FreeSprite(s);
     } else {

	// Ensure text colors are correct (menus and dialog boxes can screw them up   ... hack
	//GraphicsHdlNewForm(gameScreenForm);

	// OK, all sprites are now exploding
	//gameInfo.userState = user_dead;
	
	TextMgrAddCenteredText("GAMEOVER",0,largeBoldFont,50,true);
	TextMgrAddCenteredText("Press a button to restart",1,stdFont,70,true);
	
	// See if a high score/wave was achieved
	if (gameInfo.score > mainGameRec1.highScore) { 
	   hiScore=true;
	   StrCopy(miscText,"HIGH SCORE!");
	}
	
	if (gameInfo.round > mainGameRec1.highWave) { 
	   hiRound=true;
	   StrCopy(miscText,"HIGHEST WAVE SO FAR!");
	}
	
	if (hiScore && hiRound) {
	   StrCopy(miscText,"HIGHEST SCORE & WAVE SO FAR!");
	}

	if (hiScore || hiRound) {
	   // create banner, save hi score/wave in record
	   TextMgrAddCenteredText(miscText,2,stdFont,90,true);
	   DbMgrWriteMainAppRecords();  
	}
	

	// Set a limit on how long we wait for the user to restart the game
	gameInfo.gameState=waitForGameRestart;
	roundInfo.numTicks = gt+TICKSWAITFORRESTART;  

     }

   }


   debugFuncExit("HdlGameState_GameOver()");
}

//-----------------------------------------------------------------------
// GameState == Waiting for a restart
//-----------------------------------------------------------------------
void HdlGameState_WaitForGameRestart()
{

   debugFuncEnter("HdlGameState_WaitForGameRestart()");


   // Return to splash screen if we have been waiting for so long
   if (gt >= roundInfo.numTicks) {
      gt=0;
      TextMgrClr();
      gameInfo.gameState=before1stRound;

      UtilRestoreHardKeys();
      FrmGotoForm(splashScreenForm); 
   }
   

   // Wait for button press
   if ((KeyCurrentState() & (keyBitPageUp|keyBitPageDown|keyBitHard1|
			     keyBitHard2|keyBitHard3|keyBitHard4)) >0)
   {
      gt=0;
      TextMgrClr();
      gameInfo.gameState=before1stRound;

      
      // Ensure restarts are allowed, if not go to nag screen instead of
      // restarting the game
      // if (SecureMgrAllowImmediateGamePlay() != unrestricted) {
      //UtilRestoreHardKeys();
      // HdlUserSaysStart();  // calculate nag value and go to nag screen
      //} else {
	 // nag-encumbered starts will increment on their own
	 numTimesRestarted++;	 
	 // }    
      
   }

   debugFuncExit("HdlGameState_WaitForGameRestart(()");
}



//-----------------------------------------------------------------------
// GameState == Paused
//-----------------------------------------------------------------------
void HdlGameState_Paused()
{

   // debug
   debugFuncEnter("Paused()");
   debugOnscreen("paused");

   
   TextMgrAddCenteredText("    PAUSED    ",0,largeBoldFont,50,true);
   TextMgrAddCenteredText(" Press a button to resume ",1,stdFont,70,true);
   
   // Wait for button press
   if ((KeyCurrentState() & (keyBitPageUp|keyBitPageDown|keyBitHard1|
			     keyBitHard2|keyBitHard3|keyBitHard4)) >0)
   {
      appState = gameIsActive;
      TextMgrClr();
   }

   debugFuncExit("HdlGameState_Paused()");
}



//-----------------------------------------------------------------------
//  dummy function
//-----------------------------------------------------------------------
void HdlGameState_blahPlay()
{

   // debug
   debugFuncEnter("HdlGameState_blahPlay()");


   debugFuncExit("HdlGameState_blahPlay()");
}


//-----------------------------------------------------------------------
//  *** DEBUG function ***
//-----------------------------------------------------------------------
void debugSkipIntroWaves()
{

   Int i;

   debugFuncEnter("debugSkipIntroWaves()");

   for (i=0;i<roundInfo.numStartBugs;i++)
   {
      AddToActiveList(allSprites[i]);
      HandleSpriteOverBorder(allSprites[i],0);
   }

   roundInfo.numTicks=gt+rps.timeBefore1stBugDive;
   gameInfo.gameState=gamePlay;

   roundInfo.numBugsDiving=0;  // required

   debugFuncExit("debugSkipIntroWaves()");
} 


//--------------------------------------------------------------------
//  GameState == IntroWave
//
//   Count ticks, then start the intro waves   
//-------------------------------------------------------------------- 
void HdlGameState_IntroWave() 
{

   // debug
   debugFuncEnter( "HdlGameState_IntroWave" );
   debugOnscreen( "IntroWave" );

   updateFormationMoveStruct();   // Do the formation movement

   // See if all bugs in the current wave were killed on fly in, if so
   // start next wave sooner
   if ((roundInfo.currentWave !=0) && 
       (roundInfo.numBugsDiving ==0) && 
       (roundInfo.numTicks - gt) > rps.maxTimeBetwIntroWaves) 
   {
      debugPuts("  all bugs killed...next wave started early");
      roundInfo.numTicks = rps.maxTimeBetwIntroWaves;
   }


   // See if a new wave should start
   if (gt >= roundInfo.numTicks ) {

      // Set next wave of bugs flying
      doWave(roundInfo.currentWave);

      // increment to next wave
      roundInfo.currentWave++;
      textMgrTitleBar.needsRedraw = true;

      // See if all into waves are complete
      if ( roundInfo.currentWave == wavesCount ) {

         // Yes, ok wait for bugs to get back to formation
         roundInfo.numTicks=gt+20;
         gameInfo.gameState=waitForFormation;

      } else {

         // No, so set time of start of next wave
         roundInfo.numTicks = gt + thisRound.delayBetwWaves[roundInfo.currentWave];

      }

   }
 
   debugFuncExit("HdlGameState_IntroWave()");
}  


//-----------------------------------------------------------------------
// GameState == WaitForFormation
//
// After intro waves, wait for all bugs to get back to formation,
// before proceeding to regular game play.
//-----------------------------------------------------------------------
void HdlGameState_WaitForFormation()
{
 
  debugOnscreen("WaitForFormation");

    debugFuncEnter("HdlGameState_WaitForFormation()");
    debugPutInt("   roundInfo.numStartBugs",roundInfo.numStartBugs);
    debugPutInt("   roundInfo.numRemainBugs",roundInfo.numRemainBugs);
    debugPutInt("   roundInfo.numBugsDiving",roundInfo.numBugsDiving);

    // Do the formation movement
    updateFormationMoveStruct();

    // See if all alive bugs have been in formation
    if ( roundInfo.numBugsDiving == 0 )
    {
         roundInfo.numTicks=gt + rps.timeBefore1stBugDive;
         gameInfo.gameState=gamePlay;

    }

   debugFuncExit("HdlGameState_WaitForFormation()");
}

//-----------------------------------------------------------------------
// GameState -- WaitForEnemyDivingOver
//-----------------------------------------------------------------------
void HdlGameState_WaitForEnemyDivingOver()
{
    debugOnscreen("WaitForEnemyDivingOver");
    
    debugFuncEnter("HdlGameState_WaitForEnemyDivingOver()");
    debugPutInt("   roundInfo.numBugsDiving",roundInfo.numBugsDiving);


    // Do the formation movement...
    updateFormationMoveStruct();


    // See if all diving bugs are done
    if ( roundInfo.numBugsDiving == 0 ) 
    {
       roundInfo.numTicks=gt+20; // how long to remain in reintroducePlayer mode
       gameInfo.gameState=reintroducePlayer;
    }

   debugFuncExit("HdlGameState_WaitForEnemyDivingOver()");
}

//-----------------------------------------------------------------------
//  GameState == ReintroducePlayer
//-----------------------------------------------------------------------
void HdlGameState_ReintroducePlayer()
{

   // debug
   debugFuncEnter("HdlGameState_ReintroducePlayer()");
   debugOnscreen("ReintroducePlayer");
   debugPutInt("  gameInfo.savedGameState",gameInfo.savedGameState);

   updateFormationMoveStruct();

   // restart play
   if (gt == roundInfo.numTicks) {
      ResetUserSprite();
      SoundMgrThisSound(playerReintroduced);
      roundInfo.numTicks = gt + 15;
      gameInfo.gameState=gameInfo.savedGameState;
   }

 

   debugFuncExit("HdlGameState_ReintroducePlayer()");
}

//-----------------------------------------------------------------------
// GameState == GamePlay
//
// Proc which runs normal game play.  Responsible for sending out new
// bugs.
//----------------------------------------------------------------------
void HdlGameState_GamePlay()
{
   Int bgi; 
   Int findSpriteCount=3;  // controls how many times we look for a bug
   Int spriteNum=0;
   Boolean foundSprite=false,sendTracBeam=false;
   sprite * s;
   sprite * barrierSprite=NULL;

   //char t1[40] = " ";
   //char t2[40] = " ";

   debugFuncEnter("HdlGameState_GamePlay()");


   // debug 
   if ( SKIPREGPLAY ) {
      gameInfo.gameState=restAfterGamePlay;
      roundInfo.numTicks = gt + 30;
   }

   // debug
   if ( HOLDFORMATION ) {
     debugFuncExit("HdlGameState_GamePlay()");
     return;
   }


   // check to see if all the bugs have been killed -- jazz up (and add to intro stage)
   debugPutInt("   roundInfo.numRemainBugs",roundInfo.numRemainBugs);
   debugPutInt("   roundInfo.numBugsDiving",roundInfo.numBugsDiving);

   if (roundInfo.numRemainBugs < 1) {
      gameInfo.gameState=restAfterGamePlay;
      roundInfo.numTicks = gt + 30;
   } 

   updateFormationMoveStruct();

   // See if blitz mode has started
   if (roundInfo.numRemainBugs <= rps.numBugsForBlitzMode) 
   {
     roundInfo.inBlitzMode = true;   
   }

   // If in blitz, destroy any remaining barriers
   if ((roundInfo.inBlitzMode)&&(roundInfo.numBarriers !=0))
   {
      barrierSprite = SpriteEngFindThisSpriteType(BugSpriteClass,barrierBmp,NULL);

      if (barrierSprite != NULL) {
	showExplosion(barrierSprite);
        FreeSprite(barrierSprite);
        roundInfo.numBarriers--;
      }
   }

   // See if enough bugs are already flying
   if ((gt == roundInfo.numTicks) &&
       (roundInfo.numBugsDiving >= rps.maxBugsInFlight))
   {
     // Prevent new launch
     roundInfo.numTicks+=3;
   }

   // See if we are in a tight CPU case 
   if ((UtilCPULevel >= CPULimitForBugFlight) &&
       (gt == roundInfo.numTicks) &&
       (roundInfo.numBugsDiving >= CPULimitForBugsDiving ))
   {
      // Prevent new launch
      roundInfo.numTicks+=CPUTicksToDelayBugFlight;
      UtilCPURestrictions++;
   } 
   
 
   // Launch bugs  
   if (gt == roundInfo.numTicks) {

      while ((findSpriteCount != 0) && (foundSprite == false )) { 

        findSpriteCount--;

        // debug
        debugPutInt("AA Getting bug from group",roundInfo.currBugGroup);
        debugPutInt("AA From group # bugs left", roundInfo.bugGroupCount[roundInfo.currBugGroup]);
	debugDumpRoundInfoNonBugs();

        // Ensure there are bugs alive in this group, if not go to next group
        if  ( roundInfo.bugGroupCount[roundInfo.currBugGroup] == 0) 
	{
	   // Code is duplicated below for mineLayers
	   roundInfo.currBugGroup = (roundInfo.currBugGroup == roundInfo.numBugGroups) ? 
	      0 :roundInfo.currBugGroup+1;
	   continue;
	}

 
        // Pick a bug at random from group  -- need to seed!!!
	bgi = randomNum(roundInfo.bugGroupCount[roundInfo.currBugGroup]);
        spriteNum = roundInfo.bugGroup[roundInfo.currBugGroup][bgi];

        debugPutInt("bgi (z) z",bgi); 

        // Check to make sure spriteNum is not already in motion
	if ( roundInfo.bugs[spriteNum].aliveState == inFormation ) 
        {
           if ( notActive(allSprites[spriteNum]) ) {
	      //StrCat(t1,"E1: count=");
	      //StrIToA(t2,roundInfo.bugGroupCount[roundInfo.currBugGroup]);
	      //StrCat(t1,t2);
	      //StrCat(t1," bgi=");
	      //StrIToA(t2,bgi);
	      //StrCat(t1,t2);

	     debugOnscreen("E1 sprite in formation but USED=false, launching anyway");
	     debugPutIntError("used flag is false (KLUDGE FIX), sprite",spriteNum);
	     AddToActiveList(allSprites[spriteNum]);
           }
 
           foundSprite = true;
	   
	} else {
           if ( roundInfo.bugs[spriteNum].aliveState == dead ) {
              debugPutIntError("E2 bug is dead, but still in list, sprite",spriteNum);
           } 
	}


      } // while


      // Ensure we found a bug	
      if ( foundSprite ) {

         s=allSprites[spriteNum];

         debugPutInt("Put sprite into motion, spriteNum",s->spriteNum);
         debugPutInt("  from group",roundInfo.currBugGroup);
 
         // check for mismatch
	 if ( spriteNum != roundInfo.bugs[spriteNum].spriteNum) {
            debugPutIntError("spriteNum mismatch, spriteNum",spriteNum);
	 }

         // bug??
         if (notActive(s)) {
	    ERROR_ASSERT("used flag is false, launching anyway");
            debugPutIntError("used=false for spriteNum",s->spriteNum);
	    AddToActiveList(s);
	 }

	 // Ensure bitmap is restored if the sprite is in the middle of
	 // back-in-form sequence
	 if (s->moveStructOwner == userAppl1) {
	    s->bitmap = s->myMoveStruct.leg[0].guts.inFormSeqStruct.restoreBitmap;
	    s->moveStructOwner = userAppl0;  

	    // BUG FIX (see blast.c)
	    s->realCogYcalc = false;    
	    s->realY12calc  = false;

	 }
	 
 
	 debugPutInt("rps.numBugsForBlitzMode",rps.numBugsForBlitzMode);

         // See if we are in blitz mode (end of round craziness)
         if ( !roundInfo.inBlitzMode )
	 {
            // The bug type sets the behavior
            switch (roundInfo.bugs[s->spriteNum].bugType)
            {
            case bug1:        
	       setBug1Motion(s);
	       break;

            case sweepBug:    
	       setSweepBugMotion(s);
	       break; 

            case mineLayer:   

	       if (UtilCPULevel < CPULimitForMineLayer ) {  
		  setMineLayerMotion(s);
	       } else {
		  // MineLayer puts a heavy tax on CPU, and we can launch it at this time
		  foundSprite=false;
		  roundInfo.currBugGroup = (roundInfo.currBugGroup == roundInfo.numBugGroups) ? 
		     0 :roundInfo.currBugGroup+1;

		  // Need to restore the moveStructOwner here!!!

		  UtilCPURestrictions++;
	       }
	       break;
	
	    case mothership:  sendTracBeam = randomPercTest(rps.percChanceMothershipBeam);
                              if ((roundInfo.tracBeamOwnerSpriteNum!=-1) ||
                                  (gameInfo.twoShots) || 
                                  (roundInfo.numTracBeamsTried >= rps.maxTracBeams))
                              {
				  sendTracBeam = false;
			      } 
                              setMothershipMotion(s,sendTracBeam);break;

            case dartBug:     setDartBugMotion(s);break;
            case bigOafBmp:   setBigOafMotion(s);break; 

            default: 
	       debugPutIntError("unknown bugType",roundInfo.bugs[s->spriteNum].bugType);	
	       foundSprite=false;
            }

         } else {
	    // set blitz motion
	    // StrPrintF(debugStr2,"Blitz mode for s=%d",s->spriteNum);
	    setBlitzMotion(s);
	 }

	 // Check foundSprite again since some bugs may not have launched (typically CPU bound)
	 if (foundSprite) {

	    // Set the state, so we don't reuse it
	    roundInfo.bugs[spriteNum].aliveState = diving;
	    roundInfo.numBugsDiving++;
	    
	    // Incr current bug group for next round
	    roundInfo.currBugGroup++;
	    if ( roundInfo.currBugGroup == roundInfo.numBugGroups) {
	       roundInfo.currBugGroup = 0;
	    }
	 }
	 

      }


      // Set clock for next bug  -- could randomize here
      if ((foundSprite) && (!roundInfo.inBlitzMode))
      {
         roundInfo.numTicks=gt + rps.minTicksBetwBugs; 
      } else {
         debugPuts("Couldn't find a bug, or we're in blitz mode");
         roundInfo.numTicks=gt+1;
      }
 
   }  // if gt=launch bug
  

   debugFuncExit("HdlGameState_GamePlay()");

}



//--------------------------------------------------------------------
//   
// Start an intro wave by loading the flight paths into the 
// specified sprites.
//  
//-------------------------------------------------------------------- 
void doWave(Int w)
{

   Int i,movementIndex,sn;
   sprite * s;
   introWaveStruct     * iws;
   Int numSprites;


   // debug
   debugFuncEnterInt( "doWave() w",w );


   // Get the wave struct, extract numsprites 
   iws=&waves[w];
   numSprites    = iws->numSprites;
   movementIndex = 0;

   // Loop over all the bugs, set their flight path
   for(i=0;i<numSprites;i++)
   {
      // Get the sprite using the list in the wave structure
      sn=iws->theseSprites[i];
      s=allSprites[ sn ];

      AddToActiveList(s);


      roundInfo.bugs[sn].aliveState = doingIntro;
      roundInfo.numBugsDiving++;
     
      switch ( iws->movements[movementIndex] )
	{


	case fltB1:encodeFlightpath(s,i,ifb1,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;
	case fltB2:encodeFlightpath(s,i,ifb2,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;
	case fltB3:encodeFlightpath(s,i,ifb3,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;
	case fltB4:encodeFlightpath(s,i,ifb4,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;

	case fltE1:encodeFlightpath(s,i,ife1,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;
	case fltE2:encodeFlightpath(s,i,ife2,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;

	case fltF1:encodeFlightpath(s,i,iff1,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;
	case fltF2:encodeFlightpath(s,i,iff2,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;
	case fltF3:encodeFlightpath(s,i,iff3,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break;
	case fltF4:encodeFlightpath(s,i,iff4,rps.bugXSpeedIntro,rps.bugYSpeedIntro);break; 

	case (putInForm):
          HandleSpriteOverBorder(s,5); // hack
	  break;

 
        case (dropThenDiagLf):setDropThenDiag(s,i,false);break;

        case (dropThenDiagRt):setDropThenDiag(s,i,true);break;

        case (lfThenStop):
           dirThenStop(s,i,numSprites,roundInfo.bugs[sn].origX,roundInfo.bugs[sn].origY,-1,0);
           break;

	case (rtThenStop):
           dirThenStop(s,i,numSprites,roundInfo.bugs[sn].origX,roundInfo.bugs[sn].origY,1,0);
	   debugPutInt("  s",s->spriteNum);
	   debugPutInt("  roundInfo.bugs[sn].origX",roundInfo.bugs[sn].origX);
	   debugPutInt("  roundInfo.bugs[sn].origY",roundInfo.bugs[sn].origY);
           break;

	case (drpThenStop):
           dirThenStop(s,0,numSprites,roundInfo.bugs[sn].origX,roundInfo.bugs[sn].origY,0,1);
           break;

        default:
           setDropThenDiag(s,i,true);
           break;

	}



      // Incr to next movement index
      movementIndex++;
      if (movementIndex == iws->numMovements) {
         movementIndex=0;
      }
         
      debugPutInt("sprite",sn);
      debugPutInt("  s->borderAction[0]",s->borderAction[0]);
   }


   debugFuncExit("doWave()");
}



//-----------------------------------------------------------------------
//  updateFormationMoveStruct
//-----------------------------------------------------------------------
void updateFormationMoveStruct()
{


      // Same as sprite.c NewMovement, except no signal is sent

      roundInfo.ms.countdown--;

       if (roundInfo.ms.countdown == 0) {

          // Move to next leg, reset if need be
          roundInfo.ms.currentLeg++;

          if (roundInfo.ms.currentLeg == 2) {
             roundInfo.ms.currentLeg = 0;
          }

          roundInfo.ms.countdown=  roundInfo.ms.leg[roundInfo.ms.currentLeg].duration;

          debugPutInt("Formation struct leg changed, currentLeg",roundInfo.ms.currentLeg);
       }




   debugFuncExit("updateFormationMoveStruct()");
}

//--------------------------------------------------------------------
//  
//--------------------------------------------------------------------
void dummyProc (Int x, Int y) 
{
  randomSeed = randomSeed + (x-y);  


   debugFuncExit("dummyProc()");
}



//-----------------------------------------------------------------------
//  ASSUMES RECS are NOT already present
//-----------------------------------------------------------------------
Boolean SaveGamemanRecs()
{

    UInt16 index=0,err,offset=0;
    UInt32 uniqId=gameInfoRecUniqId;
    VoidHand recHdl=NULL;
    VoidPtr ptr;

    debugFuncEnter("SaveGamemanRecs()");


    // Bail if game is over, or still paused.  NOTE: this will cause
    // a bug if the user (1) switches to astron (2)changes settings
    // (3) switches out of astron.  Startup code just kills the game, and starts over
    if ((appState == gameIsStarting) ||
	(appState == gameOver) ||
	(appState == gameIsPaused))
    {
      debugPuts("  game over, no need to save vars");
      debugFuncExit("SaveGamemanRecs()");
      return true;
    }


    // Create, lock a new gameInfo record 
    uniqId=gameInfoRecUniqId;
    offset=0;

    recHdl = DmNewRecord(dbRef,&index,sizeof(gameInfo)); 
    ptr    = MemHandleLock(recHdl);

    err = DmWrite(ptr, offset, &gameInfo, sizeof(gameInfo));
    ErrFatalDisplayIf(err, "Neal: Could not write gameInfo record.");

    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);
    DmSetRecordInfo(dbRef, index, NULL, &uniqId); 

    got_here;

    // Create, lock new roundInfo record 
    uniqId=roundInfoRecUniqId;
    offset=0;

    recHdl = DmNewRecord(dbRef,&index,sizeof(roundInfo)); 
    ptr    = MemHandleLock(recHdl);

    err = DmWrite(ptr, offset, &roundInfo, sizeof(roundInfo));
    ErrFatalDisplayIf(err, "Neal: Could not write roundInfo record.");

    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);
    DmSetRecordInfo(dbRef, index, NULL, &uniqId); 



  debugFuncExit("SaveGamemanRecs()");
  return true;

   debugFuncExit("SaveGamemanRecs()");
} 

//-----------------------------------------------------------------------
//  Game restore routines
//
// Returns true if success in reading records
//-----------------------------------------------------------------------
Boolean RestoreGamemanRecs()
{

    UInt16 index=0,err,i,sn;
    UInt32 uniqId=gameInfoRecUniqId;
    VoidHand recHdl=NULL;
    VoidPtr ptr;

    debugFuncEnter("RestoreGamemanRecs()");

    // GAMEINFO
    uniqId=gameInfoRecUniqId;
    err = DmFindRecordByID(dbRef,uniqId,&index);

    if (err == dmErrUniqueIDNotFound) {
       debugPuts("  gameInfo record NOT present");
       debugFuncExit("RestoreGamemanRecs()");
       return false;
    }

    debugPutInt("  gameInfo record IS present, index",index);

    recHdl = DmGetRecord(dbRef,index);
    ptr    = MemHandleLock(recHdl);
    MemMove(&gameInfo,ptr,sizeof(gameInfo));
    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);

    debugPutInt("  score",gameInfo.score);

    // If player does not restart the game then the records will still be
    // there, so set this flag to false.  When pause is exited, app State goes to active
    appState = gameIsPaused;
    //gameIsPaused = true;
    //gameInfo.gameShouldBeSaved = false;


    // ROUNDINFO
    uniqId=roundInfoRecUniqId;
    err = DmFindRecordByID(dbRef,uniqId,&index);

    if (err == dmErrUniqueIDNotFound) {
       debugPuts("  roundInfo record NOT present");
       debugFuncExit("RestoreGamemanRecs()");
       return false;
    }

    debugPutInt("  roundInfo record IS present, index",index);

    recHdl = DmGetRecord(dbRef,index);
    ptr    = MemHandleLock(recHdl);
    MemMove(&roundInfo,ptr,sizeof(roundInfo));
    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);

    debugPutInt("  numTicks",roundInfo.numTicks);

    // Set static vars set in _Before1stRound state
    loadNewRoundInfo();
    got_here;

    // If game was saved in (1) BetweenRounds, or (2) IntroWaves then some bug info
    // was lost (see HdlGameState_setupNextIntro), fix that here (better way would be
    // to save all sprites regardless of used flag, or another approach).

    for (i=0;i<roundSubInfoCount;i++) {
      for (sn=roundSubInfo[i].firstSpriteNum;sn<=roundSubInfo[i].lastSpriteNum;sn++) {
	debugPutInt("  sn",sn);
	debugPutInt("  i",i);
         allSprites[sn]->bitmap=roundSubInfo[i].spriteType;
      }
    }

    debugFuncExit("RestoreGamemanRecs()");
    return true;


   debugFuncExit("RestoreGamemanRecs()");
}

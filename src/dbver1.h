//
//  Database ver 1,2,3 records
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

#ifndef _DBVER1_H_
#define _DBVER1_H_

#include "defs.h"  // game version

//----------------------------------------------------------------------
// Enums
//----------------------------------------------------------------------
typedef enum
{
  appNewlyInstalled, // game has just been installed
  splashScreen, 
  nagScreen,     // used for registration purposes
  gameIsStarting,
  gameIsPaused,  // game has been restored from records, but not restarted
  gameIsActive,  // user is playing game
  gameIsOver     // game is finished, but app has not stopped, and user has now restarted it
} appStateEnum;


//----------------------------------------------------------------------
//  Database version 1 
//----------------------------------------------------------------------

typedef struct
{ 
  // Preferences
  Byte          sidePref;      // CONTROLS ON: 1=left, 0=right, 2=middle
  Byte          volPref;       // 0=off,L,M,H,Def
  Byte          bulletSizePref; // 0=fat, 1=slim, 2=dot

  // Game records
  UInt32        highScore;
  UInt16        highWave;

  // Game state
  UInt16        gt;
  appStateEnum  appState;       // saved App state

  // Password, usage
  Byte          nagTimerVal;   // time left on nag timer
  Byte          pw[4];
  Byte          numTimesRun;
  ULong         installTimGetSeconds;
  Boolean       tenDayAdditionOn;     // when true, the user is given 10 more days of free play

} mainGameRecordStructV1;


//----------------------------------------------------------------------
//  Database version 2 (new in Astron 0.90)
//----------------------------------------------------------------------

typedef struct
{ 
  ULong         lastTimRan;
  UInt16        numTimesRestarted;     
  Boolean       timeModifiedFraudFlag;  // set to true if time is rolled back to cheat free play period
} mainGameRecordStructV2;


//----------------------------------------------------------------------
//  Database version 3 (new in Astron 1.00, modified in 1.05)
//----------------------------------------------------------------------

typedef struct
{ 
   Byte          bulletRepeatPref;
   Byte          astronVersion;       // stole colorScheme field in v1.05
   UInt32        colorCompatAlertId;  // !=0 means alert already given ...UNUSED FOR NOW...
} mainGameRecordStructV3;

//----------------------------------------------------------------------
//  Database version 4 (new in Astron 2.00)
//----------------------------------------------------------------------

typedef struct
{ 
   Byte         greyscaleTheme;
   Byte         backgroundGreyShade;
   Byte         backgroundStarfield;
   Byte         cpuSpeed;                // Under game control (not user)
   Byte         graphicsLevel;           // User is allowed to alter setting
   Boolean      greyscaleInstructGiven;  // =1 means greyscale instruction alert already shown
} mainGameRecordStructV4;




#endif


//
//   DATABASE.C
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

#include "starter.h"
#include "database.h"
#include "debug.h"
#include "dbver1.h"     // for mainGameRec's
#include "gameman.h"    // for gameInfo


// database values
DmOpenRef dbRef=0;
UInt      dbVer=0;
UInt16    dbNumRecords=0;


//----------------------------------------------------------------------
// DbMgrOpenOrCreateGameDb
//
// Attempt to open an existing database.  If it is not present then the
// database is created.  
//
//        *** NO RECORDS ARE WRITTEN OR READ ***
//
// Sets: dbVer, dbNumRecords
//
//----------------------------------------------------------------------
Boolean DbMgrOpenOrCreateGameDb()
{
  UInt16  err;
  LocalID dbID;
  UInt16  cardNo;

  debugFuncEnter("DbMgrOpenOrCreateGameDb()");

  // See if database is already present..
  dbRef = DmOpenDatabaseByTypeCreator('Data','NEAL',dmModeReadWrite);

  if (dbRef == 0) {

    // Database not created yet, do it...
    // According to "Handspring Springboard Dev Guide" all variable data MUST
    // be written to card 0 
    err = DmCreateDatabase(0,"Astron game",'NEAL','Data',false);

    if (err) {
       // Big problems -- out of memory
       debugFuncExit("DbMgrOpenOrCreateGameDb()");
       return false;
    }

    dbRef = DmOpenDatabaseByTypeCreator('Data','NEAL',dmModeReadWrite);

    // Set the version of the database (get dbID and cardNo first)
    dbVer = DATABASEver;
    DmOpenDatabaseInfo(dbRef,&dbID,0,0,&cardNo,0);
    DmSetDatabaseInfo (cardNo,dbID,0,0,&dbVer,0,0,0,0,0,0,0,0);

  } else {

    // Read the version of the database (get dbID and cardNo first)
    DmOpenDatabaseInfo(dbRef, &dbID, 0, 0, &cardNo, 0);
    DmDatabaseInfo (cardNo, dbID, 0, 0, &dbVer, 0, 0, 0, 0, 0, 0, 0, 0);

    debugPutInt("   db present, dbVer",dbVer);

  }
	
  // See if there are any records
  dbNumRecords=DmNumRecords(dbRef);
  //debugPutInt("   dBNumRec",dbNumRecords);

  debugFuncExit("DbMgrOpenOrCreateGameDb()");
  return true;

}

//----------------------------------------------------------------------
// DbMgrReadMainAppRecords
//
// Reads the main game records stored by Astron.  If they are not
// present then a blank record is written to tbe database because
// future writes expect a record to be present (this functionality
// should eventually be moved to WriteMainAppRecords).
//
// If the records are present they will overwrite the defaults
// set before (if they are not present the defaults will still
// exist).
//
//----------------------------------------------------------------------
Boolean DbMgrReadMainAppRecords()
{

    UInt16 index=0,err,i;
    UInt32 uniqId=mainGameRec1UniqId;
    VoidHand recHdl=NULL;
    VoidPtr ptr;

    debugFuncEnter("DbMgrReadMainAppRecords()");

    //--------------------------- Find RECORD 1 ---------------------------
    // DmFindRecordByID finds record by uniqId and returns index which is
    // used later to get the actual record
    err = DmFindRecordByID(dbRef,uniqId,&index);

    if (err != dmErrUniqueIDNotFound) {
       // READ
       debugPuts("   mainGameRec1 record IS present");
       recHdl = DmGetRecord(dbRef,index);
       ptr    = MemHandleLock(recHdl);

       MemMove(&mainGameRec1,ptr,sizeof(mainGameRec1));       
       MemPtrUnlock(ptr);

       // Restore vars stuffed into the record... 
       gt                   = mainGameRec1.gt;
       appState             = mainGameRec1.appState;
       nagTimerVal          = mainGameRec1.nagTimerVal;
       numTimesRun          = mainGameRec1.numTimesRun;
       installTimGetSeconds = mainGameRec1.installTimGetSeconds;
       tenDayAdditionOn     = mainGameRec1.tenDayAdditionOn;

       for(i=0;i<4;i++) {
	 pwWord[i]=mainGameRec1.pw[i];
	 //debugPutHex("   recovered pwWord[i]",pwWord[i]);
       }

       //debugPutInt("   time now",(TimGetSeconds()&0x0fff));
       //debugPutInt("   game has been installed sec",
       //              (TimGetSeconds()-mainGameRec1.installTimGetSeconds)&0x0fff);
       debugPutInt("   numTimesRun",numTimesRun); 

    } else {
       // CREATE (blank record)
       debugPuts("   mainGameRec1 record NOT present, creating one");
       recHdl = DmNewRecord(dbRef,&index,sizeof(mainGameRec1)); 
       DmSetRecordInfo(dbRef, index, NULL, &uniqId);  

       // Do not overwrite gt, appState (use default set before)
    }

    DmReleaseRecord(dbRef, index, true);
    DmSetRecordInfo(dbRef, index, NULL, &uniqId);  

    ////debugPutInt("  index",index);
    ////debugPutInt("  volumePref",mainGameRec1.volPref);
    ////debugPutInt("  gt",mainGameRec1.gt);


    //--------------------------- Find RECORD 2 ---------------------------
    uniqId=mainGameRec2UniqId;
    index=1;
    err = DmFindRecordByID(dbRef,uniqId,&index);

    if (err != dmErrUniqueIDNotFound) {

       // READ
       debugPuts("   mainGameRec2 record IS present");
       recHdl = DmGetRecord(dbRef,index);
       ptr    = MemHandleLock(recHdl);

       MemMove(&mainGameRec2,ptr,sizeof(mainGameRec2));       
       MemPtrUnlock(ptr);

       // Restore vars stuffed in record
       lastTimRan            = mainGameRec2.lastTimRan;
       timeModifiedFraudFlag = mainGameRec2.timeModifiedFraudFlag;
       numTimesRestarted     = mainGameRec2.numTimesRestarted;
   
       // debug
       debugPutUInt32Hex("   lastTimRan",lastTimRan);
       debugPutInt("   timeModifiedFraudFlag",(Byte)timeModifiedFraudFlag);
       debugPutInt("   numTimesRestarted",numTimesRestarted);

    } else {

       // CREATE (blank record)
       debugPuts("   mainGameRec2 record NOT present, creating one");
       recHdl = DmNewRecord(dbRef,&index,sizeof(mainGameRec2)); 
       DmSetRecordInfo(dbRef, index, NULL, &uniqId);  

    }

    DmReleaseRecord(dbRef, index, true);
    DmSetRecordInfo(dbRef, index, NULL, &uniqId); 

    //--------------------------- Find RECORD 3 ---------------------------
    uniqId=mainGameRec3UniqId;
    index=1;
    err = DmFindRecordByID(dbRef,uniqId,&index);

    if (err != dmErrUniqueIDNotFound) {

       // READ
       debugPuts("   mainGameRec3 record IS present");
       recHdl = DmGetRecord(dbRef,index);
       ptr    = MemHandleLock(recHdl);

       MemMove(&mainGameRec3,ptr,sizeof(mainGameRec3));       
       MemPtrUnlock(ptr);

       // No data to restore.. record 3 is used "in-place"

    } else {

       // CREATE (blank record)
       debugPuts("   mainGameRec3 record NOT present, creating one");
       recHdl = DmNewRecord(dbRef,&index,sizeof(mainGameRec3)); 
       DmSetRecordInfo(dbRef, index, NULL, &uniqId);  

    }

    DmReleaseRecord(dbRef, index, true);
    DmSetRecordInfo(dbRef, index, NULL, &uniqId);  

    //--------------------------- Find RECORD 4 ---------------------------
    uniqId=mainGameRec4UniqId;
    index=1;
    err = DmFindRecordByID(dbRef,uniqId,&index);

    if (err != dmErrUniqueIDNotFound) {

       // READ
       debugPuts("   mainGameRec4 record IS present");
       recHdl = DmGetRecord(dbRef,index);
       ptr    = MemHandleLock(recHdl);

       MemMove(&mainGameRec4,ptr,sizeof(mainGameRec4));       
       MemPtrUnlock(ptr);

       // No data to restore.. record 4 is used "in-place"

       debugPutInt("   mainGameRec4.backgroundGreyShade",
		   mainGameRec4.backgroundGreyShade);
       debugPutInt("   mainGameRec4.greyscaleTheme",
		   mainGameRec4.greyscaleTheme);
    

    } else {
       // CREATE (blank record)
       debugPuts("   mainGameRec4 record NOT present, creating one");
       recHdl = DmNewRecord(dbRef,&index,sizeof(mainGameRec4)); 
       DmSetRecordInfo(dbRef, index, NULL, &uniqId);  
    }

    DmReleaseRecord(dbRef, index, true);
    DmSetRecordInfo(dbRef, index, NULL, &uniqId);  


    debugFuncExit("DbMgrReadOrCreateMainAppRecords()");
    return true;
}


//----------------------------------------------------------------------
//  DbMgrWriteMainAppRecords
//
//  ASSUMES RECORD IS ALREADY PRESENT!
//----------------------------------------------------------------------
void DbMgrWriteMainAppRecords()
{
    UInt16 index=0,err,offset=0,i;
    VoidHand recHdl=NULL;
    VoidPtr ptr;

    debugFuncEnter("DbMgrWriteMainAppRecords()");

    //---------Fill in Rec1------------------------------------------------
    if (gameInfo.score > mainGameRec1.highScore) { 
       mainGameRec1.highScore=gameInfo.score;
    }

    if (gameInfo.round > mainGameRec1.highWave) { 
       mainGameRec1.highWave=gameInfo.round;
    }

    mainGameRec1.gt=gt;
    mainGameRec1.appState = appState;
    mainGameRec1.nagTimerVal = nagTimerVal;

    for(i=0;i<4;i++) { mainGameRec1.pw[i]=pwWord[i]; }

    mainGameRec1.numTimesRun      = numTimesRun;
    mainGameRec1.tenDayAdditionOn = tenDayAdditionOn;

    //debugPutInt("  saved appState",appState);

    // find, lock record mainGameRec1 record
    err = DmFindRecordByID(dbRef,mainGameRec1UniqId,&index);
    ErrFatalDisplayIf( err, "FATAL ERROR: Could not find record that is present");


    recHdl = DmGetRecord(dbRef,index);
    ptr    = MemHandleLock(recHdl);

    // write to the new record
    err = DmWrite(ptr, offset, &mainGameRec1, sizeof(mainGameRec1));
    ErrFatalDisplayIf(err, "FATAL ERROR: Could not write to new record.");

    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);


    //-------Fill in Rec2--------------------------------------------------
    mainGameRec2.lastTimRan            = TimGetSeconds();
    mainGameRec2.timeModifiedFraudFlag = timeModifiedFraudFlag;
    mainGameRec2.numTimesRestarted     = numTimesRestarted;

    // find, lock record mainGameRec2 record
    err = DmFindRecordByID(dbRef,mainGameRec2UniqId,&index);
    ErrFatalDisplayIf( err, "FATAL ERROR: Could not find record that is present");

    recHdl = DmGetRecord(dbRef,index);
    ptr    = MemHandleLock(recHdl);

    // write to the new record
    err = DmWrite(ptr, offset, &mainGameRec2, sizeof(mainGameRec2));
    ErrFatalDisplayIf(err, "FATAL ERROR: Could not write to new record.");

    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);

    //-------Fill in Rec3--------------------------------------------------

    // Record 3 is used "in-place" .. it does not need many writes
    mainGameRec3.astronVersion = ASTRONVERSION;
    
    // find, lock record mainGameRec3 record
    err = DmFindRecordByID(dbRef,mainGameRec3UniqId,&index);
    ErrFatalDisplayIf( err, "FATAL ERROR: Could not find record 3 that is present");

    recHdl = DmGetRecord(dbRef,index);
    ptr    = MemHandleLock(recHdl);

    // write to the new record
    err = DmWrite(ptr, offset, &mainGameRec3, sizeof(mainGameRec3));
    ErrFatalDisplayIf(err, "FATAL ERROR: Could not write to new record.");

    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);

    //-------Fill in Rec4--------------------------------------------------

    // Record 4 is used "in-place" .. it does not need any writes 
    // find, lock record mainGameRec4 record
    err = DmFindRecordByID(dbRef,mainGameRec4UniqId,&index);
    ErrFatalDisplayIf( err, "FATAL ERROR: Could not find record 4 that is present");

    recHdl = DmGetRecord(dbRef,index);
    ptr    = MemHandleLock(recHdl);

    // write to the new record
    err = DmWrite(ptr, offset, &mainGameRec4, sizeof(mainGameRec4));
    ErrFatalDisplayIf(err, "FATAL ERROR: Could not write to new record 4.");

    MemPtrUnlock(ptr);
    DmReleaseRecord(dbRef, index, true);

   debugFuncExit("DbMgrWriteMainAppRecords()");
}


//----------------------------------------------------------------------
// DbMgrDeleteAllRecords()
//
// Deletes all the records in the database by actually deleting the 
// entire database itself, and then calling DbMgrOpenOrCreateGameDb
// to recreate it (seems to be simplest way)
// 
//----------------------------------------------------------------------
Boolean DbMgrDeleteAllRecords()
{
   UInt16 err;
   UInt32 dbId;
   UInt16 cardNo;

   debugFuncEnter("DbMgrDeleteAllRecords()");

   // Get the dbId and cardNo for the delete
   err = DmOpenDatabaseInfo(dbRef,&dbId,0,0,&cardNo,0);
   //ErrFatalDisplayIf( err, "NEAL: problems deleting database");

   // Database must be closed to be deleted
   DmCloseDatabase(dbRef);

   // BUG FIX:
   // Card number was hardcoded to 0.  This causes a fatal error in device
   // (handspring, sony?) where the game is stored on a memory stick which
   // is not writeable).  
   err =  DmDeleteDatabase(cardNo,dbId);
   //ErrFatalDisplayIf( err, "NEAL: problems deleting database");

   // Now create an empty database
   debugFuncExit("DbMgrDeleteAllRecords()");
   return DbMgrOpenOrCreateGameDb();

}


//----------------------------------------------------------------------
// DbMgrCloseGameDb()
//
// 
//----------------------------------------------------------------------
void DbMgrCloseGameDb()
{
   DmCloseDatabase(dbRef);   
}



//
//  DATABASE.H
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

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <PalmOS.h>
#include <PalmTypes.h>
#include <PalmCompatibility.h>
#include "starter.h"    // for SEC macros

//----------------------------------------------------------------------
// Vars
//----------------------------------------------------------------------

// Database 
DmOpenRef              dbRef;
mainGameRecordStructV1 mainGameRec1;
mainGameRecordStructV2 mainGameRec2;
UInt                   dbVer;
Boolean                allRecordMustBeRewritten;  // written to in misc.c


Boolean DbMgrOpenOrCreateGameDb() SEC4;
Boolean DbMgrReadMainAppRecords() SEC4;
void    DbMgrWriteMainAppRecords() SEC4;
Boolean DbMgrDeleteAllRecords() SEC4;
void    DbMgrCloseGameDb() SEC4;

#endif

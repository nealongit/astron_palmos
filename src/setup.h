//
//  BITMAP SETUP ROUTINES
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

#ifndef _SETUP_H_
#define _SETUP_H_

#include "starter.h" // for SECx macros

//===================================================================
//     PROCVARs     (to handle different screen depths)
//===================================================================
void (*RegisterGameGraphics)();
void (*SetGreyShades)();

//===================================================================
//    APIs
//===================================================================

// Procvar targets
void Register1BitGameBitmaps() SEC3; 
void RegisterMultiBitGameBitmaps() SEC3; 

void Set2BitGreyShades() SEC4;      // 2-bit greyscale

void SetStandard4BitColors() SEC4;  // 4-bit color palatte setup
void Set4BitGreyShades() SEC4;      // 4-bit greyscale setup



#endif

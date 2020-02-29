//
//  GRAPHICS ROUTINES
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

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <DebugMgr.h>

#include "sprite.h"
#include "blast.h"



//===================================================================
//  Vars -- window handles
//===================================================================
WinHandle scrnBuffer;
WinHandle backBuffer;


//===================================================================
//                         Structures
//===================================================================

#define NUM_BMPSLIDES    8
#define NUM_MASKSLIDES   8

typedef struct 
{
   UInt16   h;       // DO NOT MOVE!!! -- ASM proc accesses directly
   UInt16   w;       // DO NOT MOVE!!! -- ASM proc accesses directly

   // 4 params used for tighter collision detection
   UInt16   x1;      // DO NOT MOVE!!! -- ASM proc accesses directly
   UInt16   y1;      // DO NOT MOVE!!! -- ASM proc accesses directly
   UInt16   x2;      // DO NOT MOVE!!! -- ASM proc accesses directly
   UInt16   y2;      // DO NOT MOVE!!! -- ASM proc accesses directly

   // 2 params used for collision detection
   UInt16   centerX;  // DO NOT MOVE!!! -- ASM proc accesses directly
   UInt16   centerY;  // DO NOT MOVE!!! -- ASM proc accesses directly

   UInt16  * bmp[NUM_BMPSLIDES];   // Bitmap slides
   UInt16  * mask[NUM_MASKSLIDES];  // Bitmap mask slides (used by 4-bit color only)
   
} bmpStruct;



//===================================================================
//                          Vars
//===================================================================


// Control whether the game world may be larger than the visible screen
// causing the x and y coordinates to have to be checked
Boolean  mustCheckXCoor;
Boolean  mustCheckYcoor;

// Externel vars controlled by game.  These are used if either (or both)
// of the above set to true.  They control what part of the game world
// is viewed on the screen
Int      topLeftXCoor;   
Int      topLeftYcoor;


bmpStruct  bmpTableNew[MAXNUMBITMAPS];   // wasteful memory wise


#define maxDigitsForNumToStr 8

// ROM version -- move
UInt32 romVersion;


// Procvar
void (*GraphicsRegComplexBmp)( 
   UInt16   bmpNum,
   UInt16   h,          
   UInt16   w,            // in pixels  (NOT words like GraphicsRegBmp)
   char   * bmp,   
   UInt16   x1,
   UInt16   y1,
   UInt16   x2,
   UInt16   y2,
   UInt16   centerX,
   UInt16   centerY
   );


  

//-----------------------------------------------------------------------
//                       Sound Manager
//-----------------------------------------------------------------------
typedef enum
{
  noSound=0,
  bugShoots,
  bugExplosion,
  bugDive,
  userShoots,         //4
  layedAMine,
  tracBeam,
  doubleShotAwarded,
  extraShipAwarded,
  newRound,
  playerReintroduced,
  userExplosion         // 11
} soundType;

typedef struct
{
  Int freq;
  Int length;
  Int vol;
  Int numTicksLeft;
} soundInfoStruct;

typedef struct
{
  UInt16       vol;
  soundType    thisSound;
  Int          numTicksLeft;
} SoundMgrStruct;


SoundMgrStruct SoundMgr;



//===================================================================
//     PROCVARs     (to handle different screen depths)
//===================================================================
void (*GraphicsClrBackBuffer)();
void (*GraphicsDrawAllSprites)();

void (*TextMgrDisplayTitleBar)();


//===================================================================
//                        External APIs
//===================================================================

void GraphicsInit( 
  Int     depth,
  Int     w, 
  Int     h, 
  Boolean checkX,
  Boolean checkY
) SEC2;

void GraphicsClrTitleBar() SEC3;
void Graphics1BitClrBackBuffer() SEC3;
void Graphics4BitClrBackBuffer();
void GraphicsRepaint() SEC3;
inline void GraphicsRepaintNative() SEC3;
void GraphicsDrawString(CharPtr,Word,SWord,SWord) SEC3;
void Graphics1BitDrawAllSprites();
void Graphics4BitASMDrawAllSprites();

void GraphicsRegBmp( 
   UInt16   s,
   UInt16   h,
   UInt16   w,
   UInt16 * bmp,
   UInt16   xOffset,
   UInt16   yOffset,
   UInt16   realH,
   UInt16   realW,
   UInt16   centerX,
   UInt16   centerY 
) SEC2;


void GraphicsReg4BitColorBmp( 
   UInt16   num,
   UInt16   h,
   UInt16   w,
   char   * bmp,
   UInt16   x1,
   UInt16   y1,
   UInt16   x2,
   UInt16   y2,
   UInt16   centerX,
   UInt16   centerY
) SEC3;




void GraphicsFreeMemory() SEC3;
void GraphicsStopDoCleanup() SEC3;

#define Graphics1BitCalcSpritePosMACRO(x,y,bytesPerYRow,wordIdx,frame) \
wordIdx=(bytesPerYRow *y) + (x>>4); frame=(x & 15); frame=frame>>1;


// Added for color...
//void  GraphicsReadPalette();
void  GraphicsPaletteIsDone();
UInt8 GraphicsGetNextAvailPaletteIndex();
UInt8 GraphicsRegisterColor(
   UInt8 r,
   UInt8 g,
   UInt8 b,
   char bmpChar
);
void GraphicsRegisterSameColor(
   char prevColorChar,
   char newColorChar
);
void GraphicsSetTextAndOtherColors(
   UInt32 backgroundColor,
   UInt32 textColor,
   UInt32 titleBarColor
);


UInt8 GraphicsRegisterGrey(
   UInt8   shade,
   UInt8   bmpCharVal
);


void GraphicsHdlNewForm(UInt32 formId);


// Added for 2-bit greyscale..
void Graphics2BitASMDrawAllSprites();

void GraphicsReg2BitBmp( 
   UInt16   num,
   UInt16   h,
   UInt16   w,
   char   * bmp,
   UInt16   x1,
   UInt16   y1,
   UInt16   x2,
   UInt16   y2,
   UInt16   centerX,
   UInt16   centerY
) SEC3;

void TextMgrDisplay2BitTitleBar() SEC2;



//===================================================================
//                        Internel APIs
//===================================================================

//void GraphicsSetScreenAndPosVars(Int) SEC1;

void Graphics1BitDraw1Sprite();

void GraphicsCreateShiftedBmp(
   UInt16  * prev,
   UInt16  * now,
   UInt16    h,
   UInt16    w,
   Byte      numShifts,
   Byte      fillBit
) SEC1;


//-----------------------------------------------------------------------
//                          Text Manager Methods
//-----------------------------------------------------------------------

typedef struct
{
   Boolean   displayFlag;
   char      str[40];         // strings 39 in length
   Int       len;             // string length
   FontID    fontId;
   Int       posX;
   Int       posY;
} TextMgrStrStruct;

typedef struct
{
  Int     numShips;      // UNUSED
  Int     score;         // UNUSED
  Boolean needsRedraw;
} TextMgrTitleBarStruct;


// Must put these in header file so multigen does not barf
#define TEXTDISPLINES 10
TextMgrStrStruct      textMgrStr[TEXTDISPLINES]; 
TextMgrTitleBarStruct textMgrTitleBar;  // junk values


void TextMgrAddCenteredText(
  char * text,
  Int    n,
  FontID fontId,
  Int    y,
  Boolean center
) SEC2;

void TextMgrInit(Byte depth) SEC2;
void TextMgrClr()  SEC2;
void TextMgrCreateRoundBanner() SEC2;
void TextMgrNumToStr1Bit (
   UInt32   num,
   UInt16   x,
   UInt16   y
) SEC2;

void TextMgrNumToStr2Bit (
   UInt32   num,
   UInt16   x,
   UInt16   y
) SEC2;

void TextMgrNumToStr4Bit (
   UInt32   num,
   UInt16   x,
   UInt16   y
) SEC2;


void TextMgrDisplayText() SEC2;
void TextMgrDisplayGameText() SEC2;
void TextMgrDisplayDebugText() SEC2;

void TextMgrDisplay1BitTitleBar() SEC2;
void TextMgrDisplay2BitTitleBar() SEC2;
void TextMgrDisplay4BitTitleBar() SEC2;

//-----------------------------------------------------------------------
//                          Sound Manager Methods
//-----------------------------------------------------------------------

void SoundMgrSetSoundPreferences(mainGameRecordStructV1 * pref) SEC3;
void SoundMgrPlaySounds() SEC3;
void SoundMgrThisSound(soundType sound) SEC3;

//-----------------------------------------------------------------------
//                         ASM vars
//-----------------------------------------------------------------------
// x,y ASM inputs for bitmap 
unsigned int ASM_shipx;
unsigned int ASM_shipy;
UInt16    *  ASM_thisBmp;
bmpStruct *  ASM_thisBug; 

// more ASM inputs 
unsigned int ASM_bitsPerPixel;
unsigned int ASM_coarseXPosLSRs;
UInt16       ASM_fineXPosMask;
unsigned int ASM_wordsPerYRow;

// ptr to video memory for back buffer
UInt16 * ASM_vm      ;  // back
UInt16 * ASM_vmFront ;  // front

// Vars returned for asmCalcPos 
//unsigned int ASM_wordPos;
UInt32 ASM_wordPos;
unsigned int ASM_bmpIndex;












#endif


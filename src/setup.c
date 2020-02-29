//
//  SETUP 
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

// *** THIS MODULE IS FOR 1-BIT GRAPHICS ONLY ***

#include "setup.h"
#include "graphics.h" 
#include "debug.h"
#include "defs.h"
#include "starter.h" // for ScreenDepth


//-------------------------------------------------------------------------
//  Sprite bitmaps
//-------------------------------------------------------------------------

UInt16 bugBullet[]  = {0x6000,0xf000,0xf000,0x6000};     // fat bullet

UInt16 userBullet[] = {0x6000,0x6000,0x6000,0x6000};  // slim bullet

UInt16 userShip[]   = {0x0000,0x0200,0x0700,0x0700,0x0D80,0x0d80,
                       0x0F80,0x1FC0,0x1FC0,0x7ff0,0x3AE0,0x18C0};

// 2 shot ship
UInt16 userShip2[]   = {0x0000,0x0200,0x0700,0x0700,0x0D80,0x0d80,
                       0x4f90,0x5fd0,0x1FC0,0x7ff0,0x3AE0,0x18C0};

UInt16 explos[]     = {0x0000,0x0100,0x0920,0x0540,0x0000,0x0c60,
                       0x0000,0x0540,0x0920,0x0100,0x0000,0x0000};

UInt16 tracBeamB[]  = {0x0000,0x0010,0x0000,0x0028,0x0010,0x0044,
                       0x0038,0x0082,0x007c};

UInt16 mineB[]      = {0x0000,0x0000,0x0100,0x0380,0x0ee0,0x0380,0x0100};

UInt16 numShipsB[]  = {0x0c00,0x0c00,0x1e00,0x1e00,0x3f00,0x3f00};

UInt16 waveSymbolB[] = {0x3f00,0x3f00,0x3500,0x2b00,0x3f00,0x3f00};

UInt16 twoShotAwardB[] = {0x0E00,0x1F00,0x1300,0x0600,0x0C00,0x1F00,0x1F00};

UInt16 barrierB[] = {0x0000,0x381c,0x1ff8,0x03c0};

UInt16 thrust1B[] = {0x0200,0x0500,0x0000,0x0000,0x0000,0x0000};
UInt16 thrust2B[] = {0x0200,0x0500,0x0000,0x0200,0x0500,0x0000};


UInt16 inForm1B[] = {0,0,0x1000,0x3800,0x1000};
UInt16 inForm2B[] = {0,0,0x1000,0x2800,0x1000};
UInt16 inForm3B[] = {0x1000,0x1000,0,0xc600,0,0x1000,0x1000};


//-------------------------------------------------------------------------
//  Bug bitmaps
//-------------------------------------------------------------------------

UInt16 motherShipB[] = {0x0000,0x366c,0x1ff8,0x1ff8,0x07e0,0x0180,
                   0x03c0,0x03c0,0x0240,0x03c0,0x0180,0x0000};

UInt16 fullMotherShipB[] = {0x0000,0x3ffC,0x1818,0x1818,0x0c30,0x0660,0x03c0,
                            0x03C0,0x0240,0x03C0,0x0180,0x0000,0x0000,0x0000};

UInt16 bug1B[] = {0x0000,0x0000,0x3c78,0x3ff8,0x2fe8,0x07c0,
                   0x0380,0x0ee0,0x06c0,0x0380,0x0000};

UInt16 bug1B2[] = {0x0000,0x0000,0x36d8,0x3ff8,0x2fe8,0x07c0,
                   0x0380,0x0ee0,0x06c0,0x0380,0x0000}; 

UInt16 sweepBugB[] = {0x0000,0x0c30,0x1818,0x399c,0x2ff4,0x27e4,
                      0x03c0,0x0ff0,0x0660,0x03c0};

UInt16 sweepBugB2[] = {0x0000,0x0000,0x0660,0x0C30,0x1998,0x0FF0,0x07E0,
                       0x03C0,0x07e0,0x0660,0x03C0,0x0000};


UInt16 dartBugB[] = {0x0000,0x0540,0x1bb0,0x1ff0,0x0fe0,0x07c0,
                  0x0380,0x0280,0x0380,0x0100};

UInt16 mineLayerB[] = {0x07c0,0x0380,0x1930,0x1ff0,0x0fe0,0x07C0,
                       0x0380,0x07c0,0x06C0,0x06c0,0x0380};

UInt16 bigOafB[] = {0x36d8,0x0920,0x0fe0,0x07c0,0x0380,
                    0x0920,0x0fe0,0x06c0,0x0380,0x0000,0x0000};





//-------------------------------------------------------------------------
//  Number bitmaps
//-------------------------------------------------------------------------

UInt16 dig0Bmp[] = {0x6000,0x9000,0x9000,0x9000,0x9000,0x9000,0x6000};
UInt16 dig1Bmp[] = {0x2000,0x6000,0x2000,0x2000,0x2000,0x2000,0x2000};
UInt16 dig2Bmp[] = {0x6000,0x9000,0x1000,0x2000,0x4000,0x8000,0xf000};
UInt16 dig3Bmp[] = {0x6000,0x9000,0x1000,0x2000,0x1000,0x9000,0x6000};
UInt16 dig4Bmp[] = {0x1000,0x3000,0x5000,0x9000,0xf000,0x1000,0x1000};
UInt16 dig5Bmp[] = {0xf000,0x8000,0x8000,0xe000,0x1000,0x1000,0xe000};
UInt16 dig6Bmp[] = {0x2000,0x4000,0x8000,0xe000,0x9000,0x9000,0x6000};
UInt16 dig7Bmp[] = {0xf000,0x1000,0x2000,0x2000,0x4000,0x4000,0x4000};
UInt16 dig8Bmp[] = {0x6000,0x9000,0x9000,0x6000,0x9000,0x9000,0x6000};
UInt16 dig9Bmp[] = {0x6000,0x9000,0x9000,0x7000,0x1000,0x2000,0x4000};


//-------------------------------------------------------------------------
//  Register1BitGameBitmaps  
//-------------------------------------------------------------------------
void Register1BitGameBitmaps()
{

  debugFuncEnter("Register1BitGameBitmaps()");

  // number digit  bitmaps
  GraphicsRegBmp(0,7,1,dig0Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(1,7,1,dig1Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(2,7,1,dig2Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(3,7,1,dig3Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(4,7,1,dig4Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(5,7,1,dig5Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(6,7,1,dig6Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(7,7,1,dig7Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(8,7,1,dig8Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(9,7,1,dig9Bmp,0,0,0,0,0,0);
  
  // game bitmaps 
  //             bmpNum,         h,w,*array,x1,y1,x2,y2,centerX,centerY
  GraphicsRegBmp(userShipBmp    ,12,1,userShip     ,2,2,10,11,6,8); 
  GraphicsRegBmp(fatBulletBmp   ,4,1,bugBullet     ,0,0,2,4,2,0);
  GraphicsRegBmp(slimBulletBmp  ,4,1,userBullet    ,1,0,2,4,2,0);
  GraphicsRegBmp(explosBmp      ,12,1,explos       ,0,0,16,16,0,0);
  GraphicsRegBmp(twoShotAwardBmp,7,1,twoShotAwardB ,3,1,7,7,5,6);
  GraphicsRegBmp(barrierBmp     ,4,1,barrierB      ,0,0,16,8,0,0);   // y2=8 to catch user bullet
  // starBmp
  GraphicsRegBmp(bug1_2,         11,1,bug1B2             ,2,2,12,9,0,0);
  GraphicsRegBmp(sweepBug2,      12,1,sweepBugB2         ,2,2,13,10,0,0);
  GraphicsRegBmp(bug1,           11,1,bug1B              ,2,2,12,9,0,0);
  GraphicsRegBmp(sweepBug,       11,1,sweepBugB          ,2,2,13,10,0,0);  
  GraphicsRegBmp(mothership,     12,1,motherShipB        ,1,2,12,10,0,0);
  GraphicsRegBmp(dartBug,        10,1,dartBugB           ,3,2,11,10,0,0);        
  GraphicsRegBmp(mineLayer,      11,1,mineLayerB         ,4,0,11,10,0,0);
  GraphicsRegBmp(fullMotherShip, 12,1,fullMotherShipB,1,2,12,10,0,0);
  GraphicsRegBmp(bigOafBmp,      11,1,bigOafB            ,4,1,13,10,0,0);  // NOT USED
  GraphicsRegBmp(userShip2Bmp   ,12,1,userShip2    ,2,2,10,11,6,8); 
  GraphicsRegBmp(waveSymbolBmp  ,6,1,waveSymbolB   ,0,0,16,16,0,0);
  GraphicsRegBmp(mineBmp        ,7,1,mineB         ,5,2,11,8,7,4);    // y2=8 to catch user bullet 
  GraphicsRegBmp(numShipsBmp    ,6,1,numShipsB     ,0,0,16,16,0,0);
  GraphicsRegBmp(tracBeamBmp    ,9,1,tracBeamB     ,10,1,12,8,11,6);  // made purposedly narrow
  GraphicsRegBmp(thrust1,6,1,thrust1B,5,0,7,2,6,1);
  GraphicsRegBmp(thrust2,6,1,thrust2B,5,0,7,2,6,1);
  GraphicsRegBmp(inFormBmp1     ,5,1,inForm1B,0,0,0,0,0,0); // no collision while in-from seqn
  GraphicsRegBmp(inFormBmp2     ,5,1,inForm2B,0,0,0,0,0,0);
  GraphicsRegBmp(inFormBmp3     ,7,1,inForm3B,0,0,0,0,0,0);
  // thrust3
  // thrust4
  // dotBulletBmp
  // star2Bmp

  // temporary...
  GraphicsRegBmp(thrust3,7,1,dig0Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(thrust4,7,1,dig0Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(dotBulletBmp,7,1,dig0Bmp,0,0,0,0,0,0);
  GraphicsRegBmp(star2Bmp,7,1,dig0Bmp,0,0,0,0,0,0);



  // Set variables for 1-bit depth..
  ThrustSpriteXOffset=0;
  TracBeamXOffset=0;
  UserShipXMax=142;
  DigitSpacing=4; 
  CapturedByTracBeamXOffset=2;

  debugFuncExit("Register1BitGameBitmaps()");

}



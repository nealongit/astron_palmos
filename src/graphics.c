// 
// GRAPHICS
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

//-------------------------------------------------------------------------
//  SAVE-ABLES: none
//-------------------------------------------------------------------------

// Palm OS calls, types  
#include <PalmOS.h>
#include <UIPublic.h>
#include <PalmTypes.h>
#include <Font.h>
#include <Preferences.h>

//#define ALLOW_ACCESS_TO_INTERNALS_OF_WINDOWS required here..
#include <Window.h>

// Added for color functionality
#include <UIColor.h>

// Added for WinCopyRectangle
#include <Rect.h>

    
// My header files
#include "starter.h" 
#include "graphics.h"
#include "debug.h"
#include "misc.h"

#include "setup.h"  // for RegisterGameGraphics -- remove eventually
 
#include "gameman.h"  // remove eventually




//-------------------------------------------------------------------------
//  Internel Variables
//-------------------------------------------------------------------------
Boolean  mustCheckXCoor = false;
Boolean  mustCheckYcoor = false;

Int      topLeftXCoor = 0;   
Int      topLeftYCoor = 0;

Int      titleBarHeight = 12;
Int      screenHeight   = 148;  // titleBarHeight + screenHeight = 160
Int      screenWidth    = 160;

Int      totalNumTitleBytes=0;       // num bytes for title bar
Int      totalNumGameAreaBytes=0;    // num bytes for game
Int      totalNumWholeScreenBytes=0; // sum of above 2 nums

// Depth set is different from depth get.  Depth get is a bitmap, but depth
// set is an integer (i.e 4-bit support=8, 4-bit set=4).
//DWord   newDepth    = 1;         // 1-bit depth
//Boolean enableColor = false;     // color flag


//-------------------------------------------------------------------------
//  ASM global Variables
//-------------------------------------------------------------------------

// x,y ASM inputs for bitmap 
//unsigned int ASM_shipx =0;
//unsigned int ASM_shipy =50;
UInt16    *  ASM_thisBmp;
bmpStruct *  ASM_thisBug; 

// more ASM inputs 
//UInt16       ASM_fineXPosMask;
unsigned int ASM_bytesPerYRow;
unsigned int Reg_wordsPerYRow;

// ptr to video memory for back buffer .... vm = video memory
UInt16 * ASM_vm      =0;  // back
UInt16 * ASM_vmEnd   =0;  // bottom of back
UInt16 * ASM_vmFront =0;  // front
UInt16 graphicsNumCopyBlocks=0;
UInt16 graphicsNumCopyBlocks2=1;
UInt16 graphicsNumClrBlocks=0;

// Vars returned for asmCalcPos 
//unsigned int ASM_wordPos=0; 
UInt32 ASM_wordPos=0;
unsigned int ASM_bmpIndex=0;

//-------------------------------------------------------------------------
//  External Variables
//-------------------------------------------------------------------------

WinHandle scrnBuffer; 
WinHandle backBuffer;


//-------------------------------------------------------------------------
//  SPRITE ENGINE
//-------------------------------------------------------------------------

bmpStruct  bmpTableNew[MAXNUMBITMAPS];   // wasteful memory wise

spriteCategoryStruct   spriteList[ spriteClassMax ];

 
// debug
//Int debugInt;
//Boolean debugFlag;
 
 
//--------------------------------------------------------------------
//  Text Manager
//-------------------------------------------------------------------- 

TextMgrStrStruct      textMgrStr[TEXTDISPLINES]; 
TextMgrTitleBarStruct textMgrTitleBar = {1,5555,true};  // junk values


//--------------------------------------------------------------------
//  Color
//-------------------------------------------------------------------- 

Boolean      paletteEntryUsed[256];
UInt16       nextPaletteEntry=0;


// Palette consisting of system colors + game colors.  Registered with window
// indexed by "index"
RGBColorType gamePalette[256];

#define   maxGameColors 26
UInt8     firstColorCharIndex=97;  // 'a'
#define   lastColorCharIndex 122  // 'z'

// This structure maps char's in the bitmaps definitions to their palette index
UInt8        paletteIndexMap[maxGameColors];

UInt32 graphicsColorBackground4Bit[10];
UInt32 graphicsColorTitleBarBack4Bit=0;
UInt16 textColorForeground4Bit;
UInt16 textColorBackground4Bit;


RectangleType rect;  


//--------------------------------------------------------------------
//  Sound Manager
//-------------------------------------------------------------------- 

// freq in Hz, length in msec, vol, numTicks
soundInfoStruct soundCatelog[] = {
  { 0,0,0,0 },       // no sound
  { 0,0,0,0 },     // bug shot
  { 480,80,8,1 },       // bug explos
  { 0,0,0,0 },
  { 680,32,64,1 }, // user shoots
  { 600,20,64,1 }, // layed a mine
  { 290,40,64,10 }, // trac beam ... num Ticks must sync up with speed
  { 1200,30,64,8 }, // 2 shot award
  { 1200,30,64,16 },   // extra ship
  { 1800,15,32,2 }, // new round
  { 0,0,0,0 },
  { 208,120,64,3 }    // user explodes
};

SoundMgrStruct SoundMgr = { true,noSound,0 };

//--------------------------------------------------------------------
//  Procvar
//-------------------------------------------------------------------- 
void (*GraphicsClrBackBuffer)();

//{
//}

//-------------------------------------------------------------------------
//  GraphicsInit
//
//  Handles  1-bit, 2-bit grey, 4-bit grey, 4-bit color
//
//  1. Create the back buffer
//  2. Set PROCVARs
//-------------------------------------------------------------------------
void GraphicsInit( Int     depth,
                   Int     w,  // unused 
                   Int     h,  // unused 
                   Boolean checkX,
                   Boolean checkY)
{
  Word error;
  Err err;

  Boolean   setColorOn=false;
  UInt32    setDepth=1;


  debugFuncEnter("GraphicsInit()");
  debugPutInt("   depth",depth);
  

  // w,h not used correctly now...
  mustCheckXCoor=checkX;
  mustCheckYcoor=checkY;


  // Store the handle to the visible screen.  For PalmOS 3.5 and higher call
  // the newer BmpGetBits function (see PalmOS Ref. under "WindowType")
  scrnBuffer  = WinGetDrawWindow();

  ASM_vmFront = (romVersion < PALMOS35 ) ? scrnBuffer->displayAddrV20 :
                                           BmpGetBits(WinGetBitmap(scrnBuffer));
  
  
  switch (depth) {
     
     case oneBitDepth:

	//--------------------
	//  1-bit b/w depth
	//--------------------
	
	// procvar's
	GraphicsClrBackBuffer  = Graphics1BitClrBackBuffer;
	GraphicsDrawAllSprites = Graphics1BitDrawAllSprites;
	RegisterGameGraphics   = Register1BitGameBitmaps;
	

	titleBarHeight         = 12;
	screenHeight           = 148;

	graphicsNumCopyBlocks  = 13;  // 0..13 (14 240-byte blocks)
	graphicsNumCopyBlocks2 = 1;   // 1     (1  80-byte blocks)
	graphicsNumClrBlocks   = 14;  // 0..14 (15 240-byte blocks)


	graphicsNumCopyBlocks  = 13;  // = 20*148 / 240 (each "copy block" = 240 bytes)
	
	// Set the screen depth.  Code from Ref 1, p.398
	setDepth=1;
	err=ScrDisplayMode(scrDisplayModeSet,NULL,NULL,&setDepth,NULL); 
	
	break; 


     case greyTwoBitDepth:

	//--------------------
	//  2-bit grey depth
	//--------------------

	GraphicsClrBackBuffer  = Graphics4BitClrBackBuffer; // Fine for 2-bit as well
	GraphicsDrawAllSprites = Graphics2BitASMDrawAllSprites;
	RegisterGameGraphics   = RegisterMultiBitGameBitmaps; 
	SetGreyShades          = Set2BitGreyShades;        
	GraphicsRegComplexBmp  = GraphicsReg2BitBmp;  
	
	
	titleBarHeight         = 12;
	screenHeight           = 148;

	graphicsNumCopyBlocks  = 24;  // 0..25 (26 240-byte blocks)
	graphicsNumCopyBlocks2 = 4;   // 1..2  (2  80-byte blocks)
	graphicsNumClrBlocks   = 26;  // 0..26 (27 240-byte blocks)
	
	// Set the screen depth.  Code from Ref 1, p.398
	setDepth=2;
	err=ScrDisplayMode(scrDisplayModeSet,NULL,NULL,&setDepth,NULL); 

	// Set default greyshade mapping
	Set2BitGreyShades();  
	
	break; 


     case greyFourBitDepth:

	got_here;
	
	//--------------------
	//  4-bit grey depth   
	//--------------------
	
	// Nil out palette-used table
	MemSet(paletteEntryUsed,256,false); 
	
	// procvars
	GraphicsClrBackBuffer  = Graphics4BitClrBackBuffer;
	GraphicsDrawAllSprites = Graphics4BitASMDrawAllSprites;
	RegisterGameGraphics   = RegisterMultiBitGameBitmaps;   
	SetGreyShades          = Set4BitGreyShades;         
	GraphicsRegComplexBmp  = GraphicsReg4BitColorBmp;  

	titleBarHeight         = 12;
	screenHeight           = 148;

	graphicsNumCopyBlocks  = 52;  // 0..52 (53 240-byte blocks)
	graphicsNumCopyBlocks2 = 1;   // 1     (1  80-byte blocks)
	graphicsNumClrBlocks   = 53;  // 0..53 (54 240-byte blocks)
	
	// According to manual, the last parameter enableColor should be
	// a BooleanPtr. However using a BooleanPtr does not set the color
	// device to greyscale, must pass boolean (NULL) directly.
	setDepth=4;
	err=ScrDisplayMode(scrDisplayModeSet,NULL,NULL,&setDepth,NULL);

	// Set default greyshade mapping
	Set4BitGreyShades();  

	break;


     case colorFourBitDepth:

	got_here;
	
	//--------------------
	//  4-bit color depth   
	//--------------------
	
	// Nil out palette-used table
	MemSet(paletteEntryUsed,256,false); 
	
	// procvars
	GraphicsClrBackBuffer  = Graphics4BitClrBackBuffer;
	GraphicsDrawAllSprites = Graphics4BitASMDrawAllSprites;
	RegisterGameGraphics   = RegisterMultiBitGameBitmaps;
	GraphicsRegComplexBmp  = GraphicsReg4BitColorBmp;  

	// vars
	titleBarHeight         = 11; 
	screenHeight           = 149;

	graphicsNumCopyBlocks  = 52;  // 0..52 (53 240-byte blocks)
	graphicsNumCopyBlocks2 = 1;   // 1     (1  80-byte blocks)
	graphicsNumClrBlocks   = 53;  // 0..53 (54 240-byte blocks)
	
	// Set the screen depth.  Code from Ref 1, p.398
	setColorOn = true;  
	setDepth   = 4;
	err=ScrDisplayMode(scrDisplayModeSet,NULL,NULL,&setDepth,&setColorOn);
	
	// Set color mapping
	SetStandard4BitColors();  
	break;


     default:
	ERROR_ASSERT("Unknown screen depth");
	break;
		
  }



  // vars
  ASM_bytesPerYRow         = 160/(8/setDepth);
  Reg_wordsPerYRow         = ASM_bytesPerYRow/2;
  totalNumGameAreaBytes    = screenHeight   * ASM_bytesPerYRow; 
  totalNumTitleBytes       = titleBarHeight * ASM_bytesPerYRow;
  totalNumWholeScreenBytes = totalNumGameAreaBytes + totalNumTitleBytes;


  // Create a full-size back buffer (Ref1,p.402).  Must be done after the bit 
  // depth is set 
  backBuffer = WinCreateOffscreenWindow(160, 240, screenFormat, &error); 
  //backBuffer = WinCreateOffscreenWindow(160, 160, screenFormat, &error); 
  ErrFatalDisplayIf(error, "Insufficient memory");

  ASM_vm = (romVersion < PALMOS35 ) ? backBuffer->displayAddrV20 :
                                      BmpGetBits(WinGetBitmap(backBuffer));

  ASM_vmEnd = ASM_vm + totalNumWholeScreenBytes;

  // Define the GraphicsRepaintNative copy rectangle. coor=0=0, h=w=160
  RctSetRectangle(&rect,0,0,160,160);




  //debug
  debugPutInt("ASM_bytesPerYRow",ASM_bytesPerYRow);
  debugPutInt("totalNumTitleBytes",totalNumTitleBytes);
  debugPutInt("totalNumGameAreaBytes",totalNumGameAreaBytes);
  debugPutUInt32Hex("  ASM_vm is from ",(UInt32)ASM_vm);
  debugPutUInt32Hex("              to ",(UInt32)ASM_vmEnd);
 

  debugFuncExit("GraphicsInit()");
}


//-------------------------------------------------------------------------
//  GraphicsRegisterColor
//-------------------------------------------------------------------------
UInt8 GraphicsRegisterColor(
   UInt8 r,
   UInt8 g,
   UInt8 b,
   char bmpChar
)
{

   UInt8 pIndex=0;

   debugFuncEnter("GraphicsRegisterColor");
   
   // Ensure ascii character is within range
   if ( ((UInt8)bmpChar < firstColorCharIndex) ||
        ((UInt8)bmpChar > lastColorCharIndex))
   {
      ERROR_ASSERT("Hey moron, bmpChar is out of range");
   }

   // Find next available index in the palette
   pIndex = GraphicsGetNextAvailPaletteIndex();

   //kludge for 4 bits
   // pIndex=pIndex & 0xf;

   debugPutInt("   bmpChar",(UInt8)bmpChar);
   debugPutInt("   pIndex",pIndex);

   // Put color in palette
   gamePalette[pIndex].r =r;  
   gamePalette[pIndex].g =g;  
   gamePalette[pIndex].b =b;  
   paletteEntryUsed[pIndex] = true; 

   // Store mapping, and prevent index from being reused.
   paletteIndexMap[(UInt8)bmpChar - firstColorCharIndex]=pIndex; // remember mapping
   
   // not many users need rc
   debugFuncExit("GraphicsRegisterColor");
   return pIndex;
  
}

//-------------------------------------------------------------------------
//  GraphicsRegisterGrey
//
//  Greyscale differs from color in that the screen bits maps directly
//  to a known shade of grey -- the bits do NOT form a lookup into a
//  registered palette.
//-------------------------------------------------------------------------
UInt8 GraphicsRegisterGrey(
   UInt8   shade,
   UInt8   bmpCharVal
)
{

   UInt8 pIndex=0;

   debugFuncEnter("GraphicsRegisterGrey");
   
   // Ensure ascii character is within range
   if ( (bmpCharVal < firstColorCharIndex) ||
        (bmpCharVal > lastColorCharIndex))
   {
      ERROR_ASSERT("Hey moron, bmpCharVal is out of range");
      return 0;
   }

   // Ensure shade is a 4-bit number
   if (shade > 0x0f)
   {
      ERROR_ASSERT("Hey moron, shade is out of range");
      return 0;
   }

   debugPutInt("   bmpCharVal",bmpCharVal);
   debugPutInt("   shade",shade);

   // Store mapping (ascii char to bits)
   paletteIndexMap[bmpCharVal - firstColorCharIndex]=shade;
   
   // not many users need rc
   debugFuncExit("GraphicsRegisterGrey");
   return pIndex;
  
}


//-------------------------------------------------------------------------
// GraphicsRegisterSameColor
//
// Allow 2 (or more) color char's to be the same index (therefore same 
// color)
//-------------------------------------------------------------------------
void GraphicsRegisterSameColor(
			       char prevColorChar,
			       char newColorChar
			       )
{

   debugFuncEnter("GraphicsRegisterSameColor");
   

   // Allow 2 characters to point to the same color...
   paletteIndexMap[(UInt8)newColorChar - firstColorCharIndex]= 
   paletteIndexMap[(UInt8)prevColorChar - firstColorCharIndex];

   debugPutInt("   newColorChar",(UInt8)newColorChar);
   debugPutInt("   pIndex",paletteIndexMap[(UInt8)prevColorChar - firstColorCharIndex]);

   debugFuncExit("GraphicsRegisterSameColor");
 
   
}


//-------------------------------------------------------------------------
// GraphicsPaletteIsDone
//-------------------------------------------------------------------------
void GraphicsPaletteIsDone()
{

   Byte i=0,n=0;
   
   debugFuncEnter("GraphicsPaletteIsDone");

   for (i=0;i<16;i++) {
      if (paletteEntryUsed[i]) n++;
   }
   
   debugPutInt("  num color index used",n);


   WinPalette(winPaletteSet,0,16,gamePalette);  // 4bit
   
   debugFuncExit("GraphicsPaletteIsDone");
}

//-------------------------------------------------------------------------
//  GraphicsGetNextAvailPaletteIndex
//-------------------------------------------------------------------------
UInt8 GraphicsGetNextAvailPaletteIndex()
{

   // Find next available index in the palette
   while ((paletteEntryUsed[nextPaletteEntry]) && (nextPaletteEntry!=256) ) 
   {
      nextPaletteEntry++;
   }
   
   if (nextPaletteEntry == 256) {
      ERROR_ASSERT("no entries available");
      nextPaletteEntry=255;
      
   }
   
   return nextPaletteEntry;
}

//-------------------------------------------------------------------------
//  GraphicsDrawAllSprites -- 1 bit
//
//  Loop through all sprites owned by Sprite engine and draw them
//  to the screen
//-------------------------------------------------------------------------
void Graphics1BitDrawAllSprites()
{
   Word c;
   sprite * s;
   Int16 thisIndex;

   Int x=0,y=0,i,h,w;
   UInt32 d2; 
   bmpStruct *  bmpStructPtr=0; 
   UInt32 wordIdx=0;
   UInt16 * bmpPtr=0;
   
   
   // debug
   debugFuncEnter( "Graphics1BitDrawAllSprites()" );
 
   for (c=0;c<spriteClassMax;c++) {
      thisIndex = spriteList[c].firstSprite;

      // Walk the list,let each sprite handle tick
      while ( thisIndex != -1 ) {

         s = allSprites[thisIndex];

	 if (isActive(s)) {

	   // Map from game to screen coordinates X
	   if ( mustCheckXCoor ) {
	     // Adjust the x coordinate and see if it's on the screen
	     x= s->x - topLeftXCoor;

	     if (x<0 || x>screenWidth) { 
                thisIndex=s->nextSprite;
	        debugPutInt( "  offscreen (x), s",s->spriteNum );
                continue; 
             }

	   } else {
	     x=s->x;
	   }


	   // Map from game to screen coordinates Y
	   if ( mustCheckYcoor ) {
	     y= s->y - topLeftYCoor;
	     if (y<0 || y>screenHeight) { 
                thisIndex=s->nextSprite;
                debugPutInt( "  offscreen (y), s",s->spriteNum );
                continue;
             }  

	   } else {
	     y=s->y;
	   }


	   // Adjust y position to account for title bar
	   y+=titleBarHeight;

	   debugPutInt("  drawing, s",s->spriteNum );
	   debugPutInt("    x",x);
	   debugPutInt("    y",y);   
	   debugPutInt("    bitmap",s->bitmap);  

	   // From x,y set:
           //   wordIdx = screen memory position
	   //   d2      = frame of bitmap
	   // Reg_wordsPerYRow is used instead of ASM_bytesPerYRow
	   Graphics1BitCalcSpritePosMACRO(x,y,Reg_wordsPerYRow,wordIdx,d2);
	   debugPutInt("    wordIdx",wordIdx);  
	   debugPutInt("    d2",d2);  

	   bmpStructPtr = &bmpTableNew[s->bitmap];

           // ******Draw the bitmap on screen******
	   h=bmpStructPtr->h;
	   bmpPtr = bmpTableNew[s->bitmap].bmp[d2];

	   //debugPutUInt32Hex("bmpPtr",bmpPtr);
	   
	   i=0;
	   while (h>0) {
	     w=bmpStructPtr->w;

	     while(w>0) {

		ASM_vm[wordIdx] |= bmpPtr[i];  // do an OR
		wordIdx++;
		i++;
		w--;
	     }
	    
	     wordIdx+=(10-bmpStructPtr->w);  // wordIdx+=8, optimized for 1-bit depth
	     h--;
	   }


	 } else {
	   //debugPutInt("  (used is false) s",thisIndex); 
	 }

         thisIndex=s->nextSprite;

      } //while thisIndex

   } // for c


   debugFuncExit("Graphics1BitDrawAllSprites()");
}

//--------------------------------------------------------------------
//  GraphicsReg2BitBmp
//--------------------------------------------------------------------
void GraphicsReg2BitBmp( 
   UInt16   bmpNum,
   UInt16   h,          
   UInt16   w,            // in pixels  
   char   * bmp,   
   UInt16   x1,
   UInt16   y1,
   UInt16   x2,
   UInt16   y2,
   UInt16   centerX,
   UInt16   centerY
)
{
   Int i=0,j,x,y,b;   
   UInt16 bmpLen,adjWidth;
   UInt16 hb=0,mb=0;
   UInt8 pIndex=0,maskNibl=0;
   
   char px=' ';
   

   debugFuncEnterInt("GraphicsReg2BitBmp, bmpNum",bmpNum);

   bmpLen = StrLen(bmp);


   // Ensure that string is complete
   if((h*w) != bmpLen) {
     ERROR_ASSERT("bitmap string != length");
     debugPutIntError("StrLen", bmpLen);
     debugFuncExit("GraphicsReg2BitBmp()");
     return;
   }

   // Ensure width is not odd
   if (w & 0x01) {
     ERROR_ASSERT("bitmap width is odd");
     debugPutIntError("w",w);
     debugFuncExit("GraphicsReg2BitBmp()");
     return;
   }

   // Convert pixels->word16's
   adjWidth = (w+7)/8;    // round-up to next word!
   adjWidth++;            // add 1 for spill-over word

   debugPutInt("w (pixels)",w);
   debugPutInt("adjWidth",adjWidth);
   

   // Set height, width
   bmpTableNew[bmpNum].h = h-1;          // -1 is required for ASM dbra instructions
   bmpTableNew[bmpNum].w = adjWidth-1;   // -1 is required for ASM dbra instructions
   bmpTableNew[bmpNum].x1 = x1;
   bmpTableNew[bmpNum].y1 = y1;
   bmpTableNew[bmpNum].x2 = x2;
   bmpTableNew[bmpNum].y2 = y2;
   bmpTableNew[bmpNum].centerX = centerX;
   bmpTableNew[bmpNum].centerY = centerY;

   //got_here;
   

   // Translate the ascii bitmap to correct indexes
   bmpTableNew[bmpNum].bmp[0]  = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
   bmpTableNew[bmpNum].mask[0] = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
   i=0;
   j=0;

   for (y=0;y<h;y++) {
      for (x=0;x<(adjWidth-1);x++) {

	 hb=0x0000;
	 mb=0x0000;
	 	 
	 // 1 UInt16 is 8 pixels
	 for (b=0;b<8;b++) {

	    // w = width in pixels
	    if ((x*8+b) >= w) {
	       px = '.';     // No more valid bitmap chars.. stuff with period
	    } else {
	       px = bmp[i];  // 
	       i++;
	    }   
	    
	    //debugPutInt("px",(UInt8)px);   
	    
	    // Convert bitmap ascii -> palette index
	    if ( px != '.') {
	       if ( ((UInt8)px < firstColorCharIndex) || ((UInt8)px > lastColorCharIndex))
	       {
		  ERROR_ASSERT("bmpChar out of range");
	       }
	       
	       pIndex = paletteIndexMap[ px-firstColorCharIndex ] & 0xf;
	       //debugPutHex("pIndex",pIndex);


	       // 2 bits only
	       pIndex = pIndex & 0x3;
	       

	    } else {
	       // zero is a signal to the ASM routine to let the background
	       pIndex = 0x00;
	    }
		    
	    hb=(hb<<2)+pIndex;

	    // Determine the mask char
	    if ( px == '.' ) {
	       maskNibl = 0x3;  // bg shows through
	    } else {
	       maskNibl = 0x0;  // bg is masked
	    }
	

	    mb = (mb<<2)+maskNibl;
	    

	 } // b

	 // write the bitmap
	 //debugPutHex("hb",hb);
	 //debugPutHex("mb",mb);
	 bmpTableNew[bmpNum].bmp[0][j]=hb;
	 bmpTableNew[bmpNum].mask[0][j]=mb;
	 
	 j++;
	 
      } // width=x

      //got_here;
      
      // Zero out the spillover word
      bmpTableNew[bmpNum].bmp[0][j]=0x0000;
      bmpTableNew[bmpNum].mask[0][j]=0xffff;
      j++;
      
     
   } // height=y

   //got_here;
   

   debugPutInt("j",j);
   debugPutInt("size",(h*adjWidth));
   

   // Now create 7 shifted bitmap copies 
   for (y=1;y<8;y++) {
      bmpTableNew[bmpNum].bmp[y] = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
      
      GraphicsCreateShiftedBmp(&bmpTableNew[bmpNum].bmp[y-1][0],
			       &bmpTableNew[bmpNum].bmp[y][0],
			       h,adjWidth,2,0);
   }

   // Now create 7 shifted mask copies  
   for (y=1;y<8;y++) {
      bmpTableNew[bmpNum].mask[y] = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
      
      GraphicsCreateShiftedBmp(&bmpTableNew[bmpNum].mask[y-1][0],
			       &bmpTableNew[bmpNum].mask[y][0],
			       h,adjWidth,2,1);
   }
   

   debugFuncExit("GraphicsReg2BitBmp()");
}


//--------------------------------------------------------------------
//  GraphicsReg4BitColorBmp
//--------------------------------------------------------------------
void GraphicsReg4BitColorBmp( 
   UInt16   bmpNum,
   UInt16   h,          
   UInt16   w,            // in pixels  
   char   * bmp,   
   UInt16   x1,
   UInt16   y1,
   UInt16   x2,
   UInt16   y2,
   UInt16   centerX,
   UInt16   centerY
)
{
   Int i=0,j,x,y,b;   
   UInt16 bmpLen,adjWidth;
   UInt16 hb=0,mb=0;
   UInt8 pIndex=0,maskNibl=0;
   
   char px=' ';
   

   debugFuncEnterInt("GraphicsReg4BitColorBmp, bmpNum",bmpNum);

   bmpLen = StrLen(bmp);


   // Ensure that string is complete
   if((h*w) != bmpLen) {
     ERROR_ASSERT("bitmap string != length");
     debugPutIntError("StrLen", bmpLen);
     debugFuncExit("GraphicsRegColorBmp()");
     return;
   }

   // Ensure width is not odd
   if (w & 0x01) {
     ERROR_ASSERT("bitmap width is odd");
     debugPutIntError("w",w);
     debugFuncExit("GraphicsRegColorBmp()");
     return;
   }

   // Convert pixels->word16's
   adjWidth = (w+3)/4;    // round-up to next word!
   adjWidth++;            // add 1 for spill-over word

   debugPutInt("h",h);
   debugPutInt("adjWidth",adjWidth);
   

   // Set height, width
   bmpTableNew[bmpNum].h = h-1;          // -1 is required for ASM dbra instructions
   bmpTableNew[bmpNum].w = adjWidth-1;   // -1 is required for ASM dbra instructions
   bmpTableNew[bmpNum].x1 = x1;
   bmpTableNew[bmpNum].y1 = y1;
   bmpTableNew[bmpNum].x2 = x2;
   bmpTableNew[bmpNum].y2 = y2;
   bmpTableNew[bmpNum].centerX = centerX;
   bmpTableNew[bmpNum].centerY = centerY;

   got_here;
   

   // Translate the ascii bitmap to correct indexes
   bmpTableNew[bmpNum].bmp[0]  = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
   bmpTableNew[bmpNum].mask[0] = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
   i=0;
   j=0;

   for (y=0;y<h;y++) {
      for (x=0;x<(adjWidth-1);x++) {

	 hb=0x0000;
	 mb=0x0000;
	 	 
	 // 1 UInt16 is 4 pixels
	 for (b=0;b<4;b++) {

	    // Stuff bitmap to get 4 bit wide sprites
	    if ((x*4+b) >= w) {
	       px = '.';
	    } else {
	       px = bmp[i];
	       i++;
	    }   
	    
	    //debugPutInt("px",(UInt8)px);   
	    
	    // Convert bitmap ascii -> palette index
	    if ( px != '.') {
	       if ( ((UInt8)px < firstColorCharIndex) || ((UInt8)px > lastColorCharIndex))
	       {
		  ERROR_ASSERT("bmpChar out of range");
	       }
	       
	       pIndex = paletteIndexMap[ px-firstColorCharIndex ] & 0xf;
	       //debugPutHex("pIndex",pIndex);

	    } else {
	       // zero is a signal to the ASM routine to let the background
	       pIndex = 0x00;
	    }
	 	 
	    hb=(hb<<4)+pIndex;

	    // Determine the mask char
	    if ( px == '.' ) {
	       maskNibl = 0xf;  // bg shows through
	    } else {
	       maskNibl = 0x0;  // bg is masked
	    }
	

	    mb = (mb<<4)+maskNibl;
	    

	 } // b

	 // write the bitmap
	 //debugPutHex("hb",hb);
	 //debugPutHex("mb",mb);
	 bmpTableNew[bmpNum].bmp[0][j]=hb;
	 bmpTableNew[bmpNum].mask[0][j]=mb;
	 
	 j++;
	 
      } // width=x

      //got_here;
      
      // Zero out the spillover word
      bmpTableNew[bmpNum].bmp[0][j]=0x0000;
      bmpTableNew[bmpNum].mask[0][j]=0xffff;
      j++;
      
     
   } // height=y

   //got_here;
   

   debugPutInt("j",j);
   debugPutInt("size",(h*adjWidth));
   

   // Now create 3 shifted bitmap copies 
   for (y=1;y<4;y++) {
      bmpTableNew[bmpNum].bmp[y] = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
      
      GraphicsCreateShiftedBmp(&bmpTableNew[bmpNum].bmp[y-1][0],
			       &bmpTableNew[bmpNum].bmp[y][0],
			       h,adjWidth,4,0);
   }

   // Now create 3 shifted mask copies 
   for (y=1;y<4;y++) {
      bmpTableNew[bmpNum].mask[y] = MemPtrNew(sizeof(UInt16)*h*(adjWidth));
      
      GraphicsCreateShiftedBmp(&bmpTableNew[bmpNum].mask[y-1][0],
			       &bmpTableNew[bmpNum].mask[y][0],
			       h,adjWidth,4,1);
   }

   // NULL these pointers out since they are unused, and GraphicsFreeMemory tests them
   bmpTableNew[bmpNum].bmp[4] = 0;
   bmpTableNew[bmpNum].bmp[5] = 0;
   bmpTableNew[bmpNum].bmp[6] = 0;
   bmpTableNew[bmpNum].bmp[7] = 0;

   bmpTableNew[bmpNum].mask[4] = 0;
   bmpTableNew[bmpNum].mask[5] = 0;
   bmpTableNew[bmpNum].mask[6] = 0;
   bmpTableNew[bmpNum].mask[7] = 0;

   debugFuncExit("GraphicsReg4BitColorBmp()");
}



//-------------------------------------------------------------------------
//  Graphics4BitASMDrawAllSprites
//
//  Loop through all sprites owned by Sprite engine and draw them
//  to the screen
//-------------------------------------------------------------------------
void Graphics4BitASMDrawAllSprites()
{
   Word c,cl;
   sprite * s;
   Int16 thisIndex;
   //Int16 wordsPerYRow;


   UInt16    * bmpFramePtr=0; 
   UInt16    * bmpMaskPtr=0; 
   bmpStruct * bmpStructPtr=0; 
   UInt32 wordIdx=0;
   
   UInt32 * vmPtr=0;
   
   Int x,y;
   
   
   // debug
   debugFuncEnter( "Graphics4BitASMDrawAllSprites()" );
 
   for (cl=0;cl<spriteClassMax;cl++) {

      // Adjust for render depth..
      c=spriteRenderDepthList[cl];

      thisIndex = spriteList[c].firstSprite;

      // Walk the list,let each sprite handle tick
      while ( thisIndex != -1 ) {

         s = allSprites[thisIndex];

	 if (notActive(s)) {
           thisIndex=s->nextSprite;
           continue;
	 }  

	 else {

	   debugPutInt( "  drawing, s",s->spriteNum );
	   
	   // Map from game to screen coordinates X
	   if ( mustCheckXCoor ) {
	     // Adjust the x coordinate and see if it's on the screen
	     x= s->x - topLeftXCoor;

	     if (x<0 || x>screenWidth) { 
                thisIndex=s->nextSprite;
	        debugPutInt( "  offscreen (x), s",s->spriteNum );
                continue; 
             }
	   } else {
	     x=s->x;
	   }

	   // Map from game to screen coordinates Y
	   if ( mustCheckYcoor ) {
	     y= s->y - topLeftYCoor;
	     if (y<0 || y>screenHeight) { 
                thisIndex=s->nextSprite;
                debugPutInt( "  offscreen (y), s",s->spriteNum );
                continue;
             }  
	   } else {
	     y=s->y;
	   }
	    

	   // Adjust y position to account for title bar
	   y+=titleBarHeight;

	   // debug
           debugPutInt("    x",x);
           debugPutInt("    y",y);   


	   // debug..
	   // if (s->collisionRegion < 8) {
	   // s->bitmap = s->collisionRegion;
	   //}
	   

	   // 4-bit depth
	   wordIdx=Reg_wordsPerYRow*y+((x>>2));  
	
	   bmpStructPtr = &bmpTableNew[s->bitmap];   
           bmpFramePtr  = &bmpTableNew[s->bitmap].bmp[x&2][0]; 
	   bmpMaskPtr   = &bmpTableNew[s->bitmap].mask[x&2][0];
	   vmPtr        = (UInt32*)&ASM_vm[wordIdx];
           //wordsPerYRow = ASM_bytesPerYRow;

	   //debugPutUInt32Hex("bmpFramePtr",bmpFramePtr);
	   //debugPutUInt32Hex("vmPtr",vmPtr);	   
	   //debugPutUInt32Hex("[0]",&bmpTableNew[s->bitmap].mask[x&2][0]);
	   //debugPutUInt32Hex("[1]",&bmpTableNew[s->bitmap].mask[x&2][1]);

	   // WARNING (Confirned) and BUG FIX.. "reading from unallocated chunk"
	   // If wordPtr (ASM_wordPos) on an odd boundary will cause an "address error"
      
	   __asm__ __volatile__ (   
	     "  movea.l %0, %%a2           ;     /*  a2= ptr to ASM_thisBug struct */ 
                movea.l %1, %%a0           ;     /*  a0= bmpFramePtr */ 
                movea.l %2, %%a1           ;     /*  a1= vmPtr */
       	        movea.l %4, %%a4           ;     /*  a4= bmpMaskPtr */ 


                move.w  (%%a2), %%d3       ;     /*  (%%a2).. d3= bmpWordHeight (1st word of struct) */
         .l08:  move.w  %5, %%d2       ;    /*    2(%%a2).. d2= bmp width (could store in reg for speed)*/

                move.l %%a1,%%a2;

      .lp05:    move.w (%%a1),%%d4          ;    /*   Get current screen value */
	        and.w  (%%a4)+,%%d4         ;    /*   AND the mask over the top */
	        or.w   (%%a0)+,%%d4         ;    /*   OR the  bitmap on top */
                move.w %%d4, (%%a1)+        ;    /*   Do the write */

                dbra %%d2, .lp05;       /* loop over width */

                move.l %%a2,%%a1;
                add.l  #80, %%a1            ;    /* goto next row -- VERY important to do long addition here */
                dbra %%d3, .l08"                /*  loop over height */

	     :
	     :"r"(bmpStructPtr),
	     "r" (bmpFramePtr),        
	     "r" (vmPtr),
	     "r" (80),    /* ignored */
	     "r" (bmpMaskPtr),
	     "r" (bmpTableNew[s->bitmap].w)
	     : "%%a0","%%a1","%%a2","%%a4","%%d3","%%d2","%%d4"
	     );
	   
   

             thisIndex=s->nextSprite;

	 }

      } //while thisIndex

   } // for c

   // debug, show the collision regions
   /*
   ASM_vm[Reg_wordsPerYRow * region2h] = 0xaaaa;
   ASM_vm[Reg_wordsPerYRow * region3l] = 0xaaaa;
   ASM_vm[Reg_wordsPerYRow * region1h] = 0xaaaa;
   ASM_vm[Reg_wordsPerYRow * region2l] = 0xaaaa;
   */

   debugFuncExit("Graphics4BitASMDrawAllSprites()");

}


//-------------------------------------------------------------------------
//  Graphics2BitASMDrawAllSprites
//
//  Loop through all sprites owned by Sprite engine and draw them
//  to the screen
//-------------------------------------------------------------------------
void Graphics2BitASMDrawAllSprites()
{
   Word c,cl;
   sprite * s;
   Int16 thisIndex;
   //Int16 wordsPerYRow;


   UInt16    * bmpFramePtr=0; 
   UInt16    * bmpMaskPtr=0; 
   bmpStruct * bmpStructPtr=0; 
   UInt32 wordIdx=0;
   
   UInt32 * vmPtr=0;
   
   Int x,y;
   
   
   // debug
   debugFuncEnter( "Graphics2BitASMDrawAllSprites()" );
 
   for (cl=0;cl<spriteClassMax;cl++) {

     
      // Adjust for render depth..
      c=spriteRenderDepthList[cl];

      thisIndex = spriteList[c].firstSprite;

      // Walk the list,let each sprite handle tick
      while ( thisIndex != -1 ) {

         s = allSprites[thisIndex];

	 if (notActive(s)) {
           thisIndex=s->nextSprite;
           continue;
	 }  

	 else {

	   debugPutInt( "  drawing, s",s->spriteNum );
	   
	   // Map from game to screen coordinates X
	   if ( mustCheckXCoor ) {
	     // Adjust the x coordinate and see if it's on the screen
	     x= s->x - topLeftXCoor;

	     if (x<0 || x>screenWidth) { 
                thisIndex=s->nextSprite;
	        debugPutInt( "  offscreen (x), s",s->spriteNum );
                continue; 
             }
	   } else {
	     x=s->x;
	   }

	   // Map from game to screen coordinates Y
	   if ( mustCheckYcoor ) {
	     y= s->y - topLeftYCoor;
	     if (y<0 || y>screenHeight) { 
                thisIndex=s->nextSprite;
                debugPutInt( "  offscreen (y), s",s->spriteNum );
                continue;
             }  
	   } else {
	     y=s->y;
	   }
	    

	   // Adjust y position to account for title bar
	   y+=titleBarHeight;

	   // debug
           debugPutInt("    x",x);
           debugPutInt("    y",y);      

	   // 2-bit depth
	   wordIdx=Reg_wordsPerYRow*y+((x>>3));   // correct
	   debugPutInt("    wordIdx",wordIdx);
	
	   bmpStructPtr = &bmpTableNew[s->bitmap];   
           bmpFramePtr  = &bmpTableNew[s->bitmap].bmp[x&7][0]; 
	   bmpMaskPtr   = &bmpTableNew[s->bitmap].mask[x&7][0];
	   vmPtr        = (UInt32*)&ASM_vm[wordIdx];


	   // WARNING (Confirned) and BUG FIX.. "reading from unallocated chunk"
	   // If wordPtr (ASM_wordPos) on an odd boundary will cause an "address error"
      
	   __asm__ __volatile__ (   
	     "  movea.l %0, %%a2           ;     /*  a2= ptr to ASM_thisBug struct */ 
                movea.l %1, %%a0           ;     /*  a0= bmpFramePtr */ 
                movea.l %2, %%a1           ;     /*  a1= vmPtr */
       	        movea.l %4, %%a4           ;     /*  a4= bmpMaskPtr */ 


                move.w  (%%a2), %%d3       ;     /*  (%%a2).. d3= bmpWordHeight (1st word of struct) */
         .l08a: move.w  %5, %%d2       ;    /*    2(%%a2).. d2= bmp width (could store in reg for speed)*/

                move.l %%a1,%%a2;

      .lp05a:   move.w (%%a1),%%d4          ;    /*   Get current screen value */
	        and.w  (%%a4)+,%%d4         ;    /*   AND the mask over the top */
	        or.w   (%%a0)+,%%d4         ;    /*   OR the  bitmap on top */
                move.w %%d4, (%%a1)+        ;    /*   Do the write */

                dbra %%d2, .lp05a;       /* loop over width */

                move.l %%a2,%%a1;
                add.l  #40, %%a1            ;    /* goto next row -- VERY important to do long addition here */
                dbra %%d3, .l08a"                /*  loop over height */

	     :
	     :"r"(bmpStructPtr),
	     "r" (bmpFramePtr),        
	     "r" (vmPtr),
	     "r" (40),    /* ignored */
	     "r" (bmpMaskPtr),
	     "r" (bmpTableNew[s->bitmap].w)
	     : "%%a0","%%a1","%%a2","%%a4","%%d3","%%d2","%%d4"
	     );

             thisIndex=s->nextSprite;

	 }

      } //while thisIndex

   } // for c

   debugFuncExit("Graphics2BitASMDrawAllSprites()");
}


//--------------------------------------------------------------------
//  GraphicsRegBmp
//
//  Given the info defining the sprite, create shifted copies, and
//  store them.
//--------------------------------------------------------------------
void GraphicsRegBmp( 
   UInt16   s,
   UInt16   h,
   UInt16   w,
   UInt16 * bmp,
   UInt16   x1,
   UInt16   y1,
   UInt16   x2,
   UInt16   y2,
   UInt16   centerX,
   UInt16   centerY
)
{

   Int i,x,y,n1=0,n2=0;
   UInt16 * p;
   UInt16 * prev=0;

   //Int ii;
   //char junk[21];
   //char j;
   //UInt32 n = 2^15;

   debugFuncEnterInt("GraphicsRegBmp, bitmap",s);

   // make this more graceful later...
   if(h*w>12) {
     ERROR_ASSERT("bitmap  h*w too large");
     debugFuncExit("GraphicsRegBmp()");
     return;
   }

   /*
   //-------convert bitmap to ascii------------
   for(i=0;i<h;i++) {
     n=0x8000;

     for (ii=15;ii>=0;ii--) {
        if (bmp[i] & n) {
          junk[16-ii]='a';
        } else {
          junk[16-ii]='.';
        }
	n=n>>1;
     } // ii

     junk[0]='"';
     junk[17]='"';
     junk[18] = ' ';
     junk[19]='\\';
     junk[20]=0;
     debugPutsNoFormat(junk);
   }
   //-------convert bitmap to ascii------------
   */

   // Set height, width
   bmpTableNew[s].h = h;
   bmpTableNew[s].w = w +1;  // add 1 word for rollover
   bmpTableNew[s].x1 = x1;
   bmpTableNew[s].y1 = y1;
   bmpTableNew[s].x2 = x2;
   bmpTableNew[s].y2 = y2;
   bmpTableNew[s].centerX = centerX;
   bmpTableNew[s].centerY = centerY;

   // Create the 8 bitmap slides
   for (i=0;i<8;i++)
   {

      bmpTableNew[s].bmp[i] = NULL;
      bmpTableNew[s].bmp[i] = MemPtrNew(sizeof(UInt16)*24);  // current limit of 24 words

      if (bmpTableNew[s].bmp[i] == NULL) {
	 ERROR_ASSERT("Out of Memory.  MemPtrNew failed");
      }


      debugPutUInt32Hex("bmpTableNew[s].bmp[i]",bmpTableNew[s].bmp[i]);
            
      p = bmpTableNew[s].bmp[i];

      // Load up first unshifted bitmap
      if (i==0) {
         for (y=0;y<h;y++) {
	    for (x=0;x<w;x++) {
               p[n2] = bmp[n1];
               n1++;n2++;
	    }
            p[n2]=0;  // nil the rollover word
            n2++;
	 }

      } else {
         // Not 0, create the shifted version
         GraphicsCreateShiftedBmp(prev,p,bmpTableNew[s].h,bmpTableNew[s].w,2,0);  // shift by 2
      }

      prev=p;

   }

   debugFuncExit("GraphicsRegBmp()");
}


//--------------------------------------------------------------------
//  GraphicsCreateShiftedBmp
// 
//  Create 1 shifted bmp pointed to by prev, and assigns it to now
//--------------------------------------------------------------------
void GraphicsCreateShiftedBmp(
   UInt16  * prev,
   UInt16  * now,
   UInt16    h,
   UInt16    w,
   Byte      numShifts,
   Byte      fillBit
)
{

   Int n1=0,Xnow=0,Xnext=0,i,x,y,z,r;

   //debugFuncEnter("GraphicsCreateShiftedBmp");

   // 1st, copy prev to now
   for (i=0;i<(h*w);i++) {
      now[i]=prev[i];
      //debugPutHex("now>>  ",now[i]);
   }
 
   // loop top-to-bottom
   for(y=0;y<h;y++){
      
      //debugPutInt("row",y);
      r=n1;

      // loop over shifts
      for (z=0;z<numShifts;z++) {

	 Xnow=fillBit;     // controls whether 0 or 1 will be shifted in from left
	 n1=r;  // go back
         
	 // loop left-to-right 
	 for (x=0;x<w;x++) 
	 {
	    
	    //------------------------start simulate ROXR----------------------------
	    Xnext   = (now[n1] & 0x0001);       // remember the least-significant-bit 
	    now[n1] = now[n1]>>1;    
	    if (Xnow !=0) { now[n1] |= 0x8000;} // set the most-significant-bit
	    Xnow=Xnext;                         
	    n1++;
	    //------------------------end simulate ROXR------------------------------
	    
	 } //x (left-to-right)

      } // z (numShifts)     

      //debug
      // for (x=0;x<w;x++) 
      //{
      //debugPutHex("new>>  ",now[r+x]);
      //}


   } // y (top-to-bottom)

   //debugPutInt("n1",n1);
   

   //debugFuncExit("GraphicsCreateShiftedBmp()");
}



//--------------------------------------------------------------------
// GraphicsFreeMemory()
//--------------------------------------------------------------------
void GraphicsFreeMemory()
{
   UInt16 i,j; 

   debugFuncEnter("GraphicsFreeMemory");

   // Free all memory
   for (i=0;i<MAXNUMBITMAPS;i++) {

      debugPutInt("i",i);
      
      for (j=0;j<NUM_BMPSLIDES;j++) {
	 // bitmaps
	 if (bmpTableNew[i].bmp[j]) {
	    debugPutInt("  bmp,j",j);
	    debugPutInt("    size",MemPtrSize(bmpTableNew[i].bmp[j]));
	    if (MemPtrSize(bmpTableNew[i].bmp[j]) > 0) 
	    {
	       MemPtrFree(bmpTableNew[i].bmp[j]);
	    }
		
	 }
      }
      
      for (j=0;j<NUM_MASKSLIDES;j++) {
	 // mask
	 if (bmpTableNew[i].mask[j]) {
	    debugPutInt("  mask,j",j);
	    debugPutInt("    size",MemPtrSize(bmpTableNew[i].mask[j]));
	    if (MemPtrSize(bmpTableNew[i].mask[j]) >0) 
	    {
	       MemPtrFree(bmpTableNew[i].mask[j]);
	    }
		
	 }
      }
   }

   debugFuncExit("GraphicsFreeMemory");
}


//--------------------------------------------------------------------
// GraphicsStopDoCleanup()
//
// App stopping, so do cleanup
//--------------------------------------------------------------------
void GraphicsStopDoCleanup()
{
   Err err;
   
   debugFuncEnter("GraphicsStopDoCleanup()");
   
   // Free the offscreen window 
   WinSetDrawWindow(backBuffer);
   WinDeleteWindow(backBuffer,false);
   
   // Reset the depth (Code from Ref 1, p.398)
   WinSetDrawWindow(scrnBuffer);
   err=ScrDisplayMode(scrDisplayModeSetToDefaults,NULL,NULL,NULL,NULL);
   
   // Free all memory
   GraphicsFreeMemory();
   
   got_here;

   debugFuncExit("GraphicsStopDoCleanup()");
}


//--------------------------------------------------------------------
// Graphics1BitDraw1Sprite
//
// Called to render a single bitmap.  Normally called for score bar
//
// Inputs:
//  ASM_thisBug = addr of bmpStruct which details the sprite (used for h,w access)
//  ASM_thisBmp = addr of array of words describing sprite (within bmpStruct)
//  ASM_bmpIndex = (unused)
//  ASM_wordPos = vm to draw to
//--------------------------------------------------------------------
void Graphics1BitDraw1Sprite()
{

   //debugFuncEnter("Graphics1BitDraw1Sprite");
   //debugPutHex("   ASM_wordPos",ASM_wordPos);
   
   // safety check
   if ( ((UInt32)ASM_wordPos < (UInt32)ASM_vm) || 
	((UInt32)ASM_wordPos > (UInt32)ASM_vmEnd)) 
   {
      debugPutIntError("ASM_wordPos out of range",ASM_wordPos);
      debugPutHex("hex",ASM_wordPos);
      return;
   }

  got_here;

__asm__  __volatile__ (   
	     "  movea.l %2, %%a1           ;     /*  a1= ASM_wordPos */
                movea.l %0, %%a2           ;     /*  a2= ptr to ASM_thisBug struct */ 
                movea.l %1, %%a0           ;     /*  a0= shipBmp */ 
                move.w (%%a2), %%d3        ;     /*  d3= bmpWordHeight (1st word of struct) */
                nop                        ;     /* START... write to screen */ 
         .l88:  move.w 2(%%a2), %%d2       ;     /* d2= bmp width (could store in reg for speed)*/
                movea.l %%a1, %%a3         ;     /* save ASM_wordPos */ 
        .lp55:  move.w (%%a0)+, %%d0       ;
                or.w   (%%a1), %%d0        ;      /* do an OR so we don't blank underneath */
                move.w %%d0, (%%a1)+       ;    
                subq.w #1, %%d2            ;    
                bne .lp55                  ;
                nop                        ;    /* START... do other rows */  
                movea.l %%a3, %%a1         ;
                add.l  #20, %%a1           ;    /*   VERY important to do long addition here */
                subq.w #1, %%d3            ;
                bne .l88"                      /*  loop back */
       :
       :"r" (ASM_thisBug),
        "r" (ASM_thisBmp),        
        "r" (ASM_wordPos)
       : "%%a0","%%a1","%%a2","%%a3",
	     "%%d3","%%d2","%%d0","memory"
);

   // try prevent gcc from screwing these up
//a=ASM_thisBmp;
//b=ASM_thisBug; 
//c=ASM_wordPos;

   //dummyProc(ASM_wordPos,((Int)ASM_thisBug&0xff));  
   //dummyProc((Int)ASM_thisBmp,99);  

}


//--------------------------------------------------------------------
// Graphics2BitDraw1Sprite
//
// Called for 2-bit title bar rendering
//--------------------------------------------------------------------
void Graphics2BitDraw1Sprite (
			  Byte   x,
			  Byte   y,
                          Byte   bmp
)
{

   UInt16    * bmpFramePtr=0; 
   UInt16    * bmpMaskPtr=0; 
   bmpStruct * bmpStructPtr=0; 
   UInt32      wordIdx=0;
   UInt32    * vmPtr=0;


   // 2-bit depth
   wordIdx=Reg_wordsPerYRow*y+((x>>3));   
   debugPutInt("    wordIdx",wordIdx);
   
   bmpStructPtr = &bmpTableNew[bmp];   
   bmpFramePtr  = &bmpTableNew[bmp].bmp[x&7][0]; 
   bmpMaskPtr   = &bmpTableNew[bmp].mask[x&7][0];
   vmPtr        = (UInt32*)&ASM_vm[wordIdx];
   
   // from Graphics2BitASMDrawAllSprites...

   __asm__ __volatile__ 
      (   
       "        movea.l %0, %%a2           ;     /*  a2= ptr to ASM_thisBug struct */ 
                movea.l %1, %%a0           ;     /*  a0= bmpFramePtr */ 
                movea.l %2, %%a1           ;     /*  a1= vmPtr */
       	        movea.l %4, %%a4           ;     /*  a4= bmpMaskPtr */ 


                move.w  (%%a2), %%d3       ;     /*  (%%a2).. d3= bmpWordHeight (1st word of struct) */
         .l08b: move.w  %5, %%d2       ;    /*    2(%%a2).. d2= bmp width (could store in reg for speed)*/

                move.l %%a1,%%a2;

      .lp05b:   move.w (%%a1),%%d4          ;    /*   Get current screen value */
	        and.w  (%%a4)+,%%d4         ;    /*   AND the mask over the top */
	        or.w   (%%a0)+,%%d4         ;    /*   OR the  bitmap on top */
                move.w %%d4, (%%a1)+        ;    /*   Do the write */

                dbra %%d2, .lp05b;       /* loop over width */

                move.l %%a2,%%a1;
                add.l  #40, %%a1            ;    /* goto next row -- VERY important to do long addition here */
                dbra %%d3, .l08b"                /*  loop over height */

	     :
	     :"r"(bmpStructPtr),
	     "r" (bmpFramePtr),        
	     "r" (vmPtr),
	     "r" (40),    /* ignored */
	     "r" (bmpMaskPtr),
	     "r" (bmpTableNew[bmp].w)
	     : "%%a0","%%a1","%%a2","%%a4","%%d3","%%d2","%%d4"
	     );

 
   debugFuncExit("Graphics2BitASMDrawAllSprites()");
}


//--------------------------------------------------------------------
// Graphics4BitDraw1Sprite
//
// Called for 4-bit title bar rendering
//--------------------------------------------------------------------
void Graphics4BitDraw1Sprite (
			  Byte   x,
			  Byte   y,
                          Byte   bmp
)
{

   UInt16    * bmpFramePtr=0; 
   UInt16    * bmpMaskPtr=0; 
   bmpStruct * bmpStructPtr=0; 
   UInt32      wordIdx=0;
   UInt32    * vmPtr=0;

   //
   // Debug, test 2-bit
   //
   //Graphics2BitDraw1Sprite(x,y,bmp);
   //return;
   //
   //

   wordIdx=Reg_wordsPerYRow*y+((x>>2));  
   
   bmpStructPtr = &bmpTableNew[bmp];   
   bmpFramePtr  = &bmpTableNew[bmp].bmp[x&2][0];   // only bmp[0] and bmp[2] are used
   bmpMaskPtr   = &bmpTableNew[bmp].mask[x&2][0];
   vmPtr        = (UInt32*)&ASM_vm[wordIdx];
   
   // from Graphics4BitASMDrawAllSprites...
      
	   __asm__ __volatile__ (   
	     "  movea.l %0, %%a2           ;     /*  a2= ptr to ASM_thisBug struct */ 
                movea.l %1, %%a0           ;     /*  a0= bmpFramePtr */ 
                movea.l %2, %%a1           ;     /*  a1= vmPtr */
	        movea.l %4, %%a4           ;     /*  a4= bmpMaskPtr */ 

                move.w  (%%a2), %%d3       ;     /*  (%%a2).. d3= bmpWordHeight (1st word of struct) */
         .ql08:  move.w  %5, %%d2       ;    /*    2(%%a2).. d2= bmp width (could store in reg for speed)*/

                move.l %%a1,%%a2;

      .qlp05:    move.w (%%a1),%%d4          ;    /*    Get current screen value */
	        and.w  (%%a4)+,%%d4         ;    /*   AND the mask over the top */
	        or.w   (%%a0)+,%%d4         ;    /*   OR the  bitmap on top */
                move.w %%d4, (%%a1)+        ;      /* Do the write */

                dbra %%d2, .qlp05;       /* loop over width */

                move.l %%a2,%%a1;
                add.l  #80, %%a1            ;    /* goto next row -- VERY important to do long addition here */
                dbra %%d3, .ql08"                /*  loop over height */

	     :
	     :"r"(bmpStructPtr),
	     "r" (bmpFramePtr),        
	     "r" (vmPtr),
	     "r" (80),    /* ignored */
	     "r" (bmpMaskPtr),
	     "r" (bmpTableNew[bmp].w)
	     : "%%a0","%%a1","%%a2","%%a4","%%a6","%%d3","%%d2","%%d4"
	     );


}


//-------------------------------------------------------------------------
//  GraphicsClrTitleBar
//-------------------------------------------------------------------------
void GraphicsClrTitleBar()
{

   // debug
   debugFuncEnter( "GraphicsClrTitleBar()" );

   WinSetDrawWindow(backBuffer);
   MemSet(ASM_vm,totalNumTitleBytes,graphicsColorTitleBarBack4Bit);  // was 0
 
   debugFuncExit("GraphicsClrTitleBar()");
}

//-------------------------------------------------------------------------
// Graphics1BitClrBackBuffer
//
// Clear the back buffer (used before drawing a new frame)
// Much more efficient than WinEraseRectangle(&screen,0)... ~2 ticks saved
//-------------------------------------------------------------------------
void Graphics1BitClrBackBuffer()
{
  
   // debug
   debugFuncEnter( "Graphics1BitClrBackBuffer()" );

   WinSetDrawWindow(backBuffer);
   MemSet(ASM_vm+totalNumTitleBytes/2,totalNumGameAreaBytes,0x00);

 
   debugFuncExit("Graphics1BitClrBackBuffer()");
}

//-------------------------------------------------------------------------
// Graphics4BitClrBackBuffer
//
// ALSO CALLED FOR 2-bit grey
//-------------------------------------------------------------------------
void Graphics4BitClrBackBuffer()
{
   debugFuncEnter( "Graphics4BitClrBackBuffer()" );

   // Unfortunately, I cannot squeeze 8 registers free, only 6 or 7 

	   __asm__ __volatile__ (   
	     "  movea.l %0, %%a0           ;     /*  a0= ptr to start of screen */ 
                move.l  %1, %%d1           ;     /*  d1= num times */
                move.l  %2, %%a1           ;     /*   */
               
                movem.l (%%a1), %%d2-%%d5/%%a3-%%a4;      /* 6 regs, zero out 24 bytes*/
   ClrStart:    movem.l %%d2-%%d5/%%a3-%%a4,   (%%a0);
                movem.l %%d2-%%d5/%%a3-%%a4, 24(%%a0);
                movem.l %%d2-%%d5/%%a3-%%a4, 48(%%a0); 
                movem.l %%d2-%%d5/%%a3-%%a4, 72(%%a0); 
                movem.l %%d2-%%d5/%%a3-%%a4, 96(%%a0);
                movem.l %%d2-%%d5/%%a3-%%a4, 120(%%a0);
                movem.l %%d2-%%d5/%%a3-%%a4, 144(%%a0);
                movem.l %%d2-%%d5/%%a3-%%a4, 168(%%a0); 
                movem.l %%d2-%%d5/%%a3-%%a4, 192(%%a0);
                movem.l %%d2-%%d5/%%a3-%%a4, 216(%%a0); 
 
                adda.l #240, %%a0;
                dbra %%d1, ClrStart;

                nop"
	     :
             : "r" (ASM_vm+totalNumTitleBytes/2), // div by 2 to convert words to bytes
	       "r" (graphicsNumClrBlocks), 
	       "r" (&graphicsColorBackground4Bit[0])
	     :"%%d1","%%d2","%%d3","%%d4","%%d5","%%a0","%%a1","%%a3","%%a4"
	     );


    debugFuncExit("Graphics4BitClrBackBuffer()");
}


//--------------------------------------------------------------------
// GraphicsDrawString -- for debug only text
//
// Note: ASSUMES WinSetDrawWindow HAS BEEN CALLED
//--------------------------------------------------------------------
void GraphicsDrawString(CharPtr chars, Word len, SWord x, SWord y)
{
   debugFuncEnter("GraphicsDrawString()");
   
   // This is slow as all hell (2 ticks)....  
   WinDrawChars(chars, len, x, y);
   
   debugFuncExit("GraphicsDrawString()");
}


//--------------------------------------------------------------------
// GraphicsRepaintNative
//
// Repaint routine using PalmOS calls
//--------------------------------------------------------------------
inline void GraphicsRepaintNative()
{
   debugFuncEnter( "GraphicsRepaintNative()" ); 
   WinCopyRectangle(backBuffer,scrnBuffer,&rect,0,0,winPaint);
   debugFuncExit( "GraphicsRepaintNative()" );    
}


//--------------------------------------------------------------------
// GraphicsRepaint
//--------------------------------------------------------------------
void GraphicsRepaint()
{
    debugFuncEnter( "GraphicsRepaint()" );

   ERROR_ASSERT("Dead proc called"); 
   return;
   

//  ********* Replaced by GraphicsRepaintNative   **********

   
	   __asm__ __volatile__ (   
	     "  movea.l %0, %%a0           ;     /*  a0= LCD */ 
                move.l  %1, %%d1           ;     /*  d1= num 240 byte blocks */
                move.l  %2, %%a1           ;     /*  a1= backbuffer */
               
 RepaintStart:  movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;      /* 6 regs, copy 24 bytes*/
                movem.l %%d2-%%d5/%%a3-%%a4,   (%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 24(%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 48(%%a0); 

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 72(%%a0); 

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 96(%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 120(%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 144(%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 168(%%a0); 

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 192(%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 216(%%a0);

                adda.l #240, %%a0;
                dbra %%d1, RepaintStart;

                move.l  %3, %%d1;     /*  d1= num 80 byte block copies */
                beq  Done;            /* Skip below if zero */
                subq.w #1, %%d2;      /* ensure we are counting from 1, not 0 */

 RepaintStart2: movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;      /* 6 regs, copy 24 bytes*/
                movem.l %%d2-%%d5/%%a3-%%a4,   (%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 24(%%a0);

                movem.l (%%a1)+, %%d2-%%d5/%%a3-%%a4;
                movem.l %%d2-%%d5/%%a3-%%a4, 48(%%a0); 

                movem.l (%%a1)+, %%d2-%%d3;
                movem.l %%d2-%%d3, 72(%%a0); 

                adda.l #80, %%a0;
                dbra %%d1, RepaintStart2;

      Done:     nop"
	     :
             : "r" (ASM_vmFront),
	       "r" (graphicsNumCopyBlocks),
	       "r" (ASM_vm),
	       "r" (graphicsNumCopyBlocks2)
	     :"%%d1","%%d2","%%d3","%%d4","%%d5","%%a0","%%a1","%%a3","%%a4"
	     );
   
   
   debugFuncExit("GraphicsRepaint()");
} 




//--------------------------------------------------------------------
//  GraphicsSetTextAndOtherColors
//
//  Background colors for the game screen and title bars are 
//  registered here to ensure (1) the backbuffer clear routines
//  (gamescreen and titlebar)  "washes" the correct color, 
//  (2) the background color for text can be remembered.
//  
//-------------------------------------------------------------------- 
void GraphicsSetTextAndOtherColors(
   UInt32 gamescreenBackdropColor,
   UInt32 foregroundTextColor,
   UInt32 titlebarBackdropColor
)
{

   UInt32 i;  

   debugFuncEnter("GraphicsSetTextAndOtherColors");

   // graphicsColorBackground4Bit is used in the backbuffer clearClrBackBuffer procs 
   // graphicsColorTitleBarBack4Bit is used in the titlebar graphicsClrTitleBar proc
   for (i=0;i<10;i++) {
      graphicsColorBackground4Bit[i] = gamescreenBackdropColor;
   }

   graphicsColorTitleBarBack4Bit = titlebarBackdropColor;  


   // Set the text foreground and background text colors
   textColorForeground4Bit = foregroundTextColor     & 0xf;   
   textColorBackground4Bit = gamescreenBackdropColor & 0xf;

   
   debugFuncExit("GraphicsSetTextAndOtherColors");

}

//--------------------------------------------------------------------
// GraphicsHdlNewForm
//
// Set the window colors and screen depth so that text is displayed correctly
//-------------------------------------------------------------------- 
void GraphicsHdlNewForm(UInt32 formId) 
{

   RGBColorType dummyPalette[256];

   RGBColorType t;
   
   Boolean   setColor=false;
   //UInt32 *  setDepth=1;
   UInt32 setDepth=1;
   
   debugFuncEnterInt("GraphicsHdlNewForm",formId);
   //debugPutInt("newDepth",newDepth);
   //debugPutInt("enableColor",(UInt16)enableColor);
   

   // Quick exit for monochrome
   if (ScreenDepth == oneBitDepth) {
      setDepth=1;
      ScrDisplayMode(scrDisplayModeSet,0,0,&setDepth,0); 
      return;
   }
  
   
   // Handle 4-bit color and grey..
   switch (formId)
   {
    
      case splashScreenForm:
	 // Must use PalmOS 3.0 ScrDisplayMode call 
	 ScrDisplayMode(scrDisplayModeSetToDefaults,NULL,NULL,NULL,NULL);
	 break;

      case gameScreenForm:
	 	 
	 // 2-bit grey
	 if (ScreenDepth == greyTwoBitDepth) {
	    setDepth=2;
	    ScrDisplayMode(scrDisplayModeSet,NULL,NULL,&setDepth,NULL); 

	    if (romVersion >= PALMOS35)
	    {
	       got_here;
	       
	       // 2-bit GREYSCALE command to set text colors  *** WinDrawChars is used  
	       // to write text *** WinSetTextColor, WinSetForeColor, WinSetBackColor do not
	       // appear to work.

	       WinIndexToRGB(textColorForeground4Bit & 0x3,&t);
	       UIColorSetTableEntry(UIObjectForeground,&t); // foreground text color
	       WinIndexToRGB((textColorBackground4Bit & 0x3),&t);
	       UIColorSetTableEntry(UIFormFill,&t); // background text color    
	    }
	 }
	 

	 // 4-bit grey
	 if (ScreenDepth == greyFourBitDepth) {
	    setDepth=4;
	    ScrDisplayMode(scrDisplayModeSet,NULL,NULL,&setDepth,NULL);

	    if (romVersion >= PALMOS35)
	    {
	       got_here;
	      	       
	       // 4-bit GREYSCALE command to set text colors  *** WinDrawChars is used  
	       // to write text *** WinSetTextColor, WinSetForeColor, WinSetBackColor do not
	       // appear to work. 

	       WinIndexToRGB(textColorForeground4Bit & 0xf,&t);
	       UIColorSetTableEntry(UIObjectForeground,&t); // foreground text color
	       //WinIndexToRGB(0,&t);
	       WinIndexToRGB((textColorBackground4Bit & 0xf),&t);
	       UIColorSetTableEntry(UIFormFill,&t); // background text color    
	    }
	 }

	 
	 // 4-bit COLOR
	 if (ScreenDepth == colorFourBitDepth) {
	    setDepth=4;
	    setColor=true;
	    ScrDisplayMode(scrDisplayModeSet,NULL,NULL,&setDepth,&setColor);
	    WinPalette(winPaletteSet,0,16,gamePalette);
	 
	    // Set text foreground and background colors...
	    // WinPalette destroys the UI color palette, so selectively rebuild it here
	    // with colors used in the game to enable correct game text rendering.
	    // WinSetTextColor, etc. does not appear to work well
	    WinIndexToRGB((textColorForeground4Bit & 0xf),&t);
	    UIColorSetTableEntry(UIObjectForeground,&t); // foreground text color
	    
	    WinIndexToRGB((textColorBackground4Bit & 0xf),&t);
	    UIColorSetTableEntry(UIFormFill,&t); // background text color
	    
	 }
   	 	 

	 break;

      default: 
	 
	 // Reset screen depth to default.  ScrDisplayMode is a PalmOS 3.0 routine, it was
	 // rename WinScreenMode in PalmOS 3.5.  We must use ScrDisplayMode since PalmOS 3.0
	 // may be running here (for the 2-bit greyscale mode).
	 ScrDisplayMode(scrDisplayModeSetToDefaults,0,0,0,0); 

	 // For color, reset palette to default
	 if (ScreenDepth == colorFourBitDepth) {
	    WinPalette(winPaletteSetToDefault,0,255,dummyPalette);
	 }
	 
	 break; 
	 
   }
      
   debugFuncExit("GraphicsHdlNewForm");
}


//--------------------------------------------------------------------
//  TextMgrInit() -- Initialize text manager
//-------------------------------------------------------------------- 
void TextMgrInit(Byte depth)
{
   debugFuncEnter("TextMgrInit()");
 
   // Set procvar
   switch(depth) {
      
      case oneBitDepth:         TextMgrDisplayTitleBar = TextMgrDisplay1BitTitleBar; break;
      case greyTwoBitDepth:     TextMgrDisplayTitleBar = TextMgrDisplay2BitTitleBar; break;
      case greyFourBitDepth:    TextMgrDisplayTitleBar = TextMgrDisplay4BitTitleBar; break;
      case colorFourBitDepth: 	TextMgrDisplayTitleBar = TextMgrDisplay4BitTitleBar; break;
	 
      default:
         debugPutsError("TextMgrInit depth confusion!");
         TextMgrDisplayTitleBar = TextMgrDisplay1BitTitleBar; 
   }
   
   TextMgrClr();

   debugFuncExit("TextMgrInit()");
} 



//--------------------------------------------------------------------
//  TextMgrClr() --  Remove all text
//-------------------------------------------------------------------- 
void TextMgrClr()
{
   Int i;

   debugFuncEnter("TextMgrClr()");
   
   for (i=0;i<TEXTDISPLINES;i++)
   {
      textMgrStr[i].displayFlag=false;
   }
   
   FntSetFont(stdFont);
   
   debugFuncExit("TextMgrClr()");
} 


//--------------------------------------------------------------------
// TextMgrAddCenteredText
//-------------------------------------------------------------------- 
void TextMgrAddCenteredText(
  char * text,
  Int n,
  FontID fontId,
  Int y,
  Boolean center
)
{
   FontID lastFontId = stdFont;

   // hope text fits in space... (check later) 
   StrCopy ( textMgrStr[n].str, text );

   textMgrStr[n].len=StrLen( textMgrStr[n].str );

   // Do this to correctly calc the length
   lastFontId = FntSetFont(fontId);

   // Well, maybe don't center it!
   if (center) {
      textMgrStr[n].posX=(80 - FntCharsWidth(textMgrStr[n].str,textMgrStr[n].len)/2); 
   } else {
      textMgrStr[n].posX=2;
   }
   
   FntSetFont(lastFontId);

   textMgrStr[n].posY=y;
   textMgrStr[n].fontId = fontId;
   textMgrStr[n].displayFlag=true;
   debugFuncExit("TextMgrAddCenteredText()");
}

//--------------------------------------------------------------------
//  Load the round banner (and sub banner) into text mgr
//-------------------------------------------------------------------- 
void TextMgrCreateRoundBanner()
{
   char t1[40] =" ";
   char t2[20] =" ";

 
   // debug
   debugFuncEnter( "TextMgrDisplayRoundBanner()" );

   StrCat(t1,"Next Wave: ");
   StrIToA(t2,gameInfo.round);
   StrCat(t1,t2);

   TextMgrAddCenteredText(t1,0,largeBoldFont,30,true);

   debugFuncExit("TextMgrCreateRoundBanner()");
} 

//--------------------------------------------------------------------
//  Write text to display
//-------------------------------------------------------------------- 
void TextMgrDisplayText()
{

  debugFuncEnter("TextMgrDisplayText()" );

  if (TITLEBAROFF == false) {
     TextMgrDisplayTitleBar();  // procvar
  }

  WinSetDrawWindow(backBuffer);  
  TextMgrDisplayGameText();

  if (DEBUGTEXT) {
     TextMgrDisplayDebugText();
  }

   debugFuncExit("TextMgrDisplayText()");
}

//--------------------------------------------------------------------
//  Write debug text to display
//-------------------------------------------------------------------- 
void TextMgrDisplayDebugText()
{
   debugFuncEnter("TextMgrDisplayDebugText()");

   WinDrawChars(debugStr,StrLen(debugStr),1,1);
   WinDrawChars(debugStr2,StrLen(debugStr2),1,10);

   debugFuncExit("TextMgrDisplayDebugText()");
}



//--------------------------------------------------------------------
//  Write text to display
//-------------------------------------------------------------------- 
void TextMgrDisplayGameText()
{
   Int i;

   debugFuncEnter("TextMgrDisplayGameText()");

   for(i=0;i<TEXTDISPLINES;i++) {

      if ( textMgrStr[i].displayFlag ) {
         FntSetFont(textMgrStr[i].fontId);

	 WinDrawChars( (CharPtr)textMgrStr[i].str,  // PalmOS 3.0 call
		       (Word)textMgrStr[i].len,
		        textMgrStr[i].posX,
		        textMgrStr[i].posY );
      }  
   }


   debugFuncExit("TextMgrDisplayGameText()");
}  

//--------------------------------------------------------------------
//  TextMgrNumToStr1Bit
//-------------------------------------------------------------------- 
void TextMgrNumToStr1Bit (
   UInt32   num,
   UInt16   x,
   UInt16   y
)
{

   Int i,bmp=0,z,z1;
   char numText[] = "       "; 
 
   UInt32 div=10000000;
   Int length=0;  
   Boolean firstDigitFound = false;

   UInt32 wordIdx;
   UInt32 frame;
 
   debugFuncEnterInt("TextMgrNumToStr1Bit() num",num);

   // Convert num to array of BCDs
   for(i=0;i<maxDigitsForNumToStr;i++)
   {
     numText[i] = num/div;  // get digit
     debugPutInt("  numText[i]",numText[i]);

     num -= numText[i]*div;

     if ((numText[i] !=0) || (firstDigitFound)) {
       length++;
       firstDigitFound = true;
     }
 
     div/=10;
   }

   if (length ==0) { length=1;}

   z  = maxDigitsForNumToStr-length;
   z1 = maxDigitsForNumToStr-1;

   debugPutInt("z",z);

   // Display score digits
   for (i=z;i<maxDigitsForNumToStr;i++) {

     bmp=numText[z1];
     z1--;

     got_here;

     Graphics1BitCalcSpritePosMACRO(x,y,Reg_wordsPerYRow,wordIdx,frame);
     ASM_wordPos=(UInt32)&ASM_vm[wordIdx];
     ASM_bmpIndex=frame;

     debugPutHex("ASM_wordPos",ASM_wordPos);
     debugPutInt("ASM_bmpIndex",ASM_bmpIndex);
     debugPutInt("bmp",bmp);

     ASM_thisBug = &bmpTableNew[bmp];
     ASM_thisBmp = bmpTableNew[bmp].bmp[ASM_bmpIndex];

     Graphics1BitDraw1Sprite();

     /*   
     debugPutHex("ASM_wordPos",ASM_wordPos);
     debugPutInt("ASM_bmpIndex",ASM_bmpIndex);
     debugPutHex("ASM_thisBug",(Int)ASM_thisBug);
     debugPutHex("ASM_thisBmp",(Int)ASM_thisBmp);
     */

     x-=4;
   }

   debugPuts("done");

   debugFuncExit("TextMgrNumToStr1Bit()");
   return;


}

//--------------------------------------------------------------------
//  TextMgrNumToStr2Bit
//-------------------------------------------------------------------- 
void TextMgrNumToStr2Bit (
   UInt32   num,
   UInt16   x,
   UInt16   y
)
{

   Int i,bmp=0,z,z1;
   char numText[] = "       "; 
 
   UInt32 div=10000000;
   Int length=0;  
   Boolean firstDigitFound = false;


   debugFuncEnterInt("TextMgrNumToStr2Bit() num",num);

   // Convert num to array of BCDs
   for(i=0;i<maxDigitsForNumToStr;i++)
   {
     numText[i] = num/div;  // get digit
     debugPutInt("  numText[i]",numText[i]);

     num -= numText[i]*div;

     if ((numText[i] !=0) || (firstDigitFound)) {
       length++;
       firstDigitFound = true;
     }
 
     div/=10;
   }

   if (length ==0) { length=1;}

   z  = maxDigitsForNumToStr-length;
   z1 = maxDigitsForNumToStr-1;

   debugPutInt("z",z);

   // Display digits
   for (i=z;i<maxDigitsForNumToStr;i++) {

     bmp=numText[z1];
     z1--;

     Graphics2BitDraw1Sprite(x,y,bmp); 

     x-=DigitSpacing;
   }

   debugFuncExit("TextMgrNumToStr2Bit()");
   return;
}



//--------------------------------------------------------------------
//  TextMgrNumToStr4Bit
//-------------------------------------------------------------------- 
void TextMgrNumToStr4Bit (
   UInt32   num,
   UInt16   x,
   UInt16   y
)
{

   Int i,bmp=0,z,z1;
   char numText[] = "       "; 
 
   UInt32 div=10000000;
   Int length=0;  
   Boolean firstDigitFound = false;


   debugFuncEnterInt("TextMgrNumToStr4Bit() num",num);

   // Convert num to array of BCDs
   for(i=0;i<maxDigitsForNumToStr;i++)
   {
     numText[i] = num/div;  // get digit
     debugPutInt("  numText[i]",numText[i]);

     num -= numText[i]*div;

     if ((numText[i] !=0) || (firstDigitFound)) {
       length++;
       firstDigitFound = true;
     }
 
     div/=10;
   }

   if (length ==0) { length=1;}

   z  = maxDigitsForNumToStr-length;
   z1 = maxDigitsForNumToStr-1;

   debugPutInt("z",z);

   // Display digits
   for (i=z;i<maxDigitsForNumToStr;i++) {

     bmp=numText[z1];
     z1--;

     Graphics4BitDraw1Sprite(x,y,bmp); 

     x-=DigitSpacing;
   }

   debugFuncExit("TextMgrNumToStr4Bit()");
   return;


}


//--------------------------------------------------------------------
// TextMgrDisplay1BitTitleBar
//-------------------------------------------------------------------- 
void TextMgrDisplay1BitTitleBar()
{
   Int i;
  
   debugFuncEnter("TextMgrDisplay1BitTitleBar()");

   if (!textMgrTitleBar.needsRedraw) {
     debugFuncExit("TextMgrDisplay1BitTitleBar()");
     return;
   }

   GraphicsClrTitleBar();
   textMgrTitleBar.needsRedraw = false;

   // round
   debugPutInt("   gameInfo.round%100",gameInfo.round%100);
   TextMgrNumToStr1Bit(gameInfo.round%100,6,2);

   // numships
   debugPutInt("   gameInfo.numShips%10",gameInfo.numShips%10);
   TextMgrNumToStr1Bit(gameInfo.numShips%10,24,2);

   // debugTimesHit
   if (USERIMMUNE) { 
      TextMgrNumToStr1Bit(gameInfo.debugTimesHit,60,2); 
   }

   // debug CPU level
   //TextMgrNumToStr1Bit(UtilCPULevel,90,2);

   // score
   TextMgrNumToStr1Bit(gameInfo.score,152,2); 

   debugPutHex("  ASM_vm is from ",(Int)ASM_vm);
   debugPutHex("              to ",(Int)(ASM_vm+totalNumWholeScreenBytes));


   // Draw wave symbol -- hardcoded for 1-bit depth 
   ASM_thisBug = &bmpTableNew[waveSymbolBmp];
   ASM_thisBmp = &bmpTableNew[waveSymbolBmp].bmp[5][0];
   ASM_wordPos = (UInt32)ASM_vm+30;
   Graphics1BitDraw1Sprite();

   // Draw men symbol -- hardcoded for 1-bit depth 
   ASM_thisBug = &bmpTableNew[numShipsBmp];
   ASM_thisBmp = &bmpTableNew[numShipsBmp].bmp[6][0];
   ASM_wordPos = (UInt32)ASM_vm+31;
   Graphics1BitDraw1Sprite();


   // Draw border line. ASM_vm is a UInt16 array, so we must divide byte count by 2
   for (i=0;i<10;i++) {
      ASM_vm[(totalNumTitleBytes-ASM_bytesPerYRow)/2 +i] = 0xaaaa; 
   }

 
   debugFuncExit("TextMgrDisplayTitleBar()");
}


//--------------------------------------------------------------------
// TextMgrDisplay2BitTitleBar
//-------------------------------------------------------------------- 
void TextMgrDisplay2BitTitleBar()
{
   debugFuncEnter("TextMgrDisplay2BitTitleBar()");

   if (!textMgrTitleBar.needsRedraw) {
     return;
   }

   GraphicsClrTitleBar();
   textMgrTitleBar.needsRedraw = false;
 
   debugFuncEnter("TextMgrDisplay4BitTitleBar()");
   
   // round (wave)
   debugPutInt("   gameInfo.round%100",gameInfo.round%100);
   TextMgrNumToStr2Bit(gameInfo.round%100,10,1);
   Graphics2BitDraw1Sprite(18,2,waveSymbolBmp);

   // numships
   debugPutInt("   gameInfo.numShips%10",gameInfo.numShips%10);
   TextMgrNumToStr2Bit(gameInfo.numShips%10,32,1);
   Graphics2BitDraw1Sprite(40,2,numShipsBmp);

   // score
   TextMgrNumToStr2Bit(gameInfo.score,152,1); 


   debugFuncExit("TextMgrDisplay2BitTitleBar()");
}



//--------------------------------------------------------------------
// TextMgrDisplay4BitTitleBar
//-------------------------------------------------------------------- 
void TextMgrDisplay4BitTitleBar()
{

   debugFuncEnter("TextMgrDisplay4BitTitleBar()");

   if (!textMgrTitleBar.needsRedraw) {
     return;
   }

   GraphicsClrTitleBar();
   textMgrTitleBar.needsRedraw = false;
 
   debugFuncEnter("TextMgrDisplay4BitTitleBar()");
   
   // round (wave)
   debugPutInt("   gameInfo.round%100",gameInfo.round%100);
   TextMgrNumToStr4Bit(gameInfo.round%100,10,1);
   Graphics4BitDraw1Sprite(18,2,waveSymbolBmp);

   // numships
   debugPutInt("   gameInfo.numShips%10",gameInfo.numShips%10);
   TextMgrNumToStr4Bit(gameInfo.numShips%10,32,1);
   Graphics4BitDraw1Sprite(40,2,numShipsBmp);

   // debugTimesHit
   /*
   if (USERIMMUNE) { 
      TextMgrNumToStr4Bit(gameInfo.debugTimesHit,70,1);
   }

   // debug CPU level
   TextMgrNumToStr4Bit(UtilCPULevel,90,1);
   */

   // score
   TextMgrNumToStr4Bit(gameInfo.score,152,1); 

   debugFuncExit("TextMgrDisplay4BitTitleBar()");
}



//--------------------------------------------------------------------
// SoundMgrSetSoundPreferences
//
// Called when preference setting change
//-------------------------------------------------------------------- 
void SoundMgrSetSoundPreferences(mainGameRecordStructV1 * pref)
{

   UInt16 palmosGameVolSettings[]={0,8,32,64,0};   // off,l,m,h settings of Palmos
   
   debugFuncEnterInt("SoundMgrSetSoundPreferences(), vol",SoundMgr.vol);
   
   if (pref->volPref == 4) {
      // Lookup the default settings
      SoundMgr.vol = PrefGetPreference(prefGameSoundVolume);
   } else {
      SoundMgr.vol = palmosGameVolSettings[mainGameRec1.volPref];
   }
   
   
   debugFuncExit("SoundMgrSetSoundPreferences()");
}

//--------------------------------------------------------------------
//  SoundMgrPlaySounds
//-------------------------------------------------------------------- 
void SoundMgrPlaySounds()
{

   SndCommandType   sndCmd;

   debugFuncEnter("SoundMgrPlaySounds()");

   if ((SoundMgr.thisSound != noSound) && (SoundMgr.vol != 0))
   {
        sndCmd.cmd    = sndCmdFrqOn; // non-blocking
	sndCmd.param1 = soundCatelog[SoundMgr.thisSound].freq;
	sndCmd.param2 = soundCatelog[SoundMgr.thisSound].length;

	// volume is the min of pref or effect
	sndCmd.param3 = (SoundMgr.vol < soundCatelog[SoundMgr.thisSound].vol) ?
                                 SoundMgr.vol :soundCatelog[SoundMgr.thisSound].vol;  

	SndDoCmd( 0, &sndCmd, true );   // true = no wait

	SoundMgr.numTicksLeft--;
	if ( SoundMgr.numTicksLeft <= 0)
	{
           SoundMgr.thisSound = noSound;
	}
   } 

   debugFuncExit("SoundMgrPlaySounds()");
}

//--------------------------------------------------------------------
//  SoundMgrThisSound
//-------------------------------------------------------------------- 
void SoundMgrThisSound(soundType sound) 
{

  debugFuncEnter("SoundMgrThisSound()");

  if (sound > SoundMgr.thisSound) {
     debugFuncEnterInt("SoundMgrThisSound(), will play sound",(Int)sound);
     SoundMgr.thisSound = (soundType)sound;
     SoundMgr.numTicksLeft = soundCatelog[(Int)sound].numTicksLeft;
  } else {
     debugFuncEnterInt("SoundMgrThisSound() rejected sound",(Int)sound);
     debugPutInt("   current sound",(Int)SoundMgr.thisSound);
  }


   debugFuncExit("SoundMgrThisSound()");
}






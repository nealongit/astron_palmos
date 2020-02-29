//
//  SETUP -- 2,4 bit grey and color graphics
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


#include "setup.h"
#include "graphics.h" 
#include "debug.h"
#include "defs.h"
#include "misc.h"
#include "starter.h" // for ScreenDepth and mainGameRec4



//-------------------------------------------------------------------------
//  Grey digit bitmaps
//-------------------------------------------------------------------------

char dig0BmpGrey[] =
"..aaa..." \
".aabba.." \
"aababaz." \
"aababaz." \
"aababaz." \
"aababaz." \
"aababaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";
char dig1BmpGrey[] =
"..aaa..." \
".aabaa.." \
"aabbaaz." \
"aaabaaz." \
"aaabaaz." \
"aaabaaz." \
"aaabaaz." \
"aaabaaz." \
".aaaaz.." \
"..zzz...";
char dig2BmpGrey[] =
"..aaa..." \
".abbaa.." \
"aaaabaz." \
"aaaabaz." \
"aabbbaz." \
"aabaaaz." \
"aabaaaz." \
"aabbbaz." \
".aaaaz.." \
"..zzz...";
char dig3BmpGrey[] =
"..aaa..." \
".abbaa.." \
"aaaabaz." \
"aaaabaz." \
"aaabaaz." \
"aaaabaz." \
"aaaabaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";
char dig4BmpGrey[] =
"..aaa..." \
".aaaba.." \
"aababaz." \
"aababaz." \
"aabbbaz." \
"aaaabaz." \
"aaaabaz." \
"aaaabaz." \
".aaaaz.." \
"..zzz...";
char dig5BmpGrey[] =
"..aaa..." \
".abbba.." \
"aabaaaz." \
"aabbbaz." \
"aaaabaz." \
"aaaabaz." \
"aaaabaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";
char dig6BmpGrey[] =
"..aaa..." \
".aabba.." \
"aabaaaz." \
"aabaaaz." \
"aabbbaz." \
"aababaz." \
"aababaz." \
"aabbbaz." \
".aaaaz.." \
"..zzz...";
char dig7BmpGrey[] =
"..aaa..." \
".abbba.." \
"aaaabaz." \
"aaaabaz." \
"aaabaaz." \
"aaabaaz." \
"aaabaaz." \
"aaabaaz." \
".aaaaz.." \
"..zzz...";
char dig8BmpGrey[] =
"..aaa..." \
".aabba.." \
"aababaz." \
"aababaz." \
"aabbbaz." \
"aababaz." \
"aababaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";
char dig9BmpGrey[] =
"..aaa..." \
".aabba.." \
"aababaz." \
"aababaz." \
"aabbbaz." \
"aaaabaz." \
"aaaabaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";


//-------------------------------------------------------------------------
//  Colors
//-------------------------------------------------------------------------



char colorDigitBg1     = 'a';     // bg = background
char colorDigitFg      = 'b';     // fg = foreground
char colorCockpit      = 'c';
char colorStar         = 'd';
char colorBullet       = 'e';
// 'f'
char colorReappear     = 'g';
char colorThrust1      = 'h';
char colorThrust2      = 'i';
char colorTtlBarSymbol1 = 'j';    // darker accent
char colorTtlBarSymbol2 = 'k';
char colorStar2        = 'l';
char colorUser1        = 'm';
char colorBug1         = 'n';
char colorUser2        = 'o';
char colorBug2         = 'p';
char colorTracBeam     = 'q';
char colorExplos       = 'r';
char colorMine1        = 's';
char colorMine2        = 't';
char colorMine3        = 'u';
char colorBug3         = 'v';
char color2Shot        = 'w';
// 'x'
char colorBackground   = 'y';     // ***COLOR ONLY***
char colorDigitBg2     = 'z';   // outline color

char FIRSTCOLORCHAR    = 'a';
char LASTCOLORCHAR     = 'z';     // Must be set to last char



//-------------------------------------------------------------------------
//  Color bitmaps
//-------------------------------------------------------------------------

char dig0BmpColor[] =
"..aa...." \
".aabba.." \
"aababaz." \
"aababaz." \
"aababaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";
char dig1BmpColor[] =
"..aa...." \
".aabaa.." \
"aabbaaz." \
"aaabaaz." \
"aaabaaz." \
"aaabaaz." \
".aaaaz.." \
"..zzz...";
char dig2BmpColor[] =
"..aa...." \
".abbba.." \
"aaaabaz." \
"aabbbaz." \
"aabaaaz." \
"aabbbaz." \
".aaaaz.." \
"..zzz...";
char dig3BmpColor[] =
"..aa...." \
".abbba.." \
"aaaabaz." \
"aaabbaz." \
"aaaabaz." \
"aabbbaz." \
".aaaaz.." \
"..zzz...";
char dig4BmpColor[] =
"..aa...." \
".ababa.." \
"aababaz." \
"aabbbaz." \
"aaaabaz." \
"aaaabaz." \
".aaaaz.." \
"..zzz...";
char dig5BmpColor[] =
"..aa...." \
".abbba.." \
"aabaaaz." \
"aabbbaz." \
"aaaabaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";
char dig6BmpColor[] =
"..aa...." \
".aabba.." \
"aabaaaz." \
"aabbbaz." \
"aababaz." \
"aabbbaz." \
".aaaaz.." \
"..zzz...";
char dig7BmpColor[] =
"..aa...." \
".abbba.." \
"aaaabaz." \
"aaabaaz." \
"aaabaaz." \
"aaabaaz." \
".aaaaz.." \
"..zzz...";
char dig8BmpColor[] =
"..aa...." \
".abbba.." \
"aababaz." \
"aabbbaz." \
"aababaz." \
"aabbbaz." \
".aaaaz.." \
"..zzz...";
char dig9BmpColor[] =
"..aa...." \
".abbba.." \
"aababaz." \
"aabbbaz." \
"aaaabaz." \
"aabbaaz." \
".aaaaz.." \
"..zzz...";
char userShipBmpColor[] =
".....m......" \
"....mmm....." \
"....mom....." \
"...mocom...." \
"...mocom...." \
"...mmmmm...." \
"..mmmmmmm..." \
"..mmomomm..." \
"mmmoomoommm." \
".mmoomoomm.." \
"..oo...oo...";
char bugBulletBmpColor[] =
".ee." \
"eeee" \
"eeee" \
".ee.";
char userBulletBmpColor[] =
".ee." \
".ee." \
".ee." \
".ee.";
char dotBulletBmpColor[] =
"...." \
".ee." \
".ee." \
"....";
char explosBmpColor[] =
"....r..." \
".r..r..r" \
"..r.r.r." \
"........" \
".rr...rr" \
"........" \
"..r.r.r." \
".r..r..r" \
"....r...";
char twoShotAwardBmpColor[] =
"..www." \
".wwwww" \
".w..ww" \
"...ww." \
"..ww.." \
".wwwww" \
".wwwww";
char barrierBmpColor[] =
".............." \
".nnn......nnn." \
"..nnnnnnnnnn.." \
".....nnnn.....";
char starBmpColor[] =
".d..";
char star2BmpColor[] =
".l..";
char bug1_2Color[] =
"............" \
"nn.pn.np.nn." \
"nnnpppppnnn." \
"n.nnpppnn.n." \
"...nnpnn...." \
"....nnn....." \
"..nnncnnn..." \
"...nncnn...." \
"....npn.....";
char sweepBug2Color[] =
"...pp..pp..." \
"..pp....pp.." \
".pp..vv..pp." \
"..pvvvvvvp.." \
"...pvvvvp..." \
"....pnnp...." \
"...nnnnnn..." \
"...nnccnn..." \
"....nnnn....";
char bug1Color[] =
"............" \
"nnnp...pnnn." \
"nnnpppppnnn." \
"n.nnpppnn.n." \
"...nnpnn...." \
"....nnn....." \
"..nnncnnn..." \
"...nncnn...." \
"....npn.....";
char sweepBugColor[] =
"..pp....pp.." \
".pp......pp." \
"ppp..vv..ppp" \
"p.pvvvvvvp.p" \
"p..pvvvvp..p" \
"....pnnp...." \
"..nnnnnnnn.." \
"...nnccnn..." \
"....nnnn....";
char mothershipColor[] =
"nn.nn..nn.nn" \
".nnnpnnpnnn." \
".nnnnnnnnnn." \
"...npnnpn..." \
".....nn....." \
"....nppn...." \
"....pppp...." \
"....pccp...." \
".....pp....." \
"............";
char dartBugColor[] =
"............" \
"...n.p.n...." \
".pn.npn.np.." \
".pnnnnnnnp.." \
"..nnnnnnn..." \
"...nnnnn...." \
"....nnn....." \
"....ncn....." \
"....nnn....." \
".....n......";
char mineLayerColor[] =
"...nnnnn...." \
"....nnn....." \
".nv..n..vn.." \
".nvppvppvn.." \
"..nvpvpvn..." \
"...nnvnn...." \
"....ppp....." \
"...ppcpp...." \
"...ppcpp...." \
"....ppp....." \
"............";
char fullMotherShipColor[] =
"nnn.nppn.nnn" \
".nnppppppnn." \
".neeppppeen." \
"..neeppeen.." \
"...neppen..." \
"....neen...." \
"....nnnn...." \
"....pppp...." \
"....pccp...." \
".....pp.....";
char bigOafBmpColor[] =
"nn.nn.nn.nn" \
"..n..n..n.." \
"..nnnnnnn.." \
"...nnnnn..." \
"....nnn...." \
"..n..n..n.." \
"..nnnnnnn.." \
"...nn.nn..." \
"....nnn....";
char userShip2BmpColor[] =
".....m......" \
"....mmm....." \
"....mom....." \
"...mocom...." \
"...mocom...." \
"m..mmmmm..m." \
"m.mmmmmmm.m." \
"..mmomomm..." \
"mmmoomoommm." \
".mmoomoomm.." \
"..oo...oo...";
char waveSymbolBmpColor[] =
".jk.kj" \
".jjkjj" \
".kjjjk" \
".jkjkj" \
".jjkjj" \
"..jjj." \
"...j..";
char mineBmpColor[] =
"...t...." \
"..ttt..." \
"uttsttu." \
"..ttt..." \
"...t....";
char numShipsBmpColor[] =
"...j...." \
"..jkj..." \
"..jkj..." \
"..jjj..." \
".jjjjj.." \
"kjjjjjk." \
"kjkjkjk.";
char tracBeamBmpColor[] =
"...q...." \
"........" \
"..q.q..." \
"...q...." \
".q...q.." \
"..qqq..." \
"q.....q." \
".qqqqq..";
char thrust1Color[] =
"hih." \
"hih." \
".h.." \
"....";
char thrust2Color[] =
"hih." \
"hih." \
"hhh." \
".h..";
char thrust3Color[] =
"hih." \
".h.." \
"...." \
"....";
char thrust4Color[] =
"hih." \
"hhh." \
".h.." \
"....";
char inFormBmp1Color[] =
"........" \
"........" \
"...g...." \
"..ggg..." \
"...g....";

char inFormBmp2Color[] =
"........" \
"........" \
"...g...." \
"..g.g..." \
"...g....";

char inFormBmp3Color[] =
"...g...." \
"...g...." \
"........" \
"gg...gg." \
"........" \
"...g...." \
"...g....";


//-------------------------------------------------------------------------
//  Set2BitGreyShades
//
//  
//-------------------------------------------------------------------------
void Set2BitGreyShades()
{
 
   UInt8   ch,i=0,s=0,textColor;
   UInt32  bgShade32=0,ttlBar32=0; 
   UInt8   greyMap4Bit[4] = {0,1,2,2}; // darkest setting is lowered to medium

   UInt8   x[7][26]={

      // 
      {0,3,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,3,3,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       3,3,2,     // colorReappear,colorThrust1,colorThrust2
       3,0,3,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       1,1,2,2, // colorUser1,colorBug1,colorUser2,colorBug2
       3,3,       // colorTracBeam,colorExplos                (NO CHANGE)
       1,3,2,     // colorMine1,2,3
       2,         // colorBug3
       3,0,2,    // color2Shot,unused,colorBackground         (NO CHANGE)
       2},         // colorDigitBg2                            (NO CHANGE)

      // 
      {0,3,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,3,3,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       3,3,2,     // colorReappear,colorThrust1,colorThrust2
       3,0,3,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       2,2,3,3, // colorUser1,colorBug1,colorUser2,colorBug2
       3,3,       // colorTracBeam,colorExplos                (NO CHANGE)
       1,3,2,     // colorMine1,2,3
       3,         // colorBug3
       3,0,2,    // color2Shot,unused,colorBackground         (NO CHANGE)
       2},         // colorDigitBg2                            (NO CHANGE)

      // 
      {0,3,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,3,3,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       3,3,2,     // colorReappear,colorThrust1,colorThrust2
       3,0,3,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       2,3,3,2, // colorUser1,colorBug1,colorUser2,colorBug2
       3,3,       // colorTracBeam,colorExplos                (NO CHANGE)
       1,3,2,     // colorMine1,2,3
       3,         // colorBug3
       3,0,2,    // color2Shot,unused,colorBackground         (NO CHANGE)
       2},         // colorDigitBg2                            (NO CHANGE)

      // DEFAULT SETTING
      {0,3,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,3,3,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       3,3,2,     // colorReappear,colorThrust1,colorThrust2
       3,0,3,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       2,3,3,3, // colorUser1,colorBug1,colorUser2,colorBug2
       3,3,       // colorTracBeam,colorExplos                (NO CHANGE)
       1,3,2,     // colorMine1,2,3
       3,         // colorBug3
       3,0,2,    // color2Shot,unused,colorBackground         (NO CHANGE)
       2},         // colorDigitBg2  

      // 
      {0,3,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,3,3,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       3,3,2,     // colorReappear,colorThrust1,colorThrust2
       3,0,3,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       3,3,2,3, // colorUser1,colorBug1,colorUser2,colorBug2
       3,3,       // colorTracBeam,colorExplos                (NO CHANGE)
       1,3,2,     // colorMine1,2,3
       3,         // colorBug3
       3,0,2,    // color2Shot,unused,colorBackground         (NO CHANGE)
       2},         // colorDigitBg2                              (NO CHANGE)

      //
      {0,3,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,3,3,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       3,3,2,     // colorReappear,colorThrust1,colorThrust2
       3,0,3,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       3,3,2,3, // colorUser1,colorBug1,colorUser2,colorBug2
       3,3,       // colorTracBeam,colorExplos                (NO CHANGE)
       1,3,2,     // colorMine1,2,3
       3,         // colorBug3
       3,0,2,    // color2Shot,unused,colorBackground         (NO CHANGE)
       2},         // colorDigitBg2                            (NO CHANGE)

      // DARKEST (MOSTLY BLACK)
      {0,3,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,3,3,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       3,3,2,     // colorReappear,colorThrust1,colorThrust2
       3,0,3,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       3,3,3,3, // colorUser1,colorBug1,colorUser2,colorBug2
       3,3,       // colorTracBeam,colorExplos                (NO CHANGE)
       3,3,3,     // colorMine1,2,3
       3,         // colorBug3
       3,0,2,    // color2Shot,unused,colorBackground         (NO CHANGE)
       2}         // colorDigitBg2                            (NO CHANGE)

   };
   
   debugFuncEnter("Set2BitGreyShades()");


   // Set the char --> shade mapping
   ch = (UInt8)FIRSTCOLORCHAR;
   while (ch <= (UInt8)LASTCOLORCHAR) {
      GraphicsRegisterGrey(x[mainGameRec4.greyscaleTheme][i],ch);
      ch++; i++;
   }

   // Set the star color to black or non-visible (set to background)
   s = (mainGameRec4.backgroundStarfield) ? 0x3 : (greyMap4Bit[mainGameRec4.backgroundGreyShade]);
   GraphicsRegisterGrey(s,(UInt8)colorStar);
   GraphicsRegisterGrey(s,(UInt8)colorStar2);


   // Set background shade
   bgShade32 = Util2BitTo32Bit(greyMap4Bit[mainGameRec4.backgroundGreyShade]);

   // Set text color to black (0x3).  Set title bar to light grey
   textColor = Util2BitTo32Bit(3);
   ttlBar32  = Util2BitTo32Bit(1);
   
   // background,text,title bar colors
   GraphicsSetTextAndOtherColors(bgShade32,textColor,ttlBar32); 

   debugFuncExit("Set2BitGreyShades()");

}

//-------------------------------------------------------------------------
//  Set4BitGreyShades
//
//  Standard black background with star field
//-------------------------------------------------------------------------
void Set4BitGreyShades()
{
 
   UInt8   ch,i=0,textColor,s=0;
   UInt32  bgShade32=0,ttlBar32=0; 

   UInt8   greyMap4Bit[4] = {0,2,6,12};
   
   

   UInt8   x[7][26]={

      // 1
      {0,15,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,15,15,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       15,15,10,     // colorReappear,colorThrust1,colorThrust2
       15,0,15,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       5,5,10,10, // colorUser1,colorBug1,colorUser2,colorBug2
       15,15,       // colorTracBeam,colorExplos                (NO CHANGE)
       5,15,10,     // colorMine1,2,3
       10,         // colorBug3
       15,0,10,    // color2Shot,unused,colorBackground         (NO CHANGE)
       10},         // colorDigitBg2                            (NO CHANGE)

      // 2
      {0,15,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,15,15,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       15,15,10,     // colorReappear,colorThrust1,colorThrust2
       15,0,15,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       10,10,15,15, // colorUser1,colorBug1,colorUser2,colorBug2
       15,15,       // colorTracBeam,colorExplos                (NO CHANGE)
       5,15,10,     // colorMine1,2,3
       15,         // colorBug3
       15,0,10,    // color2Shot,unused,colorBackground         (NO CHANGE)
       10},         // colorDigitBg2                            (NO CHANGE)

      // 3
      {0,15,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,15,15,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       15,15,10,     // colorReappear,colorThrust1,colorThrust2
       15,0,15,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       10,15,15,10, // colorUser1,colorBug1,colorUser2,colorBug2
       15,15,       // colorTracBeam,colorExplos                (NO CHANGE)
       5,15,10,     // colorMine1,2,3
       15,         // colorBug3
       15,0,10,    // color2Shot,unused,colorBackground         (NO CHANGE)
       10},         // colorDigitBg2                            (NO CHANGE)

      // 4, DEFAULT SETTING
      {0,15,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,15,15,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       15,15,2,     // colorReappear,colorThrust1,colorThrust2
       15,0,15,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       10,10,15,15, // colorUser1,colorBug1,colorUser2,colorBug2
       15,15,       // colorTracBeam,colorExplos                (NO CHANGE)
       5,15,10,     // colorMine1,2,3
       15,         // colorBug3
       15,0,10,    // color2Shot,unused,colorBackground         (NO CHANGE)
       10},         // colorDigitBg2  

      // 5
      {0,15,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,15,15,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       15,15,10,     // colorReappear,colorThrust1,colorThrust2
       15,0,15,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       15,10,10,15, // colorUser1,colorBug1,colorUser2,colorBug2
       15,15,       // colorTracBeam,colorExplos                (NO CHANGE)
       5,15,10,     // colorMine1,2,3
       15,         // colorBug3
       15,0,10,    // color2Shot,unused,colorBackground         (NO CHANGE)
       10},         // colorDigitBg2                              (NO CHANGE)

      // 6
      {0,15,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,15,15,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       15,15,10,     // colorReappear,colorThrust1,colorThrust2
       15,0,15,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       15,15,10,15, // colorUser1,colorBug1,colorUser2,colorBug2
       15,15,       // colorTracBeam,colorExplos                (NO CHANGE)
       5,15,10,     // colorMine1,2,3
       15,         // colorBug3
       15,0,10,    // color2Shot,unused,colorBackground         (NO CHANGE)
       10},         // colorDigitBg2                            (NO CHANGE)

      // 7, DARKEST (MOSTLY BLACK)
      {0,15,      // colorDigitBg1,colorDigitFg                (NO CHANGE)
       0,15,15,0, // colorCockpit,colorStar,colorBullet,unused  (NO CHANGE)
       15,15,10,     // colorReappear,colorThrust1,colorThrust2
       15,0,15,    // colorTtlBarSymbol1,colorTtlBarSymbol2,colorStar2 (NO CHANGE)
       15,15,15,15, // colorUser1,colorBug1,colorUser2,colorBug2
       15,15,       // colorTracBeam,colorExplos                (NO CHANGE)
       15,15,15,     // colorMine1,2,3
       15,         // colorBug3
       15,0,10,    // color2Shot,unused,colorBackground         (NO CHANGE)
       10}         // colorDigitBg2                            (NO CHANGE)

   };

   debugFuncEnter("Set4BitGreyShades()");

   // Set the char --> shade mapping
   ch = (UInt8)FIRSTCOLORCHAR;
   while (ch <= (UInt8)LASTCOLORCHAR) {
      GraphicsRegisterGrey(x[mainGameRec4.greyscaleTheme][i],ch);
      ch++; i++;
   }

   // Overwrite the star color with the user's preference - 2 different shades
   s = (mainGameRec4.backgroundStarfield) ? 0xf : (greyMap4Bit[mainGameRec4.backgroundGreyShade]);
   GraphicsRegisterGrey(s,(UInt8)colorStar);  
   s = (mainGameRec4.backgroundStarfield) ? 0x8 : (greyMap4Bit[mainGameRec4.backgroundGreyShade]);
   GraphicsRegisterGrey(s,(UInt8)colorStar2);  


   // In 2-bit greyscale the 2-bit shade var maps directly to the background shade, but in
   // 4-bit an indexing must occur.
   bgShade32 = Util4BitTo32Bit( greyMap4Bit[mainGameRec4.backgroundGreyShade] );

   debugPutInt("mainGameRec4.backgroundGreyShade",mainGameRec4.backgroundGreyShade);
   debugPutHex("bgShade32",bgShade32);

   // Make text white if background is darkest setting
   textColor = (mainGameRec4.backgroundGreyShade != 3) ? 0x0f : 0x00;

   // Hardcode the title bar shade
   ttlBar32  = Util4BitTo32Bit(5); 

   GraphicsSetTextAndOtherColors(bgShade32,textColor,ttlBar32);
  

   debugFuncExit("Set4BitGreyShades()");

}



//-------------------------------------------------------------------------
//  SetStandard4BitColors 
//
//  Standard black background with star field
//-------------------------------------------------------------------------
void SetStandard4BitColors()
{
 
   UInt32  bIndex,textIndex,ttlBarIndex;
   UInt32  bIndex32=0,ttlBarIndex32=0;
   

   // background..
   bIndex = GraphicsRegisterColor(0x00,0x00,0x00,colorBackground);     // y, black
   
   // bugs..
   ttlBarIndex = GraphicsRegisterColor(0xff,0xff,0xff,colorCockpit);  // c, white
   GraphicsRegisterColor(0x33,0x33,0xcc,colorBug1);  // n, dark blue
   GraphicsRegisterColor(0x33,0xcc,0xff,colorBug2);  // p, baby blue 
   GraphicsRegisterColor(0x66,0x66,0xff,colorBug3);  // v, plum

   // user
   GraphicsRegisterColor(0x33,0x33,0xcc,colorUser2);  // o, blue
   GraphicsRegisterColor(0x99,0xcc,0xff,colorUser1);  // m

   // thrust
   GraphicsRegisterColor(0xff,0xff,0x66,colorThrust1); // h   yellow
   GraphicsRegisterColor(0xff,0x66,0x33,colorThrust2); // i   red

   // digits..
   textIndex = GraphicsRegisterColor(0,0xff,0,colorDigitFg);  // b, green
   GraphicsRegisterColor(0x00,0x00,0xff,colorDigitBg1);  // a, hunter green

   // other..
   GraphicsRegisterColor(0xff,0xff,0xcc,colorStar);  //  d, dim yellow 
   GraphicsRegisterColor(0xcc,0x33,0x33,colorStar2); //  l, deep red
   GraphicsRegisterColor(0x00,0xff,0x00,colorBullet); // e


   // Mine
   GraphicsRegisterColor(0x00,0xff,0x00,colorMine3);  // u
   GraphicsRegisterColor(0xdd,0xdd,0xdd,colorMine2);  // t

   // Same colors
   GraphicsRegisterSameColor(colorDigitFg,colorReappear);  // g
   GraphicsRegisterSameColor(colorUser2,colorTtlBarSymbol1); // j
   GraphicsRegisterSameColor(colorUser1,colorTtlBarSymbol2); // k
   GraphicsRegisterSameColor(colorBackground,colorDigitBg2); // z
   GraphicsRegisterSameColor(colorBullet,colorTracBeam);     // q
   GraphicsRegisterSameColor(colorMine2,colorExplos);
   GraphicsRegisterSameColor(colorBug1,colorMine1);  // s
   GraphicsRegisterSameColor(colorThrust1,color2Shot);  // w

   // Set text, then entire palette
   bIndex32      = Util4BitTo32Bit(bIndex);
   ttlBarIndex32 = Util4BitTo32Bit(ttlBarIndex);

   GraphicsSetTextAndOtherColors(bIndex32,textIndex,ttlBarIndex32);
   GraphicsPaletteIsDone();

}


//-------------------------------------------------------------------------
//  RegisterMultiBitGameBitmaps 
//
//  Used by 2bit and 4bit greyscale, and 4bit color
//-------------------------------------------------------------------------
void RegisterMultiBitGameBitmaps()
{

  debugFuncEnter("RegisterMultiBitGameBitmaps()");


   if (ScreenDepth == colorFourBitDepth) {
     // Color digit bitmaps
     GraphicsRegComplexBmp(0,8,8,dig0BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(1,8,8,dig1BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(2,8,8,dig2BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(3,8,8,dig3BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(4,8,8,dig4BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(5,8,8,dig5BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(6,8,8,dig6BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(7,8,8,dig7BmpColor,0,0,0,0,0,0);
     GraphicsRegComplexBmp(8,8,8,dig8BmpColor,0,0,0,0,0,0);  
     GraphicsRegComplexBmp(9,8,8,dig9BmpColor,0,0,0,0,0,0);
   } else {
     // 2,4 bit grey digit bitmaps
     GraphicsRegComplexBmp(0,10,8,dig0BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(1,10,8,dig1BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(2,10,8,dig2BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(3,10,8,dig3BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(4,10,8,dig4BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(5,10,8,dig5BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(6,10,8,dig6BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(7,10,8,dig7BmpGrey,0,0,0,0,0,0);
     GraphicsRegComplexBmp(8,10,8,dig8BmpGrey,0,0,0,0,0,0);  
     GraphicsRegComplexBmp(9,10,8,dig9BmpGrey,0,0,0,0,0,0);
  }
     
  

  //                     bmpNum,h,w,*,x1,y1,x2,y2,centerX,centerY
  GraphicsRegComplexBmp(userShipBmp  ,11,12,userShipBmpColor,0,0,10,10,5,5);
  GraphicsRegComplexBmp(fatBulletBmp ,4,4,bugBulletBmpColor,0,0,3,3,2,1);
  GraphicsRegComplexBmp(slimBulletBmp,4,4,userBulletBmpColor,0,0,3,3,2,1);  // same "size" as round bullet
  GraphicsRegComplexBmp(explosBmp    ,9,8,explosBmpColor,0,0,7,8,3,3);
  GraphicsRegComplexBmp(twoShotAwardBmp,7,6,twoShotAwardBmpColor,1,0,5,6,2,4);
  GraphicsRegComplexBmp(barrierBmp   ,4,14,barrierBmpColor,1,0,14,8,7,2);  // y2=8 to catch user bullet
  GraphicsRegComplexBmp(starBmp      ,1,4,starBmpColor,0,0,0,0,0,0);
  GraphicsRegComplexBmp(bug1_2       ,9,12,bug1_2Color,0,1,10,8,4,4);
  GraphicsRegComplexBmp(sweepBug2    ,9,12,sweepBug2Color,1,0,10,8,5,5);
  GraphicsRegComplexBmp(bug1         ,9,12,bug1Color,1,0,10,8,5,5);
  GraphicsRegComplexBmp(sweepBug     ,9,12,sweepBugColor,0,0,8,11,5,5);
  GraphicsRegComplexBmp(mothership   ,10,12,mothershipColor,0,0,9,11,5,5);
  GraphicsRegComplexBmp(dartBug      ,10,12,dartBugColor,0,0,9,11,5,5);
  GraphicsRegComplexBmp(mineLayer    ,11,12,mineLayerColor,1,0,10,10,5,5);
  GraphicsRegComplexBmp(fullMotherShip,10,12,fullMotherShipColor,0,0,11,9,5,5);
  // bigOafBmpColor goes here
  GraphicsRegComplexBmp(userShip2Bmp ,11,12,userShip2BmpColor,0,0,11,10,5,5);
  GraphicsRegComplexBmp(waveSymbolBmp,7,6,waveSymbolBmpColor,0,0,0,0,0,0); 
  GraphicsRegComplexBmp(mineBmp      ,5,8,mineBmpColor,0,0,6,8,3,3); // y2=8 to catch user bullet
  GraphicsRegComplexBmp(numShipsBmp  ,7,8,numShipsBmpColor,0,0,5,5,0,0);
  GraphicsRegComplexBmp(tracBeamBmp  ,8,8,tracBeamBmpColor,0,1,5,7,3,4);
  GraphicsRegComplexBmp(thrust1      ,4,4,thrust1Color,0,0,3,2,0,1);
  GraphicsRegComplexBmp(thrust2      ,4,4,thrust2Color,0,0,3,2,0,1);
  GraphicsRegComplexBmp(inFormBmp1   ,5,8,inFormBmp1Color,0,0,0,0,0,0);  // no collision while in-from seqn
  GraphicsRegComplexBmp(inFormBmp2   ,5,8,inFormBmp2Color,0,0,0,0,0,0);
  GraphicsRegComplexBmp(inFormBmp3   ,7,8,inFormBmp3Color,0,0,0,0,0,0);
  GraphicsRegComplexBmp(thrust3      ,4,4,thrust3Color,0,0,3,2,0,1);
  GraphicsRegComplexBmp(thrust4      ,4,4,thrust4Color,0,0,3,2,0,1);
  GraphicsRegComplexBmp(dotBulletBmp ,4,4,dotBulletBmpColor,0,0,3,3,2,1);
  GraphicsRegComplexBmp(star2Bmp     ,1,4,star2BmpColor,0,0,0,0,0,0);


  // Set variables for 4 bit depth
  ThrustSpriteXOffset=4;
  TracBeamXOffset=7;
  UserShipXMax=149;
  DigitSpacing=8;  // was 4, 
  CapturedByTracBeamXOffset=0;

  debugFuncExit("RegisterMultiBitGameBitmaps()");
  
}


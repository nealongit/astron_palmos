# Astron (PalmOS) - source code

### About

Astron is an old PalmOS arcade-style space game I wrote and published commercially between 2000-2003.  It was
 well reviewed by CNET
 and
 ZDNet.  I eventually stopped development and released the fully functioning source code on my website (that has long
  ago been shuttered). 

Here's some great 16-bit graphics!
https://web.archive.org/web/20080820183332/http://www.ncblast.com/screenshots/screenshots.html

Here's the old website:
https://web.archive.org/web/20090530155433/http://www.ncblast.com/

If you've nostalgic for retro 16-bit graphics, C coding, and a little assembler feel free to take this code and do
 what you want with it. 
 
 **SORRY - THERE IS NO EMAIL SUPPORT. JUST TAKE THE CODE AND RUN!**
 
 

### Introduction

Astron was developed on a Windows PC using the PRC-Tools tool set.  I haven’t done development on the game in yearssssss
 but I believe there’s a lot that can be done with the code for someone who enjoys creating games. 

Astron can either be extended to add new enemies, challenges, and waves, or the “guts” can be ripped out to create new games – the code is modular and contains amongst other things: game lifecycle code, sprite animation, a blitter, a collision-detection engine, save/restore capabilities, debug facilities, etc. 

The code is 95% C code and about 5% Assembler (though the Assembler code could possibly be removed now that CPUs are more powerful).  Greyscale, color, and monochrome PalmOS devices are all supported.

Astron was last compiled against SDK 5.3 (or was that 3.5?).  The game runs successfully on PalmOS 5.0.  


This link gives the installation instructions for PRC-Tools on Windows:
http://prc-tools.sourceforge.net/install/cygwin.html


 

## Understanding the code

Astron was first developed for the 16MHz CPU of the Palm III.  Producing a smooth motion action game on that low a CPU is extremely challenging -- especially when the device does not provide any native sprite management, collision detection, or a graphics blitter.

AppInit() in starter.c is the main initialization routine for when Astron is started.  Start there to see how Astron initializes including accesses to the database to load and use Astron’s stored records.  FormEventHandlerGameScreen() in starter.c is the main control loop for the game.  Start there for your understanding of the gameplay. 

Note: Astron used to require paid registration to play after the free trial period ended.  The SecurityMgr code which used to handle this has now largely been removed from the code base, but there are a few instances of code related to the SecurityMgr which remain, largely because I didn’t want to take the time to cleanup the code completely.  They won’t affect runtime behavior.

The code is fairly clean and generally well commented (most procs have their entrance and exit commented).  Some of the modules should be subdivided for maintainability.  Currently the Astron code base is about 17000 lines of code.

More documentation would be helpful of course, especially regarding the sprite collision detection algorithm and how the enemy movement is encoded, but this is all there is.  Turn the debugs on and follow the flow to understand the code.

The code is released under the conditions detailed in the following section.


## Terms of Release

The Astron code base was released for the fun and enjoyment of developers.  The Astron game or any game or application derived from the code base may not be sold for profit without the specific written consent of the original developer (Neal Coleman).  And I’m not likely to grant that consent – this code is for fun.   

 

## Debugging

The debug facilities for Astron are located in files debug.h/debug.c. **IMPORTANT**: the #define THIS_IS_A_RELEASE_LOAD in starter.h needs to be commented out to turn debugging on.  This is because the debug methods were defined as macros so that they could be removed from the “released” game because debug output slows the game (at least on a 16 or 32 MHz CPU).  

 

The three main debugging techniques used on Astron are:

    Debug trace output (i.e. HostTraceOuputxx() calls). 
    A hidden debug menu
    On-screen debug text

 

For HostTraceOuputxx() calls, POSEr will then send debugs to a textfile or the Palm Reporter depending on how POSEr is configured.

The hidden debug menu is accessed by pressing Nav-Up (page up) during game play.  You can edit the list of options in HdlDebugMenuEvent() in starter.c.  The current list allows you to flip several debug flags, change the debug log level, run some timing tests, etc.

On-screen debug text is enabled by accessing the hidden debug menu, and selecting “DEBUG ON SCREEN”.  You can change the displayed text in HdlGameProgress() in gameman.c.


The AUTOPLAY, SKIPINTROWAVES, HOLDFORMATION, etc. flags set in debug.c (and available on the hidden debug menu) allow you to modify gameplay to zero in on specific areas of concern.

 

## Compiling overview

    Install Cygwin, PRC-Tools, PilRC, POSEr, PalmOS SDKs
    Make sure the Cygwin directory (i.e. c:\cygwin\bin) is in your Path.
    Unzip the Astron source zipfile
    Look at the notes in bld/Makefile to understand the build process and how to tune it to your setup
    cd /bld
    make clean (optional)
    make
    astron.prc is created in /bld

 

## Now what?

That’s it.. take the code, and start playing!  There is no more documentation except for the code comments.  For
 building the code start with Makefile.  For understanding the code, refer to the notes above.  There is no email
  support.  Enjoy and have fun!

 
Neal Coleman


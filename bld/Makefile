#----------------------------------------------------------------------
# Makefile
#
# This makefile builds astron.prc. It could likely be cleaned up a little, but it
# works. 
#
# WHEN INSTALLING ON YOUR PC:
#  1. MAKE SURE cygwin/bin IS IN YOU PATH
#  2. EDIT THE "PILRC" VAR BELOW TO MATCH YOUR PC
#
# OTHER FILES YOU NEED TO KNOW ABOUT:
#
#  (1) astron.def - Currently the code is multi-segment. This file determines the segments
#               I'm not sure if PalmOS still has this limitation.  Don't muck with it if you
#               don't understand it.
#
#  (2) ../src/starterRsc.rcp - Resource definition file for PiLRC
#
# TO COMPILE:
#  cd to /bld
#  make clean (optional)
#  make
#
#----------------------------------------------------------------------


#----------------------------------------------------------------------
# PRC-tool, makefile, etc. Documentation:
#    http://prc-tools.sourceforge.net/doc/
#
# General Makefile rule syntax:
#
# 	targets : prerequisites
# 		command
#
# Compare target to prereqs, if target is older, then run the command.  
# A <tab> must go before the command.
#
# make begins by processing the first rule, which in turn forces make to process the
# rules for prerequisite files, and on down the line.
#----------------------------------------------------------------------


#-----------------------------------------------------------------------------
#                               VARIABLES
#-----------------------------------------------------------------------------

# MYAPP   = variable which is the app name 
MYAPP    = astron

# DEFFILE = user written file defining the multiple code segments, read by multigen, which
#           in turn writes an asm stub file and a linker file ($MYAPP-sections.s, $MYAPP-sections.ld)
DEFFILE  = $(MYAPP).def

# RCPNAME = user written file read by PiLRC which defines forms, menus, alerts, etc
RCPNAME  = "../src/starterRsc.rcp"

# EXEC = 
EXEC     = $(MYAPP).prc


#
# BUILD EXECUTABLES (picked up because they are in the $PATH (c:\cygwin\bin))
#
CC       = m68k-palmos-gcc
OBJRES   = m68k-palmos-obj-res
BUILDPRC = build-prc
MULTIGEN = m68k-palmos-multigen

PILRC    = "C:/PalmDev Tools/pilrc-3.0/pilrc"


# DEFINES:
#   ALLOW_ACCESS_TO_INTERNALS_OF_WINDOWS = This flag is needed to allow graphics.c to compile
#                                          since the displayAddrV20 has been hidden in PalmOS 5.x
#                                          (see Window.h)
#
DEFINES = -DALLOW_ACCESS_TO_INTERNALS_OF_WINDOWS


# COMPILER FLAGS:
#   -g        = generate debug files for gdb
#   -Wall     = show all compiler warnings
#   -palmosX  = ***deprecated*** use palmdev-prep tool
#   -01       = 01=optimize some
#   -Winline  = forces gcc to say when it is declining the inline directive (and why)
CCFLAGS = -g -Wall -palmos5.3 -O1 


# LINKER FLAGS:
#   -Xlinker  = pass next option to linker (ld) needed before each flag
#   -XMap     = directives to produce the app.map file to determine code sizes
#   app.map   = map file produced
LDFLAGS = -Xlinker -XMap -Xlinker app.map


# OBJECTS
OBJS    = starter.o gameman.o sprite.o debug.o graphics.o wave.o \
          behav.o blast.o misc.o setup.o setup4Bit.o database.o \
          $(MYAPP)-sections.o


#-----------------------------------------------------------------------------
#                             TARGETS/RULES
#-----------------------------------------------------------------------------

# 1st rule which causes all other rules to be called.  build-prc needs
# 2 files to be built.. $(MYAPP) and tFRM9998.bin (the PiLRC file) before it
# can be invoked to build the .prc executable file.
$(EXEC): $(MYAPP) tFRM9998.bin 
	$(BUILDPRC) $(DEFFILE) $(MYAPP) *.bin

#
# This line forces PILRC to create all the bin files each time.  Note tFRM9998.bin is
# never produced.  We could likely use the -o and/or -ro options to put bin files all
# in 1 file/dir
#
# Manual: http://cvs.sourceforge.net/viewcvs.py/*checkout*/pilrc/pilrc/doc/manual.html
#
# PiLRC...
tFRM9998.bin:
	$(PILRC) $(RCPNAME)

#
# following lines needed for multiple code sections (see PRC html helpfile)
#
$(MYAPP)-sections.o: $(MYAPP)-sections.s
	m68k-palmos-gcc -c $(MYAPP)-sections.s

#
# multigen creates 2 helper files ($MYAPP-sections.s, $MYAPP-sections.ld) 
# used by the compiler and linker to handle apps with multiple code segments
#
$(MYAPP)-sections.s $(MYAPP)-sections.ld: $(DEFFILE)
	$(MULTIGEN) $(DEFFILE)

$(MYAPP): $(OBJS) $(MYAPP)-sections.ld
	m68k-palmos-gcc $(LDFLAGS) -o $(MYAPP) $(OBJS) $(MYAPP)-sections.ld


#
# Explicit build rules
#
starter.o : ../src/starter.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
gameman.o : ../src/gameman.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
sprite.o : ../src/sprite.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
debug.o : ../src/debug.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
graphics.o : ../src/graphics.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
wave.o : ../src/wave.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
behav.o : ../src/behav.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
blast.o : ../src/blast.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
misc.o : ../src/misc.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
setup.o : ../src/setup.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
setup4Bit.o : ../src/setup4Bit.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<
database.o : ../src/database.c
	$(CC) -c $(CCFLAGS) $(DEFINES) -o $*.o $<


#-----------------------------------------------------------------------------
#                          DUMMY TARGETS/RULES
#-----------------------------------------------------------------------------
clean:
	rm -f *.[oa] *.bin bin.res app.map *.grc *.prc *~ $(MYAPP) $(MYAPP)-*


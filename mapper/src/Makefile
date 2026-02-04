
# Makefile for the Mapper library
#
# Richard Bown, February 1997


# # These three should have been exported from the superior make
#
# ROSEGARDEN	= /home/rich/rosegarden
# SYSINCDIRS	= -I/usr/include -I/usr/include/X11 -I/usr/include/X11/Xaw -I/usr/include/X11/bitmaps
# DEFINES		= -DFUNCPROTO -DHAVE_SPECIALIST_MALLOC_LIBRARY

INTERLOCK	= $(ROSEGARDEN)/interlock
YAWN		= $(ROSEGARDEN)/yawn
LISTS		= $(ROSEGARDEN)/lists
MAPPER		= $(ROSEGARDEN)/mapper
COMMON		= $(ROSEGARDEN)/common

INCDIRS		= $(SYSINCDIRS) -I$(ROSEGARDEN)/include -I$(COMMON)/bitmaps -I../include

SYSLDFLAGS	= -L/usr/lib -L/usr/lib/X11
SYSLIBS		= -lmalloc -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lLists -lMidi -lInterlock $(SYSLIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib $(LIBS)

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)

TARGET		= $(MAPPER)/lib/libMapper.a
OBJECTS		= Device.o Mapper.o Timer.o


$(TARGET):	$(OBJECTS)
		rm -f $(TARGET)
		ar q $(TARGET) $(OBJECTS)
		$(RANLIB) $(TARGET)

.c.o:
		$(CC) -c $(CFLAGS) $<

clean:
		rm -f $(OBJECTS)
		rm -f $(MAPPER)/lib/libMapper.a

depend:		
# don't use Makedepend in here, there are too many different systems' files
# here are some dependencies, but this ain't perfect:

Device.o: ../../include/Mapper.h
Device.o: ../../include/SysDeps.h ../../config.h
Device.o: ../../include/Lists.h /usr/include/stdio.h
Device.o: ../../include/MidiXInclude.h
Device.o: ../../include/Yawn.h
Device.o: ../../include/MidiEvent.h
Device.o: ../../include/MidiConsts.h
Device.o: ../../include/MidiFile.h
Device.o: ../../include/MidiErrorHandler.h
Device.o: ../../include/MidiBHeap.h
Device.o: ../../include/MidiTrack.h
Device.o: ../../include/Debug.h
Device.o: ../../sequencer/src/Globals.h ../../sequencer/src/Types.h
Device.o: ../../include/ServiceNames.h
Mapper.o: ../../include/Mapper.h
Mapper.o: ../../include/SysDeps.h ../../config.h
Mapper.o: ../../include/Lists.h
Mapper.o: ../../include/MidiXInclude.h
Mapper.o: ../../include/Yawn.h
Mapper.o: ../../include/MidiEvent.h
Mapper.o: ../../include/MidiConsts.h
Mapper.o: ../../include/Debug.h

Mapper.o:	Mapper_OSS.c Mapper_SGI.c Mapper_ZILOG.c
Timer.o:	Timer_OSS.c Timer_SGI.c Timer_ZILOG.c


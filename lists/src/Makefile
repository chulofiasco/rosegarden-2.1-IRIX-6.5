
# Makefile for the Linked List Library
#
# Chris Cannam, February 1995


# # These three should have been exported from the superior make
#
# ROSEGARDEN	= /usr/people/cannam/rosegarden
# SYSINCDIRS	= -I/usr/include -I/usr/include/X11 -I/usr/include/X11/Xaw -I/usr/include/X11/bitmaps
# DEFINES		= -DFUNCPROTO -DHAVE_SPECIALIST_MALLOC_LIBRARY

INTERLOCK	= $(ROSEGARDEN)/interlock
YAWN		= $(ROSEGARDEN)/yawn
LISTS		= $(ROSEGARDEN)/lists
MIDI		= $(ROSEGARDEN)/midi
COMMON		= $(ROSEGARDEN)/common

INCDIRS		= $(SYSINCDIRS) -I$(ROSEGARDEN)/include -I$(COMMON)/bitmaps

SYSLIBS		= -lmalloc -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lLists -lMidi -lInterlock $(SYSLIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib $(LIBS)

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)
CCDEPFLAGS	=

TARGET		= $(LISTS)/lib/libLists.a
OBJECTS		= Lists.o

$(TARGET):	$(OBJECTS)
		rm -f $(TARGET)
		ar q $(TARGET) $(OBJECTS)
		$(RANLIB) $(TARGET)

.c.o:
		$(CC) -c $(CFLAGS) $<

clean:
		rm -f $(OBJECTS)

depend:		
		$(MAKEDEPEND) -- $(INCDIRS) $(DEFINES) $(CCDEPFLAGS) -- *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

Lists.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
Lists.o: /usr/include/standards.h /usr/include/sgidefs.h
Lists.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Lists.o: /usr/include/internal/stdlib_core.h
Lists.o: /root/build/rosegarden/include/Debug.h
Lists.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Lists.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Lists.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Lists.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Lists.o: /usr/include/internal/string_core.h
Lists.o: /root/build/rosegarden/include/Lists.h

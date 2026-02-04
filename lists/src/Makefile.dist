
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
		$(MAKEDEPEND) -- $(CFLAGS) -- *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

Lists.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
Lists.o: /usr/include/sys/cdefs.h /usr/include/stdlib.h
Lists.o: /usr/include/features.h
Lists.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Lists.o: /usr/include/errno.h /usr/include/linux/errno.h
Lists.o: /usr/include/asm/errno.h /usr/include/alloca.h
Lists.o: /home/cannam/rosegarden/include/Debug.h
Lists.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Lists.o: /usr/include/signal.h /usr/include/sys/types.h
Lists.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Lists.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Lists.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Lists.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Lists.o: /usr/include/bytesex.h /usr/include/string.h
Lists.o: /home/cannam/rosegarden/include/Lists.h

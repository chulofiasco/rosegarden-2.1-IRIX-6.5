
# Makefile for the Midi library
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

SYSLDFLAGS	= -L/usr/lib -L/usr/lib/X11
SYSLIBS		= -lmalloc -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lLists -lMidi -lInterlock $(SYSLIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib $(LIBS)

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)

TARGET		= $(MIDI)/lib/libMidi.a
OBJECTS		= ErrorHandler.o File.o VarLenNums.o Event.o BHeap.o Track.o


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

BHeap.o: /usr/include/stdlib.h /usr/include/features.h
BHeap.o: /usr/include/sys/cdefs.h
BHeap.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
BHeap.o: /usr/include/errno.h /usr/include/linux/errno.h
BHeap.o: /usr/include/asm/errno.h /usr/include/alloca.h
BHeap.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
BHeap.o: /home/cannam/rosegarden/include/MidiBHeap.h
BHeap.o: /home/cannam/rosegarden/include/Debug.h
BHeap.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
BHeap.o: /usr/include/signal.h /usr/include/sys/types.h
BHeap.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
BHeap.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
BHeap.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
BHeap.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
BHeap.o: /usr/include/bytesex.h /usr/include/string.h /usr/include/stdio.h
BHeap.o: /usr/include/libio.h /usr/include/_G_config.h
ErrorHandler.o: /usr/include/stdio.h /usr/include/libio.h
ErrorHandler.o: /usr/include/_G_config.h
ErrorHandler.o: /home/cannam/rosegarden/include/Yawn.h
ErrorHandler.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
ErrorHandler.o: /usr/X11R6/include/X11/Intrinsic.h
ErrorHandler.o: /usr/X11R6/include/X11/Xlib.h /usr/include/sys/types.h
ErrorHandler.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
ErrorHandler.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
ErrorHandler.o: /usr/include/sys/bitypes.h /usr/X11R6/include/X11/X.h
ErrorHandler.o: /usr/X11R6/include/X11/Xfuncproto.h
ErrorHandler.o: /usr/X11R6/include/X11/Xosdefs.h
ErrorHandler.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
ErrorHandler.o: /usr/X11R6/include/X11/Xutil.h
ErrorHandler.o: /usr/X11R6/include/X11/Xresource.h /usr/include/string.h
ErrorHandler.o: /usr/include/features.h /usr/include/sys/cdefs.h
ErrorHandler.o: /usr/X11R6/include/X11/Core.h
ErrorHandler.o: /usr/X11R6/include/X11/Composite.h
ErrorHandler.o: /usr/X11R6/include/X11/Constraint.h
ErrorHandler.o: /usr/X11R6/include/X11/Object.h
ErrorHandler.o: /usr/X11R6/include/X11/RectObj.h
ErrorHandler.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
ErrorHandler.o: /home/cannam/rosegarden/include/MidiFile.h
ErrorHandler.o: /home/cannam/rosegarden/include/MidiEvent.h
ErrorHandler.o: /home/cannam/rosegarden/include/MidiConsts.h
ErrorHandler.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
ErrorHandler.o: /usr/include/errno.h /usr/include/linux/errno.h
ErrorHandler.o: /usr/include/asm/errno.h /usr/include/alloca.h
ErrorHandler.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
ErrorHandler.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
ErrorHandler.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
ErrorHandler.o: /usr/include/linux/uio.h /usr/include/linux/in.h
ErrorHandler.o: /usr/include/asm/byteorder.h
ErrorHandler.o: /home/cannam/rosegarden/include/Debug.h
ErrorHandler.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
ErrorHandler.o: /usr/include/signal.h /usr/include/linux/signal.h
ErrorHandler.o: /usr/include/asm/signal.h /usr/include/ctype.h
ErrorHandler.o: /usr/include/endian.h /usr/include/bytesex.h
Event.o: /home/cannam/rosegarden/include/MidiEvent.h
Event.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Event.o: /usr/include/sys/types.h /usr/include/linux/types.h
Event.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Event.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Event.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Event.o: /usr/X11R6/include/X11/Xosdefs.h
Event.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Event.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
Event.o: /usr/include/string.h /usr/include/features.h
Event.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
Event.o: /usr/X11R6/include/X11/Composite.h
Event.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Event.o: /usr/X11R6/include/X11/RectObj.h
Event.o: /home/cannam/rosegarden/include/MidiConsts.h
Event.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Event.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/stdlib.h
Event.o: /usr/include/errno.h /usr/include/linux/errno.h
Event.o: /usr/include/asm/errno.h /usr/include/alloca.h
Event.o: /home/cannam/rosegarden/include/Debug.h
Event.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Event.o: /usr/include/signal.h /usr/include/linux/signal.h
Event.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Event.o: /usr/include/bytesex.h
Event.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
File.o: /home/cannam/rosegarden/include/MidiFile.h
File.o: /home/cannam/rosegarden/include/MidiEvent.h
File.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
File.o: /usr/include/sys/types.h /usr/include/linux/types.h
File.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
File.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
File.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
File.o: /usr/X11R6/include/X11/Xosdefs.h
File.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
File.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
File.o: /usr/include/string.h /usr/include/features.h
File.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
File.o: /usr/X11R6/include/X11/Composite.h
File.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
File.o: /usr/X11R6/include/X11/RectObj.h
File.o: /home/cannam/rosegarden/include/MidiConsts.h
File.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
File.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/stdlib.h
File.o: /usr/include/errno.h /usr/include/linux/errno.h
File.o: /usr/include/asm/errno.h /usr/include/alloca.h
File.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
File.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
File.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
File.o: /usr/include/linux/uio.h /usr/include/linux/in.h
File.o: /usr/include/asm/byteorder.h
File.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
File.o: /home/cannam/rosegarden/include/MidiVarLenNums.h
File.o: /home/cannam/rosegarden/include/Debug.h
File.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
File.o: /usr/include/signal.h /usr/include/linux/signal.h
File.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
File.o: /usr/include/bytesex.h
Track.o: /home/cannam/rosegarden/include/MidiConsts.h
Track.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Track.o: /home/cannam/rosegarden/include/MidiFile.h
Track.o: /home/cannam/rosegarden/include/MidiEvent.h
Track.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Track.o: /usr/include/sys/types.h /usr/include/linux/types.h
Track.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Track.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Track.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Track.o: /usr/X11R6/include/X11/Xosdefs.h
Track.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Track.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
Track.o: /usr/include/string.h /usr/include/features.h
Track.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
Track.o: /usr/X11R6/include/X11/Composite.h
Track.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Track.o: /usr/X11R6/include/X11/RectObj.h
Track.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Track.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/stdlib.h
Track.o: /usr/include/errno.h /usr/include/linux/errno.h
Track.o: /usr/include/asm/errno.h /usr/include/alloca.h
Track.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Track.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Track.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Track.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Track.o: /usr/include/asm/byteorder.h
Track.o: /home/cannam/rosegarden/include/MidiTrack.h
Track.o: /home/cannam/rosegarden/include/MidiBHeap.h
Track.o: /home/cannam/rosegarden/include/Debug.h
Track.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Track.o: /usr/include/signal.h /usr/include/linux/signal.h
Track.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Track.o: /usr/include/bytesex.h
VarLenNums.o: /home/cannam/rosegarden/include/Debug.h
VarLenNums.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
VarLenNums.o: /usr/include/signal.h /usr/include/features.h
VarLenNums.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
VarLenNums.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
VarLenNums.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
VarLenNums.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
VarLenNums.o: /usr/include/asm/signal.h /usr/include/ctype.h
VarLenNums.o: /usr/include/endian.h /usr/include/bytesex.h
VarLenNums.o: /usr/include/string.h
VarLenNums.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
VarLenNums.o: /usr/include/stdio.h /usr/include/libio.h
VarLenNums.o: /usr/include/_G_config.h
VarLenNums.o: /home/cannam/rosegarden/include/MidiConsts.h
VarLenNums.o: /home/cannam/rosegarden/include/MidiVarLenNums.h

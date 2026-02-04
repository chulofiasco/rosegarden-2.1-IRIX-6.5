
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
CCDEPFLAGS	=

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
		$(MAKEDEPEND) -- $(INCDIRS) $(DEFINES) $(CCDEPFLAGS) -- *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

BHeap.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
BHeap.o: /usr/include/standards.h /usr/include/sgidefs.h
BHeap.o: /usr/include/internal/sgimacros.h
BHeap.o: /root/build/rosegarden/include/MidiErrorHandler.h
BHeap.o: /root/build/rosegarden/include/MidiBHeap.h
BHeap.o: /root/build/rosegarden/include/Debug.h
BHeap.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
BHeap.o: /usr/include/signal.h /usr/include/internal/signal_core.h
BHeap.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
BHeap.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
BHeap.o: /usr/include/internal/string_core.h /usr/include/stdio.h
BHeap.o: /usr/include/internal/stdio_core.h
ErrorHandler.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
ErrorHandler.o: /usr/include/standards.h /usr/include/sgidefs.h
ErrorHandler.o: /usr/include/internal/sgimacros.h
ErrorHandler.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
ErrorHandler.o: /usr/include/internal/stdarg_core.h
ErrorHandler.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
ErrorHandler.o: /usr/include/sys/types.h /usr/include/X11/X.h
ErrorHandler.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
ErrorHandler.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
ErrorHandler.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
ErrorHandler.o: /usr/include/string.h /usr/include/internal/string_core.h
ErrorHandler.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
ErrorHandler.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
ErrorHandler.o: /usr/include/X11/RectObj.h
ErrorHandler.o: /root/build/rosegarden/include/MidiErrorHandler.h
ErrorHandler.o: /root/build/rosegarden/include/MidiFile.h
ErrorHandler.o: /root/build/rosegarden/include/MidiEvent.h
ErrorHandler.o: /root/build/rosegarden/include/MidiConsts.h
ErrorHandler.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
ErrorHandler.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
ErrorHandler.o: /root/build/rosegarden/include/Debug.h
ErrorHandler.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
ErrorHandler.o: /usr/include/signal.h /usr/include/internal/signal_core.h
ErrorHandler.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
ErrorHandler.o: /usr/include/internal/locale_attr_core.h
Event.o: /root/build/rosegarden/include/MidiEvent.h
Event.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Event.o: /usr/include/sys/types.h /usr/include/standards.h
Event.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Event.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Event.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Event.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Event.o: /usr/include/X11/Xresource.h /usr/include/string.h
Event.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
Event.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Event.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Event.o: /root/build/rosegarden/include/MidiConsts.h
Event.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Event.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Event.o: /usr/include/internal/stdlib_core.h
Event.o: /root/build/rosegarden/include/Debug.h
Event.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Event.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Event.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Event.o: /usr/include/internal/locale_attr_core.h
Event.o: /root/build/rosegarden/include/MidiErrorHandler.h
File.o: /root/build/rosegarden/include/MidiFile.h
File.o: /root/build/rosegarden/include/MidiEvent.h
File.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
File.o: /usr/include/sys/types.h /usr/include/standards.h
File.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
File.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
File.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
File.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
File.o: /usr/include/X11/Xresource.h /usr/include/string.h
File.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
File.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
File.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
File.o: /root/build/rosegarden/include/MidiConsts.h
File.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
File.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
File.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
File.o: /root/build/rosegarden/include/MidiErrorHandler.h
File.o: /root/build/rosegarden/include/MidiVarLenNums.h
File.o: /root/build/rosegarden/include/Debug.h
File.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
File.o: /usr/include/signal.h /usr/include/internal/signal_core.h
File.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
File.o: /usr/include/internal/locale_attr_core.h
Track.o: /root/build/rosegarden/include/MidiConsts.h
Track.o: /root/build/rosegarden/include/MidiErrorHandler.h
Track.o: /root/build/rosegarden/include/MidiFile.h
Track.o: /root/build/rosegarden/include/MidiEvent.h
Track.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Track.o: /usr/include/sys/types.h /usr/include/standards.h
Track.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Track.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Track.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Track.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Track.o: /usr/include/X11/Xresource.h /usr/include/string.h
Track.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
Track.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Track.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Track.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Track.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Track.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Track.o: /root/build/rosegarden/include/MidiTrack.h
Track.o: /root/build/rosegarden/include/MidiBHeap.h
Track.o: /root/build/rosegarden/include/Debug.h
Track.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Track.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Track.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Track.o: /usr/include/internal/locale_attr_core.h
VarLenNums.o: /root/build/rosegarden/include/Debug.h
VarLenNums.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
VarLenNums.o: /usr/include/signal.h /usr/include/internal/signal_core.h
VarLenNums.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
VarLenNums.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
VarLenNums.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
VarLenNums.o: /usr/include/string.h /usr/include/internal/string_core.h
VarLenNums.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
VarLenNums.o: /root/build/rosegarden/include/MidiConsts.h
VarLenNums.o: /root/build/rosegarden/include/MidiVarLenNums.h

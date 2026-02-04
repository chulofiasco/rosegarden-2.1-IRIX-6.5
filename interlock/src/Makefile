
# Makefile for the Interlock library
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

TARGET		= $(INTERLOCK)/lib/libInterlock.a
OBJECTS		= ILClient.o ILServer.o 


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

ILClient.o: /root/build/rosegarden/include/ILClient.h
ILClient.o: /root/build/rosegarden/include/ILTypes.h
ILClient.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
ILClient.o: /usr/include/sys/types.h /usr/include/standards.h
ILClient.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
ILClient.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
ILClient.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
ILClient.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
ILClient.o: /usr/include/X11/Xresource.h /usr/include/string.h
ILClient.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
ILClient.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
ILClient.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
ILClient.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
ILClient.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
ILClient.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
ILClient.o: /usr/include/X11/cursorfont.h
ILClient.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
ILClient.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
ILClient.o: /usr/include/internal/stdlib_core.h
ILClient.o: /root/build/rosegarden/include/Debug.h
ILClient.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
ILClient.o: /usr/include/signal.h /usr/include/internal/signal_core.h
ILClient.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
ILClient.o: /usr/include/internal/locale_attr_core.h
ILServer.o: /root/build/rosegarden/include/ILTypes.h
ILServer.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
ILServer.o: /usr/include/sys/types.h /usr/include/standards.h
ILServer.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
ILServer.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
ILServer.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
ILServer.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
ILServer.o: /usr/include/X11/Xresource.h /usr/include/string.h
ILServer.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
ILServer.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
ILServer.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
ILServer.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
ILServer.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
ILServer.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
ILServer.o: /usr/include/X11/cursorfont.h
ILServer.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
ILServer.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
ILServer.o: /usr/include/internal/stdlib_core.h
ILServer.o: /root/build/rosegarden/include/ILServer.h
ILServer.o: /root/build/rosegarden/include/Debug.h
ILServer.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
ILServer.o: /usr/include/signal.h /usr/include/internal/signal_core.h
ILServer.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
ILServer.o: /usr/include/internal/locale_attr_core.h /usr/include/unistd.h


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
		$(MAKEDEPEND) -- $(CFLAGS) -- *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

ILClient.o: /home/cannam/rosegarden/include/ILClient.h
ILClient.o: /home/cannam/rosegarden/include/ILTypes.h
ILClient.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
ILClient.o: /usr/include/sys/types.h /usr/include/linux/types.h
ILClient.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
ILClient.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
ILClient.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
ILClient.o: /usr/X11R6/include/X11/Xosdefs.h
ILClient.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
ILClient.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
ILClient.o: /usr/include/string.h /usr/include/features.h
ILClient.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
ILClient.o: /usr/X11R6/include/X11/Composite.h
ILClient.o: /usr/X11R6/include/X11/Constraint.h
ILClient.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
ILClient.o: /usr/X11R6/include/X11/StringDefs.h
ILClient.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
ILClient.o: /usr/X11R6/include/X11/SM/SM.h
ILClient.o: /usr/X11R6/include/X11/ICE/ICElib.h
ILClient.o: /usr/X11R6/include/X11/ICE/ICE.h
ILClient.o: /usr/X11R6/include/X11/cursorfont.h
ILClient.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
ILClient.o: /usr/include/libio.h /usr/include/_G_config.h
ILClient.o: /usr/include/stdlib.h /usr/include/errno.h
ILClient.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
ILClient.o: /usr/include/alloca.h /home/cannam/rosegarden/include/Debug.h
ILClient.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
ILClient.o: /usr/include/signal.h /usr/include/linux/signal.h
ILClient.o: /usr/include/asm/signal.h /usr/include/ctype.h
ILClient.o: /usr/include/endian.h /usr/include/bytesex.h
ILServer.o: /home/cannam/rosegarden/include/ILTypes.h
ILServer.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
ILServer.o: /usr/include/sys/types.h /usr/include/linux/types.h
ILServer.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
ILServer.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
ILServer.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
ILServer.o: /usr/X11R6/include/X11/Xosdefs.h
ILServer.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
ILServer.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
ILServer.o: /usr/include/string.h /usr/include/features.h
ILServer.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
ILServer.o: /usr/X11R6/include/X11/Composite.h
ILServer.o: /usr/X11R6/include/X11/Constraint.h
ILServer.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
ILServer.o: /usr/X11R6/include/X11/StringDefs.h
ILServer.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
ILServer.o: /usr/X11R6/include/X11/SM/SM.h
ILServer.o: /usr/X11R6/include/X11/ICE/ICElib.h
ILServer.o: /usr/X11R6/include/X11/ICE/ICE.h
ILServer.o: /usr/X11R6/include/X11/cursorfont.h
ILServer.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
ILServer.o: /usr/include/libio.h /usr/include/_G_config.h
ILServer.o: /usr/include/stdlib.h /usr/include/errno.h
ILServer.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
ILServer.o: /usr/include/alloca.h /home/cannam/rosegarden/include/ILServer.h
ILServer.o: /home/cannam/rosegarden/include/Debug.h
ILServer.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
ILServer.o: /usr/include/signal.h /usr/include/linux/signal.h
ILServer.o: /usr/include/asm/signal.h /usr/include/ctype.h
ILServer.o: /usr/include/endian.h /usr/include/bytesex.h
ILServer.o: /usr/include/unistd.h /usr/include/posix_opt.h
ILServer.o: /usr/include/gnu/types.h /usr/include/confname.h

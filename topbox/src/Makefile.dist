
# Makefile for the Rosegarden Top Box
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

SYSLIBS		= -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lMidi -lInterlock -lLists $(SYSLIBS) $(SYSEXTRALIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib $(LIBS)

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)

TARGET		= $(ROSEGARDEN)/bin/rosegarden
OBJECTS		= Main.o Remote.o Visuals.o Widgets.o


$(TARGET):	$(OBJECTS)
		rm -f $(TARGET)
		$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

.c.o:
		$(CC) -c $(CFLAGS) $<

clean:
		rm -f $(OBJECTS)

depend:		
		$(MAKEDEPEND) -- $(CFLAGS) -- *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

Main.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Main.o: /usr/include/signal.h /usr/include/features.h
Main.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Main.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Main.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Main.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Main.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Main.o: /usr/include/bytesex.h /usr/include/string.h
Main.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Main.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Main.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Main.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Main.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Main.o: /usr/X11R6/include/X11/Composite.h
Main.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Main.o: /usr/X11R6/include/X11/RectObj.h /usr/X11R6/include/X11/StringDefs.h
Main.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Main.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
Main.o: /usr/X11R6/include/X11/ICE/ICE.h /usr/X11R6/include/X11/cursorfont.h
Main.o: General.h /usr/include/stdio.h /usr/include/libio.h
Main.o: /usr/include/_G_config.h /usr/include/stdlib.h /usr/include/errno.h
Main.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Main.o: /usr/include/unistd.h /usr/include/posix_opt.h
Main.o: /usr/include/gnu/types.h /usr/include/confname.h
Main.o: /home/cannam/rosegarden/include/Debug.h
Main.o: /home/cannam/rosegarden/include/ServiceNames.h Resources.h
Main.o: Fallbacks.h Widgets.h Visuals.h Remote.h
Main.o: /home/cannam/rosegarden/include/Yawn.h
Main.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Main.o: /home/cannam/rosegarden/include/YHelp.h
Main.o: /home/cannam/rosegarden/include/ILServer.h
Main.o: /home/cannam/rosegarden/include/ILTypes.h
Main.o: /home/cannam/rosegarden/include/Lists.h
Remote.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Remote.o: /usr/include/signal.h /usr/include/features.h
Remote.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Remote.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Remote.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Remote.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Remote.o: /usr/include/asm/signal.h /usr/include/ctype.h
Remote.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Remote.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Remote.o: /usr/X11R6/include/X11/Xatom.h /usr/X11R6/include/X11/Intrinsic.h
Remote.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
Remote.o: /usr/X11R6/include/X11/Xfuncproto.h
Remote.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Remote.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Remote.o: /usr/X11R6/include/X11/Composite.h
Remote.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Remote.o: /usr/X11R6/include/X11/RectObj.h
Remote.o: /home/cannam/rosegarden/include/ILClient.h
Remote.o: /home/cannam/rosegarden/include/ILTypes.h
Remote.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Remote.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Remote.o: /usr/X11R6/include/X11/ICE/ICElib.h
Remote.o: /usr/X11R6/include/X11/ICE/ICE.h
Remote.o: /usr/X11R6/include/X11/cursorfont.h
Remote.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Remote.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/stdlib.h
Remote.o: /usr/include/errno.h /usr/include/linux/errno.h
Remote.o: /usr/include/asm/errno.h /usr/include/unistd.h
Remote.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
Remote.o: /usr/include/confname.h /home/cannam/rosegarden/include/Debug.h
Remote.o: General.h /home/cannam/rosegarden/include/ServiceNames.h Remote.h
Visuals.o: General.h /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Visuals.o: /usr/include/signal.h /usr/include/features.h
Visuals.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Visuals.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Visuals.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Visuals.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Visuals.o: /usr/include/asm/signal.h /usr/include/ctype.h
Visuals.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Visuals.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Visuals.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
Visuals.o: /usr/include/stdlib.h /usr/include/errno.h
Visuals.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Visuals.o: /usr/include/unistd.h /usr/include/posix_opt.h
Visuals.o: /usr/include/gnu/types.h /usr/include/confname.h
Visuals.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xlib.h
Visuals.o: /usr/X11R6/include/X11/Xfuncproto.h
Visuals.o: /usr/X11R6/include/X11/Xosdefs.h
Visuals.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xutil.h
Visuals.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Visuals.o: /usr/X11R6/include/X11/Composite.h
Visuals.o: /usr/X11R6/include/X11/Constraint.h
Visuals.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Visuals.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Visuals.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Visuals.o: /usr/X11R6/include/X11/ICE/ICElib.h
Visuals.o: /usr/X11R6/include/X11/ICE/ICE.h
Visuals.o: /home/cannam/rosegarden/include/Debug.h
Visuals.o: /home/cannam/rosegarden/include/ServiceNames.h Visuals.h
Visuals.o: /home/cannam/rosegarden/include/Yawn.h
Visuals.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rose.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rose_mask.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/grey.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/lightgrey.xbm
Widgets.o: General.h /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Widgets.o: /usr/include/signal.h /usr/include/features.h
Widgets.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Widgets.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Widgets.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Widgets.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Widgets.o: /usr/include/asm/signal.h /usr/include/ctype.h
Widgets.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Widgets.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Widgets.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
Widgets.o: /usr/include/stdlib.h /usr/include/errno.h
Widgets.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Widgets.o: /usr/include/unistd.h /usr/include/posix_opt.h
Widgets.o: /usr/include/gnu/types.h /usr/include/confname.h
Widgets.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xlib.h
Widgets.o: /usr/X11R6/include/X11/Xfuncproto.h
Widgets.o: /usr/X11R6/include/X11/Xosdefs.h
Widgets.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xutil.h
Widgets.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Widgets.o: /usr/X11R6/include/X11/Composite.h
Widgets.o: /usr/X11R6/include/X11/Constraint.h
Widgets.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Widgets.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Widgets.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Widgets.o: /usr/X11R6/include/X11/ICE/ICElib.h
Widgets.o: /usr/X11R6/include/X11/ICE/ICE.h
Widgets.o: /home/cannam/rosegarden/include/Debug.h
Widgets.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h
Widgets.o: /home/cannam/rosegarden/include/ILTypes.h
Widgets.o: /usr/X11R6/include/X11/cursorfont.h
Widgets.o: /home/cannam/rosegarden/include/Lists.h
Widgets.o: /home/cannam/rosegarden/include/ILServer.h
Widgets.o: /home/cannam/rosegarden/include/ILClient.h
Widgets.o: /home/cannam/rosegarden/include/Yawn.h
Widgets.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Widgets.o: /home/cannam/rosegarden/include/YHelp.h Visuals.h
Widgets.o: /home/cannam/rosegarden/include/Version.h /usr/include/sys/stat.h
Widgets.o: /usr/include/linux/stat.h /usr/X11R6/include/X11/Xaw/Form.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Box.h
Widgets.o: /usr/X11R6/include/X11/Xmu/Converters.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Command.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Label.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Simple.h

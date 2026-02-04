
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
CCDEPFLAGS	=

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
		$(MAKEDEPEND) -- $(INCDIRS) $(DEFINES) $(CCDEPFLAGS) -- *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

Main.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Main.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Main.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Main.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Main.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Main.o: /usr/include/string.h /usr/include/internal/string_core.h
Main.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Main.o: /usr/include/sys/types.h /usr/include/X11/X.h
Main.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Main.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Main.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Main.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Main.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Main.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Main.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Main.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Main.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h General.h
Main.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
Main.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Main.o: /usr/include/errno.h /usr/include/internal/errno_core.h
Main.o: /usr/include/sys/errno.h /usr/include/unistd.h
Main.o: /root/build/rosegarden/include/Debug.h
Main.o: /root/build/rosegarden/include/ServiceNames.h Resources.h Fallbacks.h
Main.o: Widgets.h Visuals.h Remote.h /root/build/rosegarden/include/Yawn.h
Main.o: /usr/include/stdarg.h /usr/include/internal/stdarg_core.h
Main.o: /root/build/rosegarden/include/YHelp.h
Main.o: /root/build/rosegarden/include/ILServer.h
Main.o: /root/build/rosegarden/include/ILTypes.h
Main.o: /root/build/rosegarden/include/Lists.h
Remote.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Remote.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Remote.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Remote.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Remote.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Remote.o: /usr/include/string.h /usr/include/internal/string_core.h
Remote.o: /usr/include/X11/Xatom.h /usr/include/X11/Intrinsic.h
Remote.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Remote.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Remote.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Remote.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Remote.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Remote.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Remote.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Remote.o: /root/build/rosegarden/include/ILClient.h
Remote.o: /root/build/rosegarden/include/ILTypes.h
Remote.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
Remote.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
Remote.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
Remote.o: /usr/include/X11/cursorfont.h
Remote.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Remote.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Remote.o: /usr/include/internal/stdlib_core.h /usr/include/unistd.h
Remote.o: /root/build/rosegarden/include/Debug.h General.h
Remote.o: /usr/include/errno.h /usr/include/internal/errno_core.h
Remote.o: /usr/include/sys/errno.h
Remote.o: /root/build/rosegarden/include/ServiceNames.h Remote.h
Visuals.o: General.h /root/build/rosegarden/include/SysDeps.h ../../config.h
Visuals.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Visuals.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Visuals.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Visuals.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Visuals.o: /usr/include/string.h /usr/include/internal/string_core.h
Visuals.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
Visuals.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Visuals.o: /usr/include/errno.h /usr/include/internal/errno_core.h
Visuals.o: /usr/include/sys/errno.h /usr/include/unistd.h
Visuals.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
Visuals.o: /usr/include/sys/types.h /usr/include/X11/Xfuncproto.h
Visuals.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Visuals.o: /usr/include/internal/stddef_core.h /usr/include/X11/Intrinsic.h
Visuals.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Visuals.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Visuals.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Visuals.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Visuals.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Visuals.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Visuals.o: /usr/include/X11/ICE/ICE.h /root/build/rosegarden/include/Debug.h
Visuals.o: /root/build/rosegarden/include/ServiceNames.h Visuals.h
Visuals.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Visuals.o: /usr/include/internal/stdarg_core.h
Visuals.o: /root/build/rosegarden/common/bitmaps/rose.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rose_mask.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/grey.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/lightgrey.xbm
Widgets.o: General.h /root/build/rosegarden/include/SysDeps.h ../../config.h
Widgets.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Widgets.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Widgets.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Widgets.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Widgets.o: /usr/include/string.h /usr/include/internal/string_core.h
Widgets.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
Widgets.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Widgets.o: /usr/include/errno.h /usr/include/internal/errno_core.h
Widgets.o: /usr/include/sys/errno.h /usr/include/unistd.h
Widgets.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
Widgets.o: /usr/include/sys/types.h /usr/include/X11/Xfuncproto.h
Widgets.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Widgets.o: /usr/include/internal/stddef_core.h /usr/include/X11/Intrinsic.h
Widgets.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Widgets.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Widgets.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Widgets.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Widgets.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Widgets.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Widgets.o: /usr/include/X11/ICE/ICE.h /root/build/rosegarden/include/Debug.h
Widgets.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h
Widgets.o: /root/build/rosegarden/include/ILTypes.h
Widgets.o: /usr/include/X11/cursorfont.h
Widgets.o: /root/build/rosegarden/include/Lists.h
Widgets.o: /root/build/rosegarden/include/ILServer.h
Widgets.o: /root/build/rosegarden/include/ILClient.h
Widgets.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Widgets.o: /usr/include/internal/stdarg_core.h
Widgets.o: /root/build/rosegarden/include/YHelp.h Visuals.h
Widgets.o: /root/build/rosegarden/include/Version.h /usr/include/sys/stat.h
Widgets.o: /usr/include/sys/timespec.h /usr/include/X11/Xaw/Form.h
Widgets.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xmu/Converters.h
Widgets.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Widgets.o: /usr/include/X11/Xaw/Simple.h

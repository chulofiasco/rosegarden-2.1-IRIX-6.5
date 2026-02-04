
# Makefile for the YAWN Library
#
# Chris Cannam, February 1995

# This needs a bit of care, because YHelp.c and YSmeHack.c are
# K&R whilst the other files are ANSI.  If you get loads of errors,
# that could be why.


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

INCDIRS		= $(SYSINCDIRS) -I$(ROSEGARDEN)/include -I$(COMMON)/bitmaps -I../bitmaps

SYSLIBS		= -lmalloc -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lLists -lMidi -lInterlock $(SYSLIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib $(LIBS)

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)
CCDEPFLAGS	=

TARGET		= $(YAWN)/lib/libY.a
OBJECTS		= Yawn.o YFile.o YHelp.o YMenu.o YSmeBSB.o YPop.o YTextEntry.o YToolbar.o YOptionMenu.o

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

# DO NOT DELETE

YFile.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
YFile.o: /usr/include/signal.h /usr/include/internal/signal_core.h
YFile.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
YFile.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
YFile.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
YFile.o: /usr/include/string.h /usr/include/internal/string_core.h
YFile.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
YFile.o: /usr/include/errno.h /usr/include/internal/errno_core.h
YFile.o: /usr/include/sys/errno.h /usr/include/stdlib.h
YFile.o: /usr/include/internal/stdlib_core.h /usr/include/unistd.h
YFile.o: /usr/include/dirent.h /usr/include/sys/types.h
YFile.o: /usr/include/sys/dirent.h /usr/include/sys/stat.h
YFile.o: /usr/include/sys/timespec.h /usr/include/fcntl.h
YFile.o: /usr/include/sys/fcntl.h /usr/include/X11/Intrinsic.h
YFile.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
YFile.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
YFile.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
YFile.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
YFile.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
YFile.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
YFile.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
YFile.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
YFile.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
YFile.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
YFile.o: /usr/include/X11/Xaw/AsciiText.h /usr/include/X11/Xaw/Text.h
YFile.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
YFile.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
YFile.o: /usr/include/X11/Xaw/AsciiSrc.h /usr/include/X11/Xaw/AsciiSink.h
YFile.o: /usr/include/X11/Xaw/MultiSrc.h /usr/include/X11/Xaw/Box.h
YFile.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
YFile.o: /usr/include/X11/Xaw/Dialog.h /usr/include/X11/Xaw/Form.h
YFile.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/Paned.h
YFile.o: /usr/include/X11/Xaw/Scrollbar.h /usr/include/X11/Xaw/Toggle.h
YFile.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
YFile.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
YFile.o: /usr/include/internal/stdarg_core.h
YFile.o: /root/build/rosegarden/include/Debug.h
YHelp.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
YHelp.o: /usr/include/internal/sgimacros.h /usr/include/standards.h
YHelp.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
YHelp.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
YHelp.o: /usr/include/string.h /usr/include/internal/string_core.h
YHelp.o: /usr/include/errno.h /usr/include/internal/errno_core.h
YHelp.o: /usr/include/sys/errno.h /usr/include/stdlib.h
YHelp.o: /usr/include/internal/stdlib_core.h /usr/include/unistd.h
YHelp.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
YHelp.o: /usr/include/signal.h /usr/include/internal/signal_core.h
YHelp.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
YHelp.o: /usr/include/sys/types.h /usr/include/X11/X.h
YHelp.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
YHelp.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
YHelp.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
YHelp.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
YHelp.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
YHelp.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
YHelp.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
YHelp.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
YHelp.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
YHelp.o: /usr/include/X11/Xaw/AsciiText.h /usr/include/X11/Xaw/Text.h
YHelp.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
YHelp.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
YHelp.o: /usr/include/X11/Xaw/AsciiSrc.h /usr/include/X11/Xaw/AsciiSink.h
YHelp.o: /usr/include/X11/Xaw/MultiSrc.h /usr/include/X11/Xaw/Box.h
YHelp.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
YHelp.o: /usr/include/X11/Xaw/Dialog.h /usr/include/X11/Xaw/Form.h
YHelp.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/Paned.h
YHelp.o: /usr/include/X11/Xaw/Scrollbar.h /usr/include/X11/Xaw/Toggle.h
YHelp.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
YHelp.o: /root/build/rosegarden/include/Debug.h
YHelp.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
YHelp.o: /usr/include/internal/stdarg_core.h
YHelp.o: /root/build/rosegarden/include/YHelp.h
YHelp.o: /root/build/rosegarden/common/bitmaps/right.xbm
YHelp.o: /root/build/rosegarden/common/bitmaps/left.xbm
YHelp.o: /root/build/rosegarden/common/bitmaps/up.xbm
YMenu.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
YMenu.o: /usr/include/standards.h /usr/include/sgidefs.h
YMenu.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
YMenu.o: /usr/include/internal/stdlib_core.h /usr/include/string.h
YMenu.o: /usr/include/internal/string_core.h /usr/include/ctype.h
YMenu.o: /usr/include/internal/ctype_core.h
YMenu.o: /usr/include/internal/locale_attr_core.h
YMenu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
YMenu.o: /usr/include/sys/types.h /usr/include/X11/X.h
YMenu.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
YMenu.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
YMenu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
YMenu.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
YMenu.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
YMenu.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
YMenu.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
YMenu.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
YMenu.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/Xaw/MenuButton.h
YMenu.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
YMenu.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
YMenu.o: /usr/include/X11/Xaw/SimpleMenu.h /usr/include/X11/Xaw/Sme.h
YMenu.o: /usr/include/X11/Xaw/SmeLine.h /usr/include/X11/Xaw/AsciiText.h
YMenu.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
YMenu.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
YMenu.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
YMenu.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
YMenu.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/SmeBSB.h
YMenu.o: /root/build/rosegarden/include/Debug.h
YMenu.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
YMenu.o: /usr/include/signal.h /usr/include/internal/signal_core.h YToolbar.h
YMenu.o: /root/build/rosegarden/common/bitmaps/darkgrey.xbm
YOptionMenu.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
YOptionMenu.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
YOptionMenu.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
YOptionMenu.o: /usr/include/stdarg.h /usr/include/internal/stdarg_core.h
YOptionMenu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
YOptionMenu.o: /usr/include/sys/types.h /usr/include/X11/X.h
YOptionMenu.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
YOptionMenu.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
YOptionMenu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
YOptionMenu.o: /usr/include/string.h /usr/include/internal/string_core.h
YOptionMenu.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
YOptionMenu.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
YOptionMenu.o: /usr/include/X11/RectObj.h /usr/include/stdlib.h
YOptionMenu.o: /usr/include/internal/stdlib_core.h
YOptionMenu.o: /usr/include/X11/StringDefs.h
YOptionMenu.o: /root/build/rosegarden/include/Debug.h
YOptionMenu.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
YOptionMenu.o: /usr/include/signal.h /usr/include/internal/signal_core.h
YOptionMenu.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
YOptionMenu.o: /usr/include/internal/locale_attr_core.h
YPop.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
YPop.o: /usr/include/sys/types.h /usr/include/standards.h
YPop.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
YPop.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
YPop.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
YPop.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
YPop.o: /usr/include/X11/Xresource.h /usr/include/string.h
YPop.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
YPop.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
YPop.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
YPop.o: /usr/include/X11/StringDefs.h /root/build/rosegarden/include/Yawn.h
YPop.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
YPop.o: /usr/include/stdarg.h /usr/include/internal/stdarg_core.h
YPop.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
YPop.o: /usr/include/signal.h /usr/include/internal/signal_core.h
YPop.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
YPop.o: /usr/include/internal/locale_attr_core.h
YPop.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
YPop.o: /usr/include/internal/stdlib_core.h
YPop.o: /root/build/rosegarden/include/Debug.h YPop.h
YSmeBSB.o: /usr/include/X11/IntrinsicP.h /usr/include/X11/Intrinsic.h
YSmeBSB.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
YSmeBSB.o: /usr/include/standards.h /usr/include/sgidefs.h
YSmeBSB.o: /usr/include/internal/sgimacros.h /usr/include/X11/X.h
YSmeBSB.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
YSmeBSB.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
YSmeBSB.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
YSmeBSB.o: /usr/include/string.h /usr/include/internal/string_core.h
YSmeBSB.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
YSmeBSB.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
YSmeBSB.o: /usr/include/X11/RectObj.h /usr/include/X11/CoreP.h
YSmeBSB.o: /usr/include/X11/CompositeP.h /usr/include/X11/ConstrainP.h
YSmeBSB.o: /usr/include/X11/ObjectP.h /usr/include/X11/RectObjP.h
YSmeBSB.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xos.h
YSmeBSB.o: /usr/include/strings.h /usr/include/fcntl.h
YSmeBSB.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
YSmeBSB.o: /usr/include/sys/time.h /usr/include/X11/Xmu/Drawing.h
YSmeBSB.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
YSmeBSB.o: /usr/include/X11/Xaw/XawInit.h /usr/include/X11/Xaw/SimpleMenu.h
YSmeBSB.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
YSmeBSB.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
YSmeBSB.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/Xmu/Converters.h
YSmeBSB.o: /usr/include/X11/Xaw/Cardinals.h YSmeBSBP.h
YSmeBSB.o: /usr/include/X11/Xaw/SmeP.h /usr/include/X11/Xaw/Sme.h
YSmeBSB.o: /root/build/rosegarden/include/YSmeBSB.h
YTextEntry.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
YTextEntry.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
YTextEntry.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
YTextEntry.o: /usr/include/stdarg.h /usr/include/internal/stdarg_core.h
YTextEntry.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
YTextEntry.o: /usr/include/sys/types.h /usr/include/X11/X.h
YTextEntry.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
YTextEntry.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
YTextEntry.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
YTextEntry.o: /usr/include/string.h /usr/include/internal/string_core.h
YTextEntry.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
YTextEntry.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
YTextEntry.o: /usr/include/X11/RectObj.h /usr/include/stdlib.h
YTextEntry.o: /usr/include/internal/stdlib_core.h
YTextEntry.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/Form.h
YTextEntry.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
YTextEntry.o: /usr/include/X11/Xmu/Converters.h
YTextEntry.o: /usr/include/X11/Xaw/AsciiText.h /usr/include/X11/Xaw/Text.h
YTextEntry.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
YTextEntry.o: /usr/include/X11/Xaw/AsciiSrc.h
YTextEntry.o: /usr/include/X11/Xaw/AsciiSink.h
YTextEntry.o: /usr/include/X11/Xaw/MultiSrc.h
YTextEntry.o: /root/build/rosegarden/include/Debug.h
YTextEntry.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
YTextEntry.o: /usr/include/signal.h /usr/include/internal/signal_core.h
YTextEntry.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
YTextEntry.o: /usr/include/internal/locale_attr_core.h
YToolbar.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
YToolbar.o: /usr/include/signal.h /usr/include/internal/signal_core.h
YToolbar.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
YToolbar.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
YToolbar.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
YToolbar.o: /usr/include/string.h /usr/include/internal/string_core.h
YToolbar.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
YToolbar.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
YToolbar.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
YToolbar.o: /usr/include/sys/types.h /usr/include/X11/X.h
YToolbar.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
YToolbar.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
YToolbar.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
YToolbar.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
YToolbar.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
YToolbar.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
YToolbar.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
YToolbar.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
YToolbar.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/Xaw/Command.h
YToolbar.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
YToolbar.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Form.h
YToolbar.o: /root/build/rosegarden/include/Debug.h YToolbar.h
YToolbar.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
YToolbar.o: /usr/include/internal/stdarg_core.h YawnP.h
Yawn.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
Yawn.o: /usr/include/standards.h /usr/include/sgidefs.h
Yawn.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Yawn.o: /usr/include/internal/stdlib_core.h /usr/include/string.h
Yawn.o: /usr/include/internal/string_core.h /usr/include/stdarg.h
Yawn.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Yawn.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/X11/X.h
Yawn.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Yawn.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Yawn.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Yawn.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Yawn.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Yawn.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Yawn.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Yawn.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Yawn.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/Xaw/Box.h
Yawn.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Form.h
Yawn.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
Yawn.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xaw/Toggle.h
Yawn.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Repeater.h
Yawn.o: /usr/include/X11/Xaw/MenuButton.h /usr/include/X11/Xaw/AsciiText.h
Yawn.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Yawn.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Yawn.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Yawn.o: /root/build/rosegarden/include/Yawn.h
Yawn.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Yawn.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Yawn.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Yawn.o: /usr/include/internal/locale_attr_core.h
Yawn.o: /root/build/rosegarden/include/Debug.h YPop.h
Yawn.o: /root/build/rosegarden/common/bitmaps/grey.xbm
Yawn.o: /root/build/rosegarden/common/bitmaps/lightgrey.xbm
Yawn.o: /root/build/rosegarden/common/bitmaps/down.xbm
Yawn.o: /root/build/rosegarden/common/bitmaps/right.xbm
Yawn.o: ../bitmaps/left_arrow.xbm ../bitmaps/right_arrow.xbm
Yawn.o: ../bitmaps/up_arrow.xbm ../bitmaps/down_arrow.xbm
Yawn.o: /root/build/rosegarden/common/bitmaps/ticked.xbm
Yawn.o: /root/build/rosegarden/common/bitmaps/unticked.xbm

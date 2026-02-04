
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
		$(MAKEDEPEND) -- $(CFLAGS) -- *.c

# DO NOT DELETE

YFile.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
YFile.o: /usr/include/signal.h /usr/include/features.h
YFile.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
YFile.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
YFile.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
YFile.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
YFile.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
YFile.o: /usr/include/bytesex.h /usr/include/string.h
YFile.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YFile.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
YFile.o: /usr/include/errno.h /usr/include/linux/errno.h
YFile.o: /usr/include/asm/errno.h /usr/include/stdlib.h /usr/include/unistd.h
YFile.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
YFile.o: /usr/include/confname.h /usr/include/dirent.h
YFile.o: /usr/include/linux/limits.h /usr/include/linux/dirent.h
YFile.o: /usr/include/sys/stat.h /usr/include/linux/stat.h
YFile.o: /usr/include/fcntl.h /usr/include/linux/fcntl.h
YFile.o: /usr/include/asm/fcntl.h /usr/X11R6/include/X11/Intrinsic.h
YFile.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
YFile.o: /usr/X11R6/include/X11/Xfuncproto.h /usr/X11R6/include/X11/Xosdefs.h
YFile.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
YFile.o: /usr/X11R6/include/X11/Core.h /usr/X11R6/include/X11/Composite.h
YFile.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
YFile.o: /usr/X11R6/include/X11/RectObj.h /usr/X11R6/include/X11/StringDefs.h
YFile.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
YFile.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
YFile.o: /usr/X11R6/include/X11/ICE/ICE.h /usr/X11R6/include/X11/cursorfont.h
YFile.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
YFile.o: /usr/X11R6/include/X11/Xaw/Text.h
YFile.o: /usr/X11R6/include/X11/Xaw/Simple.h
YFile.o: /usr/X11R6/include/X11/Xmu/Converters.h
YFile.o: /usr/X11R6/include/X11/Xaw/TextSink.h
YFile.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
YFile.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
YFile.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
YFile.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
YFile.o: /usr/X11R6/include/X11/Xaw/Box.h
YFile.o: /usr/X11R6/include/X11/Xaw/Command.h
YFile.o: /usr/X11R6/include/X11/Xaw/Label.h
YFile.o: /usr/X11R6/include/X11/Xaw/Dialog.h
YFile.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/List.h
YFile.o: /usr/X11R6/include/X11/Xaw/Paned.h
YFile.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
YFile.o: /usr/X11R6/include/X11/Xaw/Toggle.h
YFile.o: /usr/X11R6/include/X11/Xaw/Viewport.h
YFile.o: /usr/X11R6/include/X11/Xaw/Reports.h
YFile.o: /home/cannam/rosegarden/include/Yawn.h
YFile.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
YFile.o: /home/cannam/rosegarden/include/Debug.h
YHelp.o: /usr/include/ctype.h /usr/include/features.h
YHelp.o: /usr/include/sys/cdefs.h /usr/include/endian.h
YHelp.o: /usr/include/bytesex.h /usr/include/stdio.h /usr/include/libio.h
YHelp.o: /usr/include/_G_config.h /usr/include/string.h
YHelp.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YHelp.o: /usr/include/errno.h /usr/include/linux/errno.h
YHelp.o: /usr/include/asm/errno.h /usr/include/stdlib.h /usr/include/unistd.h
YHelp.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
YHelp.o: /usr/include/confname.h /home/cannam/rosegarden/include/SysDeps.h
YHelp.o: ../../config.h /usr/include/signal.h /usr/include/sys/types.h
YHelp.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
YHelp.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
YHelp.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
YHelp.o: /usr/include/asm/signal.h /usr/X11R6/include/X11/Intrinsic.h
YHelp.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
YHelp.o: /usr/X11R6/include/X11/Xfuncproto.h /usr/X11R6/include/X11/Xosdefs.h
YHelp.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
YHelp.o: /usr/X11R6/include/X11/Core.h /usr/X11R6/include/X11/Composite.h
YHelp.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
YHelp.o: /usr/X11R6/include/X11/RectObj.h /usr/X11R6/include/X11/StringDefs.h
YHelp.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
YHelp.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
YHelp.o: /usr/X11R6/include/X11/ICE/ICE.h /usr/X11R6/include/X11/cursorfont.h
YHelp.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Text.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Simple.h
YHelp.o: /usr/X11R6/include/X11/Xmu/Converters.h
YHelp.o: /usr/X11R6/include/X11/Xaw/TextSink.h
YHelp.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
YHelp.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
YHelp.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
YHelp.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Box.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Command.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Label.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Dialog.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/List.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Paned.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Toggle.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Viewport.h
YHelp.o: /usr/X11R6/include/X11/Xaw/Reports.h
YHelp.o: /home/cannam/rosegarden/include/Debug.h
YHelp.o: /home/cannam/rosegarden/include/Yawn.h
YHelp.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
YHelp.o: /home/cannam/rosegarden/include/YHelp.h
YHelp.o: /home/cannam/rosegarden/common/bitmaps/right.xbm
YHelp.o: /home/cannam/rosegarden/common/bitmaps/left.xbm
YHelp.o: /home/cannam/rosegarden/common/bitmaps/up.xbm
YMenu.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
YMenu.o: /usr/include/stdlib.h /usr/include/features.h
YMenu.o: /usr/include/sys/cdefs.h
YMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YMenu.o: /usr/include/errno.h /usr/include/linux/errno.h
YMenu.o: /usr/include/asm/errno.h /usr/include/string.h /usr/include/ctype.h
YMenu.o: /usr/include/endian.h /usr/include/bytesex.h
YMenu.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
YMenu.o: /usr/include/sys/types.h /usr/include/linux/types.h
YMenu.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
YMenu.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
YMenu.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
YMenu.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
YMenu.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
YMenu.o: /usr/X11R6/include/X11/Composite.h
YMenu.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
YMenu.o: /usr/X11R6/include/X11/RectObj.h /usr/X11R6/include/X11/StringDefs.h
YMenu.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
YMenu.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
YMenu.o: /usr/X11R6/include/X11/ICE/ICE.h
YMenu.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
YMenu.o: /usr/X11R6/include/X11/Xaw/Command.h
YMenu.o: /usr/X11R6/include/X11/Xaw/Label.h
YMenu.o: /usr/X11R6/include/X11/Xaw/Simple.h
YMenu.o: /usr/X11R6/include/X11/Xmu/Converters.h
YMenu.o: /usr/X11R6/include/X11/Xaw/SimpleMenu.h
YMenu.o: /usr/X11R6/include/X11/Xaw/Sme.h
YMenu.o: /usr/X11R6/include/X11/Xaw/SmeLine.h
YMenu.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
YMenu.o: /usr/X11R6/include/X11/Xaw/Text.h
YMenu.o: /usr/X11R6/include/X11/Xaw/TextSink.h
YMenu.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
YMenu.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
YMenu.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
YMenu.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
YMenu.o: /home/cannam/rosegarden/include/Yawn.h
YMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
YMenu.o: /usr/X11R6/include/X11/Xaw/SmeBSB.h
YMenu.o: /home/cannam/rosegarden/include/Debug.h
YMenu.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
YMenu.o: /usr/include/signal.h /usr/include/linux/signal.h
YMenu.o: /usr/include/asm/signal.h YToolbar.h
YMenu.o: /home/cannam/rosegarden/common/bitmaps/darkgrey.xbm
YOptionMenu.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
YOptionMenu.o: /usr/include/libio.h /usr/include/_G_config.h
YOptionMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
YOptionMenu.o: /usr/X11R6/include/X11/Intrinsic.h
YOptionMenu.o: /usr/X11R6/include/X11/Xlib.h /usr/include/sys/types.h
YOptionMenu.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
YOptionMenu.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
YOptionMenu.o: /usr/include/sys/bitypes.h /usr/X11R6/include/X11/X.h
YOptionMenu.o: /usr/X11R6/include/X11/Xfuncproto.h
YOptionMenu.o: /usr/X11R6/include/X11/Xosdefs.h
YOptionMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YOptionMenu.o: /usr/X11R6/include/X11/Xutil.h
YOptionMenu.o: /usr/X11R6/include/X11/Xresource.h /usr/include/string.h
YOptionMenu.o: /usr/include/features.h /usr/include/sys/cdefs.h
YOptionMenu.o: /usr/X11R6/include/X11/Core.h
YOptionMenu.o: /usr/X11R6/include/X11/Composite.h
YOptionMenu.o: /usr/X11R6/include/X11/Constraint.h
YOptionMenu.o: /usr/X11R6/include/X11/Object.h
YOptionMenu.o: /usr/X11R6/include/X11/RectObj.h /usr/include/stdlib.h
YOptionMenu.o: /usr/include/errno.h /usr/include/linux/errno.h
YOptionMenu.o: /usr/include/asm/errno.h /usr/X11R6/include/X11/StringDefs.h
YOptionMenu.o: /home/cannam/rosegarden/include/Debug.h
YOptionMenu.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
YOptionMenu.o: /usr/include/signal.h /usr/include/linux/signal.h
YOptionMenu.o: /usr/include/asm/signal.h /usr/include/ctype.h
YOptionMenu.o: /usr/include/endian.h /usr/include/bytesex.h
YPop.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
YPop.o: /usr/include/sys/types.h /usr/include/linux/types.h
YPop.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
YPop.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
YPop.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
YPop.o: /usr/X11R6/include/X11/Xosdefs.h
YPop.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YPop.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
YPop.o: /usr/include/string.h /usr/include/features.h
YPop.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
YPop.o: /usr/X11R6/include/X11/Composite.h
YPop.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
YPop.o: /usr/X11R6/include/X11/RectObj.h /usr/X11R6/include/X11/StringDefs.h
YPop.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
YPop.o: /usr/include/libio.h /usr/include/_G_config.h
YPop.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
YPop.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
YPop.o: /usr/include/signal.h /usr/include/linux/signal.h
YPop.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
YPop.o: /usr/include/bytesex.h /home/cannam/rosegarden/include/Lists.h
YPop.o: /usr/include/stdlib.h /usr/include/errno.h /usr/include/linux/errno.h
YPop.o: /usr/include/asm/errno.h /home/cannam/rosegarden/include/Debug.h
YPop.o: YPop.h
YSmeBSB.o: /usr/X11R6/include/X11/IntrinsicP.h
YSmeBSB.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
YSmeBSB.o: /usr/include/sys/types.h /usr/include/linux/types.h
YSmeBSB.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
YSmeBSB.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
YSmeBSB.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
YSmeBSB.o: /usr/X11R6/include/X11/Xosdefs.h
YSmeBSB.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YSmeBSB.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
YSmeBSB.o: /usr/include/string.h /usr/include/features.h
YSmeBSB.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
YSmeBSB.o: /usr/X11R6/include/X11/Composite.h
YSmeBSB.o: /usr/X11R6/include/X11/Constraint.h
YSmeBSB.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
YSmeBSB.o: /usr/X11R6/include/X11/CoreP.h /usr/X11R6/include/X11/CompositeP.h
YSmeBSB.o: /usr/X11R6/include/X11/ConstrainP.h
YSmeBSB.o: /usr/X11R6/include/X11/ObjectP.h /usr/X11R6/include/X11/RectObjP.h
YSmeBSB.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Xos.h
YSmeBSB.o: /usr/include/fcntl.h /usr/include/gnu/types.h
YSmeBSB.o: /usr/include/linux/fcntl.h /usr/include/asm/fcntl.h
YSmeBSB.o: /usr/include/unistd.h /usr/include/posix_opt.h
YSmeBSB.o: /usr/include/confname.h /usr/include/sys/time.h
YSmeBSB.o: /usr/include/linux/time.h /usr/include/sys/time.h
YSmeBSB.o: /usr/X11R6/include/X11/Xmu/Drawing.h /usr/include/stdio.h
YSmeBSB.o: /usr/include/libio.h /usr/include/_G_config.h
YSmeBSB.o: /usr/X11R6/include/X11/Xaw/XawInit.h
YSmeBSB.o: /usr/X11R6/include/X11/Xaw/SimpleMenu.h
YSmeBSB.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
YSmeBSB.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
YSmeBSB.o: /usr/X11R6/include/X11/ICE/ICE.h
YSmeBSB.o: /usr/X11R6/include/X11/Xmu/Converters.h
YSmeBSB.o: /usr/X11R6/include/X11/Xaw/Cardinals.h YSmeBSBP.h
YSmeBSB.o: /usr/X11R6/include/X11/Xaw/SmeP.h /usr/X11R6/include/X11/Xaw/Sme.h
YSmeBSB.o: /home/cannam/rosegarden/include/YSmeBSB.h
YTextEntry.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
YTextEntry.o: /usr/include/libio.h /usr/include/_G_config.h
YTextEntry.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
YTextEntry.o: /usr/X11R6/include/X11/Intrinsic.h
YTextEntry.o: /usr/X11R6/include/X11/Xlib.h /usr/include/sys/types.h
YTextEntry.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
YTextEntry.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
YTextEntry.o: /usr/include/sys/bitypes.h /usr/X11R6/include/X11/X.h
YTextEntry.o: /usr/X11R6/include/X11/Xfuncproto.h
YTextEntry.o: /usr/X11R6/include/X11/Xosdefs.h
YTextEntry.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YTextEntry.o: /usr/X11R6/include/X11/Xutil.h
YTextEntry.o: /usr/X11R6/include/X11/Xresource.h /usr/include/string.h
YTextEntry.o: /usr/include/features.h /usr/include/sys/cdefs.h
YTextEntry.o: /usr/X11R6/include/X11/Core.h
YTextEntry.o: /usr/X11R6/include/X11/Composite.h
YTextEntry.o: /usr/X11R6/include/X11/Constraint.h
YTextEntry.o: /usr/X11R6/include/X11/Object.h
YTextEntry.o: /usr/X11R6/include/X11/RectObj.h /usr/include/stdlib.h
YTextEntry.o: /usr/include/errno.h /usr/include/linux/errno.h
YTextEntry.o: /usr/include/asm/errno.h /usr/X11R6/include/X11/StringDefs.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/Form.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/Label.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/Simple.h
YTextEntry.o: /usr/X11R6/include/X11/Xmu/Converters.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/Text.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/TextSink.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
YTextEntry.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
YTextEntry.o: /home/cannam/rosegarden/include/Debug.h
YTextEntry.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
YTextEntry.o: /usr/include/signal.h /usr/include/linux/signal.h
YTextEntry.o: /usr/include/asm/signal.h /usr/include/ctype.h
YTextEntry.o: /usr/include/endian.h /usr/include/bytesex.h
YToolbar.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
YToolbar.o: /usr/include/signal.h /usr/include/features.h
YToolbar.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
YToolbar.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
YToolbar.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
YToolbar.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
YToolbar.o: /usr/include/asm/signal.h /usr/include/ctype.h
YToolbar.o: /usr/include/endian.h /usr/include/bytesex.h
YToolbar.o: /usr/include/string.h
YToolbar.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
YToolbar.o: /usr/include/stdio.h /usr/include/libio.h
YToolbar.o: /usr/include/_G_config.h /usr/include/stdlib.h
YToolbar.o: /usr/include/errno.h /usr/include/linux/errno.h
YToolbar.o: /usr/include/asm/errno.h /usr/X11R6/include/X11/Intrinsic.h
YToolbar.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
YToolbar.o: /usr/X11R6/include/X11/Xfuncproto.h
YToolbar.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
YToolbar.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
YToolbar.o: /usr/X11R6/include/X11/Composite.h
YToolbar.o: /usr/X11R6/include/X11/Constraint.h
YToolbar.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
YToolbar.o: /usr/X11R6/include/X11/StringDefs.h
YToolbar.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
YToolbar.o: /usr/X11R6/include/X11/SM/SM.h
YToolbar.o: /usr/X11R6/include/X11/ICE/ICElib.h
YToolbar.o: /usr/X11R6/include/X11/ICE/ICE.h
YToolbar.o: /usr/X11R6/include/X11/Xaw/Command.h
YToolbar.o: /usr/X11R6/include/X11/Xaw/Label.h
YToolbar.o: /usr/X11R6/include/X11/Xaw/Simple.h
YToolbar.o: /usr/X11R6/include/X11/Xmu/Converters.h
YToolbar.o: /usr/X11R6/include/X11/Xaw/Form.h
YToolbar.o: /home/cannam/rosegarden/include/Debug.h YToolbar.h
YToolbar.o: /home/cannam/rosegarden/include/Yawn.h
YToolbar.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h YawnP.h
Yawn.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
Yawn.o: /usr/include/stdlib.h /usr/include/features.h
Yawn.o: /usr/include/sys/cdefs.h
Yawn.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Yawn.o: /usr/include/errno.h /usr/include/linux/errno.h
Yawn.o: /usr/include/asm/errno.h /usr/include/string.h
Yawn.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Yawn.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Yawn.o: /usr/include/sys/types.h /usr/include/linux/types.h
Yawn.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Yawn.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Yawn.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Yawn.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Yawn.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Yawn.o: /usr/X11R6/include/X11/Composite.h
Yawn.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Yawn.o: /usr/X11R6/include/X11/RectObj.h /usr/X11R6/include/X11/StringDefs.h
Yawn.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Yawn.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
Yawn.o: /usr/X11R6/include/X11/ICE/ICE.h /usr/X11R6/include/X11/Xaw/Box.h
Yawn.o: /usr/X11R6/include/X11/Xmu/Converters.h
Yawn.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/Label.h
Yawn.o: /usr/X11R6/include/X11/Xaw/Simple.h
Yawn.o: /usr/X11R6/include/X11/Xaw/Paned.h
Yawn.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Yawn.o: /usr/X11R6/include/X11/Xaw/Command.h
Yawn.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Yawn.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Yawn.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Yawn.o: /usr/X11R6/include/X11/Xaw/Text.h
Yawn.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Yawn.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Yawn.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Yawn.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Yawn.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Yawn.o: /home/cannam/rosegarden/include/Yawn.h
Yawn.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Yawn.o: /usr/include/signal.h /usr/include/linux/signal.h
Yawn.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Yawn.o: /usr/include/bytesex.h /home/cannam/rosegarden/include/Debug.h YPop.h
Yawn.o: /home/cannam/rosegarden/common/bitmaps/grey.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/lightgrey.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/down.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/right.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/left_arrow.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/right_arrow.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/up_arrow.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/down_arrow.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/ticked.xbm
Yawn.o: /home/cannam/rosegarden/common/bitmaps/unticked.xbm

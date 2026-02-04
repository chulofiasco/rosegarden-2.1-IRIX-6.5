# Makefile for the Musical Notation Editor
#
# Chris Cannam, February 1995


# # These three should have been exported from the superior make
#
# ROSEGARDEN	= /usr/people/cannam/rosegarden
# SYSINCDIRS	= -I/usr/include -I/usr/include/X11 -I/usr/include/X11/Xaw -I/usr/include/X11/bitmaps
# DEFINES		= -DFUNCPROTO -DHAVE_SPECIALIST_MALLOC_LIBRARY

INTERLOCK	= $(ROSEGARDEN)/interlock
REGEXP		= $(ROSEGARDEN)/regexp
YAWN		= $(ROSEGARDEN)/yawn
LISTS		= $(ROSEGARDEN)/lists
MIDI		= $(ROSEGARDEN)/midi
COMMON		= $(ROSEGARDEN)/common

INCDIRS		= $(SYSINCDIRS) -I$(ROSEGARDEN)/include -I$(COMMON)/bitmaps

SYSLIBS		= -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lLists -lMidi -lInterlock -lRegexp $(SYSLIBS) $(SYSEXTRALIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib -L$(REGEXP)/lib $(LIBS) 

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)

TARGET		= $(ROSEGARDEN)/bin/editor
OBJECTS		= Constructors.o Dechord.o DrawElements.o DrawGroups.o \
		DrawMTElements.o DrawMTGroups.o DrawMTStave.o \
		DrawOTElements.o DrawOTGroups.o DrawOTStave.o DrawPMX.o \
		Equation.o Filter.o Format.o GC.o IO.o ItemList.o MTime.o \
		Main.o Marks.o Menu.o MenuBar.o MenuChord.o MenuEdit.o \
		MenuFile.o MenuGroup.o MenuMarks.o MenuStave.o MenuText.o \
		MenuTools.o Methods.o MidiIn.o MidiOut.o Palette.o Rechord.o \
		Spline.o Stave.o StaveCursor.o StaveEdit.o Undo.o Visuals.o \
		Widgets.o

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

Constructors.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Constructors.o: /usr/include/libio.h /usr/include/_G_config.h
Constructors.o: /usr/include/sys/cdefs.h /usr/include/stdlib.h
Constructors.o: /usr/include/features.h
Constructors.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Constructors.o: /usr/include/errno.h /usr/include/linux/errno.h
Constructors.o: /usr/include/asm/errno.h /usr/include/alloca.h General.h
Constructors.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Constructors.o: /usr/include/signal.h /usr/include/sys/types.h
Constructors.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Constructors.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Constructors.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Constructors.o: /usr/include/asm/signal.h /usr/include/ctype.h
Constructors.o: /usr/include/endian.h /usr/include/bytesex.h
Constructors.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
Constructors.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Constructors.o: /usr/X11R6/include/X11/Intrinsic.h
Constructors.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
Constructors.o: /usr/X11R6/include/X11/Xfuncproto.h
Constructors.o: /usr/X11R6/include/X11/Xosdefs.h
Constructors.o: /usr/X11R6/include/X11/Xutil.h
Constructors.o: /usr/X11R6/include/X11/Xresource.h
Constructors.o: /usr/X11R6/include/X11/Core.h
Constructors.o: /usr/X11R6/include/X11/Composite.h
Constructors.o: /usr/X11R6/include/X11/Constraint.h
Constructors.o: /usr/X11R6/include/X11/Object.h
Constructors.o: /usr/X11R6/include/X11/RectObj.h
Constructors.o: /home/cannam/rosegarden/include/Debug.h
Constructors.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Notes.h
Constructors.o: Visuals.h MTime.h /usr/include/limits.h
Constructors.o: /usr/include/posix1_lim.h /usr/include/linux/limits.h
Constructors.o: /usr/include/posix2_lim.h Classes.h Stave.h ItemList.h
Dechord.o: Dechord.h General.h /usr/include/stdio.h /usr/include/libio.h
Dechord.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Dechord.o: /usr/include/stdlib.h /usr/include/features.h
Dechord.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Dechord.o: /usr/include/errno.h /usr/include/linux/errno.h
Dechord.o: /usr/include/asm/errno.h /usr/include/alloca.h
Dechord.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Dechord.o: /usr/include/signal.h /usr/include/sys/types.h
Dechord.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Dechord.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Dechord.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Dechord.o: /usr/include/asm/signal.h /usr/include/ctype.h
Dechord.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Dechord.o: /home/cannam/rosegarden/include/Yawn.h
Dechord.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Dechord.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Dechord.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Dechord.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Dechord.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Dechord.o: /usr/X11R6/include/X11/Composite.h
Dechord.o: /usr/X11R6/include/X11/Constraint.h
Dechord.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Dechord.o: /home/cannam/rosegarden/include/Debug.h
Dechord.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Notes.h
Dechord.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
Dechord.o: /usr/include/limits.h /usr/include/posix1_lim.h
Dechord.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Classes.h
Dechord.o: MidiIn.h Stave.h MidiOut.h
Dechord.o: /home/cannam/rosegarden/include/Regexp.h Menu.h
DrawElements.o: Draw.h General.h /usr/include/stdio.h /usr/include/libio.h
DrawElements.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
DrawElements.o: /usr/include/stdlib.h /usr/include/features.h
DrawElements.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawElements.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawElements.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawElements.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawElements.o: /usr/include/signal.h /usr/include/sys/types.h
DrawElements.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawElements.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawElements.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawElements.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawElements.o: /usr/include/endian.h /usr/include/bytesex.h
DrawElements.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
DrawElements.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawElements.o: /usr/X11R6/include/X11/Intrinsic.h
DrawElements.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawElements.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawElements.o: /usr/X11R6/include/X11/Xosdefs.h
DrawElements.o: /usr/X11R6/include/X11/Xutil.h
DrawElements.o: /usr/X11R6/include/X11/Xresource.h
DrawElements.o: /usr/X11R6/include/X11/Core.h
DrawElements.o: /usr/X11R6/include/X11/Composite.h
DrawElements.o: /usr/X11R6/include/X11/Constraint.h
DrawElements.o: /usr/X11R6/include/X11/Object.h
DrawElements.o: /usr/X11R6/include/X11/RectObj.h
DrawElements.o: /home/cannam/rosegarden/include/Debug.h
DrawElements.o: /home/cannam/rosegarden/include/ServiceNames.h Notes.h
DrawElements.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawElements.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawElements.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawElements.o: Classes.h GC.h ItemList.h Spline.h
DrawElements.o: /home/cannam/rosegarden/common/bitmaps/chordmod_pause.xbm
DrawGroups.o: Draw.h General.h /usr/include/stdio.h /usr/include/libio.h
DrawGroups.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
DrawGroups.o: /usr/include/stdlib.h /usr/include/features.h
DrawGroups.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawGroups.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawGroups.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawGroups.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawGroups.o: /usr/include/signal.h /usr/include/sys/types.h
DrawGroups.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawGroups.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawGroups.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawGroups.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawGroups.o: /usr/include/endian.h /usr/include/bytesex.h
DrawGroups.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
DrawGroups.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawGroups.o: /usr/X11R6/include/X11/Intrinsic.h
DrawGroups.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawGroups.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawGroups.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
DrawGroups.o: /usr/X11R6/include/X11/Xresource.h
DrawGroups.o: /usr/X11R6/include/X11/Core.h
DrawGroups.o: /usr/X11R6/include/X11/Composite.h
DrawGroups.o: /usr/X11R6/include/X11/Constraint.h
DrawGroups.o: /usr/X11R6/include/X11/Object.h
DrawGroups.o: /usr/X11R6/include/X11/RectObj.h
DrawGroups.o: /home/cannam/rosegarden/include/Debug.h
DrawGroups.o: /home/cannam/rosegarden/include/ServiceNames.h Notes.h
DrawGroups.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawGroups.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawGroups.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Classes.h
DrawGroups.o: GC.h ItemList.h Spline.h
DrawMTElements.o: Draw.h General.h /usr/include/stdio.h /usr/include/libio.h
DrawMTElements.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
DrawMTElements.o: /usr/include/stdlib.h /usr/include/features.h
DrawMTElements.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawMTElements.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawMTElements.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawMTElements.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawMTElements.o: /usr/include/signal.h /usr/include/sys/types.h
DrawMTElements.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawMTElements.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawMTElements.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawMTElements.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawMTElements.o: /usr/include/endian.h /usr/include/bytesex.h
DrawMTElements.o: /usr/include/string.h
DrawMTElements.o: /home/cannam/rosegarden/include/Yawn.h
DrawMTElements.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawMTElements.o: /usr/X11R6/include/X11/Intrinsic.h
DrawMTElements.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawMTElements.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawMTElements.o: /usr/X11R6/include/X11/Xosdefs.h
DrawMTElements.o: /usr/X11R6/include/X11/Xutil.h
DrawMTElements.o: /usr/X11R6/include/X11/Xresource.h
DrawMTElements.o: /usr/X11R6/include/X11/Core.h
DrawMTElements.o: /usr/X11R6/include/X11/Composite.h
DrawMTElements.o: /usr/X11R6/include/X11/Constraint.h
DrawMTElements.o: /usr/X11R6/include/X11/Object.h
DrawMTElements.o: /usr/X11R6/include/X11/RectObj.h
DrawMTElements.o: /home/cannam/rosegarden/include/Debug.h
DrawMTElements.o: /home/cannam/rosegarden/include/ServiceNames.h Notes.h
DrawMTElements.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawMTElements.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawMTElements.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawMTElements.o: Classes.h GC.h ItemList.h Spline.h
DrawMTGroups.o: Draw.h General.h /usr/include/stdio.h /usr/include/libio.h
DrawMTGroups.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
DrawMTGroups.o: /usr/include/stdlib.h /usr/include/features.h
DrawMTGroups.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawMTGroups.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawMTGroups.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawMTGroups.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawMTGroups.o: /usr/include/signal.h /usr/include/sys/types.h
DrawMTGroups.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawMTGroups.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawMTGroups.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawMTGroups.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawMTGroups.o: /usr/include/endian.h /usr/include/bytesex.h
DrawMTGroups.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
DrawMTGroups.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawMTGroups.o: /usr/X11R6/include/X11/Intrinsic.h
DrawMTGroups.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawMTGroups.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawMTGroups.o: /usr/X11R6/include/X11/Xosdefs.h
DrawMTGroups.o: /usr/X11R6/include/X11/Xutil.h
DrawMTGroups.o: /usr/X11R6/include/X11/Xresource.h
DrawMTGroups.o: /usr/X11R6/include/X11/Core.h
DrawMTGroups.o: /usr/X11R6/include/X11/Composite.h
DrawMTGroups.o: /usr/X11R6/include/X11/Constraint.h
DrawMTGroups.o: /usr/X11R6/include/X11/Object.h
DrawMTGroups.o: /usr/X11R6/include/X11/RectObj.h
DrawMTGroups.o: /home/cannam/rosegarden/include/Debug.h
DrawMTGroups.o: /home/cannam/rosegarden/include/ServiceNames.h Notes.h
DrawMTGroups.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawMTGroups.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawMTGroups.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawMTGroups.o: Classes.h GC.h ItemList.h Spline.h
DrawMTStave.o: /usr/include/stdio.h /usr/include/libio.h
DrawMTStave.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h Format.h
DrawMTStave.o: Classes.h Notes.h General.h /usr/include/stdlib.h
DrawMTStave.o: /usr/include/features.h
DrawMTStave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawMTStave.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawMTStave.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawMTStave.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawMTStave.o: /usr/include/signal.h /usr/include/sys/types.h
DrawMTStave.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawMTStave.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawMTStave.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawMTStave.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawMTStave.o: /usr/include/endian.h /usr/include/bytesex.h
DrawMTStave.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
DrawMTStave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawMTStave.o: /usr/X11R6/include/X11/Intrinsic.h
DrawMTStave.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawMTStave.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawMTStave.o: /usr/X11R6/include/X11/Xosdefs.h
DrawMTStave.o: /usr/X11R6/include/X11/Xutil.h
DrawMTStave.o: /usr/X11R6/include/X11/Xresource.h
DrawMTStave.o: /usr/X11R6/include/X11/Core.h
DrawMTStave.o: /usr/X11R6/include/X11/Composite.h
DrawMTStave.o: /usr/X11R6/include/X11/Constraint.h
DrawMTStave.o: /usr/X11R6/include/X11/Object.h
DrawMTStave.o: /usr/X11R6/include/X11/RectObj.h
DrawMTStave.o: /home/cannam/rosegarden/include/Debug.h
DrawMTStave.o: /home/cannam/rosegarden/include/ServiceNames.h
DrawMTStave.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawMTStave.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawMTStave.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawMTStave.o: StaveEdit.h Stave.h StavePrivate.h StaveCursor.h Undo.h Draw.h
DrawMTStave.o: GC.h ItemList.h Spline.h
DrawOTElements.o: /usr/include/ctype.h /usr/include/features.h
DrawOTElements.o: /usr/include/sys/cdefs.h /usr/include/endian.h
DrawOTElements.o: /usr/include/bytesex.h Draw.h General.h
DrawOTElements.o: /usr/include/stdio.h /usr/include/libio.h
DrawOTElements.o: /usr/include/_G_config.h /usr/include/stdlib.h
DrawOTElements.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawOTElements.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawOTElements.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawOTElements.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawOTElements.o: /usr/include/signal.h /usr/include/sys/types.h
DrawOTElements.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawOTElements.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawOTElements.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawOTElements.o: /usr/include/asm/signal.h /usr/include/string.h
DrawOTElements.o: /home/cannam/rosegarden/include/Yawn.h
DrawOTElements.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawOTElements.o: /usr/X11R6/include/X11/Intrinsic.h
DrawOTElements.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawOTElements.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawOTElements.o: /usr/X11R6/include/X11/Xosdefs.h
DrawOTElements.o: /usr/X11R6/include/X11/Xutil.h
DrawOTElements.o: /usr/X11R6/include/X11/Xresource.h
DrawOTElements.o: /usr/X11R6/include/X11/Core.h
DrawOTElements.o: /usr/X11R6/include/X11/Composite.h
DrawOTElements.o: /usr/X11R6/include/X11/Constraint.h
DrawOTElements.o: /usr/X11R6/include/X11/Object.h
DrawOTElements.o: /usr/X11R6/include/X11/RectObj.h
DrawOTElements.o: /home/cannam/rosegarden/include/Debug.h
DrawOTElements.o: /home/cannam/rosegarden/include/ServiceNames.h Notes.h
DrawOTElements.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawOTElements.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawOTElements.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawOTElements.o: Classes.h GC.h ItemList.h Spline.h
DrawOTGroups.o: Draw.h General.h /usr/include/stdio.h /usr/include/libio.h
DrawOTGroups.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
DrawOTGroups.o: /usr/include/stdlib.h /usr/include/features.h
DrawOTGroups.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawOTGroups.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawOTGroups.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawOTGroups.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawOTGroups.o: /usr/include/signal.h /usr/include/sys/types.h
DrawOTGroups.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawOTGroups.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawOTGroups.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawOTGroups.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawOTGroups.o: /usr/include/endian.h /usr/include/bytesex.h
DrawOTGroups.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
DrawOTGroups.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawOTGroups.o: /usr/X11R6/include/X11/Intrinsic.h
DrawOTGroups.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawOTGroups.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawOTGroups.o: /usr/X11R6/include/X11/Xosdefs.h
DrawOTGroups.o: /usr/X11R6/include/X11/Xutil.h
DrawOTGroups.o: /usr/X11R6/include/X11/Xresource.h
DrawOTGroups.o: /usr/X11R6/include/X11/Core.h
DrawOTGroups.o: /usr/X11R6/include/X11/Composite.h
DrawOTGroups.o: /usr/X11R6/include/X11/Constraint.h
DrawOTGroups.o: /usr/X11R6/include/X11/Object.h
DrawOTGroups.o: /usr/X11R6/include/X11/RectObj.h
DrawOTGroups.o: /home/cannam/rosegarden/include/Debug.h
DrawOTGroups.o: /home/cannam/rosegarden/include/ServiceNames.h Notes.h
DrawOTGroups.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawOTGroups.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawOTGroups.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawOTGroups.o: Classes.h GC.h ItemList.h Spline.h
DrawOTStave.o: /usr/include/stdio.h /usr/include/libio.h
DrawOTStave.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h Format.h
DrawOTStave.o: Classes.h Notes.h General.h /usr/include/stdlib.h
DrawOTStave.o: /usr/include/features.h
DrawOTStave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawOTStave.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawOTStave.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawOTStave.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawOTStave.o: /usr/include/signal.h /usr/include/sys/types.h
DrawOTStave.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawOTStave.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawOTStave.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawOTStave.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawOTStave.o: /usr/include/endian.h /usr/include/bytesex.h
DrawOTStave.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
DrawOTStave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawOTStave.o: /usr/X11R6/include/X11/Intrinsic.h
DrawOTStave.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
DrawOTStave.o: /usr/X11R6/include/X11/Xfuncproto.h
DrawOTStave.o: /usr/X11R6/include/X11/Xosdefs.h
DrawOTStave.o: /usr/X11R6/include/X11/Xutil.h
DrawOTStave.o: /usr/X11R6/include/X11/Xresource.h
DrawOTStave.o: /usr/X11R6/include/X11/Core.h
DrawOTStave.o: /usr/X11R6/include/X11/Composite.h
DrawOTStave.o: /usr/X11R6/include/X11/Constraint.h
DrawOTStave.o: /usr/X11R6/include/X11/Object.h
DrawOTStave.o: /usr/X11R6/include/X11/RectObj.h
DrawOTStave.o: /home/cannam/rosegarden/include/Debug.h
DrawOTStave.o: /home/cannam/rosegarden/include/ServiceNames.h
DrawOTStave.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
DrawOTStave.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
DrawOTStave.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawOTStave.o: StaveEdit.h Stave.h StavePrivate.h StaveCursor.h Undo.h Draw.h
DrawOTStave.o: GC.h ItemList.h Spline.h
DrawPMX.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
DrawPMX.o: /usr/include/sys/cdefs.h Format.h Classes.h Notes.h General.h
DrawPMX.o: /usr/include/stdlib.h /usr/include/features.h
DrawPMX.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
DrawPMX.o: /usr/include/errno.h /usr/include/linux/errno.h
DrawPMX.o: /usr/include/asm/errno.h /usr/include/alloca.h
DrawPMX.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
DrawPMX.o: /usr/include/signal.h /usr/include/sys/types.h
DrawPMX.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
DrawPMX.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
DrawPMX.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
DrawPMX.o: /usr/include/asm/signal.h /usr/include/ctype.h
DrawPMX.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
DrawPMX.o: /home/cannam/rosegarden/include/Yawn.h
DrawPMX.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
DrawPMX.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
DrawPMX.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
DrawPMX.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
DrawPMX.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
DrawPMX.o: /usr/X11R6/include/X11/Composite.h
DrawPMX.o: /usr/X11R6/include/X11/Constraint.h
DrawPMX.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
DrawPMX.o: /home/cannam/rosegarden/include/Debug.h
DrawPMX.o: /home/cannam/rosegarden/include/ServiceNames.h
DrawPMX.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
DrawPMX.o: /usr/include/limits.h /usr/include/posix1_lim.h
DrawPMX.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
DrawPMX.o: StavePrivate.h StaveCursor.h Stave.h Undo.h ItemList.h Marks.h
Equation.o: /usr/include/stdlib.h /usr/include/features.h
Equation.o: /usr/include/sys/cdefs.h
Equation.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Equation.o: /usr/include/errno.h /usr/include/linux/errno.h
Equation.o: /usr/include/asm/errno.h /usr/include/alloca.h
Equation.o: /usr/X11R6/include/X11/Xlib.h /usr/include/sys/types.h
Equation.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Equation.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Equation.o: /usr/include/sys/bitypes.h /usr/X11R6/include/X11/X.h
Equation.o: /usr/X11R6/include/X11/Xfuncproto.h
Equation.o: /usr/X11R6/include/X11/Xosdefs.h Equation.h
Filter.o: General.h /usr/include/stdio.h /usr/include/libio.h
Filter.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Filter.o: /usr/include/stdlib.h /usr/include/features.h
Filter.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Filter.o: /usr/include/errno.h /usr/include/linux/errno.h
Filter.o: /usr/include/asm/errno.h /usr/include/alloca.h
Filter.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Filter.o: /usr/include/signal.h /usr/include/sys/types.h
Filter.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Filter.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Filter.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Filter.o: /usr/include/asm/signal.h /usr/include/ctype.h
Filter.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Filter.o: /home/cannam/rosegarden/include/Yawn.h
Filter.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Filter.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Filter.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Filter.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Filter.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Filter.o: /usr/X11R6/include/X11/Composite.h
Filter.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Filter.o: /usr/X11R6/include/X11/RectObj.h
Filter.o: /home/cannam/rosegarden/include/Debug.h
Filter.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
Filter.o: Tags.h Stave.h Classes.h Notes.h
Filter.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
Filter.o: /usr/include/limits.h /usr/include/posix1_lim.h
Filter.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
Filter.o: StavePrivate.h StaveCursor.h Undo.h ItemList.h
Filter.o: /usr/include/unistd.h /usr/include/posix_opt.h
Filter.o: /usr/include/gnu/types.h /usr/include/confname.h
Filter.o: /usr/include/sys/wait.h /usr/include/waitflags.h
Filter.o: /usr/include/waitstatus.h /usr/include/dirent.h
Filter.o: /usr/include/linux/dirent.h /usr/include/sys/stat.h
Filter.o: /usr/include/linux/stat.h
Format.o: General.h /usr/include/stdio.h /usr/include/libio.h
Format.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Format.o: /usr/include/stdlib.h /usr/include/features.h
Format.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Format.o: /usr/include/errno.h /usr/include/linux/errno.h
Format.o: /usr/include/asm/errno.h /usr/include/alloca.h
Format.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Format.o: /usr/include/signal.h /usr/include/sys/types.h
Format.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Format.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Format.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Format.o: /usr/include/asm/signal.h /usr/include/ctype.h
Format.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Format.o: /home/cannam/rosegarden/include/Yawn.h
Format.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Format.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Format.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Format.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Format.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Format.o: /usr/X11R6/include/X11/Composite.h
Format.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Format.o: /usr/X11R6/include/X11/RectObj.h
Format.o: /home/cannam/rosegarden/include/Debug.h
Format.o: /home/cannam/rosegarden/include/ServiceNames.h Notes.h
Format.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
Format.o: /usr/include/limits.h /usr/include/posix1_lim.h
Format.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Classes.h
Format.o: Format.h ItemList.h StavePrivate.h StaveCursor.h Stave.h Undo.h
GC.o: General.h /usr/include/stdio.h /usr/include/libio.h
GC.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h /usr/include/stdlib.h
GC.o: /usr/include/features.h
GC.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h /usr/include/errno.h
GC.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
GC.o: /usr/include/alloca.h /home/cannam/rosegarden/include/SysDeps.h
GC.o: ../../config.h /usr/include/signal.h /usr/include/sys/types.h
GC.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
GC.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
GC.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
GC.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
GC.o: /usr/include/bytesex.h /usr/include/string.h
GC.o: /home/cannam/rosegarden/include/Yawn.h
GC.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
GC.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
GC.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
GC.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
GC.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
GC.o: /usr/X11R6/include/X11/Composite.h /usr/X11R6/include/X11/Constraint.h
GC.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
GC.o: /home/cannam/rosegarden/include/Debug.h
GC.o: /home/cannam/rosegarden/include/ServiceNames.h
IO.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
IO.o: /usr/include/sys/cdefs.h /usr/include/ctype.h /usr/include/features.h
IO.o: /usr/include/endian.h /usr/include/bytesex.h
IO.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h General.h
IO.o: /usr/include/stdlib.h /usr/include/errno.h /usr/include/linux/errno.h
IO.o: /usr/include/asm/errno.h /usr/include/alloca.h
IO.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
IO.o: /usr/include/signal.h /usr/include/sys/types.h
IO.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
IO.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
IO.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
IO.o: /usr/include/asm/signal.h /usr/include/string.h
IO.o: /home/cannam/rosegarden/include/Yawn.h
IO.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
IO.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
IO.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
IO.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
IO.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
IO.o: /usr/X11R6/include/X11/Composite.h /usr/X11R6/include/X11/Constraint.h
IO.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
IO.o: /home/cannam/rosegarden/include/Debug.h
IO.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Classes.h Notes.h
IO.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
IO.o: /usr/include/limits.h /usr/include/posix1_lim.h
IO.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Stave.h
IO.o: StaveEdit.h Widgets.h Menu.h Format.h ItemList.h IO.h Marks.h
ItemList.o: Classes.h /usr/include/stdio.h /usr/include/libio.h
ItemList.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h Notes.h
ItemList.o: General.h /usr/include/stdlib.h /usr/include/features.h
ItemList.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
ItemList.o: /usr/include/errno.h /usr/include/linux/errno.h
ItemList.o: /usr/include/asm/errno.h /usr/include/alloca.h
ItemList.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
ItemList.o: /usr/include/signal.h /usr/include/sys/types.h
ItemList.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
ItemList.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
ItemList.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
ItemList.o: /usr/include/asm/signal.h /usr/include/ctype.h
ItemList.o: /usr/include/endian.h /usr/include/bytesex.h
ItemList.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
ItemList.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
ItemList.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
ItemList.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
ItemList.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
ItemList.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
ItemList.o: /usr/X11R6/include/X11/Composite.h
ItemList.o: /usr/X11R6/include/X11/Constraint.h
ItemList.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
ItemList.o: /home/cannam/rosegarden/include/Debug.h
ItemList.o: /home/cannam/rosegarden/include/ServiceNames.h
ItemList.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
ItemList.o: /usr/include/limits.h /usr/include/posix1_lim.h
ItemList.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h ItemList.h
ItemList.o: Marks.h
MTime.o: Notes.h General.h /usr/include/stdio.h /usr/include/libio.h
MTime.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MTime.o: /usr/include/stdlib.h /usr/include/features.h
MTime.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MTime.o: /usr/include/errno.h /usr/include/linux/errno.h
MTime.o: /usr/include/asm/errno.h /usr/include/alloca.h
MTime.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MTime.o: /usr/include/signal.h /usr/include/sys/types.h
MTime.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MTime.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MTime.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MTime.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
MTime.o: /usr/include/bytesex.h /usr/include/string.h
MTime.o: /home/cannam/rosegarden/include/Yawn.h
MTime.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MTime.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MTime.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MTime.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MTime.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MTime.o: /usr/X11R6/include/X11/Composite.h
MTime.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
MTime.o: /usr/X11R6/include/X11/RectObj.h
MTime.o: /home/cannam/rosegarden/include/Debug.h
MTime.o: /home/cannam/rosegarden/include/ServiceNames.h
MTime.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
MTime.o: /usr/include/limits.h /usr/include/posix1_lim.h
MTime.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
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
Main.o: /usr/include/alloca.h /home/cannam/rosegarden/include/Yawn.h
Main.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Main.o: /home/cannam/rosegarden/include/Debug.h
Main.o: /home/cannam/rosegarden/include/ServiceNames.h Resources.h
Main.o: Fallbacks.h Widgets.h Visuals.h Tags.h GC.h Menu.h Stave.h Classes.h
Main.o: Notes.h /home/cannam/rosegarden/include/Lists.h MTime.h
Main.o: /usr/include/limits.h /usr/include/posix1_lim.h
Main.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h StaveEdit.h
Main.o: /home/cannam/rosegarden/include/ILClient.h
Main.o: /home/cannam/rosegarden/include/ILTypes.h
Marks.o: Notes.h General.h /usr/include/stdio.h /usr/include/libio.h
Marks.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Marks.o: /usr/include/stdlib.h /usr/include/features.h
Marks.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Marks.o: /usr/include/errno.h /usr/include/linux/errno.h
Marks.o: /usr/include/asm/errno.h /usr/include/alloca.h
Marks.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Marks.o: /usr/include/signal.h /usr/include/sys/types.h
Marks.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Marks.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Marks.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Marks.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Marks.o: /usr/include/bytesex.h /usr/include/string.h
Marks.o: /home/cannam/rosegarden/include/Yawn.h
Marks.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Marks.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Marks.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Marks.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Marks.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Marks.o: /usr/X11R6/include/X11/Composite.h
Marks.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Marks.o: /usr/X11R6/include/X11/RectObj.h
Marks.o: /home/cannam/rosegarden/include/Debug.h
Marks.o: /home/cannam/rosegarden/include/ServiceNames.h
Marks.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
Marks.o: /usr/include/limits.h /usr/include/posix1_lim.h
Marks.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Marks.h
Marks.o: Classes.h ItemList.h StavePrivate.h StaveCursor.h Stave.h Undo.h
Marks.o: Draw.h GC.h Spline.h Equation.h
Menu.o: General.h /usr/include/stdio.h /usr/include/libio.h
Menu.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Menu.o: /usr/include/stdlib.h /usr/include/features.h
Menu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Menu.o: /usr/include/errno.h /usr/include/linux/errno.h
Menu.o: /usr/include/asm/errno.h /usr/include/alloca.h
Menu.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Menu.o: /usr/include/signal.h /usr/include/sys/types.h
Menu.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Menu.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Menu.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Menu.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Menu.o: /usr/include/bytesex.h /usr/include/string.h
Menu.o: /home/cannam/rosegarden/include/Yawn.h
Menu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Menu.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Menu.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Menu.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Menu.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Menu.o: /usr/X11R6/include/X11/Composite.h
Menu.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Menu.o: /usr/X11R6/include/X11/RectObj.h
Menu.o: /home/cannam/rosegarden/include/Debug.h
Menu.o: /home/cannam/rosegarden/include/ServiceNames.h IO.h Stave.h Classes.h
Menu.o: Notes.h /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
Menu.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
Menu.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Widgets.h
Menu.o: Menu.h Undo.h /home/cannam/rosegarden/common/bitmaps/text_above.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/text_above_large.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/text_below.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/text_below_italic.xbm
Menu.o: /home/cannam/rosegarden/include/YSmeBSB.h
Menu.o: /usr/X11R6/include/X11/Xmu/Converters.h
Menu.o: /usr/X11R6/include/X11/Xaw/Sme.h
Menu.o: /home/cannam/rosegarden/include/ILClient.h
Menu.o: /home/cannam/rosegarden/include/ILTypes.h
Menu.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Menu.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Menu.o: /usr/X11R6/include/X11/ICE/ICElib.h /usr/X11R6/include/X11/ICE/ICE.h
Menu.o: /usr/X11R6/include/X11/cursorfont.h
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/undo.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/redo.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/cut.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/copy.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/paste.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/edit.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/chord.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/slur.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/tie.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/beam.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/tuplet.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/break.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/text1.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/text2.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/text3.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/text4.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/dynamic.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/crescendo.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/decrescendo.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/chordmod_pause.xbm
MenuBar.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuBar.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuBar.o: /usr/include/stdlib.h /usr/include/features.h
MenuBar.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuBar.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuBar.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuBar.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuBar.o: /usr/include/signal.h /usr/include/sys/types.h
MenuBar.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuBar.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuBar.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuBar.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuBar.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
MenuBar.o: /home/cannam/rosegarden/include/Yawn.h
MenuBar.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuBar.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuBar.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuBar.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuBar.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuBar.o: /usr/X11R6/include/X11/Composite.h
MenuBar.o: /usr/X11R6/include/X11/Constraint.h
MenuBar.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuBar.o: /home/cannam/rosegarden/include/Debug.h
MenuBar.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuBar.o: Tags.h Stave.h Classes.h Notes.h
MenuBar.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuBar.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuBar.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h StaveEdit.h
MenuBar.o: StavePrivate.h StaveCursor.h Undo.h ItemList.h GC.h
MenuBar.o: /usr/X11R6/include/X11/Xaw/Box.h
MenuBar.o: /usr/X11R6/include/X11/Xmu/Converters.h
MenuBar.o: /usr/X11R6/include/X11/Xaw/Form.h
MenuBar.o: /usr/X11R6/include/X11/Xaw/Paned.h
MenuBar.o: /usr/X11R6/include/X11/Xaw/Repeater.h
MenuBar.o: /usr/X11R6/include/X11/Xaw/Command.h
MenuBar.o: /usr/X11R6/include/X11/Xaw/Label.h
MenuBar.o: /usr/X11R6/include/X11/Xaw/Simple.h /usr/X11R6/include/X11/Shell.h
MenuBar.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
MenuBar.o: /usr/X11R6/include/X11/ICE/ICElib.h
MenuBar.o: /usr/X11R6/include/X11/ICE/ICE.h
MenuBar.o: /usr/X11R6/include/X11/StringDefs.h
MenuChord.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuChord.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuChord.o: /usr/include/stdlib.h /usr/include/features.h
MenuChord.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuChord.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuChord.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuChord.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuChord.o: /usr/include/signal.h /usr/include/sys/types.h
MenuChord.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuChord.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuChord.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuChord.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuChord.o: /usr/include/endian.h /usr/include/bytesex.h
MenuChord.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuChord.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuChord.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuChord.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuChord.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuChord.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuChord.o: /usr/X11R6/include/X11/Composite.h
MenuChord.o: /usr/X11R6/include/X11/Constraint.h
MenuChord.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuChord.o: /home/cannam/rosegarden/include/Debug.h
MenuChord.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuChord.o: Tags.h Stave.h Classes.h Notes.h
MenuChord.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuChord.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuChord.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
MenuChord.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h GC.h
MenuChord.o: Marks.h Draw.h Spline.h /usr/X11R6/include/X11/Xaw/Box.h
MenuChord.o: /usr/X11R6/include/X11/Xmu/Converters.h
MenuChord.o: /usr/X11R6/include/X11/Xaw/Label.h
MenuChord.o: /usr/X11R6/include/X11/Xaw/Simple.h
MenuChord.o: /usr/X11R6/include/X11/Xaw/Form.h
MenuChord.o: /usr/X11R6/include/X11/Xaw/Paned.h
MenuChord.o: /usr/X11R6/include/X11/Xaw/Viewport.h
MenuChord.o: /usr/X11R6/include/X11/Xaw/Reports.h
MenuChord.o: /usr/X11R6/include/X11/Xaw/Command.h
MenuChord.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
MenuChord.o: /usr/X11R6/include/X11/SM/SM.h
MenuChord.o: /usr/X11R6/include/X11/ICE/ICElib.h
MenuChord.o: /usr/X11R6/include/X11/ICE/ICE.h
MenuChord.o: /usr/X11R6/include/X11/StringDefs.h
MenuEdit.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuEdit.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuEdit.o: /usr/include/stdlib.h /usr/include/features.h
MenuEdit.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuEdit.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuEdit.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuEdit.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuEdit.o: /usr/include/signal.h /usr/include/sys/types.h
MenuEdit.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuEdit.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuEdit.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuEdit.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuEdit.o: /usr/include/endian.h /usr/include/bytesex.h
MenuEdit.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuEdit.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuEdit.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuEdit.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuEdit.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuEdit.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuEdit.o: /usr/X11R6/include/X11/Composite.h
MenuEdit.o: /usr/X11R6/include/X11/Constraint.h
MenuEdit.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuEdit.o: /home/cannam/rosegarden/include/Debug.h
MenuEdit.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuEdit.o: Tags.h Stave.h Classes.h Notes.h
MenuEdit.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuEdit.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuEdit.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h StaveEdit.h
MenuEdit.o: StavePrivate.h StaveCursor.h Undo.h ItemList.h GC.h Marks.h
MenuEdit.o: /usr/X11R6/include/X11/Xaw/Box.h
MenuEdit.o: /usr/X11R6/include/X11/Xmu/Converters.h
MenuEdit.o: /usr/X11R6/include/X11/Xaw/Label.h
MenuEdit.o: /usr/X11R6/include/X11/Xaw/Simple.h
MenuEdit.o: /usr/X11R6/include/X11/Xaw/Form.h
MenuEdit.o: /usr/X11R6/include/X11/Xaw/Paned.h
MenuEdit.o: /usr/X11R6/include/X11/Xaw/Viewport.h
MenuEdit.o: /usr/X11R6/include/X11/Xaw/Reports.h
MenuEdit.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
MenuEdit.o: /usr/X11R6/include/X11/SM/SM.h
MenuEdit.o: /usr/X11R6/include/X11/ICE/ICElib.h
MenuEdit.o: /usr/X11R6/include/X11/ICE/ICE.h
MenuEdit.o: /usr/X11R6/include/X11/StringDefs.h /usr/include/sys/wait.h
MenuEdit.o: /usr/include/gnu/types.h /usr/include/waitflags.h
MenuEdit.o: /usr/include/waitstatus.h /usr/include/unistd.h
MenuEdit.o: /usr/include/posix_opt.h /usr/include/confname.h
MenuFile.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuFile.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuFile.o: /usr/include/stdlib.h /usr/include/features.h
MenuFile.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuFile.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuFile.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuFile.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuFile.o: /usr/include/signal.h /usr/include/sys/types.h
MenuFile.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuFile.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuFile.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuFile.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuFile.o: /usr/include/endian.h /usr/include/bytesex.h
MenuFile.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuFile.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuFile.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuFile.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuFile.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuFile.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuFile.o: /usr/X11R6/include/X11/Composite.h
MenuFile.o: /usr/X11R6/include/X11/Constraint.h
MenuFile.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuFile.o: /home/cannam/rosegarden/include/Debug.h
MenuFile.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuFile.o: Tags.h Stave.h Classes.h Notes.h
MenuFile.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuFile.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuFile.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h IO.h
MenuFile.o: StaveEdit.h StaveCursor.h Undo.h
MenuFile.o: /home/cannam/rosegarden/include/YSmeBSB.h
MenuFile.o: /usr/X11R6/include/X11/Xmu/Converters.h
MenuFile.o: /usr/X11R6/include/X11/Xaw/Sme.h
MenuFile.o: /home/cannam/rosegarden/common/bitmaps/buffer.xbm
MenuFile.o: /home/cannam/rosegarden/common/bitmaps/toolbar/new.xbm
MenuFile.o: /home/cannam/rosegarden/common/bitmaps/toolbar/open.xbm
MenuFile.o: /home/cannam/rosegarden/common/bitmaps/toolbar/save.xbm
MenuFile.o: /home/cannam/rosegarden/common/bitmaps/toolbar/sequence.xbm
MenuGroup.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuGroup.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuGroup.o: /usr/include/stdlib.h /usr/include/features.h
MenuGroup.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuGroup.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuGroup.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuGroup.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuGroup.o: /usr/include/signal.h /usr/include/sys/types.h
MenuGroup.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuGroup.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuGroup.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuGroup.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuGroup.o: /usr/include/endian.h /usr/include/bytesex.h
MenuGroup.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuGroup.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuGroup.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuGroup.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuGroup.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuGroup.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuGroup.o: /usr/X11R6/include/X11/Composite.h
MenuGroup.o: /usr/X11R6/include/X11/Constraint.h
MenuGroup.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuGroup.o: /home/cannam/rosegarden/include/Debug.h
MenuGroup.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuGroup.o: Tags.h Stave.h Classes.h Notes.h
MenuGroup.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuGroup.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuGroup.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
MenuGroup.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuGroup.o: Marks.h /usr/X11R6/include/X11/Xaw/Box.h
MenuGroup.o: /usr/X11R6/include/X11/Xmu/Converters.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/Label.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/Simple.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/Form.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/Paned.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/Text.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/TextSink.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
MenuGroup.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
MenuGroup.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
MenuGroup.o: /usr/X11R6/include/X11/SM/SM.h
MenuGroup.o: /usr/X11R6/include/X11/ICE/ICElib.h
MenuGroup.o: /usr/X11R6/include/X11/ICE/ICE.h
MenuGroup.o: /usr/X11R6/include/X11/StringDefs.h
MenuMarks.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuMarks.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuMarks.o: /usr/include/stdlib.h /usr/include/features.h
MenuMarks.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuMarks.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuMarks.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuMarks.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuMarks.o: /usr/include/signal.h /usr/include/sys/types.h
MenuMarks.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuMarks.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuMarks.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuMarks.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuMarks.o: /usr/include/endian.h /usr/include/bytesex.h
MenuMarks.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuMarks.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuMarks.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuMarks.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuMarks.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuMarks.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuMarks.o: /usr/X11R6/include/X11/Composite.h
MenuMarks.o: /usr/X11R6/include/X11/Constraint.h
MenuMarks.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuMarks.o: /home/cannam/rosegarden/include/Debug.h
MenuMarks.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuMarks.o: Tags.h Stave.h Classes.h Notes.h
MenuMarks.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuMarks.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuMarks.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
MenuMarks.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuStave.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuStave.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuStave.o: /usr/include/stdlib.h /usr/include/features.h
MenuStave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuStave.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuStave.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuStave.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuStave.o: /usr/include/signal.h /usr/include/sys/types.h
MenuStave.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuStave.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuStave.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuStave.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuStave.o: /usr/include/endian.h /usr/include/bytesex.h
MenuStave.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuStave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuStave.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuStave.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuStave.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuStave.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuStave.o: /usr/X11R6/include/X11/Composite.h
MenuStave.o: /usr/X11R6/include/X11/Constraint.h
MenuStave.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuStave.o: /home/cannam/rosegarden/include/Debug.h
MenuStave.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuStave.o: Tags.h Stave.h Classes.h Notes.h
MenuStave.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuStave.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuStave.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h
MenuStave.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuStave.o: /usr/X11R6/include/X11/Xaw/Label.h
MenuStave.o: /usr/X11R6/include/X11/Xaw/Simple.h
MenuStave.o: /usr/X11R6/include/X11/Xmu/Converters.h
MenuText.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuText.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuText.o: /usr/include/stdlib.h /usr/include/features.h
MenuText.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuText.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuText.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuText.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuText.o: /usr/include/signal.h /usr/include/sys/types.h
MenuText.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuText.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuText.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuText.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuText.o: /usr/include/endian.h /usr/include/bytesex.h
MenuText.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuText.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuText.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuText.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuText.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuText.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuText.o: /usr/X11R6/include/X11/Composite.h
MenuText.o: /usr/X11R6/include/X11/Constraint.h
MenuText.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuText.o: /home/cannam/rosegarden/include/Debug.h
MenuText.o: /home/cannam/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuText.o: Tags.h Stave.h Classes.h Notes.h
MenuText.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MenuText.o: /usr/include/limits.h /usr/include/posix1_lim.h
MenuText.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h StaveEdit.h
MenuText.o: StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuText.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
MenuText.o: /usr/X11R6/include/X11/SM/SM.h
MenuText.o: /usr/X11R6/include/X11/ICE/ICElib.h
MenuText.o: /usr/X11R6/include/X11/ICE/ICE.h
MenuText.o: /usr/X11R6/include/X11/StringDefs.h
MenuText.o: /usr/X11R6/include/X11/Xaw/Box.h
MenuText.o: /usr/X11R6/include/X11/Xmu/Converters.h
MenuText.o: /usr/X11R6/include/X11/Xaw/Form.h
MenuText.o: /usr/X11R6/include/X11/Xaw/Label.h
MenuText.o: /usr/X11R6/include/X11/Xaw/Simple.h
MenuText.o: /usr/X11R6/include/X11/Xaw/Paned.h
MenuTools.o: General.h /usr/include/stdio.h /usr/include/libio.h
MenuTools.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MenuTools.o: /usr/include/stdlib.h /usr/include/features.h
MenuTools.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MenuTools.o: /usr/include/errno.h /usr/include/linux/errno.h
MenuTools.o: /usr/include/asm/errno.h /usr/include/alloca.h
MenuTools.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MenuTools.o: /usr/include/signal.h /usr/include/sys/types.h
MenuTools.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MenuTools.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MenuTools.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MenuTools.o: /usr/include/asm/signal.h /usr/include/ctype.h
MenuTools.o: /usr/include/endian.h /usr/include/bytesex.h
MenuTools.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
MenuTools.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MenuTools.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MenuTools.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MenuTools.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MenuTools.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MenuTools.o: /usr/X11R6/include/X11/Composite.h
MenuTools.o: /usr/X11R6/include/X11/Constraint.h
MenuTools.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MenuTools.o: /home/cannam/rosegarden/include/Debug.h
MenuTools.o: /home/cannam/rosegarden/include/ServiceNames.h IO.h Stave.h
MenuTools.o: Classes.h Notes.h /home/cannam/rosegarden/include/Lists.h Tags.h
MenuTools.o: Visuals.h MTime.h /usr/include/limits.h
MenuTools.o: /usr/include/posix1_lim.h /usr/include/linux/limits.h
MenuTools.o: /usr/include/posix2_lim.h MidiOut.h MidiIn.h Menu.h StaveEdit.h
MenuTools.o: /usr/include/unistd.h /usr/include/posix_opt.h
MenuTools.o: /usr/include/gnu/types.h /usr/include/confname.h
MenuTools.o: /home/cannam/rosegarden/include/ILClient.h
MenuTools.o: /home/cannam/rosegarden/include/ILTypes.h
MenuTools.o: /usr/X11R6/include/X11/StringDefs.h
MenuTools.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
MenuTools.o: /usr/X11R6/include/X11/SM/SM.h
MenuTools.o: /usr/X11R6/include/X11/ICE/ICElib.h
MenuTools.o: /usr/X11R6/include/X11/ICE/ICE.h
MenuTools.o: /usr/X11R6/include/X11/cursorfont.h
Methods.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Methods.o: /usr/include/libio.h /usr/include/_G_config.h
Methods.o: /usr/include/sys/cdefs.h /usr/include/stdlib.h
Methods.o: /usr/include/features.h
Methods.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Methods.o: /usr/include/errno.h /usr/include/linux/errno.h
Methods.o: /usr/include/asm/errno.h /usr/include/alloca.h General.h
Methods.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Methods.o: /usr/include/signal.h /usr/include/sys/types.h
Methods.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Methods.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Methods.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Methods.o: /usr/include/asm/signal.h /usr/include/ctype.h
Methods.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Methods.o: /home/cannam/rosegarden/include/Yawn.h
Methods.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Methods.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Methods.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Methods.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Methods.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Methods.o: /usr/X11R6/include/X11/Composite.h
Methods.o: /usr/X11R6/include/X11/Constraint.h
Methods.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Methods.o: /home/cannam/rosegarden/include/Debug.h
Methods.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Visuals.h
Methods.o: Classes.h Notes.h MTime.h /usr/include/limits.h
Methods.o: /usr/include/posix1_lim.h /usr/include/linux/limits.h
Methods.o: /usr/include/posix2_lim.h GC.h ItemList.h Menu.h Stave.h
MidiIn.o: MidiIn.h Stave.h /usr/include/stdio.h /usr/include/libio.h
MidiIn.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h Classes.h Notes.h
MidiIn.o: General.h /usr/include/stdlib.h /usr/include/features.h
MidiIn.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MidiIn.o: /usr/include/errno.h /usr/include/linux/errno.h
MidiIn.o: /usr/include/asm/errno.h /usr/include/alloca.h
MidiIn.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MidiIn.o: /usr/include/signal.h /usr/include/sys/types.h
MidiIn.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MidiIn.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MidiIn.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MidiIn.o: /usr/include/asm/signal.h /usr/include/ctype.h
MidiIn.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
MidiIn.o: /home/cannam/rosegarden/include/Yawn.h
MidiIn.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MidiIn.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MidiIn.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MidiIn.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MidiIn.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MidiIn.o: /usr/X11R6/include/X11/Composite.h
MidiIn.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
MidiIn.o: /usr/X11R6/include/X11/RectObj.h
MidiIn.o: /home/cannam/rosegarden/include/Debug.h
MidiIn.o: /home/cannam/rosegarden/include/ServiceNames.h
MidiIn.o: /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
MidiIn.o: /usr/include/limits.h /usr/include/posix1_lim.h
MidiIn.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Widgets.h
MidiIn.o: StaveEdit.h ItemList.h Menu.h GC.h
MidiIn.o: /home/cannam/rosegarden/include/MidiFile.h
MidiIn.o: /home/cannam/rosegarden/include/MidiEvent.h
MidiIn.o: /home/cannam/rosegarden/include/MidiConsts.h
MidiIn.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
MidiIn.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
MidiIn.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
MidiIn.o: /usr/include/linux/uio.h /usr/include/linux/in.h
MidiIn.o: /usr/include/asm/byteorder.h
MidiIn.o: /home/cannam/rosegarden/include/MidiTrack.h
MidiIn.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
MidiIn.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
MidiIn.o: /usr/X11R6/include/X11/ICE/ICE.h
MidiIn.o: /usr/X11R6/include/X11/StringDefs.h
MidiIn.o: /usr/X11R6/include/X11/Xaw/Box.h
MidiIn.o: /usr/X11R6/include/X11/Xmu/Converters.h
MidiIn.o: /usr/X11R6/include/X11/Xaw/Label.h
MidiIn.o: /usr/X11R6/include/X11/Xaw/Simple.h
MidiIn.o: /usr/X11R6/include/X11/Xaw/Form.h
MidiIn.o: /usr/X11R6/include/X11/Xaw/Paned.h
MidiOut.o: General.h /usr/include/stdio.h /usr/include/libio.h
MidiOut.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
MidiOut.o: /usr/include/stdlib.h /usr/include/features.h
MidiOut.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MidiOut.o: /usr/include/errno.h /usr/include/linux/errno.h
MidiOut.o: /usr/include/asm/errno.h /usr/include/alloca.h
MidiOut.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MidiOut.o: /usr/include/signal.h /usr/include/sys/types.h
MidiOut.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MidiOut.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MidiOut.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MidiOut.o: /usr/include/asm/signal.h /usr/include/ctype.h
MidiOut.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
MidiOut.o: /home/cannam/rosegarden/include/Yawn.h
MidiOut.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MidiOut.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
MidiOut.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
MidiOut.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MidiOut.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
MidiOut.o: /usr/X11R6/include/X11/Composite.h
MidiOut.o: /usr/X11R6/include/X11/Constraint.h
MidiOut.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
MidiOut.o: /home/cannam/rosegarden/include/Debug.h
MidiOut.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Classes.h
MidiOut.o: Notes.h /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
MidiOut.o: /usr/include/limits.h /usr/include/posix1_lim.h
MidiOut.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Stave.h
MidiOut.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h Format.h
MidiOut.o: ItemList.h /home/cannam/rosegarden/include/MidiFile.h
MidiOut.o: /home/cannam/rosegarden/include/MidiEvent.h
MidiOut.o: /home/cannam/rosegarden/include/MidiConsts.h
MidiOut.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
MidiOut.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
MidiOut.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
MidiOut.o: /usr/include/linux/uio.h /usr/include/linux/in.h
MidiOut.o: /usr/include/asm/byteorder.h
MidiOut.o: /home/cannam/rosegarden/include/MidiTrack.h
Palette.o: General.h /usr/include/stdio.h /usr/include/libio.h
Palette.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Palette.o: /usr/include/stdlib.h /usr/include/features.h
Palette.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Palette.o: /usr/include/errno.h /usr/include/linux/errno.h
Palette.o: /usr/include/asm/errno.h /usr/include/alloca.h
Palette.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Palette.o: /usr/include/signal.h /usr/include/sys/types.h
Palette.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Palette.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Palette.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Palette.o: /usr/include/asm/signal.h /usr/include/ctype.h
Palette.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Palette.o: /home/cannam/rosegarden/include/Yawn.h
Palette.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Palette.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Palette.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Palette.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Palette.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Palette.o: /usr/X11R6/include/X11/Composite.h
Palette.o: /usr/X11R6/include/X11/Constraint.h
Palette.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Palette.o: /home/cannam/rosegarden/include/Debug.h
Palette.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Visuals.h
Palette.o: Palette.h GC.h Notes.h /home/cannam/rosegarden/include/Lists.h
Palette.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
Palette.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Classes.h
Palette.o: Stave.h StaveEdit.h StaveCursor.h /usr/X11R6/include/X11/Shell.h
Palette.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Palette.o: /usr/X11R6/include/X11/ICE/ICElib.h
Palette.o: /usr/X11R6/include/X11/ICE/ICE.h
Palette.o: /usr/X11R6/include/X11/StringDefs.h
Palette.o: /usr/X11R6/include/X11/Xaw/Form.h
Palette.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Palette.o: /usr/X11R6/include/X11/Xaw/Command.h
Palette.o: /usr/X11R6/include/X11/Xaw/Label.h
Palette.o: /usr/X11R6/include/X11/Xaw/Simple.h
Palette.o: /usr/X11R6/include/X11/Xmu/Converters.h
Palette.o: /usr/X11R6/include/X11/Xaw/Paned.h
Rechord.o: Rechord.h General.h /usr/include/stdio.h /usr/include/libio.h
Rechord.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Rechord.o: /usr/include/stdlib.h /usr/include/features.h
Rechord.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Rechord.o: /usr/include/errno.h /usr/include/linux/errno.h
Rechord.o: /usr/include/asm/errno.h /usr/include/alloca.h
Rechord.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Rechord.o: /usr/include/signal.h /usr/include/sys/types.h
Rechord.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Rechord.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Rechord.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Rechord.o: /usr/include/asm/signal.h /usr/include/ctype.h
Rechord.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Rechord.o: /home/cannam/rosegarden/include/Yawn.h
Rechord.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Rechord.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Rechord.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Rechord.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Rechord.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Rechord.o: /usr/X11R6/include/X11/Composite.h
Rechord.o: /usr/X11R6/include/X11/Constraint.h
Rechord.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Rechord.o: /home/cannam/rosegarden/include/Debug.h
Rechord.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Notes.h
Rechord.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
Rechord.o: /usr/include/limits.h /usr/include/posix1_lim.h
Rechord.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Classes.h
Spline.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xlib.h
Spline.o: /usr/include/sys/types.h /usr/include/linux/types.h
Spline.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Spline.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Spline.o: /usr/X11R6/include/X11/Xfuncproto.h
Spline.o: /usr/X11R6/include/X11/Xosdefs.h
Spline.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Spline.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xutil.h
Spline.o: /usr/X11R6/include/X11/Xresource.h /usr/include/string.h
Spline.o: /usr/include/features.h /usr/include/sys/cdefs.h
Spline.o: /usr/X11R6/include/X11/Core.h /usr/X11R6/include/X11/Composite.h
Spline.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Spline.o: /usr/X11R6/include/X11/RectObj.h /usr/include/stdio.h
Spline.o: /usr/include/libio.h /usr/include/_G_config.h GC.h
Stave.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
Stave.o: /usr/include/sys/cdefs.h General.h /usr/include/stdlib.h
Stave.o: /usr/include/features.h
Stave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Stave.o: /usr/include/errno.h /usr/include/linux/errno.h
Stave.o: /usr/include/asm/errno.h /usr/include/alloca.h
Stave.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Stave.o: /usr/include/signal.h /usr/include/sys/types.h
Stave.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Stave.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Stave.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Stave.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Stave.o: /usr/include/bytesex.h /usr/include/string.h
Stave.o: /home/cannam/rosegarden/include/Yawn.h
Stave.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Stave.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Stave.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Stave.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Stave.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Stave.o: /usr/X11R6/include/X11/Composite.h
Stave.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Stave.o: /usr/X11R6/include/X11/RectObj.h
Stave.o: /home/cannam/rosegarden/include/Debug.h
Stave.o: /home/cannam/rosegarden/include/ServiceNames.h Tags.h Notes.h
Stave.o: /home/cannam/rosegarden/include/Lists.h Visuals.h MTime.h
Stave.o: /usr/include/limits.h /usr/include/posix1_lim.h
Stave.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Classes.h GC.h
Stave.o: IO.h Stave.h Palette.h Widgets.h Format.h StaveEdit.h StavePrivate.h
Stave.o: StaveCursor.h Undo.h ItemList.h Marks.h Menu.h Spline.h
Stave.o: /usr/X11R6/include/X11/StringDefs.h
Stave.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/Label.h
Stave.o: /usr/X11R6/include/X11/Xaw/Simple.h
Stave.o: /usr/X11R6/include/X11/Xmu/Converters.h
Stave.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
StaveCursor.o: General.h /usr/include/stdio.h /usr/include/libio.h
StaveCursor.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
StaveCursor.o: /usr/include/stdlib.h /usr/include/features.h
StaveCursor.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
StaveCursor.o: /usr/include/errno.h /usr/include/linux/errno.h
StaveCursor.o: /usr/include/asm/errno.h /usr/include/alloca.h
StaveCursor.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
StaveCursor.o: /usr/include/signal.h /usr/include/sys/types.h
StaveCursor.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
StaveCursor.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
StaveCursor.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
StaveCursor.o: /usr/include/asm/signal.h /usr/include/ctype.h
StaveCursor.o: /usr/include/endian.h /usr/include/bytesex.h
StaveCursor.o: /usr/include/string.h /home/cannam/rosegarden/include/Yawn.h
StaveCursor.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
StaveCursor.o: /usr/X11R6/include/X11/Intrinsic.h
StaveCursor.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
StaveCursor.o: /usr/X11R6/include/X11/Xfuncproto.h
StaveCursor.o: /usr/X11R6/include/X11/Xosdefs.h
StaveCursor.o: /usr/X11R6/include/X11/Xutil.h
StaveCursor.o: /usr/X11R6/include/X11/Xresource.h
StaveCursor.o: /usr/X11R6/include/X11/Core.h
StaveCursor.o: /usr/X11R6/include/X11/Composite.h
StaveCursor.o: /usr/X11R6/include/X11/Constraint.h
StaveCursor.o: /usr/X11R6/include/X11/Object.h
StaveCursor.o: /usr/X11R6/include/X11/RectObj.h
StaveCursor.o: /home/cannam/rosegarden/include/Debug.h
StaveCursor.o: /home/cannam/rosegarden/include/ServiceNames.h Classes.h
StaveCursor.o: Notes.h /home/cannam/rosegarden/include/Lists.h Tags.h
StaveCursor.o: Visuals.h MTime.h /usr/include/limits.h
StaveCursor.o: /usr/include/posix1_lim.h /usr/include/linux/limits.h
StaveCursor.o: /usr/include/posix2_lim.h GC.h Stave.h StavePrivate.h
StaveCursor.o: StaveCursor.h Undo.h StaveEdit.h ItemList.h Menu.h
StaveCursor.o: /usr/X11R6/include/X11/StringDefs.h
StaveEdit.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Intrinsic.h
StaveEdit.o: /usr/X11R6/include/X11/Xlib.h /usr/include/sys/types.h
StaveEdit.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
StaveEdit.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
StaveEdit.o: /usr/include/sys/bitypes.h /usr/X11R6/include/X11/Xfuncproto.h
StaveEdit.o: /usr/X11R6/include/X11/Xosdefs.h
StaveEdit.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
StaveEdit.o: /usr/X11R6/include/X11/Xutil.h
StaveEdit.o: /usr/X11R6/include/X11/Xresource.h /usr/include/string.h
StaveEdit.o: /usr/include/features.h /usr/include/sys/cdefs.h
StaveEdit.o: /usr/X11R6/include/X11/Core.h /usr/X11R6/include/X11/Composite.h
StaveEdit.o: /usr/X11R6/include/X11/Constraint.h
StaveEdit.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
StaveEdit.o: /usr/X11R6/include/X11/cursorfont.h
StaveEdit.o: /usr/X11R6/include/X11/StringDefs.h
StaveEdit.o: /usr/X11R6/include/X11/Xaw/Simple.h
StaveEdit.o: /usr/X11R6/include/X11/Xmu/Converters.h General.h
StaveEdit.o: /usr/include/stdio.h /usr/include/libio.h
StaveEdit.o: /usr/include/_G_config.h /usr/include/stdlib.h
StaveEdit.o: /usr/include/errno.h /usr/include/linux/errno.h
StaveEdit.o: /usr/include/asm/errno.h /usr/include/alloca.h
StaveEdit.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
StaveEdit.o: /usr/include/signal.h /usr/include/linux/signal.h
StaveEdit.o: /usr/include/asm/signal.h /usr/include/ctype.h
StaveEdit.o: /usr/include/endian.h /usr/include/bytesex.h
StaveEdit.o: /home/cannam/rosegarden/include/Yawn.h
StaveEdit.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
StaveEdit.o: /home/cannam/rosegarden/include/Debug.h
StaveEdit.o: /home/cannam/rosegarden/include/ServiceNames.h Visuals.h Tags.h
StaveEdit.o: Classes.h Notes.h /home/cannam/rosegarden/include/Lists.h
StaveEdit.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
StaveEdit.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Stave.h
StaveEdit.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h GC.h ItemList.h
StaveEdit.o: Palette.h Menu.h Marks.h
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hourglass.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hourglass_mask.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/drag.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/drag_mask.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour0.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour1.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour2.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour3.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour4.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour5.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour6.xbm
StaveEdit.o: /home/cannam/rosegarden/common/bitmaps/hour7.xbm
StaveEdit.o: /usr/X11R6/include/X11/keysym.h
StaveEdit.o: /usr/X11R6/include/X11/keysymdef.h
Undo.o: General.h /usr/include/stdio.h /usr/include/libio.h
Undo.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Undo.o: /usr/include/stdlib.h /usr/include/features.h
Undo.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Undo.o: /usr/include/errno.h /usr/include/linux/errno.h
Undo.o: /usr/include/asm/errno.h /usr/include/alloca.h
Undo.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Undo.o: /usr/include/signal.h /usr/include/sys/types.h
Undo.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Undo.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Undo.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Undo.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Undo.o: /usr/include/bytesex.h /usr/include/string.h
Undo.o: /home/cannam/rosegarden/include/Yawn.h
Undo.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Undo.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Undo.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Undo.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Undo.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Undo.o: /usr/X11R6/include/X11/Composite.h
Undo.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Undo.o: /usr/X11R6/include/X11/RectObj.h
Undo.o: /home/cannam/rosegarden/include/Debug.h
Undo.o: /home/cannam/rosegarden/include/ServiceNames.h Stave.h Classes.h
Undo.o: Notes.h /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
Undo.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
Undo.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h StavePrivate.h
Undo.o: StaveCursor.h Undo.h ItemList.h Marks.h Menu.h
Undo.o: /usr/X11R6/include/X11/Xaw/Label.h
Undo.o: /usr/X11R6/include/X11/Xaw/Simple.h
Undo.o: /usr/X11R6/include/X11/Xmu/Converters.h
Visuals.o: General.h /usr/include/stdio.h /usr/include/libio.h
Visuals.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Visuals.o: /usr/include/stdlib.h /usr/include/features.h
Visuals.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Visuals.o: /usr/include/errno.h /usr/include/linux/errno.h
Visuals.o: /usr/include/asm/errno.h /usr/include/alloca.h
Visuals.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Visuals.o: /usr/include/signal.h /usr/include/sys/types.h
Visuals.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Visuals.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Visuals.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Visuals.o: /usr/include/asm/signal.h /usr/include/ctype.h
Visuals.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Visuals.o: /home/cannam/rosegarden/include/Yawn.h
Visuals.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Visuals.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Visuals.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Visuals.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Visuals.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Visuals.o: /usr/X11R6/include/X11/Composite.h
Visuals.o: /usr/X11R6/include/X11/Constraint.h
Visuals.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Visuals.o: /home/cannam/rosegarden/include/Debug.h
Visuals.o: /home/cannam/rosegarden/include/ServiceNames.h Visuals.h Tags.h
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rose.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rose_mask.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/grey.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/lightgrey.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/up.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/down.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/note_bodyfilled.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/note_bodyempty.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/dot.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_up_1.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_up_2.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_up_3.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_up_4.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_down_1.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_down_2.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_down_3.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/tail_down_4.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_hemidemisemi.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_demisemi.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_semiquaver.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_quaver.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_crotchet.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_minim.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_semibreve.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/rest_breve.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/notemod_sharp.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/notemod_flat.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/notemod_natural.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_dot.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_accent.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_legato.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_sfz.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_rfz.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_trill.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_turn.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/chordmod_pause.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/clef_treble.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/clef_tenor.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/clef_alto.xbm
Visuals.o: /home/cannam/rosegarden/common/bitmaps/clef_bass.xbm
Widgets.o: General.h /usr/include/stdio.h /usr/include/libio.h
Widgets.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
Widgets.o: /usr/include/stdlib.h /usr/include/features.h
Widgets.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Widgets.o: /usr/include/errno.h /usr/include/linux/errno.h
Widgets.o: /usr/include/asm/errno.h /usr/include/alloca.h
Widgets.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Widgets.o: /usr/include/signal.h /usr/include/sys/types.h
Widgets.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Widgets.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Widgets.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Widgets.o: /usr/include/asm/signal.h /usr/include/ctype.h
Widgets.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Widgets.o: /home/cannam/rosegarden/include/Yawn.h
Widgets.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Widgets.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Widgets.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Widgets.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Widgets.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Widgets.o: /usr/X11R6/include/X11/Composite.h
Widgets.o: /usr/X11R6/include/X11/Constraint.h
Widgets.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Widgets.o: /home/cannam/rosegarden/include/Debug.h
Widgets.o: /home/cannam/rosegarden/include/ServiceNames.h GC.h Classes.h
Widgets.o: Notes.h /home/cannam/rosegarden/include/Lists.h Tags.h Visuals.h
Widgets.o: MTime.h /usr/include/limits.h /usr/include/posix1_lim.h
Widgets.o: /usr/include/linux/limits.h /usr/include/posix2_lim.h Menu.h
Widgets.o: Stave.h /home/cannam/rosegarden/include/ILClient.h
Widgets.o: /home/cannam/rosegarden/include/ILTypes.h
Widgets.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Widgets.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Widgets.o: /usr/X11R6/include/X11/ICE/ICElib.h
Widgets.o: /usr/X11R6/include/X11/ICE/ICE.h
Widgets.o: /usr/X11R6/include/X11/cursorfont.h Palette.h
Widgets.o: /home/cannam/rosegarden/include/Version.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Form.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Paned.h
Widgets.o: /usr/X11R6/include/X11/Xmu/Converters.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Reports.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Box.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Label.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Simple.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Command.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Widgets.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h /usr/include/sys/stat.h
Widgets.o: /usr/include/linux/stat.h
Widgets.o: /home/cannam/rosegarden/common/bitmaps/rewind.xbm
Widgets.o: /home/cannam/rosegarden/common/bitmaps/back.xbm
Widgets.o: /home/cannam/rosegarden/common/bitmaps/forward.xbm
Widgets.o: /home/cannam/rosegarden/common/bitmaps/ffwd.xbm

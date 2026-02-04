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
CCDEPFLAGS	=

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
		$(MAKEDEPEND) -- $(INCDIRS) $(DEFINES) $(CCDEPFLAGS) -- *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

Constructors.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Constructors.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
Constructors.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Constructors.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Constructors.o: General.h /root/build/rosegarden/include/SysDeps.h
Constructors.o: ../../config.h /usr/include/signal.h
Constructors.o: /usr/include/internal/signal_core.h /usr/include/ctype.h
Constructors.o: /usr/include/internal/ctype_core.h
Constructors.o: /usr/include/internal/locale_attr_core.h
Constructors.o: /usr/include/string.h /usr/include/internal/string_core.h
Constructors.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Constructors.o: /usr/include/internal/stdarg_core.h
Constructors.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Constructors.o: /usr/include/sys/types.h /usr/include/X11/X.h
Constructors.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Constructors.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Constructors.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Constructors.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Constructors.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Constructors.o: /usr/include/X11/RectObj.h
Constructors.o: /root/build/rosegarden/include/Debug.h
Constructors.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Notes.h
Constructors.o: Visuals.h MTime.h /usr/include/limits.h
Constructors.o: /usr/include/internal/limits_core.h Classes.h Stave.h
Constructors.o: ItemList.h
Dechord.o: Dechord.h General.h /usr/include/stdio.h
Dechord.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
Dechord.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Dechord.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Dechord.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Dechord.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Dechord.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Dechord.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Dechord.o: /usr/include/internal/string_core.h
Dechord.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Dechord.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Dechord.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Dechord.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Dechord.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Dechord.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Dechord.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Dechord.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Dechord.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Dechord.o: /root/build/rosegarden/include/Debug.h
Dechord.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Notes.h
Dechord.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
Dechord.o: /usr/include/limits.h /usr/include/internal/limits_core.h
Dechord.o: Classes.h MidiIn.h Stave.h MidiOut.h
Dechord.o: /root/build/rosegarden/include/Regexp.h Menu.h
DrawElements.o: Draw.h General.h /usr/include/stdio.h
DrawElements.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
DrawElements.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
DrawElements.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
DrawElements.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawElements.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawElements.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawElements.o: /usr/include/internal/locale_attr_core.h
DrawElements.o: /usr/include/string.h /usr/include/internal/string_core.h
DrawElements.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawElements.o: /usr/include/internal/stdarg_core.h
DrawElements.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawElements.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawElements.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawElements.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawElements.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawElements.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawElements.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawElements.o: /usr/include/X11/RectObj.h
DrawElements.o: /root/build/rosegarden/include/Debug.h
DrawElements.o: /root/build/rosegarden/include/ServiceNames.h Notes.h
DrawElements.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
DrawElements.o: MTime.h /usr/include/limits.h
DrawElements.o: /usr/include/internal/limits_core.h Classes.h GC.h ItemList.h
DrawElements.o: Spline.h
DrawElements.o: /root/build/rosegarden/common/bitmaps/chordmod_pause.xbm
DrawGroups.o: Draw.h General.h /usr/include/stdio.h
DrawGroups.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
DrawGroups.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
DrawGroups.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
DrawGroups.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawGroups.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawGroups.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawGroups.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
DrawGroups.o: /usr/include/internal/string_core.h
DrawGroups.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawGroups.o: /usr/include/internal/stdarg_core.h
DrawGroups.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawGroups.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawGroups.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawGroups.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawGroups.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawGroups.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawGroups.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawGroups.o: /usr/include/X11/RectObj.h
DrawGroups.o: /root/build/rosegarden/include/Debug.h
DrawGroups.o: /root/build/rosegarden/include/ServiceNames.h Notes.h
DrawGroups.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
DrawGroups.o: /usr/include/limits.h /usr/include/internal/limits_core.h
DrawGroups.o: Classes.h GC.h ItemList.h Spline.h
DrawMTElements.o: Draw.h General.h /usr/include/stdio.h
DrawMTElements.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
DrawMTElements.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
DrawMTElements.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
DrawMTElements.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawMTElements.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawMTElements.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawMTElements.o: /usr/include/internal/locale_attr_core.h
DrawMTElements.o: /usr/include/string.h /usr/include/internal/string_core.h
DrawMTElements.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawMTElements.o: /usr/include/internal/stdarg_core.h
DrawMTElements.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawMTElements.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawMTElements.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawMTElements.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawMTElements.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawMTElements.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawMTElements.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawMTElements.o: /usr/include/X11/RectObj.h
DrawMTElements.o: /root/build/rosegarden/include/Debug.h
DrawMTElements.o: /root/build/rosegarden/include/ServiceNames.h Notes.h
DrawMTElements.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
DrawMTElements.o: MTime.h /usr/include/limits.h
DrawMTElements.o: /usr/include/internal/limits_core.h Classes.h GC.h
DrawMTElements.o: ItemList.h Spline.h
DrawMTGroups.o: Draw.h General.h /usr/include/stdio.h
DrawMTGroups.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
DrawMTGroups.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
DrawMTGroups.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
DrawMTGroups.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawMTGroups.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawMTGroups.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawMTGroups.o: /usr/include/internal/locale_attr_core.h
DrawMTGroups.o: /usr/include/string.h /usr/include/internal/string_core.h
DrawMTGroups.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawMTGroups.o: /usr/include/internal/stdarg_core.h
DrawMTGroups.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawMTGroups.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawMTGroups.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawMTGroups.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawMTGroups.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawMTGroups.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawMTGroups.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawMTGroups.o: /usr/include/X11/RectObj.h
DrawMTGroups.o: /root/build/rosegarden/include/Debug.h
DrawMTGroups.o: /root/build/rosegarden/include/ServiceNames.h Notes.h
DrawMTGroups.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
DrawMTGroups.o: MTime.h /usr/include/limits.h
DrawMTGroups.o: /usr/include/internal/limits_core.h Classes.h GC.h ItemList.h
DrawMTGroups.o: Spline.h
DrawMTStave.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
DrawMTStave.o: /usr/include/standards.h /usr/include/sgidefs.h
DrawMTStave.o: /usr/include/internal/sgimacros.h Format.h Classes.h Notes.h
DrawMTStave.o: General.h /usr/include/stdlib.h
DrawMTStave.o: /usr/include/internal/stdlib_core.h
DrawMTStave.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawMTStave.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawMTStave.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawMTStave.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
DrawMTStave.o: /usr/include/internal/string_core.h
DrawMTStave.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawMTStave.o: /usr/include/internal/stdarg_core.h
DrawMTStave.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawMTStave.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawMTStave.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawMTStave.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawMTStave.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawMTStave.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawMTStave.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawMTStave.o: /usr/include/X11/RectObj.h
DrawMTStave.o: /root/build/rosegarden/include/Debug.h
DrawMTStave.o: /root/build/rosegarden/include/ServiceNames.h
DrawMTStave.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
DrawMTStave.o: MTime.h /usr/include/limits.h
DrawMTStave.o: /usr/include/internal/limits_core.h StaveEdit.h Stave.h
DrawMTStave.o: StavePrivate.h StaveCursor.h Undo.h Draw.h GC.h ItemList.h
DrawMTStave.o: Spline.h
DrawOTElements.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawOTElements.o: /usr/include/internal/sgimacros.h /usr/include/standards.h
DrawOTElements.o: /usr/include/internal/locale_attr_core.h
DrawOTElements.o: /usr/include/sgidefs.h Draw.h General.h
DrawOTElements.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
DrawOTElements.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
DrawOTElements.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawOTElements.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawOTElements.o: /usr/include/string.h /usr/include/internal/string_core.h
DrawOTElements.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawOTElements.o: /usr/include/internal/stdarg_core.h
DrawOTElements.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawOTElements.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawOTElements.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawOTElements.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawOTElements.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawOTElements.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawOTElements.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawOTElements.o: /usr/include/X11/RectObj.h
DrawOTElements.o: /root/build/rosegarden/include/Debug.h
DrawOTElements.o: /root/build/rosegarden/include/ServiceNames.h Notes.h
DrawOTElements.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
DrawOTElements.o: MTime.h /usr/include/limits.h
DrawOTElements.o: /usr/include/internal/limits_core.h Classes.h GC.h
DrawOTElements.o: ItemList.h Spline.h
DrawOTGroups.o: Draw.h General.h /usr/include/stdio.h
DrawOTGroups.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
DrawOTGroups.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
DrawOTGroups.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
DrawOTGroups.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawOTGroups.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawOTGroups.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawOTGroups.o: /usr/include/internal/locale_attr_core.h
DrawOTGroups.o: /usr/include/string.h /usr/include/internal/string_core.h
DrawOTGroups.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawOTGroups.o: /usr/include/internal/stdarg_core.h
DrawOTGroups.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawOTGroups.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawOTGroups.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawOTGroups.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawOTGroups.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawOTGroups.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawOTGroups.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawOTGroups.o: /usr/include/X11/RectObj.h
DrawOTGroups.o: /root/build/rosegarden/include/Debug.h
DrawOTGroups.o: /root/build/rosegarden/include/ServiceNames.h Notes.h
DrawOTGroups.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
DrawOTGroups.o: MTime.h /usr/include/limits.h
DrawOTGroups.o: /usr/include/internal/limits_core.h Classes.h GC.h ItemList.h
DrawOTGroups.o: Spline.h
DrawOTStave.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
DrawOTStave.o: /usr/include/standards.h /usr/include/sgidefs.h
DrawOTStave.o: /usr/include/internal/sgimacros.h Format.h Classes.h Notes.h
DrawOTStave.o: General.h /usr/include/stdlib.h
DrawOTStave.o: /usr/include/internal/stdlib_core.h
DrawOTStave.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawOTStave.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawOTStave.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawOTStave.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
DrawOTStave.o: /usr/include/internal/string_core.h
DrawOTStave.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawOTStave.o: /usr/include/internal/stdarg_core.h
DrawOTStave.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
DrawOTStave.o: /usr/include/sys/types.h /usr/include/X11/X.h
DrawOTStave.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
DrawOTStave.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
DrawOTStave.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
DrawOTStave.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
DrawOTStave.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
DrawOTStave.o: /usr/include/X11/RectObj.h
DrawOTStave.o: /root/build/rosegarden/include/Debug.h
DrawOTStave.o: /root/build/rosegarden/include/ServiceNames.h
DrawOTStave.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
DrawOTStave.o: MTime.h /usr/include/limits.h
DrawOTStave.o: /usr/include/internal/limits_core.h StaveEdit.h Stave.h
DrawOTStave.o: StavePrivate.h StaveCursor.h Undo.h Draw.h GC.h ItemList.h
DrawOTStave.o: Spline.h
DrawPMX.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
DrawPMX.o: /usr/include/standards.h /usr/include/sgidefs.h
DrawPMX.o: /usr/include/internal/sgimacros.h Format.h Classes.h Notes.h
DrawPMX.o: General.h /usr/include/stdlib.h
DrawPMX.o: /usr/include/internal/stdlib_core.h
DrawPMX.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
DrawPMX.o: /usr/include/signal.h /usr/include/internal/signal_core.h
DrawPMX.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
DrawPMX.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
DrawPMX.o: /usr/include/internal/string_core.h
DrawPMX.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
DrawPMX.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
DrawPMX.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
DrawPMX.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
DrawPMX.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
DrawPMX.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
DrawPMX.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
DrawPMX.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
DrawPMX.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
DrawPMX.o: /root/build/rosegarden/include/Debug.h
DrawPMX.o: /root/build/rosegarden/include/ServiceNames.h
DrawPMX.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
DrawPMX.o: /usr/include/limits.h /usr/include/internal/limits_core.h
DrawPMX.o: StavePrivate.h StaveCursor.h Stave.h Undo.h ItemList.h Marks.h
Equation.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Equation.o: /usr/include/standards.h /usr/include/sgidefs.h
Equation.o: /usr/include/internal/sgimacros.h /usr/include/X11/Xlib.h
Equation.o: /usr/include/sys/types.h /usr/include/X11/X.h
Equation.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Equation.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Equation.o: Equation.h
Filter.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
Filter.o: /usr/include/standards.h /usr/include/sgidefs.h
Filter.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Filter.o: /usr/include/internal/stdlib_core.h
Filter.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Filter.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Filter.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Filter.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Filter.o: /usr/include/internal/string_core.h
Filter.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Filter.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Filter.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Filter.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Filter.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Filter.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Filter.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Filter.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Filter.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Filter.o: /root/build/rosegarden/include/Debug.h
Filter.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
Filter.o: Tags.h Stave.h Classes.h Notes.h
Filter.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
Filter.o: /usr/include/limits.h /usr/include/internal/limits_core.h
Filter.o: StavePrivate.h StaveCursor.h Undo.h ItemList.h
Filter.o: /usr/include/unistd.h /usr/include/sys/wait.h /usr/include/dirent.h
Filter.o: /usr/include/sys/dirent.h /usr/include/sys/stat.h
Filter.o: /usr/include/sys/timespec.h
Format.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
Format.o: /usr/include/standards.h /usr/include/sgidefs.h
Format.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Format.o: /usr/include/internal/stdlib_core.h
Format.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Format.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Format.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Format.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Format.o: /usr/include/internal/string_core.h
Format.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Format.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Format.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Format.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Format.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Format.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Format.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Format.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Format.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Format.o: /root/build/rosegarden/include/Debug.h
Format.o: /root/build/rosegarden/include/ServiceNames.h Notes.h
Format.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
Format.o: /usr/include/limits.h /usr/include/internal/limits_core.h Classes.h
Format.o: Format.h ItemList.h StavePrivate.h StaveCursor.h Stave.h Undo.h
GC.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
GC.o: /usr/include/standards.h /usr/include/sgidefs.h
GC.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
GC.o: /usr/include/internal/stdlib_core.h
GC.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
GC.o: /usr/include/signal.h /usr/include/internal/signal_core.h
GC.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
GC.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
GC.o: /usr/include/internal/string_core.h
GC.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
GC.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
GC.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/X11/X.h
GC.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
GC.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
GC.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
GC.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
GC.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
GC.o: /usr/include/X11/RectObj.h /root/build/rosegarden/include/Debug.h
GC.o: /root/build/rosegarden/include/ServiceNames.h
IO.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
IO.o: /usr/include/standards.h /usr/include/sgidefs.h
IO.o: /usr/include/internal/sgimacros.h /usr/include/ctype.h
IO.o: /usr/include/internal/ctype_core.h
IO.o: /usr/include/internal/locale_attr_core.h /usr/include/stddef.h
IO.o: /usr/include/internal/stddef_core.h General.h /usr/include/stdlib.h
IO.o: /usr/include/internal/stdlib_core.h
IO.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
IO.o: /usr/include/signal.h /usr/include/internal/signal_core.h
IO.o: /usr/include/string.h /usr/include/internal/string_core.h
IO.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
IO.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
IO.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/X11/X.h
IO.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
IO.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
IO.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
IO.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
IO.o: /usr/include/X11/RectObj.h /root/build/rosegarden/include/Debug.h
IO.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Classes.h Notes.h
IO.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
IO.o: /usr/include/limits.h /usr/include/internal/limits_core.h Stave.h
IO.o: StaveEdit.h Widgets.h Menu.h Format.h ItemList.h IO.h Marks.h
ItemList.o: Classes.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
ItemList.o: /usr/include/standards.h /usr/include/sgidefs.h
ItemList.o: /usr/include/internal/sgimacros.h Notes.h General.h
ItemList.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
ItemList.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
ItemList.o: /usr/include/signal.h /usr/include/internal/signal_core.h
ItemList.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
ItemList.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
ItemList.o: /usr/include/internal/string_core.h
ItemList.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
ItemList.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
ItemList.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
ItemList.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
ItemList.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
ItemList.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
ItemList.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
ItemList.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
ItemList.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
ItemList.o: /root/build/rosegarden/include/Debug.h
ItemList.o: /root/build/rosegarden/include/ServiceNames.h
ItemList.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
ItemList.o: /usr/include/limits.h /usr/include/internal/limits_core.h
ItemList.o: ItemList.h Marks.h
MTime.o: Notes.h General.h /usr/include/stdio.h
MTime.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
MTime.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
MTime.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
MTime.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MTime.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MTime.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MTime.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MTime.o: /usr/include/internal/string_core.h
MTime.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MTime.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MTime.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MTime.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MTime.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MTime.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MTime.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MTime.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MTime.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MTime.o: /root/build/rosegarden/include/Debug.h
MTime.o: /root/build/rosegarden/include/ServiceNames.h
MTime.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
MTime.o: /usr/include/limits.h /usr/include/internal/limits_core.h
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
Main.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Main.o: /usr/include/internal/stdarg_core.h
Main.o: /root/build/rosegarden/include/Debug.h
Main.o: /root/build/rosegarden/include/ServiceNames.h Resources.h Fallbacks.h
Main.o: Widgets.h Visuals.h Tags.h GC.h Menu.h Stave.h Classes.h Notes.h
Main.o: /root/build/rosegarden/include/Lists.h MTime.h /usr/include/limits.h
Main.o: /usr/include/internal/limits_core.h StaveEdit.h
Main.o: /root/build/rosegarden/include/ILClient.h
Main.o: /root/build/rosegarden/include/ILTypes.h
Marks.o: Notes.h General.h /usr/include/stdio.h
Marks.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
Marks.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Marks.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Marks.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Marks.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Marks.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Marks.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Marks.o: /usr/include/internal/string_core.h
Marks.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Marks.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Marks.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Marks.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Marks.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Marks.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Marks.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Marks.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Marks.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Marks.o: /root/build/rosegarden/include/Debug.h
Marks.o: /root/build/rosegarden/include/ServiceNames.h
Marks.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
Marks.o: /usr/include/limits.h /usr/include/internal/limits_core.h Marks.h
Marks.o: Classes.h ItemList.h StavePrivate.h StaveCursor.h Stave.h Undo.h
Marks.o: Draw.h GC.h Spline.h Equation.h
Menu.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
Menu.o: /usr/include/standards.h /usr/include/sgidefs.h
Menu.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Menu.o: /usr/include/internal/stdlib_core.h
Menu.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Menu.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Menu.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Menu.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Menu.o: /usr/include/internal/string_core.h
Menu.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Menu.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Menu.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/X11/X.h
Menu.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Menu.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Menu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Menu.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Menu.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Menu.o: /usr/include/X11/RectObj.h /root/build/rosegarden/include/Debug.h
Menu.o: /root/build/rosegarden/include/ServiceNames.h IO.h Stave.h Classes.h
Menu.o: Notes.h /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
Menu.o: MTime.h /usr/include/limits.h /usr/include/internal/limits_core.h
Menu.o: Widgets.h Menu.h Undo.h
Menu.o: /root/build/rosegarden/common/bitmaps/text_above.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/text_above_large.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/text_below.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/text_below_italic.xbm
Menu.o: /root/build/rosegarden/include/YSmeBSB.h
Menu.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Sme.h
Menu.o: /root/build/rosegarden/include/ILClient.h
Menu.o: /root/build/rosegarden/include/ILTypes.h
Menu.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
Menu.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
Menu.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
Menu.o: /usr/include/X11/cursorfont.h
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/undo.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/redo.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/cut.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/copy.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/paste.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/edit.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/chord.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/slur.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/tie.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/beam.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/tuplet.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/break.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/text1.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/text2.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/text3.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/text4.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/dynamic.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/crescendo.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/decrescendo.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/chordmod_pause.xbm
MenuBar.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
MenuBar.o: /usr/include/standards.h /usr/include/sgidefs.h
MenuBar.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
MenuBar.o: /usr/include/internal/stdlib_core.h
MenuBar.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuBar.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuBar.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuBar.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuBar.o: /usr/include/internal/string_core.h
MenuBar.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuBar.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuBar.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuBar.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuBar.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuBar.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuBar.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuBar.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuBar.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuBar.o: /root/build/rosegarden/include/Debug.h
MenuBar.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuBar.o: Tags.h Stave.h Classes.h Notes.h
MenuBar.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuBar.o: /usr/include/limits.h /usr/include/internal/limits_core.h
MenuBar.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h GC.h
MenuBar.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xmu/Converters.h
MenuBar.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
MenuBar.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Command.h
MenuBar.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
MenuBar.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
MenuBar.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
MenuBar.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/StringDefs.h
MenuChord.o: General.h /usr/include/stdio.h
MenuChord.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
MenuChord.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
MenuChord.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
MenuChord.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuChord.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuChord.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuChord.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuChord.o: /usr/include/internal/string_core.h
MenuChord.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuChord.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuChord.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuChord.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuChord.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuChord.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuChord.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuChord.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuChord.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuChord.o: /root/build/rosegarden/include/Debug.h
MenuChord.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuChord.o: Tags.h Stave.h Classes.h Notes.h
MenuChord.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuChord.o: /usr/include/limits.h /usr/include/internal/limits_core.h
MenuChord.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h GC.h
MenuChord.o: Marks.h Draw.h Spline.h /usr/include/X11/Xaw/Box.h
MenuChord.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Label.h
MenuChord.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Form.h
MenuChord.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xaw/Viewport.h
MenuChord.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Command.h
MenuChord.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
MenuChord.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
MenuChord.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/StringDefs.h
MenuEdit.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
MenuEdit.o: /usr/include/standards.h /usr/include/sgidefs.h
MenuEdit.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
MenuEdit.o: /usr/include/internal/stdlib_core.h
MenuEdit.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuEdit.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuEdit.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuEdit.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuEdit.o: /usr/include/internal/string_core.h
MenuEdit.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuEdit.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuEdit.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuEdit.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuEdit.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuEdit.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuEdit.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuEdit.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuEdit.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuEdit.o: /root/build/rosegarden/include/Debug.h
MenuEdit.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuEdit.o: Tags.h Stave.h Classes.h Notes.h
MenuEdit.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuEdit.o: /usr/include/limits.h /usr/include/internal/limits_core.h
MenuEdit.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h GC.h
MenuEdit.o: Marks.h /usr/include/X11/Xaw/Box.h
MenuEdit.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Label.h
MenuEdit.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Form.h
MenuEdit.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xaw/Viewport.h
MenuEdit.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Shell.h
MenuEdit.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
MenuEdit.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
MenuEdit.o: /usr/include/X11/StringDefs.h /usr/include/sys/wait.h
MenuEdit.o: /usr/include/unistd.h
MenuFile.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
MenuFile.o: /usr/include/standards.h /usr/include/sgidefs.h
MenuFile.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
MenuFile.o: /usr/include/internal/stdlib_core.h
MenuFile.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuFile.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuFile.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuFile.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuFile.o: /usr/include/internal/string_core.h
MenuFile.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuFile.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuFile.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuFile.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuFile.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuFile.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuFile.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuFile.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuFile.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuFile.o: /root/build/rosegarden/include/Debug.h
MenuFile.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuFile.o: Tags.h Stave.h Classes.h Notes.h
MenuFile.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuFile.o: /usr/include/limits.h /usr/include/internal/limits_core.h IO.h
MenuFile.o: StaveEdit.h StaveCursor.h Undo.h
MenuFile.o: /root/build/rosegarden/include/YSmeBSB.h
MenuFile.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Sme.h
MenuFile.o: /root/build/rosegarden/common/bitmaps/buffer.xbm
MenuFile.o: /root/build/rosegarden/common/bitmaps/toolbar/new.xbm
MenuFile.o: /root/build/rosegarden/common/bitmaps/toolbar/open.xbm
MenuFile.o: /root/build/rosegarden/common/bitmaps/toolbar/save.xbm
MenuFile.o: /root/build/rosegarden/common/bitmaps/toolbar/sequence.xbm
MenuGroup.o: General.h /usr/include/stdio.h
MenuGroup.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
MenuGroup.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
MenuGroup.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
MenuGroup.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuGroup.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuGroup.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuGroup.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuGroup.o: /usr/include/internal/string_core.h
MenuGroup.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuGroup.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuGroup.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuGroup.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuGroup.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuGroup.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuGroup.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuGroup.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuGroup.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuGroup.o: /root/build/rosegarden/include/Debug.h
MenuGroup.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuGroup.o: Tags.h Stave.h Classes.h Notes.h
MenuGroup.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuGroup.o: /usr/include/limits.h /usr/include/internal/limits_core.h
MenuGroup.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuGroup.o: Marks.h /usr/include/X11/Xaw/Box.h
MenuGroup.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Label.h
MenuGroup.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Form.h
MenuGroup.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xaw/AsciiText.h
MenuGroup.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
MenuGroup.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
MenuGroup.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
MenuGroup.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
MenuGroup.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
MenuGroup.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/StringDefs.h
MenuMarks.o: General.h /usr/include/stdio.h
MenuMarks.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
MenuMarks.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
MenuMarks.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
MenuMarks.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuMarks.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuMarks.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuMarks.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuMarks.o: /usr/include/internal/string_core.h
MenuMarks.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuMarks.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuMarks.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuMarks.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuMarks.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuMarks.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuMarks.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuMarks.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuMarks.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuMarks.o: /root/build/rosegarden/include/Debug.h
MenuMarks.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuMarks.o: Tags.h Stave.h Classes.h Notes.h
MenuMarks.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuMarks.o: /usr/include/limits.h /usr/include/internal/limits_core.h
MenuMarks.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuStave.o: General.h /usr/include/stdio.h
MenuStave.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
MenuStave.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
MenuStave.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
MenuStave.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuStave.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuStave.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuStave.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuStave.o: /usr/include/internal/string_core.h
MenuStave.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuStave.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuStave.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuStave.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuStave.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuStave.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuStave.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuStave.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuStave.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuStave.o: /root/build/rosegarden/include/Debug.h
MenuStave.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuStave.o: Tags.h Stave.h Classes.h Notes.h
MenuStave.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuStave.o: /usr/include/limits.h /usr/include/internal/limits_core.h
MenuStave.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuStave.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
MenuStave.o: /usr/include/X11/Xmu/Converters.h
MenuText.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
MenuText.o: /usr/include/standards.h /usr/include/sgidefs.h
MenuText.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
MenuText.o: /usr/include/internal/stdlib_core.h
MenuText.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuText.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuText.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuText.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuText.o: /usr/include/internal/string_core.h
MenuText.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuText.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuText.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuText.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuText.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuText.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuText.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuText.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuText.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuText.o: /root/build/rosegarden/include/Debug.h
MenuText.o: /root/build/rosegarden/include/ServiceNames.h Widgets.h Menu.h
MenuText.o: Tags.h Stave.h Classes.h Notes.h
MenuText.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MenuText.o: /usr/include/limits.h /usr/include/internal/limits_core.h
MenuText.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h ItemList.h
MenuText.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
MenuText.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
MenuText.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/StringDefs.h
MenuText.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xmu/Converters.h
MenuText.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Label.h
MenuText.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Paned.h
MenuTools.o: General.h /usr/include/stdio.h
MenuTools.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
MenuTools.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
MenuTools.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
MenuTools.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MenuTools.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MenuTools.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MenuTools.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MenuTools.o: /usr/include/internal/string_core.h
MenuTools.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MenuTools.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MenuTools.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MenuTools.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MenuTools.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MenuTools.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MenuTools.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MenuTools.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MenuTools.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MenuTools.o: /root/build/rosegarden/include/Debug.h
MenuTools.o: /root/build/rosegarden/include/ServiceNames.h IO.h Stave.h
MenuTools.o: Classes.h Notes.h /root/build/rosegarden/include/Lists.h Tags.h
MenuTools.o: Visuals.h MTime.h /usr/include/limits.h
MenuTools.o: /usr/include/internal/limits_core.h MidiOut.h MidiIn.h Menu.h
MenuTools.o: StaveEdit.h /usr/include/unistd.h
MenuTools.o: /root/build/rosegarden/include/ILClient.h
MenuTools.o: /root/build/rosegarden/include/ILTypes.h
MenuTools.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
MenuTools.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
MenuTools.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
MenuTools.o: /usr/include/X11/cursorfont.h
Methods.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Methods.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
Methods.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Methods.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Methods.o: General.h /root/build/rosegarden/include/SysDeps.h ../../config.h
Methods.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Methods.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Methods.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Methods.o: /usr/include/internal/string_core.h
Methods.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Methods.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Methods.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Methods.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Methods.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Methods.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Methods.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Methods.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Methods.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Methods.o: /root/build/rosegarden/include/Debug.h
Methods.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Visuals.h
Methods.o: Classes.h Notes.h MTime.h /usr/include/limits.h
Methods.o: /usr/include/internal/limits_core.h GC.h ItemList.h Menu.h Stave.h
MidiIn.o: MidiIn.h Stave.h /usr/include/stdio.h
MidiIn.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
MidiIn.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h Classes.h
MidiIn.o: Notes.h General.h /usr/include/stdlib.h
MidiIn.o: /usr/include/internal/stdlib_core.h
MidiIn.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MidiIn.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MidiIn.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MidiIn.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MidiIn.o: /usr/include/internal/string_core.h
MidiIn.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MidiIn.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MidiIn.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MidiIn.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MidiIn.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MidiIn.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MidiIn.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MidiIn.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MidiIn.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MidiIn.o: /root/build/rosegarden/include/Debug.h
MidiIn.o: /root/build/rosegarden/include/ServiceNames.h
MidiIn.o: /root/build/rosegarden/include/Lists.h Tags.h Visuals.h MTime.h
MidiIn.o: /usr/include/limits.h /usr/include/internal/limits_core.h Widgets.h
MidiIn.o: StaveEdit.h ItemList.h Menu.h GC.h
MidiIn.o: /root/build/rosegarden/include/MidiFile.h
MidiIn.o: /root/build/rosegarden/include/MidiEvent.h
MidiIn.o: /root/build/rosegarden/include/MidiConsts.h
MidiIn.o: /usr/include/netinet/in.h
MidiIn.o: /root/build/rosegarden/include/MidiTrack.h /usr/include/X11/Shell.h
MidiIn.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
MidiIn.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
MidiIn.o: /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/Box.h
MidiIn.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Label.h
MidiIn.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Form.h
MidiIn.o: /usr/include/X11/Xaw/Paned.h
MidiOut.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
MidiOut.o: /usr/include/standards.h /usr/include/sgidefs.h
MidiOut.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
MidiOut.o: /usr/include/internal/stdlib_core.h
MidiOut.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MidiOut.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MidiOut.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MidiOut.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
MidiOut.o: /usr/include/internal/string_core.h
MidiOut.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MidiOut.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
MidiOut.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
MidiOut.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
MidiOut.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
MidiOut.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
MidiOut.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
MidiOut.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
MidiOut.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
MidiOut.o: /root/build/rosegarden/include/Debug.h
MidiOut.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Classes.h
MidiOut.o: Notes.h /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
MidiOut.o: /usr/include/limits.h /usr/include/internal/limits_core.h Stave.h
MidiOut.o: StaveEdit.h StavePrivate.h StaveCursor.h Undo.h Format.h
MidiOut.o: ItemList.h /root/build/rosegarden/include/MidiFile.h
MidiOut.o: /root/build/rosegarden/include/MidiEvent.h
MidiOut.o: /root/build/rosegarden/include/MidiConsts.h
MidiOut.o: /usr/include/netinet/in.h
MidiOut.o: /root/build/rosegarden/include/MidiTrack.h
Palette.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
Palette.o: /usr/include/standards.h /usr/include/sgidefs.h
Palette.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Palette.o: /usr/include/internal/stdlib_core.h
Palette.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Palette.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Palette.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Palette.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Palette.o: /usr/include/internal/string_core.h
Palette.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Palette.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Palette.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Palette.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Palette.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Palette.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Palette.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Palette.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Palette.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Palette.o: /root/build/rosegarden/include/Debug.h
Palette.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Visuals.h
Palette.o: Palette.h GC.h Notes.h /root/build/rosegarden/include/Lists.h
Palette.o: MTime.h /usr/include/limits.h /usr/include/internal/limits_core.h
Palette.o: Classes.h Stave.h StaveEdit.h StaveCursor.h
Palette.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Palette.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Palette.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/StringDefs.h
Palette.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Toggle.h
Palette.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Palette.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
Palette.o: /usr/include/X11/Xaw/Paned.h
Rechord.o: Rechord.h General.h /usr/include/stdio.h
Rechord.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
Rechord.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Rechord.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Rechord.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Rechord.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Rechord.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Rechord.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Rechord.o: /usr/include/internal/string_core.h
Rechord.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Rechord.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Rechord.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Rechord.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Rechord.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Rechord.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Rechord.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Rechord.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Rechord.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Rechord.o: /root/build/rosegarden/include/Debug.h
Rechord.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Notes.h
Rechord.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
Rechord.o: /usr/include/limits.h /usr/include/internal/limits_core.h
Rechord.o: Classes.h
Spline.o: /usr/include/X11/X.h /usr/include/X11/Xlib.h
Spline.o: /usr/include/sys/types.h /usr/include/standards.h
Spline.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Spline.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Spline.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Spline.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xutil.h
Spline.o: /usr/include/X11/Xresource.h /usr/include/string.h
Spline.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
Spline.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Spline.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Spline.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h GC.h
Stave.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
Stave.o: /usr/include/standards.h /usr/include/sgidefs.h
Stave.o: /usr/include/internal/sgimacros.h General.h /usr/include/stdlib.h
Stave.o: /usr/include/internal/stdlib_core.h
Stave.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Stave.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Stave.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Stave.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Stave.o: /usr/include/internal/string_core.h
Stave.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Stave.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Stave.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Stave.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Stave.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Stave.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Stave.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Stave.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Stave.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Stave.o: /root/build/rosegarden/include/Debug.h
Stave.o: /root/build/rosegarden/include/ServiceNames.h Tags.h Notes.h
Stave.o: /root/build/rosegarden/include/Lists.h Visuals.h MTime.h
Stave.o: /usr/include/limits.h /usr/include/internal/limits_core.h Classes.h
Stave.o: GC.h IO.h Stave.h Palette.h Widgets.h Format.h StaveEdit.h
Stave.o: StavePrivate.h StaveCursor.h Undo.h ItemList.h Marks.h Menu.h
Stave.o: Spline.h /usr/include/X11/StringDefs.h /usr/include/X11/Xaw/Form.h
Stave.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
Stave.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Scrollbar.h
StaveCursor.o: General.h /usr/include/stdio.h
StaveCursor.o: /usr/include/internal/stdio_core.h /usr/include/standards.h
StaveCursor.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
StaveCursor.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
StaveCursor.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
StaveCursor.o: /usr/include/signal.h /usr/include/internal/signal_core.h
StaveCursor.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
StaveCursor.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
StaveCursor.o: /usr/include/internal/string_core.h
StaveCursor.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
StaveCursor.o: /usr/include/internal/stdarg_core.h
StaveCursor.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
StaveCursor.o: /usr/include/sys/types.h /usr/include/X11/X.h
StaveCursor.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
StaveCursor.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
StaveCursor.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
StaveCursor.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
StaveCursor.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
StaveCursor.o: /usr/include/X11/RectObj.h
StaveCursor.o: /root/build/rosegarden/include/Debug.h
StaveCursor.o: /root/build/rosegarden/include/ServiceNames.h Classes.h
StaveCursor.o: Notes.h /root/build/rosegarden/include/Lists.h Tags.h
StaveCursor.o: Visuals.h MTime.h /usr/include/limits.h
StaveCursor.o: /usr/include/internal/limits_core.h GC.h Stave.h
StaveCursor.o: StavePrivate.h StaveCursor.h Undo.h StaveEdit.h ItemList.h
StaveCursor.o: Menu.h /usr/include/X11/StringDefs.h
StaveEdit.o: /usr/include/X11/X.h /usr/include/X11/Intrinsic.h
StaveEdit.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
StaveEdit.o: /usr/include/standards.h /usr/include/sgidefs.h
StaveEdit.o: /usr/include/internal/sgimacros.h /usr/include/X11/Xfuncproto.h
StaveEdit.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
StaveEdit.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
StaveEdit.o: /usr/include/X11/Xresource.h /usr/include/string.h
StaveEdit.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
StaveEdit.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
StaveEdit.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
StaveEdit.o: /usr/include/X11/cursorfont.h /usr/include/X11/StringDefs.h
StaveEdit.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xmu/Converters.h
StaveEdit.o: General.h /usr/include/stdio.h
StaveEdit.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
StaveEdit.o: /usr/include/internal/stdlib_core.h
StaveEdit.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
StaveEdit.o: /usr/include/signal.h /usr/include/internal/signal_core.h
StaveEdit.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
StaveEdit.o: /usr/include/internal/locale_attr_core.h
StaveEdit.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
StaveEdit.o: /usr/include/internal/stdarg_core.h
StaveEdit.o: /root/build/rosegarden/include/Debug.h
StaveEdit.o: /root/build/rosegarden/include/ServiceNames.h Visuals.h Tags.h
StaveEdit.o: Classes.h Notes.h /root/build/rosegarden/include/Lists.h MTime.h
StaveEdit.o: /usr/include/limits.h /usr/include/internal/limits_core.h
StaveEdit.o: Stave.h StaveEdit.h StavePrivate.h StaveCursor.h Undo.h GC.h
StaveEdit.o: ItemList.h Palette.h Menu.h Marks.h
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hourglass.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hourglass_mask.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/drag.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/drag_mask.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour0.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour1.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour2.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour3.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour4.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour5.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour6.xbm
StaveEdit.o: /root/build/rosegarden/common/bitmaps/hour7.xbm
StaveEdit.o: /usr/include/X11/keysym.h /usr/include/X11/keysymdef.h
StaveEdit.o: /usr/include/X11/SGIkeysym.h
Undo.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
Undo.o: /usr/include/standards.h /usr/include/sgidefs.h
Undo.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Undo.o: /usr/include/internal/stdlib_core.h
Undo.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Undo.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Undo.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Undo.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Undo.o: /usr/include/internal/string_core.h
Undo.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Undo.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Undo.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/X11/X.h
Undo.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Undo.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Undo.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Undo.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Undo.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Undo.o: /usr/include/X11/RectObj.h /root/build/rosegarden/include/Debug.h
Undo.o: /root/build/rosegarden/include/ServiceNames.h Stave.h Classes.h
Undo.o: Notes.h /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
Undo.o: MTime.h /usr/include/limits.h /usr/include/internal/limits_core.h
Undo.o: StavePrivate.h StaveCursor.h Undo.h ItemList.h Marks.h Menu.h
Undo.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
Undo.o: /usr/include/X11/Xmu/Converters.h
Visuals.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
Visuals.o: /usr/include/standards.h /usr/include/sgidefs.h
Visuals.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Visuals.o: /usr/include/internal/stdlib_core.h
Visuals.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Visuals.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Visuals.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Visuals.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Visuals.o: /usr/include/internal/string_core.h
Visuals.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Visuals.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Visuals.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Visuals.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Visuals.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Visuals.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Visuals.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Visuals.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Visuals.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Visuals.o: /root/build/rosegarden/include/Debug.h
Visuals.o: /root/build/rosegarden/include/ServiceNames.h Visuals.h Tags.h
Visuals.o: /root/build/rosegarden/common/bitmaps/rose.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rose_mask.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/grey.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/lightgrey.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/up.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/down.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/note_bodyfilled.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/note_bodyempty.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/dot.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_up_1.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_up_2.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_up_3.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_up_4.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_down_1.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_down_2.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_down_3.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/tail_down_4.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_hemidemisemi.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_demisemi.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_semiquaver.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_quaver.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_crotchet.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_minim.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_semibreve.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/rest_breve.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/notemod_sharp.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/notemod_flat.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/notemod_natural.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_dot.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_accent.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_legato.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_sfz.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_rfz.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_trill.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_turn.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/chordmod_pause.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/clef_treble.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/clef_tenor.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/clef_alto.xbm
Visuals.o: /root/build/rosegarden/common/bitmaps/clef_bass.xbm
Widgets.o: General.h /usr/include/stdio.h /usr/include/internal/stdio_core.h
Widgets.o: /usr/include/standards.h /usr/include/sgidefs.h
Widgets.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
Widgets.o: /usr/include/internal/stdlib_core.h
Widgets.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Widgets.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Widgets.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Widgets.o: /usr/include/internal/locale_attr_core.h /usr/include/string.h
Widgets.o: /usr/include/internal/string_core.h
Widgets.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Widgets.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Intrinsic.h
Widgets.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Widgets.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Widgets.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Widgets.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Widgets.o: /usr/include/X11/Xresource.h /usr/include/X11/Core.h
Widgets.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Widgets.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Widgets.o: /root/build/rosegarden/include/Debug.h
Widgets.o: /root/build/rosegarden/include/ServiceNames.h GC.h Classes.h
Widgets.o: Notes.h /root/build/rosegarden/include/Lists.h Tags.h Visuals.h
Widgets.o: MTime.h /usr/include/limits.h /usr/include/internal/limits_core.h
Widgets.o: Menu.h Stave.h /root/build/rosegarden/include/ILClient.h
Widgets.o: /root/build/rosegarden/include/ILTypes.h
Widgets.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
Widgets.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
Widgets.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
Widgets.o: /usr/include/X11/cursorfont.h Palette.h
Widgets.o: /root/build/rosegarden/include/Version.h
Widgets.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
Widgets.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Viewport.h
Widgets.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Box.h
Widgets.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
Widgets.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Repeater.h
Widgets.o: /usr/include/X11/Xaw/Scrollbar.h /usr/include/sys/stat.h
Widgets.o: /usr/include/sys/timespec.h
Widgets.o: /root/build/rosegarden/common/bitmaps/rewind.xbm
Widgets.o: /root/build/rosegarden/common/bitmaps/back.xbm
Widgets.o: /root/build/rosegarden/common/bitmaps/forward.xbm
Widgets.o: /root/build/rosegarden/common/bitmaps/ffwd.xbm

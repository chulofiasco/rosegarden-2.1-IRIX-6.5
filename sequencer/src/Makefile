
# Makefile for the Sequencer
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
MAPPER		= $(ROSEGARDEN)/mapper

INCDIRS		= $(SYSINCDIRS) -I$(ROSEGARDEN)/include -I$(COMMON)/bitmaps

SYSLIBS		= -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lMapper -lLists -lMidi -lInterlock $(SYSLIBS) $(SYSEXTRALIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(MAPPER)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib $(LIBS) 

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)
CCDEPFLAGS	=

TARGET		= $(ROSEGARDEN)/bin/sequencer
OBJECTS		= Clipboard.o Csound.o Dispatch.o EventDlgs.o EventListMenu.o \
		  EventListWindow.o Filter.o Main.o MainWindow.o Menu.o \
		  Message.o MidiSetupDlgs.o Parse.o PianoRoll.o PianoRollMenu.o\
		  Record.o Sequence.o Tracking.o TrackList.o Undo.o

$(TARGET):	$(OBJECTS)
		rm -f $(TARGET)
		$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

.c.o:
		$(CC) -c $(CFLAGS) $<

clean:
		rm -f $(OBJECTS)

depend:		
		$(MAKEDEPEND) -- $(INCDIRS) $(DEFINES) $(CCDEPFLAGS) -- `echo $(OBJECTS)|sed s/\\\.o/.c/g`

# DO NOT DELETE THIS LINE -- make depend depends on it.

Clipboard.o: /root/build/rosegarden/include/MidiFile.h
Clipboard.o: /root/build/rosegarden/include/MidiEvent.h
Clipboard.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Clipboard.o: /usr/include/sys/types.h /usr/include/standards.h
Clipboard.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Clipboard.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Clipboard.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Clipboard.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Clipboard.o: /usr/include/X11/Xresource.h /usr/include/string.h
Clipboard.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
Clipboard.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Clipboard.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Clipboard.o: /root/build/rosegarden/include/MidiConsts.h
Clipboard.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Clipboard.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Clipboard.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Clipboard.o: /root/build/rosegarden/include/MidiErrorHandler.h Parse.h
Clipboard.o: Globals.h /root/build/rosegarden/include/SysDeps.h
Clipboard.o: ../../config.h /usr/include/signal.h
Clipboard.o: /usr/include/internal/signal_core.h /usr/include/ctype.h
Clipboard.o: /usr/include/internal/ctype_core.h
Clipboard.o: /usr/include/internal/locale_attr_core.h Types.h
Clipboard.o: /root/build/rosegarden/include/MidiXInclude.h
Clipboard.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
Clipboard.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
Clipboard.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
Clipboard.o: /usr/include/X11/cursorfont.h
Clipboard.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Clipboard.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Clipboard.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Clipboard.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Clipboard.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Clipboard.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Clipboard.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Clipboard.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Clipboard.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Clipboard.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Clipboard.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Clipboard.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Clipboard.o: /root/build/rosegarden/include/MidiTrack.h
Clipboard.o: /root/build/rosegarden/include/ServiceNames.h
Clipboard.o: /root/build/rosegarden/include/Systems.h Consts.h
Clipboard.o: EventListWindow.h MainWindow.h
Clipboard.o: /root/build/rosegarden/include/ILClient.h
Clipboard.o: /root/build/rosegarden/include/ILTypes.h Clipboard.h Menu.h
Clipboard.o: EventListMenu.h PianoRollMenu.h PianoRoll.h
Clipboard.o: /root/build/rosegarden/include/Debug.h
Csound.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Csound.o: /usr/include/internal/sgimacros.h /usr/include/standards.h
Csound.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Csound.o: /root/build/rosegarden/include/MidiXInclude.h
Csound.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Csound.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Csound.o: /usr/include/string.h /usr/include/internal/string_core.h
Csound.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Csound.o: /usr/include/sys/types.h /usr/include/X11/X.h
Csound.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Csound.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Csound.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Csound.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Csound.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Csound.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Csound.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Csound.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Csound.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Csound.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
Csound.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
Csound.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Csound.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Csound.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Csound.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Csound.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Csound.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Csound.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Csound.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Csound.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Csound.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Csound.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Csound.o: /root/build/rosegarden/include/MidiErrorHandler.h
Csound.o: /root/build/rosegarden/include/MidiFile.h
Csound.o: /root/build/rosegarden/include/MidiEvent.h
Csound.o: /root/build/rosegarden/include/MidiConsts.h
Csound.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
Csound.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Csound.o: Globals.h Types.h /root/build/rosegarden/include/MidiTrack.h
Csound.o: /root/build/rosegarden/include/ServiceNames.h
Csound.o: /root/build/rosegarden/include/Systems.h Menu.h Main.h Tracking.h
Csound.o: Csound.h /root/build/rosegarden/include/Debug.h
Dispatch.o: /root/build/rosegarden/include/MidiXInclude.h
Dispatch.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Dispatch.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Dispatch.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Dispatch.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Dispatch.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Dispatch.o: /usr/include/string.h /usr/include/internal/string_core.h
Dispatch.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Dispatch.o: /usr/include/sys/types.h /usr/include/X11/X.h
Dispatch.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Dispatch.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Dispatch.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Dispatch.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Dispatch.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Dispatch.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Dispatch.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Dispatch.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Dispatch.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Dispatch.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
Dispatch.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
Dispatch.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Dispatch.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Dispatch.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Dispatch.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Dispatch.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Dispatch.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Dispatch.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Dispatch.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Dispatch.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Dispatch.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Dispatch.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Dispatch.o: /root/build/rosegarden/include/MidiFile.h
Dispatch.o: /root/build/rosegarden/include/MidiEvent.h
Dispatch.o: /root/build/rosegarden/include/MidiConsts.h
Dispatch.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
Dispatch.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Dispatch.o: TrackList.h Menu.h EventListWindow.h
Dispatch.o: /root/build/rosegarden/include/MidiTrack.h EventListMenu.h
Dispatch.o: PianoRoll.h PianoRollMenu.h Globals.h Types.h
Dispatch.o: /root/build/rosegarden/include/ServiceNames.h
Dispatch.o: /root/build/rosegarden/include/Systems.h
Dispatch.o: /root/build/rosegarden/include/Debug.h
EventDlgs.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
EventDlgs.o: /usr/include/internal/sgimacros.h /usr/include/standards.h
EventDlgs.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
EventDlgs.o: /root/build/rosegarden/include/MidiXInclude.h
EventDlgs.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
EventDlgs.o: /usr/include/signal.h /usr/include/internal/signal_core.h
EventDlgs.o: /usr/include/string.h /usr/include/internal/string_core.h
EventDlgs.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
EventDlgs.o: /usr/include/sys/types.h /usr/include/X11/X.h
EventDlgs.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
EventDlgs.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
EventDlgs.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
EventDlgs.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
EventDlgs.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
EventDlgs.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
EventDlgs.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
EventDlgs.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
EventDlgs.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
EventDlgs.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
EventDlgs.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
EventDlgs.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
EventDlgs.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
EventDlgs.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
EventDlgs.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
EventDlgs.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
EventDlgs.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
EventDlgs.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
EventDlgs.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
EventDlgs.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
EventDlgs.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
EventDlgs.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
EventDlgs.o: /root/build/rosegarden/include/MidiFile.h
EventDlgs.o: /root/build/rosegarden/include/MidiEvent.h
EventDlgs.o: /root/build/rosegarden/include/MidiConsts.h
EventDlgs.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
EventDlgs.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
EventDlgs.o: /root/build/rosegarden/include/MidiErrorHandler.h Globals.h
EventDlgs.o: Types.h /root/build/rosegarden/include/MidiTrack.h
EventDlgs.o: /root/build/rosegarden/include/ServiceNames.h
EventDlgs.o: /root/build/rosegarden/include/Systems.h EventDlgs.h
EventDlgs.o: /root/build/rosegarden/include/Debug.h
EventListMenu.o: /root/build/rosegarden/include/MidiXInclude.h
EventListMenu.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
EventListMenu.o: /usr/include/signal.h /usr/include/internal/signal_core.h
EventListMenu.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
EventListMenu.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
EventListMenu.o: /usr/include/internal/locale_attr_core.h
EventListMenu.o: /usr/include/sgidefs.h /usr/include/string.h
EventListMenu.o: /usr/include/internal/string_core.h
EventListMenu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
EventListMenu.o: /usr/include/sys/types.h /usr/include/X11/X.h
EventListMenu.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
EventListMenu.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
EventListMenu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
EventListMenu.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
EventListMenu.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
EventListMenu.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
EventListMenu.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
EventListMenu.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
EventListMenu.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
EventListMenu.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
EventListMenu.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
EventListMenu.o: /usr/include/internal/stdarg_core.h
EventListMenu.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
EventListMenu.o: /usr/include/X11/Xmu/Converters.h
EventListMenu.o: /usr/include/X11/Xaw/Viewport.h
EventListMenu.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Box.h
EventListMenu.o: /usr/include/X11/Xaw/MenuButton.h
EventListMenu.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
EventListMenu.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
EventListMenu.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
EventListMenu.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
EventListMenu.o: /usr/include/X11/Xaw/TextSrc.h
EventListMenu.o: /usr/include/X11/Xaw/AsciiSrc.h
EventListMenu.o: /usr/include/X11/Xaw/AsciiSink.h
EventListMenu.o: /usr/include/X11/Xaw/MultiSrc.h
EventListMenu.o: /usr/include/X11/Xaw/Repeater.h
EventListMenu.o: /usr/include/X11/Xaw/Scrollbar.h
EventListMenu.o: /root/build/rosegarden/include/MidiFile.h
EventListMenu.o: /root/build/rosegarden/include/MidiEvent.h
EventListMenu.o: /root/build/rosegarden/include/MidiConsts.h
EventListMenu.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
EventListMenu.o: /usr/include/internal/stdlib_core.h
EventListMenu.o: /usr/include/netinet/in.h
EventListMenu.o: /root/build/rosegarden/include/MidiErrorHandler.h Main.h
EventListMenu.o: Tracking.h Globals.h Types.h
EventListMenu.o: /root/build/rosegarden/include/MidiTrack.h
EventListMenu.o: /root/build/rosegarden/include/ServiceNames.h
EventListMenu.o: /root/build/rosegarden/include/Systems.h EventListWindow.h
EventListMenu.o: Menu.h TrackList.h Clipboard.h EventDlgs.h PianoRoll.h
EventListMenu.o: Dispatch.h Undo.h /root/build/rosegarden/include/Debug.h
EventListMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/piano_roll.xbm
EventListMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/quantize.xbm
EventListMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/cut.xbm
EventListMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/copy.xbm
EventListMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/paste.xbm
EventListMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
EventListMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/undo.xbm
EventListWindow.o: /root/build/rosegarden/include/Mapper.h
EventListWindow.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
EventListWindow.o: /usr/include/signal.h /usr/include/internal/signal_core.h
EventListWindow.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
EventListWindow.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
EventListWindow.o: /usr/include/internal/locale_attr_core.h
EventListWindow.o: /usr/include/sgidefs.h /usr/include/string.h
EventListWindow.o: /usr/include/internal/string_core.h
EventListWindow.o: /root/build/rosegarden/include/Systems.h
EventListWindow.o: /root/build/rosegarden/include/Lists.h
EventListWindow.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
EventListWindow.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
EventListWindow.o: /root/build/rosegarden/include/MidiXInclude.h
EventListWindow.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
EventListWindow.o: /usr/include/sys/types.h /usr/include/X11/X.h
EventListWindow.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
EventListWindow.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
EventListWindow.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
EventListWindow.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
EventListWindow.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
EventListWindow.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
EventListWindow.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
EventListWindow.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
EventListWindow.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
EventListWindow.o: /root/build/rosegarden/include/Yawn.h
EventListWindow.o: /usr/include/stdarg.h /usr/include/internal/stdarg_core.h
EventListWindow.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
EventListWindow.o: /usr/include/X11/Xmu/Converters.h
EventListWindow.o: /usr/include/X11/Xaw/Viewport.h
EventListWindow.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Box.h
EventListWindow.o: /usr/include/X11/Xaw/MenuButton.h
EventListWindow.o: /usr/include/X11/Xaw/Command.h
EventListWindow.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
EventListWindow.o: /usr/include/X11/Xaw/Toggle.h /usr/include/X11/Xaw/List.h
EventListWindow.o: /usr/include/X11/Xaw/AsciiText.h
EventListWindow.o: /usr/include/X11/Xaw/Text.h
EventListWindow.o: /usr/include/X11/Xaw/TextSink.h
EventListWindow.o: /usr/include/X11/Xaw/TextSrc.h
EventListWindow.o: /usr/include/X11/Xaw/AsciiSrc.h
EventListWindow.o: /usr/include/X11/Xaw/AsciiSink.h
EventListWindow.o: /usr/include/X11/Xaw/MultiSrc.h
EventListWindow.o: /usr/include/X11/Xaw/Repeater.h
EventListWindow.o: /usr/include/X11/Xaw/Scrollbar.h
EventListWindow.o: /root/build/rosegarden/include/MidiEvent.h
EventListWindow.o: /root/build/rosegarden/include/MidiConsts.h
EventListWindow.o: /root/build/rosegarden/include/Mapper_SGI.h
EventListWindow.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
EventListWindow.o: /usr/include/dmedia/midi.h /usr/include/sys/time.h
EventListWindow.o: /usr/include/sys/midi.h /usr/include/sys/ioctl.h
EventListWindow.o: /usr/include/sys/ioccom.h /usr/include/net/soioctl.h
EventListWindow.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
EventListWindow.o: /usr/include/sys/ttydev.h
EventListWindow.o: /root/build/rosegarden/include/MidiFile.h
EventListWindow.o: /usr/include/netinet/in.h
EventListWindow.o: /root/build/rosegarden/include/MidiErrorHandler.h
EventListWindow.o: Globals.h Types.h
EventListWindow.o: /root/build/rosegarden/include/MidiTrack.h
EventListWindow.o: /root/build/rosegarden/include/ServiceNames.h Consts.h
EventListWindow.o: Main.h Tracking.h EventListWindow.h EventListMenu.h
EventListWindow.o: Parse.h Clipboard.h Undo.h TrackList.h
EventListWindow.o: /root/build/rosegarden/include/Debug.h
Filter.o: Globals.h /root/build/rosegarden/include/SysDeps.h ../../config.h
Filter.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Filter.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Filter.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Filter.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Filter.o: /usr/include/string.h /usr/include/internal/string_core.h Types.h
Filter.o: /root/build/rosegarden/include/MidiXInclude.h
Filter.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Filter.o: /usr/include/sys/types.h /usr/include/X11/X.h
Filter.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Filter.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Filter.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Filter.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Filter.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Filter.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Filter.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Filter.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Filter.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Filter.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
Filter.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
Filter.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Filter.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Filter.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Filter.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Filter.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Filter.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Filter.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Filter.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Filter.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Filter.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Filter.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Filter.o: /root/build/rosegarden/include/MidiFile.h
Filter.o: /root/build/rosegarden/include/MidiEvent.h
Filter.o: /root/build/rosegarden/include/MidiConsts.h
Filter.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
Filter.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Filter.o: /root/build/rosegarden/include/MidiTrack.h
Filter.o: /root/build/rosegarden/include/ServiceNames.h
Filter.o: /root/build/rosegarden/include/Systems.h Menu.h
Filter.o: /root/build/rosegarden/include/Debug.h
Filter.o: /root/build/rosegarden/include/MidiErrorHandler.h
Filter.o: /usr/include/unistd.h /usr/include/sys/wait.h /usr/include/dirent.h
Filter.o: /usr/include/sys/dirent.h /usr/include/sys/stat.h
Filter.o: /usr/include/sys/timespec.h
Main.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Main.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Main.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Main.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Main.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Main.o: /usr/include/string.h /usr/include/internal/string_core.h
Main.o: /root/build/rosegarden/include/ILClient.h
Main.o: /root/build/rosegarden/include/ILTypes.h /usr/include/X11/Intrinsic.h
Main.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h /usr/include/X11/X.h
Main.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Main.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Main.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Main.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Main.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Main.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Main.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Main.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Main.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Main.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Main.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Main.o: /usr/include/internal/stdlib_core.h
Main.o: /root/build/rosegarden/include/Mapper.h
Main.o: /root/build/rosegarden/include/Systems.h
Main.o: /root/build/rosegarden/include/MidiXInclude.h
Main.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Main.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Main.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Main.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Main.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Main.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Main.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Main.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Main.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Main.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Main.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Main.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Main.o: /root/build/rosegarden/include/MidiEvent.h
Main.o: /root/build/rosegarden/include/MidiConsts.h
Main.o: /root/build/rosegarden/include/Mapper_SGI.h
Main.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
Main.o: /usr/include/dmedia/midi.h /usr/include/sys/time.h
Main.o: /usr/include/sys/midi.h /usr/include/sys/ioctl.h
Main.o: /usr/include/sys/ioccom.h /usr/include/net/soioctl.h
Main.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
Main.o: /usr/include/sys/ttydev.h /usr/include/unistd.h
Main.o: /root/build/rosegarden/include/MidiFile.h /usr/include/netinet/in.h
Main.o: /root/build/rosegarden/include/MidiErrorHandler.h
Main.o: /root/build/rosegarden/include/MidiVarLenNums.h Sequence.h Types.h
Main.o: Consts.h Resources.h Fallbacks.h Globals.h
Main.o: /root/build/rosegarden/include/MidiTrack.h
Main.o: /root/build/rosegarden/include/ServiceNames.h Menu.h Main.h
Main.o: Tracking.h MainWindow.h TrackList.h PianoRoll.h EventListWindow.h
Main.o: /root/build/rosegarden/include/Debug.h
Main.o: /root/build/rosegarden/common/bitmaps/hourglass.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hourglass_mask.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour0.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour1.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour2.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour3.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour4.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour5.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour6.xbm
Main.o: /root/build/rosegarden/common/bitmaps/hour7.xbm
MainWindow.o: Types.h /root/build/rosegarden/include/MidiXInclude.h
MainWindow.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MainWindow.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MainWindow.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
MainWindow.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MainWindow.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
MainWindow.o: /usr/include/string.h /usr/include/internal/string_core.h
MainWindow.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
MainWindow.o: /usr/include/sys/types.h /usr/include/X11/X.h
MainWindow.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
MainWindow.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
MainWindow.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
MainWindow.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
MainWindow.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
MainWindow.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
MainWindow.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
MainWindow.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
MainWindow.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
MainWindow.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
MainWindow.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
MainWindow.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
MainWindow.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
MainWindow.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
MainWindow.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
MainWindow.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
MainWindow.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
MainWindow.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
MainWindow.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
MainWindow.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
MainWindow.o: /usr/include/X11/Xaw/AsciiSink.h
MainWindow.o: /usr/include/X11/Xaw/MultiSrc.h /usr/include/X11/Xaw/Repeater.h
MainWindow.o: /usr/include/X11/Xaw/Scrollbar.h
MainWindow.o: /root/build/rosegarden/include/MidiFile.h
MainWindow.o: /root/build/rosegarden/include/MidiEvent.h
MainWindow.o: /root/build/rosegarden/include/MidiConsts.h
MainWindow.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
MainWindow.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
MainWindow.o: Globals.h /root/build/rosegarden/include/MidiTrack.h
MainWindow.o: /root/build/rosegarden/include/ServiceNames.h
MainWindow.o: /root/build/rosegarden/include/Systems.h MainWindow.h
MainWindow.o: /root/build/rosegarden/include/ILClient.h
MainWindow.o: /root/build/rosegarden/include/ILTypes.h TrackList.h Message.h
MainWindow.o: Menu.h Main.h Tracking.h
MainWindow.o: /root/build/rosegarden/include/Version.h
MainWindow.o: /root/build/rosegarden/include/MidiErrorHandler.h
MainWindow.o: /root/build/rosegarden/include/Debug.h
MainWindow.o: /root/build/rosegarden/common/bitmaps/rose.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/rose_mask.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/up.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/down.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/rewind.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/back.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/forward.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/ffwd.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/lightgrey.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/darkgrey.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/zoomin.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/zoomout.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/clef_treble.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/clef_bass.xbm
MainWindow.o: /root/build/rosegarden/common/bitmaps/notemod_sharp.xbm
Menu.o: /root/build/rosegarden/include/ILClient.h
Menu.o: /root/build/rosegarden/include/ILTypes.h /usr/include/X11/Intrinsic.h
Menu.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
Menu.o: /usr/include/standards.h /usr/include/sgidefs.h
Menu.o: /usr/include/internal/sgimacros.h /usr/include/X11/X.h
Menu.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Menu.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Menu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Menu.o: /usr/include/string.h /usr/include/internal/string_core.h
Menu.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Menu.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Menu.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Menu.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Menu.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Menu.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Menu.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Menu.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Menu.o: /usr/include/internal/stdlib_core.h Globals.h
Menu.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Menu.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Menu.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Menu.o: /usr/include/internal/locale_attr_core.h Types.h
Menu.o: /root/build/rosegarden/include/MidiXInclude.h
Menu.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Menu.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Menu.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Menu.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Menu.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Menu.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Menu.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Menu.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Menu.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Menu.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Menu.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Menu.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Menu.o: /root/build/rosegarden/include/MidiFile.h
Menu.o: /root/build/rosegarden/include/MidiEvent.h
Menu.o: /root/build/rosegarden/include/MidiConsts.h /usr/include/netinet/in.h
Menu.o: /root/build/rosegarden/include/MidiTrack.h
Menu.o: /root/build/rosegarden/include/ServiceNames.h
Menu.o: /root/build/rosegarden/include/Systems.h Menu.h Main.h Tracking.h
Menu.o: MainWindow.h TrackList.h EventListWindow.h PianoRollMenu.h
Menu.o: PianoRoll.h Message.h Undo.h Clipboard.h Consts.h Sequence.h Record.h
Menu.o: Dispatch.h EventListMenu.h Csound.h MidiSetupDlgs.h
Menu.o: /usr/include/unistd.h /root/build/rosegarden/include/Debug.h
Menu.o: /root/build/rosegarden/include/MidiErrorHandler.h
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/beam.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/undo.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/copy.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/cut.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/paste.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/open.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/save.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/quantize.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/event_list.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/piano_roll.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/sequence.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/stop.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/record.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/ffwd.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/rwd.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/rrwd.xbm
Menu.o: /root/build/rosegarden/common/bitmaps/toolbar/skp.xbm
Message.o: /root/build/rosegarden/include/MidiXInclude.h
Message.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Message.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Message.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Message.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Message.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Message.o: /usr/include/string.h /usr/include/internal/string_core.h
Message.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Message.o: /usr/include/sys/types.h /usr/include/X11/X.h
Message.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Message.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Message.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Message.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Message.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Message.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Message.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Message.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Message.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Message.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
Message.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
Message.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Message.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Message.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Message.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Message.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Message.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Message.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Message.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Message.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Message.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Message.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Message.o: /root/build/rosegarden/include/MidiFile.h
Message.o: /root/build/rosegarden/include/MidiEvent.h
Message.o: /root/build/rosegarden/include/MidiConsts.h
Message.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
Message.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Message.o: Globals.h Types.h /root/build/rosegarden/include/MidiTrack.h
Message.o: /root/build/rosegarden/include/ServiceNames.h
Message.o: /root/build/rosegarden/include/Systems.h Message.h
Message.o: /root/build/rosegarden/include/Debug.h
MidiSetupDlgs.o: /root/build/rosegarden/include/Mapper.h
MidiSetupDlgs.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
MidiSetupDlgs.o: /usr/include/signal.h /usr/include/internal/signal_core.h
MidiSetupDlgs.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
MidiSetupDlgs.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
MidiSetupDlgs.o: /usr/include/internal/locale_attr_core.h
MidiSetupDlgs.o: /usr/include/sgidefs.h /usr/include/string.h
MidiSetupDlgs.o: /usr/include/internal/string_core.h
MidiSetupDlgs.o: /root/build/rosegarden/include/Systems.h
MidiSetupDlgs.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
MidiSetupDlgs.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
MidiSetupDlgs.o: /usr/include/internal/stdlib_core.h
MidiSetupDlgs.o: /root/build/rosegarden/include/MidiXInclude.h
MidiSetupDlgs.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
MidiSetupDlgs.o: /usr/include/sys/types.h /usr/include/X11/X.h
MidiSetupDlgs.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
MidiSetupDlgs.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
MidiSetupDlgs.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
MidiSetupDlgs.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
MidiSetupDlgs.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
MidiSetupDlgs.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
MidiSetupDlgs.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
MidiSetupDlgs.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
MidiSetupDlgs.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
MidiSetupDlgs.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
MidiSetupDlgs.o: /usr/include/internal/stdarg_core.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
MidiSetupDlgs.o: /usr/include/X11/Xmu/Converters.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Viewport.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Box.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/MenuButton.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/TextSrc.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/AsciiSrc.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/AsciiSink.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/MultiSrc.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Repeater.h
MidiSetupDlgs.o: /usr/include/X11/Xaw/Scrollbar.h
MidiSetupDlgs.o: /root/build/rosegarden/include/MidiEvent.h
MidiSetupDlgs.o: /root/build/rosegarden/include/MidiConsts.h
MidiSetupDlgs.o: /root/build/rosegarden/include/Mapper_SGI.h
MidiSetupDlgs.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
MidiSetupDlgs.o: /usr/include/dmedia/midi.h /usr/include/sys/time.h
MidiSetupDlgs.o: /usr/include/sys/midi.h /usr/include/sys/ioctl.h
MidiSetupDlgs.o: /usr/include/sys/ioccom.h /usr/include/net/soioctl.h
MidiSetupDlgs.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
MidiSetupDlgs.o: /usr/include/sys/ttydev.h
MidiSetupDlgs.o: /root/build/rosegarden/include/MidiFile.h
MidiSetupDlgs.o: /usr/include/netinet/in.h Globals.h Types.h
MidiSetupDlgs.o: /root/build/rosegarden/include/MidiTrack.h
MidiSetupDlgs.o: /root/build/rosegarden/include/ServiceNames.h EventDlgs.h
MidiSetupDlgs.o: Sequence.h Main.h Tracking.h Menu.h MidiSetupDlgs.h
MidiSetupDlgs.o: EventListWindow.h TrackList.h EventListMenu.h
MidiSetupDlgs.o: /root/build/rosegarden/include/Debug.h
Parse.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
Parse.o: /usr/include/standards.h /usr/include/sgidefs.h
Parse.o: /usr/include/internal/sgimacros.h
Parse.o: /root/build/rosegarden/include/MidiConsts.h
Parse.o: /root/build/rosegarden/include/MidiFile.h
Parse.o: /root/build/rosegarden/include/MidiEvent.h
Parse.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Parse.o: /usr/include/sys/types.h /usr/include/X11/X.h
Parse.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Parse.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Parse.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Parse.o: /usr/include/string.h /usr/include/internal/string_core.h
Parse.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Parse.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Parse.o: /usr/include/X11/RectObj.h /root/build/rosegarden/include/Lists.h
Parse.o: /usr/include/stdlib.h /usr/include/internal/stdlib_core.h
Parse.o: /usr/include/netinet/in.h
Parse.o: /root/build/rosegarden/include/MidiErrorHandler.h
Parse.o: /root/build/rosegarden/include/MidiVarLenNums.h Types.h
Parse.o: /root/build/rosegarden/include/MidiXInclude.h
Parse.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Parse.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Parse.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Parse.o: /usr/include/internal/locale_attr_core.h
Parse.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
Parse.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
Parse.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
Parse.o: /usr/include/X11/cursorfont.h /root/build/rosegarden/include/Yawn.h
Parse.o: /usr/include/stdarg.h /usr/include/internal/stdarg_core.h
Parse.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
Parse.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Viewport.h
Parse.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Box.h
Parse.o: /usr/include/X11/Xaw/MenuButton.h /usr/include/X11/Xaw/Command.h
Parse.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
Parse.o: /usr/include/X11/Xaw/Toggle.h /usr/include/X11/Xaw/List.h
Parse.o: /usr/include/X11/Xaw/AsciiText.h /usr/include/X11/Xaw/Text.h
Parse.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
Parse.o: /usr/include/X11/Xaw/AsciiSrc.h /usr/include/X11/Xaw/AsciiSink.h
Parse.o: /usr/include/X11/Xaw/MultiSrc.h /usr/include/X11/Xaw/Repeater.h
Parse.o: /usr/include/X11/Xaw/Scrollbar.h Parse.h Globals.h
Parse.o: /root/build/rosegarden/include/MidiTrack.h
Parse.o: /root/build/rosegarden/include/ServiceNames.h
Parse.o: /root/build/rosegarden/include/Systems.h
Parse.o: /root/build/rosegarden/include/Debug.h
PianoRoll.o: /root/build/rosegarden/include/MidiXInclude.h
PianoRoll.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
PianoRoll.o: /usr/include/signal.h /usr/include/internal/signal_core.h
PianoRoll.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
PianoRoll.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
PianoRoll.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
PianoRoll.o: /usr/include/string.h /usr/include/internal/string_core.h
PianoRoll.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
PianoRoll.o: /usr/include/sys/types.h /usr/include/X11/X.h
PianoRoll.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
PianoRoll.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
PianoRoll.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
PianoRoll.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
PianoRoll.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
PianoRoll.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
PianoRoll.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
PianoRoll.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
PianoRoll.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
PianoRoll.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
PianoRoll.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
PianoRoll.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
PianoRoll.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
PianoRoll.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
PianoRoll.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
PianoRoll.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
PianoRoll.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
PianoRoll.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
PianoRoll.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
PianoRoll.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
PianoRoll.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
PianoRoll.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
PianoRoll.o: /root/build/rosegarden/include/MidiFile.h
PianoRoll.o: /root/build/rosegarden/include/MidiEvent.h
PianoRoll.o: /root/build/rosegarden/include/MidiConsts.h
PianoRoll.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
PianoRoll.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
PianoRoll.o: /root/build/rosegarden/include/MidiErrorHandler.h Globals.h
PianoRoll.o: Types.h /root/build/rosegarden/include/MidiTrack.h
PianoRoll.o: /root/build/rosegarden/include/ServiceNames.h
PianoRoll.o: /root/build/rosegarden/include/Systems.h Consts.h Main.h
PianoRoll.o: Tracking.h PianoRoll.h PianoRollMenu.h EventListMenu.h
PianoRoll.o: EventListWindow.h Parse.h Menu.h Clipboard.h Undo.h
PianoRoll.o: /root/build/rosegarden/include/Debug.h
PianoRollMenu.o: /root/build/rosegarden/include/MidiXInclude.h
PianoRollMenu.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
PianoRollMenu.o: /usr/include/signal.h /usr/include/internal/signal_core.h
PianoRollMenu.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
PianoRollMenu.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
PianoRollMenu.o: /usr/include/internal/locale_attr_core.h
PianoRollMenu.o: /usr/include/sgidefs.h /usr/include/string.h
PianoRollMenu.o: /usr/include/internal/string_core.h
PianoRollMenu.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
PianoRollMenu.o: /usr/include/sys/types.h /usr/include/X11/X.h
PianoRollMenu.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
PianoRollMenu.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
PianoRollMenu.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
PianoRollMenu.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
PianoRollMenu.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
PianoRollMenu.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
PianoRollMenu.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
PianoRollMenu.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
PianoRollMenu.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
PianoRollMenu.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
PianoRollMenu.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
PianoRollMenu.o: /usr/include/internal/stdarg_core.h
PianoRollMenu.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
PianoRollMenu.o: /usr/include/X11/Xmu/Converters.h
PianoRollMenu.o: /usr/include/X11/Xaw/Viewport.h
PianoRollMenu.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Box.h
PianoRollMenu.o: /usr/include/X11/Xaw/MenuButton.h
PianoRollMenu.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
PianoRollMenu.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
PianoRollMenu.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
PianoRollMenu.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
PianoRollMenu.o: /usr/include/X11/Xaw/TextSrc.h
PianoRollMenu.o: /usr/include/X11/Xaw/AsciiSrc.h
PianoRollMenu.o: /usr/include/X11/Xaw/AsciiSink.h
PianoRollMenu.o: /usr/include/X11/Xaw/MultiSrc.h
PianoRollMenu.o: /usr/include/X11/Xaw/Repeater.h
PianoRollMenu.o: /usr/include/X11/Xaw/Scrollbar.h
PianoRollMenu.o: /root/build/rosegarden/include/MidiFile.h
PianoRollMenu.o: /root/build/rosegarden/include/MidiEvent.h
PianoRollMenu.o: /root/build/rosegarden/include/MidiConsts.h
PianoRollMenu.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
PianoRollMenu.o: /usr/include/internal/stdlib_core.h
PianoRollMenu.o: /usr/include/netinet/in.h
PianoRollMenu.o: /root/build/rosegarden/include/MidiErrorHandler.h Globals.h
PianoRollMenu.o: Types.h /root/build/rosegarden/include/MidiTrack.h
PianoRollMenu.o: /root/build/rosegarden/include/ServiceNames.h
PianoRollMenu.o: /root/build/rosegarden/include/Systems.h PianoRoll.h Menu.h
PianoRollMenu.o: TrackList.h Clipboard.h EventDlgs.h Dispatch.h Undo.h
PianoRollMenu.o: EventListWindow.h /root/build/rosegarden/include/Debug.h
PianoRollMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/event_list.xbm
PianoRollMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/quantize.xbm
PianoRollMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/cut.xbm
PianoRollMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/copy.xbm
PianoRollMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/paste.xbm
PianoRollMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
PianoRollMenu.o: /root/build/rosegarden/common/bitmaps/toolbar/undo.xbm
Record.o: Globals.h /root/build/rosegarden/include/SysDeps.h ../../config.h
Record.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Record.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Record.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Record.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Record.o: /usr/include/string.h /usr/include/internal/string_core.h Types.h
Record.o: /root/build/rosegarden/include/MidiXInclude.h
Record.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Record.o: /usr/include/sys/types.h /usr/include/X11/X.h
Record.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Record.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Record.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Record.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Record.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Record.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Record.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Record.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Record.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Record.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdio.h
Record.o: /usr/include/internal/stdio_core.h /usr/include/stdarg.h
Record.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Record.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Record.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Record.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Record.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Record.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Record.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Record.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Record.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Record.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Record.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Record.o: /root/build/rosegarden/include/MidiFile.h
Record.o: /root/build/rosegarden/include/MidiEvent.h
Record.o: /root/build/rosegarden/include/MidiConsts.h
Record.o: /root/build/rosegarden/include/Lists.h /usr/include/stdlib.h
Record.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Record.o: /root/build/rosegarden/include/MidiTrack.h
Record.o: /root/build/rosegarden/include/ServiceNames.h
Record.o: /root/build/rosegarden/include/Systems.h Consts.h Main.h Tracking.h
Record.o: Menu.h /root/build/rosegarden/include/Mapper.h
Record.o: /root/build/rosegarden/include/Mapper_SGI.h
Record.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
Record.o: /usr/include/dmedia/midi.h /usr/include/sys/time.h
Record.o: /usr/include/sys/midi.h /usr/include/sys/ioctl.h
Record.o: /usr/include/sys/ioccom.h /usr/include/net/soioctl.h
Record.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
Record.o: /usr/include/sys/ttydev.h Record.h Sequence.h
Record.o: /root/build/rosegarden/include/MidiErrorHandler.h
Record.o: /root/build/rosegarden/include/Debug.h
Record.o: /root/build/rosegarden/include/MidiBHeap.h TrackList.h
Record.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
Record.o: /usr/include/sys/times.h /usr/include/unistd.h
Record.o: /usr/include/sys/signal.h
Sequence.o: /root/build/rosegarden/include/Mapper.h
Sequence.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Sequence.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Sequence.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Sequence.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Sequence.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Sequence.o: /usr/include/string.h /usr/include/internal/string_core.h
Sequence.o: /root/build/rosegarden/include/Systems.h
Sequence.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Sequence.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Sequence.o: /usr/include/internal/stdlib_core.h
Sequence.o: /root/build/rosegarden/include/MidiXInclude.h
Sequence.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Sequence.o: /usr/include/sys/types.h /usr/include/X11/X.h
Sequence.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Sequence.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Sequence.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Sequence.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Sequence.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Sequence.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Sequence.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Sequence.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Sequence.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Sequence.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Sequence.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Sequence.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Sequence.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Sequence.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Sequence.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Sequence.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Sequence.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Sequence.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Sequence.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Sequence.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Sequence.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Sequence.o: /root/build/rosegarden/include/MidiEvent.h
Sequence.o: /root/build/rosegarden/include/MidiConsts.h
Sequence.o: /root/build/rosegarden/include/Mapper_SGI.h
Sequence.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
Sequence.o: /usr/include/dmedia/midi.h /usr/include/sys/time.h
Sequence.o: /usr/include/sys/midi.h /usr/include/sys/ioctl.h
Sequence.o: /usr/include/sys/ioccom.h /usr/include/net/soioctl.h
Sequence.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
Sequence.o: /usr/include/sys/ttydev.h Globals.h Types.h
Sequence.o: /root/build/rosegarden/include/MidiFile.h
Sequence.o: /usr/include/netinet/in.h
Sequence.o: /root/build/rosegarden/include/MidiTrack.h
Sequence.o: /root/build/rosegarden/include/ServiceNames.h Consts.h
Sequence.o: /root/build/rosegarden/include/MidiErrorHandler.h
Sequence.o: /root/build/rosegarden/include/MidiBHeap.h Tracking.h Sequence.h
Sequence.o: Menu.h Record.h MidiSetupDlgs.h
Sequence.o: /root/build/rosegarden/include/Debug.h
Tracking.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
Tracking.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Tracking.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
Tracking.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Tracking.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
Tracking.o: /usr/include/string.h /usr/include/internal/string_core.h
Tracking.o: /root/build/rosegarden/include/ILClient.h
Tracking.o: /root/build/rosegarden/include/ILTypes.h
Tracking.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Tracking.o: /usr/include/sys/types.h /usr/include/X11/X.h
Tracking.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
Tracking.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
Tracking.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
Tracking.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
Tracking.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
Tracking.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
Tracking.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
Tracking.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
Tracking.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
Tracking.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Tracking.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Tracking.o: /usr/include/internal/stdlib_core.h /usr/include/unistd.h
Tracking.o: /root/build/rosegarden/include/MidiFile.h
Tracking.o: /root/build/rosegarden/include/MidiEvent.h
Tracking.o: /root/build/rosegarden/include/MidiConsts.h
Tracking.o: /usr/include/netinet/in.h
Tracking.o: /root/build/rosegarden/include/MidiErrorHandler.h
Tracking.o: /root/build/rosegarden/include/MidiVarLenNums.h Menu.h Types.h
Tracking.o: /root/build/rosegarden/include/MidiXInclude.h
Tracking.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
Tracking.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
Tracking.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
Tracking.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
Tracking.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
Tracking.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
Tracking.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
Tracking.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
Tracking.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
Tracking.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
Tracking.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
Tracking.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
Tracking.o: Consts.h Globals.h /root/build/rosegarden/include/MidiTrack.h
Tracking.o: /root/build/rosegarden/include/ServiceNames.h
Tracking.o: /root/build/rosegarden/include/Systems.h Main.h Tracking.h
Tracking.o: /root/build/rosegarden/include/Debug.h
TrackList.o: /root/build/rosegarden/include/Mapper.h
TrackList.o: /root/build/rosegarden/include/SysDeps.h ../../config.h
TrackList.o: /usr/include/signal.h /usr/include/internal/signal_core.h
TrackList.o: /usr/include/standards.h /usr/include/internal/sgimacros.h
TrackList.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
TrackList.o: /usr/include/internal/locale_attr_core.h /usr/include/sgidefs.h
TrackList.o: /usr/include/string.h /usr/include/internal/string_core.h
TrackList.o: /root/build/rosegarden/include/Systems.h
TrackList.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
TrackList.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
TrackList.o: /usr/include/internal/stdlib_core.h
TrackList.o: /root/build/rosegarden/include/MidiXInclude.h
TrackList.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
TrackList.o: /usr/include/sys/types.h /usr/include/X11/X.h
TrackList.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
TrackList.o: /usr/include/stddef.h /usr/include/internal/stddef_core.h
TrackList.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
TrackList.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
TrackList.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
TrackList.o: /usr/include/X11/RectObj.h /usr/include/X11/StringDefs.h
TrackList.o: /usr/include/X11/Shell.h /usr/include/X11/SM/SMlib.h
TrackList.o: /usr/include/X11/SM/SM.h /usr/include/X11/ICE/ICElib.h
TrackList.o: /usr/include/X11/ICE/ICE.h /usr/include/X11/cursorfont.h
TrackList.o: /root/build/rosegarden/include/Yawn.h /usr/include/stdarg.h
TrackList.o: /usr/include/internal/stdarg_core.h /usr/include/X11/Xaw/Form.h
TrackList.o: /usr/include/X11/Xaw/Paned.h /usr/include/X11/Xmu/Converters.h
TrackList.o: /usr/include/X11/Xaw/Viewport.h /usr/include/X11/Xaw/Reports.h
TrackList.o: /usr/include/X11/Xaw/Box.h /usr/include/X11/Xaw/MenuButton.h
TrackList.o: /usr/include/X11/Xaw/Command.h /usr/include/X11/Xaw/Label.h
TrackList.o: /usr/include/X11/Xaw/Simple.h /usr/include/X11/Xaw/Toggle.h
TrackList.o: /usr/include/X11/Xaw/List.h /usr/include/X11/Xaw/AsciiText.h
TrackList.o: /usr/include/X11/Xaw/Text.h /usr/include/X11/Xaw/TextSink.h
TrackList.o: /usr/include/X11/Xaw/TextSrc.h /usr/include/X11/Xaw/AsciiSrc.h
TrackList.o: /usr/include/X11/Xaw/AsciiSink.h /usr/include/X11/Xaw/MultiSrc.h
TrackList.o: /usr/include/X11/Xaw/Repeater.h /usr/include/X11/Xaw/Scrollbar.h
TrackList.o: /root/build/rosegarden/include/MidiEvent.h
TrackList.o: /root/build/rosegarden/include/MidiConsts.h
TrackList.o: /root/build/rosegarden/include/Mapper_SGI.h
TrackList.o: /usr/include/sys/bsd_types.h /usr/include/sys/select.h
TrackList.o: /usr/include/dmedia/midi.h /usr/include/sys/time.h
TrackList.o: /usr/include/sys/midi.h /usr/include/sys/ioctl.h
TrackList.o: /usr/include/sys/ioccom.h /usr/include/net/soioctl.h
TrackList.o: /usr/include/sys/termio.h /usr/include/sys/termios.h
TrackList.o: /usr/include/sys/ttydev.h Globals.h Types.h
TrackList.o: /root/build/rosegarden/include/MidiFile.h
TrackList.o: /usr/include/netinet/in.h
TrackList.o: /root/build/rosegarden/include/MidiTrack.h
TrackList.o: /root/build/rosegarden/include/ServiceNames.h Consts.h
TrackList.o: EventListWindow.h PianoRoll.h Clipboard.h MainWindow.h
TrackList.o: /root/build/rosegarden/include/ILClient.h
TrackList.o: /root/build/rosegarden/include/ILTypes.h Main.h Tracking.h
TrackList.o: Menu.h EventListMenu.h PianoRollMenu.h Undo.h
TrackList.o: /root/build/rosegarden/include/MidiErrorHandler.h
TrackList.o: /root/build/rosegarden/include/Debug.h
Undo.o: /root/build/rosegarden/include/MidiErrorHandler.h
Undo.o: /root/build/rosegarden/include/MidiFile.h
Undo.o: /root/build/rosegarden/include/MidiEvent.h
Undo.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
Undo.o: /usr/include/sys/types.h /usr/include/standards.h
Undo.o: /usr/include/sgidefs.h /usr/include/internal/sgimacros.h
Undo.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
Undo.o: /usr/include/X11/Xosdefs.h /usr/include/stddef.h
Undo.o: /usr/include/internal/stddef_core.h /usr/include/X11/Xutil.h
Undo.o: /usr/include/X11/Xresource.h /usr/include/string.h
Undo.o: /usr/include/internal/string_core.h /usr/include/X11/Core.h
Undo.o: /usr/include/X11/Composite.h /usr/include/X11/Constraint.h
Undo.o: /usr/include/X11/Object.h /usr/include/X11/RectObj.h
Undo.o: /root/build/rosegarden/include/MidiConsts.h
Undo.o: /root/build/rosegarden/include/Lists.h /usr/include/stdio.h
Undo.o: /usr/include/internal/stdio_core.h /usr/include/stdlib.h
Undo.o: /usr/include/internal/stdlib_core.h /usr/include/netinet/in.h
Undo.o: Globals.h /root/build/rosegarden/include/SysDeps.h ../../config.h
Undo.o: /usr/include/signal.h /usr/include/internal/signal_core.h
Undo.o: /usr/include/ctype.h /usr/include/internal/ctype_core.h
Undo.o: /usr/include/internal/locale_attr_core.h Types.h
Undo.o: /root/build/rosegarden/include/MidiXInclude.h
Undo.o: /usr/include/X11/StringDefs.h /usr/include/X11/Shell.h
Undo.o: /usr/include/X11/SM/SMlib.h /usr/include/X11/SM/SM.h
Undo.o: /usr/include/X11/ICE/ICElib.h /usr/include/X11/ICE/ICE.h
Undo.o: /usr/include/X11/cursorfont.h /root/build/rosegarden/include/Yawn.h
Undo.o: /usr/include/stdarg.h /usr/include/internal/stdarg_core.h
Undo.o: /usr/include/X11/Xaw/Form.h /usr/include/X11/Xaw/Paned.h
Undo.o: /usr/include/X11/Xmu/Converters.h /usr/include/X11/Xaw/Viewport.h
Undo.o: /usr/include/X11/Xaw/Reports.h /usr/include/X11/Xaw/Box.h
Undo.o: /usr/include/X11/Xaw/MenuButton.h /usr/include/X11/Xaw/Command.h
Undo.o: /usr/include/X11/Xaw/Label.h /usr/include/X11/Xaw/Simple.h
Undo.o: /usr/include/X11/Xaw/Toggle.h /usr/include/X11/Xaw/List.h
Undo.o: /usr/include/X11/Xaw/AsciiText.h /usr/include/X11/Xaw/Text.h
Undo.o: /usr/include/X11/Xaw/TextSink.h /usr/include/X11/Xaw/TextSrc.h
Undo.o: /usr/include/X11/Xaw/AsciiSrc.h /usr/include/X11/Xaw/AsciiSink.h
Undo.o: /usr/include/X11/Xaw/MultiSrc.h /usr/include/X11/Xaw/Repeater.h
Undo.o: /usr/include/X11/Xaw/Scrollbar.h
Undo.o: /root/build/rosegarden/include/MidiTrack.h
Undo.o: /root/build/rosegarden/include/ServiceNames.h
Undo.o: /root/build/rosegarden/include/Systems.h Undo.h EventListWindow.h
Undo.o: PianoRoll.h Main.h Tracking.h Menu.h TrackList.h EventListMenu.h
Undo.o: PianoRollMenu.h /root/build/rosegarden/include/Debug.h

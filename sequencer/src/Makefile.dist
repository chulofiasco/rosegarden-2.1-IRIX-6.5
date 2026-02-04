
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
		$(MAKEDEPEND) -- $(CFLAGS) -- `echo $(OBJECTS)|sed s/\\\.o/.c/g`

# DO NOT DELETE THIS LINE -- make depend depends on it.

Clipboard.o: /home/cannam/rosegarden/include/MidiFile.h
Clipboard.o: /home/cannam/rosegarden/include/MidiEvent.h
Clipboard.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Clipboard.o: /usr/include/sys/types.h /usr/include/linux/types.h
Clipboard.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Clipboard.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Clipboard.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Clipboard.o: /usr/X11R6/include/X11/Xosdefs.h
Clipboard.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Clipboard.o: /usr/X11R6/include/X11/Xutil.h
Clipboard.o: /usr/X11R6/include/X11/Xresource.h /usr/include/string.h
Clipboard.o: /usr/include/features.h /usr/include/sys/cdefs.h
Clipboard.o: /usr/X11R6/include/X11/Core.h /usr/X11R6/include/X11/Composite.h
Clipboard.o: /usr/X11R6/include/X11/Constraint.h
Clipboard.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Clipboard.o: /home/cannam/rosegarden/include/MidiConsts.h
Clipboard.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Clipboard.o: /usr/include/libio.h /usr/include/_G_config.h
Clipboard.o: /usr/include/stdlib.h /usr/include/errno.h
Clipboard.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Clipboard.o: /usr/include/alloca.h /usr/include/netinet/in.h
Clipboard.o: /usr/include/sys/socket.h /usr/include/linux/socket.h
Clipboard.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
Clipboard.o: /usr/include/linux/sockios.h /usr/include/linux/uio.h
Clipboard.o: /usr/include/linux/in.h /usr/include/asm/byteorder.h
Clipboard.o: /home/cannam/rosegarden/include/MidiErrorHandler.h Parse.h
Clipboard.o: Globals.h /home/cannam/rosegarden/include/SysDeps.h
Clipboard.o: ../../config.h /usr/include/signal.h /usr/include/linux/signal.h
Clipboard.o: /usr/include/asm/signal.h /usr/include/ctype.h
Clipboard.o: /usr/include/endian.h /usr/include/bytesex.h Types.h
Clipboard.o: /home/cannam/rosegarden/include/MidiXInclude.h
Clipboard.o: /usr/X11R6/include/X11/StringDefs.h
Clipboard.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Clipboard.o: /usr/X11R6/include/X11/SM/SM.h
Clipboard.o: /usr/X11R6/include/X11/ICE/ICElib.h
Clipboard.o: /usr/X11R6/include/X11/ICE/ICE.h
Clipboard.o: /usr/X11R6/include/X11/cursorfont.h
Clipboard.o: /home/cannam/rosegarden/include/Yawn.h
Clipboard.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Form.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Paned.h
Clipboard.o: /usr/X11R6/include/X11/Xmu/Converters.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Reports.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Box.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Command.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Label.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Simple.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/List.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Text.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Clipboard.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Clipboard.o: /home/cannam/rosegarden/include/MidiTrack.h
Clipboard.o: /home/cannam/rosegarden/include/ServiceNames.h
Clipboard.o: /home/cannam/rosegarden/include/Systems.h Consts.h
Clipboard.o: EventListWindow.h MainWindow.h
Clipboard.o: /home/cannam/rosegarden/include/ILClient.h
Clipboard.o: /home/cannam/rosegarden/include/ILTypes.h Clipboard.h Menu.h
Clipboard.o: EventListMenu.h PianoRollMenu.h PianoRoll.h
Clipboard.o: /home/cannam/rosegarden/include/Debug.h
Csound.o: /usr/include/ctype.h /usr/include/features.h
Csound.o: /usr/include/sys/cdefs.h /usr/include/endian.h
Csound.o: /usr/include/bytesex.h
Csound.o: /home/cannam/rosegarden/include/MidiXInclude.h
Csound.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Csound.o: /usr/include/signal.h /usr/include/sys/types.h
Csound.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Csound.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Csound.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Csound.o: /usr/include/asm/signal.h /usr/include/string.h
Csound.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Csound.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Csound.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Csound.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Csound.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Csound.o: /usr/X11R6/include/X11/Composite.h
Csound.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Csound.o: /usr/X11R6/include/X11/RectObj.h
Csound.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Csound.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Csound.o: /usr/X11R6/include/X11/ICE/ICElib.h
Csound.o: /usr/X11R6/include/X11/ICE/ICE.h
Csound.o: /usr/X11R6/include/X11/cursorfont.h
Csound.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
Csound.o: /usr/include/libio.h /usr/include/_G_config.h
Csound.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Csound.o: /usr/X11R6/include/X11/Xaw/Form.h
Csound.o: /usr/X11R6/include/X11/Xaw/Paned.h
Csound.o: /usr/X11R6/include/X11/Xmu/Converters.h
Csound.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Csound.o: /usr/X11R6/include/X11/Xaw/Reports.h
Csound.o: /usr/X11R6/include/X11/Xaw/Box.h
Csound.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Csound.o: /usr/X11R6/include/X11/Xaw/Command.h
Csound.o: /usr/X11R6/include/X11/Xaw/Label.h
Csound.o: /usr/X11R6/include/X11/Xaw/Simple.h
Csound.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Csound.o: /usr/X11R6/include/X11/Xaw/List.h
Csound.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Csound.o: /usr/X11R6/include/X11/Xaw/Text.h
Csound.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Csound.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Csound.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Csound.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Csound.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Csound.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Csound.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Csound.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Csound.o: /home/cannam/rosegarden/include/MidiFile.h
Csound.o: /home/cannam/rosegarden/include/MidiEvent.h
Csound.o: /home/cannam/rosegarden/include/MidiConsts.h
Csound.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
Csound.o: /usr/include/errno.h /usr/include/linux/errno.h
Csound.o: /usr/include/asm/errno.h /usr/include/alloca.h
Csound.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Csound.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Csound.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Csound.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Csound.o: /usr/include/asm/byteorder.h Globals.h Types.h
Csound.o: /home/cannam/rosegarden/include/MidiTrack.h
Csound.o: /home/cannam/rosegarden/include/ServiceNames.h
Csound.o: /home/cannam/rosegarden/include/Systems.h Menu.h Main.h Tracking.h
Csound.o: Csound.h /home/cannam/rosegarden/include/Debug.h
Dispatch.o: /home/cannam/rosegarden/include/MidiXInclude.h
Dispatch.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Dispatch.o: /usr/include/signal.h /usr/include/features.h
Dispatch.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Dispatch.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Dispatch.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Dispatch.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Dispatch.o: /usr/include/asm/signal.h /usr/include/ctype.h
Dispatch.o: /usr/include/endian.h /usr/include/bytesex.h
Dispatch.o: /usr/include/string.h
Dispatch.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Dispatch.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Dispatch.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Dispatch.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Dispatch.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Dispatch.o: /usr/X11R6/include/X11/Composite.h
Dispatch.o: /usr/X11R6/include/X11/Constraint.h
Dispatch.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Dispatch.o: /usr/X11R6/include/X11/StringDefs.h
Dispatch.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Dispatch.o: /usr/X11R6/include/X11/SM/SM.h
Dispatch.o: /usr/X11R6/include/X11/ICE/ICElib.h
Dispatch.o: /usr/X11R6/include/X11/ICE/ICE.h
Dispatch.o: /usr/X11R6/include/X11/cursorfont.h
Dispatch.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
Dispatch.o: /usr/include/libio.h /usr/include/_G_config.h
Dispatch.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Form.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Paned.h
Dispatch.o: /usr/X11R6/include/X11/Xmu/Converters.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Reports.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Box.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Command.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Label.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Simple.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/List.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Text.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Dispatch.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Dispatch.o: /home/cannam/rosegarden/include/MidiFile.h
Dispatch.o: /home/cannam/rosegarden/include/MidiEvent.h
Dispatch.o: /home/cannam/rosegarden/include/MidiConsts.h
Dispatch.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
Dispatch.o: /usr/include/errno.h /usr/include/linux/errno.h
Dispatch.o: /usr/include/asm/errno.h /usr/include/alloca.h
Dispatch.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Dispatch.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Dispatch.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Dispatch.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Dispatch.o: /usr/include/asm/byteorder.h TrackList.h Menu.h EventListWindow.h
Dispatch.o: /home/cannam/rosegarden/include/MidiTrack.h EventListMenu.h
Dispatch.o: PianoRoll.h PianoRollMenu.h Globals.h Types.h
Dispatch.o: /home/cannam/rosegarden/include/ServiceNames.h
Dispatch.o: /home/cannam/rosegarden/include/Systems.h
Dispatch.o: /home/cannam/rosegarden/include/Debug.h
EventDlgs.o: /usr/include/ctype.h /usr/include/features.h
EventDlgs.o: /usr/include/sys/cdefs.h /usr/include/endian.h
EventDlgs.o: /usr/include/bytesex.h
EventDlgs.o: /home/cannam/rosegarden/include/MidiXInclude.h
EventDlgs.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
EventDlgs.o: /usr/include/signal.h /usr/include/sys/types.h
EventDlgs.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
EventDlgs.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
EventDlgs.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
EventDlgs.o: /usr/include/asm/signal.h /usr/include/string.h
EventDlgs.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
EventDlgs.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
EventDlgs.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
EventDlgs.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
EventDlgs.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
EventDlgs.o: /usr/X11R6/include/X11/Composite.h
EventDlgs.o: /usr/X11R6/include/X11/Constraint.h
EventDlgs.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
EventDlgs.o: /usr/X11R6/include/X11/StringDefs.h
EventDlgs.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
EventDlgs.o: /usr/X11R6/include/X11/SM/SM.h
EventDlgs.o: /usr/X11R6/include/X11/ICE/ICElib.h
EventDlgs.o: /usr/X11R6/include/X11/ICE/ICE.h
EventDlgs.o: /usr/X11R6/include/X11/cursorfont.h
EventDlgs.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
EventDlgs.o: /usr/include/libio.h /usr/include/_G_config.h
EventDlgs.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Form.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Paned.h
EventDlgs.o: /usr/X11R6/include/X11/Xmu/Converters.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Viewport.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Reports.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Box.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Command.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Label.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Simple.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Toggle.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/List.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Text.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/TextSink.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Repeater.h
EventDlgs.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
EventDlgs.o: /home/cannam/rosegarden/include/MidiFile.h
EventDlgs.o: /home/cannam/rosegarden/include/MidiEvent.h
EventDlgs.o: /home/cannam/rosegarden/include/MidiConsts.h
EventDlgs.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
EventDlgs.o: /usr/include/errno.h /usr/include/linux/errno.h
EventDlgs.o: /usr/include/asm/errno.h /usr/include/alloca.h
EventDlgs.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
EventDlgs.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
EventDlgs.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
EventDlgs.o: /usr/include/linux/uio.h /usr/include/linux/in.h
EventDlgs.o: /usr/include/asm/byteorder.h
EventDlgs.o: /home/cannam/rosegarden/include/MidiErrorHandler.h Globals.h
EventDlgs.o: Types.h /home/cannam/rosegarden/include/MidiTrack.h
EventDlgs.o: /home/cannam/rosegarden/include/ServiceNames.h
EventDlgs.o: /home/cannam/rosegarden/include/Systems.h EventDlgs.h
EventDlgs.o: /home/cannam/rosegarden/include/Debug.h
EventListMenu.o: /home/cannam/rosegarden/include/MidiXInclude.h
EventListMenu.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
EventListMenu.o: /usr/include/signal.h /usr/include/features.h
EventListMenu.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
EventListMenu.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
EventListMenu.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
EventListMenu.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
EventListMenu.o: /usr/include/asm/signal.h /usr/include/ctype.h
EventListMenu.o: /usr/include/endian.h /usr/include/bytesex.h
EventListMenu.o: /usr/include/string.h
EventListMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
EventListMenu.o: /usr/X11R6/include/X11/Intrinsic.h
EventListMenu.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
EventListMenu.o: /usr/X11R6/include/X11/Xfuncproto.h
EventListMenu.o: /usr/X11R6/include/X11/Xosdefs.h
EventListMenu.o: /usr/X11R6/include/X11/Xutil.h
EventListMenu.o: /usr/X11R6/include/X11/Xresource.h
EventListMenu.o: /usr/X11R6/include/X11/Core.h
EventListMenu.o: /usr/X11R6/include/X11/Composite.h
EventListMenu.o: /usr/X11R6/include/X11/Constraint.h
EventListMenu.o: /usr/X11R6/include/X11/Object.h
EventListMenu.o: /usr/X11R6/include/X11/RectObj.h
EventListMenu.o: /usr/X11R6/include/X11/StringDefs.h
EventListMenu.o: /usr/X11R6/include/X11/Shell.h
EventListMenu.o: /usr/X11R6/include/X11/SM/SMlib.h
EventListMenu.o: /usr/X11R6/include/X11/SM/SM.h
EventListMenu.o: /usr/X11R6/include/X11/ICE/ICElib.h
EventListMenu.o: /usr/X11R6/include/X11/ICE/ICE.h
EventListMenu.o: /usr/X11R6/include/X11/cursorfont.h
EventListMenu.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
EventListMenu.o: /usr/include/libio.h /usr/include/_G_config.h
EventListMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Form.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Paned.h
EventListMenu.o: /usr/X11R6/include/X11/Xmu/Converters.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Viewport.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Reports.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Box.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Command.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Label.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Simple.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Toggle.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/List.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Text.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/TextSink.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Repeater.h
EventListMenu.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
EventListMenu.o: /home/cannam/rosegarden/include/MidiFile.h
EventListMenu.o: /home/cannam/rosegarden/include/MidiEvent.h
EventListMenu.o: /home/cannam/rosegarden/include/MidiConsts.h
EventListMenu.o: /home/cannam/rosegarden/include/Lists.h
EventListMenu.o: /usr/include/stdlib.h /usr/include/errno.h
EventListMenu.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
EventListMenu.o: /usr/include/alloca.h /usr/include/netinet/in.h
EventListMenu.o: /usr/include/sys/socket.h /usr/include/linux/socket.h
EventListMenu.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
EventListMenu.o: /usr/include/linux/sockios.h /usr/include/linux/uio.h
EventListMenu.o: /usr/include/linux/in.h /usr/include/asm/byteorder.h
EventListMenu.o: /home/cannam/rosegarden/include/MidiErrorHandler.h Main.h
EventListMenu.o: Tracking.h Globals.h Types.h
EventListMenu.o: /home/cannam/rosegarden/include/MidiTrack.h
EventListMenu.o: /home/cannam/rosegarden/include/ServiceNames.h
EventListMenu.o: /home/cannam/rosegarden/include/Systems.h EventListWindow.h
EventListMenu.o: Menu.h TrackList.h Clipboard.h EventDlgs.h PianoRoll.h
EventListMenu.o: Dispatch.h Undo.h /home/cannam/rosegarden/include/Debug.h
EventListMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/piano_roll.xbm
EventListMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/quantize.xbm
EventListMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/cut.xbm
EventListMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/copy.xbm
EventListMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/paste.xbm
EventListMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
EventListMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/undo.xbm
EventListWindow.o: /home/cannam/rosegarden/include/Mapper.h
EventListWindow.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
EventListWindow.o: /usr/include/signal.h /usr/include/features.h
EventListWindow.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
EventListWindow.o: /usr/include/linux/types.h
EventListWindow.o: /usr/include/linux/posix_types.h
EventListWindow.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
EventListWindow.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
EventListWindow.o: /usr/include/asm/signal.h /usr/include/ctype.h
EventListWindow.o: /usr/include/endian.h /usr/include/bytesex.h
EventListWindow.o: /usr/include/string.h
EventListWindow.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
EventListWindow.o: /home/cannam/rosegarden/include/Systems.h
EventListWindow.o: /home/cannam/rosegarden/include/Lists.h
EventListWindow.o: /usr/include/stdio.h /usr/include/libio.h
EventListWindow.o: /usr/include/_G_config.h /usr/include/stdlib.h
EventListWindow.o: /usr/include/errno.h /usr/include/linux/errno.h
EventListWindow.o: /usr/include/asm/errno.h /usr/include/alloca.h
EventListWindow.o: /home/cannam/rosegarden/include/MidiXInclude.h
EventListWindow.o: /usr/X11R6/include/X11/Intrinsic.h
EventListWindow.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
EventListWindow.o: /usr/X11R6/include/X11/Xfuncproto.h
EventListWindow.o: /usr/X11R6/include/X11/Xosdefs.h
EventListWindow.o: /usr/X11R6/include/X11/Xutil.h
EventListWindow.o: /usr/X11R6/include/X11/Xresource.h
EventListWindow.o: /usr/X11R6/include/X11/Core.h
EventListWindow.o: /usr/X11R6/include/X11/Composite.h
EventListWindow.o: /usr/X11R6/include/X11/Constraint.h
EventListWindow.o: /usr/X11R6/include/X11/Object.h
EventListWindow.o: /usr/X11R6/include/X11/RectObj.h
EventListWindow.o: /usr/X11R6/include/X11/StringDefs.h
EventListWindow.o: /usr/X11R6/include/X11/Shell.h
EventListWindow.o: /usr/X11R6/include/X11/SM/SMlib.h
EventListWindow.o: /usr/X11R6/include/X11/SM/SM.h
EventListWindow.o: /usr/X11R6/include/X11/ICE/ICElib.h
EventListWindow.o: /usr/X11R6/include/X11/ICE/ICE.h
EventListWindow.o: /usr/X11R6/include/X11/cursorfont.h
EventListWindow.o: /home/cannam/rosegarden/include/Yawn.h
EventListWindow.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Form.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Paned.h
EventListWindow.o: /usr/X11R6/include/X11/Xmu/Converters.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Viewport.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Reports.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Box.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Command.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Label.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Simple.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Toggle.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/List.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Text.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/TextSink.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Repeater.h
EventListWindow.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
EventListWindow.o: /home/cannam/rosegarden/include/MidiEvent.h
EventListWindow.o: /home/cannam/rosegarden/include/MidiConsts.h
EventListWindow.o: /home/cannam/rosegarden/include/Mapper_OSS.h
EventListWindow.o: /usr/include/sys/soundcard.h
EventListWindow.o: /usr/include/linux/soundcard.h /usr/include/linux/ioctl.h
EventListWindow.o: /usr/include/asm/ioctl.h
EventListWindow.o: /home/cannam/rosegarden/include/MidiFile.h
EventListWindow.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
EventListWindow.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
EventListWindow.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
EventListWindow.o: /usr/include/linux/uio.h /usr/include/linux/in.h
EventListWindow.o: /usr/include/asm/byteorder.h
EventListWindow.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
EventListWindow.o: Globals.h Types.h
EventListWindow.o: /home/cannam/rosegarden/include/MidiTrack.h
EventListWindow.o: /home/cannam/rosegarden/include/ServiceNames.h Consts.h
EventListWindow.o: Main.h Tracking.h EventListWindow.h EventListMenu.h
EventListWindow.o: Parse.h Clipboard.h Undo.h TrackList.h
EventListWindow.o: /home/cannam/rosegarden/include/Debug.h
Filter.o: Globals.h /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Filter.o: /usr/include/signal.h /usr/include/features.h
Filter.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Filter.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Filter.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Filter.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Filter.o: /usr/include/asm/signal.h /usr/include/ctype.h
Filter.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Filter.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h Types.h
Filter.o: /home/cannam/rosegarden/include/MidiXInclude.h
Filter.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Filter.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Filter.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Filter.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Filter.o: /usr/X11R6/include/X11/Composite.h
Filter.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Filter.o: /usr/X11R6/include/X11/RectObj.h
Filter.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Filter.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Filter.o: /usr/X11R6/include/X11/ICE/ICElib.h
Filter.o: /usr/X11R6/include/X11/ICE/ICE.h
Filter.o: /usr/X11R6/include/X11/cursorfont.h
Filter.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
Filter.o: /usr/include/libio.h /usr/include/_G_config.h
Filter.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Filter.o: /usr/X11R6/include/X11/Xaw/Form.h
Filter.o: /usr/X11R6/include/X11/Xaw/Paned.h
Filter.o: /usr/X11R6/include/X11/Xmu/Converters.h
Filter.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Filter.o: /usr/X11R6/include/X11/Xaw/Reports.h
Filter.o: /usr/X11R6/include/X11/Xaw/Box.h
Filter.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Filter.o: /usr/X11R6/include/X11/Xaw/Command.h
Filter.o: /usr/X11R6/include/X11/Xaw/Label.h
Filter.o: /usr/X11R6/include/X11/Xaw/Simple.h
Filter.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Filter.o: /usr/X11R6/include/X11/Xaw/List.h
Filter.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Filter.o: /usr/X11R6/include/X11/Xaw/Text.h
Filter.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Filter.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Filter.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Filter.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Filter.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Filter.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Filter.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Filter.o: /home/cannam/rosegarden/include/MidiFile.h
Filter.o: /home/cannam/rosegarden/include/MidiEvent.h
Filter.o: /home/cannam/rosegarden/include/MidiConsts.h
Filter.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
Filter.o: /usr/include/errno.h /usr/include/linux/errno.h
Filter.o: /usr/include/asm/errno.h /usr/include/alloca.h
Filter.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Filter.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Filter.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Filter.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Filter.o: /usr/include/asm/byteorder.h
Filter.o: /home/cannam/rosegarden/include/MidiTrack.h
Filter.o: /home/cannam/rosegarden/include/ServiceNames.h
Filter.o: /home/cannam/rosegarden/include/Systems.h Menu.h
Filter.o: /home/cannam/rosegarden/include/Debug.h
Filter.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Filter.o: /usr/include/unistd.h /usr/include/posix_opt.h
Filter.o: /usr/include/gnu/types.h /usr/include/confname.h
Filter.o: /usr/include/sys/wait.h /usr/include/waitflags.h
Filter.o: /usr/include/waitstatus.h /usr/include/dirent.h
Filter.o: /usr/include/linux/limits.h /usr/include/linux/dirent.h
Filter.o: /usr/include/posix1_lim.h /usr/include/sys/stat.h
Filter.o: /usr/include/linux/stat.h
Main.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Main.o: /usr/include/signal.h /usr/include/features.h
Main.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Main.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Main.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Main.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Main.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Main.o: /usr/include/bytesex.h /usr/include/string.h
Main.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Main.o: /home/cannam/rosegarden/include/ILClient.h
Main.o: /home/cannam/rosegarden/include/ILTypes.h
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
Main.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Main.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/stdlib.h
Main.o: /usr/include/errno.h /usr/include/linux/errno.h
Main.o: /usr/include/asm/errno.h /usr/include/alloca.h
Main.o: /home/cannam/rosegarden/include/Mapper.h
Main.o: /home/cannam/rosegarden/include/Systems.h
Main.o: /home/cannam/rosegarden/include/MidiXInclude.h
Main.o: /home/cannam/rosegarden/include/Yawn.h
Main.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Main.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/Paned.h
Main.o: /usr/X11R6/include/X11/Xmu/Converters.h
Main.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Main.o: /usr/X11R6/include/X11/Xaw/Reports.h /usr/X11R6/include/X11/Xaw/Box.h
Main.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Main.o: /usr/X11R6/include/X11/Xaw/Command.h
Main.o: /usr/X11R6/include/X11/Xaw/Label.h
Main.o: /usr/X11R6/include/X11/Xaw/Simple.h
Main.o: /usr/X11R6/include/X11/Xaw/Toggle.h /usr/X11R6/include/X11/Xaw/List.h
Main.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Main.o: /usr/X11R6/include/X11/Xaw/Text.h
Main.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Main.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Main.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Main.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Main.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Main.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Main.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Main.o: /home/cannam/rosegarden/include/MidiEvent.h
Main.o: /home/cannam/rosegarden/include/MidiConsts.h
Main.o: /home/cannam/rosegarden/include/Mapper_OSS.h
Main.o: /usr/include/sys/soundcard.h /usr/include/linux/soundcard.h
Main.o: /usr/include/linux/ioctl.h /usr/include/asm/ioctl.h
Main.o: /usr/include/unistd.h /usr/include/posix_opt.h
Main.o: /usr/include/gnu/types.h /usr/include/confname.h
Main.o: /home/cannam/rosegarden/include/MidiFile.h /usr/include/netinet/in.h
Main.o: /usr/include/sys/socket.h /usr/include/linux/socket.h
Main.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
Main.o: /usr/include/linux/sockios.h /usr/include/linux/uio.h
Main.o: /usr/include/linux/in.h /usr/include/asm/byteorder.h
Main.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Main.o: /home/cannam/rosegarden/include/MidiVarLenNums.h Sequence.h Types.h
Main.o: Consts.h Resources.h Fallbacks.h Globals.h
Main.o: /home/cannam/rosegarden/include/MidiTrack.h
Main.o: /home/cannam/rosegarden/include/ServiceNames.h Menu.h Main.h
Main.o: Tracking.h MainWindow.h TrackList.h PianoRoll.h EventListWindow.h
Main.o: /home/cannam/rosegarden/include/Debug.h
Main.o: /home/cannam/rosegarden/common/bitmaps/hourglass.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hourglass_mask.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour0.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour1.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour2.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour3.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour4.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour5.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour6.xbm
Main.o: /home/cannam/rosegarden/common/bitmaps/hour7.xbm
MainWindow.o: Types.h /home/cannam/rosegarden/include/MidiXInclude.h
MainWindow.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MainWindow.o: /usr/include/signal.h /usr/include/features.h
MainWindow.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
MainWindow.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MainWindow.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MainWindow.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MainWindow.o: /usr/include/asm/signal.h /usr/include/ctype.h
MainWindow.o: /usr/include/endian.h /usr/include/bytesex.h
MainWindow.o: /usr/include/string.h
MainWindow.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MainWindow.o: /usr/X11R6/include/X11/Intrinsic.h
MainWindow.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
MainWindow.o: /usr/X11R6/include/X11/Xfuncproto.h
MainWindow.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
MainWindow.o: /usr/X11R6/include/X11/Xresource.h
MainWindow.o: /usr/X11R6/include/X11/Core.h
MainWindow.o: /usr/X11R6/include/X11/Composite.h
MainWindow.o: /usr/X11R6/include/X11/Constraint.h
MainWindow.o: /usr/X11R6/include/X11/Object.h
MainWindow.o: /usr/X11R6/include/X11/RectObj.h
MainWindow.o: /usr/X11R6/include/X11/StringDefs.h
MainWindow.o: /usr/X11R6/include/X11/Shell.h
MainWindow.o: /usr/X11R6/include/X11/SM/SMlib.h
MainWindow.o: /usr/X11R6/include/X11/SM/SM.h
MainWindow.o: /usr/X11R6/include/X11/ICE/ICElib.h
MainWindow.o: /usr/X11R6/include/X11/ICE/ICE.h
MainWindow.o: /usr/X11R6/include/X11/cursorfont.h
MainWindow.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
MainWindow.o: /usr/include/libio.h /usr/include/_G_config.h
MainWindow.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Form.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Paned.h
MainWindow.o: /usr/X11R6/include/X11/Xmu/Converters.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Viewport.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Reports.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Box.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Command.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Label.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Simple.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Toggle.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/List.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Text.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/TextSink.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Repeater.h
MainWindow.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
MainWindow.o: /home/cannam/rosegarden/include/MidiFile.h
MainWindow.o: /home/cannam/rosegarden/include/MidiEvent.h
MainWindow.o: /home/cannam/rosegarden/include/MidiConsts.h
MainWindow.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
MainWindow.o: /usr/include/errno.h /usr/include/linux/errno.h
MainWindow.o: /usr/include/asm/errno.h /usr/include/alloca.h
MainWindow.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
MainWindow.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
MainWindow.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
MainWindow.o: /usr/include/linux/uio.h /usr/include/linux/in.h
MainWindow.o: /usr/include/asm/byteorder.h Globals.h
MainWindow.o: /home/cannam/rosegarden/include/MidiTrack.h
MainWindow.o: /home/cannam/rosegarden/include/ServiceNames.h
MainWindow.o: /home/cannam/rosegarden/include/Systems.h MainWindow.h
MainWindow.o: /home/cannam/rosegarden/include/ILClient.h
MainWindow.o: /home/cannam/rosegarden/include/ILTypes.h TrackList.h Message.h
MainWindow.o: Menu.h Main.h Tracking.h
MainWindow.o: /home/cannam/rosegarden/include/Version.h
MainWindow.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
MainWindow.o: /home/cannam/rosegarden/include/Debug.h
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/rose.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/rose_mask.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/up.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/down.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/rewind.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/back.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/forward.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/ffwd.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/lightgrey.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/darkgrey.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/zoomin.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/zoomout.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/clef_treble.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/clef_bass.xbm
MainWindow.o: /home/cannam/rosegarden/common/bitmaps/notemod_sharp.xbm
Menu.o: /home/cannam/rosegarden/include/ILClient.h
Menu.o: /home/cannam/rosegarden/include/ILTypes.h
Menu.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Menu.o: /usr/include/sys/types.h /usr/include/linux/types.h
Menu.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Menu.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Menu.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Menu.o: /usr/X11R6/include/X11/Xosdefs.h
Menu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Menu.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
Menu.o: /usr/include/string.h /usr/include/features.h
Menu.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
Menu.o: /usr/X11R6/include/X11/Composite.h
Menu.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Menu.o: /usr/X11R6/include/X11/RectObj.h /usr/X11R6/include/X11/StringDefs.h
Menu.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Menu.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
Menu.o: /usr/X11R6/include/X11/ICE/ICE.h /usr/X11R6/include/X11/cursorfont.h
Menu.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Menu.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/stdlib.h
Menu.o: /usr/include/errno.h /usr/include/linux/errno.h
Menu.o: /usr/include/asm/errno.h /usr/include/alloca.h Globals.h
Menu.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Menu.o: /usr/include/signal.h /usr/include/linux/signal.h
Menu.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Menu.o: /usr/include/bytesex.h Types.h
Menu.o: /home/cannam/rosegarden/include/MidiXInclude.h
Menu.o: /home/cannam/rosegarden/include/Yawn.h
Menu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Menu.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/Paned.h
Menu.o: /usr/X11R6/include/X11/Xmu/Converters.h
Menu.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Menu.o: /usr/X11R6/include/X11/Xaw/Reports.h /usr/X11R6/include/X11/Xaw/Box.h
Menu.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Menu.o: /usr/X11R6/include/X11/Xaw/Command.h
Menu.o: /usr/X11R6/include/X11/Xaw/Label.h
Menu.o: /usr/X11R6/include/X11/Xaw/Simple.h
Menu.o: /usr/X11R6/include/X11/Xaw/Toggle.h /usr/X11R6/include/X11/Xaw/List.h
Menu.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Menu.o: /usr/X11R6/include/X11/Xaw/Text.h
Menu.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Menu.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Menu.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Menu.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Menu.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Menu.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Menu.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Menu.o: /home/cannam/rosegarden/include/MidiFile.h
Menu.o: /home/cannam/rosegarden/include/MidiEvent.h
Menu.o: /home/cannam/rosegarden/include/MidiConsts.h
Menu.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Menu.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Menu.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Menu.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Menu.o: /usr/include/asm/byteorder.h
Menu.o: /home/cannam/rosegarden/include/MidiTrack.h
Menu.o: /home/cannam/rosegarden/include/ServiceNames.h
Menu.o: /home/cannam/rosegarden/include/Systems.h Menu.h Main.h Tracking.h
Menu.o: MainWindow.h TrackList.h EventListWindow.h PianoRollMenu.h
Menu.o: PianoRoll.h Message.h Undo.h Clipboard.h Consts.h Sequence.h Record.h
Menu.o: Dispatch.h EventListMenu.h Csound.h MidiSetupDlgs.h
Menu.o: /usr/include/unistd.h /usr/include/posix_opt.h
Menu.o: /usr/include/gnu/types.h /usr/include/confname.h
Menu.o: /home/cannam/rosegarden/include/Debug.h
Menu.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/beam.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/undo.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/copy.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/cut.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/paste.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/open.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/save.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/quantize.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/event_list.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/piano_roll.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/sequence.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/stop.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/record.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/ffwd.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/rwd.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/rrwd.xbm
Menu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/skp.xbm
Message.o: /home/cannam/rosegarden/include/MidiXInclude.h
Message.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Message.o: /usr/include/signal.h /usr/include/features.h
Message.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Message.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Message.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Message.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Message.o: /usr/include/asm/signal.h /usr/include/ctype.h
Message.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Message.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Message.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Message.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Message.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Message.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Message.o: /usr/X11R6/include/X11/Composite.h
Message.o: /usr/X11R6/include/X11/Constraint.h
Message.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Message.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Message.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Message.o: /usr/X11R6/include/X11/ICE/ICElib.h
Message.o: /usr/X11R6/include/X11/ICE/ICE.h
Message.o: /usr/X11R6/include/X11/cursorfont.h
Message.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
Message.o: /usr/include/libio.h /usr/include/_G_config.h
Message.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Message.o: /usr/X11R6/include/X11/Xaw/Form.h
Message.o: /usr/X11R6/include/X11/Xaw/Paned.h
Message.o: /usr/X11R6/include/X11/Xmu/Converters.h
Message.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Message.o: /usr/X11R6/include/X11/Xaw/Reports.h
Message.o: /usr/X11R6/include/X11/Xaw/Box.h
Message.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Message.o: /usr/X11R6/include/X11/Xaw/Command.h
Message.o: /usr/X11R6/include/X11/Xaw/Label.h
Message.o: /usr/X11R6/include/X11/Xaw/Simple.h
Message.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Message.o: /usr/X11R6/include/X11/Xaw/List.h
Message.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Message.o: /usr/X11R6/include/X11/Xaw/Text.h
Message.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Message.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Message.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Message.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Message.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Message.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Message.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Message.o: /home/cannam/rosegarden/include/MidiFile.h
Message.o: /home/cannam/rosegarden/include/MidiEvent.h
Message.o: /home/cannam/rosegarden/include/MidiConsts.h
Message.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
Message.o: /usr/include/errno.h /usr/include/linux/errno.h
Message.o: /usr/include/asm/errno.h /usr/include/alloca.h
Message.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Message.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Message.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Message.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Message.o: /usr/include/asm/byteorder.h Globals.h Types.h
Message.o: /home/cannam/rosegarden/include/MidiTrack.h
Message.o: /home/cannam/rosegarden/include/ServiceNames.h
Message.o: /home/cannam/rosegarden/include/Systems.h Message.h
Message.o: /home/cannam/rosegarden/include/Debug.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/Mapper.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
MidiSetupDlgs.o: /usr/include/signal.h /usr/include/features.h
MidiSetupDlgs.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
MidiSetupDlgs.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
MidiSetupDlgs.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
MidiSetupDlgs.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
MidiSetupDlgs.o: /usr/include/asm/signal.h /usr/include/ctype.h
MidiSetupDlgs.o: /usr/include/endian.h /usr/include/bytesex.h
MidiSetupDlgs.o: /usr/include/string.h
MidiSetupDlgs.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/Systems.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
MidiSetupDlgs.o: /usr/include/libio.h /usr/include/_G_config.h
MidiSetupDlgs.o: /usr/include/stdlib.h /usr/include/errno.h
MidiSetupDlgs.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
MidiSetupDlgs.o: /usr/include/alloca.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/MidiXInclude.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Intrinsic.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xfuncproto.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xosdefs.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xutil.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xresource.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Core.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Composite.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Constraint.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Object.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/RectObj.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/StringDefs.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Shell.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/SM/SMlib.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/SM/SM.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/ICE/ICElib.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/ICE/ICE.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/cursorfont.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/Yawn.h
MidiSetupDlgs.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Form.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Paned.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xmu/Converters.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Viewport.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Reports.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Box.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Command.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Label.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Simple.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Toggle.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/List.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Text.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/TextSink.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Repeater.h
MidiSetupDlgs.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/MidiEvent.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/MidiConsts.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/Mapper_OSS.h
MidiSetupDlgs.o: /usr/include/sys/soundcard.h /usr/include/linux/soundcard.h
MidiSetupDlgs.o: /usr/include/linux/ioctl.h /usr/include/asm/ioctl.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/MidiFile.h
MidiSetupDlgs.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
MidiSetupDlgs.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
MidiSetupDlgs.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
MidiSetupDlgs.o: /usr/include/linux/uio.h /usr/include/linux/in.h
MidiSetupDlgs.o: /usr/include/asm/byteorder.h Globals.h Types.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/MidiTrack.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/ServiceNames.h EventDlgs.h
MidiSetupDlgs.o: Sequence.h Main.h Tracking.h Menu.h MidiSetupDlgs.h
MidiSetupDlgs.o: EventListWindow.h TrackList.h EventListMenu.h
MidiSetupDlgs.o: /home/cannam/rosegarden/include/Debug.h
Parse.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
Parse.o: /home/cannam/rosegarden/include/MidiConsts.h
Parse.o: /home/cannam/rosegarden/include/MidiFile.h
Parse.o: /home/cannam/rosegarden/include/MidiEvent.h
Parse.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Parse.o: /usr/include/sys/types.h /usr/include/linux/types.h
Parse.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Parse.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Parse.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Parse.o: /usr/X11R6/include/X11/Xosdefs.h
Parse.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Parse.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
Parse.o: /usr/include/string.h /usr/include/features.h
Parse.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
Parse.o: /usr/X11R6/include/X11/Composite.h
Parse.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Parse.o: /usr/X11R6/include/X11/RectObj.h
Parse.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
Parse.o: /usr/include/errno.h /usr/include/linux/errno.h
Parse.o: /usr/include/asm/errno.h /usr/include/alloca.h
Parse.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Parse.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Parse.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Parse.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Parse.o: /usr/include/asm/byteorder.h
Parse.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Parse.o: /home/cannam/rosegarden/include/MidiVarLenNums.h Types.h
Parse.o: /home/cannam/rosegarden/include/MidiXInclude.h
Parse.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Parse.o: /usr/include/signal.h /usr/include/linux/signal.h
Parse.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Parse.o: /usr/include/bytesex.h /usr/X11R6/include/X11/StringDefs.h
Parse.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Parse.o: /usr/X11R6/include/X11/SM/SM.h /usr/X11R6/include/X11/ICE/ICElib.h
Parse.o: /usr/X11R6/include/X11/ICE/ICE.h /usr/X11R6/include/X11/cursorfont.h
Parse.o: /home/cannam/rosegarden/include/Yawn.h
Parse.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Parse.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/Paned.h
Parse.o: /usr/X11R6/include/X11/Xmu/Converters.h
Parse.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Parse.o: /usr/X11R6/include/X11/Xaw/Reports.h
Parse.o: /usr/X11R6/include/X11/Xaw/Box.h
Parse.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Parse.o: /usr/X11R6/include/X11/Xaw/Command.h
Parse.o: /usr/X11R6/include/X11/Xaw/Label.h
Parse.o: /usr/X11R6/include/X11/Xaw/Simple.h
Parse.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Parse.o: /usr/X11R6/include/X11/Xaw/List.h
Parse.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Parse.o: /usr/X11R6/include/X11/Xaw/Text.h
Parse.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Parse.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Parse.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Parse.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Parse.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Parse.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Parse.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h Parse.h Globals.h
Parse.o: /home/cannam/rosegarden/include/MidiTrack.h
Parse.o: /home/cannam/rosegarden/include/ServiceNames.h
Parse.o: /home/cannam/rosegarden/include/Systems.h
Parse.o: /home/cannam/rosegarden/include/Debug.h
PianoRoll.o: /home/cannam/rosegarden/include/MidiXInclude.h
PianoRoll.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
PianoRoll.o: /usr/include/signal.h /usr/include/features.h
PianoRoll.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
PianoRoll.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
PianoRoll.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
PianoRoll.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
PianoRoll.o: /usr/include/asm/signal.h /usr/include/ctype.h
PianoRoll.o: /usr/include/endian.h /usr/include/bytesex.h
PianoRoll.o: /usr/include/string.h
PianoRoll.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
PianoRoll.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
PianoRoll.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
PianoRoll.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
PianoRoll.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
PianoRoll.o: /usr/X11R6/include/X11/Composite.h
PianoRoll.o: /usr/X11R6/include/X11/Constraint.h
PianoRoll.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
PianoRoll.o: /usr/X11R6/include/X11/StringDefs.h
PianoRoll.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
PianoRoll.o: /usr/X11R6/include/X11/SM/SM.h
PianoRoll.o: /usr/X11R6/include/X11/ICE/ICElib.h
PianoRoll.o: /usr/X11R6/include/X11/ICE/ICE.h
PianoRoll.o: /usr/X11R6/include/X11/cursorfont.h
PianoRoll.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
PianoRoll.o: /usr/include/libio.h /usr/include/_G_config.h
PianoRoll.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Form.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Paned.h
PianoRoll.o: /usr/X11R6/include/X11/Xmu/Converters.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Viewport.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Reports.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Box.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Command.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Label.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Simple.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Toggle.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/List.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Text.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/TextSink.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Repeater.h
PianoRoll.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
PianoRoll.o: /home/cannam/rosegarden/include/MidiFile.h
PianoRoll.o: /home/cannam/rosegarden/include/MidiEvent.h
PianoRoll.o: /home/cannam/rosegarden/include/MidiConsts.h
PianoRoll.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
PianoRoll.o: /usr/include/errno.h /usr/include/linux/errno.h
PianoRoll.o: /usr/include/asm/errno.h /usr/include/alloca.h
PianoRoll.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
PianoRoll.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
PianoRoll.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
PianoRoll.o: /usr/include/linux/uio.h /usr/include/linux/in.h
PianoRoll.o: /usr/include/asm/byteorder.h
PianoRoll.o: /home/cannam/rosegarden/include/MidiErrorHandler.h Globals.h
PianoRoll.o: Types.h /home/cannam/rosegarden/include/MidiTrack.h
PianoRoll.o: /home/cannam/rosegarden/include/ServiceNames.h
PianoRoll.o: /home/cannam/rosegarden/include/Systems.h Consts.h Main.h
PianoRoll.o: Tracking.h PianoRoll.h PianoRollMenu.h EventListMenu.h
PianoRoll.o: EventListWindow.h Parse.h Menu.h Clipboard.h Undo.h
PianoRoll.o: /home/cannam/rosegarden/include/Debug.h
PianoRollMenu.o: /home/cannam/rosegarden/include/MidiXInclude.h
PianoRollMenu.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
PianoRollMenu.o: /usr/include/signal.h /usr/include/features.h
PianoRollMenu.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
PianoRollMenu.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
PianoRollMenu.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
PianoRollMenu.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
PianoRollMenu.o: /usr/include/asm/signal.h /usr/include/ctype.h
PianoRollMenu.o: /usr/include/endian.h /usr/include/bytesex.h
PianoRollMenu.o: /usr/include/string.h
PianoRollMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
PianoRollMenu.o: /usr/X11R6/include/X11/Intrinsic.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xlib.h /usr/X11R6/include/X11/X.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xfuncproto.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xosdefs.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xutil.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xresource.h
PianoRollMenu.o: /usr/X11R6/include/X11/Core.h
PianoRollMenu.o: /usr/X11R6/include/X11/Composite.h
PianoRollMenu.o: /usr/X11R6/include/X11/Constraint.h
PianoRollMenu.o: /usr/X11R6/include/X11/Object.h
PianoRollMenu.o: /usr/X11R6/include/X11/RectObj.h
PianoRollMenu.o: /usr/X11R6/include/X11/StringDefs.h
PianoRollMenu.o: /usr/X11R6/include/X11/Shell.h
PianoRollMenu.o: /usr/X11R6/include/X11/SM/SMlib.h
PianoRollMenu.o: /usr/X11R6/include/X11/SM/SM.h
PianoRollMenu.o: /usr/X11R6/include/X11/ICE/ICElib.h
PianoRollMenu.o: /usr/X11R6/include/X11/ICE/ICE.h
PianoRollMenu.o: /usr/X11R6/include/X11/cursorfont.h
PianoRollMenu.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
PianoRollMenu.o: /usr/include/libio.h /usr/include/_G_config.h
PianoRollMenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Form.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Paned.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xmu/Converters.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Viewport.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Reports.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Box.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Command.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Label.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Simple.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Toggle.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/List.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Text.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/TextSink.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Repeater.h
PianoRollMenu.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
PianoRollMenu.o: /home/cannam/rosegarden/include/MidiFile.h
PianoRollMenu.o: /home/cannam/rosegarden/include/MidiEvent.h
PianoRollMenu.o: /home/cannam/rosegarden/include/MidiConsts.h
PianoRollMenu.o: /home/cannam/rosegarden/include/Lists.h
PianoRollMenu.o: /usr/include/stdlib.h /usr/include/errno.h
PianoRollMenu.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
PianoRollMenu.o: /usr/include/alloca.h /usr/include/netinet/in.h
PianoRollMenu.o: /usr/include/sys/socket.h /usr/include/linux/socket.h
PianoRollMenu.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
PianoRollMenu.o: /usr/include/linux/sockios.h /usr/include/linux/uio.h
PianoRollMenu.o: /usr/include/linux/in.h /usr/include/asm/byteorder.h
PianoRollMenu.o: /home/cannam/rosegarden/include/MidiErrorHandler.h Globals.h
PianoRollMenu.o: Types.h /home/cannam/rosegarden/include/MidiTrack.h
PianoRollMenu.o: /home/cannam/rosegarden/include/ServiceNames.h
PianoRollMenu.o: /home/cannam/rosegarden/include/Systems.h PianoRoll.h Menu.h
PianoRollMenu.o: TrackList.h Clipboard.h EventDlgs.h Dispatch.h Undo.h
PianoRollMenu.o: EventListWindow.h /home/cannam/rosegarden/include/Debug.h
PianoRollMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/event_list.xbm
PianoRollMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/quantize.xbm
PianoRollMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/cut.xbm
PianoRollMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/copy.xbm
PianoRollMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/paste.xbm
PianoRollMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/ninja_cross.xbm
PianoRollMenu.o: /home/cannam/rosegarden/common/bitmaps/toolbar/undo.xbm
Record.o: Globals.h /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Record.o: /usr/include/signal.h /usr/include/features.h
Record.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Record.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Record.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Record.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Record.o: /usr/include/asm/signal.h /usr/include/ctype.h
Record.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/string.h
Record.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h Types.h
Record.o: /home/cannam/rosegarden/include/MidiXInclude.h
Record.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Record.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Record.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Record.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Record.o: /usr/X11R6/include/X11/Composite.h
Record.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Record.o: /usr/X11R6/include/X11/RectObj.h
Record.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Record.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Record.o: /usr/X11R6/include/X11/ICE/ICElib.h
Record.o: /usr/X11R6/include/X11/ICE/ICE.h
Record.o: /usr/X11R6/include/X11/cursorfont.h
Record.o: /home/cannam/rosegarden/include/Yawn.h /usr/include/stdio.h
Record.o: /usr/include/libio.h /usr/include/_G_config.h
Record.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Record.o: /usr/X11R6/include/X11/Xaw/Form.h
Record.o: /usr/X11R6/include/X11/Xaw/Paned.h
Record.o: /usr/X11R6/include/X11/Xmu/Converters.h
Record.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Record.o: /usr/X11R6/include/X11/Xaw/Reports.h
Record.o: /usr/X11R6/include/X11/Xaw/Box.h
Record.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Record.o: /usr/X11R6/include/X11/Xaw/Command.h
Record.o: /usr/X11R6/include/X11/Xaw/Label.h
Record.o: /usr/X11R6/include/X11/Xaw/Simple.h
Record.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Record.o: /usr/X11R6/include/X11/Xaw/List.h
Record.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Record.o: /usr/X11R6/include/X11/Xaw/Text.h
Record.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Record.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Record.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Record.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Record.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Record.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Record.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Record.o: /home/cannam/rosegarden/include/MidiFile.h
Record.o: /home/cannam/rosegarden/include/MidiEvent.h
Record.o: /home/cannam/rosegarden/include/MidiConsts.h
Record.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdlib.h
Record.o: /usr/include/errno.h /usr/include/linux/errno.h
Record.o: /usr/include/asm/errno.h /usr/include/alloca.h
Record.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Record.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Record.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Record.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Record.o: /usr/include/asm/byteorder.h
Record.o: /home/cannam/rosegarden/include/MidiTrack.h
Record.o: /home/cannam/rosegarden/include/ServiceNames.h
Record.o: /home/cannam/rosegarden/include/Systems.h Consts.h Main.h
Record.o: Tracking.h Menu.h /home/cannam/rosegarden/include/Mapper.h
Record.o: /home/cannam/rosegarden/include/Mapper_OSS.h
Record.o: /usr/include/sys/soundcard.h /usr/include/linux/soundcard.h
Record.o: /usr/include/linux/ioctl.h /usr/include/asm/ioctl.h Record.h
Record.o: Sequence.h /home/cannam/rosegarden/include/MidiErrorHandler.h
Record.o: /home/cannam/rosegarden/include/Debug.h
Record.o: /home/cannam/rosegarden/include/MidiBHeap.h TrackList.h
Record.o: /usr/include/fcntl.h /usr/include/gnu/types.h
Record.o: /usr/include/linux/fcntl.h /usr/include/asm/fcntl.h
Record.o: /usr/include/sys/time.h /usr/include/linux/time.h
Record.o: /usr/include/sys/time.h /usr/include/sys/times.h
Record.o: /usr/include/linux/times.h /usr/include/unistd.h
Record.o: /usr/include/posix_opt.h /usr/include/confname.h
Record.o: /usr/include/sys/signal.h
Sequence.o: /home/cannam/rosegarden/include/Mapper.h
Sequence.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Sequence.o: /usr/include/signal.h /usr/include/features.h
Sequence.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Sequence.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Sequence.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Sequence.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Sequence.o: /usr/include/asm/signal.h /usr/include/ctype.h
Sequence.o: /usr/include/endian.h /usr/include/bytesex.h
Sequence.o: /usr/include/string.h
Sequence.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Sequence.o: /home/cannam/rosegarden/include/Systems.h
Sequence.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Sequence.o: /usr/include/libio.h /usr/include/_G_config.h
Sequence.o: /usr/include/stdlib.h /usr/include/errno.h
Sequence.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Sequence.o: /usr/include/alloca.h
Sequence.o: /home/cannam/rosegarden/include/MidiXInclude.h
Sequence.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Sequence.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Sequence.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Sequence.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Sequence.o: /usr/X11R6/include/X11/Composite.h
Sequence.o: /usr/X11R6/include/X11/Constraint.h
Sequence.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Sequence.o: /usr/X11R6/include/X11/StringDefs.h
Sequence.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Sequence.o: /usr/X11R6/include/X11/SM/SM.h
Sequence.o: /usr/X11R6/include/X11/ICE/ICElib.h
Sequence.o: /usr/X11R6/include/X11/ICE/ICE.h
Sequence.o: /usr/X11R6/include/X11/cursorfont.h
Sequence.o: /home/cannam/rosegarden/include/Yawn.h
Sequence.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Form.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Paned.h
Sequence.o: /usr/X11R6/include/X11/Xmu/Converters.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Reports.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Box.h
Sequence.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Command.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Label.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Simple.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Sequence.o: /usr/X11R6/include/X11/Xaw/List.h
Sequence.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Text.h
Sequence.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Sequence.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Sequence.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Sequence.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Sequence.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Sequence.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Sequence.o: /home/cannam/rosegarden/include/MidiEvent.h
Sequence.o: /home/cannam/rosegarden/include/MidiConsts.h
Sequence.o: /home/cannam/rosegarden/include/Mapper_OSS.h
Sequence.o: /usr/include/sys/soundcard.h /usr/include/linux/soundcard.h
Sequence.o: /usr/include/linux/ioctl.h /usr/include/asm/ioctl.h Globals.h
Sequence.o: Types.h /home/cannam/rosegarden/include/MidiFile.h
Sequence.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Sequence.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Sequence.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Sequence.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Sequence.o: /usr/include/asm/byteorder.h
Sequence.o: /home/cannam/rosegarden/include/MidiTrack.h
Sequence.o: /home/cannam/rosegarden/include/ServiceNames.h Consts.h
Sequence.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Sequence.o: /home/cannam/rosegarden/include/MidiBHeap.h Tracking.h Sequence.h
Sequence.o: Menu.h Record.h MidiSetupDlgs.h
Sequence.o: /home/cannam/rosegarden/include/Debug.h
Tracking.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Tracking.o: /usr/include/signal.h /usr/include/features.h
Tracking.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
Tracking.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
Tracking.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
Tracking.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
Tracking.o: /usr/include/asm/signal.h /usr/include/ctype.h
Tracking.o: /usr/include/endian.h /usr/include/bytesex.h
Tracking.o: /usr/include/string.h
Tracking.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Tracking.o: /home/cannam/rosegarden/include/ILClient.h
Tracking.o: /home/cannam/rosegarden/include/ILTypes.h
Tracking.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Tracking.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Tracking.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
Tracking.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
Tracking.o: /usr/X11R6/include/X11/Composite.h
Tracking.o: /usr/X11R6/include/X11/Constraint.h
Tracking.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
Tracking.o: /usr/X11R6/include/X11/StringDefs.h
Tracking.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
Tracking.o: /usr/X11R6/include/X11/SM/SM.h
Tracking.o: /usr/X11R6/include/X11/ICE/ICElib.h
Tracking.o: /usr/X11R6/include/X11/ICE/ICE.h
Tracking.o: /usr/X11R6/include/X11/cursorfont.h
Tracking.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Tracking.o: /usr/include/libio.h /usr/include/_G_config.h
Tracking.o: /usr/include/stdlib.h /usr/include/errno.h
Tracking.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
Tracking.o: /usr/include/alloca.h /usr/include/unistd.h
Tracking.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
Tracking.o: /usr/include/confname.h
Tracking.o: /home/cannam/rosegarden/include/MidiFile.h
Tracking.o: /home/cannam/rosegarden/include/MidiEvent.h
Tracking.o: /home/cannam/rosegarden/include/MidiConsts.h
Tracking.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Tracking.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Tracking.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Tracking.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Tracking.o: /usr/include/asm/byteorder.h
Tracking.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Tracking.o: /home/cannam/rosegarden/include/MidiVarLenNums.h Menu.h Types.h
Tracking.o: /home/cannam/rosegarden/include/MidiXInclude.h
Tracking.o: /home/cannam/rosegarden/include/Yawn.h
Tracking.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Form.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Paned.h
Tracking.o: /usr/X11R6/include/X11/Xmu/Converters.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Reports.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Box.h
Tracking.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Command.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Label.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Simple.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Toggle.h
Tracking.o: /usr/X11R6/include/X11/Xaw/List.h
Tracking.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Text.h
Tracking.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Tracking.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Tracking.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Tracking.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Tracking.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Tracking.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h Consts.h Globals.h
Tracking.o: /home/cannam/rosegarden/include/MidiTrack.h
Tracking.o: /home/cannam/rosegarden/include/ServiceNames.h
Tracking.o: /home/cannam/rosegarden/include/Systems.h Main.h Tracking.h
Tracking.o: /home/cannam/rosegarden/include/Debug.h
TrackList.o: /home/cannam/rosegarden/include/Mapper.h
TrackList.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
TrackList.o: /usr/include/signal.h /usr/include/features.h
TrackList.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
TrackList.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
TrackList.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
TrackList.o: /usr/include/sys/bitypes.h /usr/include/linux/signal.h
TrackList.o: /usr/include/asm/signal.h /usr/include/ctype.h
TrackList.o: /usr/include/endian.h /usr/include/bytesex.h
TrackList.o: /usr/include/string.h
TrackList.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
TrackList.o: /home/cannam/rosegarden/include/Systems.h
TrackList.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
TrackList.o: /usr/include/libio.h /usr/include/_G_config.h
TrackList.o: /usr/include/stdlib.h /usr/include/errno.h
TrackList.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
TrackList.o: /usr/include/alloca.h
TrackList.o: /home/cannam/rosegarden/include/MidiXInclude.h
TrackList.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
TrackList.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
TrackList.o: /usr/X11R6/include/X11/Xosdefs.h /usr/X11R6/include/X11/Xutil.h
TrackList.o: /usr/X11R6/include/X11/Xresource.h /usr/X11R6/include/X11/Core.h
TrackList.o: /usr/X11R6/include/X11/Composite.h
TrackList.o: /usr/X11R6/include/X11/Constraint.h
TrackList.o: /usr/X11R6/include/X11/Object.h /usr/X11R6/include/X11/RectObj.h
TrackList.o: /usr/X11R6/include/X11/StringDefs.h
TrackList.o: /usr/X11R6/include/X11/Shell.h /usr/X11R6/include/X11/SM/SMlib.h
TrackList.o: /usr/X11R6/include/X11/SM/SM.h
TrackList.o: /usr/X11R6/include/X11/ICE/ICElib.h
TrackList.o: /usr/X11R6/include/X11/ICE/ICE.h
TrackList.o: /usr/X11R6/include/X11/cursorfont.h
TrackList.o: /home/cannam/rosegarden/include/Yawn.h
TrackList.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Form.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Paned.h
TrackList.o: /usr/X11R6/include/X11/Xmu/Converters.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Viewport.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Reports.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Box.h
TrackList.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Command.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Label.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Simple.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Toggle.h
TrackList.o: /usr/X11R6/include/X11/Xaw/List.h
TrackList.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Text.h
TrackList.o: /usr/X11R6/include/X11/Xaw/TextSink.h
TrackList.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
TrackList.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
TrackList.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
TrackList.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Repeater.h
TrackList.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
TrackList.o: /home/cannam/rosegarden/include/MidiEvent.h
TrackList.o: /home/cannam/rosegarden/include/MidiConsts.h
TrackList.o: /home/cannam/rosegarden/include/Mapper_OSS.h
TrackList.o: /usr/include/sys/soundcard.h /usr/include/linux/soundcard.h
TrackList.o: /usr/include/linux/ioctl.h /usr/include/asm/ioctl.h Globals.h
TrackList.o: Types.h /home/cannam/rosegarden/include/MidiFile.h
TrackList.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
TrackList.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
TrackList.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
TrackList.o: /usr/include/linux/uio.h /usr/include/linux/in.h
TrackList.o: /usr/include/asm/byteorder.h
TrackList.o: /home/cannam/rosegarden/include/MidiTrack.h
TrackList.o: /home/cannam/rosegarden/include/ServiceNames.h Consts.h
TrackList.o: EventListWindow.h PianoRoll.h Clipboard.h MainWindow.h
TrackList.o: /home/cannam/rosegarden/include/ILClient.h
TrackList.o: /home/cannam/rosegarden/include/ILTypes.h Main.h Tracking.h
TrackList.o: Menu.h EventListMenu.h PianoRollMenu.h Undo.h
TrackList.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
TrackList.o: /home/cannam/rosegarden/include/Debug.h
Undo.o: /home/cannam/rosegarden/include/MidiErrorHandler.h
Undo.o: /home/cannam/rosegarden/include/MidiFile.h
Undo.o: /home/cannam/rosegarden/include/MidiEvent.h
Undo.o: /usr/X11R6/include/X11/Intrinsic.h /usr/X11R6/include/X11/Xlib.h
Undo.o: /usr/include/sys/types.h /usr/include/linux/types.h
Undo.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
Undo.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
Undo.o: /usr/X11R6/include/X11/X.h /usr/X11R6/include/X11/Xfuncproto.h
Undo.o: /usr/X11R6/include/X11/Xosdefs.h
Undo.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
Undo.o: /usr/X11R6/include/X11/Xutil.h /usr/X11R6/include/X11/Xresource.h
Undo.o: /usr/include/string.h /usr/include/features.h
Undo.o: /usr/include/sys/cdefs.h /usr/X11R6/include/X11/Core.h
Undo.o: /usr/X11R6/include/X11/Composite.h
Undo.o: /usr/X11R6/include/X11/Constraint.h /usr/X11R6/include/X11/Object.h
Undo.o: /usr/X11R6/include/X11/RectObj.h
Undo.o: /home/cannam/rosegarden/include/MidiConsts.h
Undo.o: /home/cannam/rosegarden/include/Lists.h /usr/include/stdio.h
Undo.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/stdlib.h
Undo.o: /usr/include/errno.h /usr/include/linux/errno.h
Undo.o: /usr/include/asm/errno.h /usr/include/alloca.h
Undo.o: /usr/include/netinet/in.h /usr/include/sys/socket.h
Undo.o: /usr/include/linux/socket.h /usr/include/asm/socket.h
Undo.o: /usr/include/asm/sockios.h /usr/include/linux/sockios.h
Undo.o: /usr/include/linux/uio.h /usr/include/linux/in.h
Undo.o: /usr/include/asm/byteorder.h Globals.h
Undo.o: /home/cannam/rosegarden/include/SysDeps.h ../../config.h
Undo.o: /usr/include/signal.h /usr/include/linux/signal.h
Undo.o: /usr/include/asm/signal.h /usr/include/ctype.h /usr/include/endian.h
Undo.o: /usr/include/bytesex.h Types.h
Undo.o: /home/cannam/rosegarden/include/MidiXInclude.h
Undo.o: /usr/X11R6/include/X11/StringDefs.h /usr/X11R6/include/X11/Shell.h
Undo.o: /usr/X11R6/include/X11/SM/SMlib.h /usr/X11R6/include/X11/SM/SM.h
Undo.o: /usr/X11R6/include/X11/ICE/ICElib.h /usr/X11R6/include/X11/ICE/ICE.h
Undo.o: /usr/X11R6/include/X11/cursorfont.h
Undo.o: /home/cannam/rosegarden/include/Yawn.h
Undo.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stdarg.h
Undo.o: /usr/X11R6/include/X11/Xaw/Form.h /usr/X11R6/include/X11/Xaw/Paned.h
Undo.o: /usr/X11R6/include/X11/Xmu/Converters.h
Undo.o: /usr/X11R6/include/X11/Xaw/Viewport.h
Undo.o: /usr/X11R6/include/X11/Xaw/Reports.h /usr/X11R6/include/X11/Xaw/Box.h
Undo.o: /usr/X11R6/include/X11/Xaw/MenuButton.h
Undo.o: /usr/X11R6/include/X11/Xaw/Command.h
Undo.o: /usr/X11R6/include/X11/Xaw/Label.h
Undo.o: /usr/X11R6/include/X11/Xaw/Simple.h
Undo.o: /usr/X11R6/include/X11/Xaw/Toggle.h /usr/X11R6/include/X11/Xaw/List.h
Undo.o: /usr/X11R6/include/X11/Xaw/AsciiText.h
Undo.o: /usr/X11R6/include/X11/Xaw/Text.h
Undo.o: /usr/X11R6/include/X11/Xaw/TextSink.h
Undo.o: /usr/X11R6/include/X11/Xaw/TextSrc.h
Undo.o: /usr/X11R6/include/X11/Xaw/AsciiSrc.h
Undo.o: /usr/X11R6/include/X11/Xaw/AsciiSink.h
Undo.o: /usr/X11R6/include/X11/Xaw/MultiSrc.h
Undo.o: /usr/X11R6/include/X11/Xaw/Repeater.h
Undo.o: /usr/X11R6/include/X11/Xaw/Scrollbar.h
Undo.o: /home/cannam/rosegarden/include/MidiTrack.h
Undo.o: /home/cannam/rosegarden/include/ServiceNames.h
Undo.o: /home/cannam/rosegarden/include/Systems.h Undo.h EventListWindow.h
Undo.o: PianoRoll.h Main.h Tracking.h Menu.h TrackList.h EventListMenu.h
Undo.o: PianoRollMenu.h /home/cannam/rosegarden/include/Debug.h

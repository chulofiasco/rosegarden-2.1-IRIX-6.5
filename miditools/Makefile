# Makefile for MIDI utility tools
# These tools are built as part of the main Rosegarden build

CC = cc
CFLAGS = -n32
LDFLAGS = -L/usr/lib32
LIBS = -lmd -ldmedia

BINDIR = ../bin

TOOLS = dump-midi-events test-midi-devices test-midi-timing

all: $(TOOLS)

dump-midi-events: dump-midi-events.c
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $< $(LDFLAGS) $(LIBS)

test-midi-devices: test-midi-devices.c
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $< $(LDFLAGS) $(LIBS)

test-midi-timing: test-midi-timing.c
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f $(BINDIR)/dump-midi-events
	rm -f $(BINDIR)/test-midi-devices
	rm -f $(BINDIR)/test-midi-timing

install: all
	mkdir -p $(DESTDIR)/usr/local/lib/rosegarden/miditools
	cp $(BINDIR)/dump-midi-events $(DESTDIR)/usr/local/lib/rosegarden/miditools/
	cp $(BINDIR)/test-midi-devices $(DESTDIR)/usr/local/lib/rosegarden/miditools/
	cp $(BINDIR)/test-midi-timing $(DESTDIR)/usr/local/lib/rosegarden/miditools/

.PHONY: all clean install

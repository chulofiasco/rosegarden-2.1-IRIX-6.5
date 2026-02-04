mandir =	${prefix}/share/man
prefix = 	/usr/local
#
# This is the command to format the man pages
#
ROFF = groff -mandoc -Tascii

MANOWN = bin
MANGRP = bin

MANN = midiconf.n midievnt.n midifree.n midiget.n midigrep.n midimake.n \
       midimerg.n midiplay.n midiput.n midiread.n midirec.n \
       midirew.n midisplt.n midistop.n midiwait.n midiwrit.n \
       midivers.n midimove.n midicopy.n mididel.n miditrck.n \
       mididev.n miditime.n midifeat.n
MAN1 = mplay.1 mrec.1 tclmidi.1 minfo.1 midtotcl.1

MANNCOOKED = midiconf.0 midievnt.0 midifree.0 midiget.0 midigrep.0 midimake.0 \
       midimerg.0 midiplay.0 midiput.0 midiread.0 midirec.0 \
       midirew.0 midisplt.0 midistop.0 midiwait.0 midiwrit.0 \
       midivers.0 midimove.0 midicopy.0 mididel.0 miditrck.0 \
       mididev.0 miditime.0 midifeat.0
MAN1COOKED = mplay.0 mrec.0 tclmidi.0 minfo.0 midtotcl.0

.SUFFIXES: .0 .1 .n

install:
	.././install-sh -c -o $(MANOWN) -g $(MANGRP) -m 444 $(MANN) $(mandir)/mann
	.././install-sh -c -o $(MANOWN) -g $(MANGRP) -m 444 $(MAN1) $(mandir)/man1

install-cooked: $(MANNCOOKED) $(MAN1COOKED)
	.././install-sh -c -o $(MANOWN) -g $(MANGRP) -m 444 $(MANNCOOKED) $(mandir)/catn
	.././install-sh -c -o $(MANOWN) -g $(MANGRP) -m 444 $(MAN1COOKED) $(mandir)/cat1

cooked: $(MANNCOOKED) $(MAN1COOKED)

clean:
	rm -f $(MANNCOOKED) $(MAN1COOKED)

.1.0:
	$(ROFF) $< > $@

.n.0:
	$(ROFF) $< > $@

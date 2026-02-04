
# Makefile for the Regexp library
#
# This is Henry Spencer's regexp library.  See README
# and COPYRIGHT for original distribution notices.
# 
# Some minor changes by Chris Cannam, solely to make
# function definitions ANSI-style rather than K&R-style
# so as to compile with the same environment/options as
# the Rosegarden source.  (I've also changed the
# include from regexp.h to Regexp.h and moved it into
# ../include, again in line with the policy of the rest
# of this source tree.)  Any errors are doubtless the
# fault of this mangling.


# # These three should have been exported from the superior make
#
# ROSEGARDEN	= /usr/people/cannam/rosegarden
# SYSINCDIRS	= -I/usr/include -I/usr/include/X11 -I/usr/include/X11/Xaw -I/usr/include/X11/bitmaps
# DEFINES		= -DFUNCPROTO -DHAVE_SPECIALIST_MALLOC_LIBRARY

REGEXP  	= $(ROSEGARDEN)/regexp
INTERLOCK	= $(ROSEGARDEN)/interlock
YAWN		= $(ROSEGARDEN)/yawn
LISTS		= $(ROSEGARDEN)/lists
MIDI		= $(ROSEGARDEN)/midi
COMMON		= $(ROSEGARDEN)/common

INCDIRS		= $(SYSINCDIRS) -I$(ROSEGARDEN)/include -I$(COMMON)/bitmaps

SYSLIBS		= -lmalloc -lXaw -lXext -lXmu -lXt -lX11
LIBS		= -lY -lLists -lMidi -lInterlock -lRegexp $(SYSLIBS)
LDFLAGS		= $(SYSLDFLAGS) -L$(INTERLOCK)/lib -L$(YAWN)/lib -L$(LISTS)/lib -L$(MIDI)/lib -L$(REGEXP)/lib $(LIBS)

CFLAGS		= $(INCDIRS) $(DEFINES) $(OPTFLAGS)
CCDEPFLAGS	=

TARGET		= $(REGEXP)/lib/libRegexp.a
OBJECTS		= regerror.o regexp.o regsub.o 


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

regerror.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
regerror.o: /usr/include/standards.h /usr/include/sgidefs.h
regerror.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
regerror.o: /usr/include/internal/stdlib_core.h
regexp.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
regexp.o: /usr/include/standards.h /usr/include/sgidefs.h
regexp.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
regexp.o: /usr/include/internal/stdlib_core.h /usr/include/string.h
regexp.o: /usr/include/internal/string_core.h
regexp.o: /root/build/rosegarden/include/Regexp.h regmagic.h
regsub.o: /usr/include/stdio.h /usr/include/internal/stdio_core.h
regsub.o: /usr/include/standards.h /usr/include/sgidefs.h
regsub.o: /usr/include/internal/sgimacros.h /usr/include/stdlib.h
regsub.o: /usr/include/internal/stdlib_core.h /usr/include/string.h
regsub.o: /usr/include/internal/string_core.h /usr/include/ctype.h
regsub.o: /usr/include/internal/ctype_core.h
regsub.o: /usr/include/internal/locale_attr_core.h
regsub.o: /root/build/rosegarden/include/Regexp.h regmagic.h

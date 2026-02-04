
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
		$(MAKEDEPEND) -- $(CFLAGS) -- *.c

# DO NOT DELETE

regerror.o: /usr/include/stdio.h /usr/include/libio.h
regerror.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
regerror.o: /usr/include/stdlib.h /usr/include/features.h
regerror.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
regerror.o: /usr/include/errno.h /usr/include/linux/errno.h
regerror.o: /usr/include/asm/errno.h /usr/include/alloca.h
regexp.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
regexp.o: /usr/include/sys/cdefs.h /usr/include/stdlib.h
regexp.o: /usr/include/features.h
regexp.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
regexp.o: /usr/include/errno.h /usr/include/linux/errno.h
regexp.o: /usr/include/asm/errno.h /usr/include/alloca.h
regexp.o: /usr/include/string.h /home/cannam/rosegarden/include/Regexp.h
regexp.o: regmagic.h
regsub.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
regsub.o: /usr/include/sys/cdefs.h /usr/include/stdlib.h
regsub.o: /usr/include/features.h
regsub.o: /usr/lib/gcc-lib/i486-linux/2.7.2/include/stddef.h
regsub.o: /usr/include/errno.h /usr/include/linux/errno.h
regsub.o: /usr/include/asm/errno.h /usr/include/alloca.h
regsub.o: /usr/include/string.h /usr/include/ctype.h /usr/include/endian.h
regsub.o: /usr/include/bytesex.h /home/cannam/rosegarden/include/Regexp.h
regsub.o: regmagic.h

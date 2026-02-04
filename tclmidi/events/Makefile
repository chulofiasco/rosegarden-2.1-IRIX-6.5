#----------------------------------------------------------------
# The information below is modified by the configure script when
# Makefile is generated from Makefile.in.  You shouldn't normally
# modify any of this stuff by hand.
#----------------------------------------------------------------

SHLIB_CFLAGS =		
SHLIB_LD =		ld -n32 -shared -rdata_shared
SHLIB_SUFFIX =		.so
SHLIB_VERSION =		
SRC_DIR =		.

srcdir = .


CFLAGS = -n32 -woff 608,1552,1047,3970 -I/usr/freeware/include -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_FILIO_H=1 -DTIME_WITH_SYS_TIME=1 -DRETSIGTYPE=void -DWORDS_BIGENDIAN=1 -Dinline=__inline
CXXFLAGS = -n32 -woff 608,1552,1047,3970 -I/usr/include -I/usr/include/CC -I/usr/freeware/include -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_FILIO_H=1 -DTIME_WITH_SYS_TIME=1 -DRETSIGTYPE=void -DWORDS_BIGENDIAN=1 -Dinline=__inline
CPPFLAGS = -I/usr/freeware/include/tcl
CC_SWITCHES = ${CPPFLAGS} -n32 -woff 608,1552,1047,3970 -I/usr/include -I/usr/include/CC -I/usr/freeware/include -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_FILIO_H=1 -DTIME_WITH_SYS_TIME=1 -DRETSIGTYPE=void -DWORDS_BIGENDIAN=1 -Dinline=__inline -DTCL_7_5 -I${TCL_GENERIC_DIR} ${SHLIB_CFLAGS} \
    -I$(srcdir)/../smf -I$(srcdir)/../song -I$(srcdir)/.. -I$(srcdir)

EVENTFILES = ChanPres.o Event.o MChanPrf.o MCopy.o MCue.o \
    MEOT.o MEvent.o MInstNam.o MKey.o MLyric.o \
    MMarker.o MPortNum.o MSMPTE.o MSeqNam.o MSeqNum.o \
    MSeqSpec.o MTempo.o MText.o MTime.o NormEvnt.o \
    NoteOff.o NoteOn.o KeyPres.o Param.o PitchWhl.o Program.o \
    SysEx.o MUnknown.o EvntUtil.o Note.o

EVENTFILES_SRC = ChanPres.cxx Event.cxx MChanPrf.cxx MCopy.cxx MCue.cxx \
    MEOT.cxx MEvent.cxx MInstNam.cxx MKey.cxx MLyric.cxx \
    MMarker.cxx MPortNum.cxx MSMPTE.cxx MSeqNam.cxx MSeqNum.cxx \
    MSeqSpec.cxx MTempo.cxx MText.cxx MTime.cxx NormEvnt.cxx \
    NoteOff.cxx NoteOn.cxx KeyPres.cxx Param.cxx PitchWhl.cxx Program.cxx \
    SysEx.cxx MUnknown.cxx EvntUtil.cxx Note.cxx


.SUFFIXES: .cxx $(SUFFIXES)

.cxx.o:
	CC -c $(CC_SWITCHES) $<

objs: $(EVENTFILES)
	for f in $(EVENTFILES) ; do echo events/$$f ; done > objs

depend:
	sed "/[#]# Dependancies/,$$$$d" < Makefile > Makefile.tmp
	echo >> Makefile.tmp
	echo "##" "Dependancies" >> Makefile.tmp
	CC -M $(CC_SWITCHES) $(EVENTFILES_SRC) >> Makefile.tmp
	cp Makefile.tmp Makefile
	rm Makefile.tmp

clean:
	-rm *.o objs

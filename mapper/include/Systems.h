
#ifndef _MIDI_SYSTEMS_H_
#define _MIDI_SYSTEMS_H_

#ifdef SYSTEM_LINUX
#error The SYSTEM_LINUX symbol is no longer supported; use SYSTEM_OSS instead.
#endif

#ifdef SYSTEM_FREEBSD
#error The SYSTEM_FREEBSD symbol is no longer supported; use SYSTEM_OSS instead.
#endif


/* The rest are for Makefile back-compatibility */

#ifdef SEQUENCE_SGI
#define SYSTEM_SGI
#endif

#ifdef RECORD_SGI
#define SYSTEM_SGI
#endif

#ifdef SEQUENCE_ZILOG
#define SYSTEM_ZILOG
#endif

#ifdef RECORD_ZILOG
#define SYSTEM_ZILOG
#endif

#ifdef SEQUENCE_OSS
#define SYSTEM_OSS
#endif

#ifdef RECORD_OSS
#define SYSTEM_OSS
#endif

#ifdef SYSTEM_SGI
#ifdef SYSTEM_OSS
#error Too many SYSTEM defines -- SGI and OSS or FREEBSD
#endif
#ifdef SYSTEM_ZILOG
#error Too many SYSTEM defines -- SGI and ZILOG
#endif
#endif

#ifdef SYSTEM_OSS
#ifdef SYSTEM_ZILOG
#error Too many SYSTEM defines -- OSS or FREEBSD and ZILOG
#endif
#endif


/* and finally, SYSTEM_SILENT */

#ifndef SYSTEM_OSS
#ifndef SYSTEM_ZILOG
#ifndef SYSTEM_SGI
#ifndef SYSTEM_SILENT
#define SYSTEM_SILENT
#endif
#endif
#endif
#endif


#endif /* _SYSTEMS_H_ */


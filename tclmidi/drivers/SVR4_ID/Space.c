#include "config.h"
#include "sys/param.h"
#include "sys/types.h"
#include "sys/signal.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/poll.h"
#include "sys/midios.h"

int NumMidi = MIDI_UNITS;
struct unixware_midi_softc midi_sc[MIDI_UNITS];

/*
 * Fill in these values with the type of device to use.  The complete
 * list can be found in midi_iface.h.  The following list might
 * be out of date:
 * 	0	Fully MPU401 compat.
 *	1	MPU401 only in UART mode
 *	2	Music Quest MQX32 with SMPTE
 *	3	Gravis w/ internal voices - GF1 (GUS)
 *	4	Gravis w/ internal voices - Iwave (PnP)
 *	5	Gravis w/ external voices - GF1 (GUS)
 *	6	Gravis w/ external voices - Iwave (PnP)
 *	7	SoundBlaster
 *	8	A PC style UART (16450, etc.)
 *
 * XXX
 * Ideally this information should be in the System file somewhere,
 * but I've lost my documentation of the file format.
 */
long midi_types[MIDI_UNITS] = {0};

/* These are done automatically. */
int midi_addrs[MIDI_UNITS] = {
#ifdef MIDI_0_SIOA
MIDI_0_SIOA
#endif
#ifdef MIDI_1_SIOA
, MIDI_1_SIOA
#endif
#ifdef MIDI_2_SIOA
, MIDI_2_SIOA
#endif
#ifdef MIDI_3_SIOA
, MIDI_3_SIOA
#endif
};

int midi_intrs[MIDI_UNITS] = {
#ifdef MIDI_0_VECT
MIDI_0_VECT
#endif
#ifdef MIDI_1_VECT
, MIDI_1_VECT
#endif
#ifdef MIDI_2_VECT
, MIDI_2_VECT
#endif
#ifdef MIDI_3_VECT
, MIDI_3_VECT
#endif
};

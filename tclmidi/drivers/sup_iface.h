/*-
 * Copyright (c) 1993, 1994, 1995, 1996 Michael B. Durian.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Michael B. Durian.
 * 4. The name of the the Author may be used to endorse or promote 
 *    products derived from this software without specific prior written 
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef SUP_IFACE_H
#define SUP_IFACE_H

#include "sup_stynamic.h"
#include "sup_event.h"


/* list of currently understood midi devices. */
#define MIDI_DEV_MPU401			0 /* fully MPU401 compat. */
#define MIDI_DEV_MPU401UART		1 /* MPU401 compat. only in UART mode */
#define MIDI_DEV_MQX32			2 /* Music Quest MQX32 with SMPTE */
#define MIDI_DEV_GRAVIS_INT_GF1		3 /* Gravis w/ internal voices
					     - GF1 (GUS) */
#define MIDI_DEV_GRAVIS_INT_IWAVE	4 /* Gravis w/ internal voices
					     - Iwave (PnP) */
#define MIDI_DEV_GRAVIS_EXT_GF1		5 /* Gravis w/ external voices
					     - GF1 (GUS) */
#define MIDI_DEV_GRAVIS_EXT_IWAVE	6 /* Gravis w/ external voices
					     - Iwave (PnP) */
#define MIDI_DEV_SB			7 /* SoundBlaster */
#define MIDI_DEV_SERIAL			8 /* A PC style UART (16450, etc.) */

typedef enum {MIDI_ISA_IFACE} MIDI_IFACE_TYPE;

/* structure for ISA devices when probing */
struct midi_isa_iface {
	unsigned	short io_port;
	int		irq;
};

/*
 * Generalized interface functions.
 */
/*
 * probe the device to see if it exists.  Returns a struct iface if it
 * exists, NULL otherwise.
 * addr is a void * to ease porting to different archetectures if we
 * ever want to, but under intel, it's the i/o port.
 */
typedef struct midi_iface *(*ProbeFunc)(struct midi_softc *softc,
    MIDI_IFACE_TYPE type, void *loc);

/*
 * What are we?
 */
typedef const char *(*NameFunc)(struct midi_iface *iface);

/*
 * Fills in size with the size of the interface.  On PC's this would
 * be an integer containing the number of consecutive I/O ports in use.
 * Other arch's might fill in different structures.
 */
typedef void (*IFaceSizeFunc)(struct midi_iface *iface, void *size);

/* generate an interrupt.  Useful for automatic interrupt detection. */
typedef void (*GenIntrFunc)(struct midi_iface *iface);

/* Resets and initializes the device.  Will put the board in UART mode. */
typedef int (*ResetFunc)(struct midi_iface *iface);

/*
 * Called in open function.  Returns 0 ok, errno otherwise.
 */
typedef int (*OpenFunc)(struct midi_iface *iface);

/*
 * Called at final close.
 */
typedef void (*CloseFunc)(struct midi_iface *iface);

/*
 * Called at the beginning of midiintr.  This function returns the
 * byte read, or -1 if there is no data to be processed.
 */
typedef int (*IntrFunc)(struct midi_iface *iface);

/*
 * Used to check if data is available for reading.  In case another
 * intr occurs while we're processing the first.
 */
typedef int (*DataAvailFunc)(struct midi_iface *iface);

/* Output a byte to device. */
typedef void (*WriteEventFunc)(struct midi_iface *iface, struct event *event);

/* Input a byte from device. */
typedef unsigned char (*ReadByteFunc)(struct midi_iface *iface);

/*
 * Handle extended features via ioctl(2).  This function is called
 * from gen_midiioctl when ever a MFEATURE ioctl is encountered.
 */
typedef int (*FeatureFunc)(struct midi_iface *iface,
    struct midi_feature *feature);

/* We know how to clean up after ourselves */
typedef void (*FreeFunc)(struct midi_iface *iface);

struct midi_iface {
	struct	midi_softc *softc;
	MIDI_IFACE_TYPE	type;
	void		*loc;
	NameFunc	name;
	IFaceSizeFunc	size;
	GenIntrFunc	gen_intr;
	ResetFunc	reset;
	OpenFunc	open;
	CloseFunc	close;
	IntrFunc	intr;
	DataAvailFunc	data_avail;
	WriteEventFunc	write;
	ReadByteFunc	read;
	FeatureFunc	feature;
	FreeFunc	free;
};

struct midi_iface *midi_dev_probe(struct midi_softc *softc, long dev_type,
    MIDI_IFACE_TYPE type, void *iface_loc);

struct midi_iface *mpu401_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *iface_loc);
struct midi_iface *mpu401uart_probe(struct midi_softc *softc,
    MIDI_IFACE_TYPE type, void *iface_loc);
struct midi_iface *mqx32_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *iface_loc);
struct midi_iface *gus_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *iface_loc);
struct midi_iface *sb_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *iface_loc);
struct midi_iface *serial_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *iface_loc);
/*
 * Three types of probes, 'cuz the driver can be in three modes: 1) using
 * an external synthesizer and the MIDIOUT port, 2) using internal
 * wavetable voices with the old GF1 GUS chip, or 3) using internal
 * wavetable voices with the new PnP Iwave chip.
 */
struct midi_iface *gravis_internal_gf1_probe(struct midi_softc *softc,
    MIDI_IFACE_TYPE type, void *vloc);
struct midi_iface *gravis_internal_iwave_probe(struct midi_softc *softc,
    MIDI_IFACE_TYPE type, void *vloc);
struct midi_iface *gravis_external_gf1_probe(struct midi_softc *softc,
    MIDI_IFACE_TYPE type, void *vloc);
struct midi_iface *gravis_external_iwave_probe(struct midi_softc *softc,
    MIDI_IFACE_TYPE type, void *vloc);

#endif

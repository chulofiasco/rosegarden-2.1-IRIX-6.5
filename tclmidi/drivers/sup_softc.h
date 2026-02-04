/*-
 * Copyright (c) 1995, 1996 Michael B. Durian.  All rights reserved.
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
#ifndef SUP_SOFTC_H
#define SUP_SOFTC_H

/* include for SMPTE_frame def */
#include "midiioctl.h"

/* for interface functions */
#include "sup_iface.h"

struct midi_softc {      /* Driver status information */
	/* midi specific stuff */
	SELCHAN	wsel;
	SELCHAN	rsel;
	SELCHAN	esel;
	struct	midi_iface *iface;
	struct	midi_timer *timer;
	struct	event_queue *rqueue;
	struct	event_queue *wqueue;
	struct	event_queue *thruqueue;	/* pass thru and raw event */
	struct	stynamic rpartial;
	struct	stynamic wpartial;
	struct	partial_event partial_event;
	struct	tempo_change *tempo_changes;
	volatile	long status;
	/*
	 * SVR4 does its kernel building using a C compiler that does
	 * not understand long long.  Though this midi driver requires
	 * long longs to build (ie gcc), we can't avoid using the stock
	 * C compiler for some stages of compiling, thus we make sure
	 * the space requirements are correct.
	 */
#ifdef __GNUC__
	long long	premainder;
	long long	rremainder;
#else
	long	premainder[2];
	long	rremainder[2];
#endif
	u_long	extclock;
	u_long	prev_incoming;
	u_long	prev_outgoing;
	u_long	write_smf_time;		/* ab smf time of last event in write */
	long	features;
	long	ptempo;			/* usec / beat */
	long	rtempo;			/* usec / beat */
	long	prev_rtempo;
	int	id;			/* device id */
	int	pgid;
	int	division;
	int	wpartialpos;
	int	master;			/* our master, if a slave */
	int	num_slaves;		/* number of slaves */
	int	slaves[MAX_MIDI_DEV];	/* if a master, the slaves list */
	short	raw_count;		/* number of raw devices opened */
	short	timed_count;		/* number of timed devices opened */
	short	oread_count;		/* number of read devices opened */
	short	owrite_count;		/* number of write devices opened */
	short	channel_mask;		/* a bit mask of tracks *not* to play */
	short	noteon[0x80];		/*
					 * each element is a different pitch
					 * each bit is a different channel
					 */
	u_char	readrs;
	u_char	writers;
	u_char	raw_writers;
	u_char	noteonrs;
	/*
	 * the following are sleep addresses - BSD doesn't really need
	 * separate variables for sleeping, but Linux does and we want
	 * to be portable.
	 */
	SLEEPCHAN	sleep_flush;
	SLEEPCHAN	sleep_read;
	SLEEPCHAN	sleep_write;
	SLEEPCHAN	sleep_write_event;
	/*
	 * some OSs use special IDs for timeouts instead of the function
	 * pointer.
	 */
	void		*timeout_id;
	TIMEOUT_ID	*thru_timeout_id;	/* pass thru & raw id */
};

#endif

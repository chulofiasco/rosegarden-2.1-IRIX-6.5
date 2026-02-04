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
#ifndef SUP_VAR_H
#define SUP_VAR_H

#include "sup_stynamic.h"
#include "sup_event.h"
#include "sup_timer.h"

/*
 * This is the maximum number of midi devices we can have in one system.
 * This is a software limit, not a hardware limit.  If you want more,
 * just increase this value.
 */
#define MAX_MIDI_DEV 4

/*
 * midi status flags
 */
#define MIDI_UNAVAILABLE	(1 << 0) /* not in uart mode */
#define MIDI_READING		(1 << 1) /* open for reading */
#define MIDI_WRITING		(1 << 2) /* open for writing */
#define MIDI_RD_BLOCK		(1 << 3) /* read will block */
#define MIDI_WR_BLOCK		(1 << 4) /* write will block */
#define MIDI_WR_ABORT		(1 << 5) /* write should abort */
#define MIDI_OPEN		(1 << 6) /* device is open */
#define MIDI_FLUSH_SLEEP	(1 << 7) /* blocking on flush */
#define MIDI_RD_SLEEP		(1 << 8) /* blocking on read */
#define MIDI_WR_SLEEP		(1 << 9) /* blocking on write */
#define MIDI_ASYNC		(1 << 11) /* send sigios */
#define MIDI_SENDIO		(1 << 12) /* a sigio should be send at low h2o */
#define MIDI_THRU		(1 << 13) /* pass in port to out port? */
#define MIDI_RECONPLAY		(1 << 14) /* don't record until we start to play */
#define MIDI_EXTCLK		(1 << 15) /* use external clock */
#define MIDI_FIRST_WRITE	(1 << 16) /* reset the clock on first t/o */
#define MIDI_FIRST_SMPTE	(1 << 17) /* set if no SMPTE yet */
#define MIDI_RSEL		(1 << 18) /* select on read active */
#define MIDI_WSEL		(1 << 19) /* select on write active */
#define MIDI_ESEL		(1 << 20) /* select on execption active */
#define MIDI_TIMEOUT_PENDING	(1 << 21) /* timeout is pending */
#define MIDI_THRU_CB_SCHED	(1 << 24) /* thru callback scheduled? */
#define MIDI_MASTER		(1 << 25) /* are we a master? */
#define MIDI_TIME_WARP		(1 << 26) /* did time shift? */
#define MIDI_WRITING_EVENT	(1 << 27) /* currently writing an event */

/*
 * access flags
 */
#define MIDI_OREAD		(1 << 0)
#define MIDI_OWRITE		(1 << 1)
#define MIDI_NDELAY		(1 << 2)
#define MIDI_RAW_DEVICE		(1 << 3)

/*
 * These are the various input data states
 */
typedef enum {START, NEEDDATA1, NEEDDATA2, SYSEX, SYSTEM1, SYSTEM2, MTC}
    InputState;

/*
 * SMPTE configuration
 */
#define SMPTE_DROPCHK 10	/* check for loss of sync every 10 ticks */
#define SMPTE_DROPOUT 50	/* 50 ticks without 0xf1 ==> loss of sync */


/*
 * data from the board that hasn't formed a complete event yet
 */
struct partial_event {
	struct	stynamic event;		/* the data */
	u_long	time;			/* event time */
	long	tempo;			/* tempo setting when event arrived */
	InputState	state;		/* what we are expecting next */
	u_char	rs;			/* the midi running state */
};

/*
 * keep a list of tempo changes
 */
struct tempo_change {
	u_long	time;		/* time tempo change occured in timing ticks */
	u_long	smf_time;	/* same but in SMF ticks */
	long	tempo;		/* the new tempo */
	struct	tempo_change *next;
};

/*
 * A event queue, used for both incoming and outgoing
 */
#define MIDI_Q_SIZE 150
#define MIDI_LOW_WATER 40

struct event_queue {
	int	count;
	struct	event events[MIDI_Q_SIZE];
	struct	event *end;
	struct	event *head;
	struct	event *tail;
};

/*
 * SMPTE stuff
 */
#define SMPTE_ORIGIN 3

/*
 * slop time
 * If we've fallen more than BACKLOG_TIME ticks behind, don't play
 * the events.  This is so we can "fastforward" through a song -
 * picking up important events like tempo changes, but not actually
 * playing any note on events.
 */
#define BACKLOG_TIME 5

#endif

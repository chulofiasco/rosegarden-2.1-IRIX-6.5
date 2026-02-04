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

#ifndef SUP_TIMER_H
#define SUP_TIMER_H

/*
 * General purpose timing structure.
 */
typedef enum {MT_KERNEL, MT_SMPTE, MT_MPU401} MT_TYPE;

/* forward references */
struct midi_timer;
struct midi_softc;

typedef void (*MT_TimeOutFn)(struct midi_timer *mt, void *id,
    TIMEOUT_FN fn, TIMEOUT_ARG arg, int t);
typedef void (*MT_UntimeOutFn)(struct midi_timer *mt, void *id,
    TIMEOUT_ARG arg);
typedef u_long (*MT_GetClockFn)(struct midi_timer *mt);
typedef void (*MT_InitClockFn)(struct midi_timer *mt);
typedef void (*MT_ResetFn)(struct midi_timer *mt);
typedef void (*MT_FreeFn)(struct midi_timer *mt);
typedef struct midi_timer *(*MT_DupFn)(struct midi_timer *mt);
typedef void *(*MT_CreateTimeOutIDFn)(void);
typedef void (*MT_ReleaseTimeOutIDFn)(void *id);
struct midi_timer {
	struct	midi_softc *softc;
	MT_TYPE		type;
	int		hz;
	int		ref_count;
	MT_TimeOutFn	timeout;	/* called to schedule timeouts */
	MT_UntimeOutFn	untimeout;	/* called to deschedule a timeout */
	MT_GetClockFn	get_clock;	/* gets the current time */
	MT_InitClockFn	init_clock;	/* called before first event schedule */
	MT_ResetFn	reset;		/* called from fullreset */
	MT_FreeFn	free;		/* release this structure */
	MT_DupFn	dup;		/* duplicate this timer */
	MT_CreateTimeOutIDFn	create_id;	/* create a timeout id */
	MT_ReleaseTimeOutIDFn	release_id;	/* release a timeout id */
};

/* how to create different types of timers */
struct midi_timer *midi_create_kernel_timer(struct midi_softc *softc);
struct midi_timer *midi_create_SMPTE_timer(struct midi_softc *softc);
struct midi_timer *midi_create_MPU401_timer(struct midi_softc *softc);

/*
 * These are the only SMPTE support functions the main part of the
 * driver need know about.
 */
void midi_parse_MTC_event __P((struct midi_softc *, int /* u_char */));
void midi_drop_MTC_event __P((struct midi_softc *, int /* u_char */));
void midi_SMPTE_pause __P((struct midi_softc *, struct event *));

/*
 * How to update the MPU401 timer
 */
void mpu401_update_timer __P((struct midi_timer *t));
void mpu401_dropped_timer __P((struct midi_timer *t));

/* function to switch timers */
int midi_switch_timer __P((struct midi_softc *softc,
    struct midi_timer *(*new_timer)(struct midi_softc *softc)));

#endif

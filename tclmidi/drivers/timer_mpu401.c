/*-
 * Copyright (c) 1995 Michael B. Durian.  All rights reserved.
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

/*
 * this timing module is almost completely self contained, but a bit
 * of the SMPTE code leaks over to midi.c.  This is because midi.c
 * takes a SMF stream as input and some SMPTE features are defined
 * in that stream.  Thus midi.c must know when to call parse_MTC
 * and when it should call SMPTE_pause.
 */
#include "sup_os.h"
#include "sup_timer_list.h"
#include "sup_timer.h"
#include "timer_mpu401.h"
#include "sup_export.h"

static void mpu401_timeout(struct midi_timer *mt, void *id, TIMEOUT_FN fn,
    TIMEOUT_ARG arg, int t);
static void mpu401_untimeout(struct midi_timer *mt, void *id, TIMEOUT_ARG arg);
static u_long mpu401_get_clock(struct midi_timer *mt);
static void mpu401_init_clock(struct midi_timer *mt);
static void mpu401_reset(struct midi_timer *mt);
static void mpu401_free(struct midi_timer *mt);
static struct midi_timer *mpu401_dup(struct midi_timer *mt);

struct midi_timer *
midi_create_MPU401_timer(struct midi_softc *softc)
{
	struct midi_timer_mpu401 *mts;

	mts = MALLOC_NOWAIT(sizeof(struct midi_timer_mpu401));
	if (mts == NULL)
		return (NULL);
	mts->timer.softc = softc;
	mts->timer.type = MT_MPU401;
	/* our first hz guess is 120 */
	/*
	 * 100 BPM by 120 ticks per beat = 100 * 120 ticks / min
	 * = 100 * 120 / 60 ticks per sec = 200 HZ
	 */
	mts->timer.hz = 200;
	mts->timer.ref_count = 1;
	mts->timer.timeout = mpu401_timeout;
	mts->timer.untimeout = mpu401_untimeout;
	mts->timer.get_clock = mpu401_get_clock;
	mts->timer.init_clock = mpu401_init_clock;
	mts->timer.reset = mpu401_reset;
	mts->timer.dup = mpu401_dup;
	mts->timer.free = mpu401_free;
	mts->timer.create_id = midi_create_timeout_id;
	mts->timer.release_id = midi_free_timeout_id;

	mts->cntl = midi_create_timer_cntl();
	mts->clock = 0;

	return ((struct midi_timer *)mts);
}

static void
mpu401_timeout(struct midi_timer *mt, void *id, TIMEOUT_FN fn, TIMEOUT_ARG arg,
    int t)
{
	struct midi_timer_mpu401 *mts;

	mts = (struct midi_timer_mpu401 *)mt;
	midi_add_timeout(mts->cntl, (int *)id, fn, arg, t);
}

static void
mpu401_untimeout(struct midi_timer *mt, void *id, TIMEOUT_ARG arg)
{
	struct midi_timer_mpu401 *mts;

	mts = (struct midi_timer_mpu401 *)mt;
	midi_remove_timeout(mts->cntl, *(int *)id, arg);
}

static u_long
mpu401_get_clock(struct midi_timer *mt)
{
	struct midi_timer_mpu401 *mts;

	mts = (struct midi_timer_mpu401 *)mt;
	return (mts->clock);
}

static void
mpu401_init_clock(struct midi_timer *mt)
{
	struct midi_timer_mpu401 *mts;

	mts = (struct midi_timer_mpu401 *)mt;
	mts->clock = 0;
}

static void
mpu401_reset(struct midi_timer *mt)
{
	struct midi_timer_mpu401 *mts;

	mts = (struct midi_timer_mpu401 *)mt;
	midi_reset_timer_cntl(mts->cntl);
}

static struct midi_timer *
mpu401_dup(struct midi_timer *mt)
{

	mt->ref_count++;
	return (mt);
}

static void
mpu401_free(struct midi_timer *mt)
{
	struct midi_timer_mpu401 *mts;

	mts = (struct midi_timer_mpu401 *)mt;

	mts->timer.ref_count--;
	if (mts->timer.ref_count == 0) {
		/* dequeue anything already queued */
		midi_release_timer_cntl(mts->cntl);

		FREE(mts, sizeof(struct midi_timer_mpu401));
	}
}


/* Now comes all the non-timer related stuff */

/*
 * A timer tick has arrived, but for some reason we shouldn't try
 * to do any call backs, just adjust the timer
 */
void
mpu401_dropped_timer(t)
	struct midi_timer *t;
{
	struct midi_timer_mpu401 *timer;

	timer = (struct midi_timer_mpu401 *)t;

	timer->clock++;
	midi_dropped_timer(timer->cntl);
}

void
mpu401_update_timer(t)
	struct midi_timer *t;
{
	struct midi_timer_mpu401 *timer;

	timer = (struct midi_timer_mpu401 *)t;

	timer->clock++;

	/*
	 * advance the timer one tick.  This will also call any timeout
	 * functions that have come due.
	 */
	midi_kick_timer(timer->cntl);
}

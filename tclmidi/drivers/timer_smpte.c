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
#include "timer_smpte.h"
#include "sup_export.h"

static void smpte_timeout(struct midi_timer *mt, void *id, TIMEOUT_FN fn,
    TIMEOUT_ARG arg, int t);
static void smpte_untimeout(struct midi_timer *mt, void *id, TIMEOUT_ARG arg);
static u_long smpte_get_clock(struct midi_timer *mt);
static void smpte_init_clock(struct midi_timer *mt);
static void smpte_reset(struct midi_timer *mt);
static void smpte_free(struct midi_timer *mt);
static struct midi_timer *smpte_dup(struct midi_timer *mt);

static void midi_real_MTC_parse __P((struct midi_softc *, int /* u_char */));
static int midi_midi2SMPTE __P((struct event *, struct SMPTE_frame *));
static int midi_SMPTE_framerate __P((struct SMPTE_frame *, int /* u_char */));
static void midi_register_SMPTE_sync __P((struct midi_softc *,
    int /* u_char */));
static void midi_dropped_SMPTE_sync __P((TIMEOUT_ARG arg));
static u_long midi_SMPTE2timing __P((struct SMPTE_frame *));
static int midi_compare_SMPTE_time __P((struct SMPTE_frame *t1,
    struct SMPTE_frame *t2));

struct midi_timer *
midi_create_SMPTE_timer(struct midi_softc *softc)
{
	struct midi_timer_smpte *mts;

	mts = MALLOC_NOWAIT(sizeof(struct midi_timer_smpte));
	if (mts == NULL)
		return (NULL);
	mts->timer.softc = softc;
	mts->timer.type = MT_SMPTE;
	/* our first hz guess is 120 */
	mts->timer.hz = 120;
	mts->timer.ref_count = 1;
	mts->timer.timeout = smpte_timeout;
	mts->timer.untimeout = smpte_untimeout;
	mts->timer.get_clock = smpte_get_clock;
	mts->timer.init_clock = smpte_init_clock;
	mts->timer.reset = smpte_reset;
	mts->timer.dup = smpte_dup;
	mts->timer.free = smpte_free;
	mts->timer.create_id = midi_create_timeout_id;
	mts->timer.release_id = midi_free_timeout_id;

	mts->cntl = midi_create_timer_cntl();
	mts->clock = 0;
	mts->status = SMPTE_FIRST;
	BZERO(&mts->SMPTE_current, sizeof(struct SMPTE_frame));
	mts->SMPTE_current.rate = 30;
	BZERO(&mts->SMPTE_partial, sizeof(struct SMPTE_frame));
	mts->SMPTE_partial.rate = 30;
	BZERO(&mts->SMPTE_pause,   sizeof(struct SMPTE_frame));
	INIT_TIMEOUT_ID(&mts->smpte_timeout_id);

	return ((struct midi_timer *)mts);
}

static void
smpte_timeout(struct midi_timer *mt, void *id, TIMEOUT_FN fn, TIMEOUT_ARG arg,
    int t)
{
	struct midi_timer_smpte *mts;

	mts = (struct midi_timer_smpte *)mt;

	/*
	 * if timer is stopped and a timeout is scheduled with t == 0,
	 * just do it now.
	 */
	if ((mts->status & SMPTE_PAUSE) && (t == 0)) {
		fn(arg);
		return;
	}
	midi_add_timeout(mts->cntl, (int *)id, fn, arg, t);
}

static void
smpte_untimeout(struct midi_timer *mt, void *id, TIMEOUT_ARG arg)
{
	struct midi_timer_smpte *mts;

	mts = (struct midi_timer_smpte *)mt;
	midi_remove_timeout(mts->cntl, *(int *)id, arg);
}

static u_long
smpte_get_clock(struct midi_timer *mt)
{
	struct midi_timer_smpte *mts;

	mts = (struct midi_timer_smpte *)mt;

        /*
         * If the new sync time is *before* our first MetaSMPTE,
         * the easiest thing to do is resend the whole song from
         * smf 0.  Since the SMPTE_pause struct is zeroed out to
         * begin with, this handles the default case when there
         * is no MetaSMPTE event as well.  This isn't strictly 
         * correct, but it works for the case of zero MetaSMPTE
         * events and for the case of one MetaSMPTE event used
         * to indicate an "origin".  Without this, we only sync
         * properly the first time through -- the next time we
         * never see the MetaSMPTE event, we never pause, and we
         * start too early.
         */
        if (midi_compare_SMPTE_time(&mts->SMPTE_current,
	    &mts->SMPTE_pause) != 1)
        	 return 0;
        else
		return (mts->clock);
}

static void
smpte_init_clock(struct midi_timer *mt)
{
}

static void
smpte_reset(struct midi_timer *mt)
{
	struct midi_timer_smpte *mts;

	mts = (struct midi_timer_smpte *)mt;
	mts->clock = 0;
	mts->status = SMPTE_FIRST;
	BZERO(&mts->SMPTE_current, sizeof(struct SMPTE_frame));
	mts->SMPTE_current.rate = 30;
	BZERO(&mts->SMPTE_partial, sizeof(struct SMPTE_frame));
	mts->SMPTE_partial.rate = 30;
	BZERO(&mts->SMPTE_pause,   sizeof(struct SMPTE_frame));
	midi_reset_timer_cntl(mts->cntl);

	/* remove any sync check timer */
	UNTIMEOUT(&mts->smpte_timeout_id, (TIMEOUT_ARG)mts);
}

static struct midi_timer *
smpte_dup(struct midi_timer *mt)
{

	mt->ref_count++;
	return (mt);
}

static void
smpte_free(struct midi_timer *mt)
{
	struct midi_timer_smpte *mts;

	mts = (struct midi_timer_smpte *)mt;

	mts->timer.ref_count--;
	if (mts->timer.ref_count == 0) {
		/* dequeue anything already queued */
		midi_release_timer_cntl(mts->cntl);

		/* remove drop timeout */
		UNTIMEOUT(&mts->smpte_timeout_id, (TIMEOUT_ARG)mts);

		FREE(mts, sizeof(struct midi_timer_smpte));
	}
}


/* Now comes all the non-timer related stuff */

/*
 * An MTC event looks like 0xF1 0xNN.  Here code = 0xNN,
 * and this function interprets the data 0xNN as a quarter
 * frame, updating the partial frame structure with incoming
 * frame data.  We also update the current frame structure
 * with fractional frame data, and keep an eye out for loss
 * of tape sync via kernel timer.  See the MTC spec for info
 * on interpreting the way a SMPTE frame is encoded into 8
 * quarter frames of MTC.
 */
void
midi_parse_MTC_event(softc, code)
	struct midi_softc *softc;
	u_char code;
{
	struct midi_timer_smpte *timer;

	timer = (struct midi_timer_smpte *)softc->timer;

	midi_real_MTC_parse(softc, code);
	/*
	 * advance the timer one tick.  This will also call any timeout
	 * functions that have come due.
	 */
	if (timer->SMPTE_current.status & SMPTE_SYNC)
		midi_kick_timer(timer->cntl);
}

/*
 * never called anymore
 */
#if 0
void
midi_drop_MTC_event(softc, code)
	struct midi_softc *softc;
	u_char code;
{
	struct midi_timer_smpte *timer;

	timer = (struct midi_timer_smpte *)softc->timer;

	midi_real_MTC_parse(softc, code);
	/*
	 * advance the timer one tick, but this will not call any callbacks.
	 */
	if (timer->SMPTE_current.status & SMPTE_SYNC)
		midi_dropped_timer(timer->cntl);
}
#endif

void
midi_real_MTC_parse(softc, code)
	struct midi_softc *softc;
	u_char code;
{
	struct midi_timer_smpte *timer;
	struct SMPTE_frame *cf, *pf;

	timer = (struct midi_timer_smpte *)softc->timer;
	pf = &timer->SMPTE_partial;
	cf = &timer->SMPTE_current;

	switch (code & 0xf0) {
	case 0x00:
		/* Frame count LS nibble  -- first quarter frame */
		timer->status |= SMPTE_FIRST0xF1;
		pf->frame = code & 0x0f;
		cf->fraction = 25;
		break;
	case 0x10:
		/* Frame count MS nibble */
		pf->frame |= ((code & 0x01) << 4);
		cf->fraction = 50;
		/* Reschedule our sync drop every SMPTE_DROPOUT frames */
		if ((pf->frame % SMPTE_DROPCHK) == 0) {
			/*
			 * we use the normal kernel timer to check periodically
			 * and verify that we are still receiving synch
			 * messages.
			 */
			if (timer->SMPTE_current.status & SMPTE_SYNC)
				UNTIMEOUT(&timer->smpte_timeout_id,
				    (TIMEOUT_ARG)timer);
			TIMEOUT(&timer->smpte_timeout_id,
			    midi_dropped_SMPTE_sync, (TIMEOUT_ARG)timer,
			    SMPTE_DROPOUT);
		}
		break;
	case 0x20:
		/* second LS nibble */
		pf->second = code & 0x0f;
		cf->fraction = 75;
		break;
	case 0x30:
		/* second MS nibble */
		pf->second |= ((code & 0x03) << 4);
		cf->fraction = 0;
		cf->frame++;
		/* 25-frame has this problem */
		if ((cf->rate == 25) && (cf->frame == 25)) {
			cf->frame &= 0xe0;
			if (++cf->second == 60) {
				cf->second &= 0xc0;
				if (++cf->minute == 60) {
					cf->minute &= 0xc0;
					if (++cf->hour == 24)
						cf->hour &= 0xe0;
				}
			}
		}
		break;
	case 0x40:
		/* minute LS nibble */
		pf->minute = code & 0x0f;
		cf->fraction = 25;
		break;
	case 0x50:
		/* minute MS nibble */
		pf->minute |= ((code & 0x03) << 4);
		cf->fraction = 50;
		break;
	case 0x60:
		/* hour LS nibble */
		pf->hour = code & 0x0f;
		cf->fraction = 75;
		break;
	case 0x70:
		/* hour MS nibble */
		pf->hour |= ((code & 0x01) << 4);
		/*
		 * correct this two-frame bullshit here, one.
		 * that way all timing we have to deal with is
		 * clean, with no offsets at all, except for
		 * right here as we pull it off the tape.
		 */
		pf->frame += 2;
		if (pf->frame >= pf->rate) {
			pf->second++;
			pf->frame -= pf->rate;
			if (pf->second == 60) {
				pf->minute++;
				pf->second = 0;
				if (pf->minute == 60) {
					pf->hour++;
					pf->minute = 0;
					if (pf->hour == 24)
						pf->hour = 0;
				}
			}
		}

		/*
		 * This is the last of 8 quarter-frame MTC messages used
		 * to encode one SMPTE frame.  If we weren't sync'd before,
		 * we are now (assuming we caught all 8 quarter frames..)
		 */

		/* If we're not currently sync'd */
		if (!(cf->status & SMPTE_SYNC)) {
			/* and we have read all 8 frames */
			if (timer->status & SMPTE_FIRST0xF1)
				midi_register_SMPTE_sync(softc, code);
		}
		*cf = *pf;
		cf->fraction = 0;
		break;
	default:
		LOGWARN("Unknown MTC state\n");
		break;
	}

	timer->clock++;

	/*
	 * If we are paused and waiting for a specific SMPTE time
	 * before we continue playing, we should check to see if
	 * that time has arrived, and if so call midi_timeout.
	 */
	if (timer->status & SMPTE_PAUSE) {
		if (midi_compare_SMPTE_time(cf, &timer->SMPTE_pause) != -1) {
			/* the time has arrived */
			timer->status &= ~SMPTE_PAUSE;
			softc->prev_outgoing =
			    midi_SMPTE2timing(&timer->SMPTE_pause);
			midi_timeout((TIMEOUT_ARG)softc);
			return;
		}
	}
}


/*
 * SMPTE sync was just detected.. set everything up
 */
void
midi_register_SMPTE_sync(softc, code)
	struct midi_softc *softc;
	u_char code;
{
	struct event *event;
	struct SMPTE_frame *cf, *pf;
	struct midi_timer_smpte *timer;

	timer = (struct midi_timer_smpte *)softc->timer;
	pf = &timer->SMPTE_partial;
	cf = &timer->SMPTE_current;

	/* indicate synchronization */
	timer->SMPTE_current.status |= SMPTE_SYNC;
	timer->SMPTE_partial.status |= SMPTE_SYNC;

	/* set up timer to check for (future) loss of sync */
	if (!(timer->status & SMPTE_FIRST))
		UNTIMEOUT(&timer->smpte_timeout_id, (TIMEOUT_ARG)timer);
	TIMEOUT(&timer->smpte_timeout_id, midi_dropped_SMPTE_sync,
	    (TIMEOUT_ARG)timer, SMPTE_DROPOUT);

	/* get frame rate from upper bits of code */
	midi_SMPTE_framerate(pf, (code & 0x0e) >> 1);

	/* set external clock to match the received frame */
	timer->clock = midi_SMPTE2timing(pf);

	if (timer->status & SMPTE_FIRST) {
		timer->status &= ~SMPTE_FIRST;

		/* set external clock resolution based on frame rate */
		timer->timer.hz = 4 * pf->rate;

		/* We have to play from the beginning first time around */
		softc->prev_outgoing = 0;
		softc->write_smf_time = midi_get_smf_time(softc);
	} else {
		/* check clock resolution */
		if (pf->rate != cf->rate)
			LOGWARN("SMPTE frame rate changed! All bets off.\n");

		/*
		 * at this point, we've received sync already, then
		 * lost it, and then regained it.
		 */
		if (midi_compare_SMPTE_time(cf, pf) != 0) {
			/* these are no longer reliable */
			softc->premainder = 0;
			softc->rremainder = 0;

			softc->prev_outgoing = timer->clock;
			softc->write_smf_time = midi_get_smf_time(softc);
			midi_time_warp(softc);

			while (midi_deq(softc->wqueue, &event))
				stynamic_release(&event->data);
			/* XXX should we dequeue rqueue too? */
			while (midi_deq(softc->rqueue, &event))
				stynamic_release(&event->data);
			softc->status |= MIDI_RD_BLOCK;
			/* remove any events already being timed */
			softc->status &= ~MIDI_WR_BLOCK;
			if (softc->status & MIDI_WR_SLEEP) {
				softc->status &= ~MIDI_WR_SLEEP;
				softc->status |= MIDI_WR_ABORT;
				WAKEUP(&softc->sleep_write);
			}
		}
	}
#ifdef GJWDEBUG
	PRINTF("SMPTE sync achieved at %2d:%2d:%2d:%2d\n",
	    pf->hour, pf->minute, pf->second, pf->frame);
	PRINTF("Format is %u-frame", pf->rate);
	if (!(pf->status & SMPTE_FRAMEDROP))
		PRINTF(" no-");
	PRINTF("drop.\n");
#endif
}

/*
 * Convert a SMPTE frame into timing ticks.  We get 4
 * quarter-frames per frame, and we get smpte->rate frames
 * per second.  (Frame-drop formats are handled in the
 * hardware.. not my job ;-)  Two subtle points:
 *
 *   1) All SMPTE data is always two frames old, but the
 *      offset is corrected in parse_MTC_event -- as soon
 *      as it comes off the tape.  That's the only place
 *      you need to worry about it.
 *   2) Internal timing is proportional to SMPTE timing.
 *      Specifically SMPTE 00:00:00:00 is 0 ticks.
 */
u_long
midi_SMPTE2timing(smpte)
	struct SMPTE_frame *smpte;
{
	u_long ticks;
	u_long qrate;

	qrate = (u_long)(4 * smpte->rate);
	ticks = smpte->fraction / 25 + smpte->frame * 4 +
	    smpte->second *qrate +
	    smpte->minute * 60 * qrate +
	    smpte->hour * 60 * 60 * qrate;
	return (ticks);
}

int
midi_midi2SMPTE(event, tf)
	struct event *event;
	struct SMPTE_frame *tf;
{
	u_char code;

	if (event->type != SMPTE) {
		LOGWARN(
		    "SMPTE error: midi2SMPTE called with non-SMPTE event.\n");
		return (-1);
	}

	tf->status   = 0;
	tf->hour     = stynamic_get_byte(&event->data, 0) & 0x1f;
	tf->minute   = stynamic_get_byte(&event->data, 1) & 0x3f;
	tf->second   = stynamic_get_byte(&event->data, 2) & 0x3f;
	tf->frame    = stynamic_get_byte(&event->data, 3) & 0x1f;
	tf->fraction = stynamic_get_byte(&event->data, 4);

	code = (stynamic_get_byte(&event->data, 0) & 0xe0) >> 5;
	if (midi_SMPTE_framerate(tf, code) == -1) {
		LOGWARN(
		    "SMPTE error: frame rate is not encoded in hour bits.\n");
		LOGWARN("Assuming 30-frame no-drop.\n");
		tf->rate = 30;
		tf->status &= ~SMPTE_FRAMEDROP;
		return (-1);
	}
	return (0);
}

int
midi_SMPTE_framerate(tf, code)
	struct SMPTE_frame *tf;
	u_char code;
{

	/* get frame rate from code */
	switch (code) {
	case 0x00:	/* 24 frame */
		tf->rate = 24;
		tf->status &= ~SMPTE_FRAMEDROP;
		break;
	case 0x01:	/* 25 frame */
		tf->rate = 25;
		tf->status &= ~SMPTE_FRAMEDROP;
		break;
	case 0x02:	/* 30 frame drop */
		tf->rate = 30;
		tf->status |= SMPTE_FRAMEDROP;
		break;
	case 0x04:	/* 30 frame non-drop */
		tf->rate = 30;
		tf->status &= ~SMPTE_FRAMEDROP;
		break;
	default:
		return (-1);
	}
	return (0);
}

/*
 * return -1 if t1 < t2
 *         0 if t1 == t2
 *         1 if t1 > t2
 */
int
midi_compare_SMPTE_time(t1, t2)
	struct SMPTE_frame *t1;
	struct SMPTE_frame *t2;
{

	if (t1->hour < t2->hour)
		return (-1);
	else if (t1->hour > t2->hour)
		return (1);
	if (t1->minute < t2->minute)
		return (-1);
	else if (t1->minute > t2->minute)
		return (1);
	if (t1->second < t2->second)
		return (-1);
	else if (t1->second > t2->second)
		return (1);
	if (t1->frame < t2->frame)
		return (-1);
	else if (t1->frame > t2->frame)
		return (1);
	if (t1->fraction < t2->fraction)
		return (-1);
	else if (t1->fraction > t2->fraction)
		return (1);
	return (0);
}

void
midi_dropped_SMPTE_sync(arg)
	TIMEOUT_ARG arg;
{
	struct midi_timer_smpte *timer;
	struct SMPTE_frame *cf;

	timer = (struct midi_timer_smpte *)arg;
	cf = &timer->SMPTE_current;

	/*
	 * If we are here, then too many ticks went by without
	 * a SMPTE frame.  Assume that sync is lost
	 */
	timer->SMPTE_current.status &= ~SMPTE_SYNC;
	timer->SMPTE_partial.status &= ~SMPTE_SYNC;

	/*
	 * We'll need to catch another set of 8 quarter-frame
	 * messages before we can re-sync.  Make sure we get a
	 * full set, starting with the first one.
	 */
	timer->status &= ~SMPTE_FIRST0xF1;

#ifdef GJWDEBUG
	printf("SMPTE sync dropped at %2d:%2d:%2d:%2d:%2d\n",
	    cf->hour, cf->minute, cf->second, cf->frame, cf->fraction);
#endif
}

void
midi_SMPTE_pause(softc, event)
	struct midi_softc *softc;
	struct event *event;
{
	struct midi_timer_smpte *timer;

	timer = (struct midi_timer_smpte *)softc->timer;
	midi_midi2SMPTE(event, &timer->SMPTE_pause);
	timer->status |= SMPTE_PAUSE;
}

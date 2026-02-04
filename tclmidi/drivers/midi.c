/*-
 * Copyright (c) 1993, 1994, 1995 Michael B. Durian.  All rights reserved.
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

#include "sup_os.h"
#include "sup_export.h"

/* set this if you want MIDI_THRU to be on by default */
/* #define DEFAULT_THRU */


/* a table so we can map from a MidiID to a softc - used when slaving */
static struct midi_softc *MidiIDs[MAX_MIDI_DEV];
static int MidiIDCount = 0;

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif

static void midi_copy_event __P((struct event *, struct event *));
static int midi_event2smf __P((struct midi_softc *, struct event *,
    struct stynamic *, int));
static int midi_next_byte __P((struct midi_softc *, void *));
static int midi_uio2event __P((struct midi_softc *, void *, struct event *,
    int));
static int midi_fix2var __P((u_long, u_char *));
static int midi_var2fix __P((u_char *, u_long *));
static u_long midi_smf2timing_tick __P((struct midi_softc *, long));
static u_long midi_timing2smf_tick __P((struct midi_softc *, long, long));
static void midi_add_complete_event __P((struct midi_softc *));
static void midi_schedule_timeout __P((struct midi_softc *, u_long));
static void midi_thru_timeout __P((TIMEOUT_ARG arg));
static void midi_write_event __P((struct midi_softc *, struct event *, int,
    int, int));
static void midi_mask_change __P((struct midi_softc *, int));
static void midi_reset_devices __P((struct midi_softc *));
static void midi_send_sigio __P((struct midi_softc *softc));
static int midi_add_slave __P((struct midi_softc *softc, int mid));

/* unused functions
static void stynamic_copy_from __P((struct stynamic *, int, void *, int));
static void stynamic_print __P((struct stynamic *));
*/

int
midi_init_dev(softc, dev_type, type, loc)
	struct midi_softc *softc;
	long dev_type;
	MIDI_IFACE_TYPE type;
	void *loc;
{

	softc->status = MIDI_RD_BLOCK | MIDI_MASTER;
	softc->tempo_changes = NULL;

#ifdef DEFAULT_THRU
	softc->status |= MIDI_THRU;
#endif

	softc->raw_count = 0;
	softc->timed_count = 0;
	softc->oread_count = 0;
	softc->owrite_count = 0;

	softc->iface = midi_dev_probe(softc, dev_type, type, loc);
	if (softc->iface == NULL) {
		LOGERR("mpu401_probe failed\n");
		softc->status |= MIDI_UNAVAILABLE;
		return (0);
	}

	/* give us the default kernel timer to handle midi thru */
	softc->timer = midi_create_kernel_timer(softc);
	if (softc->timer == NULL) {
		LOGERR("couldn't create kernel timer\n");
		softc->iface->free(softc->iface);
		softc->status |= MIDI_UNAVAILABLE;
		return (0);
	}
	softc->timer->init_clock(softc->timer);
	/* allocate space for our timeouts */
	softc->timeout_id = softc->timer->create_id();
	if (softc->timeout_id == NULL) {
		LOGERR("couldn't create timeout id\n");
		softc->iface->free(softc->iface);
		softc->timer->free(softc->timer);
		softc->status |= MIDI_UNAVAILABLE;
		return (0);
	}
	softc->thru_timeout_id = MALLOC_NOWAIT(sizeof(TIMEOUT_ID));
	if (softc->thru_timeout_id == NULL) {
		LOGERR("couldn't create thru timeout id\n");
		softc->iface->free(softc->iface);
		softc->timer->free(softc->timer);
		softc->timer->release_id(softc->timeout_id);
		softc->status |= MIDI_UNAVAILABLE;
		return (0);
	}
	INIT_TIMEOUT_ID(softc->thru_timeout_id);

	/* assign an id */
	if (MidiIDCount >= MAX_MIDI_DEV)
		LOGWARN("Too many MIDI devices, edit sup_var.h and increase "
		    "MAX_MIDI_DEV\n");
	else {
		softc->id = MidiIDCount;
		MidiIDs[MidiIDCount++] = softc;
	}
	return (1);
}

int
gen_midiopen(dev, flags, pgid, client)
	void *dev;
	int flags, pgid;
	void *client;
{
	register struct midi_softc *softc;
	int error, raw, unit;

	unit = GET_MINOR(dev);
	raw = unit & 0x40;
	unit &= 0x3f;

	softc = UNIT_TO_SOFTC(unit, client);
	if (softc == NULL)
		return (ENXIO);

	if (softc->status & MIDI_UNAVAILABLE)
		return (EIO);

	/*
	 * must do special voodoo if already open for reading and
	 * now opening for timed use.
	 */
	if (!raw && softc->timed_count != 0)
		return (EBUSY);

	if (!raw)
		softc->pgid = pgid;

	/* check to see if this is the first open or additional */
	if (softc->raw_count + softc->timed_count == 0) {
		softc->status = MIDI_OPEN | (softc->status & MIDI_THRU);
		/* reads will block until something appears */
		softc->status |= MIDI_RD_BLOCK;
		/* we begin as our own master */
		softc->status |= MIDI_MASTER;

		/* initialize the queues */
		midi_initq(softc->rqueue);
		midi_initq(softc->wqueue);
		midi_initq(softc->thruqueue);

		stynamic_init(&softc->partial_event.event);

		/*
		 * remove any existing timer since it might be the
		 * wrong type, and replace with the default kernel timer
		 */
		if (!midi_switch_timer(softc, midi_create_kernel_timer)) {
			LOGERR("Couldn't create kernel timer\n");
			softc->status |= MIDI_UNAVAILABLE;
			return (EIO);
		}

		/* don't mask any tracks */
		softc->channel_mask = 0;

		/* make sure we are in UART mode */
		if (!midi_fullreset(softc)) {
			LOGERR("Couldn't put MPU401 into UART mode!\n");
			softc->status |= MIDI_UNAVAILABLE;
			return (EIO);
		}

		/* do anything else that's needed */
		if ((error = softc->iface->open(softc->iface)) != 0)
			return (error);
	}

	if (raw)
		softc->raw_count++;
	else
		softc->timed_count++;

	if (flags & MIDI_OREAD) {
		softc->oread_count++;
		softc->status |= MIDI_READING;
	}
	if (flags & MIDI_OWRITE) {
		softc->owrite_count++;
		softc->status |= MIDI_WRITING;
	}

	return (0);
}

int
gen_midiclose(softc, flags)
	struct midi_softc *softc;
	int flags;
{
	struct midi_softc *master_softc;
	int i, raw;

	raw = flags & MIDI_RAW_DEVICE;

	if (raw)
		softc->raw_count--;
	else
		softc->timed_count--;

	if (softc->raw_count + softc->timed_count == 0) {
		/*
		 * we're not going to finish closing until everything has
		 * been played.
		 */
		if (softc->status & MIDI_WRITING) {
			/* same as MFLUSHQ ioctl */
			if (softc->wqueue->count != 0) {
				softc->status |= MIDI_FLUSH_SLEEP;
				SLEEP_INTR(&softc->sleep_flush, "midiclose");
				softc->status &= ~MIDI_FLUSH_SLEEP;
			}
		}

		/* turn off any notes that might be stuck on */
		midi_reset_devices(softc);
		midi_fullreset(softc);

		softc->iface->close(softc->iface);

		/*
		 * remove existing timer and install kernel timer
		 * we need to install the default timer to handle midithru
		 * with the device closed.
		 */
		if (!midi_switch_timer(softc, midi_create_kernel_timer))
			LOGWARN(
			    "install ktimer failed, midithru might not work\n");

		softc->status &= ~MIDI_OPEN;

		/* remove ourself from our master's slave list */
		if (softc->status & MIDI_MASTER) {
			/* check to see if we're going to leave any slaves */
			if (softc->num_slaves > 0)
				LOGWARN(
				    "closing master that still has slaves\n");
		} else {
			/* remove ourself from master's slave list */
			master_softc = midi_id2softc(softc->master);
			if (master_softc == NULL)
				LOGWARN("invalid master id\n");
			else {
				for (i = 0; i < master_softc->num_slaves; i++)
					if (master_softc->slaves[i] ==
					    softc->id)
						break;
				for (; i < master_softc->num_slaves - 1; i++)
					master_softc->slaves[i] =
					    master_softc->slaves[i + 1];
				master_softc->num_slaves--;
			}
		}
	}

	if (flags & MIDI_OREAD) {
		if (--softc->oread_count == 0)
			softc->status &= ~MIDI_READING;
	}
	if (flags & MIDI_OWRITE) {
		if (--softc->owrite_count == 0)
			softc->status &= ~MIDI_WRITING;
	}

	return (0);
}

int
gen_midiread(softc, uio, flags)
	struct midi_softc *softc;
	void *uio;
	int flags;
{
	struct event *event;
	int error, init_len, num_to_move, raw;

	raw = flags & MIDI_RAW_DEVICE;
	init_len = UIO_REMAIN(uio);
	if (softc->rqueue->count == 0 && stynamic_len(&softc->rpartial) == 0) {
		if (flags & MIDI_NDELAY)
			return (EWOULDBLOCK);
		else {
			do {
				softc->status |= MIDI_RD_SLEEP;
				error = SLEEP_INTR(&softc->sleep_read,
				    "midiread");
				softc->status &= ~MIDI_RD_SLEEP;
			} while (error == 0 && softc->rqueue->count == 0 &&
			    stynamic_len(&softc->rpartial) == 0);
			if (error)
				return (error);
		}
	}
	while (UIO_REMAIN(uio)) {
		/*
		 * dequeue an event if partial is empty
		 */
		if (stynamic_len(&softc->rpartial) == 0) {
			if (!midi_deq(softc->rqueue, &event)) {
				softc->status |= MIDI_RD_BLOCK;
				return (0);
			}
			midi_event2smf(softc, event, &softc->rpartial, raw);
			stynamic_release(&event->data);
		}
		/* read out as much of rpartial as possible */
		num_to_move = MIN(stynamic_len(&softc->rpartial),
		    UIO_REMAIN(uio));
		if (stynamic_len(&softc->rpartial) <= STYNAMIC_SIZE) {
			if ((error = MOVE_TO_UIO(uio, softc->rpartial.datas,
			    num_to_move)) != 0)
				return (error);
		} else {
			if ((error = MOVE_TO_UIO(uio, softc->rpartial.datad,
			    num_to_move)) != 0)
				return (error);
		}
		stynamic_shift(&softc->rpartial, num_to_move);
	}
	return (0);
}

int
gen_midiwrite(softc, uio, flags)
	struct midi_softc *softc;
	void *uio;
	int flags;
{
	struct midi_softc *slave_softc;
	struct event event, *e;
	int convert, error, i, raw, s;

	raw = flags & MIDI_RAW_DEVICE;
	/* check to see if we'll block */
	if (!raw && (softc->status & MIDI_WR_BLOCK)) {
		if (flags & MIDI_NDELAY)
			return (EWOULDBLOCK);
		else {
			do {
				softc->status |= MIDI_WR_SLEEP;
				error = SLEEP_INTR(&softc->sleep_write,
				    "midiwrite");
				softc->status &= ~MIDI_WR_SLEEP;
			} while (error == 0 && softc->status & MIDI_WR_BLOCK);
			if (error != 0)
				return (error);
		}
	}
	/* if returns from sleep and should abort because of DRAIN */
	if (!raw && (softc->status & MIDI_WR_ABORT)) {
		softc->status &= ~MIDI_WR_ABORT;
		return (0);
	}

	stynamic_init(&event.data);
	while (UIO_REMAIN(uio)) {
		if (!raw && (softc->wqueue->count == MIDI_Q_SIZE)) {
			softc->status |= MIDI_WR_BLOCK;
			if (!(softc->status & MIDI_TIMEOUT_PENDING) &&
			    (softc->status & MIDI_MASTER)) {
				if (!midi_peekq(softc->wqueue, &e))
					return (0);
				midi_schedule_timeout(softc, e->time);

				/* now kick off the slaves */
				for (i = 0; i < softc->num_slaves; i++) {
					slave_softc = midi_id2softc(
					     softc->slaves[i]);
					if (slave_softc == NULL) {
						LOGWARN("bad slave id\n");
						continue;
					}
					if (!midi_peekq(slave_softc->wqueue,
					    &e))
						continue;
					midi_schedule_timeout(slave_softc,
					    e->time);
				}
			}
			if (flags & MIDI_NDELAY)
				return (0);
			else {
				do {
					softc->status |= MIDI_WR_SLEEP;
					error = SLEEP_INTR(&softc->sleep_write,
					    "midiwrite");
					softc->status &= ~MIDI_WR_SLEEP;
				} while (error == 0 &&
				    softc->wqueue->count == MIDI_Q_SIZE);
				if (error != 0)
					return (error);
			}
		}

		/*
		 * 1) get a complete event off queue
		 * 2) convert it from SMF to board format
		 * 3) deal with it
		 */
		convert = midi_uio2event(softc, uio, &event, raw);
		switch (convert) {
		case 0:
			break;
		case -1:
			/* not a complete event - we'll get it next time */
			if (!raw && !(softc->status & MIDI_TIMEOUT_PENDING) &&
			    (softc->status & MIDI_MASTER)) {
				if (!midi_peekq(softc->wqueue, &e))
					return (0);
				midi_schedule_timeout(softc, e->time);

				/* now kick off the slaves */
				for (i = 0; i < softc->num_slaves; i++) {
					slave_softc = midi_id2softc(
					     softc->slaves[i]);
					if (slave_softc == NULL) {
						LOGWARN("bad slave id\n");
						continue;
					}
					if (!midi_peekq(slave_softc->wqueue,
					    &e))
						continue;
					midi_schedule_timeout(slave_softc,
					    e->time);
				}
			}
			return (0);
		default:
			return (convert);
		}

		/* 0 delay timeout for writing the event */
		if (raw) {
			midi_enq(softc->thruqueue, &event);
			/* only set timeout for first event */
			s = BLOCK_INTR();
			if (!(softc->status & MIDI_THRU_CB_SCHED)) {
				softc->status |= MIDI_THRU_CB_SCHED;
				TIMEOUT(softc->thru_timeout_id,
				    midi_thru_timeout, (TIMEOUT_ARG)softc, 0);
			}
			UNBLOCK_INTR(s);
		} else {
			if (midi_enq(softc->wqueue, &event) == -1)
				return (EIO);
		}
		stynamic_release(&event.data);
		/* set flag so next time we cross LOW water we will SIGIO */
		if (!raw && (softc->wqueue->count >= MIDI_LOW_WATER))
			softc->status |= MIDI_SENDIO;
	}
	/*
	 * we set the flag if the last write filled the queue, but
	 * we don't need to block
	 */
	if (!raw && (softc->wqueue->count == MIDI_Q_SIZE))
		softc->status |= MIDI_WR_BLOCK;

	if (!raw && !(softc->status & MIDI_TIMEOUT_PENDING) &&
	    (softc->status & MIDI_MASTER)) {
		if (!midi_peekq(softc->wqueue, &e))
			return (0);
		midi_schedule_timeout(softc, e->time);

		/* now kick off the slaves */
		for (i = 0; i < softc->num_slaves; i++) {
			slave_softc = midi_id2softc(softc->slaves[i]);
			if (slave_softc == NULL) {
				LOGWARN("bad slave id\n");
				continue;
			}
			if (!midi_peekq(slave_softc->wqueue, &e))
				continue;
			midi_schedule_timeout(slave_softc, e->time);
		}
	}

	return (0);
}

int
gen_midiintr(softc)
	struct midi_softc *softc;
{
	int icode;
	u_char code;
	struct partial_event *pe;

	while (softc->iface->data_avail(softc->iface)) {
		/* get the data */
		icode = softc->iface->intr(softc->iface);
		if (icode == -1)
			continue;

		code = icode;

		pe = &softc->partial_event;

		/* check for realtime events */
		if ((code & 0xf8) == 0xf8) {
			switch (code) {
			case 0xfa:	/* start */
				break;
			case 0xff:	/* reset */
				/*
				 * I don't really want to do a full reset
				 * I'll just clear any event in progress
				 */
				stynamic_release(&pe->event);
				pe->state = START;
				break;
			case 0xf8:	/* timing clock */
				break;
			case 0xf9:	/* undefined */
				break;
			case 0xfb:	/* continue */
				break;
			case 0xfc:	/* stop */
				break;
			case 0xfd:	/* undefined */
				break;
			case 0xfe:	/* active sensing */
				break;
			}
			continue;
		}
	INTR_SWITCH:
		switch (pe->state) {
		case START:
			/* record when the time when the event started */
			pe->time = softc->timer->get_clock(softc->timer);
			pe->tempo = softc->rtempo;
			/* start collecting the input */
			stynamic_release(&pe->event);
			/* decide what state is next */
			if (!(code & 0x80)) {
				switch (pe->rs & 0xf0) {
				case 0x80:
				case 0x90:
				case 0xa0:
				case 0xb0:
				case 0xe0:
					/* explicitly add running state */
					stynamic_add_byte(&pe->event, pe->rs);
					stynamic_add_byte(&pe->event, code);
					/*
					 * code is the first data byte, but
					 * we still need to get the second
					 */
					pe->state = NEEDDATA2;
					break;
				case 0xc0:
				case 0xd0:
					/* explicitly add running state */
					stynamic_add_byte(&pe->event, pe->rs);
					stynamic_add_byte(&pe->event, code);
					/* code is the only data byte */
					pe->state = START;
					midi_add_complete_event(softc);
					break;
				default:
					/*
					 * I don't think running state
					 * applies to these events
					 */
					stynamic_add_byte(&pe->event, code);
					break;
				}
			} else {
				switch (code & 0xf0) {
				case 0x80:
				case 0x90:
				case 0xa0:
				case 0xb0:
				case 0xe0:
					pe->rs = code;
					stynamic_add_byte(&pe->event, code);
					pe->state = NEEDDATA1;
					break;
				case 0xc0:
				case 0xd0:
					pe->rs = code;
					stynamic_add_byte(&pe->event, code);
					pe->state = NEEDDATA2;
					break;
				default:
					switch (code) {
					case 0xf0: /* sysex */
						stynamic_add_byte(&pe->event,
						    code);
						pe->state = SYSEX;
						break;
					case 0xf1: /* MTC quarter-frame */
						pe->state = MTC;
						break;
					case 0xf4: /* undefined */
					case 0xf5: /* undefined */
					case 0xf6: /* tune request */
					case 0xf7: /* EOX (terminator) */
						/* ignore these */
						break;
					case 0xf2: /* song position */
						pe->state = SYSTEM2;
						break;
					case 0xf3: /* song select */
						pe->state = SYSTEM1;
						break;
					}
					break;
				}
			}
			break;
		case NEEDDATA1:
			stynamic_add_byte(&pe->event, code);
			pe->state = NEEDDATA2;
			break;
		case NEEDDATA2:
			stynamic_add_byte(&pe->event, code);
			pe->state = START;
			midi_add_complete_event(softc);
			break;
		case SYSEX:
			/* any non-data byte ends sysex */
			if (!(code & 0x80))
				stynamic_add_byte(&pe->event, code);
			else {
				stynamic_add_byte(&pe->event, 0xf7);
				midi_add_complete_event(softc);
				pe->state = START;
				if (code != 0xf7)
					goto INTR_SWITCH;
			}
			break;
		case SYSTEM1:
			/* throw away one data byte of a system message */
			pe->state = START;
			break;
		case SYSTEM2:
			/* throw away two data bytes of a system message */
			pe->state = SYSTEM1;
			break;
		case MTC:
			pe->state = START;
			if (softc->timer->type == MT_SMPTE) {
#if 0
				if (softc->status & MIDI_WRITING_EVENT) {
					/*
					 * protect ourselves from possible
					 * recursion as parsing the MTC
					 * event might result in a callback.
					 */
					midi_drop_MTC_event(softc, code);
				} else {
					/*
					 * safe to parse and execute possible
					 * callbacks.
					 */
					midi_parse_MTC_event(softc, code);
				}
#endif
				midi_parse_MTC_event(softc, code);
			}
			break;
		}
	}
	return (1);
}

int
gen_midiioctl(softc, cmd, data, flags)
	struct midi_softc *softc;
	int cmd;
	void *data;
	int flags;
{
	struct event *event;
	struct midi_feature feature;
	u_long ulval;
	int error, raw, val;
	u_short new_mask;

	raw = (flags & MIDI_RAW_DEVICE) ? 1 : 0;

	switch (cmd) {
	case FIOASYNC:
	case MASYNC:
		/*
		 * Linux doesn't properly process the FIOASYNC
		 * ioctl entry point, thus we have two.
		 */
		IOCTL_U_TO_K(&val, data, sizeof(int));
		if (!val)
			softc->status &= ~MIDI_ASYNC;
		else {
			softc->status |= MIDI_ASYNC;
			if (softc->wqueue->count < MIDI_LOW_WATER
			    && softc->status & MIDI_WRITING)
				SIGNAL_PG(softc->pgid, SIGIO);
		}
		break;
	case TIOCSPGRP:
		IOCTL_U_TO_K(&softc->pgid, data, sizeof(int));
		break;
	case TIOCGPGRP:
		IOCTL_K_TO_U(data, &softc->pgid, sizeof(int));
		break;
	case MRESET:
		if (!midi_fullreset(softc))
			return (EIO);
		break;
	case MSDIVISION:
		/* must recalculate play remainder */
		IOCTL_U_TO_K(&val, data, sizeof(int));
		softc->premainder = softc->premainder * val / softc->division;
		softc->division = val;
		break;
	case MGDIVISION:
		IOCTL_K_TO_U(data, &softc->division, sizeof(int));
		break;
	case MDRAIN:
		/* dequeue all everything */
		while (midi_deq(softc->rqueue, &event))
			stynamic_release(&event->data);
		while (midi_deq(softc->wqueue, &event))
			stynamic_release(&event->data);
		while (midi_deq(softc->thruqueue, &event))
			stynamic_release(&event->data);
		/* remove any events already being timed */
		softc->timer->untimeout(softc->timer, softc->timeout_id,
		    (TIMEOUT_ARG)softc);
		UNTIMEOUT(softc->thru_timeout_id, (TIMEOUT_ARG)softc);
		softc->status &= ~MIDI_WR_BLOCK;
		softc->status |= MIDI_RD_BLOCK;
		if (softc->status & MIDI_WR_SLEEP) {
			softc->status &= ~MIDI_WR_SLEEP;
			softc->status |= MIDI_WR_ABORT;
			WAKEUP(&softc->sleep_write);
		}
		if (softc->status & MIDI_FLUSH_SLEEP) {
			softc->status &= ~MIDI_FLUSH_SLEEP;
			WAKEUP(&softc->sleep_flush);
		}
		midi_reset_devices(softc);
		break;
	case MFLUSH:
		if (softc->wqueue->count != 0) {
			softc->status |= MIDI_FLUSH_SLEEP;
			SLEEP_INTR(&softc->sleep_flush, "midiflush");
			softc->status &= ~MIDI_FLUSH_SLEEP;
		}
		break;
	case MGPLAYQ:
		IOCTL_K_TO_U(data, &softc->wqueue->count, sizeof(int));
		break;
	case MGRECQ:
		IOCTL_K_TO_U(data, &softc->rqueue->count, sizeof(int));
		break;
	case MGQAVAIL:
		val = MIDI_Q_SIZE - softc->wqueue->count;
		IOCTL_K_TO_U(data, &val, sizeof(int));
		break;
	case MTHRU:
		IOCTL_U_TO_K(&val, data, sizeof(int));
		if (val)
			softc->status |= MIDI_THRU;
		else
			softc->status &= ~MIDI_THRU;
		break;
	case MRECONPLAY:
		IOCTL_U_TO_K(&val, data, sizeof(int));
		if (val)
			softc->status |= MIDI_RECONPLAY;
		else
			softc->status &= ~MIDI_RECONPLAY;
		break;
	case MGSMFTIME:
		ulval = midi_get_smf_time(softc);
		IOCTL_K_TO_U(data, &ulval, sizeof(u_long));
		break;
	case MGTHRU:
		val = (softc->status & MIDI_THRU ? 1 : 0);
		IOCTL_K_TO_U(data, &val, sizeof(int));
		break;
	case MFEATURE:
		/*
		 * I don't know if this will work correctly under BSD
		 * since the IOCTL_U_TO_K and IOCTL_K_TO_U are normal
		 * bcopy's and struct midi_feature has an unresolved
		 * data type (a void *).
		 */
		IOCTL_U_TO_K(&feature, data, sizeof(struct midi_feature));
		if ((error = softc->iface->feature(softc->iface,
		    &feature)) != 0)
			return (error);
		IOCTL_K_TO_U(data, &feature, sizeof(struct midi_feature));
		break;
	case MGETID:
		IOCTL_K_TO_U(data, &softc->id, sizeof(int));
		break;
	case MSLAVE:
		/* indenture ourselves to another MIDI device */
		IOCTL_U_TO_K(&val, data, sizeof(int));
		if ((error = midi_add_slave(softc, val)) != 0)
			return (error);
		break;
	case MGRAW:
		/* report if we are a raw device */
		IOCTL_K_TO_U(data, &raw, sizeof(int));
		break;
	case MGCMASK:
		IOCTL_K_TO_U(data, &softc->channel_mask, sizeof(short));
		break;
	case MSCMASK:
		IOCTL_U_TO_K(&new_mask, data, sizeof(short));
		midi_mask_change(softc, new_mask);
		softc->channel_mask = new_mask;
		break;
	default:
		return (ENOTTY);
	}
	return (0);
}

/*
 * gen_midiselect
 * I don't have a good way of generalizing select yet, so it is done
 * on a per machine basis.
 */

int
midi_fullreset(softc)
	struct midi_softc *softc;
{
	u_char pitch;
	struct event *event;
	struct tempo_change *tch, *nextch;

	/* dequeue all everything */
	while (midi_deq(softc->rqueue, &event))
		stynamic_release(&event->data);
	while (midi_deq(softc->wqueue, &event))
		stynamic_release(&event->data);
	while (midi_deq(softc->thruqueue, &event))
		stynamic_release(&event->data);

	/* free any storage allocated for tempo changes */
	for (tch = softc->tempo_changes; tch != NULL; tch = nextch) {
		nextch = tch->next;
		FREE(tch, sizeof(struct tempo_change));
	}
	softc->tempo_changes = NULL;

	/* remove any events already being timed */
	softc->timer->untimeout(softc->timer, softc->timeout_id,
	    (TIMEOUT_ARG)softc);
	UNTIMEOUT(softc->thru_timeout_id, (TIMEOUT_ARG)softc);

	/* mark start time */
	softc->timer->reset(softc->timer);
	softc->timer->init_clock(softc->timer);
	softc->status |= MIDI_FIRST_WRITE;
	softc->prev_incoming = softc->timer->get_clock(softc->timer);
	softc->prev_outgoing = 0;

	/* zero smf position counter */
	softc->write_smf_time = 0;

	/* reset fractional count */
	softc->premainder = 0;
	softc->rremainder = 0;

	/* initialize some variables */
	/* this is 120 bpm when a quarter note == 1 beat */
	softc->ptempo = 500000;
	softc->rtempo = 500000;
	softc->prev_rtempo = 500000;

	/* clear noteon */
	for (pitch = 0; pitch <= 0x7f; pitch++)
		softc->noteon[pitch] = 0;
	softc->noteonrs = 0;

	/* clear running play state */
	softc->writers = 0;
	softc->raw_writers = 0;
	softc->readrs = 0;

	/* reset partial event stuff */
	stynamic_release(&softc->partial_event.event);
	softc->partial_event.state = START;
	stynamic_release(&softc->rpartial);
	stynamic_release(&softc->wpartial);
	softc->wpartialpos = 0;

	/* defaults to 120 clocks per beat */
	softc->division = 120;

	softc->status &= ~MIDI_TIMEOUT_PENDING;
	softc->status |= MIDI_RD_BLOCK;
	softc->status &= ~MIDI_WR_BLOCK;
	softc->status &= ~MIDI_SENDIO;
	if (softc->status & MIDI_FLUSH_SLEEP) {
		softc->status &= ~MIDI_FLUSH_SLEEP;
		WAKEUP(&softc->sleep_flush);
	}
	if (softc->status & MIDI_WR_SLEEP) {
		softc->status &= ~MIDI_WR_SLEEP;
		WAKEUP(&softc->sleep_write);
	}

	return (softc->iface->reset(softc->iface));
}

int
midi_event2smf(softc, event, smf, raw)
	struct midi_softc *softc;
	struct event *event;
	struct stynamic *smf;
	int raw;
{
	unsigned long smf_ticks;
	long tempo;
	int tmp_len;
	u_char tmp_buf[4];

	/* convert from timing ticks to SMF ticks */
	if (!raw) {
		smf_ticks = midi_timing2smf_tick(softc, event->tempo,
		    event->time);
		tmp_len = midi_fix2var(smf_ticks, tmp_buf);
		stynamic_add_bytes(smf, tmp_buf, tmp_len);
	}

	switch (event->type) {
	case NORMAL:
		stynamic_append(smf, &event->data);
		break;
	case TEMPO:
		/* this case just won't occur */
		stynamic_copy(&event->data, (void *)&tempo, sizeof(tempo));
		stynamic_add_byte(smf, 0xff);
		stynamic_add_byte(smf, 0x51);
		stynamic_add_byte(smf, 0x03);
		stynamic_add_byte(smf, (tempo & 0xff0000) >> 16);
		stynamic_add_byte(smf, (tempo & 0xff00) >> 8);
		stynamic_add_byte(smf, tempo & 0xff);
		break;
	case SYSX:
		stynamic_add_byte(smf, 0xf0);
		/* skip over the leading 0xf0 */
		stynamic_shift(&event->data, 1);
		tmp_len = midi_fix2var(stynamic_len(&event->data), tmp_buf);
		stynamic_add_bytes(smf, tmp_buf, tmp_len);
		stynamic_append(smf, &event->data);
		break;
	case TIMESIG:
	case SMPTE:
	case NOP:
		/* These cases will never appear as input */
		break;
	}
	return (1);
}

int
midi_next_byte(softc, uio)
	struct midi_softc *softc;
	void *uio;
{
	int byte;

	/* if we're not at the end of a partial event, read from it */
	if (softc->wpartialpos < stynamic_len(&softc->wpartial)) {
		byte = stynamic_get_byte(&softc->wpartial,
		    softc->wpartialpos++);
		return (byte);
	} else {
		/* read from uio and copy uio onto partial event */
		if ((byte = UWRITEC(uio)) == -1) {
			/*
			 * reset partialpos so next time through
			 * we'll read from the partial event if
			 * it is non-zero in length
			 */
			softc->wpartialpos = 0;
			return (-1);
		}
		stynamic_add_byte(&softc->wpartial, byte);
		softc->wpartialpos = stynamic_len(&softc->wpartial);
		return (byte);
	}
}

int
midi_uio2event(softc, uio, event, raw)
	struct midi_softc *softc;
	void *uio;
	struct event *event;
	int raw;
{
	u_long smf_ticks, ulen;
	long tempo;
	int byte, extra_byte, i, len, num_data_bytes;
	int rs_change;
	u_char meta_type, *rs, tmp_buf[256];

	if (raw) {
		event->time = 0;
		event->smf_time = 0;
		rs = &softc->raw_writers;
	} else {
		rs = &softc->writers;
		/* copy in timing portion */
		len = 0;
		do {
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			tmp_buf[len++] = byte;
		} while (byte & 0x80);

		/* compute time in smf ticks */
		midi_var2fix(tmp_buf, &smf_ticks);

		/* now convert from smf to timing */
		event->time = midi_smf2timing_tick(softc, smf_ticks);
		softc->write_smf_time += smf_ticks;
		event->smf_time = softc->write_smf_time;
	}

	/* get first byte of event data */
	if ((byte = midi_next_byte(softc, uio)) == -1) {
		stynamic_release(&event->data);
		return (-1);
	}
	switch (byte) {
	case 0xf0:
		/* basic sysex type */
		event->type = SYSX;
		len = 0;
		do {
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			tmp_buf[len++] = byte;
		} while (byte & 0x80);
		midi_var2fix(tmp_buf, &ulen);
		stynamic_add_byte(&event->data, 0xf0);
		for (; ulen > 0; ulen--) {
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			stynamic_add_byte(&event->data, byte);
		}
		break;
	case 0xf7:
		/* continued sysex type */
		event->type = SYSX;
		len = 0;
		do {
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			tmp_buf[len++] = byte;
		} while (byte & 0x80);
		midi_var2fix(tmp_buf, &ulen);
		for (; ulen > 0; ulen--) {
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			stynamic_add_byte(&event->data, byte);
		}
		break;
	case 0xff:
		/* meta events */
		if ((byte = midi_next_byte(softc, uio)) == -1) {
			stynamic_release(&event->data);
			return (-1);
		}
		meta_type = byte;
		/* get length of meta data */
		len = 0;
		do {
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			tmp_buf[len++] = byte;
		} while (byte & 0x80);
		midi_var2fix(tmp_buf, &ulen);
		/* read it in  - meta events are not over 256 in size */
		for (i = 0; i < ulen; i++) {
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			tmp_buf[i] = byte;
		}
		switch (meta_type) {
		default:
			/*
			 * we'll skip these events, but we need to
			 * save the timing info
			 */
			event->type = NOP;
			break;
		case 0x51:
			/* tempo event */
			event->type = TEMPO;
			tempo = 0;
			for (i = 0; i < 3; i++) {
				tempo = tempo << 8;
				tempo |= tmp_buf[i];
			}
			stynamic_add_bytes(&event->data, (u_char *)&tempo,
			    sizeof(tempo));
			/*
			 * change ptempo now, rtempo will change when
			 * the event's time comes up
			 */
			softc->ptempo = tempo;
			break;
		case 0x54:
			/* MetaSMPTE */
			event->type = SMPTE;
			stynamic_add_bytes(&event->data, tmp_buf, 5);

			/*
			 * MetaSMPTE must come before any non-zero delta
			 * events.  Therefore let MetaSMPTE *define* the
			 * SMF zero point
			 */
			softc->write_smf_time = 9;
		}
		break;
	default:
		if ((byte & 0xf0) == 0x80) {
#if 0
			u_char rs_chan, rs_type;
#endif

			/* check note off events separately */
			tmp_buf[0] = byte;
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			tmp_buf[1] = byte;
			if ((byte = midi_next_byte(softc, uio)) == -1) {
				stynamic_release(&event->data);
				return (-1);
			}
			tmp_buf[2] = byte;
			len = 3;
#if 0
			/*
			 * check to see if we can collapse and use
			 * running state
			 */
			rs_type = *rs & 0xf0;
			rs_chan = *rs & 0x0f;
			/*
			 * We are no longer using running status, as
			 * it causes trouble when MIDI_THRU is on.
			 */
			/*
			 * collapse to running state if time is 0
			 * and the running state is the same
			 * or the running state is note on for the
			 * same channel and the note off velocity is
			 * zero.
			 */
			if (event->time == 0 && (*rs == tmp_buf[0]
			    || (tmp_buf[2] == 0 && rs_type == 0x90
			    && rs_chan == (tmp_buf[0] & 0x0f)))) {
				tmp_buf[0] = tmp_buf[1];
				tmp_buf[1] = tmp_buf[2];
				len = 2;
			} else {
				*rs = tmp_buf[0];
			}
#else
			*rs = tmp_buf[0];
#endif
		} else {
			extra_byte = 0;
			rs_change = 0;
			if ((byte & 0x80) && (byte != *rs)) {
				*rs = byte;
				rs_change = 1;
			}
			len = 0;
#if 0
			if (event->time != 0 || rs_change) {
				/*
				 * stick in a mode byte if time is non-zero
				 * This is so we don't confuse hardware that
				 * is turned on while we're playing
				 * also add it if the running state changes
				 */
				tmp_buf[0] = *rs;
				len = 1;
			}
#else
			tmp_buf[0] = *rs;
			len = 1;
#endif
			if (byte & 0x80)
				extra_byte = 1;
			else {
				tmp_buf[len] = byte;
				len++;
			}

			switch (*rs & 0xf0) {
			case 0x80:
			case 0x90:
			case 0xa0:
			case 0xb0:
			case 0xe0:
				num_data_bytes = 1;
				break;
			default:
				num_data_bytes = 0;
			}
			for (i = 0; i < num_data_bytes + extra_byte; i++) {
				if ((byte = midi_next_byte(softc, uio)) == -1) {
					stynamic_release(&event->data);
					return (-1);
				}
				tmp_buf[len++] = byte;
			}
		}
		event->type = NORMAL;
		stynamic_add_bytes(&event->data, tmp_buf, len);
	}

	stynamic_release(&softc->wpartial);
	softc->wpartialpos = 0;
	return (0);
}

int
midi_fix2var(fix, var)
	u_long fix;
	u_char *var;
{
	int i;
	unsigned char buf[4], *bptr;

	buf[0] = buf[1] = buf[2] = buf[3] = 0;
	bptr = buf;
	*bptr++ = fix & 0x7f;
	while ((fix >>= 7) > 0) {
		*bptr |= 0x80;
		*bptr++ += (fix & 0x7f);
        }

	i = 0;
	do {
		*var++ = *--bptr;
		i++;
	} while (bptr != buf);

	return (i);
}

int
midi_var2fix(var, fix)
	u_char *var;
	u_long *fix;
{
	int delta;

	*fix = 0;
	delta = 0;
	do {
		*fix = (*fix << 7) + (*var & 0x7f);
		delta++;
	} while (*var++ & 0x80);

	return (delta);
}

u_long
midi_smf2timing_tick(softc, smf)
	struct midi_softc *softc;
	long smf;
{
	long long denominator, numerator;
	u_long timing;

	numerator = (long long)softc->ptempo * softc->timer->hz * smf
	    + softc->premainder;
	denominator = 1000000 * (long long)softc->division;
	timing = numerator / denominator;
	softc->premainder = numerator % denominator;
	return (timing);
}

u_long
midi_timing2smf_tick(softc, tempo, timing)
	struct midi_softc *softc;
	long tempo, timing;
{
	long long numerator, denominator;
	u_long smf;

	if (softc->prev_rtempo != tempo) {
		/*
		 * also update the rremainder to reflect tempo
		 * change
		 */
		softc->rremainder = softc->rremainder * tempo
		    / softc->prev_rtempo;
		softc->prev_rtempo = tempo;
	}
	numerator = (long long)softc->division * 1000000 * timing
	    + softc->rremainder;
	denominator = (long long)tempo * softc->timer->hz;
	smf = numerator / denominator;
	softc->rremainder = numerator % denominator;
	return (smf);
}

void
midi_add_complete_event(softc)
	struct midi_softc *softc;
{
	struct event event;
	struct partial_event *pe;
	long ticks;

	stynamic_init(&event.data);
	pe = &softc->partial_event;

	ticks = pe->time - softc->prev_incoming;
	if (ticks < 0) {
		ticks = 0;
		pe->time = softc->prev_incoming;
	}
	event.time = ticks;
	event.tempo = pe->tempo;
	switch (stynamic_get_byte(&pe->event, 0)) {
	case 0xf0:
		/* sysex */
		event.type = SYSX;
		break;
	default:
		event.type = NORMAL;
		break;
	}
	stynamic_append(&event.data, &pe->event);
	if (softc->status & MIDI_THRU) {
		/* use a 0 delay timeout to write the event */
		midi_enq(softc->thruqueue, &event);
		/* only set timer for first event */
		if (!(softc->status & MIDI_THRU_CB_SCHED)) {
			softc->status |= MIDI_THRU_CB_SCHED;
			TIMEOUT(softc->thru_timeout_id, midi_thru_timeout,
			    (TIMEOUT_ARG)softc, 0);
		}
	}
	/* enqueue new event */
	if (softc->status & MIDI_READING)
		midi_enq(softc->rqueue, &event);
	stynamic_release(&event.data);
	/* readjust previous event time */
	softc->prev_incoming = pe->time;

	if (!(softc->status & MIDI_READING))
		return;
	softc->status &= ~MIDI_RD_BLOCK;
	if (softc->status & MIDI_RD_SLEEP) {
		softc->status &= ~MIDI_RD_SLEEP;
		WAKEUP(&softc->sleep_read);
	}
	if (softc->status & MIDI_ASYNC)
		SIGNAL_PG(softc->pgid, SIGIO);
	if (softc->status & MIDI_RSEL) {
		softc->status &= ~MIDI_RSEL;
		SELWAKEUP(SEL_RCHAN(softc));
	}

	return;
}

void
midi_schedule_timeout(softc, t)
	struct midi_softc *softc;
	u_long t;
{

	if (softc->status & MIDI_FIRST_WRITE) {
		softc->timer->init_clock(softc->timer);
		softc->status &= ~MIDI_FIRST_WRITE;
	}

	/* clear record timer? */
	if (softc->status & MIDI_RECONPLAY) {
		softc->prev_incoming = softc->timer->get_clock(softc->timer);
		softc->status &= ~MIDI_RECONPLAY;
	}

	softc->status |= MIDI_TIMEOUT_PENDING;
	softc->timer->timeout(softc->timer, softc->timeout_id,
	    midi_timeout, (TIMEOUT_ARG)softc, t);
}

void
midi_timeout(arg)
	TIMEOUT_ARG arg;
{
	struct event *event;
	struct midi_softc *softc = (struct midi_softc *)arg;
	u_long sched_time, t;
	int quiet;

	softc->status &= ~MIDI_TIMEOUT_PENDING;
	if (!midi_peekq(softc->wqueue, &event))
		return;
	while ((softc->prev_outgoing + event->time) <=
	    (t = softc->timer->get_clock(softc->timer))) {
		midi_deq(softc->wqueue, NULL);

		/*
		 * If the event is backlogged for more than BACKLOG_TIME ticks,
		 * don't play it
		 */
		if ((t - softc->prev_outgoing - event->time) > BACKLOG_TIME)
			quiet = 1;
		else
			quiet = 0;

		/*
		 * if we have a SMPTE meta event, we must wait for that
		 * time to roll around.  midi_SMPTE_pause will set up
		 * the SMPTE handling code to wait until the time
		 * comes and then call midi_timeout again.
		 */
		if (event->type == SMPTE && softc->timer->type == MT_SMPTE) {
			midi_SMPTE_pause(softc, event);
			stynamic_release(&event->data);
			return;
		}

		midi_write_event(softc, event, quiet, 1, 1);
		softc->prev_outgoing += event->time;
		stynamic_release(&event->data);
		if (!midi_peekq(softc->wqueue, &event))
			return;
	}
	sched_time = event->time - (t - softc->prev_outgoing);
	midi_schedule_timeout(softc, sched_time);
	return;
}

void
midi_thru_timeout(arg)
	TIMEOUT_ARG arg;
{
	struct midi_softc *softc = (struct midi_softc *)arg;
	struct event *e;

	while (midi_deq(softc->thruqueue, &e)) {
		midi_write_event(softc, e, 0, 0, 0);
		stynamic_release(&e->data);
	}
	softc->status &= ~MIDI_THRU_CB_SCHED;
}


void
midi_write_event(softc, event, quiet, notify, mask)
	struct midi_softc *softc;
	struct event *event;
	int quiet;
	int notify;
	int mask;
{
	struct tempo_change *tchange, *prev_tchange;
	u_char bytes[4], channel, command, *dataptr;

	switch (event->type) {
	case NOP:
		break;
	case SMPTE:
		break;
	case TEMPO:
		stynamic_copy(&event->data, &softc->rtempo,
		    sizeof(softc->rtempo));
		for (tchange = softc->tempo_changes, prev_tchange = NULL;
		    tchange != NULL;
		    prev_tchange = tchange, tchange = tchange->next) {
			if (tchange->smf_time < event->smf_time)
				break;
		}
		if (tchange == NULL) {
			/* add the tempo change to our list */
			tchange = MALLOC_NOWAIT(sizeof(struct tempo_change));
			if (tchange == NULL) {
				LOGERR("Out of kernel memory: we're screwed\n");
				break;
			} else {
				tchange->time = event->time +
				    softc->prev_outgoing;
				tchange->smf_time = event->smf_time;
				tchange->tempo = softc->rtempo;
				tchange->next = NULL;
				/*
				 * Don't update through a null pointer.
				 * On the other hand, make sure to update
				 * softc->tempo_changes to reflect storage.
				 */
				if (prev_tchange != NULL)
					prev_tchange->next = tchange;
				else
					softc->tempo_changes = tchange;
			}
		}
		break;
	case NORMAL:
		/*
		 * fourth byte might not be valid, but who cares,
		 * we're only reading and in the kernel.  We'll
		 * ignore it if it isn't.
		 */
		stynamic_copy(&event->data, &bytes, 4);
		if (!(bytes[0] & 0x80))
			dataptr = &bytes[0];
		else {
			softc->noteonrs = bytes[0];
			dataptr = &bytes[1];
		}
		command = softc->noteonrs & 0xf0;
		channel = softc->noteonrs & 0x0f;

		/* don't send masked tracks */
		if (mask && (softc->channel_mask & (1 << channel)))
			quiet = 1;
		/*
		 * In `quiet' mode, don't turn on any new notes but do
		 * turn them off if we're asked.
		 */
		if (command == 0x90) {
			/*
			 * set or clear appropriate bit in noteon
			 * array depending on velocity value
			 */
			if (dataptr[1] != 0)
				softc->noteon[dataptr[0]] |= 1 << channel;
			else {
				softc->noteon[dataptr[0]] &= ~(1 << channel);
				/* never mask note off events */
				quiet = 0;
			}
		}
		if (command == 0x80) {
			/* clear bit */
			softc->noteon[dataptr[0]] &= ~(1 << channel);
			/* never mask note off events */
			quiet = 0;
		}
		/* FALLTHRU */
	default:
		if (!quiet)
			softc->iface->write(softc->iface, event);
		break;
	}

	if (notify)
		midi_send_sigio(softc);

	if ((softc->status & MIDI_FLUSH_SLEEP) && softc->wqueue->count == 0) {
		softc->status &= ~MIDI_FLUSH_SLEEP;
		WAKEUP(&softc->sleep_flush);
	}
}

void
midi_mask_change(softc, new_mask)
	struct midi_softc *softc;
	int new_mask;
{
	struct event event;
	int delta, s;
	u_char channel, pitch;

	stynamic_init(&event.data);
	delta = softc->channel_mask ^ new_mask;
	for (channel = 0; channel <= 16;  channel++) {
		/*
		 * If the bit has changed, and is now set (ie masked),
		 * then turn off all NoteOns
		 */
		if ((delta & (1 << channel)) && (new_mask & (1 << channel))) {
			for (pitch = 0; pitch <= 0x7f; pitch++) {
				if (softc->noteon[pitch] & (1 << channel)) {
					/*
					 * use a 0 delay timeout to
					 * write the event
					 */
					event.type = NORMAL;
					stynamic_add_byte(&event.data,
					    channel | 0x90);
					stynamic_add_byte(&event.data, pitch);
					stynamic_add_byte(&event.data, 0);
					midi_enq(softc->thruqueue, &event);
					stynamic_release(&event.data);
					/* only set timer for first event */
					s = BLOCK_INTR();
					if (!(softc->status &
					    MIDI_THRU_CB_SCHED)) {
						softc->status |=
						    MIDI_THRU_CB_SCHED;
						TIMEOUT(softc->thru_timeout_id,
						    midi_thru_timeout,
						    (TIMEOUT_ARG)softc, 0);
					}
					UNBLOCK_INTR(s);
					softc->noteon[pitch] &= ~(1 << channel);
				}
			}
		}
	}
}


/*
 * try to reset the midi devices as best we can
 */
void
midi_reset_devices(softc)
	struct midi_softc *softc;
{
	u_char channel, pitch;
	struct event event;

	/* manual note off calls - turn off any note that is on */
	stynamic_init(&event.data);
	for (pitch = 0; pitch <= 0x7f; pitch++) {
		for (channel = 0; channel <= 0x0f; channel++) {
			if (softc->noteon[pitch] & (1 << channel)) {
				event.type = NORMAL;
				stynamic_add_byte(&event.data, channel | 0x90);
				stynamic_add_byte(&event.data, pitch);
				stynamic_add_byte(&event.data, 0);
#if 0
				softc->status |= MIDI_WRITING_EVENT;
#endif
				softc->iface->write(softc->iface, &event);
#if 0
				softc->status &= ~MIDI_WRITING_EVENT;
#endif
				stynamic_release(&event.data);
			}
		}
		softc->noteon[pitch] = 0;
	}
	for (channel = 0; channel <= 0x0f; channel++) {
		/*
		 * send paramter event for all notes off for redundancy
		 * some older synths don't support this
		 */
		event.type = NORMAL;
		stynamic_add_byte(&event.data, channel | 0xb0);
		stynamic_add_byte(&event.data, 0x7b);
		stynamic_add_byte(&event.data, 0);
#if 0
		softc->status |= MIDI_WRITING_EVENT;
#endif
		softc->iface->write(softc->iface, &event);
#if 0
		softc->status &= ~MIDI_WRITING_EVENT;
#endif
		stynamic_release(&event.data);

		/* modulation controller to zero */
		event.type = NORMAL;
		stynamic_add_byte(&event.data, 0x01);
		stynamic_add_byte(&event.data, 0);
#if 0
		softc->status |= MIDI_WRITING_EVENT;
#endif
		softc->iface->write(softc->iface, &event);
#if 0
		softc->status &= ~MIDI_WRITING_EVENT;
#endif
		stynamic_release(&event.data);

		/* reset all controllers */
		event.type = NORMAL;
		stynamic_add_byte(&event.data, 0x79);
		stynamic_add_byte(&event.data, 0);
#if 0
		softc->status |= MIDI_WRITING_EVENT;
#endif
		softc->iface->write(softc->iface, &event);
#if 0
		softc->status &= ~MIDI_WRITING_EVENT;
#endif
		stynamic_release(&event.data);

		/* lift sustain pedal */
		event.type = NORMAL;
		stynamic_add_byte(&event.data, 0x40);
		stynamic_add_byte(&event.data, 0);
#if 0
		softc->status |= MIDI_WRITING_EVENT;
#endif
		softc->iface->write(softc->iface, &event);
#if 0
		softc->status &= ~MIDI_WRITING_EVENT;
#endif
		stynamic_release(&event.data);

		/* center pitch wheel */
		event.type = NORMAL;
		stynamic_add_byte(&event.data, 0xe0 | channel);
		stynamic_add_byte(&event.data, 0);
		stynamic_add_byte(&event.data, 0x40);
#if 0
		softc->status |= MIDI_WRITING_EVENT;
#endif
		softc->iface->write(softc->iface, &event);
#if 0
		softc->status &= ~MIDI_WRITING_EVENT;
#endif
		stynamic_release(&event.data);
	}
	softc->noteonrs = 0;
}

/*
 * checks to see if we should send a signal and sends it if we
 * should.
 */
void
midi_send_sigio(softc)
	struct midi_softc *softc;
{

	if (softc->wqueue->count < MIDI_LOW_WATER) {
		softc->status &= ~MIDI_WR_BLOCK;
		if (softc->status & MIDI_WR_SLEEP) {
			softc->status &= ~MIDI_WR_SLEEP;
			WAKEUP(&softc->sleep_write);
		}
		if (softc->status & MIDI_ASYNC &&
		    (softc->status & MIDI_SENDIO || softc->wqueue->count
		    == 0)) {
			SIGNAL_PG(softc->pgid, SIGIO);
		}
		softc->status &= ~MIDI_SENDIO;
		/* notify select that writes will succeed */
		if (softc->status & MIDI_WSEL) {
			softc->status &= ~MIDI_WSEL;
			SELWAKEUP(SEL_WCHAN(softc));
		}
	}
}

void
midi_time_warp(softc)
	struct midi_softc *softc;
{

	/* turn off any notes that may be on */
	midi_reset_devices(softc);

	if (softc->status & MIDI_ASYNC) {
		softc->status |= MIDI_TIME_WARP;
		SIGNAL_PG(softc->pgid, SIGURG);
	}
	if (softc->status & MIDI_ESEL) {
		softc->status &= ~MIDI_ESEL;
		SELWAKEUP(SEL_ECHAN(softc));
	}
}

u_long
midi_get_smf_time(softc)
	struct midi_softc *softc;
{
	long long numerator, denominator;
	u_long smf, tc, tdiff, tempo;
	struct tempo_change *prev, *tchange;

	tc = softc->timer->get_clock(softc->timer);

	/* find last tempo change */
	for (tchange = softc->tempo_changes, prev = NULL;
	    tchange != NULL;
	    prev = tchange, tchange = tchange->next) {
		if (tc <= tchange->time)
			break;
	}
	if (prev == NULL) {
		/*
		 * no tempos found -- default tempo for MIDI files is
		 * 120 bpm.
		 */
		tempo = 500000;
		tdiff = tc;
		smf = 0;
	} else {
		tempo = prev->tempo;
		tdiff = tc - prev->time;
		smf = prev->smf_time;
	}

	numerator = (long long)softc->division * 1000000 * tdiff;
	denominator = (long long)tempo * softc->timer->hz;
	smf += numerator / denominator;
	return (smf);
}

struct midi_softc *
midi_id2softc(id)
	int id;
{

	if (id < 0 || id >= MAX_MIDI_DEV || id > MidiIDCount)
		return (NULL);
	return (MidiIDs[id]);
}

static int
midi_add_slave(softc, mid)
	struct midi_softc *softc;
	int mid;
{
	struct midi_softc *master_softc;
	struct event *e;

	master_softc = midi_id2softc(mid);
	if (master_softc == NULL)
		return (EINVAL);
	if (master_softc->num_slaves == MAX_MIDI_DEV)
		return (EINVAL);
	/* see if we are already a slave */
	if (!(softc->status & MIDI_MASTER))
		return (ENOTTY);
	master_softc->slaves[master_softc->num_slaves++] = softc->id;
	softc->status &= ~MIDI_MASTER;

	/* duplicate master's timer */
	softc->timer->free(softc->timer);
	softc->timer = master_softc->timer->dup(master_softc->timer);

	softc->master = mid;

	/* if we don't have a timeout pending, we should schedule one */
	if (!(softc->status & MIDI_TIMEOUT_PENDING)) {
		if (midi_peekq(softc->wqueue, &e))
			midi_schedule_timeout(softc, e->time);
	}
	return (0);
}

void
midi_initq(eq)
	struct event_queue *eq;
{

	eq->count = 0;
	eq->end = &eq->events[MIDI_Q_SIZE - 1];
	eq->head = eq->events;
	eq->tail = eq->events;
	/* zero events to clear stynamic stuctures */
	BZERO(eq->events, MIDI_Q_SIZE * sizeof(struct event));
}

void
midi_copy_event(e1, e2)
	struct event *e1, *e2;
{

	e1->time = e2->time;
	e1->smf_time = e2->smf_time;
	e1->type = e2->type;
	e1->tempo = e2->tempo;
	stynamic_release(&e1->data);
	stynamic_append(&e1->data, &e2->data);
}

int
midi_deq(eq, event)
	struct event_queue *eq;
	struct event **event;
{
	int s;

	s = BLOCK_INTR();
	if (eq->count == 0) {
		UNBLOCK_INTR(s);
		return (0);
	}
	if (event == NULL)
		eq->head++;
	else
		*event = eq->head++;
	if (eq->head > eq->end)
		eq->head = eq->events;
	eq->count--;
	UNBLOCK_INTR(s);
	return (1);
}

int
midi_peekq(eq, event)
	struct event_queue *eq;
	struct event **event;
{
	int s;

	s = BLOCK_INTR();
	if (eq->count == 0) {
		UNBLOCK_INTR(s);
		return (0);
	}
	*event = eq->head;
	UNBLOCK_INTR(s);
	return (1);
}

int
midi_browseq(eq, offset, event)
	struct event_queue *eq;
	u_long offset;
	struct event **event;
{
	int s;

	s = BLOCK_INTR();
	if (eq->count <= offset) {
		UNBLOCK_INTR(s);
		return (0);
	}
	*event = (eq->head + offset);
	UNBLOCK_INTR(s);
	return (1);
}

int
midi_enq(eq, event)
	struct event_queue *eq;
	struct event *event;
{
	int s;

	s = BLOCK_INTR();
	if (eq->count == MIDI_Q_SIZE) {
		UNBLOCK_INTR(s);
		return (0);
	}
	midi_copy_event(eq->tail++, event);
	if (eq->tail > eq->end)
		eq->tail = eq->events;
	eq->count++;
	UNBLOCK_INTR(s);
	return (1);
}

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

#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_MPU401) \
    || defined(MIDI_BUILD_MPU401_UART) || defined(MIDI_BUILD_MQX32)

#include "sup_os.h"
#include "sup_iface.h"
#include "iface_mpu401.h"
#include "sup_timer.h"
#include "sup_export.h"

struct midi_iface *
mpu401_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type, void *vloc)
{
	struct midi_iface_mpu401 *iface;
	struct midi_isa_iface *loc;

	if (type != MIDI_ISA_IFACE)
		return (NULL);
	loc = (struct midi_isa_iface *)vloc;

	iface = MALLOC_NOWAIT(sizeof(struct midi_iface_mpu401));
	if (iface == NULL)
		return (NULL);
	iface->iface.loc = MALLOC_NOWAIT(sizeof(struct midi_isa_iface));
	if (iface->iface.loc == NULL) {
		FREE(iface, sizeof(struct midi_iface_mpu401));
		return (NULL);
	}

	/* copy location */
	iface->iface.type = type;
	*(struct midi_isa_iface *)iface->iface.loc = *loc;

	iface->status = 0;
	iface->iface.softc = softc;
	iface->iface.name = mpu401_name;
	iface->iface.size = mpu401_size;
	iface->iface.gen_intr = mpu401_gen_intr;
	iface->iface.reset = mpu401_big_reset;
	iface->iface.open = mpu401_open;
	iface->iface.close = mpu401_close;
	iface->iface.intr = mpu401_intr;
	iface->iface.data_avail = mpu401_data_avail;
	iface->iface.write = mpu401_write;
	iface->iface.read = mpu401_read;
	iface->iface.feature = mpu401_feature;
	iface->iface.free = mpu401_free;

	if (!mpu401_reset(iface)) {
		if (!mpu401_reset(iface)) {
			mpu401_free((struct midi_iface *)iface);
			return (NULL);
		}
	}
	return ((struct midi_iface *)iface);
}

const char *
mpu401_name(struct midi_iface *mif)
{

	return ("MPU401");
}

void
mpu401_size(struct midi_iface *mif, void *size)
{

	*(int *)size = 2;
}

void
mpu401_gen_intr(struct midi_iface *mif)
{
	struct midi_iface_mpu401 *iface;

	iface = (struct midi_iface_mpu401 *)mif;

	if (!mpu401_reset(iface))
		mpu401_reset(iface);
}

int
mpu401_big_reset(struct midi_iface *mif)
{

	return (1);
}

int
mpu401_open(struct midi_iface *mif)
{
	struct midi_iface_mpu401 *iface;

	iface = (struct midi_iface_mpu401 *)mif;
	if (!mpu401_reset(iface))
		if (!mpu401_reset(iface))
			return (EIO);
	if (!mpu401_uart(iface))
		return (EIO);
	return (0);
}

void
mpu401_close(struct midi_iface *mif)
{
	struct midi_iface_mpu401 *iface;

	iface = (struct midi_iface_mpu401 *)mif;
	(void)mpu401_reset(iface);
	(void)mpu401_uart(iface);

	/* change the timer */
	if (!midi_switch_timer(mif->softc, midi_create_kernel_timer))
		LOGERR("couldn't return to kernel_timer\n");
}

int
mpu401_intr(struct midi_iface *mif)
{
	struct midi_iface_mpu401 *iface;
	struct midi_isa_iface *loc;
	u_char code;

	iface = (struct midi_iface_mpu401 *)mif;

	if ((iface->status & MPU401_NEED_DATA) &&
	    !(iface->status & MPU401_NEED_ACK))
		return (-1);
	loc = (struct midi_isa_iface *)mif->loc;
	code = INB(loc->io_port + MPU401_DATA);
	if (code == MPU401_ACK && (iface->status & MPU401_NEED_ACK)) {
		iface->status &= ~MPU401_NEED_ACK;
		return (-1);
	}

	if (mif->softc != NULL && mif->softc->timer != NULL &&
	    mif->softc->timer->type == MT_MPU401) {
		switch (code) {
		case 0xff:
			/*
			 * MCC system message identifier
			 * Sysex message come with this prefix - dump it.
			 */
			return (-1);
		case MPU401_CLOCK_TO_PC:
			/* our timing identifier */
			/* don't process until we've finished any commands */
#if 0
			if (iface->status & MPU401_PROCESSING_COMMAND)
				mpu401_dropped_timer(mif->softc->timer);
			else
				mpu401_update_timer(mif->softc->timer);
#endif
			mpu401_update_timer(mif->softc->timer);
			return (-1);
		default:
			;
		}
	}
	return (code);
}

int
mpu401_data_avail(struct midi_iface *mif)
{
	struct midi_isa_iface *loc;
	int flag;

	loc = (struct midi_isa_iface *)mif->loc;
	flag = INB(loc->io_port + MPU401_STATUS);
	return ((flag & MPU401_DATA_AVL) == 0);
}

void
mpu401_write(struct midi_iface *mif, struct event *event)
{
	struct midi_iface_mpu401 *iface;
	struct midi_isa_iface *loc;
	int i;
	u_char byte;

	iface = (struct midi_iface_mpu401 *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	if (mif->softc->timer->type == MT_MPU401) {
		/* we need to issue special commands to send events */
		switch (event->type) {
		case NORMAL:
			if (!mpu401_send_command(iface,
			    MPU401_REQ_SEND_MESSAGE))
				return;
			break;
		case SYSX:
			if (!mpu401_send_command(iface,
			    MPU401_REQ_SEND_SYS_MESSAGE))
				return;
			break;
		default:
			return;
		}
	}

	for (i = 0; i < stynamic_len(&event->data); i++) {
		byte = stynamic_get_byte(&event->data, i);
		mpu401_wait_rdy_rcv(iface);
		OUTB(loc->io_port + MPU401_DATA, byte);
	}
}

u_char
mpu401_read(struct midi_iface *mif)
{
	struct midi_iface_mpu401 *iface;
	struct midi_isa_iface *loc;
	u_char byte;

	iface = (struct midi_iface_mpu401 *)mif;
	loc = (struct midi_isa_iface *)mif->loc;
	byte = INB(loc->io_port + MPU401_DATA);
	return (byte);
}

int
mpu401_feature(struct midi_iface *mif, struct midi_feature *feature)
{
	struct midi_softc *softc;
	struct midi_iface_mpu401 *iface;

	iface = (struct midi_iface_mpu401 *)mif;
	softc = mif->softc;

	switch (feature->type) {
	case MFEAT_MPU401_TIMING:
		if (softc->timer->type == MT_MPU401)
			return (0);
		else {
			/* switch to MPU401 timer */
			/* change the timer */
			if (!midi_switch_timer(mif->softc,
			    midi_create_MPU401_timer))
				return (EIO);

			/* reset again so we can issue commands */
			(void)mpu401_reset(iface);
			if (!mpu401_enter_mpu401_timing_mode(iface))
				return (EIO);

			return (0);
		}
		break;
	case MFEAT_KERNEL_TIMING:
		if (softc->timer->type == MT_KERNEL)
			return (0);
		else {
			/* switch to internal timer */
			/* reset again so we can issue commands */
			(void)mpu401_reset(iface);
			if (!mpu401_uart(iface))
				return (EIO);

			/* change the timer */
			if (!midi_switch_timer(softc, midi_create_kernel_timer))
				return (EIO);
			return (0);
		}
		break;
	default:
		return (ENOTTY);
	}
}

void
mpu401_free(struct midi_iface *mif)
{
	struct midi_iface_mpu401 *iface;

	FREE(mif->loc, sizeof(struct midi_isa_iface));

	iface = (struct midi_iface_mpu401 *)mif;
	FREE(iface, sizeof(struct midi_iface_mpu401));
}

int
mpu401_wait_rdy_rcv(struct midi_iface_mpu401 *iface)
{
	int flag, i;
	struct midi_isa_iface *loc;

	loc = (struct midi_isa_iface *)iface->iface.loc;
	for (i = 0; i < MPU401_TRIES; i++) {
		flag = INB(loc->io_port + MPU401_STATUS);
		/* keep eyes open for data ariving */
		if ((flag & MPU401_DATA_AVL) == 0) {
			if (iface->iface.softc != NULL &&
			   iface->iface.softc->iface != NULL) {
				gen_midiintr(iface->iface.softc);
			} else {
				mpu401_intr((struct midi_iface *)iface);
			}
		}
		if ((flag & MPU401_RDY_RCV) == 0)
			break;
		U_DELAY(10);
	}
	if (i == MPU401_TRIES)
		return (0);
	return (1);
}

int
mpu401_send_command(struct midi_iface_mpu401 *iface, u_char comm)
{
	struct midi_isa_iface *loc;
	int s;

	loc = (struct midi_isa_iface *)iface->iface.loc;
#if 0
	iface->status |= MPU401_PROCESSING_COMMAND;
#endif

	if (!mpu401_wait_rdy_rcv(iface)) {
#if 0
		iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
		return (0);
	}

	/* writing command and setting ack flag must be atomic */
	s = BLOCK_INTR();
	OUTB(loc->io_port + MPU401_COMMAND, comm);
	iface->status |= MPU401_NEED_ACK;
	UNBLOCK_INTR(s);

	if (!mpu401_get_command_ack(iface)) {
#if 0
		iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
		iface->status &= ~MPU401_NEED_ACK;
		return (0);
	}

#if 0
	iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
	return (1);
}

int
mpu401_get_command_ack(iface)
	struct midi_iface_mpu401 *iface;
{
	struct midi_isa_iface *loc;
	int flag, i;

	loc = (struct midi_isa_iface *)iface->iface.loc;
	/*
	 * we have to be prepared to get the ack either ourselves
	 * (before interrupts have been enabled), or via an intr
	 * In either case, we should process the ack (or other data)
	 * through mpu401_intr
	 */
	while (iface->status & MPU401_NEED_ACK) {
		/* time out after MPU401_TRIES times */
		for (i = 0; i < MPU401_TRIES; i++) {
			/* did we pick up the ack via midiintr? */
			if (!(iface->status & MPU401_NEED_ACK))
				break;
			/* can we read a data byte */
			flag = INB(loc->io_port + MPU401_STATUS) &
			    MPU401_DATA_AVL;
			if (flag == 0)
				break;
			U_DELAY(10);
		}
		if (i == MPU401_TRIES)
			return (0);

		/* process data through intr */
		if (iface->status & MPU401_NEED_ACK) {
			if (iface->iface.softc != NULL &&
			   iface->iface.softc->iface != NULL) {
				gen_midiintr(iface->iface.softc);
			} else {
				mpu401_intr((struct midi_iface *)iface);
			}
		}
	}
	return (1);
}

int
mpu401_send_command_with_response(struct midi_iface_mpu401 *iface, u_char comm,
    void *response, int resp_size)
{
	struct midi_isa_iface *loc;
	int flag, i, j, s;
	u_char *resp;

	loc = (struct midi_isa_iface *)iface->iface.loc;
#if 0
	iface->status |= MPU401_PROCESSING_COMMAND;
#endif

	if (!mpu401_wait_rdy_rcv(iface)) {
#if 0
		iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
		return (0);
	}

	/* writing command and setting ack flag must be atomic */
	s = BLOCK_INTR();
	OUTB(loc->io_port + MPU401_COMMAND, comm);
	iface->status |= MPU401_NEED_DATA;
	iface->status |= MPU401_NEED_ACK;
	UNBLOCK_INTR(s);

	if (!mpu401_get_command_ack(iface)) {
		iface->status &= ~MPU401_NEED_DATA;
		iface->status &= ~MPU401_NEED_ACK;
#if 0
		iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
		return (0);
	}


	resp = response;
	for (j = 0; j < resp_size; j++) {
		for (i = 0; i < MPU401_TRIES; i++) {
			flag = INB(loc->io_port + MPU401_STATUS)
			    & MPU401_DATA_AVL;
			if (flag == 0)
				break;
			U_DELAY(10);
		}
		if (i == MPU401_TRIES) {
			iface->status &= ~MPU401_NEED_DATA;
#if 0
			iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
			return (0);
		}
		resp[j] = INB(loc->io_port + MPU401_DATA);
	}
	iface->status &= ~MPU401_NEED_DATA;
#if 0
	iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
	return (1);
}

int
mpu401_send_command_with_args(struct midi_iface_mpu401 *iface, u_char comm,
    void *vargs, int arg_size)
{
	struct midi_isa_iface *loc;
	int j, s;
	u_char *args;

	args = (u_char *)vargs;
	loc = (struct midi_isa_iface *)iface->iface.loc;

#if 0
	iface->status |= MPU401_PROCESSING_COMMAND;
#endif

	if (!mpu401_wait_rdy_rcv(iface)) {
#if 0
		iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
		return (0);
	}

	s = BLOCK_INTR();
	OUTB(loc->io_port + MPU401_COMMAND, comm);
	iface->status |= MPU401_NEED_ACK;
	UNBLOCK_INTR(s);

	if (!mpu401_get_command_ack(iface)) {
		iface->status &= ~MPU401_NEED_ACK;
#if 0
		iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
		return (0);
	}

	for (j = 0; j < arg_size; j++) {
		if (!mpu401_wait_rdy_rcv(iface)) {
			LOGERR("MPU401 not ready for data after command\n");
#if 0
			iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
			return (0);
		}
		OUTB(loc->io_port + MPU401_DATA, args[j]);
	}
#if 0
	iface->status &= ~MPU401_PROCESSING_COMMAND;
#endif
	return (1);
}

int
mpu401_reset(struct midi_iface_mpu401 *iface)
{

	if (!mpu401_send_command(iface, MPU401_RESET))
		return (0);
	return (1);
}

int
mpu401_uart(struct midi_iface_mpu401 *iface)
{

	if (!mpu401_send_command(iface, MPU401_UART))
		return (0);
	return (1);
}

int
mpu401_enter_mpu401_timing_mode(struct midi_iface_mpu401 *iface)
{
	u_char args[2];

	/* we're already in intelligent mode, so just do the other tricks */

	/*
	 * enable data transfer in stopped state
	 * by default, no timing byte is transfered, and this is
	 * what we want.
	 */
	if (!mpu401_send_command(iface, MPU401_ENABLE_DATA_TRANS))
		return (0);

	/* disable real time messages */
	if (!mpu401_send_command(iface, MPU401_DISABLE_RT))
		return (0);

	/*
	 * disable external control - we don't want external devices
	 * to start us playing.
	 */
	if (!mpu401_send_command(iface, MPU401_DISABLE_EXTERN))
		return (0);

	/* set base tempo to 100 bpm */
	args[0] = 100;
	if (!mpu401_send_command_with_args(iface, MPU401_TEMPO, args, 1))
		return (0);

	/* set internal clock resolution to 120 ticks per beat */
	if (!mpu401_send_command(iface, MPU401_CLOCK_RES_120))
		return (0);

	/* set clock to PC message rate to be at tick frequency */
	args[0] = 4;
	if (!mpu401_send_command_with_args(iface, MPU401_CLOCK_TO_PC_RATE,
	    args, 1))
		return (0);

	/* enable clock to PC messages */
	if (!mpu401_send_command(iface, MPU401_ENABLE_CLOCK_TO_PC))
		return (0);

	/* disable all play tracks */
	args[0] = 0x00;
	if (!mpu401_send_command_with_args(iface, MPU401_ACTIVE_TRACKS,
	   args, 1))
		return (0);

	/* set active tracks */
	if (!mpu401_send_command(iface, MPU401_CLEAR_PCS))
		return (0);

	/* disable conductor track */
	if (!mpu401_send_command(iface, MPU401_DISABLE_COND))
		return (0);

	/* enable system exlcusive messages to PC */
	if (!mpu401_send_command(iface, MPU401_ENABLE_SYSEX))
		return (0);

	/* no midithru */
	if (!mpu401_send_command(iface, MPU401_DISABLE_MT))
		return (0);
	if (!mpu401_send_command(iface, MPU401_UNACCEPT_CHAN))
		return (0);

	/* start "playing" so we get timer messages */
#if 0
	if (!mpu401_send_command(iface, MPU401_START_PLAYBACK))
		return (0);
#endif
	
	/* that should do it */
	return (1);
}
#endif

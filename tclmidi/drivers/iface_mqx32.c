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

#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_MQX32)

#include "sup_os.h"
#include "sup_iface.h"
#include "iface_mpu401.h"
#include "iface_mqx32.h"
#include "sup_timer_list.h"
#include "midiioctl.h"
#include "timer_smpte.h"

const char *mqx32_name(struct midi_iface *mif);
int mqx32_feature(struct midi_iface *iface, struct midi_feature *feature);
static int mqx32_enter_smpte_mode(struct midi_iface_mpu401 *iface);

struct midi_iface *
mqx32_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type, void *loc)
{
	struct midi_iface_mpu401 *iface;
	u_char rev;

	if (type != MIDI_ISA_IFACE)
		return (NULL);

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
	*(struct midi_isa_iface *)iface->iface.loc =
	    *(struct midi_isa_iface *)loc;

	iface->status = 0;
	iface->iface.softc = softc;
	iface->iface.name = mqx32_name;
	iface->iface.size = mpu401_size;
	iface->iface.gen_intr = mpu401_gen_intr;
	iface->iface.reset = mpu401_big_reset;
	iface->iface.open = mpu401_open;
	iface->iface.close = mpu401_close;
	iface->iface.intr = mpu401_intr;
	iface->iface.data_avail = mpu401_data_avail;
	iface->iface.write = mpu401_write;
	iface->iface.read = mpu401_read;
	iface->iface.feature = mqx32_feature;
	iface->iface.free = mpu401_free;

	if (!mpu401_reset(iface)) {
		if (!mpu401_reset(iface)) {
			mpu401_free((struct midi_iface *)iface);
			return (NULL);
		}
	}
	if (!mpu401_wait_rdy_rcv(iface)) {
		mpu401_free((struct midi_iface *)iface);
		return (NULL);
	}
	if (!mpu401_send_command_with_response(iface, MPU401_REVISION,
	    &rev, 1)) {
		mpu401_free((struct midi_iface *)iface);
		return (NULL);
	}
	if (rev & 0x04)
		iface->status = MQX32_SMPTE_EQUIP;
	else
		iface->status = 0;

	if (!mpu401_uart(iface)) {
		mpu401_free((struct midi_iface *)iface);
		return (NULL);
	}
	return ((struct midi_iface *)iface);
}

const char *
mqx32_name(struct midi_iface *mif)
{
	struct midi_iface_mpu401 *iface;

	iface = (struct midi_iface_mpu401 *)mif;
	if (iface->status & MQX32_SMPTE_EQUIP)
		return ("Music Quest MQX32 SMPTE equipped");
	else
		return ("Music Quest MQX32 no SMPTE");
}

int
mqx32_feature(struct midi_iface *mif, struct midi_feature *feature)
{
	struct midi_softc *softc;
	struct midi_iface_mpu401 *iface;

	softc = mif->softc;
	iface = (struct midi_iface_mpu401 *)mif;

	switch (feature->type) {
	case MFEAT_GET_SMPTE:
		if (softc->timer->type != MT_SMPTE)
			return (ENOTTY);
		else {
			struct midi_timer_smpte *timer;

			timer = (struct midi_timer_smpte *)softc->timer;
			FEATURE_K_TO_U(feature->data, &timer->SMPTE_current,
			    sizeof(struct SMPTE_frame));
			return (0);
		}
		break;
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
	case MFEAT_SMPTE_TIMING:
		if (softc->timer->type == MT_SMPTE)
			return (0);
		else {

			/* switch to SMPTE timer */
			if (!(iface->status & MQX32_SMPTE_EQUIP))
				return (ENXIO);
			else {
				/* reset again so we can issue commands */
				(void)mpu401_reset(iface);
				if (!mqx32_enter_smpte_mode(iface))
					return (EIO);
				if (!mpu401_uart(iface))
					return (EIO);

				/* change the timer */
				if (!midi_switch_timer(softc,
				    midi_create_SMPTE_timer))
					return (EIO);

				return (0);
			}
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

static int
mqx32_enter_smpte_mode(struct midi_iface_mpu401 *iface)
{

	if (!(iface->status & MQX32_SMPTE_EQUIP))
		return (0);
	/* MQX on-board clock */
	if (!mpu401_wait_rdy_rcv(iface))
		return (0);
	if (!mpu401_send_command(iface, 0x80))
		return (0);
	/* SMPTE tape sync */
	if (!mpu401_wait_rdy_rcv(iface))
		return (0);
	if (!mpu401_send_command(iface, 0x3d))
		return (0);
	/* SMPTE reading active */
	if (!mpu401_wait_rdy_rcv(iface))
		return (0);
	if (!mpu401_send_command(iface, 0x2d))
		return (0);
	/* system-exclusive messages to PC */
	if (!mpu401_wait_rdy_rcv(iface))
		return (0);
	if (!mpu401_send_command(iface, 0x38))
		return (0);
	if (!mpu401_wait_rdy_rcv(iface))
		return (0);
	if (!mpu401_send_command(iface, 0x39))
		return (0);
	/* clock messages to PC */
	if (!mpu401_wait_rdy_rcv(iface))
		return (0);
	if (!mpu401_send_command(iface, 0x95))
		return (0);

	return (1);
}
#endif

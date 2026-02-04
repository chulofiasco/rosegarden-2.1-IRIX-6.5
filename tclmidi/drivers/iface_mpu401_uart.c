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
 * This is like the mpu401, but only has a UART mode.  I does not
 * understand the reset and uart commands.  By default it is in UART
 * mode, but still has the data and status/comm i/o ports in the
 * same relative locations with the same bit meanings.
 */

#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_MPU401_UART)

#include "sup_os.h"
#include "sup_iface.h"
#include "iface_mpu401.h"

const char *mpu401uart_name(struct midi_iface *mif);
void mpu401uart_gen_intr(struct midi_iface *mif);
int mpu401uart_reset(struct midi_iface *mif);
int mpu401uart_feature(struct midi_iface *iface, struct midi_feature *feature);

struct midi_iface *
mpu401uart_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type, void *loc)
{
	struct midi_iface_mpu401 *iface;

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
	iface->iface.name = mpu401uart_name;
	iface->iface.size = mpu401_size;
	iface->iface.gen_intr = mpu401uart_gen_intr;
	iface->iface.reset = mpu401uart_reset;
	iface->iface.open = mpu401_open;
	iface->iface.close = mpu401_close;
	iface->iface.intr = mpu401_intr;
	iface->iface.data_avail = mpu401_data_avail;
	iface->iface.write = mpu401_write;
	iface->iface.read = mpu401_read;
	iface->iface.feature = mpu401uart_feature;
	iface->iface.free = mpu401_free;

	return ((struct midi_iface *)iface);
}

const char *
mpu401uart_name(struct midi_iface *mif)
{

	return ("MPU401 UART only");
}

void
mpu401uart_gen_intr(struct midi_iface *mif)
{

	/* we don't have anyway of generating an intr. */
	return;
}

int
mpu401uart_reset(struct midi_iface *mif)
{

	/* no reset exists - already in UART mode */
	return (1);
}

int
mpu401uart_feature(struct midi_iface *iface, struct midi_feature *feature)
{

	/* no features */
	return (ENOTTY);
}
#endif

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

#include "sup_os.h"
#include "midiioctl.h"
#include "sup_iface.h"

struct midi_iface *
midi_dev_probe(struct midi_softc *softc, long dev_type, MIDI_IFACE_TYPE type,
    void *loc)
{

	switch (dev_type) {
#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_MPU401)
	case MIDI_DEV_MPU401:
		return (mpu401_probe(softc, type, loc));
#endif
#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_MPU401_UART)
	case MIDI_DEV_MPU401UART:
		return (mpu401uart_probe(softc, type, loc));
#endif
#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_MQX32)
	case MIDI_DEV_MQX32:
		return (mqx32_probe(softc, type, loc));
#endif
#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_GRAVIS)
	case MIDI_DEV_GRAVIS_INT_GF1:
		return (gravis_internal_gf1_probe(softc, type, loc));
	case MIDI_DEV_GRAVIS_INT_IWAVE:
		return (gravis_internal_iwave_probe(softc, type, loc));
	case MIDI_DEV_GRAVIS_EXT_GF1:
		return (gravis_external_gf1_probe(softc, type, loc));
	case MIDI_DEV_GRAVIS_EXT_IWAVE:
		return (gravis_external_iwave_probe(softc, type, loc));
#endif
#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_SB)
	case MIDI_DEV_SB:
		return (sb_probe(softc, type, loc));
#endif
#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_SERIAL)
	case MIDI_DEV_SERIAL:
		return (serial_probe(softc, type, loc));
#endif
	default:
		return (0);
	}
}

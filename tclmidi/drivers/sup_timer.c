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
#include "sup_timer.h"
#include "sup_export.h"

int
midi_switch_timer(softc, new_timer)
	struct midi_softc *softc;
	struct midi_timer *(*new_timer) __P((struct midi_softc *));
{
	struct midi_softc *slave_softc;
	int i;

	/* remove existing timeouts */
	softc->timer->untimeout(softc->timer, softc->timeout_id,
	    (TIMEOUT_ARG)softc);
	if (softc->timeout_id != NULL)
		softc->timer->release_id(softc->timeout_id);

	if (softc->timer != NULL)
		softc->timer->free(softc->timer);

	softc->timer = new_timer(softc);
	if (softc->timer == NULL)
		return (0);
	softc->timer->init_clock(softc->timer);

	softc->timeout_id = softc->timer->create_id();
	if (softc->timeout_id == NULL)
		return (0);

	/* do the same for any slaves */
	if (softc->status & MIDI_MASTER) {
		for (i = 0; i < softc->num_slaves; i++) {
			slave_softc = midi_id2softc(softc->slaves[i]);
			if (slave_softc == NULL)
				continue;

			slave_softc->timer->untimeout(slave_softc->timer,
			    slave_softc->timeout_id, (TIMEOUT_ARG)slave_softc);
			if (slave_softc->timeout_id != NULL)
				slave_softc->timer->release_id(
				    slave_softc->timeout_id);

			if (slave_softc->timer != NULL)
				slave_softc->timer->free(slave_softc->timer);

			slave_softc->timer = softc->timer->dup(softc->timer);

			slave_softc->timeout_id =
			    slave_softc->timer->create_id();
			if (slave_softc->timeout_id == NULL)
				return (0);
		}
	}

	return (1);
}

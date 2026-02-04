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
#ifndef SUP_EXPORT_H
#define SUP_EXPORT_H

#include "sup_iface.h"

/*
 * These are the generic access functions
 */
extern int gen_midiopen __P((void *dev, int flags, int pgid, void *client));
extern int gen_midiclose __P((struct midi_softc *softc, int flags));
extern int gen_midiread __P((struct midi_softc *softc, void *uio, int flags));
extern int gen_midiwrite __P((struct midi_softc *softc, void *uio, int flags));
extern int gen_midiintr __P((struct midi_softc *softc));
extern int gen_midiioctl __P((struct midi_softc *softc, int cmd, void *data,
    int flags));

/*
 * And these are the exported support functions.
 */
extern int midi_fullreset __P((struct midi_softc *softc));
extern int midi_init_dev __P((struct midi_softc *softc, long dev_type,
    MIDI_IFACE_TYPE type, void *loc));
extern void midi_timeout __P((TIMEOUT_ARG arg));
extern void midi_initq __P((struct event_queue *));
extern int midi_deq __P((struct event_queue *, struct event **));
extern int midi_peekq __P((struct event_queue *, struct event **));
extern int midi_browseq __P((struct event_queue *, u_long, struct event **));
extern int midi_enq __P((struct event_queue *, struct event *));
extern void midi_time_warp __P((struct midi_softc *softc));
extern u_long midi_get_smf_time __P((struct midi_softc *softc));
extern struct midi_softc *midi_id2softc __P((int id));
#endif

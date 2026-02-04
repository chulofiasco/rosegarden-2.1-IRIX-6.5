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
#ifndef SUP_TIMER_LIST_H
#define SUP_TIMER_LIST_H


/* general timer functions */
extern void *midi_create_timeout_id(void);
extern void midi_free_timeout_id(void *vid);

/* stuff specific for the midi timer */
struct midi_timer_elem {
	int	id;
	int	ticks;
	TIMEOUT_FN	func;
	TIMEOUT_ARG	arg;
	struct	midi_timer_elem *next;
};

struct midi_timer_cntl {
	struct	midi_timer_elem *tl;
	struct	midi_timer_elem *to_due;
	TIMEOUT_ID	*to_id;
	volatile long	status;
};

/*
 * status flags
 */
#define MIDI_TL_TO_SCHEDULED	(1 << 0)


struct midi_timer_cntl *midi_create_timer_cntl(void);
void midi_add_timeout(struct midi_timer_cntl *cntl, int *id, TIMEOUT_FN fn,
    TIMEOUT_ARG arg, int ticks);
void midi_remove_timeout(struct midi_timer_cntl *cntl, int id, TIMEOUT_ARG arg);
void midi_dropped_timer(struct midi_timer_cntl *cntl);
void midi_kick_timer(struct midi_timer_cntl *cntl);
void midi_reset_timer_cntl(struct midi_timer_cntl *cntl);
void midi_release_timer_cntl(struct midi_timer_cntl *cntl);

#endif

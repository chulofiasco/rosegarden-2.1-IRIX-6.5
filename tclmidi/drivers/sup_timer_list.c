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
#include "sup_timer_list.h"

int midi_to_id_counter = 0;

static void timeout_wrapper(TIMEOUT_ARG arg);

void *
midi_create_timeout_id(void)
{
	int *id;

	id = MALLOC_NOWAIT(sizeof(int));
	return (id);
}

void
midi_free_timeout_id(void *vid)
{
	int *id;

	id = (int *)vid;
	FREE(id, sizeof(int));
}

struct midi_timer_cntl *
midi_create_timer_cntl(void)
{
	struct midi_timer_cntl *cntl;
	int s;

	s = BLOCK_INTR();
	cntl = MALLOC_NOWAIT(sizeof(struct midi_timer_cntl));
	if (cntl == NULL) {
		UNBLOCK_INTR(s);
		return (NULL);
	}
	cntl->tl = NULL;
	cntl->to_due = NULL;
	cntl->to_id = MALLOC_NOWAIT(sizeof(TIMEOUT_ID));
	if (cntl->to_id == NULL) {
		FREE(cntl, sizeof(struct midi_timer_cntl));
		UNBLOCK_INTR(s);
		return (NULL);
	}
	INIT_TIMEOUT_ID(cntl->to_id);
	cntl->status = 0;
	UNBLOCK_INTR(s);
	return (cntl);
}

/*
 * Maintain a list of the timeouts scheduled.
 * The list is sorted so the first element is the next to occur, and
 * the ticks value in that element is the time until the timeout expires.
 * The reset of the elements contain a ticks value that is the time
 * from the previous element.
 */
void
midi_add_timeout(struct midi_timer_cntl *cntl, int *id, TIMEOUT_FN fn,
    TIMEOUT_ARG arg, int ticks)
{
	struct midi_timer_elem *elem, *elem_ptr, *last_elem;
	int s, total_ticks;

	s = BLOCK_INTR();
	elem = MALLOC_NOWAIT(sizeof(struct midi_timer_elem));
	if (elem == NULL)
		panic("no more memory for creating a midi timeout");

	elem->id = midi_to_id_counter++;

	last_elem = NULL;
	total_ticks = 0;
	for (elem_ptr = cntl->tl; elem_ptr != NULL; elem_ptr = elem_ptr->next) {
		if (elem_ptr->ticks > 0)
			total_ticks += elem_ptr->ticks;
		if (total_ticks > ticks) {
			/* we've found the place to insert the new T.O. */
			total_ticks -= elem_ptr->ticks;
			break;
		}
		last_elem = elem_ptr;
	}
	if (total_ticks < 0)
		total_ticks = 0;
	elem->ticks = ticks - total_ticks;
	elem->func = fn;
	elem->arg = arg;
	if (last_elem == NULL) {
		elem->next = cntl->tl;
		cntl->tl = elem;
	} else {
		elem->next = last_elem->next;
		last_elem->next = elem;
	}
	*id = elem->id;
	UNBLOCK_INTR(s);
}

/*
 * Removes the timeout element identified by id.
 * It adjust the timing on the following event so it is still accurate.
 */
void
midi_remove_timeout(struct midi_timer_cntl *cntl, int id, TIMEOUT_ARG arg)
{
	struct midi_timer_elem *elem, *last_elem;
	int s;

	s = BLOCK_INTR();
	/* first check to see if it's due */
#if 0
	last_elem = NULL;
	for (elem = cntl->to_due; elem != NULL; elem = elem->next) {
		if (elem->id == id)
			break;
		last_elem = elem;
	}
	if (elem != NULL) {
		if (last_elem == NULL)
			cntl->to_due = elem->next;
		else
			last_elem->next = elem->next;
		FREE(elem, sizeof(struct midi_timer_elem));
		UNBLOCK_INTR(s);
		return;
	}
#endif

	/* now check our regular list */
	last_elem = NULL;
	for (elem = cntl->tl; elem != NULL; elem = elem->next) {
		if (elem->id == id)
			break;
		last_elem = elem;
	}
	if (elem == NULL) {
		UNBLOCK_INTR(s);
		return;
	}
	if (last_elem == NULL)
		cntl->tl = elem->next;
	else
		last_elem->next = elem->next;
	/* next element gets the deleted elements ticks too */
	if (elem->next != NULL)
		elem->next->ticks += elem->ticks;
	FREE(elem, sizeof(struct midi_timer_elem));
	UNBLOCK_INTR(s);
}

/*
 * We need to update the timer, but for some reason shouldn't
 * do any callbacks
 */
void
midi_dropped_timer(struct midi_timer_cntl *cntl)
{
	struct midi_timer_elem *elem;
	int s;

	s = BLOCK_INTR();
	elem = cntl->tl;
	if (elem == NULL) {
		UNBLOCK_INTR(s);
		return;
	}
	elem->ticks--;
	UNBLOCK_INTR(s);
}

/*
 * moves the timer ahead one tick.
 * if a timeout has expired, it's function is called and the list
 * is adjusted to remove the timeout.
 * The function is no longer called directly.  Instead it is added to
 * the timeout due list, and a kernel timeout scheduled in 0 ticks
 * if it is not already scheduled.
 */
void
midi_kick_timer(struct midi_timer_cntl *cntl)
{
	struct midi_timer_elem *elem;
	int s;

	s = BLOCK_INTR();
	elem = cntl->tl;
	if (elem == NULL) {
		UNBLOCK_INTR(s);
		return;
	}
	elem->ticks--;
	while (elem != NULL && elem->ticks <= 0) {
		/* if we're overdue, adjust the ticks for the next callback */
		if (elem->ticks < 0) {
			if (elem->next != NULL)
				elem->next->ticks += elem->ticks;
		}
		/* our time has come */
		cntl->tl = elem->next;
		elem->next = cntl->to_due;
		cntl->to_due = elem;
		if (!(cntl->status & MIDI_TL_TO_SCHEDULED)) {
			/* we need to schedule a timeout */
			TIMEOUT(cntl->to_id, timeout_wrapper,
			    (TIMEOUT_ARG)cntl, 0);
			cntl->status |= MIDI_TL_TO_SCHEDULED;
		}
		elem = cntl->tl;
	}
	UNBLOCK_INTR(s);
}

void
midi_reset_timer_cntl(struct midi_timer_cntl *cntl)
{
	struct midi_timer_elem *elem, *next_elem;
	int s;

	s = BLOCK_INTR();
	elem = cntl->tl;
	while (elem != NULL) {
		next_elem = elem->next;
		FREE(elem, sizeof(struct midi_timer_elem));
		elem = next_elem;
	}
	cntl->tl = NULL;

	elem = cntl->to_due;
	while (elem != NULL) {
		next_elem = elem->next;
		FREE(elem, sizeof(struct midi_timer_elem));
		elem = next_elem;
	}
	cntl->to_due = NULL;
	if (cntl->status & MIDI_TL_TO_SCHEDULED) {
		UNTIMEOUT(cntl->to_id, cntl);
		cntl->status &= ~MIDI_TL_TO_SCHEDULED;
	}
}

void
midi_release_timer_cntl(struct midi_timer_cntl *cntl)
{
	struct midi_timer_elem *elem, *next_elem;
	int s;

	s = BLOCK_INTR();
	elem = cntl->tl;
	while (elem != NULL) {
		next_elem = elem->next;
		FREE(elem, sizeof(struct midi_timer_elem));
		elem = next_elem;
	}
	cntl->tl = NULL;

	elem = cntl->to_due;
	while (elem != NULL) {
		next_elem = elem->next;
		FREE(elem, sizeof(struct midi_timer_elem));
		elem = next_elem;
	}
	cntl->to_due = NULL;
	if (cntl->status & MIDI_TL_TO_SCHEDULED)
		UNTIMEOUT(cntl->to_id, cntl);
	FREE(cntl->to_id, sizeof(TIMEOUT_ID));
	FREE(cntl, sizeof(struct midi_timer_cntl));
	UNBLOCK_INTR(s);
}

static void
timeout_wrapper(TIMEOUT_ARG arg)
{
	struct midi_timer_cntl *cntl;
	struct midi_timer_elem *elem, *next_elem;
	int s;

	cntl = (struct midi_timer_cntl *)arg;

	s = BLOCK_INTR();
	for (elem = cntl->to_due; elem != NULL; elem = elem->next) {
		UNBLOCK_INTR(s);
		elem->func(elem->arg);
		s = BLOCK_INTR();
	}
	/* release timeouts */
	elem = cntl->to_due;
	while (elem != NULL) {
		next_elem = elem->next;
		FREE(elem, sizeof(struct midi_timer_elem));
		elem = next_elem;
	}
	cntl->to_due = NULL;
	cntl->status &= ~MIDI_TL_TO_SCHEDULED;
	UNBLOCK_INTR(s);
}

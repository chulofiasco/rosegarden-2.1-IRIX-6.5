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
#include "timer_kern.h"

static void kt_timeout(struct midi_timer *mt, void *vid,
    TIMEOUT_FN fn, TIMEOUT_ARG arg, int t);
static void kt_untimeout(struct midi_timer *mtk, void *vid, TIMEOUT_ARG arg);
static u_long kt_get_clock(struct midi_timer *mt);
static void kt_init_clock(struct midi_timer *mt);
static void kt_reset(struct midi_timer *mt);
static void kt_free(struct midi_timer *mt);
static struct midi_timer *kt_dup(struct midi_timer *mt);
static void *kt_create_id(void);
static void kt_free_id(void *vid);



/*
 * Our midi generic wrapper around the kernel timer
 */
struct midi_timer *
midi_create_kernel_timer(struct midi_softc *softc)
{
	struct midi_timer_kernel *mtk;

	mtk = MALLOC_NOWAIT(sizeof(struct midi_timer_kernel));
	if (mtk == NULL)
		return (NULL);
	mtk->timer.softc = softc;
	mtk->timer.type = MT_KERNEL;
	mtk->timer.hz = KERNEL_HZ;
	mtk->timer.ref_count = 1;
	mtk->timer.timeout = kt_timeout;
	mtk->timer.untimeout = kt_untimeout;
	mtk->timer.get_clock = kt_get_clock;
	mtk->timer.init_clock = kt_init_clock;
	mtk->timer.reset = kt_reset;
	mtk->timer.free = kt_free;
	mtk->timer.dup = kt_dup;
	mtk->timer.create_id = kt_create_id;
	mtk->timer.release_id = kt_free_id;

	return ((struct midi_timer *)mtk);
}

static void
kt_timeout(struct midi_timer *mt, void *vid, TIMEOUT_FN fn,
    TIMEOUT_ARG arg, int t)
{
	struct midi_timer_kernel *mtk;
	TIMEOUT_ID *id;

	mtk = (struct midi_timer_kernel *)mt;
	id = (TIMEOUT_ID *)vid;

	TIMEOUT(id, fn, arg, t);
}

static void
kt_untimeout(struct midi_timer *mt, void *vid, TIMEOUT_ARG arg)
{
	TIMEOUT_ID *id;

	id = (TIMEOUT_ID *)vid;
	UNTIMEOUT(id, arg);
}

static u_long
kt_get_clock(struct midi_timer *mt)
{
	u_long t;
	struct midi_timer_kernel *mtk;

	mtk = (struct midi_timer_kernel *)mt;

	t = GET_KERN_TIME((struct midi_timer *)mtk) - mtk->start_time;
	return (t);
}

static void
kt_init_clock(struct midi_timer *mt)
{
	struct midi_timer_kernel *mtk;

	mtk = (struct midi_timer_kernel *)mt;

	mtk->start_time = GET_KERN_TIME((struct midi_timer *)mtk);
}

static void
kt_reset(struct midi_timer *mt)
{
}

static struct midi_timer *
kt_dup(struct midi_timer *mt)
{

	mt->ref_count++;
	return (mt);
}

static void
kt_free(struct midi_timer *mt)
{
	struct midi_timer_kernel *mtk;

	mtk = (struct midi_timer_kernel *)mt;

	mtk->timer.ref_count--;
	if (mtk->timer.ref_count == 0)
		FREE(mtk, sizeof(struct midi_timer_kernel));
}

/*
 * creates and destorys a pointer to the kernel specific timer id
 */
static void *
kt_create_id(void)
{
	TIMEOUT_ID *id;

	id = MALLOC_NOWAIT(sizeof(TIMEOUT_ID));
	INIT_TIMEOUT_ID(id);
	return (id);
}

static void
kt_free_id(void *vid)
{
	TIMEOUT_ID *id;

	id = (TIMEOUT_ID *)vid;
	FREE(id, sizeof(TIMEOUT_ID));
}

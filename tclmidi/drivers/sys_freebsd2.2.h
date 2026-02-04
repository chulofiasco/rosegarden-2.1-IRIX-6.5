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
/*
 * FreeBSD's Driver dependent code
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/reboot.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/conf.h>
#include <sys/dkstat.h>
#include <sys/file.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/syslog.h>
#include <sys/kernel.h>
#include <sys/vnode.h>
#include <sys/devconf.h>
#include <sys/signalvar.h>
#include <machine/clock.h>
#ifdef DEVFS
#include <sys/devfsext.h>
#endif

#include <i386/isa/icu.h>
#include <i386/isa/isa.h>
#include <i386/isa/isa_device.h>


typedef void * TIMEOUT_ARG;
typedef void (*TIMEOUT_FN)(TIMEOUT_ARG);
typedef void (*TIMEOUT_ID)(void *);
#define SELCHAN struct selinfo
#define SLEEPCHAN u_char

#include "sup_var.h"
#include "midiioctl.h"
#include "sup_softc.h"


/*
 * port definitions
 */
#define OUTB(x, y)	outb((x), (y))
#define INB(x)		inb((x))
#define OUTW(x, y)	outw((x), (y))
#define INW(x)		inw((x))
#define LOGERR(x)	log(LOG_ERR, x)
#define LOGWARN(x)	log(LOG_WARNING, x)
#define PRINTF		printf
#define GET_MINOR(x)	minor((dev_t)(x))
#define MALLOC_NOWAIT(x)	malloc(x, M_DEVBUF, M_NOWAIT)
#define MALLOC_WAIT(x)		malloc(x, M_DEVBUF, M_WAITOK)
#undef FREE	/* defined in sys/malloc.h for tracing */
#define FREE(x, y)		free((x), M_DEVBUF)
#define SLEEP_INTR(x, y)	tsleep((caddr_t)(x), PWAIT | PCATCH, y, 0)
#define WAKEUP(x)		wakeup((x))
#define UIO_REMAIN(x)		((struct uio *)(x))->uio_resid
#define MOVE_TO_UIO(uio, src, num)	uiomove((src), (num), \
					    (struct uio *)(uio))
#define IOCTL_U_TO_K(dest, src, sz)	bcopy((src), (dest), (sz))
#define IOCTL_K_TO_U(dest, src, sz)	bcopy((src), (dest), (sz))
#define FEATURE_U_TO_K(dest, src, sz)	copyin((src), (dest), (sz))
#define FEATURE_K_TO_U(dest, src, sz)	copyout((src), (dest), (sz))
#define INIT_TIMEOUT_ID(x)
#define KERNEL_HZ	hz
#define BLOCK_INTR	splhigh
#define UNBLOCK_INTR(x)	splx(x)
#define BZERO(dest, size)	bzero(dest, size)
#define BCOPY(src, dest, size)	bcopy(src, dest, size)
#define SELWAKEUP(x)	selwakeup(x)
#define UNTIMEOUT(x, y)	untimeout(*(x), (y))
#define GET_KERN_TIME(x)        ticks
#define FIND_IRQ(x)	-1
#define U_DELAY(x)	DELAY(x)

/* select channels */
#define SEL_RCHAN(softc)	&softc->rsel
#define SEL_WCHAN(softc)	&softc->wsel
#define SEL_ECHAN(softc)	&softc->esel

/*
 * port functions
 */
extern struct midi_softc *UNIT_TO_SOFTC __P((int unit, void *client));
extern void SIGNAL_PG __P((int pgid, int sig));
extern void TIMEOUT __P((TIMEOUT_ID *id, TIMEOUT_FN fn, TIMEOUT_ARG arg,
    int t));
extern int UWRITEC __P((struct uio *uio));

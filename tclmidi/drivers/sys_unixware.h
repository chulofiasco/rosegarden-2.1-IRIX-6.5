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
 * Unixware Driver dependent code
 */
#define INKERNEL 1
#define _KERNEL 1

#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/dir.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/open.h>
#include <sys/uio.h>
#include <sys/cred.h>
#include <sys/ddi.h>
#include <sys/poll.h>
#include <sys/kmem.h>
#include <sys/filio.h>
#include <sys/termios.h>

#include <sys/conf.h>
#include <sys/cmn_err.h>
#include <sys/time.h>
#include <sys/moddefs.h>
#include <sys/ioccom.h>

#include "sup_var.h"

typedef caddr_t TIMEOUT_ARG;
typedef void (*TIMEOUT_FN)(TIMEOUT_ARG);
typedef int TIMEOUT_ID;
/* only one of the three selchannels is used */
#define SELCHAN struct pollhead
#define SLEEPCHAN u_char

#include "sup_softc.h"

#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos)	protos
#else
#define __P(protos)	()
#endif

extern void outb __P((unsigned, ulong));
extern ulong inb __P((unsigned));

struct unixware_midi_softc {
	struct	midi_softc s;
	long	local_status;
};

/* unixware specific entry points */
void midiinit __P(());
int midiopen __P((dev_t *dev, int flag, int otyp, cred_t *cred_p));
int midiclose __P((dev_t dev, int flag, int otyp, cred_t *cred_p));
int midiread __P((dev_t dev, uio_t *uio_p, cred_t *cred_p));
int midiwrite __P((dev_t dev, uio_t *uio_p, cred_t *cred_p));
int midiioctl __P((dev_t dev, int cmd, int arg, int mode, cred_t *cred_p,
    int *rval_p));
int midichpoll __P((dev_t dev, int /* short */ events, int anyyet,
    short *reventsp, struct pollhead **phpp));
int midiintr __P((int ivec));

/*
 * local status flags
 */
#define MIDI_NONBLOCK (1 << 0)

/*
 * port definitions
 */
#define OUTB(x, y)	outb((x), (y))
#define INB(x)		inb((x))
#define OUTW(x, y)	outw((x), (y))
#define INW(x)		inw((x))
#define LOGERR(x)	cmn_err(CE_CONT, x)
#define LOGWARN(x)	cmn_err(CE_WARN, x)
#define PRINTF		printf
#define GET_MINOR(x)	getminor(*(dev_t *)(x))
#define MALLOC_NOWAIT(x)	kmem_alloc((x), KM_NOSLEEP)
/* XXX MALLOC_WAIT should have a different flag, but I don't know what */
#define MALLOC_WAIT(x)		kmem_alloc((x), KM_NOSLEEP)
#define FREE(x, y)		kmem_free((x), (y))
#define SLEEP_INTR(x, y)	sleep((caddr_t)(x), PWAIT | PCATCH)
#define WAKEUP(x)		wakeup((caddr_t)(x))
#define UIO_REMAIN(x)		((uio_t *)(x))->uio_resid
#define MOVE_TO_UIO(uio, s, num) uiomove((s), (num), UIO_READ, (uio_t *)(uio))
#define UWRITEC(uio)		uwritec((uio_t *)(uio))
#define IOCTL_U_TO_K(dest,src,sz) copyin((caddr_t)(src),(caddr_t)(dest),(sz))
#define IOCTL_K_TO_U(dest,src,sz) copyout((caddr_t)(src),(caddr_t)(dest),(sz))
#define FEATURE_U_TO_K(dest,src,sz) copyin((caddr_t)(src),(caddr_t)(dest),(sz))
#define FEATURE_K_TO_U(dest,src,sz) copyout((caddr_t)(src),(caddr_t)(dest),(sz))
#define INIT_TIMEOUT_ID
#define UNTIMEOUT(id, arg)	untimeout(((int)id))
#define KERNEL_HZ	Hz
#define BLOCK_INTR	spltty
#define UNBLOCK_INTR(x)	splx((x))
#define BZERO(dest, size)	bzero((caddr_t)(dest), (size))
#define BCOPY(src, dest, size)	bcopy((src), (dest), (size))
#define SELWAKEUP(x)		pollwakeup((x))
#define GET_KERN_TIME(x)	lbolt
#define FIND_IRQ(x)		-1
#define U_DELAY(x)		delay(x)

/* select channels */
#define SEL_RCHAN(softc)	&softc->rsel, POLLIN
#define SEL_WCHAN(softc)	&softc->rsel, POLLOUT
#define SEL_ECHAN(softc)	&softc->rsel, POLLPRI

/*
 * port functions
 */
extern struct midi_softc *UNIT_TO_SOFTC __P((int unit, void *client));
extern void SIGNAL_PG __P((int pgid, int sig));
extern void TIMEOUT __P((TIMEOUT_ID *id, TIMEOUT_FN fn, TIMEOUT_ARG arg,
    int t));

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
 * Linux Driver dependent code
 */

#include <linux/version.h>
#undef NEED_KERNEL_VERSION_STRING

#ifdef MODULE
#ifdef MODVERSIONS

/*
 * Some module things changed after 1.3.37
 */

#if (LINUX_VERSION_CODE > 0x010325)
#include <linux/modversions.h>
#else
#define NEED_KERNEL_VERSION_STRING
#define CONFIG_MODVERSIONS
#endif

#endif /* MODVERSIONS */
#endif /* MODULE */

#include <stdlib.h>
#include <linux/types.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/fcntl.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/malloc.h>
#include <linux/sys.h>
#include <linux/termios.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/segment.h>
#include <asm/system.h>

typedef unsigned long TIMEOUT_ARG;
typedef void (*TIMEOUT_FN)(TIMEOUT_ARG);
typedef struct timer_list TIMEOUT_ID;
#define SELCHAN struct wait_queue *
#define SLEEPCHAN struct wait_queue *

#include "sup_var.h"
#include "sup_softc.h"

struct linux_midi_softc {
	struct	midi_softc s;
	long	local_status;
};

struct midi_config {
	long	type;
	int	addr;
	int	intr;
};

/* linux specific entry points */
int midiopen __P((struct inode *inode, struct file *file));
void midirelease __P((struct inode *inode, struct file *file));
int midiread __P((struct inode *inode, struct file *file, char *buf, int len));
#if (LINUX_VERSION_CODE < 0x01030F)
int midiwrite __P((struct inode *inode, struct file *file, char *buf, int len));
#else
int midiwrite __P((struct inode *inode, struct file *file, const char *buf,
    int len));
#endif
int midiioctl __P((struct inode *inode, struct file *file, unsigned int cmd,
    unsigned long arg));
int midiselect __P((struct inode *inode, struct file *file, int type,
    select_table *wait));
void midiintr __P((int irq, struct pt_regs *reg));

/*
 * fake uio structure
 */
struct uio {
	char	*s;
	char	*pos;
	int	len;
	int	remain;
};

/*
 * local status flags
 */
#define MIDI_NONBLOCK (1 << 0)

/*
 * port definitions
 */
#define OUTB(x, y)		outb((y), (x))
#define INB(x)			inb((x))
#define OUTW(x, y)		outw((y), (x))
#define INW(x)			inw((x))
#define LOGERR(x)		printk(KERN_ERR x)
#define LOGWARN(x)		printk(KERN_WARNING x)
#define GET_MINOR(x)		MINOR(*(dev_t *)(x))
#define PRINTF			printk
#define SLEEP_INTR(x, y)	({interruptible_sleep_on((x)); \
				    (current->signal & ~current->blocked) \
				    ? EINTR : 0;})
#define WAKEUP(x)		wake_up_interruptible((x))
#define UIO_REMAIN(x)		((struct uio *)(x))->remain
#define IOCTL_U_TO_K(dest, src, sz)	memcpy_fromfs((dest), (src), (sz))
#define IOCTL_K_TO_U(dest, src, sz)	memcpy_tofs((dest), (src), (sz))
#define FEATURE_U_TO_K(dest, src, sz)	memcpy_fromfs((dest), (src), (sz))
#define FEATURE_K_TO_U(dest, src, sz)	memcpy_tofs((dest), (src), (sz))
#define INIT_TIMEOUT_ID(x)	init_timer((x))
#define UNTIMEOUT(id, arg)	del_timer((id))
#define KERNEL_HZ		HZ
#define BLOCK_INTR()		({unsigned long flags; save_flags(flags); \
				    cli(); flags;})
#define UNBLOCK_INTR(x)		restore_flags((x))
#define BZERO(dest, size)	memset((dest), 0, (size))
#define BCOPY(src, dest, size)	memcpy((dest), (src), (size))
#define SELWAKEUP(x)		wake_up_interruptible((x))
#define GET_KERN_TIME(x)	jiffies
#define FIND_IRQ(x)		-1
#define U_DELAY(x)		udelay(x)

/* select channels */
#define SEL_RCHAN(softc)	&softc->rsel
#define SEL_WCHAN(softc)	&softc->wsel
#define SEL_ECHAN(softc)	&softc->esel

/*
 * port functions
 */
extern void *MALLOC_NOWAIT __P((unsigned int size));
extern void *MALLOC_WAIT __P((unsigned int size));
extern void FREE __P((void *obj, unsigned int size));
extern int MOVE_TO_UIO __P((void *uio, char *s, int num));
extern int UWRITEC __P((void *uio));
extern struct midi_softc *UNIT_TO_SOFTC __P((int unit, void *client));
extern void SIGNAL_PG __P((int pgid, int sig));
extern void TIMEOUT __P((TIMEOUT_ID *id, TIMEOUT_FN fn, TIMEOUT_ARG arg,
    int t));

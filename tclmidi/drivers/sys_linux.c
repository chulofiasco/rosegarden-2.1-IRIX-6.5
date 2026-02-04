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
/*
 * Linux driver dependent code
 */

#include "sys_linux.h"
#include "sup_export.h"

#include <linux/module.h>

#ifdef NEED_KERNEL_VERSION_STRING
char kernel_version[] = UTS_RELEASE;
#endif

/*
 * This is where you configure the MPU401 devices.
 * Define NMIDI to be the number of MPU401 devices you have installed.
 * And add an entry to the midi_conf array describing each
 * device's type, I/O addr and IRQ number.
 * For a list of types and their codes, see the MIDI_DEV_* defines
 * in sup_iface.h.
 */

#define NMIDI 1
int NumMidi = NMIDI;
struct midi_config midi_conf[NMIDI] = {
	{MIDI_DEV_MPU401, 0x330, 5}	/* 'standard' MPU401 */
};
struct linux_midi_softc midi_sc[NMIDI];


extern volatile u_long jiffies;

/*
 * SMP support broke this in 1.3.31.. current is now a macro
 * Seems like an awfully generic name to make into a macro..
 * My guess is that it will change again soon..
 */
#if (LINUX_VERSION_CODE < 0x01031F)
extern struct task_struct *current;
#endif

static struct file_operations midi_fops = {
	NULL,		/* lseek */
	midiread,	/* read */
	midiwrite,	/* write */
	NULL,		/* readdir */
	midiselect,	/* select */
	midiioctl,	/* ioctl */
	NULL,		/* mmap */
	midiopen,	/* open */
	midirelease,	/* release */
	NULL,		/* fsync */
	NULL,		/* fasync 	(since 1.1.13) */
	NULL,		/* media change	(since 1.1.24) */
	NULL		/* revalidate	(since 1.1.24) */
};

#ifdef MODULE
long midiinit __P((long));

int
init_module(void)
{

	printk("midi.c: init_module called\n");
	return (midiinit(0));
}

void
cleanup_module(void)
{
	struct midi_isa_iface *loc;
	struct midi_softc *softc;
	int i;

	/* don't free anything until all devices are closed */
	for (i = 0; i < NumMidi; i++) {
		softc = &(midi_sc[i].s);
		if (softc->status & MIDI_OPEN) {
			printk("midi device #%d is in use. Try again later\n",
			    i);
			return;
		}
	}
	for (i = 0; i < NumMidi; i++) {
		softc = &(midi_sc[i].s);
		if (softc->status & MIDI_UNAVAILABLE)
			continue;
		midi_fullreset(softc);
		kfree_s(softc->wqueue, sizeof(struct event_queue));
		kfree_s(softc->rqueue, sizeof(struct event_queue));
		kfree_s(softc->thruqueue, sizeof(struct event_queue));
		loc = (struct midi_isa_iface *)softc->iface->loc;
#if (LINUX_VERSION_CODE > 0x010345)
		free_irq(loc->irq, NULL);
#else
		free_irq(loc->irq);
#endif
		softc->iface->free(softc->iface);
		softc->timer->release_id(softc->timeout_id);
		softc->timer->release_id(softc->thru_timeout_id);
		softc->timer->free(softc->timer);
	}
	if (unregister_chrdev(MIDI_MAJOR, "midi") != 0)
		printk("cleanup_module failed\n");
	else
		printk("cleanup_module succeeded\n");
}
#endif

long
midiinit(kmem_start)
	long kmem_start;
{
	struct midi_isa_iface loc, *ploc;
	struct midi_softc *softc;
	int i;

	if (register_chrdev(MIDI_MAJOR, "midi", &midi_fops)) {
		printk("unable to get major %d for MPU401 MIDI\n",
		    MIDI_MAJOR);
		return (kmem_start);
	}
	for (i = 0; i < NumMidi; i++) {
		softc = &(midi_sc[i].s);

		loc.irq = midi_conf[i].intr;
		loc.io_port = midi_conf[i].addr;
		if (!midi_init_dev(softc, midi_conf[i].type, MIDI_ISA_IFACE,
		    &loc)) {
			printk("Couldn't init MIDI dev %d at 0x%x, IRQ %x\n",
			    (int)midi_conf[i].type, loc.io_port, loc.irq);
			continue;
		}

		init_timer(softc->timeout_id);
		init_timer(softc->thru_timeout_id);

		/* allocate memory for event queues */
#ifndef MODULE
		softc->rqueue = (struct event_queue *)kmem_start;
		kmem_start += sizeof(struct event_queue);
		softc->wqueue = (struct event_queue *)kmeme_start;
		kmem_start += sizeof(struct event_queue);
		softc->thruqueue = (struct event_queue *)kmeme_start;
		kmem_start += sizeof(struct event_queue);
#else
		softc->rqueue = kmalloc(sizeof(struct event_queue),
		    GFP_KERNEL);
		if (softc->rqueue == 0) {
			printk("Out of memory for read queue\n");
			return (-1);
		}
		softc->wqueue = kmalloc(sizeof(struct event_queue),
		    GFP_KERNEL);
		if (softc->wqueue == 0) {
			printk("Out of memory for write queue\n");
			kfree_s(softc->rqueue, sizeof(struct event_queue));
			return (-1);
		}
		softc->thruqueue = kmalloc(sizeof(struct event_queue),
		    GFP_KERNEL);
		if (softc->thruqueue == 0) {
			printk("Out of memory for thru queue\n");
			kfree_s(softc->rqueue, sizeof(struct event_queue));
			kfree_s(softc->wqueue, sizeof(struct event_queue));
			return (-1);
		}
#endif
		memset(softc->rqueue, 0, sizeof(struct event_queue));
		memset(softc->wqueue, 0, sizeof(struct event_queue));
		memset(softc->thruqueue, 0, sizeof(struct event_queue));
		stynamic_init(&softc->rpartial);
		stynamic_init(&softc->wpartial);
		softc->wpartialpos = 0;

		/* register the intr */
		ploc = (struct midi_isa_iface *)softc->iface->loc;
#if (LINUX_VERSION_CODE > 0x010345)
		if (request_irq(ploc->irq, (void *)midiintr, 0, "midi", NULL)) {
#else
		if (request_irq(ploc->irq, (void *)midiintr, 0, "midi")) {
#endif
			printk("MIDI #%d unable to use interrupt %d\n",
			    i, ploc->irq);
#ifndef MODULE
			kmem_start -= 2 * sizeof(struct event_queue);
#else
			kfree_s(softc->rqueue, sizeof(struct event_queue));
			kfree_s(softc->wqueue, sizeof(struct event_queue));
			kfree_s(softc->thruqueue, sizeof(struct event_queue));
#endif
			softc->status |= MIDI_UNAVAILABLE;
			continue;
		}
/* XXX I need to look into how to allocate a DMA channel under linux */
/* Try this.. (from Greg)
		if (request_dma(ploc->dma, "midi")) {
			printk("Unable to allocate DMA%d\n", ploc->dma);
			return (-1);
		}
*/
		printk("Found %s #%d at 0x%02x irq %d\n",
		    softc->iface->name(softc->iface), i, ploc->io_port,
		    ploc->irq);
	}
	return(kmem_start);
}

int
midiopen(inode, file)
	struct inode *inode;
	struct file *file;
{
	struct linux_midi_softc *lsc;
	int f, res;

	f = 0;
	switch (file->f_flags & O_ACCMODE) {
	case O_RDONLY:
		f |= MIDI_OREAD;
		break;
	case O_WRONLY:
		f |= MIDI_OWRITE;
		break;
	case O_RDWR:
		f |= MIDI_OREAD | MIDI_OWRITE;
		break;
	}

	res = gen_midiopen(&inode->i_rdev, f, current->pid, midi_sc);

	if (res == 0) {
		MOD_INC_USE_COUNT;
		lsc = &midi_sc[MINOR(inode->i_rdev) & 0x3f];
		lsc->local_status = 0;
		if (file->f_flags & O_NONBLOCK)
			lsc->local_status |= MIDI_NONBLOCK;
	}

	return (-res);
}

void
midirelease(inode, file)
	struct inode *inode;
	struct file *file;
{
	int f;

	f = 0;
	switch (file->f_flags & O_ACCMODE) {
	case O_RDONLY:
		f |= MIDI_OREAD;
		break;
	case O_WRONLY:
		f |= MIDI_OWRITE;
		break;
	case O_RDWR:
		f |= MIDI_OREAD | MIDI_OWRITE;
		break;
	}
	if (MINOR(inode->i_rdev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	(void)gen_midiclose(&midi_sc[MINOR(inode->i_rdev) & 0x3f].s, f);

	MOD_DEC_USE_COUNT;
}

int
midiread(inode, file, buf, len)
	struct inode *inode;
	struct file *file;
	char *buf;
	int len;
{
	struct uio uio;
	struct linux_midi_softc *lsc;
	struct midi_softc *softc;
	int f, err;

	uio.s = buf;
	uio.len = 0;
	uio.pos = buf;
	uio.remain = len;

	lsc = &midi_sc[MINOR(inode->i_rdev) & 0x3f];
	softc = &lsc->s;

	f = 0;
	if (lsc->local_status & MIDI_NONBLOCK)
		f |= MIDI_NDELAY;
	if (MINOR(inode->i_rdev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	err = gen_midiread(softc, &uio, f);
	if (err != 0)
		return (-err);
	return (len - uio.remain);
}

/* file_ops entry (write) change to const char *) in 1.3.15 */
int
midiwrite(inode, file, buf, len)
	struct inode *inode;
	struct file *file;
#if (LINUX_VERSION_CODE < 0x01030F)
	char *buf;
#else
	const char *buf;
#endif
	int len;
{
	struct uio uio;
	struct linux_midi_softc *lsc;
	struct midi_softc *softc;
	int f, err;

	uio.s = (char *)buf;
	uio.len = 0;
	uio.pos = (char *)buf;
	uio.remain = len;

	lsc = &midi_sc[MINOR(inode->i_rdev) & 0x3f];
	softc = &lsc->s;

	f = 0;
	if (lsc->local_status & MIDI_NONBLOCK)
		f |= MIDI_NDELAY;
	if (MINOR(inode->i_rdev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	err = gen_midiwrite(softc, &uio, f);
	if (err != 0)
		return (-err);
	return (len - uio.remain);
}

void
midiintr(irq, reg)
	int irq;
	struct pt_regs *reg;
{
	struct midi_softc *softc;
	struct midi_isa_iface *loc;
	int i;

	for (i = 0; i < NumMidi; i++) {
		loc = (struct midi_isa_iface *)midi_sc[i].s.iface->loc;
		if (loc->irq == irq)
			break;
	}
	if (i == NumMidi) {
		printk("midi: ivec(%d) is not a valid MIDI intr number\n",
		    irq);
		return;
	}
	softc = &(midi_sc[i].s);

	gen_midiintr(softc);
}

int
midiioctl(inode, file, cmd, arg)
	struct inode *inode;
	struct file *file;
	unsigned int cmd;
	unsigned long arg;
{
	struct midi_softc *softc;
	int f;

	f = 0;
	if (MINOR(inode->i_rdev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	softc = &(midi_sc[MINOR(inode->i_rdev) & 0x3f].s);

	return (-gen_midiioctl(softc, cmd, (void *)arg, f));
}

/*
 * gen_midiselect 
 * I don't have a good way of generalizing select yet, so it is done
 * on a per machine basis.
 */
int
midiselect(inode, file, type, wait)
	struct inode *inode;
	struct file *file;
	int type;
	select_table *wait;
{
	register struct midi_softc *softc;

	softc = &(midi_sc[MINOR(inode->i_rdev) & 0x3f].s);

	switch (type) {
	case SEL_IN:
		if (!(softc->status & MIDI_RD_BLOCK))
			return (1);
		else {
			softc->status |= MIDI_RSEL;
			select_wait(&softc->rsel, wait);
			return (0);
		}
		break;
	case SEL_OUT:
		if (!(softc->status & MIDI_WR_BLOCK))
			return (1);
		else {
			softc->status |= MIDI_WSEL;
			select_wait(&softc->wsel, wait);
			return (0);
		}
		break;
	case SEL_EX:
		if (softc->status & MIDI_TIME_WARP) {
			softc->status &= ~MIDI_TIME_WARP;
			return (1);
		} else {
			softc->status |= MIDI_ESEL;
			select_wait(&softc->esel, wait);
			return (0);
		}
	default:
		return (0);
	}
}

void *
MALLOC_NOWAIT(size)
	unsigned int size;
{

	return (kmalloc(size, GFP_ATOMIC));
}

void *
MALLOC_WAIT(size)
	unsigned int size;
{

	return (kmalloc(size, GFP_KERNEL));
}

void
FREE(obj, size)
	void *obj;
	unsigned int size;
{

	kfree_s(obj, size);
}


int
MOVE_TO_UIO(uio, s, num)
	void *uio;
	char *s;
	int num;
{
	struct uio *u = (struct uio *)uio;

	memcpy_tofs(u->pos, s, num);
	u->pos += num;
	u->len += num;
	u->remain -= num;
	return (0);
}

int
UWRITEC(uio)
	void *uio;
{
	struct uio *u = (struct uio *)uio;
	u_char c;

	if (u->remain == 0)
		return (-1);
	memcpy_fromfs(&c, u->pos, 1);
	u->pos++;
	u->remain--;
	return (c);
}

struct midi_softc *
UNIT_TO_SOFTC(unit, client)
	int unit;
	void *client;
{
	struct linux_midi_softc *lsc = (struct linux_midi_softc *)client;

	if (unit >= NumMidi)
		return (NULL);
	return (&lsc[unit].s);
}

void
SIGNAL_PG(pgid, sig)
	int pgid;
	int sig;
{

	if (pgid < 0)
		kill_pg(-pgid, sig, 1);
	else
		kill_proc(pgid, sig, 1);
}

void
TIMEOUT(id, fn, arg, t)
	TIMEOUT_ID *id;
	TIMEOUT_FN fn;
	TIMEOUT_ARG arg;
	int t;
{

	/* expires changed from relative to absolute in version 1.03.14 */
#if (LINUX_VERSION_CODE < 0x01030E)
	id->expires = t;
#else
	id->expires = jiffies + t;
#endif
	id->data = arg;
	id->function = fn;
	add_timer(id);
}


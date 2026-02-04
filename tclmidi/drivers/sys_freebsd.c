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
/*
 * FreeBSD 2.0.5 specific driver entry points
 */
#include "midi.h"
#if NMIDI > 0

#include "sys_freebsd.h"
#include "sup_export.h"

struct isa_driver mididriver = {
	midiprobe, midiattach, "midi"
};

struct freebsd_midi_softc {
	struct	midi_softc s;
} midi_sc[NMIDI];

static struct kern_devconf kdc_midi[NMIDI] = { {
	0, 0, 0,		/* filled in by dev_attach */
	"midi", 0, {MDDT_ISA, 0, "tty"},
	isa_generic_externalize, 0, 0, ISA_EXTERNALLEN,
	&kdc_isa0,		/* parent */
	0,			/* parentdata */
	DC_UNKNOWN,		/* state */
	"MIDI interface",
	DC_CLS_MISC		/* class */
} };

static void
midiregisterdev(id)
	struct isa_device *id;
{
	int unit;

	unit = id->id_unit;
	if (unit != 0)
		kdc_midi[unit] = kdc_midi[0];
	kdc_midi[unit].kdc_unit = unit;
	kdc_midi[unit].kdc_isa = id;
	dev_attach(&kdc_midi[unit]);
}


int
midiprobe(dev)
	struct isa_device *dev;
{
	struct midi_iface *iface;
	struct midi_isa_iface loc, *new_loc;
	int s;

	midiregisterdev(dev);

	loc.io_port = dev->id_iobase;
	loc.irq = ffs(dev->id_irq) - 1;

	/* XXX does id_flags come from the config file? */
	iface = midi_dev_probe(NULL, dev->id_flags, MIDI_ISA_IFACE, &loc);
	if (iface == NULL)
		return (0);

	/*
	 * we'll do this for real in attach when we have a real softc
	 * to put it in.
	 */
	iface->free(iface);

	return (1);
}

int
midiattach(isdp)
	struct isa_device *isdp;
{
	struct freebsd_midi_softc *freebsd_softc;
	struct midi_softc *softc;
	struct midi_isa_iface loc;

	freebsd_softc = &midi_sc[isdp->id_unit];
	softc = &freebsd_softc->s;

	loc.io_port = isdp->id_iobase;
	loc.irq = ffs(isdp->id_irq) - 1;

	if (!midi_init_dev(softc, isdp->id_flags, MIDI_ISA_IFACE, &loc))
		return (0);

	printf(" %s\n", softc->iface->name(softc->iface));

	/* allocate memory for event queues */
	if ((softc->rqueue = malloc(sizeof(struct event_queue), M_DEVBUF,
	    M_NOWAIT)) == NULL) {
		log(LOG_ERR, "No memory for rqueue\n");
		softc->status |= MIDI_UNAVAILABLE;
		return (0);
	}
	if ((softc->wqueue = malloc(sizeof(struct event_queue), M_DEVBUF,
	    M_NOWAIT)) == NULL) {
		log(LOG_ERR, "No memory for wqueue\n");
		softc->status |= MIDI_UNAVAILABLE;
		free(softc->rqueue, M_DEVBUF);
		return (0);
	}
	if ((softc->thruqueue = malloc(sizeof(struct event_queue), M_DEVBUF,
	    M_NOWAIT)) == NULL) {
		log(LOG_ERR, "No memory for thruqueue\n");
		softc->status |= MIDI_UNAVAILABLE;
		free(softc->rqueue, M_DEVBUF);
		free(softc->wqueue, M_DEVBUF);
		return (0);
	}
	/* zero read/write queue to clear stynamic structures */
	bzero(softc->rqueue, sizeof(struct event_queue));
	bzero(softc->wqueue, sizeof(struct event_queue));
	bzero(softc->thruqueue, sizeof(struct event_queue));
	stynamic_init(&softc->rpartial);
	stynamic_init(&softc->wpartial);
	softc->wpartialpos = 0;

	return (1);
}

int
midiopen(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	int f;

	f = 0;
	if (flag & FREAD)
		f |= MIDI_OREAD;
	if (flag & FWRITE)
		f |= MIDI_OWRITE;
	return (gen_midiopen((void *)dev, f, p->p_pid, midi_sc));
}

int
midiclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	int f, unit;
	struct freebsd_midi_softc *sc;

	unit = minor(dev) & 0x3f;
	f = 0;
	if (flag & FREAD)
		f |= MIDI_OREAD;
	if (flag & FWRITE)
		f |= MIDI_OWRITE;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;

	sc = &midi_sc[unit];
	return (gen_midiclose(&sc->s, f));
}

int
midiread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	int f, unit;
	struct freebsd_midi_softc *sc;

	f = 0;
	if (flag & IO_NDELAY)
		f |= MIDI_NDELAY;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;

	unit = minor(dev) & 0x3f;
	sc = &midi_sc[unit];
	return (gen_midiread(&sc->s, uio, f));
}

int
midiwrite(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	int f, unit;
	struct freebsd_midi_softc *sc;

	f = 0;
	if (flag & IO_NDELAY)
		f |= MIDI_NDELAY;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;

	unit = minor(dev) & 0x3f;
	sc = &midi_sc[unit];
	return (gen_midiwrite(&sc->s, uio, f));
}

void
midiintr(unit)
	int unit;
{
	struct freebsd_midi_softc *softc;

	softc = &midi_sc[unit];
	gen_midiintr(&softc->s);
}

int
midiioctl(dev, cmd, data, flag, p)
	dev_t dev;
	int cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	int f, unit;
	struct freebsd_midi_softc *sc;

	f = 0;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	unit = minor(dev) & 0x3f;
	sc = &midi_sc[unit];
	return (gen_midiioctl(&sc->s, cmd, data, f));
}

/*
 * gen_midiselect
 * I don't have a good way of generalizing select yet, so it is done
 * on a per machine basis.
 */
int
midiselect(dev, which, p)
	dev_t dev;
	int which;
	struct proc *p;
{
	struct freebsd_midi_softc *sc;
	struct midi_softc *softc;
	int ret, s, unit;

	unit = minor(dev) & 0x3f;
	sc = &midi_sc[unit];
	softc = &sc->s;

	s = spltty();
	switch (which) {
	case FREAD:
		if (!(softc->status & MIDI_RD_BLOCK))
			ret = 1;
		else {
			ret = 0;
			softc->status |= MIDI_RSEL;
			selrecord(p, &softc->rsel);
		}
		break;
	case FWRITE:
		if (!(softc->status & MIDI_WR_BLOCK))
			ret = 1;
		else {
			ret = 0;
			softc->status |= MIDI_WSEL;
			selrecord(p, &softc->wsel);
		}
		break;
	default:
		if (softc->status & MIDI_TIME_WARP) {
			softc->status &= ~MIDI_TIME_WARP;
			ret = 1;
		} else {
			ret = 0;
			softc->status |= MIDI_ESEL;
			selrecord(p, &softc->esel);
		}
		break;
	}
	splx(s);
	return (ret);
}

struct midi_softc *
UNIT_TO_SOFTC(unit, client)
	int unit;
	void *client;
{
	struct freebsd_midi_softc *softcs = (struct freebsd_midi_softc *)client;
	struct freebsd_midi_softc *sc;
	struct midi_softc *softc;

	if (unit >= NMIDI || (sc = &softcs[unit]) == NULL) {
		softc = NULL;
		return (softc);
	}
	softc = &sc->s;
	return (softc);
}

void
SIGNAL_PG(pgid, sig)
	int pgid, sig;
{
	struct proc *p;

	if (pgid < 0)
		gsignal(-pgid, sig);
	else if ((p = pfind(pgid)) != 0)
		psignal(p, sig);
}

void
TIMEOUT(id, fn, arg, t)
	TIMEOUT_ID *id;
	TIMEOUT_FN fn;
	TIMEOUT_ARG arg;
	int t;
{

	timeout(fn, arg, t);
	*id = fn;
}

/*
 * Get next character written in by user from uio.
 * On NetBSD 1.0beta (FreeBSD too?), this is in kern_subr.c but
 * within #ifdef vax ... endif
 */
int
uwritec(uio)
	struct uio *uio;
{
	register struct iovec *iov;
	register int c;

	if (uio->uio_resid <= 0)
		return (-1);
again:
	if (uio->uio_iovcnt <= 0)
		panic("ureadc: non-positive iovcnt");
	iov = uio->uio_iov;
	if (iov->iov_len == 0) {
		uio->uio_iov++;
		if (--uio->uio_iovcnt == 0)
			return (-1);
		goto again;
	}
	switch (uio->uio_segflg) {

	case UIO_USERSPACE:
		c = fubyte(iov->iov_base);
		break;

	case UIO_SYSSPACE:
		c = *(u_char *) iov->iov_base;
		break;
	case UIO_USERISPACE:
		c = fuibyte(iov->iov_base);
		break;
	}
	if (c < 0)
		return (-1);
	iov->iov_base++;
	iov->iov_len--;
	uio->uio_resid--;
	uio->uio_offset++;
	return (c);
}
#endif

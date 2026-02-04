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
 * BSDI 2.0 specific driver entry points
 */

#include "sys_bsdi2.0.h"
#include "sup_export.h"

struct cfdriver midicd = {
	NULL, "midi", midiprobe, midiattach, DV_DULL,
	sizeof(struct bsdi20_midi_softc)
};

struct devsw midisw = {
	&midicd,
	midiopen, midiclose, midiread, midiwrite, midiioctl, midiselect,
	nommap, nostrat, nodump, nopsize, 0, nostop
};

int
midiprobe(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	register struct isa_attach_args *ia = (struct isa_attach_args *)aux;
	struct midi_iface *iface;
	struct midi_isa_iface loc, *new_loc;
	int s;

	loc.io_port = ia->ia_iobase;
	if (ia->ia_irq == IRQUNK)
		loc.irq = -1;
	else
		loc.irq = ffs(ia->ia_irq) - 1;

	iface = midi_dev_probe(NULL, cf->cf_flags, MIDI_ISA_IFACE, &loc);
	if (iface == NULL)
		return (0);

	new_loc = (struct midi_isa_iface *)iface->loc;
	/* should be able to do a reset if it exists */
	if (new_loc->irq != -1)
		ia->ia_irq = (1 << new_loc->irq);
	else {
		ia->ia_irq = isa_discoverintr(iface->gen_intr, iface);
		if (ffs(ia->ia_irq) - 1 == 0)
			return (0);
	}

	iface->size(iface, &s);
	ia->ia_iosize = s;
	ia->ia_msize = 0;

	/*
	 * we'll do this for real in attach when we have a real softc
	 * to put it in.
	 */
	iface->free(iface);

	return (1);
}

void
midiattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	register struct isa_attach_args *ia = (struct isa_attach_args *)aux;
	struct bsdi20_midi_softc *bsdi_softc;
	struct midi_softc *softc;
	struct midi_isa_iface loc;

	bsdi_softc = (struct bsdi20_midi_softc *)self;
	softc = &bsdi_softc->s;

	loc.io_port = ia->ia_iobase;
	loc.irq = ffs(ia->ia_irq) - 1;

	if (!midi_init_dev(softc, bsdi_softc->sc_dev.dv_flags, MIDI_ISA_IFACE,
	    &loc))
		return;

	printf(" %s\n", softc->iface->name(softc->iface));

	/* allocate memory for event queues */
	if ((softc->rqueue = malloc(sizeof(struct event_queue), M_DEVBUF,
	    M_NOWAIT)) == NULL) {
		log(LOG_ERR, "No memory for rqueue\n");
		softc->status |= MIDI_UNAVAILABLE;
		return;
	}
	if ((softc->wqueue = malloc(sizeof(struct event_queue), M_DEVBUF,
	    M_NOWAIT)) == NULL) {
		log(LOG_ERR, "No memory for wqueue\n");
		softc->status |= MIDI_UNAVAILABLE;
		free(softc->rqueue, M_DEVBUF);
		return;
	}
	if ((softc->thruqueue = malloc(sizeof(struct event_queue), M_DEVBUF,
	    M_NOWAIT)) == NULL) {
		log(LOG_ERR, "No memory for thruqueue\n");
		softc->status |= MIDI_UNAVAILABLE;
		free(softc->rqueue, M_DEVBUF);
		free(softc->wqueue, M_DEVBUF);
		return;
	}
	/* zero read/write queue to clear stynamic structures */
	bzero(softc->rqueue, sizeof(struct event_queue));
	bzero(softc->wqueue, sizeof(struct event_queue));
	bzero(softc->thruqueue, sizeof(struct event_queue));
	stynamic_init(&softc->rpartial);
	stynamic_init(&softc->wpartial);
	softc->wpartialpos = 0;

	isa_establish(&bsdi_softc->sc_id, &bsdi_softc->sc_dev);
        bsdi_softc->sc_ih.ih_fun = midiintr;
        bsdi_softc->sc_ih.ih_arg = bsdi_softc;
	intr_establish(ia->ia_irq, &bsdi_softc->sc_ih, DV_TTY);
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
	return (gen_midiopen((void *)dev, f, p->p_pid, &midicd));
}

int
midiclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	int f, unit;
	struct bsdi20_midi_softc *sc;

	unit = minor(dev) & 0x3f;
	f = 0;
	if (flag & FREAD)
		f |= MIDI_OREAD;
	if (flag & FWRITE)
		f |= MIDI_OWRITE;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;

	sc = midicd.cd_devs[unit];
	return (gen_midiclose(&sc->s, f));
}

int
midiread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	int f, unit;
	struct bsdi20_midi_softc *sc;

	f = 0;
	if (flag & IO_NDELAY)
		f |= MIDI_NDELAY;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;

	unit = minor(dev) & 0x3f;
	sc = midicd.cd_devs[unit];
	return (gen_midiread(&sc->s, uio, f));
}

int
midiwrite(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	int f, unit;
	struct bsdi20_midi_softc *sc;

	f = 0;
	if (flag & IO_NDELAY)
		f |= MIDI_NDELAY;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;

	unit = minor(dev) & 0x3f;
	sc = midicd.cd_devs[unit];
	return (gen_midiwrite(&sc->s, uio, f));
}

int
midiintr(softc)
	struct bsdi20_midi_softc *softc;
{

	return(gen_midiintr(&softc->s));
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
	struct bsdi20_midi_softc *sc;

	f = 0;
	if (minor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	unit = minor(dev) & 0x3f;
	sc = midicd.cd_devs[unit];
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
	struct bsdi20_midi_softc *sc;
	struct midi_softc *softc;
	int ret, s, unit;

	unit = minor(dev) & 0x3f;
	sc = midicd.cd_devs[unit];
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
	struct cfdriver *midicd = (struct cfdriver *)client;
	struct bsdi20_midi_softc *sc;
	struct midi_softc *softc;

	if (unit >= midicd->cd_ndevs || (sc = midicd->cd_devs[unit])
	    == NULL) {
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

int
FIND_IRQ(irqmask)
	int irqmask;
{
	int found_mask;

	found_mask = isa_irqalloc(irqmask);
	if (found_mask == 0)
		return (-1);
	else
		return (ffs(found_mask) - 1);
}

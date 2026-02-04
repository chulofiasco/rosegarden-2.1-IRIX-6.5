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
 * UnixWare driver dependent code
 */
#include "sys_unixware.h"
#include "sup_export.h"

extern struct unixware_midi_softc midi_sc[];
extern int midi_addrs[];
extern int midi_intrs[];
extern int NumMidi;

/*
 * That's new in svr4.2, all drivers must declare such a variable in the
 * form 'PFXdevflag'.  Settings are explained in 'sys/conf.h'.
 */
int mididevflag = D_NEW;

#ifdef LOADABLE /* Very usefull when porting, testing ... */

#include "version.h"
#define DRVNAME "Midi driver for MPU401 compatible boards in UART mode"

static int midi_load(void);
static int midi_unload(void);

MOD_DRV_WRAPPER(midi, midi_load, midi_unload, NULL, DRVNAME);

static int
midi_load(void)
{

	cmn_err(CE_CONT, "Loading [## %s ##]\n", version);

	midiinit();

	/*
	 * There must be a test before loading the driver.
	 */

	/* I don't know the returned type */
	mod_drvattach(&midi_attach_info);
	return (0);
}

static int
midi_unload(void)
{
	struct unixware_midi_softc *usc;
	struct midi_softc *softc;
	int i;

	cmn_err(CE_WARN, "Unloading [## %s ##]\n", version);

	/* don't free anything until all devices are closed */
	for (i = 0; i < NumMidi; i++) {
		usc = &midi_sc[i];
		softc = &usc->s;
		if (softc->status & MIDI_OPEN) {
			cmn_err(CE_WARN,
			    "midi device #%d in use.  Try again later\n", i);
			return 1;
		}
	}
	for (i = 0; i < NumMidi; i++) {
		usc = &midi_sc[i];
		softc = &usc->s;
		if (softc->status & MIDI_UNAVAILABLE)
			continue;
		midi_fullreset(softc);
		kmem_free(softc->rqueue, sizeof(struct event_queue));
		kmem_free(softc->wqueue, sizeof(struct event_queue));
		kmem_free(softc->thruqueue, sizeof(struct event_queue));
		softc->iface->free(softc->iface);
		softc->timer->release_id(softc->timeout_id);
		softc->timer->release_id(softc->thru_timeout_id);
		softc->timer->free(softc->timer);
	}
	mod_drvdetach(&midi_attach_info);
	return 0;
}
#endif /* LOADABLE */

void
midiinit()
{
	struct unixware_midi_softc *usc;
	struct midi_softc *softc;
	struct midi_isa_iface loc, *ploc;
	int i;

	for (i = 0; i < NumMidi; i++) {
		usc = &midi_sc[i];
		softc = &usc->s;

		loc.irq = midi_intrs[i];
		loc.io_port = midi_addrs[i];

		/* XXX!!! Got to get a flags field from somewhere */
		if (!midi_init_dev(softc, midi_types[i], MIDI_ISA_IFACE, &loc))
			continue;
		if ((softc->rqueue = kmem_alloc(sizeof(struct event_queue),
		    KM_NOSLEEP)) == NULL) {
			printf("No memory for rqueue\n");
			softc->status |= MIDI_UNAVAILABLE;
			continue;
		}
		if ((softc->wqueue = kmem_alloc(sizeof(struct event_queue),
		    KM_NOSLEEP)) == NULL) {
			printf("No memory for wqueue\n");
			softc->status |= MIDI_UNAVAILABLE;
			kmem_free(softc->rqueue, sizeof(struct event_queue));
			continue;
		}
		if ((softc->thruqueue = kmem_alloc(sizeof(struct event_queue),
		    KM_NOSLEEP)) == NULL) {
			printf("No memory for thruqueue\n");
			softc->status |= MIDI_UNAVAILABLE;
			kmem_free(softc->rqueue, sizeof(struct event_queue));
			kmem_free(softc->wqueue, sizeof(struct event_queue));
			continue;
		}
		/* zero read/write queue to clear stynamic structures */
		bzero((caddr_t)softc->rqueue, sizeof(struct event_queue));
		bzero((caddr_t)softc->wqueue, sizeof(struct event_queue));
		bzero((caddr_t)softc->thruqueue, sizeof(struct event_queue));
		stynamic_init(&softc->rpartial);
		stynamic_init(&softc->wpartial);
		softc->wpartialpos = 0;
		ploc = (struct midi_isa_iface *)softc->iface->loc;
		printf("Found %s #%d at 0x%02x irq %d\n",
		    softc->iface->name(softc->iface), i, ploc->io_port,
		    ploc->irq);
	}
}

int
midiopen(dev, flag, otyp, cred_p)
	dev_t *dev;
	int flag, otyp;
	cred_t *cred_p;
{
	struct unixware_midi_softc *usc;
	int f;

	f = 0;
	if (flag & FREAD)
		f |= MIDI_OREAD;
	if (flag & FWRITE)
		f |= MIDI_OWRITE;

	usc = &midi_sc[getminor(*dev) & 0x3f];
	usc->local_status = 0;
	if (flag & FNONBLOCK)
		usc->local_status |= MIDI_NONBLOCK;
	return (gen_midiopen(dev, f, u.u_procp->p_pid, midi_sc));
}

int
midiclose(dev, flag, otyp, cred_p)
	dev_t dev;
	int flag, otyp;
	cred_t *cred_p;
{
	struct midi_softc *softc;
	int f, unit;

	unit = getminor(dev) & 0x3f;
	f = 0;
	if (flag & FREAD)
		f |= MIDI_OREAD;
	if (flag & FWRITE)
		f |= MIDI_OWRITE;
	if (getminor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	softc = &(midi_sc[unit].s);

	return (gen_midiclose(softc, f));
}

int
midiread(dev, uio, cred_p)
	dev_t dev;
	uio_t *uio;
	cred_t *cred_p;
{
	struct unixware_midi_softc *usc;
	struct midi_softc *softc;
	int f;

	usc = &midi_sc[getminor(dev) & 0x3f];
	softc = &usc->s;

	f = 0;
	if (usc->local_status & MIDI_NONBLOCK)
		f |= MIDI_NDELAY;
	if (getminor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	return (gen_midiread(softc, uio, f));
}

int
midiwrite(dev, uio, cred_p)
	dev_t dev;
	uio_t *uio;
	cred_t *cred_p;
{
	struct unixware_midi_softc *usc;
	struct midi_softc *softc;
	int f;

	usc = &midi_sc[getminor(dev) & 0x3f];
	softc = &usc->s;

	f = 0;
	if (usc->local_status & MIDI_NONBLOCK)
		f |= MIDI_NDELAY;
	if (getminor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	return (gen_midiwrite(softc, uio, f));
}

int
midiintr(ivec)
	int ivec;
{
	struct midi_softc *softc;
	struct midi_isa_iface *loc;
	int i;

	for (i = 0; i < NumMidi; i++) {
		loc = (struct midi_isa_iface *)midi_sc[i].s.iface->loc;
		if (loc->irq == ivec)
			break;
	}
	if (i == NumMidi) {
		printf("midi: ivec(%d) is not a valid MIDI intr number\n",
		    ivec);
		return (0);
	}
	softc = &midi_sc[i].s;

	gen_midiintr(softc);
	return (1);
}

int
midiioctl(dev, cmd, arg, mode, cred_p, rval_p)
	dev_t dev;
	int cmd, arg, mode;
	cred_t *cred_p;
	int *rval_p;
{
	struct midi_softc *softc;
	int f;

	f = 0;
	if (getminor(dev) & 0x40)
		f |= MIDI_RAW_DEVICE;
	softc = &(midi_sc[getminor(dev) & 0x3f].s);

	return (gen_midiioctl(softc, cmd, &arg, f));
}

/*
 * gen_midiselect 
 * I don't have a good way of generalizing select yet, so it is done
 * on a per machine basis.
 */
int
midichpoll(dev, events, anyyet, reventsp, phpp)
	dev_t dev;
	short events;
	int anyyet;
	short *reventsp;
	struct pollhead **phpp;
{
	struct midi_softc *softc;
	int unit;

	unit = getminor(dev) & 0x3f;
	softc = &midi_sc[unit].s;

	*reventsp = 0;
	if (events & POLLIN && !(softc->status & MIDI_RD_BLOCK))
		*reventsp |= POLLIN;
	if (events & POLLOUT && !(softc->status & MIDI_WR_BLOCK))
		*reventsp |= POLLOUT;

	/* to avoid a warning */
	if (events & POLLPRI && (softc->status & MIDI_TIME_WARP)) {
		softc->status &= ~MIDI_TIME_WARP;
		*reventsp |= POLLPRI;
	}
	if (*reventsp == 0)
		if (!anyyet)
			*phpp = &softc->rsel;
	return (0);
}

struct midi_softc *
UNIT_TO_SOFTC(unit, client)
	int unit;
	void *client;
{
	struct unixware_midi_softc *usc = (struct unixware_midi_softc *)client;

	if (unit >= NumMidi)
		return (NULL);
	return (&usc[unit].s);
}

void
SIGNAL_PG(pgid, sig)
	int pgid;
	int sig;
{
	struct proc *p;

	if (pgid < 0)
		signal(-pgid, sig);
	else if ((p = prfind(pgid)) != 0)
		psignal(p, sig);
}

void
TIMEOUT(id, fn, arg, t)
	TIMEOUT_ID *id;
	TIMEOUT_FN fn;
	TIMEOUT_ARG arg;
	int t;
{

	*id = timeout(fn, arg, t);
}

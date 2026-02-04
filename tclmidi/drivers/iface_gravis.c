/*-
 * Copyright (c) 1996 Michael B. Durian.  All rights reserved.
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

#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_GRAVIS)
#include "sup_os.h"
#include "sup_iface.h"
#include "sup_pnp.h"
#include "iface_gravis.h"

static struct midi_iface_gravis *gravis_setup(struct midi_softc *softc,
    struct midi_isa_iface *loc);
static void gravis_irq_setup(struct midi_iface_gravis *iface);
static const char *gravis_name(struct midi_iface *mif);
static void gravis_size(struct midi_iface *mif, void *size);
static void gravis_gen_intr(struct midi_iface *mif);
static int gravis_big_reset(struct midi_iface *mif);
static int gravis_open(struct midi_iface *mif);
static void gravis_close(struct midi_iface *mif);
static int gravis_intr(struct midi_iface *mif);
static int gravis_data_avail(struct midi_iface *mif);
static void gravis_write(struct midi_iface *mif, struct event *event);
static u_char gravis_read(struct midi_iface *mif);
static int gravis_feature(struct midi_iface *iface,
    struct midi_feature *feature);
static void gravis_free(struct midi_iface *mif);

static void enter_enhanced_mode(struct midi_iface_gravis *iface);
static void leave_enhanced_mode(struct midi_iface_gravis *iface);
static int wait_rdy_rcv(struct midi_iface_gravis *iface);
static void midi_reset(struct midi_iface_gravis *iface);
static int find_irq(u_short port, int internal);
static int irq_to_bitmap(int irq);
static u_char irq_bitmap(struct midi_iface_gravis *iface, int irq);
static int gf1_ping(u_short port);
static void register_poke(u_short port, u_char reg, u_short val);
static u_short register_peek(u_short port, u_char reg);
static void poke_data(u_short port, u_long address, u_char data);
static u_char peek_data(u_short port, u_long address);
static void poke_data_l(u_short port, u_long address, u_long data);
static u_long peek_data_l(u_short port, u_long address);
static void poke_block(struct midi_iface_gravis *iface, u_char *block,
    u_long len, u_long addr);
static void iwave_gus_reset(unsigned short port);

static u_long addr_trans(struct midi_iface_gravis *iface, u_long addr);


static void gravis_mem_init(struct midi_iface_gravis *iface);
static void gravis_mem_free(struct midi_iface_gravis *iface, u_long addr,
    u_long size);
static void gravis_mem_merge(struct midi_iface_gravis *iface);
static u_long gravis_mem_alloc(struct midi_iface_gravis *iface, u_long size);
static u_long gravis_mem_max_alloc(struct midi_iface_gravis *iface);
static u_long gravis_mem_avail(struct midi_iface_gravis *iface);
static u_long gravis_mem_size(struct midi_iface_gravis *iface);

static int load_patch(struct midi_iface_gravis *iface,
    struct gravis_patch *patch);
static int delete_patch(struct midi_iface_gravis *iface,
    struct gravis_patch *patch);
static void flush_patches(struct midi_iface_gravis *iface);

static void gravis_internal_event(struct midi_iface_gravis *iface,
    struct event *event);

static void stop_voice_now(struct midi_iface_gravis *iface,
    struct gravis_voice *v);

static void note_off_event(struct midi_iface_gravis *iface, int channel,
    int pitch);
static void note_on_event(struct midi_iface_gravis *iface, int channel,
    int pitch, int vel);
static void key_pressure_event(struct midi_iface_gravis *iface, int channel,
    int pitch, int vel);
static void parameter_event(struct midi_iface_gravis *iface, int channel,
    int param, int val);
static void program_event(struct midi_iface_gravis *iface, int channel,
    int val);
static void chan_pressure_event(struct midi_iface_gravis *iface, int channel,
    int pressure);
static void pitch_wheel_event(struct midi_iface_gravis *iface, int channel,
    int val);
static void real_time_event(struct midi_iface_gravis *iface, int mode);

static short compute_volume(struct midi_iface_gravis *iface,
    struct gravis_lm_wave *w, int vel);
static int pitch_to_freq(int pitch);
static u_short freq_to_gravis(struct midi_iface_gravis *iface,
    int note_freq);

/* Configure memory in Iwave */
static unsigned short iwave_mem_size(struct midi_iface_gravis *iface);
static unsigned long iwave_mem_cfg(struct midi_iface_gravis *iface);

static int note_freqs[] = {261632, 277189, 293671, 311132, 329632,
    349232, 369998, 391998, 415306, 440000, 466162, 493880};

static u_short freq_divisor[19] = {44100, 41160, 38587, 36317, 34300,
    32494, 30870, 29400, 28063, 26843, 25725, 24696, 23746, 22866, 22050,
    21289, 20580, 19916, 19293};

/*
 * keep a list of all gravis devices so we can pass information from
 * on gravis device to another if they share the same physical board.
 */
struct gravis_loc {
	struct	midi_isa_iface loc;
	int	internal;
	unsigned	char csn;	/* if a PnP board */
	struct	gravis_loc *next;
};

static struct gravis_loc *gravis_boards = NULL;

struct midi_iface *
gravis_internal_gf1_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *vloc)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	struct gravis_loc *l;
	struct gravis_loc *b;
	unsigned char mix;

	if (type != MIDI_ISA_IFACE)
		return (NULL);
	loc = (struct midi_isa_iface *)vloc;

	for (b = gravis_boards; b != NULL; b = b->next)
		if (b->loc.io_port == loc->io_port)
			break;

	if (b == NULL) {
		iwave_gus_reset(loc->io_port);

		if (!gf1_ping(loc->io_port))
			return (NULL);
	}
	iface = gravis_setup(softc, loc);
	if (iface == NULL)
		return (NULL);
	iface->internal = 1;
	iface->iwave = 0;

	gravis_irq_setup(iface);

	/* add location to our list of know gravis devices */
	l = MALLOC_NOWAIT(sizeof(struct gravis_loc));
	if (l == NULL) {
		gravis_free((struct midi_iface *)iface);
		return (NULL);
	}
	l->loc = *(struct midi_isa_iface *)iface->iface.loc;
	l->internal = 1;
	l->csn = 0;
	l->next = gravis_boards;
	gravis_boards = l;

	/* why would there be reserved mem? */
	iface->reserved_mem = 0;

	/* determine the amount of memory on the board */
	iface->mem_size = gravis_mem_size(iface);

	sprintf(iface->name, "Gravis GF1 Internal Voices %ldk",
	    iface->mem_size);

	return ((struct midi_iface *)iface);
}

struct midi_iface *
gravis_internal_iwave_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *vloc)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	struct gravis_loc *l;
	struct gravis_loc *b;
	struct midi_pnp_config c;
	unsigned char csn, max_csn;

	if (type != MIDI_ISA_IFACE)
		return (NULL);
	loc = (struct midi_isa_iface *)vloc;

	max_csn = 0;
	for (b = gravis_boards; b != NULL; b = b->next) {
		if (b->csn > max_csn)
			max_csn = b->csn;
		if (b->loc.io_port == loc->io_port)
			break;
	}

	if (b != NULL)
		csn = b->csn;
	else {
		/* first we isolate the PnP boards */
		/*
		 * ignore the return value - a soft reboot won't reset
		 * the CSN's to 0 and isolate won't find these cards
		 */
		if (midi_pnp_isol() == -1)
			csn = midi_pnp_ping_no_isol(max_csn,
			    GRAVIS_PNP_VENDOR_ID);
		else
			csn = midi_pnp_ping(max_csn,
			    GRAVIS_PNP_VENDOR_ID);
		if (csn == 0)
			return (NULL);
	}

	iface = gravis_setup(softc, loc);
	if (iface == NULL)
		return (NULL);
	iface->internal = 1;
	iface->iwave = 1;

	/* configure this mother */
	c.flags = PNP_CONF_IO_PORT_0 | PNP_CONF_IO_PORT_1 | PNP_CONF_IO_PORT_2
	    | PNP_CONF_IRQ_0;
	c.io_port[0] = loc->io_port;
	c.io_port[1] = loc->io_port + 0x100;
	c.io_port[2] = loc->io_port + 0x10c;
	c.irq[0] = loc->irq;
	if (!midi_pnp_configure(csn, IWAVE_AUDIO, &c)) {
		midi_pnp_print_dev_config(csn, IWAVE_AUDIO);
		return (NULL);
	}

	iwave_gus_reset(loc->io_port);


	/* add location to our list of know gravis devices */
	l = MALLOC_NOWAIT(sizeof(struct gravis_loc));
	if (l == NULL) {
		gravis_free((struct midi_iface *)iface);
		return (NULL);
	}
	l->loc = *(struct midi_isa_iface *)iface->iface.loc;
	l->internal = 1;
	l->csn = csn;
	l->next = gravis_boards;
	gravis_boards = l;

	/* why would there be reserved mem? */
	iface->reserved_mem = 0;

	/* determine the amount of memory on the board */
	(void)iwave_mem_cfg(iface);
	iface->mem_size = iwave_mem_size(iface);

	gravis_irq_setup(iface);

	sprintf(iface->name, "Gravis Iwave Internal Voices %ldk",
	    iface->mem_size);

	return ((struct midi_iface *)iface);
}

struct midi_iface *
gravis_external_gf1_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *vloc)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	struct gravis_loc *l;
	struct gravis_loc *b;
	unsigned char mix;

	if (type != MIDI_ISA_IFACE)
		return (NULL);
	loc = (struct midi_isa_iface *)vloc;

	for (b = gravis_boards; b != NULL; b = b->next)
		if (b->loc.io_port == loc->io_port)
			break;

	if (b == NULL) {
		iwave_gus_reset(loc->io_port);

		if (!gf1_ping(loc->io_port))
			return (NULL);
	}
	iface = gravis_setup(softc, loc);
	if (iface == NULL)
		return (NULL);
	iface->internal = 0;
	iface->iwave = 0;

	gravis_irq_setup(iface);

	/* set receive IRQ and clear xmit IRQ */
	OUTB(loc->io_port + GRAVIS_MIDI_CONTROL, 0x80);

	/* add location to our list of know gravis devices */
	l = MALLOC_NOWAIT(sizeof(struct gravis_loc));
	if (l == NULL) {
		gravis_free((struct midi_iface *)iface);
		return (NULL);
	}
	l->loc = *(struct midi_isa_iface *)iface->iface.loc;
	l->internal = 0;
	l->csn = 0;
	l->next = gravis_boards;
	gravis_boards = l;

	sprintf(iface->name, "Gravis GF1 External Voices");

	return ((struct midi_iface *)iface);
}

struct midi_iface *
gravis_external_iwave_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type,
    void *vloc)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	struct gravis_loc *l;
	struct gravis_loc *b;
	struct midi_pnp_config c;
	unsigned char csn, max_csn;

	if (type != MIDI_ISA_IFACE)
		return (NULL);
	loc = (struct midi_isa_iface *)vloc;

	max_csn = 0;
	for (b = gravis_boards; b != NULL; b = b->next) {
		if (b->csn > max_csn)
			max_csn = b->csn;
		if (b->loc.io_port == loc->io_port)
			break;
	}

	if (b != NULL)
		csn = b->csn;
	else {
		/* first we isolate the PnP boards */
		/*
		 * ignore the return value - a soft reboot won't reset
		 * the CSN's to 0 and isolate won't find these cards
		 */
		if (midi_pnp_isol() == -1)
			csn = midi_pnp_ping_no_isol(max_csn,
			    GRAVIS_PNP_VENDOR_ID);
		else
			csn = midi_pnp_ping(max_csn, GRAVIS_PNP_VENDOR_ID);
		if (csn == 0)
			return (NULL);
	}

	iface = gravis_setup(softc, loc);
	if (iface == NULL)
		return (NULL);
	iface->internal = 0;
	iface->iwave = 1;

	/* configure this mother */
	c.flags = PNP_CONF_IO_PORT_0 | PNP_CONF_IRQ_1;
	c.io_port[0] = loc->io_port;
	c.irq[1] = loc->irq;
	if (!midi_pnp_configure(csn, IWAVE_AUDIO, &c)) {
		midi_pnp_print_dev_config(csn, IWAVE_AUDIO);
		return (NULL);
	}

	iwave_gus_reset(loc->io_port);

	/* add location to our list of know gravis devices */
	l = MALLOC_NOWAIT(sizeof(struct gravis_loc));
	if (l == NULL) {
		gravis_free((struct midi_iface *)iface);
		return (NULL);
	}
	l->loc = *(struct midi_isa_iface *)iface->iface.loc;
	l->internal = 0;
	l->csn = csn;
	l->next = gravis_boards;
	gravis_boards = l;

	gravis_irq_setup(iface);

	sprintf(iface->name, "Gravis Iwave External Voices");

	return ((struct midi_iface *)iface);
}

static struct midi_iface_gravis *
gravis_setup(struct midi_softc *softc, struct midi_isa_iface *loc)
{
	struct midi_iface_gravis *iface;
	int i, irqmask;

	/* if not irq specified, try to find one we can use. */
	if (loc->irq == -1) {
		irqmask = (1 << 2) | (1 << 3) | (1 << 5) | (1 << 7) |
		    (1 << 11) | (1 << 12) | (1 << 15);
		loc->irq = FIND_IRQ(irqmask);
		if (loc->irq == -1)
			return (NULL);
	}

	iface = MALLOC_NOWAIT(sizeof(struct midi_iface_gravis));
	if (iface == NULL)
		return (NULL);

	iface->iface.loc = MALLOC_NOWAIT(sizeof(struct midi_isa_iface));
	if (iface->iface.loc == NULL) {
		FREE(iface, sizeof(struct midi_iface_gravis));
		return (NULL);
	}

	/* zero out our voice structures */
	BZERO(iface->voices, NUM_VOICES *
	    sizeof(struct gravis_voice));
	for (i = 0; i < NUM_VOICES; i++) {
		iface->voices[i].voice_num = i;
		iface->voices[i].status = 0;
	}
	BZERO(iface->normal_programs, 128 * sizeof(struct gravis_voice *));
	BZERO(iface->drum_programs, 128 * sizeof(struct gravis_voice *));
	BZERO(iface->channel, 16 * sizeof(struct gravis_voice *));

	iface->cur_out_time = 0;

	/* copy location */
	iface->iface.type = MIDI_ISA_IFACE;
	*(struct midi_isa_iface *)iface->iface.loc = *loc;

	iface->iface.softc = softc;
	iface->iface.name = gravis_name;
	iface->iface.size = gravis_size;
	iface->iface.gen_intr = gravis_gen_intr;
	iface->iface.reset = gravis_big_reset;
	iface->iface.open = gravis_open;
	iface->iface.close = gravis_close;
	iface->iface.intr = gravis_intr;
	iface->iface.data_avail = gravis_data_avail;
	iface->iface.write = gravis_write;
	iface->iface.read = gravis_read;
	iface->iface.feature = gravis_feature;
	iface->iface.free = gravis_free;

	midi_reset(iface);
	return (iface);
}

static void
gravis_irq_setup(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	/* Set up for digital ASIC - what ever that means */
	OUTB(loc->io_port + GRAVIS_REGISTER_CONTROL, 0x05);
	OUTB(loc->io_port + GRAVIS_MIX, 0);
	OUTB(loc->io_port + GRAVIS_DMA_CONTROL, 0);
	OUTB(loc->io_port + GRAVIS_REGISTER_CONTROL, 0);

	/* set DMA channels */
	OUTB(loc->io_port + GRAVIS_MIX, 0);
	OUTB(loc->io_port + GRAVIS_DMA_CONTROL, 0);

	/* set IRQs */
	OUTB(loc->io_port + GRAVIS_MIX, MIX_CONTROL_REG_SELECT);
	OUTB(loc->io_port + GRAVIS_IRQ_CONTROL, irq_bitmap(iface, loc->irq));

	/* Do DMAs again */
	OUTB(loc->io_port + GRAVIS_MIX, 0);
	OUTB(loc->io_port + GRAVIS_DMA_CONTROL, 0);

	/* Do IRQs again */
	OUTB(loc->io_port + GRAVIS_MIX, MIX_CONTROL_REG_SELECT);
	OUTB(loc->io_port + GRAVIS_IRQ_CONTROL, irq_bitmap(iface, loc->irq));

	/* enable output & irq, disable line & mix out */
	OUTB(loc->io_port + GRAVIS_MIX, MIX_DISABLE_LINE_IN |
	    MIX_ENABLE_LATCHES);
	OUTB(loc->io_port + GRAVIS_DMA_CONTROL, 0);
}

static const char *
gravis_name(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;

	iface = (struct midi_iface_gravis *)mif;
	return (iface->name);
}

static void
gravis_size(struct midi_iface *mif, void *size)
{
	int *s;

	/*
	 * This is a lie.  The Gravis uses all sorts of I/O ports, but
	 * unfortunately, they are not contiguous.  Thus I'm just saying
	 * 0.
	 */
	s = (int *)size;
	*s = 0;
}

static void
gravis_gen_intr(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	unsigned char mix;

	iface = (struct midi_iface_gravis *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	/*
	 * set mix register so we'll latch the IRQ control, not
	 * the DMA control
	 */
	mix = INB(loc->io_port + GRAVIS_MIX);
	mix |= MIX_CONTROL_REG_SELECT;
	OUTB(loc->io_port + GRAVIS_MIX, mix);

	/* allow xmit irqs */
	OUTB(loc->io_port + GRAVIS_IRQ_CONTROL,
	    irq_bitmap(iface, loc->irq));
	/* now do the DMA setting */
	mix = INB(loc->io_port + GRAVIS_MIX);
	mix &= ~MIX_CONTROL_REG_SELECT;
	OUTB(loc->io_port + GRAVIS_MIX, mix);
	OUTB(loc->io_port + GRAVIS_DMA_CONTROL, 0);

	OUTB(loc->io_port + GRAVIS_MIDI_CONTROL, 0x20);

	/* reset board - this should generate an xmit buffer emptry irq */
	midi_reset(iface);
	/* disable xmit irqs */
	OUTB(loc->io_port + GRAVIS_MIDI_CONTROL, 0x00);
}

static int
gravis_big_reset(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	struct gravis_voice *v;
	int i;

	iface = (struct midi_iface_gravis *)mif;
	loc = (struct midi_isa_iface *)mif->loc;
	if (!iface->internal) {
		midi_reset(iface);
	} else {
		/* clear all interrupts */
		register_poke(loc->io_port, REGISTER_DMA_CONTROL, 0);
		register_poke(loc->io_port, REGISTER_TIMER_CONTROL, 0);
		if (!iface->iwave)
			register_poke(loc->io_port, REGISTER_SAMPLE_CONTROL, 0);

		/* set the number of active voices */
		register_poke(loc->io_port, VOICE_ACTIVE,
		    (NUM_VOICES - 1) | 0xc0);

		/* clear interrupts on all voices */
		/* read the irq status */
		(void)INB(loc->io_port + GRAVIS_IRQ_STATUS);
		(void)register_peek(loc->io_port, REGISTER_DMA_CONTROL);
		(void)register_peek(loc->io_port,
		    REGISTER_SAMPLE_CONTROL);
		(void)register_peek(loc->io_port, VOICE_IRQ_STATUS);
		
		/* reset all the voices */
		for (i = 0; i < NUM_VOICES; i++) {
			v = &iface->voices[i];
			stop_voice_now(iface, v);

			OUTB(loc->io_port + GRAVIS_PAGE, i);
			register_poke(loc->io_port, VOICE_CONTROL,
			    VOICEC_STOPPED | VOICEC_STOP);
			U_DELAY(8);
			register_poke(loc->io_port, VOICE_CONTROL,
			    VOICEC_STOPPED | VOICEC_STOP);

			register_poke(loc->io_port, VOICE_VOL_CONTROL,
			    VOLC_STOPPED | VOLC_RAMP_STOP);
			U_DELAY(8);
			register_poke(loc->io_port, VOICE_VOL_CONTROL,
			    VOLC_STOPPED | VOLC_RAMP_STOP);

			register_poke(loc->io_port, VOICE_FREQUENCY,
			    0x0400);

			register_poke(loc->io_port, VOICE_START_HI,
			    0x0000);
			register_poke(loc->io_port, VOICE_START_LOW,
			    0x0000);

			register_poke(loc->io_port, VOICE_END_HI,
			    0x0000);
			register_poke(loc->io_port, VOICE_END_LOW,
			    0x0000);

			register_poke(loc->io_port, VOICE_VOL_RAMP_RATE,
			    0x01);
			register_poke(loc->io_port, VOICE_VOL_RAMP_START,
			    0x10);
			register_poke(loc->io_port, VOICE_VOL_RAMP_END,
			    0xe0);

			register_poke(loc->io_port, VOICE_CURRENT_VOL,
			    0x0000);
			U_DELAY(8);
			register_poke(loc->io_port, VOICE_CURRENT_VOL,
			    0x0000);

			register_poke(loc->io_port, VOICE_CURRENT_HI,
			    0x0000);
			register_poke(loc->io_port, VOICE_CURRENT_LOW,
			    0x0000);

			register_poke(loc->io_port, VOICE_PAN_POS,
			    0x07);
		}
		(void)INB(loc->io_port + GRAVIS_IRQ_STATUS);
		(void)register_peek(loc->io_port, REGISTER_DMA_CONTROL);
		if (!iface->iwave)
			(void)register_peek(loc->io_port,
			    REGISTER_SAMPLE_CONTROL);
		(void)register_peek(loc->io_port, VOICE_IRQ_STATUS);

		/* enable GF1 interrupts */
		register_poke(loc->io_port, REGISTER_RESET, 0x07);

		/* enables line in, line out and latches */
		/* line in and line out are active low */
		OUTB(loc->io_port + GRAVIS_MIX, MIX_ENABLE_LATCHES);
	}
	iface->cur_out_time = 0;
	gravis_mem_init(iface);
	return (1);
}

static int
gravis_open(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;

	iface = (struct midi_iface_gravis *)mif;

	if (iface->iwave)
		enter_enhanced_mode(iface);
	return (0);
}

static void
gravis_close(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;

	iface = (struct midi_iface_gravis *)mif;
	if (iface->iwave)
		leave_enhanced_mode(iface);
}

static int
gravis_intr(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	int voice;
	u_char irq_status, irq_type;
	u_char code;

	iface = (struct midi_iface_gravis *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

PRINTF("gravis_intr\n");
	irq_type = INB(loc->io_port + GRAVIS_IRQ_STATUS);
	if (irq_type & IRQ_MIDI_TRANS) {
		LOGWARN("iface_gravis: spurious MIDI xmit irq\n");
		return (-1);
	} else if (irq_type & IRQ_MIDI_RECV) {
		if (iface->internal) {
			LOGWARN("iface_gravis: spurious MIDI recv irq\n");
			return (-1);
		} else {
			code = INB(loc->io_port + GRAVIS_MIDI_RECV);
			return (code);
		}
	} else if (irq_type & IRQ_TIMER1) {
		LOGWARN("iface_gravis: spurious timer1 irq\n");
		return (-1);
	} else if (irq_type & IRQ_TIMER2) {
		LOGWARN("iface_gravis: spurious timer2 irq\n");
		return (-1);
	} else if (irq_type & IRQ_WAVETABLE) {
PRINTF("IRQ_WAVETABLE\n");
		/* determine which voice generated the irq */
		irq_status = register_peek(loc->io_port, VOICE_IRQ_STATUS);
		while ((irq_status & (VIRQ_VOLUME | VIRQ_WAVETABLE)) !=
		    (VIRQ_VOLUME | VIRQ_WAVETABLE)) {
			voice = irq_status & VIRQ_VOICE_MASK;
			if (!(irq_status & VIRQ_VOLUME)) {
				LOGWARN("iface_gravis: spurious volume irq\n");
				continue;
			}
			if (!(iface->voices[voice].status & V_STOPPING)) {
				LOGWARN("iface_gravis: spurious wave irq\n");
				continue;
			}
			stop_voice_now(iface, &iface->voices[voice]);
		}
		return (-1);
	} else if (irq_type & IRQ_VOLUME) {
		LOGWARN("iface_gravis: spurious volume irq\n");
		return (-1);
	} else if (irq_type & IRQ_DMA) {
		if (!iface->internal) {
			LOGWARN("iface_gravis: spurious DMA irq\n");
			return (-1);
		} else {
			if (iface->status & GRAVIS_STATUS_SLEEP_DMA) {
				iface->status &= ~GRAVIS_STATUS_SLEEP_DMA;
				WAKEUP(&iface->sleep_dma);
			} else {
				LOGWARN("iface_gravis: spurious DMA irq\n");
			}
			return (-1);
		}
	}
	LOGWARN("iface_gravis: unknown irq type\n");
	return (-1);
}

static int
gravis_data_avail(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	int flag;

	iface = (struct midi_iface_gravis *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	/* internal gravis devices never have data available */
	if (iface->internal)
		return (0);
	else {
		flag = INB(loc->io_port + GRAVIS_MIDI_STATUS);
		return (flag & GRAVIS_MIDI_DATA_AVL);
	}
}

static void
gravis_write(struct midi_iface *mif, struct event *event)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	int i;
	u_char byte;

	iface = (struct midi_iface_gravis *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	if (!iface->internal) {
		for (i = 0; i < stynamic_len(&event->data); i++) {
			byte = stynamic_get_byte(&event->data, i);
			wait_rdy_rcv(iface);
			OUTB(loc->io_port + GRAVIS_MIDI_TRANS, byte);
		}
	} else {
		gravis_internal_event(iface, event);
	}
}

static u_char
gravis_read(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	u_char byte;

	iface = (struct midi_iface_gravis *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	if (iface->internal)
		return (0);
	else {
		byte = INB(loc->io_port + GRAVIS_MIDI_RECV);
		return (byte);
	}
}

static int
gravis_feature(struct midi_iface *mif, struct midi_feature *feature)
{
	struct midi_iface_gravis *iface;
	struct midi_isa_iface *loc;
	struct gravis_patch patch;
	int ret;

	iface = (struct midi_iface_gravis *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	/* external gravis devices don't have any features */
	if (!iface->internal)
		return (ENOTTY);

	/* XXX must add support for loading voices and flushing voices */
	switch (feature->type) {
	case MFEAT_FLUSH_PATCHES:
		flush_patches(iface);
		return (0);
	case MFEAT_LOAD_PATCH:
		/*
		 * XXX should probably check to verify the data is a
		 * gravis voice.
		 */
		FEATURE_U_TO_K(&patch, feature->data,
		    sizeof(struct gravis_patch));
		ret = load_patch(iface, &patch);
		return (ret);
	case MFEAT_DELETE_PATCH:
		FEATURE_U_TO_K(&patch, feature->data,
		    sizeof(struct gravis_patch));
		ret = delete_patch(iface, &patch);
		return (ret);
	default:
		return (ENOTTY);
	}
}

static void
gravis_free(struct midi_iface *mif)
{
	struct midi_iface_gravis *iface;
	struct gravis_loc *b, *prev_b;

	iface = (struct midi_iface_gravis *)mif;
	prev_b = NULL;
	for (b = gravis_boards; b != NULL; b = b->next) {
		if (b->loc.io_port ==
		    ((struct midi_isa_iface *)iface->iface.loc)->io_port &&
		    b->internal == iface->internal)
			break;
		prev_b = b;
	}
	if (b == NULL)
		LOGWARN("iface_gravis: Couldn't find board in loc list\n");
	else {
		if (prev_b == NULL)
			gravis_boards = b->next;
		else
			prev_b->next = b->next;
	}

	flush_patches(iface);

	FREE(mif->loc, sizeof(struct midi_isa_iface));

	iface = (struct midi_iface_gravis *)mif;
	FREE(iface, sizeof(struct midi_iface_gravis));
}

static void
enter_enhanced_mode(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	short mode;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	mode = register_peek(loc->io_port, REGISTER_ENHANCED_MODE);
	register_poke(loc->io_port, REGISTER_ENHANCED_MODE, mode | ENH_MODE);
}

static void
leave_enhanced_mode(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	short mode;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	mode = register_peek(loc->io_port, REGISTER_ENHANCED_MODE);
	mode &= ~ENH_MODE;
	register_poke(loc->io_port, REGISTER_ENHANCED_MODE, mode);
}


static int
wait_rdy_rcv(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	int flag, i;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	for (i = 0; i < GRAVIS_RECV_TRIES; i++) {
		flag = INB(loc->io_port + GRAVIS_MIDI_STATUS) &
		    GRAVIS_MIDI_RDY_RCV;
		if (flag)
			break;
		U_DELAY(10);
	}
	if (i == GRAVIS_RECV_TRIES)
		return (0);
	/*
	 * One more small delay to insure the GUS can accept data.  Sometimes
	 * it's a bit premature.
	 */
	U_DELAY(250);
	return (1);
}

static void
midi_reset(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	if (!iface->internal) {
		/* bit 0 & 1 high ... */
		OUTB(loc->io_port + GRAVIS_MIDI_CONTROL, 0x03);
		/* then low */
		OUTB(loc->io_port + GRAVIS_MIDI_CONTROL, 0x00);
	}
}

static int
find_irq(u_short port, int internal)
{
	struct gravis_loc *l;

	for (l = gravis_boards; l != NULL; l = l->next)
		if (l->loc.io_port == port && l->internal == internal)
			break;
	if (l == NULL)
		return (0);
	return (l->loc.irq);
}

static int
irq_to_bitmap(int irq)
{
	int b;

	switch (irq) {
	case 0:
		b = 0;
		break;
	case 2:
		b = 0x01;
		break;
	case 3:
		b = 0x03;
		break;
	case 5:
		b = 0x02;
		break;
	case 7:
		b = 0x04;
		break;
	case 11:
		b = 0x05;
		break;
	case 12:
		b = 0x06;
		break;
	case 15:
		b = 0x07;
		break;
	default:
		b = 0xff;
		break;
	}
	return (b);
}

/*
 * converts an irq number to a byte suitable for the irq control register.
 * 0xff means invalid irq.
 */
static u_char
irq_bitmap(struct midi_iface_gravis *iface, int irq)
{
	u_char b;
	struct midi_isa_iface *loc;
	int irq_bm, other_irq, other_irq_bm;

	irq_bm = irq_to_bitmap(irq);

	/*
	 * try to determine the irq of an existing device sharing
	 * our same hardware.
	 */
	loc = (struct midi_isa_iface *)iface->iface.loc;
	other_irq = find_irq(loc->io_port, !iface->internal);
	other_irq_bm = irq_to_bitmap(other_irq);

	if (irq == other_irq) {
		b = 0x40;
		b |= irq_bm;
	} else {
		if (iface->internal) {
			b = other_irq_bm;
			b = (b << 3) & 0x38;
			b |= irq_bm;
		} else {
			/*
			 * there must always be a GF1 irq, so if we
			 * don't have one already, duplicate the
			 * MIDI irq.
			 */
			if (other_irq == 0) {
				b = 0x40;
				b |= irq_bm;
			} else {
				b = irq_bm;
				b = (b << 3) & 0x38;
				b |= other_irq_bm;
			}
		}
	}
	return (b);
}

static int
gf1_ping(u_short port)
{
	u_char save_val0, save_val1, val0, val1;

	/* save current values  ... */
	save_val0 = peek_data(port, 0L);
	save_val1 = peek_data(port, 1L);

	poke_data(port, 0L, 0xaa);
	poke_data(port, 1L, 0x55);
	val0 = peek_data(port, 0L);
	val1 = peek_data(port, 1L);

	/* restore data to old values ... */
	poke_data(port, 0L, save_val0);
	poke_data(port, 1L, save_val1);

	if ((val0 == 0xaa) && (val1 == 0x55))
		return (1);
	else
		return (0);
}

static void
register_poke(u_short port, u_char reg, u_short val)
{


	OUTB(port + GRAVIS_REGISTER_SELECT, reg);
	switch (reg) {
	case REGISTER_DMA_CONTROL:
	case REGISTER_ADDR_HI:
	case REGISTER_TIMER_CONTROL:
	case REGISTER_TIMER1:
	case REGISTER_TIMER2:
	case REGISTER_SAMPLE_FREQ:
	case REGISTER_SAMPLE_CONTROL:
	case REGISTER_JOYSTICK_TRIM:
	case REGISTER_RESET:
	case REGISTER_ENHANCED_MODE:
	case REGISTER_SMSI:
	case REGISTER_LMCI:
/*
	case REGISTER_PCCCI:
*/
	case VOICE_CONTROL:
	case VOICE_VOL_RAMP_RATE:
	case VOICE_VOL_RAMP_START:
	case VOICE_VOL_RAMP_END:
	case VOICE_PAN_POS:
	case VOICE_VOL_CONTROL:
	case VOICE_ACTIVE:
	case VOICE_SUAI:
		/* 8 bits */
		OUTB(port + GRAVIS_DATA_HI, val);
		break;
	case REGISTER_DMA_START_ADDR:
	case REGISTER_ADDR_LOW:
	case REGISTER_LMCFI:
	case VOICE_FREQUENCY:
	case VOICE_START_HI:
	case VOICE_START_LOW:
	case VOICE_END_HI:
	case VOICE_END_LOW:
	case VOICE_CURRENT_VOL:
	case VOICE_CURRENT_HI:
	case VOICE_CURRENT_LOW:
		/* 16 bits */
		OUTW(port + GRAVIS_DATA_LOW, val);
		break;
	case VOICE_IRQ_STATUS:
		/* can't write here */
		break;
	default:
		PRINTF("Warning!  register_poke called with unknown reg "
		    "(0x%02x)\n", reg);
	}
}

static u_short
register_peek(u_short port, u_char reg)
{
	u_short val;
	u_char real_reg;

	/*
	 * You might get a warning here because VOICE_CONTROL is 0
	 * and reg is unsigned (of course it will be greater or
	 * equal) but let's just let the compiler optimize it out.
	 * I want to leave the check explicit so we know we are
	 * comparing a range of register, one of which just happens
	 * to be zero.
	 */
	if (reg >= VOICE_CONTROL && reg <= VOICE_SUAI)
		real_reg = reg | 0x80;
	else
		real_reg = reg;

	OUTB(port + GRAVIS_REGISTER_SELECT, real_reg);
	switch (reg) {
	case REGISTER_DMA_CONTROL:
	case REGISTER_ADDR_HI:
	case REGISTER_TIMER_CONTROL:
	case REGISTER_TIMER1:
	case REGISTER_TIMER2:
	case REGISTER_SAMPLE_FREQ:
	case REGISTER_SAMPLE_CONTROL:
	case REGISTER_JOYSTICK_TRIM:
	case REGISTER_RESET:
	case REGISTER_ENHANCED_MODE:
	case REGISTER_SMSI:
	case REGISTER_LMCI:
/*
	case REGISTER_PCCCI:
*/
	case VOICE_CONTROL:
	case VOICE_VOL_RAMP_RATE:
	case VOICE_VOL_RAMP_START:
	case VOICE_VOL_RAMP_END:
	case VOICE_PAN_POS:
	case VOICE_VOL_CONTROL:
	case VOICE_ACTIVE:
	case VOICE_IRQ_STATUS:
	case VOICE_SUAI:
		/* 8 bits */
		val = INB(port + GRAVIS_DATA_HI);
		break;
	case REGISTER_DMA_START_ADDR:
	case REGISTER_ADDR_LOW:
	case REGISTER_LMCFI:
	case VOICE_FREQUENCY:
	case VOICE_START_HI:
	case VOICE_START_LOW:
	case VOICE_END_HI:
	case VOICE_END_LOW:
	case VOICE_CURRENT_VOL:
	case VOICE_CURRENT_HI:
	case VOICE_CURRENT_LOW:
		/* 16 bits */
		val = INW(port + GRAVIS_DATA_LOW);
		break;
	default:
		PRINTF("Warning!  register_peek called with unknown reg "
		    "(0x%02x)\n", reg);
		val = 0;
	}
	return (val);
}

static void
poke_data(u_short port, u_long address, u_char data)
{
	int s;

	s = BLOCK_INTR();

	OUTB(port + GRAVIS_REGISTER_SELECT, SET_DRAM_LOW);
	OUTW(port + GRAVIS_DATA_LOW, (u_short)(address & 0xffff));
	OUTB(port + GRAVIS_REGISTER_SELECT, SET_DRAM_HIGH);
	OUTB(port + GRAVIS_DATA_HI, (u_short)((address >> 16) & 0xff));
	OUTB(port + GRAVIS_DRAM, data);

	UNBLOCK_INTR(s);
}

static u_char
peek_data(u_short port, u_long address)
{
	int s;
	u_char ret;

	s = BLOCK_INTR();

	OUTB(port + GRAVIS_REGISTER_SELECT, SET_DRAM_LOW);
	OUTW(port + GRAVIS_DATA_LOW, (u_short)(address & 0xffff));
	OUTB(port + GRAVIS_REGISTER_SELECT, SET_DRAM_HIGH);
	OUTB(port + GRAVIS_DATA_HI, (u_char)((address >> 16) & 0xff));
	ret = INB(port + GRAVIS_DRAM);

	UNBLOCK_INTR(s);
	return (ret);
}

static void
poke_data_l(u_short port, u_long address, u_long data)
{
	unsigned char *ptr;
	int i;

	ptr = (unsigned char *)&data;
	for (i = 0; i < 4; i++)
		poke_data(port, address++, *ptr++);
}


static u_long
peek_data_l(u_short port, u_long address)
{
	u_long ret;
	int i;
	unsigned char *ptr;

	ptr = (unsigned char *)&ret;
	for (i = 0; i < 4; i++)
		*ptr++ = peek_data(port, address++);
	return (ret);
}

/*
 * Writes a block of user space data to gravis LM one byte at a time.
 */
static void
poke_block(struct midi_iface_gravis *iface, u_char *block, u_long len,
    u_long addr)
{
	struct midi_isa_iface *loc;
	int i, s;
	u_char *b, lmci;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	b = MALLOC_WAIT(len);
	FEATURE_U_TO_K(b, block, len);

	s = BLOCK_INTR();

	if (iface->iwave) {
		register_poke(loc->io_port, REGISTER_ADDR_LOW, addr & 0xffff);
		register_poke(loc->io_port, REGISTER_ADDR_HI,
		    0xff & (addr >> 16));
		lmci = register_peek(loc->io_port, REGISTER_LMCI);
		register_poke(loc->io_port, REGISTER_LMCI,
		    (lmci | LMCI_AUTO_INC) & LMCI_DRAM_MASK);

		for (i = 0; i < len; i++)
			OUTB(loc->io_port + GRAVIS_DRAM, b[i]);
		register_poke(loc->io_port, REGISTER_LMCI, lmci);
	} else {
		/* I don't think the GF1 has an auto increment mode */
		for (i = 0; i < len; i++) {
			poke_data(loc->io_port, addr++, b[i]);
		}
	}
	UNBLOCK_INTR(s);
	FREE(b, len);
}

static void
iwave_gus_reset(unsigned short port)
{

	/* reset the GF1 */
	register_poke(port, REGISTER_RESET, 0x00);

	/* wait a little while ... */
	U_DELAY(1000);

	/* Release Reset */
	register_poke(port, REGISTER_RESET, 0x07);
	U_DELAY(1000);
}

static u_long
addr_trans(struct midi_iface_gravis *iface, u_long addr)
{
	u_long lm;

	if (!iface->iwave)
		lm = (addr & 0x000c0000) | ((addr & 0x0003ffff) >> 1);
	else
		lm = addr >> 1;
	return (lm);
}


static u_long
gravis_mem_size(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	u_long i, l;
	u_char save0, save1, val;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	/* save first location ... */
	save0 = peek_data(loc->io_port, 0);

	/* See if there is first block there.... */
	poke_data(loc->io_port, 0, 0xaa);
	val = peek_data(loc->io_port, 0);
	if (val != 0xaa)
		return (0);

	/* Now zero it out so that I can check for mirroring .. */
	poke_data(loc->io_port, 0, 0x00);

	for (i = 1; i < 1024; i++) { 
		/* check for mirroring ... */
		val = peek_data(loc->io_port, 0);
		if (val != 0)
			break;
		l = i << 10;

		/* save location so its a non-destructive sizing */
		save1 = peek_data(loc->io_port, l);

		poke_data(loc->io_port, l, 0xaa);
		val = peek_data(loc->io_port, l);
		if (val != 0xaa)
			break;
		poke_data(loc->io_port, l, save1);
	}

	/* Now restore location zero ... */
	poke_data(loc->io_port, 0, save0);

	return((int)i);
}


static void
gravis_mem_init(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	u_long app_mem, dram_avail, dram_size;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	dram_size = iface->mem_size * 1024;

	if (iface->iwave) {
		app_mem = iface->reserved_mem + 1;
		app_mem &= -2;
		dram_avail = dram_size - app_mem;
		iface->free_mem = app_mem;
		poke_data_l(loc->io_port, iface->free_mem + MEM_NEXT,
		    MEM_NO_NEXT);
		poke_data_l(loc->io_port, iface->free_mem + MEM_PREV,
		    MEM_NO_PREV);
		poke_data_l(loc->io_port, iface->free_mem + MEM_SIZE,
		    dram_avail);
	} else {
		if (iface->reserved_mem > 256 * 1024)
			LOGWARN("iface_gravis: Too much reserved mem\n");
		app_mem = iface->reserved_mem + 31;
		app_mem &= -32;
		dram_avail = dram_size - app_mem;
		iface->free_mem = app_mem;
		poke_data_l(loc->io_port, iface->free_mem + MEM_NEXT,
		    MEM_NO_NEXT);
		poke_data_l(loc->io_port, iface->free_mem + MEM_PREV,
		    MEM_NO_PREV);
		poke_data_l(loc->io_port, iface->free_mem + MEM_SIZE,
		    dram_avail);

		/* break LM blocks into no greater tha 256k byte chunks */
		if (dram_avail > 256 * 1024) {
			gravis_mem_alloc(iface, dram_avail);

			if (dram_avail > 3 * 256 * 1024)
				gravis_mem_free(iface, 3 * 256 * 1024,
				    MEM_GF1_POOL);
			if (dram_avail > 2 * 256 * 1024)
				gravis_mem_free(iface, 2 * 256 * 1024,
				    MEM_GF1_POOL);
			gravis_mem_free(iface, 256 * 1024, MEM_GF1_POOL);
			gravis_mem_free(iface, app_mem, MEM_GF1_POOL - app_mem);
		}
	}
}

static void
gravis_mem_free(struct midi_iface_gravis *iface, u_long addr, u_long size)
{
	struct midi_isa_iface *loc;
	u_long addr_ptr, prev_free, next_free;
	int mem_ok;

	loc = (struct midi_isa_iface *)iface->iface.loc;
	mem_ok = 0;

	/*
	 * Round up specified size to either a 32-byte boundary
	 * or an even byte boundary depending on the mode of
	 * operation.
	 */
	if (iface->iwave) {
		size += 1;
		size &= -2;
	} else {
		size += 31;
		size &= -32;
	}

	addr_ptr = iface->free_mem;
	if (addr_ptr == MEM_EXHAUSTED) {
		iface->free_mem = addr;
		poke_data_l(loc->io_port, addr + MEM_NEXT, MEM_NO_NEXT);
		poke_data_l(loc->io_port, addr + MEM_PREV, MEM_NO_PREV);
		poke_data_l(loc->io_port, addr + MEM_SIZE, size);
	} else {
		while (addr_ptr != MEM_NO_NEXT && !mem_ok) {
			next_free = peek_data_l(loc->io_port,
			    addr_ptr + MEM_NEXT);
			prev_free = peek_data_l(loc->io_port,
			    addr_ptr + MEM_PREV);
			if (addr < addr_ptr) {
				if (prev_free == MEM_NO_PREV)
					iface->free_mem = addr;
				else
					poke_data_l(loc->io_port,
					    prev_free + MEM_NEXT, addr);

				poke_data_l(loc->io_port, addr + MEM_NEXT,
				    addr);
				poke_data_l(loc->io_port, addr + MEM_PREV,
				    prev_free);
				poke_data_l(loc->io_port, addr + MEM_SIZE,
				    size);
				poke_data_l(loc->io_port, addr_ptr + MEM_PREV,
				    addr_ptr);
				mem_ok = 1;
			} else {
				if (next_free == MEM_NO_NEXT) {
					poke_data_l(loc->io_port,
					    addr_ptr + MEM_NEXT, addr);
					poke_data_l(loc->io_port,
					    addr + MEM_NEXT, MEM_NO_NEXT);
					poke_data_l(loc->io_port,
					    addr + MEM_PREV, addr_ptr);
					poke_data_l(loc->io_port,
					    addr + MEM_SIZE, size);
					mem_ok = 1;
				}
			}
			addr_ptr = peek_data_l(loc->io_port,
			    addr_ptr + MEM_NEXT);
		}
	}

	if (mem_ok)
		gravis_mem_merge(iface);
}

static void
gravis_mem_merge(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	u_long addr_ptr, blk_size, next_size, next_next, next_free;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	addr_ptr = iface->free_mem;

	while (addr_ptr != MEM_NO_NEXT) {
		next_free = peek_data_l(loc->io_port, addr_ptr + MEM_NEXT);
		blk_size = peek_data_l(loc->io_port, addr_ptr + MEM_SIZE);
		if ((addr_ptr + blk_size) != next_free)
			addr_ptr = peek_data_l(loc->io_port, addr_ptr +
			    MEM_NEXT);
		else {
			/* adjacent free blocks */
			next_next = peek_data_l(loc->io_port, next_free +
			    MEM_NEXT);
			next_size = peek_data_l(loc->io_port, next_free +
			    MEM_SIZE);
			blk_size += next_size;
			poke_data_l(loc->io_port, addr_ptr + MEM_SIZE,
			    blk_size);
			poke_data_l(loc->io_port, addr_ptr + MEM_NEXT,
			    next_next);

			if (next_next != MEM_NO_NEXT)
				poke_data_l(loc->io_port, next_next + MEM_PREV,
				    addr_ptr);
			else
				break;
		}
	}
}

static u_long
gravis_mem_alloc(struct midi_iface_gravis *iface, u_long size)
{
	struct midi_isa_iface *loc;
	u_long block_size, curr_addr, next, prev, size_left;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	if (iface->iwave) {
		size += 1;
		size &= -2;
	} else {
		size += 31;
		size &= -32;
	}

	curr_addr = iface->free_mem;

	while (curr_addr != MEM_NO_NEXT) {
		block_size = peek_data_l(loc->io_port, curr_addr + MEM_SIZE);

		if (block_size >= size) {
			size_left = block_size - size;
			if (size_left < MEM_HEADER_SIZE) {
				/*
				 * left over space is less than size of
				 * header - throw it away.
				 */
				next = peek_data_l(loc->io_port, curr_addr +
				    MEM_NEXT);
				prev = peek_data_l(loc->io_port, curr_addr +
				    MEM_PREV);

				if (next != MEM_NO_NEXT)
					poke_data_l(loc->io_port, next +
					    MEM_PREV, prev);
				if (prev != MEM_NO_PREV)
					poke_data_l(loc->io_port, prev +
					    MEM_NEXT, next);
				else
					iface->free_mem = next;
				return (curr_addr);
			} else {
				poke_data_l(loc->io_port, curr_addr + MEM_SIZE,
				    size_left);
				return (curr_addr + size_left);
			}
		}

		curr_addr = peek_data_l(loc->io_port, curr_addr + MEM_NEXT);
	}
	return (MEM_NO_MEM);
}

/*
 * This function returns the size of greatest block of memory that
 * can still be allocated.  When in GR1 (non iwave) mode, the largest
 * block will be < 256k.
 */
static u_long
gravis_mem_max_alloc(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	u_long block_size, curr_addr, size;

	loc = (struct midi_isa_iface *)iface->iface.loc;
	size = 0;

	curr_addr = iface->free_mem;

	while (curr_addr != MEM_NO_NEXT) {
		block_size = peek_data_l(loc->io_port, curr_addr + MEM_SIZE);
		if (block_size > size)
			size = block_size;
		curr_addr = peek_data_l(loc->io_port, curr_addr + MEM_NEXT);
	}
	return (size);
}

static u_long
gravis_mem_avail(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	u_long block_size, curr_addr, size;

	loc = (struct midi_isa_iface *)iface->iface.loc;
	size = 0;

	curr_addr = iface->free_mem;
	while (curr_addr != MEM_NO_NEXT) {
		block_size = peek_data_l(loc->io_port, curr_addr + MEM_SIZE);
		size += block_size;
		curr_addr = peek_data_l(loc->io_port, curr_addr + MEM_NEXT);
	}
	return (size);
}

static int
load_patch(struct midi_iface_gravis *iface, struct gravis_patch *patch)
{
	struct gravis_wave *waves;
	struct gravis_lm_patch *p;
	u_long lm_addr, lm_avail, mem_needed;
	int i;

	/* make sure it is a valid program number */
	if (patch->program < 0 || patch->program > 127)
		return (EINVAL);

	waves = MALLOC_WAIT(sizeof(struct gravis_wave) * patch->num_waves);

	/* copy in the voices, and find out how much memory we'll need */
	FEATURE_U_TO_K(waves, patch->waves, sizeof(struct gravis_wave) *
	    patch->num_waves);
	mem_needed = 0;
	for (i = 0; i < patch->num_waves; i++)
		mem_needed += waves[i].len;

	/* see if we have enough local memory */
	lm_avail = gravis_mem_avail(iface);
	if (mem_needed > lm_avail) {
		FREE(waves, sizeof(struct gravis_wave) * patch->num_waves);
		return (ENOMEM);
	}

	/* if we already have a patch loaded here, free it */
	delete_patch(iface, patch);

	/* allocate a lm_patch */
	p = MALLOC_WAIT(sizeof(struct gravis_lm_patch));
	p->waves = MALLOC_WAIT(sizeof(struct gravis_lm_wave) *
	    patch->num_waves);
	p->num_waves = patch->num_waves;
	p->program = patch->program;
	p->drum = patch->drum;

	for (i = 0; i < patch->num_waves; i++) {
		
		lm_addr = gravis_mem_alloc(iface, waves[i].len);
		if (lm_addr ==  MEM_NO_MEM) {
			/*
			 * we still might fail if memory isn't alvailable
			 * in the right size pieces.
			 */
			for (i--; i >= 0; i--) {
PRINTF("gravis freeing wave %d\n", i);
				gravis_mem_free(iface, p->waves[i].begin,
				    p->waves[i].len);
			}
PRINTF("freeing p->waves\n");
			FREE(p->waves, sizeof(struct gravis_lm_wave) *
			    patch->num_waves);
PRINTF("freeing p\n");
			FREE(p, sizeof(struct gravis_lm_patch));
PRINTF("freeing waves\n");
			FREE(waves, sizeof(struct gravis_wave) *
			    patch->num_waves);
PRINTF("returning error\n");
			return (ENOMEM);
		}
		p->waves[i].patch = p;
		p->waves[i].len = waves[i].len;
		p->waves[i].begin = lm_addr;
		p->waves[i].loop_start = lm_addr + waves[i].loop_start;
		p->waves[i].loop_end = lm_addr + waves[i].loop_end;
		p->waves[i].end = lm_addr + waves[i].len;
		p->waves[i].low_freq = waves[i].low_freq;
		p->waves[i].high_freq = waves[i].high_freq;
		p->waves[i].root_freq = waves[i].root_freq;
		p->waves[i].sr = waves[i].sr;
		p->waves[i].volume = waves[i].volume;
		p->waves[i].data_8_bit = waves[i].data_8_bit;
		p->waves[i].data_unsigned = waves[i].data_unsigned;
		p->waves[i].looping = waves[i].looping;

		/* load data into lm */
		poke_block(iface, waves[i].data, waves[i].len, lm_addr);
	}

	if (patch->drum)
		iface->drum_programs[patch->program] = p;
	else
		iface->normal_programs[patch->program] = p;
	FREE(waves, sizeof(struct gravis_wave) * patch->num_waves);
	return (0);
}

static int
delete_patch(struct midi_iface_gravis *iface, struct gravis_patch *patch)
{
	struct gravis_lm_patch *p;
	struct gravis_lm_wave *w;
	struct gravis_voice *v;
	int drum, i, j, program;

	program = patch->program;
	drum = patch->drum;

	if (program < 0 || program > 127)
		return (EINVAL);

	if (drum)
		p = iface->drum_programs[program];
	else
		p = iface->normal_programs[program];

	if (p != NULL) {
		for (i = 0; i < p->num_waves; i++) {
			w = &p->waves[i];
			/* see if this voice is on active list */
			for (j = 0; j < NUM_VOICES; j++) {
				v = &iface->voices[j];
				if ((v->status & V_IN_USE) && v->w == w)
					stop_voice_now(iface, v);
			}
			/* free voice from LM */
			gravis_mem_free(iface, w->begin, w->len);
		}
		/* free voices */
		FREE(p->waves, sizeof(struct gravis_lm_wave) *
		    p->num_waves);
		/* free voice data */
		if (drum) {
			FREE(iface->drum_programs[program],
			    sizeof(struct gravis_lm_patch));
			iface->drum_programs[program] = NULL;
		} else {
			FREE(iface->normal_programs[program],
			    sizeof(struct gravis_lm_patch));
			iface->normal_programs[program] = NULL;
		}
	}
	return (0);
}

static void
flush_patches(struct midi_iface_gravis *iface)
{
	int i;
	struct gravis_patch p;

	for (i = 0; i < 128; i++) {
		p.program = i;
		p.drum = 0;
		delete_patch(iface, &p);
		p.drum = 1;
		delete_patch(iface, &p);
	}
}

static void
gravis_internal_event(struct midi_iface_gravis *iface, struct event *event)
{
	u_char channel, mode, param, pitch, pressure, val, vel;

	iface->cur_out_time = event->smf_time;

	/* we only handle the basic events */
	if (event->type != NORMAL)
		return;

	/*
	 * all events are fully formed, so we can determine the type
	 * from the first byte.
	 */
	mode = stynamic_get_byte(&event->data, 0);
	switch (mode & 0xf0) {
	case 0x80:
		channel = mode & 0x0f;
		pitch = stynamic_get_byte(&event->data, 1);
		note_off_event(iface, channel, pitch);
		break;
	case 0x90:
		channel = mode & 0x0f;
		pitch = stynamic_get_byte(&event->data, 1);
		vel = stynamic_get_byte(&event->data, 2);
		note_on_event(iface, channel, pitch, vel);
		break;
	case 0xa0:
		channel = mode & 0x0f;
		pitch = stynamic_get_byte(&event->data, 1);
		pressure = stynamic_get_byte(&event->data, 2);
		key_pressure_event(iface, channel, pitch, pressure);
		break;
	case 0xb0:
		channel = mode & 0x0f;
		param = stynamic_get_byte(&event->data, 1);
		val = stynamic_get_byte(&event->data, 2);
		parameter_event(iface, channel, param, val);
		break;
	case 0xc0:
		channel = mode & 0x0f;
		val = stynamic_get_byte(&event->data, 1);
		program_event(iface, channel, val);
		break;
	case 0xd0:
		channel = mode & 0x0f;
		pressure = stynamic_get_byte(&event->data, 1);
		chan_pressure_event(iface, channel, pressure);
		break;
	case 0xe0:
		channel = mode & 0x0f;
		val = stynamic_get_byte(&event->data, 1);
		pitch_wheel_event(iface, channel, val);
		break;
	case 0xf0:
		real_time_event(iface, mode);
		break;
	}
}

static void
stop_voice_now(struct midi_iface_gravis *iface,
    struct gravis_voice *v)
{
	struct midi_isa_iface *loc;
	int s;
	u_char sxci, voice_control;

	s = BLOCK_INTR();
	loc = (struct midi_isa_iface *)iface->iface.loc;
	OUTB(loc->io_port + GRAVIS_PAGE, v->voice_num);

	voice_control = register_peek(loc->io_port, VOICE_CONTROL);
	voice_control |= VOICEC_STOPPED | VOICEC_STOP;
	voice_control &= ~VOICEC_WAVE_IRQ;
	register_poke(loc->io_port, VOICE_CONTROL, voice_control);
	U_DELAY(8);
	register_poke(loc->io_port, VOICE_CONTROL, voice_control);
	v->status &= ~(V_STOPPING | V_IN_USE);

	if (iface->iwave) {
		sxci = register_peek(loc->io_port, REGISTER_SMSI);
		sxci |= 0x02;
		register_poke(loc->io_port, REGISTER_SMSI, sxci);
	}
	UNBLOCK_INTR(s);
}

static void
note_off_event(struct midi_iface_gravis *iface, int channel, int pitch)
{
	struct midi_isa_iface *loc;
	struct gravis_voice *v;
	struct gravis_lm_patch *p;
	u_long end;
	int i, s;
	u_char voice_control;
	unsigned short dummy;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	p = iface->channel[channel];
	if (p == NULL) {
		PRINTF("note_off: no voice set for channel %d\n", channel);
		return;
	}

	for (i = 0; i < NUM_VOICES; i++) {
		v = &iface->voices[i];
		if (v->w->patch == p && v->pitch == pitch  &&
		    !(v->status & V_STOPPING))
			break;
	}
	if (i == NUM_VOICES) {
		PRINTF("Couldn't find active voice for this note off\n");
		return;
	}

	end = addr_trans(iface, v->w->end);

	s = BLOCK_INTR();
	/* select page */
	OUTB(loc->io_port + GRAVIS_PAGE, i);
dummy = INB(loc->io_port + GRAVIS_PAGE);
PRINTF("selected voice %d, read %d\n", i, dummy);

dummy = register_peek(loc->io_port, VOICE_CURRENT_LOW);
PRINTF("VOICE_CURRENT_LOW, read %d\n", dummy);
dummy = register_peek(loc->io_port, VOICE_CURRENT_HI);
PRINTF("VOICE_CURRENT_HI, read %d\n", dummy);

	/* move end */
	register_poke(loc->io_port, VOICE_END_LOW, GRAVIS_ADDR_LOW(end));
	register_poke(loc->io_port, VOICE_END_HI, GRAVIS_ADDR_HI(end));

	/* turn looping off but enable IRQ */
	voice_control = register_peek(loc->io_port, VOICE_CONTROL);
	voice_control &= ~VOICEC_LOOP;
	voice_control |= VOICEC_WAVE_IRQ;
	register_poke(loc->io_port, VOICE_CONTROL, voice_control);
	U_DELAY(8);
	register_poke(loc->io_port, VOICE_CONTROL, voice_control);

	/* set flag so irq handler knows why the irq arrived */
	v->status |= V_STOPPING;

	UNBLOCK_INTR(s);
}

static void
note_on_event(struct midi_iface_gravis *iface, int channel, int pitch, int vel)
{
	struct midi_isa_iface *loc;
	struct gravis_voice *v, *oldest_v, *oldest_same_v;
	struct gravis_lm_patch *p;
	struct gravis_lm_wave *w;
	u_long oldest_v_time, oldest_same_time;
	u_long begin, loop_start, loop_end;
	int adj_freq, freq, i, oldest_index, oldest_same_index, s;
	u_short gravis_freq;
	short vol;
	u_char voice_control;
	u_short dummy;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	freq = pitch_to_freq(pitch);

	p = iface->channel[channel];
	if (p == NULL) {
		PRINTF("note_on: no voice set for channel %d\n", channel);
		return;
	}

	/* find correct voice in block for this frequency */
	w = &p->waves[0];
	for (i = 0; i < p->num_waves; i++) {
		w = &p->waves[i];
		if (w->low_freq <= freq && freq <= w->high_freq)
			break;
	}
	if (i == p->num_waves) {
		PRINTF("note_on: no voice for freq %d\n", freq);
		return;
	}
	adj_freq = (long long)w->sr * freq / w->root_freq;

	/* find the first unused voice */
	oldest_v_time = -1;
	oldest_same_time = -1;
	oldest_index = 0;
	oldest_same_index = 0;
	oldest_v = &iface->voices[0];
	oldest_same_v = &iface->voices[0];
	for (i = 0; i < NUM_VOICES; i++) {
		v = &iface->voices[i];
		if (!(v->status & V_IN_USE)) {
PRINTF("voice %d not in use\n", i);
			break;
		}
		if (v->start_time >= oldest_v_time) {
			oldest_v_time = v->start_time;
			oldest_v = v;
			oldest_index = i;
		}
		if (v->w->patch == p && v->start_time >= oldest_same_time) {
			oldest_same_time = v->start_time;
			oldest_same_v = v;
			oldest_same_index = i;
		}
	}
PRINTF("i = %d\n", i);
	/*
	 * There are no free voices, so we'll have to toss the oldest
	 * (preferably of the same patch type)
	 */
	if (i == NUM_VOICES) {
PRINTF("i == NUM_VOICES\n");
		if (oldest_same_time != -1) {
			stop_voice_now(iface, oldest_same_v);
			v = oldest_same_v;
			i = oldest_same_index;
		} else {
			stop_voice_now(iface, oldest_v);
			v = oldest_v;
			i = oldest_index;
		}
	}

	v->w = w;
	v->start_time = iface->cur_out_time;
	v->pitch = pitch;
	v->status |= V_IN_USE;


	/* 16 bit data requires an address translation in GUS mode */
	begin = addr_trans(iface, w->begin);
	loop_start = addr_trans(iface, w->loop_start);
	loop_end = addr_trans(iface, w->loop_end);

	s = BLOCK_INTR();
	/* select the voice */
	OUTB(loc->io_port + GRAVIS_PAGE, i);
dummy = INB(loc->io_port + GRAVIS_PAGE);
PRINTF("selected voice %d, read %d\n", i, dummy);

	/* select local mem bank */
	if (iface->iwave)
		register_poke(loc->io_port, VOICE_SUAI,
		    (loop_start >> 22) & 0x03);

	/* current voice position */
	register_poke(loc->io_port, VOICE_CURRENT_LOW, GRAVIS_ADDR_LOW(begin));
	register_poke(loc->io_port, VOICE_CURRENT_HI, GRAVIS_ADDR_HI(begin));

dummy = register_peek(loc->io_port, VOICE_CURRENT_LOW);
PRINTF("wrote %d to VOICE_CURRENT_LOW, read %d\n", GRAVIS_ADDR_LOW(begin),
dummy);
dummy = register_peek(loc->io_port, VOICE_CURRENT_HI);
PRINTF("wrote %d to VOICE_CURRENT_HI, read %d\n", GRAVIS_ADDR_HI(begin),
dummy);

	/* loop start */
	register_poke(loc->io_port, VOICE_START_LOW,
	    GRAVIS_ADDR_LOW(loop_start));
	register_poke(loc->io_port, VOICE_START_HI,
	    GRAVIS_ADDR_HI(loop_start));
dummy = register_peek(loc->io_port, VOICE_START_LOW);
PRINTF("wrote %d to VOICE_START_LOW, read %d\n", GRAVIS_ADDR_LOW(loop_start),
dummy);
dummy = register_peek(loc->io_port, VOICE_START_HI);
PRINTF("wrote %d to VOICE_START_HI, read %d\n", GRAVIS_ADDR_HI(loop_start),
dummy);

	/* loop end */
	register_poke(loc->io_port, VOICE_END_LOW, GRAVIS_ADDR_LOW(loop_end));
	register_poke(loc->io_port, VOICE_END_HI, GRAVIS_ADDR_HI(loop_end));
dummy = register_peek(loc->io_port, VOICE_END_LOW);
PRINTF("wrote %d to VOICE_END_LOW, read %d\n", GRAVIS_ADDR_LOW(loop_end),
dummy);
dummy = register_peek(loc->io_port, VOICE_END_HI);
PRINTF("wrote %d to VOICE_END_HI, read %d\n", GRAVIS_ADDR_HI(loop_end),
dummy);

	/* volume */
	vol = compute_volume(iface, w, vel);
	register_poke(loc->io_port, VOICE_CURRENT_VOL, vol);
	U_DELAY(8);
	register_poke(loc->io_port, VOICE_CURRENT_VOL, vol);
dummy = register_peek(loc->io_port, VOICE_CURRENT_VOL);
PRINTF("wrote %d to VOICE_CURRENT_VOL, read %d\n", vol, dummy);

	/* frequency */
	gravis_freq = freq_to_gravis(iface, adj_freq);
	register_poke(loc->io_port, VOICE_FREQUENCY, gravis_freq);
dummy = register_peek(loc->io_port, VOICE_FREQUENCY);
PRINTF("wrote %d to VOICE_FREQUENCY, read %d\n", gravis_freq, dummy);

	voice_control = 0;
	if (!v->w->data_8_bit)
		voice_control |= VOICEC_16;
	if (v->w->looping)
		voice_control |= VOICEC_LOOP;
	else {
		voice_control &= ~VOICEC_LOOP;
		voice_control |= VOICEC_WAVE_IRQ;
		v->status |= V_STOPPING;
	}
	register_poke(loc->io_port, VOICE_CONTROL, voice_control);
	U_DELAY(8);
	register_poke(loc->io_port, VOICE_CONTROL, voice_control);
dummy = register_peek(loc->io_port, VOICE_CONTROL);
PRINTF("wrote %d to VOICE_CONTROL, read %d\n", voice_control, dummy);
	if (iface->iwave) {
		u_char reg;

		reg = register_peek(loc->io_port, REGISTER_SMSI);
		register_poke(loc->io_port, REGISTER_SMSI, reg & ~0x02);
	}
	UNBLOCK_INTR(s);
}

static void
key_pressure_event(struct midi_iface_gravis *iface, int channel, int pitch,
    int vel)
{

	PRINTF("Key Pressure Event\n");
}

static void
parameter_event(struct midi_iface_gravis *iface, int channel, int param,
    int val)
{

	PRINTF("Parameter Event\n");
}

static void
program_event(struct midi_iface_gravis *iface, int channel, int val)
{
	struct gravis_lm_patch *p;

	if (channel == 9)
		p = iface->drum_programs[val];
	else
		p = iface->normal_programs[val];

	if (p == NULL) {
		PRINTF("%s program number %d not yet loaded\n",
		    (channel == 9) ? "drum" : "melodic", val);
		return;
	}
	iface->channel[channel] = p;
}

static void
chan_pressure_event(struct midi_iface_gravis *iface, int channel, int pressure)
{

	PRINTF("Channel Pressure Event\n");
}

static void
pitch_wheel_event(struct midi_iface_gravis *iface, int channel, int val)
{

	PRINTF("Pitch Wheel Event\n");
}

static void
real_time_event(struct midi_iface_gravis *iface, int mode)
{

	PRINTF("Real Time Event\n");
}

/*
 * I have no idea what expression is, but it seems like something
 * we should deal with in the future.
 *
 * This code is lifted from some version of the voxware driver,
 * but this particular file (gus_vol.c) was written by Greg Lee
 * and does not contain a Copyleft, so I don't think we have any
 * problems.
 */
static short
compute_volume(struct midi_iface_gravis *iface, struct gravis_lm_wave *w,
    int vel)
{
	int i, m, n, x;

	/*
	 * A voice volume of 64 is considered neutral, so adjust the
	 * main volume if something other than this neutral value was
	 * assigned in the patch library.
	 */
	x = 256 + 6 * (w->volume - 64);


	x = vel * 6 + (w->volume / 4) * x;

	x = (x * GRAVIS_VOLUME * GRAVIS_VOLUME) / 10000;

	if (x < 2)
		return (0);
	else if (x > 65535)
		return ((15 << 8) | 255);

	/*
	 * convert to logarithmic form with 4 bit exponent i and 8 bit
	 * mantissa m.
	 */
	n = x;
	i = 7;
	if (n < 128)
		while (i > 0 && n < (1 << i))
			i--;
	else
		while (n > 255) {
			n >>= 1;
			i++;
		}

	/*
	 * Mantissa is part of linear volume not expressed in exponent.
	 * (This is not quite like real logs -- I wonder if it's right.)
	 */
	m = x - (1 << i);

	/* adjust mantissa to 8 bits. */
	if (m > 0) {
		if (i > 8)
			m >>= i - 8;
		else if (i < 8)
			m <<= 8 - i;
	}
	return ((short)((i << 8) + m));
}

static int
pitch_to_freq(int pitch)
{
	int octave, note, note_freq;

	octave = pitch / 12;
	note = pitch % 12;

	note_freq = note_freqs[note];

	if (octave < 5)
		note_freq >>= (5 - octave);
	else if (octave > 5)
		note_freq <<= (octave - 5);

	return (note_freq);
}

static u_short
freq_to_gravis(struct midi_iface_gravis *iface, int note_freq)
{
	u_short fc, fs;

	if (iface->iwave)
		fc = (((note_freq << 10) + 22050) / 44100) << 1;
	else {
		fs = freq_divisor[NUM_VOICES - 14];
		fc = (((note_freq << 9) + (fs >> 1)) / fs) << 1;
	}
	return (fc);
}

/*
 * Configure Iwave memory.
 */
static unsigned long
iwave_mem_cfg(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	unsigned long addr, base, cnt, bank[4];
	int i, ram, s;
	unsigned short lmcfi;
	unsigned char t, t1;

	bank[0] = bank[1] = bank[2] = bank[3] = 0;
	addr = 0;
	base = 0;
	cnt = 0;
	ram = 0;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	s = BLOCK_INTR();
	enter_enhanced_mode(iface);
	lmcfi = register_peek(loc->io_port, REGISTER_LMCFI) & 0xfff0;
	register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x000c);

	/* clear every RAM_STEPth location */
	while (addr < IWAVE_RAM_MAX) {
		poke_data(loc->io_port, addr, 0);
		addr += IWAVE_RAM_STEP;
	}

	/* determine amount of RAM in each bank */
	for (i = 0; i < 4; i++) {
		poke_data(loc->io_port, base, 0xaa);
		poke_data(loc->io_port, base + 1, 0x55);
		t = peek_data(loc->io_port, base);
		t1 = peek_data(loc->io_port, base + 1);
		if (t == 0xaa && t1 == 0x55)
			ram = 1;
		if (ram) {
			while (cnt < IWAVE_BANK_MAX) {
				bank[i] += IWAVE_RAM_STEP;
				cnt += IWAVE_RAM_STEP;
				addr = base + cnt;
				if (peek_data(loc->io_port, addr) == 0xaa)
					break;
			}
		}
		bank[i] = bank[i] >> 10;
		base += IWAVE_BANK_MAX;
		cnt = 0;
		ram = 0;
	}

	if (bank[0] == 256 && bank[1] == 0 && bank[2] == 0 && bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi);
	else if (bank[0] == 256 && bank[1] == 256 && bank[2] == 0 &&
	    bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x01);
	else if (bank[0] == 256 && bank[1] == 256 && bank[2] == 256 &&
	    bank[3] == 256)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x02);
	else if (bank[0] == 256 && bank[1] == 1024 && bank[2] == 0 &&
	    bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x03);
	else if (bank[0] == 256 && bank[1] == 1024 && bank[2] == 1024 &&
	    bank[3] == 1024)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x04);
	else if (bank[0] == 256 && bank[1] == 256 && bank[2] == 1024 &&
	    bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x05);
	else if (bank[0] == 256 && bank[1] == 256 && bank[2] == 1024 &&
	    bank[3] == 1024)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x06);
	else if (bank[0] == 1024 && bank[1] == 0 && bank[2] == 0 &&
	    bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x07);
	else if (bank[0] == 1024 && bank[1] == 1024 && bank[2] == 0 &&
	    bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x08);
	else if (bank[0] == 1024 && bank[1] == 1024 && bank[2] == 1024 &&
	    bank[3] == 1024)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x09);
	else if (bank[0] == 4096 && bank[1] == 0 && bank[2] == 0 &&
	    bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x0a);
	else if (bank[0] == 4096 && bank[1] == 4096 && bank[2] == 0 &&
	    bank[3] == 0)
		register_poke(loc->io_port, REGISTER_LMCFI, lmcfi | 0x0b);
	else {
		PRINTF("\nbank[0] = %d, bank[1] = %d, bank[2] = %d, "
		    "bank[3] = %d\n", bank[0], bank[1], bank[2], bank[3]);
		PRINTF("iface_gravis: holes in Iwave memory\n");
	}

	leave_enhanced_mode(iface);
	UNBLOCK_INTR(s);
	return (bank[0] + bank[1] + bank[2] + bank[3]);
}

static unsigned short
iwave_mem_size(struct midi_iface_gravis *iface)
{
	struct midi_isa_iface *loc;
	unsigned char datum, lmci, t1, t2, t3;
	unsigned long local;

	datum = 0x55;
	local = 0;

	loc = (struct midi_isa_iface *)iface->iface.loc;

	/* select DRAM I/O cycles */
	lmci = register_peek(loc->io_port, REGISTER_LMCI);
	register_poke(loc->io_port, REGISTER_LMCI, lmci & 0xfd);
	for (;;) {
		poke_data(loc->io_port, local, datum);
		poke_data(loc->io_port, local + 1, datum + 1);
		t1 = peek_data(loc->io_port, local);
		t2 = peek_data(loc->io_port, local + 1);
		t3 = peek_data(loc->io_port, 0);
		if ((t1 != datum) || (t2 != datum + 1) || (t3 != 0x55))
			break;
		local += IWAVE_RAM_STEP;
		datum++;
	}
	register_poke(loc->io_port, REGISTER_LMCI, lmci);
	return ((unsigned short)(local >> 10));
}

#endif

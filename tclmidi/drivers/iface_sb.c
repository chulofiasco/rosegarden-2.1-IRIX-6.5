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

#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_SB)

#include "sup_os.h"
#include "sup_iface.h"
#include "iface_sb.h"

int sb_reset(struct midi_isa_iface *loc);
static int sb_wait_rdy_rcv(struct midi_isa_iface *loc);
static int sb_wait_data_avl(struct midi_isa_iface *loc);
static int sb_write_command(struct midi_isa_iface *loc, unsigned char com);

struct midi_iface *
sb_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type, void *vloc)
{
	struct midi_iface_sb *iface;
	struct midi_isa_iface *loc;

	if (type != MIDI_ISA_IFACE)
		return (NULL);
	loc = (struct midi_isa_iface *)vloc;

	if (!sb_reset(loc))
		return (NULL);

	iface = MALLOC_NOWAIT(sizeof(struct midi_iface_sb));
	if (iface == NULL)
		return (NULL);

	/* get board version */
	if (!sb_write_command(loc, SB_VERSION)) {
		FREE(iface, sizeof(struct midi_iface_sb));
		return (NULL);
	}
	if (!sb_wait_data_avl(loc)) {
		FREE(iface, sizeof(struct midi_iface_sb));
		return (NULL);
	}
	iface->maj_ver = INB(loc->io_port + SB_DSP_READ);
	if (!sb_wait_data_avl(loc)) {
		FREE(iface, sizeof(struct midi_iface_sb));
		return (NULL);
	}
	iface->min_ver = INB(loc->io_port + SB_DSP_READ);


	iface->iface.loc = MALLOC_NOWAIT(sizeof(struct midi_isa_iface));
	if (iface->iface.loc == NULL) {
		FREE(iface, sizeof(struct midi_iface_sb));
		return (NULL);
	}

	/* copy location */
	iface->iface.type = type;
	*(struct midi_isa_iface *)iface->iface.loc = *loc;

	iface->iface.softc = softc;
	iface->iface.name = sb_name;
	iface->iface.size = sb_size;
	iface->iface.gen_intr = sb_gen_intr;
	iface->iface.reset = sb_big_reset;
	iface->iface.open = sb_open;
	iface->iface.close = sb_close;
	iface->iface.intr = sb_intr;
	iface->iface.data_avail = sb_data_avail;
	iface->iface.write = sb_write;
	iface->iface.read = sb_read;
	iface->iface.free = sb_free;

	return ((struct midi_iface *)iface);
}

const char *
sb_name(struct midi_iface *mif)
{
	struct midi_iface_sb *iface;

	iface = (struct midi_iface_sb *)mif;

	switch (iface->maj_ver) {
	case 1:
		if (iface->min_ver < 5)
			return ("SoundBlaster 1.0");
		else
			return ("SoundBlaster 1.5");
	case 2:
		return ("SoundBlaster 2.0");
	case 3:
		if (iface->min_ver == 0)
			return ("SoundBlaster Pro");
		else
			return ("SoundBlaster Pro 2");
	case 4:
		if (iface->min_ver < 11)
			return ("SoundBlaster 16");
		else if (iface->min_ver == 11)
			return ("SoundBlaster 16 SCSI-2");
		else
			return ("SoundBlaster AWE32");
	}
	return ("Unrecognized SoundBlaster");
}

void
sb_size(struct midi_iface *mif, void *size)
{
	int *s;

	s = (int *)size;
	/* I think it only uses 16 i/o ports */
	*s = 16;
}

void
sb_gen_intr(struct midi_iface *mif)
{
	struct midi_iface_sb *iface;
	struct midi_isa_iface *loc;

	iface = (struct midi_iface_sb *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	(void)sb_write_command(loc, SB_GEN_INTR);
	/* should clear intr. */
	sb_wait_data_avl(loc);
}

int
sb_big_reset(struct midi_iface *mif)
{
	struct midi_iface_sb *iface;
	struct midi_isa_iface *loc;

	iface = (struct midi_iface_sb *)mif;
	loc = (struct midi_isa_iface *)mif->loc;
	sb_reset(loc);

	if (!sb_write_command(loc, SB_UART_MODE))
		return (0);

	return (1);
}

int
sb_open(struct midi_iface *mif)
{

	/*
	 * don't do anything.  midiopen already calls fullreset which
	 * puts the board in UART mode.
	 */
	return (0);
}

void
sb_close(struct midi_iface *mif)
{
}

int
sb_intr(struct midi_iface *mif)
{
	struct midi_iface_sb *iface;
	struct midi_isa_iface *loc;
	u_char code;

	iface = (struct midi_iface_sb *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	code = INB(loc->io_port + SB_DSP_READ);
	return (code);
}

int
sb_data_avail(struct midi_iface *mif)
{
	struct midi_isa_iface *loc;
	int flag;

	loc = (struct midi_isa_iface *)mif->loc;
	flag = INB(loc->io_port + SB_DSP_DATA_AVAIL);
	return (flag & 0x80);
}

void
sb_write(struct midi_iface *mif, struct event *event)
{
	struct midi_iface_sb *iface;
	struct midi_isa_iface *loc;
	int i;
	u_char byte;

	iface = (struct midi_iface_sb *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	for (i = 0; i < stynamic_len(&event->data); i++) {
		byte = stynamic_get_byte(&event->data, i);
		sb_wait_rdy_rcv(loc);
		sb_write_command(loc, byte);
	}
}

u_char
sb_read(struct midi_iface *mif)
{
	struct midi_iface_sb *iface;
	struct midi_isa_iface *loc;
	u_char byte;

	iface = (struct midi_iface_sb *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	sb_wait_data_avl(loc);
	byte = INB(loc->io_port + SB_DSP_READ);
	return (byte);
}

void
sb_free(struct midi_iface *mif)
{
	struct midi_iface_sb *iface;

	FREE(mif->loc, sizeof(struct midi_isa_iface));

	iface = (struct midi_iface_sb *)mif;
	FREE(iface, sizeof(struct midi_iface_sb));
}


static int
sb_wait_rdy_rcv(struct midi_isa_iface *loc)
{
	int flag, i;

	for (i = 0; i < MIDI_TRIES; i++) {
		flag = INB(loc->io_port + SB_DSP_STATUS) & 0x80;
		if (flag == 0)
			break;
		U_DELAY(10);
	}
	if (i == MIDI_TRIES)
		return (0);
	return (1);
}

int
sb_reset(struct midi_isa_iface *loc)
{

	/* bit 0 & 1 high ... */
	OUTB(loc->io_port + SB_DSP_RESET, 0x01);
	U_DELAY(250);
	/* then low */
	OUTB(loc->io_port + SB_DSP_RESET, 0x00);

	if (!sb_wait_data_avl(loc))
		return (0);
	if (INB(loc->io_port + SB_DSP_READ) != 0xaa)
		return (0);
	return (1);
}

static int
sb_wait_data_avl(struct midi_isa_iface *loc)
{
	int flag, i;

	for (i = 0; i < MIDI_TRIES; i++) {
		flag = INB(loc->io_port + SB_DSP_DATA_AVAIL) & 0x80;
		if (flag)
			break;
		U_DELAY(10);
	}
	if (i == MIDI_TRIES)
		return (0);
	return (1);
}

static int
sb_write_command(struct midi_isa_iface *loc, unsigned char com)
{
	if (!sb_wait_rdy_rcv(loc))
		return (0);
	OUTB(loc->io_port + SB_DSP_COMMAND, com);
	return (1);
}
#endif

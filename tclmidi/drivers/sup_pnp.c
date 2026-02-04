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

#include "sup_pnp.h"
#include "sup_os.h"

/*
 * Abandon all hope, ye who enter here.
 * Here's all the PnP bullshit needed by the new GUS Pnp (which is
 * now the only ones available.  God only knows what these functions
 * will do to any other PnP cards in your system.  Where are jumpers
 * when you need 'em?
 */

static unsigned short pnp_rd = 0;

/*
 * Send the PnP key that takes the cards out of wait_for_key state.
 */
void
midi_pnp_send_key(void)
{
	int i;
	unsigned char code, msb;

	code = 0x6a;

	OUTB(PNP_ADDR, 0);
	OUTB(PNP_ADDR, 0);
	OUTB(PNP_ADDR, code);

	for (i = 0; i < 32; i++) {
		msb = ((code & 0x01) ^ ((code & 0x02) >> 1)) << 7;
		code = (code >> 1) | msb;
		OUTB(PNP_ADDR, code);
	}
}

/*
 * Isolate PnP cards.
 * PnP cards must be in wait_for_key state.
 * returns number of PnP cards detected and isolated.
 */
int
midi_pnp_isol(void)
{
	int i;
	unsigned short data, initial_rd;
	unsigned char bit, checksum, chksum, csn, iteration;

	initial_rd = pnp_rd;

	checksum = 0x6a;
	chksum = 0;
	bit = 0;
	pnp_rd = 0x203;
	csn = 0;
	iteration = 1;

	midi_pnp_send_key();
	midi_pnp_wake(0);
	OUTB(PNP_ADDR, PNP_REG_SET_RD);
	OUTB(PNP_WRITE, (unsigned char)(pnp_rd >> 2));
	OUTB(PNP_ADDR, PNP_REG_SER_ISOL);
	/* delay 1ms before first pair of isol reads */
	U_DELAY(1000);

	for (;;) {
		for (i = 0; i < 64; i++) {
			data = ((unsigned short)INB(pnp_rd)) << 8;
			data = data | INB(pnp_rd);
			/* delay 250us between all other isol reads */
			U_DELAY(250);
			if (data == 0x55aa)
				bit = 1;
			checksum = ((((checksum ^ (checksum >> 1)) & 0x01)
			    ^ bit) << 7) | (checksum >> 1);
			bit = 0;
		}
		for (; i < 72; i++) {
			data = ((unsigned short)INB(pnp_rd)) << 8;
			data = data | (INB(pnp_rd) & 0x00ff);
			U_DELAY(250);
			if (data == 0x55aa)
				chksum = chksum | (1 << (i - 64));
		}
		if ((checksum != 0) && (checksum == chksum)) {
			csn++;
			OUTB(PNP_ADDR, PNP_REG_CSN);
			OUTB(PNP_WRITE, csn);
			iteration++;
			midi_pnp_wake(0);
			OUTB(PNP_ADDR, PNP_REG_SER_ISOL);
		}

		if (((checksum == 0x00) || (chksum != checksum)) &&
		    (iteration == 1)) {
			if (pnp_rd > 0x23f) {
				pnp_rd = initial_rd;
				midi_pnp_wait_for_key();
				return (-1);
			}
			pnp_rd += 4;
			checksum = 0x6a;
			chksum = 0;
			midi_pnp_wake(0);
			OUTB(PNP_ADDR, PNP_REG_SET_RD);
			OUTB(PNP_WRITE, (unsigned char)(pnp_rd >> 2));
			OUTB(PNP_ADDR, PNP_REG_SER_ISOL);
		}

		if (((checksum == 0) || (chksum != checksum)) &&
		    (iteration > 1))
			break;

		checksum = 0x6a;
		chksum = 0;
		bit = 0;
	}

	midi_pnp_wait_for_key();
	return (csn);
}

void
midi_pnp_peek(unsigned short bytes, unsigned char *data)
{
	int i;
	unsigned char datum;

	for (i = 0; i < bytes; i++) {
		OUTB(PNP_ADDR, PNP_REG_STATUS);

		for (;;)
			if (INB(pnp_rd) & PNP_DATA_RDY)
				break;
		OUTB(PNP_ADDR, PNP_REG_RES_DATA);
		datum = INB(pnp_rd);
		if (data != 0)
			*(data++) = datum;
	}
}

/*
 * enter wait for key state */
void
midi_pnp_wait_for_key(void)
{

	OUTB(PNP_ADDR, PNP_REG_CONF_CNTL);
	OUTB(PNP_WRITE, PNP_WAIT_FOR_KEY);
}

void
midi_pnp_activate(unsigned char bool)
{
	int s;

	s = BLOCK_INTR();
	OUTB(PNP_ADDR, PNP_REG_ACTIVATE_DEV);
	OUTB(PNP_WRITE, bool);
	UNBLOCK_INTR(s);
}

void
midi_pnp_device(unsigned char dev)
{
	int s;

	s = BLOCK_INTR();
	OUTB(PNP_ADDR, PNP_REG_LOGICAL_DEV);
	OUTB(PNP_WRITE, dev);
	UNBLOCK_INTR(s);
}

void
midi_pnp_wake(unsigned char csn)
{
	int s;

	s = BLOCK_INTR();
	OUTB(PNP_ADDR, PNP_REG_WAKE);
	OUTB(PNP_WRITE, csn);
	UNBLOCK_INTR(s);
}

/*
 * Look for a PnP card with the specified vendor number.  It will
 * only search CSN's above the specified start_csn.
 * Returns the CSN of the device if found, 0 otherwise.
 * Must already be isolated, and in wait for key state.
 * After calling this function PnP will be left in the wait for key state.
 */
unsigned char
midi_pnp_ping(unsigned char start_csn, unsigned long vendor_id)
{
	unsigned char csn;
	unsigned long vendor;

	vendor_id &= 0xfffffff0;
	midi_pnp_send_key();
	for (csn = start_csn; csn < 20; csn++) {
		midi_pnp_wake(csn);
		midi_pnp_peek(4, (unsigned char *)&vendor);
		if ((vendor & 0xfffffff0) == vendor_id) {
			midi_pnp_wait_for_key();
			return (csn);
		}
	}
	midi_pnp_wait_for_key();
	return (0);
}

/*
 * Similar to midi_pnp_ping, but is called when midi_pnp_isol returns
 * zero.  Just because midi_pnp_isol failed to find any PnP cards,
 * doesn't mean they don't exist.  For example, PnP cards keep their
 * CSNs after soft boots, and thus won't respond to the isolation protocol.
 * Or if the isolation step has already been run once, the board won't
 * respond again.
 *
 * However, if midi_pnp_isol failed then we might not have a valid
 * pnp_rd value.  This function will ping with all pnp_rd values
 * until it either find the card or reaches the last pnp_rd value.
 * If it finds a card, it will set pnp_rd to the last used value.
 * If it doesn't find a card, it will leave pnp_rd as it was before
 * this function was called.
 */
unsigned char
midi_pnp_ping_no_isol(unsigned char start_csn, unsigned long vendor_id)
{
	unsigned short orig_rd;
	unsigned char csn;

	orig_rd = pnp_rd;
	pnp_rd = 0x203;
	for (pnp_rd = 0x203; pnp_rd < 0x23f; pnp_rd += 4) {
		csn = midi_pnp_ping(start_csn, vendor_id);
		if (csn != 0)
			return (csn);
	}
	pnp_rd = orig_rd;
	return (0);
}

int
midi_pnp_configure(unsigned char csn, unsigned char dev,
    struct midi_pnp_config *conf)
{
	struct midi_pnp_dev_config *c;
	unsigned long mask;
	int do_next, i, j;
	unsigned char reg;
	struct midi_pnp_logical_dev *ld_p;
	struct midi_pnp_dep_function *df_p;
	struct midi_pnp_irq_config *irq_p;
	struct midi_pnp_dma_config *dma_p;
	struct midi_pnp_io_config *io_p;

	c = midi_pnp_get_config(csn);
	if (c == NULL) {
		PRINTF("Couldn't get PnP Configuration\n");
		return (0);
	}
	if (dev >= c->num_logical_dev) {
		PRINTF("dev (%d) to large - max %d\n", dev, c->num_logical_dev);
		midi_pnp_free_config(c);
		return (0);
	}
	ld_p = &c->logical_dev[dev];

	midi_pnp_send_key();
	midi_pnp_wake(csn);
	midi_pnp_device(dev);

	/* configure I/O ports */
	do_next = 0;
	for (j = 0; j < ld_p->num_dep_func; j++) {
		df_p = &ld_p->dep_func[j];
		mask = PNP_CONF_IO_PORT_0;
		reg = PNP_REG_IO_PORT_0_HI;
		for (i = 0; i < PNP_MAX_CONF_IO; i++) {
			io_p = &df_p->io[i];
			if (conf->flags & mask) {
				if (i > df_p->num_io)
					do_next = 1;
				else if (conf->io_port[i] < io_p->min
				    || conf->io_port[i] > io_p->max)
					do_next = 1;
				else if (conf->io_port[i] % io_p->alignment
				    != 0)
					do_next = 1;
				if (!do_next) {
					OUTB(PNP_ADDR, reg);
					OUTB(PNP_WRITE,
					    (conf->io_port[i] >> 8) & 0xff);
					OUTB(PNP_ADDR, reg + 1);
					OUTB(PNP_WRITE,
					    conf->io_port[i] & 0xff);
				}
			}
			if (do_next)
				break;
			mask <<= 1;
			reg += 2;
		}
		if (do_next) {
			do_next = 0;
			continue;
		}

		/* configure IRQs */
		mask = PNP_CONF_IRQ_0;
		reg = PNP_REG_IRQ_0;
		for (i = 0; i < PNP_MAX_CONF_IRQ; i++) {
			irq_p = &df_p->irq[i];
			if (conf->flags & mask) {
				if (i > df_p->num_irq)
					do_next = 1;
				else if (!(irq_p->mask & (1 << conf->irq[i])))
					do_next = 1;
				if (!do_next) {
					OUTB(PNP_ADDR, reg);
					OUTB(PNP_WRITE, conf->irq[i]);
				}
			}
			if (do_next)
				break;
			mask <<= 1;
			reg += 2;
		}
		if (do_next) {
			do_next = 0;
			continue;
		}

		/* configure DMAs */
		mask = PNP_CONF_DMA_0;
		reg = PNP_REG_DMA_0;
		for (i = 0; i < PNP_MAX_CONF_DMA; i++) {
			dma_p = &df_p->dma[i];
			if (conf->flags & mask) {
				if (i > df_p->num_dma)
					do_next = 1;
				else if (!(dma_p->mask & (1 << conf->dma[i])))
					do_next = 1;
				if (!do_next) {
					OUTB(PNP_ADDR, reg);
					OUTB(PNP_WRITE, conf->dma[i]);
				}
			}
			if (do_next)
				break;
			mask <<= 1;
			reg++;
		}
		if (!do_next)
			break;
		else
			do_next = 0;
	}
	if (j == ld_p->num_dep_func) {
		midi_pnp_free_config(c);
		midi_pnp_wait_for_key();
		return (0);
	} else {
		midi_pnp_free_config(c);
		midi_pnp_activate(1);
		midi_pnp_wait_for_key();
		return (1);
	}
}

struct midi_pnp_dev_config *
midi_pnp_get_config(unsigned char csn)
{
	unsigned char raw[1024];
	unsigned char name, tag;
	char *str_p;
	int done, i, len;
	struct midi_pnp_dev_config *conf;
	struct midi_pnp_logical_dev *ld_p;
	struct midi_pnp_dep_function *df_p;
	struct midi_pnp_irq_config *irq_p;
	struct midi_pnp_io_config *io_p;
	struct midi_pnp_dma_config *dma_p;
	struct midi_pnp_mem_config *mem_p;

	conf = MALLOC_NOWAIT(sizeof(struct midi_pnp_dev_config));
	if (conf == NULL) {
		PRINTF("Not enough memory to get PnP config\n");
		return (NULL);
	}

	midi_pnp_send_key();
	midi_pnp_wake(csn);

	ld_p = NULL;
	df_p = NULL;

	conf->num_logical_dev = 0;
	conf->id_str[0] = '\0';

	/* skip card id stuff */
	midi_pnp_peek(9, &raw[0]);

	done = 0;
	while (!done) {
		midi_pnp_peek(1, &tag);
		if (tag & 0x80) {
			/* large item */
			name = tag & 0x7f;
			midi_pnp_peek(2, &raw[0]);
			len = raw[0];
			len |= (raw[1] << 8) & 0xff00;
			midi_pnp_peek(len, &raw[0]);
			switch (name) {
			case 0x01:
				/* memory 24 config */
				if (ld_p == NULL) {
					PRINTF("No logical device set\n");
					break;
				}
				if (df_p->num_mem == PNP_MAX_CONF_MEM) {
					PRINTF("Too many mem configs\n");
					break;
				}
				mem_p = &df_p->mem[df_p->num_mem++];
				mem_p->info = raw[0];
				mem_p->min = (raw[1] << 8) & 0xff00;
				mem_p->min |= (raw[2] << 16) & 0xff0000;
				mem_p->max = (raw[3] << 8) & 0xff00;
				mem_p->max |= (raw[4] << 16) & 0xff0000;
				mem_p->align = raw[5];
				mem_p->align |= (raw[6] << 8) & 0xff00;
				mem_p->len_256 = raw[7];
				mem_p->len_256 |= (raw[8] << 8) & 0xff00;
				break;
			case 0x02:
				/* ANSI id string */
				if (ld_p != NULL)
					str_p = ld_p->id_str;
				else
					str_p = conf->id_str;
				for (i = 0; i < len; i++) {
					if (i == 511) {
						str_p[i] = '\0';
						break;
					}
					str_p[i] = raw[i];
				}
				str_p[i] = '\0';
				break;
			case 0x03:
				/* UNICODE id string */
				break;
			case 0x04:
				/* vendor defined */
				break;
			case 0x05:
				/* 32 bit memory */
				if (ld_p == NULL) {
					PRINTF("No logical device set\n");
					break;
				}
				if (df_p->num_mem == PNP_MAX_CONF_MEM) {
					PRINTF("Too many mem configs\n");
					break;
				}
				mem_p = &df_p->mem[df_p->num_mem++];
				mem_p->info = raw[0];
				mem_p->min = raw[1];
				mem_p->min |= (raw[2] << 8) & 0xff00;
				mem_p->min |= (raw[3] << 16) & 0xff0000;
				mem_p->min |= (raw[4] << 24) & 0xff000000;
				mem_p->max = raw[5];
				mem_p->max |= (raw[6] << 8) & 0xff00;
				mem_p->max |= (raw[7] << 16) & 0xff0000;
				mem_p->max |= (raw[8] << 24) & 0xff000000;
				mem_p->align = raw[9];
				mem_p->align |= (raw[10] << 8) & 0xff00;
				mem_p->align |= (raw[11] << 16) & 0xff0000;
				mem_p->align |= (raw[12] << 24) & 0xff000000;
				mem_p->len = raw[13];
				mem_p->len |= (raw[14] << 8) & 0xff00;
				mem_p->len |= (raw[15] << 16) & 0xff0000;
				mem_p->len |= (raw[16] << 24) & 0xff000000;
				mem_p->len_256 = 0;
				break;
			case 0x06:
				/* memory 32 fixed */
				if (ld_p == NULL) {
					PRINTF("No logical device set\n");
					break;
				}
				if (df_p->num_mem == PNP_MAX_CONF_MEM) {
					PRINTF("Too many mem configs\n");
					break;
				}
				mem_p = &df_p->mem[df_p->num_mem++];
				mem_p->info = raw[0];
				mem_p->min = raw[1];
				mem_p->min |= (raw[2] << 8) & 0xff00;
				mem_p->min |= (raw[3] << 16) & 0xff0000;
				mem_p->min |= (raw[4] << 24) & 0xff000000;
				mem_p->len = raw[5];
				mem_p->len |= (raw[6] << 8) & 0xff00;
				mem_p->len |= (raw[7] << 16) & 0xff0000;
				mem_p->len |= (raw[8] << 24) & 0xff000000;
				mem_p->max = mem_p->min;
				mem_p->align = mem_p->min;
				break;
			default:
				break;
			}
		} else {
			/* small item */
			len = tag & 0x07;
			midi_pnp_peek(len, &raw[0]);
			name = (tag >> 3) & 0x0f;
			switch (name) {
			case 0x00:
				break;
			case 0x01:
				/* version number */
				conf->ver_maj = (raw[0] >> 4) & 0x0f;
				conf->ver_min = raw[0] & 0x0f;
				conf->ven_ver_maj = (raw[1] >> 4) & 0x0f;
				conf->ven_ver_min = raw[1] & 0x0f;
				break;
			case 0x02:
				/* logical device id */
				ld_p = &conf->logical_dev[
				    conf->num_logical_dev++];
				ld_p->id_str[0] = '\0';

				/* fill global structure first */
				df_p = &ld_p->global_dep;
				/* clear global structure */
				df_p->num_irq = 0;
				df_p->num_io = 0;
				df_p->num_dma = 0;
				df_p->num_mem = 0;
				/* no dependent functions yet */
				ld_p->num_dep_func = 0;

				ld_p->id.id[0] = ((raw[0] >> 2) & 0x1f) + 'A'
				    + 1;
				ld_p->id.id[1] = (((raw[0] << 3) & 0x18) |
				    ((raw[1] >> 5) & 0x07)) + 'A' + 1;
				ld_p->id.id[2] = (raw[1] & 0x1f) + 'A' + 1;
				ld_p->id.id[3] = '\0';
				ld_p->id.num[0] = (raw[2] >> 4) & 0x0f;
				ld_p->id.num[1] = raw[2] & 0x0f;
				ld_p->id.num[2] = (raw[3] >> 4) & 0x0f;
				ld_p->id.num[3] = raw[3] & 0x0f;
				break;
			case 0x03:
				/* compatible device ID */
				ld_p->id.compat_id[0] = ((raw[0] >> 2) & 0x1f)
				    + 'A' + 1;
				ld_p->id.compat_id[1] = (((raw[0] << 3) &
				    0x18) | ((raw[1] >> 5) & 0x07)) + 'A' + 1;
				ld_p->id.compat_id[2] = (raw[1] & 0x1f) + 'A'
				    + 1;
				ld_p->id.compat_id[3] = '\0';
				ld_p->id.compat_num[0] = (raw[2] >> 4) & 0x0f;
				ld_p->id.compat_num[1] = raw[2] & 0x0f;
				ld_p->id.compat_num[2] = (raw[3] >> 4) & 0x0f;
				ld_p->id.compat_num[3] = raw[3] & 0x0f;
				break;
			case 0x04:
				/* IRQ format - assumes no more than 4 */
				if (ld_p == NULL) {
					PRINTF("No logical device set\n");
					break;
				}
				if (df_p->num_irq == PNP_MAX_CONF_IRQ) {
					PRINTF("No room for %dth IRQ config\n",
					    PNP_MAX_CONF_IRQ);
					break;
				}
				irq_p = &df_p->irq[df_p->num_irq++];
				irq_p->mask = raw[0];
				irq_p->mask |= (raw[1] << 8) & 0xff00;
				irq_p->info = raw[2];
				break;
			case 0x05:
				/* DMA foramt - assumes no more than 4 */
				if (ld_p == NULL) {
					PRINTF("No logical device set\n");
					break;
				}
				if (df_p->num_dma == PNP_MAX_CONF_DMA) {
					PRINTF("No room for %dth DMA config\n",
					    PNP_MAX_CONF_DMA);
					break;
				}
				dma_p = &df_p->dma[df_p->num_dma++];
				dma_p->mask = raw[0];
				dma_p->info = raw[1];
				break;
			case 0x06:
				/* Dependent Functions */
				if (ld_p == NULL) {
					PRINTF("No logical dev for dep func\n");
					break;
				}
				if (ld_p->num_dep_func ==
				    PNP_MAX_DEP_FUNCTIONS) {
					PRINTF("Too many depe functions\n");
					break;
				}
				df_p = &ld_p->dep_func[ld_p->num_dep_func++];
				/* copy global settings */
				df_p->num_irq = ld_p->global_dep.num_irq;
				for (i = 0; i < df_p->num_irq; i++)
					df_p->irq[i] = ld_p->global_dep.irq[i];
				df_p->num_io = ld_p->global_dep.num_io;
				for (i = 0; i < df_p->num_io; i++)
					df_p->io[i] = ld_p->global_dep.io[i];
				df_p->num_dma = ld_p->global_dep.num_dma;
				for (i = 0; i < df_p->num_dma; i++)
					df_p->dma[i] = ld_p->global_dep.dma[i];
				df_p->num_mem = ld_p->global_dep.num_mem;
				for (i = 0; i < df_p->num_mem; i++)
					df_p->mem[i] = ld_p->global_dep.mem[i];
				if (len == 1)
					df_p->priority = raw[0];
				else
					df_p->priority = 0;
				break;
			case 0x07:
				/* end dependent function */
				break;
			case 0x08:
				/* I/O Port Descriptor */
				if (ld_p == NULL) {
					PRINTF("No logical device set\n");
					break;
				}
				if (df_p->num_io == PNP_MAX_CONF_IO) {
					PRINTF("No room for %dth IO config\n",
					    PNP_MAX_CONF_IO);
					break;
				}
				io_p = &df_p->io[df_p->num_io++];
				
				io_p->isa_16_bit = raw[0] & 0x01;
				io_p->min = raw[1];
				io_p->min |= (raw[2] << 8) & 0xff00;
				io_p->max = raw[3];
				io_p->max |= (raw[4] << 8) & 0xff00;
				io_p->alignment = raw[5];
				io_p->range = raw[6];
				break;
			case 0x09:
				/* fixed I/O port */
				if (ld_p == NULL) {
					PRINTF("No logical device set\n");
					break;
				}
				if (df_p->num_io == PNP_MAX_CONF_IO) {
					PRINTF("No room for %dth IO config\n",
					    PNP_MAX_CONF_IO);
					break;
				}
				io_p = &df_p->io[df_p->num_io++];

				io_p->isa_16_bit = 0;
				io_p->min = raw[0];
				io_p->min |= (raw[1] << 8) & 0x0300;
				io_p->max = io_p->min;
				io_p->alignment = 1;
				io_p->range = raw[2];
				break;
			case 0x0f:
				/* End */
				/* Should probably do the check sum */
				done = 1;
				break;
			default:
				break;
			}
		}
	}

	midi_pnp_wait_for_key();
	return (conf);
}

void
midi_pnp_free_config(struct midi_pnp_dev_config *c)
{

	FREE(c, sizeof(struct midi_pnp_dev_config));
}

void
midi_pnp_print_dev_config(unsigned char csn, unsigned char dev)
{
	struct midi_pnp_dev_config *conf;
	int i, j, k;
	struct midi_pnp_logical_dev *ld_p;
	struct midi_pnp_dep_function *df_p;
	struct midi_pnp_irq_config *irq_p;
	struct midi_pnp_dma_config *dma_p;
	struct midi_pnp_io_config *io_p;

	conf = midi_pnp_get_config(csn);
	if (conf == NULL) {
		PRINTF("Couldn't get PnP Configuration\n");
		return;
	}

	if (dev >= conf->num_logical_dev) {
		PRINTF("dev (%d) to large - max %d\n", dev,
		    conf->num_logical_dev);
		midi_pnp_free_config(conf);
		return;
	}
	ld_p = &conf->logical_dev[dev];

	PRINTF("ID STRING: %s\n", conf->id_str);
	PRINTF("VER = %d.%d, VEN_VER = %d.%d\n", conf->ver_maj, conf->ver_min,
	    conf->ven_ver_maj, conf->ven_ver_min);
	PRINTF("LOGICAL DEVICE: #%d %s %s%x%x%x\n", dev, ld_p->id_str,
	    ld_p->id.id, ld_p->id.num[0], ld_p->id.num[1], ld_p->id.num[2]);
	for (i = 0; i < ld_p->num_dep_func; i++) {
		df_p = &ld_p->dep_func[i];
		PRINTF("DEPENDENT FUNCTION #%d PRIORITY = %d\n", i,
		    (int)df_p->priority);
		for (j = 0; j < df_p->num_irq; j++) {
			irq_p = &df_p->irq[j];
			PRINTF("\tIRQ %d =", j);
			for (k = 0; k < 16; k++)
				if (irq_p->mask & (1 << k))
					PRINTF(" %d", k);
			PRINTF("\n");
		}
		for (j = 0; j < df_p->num_dma; j++) {
			dma_p = &df_p->dma[j];
			PRINTF("\tDMA %d =", j);
			if (dma_p->mask == 0)
				PRINTF("NULL DMA");
			else {
				for (k = 0; k < 8; k++)
					if (dma_p->mask & (1 << k))
						PRINTF(" %d", k);
			}
			PRINTF("\n");
		}
		for (j = 0; j < df_p->num_io; j++) {
			io_p = &df_p->io[j];
			if (io_p->range == 0)
				PRINTF("\tIO %d = NULL IO\n", j);
			else
				PRINTF("\tIO %d = min 0x%x, max 0x%x, "
				    "align = 0x%x\n", j, io_p->min, io_p->max,
				    io_p->alignment);
		}
	}
	midi_pnp_free_config(conf);
}

void
midi_pnp_print_cur_config(unsigned char csn, unsigned char dev)
{
	unsigned char dma, irq, reg;
	unsigned short port;
	int i;

	midi_pnp_send_key();
	midi_pnp_wake(csn);
	midi_pnp_device(dev);

	reg = PNP_REG_IO_PORT_0_HI;
	for (i = 0; i < PNP_MAX_CONF_IO; i++) {
		OUTB(PNP_ADDR, reg);
		port = INB(pnp_rd);
		port = (port << 8) & 0xff00;
		OUTB(PNP_ADDR, reg + 1);
		port |= INB(pnp_rd);
		PRINTF("I/O port #%d = 0x%x\n", i, port);
		reg += 2;
	}

	reg = PNP_REG_IRQ_0;
	for (i = 0; i < PNP_MAX_CONF_IRQ; i++) {
		OUTB(PNP_ADDR, reg);
		irq = INB(pnp_rd);
		PRINTF("IRQ #%d = %d\n", i, irq);
		reg += 2;
	}

	reg = PNP_REG_DMA_0;
	for (i = 0; i < PNP_MAX_CONF_DMA; i++) {
		OUTB(PNP_ADDR, reg);
		dma = INB(pnp_rd);
		PRINTF("DMA #%d = %d\n", i, dma);
		reg++;
	}

	midi_pnp_wait_for_key();
}

void
midi_pnp_dump_cfg_data(unsigned char csn)
{
	int i;
	unsigned short len;
	unsigned char raw[1024], tag;

	midi_pnp_send_key();
	midi_pnp_wake(csn);
	midi_pnp_peek(9, raw);
	midi_pnp_peek(1, &tag);
	while (tag != 0x79) {
		PRINTF("%02x ", tag);
		if (tag & 0x80) {
			midi_pnp_peek(2, raw);
			len = raw[0];
			len |= (raw[1] << 8) & 0xff00;
			PRINTF("%02x %02x ", raw[0], raw[1]);
			midi_pnp_peek(len, raw);
			for (i = 0; i < len; i++)
				PRINTF("%02x ", raw[i]);
			PRINTF("\n");
		} else {
			len = tag & 0x07;
			midi_pnp_peek(len, raw);
			for (i = 0; i < len; i++)
				PRINTF("%02x ", raw[i]);
			PRINTF("\n");
		}
		midi_pnp_peek(1, &tag);
	}
	midi_pnp_peek(1, raw);
	PRINTF("%02x %02x\n", tag, raw[0]);

	midi_pnp_wait_for_key();
}

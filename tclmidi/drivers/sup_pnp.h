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

#ifndef SUP_PNP_H
#define SUP_PNP_H

/* PnP stuff */
#define PNP_ADDR			0x279
#define PNP_WRITE			0xa79

#define PNP_REG_SET_RD			0x00	/* set read register */
#define PNP_REG_SER_ISOL		0x01	/* enter serial isolation */
#define PNP_REG_CONF_CNTL		0x02	/* config control */
#define PNP_REG_WAKE			0x03	/* wake */
#define PNP_REG_RES_DATA		0x04	/* resource data */
#define PNP_REG_STATUS			0x05	/* status */
#define PNP_REG_CSN			0x06	/* set CSN */
#define PNP_REG_LOGICAL_DEV		0x07	/* logical device num */
#define PNP_REG_ACTIVATE_DEV		0x30	/* activate device */

#define PNP_REG_IO_PORT_0_HI		0x60
#define PNP_REG_IO_PORT_0_LOW		0x61
#define PNP_REG_IO_PORT_1_HI		0x62
#define PNP_REG_IO_PORT_1_LOW		0x63
#define PNP_REG_IO_PORT_2_HI		0x64
#define PNP_REG_IO_PORT_2_LOW		0x65
#define PNP_REG_IO_PORT_3_HI		0x66
#define PNP_REG_IO_PORT_3_LOW		0x67
#define PNP_REG_IO_PORT_4_HI		0x68
#define PNP_REG_IO_PORT_4_LOW		0x69
#define PNP_REG_IO_PORT_5_HI		0x6a
#define PNP_REG_IO_PORT_5_LOW		0x6b
#define PNP_REG_IO_PORT_6_HI		0x6c
#define PNP_REG_IO_PORT_6_LOW		0x6d
#define PNP_REG_IO_PORT_7_HI		0x6e
#define PNP_REG_IO_PORT_7_LOW		0x6f

#define PNP_REG_IRQ_0			0x70
#define PNP_REG_IRQ_1			0x72

#define PNP_REG_DMA_0			0x74
#define PNP_REG_DMA_1			0x75

/* status register bits */
#define PNP_DATA_RDY			(1 << 0)

/* configuration control bits */
#define PNP_RESET			(1 << 0)
#define PNP_WAIT_FOR_KEY		(1 << 1)
#define PNP_RESET_CSN			(1 << 2)

/* configuration flags determine which fields need to be configured */
#define PNP_CONF_IO_PORT_0		(1 << 0)
#define PNP_CONF_IO_PORT_1		(1 << 1)
#define PNP_CONF_IO_PORT_2		(1 << 2)
#define PNP_CONF_IO_PORT_3		(1 << 3)
#define PNP_CONF_IO_PORT_4		(1 << 4)
#define PNP_CONF_IO_PORT_5		(1 << 5)
#define PNP_CONF_IO_PORT_6		(1 << 6)
#define PNP_CONF_IO_PORT_7		(1 << 7)
#define PNP_CONF_IRQ_0			(1 << 8)
#define PNP_CONF_IRQ_1			(1 << 9)
#define PNP_CONF_DMA_0			(1 << 10)
#define PNP_CONF_DMA_1			(1 << 11)

/* configuration structure contains the data for configuring a device */
struct midi_pnp_config {
	unsigned	long flags;
	unsigned	short io_port[8];
	unsigned	char irq[2];
	unsigned	char dma[2];
};

/* PnP configuration data read from board */
/* I should probably differentiate between memory and 32 bit memory */
#define PNP_MAX_CONF_IRQ		2
#define PNP_MAX_CONF_IO			8
#define PNP_MAX_CONF_DMA		2
#define PNP_MAX_CONF_MEM		4


struct midi_pnp_irq_config {
	unsigned	short mask;
	unsigned	char info;
};

struct midi_pnp_dma_config {
	unsigned	char mask;
	unsigned	char info;
};

struct midi_pnp_io_config {
	unsigned	char isa_16_bit;
	unsigned	short min;
	unsigned	short max;
	unsigned	char alignment;
	unsigned	char range;
};

struct midi_pnp_mem_config {
	unsigned	char info;
	unsigned	long min;
	unsigned	long max;
	unsigned	long align;
	unsigned	long len_256;
	unsigned	long len;
};

struct midi_pnp_log_dev_id {
	char	id[4];
	unsigned	char num[4];
	char	compat_id[4];
	unsigned	char compat_num[4];
};


struct midi_pnp_dep_function {
	unsigned	char priority;
	struct	midi_pnp_irq_config irq[PNP_MAX_CONF_IRQ];
	int	num_irq;
	struct	midi_pnp_io_config io[PNP_MAX_CONF_IO];
	int	num_io;
	struct	midi_pnp_dma_config dma[PNP_MAX_CONF_DMA];
	int	num_dma;
	struct	midi_pnp_mem_config mem[PNP_MAX_CONF_MEM];
	int	num_mem;
};

/* I don't know what the max is - picking abritrary value */
#define PNP_MAX_DEP_FUNCTIONS	10

struct midi_pnp_logical_dev {
	struct	midi_pnp_log_dev_id id;

	/* global settings apply to all dependent functions */
	struct	midi_pnp_dep_function global_dep;

	struct	midi_pnp_dep_function	dep_func[PNP_MAX_DEP_FUNCTIONS];
	int	num_dep_func;

	char	id_str[512];
};

/* This is just an abritrary max, I don't know what the real max is */
#define PNP_MAX_LOGICAL_DEVS		10

struct midi_pnp_dev_config {
	unsigned	char ver_maj;
	unsigned	char ver_min;
	unsigned	char ven_ver_maj;
	unsigned	char ven_ver_min;
	char	id_str[512];
	struct	midi_pnp_logical_dev logical_dev[PNP_MAX_LOGICAL_DEVS];
	int	num_logical_dev;
};

/* exported functions from sup_pnp.c */
void midi_pnp_send_key(void);
int midi_pnp_isol(void);
void midi_pnp_peek(unsigned short bytes, unsigned char *data);
void midi_pnp_wait_for_key(void);
void midi_pnp_activate(unsigned char bool);
void midi_pnp_device(unsigned char dev);
void midi_pnp_wake(unsigned char csn);
unsigned char midi_pnp_ping(unsigned char start_csn,
    unsigned long vendor_id);
unsigned char midi_pnp_ping_no_isol(unsigned char start_csn,
    unsigned long vendor_id);
int midi_pnp_configure(unsigned char csn, unsigned char dev,
    struct midi_pnp_config *conf);
struct midi_pnp_dev_config *midi_pnp_get_config(unsigned char csn);
void midi_pnp_free_config(struct midi_pnp_dev_config *c);
void midi_pnp_print_possible_config(unsigned char csn, unsigned char dev);
void midi_pnp_print_dev_config(unsigned char csn, unsigned char dev);
void midi_pnp_print_cur_config(unsigned char csn, unsigned char dev);
void midi_pnp_dump_cfg_data(unsigned char csn);
#endif

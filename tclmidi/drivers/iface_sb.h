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

#ifndef IFACE_SB_H
#define IFACE_SB_H


#define SB_DSP_RESET		0x06
#define SB_DSP_READ		0x0a
#define SB_DSP_DATA_AVAIL	0x0e
#define SB_DSP_STATUS		0x0c
#define SB_DSP_COMMAND		0x0c

#define SB_VERSION		0xe1
#define SB_GEN_INTR		0xf2
#define SB_UART_MODE		0x35

/*
 * Our version of the midi_iface structure.
 */
struct midi_iface_sb {
	struct	midi_iface iface;
	unsigned	char maj_ver;
	unsigned	char min_ver;
};

#define MIDI_TRIES			2000	/* in 10s of usec */

/*
 * standard mpu401 interface functions - for use by other mpu401 varients.
 */
const char *sb_name(struct midi_iface *mif);
void sb_size(struct midi_iface *mif, void *size);
void sb_gen_intr(struct midi_iface *mif);
int sb_big_reset(struct midi_iface *mif);
int sb_open(struct midi_iface *mif);
void sb_close(struct midi_iface *mif);
int sb_intr(struct midi_iface *mif);
int sb_data_avail(struct midi_iface *mif);
void sb_write(struct midi_iface *mif, struct event *event);
u_char sb_read(struct midi_iface *mif);
void sb_free(struct midi_iface *mif);

#endif

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

#ifndef IFACE_MPU401_H
#define IFACE_MPU401_H


/*
 * midi port offsets
 */
#define MPU401_DATA			0
#define MPU401_STATUS			1
#define MPU401_COMMAND			1

/*
 * midi data transfer status bits
 */
#define MPU401_RDY_RCV			(1 << 6)
#define MPU401_DATA_AVL			(1 << 7)

/*
 * midi command values
 */
#define MPU401_RESET			0xff
#define MPU401_UART			0x3f
#define MPU401_VERSION			0xac
#define MPU401_REVISION			0xad
#define MPU401_TEMPO			0xe0
#define MPU401_CLOCK_RES_120		0xc5
#define MPU401_CLOCK_TO_PC_RATE		0xe7
#define MPU401_ENABLE_CLOCK_TO_PC	0x95
#define MPU401_ENABLE_DATA_TRANS	0x8b
#define MPU401_DISABLE_RT		0x32
#define MPU401_DISABLE_EXTERN		0x90
#define MPU401_ACTIVE_TRACKS		0xec
#define MPU401_DISABLE_COND		0x8e
#define MPU401_CLEAR_PCS		0xb8
#define MPU401_START_PLAYBACK		0x0a
#define MPU401_ENABLE_SYSEX		0x97
#define MPU401_REQ_SEND_MESSAGE		0xd0
#define MPU401_REQ_SEND_SYS_MESSAGE	0xdf
#define MPU401_DISABLE_MT		0x33
#define MPU401_UNACCEPT_CHAN		0x88

#define MPU401_ACK			0xfe
#define MPU401_CLOCK_TO_PC		0xfd

#define MPU401_TRIES			2000	/* in 10s of usec */

/*
 * MPU401 status bits
 */
#define MPU401_NEED_ACK			(1 << 0)
#define MPU401_NEED_DATA		(1 << 1)
#define MPU401_PROCESSING_COMMAND	(1 << 2)

/*
 * Our version of the midi_iface structure.
 */
struct midi_iface_mpu401 {
	struct	midi_iface iface;
	volatile	long status;
};

/*
 * standard mpu401 interface functions - for use by other mpu401 varients.
 */
const char *mpu401_name(struct midi_iface *mif);
void mpu401_size(struct midi_iface *mif, void *size);
void mpu401_gen_intr(struct midi_iface *mif);
int mpu401_big_reset(struct midi_iface *mif);
int mpu401_open(struct midi_iface *mif);
void mpu401_close(struct midi_iface *mif);
int mpu401_intr(struct midi_iface *mif);
int mpu401_data_avail(struct midi_iface *mif);
void mpu401_write(struct midi_iface *mif, struct event *event);
u_char mpu401_read(struct midi_iface *mif);
int mpu401_feature(struct midi_iface *iface, struct midi_feature *feature);
void mpu401_free(struct midi_iface *mif);

int mpu401_wait_rdy_rcv(struct midi_iface_mpu401 *iface);
int mpu401_send_command(struct midi_iface_mpu401 *iface, u_char comm);
int mpu401_get_command_ack(struct midi_iface_mpu401 *iface);
int mpu401_send_command_with_response(struct midi_iface_mpu401 *mif,
    u_char comm, void *response, int resp_size);
int mpu401_send_command_with_args(struct midi_iface_mpu401 *iface, u_char comm,
    void *vargs, int arg_size);
int mpu401_reset(struct midi_iface_mpu401 *iface);
int mpu401_uart(struct midi_iface_mpu401 *iface);
int mpu401_enter_mpu401_timing_mode(struct midi_iface_mpu401 *iface);


#endif

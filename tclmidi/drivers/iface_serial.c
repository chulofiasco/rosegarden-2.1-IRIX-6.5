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

#if defined(MIDI_BUILD_ALL) || defined(MIDI_BUILD_SERIAL)

#include "sup_os.h"
#include "sup_iface.h"
#include "iface_serial.h"


const char *
serial_name(struct midi_iface *mif)
{

	return ("Generic UART");
}

void
serial_size(struct midi_iface *mif, void *size)
{

	*(int *)size = UART_SCR;
	return;
}

void
serial_gen_intr(struct midi_iface *mif)
{

	serial_reset(mif);
	return;
}

int
serial_open(struct midi_iface *mif)
{

	/*
	 * don't do anything.  midiopen already calls fullreset which
	 * puts the board in UART mode.
	 */
	return (0);
}

void
serial_close(struct midi_iface *mif)
{
        struct midi_isa_iface *loc;
	loc = (struct midi_isa_iface *)mif->loc;

	/* Disable all interrupts. */
	OUTB(loc->io_port + UART_IER, 0);

	return;
}

int
serial_intr(struct midi_iface *mif)
{
	struct midi_iface_serial *iface;
	struct midi_isa_iface *loc;
	u_char code;

	iface = (struct midi_iface_serial *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	code = INB(loc->io_port + UART_RX);
	return (code);
}

int
serial_data_avail(struct midi_iface *mif)
{
	struct midi_isa_iface *loc;
	int flag;

	loc = (struct midi_isa_iface *)mif->loc;

	flag = INB(loc->io_port + UART_LSR);
	return (flag & UART_LSR_DR);
}

int
serial_wait_rdy_rcv(struct midi_iface_serial *iface)
{
	struct midi_isa_iface *loc;
	int i;

	loc = (struct midi_isa_iface *)iface->iface.loc;

#define INSANE 1e5

	for (i = 0; i != INSANE &&
	     !(INB(loc->io_port + UART_LSR) & UART_LSR_THRE); i++);

	if (i == INSANE)
		return (0);

	return (1);
}

void
serial_write(struct midi_iface *mif, struct event *event)
{
	struct midi_iface_serial *iface;
	struct midi_isa_iface *loc;
	int i;
	u_char byte;

	iface = (struct midi_iface_serial *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	for (i = 0; i < stynamic_len(&event->data); i++) {
		byte = stynamic_get_byte(&event->data, i);
		serial_wait_rdy_rcv(iface);
		OUTB(loc->io_port + UART_TX, byte);
	}
}

u_char
serial_read(struct midi_iface *mif)
{
	struct midi_iface_serial *iface;
	struct midi_isa_iface *loc;
	u_char byte;

	iface = (struct midi_iface_serial *)mif;
	loc = (struct midi_isa_iface *)mif->loc;

	byte = INB(loc->io_port + UART_RX);
	return (byte);
}

int
serial_feature(struct midi_iface *iface, struct midi_feature *feature)
{

	/* no feeping creaturitis yet */
	return (ENOTTY);
}

void
serial_free(struct midi_iface *mif)
{
	struct midi_iface_serial *iface;

	FREE(mif->loc, sizeof(struct midi_isa_iface));

	iface = (struct midi_iface_serial *)mif;
	FREE(iface, sizeof(struct midi_iface_serial));
}


int
serial_reset(struct midi_iface *mif)
{
	struct midi_isa_iface *loc;

	loc = (struct midi_isa_iface *)mif->loc;

	/* Disable all interrupts. */
	OUTB(loc->io_port + UART_IER, 0);

	/* Disable and flush FIFOs. */
	OUTB(loc->io_port + UART_FCR,
	    UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT);

	/* Clear any eventual remains. */
	(void)INB(loc->io_port + UART_RX);
	(void)INB(loc->io_port + UART_LSR);
	(void)INB(loc->io_port + UART_IIR);
	(void)INB(loc->io_port + UART_MSR);

	/* Enable transmit FIFO. */
	OUTB(loc->io_port + UART_FCR, UART_FCR_ENABLE_FIFO);

	/* Set divisor to 4. */
	OUTB(loc->io_port + UART_LCR, UART_LCR_DLAB);
	OUTB(loc->io_port + UART_DLL, 4);

	/* Word length 8 bits. */
	OUTB(loc->io_port + UART_LCR, UART_LCR_WLEN8);

	/* Interrupt only on RX. */
	OUTB(loc->io_port + UART_IER, UART_IER_RDI);

	return(1);
}


static int
serial_ping(unsigned short port)
{

		return (1);
}


struct midi_iface *
serial_probe(struct midi_softc *softc, MIDI_IFACE_TYPE type, void *vloc)
{
	struct midi_iface_serial *iface;
	struct midi_isa_iface *loc;

	/* The only supported type. */
	if (type != MIDI_ISA_IFACE)
		return (NULL);

	loc = (struct midi_isa_iface *)vloc;

	/* Can't work without an IRQ. */
	if (loc->irq == -1)
		return(NULL);

	if (!serial_ping(loc->io_port))
		return (NULL);

	if (NULL == (iface = MALLOC_NOWAIT(sizeof(struct midi_iface_serial))))
		return(NULL);

	if (NULL == (iface->iface.loc =
	    MALLOC_NOWAIT(sizeof(struct midi_isa_iface)))) {
		FREE(iface, sizeof(struct midi_iface_serial));
		return (NULL);
	}

	/* copy location */
	iface->iface.type = type;
	*(struct midi_isa_iface *)iface->iface.loc = *loc;

	iface->iface.softc = softc;
	iface->iface.name = serial_name;
	iface->iface.size = serial_size;
	iface->iface.gen_intr = serial_gen_intr;
	iface->iface.reset = serial_reset;
	iface->iface.open = serial_open;
	iface->iface.close = serial_close;
	iface->iface.intr = serial_intr;
	iface->iface.data_avail = serial_data_avail;
	iface->iface.write = serial_write;
	iface->iface.read = serial_read;
	iface->iface.feature = serial_feature;
	iface->iface.free = serial_free;

	serial_reset(&(iface->iface));
	return ((struct midi_iface *)iface);
}
#endif

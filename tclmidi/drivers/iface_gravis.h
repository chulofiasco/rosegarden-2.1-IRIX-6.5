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

#ifndef IFACE_GUS_H
#define IFACE_GUS_H

#define GRAVIS_MIDI_CONTROL		0x100
#define GRAVIS_MIDI_STATUS		0x100
#define GRAVIS_MIDI_TRANS		0x101
#define GRAVIS_MIDI_RECV		0x101

/* midi status bits */
#define GRAVIS_MIDI_DATA_AVL		(1 << 0)
#define GRAVIS_MIDI_RDY_RCV		(1 << 1)
#define GRAVIS_MIDI_FRAME_ERR		(1 << 4)
#define GRAVIS_MIDI_OVERRUN		(1 << 5)
#define GRAVIS_MIDI_IRQ_PENDING		(1 << 7)

#define GRAVIS_PAGE			0x102
#define GRAVIS_REGISTER_SELECT		0x103
#define GRAVIS_DATA_LOW			0x104
#define GRAVIS_DATA_HI			0x105
#define GRAVIS_IRQ_STATUS		0x006
#define GRAVIS_TIMER_CONTROL		0x008
#define GRAVIS_TIMER_DATA		0x009
#define GRAVIS_DRAM			0x107
#define GRAVIS_MIX			0x000
#define GRAVIS_IRQ_CONTROL		0x00B
#define GRAVIS_DMA_CONTROL		0x00B
#define GRAVIS_REGISTER_CONTROL		0x00F
#define GRAVIS_VERSION			0x506
#define GRAVIS_MIXER_CONTROL		0x506
#define GRAVIS_MIXER_DATA		0x106

/* MIX bits */
#define MIX_DISABLE_LINE_IN		(1 << 0)
#define MIX_DISABLE_LINE_OUT		(1 << 1)
#define MIX_ENABLE_MIC_IN		(1 << 2)
#define MIX_ENABLE_LATCHES		(1 << 3)
#define MIX_COMBINE_IRQS		(1 << 4)
#define MIX_ENABLE_MIDI_LOOP		(1 << 5)
#define MIX_CONTROL_REG_SELECT		(1 << 6)

/* different registers available via GRAVIS_REGISTER_SELECT */
#define REGISTER_DMA_CONTROL		0x41
#define REGISTER_DMA_START_ADDR		0x42
#define REGISTER_ADDR_LOW		0x43
#define REGISTER_ADDR_HI		0x44
#define REGISTER_TIMER_CONTROL		0x45
#define REGISTER_TIMER1			0x46
#define REGISTER_TIMER2			0x47
#define REGISTER_SAMPLE_FREQ		0x48
#define REGISTER_SAMPLE_CONTROL		0x49
#define REGISTER_JOYSTICK_TRIM		0x4b
#define REGISTER_RESET			0x4c
#define REGISTER_ENHANCED_MODE		0x19

/* An IWave specific register - I don't know what LMCI stands for */
#define REGISTER_LMSBAI			0x51
#define REGISTER_LMCFI			0x52
#define REGISTER_LMCI			0x53
#define REGISTER_PCCCI			0x02

/* Another Iwave specific register SMSI */
#define REGISTER_SMSI			0x15


/* Enhanced mode bits */
#define ENH_MODE			(1 << 0)
#define ENH_ENABLE_LFOS			(1 << 1)
#define ENH_NO_WAVETABLE		(1 << 2)
#define ENH_RAM_TEST			(1 << 3)

/* LMCI bits */
#define LMCI_AUTO_INC			(1 << 0)
#define LMCI_ROM_MASK			0x02
#define LMCI_DRAM_MASK			0x4d


/* Voice specific registers */
#define VOICE_CONTROL			0x00
#define VOICE_FREQUENCY			0x01
#define VOICE_START_HI			0x02
#define VOICE_START_LOW			0x03
#define VOICE_END_HI			0x04
#define VOICE_END_LOW			0x05
#define VOICE_VOL_RAMP_RATE		0x06
#define VOICE_VOL_RAMP_START		0x07
#define VOICE_VOL_RAMP_END		0x08
#define VOICE_CURRENT_VOL		0x09
#define VOICE_CURRENT_HI		0x0a
#define VOICE_CURRENT_LOW		0x0b
#define VOICE_PAN_POS			0x0c
#define VOICE_VOL_CONTROL		0x0d
#define VOICE_ACTIVE			0x0e
#define VOICE_IRQ_STATUS		0x0f

/* A IWave only voice register */
#define VOICE_SUAI			0x10

/* VOICE_CONTROL bits */
#define VOICEC_STOPPED			(1 << 0)
#define VOICEC_STOP			(1 << 1)
#define VOICEC_16			(1 << 2)
#define VOICEC_LOOP			(1 << 3)
#define VOICEC_BIDIR			(1 << 4)
#define VOICEC_WAVE_IRQ			(1 << 5)
#define VOICEC_MOVE_DIR			(1 << 6)
#define VOICEC_IRQ_PENDING		(1 << 7)

/* VOL_CONTROL bits */
#define VOLC_STOPPED			(1 << 0)
#define VOLC_RAMP_STOP			(1 << 1)
#define VOLC_ROLLOVER			(1 << 2)
#define VOLC_LOOP			(1 << 3)
#define VOLC_BIDIR			(1 << 4)
#define VOLC_RAMP_IRQ			(1 << 5)
#define VOLC_DIR			(1 << 6)
#define VOLC_IRQ_PENDING		(1 << 7)


/*
 * IRQ_STATUS bits
 */
#define IRQ_MIDI_TRANS			(1 << 0)
#define IRQ_MIDI_RECV			(1 << 1)
#define IRQ_TIMER1			(1 << 2)
#define IRQ_TIMER2			(1 << 3)
#define IRQ_WAVETABLE			(1 << 5)
#define IRQ_VOLUME			(1 << 6)
#define IRQ_DMA				(1 << 7)

/*
 * Voice IRQ status bits
 */
#define VIRQ_VOICE_MASK			0x1f
#define VIRQ_VOLUME			(1 << 6)
#define VIRQ_WAVETABLE			(1 << 7)


#define SET_DRAM_LOW			0x43
#define SET_DRAM_HIGH			0x44
#define MASTER_RESET			0x01

/*
 * local memory management defines.
 */
#define MEM_NEXT			0
#define MEM_PREV			4
#define MEM_SIZE			8
#define MEM_HEADER_SIZE			12
#define MEM_NO_NEXT			0xffffffff
#define MEM_NO_PREV			0xffffffff
#define MEM_GF1_POOL			(256 * 1024)
#define MEM_EXHAUSTED			0xffffffff
#define MEM_NO_MEM			0xffffffff

/* Max memory size on an Iwave */
#define IWAVE_RAM_MAX			16777216L
#define IWAVE_RAM_STEP			65536L
#define IWAVE_BANK_MAX			4194304L

/*
 * Some macro for manipulating lm addresses
 */
#define GRAVIS_ADDR_HI(x)		(unsigned short)((x) >> 7)
#define GRAVIS_ADDR_LOW(x)		(unsigned short)((x) << 9)

/* a volume thing */
#define GRAVIS_VOLUME			90

/* max number of current active voices */
#define NUM_VOICES			24

#define GRAVIS_PNP_VENDOR_ID		0x0100561e /* intel order */

/* virtual PnP devices */
#define IWAVE_AUDIO			0
#define IWAVE_EXT			1
#define IWAVE_GAME			2
#define IWAVE_EMULATION			3
#define IWAVE_MPU401			4

struct gravis_lm_wave {
	struct	gravis_lm_patch *patch;
	u_long	len;
	u_long	begin;
	u_long	loop_start;
	u_long	loop_end;
	u_long	end;
	u_long	low_freq;
	u_long	high_freq;
	u_long	root_freq;
	u_short	sr;
	u_short	volume;
	int	data_8_bit;
	int	data_unsigned;
	int	looping;
};

struct gravis_lm_patch {
	struct	gravis_lm_wave *waves;
	int	num_waves;
	int	program;
	int	drum;
};

struct gravis_voice {
	struct	gravis_lm_wave *w;
	u_long	start_time;	/* Time when voice was started */
	long	status;		/* is this voice being used */
	int	voice_num;
	int	pitch;
};

/*
 * voice status bits
 */
#define V_IN_USE		(1 << 0)
#define V_STOPPING		(1 << 1)


/*
 * Our version of the midi_iface structure.
 */
struct midi_iface_gravis {
	struct	midi_iface iface;
	long	status;
	int	internal;	/* are we using internal voices, or external? */
	int	iwave;		/* iwave (PnP) or GUS (GF1)? */
	SLEEPCHAN	sleep_dma;	/* channel for sleeping on DMA */
	u_long	mem_size;	/* KB of RAM on board */
	u_long	reserved_mem;
	u_long	free_mem;	/* pointer to free list */
	u_long	cur_out_time;
	struct	gravis_voice voices[NUM_VOICES];
	struct	gravis_lm_patch *normal_programs[128];
	struct	gravis_lm_patch *drum_programs[128];
	struct	gravis_lm_patch *channel[16];
	char	name[80];
};

/*
 * status bits
 */
#define	GRAVIS_STATUS_SLEEP_DMA		(1 << 0)


#define GRAVIS_RECV_TRIES		2000	/* in 10s of usec */

#endif

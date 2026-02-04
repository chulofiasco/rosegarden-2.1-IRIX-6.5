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
#ifndef MIDIIOCTL_H
#define MIDIIOCTL_H
/*
 * SMPTE extensions
 */
struct SMPTE_frame {
	unsigned	char rate;
	unsigned	char hour;
	unsigned	char minute;
	unsigned	char second;
	unsigned	char frame;
	unsigned	char fraction;
	unsigned	char status;
}; 


/*
 * SMPTE status bits
 */
#define SMPTE_SYNC	(1 << 0)	/* indicates we are sync'd */
#define SMPTE_FRAMEDROP	(1 << 1)	/* this is a frame-drop format */

struct gravis_wave {
	long	len;
	long	loop_start;
	long	loop_end;
	unsigned	short volume;
	long	low_freq;
	long	high_freq;
	long	root_freq;
	unsigned	short sr;
	int	data_8_bit;
	int	data_unsigned;
	int	looping;
	unsigned	char *data;
};

struct gravis_patch {
	struct	gravis_wave *waves;
	int	num_waves;
	int	program;
	int	drum;
};



/*
 * General purpose structure for accessing the extended board specific
 * features.
 */
typedef enum {
	/* some MQX32 features */
	MFEAT_KERNEL_TIMING,	/* use normal kernel timing */
	MFEAT_SMPTE_TIMING,	/* use SMPTE timing */
	MFEAT_GET_SMPTE,	/* get current smpte time */
	MFEAT_MPU401_TIMING,	/* use the MPU401 as an external timer */
	MFEAT_FLUSH_PATCHES,	/* flush loaded patch in sound cards */
	MFEAT_LOAD_PATCH,	/* download a patch to a sound card */
	MFEAT_DELETE_PATCH	/* deletes a patch from driver */
} MIDI_FEATURE_TYPE;

struct midi_feature {
	MIDI_FEATURE_TYPE	type;	/* feature identifier */
	void			*data;	/* data for feature, will be
					 * typecast by feature type
					 */
};


/*
 * MIDI ioctls /dev/midi
 */
#define MRESET		_IO('m', 0x01)
#define MDRAIN		_IO('m', 0x02)
#define MFLUSH		_IO('m', 0x03)
#define MGPLAYQ		_IOR('m', 0x04, int)
#define MGRECQ		_IOR('m', 0x05, int)
#define MSDIVISION	_IOW('m', 0x06, int)
#define MGDIVISION	_IOR('m', 0x07, int)
#define MGQAVAIL	_IOR('m', 0x08, int)
#define MASYNC		_IOW('m', 0x09, int)
#define MTHRU		_IOW('m', 0x0a, int)
#define MRECONPLAY	_IOW('m', 0x0b, int)
#define MGSMFTIME	_IOR('m', 0x0d, unsigned long)
#define MGTHRU		_IOR('m', 0x0e, int)
#define MFEATURE	_IOWR('m', 0x0f, struct midi_feature)
#define MGETID		_IOR('m', 0x10, int)
#define MSLAVE		_IOW('m', 0x11, int)
#define MGRAW		_IOR('m', 0x12, int)
#define MGCMASK		_IOR('m', 0x13, unsigned short)
#define MSCMASK		_IOW('m', 0x14, unsigned short)
#endif

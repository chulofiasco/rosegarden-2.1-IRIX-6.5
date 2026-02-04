/*-
 * Copyright (c) 1993, 1994, 1995, 1996 Michael B. Durian.  All rights reserved.
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
#ifndef MIDIDEVICE_H
#define MIDIDEVICE_H

/* Microsoft compiler can't deal with the real name - are we surprised? */
#ifndef _MSC_VER
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#include "Song.h"

class MidiDevice {
public:
	MidiDevice();
	MidiDevice(const MidiDevice &md);
	virtual ~MidiDevice();

	virtual MidiDevice *Dup(void) = 0;
	virtual int Open(const char *dev) = 0;
	virtual int Close(void) = 0;
	virtual int Play(Song *s, int r = 0) = 0;
	virtual int Record(Song *rec_song, Song *play_song = 0,
	    int r = 0) = 0;
	virtual int Stop(void) = 0;
	virtual int Wait(void) = 0;
	virtual int SetMidiThru(int mt) = 0;
	virtual int GetMidiThru(void) = 0;
	virtual ostrstream *Feature(const char *command, const char *argv[],
	    int argc) = 0;
	virtual int Slave(const MidiDevice &md) = 0;
	virtual int GetTime(unsigned long *t) = 0;
	virtual int Send(Event **events, int num) = 0;
	virtual int Recv(Event ***events, int *num) = 0;
	virtual int SetChannelMask(int mask) = 0;
	virtual int GetChannelMask(void) = 0;

	void SetName(const char *dev);
	const char *GetName(void) const {return (name);}

	void SetRepeat(int r) {repeat = r;}
	int GetRepeat(void) const {return (repeat);}

	const char *GetError(void) const {return (error);}
	void SetError(const char *err);

private:
	char *name;
	char *error;
	int repeat;
};
#endif

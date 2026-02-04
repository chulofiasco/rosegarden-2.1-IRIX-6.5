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
#ifndef TCLM_DR75_H
#define TCLM_DR75_H

extern "C" {
#include <tcl.h>
}
#include "TclmIntp.h"
#include "MidiDev.h"

class TclmDriver75 : public MidiDevice {
	friend ostream &operator<<(ostream &os, const TclmDriver75 &dr);
	friend void TclmDriver75Event(ClientData cd, int mask);
public:
	TclmDriver75();
	TclmDriver75(const TclmDriver75 &td);
	virtual ~TclmDriver75();

	virtual MidiDevice *Dup(void) {return (new TclmDriver75(*this));}
	virtual int Open(const char *dev);
	virtual int Close(void);
	virtual int Play(Song *s, int r = 0);
	virtual int Record(Song *rec_song, Song *play_song = 0, int r = 0);
	virtual int Stop(void);
	virtual int Wait(void);
	virtual int SetMidiThru(int mt);
	virtual int GetMidiThru(void);
	virtual ostrstream *Feature(const char *command, const char *argv[],
	    int argc);
	virtual int Slave(const MidiDevice &md);
	virtual int GetTime(unsigned long *t);
	virtual int Send(Event **events, int num);
	virtual int Recv(Event ***events, int *num);
	virtual int SetChannelMask(int on);
	virtual int GetChannelMask(void);

	// so we have a place to report errors
	void SetInterp(Tcl_Interp *i) {interp = i;}
	void SetTclmInterp(TclmInterp *ti) {tclm_interp = ti;}

private:
	int WriteEvents(void);
	int ReadEvents(void);
	int TimeChanged(void);
	int LoadPatch(int argc, const char *argv[]);
	int DeletePatch(int argc, const char *argv[]);

	unsigned long last_play_time, last_rec_time;
	Event **curr_event;
	Song *play_song;
	Song *rec_song;
	int fd;
	Tcl_File tcl_file;
	int finished;
	unsigned char last_record_rs;

	Tcl_Interp *interp;
	TclmInterp *tclm_interp;
};
#endif

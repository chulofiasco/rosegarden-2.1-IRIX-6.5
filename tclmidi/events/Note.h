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
#ifndef NOTEEVENT_H
#define NOTEEVENT_H

#include "NormEvnt.h"

class NoteEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const NoteEvent &e);
public:
	NoteEvent();
	NoteEvent(unsigned long t, int chan, int pit, int vel,
	    const NoteEvent *np = 0);
	NoteEvent(const NoteEvent &e);
	virtual Event *Dup(void) const {return (new NoteEvent(*this));}

	virtual EventType GetType(void) const {return (NOTE);}
	virtual char *GetTypeStr(void) const {return ("NoteEvent");}
	virtual char *GetEventStr(void) const;
	int GetPitch(void) const {
		if (GetWildcard(wc_pitch))
			return (WC_PITCH);
		else
			return (pitch);
	}
	int GetVelocity(void) const {
		if (GetWildcard(wc_velocity))
			return (WC_VELOCITY);
		else
			return (velocity);
	}
	NoteEvent *GetNotePair(void) const {
		return (note_pair);
	}

	void SetPitch(int pit) {
		if (pit == WC_PITCH)
			SetWildcard(wc_pitch);
		else {
			pitch = pit;
			ClearWildcard(wc_pitch);
		}
	}
	void SetVelocity(int vel) {
		if (vel == WC_VELOCITY)
			SetWildcard(wc_velocity);
		else {
			velocity = vel;
			ClearWildcard(wc_velocity);
		}
	}
	virtual void SetNotePair(NoteEvent *np) {
		note_pair = np;
	}

	NoteEvent &operator=(const NoteEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PITCH;
	static const int WC_VELOCITY;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_pitch;
	static const unsigned long wc_velocity;
	unsigned char pitch;
	unsigned char velocity;
	NoteEvent *note_pair;
};
#endif

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
#ifndef NOTEONEVENT_H
#define NOTEONEVENT_H

#include "Note.h"

class NoteOnEvent : public NoteEvent {
	friend ostream &operator<<(ostream &os, const NoteOnEvent &e);
public:
	NoteOnEvent();
	NoteOnEvent(unsigned long t, int chan, int pit, int velocity,
	    const NoteEvent *np = 0);
	NoteOnEvent(unsigned long t, int chan, int pit, int velocity,
	    unsigned long duration, const NoteEvent *np = 0);
	NoteOnEvent(const NoteOnEvent &e);
	virtual Event *Dup(void) const {return (new NoteOnEvent(*this));}

	virtual EventType GetType(void) const {return (NOTEON);}
	virtual char *GetTypeStr(void) const {return ("NoteOnEvent");}
	virtual char *GetEventStr(void) const;

	unsigned long GetDuration(void) const {
		if (GetWildcard(wc_duration))
			return (WC_DURATION);
		else
			return (duration);
	}

	void SetDuration(unsigned long dur) {
		if (dur == WC_DURATION)
			SetWildcard(wc_duration);
		else {
			duration = dur;
			ClearWildcard(wc_duration);
		}
	}

	virtual void SetNotePair(NoteEvent *np) {
		NoteEvent::SetNotePair(np);
		if (GetTime() != WC_TIME && np->GetTime() != WC_TIME) {
			duration = np->GetTime() - GetTime();
			ClearWildcard(wc_duration);
		}
	}

	NoteOnEvent &operator=(const NoteOnEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (NoteEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (NoteEvent::SMFWrite(t));}

	static const unsigned long WC_DURATION;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_duration;
	unsigned long duration;
};
#endif

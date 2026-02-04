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
#ifndef NOTEOFFEVENT_H
#define NOTEOFFEVENT_H

#include "Note.h"

class NoteOffEvent : public NoteEvent {
	friend ostream &operator<<(ostream &os, const NoteOffEvent &e);
public:
	NoteOffEvent();
	NoteOffEvent(unsigned long t, int chan, int pit, int velocity = 0,
	    const NoteEvent *np = 0);
	NoteOffEvent(const NoteOffEvent &e);
	virtual Event *Dup(void) const {return (new NoteOffEvent(*this));}

	virtual EventType GetType(void) const {return (NOTEOFF);}
	virtual char *GetTypeStr(void) const {return ("NoteOffEvent");}
	virtual char *GetEventStr(void) const;

	NoteOffEvent &operator=(const NoteOffEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (NoteEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (NoteEvent::SMFWrite(t));}
protected:
	virtual int Equal(const Event *e) const {return (NoteEvent::Equal(e));}
};
#endif

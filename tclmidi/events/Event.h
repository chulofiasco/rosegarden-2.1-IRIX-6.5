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
#ifndef EVENT_H
#define EVENT_H

#include <iostream.h>
/* Microsoft compiler can't deal with the real name - are we surprised? */
#ifndef _MSC_VER
#include <strstream.h>
#else
#include <strstrea.h>
#endif
#include <iomanip.h>
#include <stdlib.h>
#include "SMFTrack.h"
#include "EvTrDefs.h"

typedef enum {NOTYPE, NORMAL, META, NOTE, NOTEOFF, NOTEON, KEYPRESSURE,
    PARAMETER, PROGRAM, CHANNELPRESSURE, PITCHWHEEL, SYSTEMEXCLUSIVE,
    METASEQUENCENUMBER, METATEXT, METACOPYRIGHT, METASEQUENCENAME,
    METAINSTRUMENTNAME, METALYRIC, METAMARKER, METACUE, METACHANNELPREFIX,
    METAPORTNUMBER, METAENDOFTRACK, METATEMPO, METASMPTE, METATIME, METAKEY,
    METASEQUENCERSPECIFIC, METAUNKNOWN} EventType;

class Event {
	friend ostream &operator<<(ostream &os, const Event &e);
public:
	Event();
	Event(unsigned long t);
	Event(const Event &e);
	virtual Event *Dup(void) const {return new Event(*this);}

	int IsWildcard(void) const {
		return (wildcard != 0);
	}

	static const unsigned long WC_TIME;
protected:
	virtual int Equal(const Event *e) const;
	void SetWildcard(unsigned long bit) {
			wildcard |= bit;
	}
	void ClearWildcard(unsigned long bit) {
			wildcard &= ~bit;
	}
	int GetWildcard(int bit) const {
		return (wildcard & bit);
	}
public:
	unsigned long GetTime(void) const {
		if (GetWildcard(wc_time))
			return (WC_TIME);
		else
			return (time);
	}
	virtual EventType GetType(void) const {return (NOTYPE);}
	virtual char *GetTypeStr(void) const {return ("NoType");}
	virtual char *GetEventStr(void) const;
	Event *GetNextEvent(void) const {return (next_event);}
	Event *GetPrevEvent(void) const {return (prev_event);}
	const EventTreeNode *GetEventTreeNode(void) const {return (node);}

	void SetTime(unsigned long t) {
		if (t == WC_TIME)
			SetWildcard(wc_time);
		else
			time = t;
	}
	void SetNextEvent(Event *n) {next_event = n;}
	void SetPrevEvent(Event *p) {prev_event = p;}
	void SetEventTreeNode(const EventTreeNode *n) {node = n;}

	Event &operator=(const Event &e);
	int operator==(const Event &e) const;
	void Print(ostream &os) const;

	virtual const char *SMFRead(SMFTrack &t) {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}
	virtual const char *SMFWrite(SMFTrack &t) const {
		// shut up a warning
		SMFTrack *dummy;
		dummy = &t;
		return (0);
	}
private:
	unsigned long time;
	unsigned long wildcard;
	static const unsigned long wc_time;
	Event *next_event;
	Event *prev_event;
	const EventTreeNode *node;
};
#endif

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
#ifndef NORMALEVENT_H
#define NORMALEVENT_H

#include "Event.h"

class NormalEvent : public Event {
	friend ostream &operator<<(ostream &os, const NormalEvent &e);
public:
	NormalEvent();
	NormalEvent(unsigned long t, int chan);
	NormalEvent(const NormalEvent &e);
	virtual Event *Dup(void) const {return (new NormalEvent(*this));}

	virtual EventType GetType(void) const {return (NORMAL);}
	virtual char *GetTypeStr(void) const {return ("NormalEvent");}
	virtual char *GetEventStr(void) const;
	int GetChannel(void) const {
		if (GetWildcard(wc_channel))
			return (WC_CHANNEL);
		else
			return (channel);
	}

	void SetChannel(int chan) {
		if (chan == WC_CHANNEL)
			SetWildcard(wc_channel);
		else {
			channel = chan;
			ClearWildcard(wc_channel);
		}
	}

	NormalEvent &operator=(const NormalEvent &e);

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

	static const int WC_CHANNEL;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_channel;
	unsigned char channel;
};
#endif

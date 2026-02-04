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
#ifndef CHANPRESEVENT_H
#define CHANPRESEVENT_H

#include "NormEvnt.h"


class ChannelPressureEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const ChannelPressureEvent &e);
public:
	ChannelPressureEvent();
	ChannelPressureEvent(unsigned long t, int chan, int pres);
	ChannelPressureEvent(const ChannelPressureEvent &e);
	virtual Event *Dup(void) const
	    {return (new ChannelPressureEvent(*this));}

	virtual EventType GetType(void) const {return (CHANNELPRESSURE);}
	virtual char *GetTypeStr(void) const {return ("ChannelPressureEvent");}
	virtual char *GetEventStr(void) const;
	int GetPressure(void) const {
		if (GetWildcard(wc_pressure))
			return (WC_PRESSURE);
		else
			return (pressure);
	}

	void SetPressure(int pres) {
		if (pres == WC_PRESSURE)
			SetWildcard(wc_pressure);
		else {
			pressure = pres;
			ClearWildcard(wc_pressure);
		}
	}

	ChannelPressureEvent &operator=(const ChannelPressureEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PRESSURE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_pressure;
	unsigned char pressure;
};
#endif

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
#ifndef PITCHWHEEL_H
#define PITCHWHEEL_H

#include "NormEvnt.h"

class PitchWheelEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const PitchWheelEvent &e);
public:
	PitchWheelEvent();
	PitchWheelEvent(unsigned long t, int chan, long val);
	PitchWheelEvent(const PitchWheelEvent &e);
	virtual Event *Dup(void) const {return (new PitchWheelEvent(*this));}

	virtual EventType GetType(void) const {return (PITCHWHEEL);}
	virtual char *GetTypeStr(void) const {return ("PitchWheelEvent");}
	virtual char *GetEventStr(void) const;
	long GetValue(void) const {
		if (GetWildcard(wc_value))
			return (WC_VALUE);
		else
			return (value);
	}

	void SetValue(long val) {
		if (val == WC_VALUE)
			SetWildcard(wc_value);
		else {
			value = val;
			ClearWildcard(wc_value);
		}
	}

	PitchWheelEvent &operator=(const PitchWheelEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const long WC_VALUE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_value;
	short value;
};
#endif

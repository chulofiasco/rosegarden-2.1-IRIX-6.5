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
#ifndef PARAMEVENT_H
#define PARAMEVENT_H

#include "NormEvnt.h"

class ParameterEvent : public NormalEvent {
	friend ostream &operator<<(ostream &os, const ParameterEvent &e);
public:
	ParameterEvent();
	ParameterEvent(unsigned long t, int chan, int param, int val);
	ParameterEvent(const ParameterEvent &e);
	virtual Event *Dup(void) const {return (new ParameterEvent(*this));}

	virtual EventType GetType(void) const {return (PARAMETER);}
	virtual char *GetTypeStr(void) const {return ("ParameterEvent");}
	virtual char *GetEventStr(void) const;
	int GetParameter(void) const {
		if (GetWildcard(wc_parameter))
			return (WC_PARAMETER);
		else
			return (parameter);
	}
	int GetValue(void) const {
		if (GetWildcard(wc_value))
			return (WC_VALUE);
		else
			return (value);
	}

	void SetParameter(int param) {
		if (param == WC_PARAMETER)
			SetWildcard(wc_parameter);
		else {
			parameter = param;
			ClearWildcard(wc_parameter);
		}
	}
	void SetValue(int val) {
		if (val == WC_VALUE)
			SetWildcard(wc_value);
		else {
			value = val;
			ClearWildcard(wc_value);
		}
	}

	ParameterEvent &operator=(const ParameterEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PARAMETER;
	static const int WC_VALUE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_parameter;
	static const unsigned long wc_value;
	unsigned char parameter;
	unsigned char value;
};
#endif

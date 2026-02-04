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
#ifndef METASMPTE_H
#define METASMPTE_H

#include "MEvent.h"

class MetaSMPTEEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaSMPTEEvent &e);
public:
	MetaSMPTEEvent();
	MetaSMPTEEvent(unsigned long t, int h, int m, int s, int f, int ff);
	MetaSMPTEEvent(const MetaSMPTEEvent &e);
	virtual Event *Dup(void) const {return (new MetaSMPTEEvent(*this));}

	virtual EventType GetType(void) const {return (METASMPTE);}
	virtual char *GetTypeStr(void) const {return ("MetaSMPTEEvent");}
	virtual char *GetEventStr(void) const;
	int GetHour(void) const {
		if (GetWildcard(wc_hour))
			return (WC_HOUR);
		else
			return (hour);
	}
	int GetMinute(void) const {
		if (GetWildcard(wc_minute))
			return (WC_MINUTE);
		else
			return (minute);
	}
	int GetSecond(void) const {
		if (GetWildcard(wc_second))
			return (WC_SECOND);
		else
			return (second);
	}
	int GetFrame(void) const {
		if (GetWildcard(wc_frame))
			return (WC_FRAME);
		else
			return (frame);
	}
	int GetFractionalFrame(void) const {
		if (GetWildcard(wc_fractional_frame))
			return (WC_FRACTIONAL_FRAME);
		else
			return (fractional_frame);
	}

	void SetHour(int h) {
		if (h == WC_HOUR)
			SetWildcard(wc_hour);
		else {
			hour = h;
			ClearWildcard(wc_hour);
		}
	}
	void SetMinute(int m) {
		if (m == WC_MINUTE)
			SetWildcard(wc_minute);
		else {
			minute = m;
			ClearWildcard(wc_minute);
		}
	}
	void SetSecond(int s) {
		if (s == WC_SECOND)
			SetWildcard(wc_second);
		else {
			second = s;
			ClearWildcard(wc_second);
		}
	}
	void SetFrame(int f) {
		if (f == WC_FRAME)
			SetWildcard(wc_frame);
		else {
			frame = f;
			ClearWildcard(wc_frame);
		}
	}
	void SetFractionalFrame(int ff) {
		if (ff == WC_FRACTIONAL_FRAME)
			SetWildcard(wc_fractional_frame);
		else {
			fractional_frame = ff;
			ClearWildcard(wc_fractional_frame);
		}
	}

	MetaSMPTEEvent &operator=(const MetaSMPTEEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_HOUR;
	static const int WC_MINUTE;
	static const int WC_SECOND;
	static const int WC_FRAME;
	static const int WC_FRACTIONAL_FRAME;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_hour;
	static const unsigned long wc_minute;
	static const unsigned long wc_second;
	static const unsigned long wc_frame;
	static const unsigned long wc_fractional_frame;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char frame;
	unsigned char fractional_frame;
};
#endif

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
#ifndef METATIME_H
#define METATIME_H

#include "MEvent.h"

class MetaTimeEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaTimeEvent &e);
public:
	MetaTimeEvent();
	MetaTimeEvent(unsigned long time, int n = 4, int d = 4, int c = 24,
	    int t = 8);
	MetaTimeEvent(const MetaTimeEvent &e);
	virtual Event *Dup(void) const {return (new MetaTimeEvent(*this));}

	virtual EventType GetType(void) const {return (METATIME);}
	virtual char *GetTypeStr(void) const {return ("MetaTimeEvent");}
	virtual char *GetEventStr(void) const;
	int GetNumerator(void) const {
		if (GetWildcard(wc_numerator))
			return (WC_NUMERATOR);
		else
			return (numerator);
	}
	int GetDenominator(void) const {
		if (GetWildcard(wc_denominator))
			return (WC_DENOMINATOR);
		else
			return (denominator);
	}
	int GetClocksPerBeat(void) const {
		if (GetWildcard(wc_clocks_per_beat))
			return (WC_CLOCKS_PER_BEAT);
		else
			return (clocks);
	}
	int Get32ndNotesPerQuarterNote(void) const {
		if (GetWildcard(wc_32nd_notes_per_quarter_note))
			return (WC_32ND_NOTES_PER_QUARTER_NOTE);
		else
			return (thirty_seconds);
	}

	void SetNumerator(int n) {
		if (n == WC_NUMERATOR)
			SetWildcard(wc_numerator);
		else {
			numerator = n;
			ClearWildcard(wc_numerator);
		}
	}
	void SetDenominator(int d) {
		if (d == WC_DENOMINATOR)
			SetWildcard(wc_denominator);
		else {
			denominator = d;
			ClearWildcard(wc_denominator);
		}
	}
	void SetClocksPerBeat(int c) {
		if (c == WC_CLOCKS_PER_BEAT)
			SetWildcard(wc_clocks_per_beat);
		else {
			clocks = c;
			ClearWildcard(wc_clocks_per_beat);
		}
	}
	void Set32ndNotesPerQuarterNote(int t) {
		if (t == WC_32ND_NOTES_PER_QUARTER_NOTE)
			SetWildcard(wc_32nd_notes_per_quarter_note);
		else {
			thirty_seconds = t;
			ClearWildcard(wc_32nd_notes_per_quarter_note);
		}
	}

	MetaTimeEvent &operator=(const MetaTimeEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_NUMERATOR;
	static const int WC_DENOMINATOR;
	static const int WC_CLOCKS_PER_BEAT;
	static const int WC_32ND_NOTES_PER_QUARTER_NOTE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_numerator;
	static const unsigned long wc_denominator;
	static const unsigned long wc_clocks_per_beat;
	static const unsigned long wc_32nd_notes_per_quarter_note;
	unsigned char numerator;
	unsigned char denominator;
	unsigned char clocks;
	unsigned char thirty_seconds;
};
#endif

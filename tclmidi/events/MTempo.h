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
#ifndef METATEMPO_H
#define METATEMPO_H

#include "MEvent.h"

class MetaTempoEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaTempoEvent &e);
public:
	MetaTempoEvent();
	MetaTempoEvent(unsigned long t, short temp = 120);
	MetaTempoEvent(const MetaTempoEvent &e);
	virtual Event *Dup(void) const {return (new MetaTempoEvent(*this));}

	virtual EventType GetType(void) const {return (METATEMPO);}
	virtual char *GetTypeStr(void) const {return ("MetaTempoEvent");}
	virtual char *GetEventStr(void) const;
	short GetTempo(void) const {
		if (GetWildcard(wc_tempo))
			return (WC_TEMPO);
		else
			return (tempo);
	}

	void SetTempo(short temp) {
		if (temp == WC_TEMPO)
			SetWildcard(wc_tempo);
		else {
			tempo = temp;
			ClearWildcard(wc_tempo);
		}
	}

	MetaTempoEvent &operator=(const MetaTempoEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const short WC_TEMPO;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_tempo;
	short tempo;
};
#endif

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
#ifndef METAPORTNUM_H
#define METAPORTNUM_H

#include "MEvent.h"

class MetaPortNumberEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaPortNumberEvent &e);
public:
	MetaPortNumberEvent();
	MetaPortNumberEvent(unsigned long t, int p);
	MetaPortNumberEvent(const MetaPortNumberEvent &e);
	virtual ~MetaPortNumberEvent();
	virtual Event *Dup(void) const
	    {return (new MetaPortNumberEvent(*this));}

	virtual EventType GetType(void) const {return (METAPORTNUMBER);}
	virtual char *GetTypeStr(void) const
	    {return ("MetaPortNumberEvent");}
	virtual char *GetEventStr(void) const;
	int GetPort(void) const {
		if (GetWildcard(wc_port))
			return (WC_PORT);
		else
			return (port);
	}

	void SetPort(int p) {
		if (p == WC_PORT)
			SetWildcard(wc_port);
		else {
			port = p;
			ClearWildcard(wc_port);
		}
	}

	MetaPortNumberEvent &operator=(const MetaPortNumberEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const int WC_PORT;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_port;
	unsigned char port;
};
#endif

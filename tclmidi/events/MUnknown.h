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
#ifndef METAUNKNOWN_H
#define METAUNKNOWN_H

#include "MEvent.h"

class MetaUnknownEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaUnknownEvent &e);
public:
	MetaUnknownEvent();
	MetaUnknownEvent(int ty);
	MetaUnknownEvent(unsigned long t, const unsigned char *data,
	    long length, int ty);
	MetaUnknownEvent(const MetaUnknownEvent &e);
	virtual ~MetaUnknownEvent();
	virtual Event *Dup(void) const {return (new MetaUnknownEvent(*this));}

	virtual EventType GetType(void) const {return (METAUNKNOWN);}
	virtual char *GetTypeStr(void) const {return ("MetaUnknownEvent");}
	virtual char *GetEventStr(void) const;
	const unsigned char *GetData(void) const {
		if (GetWildcard(wc_data))
			return (WC_DATA);
		else
			return (data);
	}
	long GetLength(void) const {
		if (GetWildcard(wc_data))
			return (WC_LENGTH);
		else
			return (length);
	}
	int GetMetaType(void) const {
		if (GetWildcard(wc_meta_type))
			return (WC_META_TYPE);
		else
			return (type);
	}

	void SetData(const unsigned char *data, long length);
	void SetMetaType(unsigned char t) {
		if (t == WC_META_TYPE)
			SetWildcard(wc_meta_type);
		else {
			type = t;
			ClearWildcard(wc_meta_type);
		}
	}

	MetaUnknownEvent &operator=(const MetaUnknownEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const unsigned char *WC_DATA;
	static const long WC_LENGTH;
	static const int WC_META_TYPE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_data;
	static const unsigned long wc_meta_type;
	long length;
	unsigned char *data;
	unsigned char type;
};
#endif

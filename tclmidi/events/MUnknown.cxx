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
#include <assert.h>
#include <string.h>

#include "MUnknown.h"

const unsigned char *MetaUnknownEvent::WC_DATA = 0;
const long MetaUnknownEvent::WC_LENGTH = 0;
const int MetaUnknownEvent::WC_META_TYPE = -1;

const unsigned long MetaUnknownEvent::wc_data = (1 << 1);
const unsigned long MetaUnknownEvent::wc_meta_type = (1 << 2);

MetaUnknownEvent::MetaUnknownEvent() : length(0L), data(0), type(0x60)
{
}

MetaUnknownEvent::MetaUnknownEvent(int ty) : length(0L), data(0),
    type(ty)
{

	if (ty == WC_META_TYPE)
		SetWildcard(wc_meta_type);
}

MetaUnknownEvent::MetaUnknownEvent(unsigned long t, const unsigned char *dat,
    long len, int ty) : MetaEvent(t), length(len), type(ty)
{

	if (ty == WC_META_TYPE)
		SetWildcard(wc_meta_type);
	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
		return;
	}
	data = new unsigned char[len];
	assert(data != 0);
	memcpy(data, dat, len);
}

MetaUnknownEvent::MetaUnknownEvent(const MetaUnknownEvent &e) : MetaEvent(e),
    length(e.length), type(e.type)
{

	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
		return;
	}
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
}

MetaUnknownEvent::~MetaUnknownEvent()
{

	delete data;
}

void
MetaUnknownEvent::SetData(const unsigned char *dat, long len)
{

	delete data;
	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
		return;
	}
	data = new unsigned char[len];
	assert(data != 0);
	memcpy(data, dat, len);
	ClearWildcard(wc_data);
}

MetaUnknownEvent &
MetaUnknownEvent::operator=(const MetaUnknownEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	delete data;
	type = e.type;
	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
		return (*this);
	}
	length = e.length;
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
	return (*this);
}

char *
MetaUnknownEvent::GetEventStr(void) const
{
	ostrstream buf;
	long i;
	char *tbuf;

	tbuf =  MetaEvent::GetEventStr();
	buf.setf(ios::showbase | ios::internal);
	buf << tbuf << " Type: ";
	if (GetWildcard(wc_meta_type))
		buf << "*";
	else
		buf << hex << setw(4) << setfill('0') << (int)type;
	buf << " Data:";
	if (GetWildcard(wc_data))
		buf << " *";
	else {
		for (i = 0; i < length; i++)
			buf << " " << hex << setw(4) << setfill('0') <<
			    (int)data[i];
	}
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
MetaUnknownEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (data != 0)
		delete data;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete MetaUnknownEvent - bad length");
	data = new unsigned char[length];
	if (data == 0)
		return ("Out of memory");
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete MetaUnknownEvent");
	memcpy(data, ptr, length);
	return (0);
}

const char *
MetaUnknownEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(length))
		return ("Out of memory");
	if (!t.PutData(data, length))
		return ("Out of memory");
	return (0);
}

int
MetaUnknownEvent::Equal(const Event *e) const
{
	long i;
	MetaUnknownEvent *eptr = (MetaUnknownEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_meta_type) && !GetWildcard(wc_meta_type) &&
	    type != eptr->type)
		return (0);
	if (eptr->GetWildcard(wc_data) || GetWildcard(wc_data))
		return (1);
	if (length != eptr->length)
		return (0);
	for (i = 0; i < length; i++)
		if (data[i] != eptr->data[i])
			return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaUnknownEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

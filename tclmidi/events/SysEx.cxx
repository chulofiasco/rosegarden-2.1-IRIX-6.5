/*-
 * Copyright (c) 1993, 199, 1995, 1996 Michael B. Durian.  All rights reserved.
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

#include "SysEx.h"

const unsigned char *SystemExclusiveEvent::WC_DATA = 0;
const long SystemExclusiveEvent::WC_LENGTH = -1;

const unsigned long SystemExclusiveEvent::wc_data = (1 << 1);

SystemExclusiveEvent::SystemExclusiveEvent() : length(0L), continued(0),
    data(0)
{
}

SystemExclusiveEvent::SystemExclusiveEvent(unsigned char c) : length(0L),
    continued(c), data(0)
{
}

SystemExclusiveEvent::SystemExclusiveEvent(unsigned long t,
    const unsigned char *dat, long len) : Event(t), length(len),
    continued(0)
{

	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
	} else {
		data = new unsigned char[len];
		assert(data != 0);
		memcpy(data, dat, len);
	}
}

SystemExclusiveEvent::SystemExclusiveEvent(const SystemExclusiveEvent &e) :
    Event(e), length(e.length), continued(e.continued)
{

	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
	} else {
		data = new unsigned char[e.length];
		assert(data != 0);
		memcpy(data, e.data, e.length);
	}
}

SystemExclusiveEvent::~SystemExclusiveEvent()
{

	delete data;
}

void
SystemExclusiveEvent::SetData(const unsigned char *dat, long len)
{

	if (data != 0)
		delete data;
	if (len == WC_LENGTH || dat == WC_DATA) {
		SetWildcard(wc_data);
		data = 0;
		return;
	}
	data = new unsigned char[len];
	assert(data != 0);
	memcpy(data, dat, len);
	ClearWildcard(wc_data);
}

SystemExclusiveEvent &
SystemExclusiveEvent::operator=(const SystemExclusiveEvent &e)
{

	(Event)*this = (Event)e;
	if (data != 0)
		delete data;
	continued = e.continued;
	length = e.length;
	if (e.GetWildcard(wc_data)) {
		data = 0;
		return (*this);
	}
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
	return (*this);
}

char *
SystemExclusiveEvent::GetEventStr(void) const
{
	ostrstream buf;
	long i;
	char *tbuf;

	tbuf = Event::GetEventStr();
	buf << tbuf << " Continued: " << (int)continued << " Data:";
	if (GetWildcard(wc_data))
		buf << " *";
	else {
		buf.setf(ios::showbase | ios::internal);
		for (i = 0; i < length; i++)
			buf << " " << hex << setw(4) << setfill('0') <<
			    (int)data[i];
	}
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
SystemExclusiveEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (data != 0)
		delete data;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete SystemExclusiveEvent - bad length");
	data = new unsigned char[length];
	if (data == 0)
		return ("Out of memory");
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete SystemExclusiveEvent");
	memcpy(data, ptr, length);
	return (0);
}

const char *
SystemExclusiveEvent::SMFWrite(SMFTrack &t) const
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
SystemExclusiveEvent::Equal(const Event *e) const
{
	long i;
	SystemExclusiveEvent *eptr = (SystemExclusiveEvent *)e;

	if (!Event::Equal(e))
		return (0);
	if (eptr->GetWildcard(wc_data) || GetWildcard(wc_data))
		return (1);
	if (continued != eptr->continued)
		return (0);
	if (length != eptr->length)
		return (0);
	for (i = 0; i < length; i++)
		if (data[i] != eptr->data[i])
			return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const SystemExclusiveEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

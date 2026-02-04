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

#include "MSeqSpec.h"

const unsigned char *MetaSequencerSpecificEvent::WC_DATA = 0;
const long MetaSequencerSpecificEvent::WC_LENGTH = -1;

const unsigned long MetaSequencerSpecificEvent::wc_data = (1 << 1);

MetaSequencerSpecificEvent::MetaSequencerSpecificEvent() : data(0), length(0L)
{
}

MetaSequencerSpecificEvent::MetaSequencerSpecificEvent(unsigned long t,
    const unsigned char *dat, long len) : MetaEvent(t), length(len)
{

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

MetaSequencerSpecificEvent::MetaSequencerSpecificEvent(
    const MetaSequencerSpecificEvent &e) : MetaEvent(e), length(e.length)
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

MetaSequencerSpecificEvent::~MetaSequencerSpecificEvent()
{

	delete data;
}

void
MetaSequencerSpecificEvent::SetData(const unsigned char *dat, long len)
{

	if (data != 0)
		delete data;
	if (dat == WC_DATA || len == WC_LENGTH) {
		SetWildcard(wc_data);
		data = 0;
		length = -1;
	} else {
		data = new unsigned char[len];
		assert(data != 0);
		memcpy(data, dat, len);
		length = len;
		ClearWildcard(wc_data);
	}
}

MetaSequencerSpecificEvent &
MetaSequencerSpecificEvent::operator=(const MetaSequencerSpecificEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	if (data != 0)
		delete data;
	if (e.GetWildcard(wc_data)) {
		data = 0;
		length = -1;
		return (*this);
	}
	length = e.length;
	data = new unsigned char[e.length];
	assert(data != 0);
	memcpy(data, e.data, e.length);
	ClearWildcard(wc_data);
	return (*this);
}

char *
MetaSequencerSpecificEvent::GetEventStr(void) const
{
	ostrstream buf;
	long i;
	char *tbuf;

	tbuf =  MetaEvent::GetEventStr();
	buf << tbuf << " Data:";
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
MetaSequencerSpecificEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (data != 0)
		delete data;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete MetaSequenceSpecificEvent - bad length");
	data = new unsigned char[length];
	if (data == 0)
		return ("Out of memory");
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete MetaSequencerSpecificEvent");
	memcpy(data, ptr, length);
	return (0);
}

const char *
MetaSequencerSpecificEvent::SMFWrite(SMFTrack &t) const
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
MetaSequencerSpecificEvent::Equal(const Event *e) const
{
	long i;
	MetaSequencerSpecificEvent *eptr = (MetaSequencerSpecificEvent *)e;

	if (!MetaEvent::Equal(e))
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
operator<<(ostream &os, const MetaSequencerSpecificEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

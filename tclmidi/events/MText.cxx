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

#include "MText.h"

const char *MetaTextEvent::WC_STRING = 0;
const long MetaTextEvent::WC_LENGTH = -1;

const unsigned long MetaTextEvent::wc_string = (1 << 1);

MetaTextEvent::MetaTextEvent() : string(0), length(0)
{
}

MetaTextEvent::MetaTextEvent(unsigned long t, const char *str) :
    MetaEvent(t)
{

	if (str == WC_STRING) {
		SetWildcard(wc_string);
		string = 0;
		length = -1;
		return;
	}
	length = strlen(str);
	if (length == 0)
		string = 0;
	else {
		string = new char[length + 1];
		assert(string != 0);
		strcpy(string, str);
	}
}

MetaTextEvent::MetaTextEvent(const MetaTextEvent &e) :
    MetaEvent(e), length(e.length)
{

	if (e.GetWildcard(wc_string)) {
		string = 0;
		length = -1;
		return;
	}
	if (e.length == 0)
		string = 0;
	else {
		string = new char[e.length + 1];
		assert(string != 0);
		strcpy(string, e.string);
	}
}

MetaTextEvent::~MetaTextEvent()
{

	delete string;
}

void
MetaTextEvent::SetString(const char *str)
{

	delete string;
	if (str == WC_STRING) {
		string = 0;
		length = -1;
		SetWildcard(wc_string);
		return;
	}
	length = strlen(str);
	string = new char[length + 1];
	assert(string != 0);
	strcpy(string, str);
	ClearWildcard(wc_string);
}

MetaTextEvent &
MetaTextEvent::operator=(const MetaTextEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	length = e.length;
	delete string;
	if (e.GetWildcard(wc_string)) {
		string = 0;
		return (*this);
	}
	string = new char[length + 1];
	assert(string != 0);
	strcpy(string, e.string);
	return (*this);
}

char *
MetaTextEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Text: ";
	if (GetWildcard(wc_string))
		buf << "*";
	else
		buf << string;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
MetaTextEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (string != 0)
		delete string;
	if ((length = t.GetVarValue()) == -1)
		return ("Incomplete MetaTextEvent - bad length");
	string = new char[length + 1];
	if ((ptr = t.GetData(length)) == 0)
		return ("Incomplete MetaTextEvent - bad data");
	memcpy(string, ptr, length);
	string[length] = '\0';
	return (0);
}

const char *
MetaTextEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(length))
		return ("Out of memory");
	if (!t.PutData((unsigned char *)string, length))
		return ("Out of memory");
	return (0);
}

int
MetaTextEvent::Equal(const Event *e) const
{
	MetaTextEvent *eptr = (MetaTextEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (eptr->GetWildcard(wc_string) || GetWildcard(wc_string))
		return (1);
	if (length != eptr->length)
		return (0);
	return (strcmp(string, eptr->string) == 0);
}

ostream &
operator<<(ostream &os, const MetaTextEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

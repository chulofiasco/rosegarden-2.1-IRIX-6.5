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

#include "MPortNum.h"

const int MetaPortNumberEvent::WC_PORT = -1;
const unsigned long MetaPortNumberEvent::wc_port = (1 << 1);

MetaPortNumberEvent::MetaPortNumberEvent() : port(0)
{
}

MetaPortNumberEvent::MetaPortNumberEvent(unsigned long t, int p) :
    MetaEvent(t), port(p)
{

	if (p == WC_PORT)
		SetWildcard(wc_port);
}

MetaPortNumberEvent::MetaPortNumberEvent(const MetaPortNumberEvent &e) :
    MetaEvent(e), port(e.port)
{
}

MetaPortNumberEvent::~MetaPortNumberEvent()
{
}

MetaPortNumberEvent &
MetaPortNumberEvent::operator=(const MetaPortNumberEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	port = e.port;
	return (*this);
}

char *
MetaPortNumberEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Port: ";
	if (GetWildcard(wc_port))
		buf << "*";
	else
		buf << port;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
MetaPortNumberEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if (t.GetVarValue() != 1)
		return ("Bad length for MetaPortNumberEvent");
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaPortNumberEvent - missing port");
	port = *ptr;
	return (0);
}

const char *
MetaPortNumberEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(1))
		return ("Out of memory");
	if (!t.PutByte(port))
		return ("Out of memory");
	return (0);
}

int
MetaPortNumberEvent::Equal(const Event *e) const
{
	MetaPortNumberEvent *eptr = (MetaPortNumberEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_port) && !GetWildcard(wc_port) &&
	    port != eptr->port)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaPortNumberEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

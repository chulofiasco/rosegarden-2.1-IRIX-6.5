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
#include "Program.h"

const int ProgramEvent::WC_VALUE = -1;
const unsigned long ProgramEvent::wc_value = (1 << 2);

ProgramEvent::ProgramEvent() : value(0)
{
}

ProgramEvent::ProgramEvent(unsigned long t, int chan, int val) :
    NormalEvent(t, chan), value(val)
{

	if (val == WC_VALUE)
		SetWildcard(wc_value);
}

ProgramEvent::ProgramEvent(const ProgramEvent &e) : NormalEvent(e),
    value(e.value)
{
}

ProgramEvent &
ProgramEvent::operator=(const ProgramEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	value = e.value;
	return (*this);
}

char *
ProgramEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Value: ";
	if (GetWildcard(wc_value))
		buf << "*";
	else
		buf << (int)value;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
ProgramEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete ProgramEvent");
	value = *ptr;
	return (0);
}

const char *
ProgramEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(value))
		return ("Out of memory");
	return (0);
}

int
ProgramEvent::Equal(const Event *e) const
{
	ProgramEvent *eptr = (ProgramEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_value) && !GetWildcard(wc_value) &&
	    value != eptr->value)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const ProgramEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

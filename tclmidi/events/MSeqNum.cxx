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
#include "MSeqNum.h"

const long MetaSequenceNumberEvent::WC_NUMBER = 0x0fffffff;

const unsigned long MetaSequenceNumberEvent::wc_number = (1 << 1);

MetaSequenceNumberEvent::MetaSequenceNumberEvent() : number(0)
{
}

MetaSequenceNumberEvent::MetaSequenceNumberEvent(unsigned long t, long num) :
    MetaEvent(t), number(num)
{

	if (num == WC_NUMBER)
		SetWildcard(wc_number);
}

MetaSequenceNumberEvent::MetaSequenceNumberEvent(
    const MetaSequenceNumberEvent &e) : MetaEvent(e), number(e.number)
{
}

MetaSequenceNumberEvent &
MetaSequenceNumberEvent::operator=(const MetaSequenceNumberEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	number = e.number;
	return (*this);
}

char *
MetaSequenceNumberEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Number: ";
	if (GetWildcard(wc_number))
		buf << "*";
	else
		buf << number;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
MetaSequenceNumberEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	// get and throw away length
	if (t.GetVarValue() != 2)
		return ("Incomplete MetaSequenceNumberEvent - bad length");

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSequenceNumberEvent");
	number = (short)*ptr << 8;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSequenceNumberEvent");
	number |= *ptr;
	return (0);
}

const char *
MetaSequenceNumberEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(2))
		return ("Out of memory");
	if (!t.PutByte((number >> 8) & 0x00ff))
		return ("Out of memory");
	if (!t.PutByte(number & 0x00ff))
		return ("Out of memory");
	return (0);
}

int
MetaSequenceNumberEvent::Equal(const Event *e) const
{
	MetaSequenceNumberEvent *eptr = (MetaSequenceNumberEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_number) && !GetWildcard(wc_number) &&
	    number != eptr->number)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaSequenceNumberEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

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
#include "MTempo.h"

const short MetaTempoEvent::WC_TEMPO = -1;
const unsigned long MetaTempoEvent::wc_tempo = (1 << 1);

MetaTempoEvent::MetaTempoEvent() : tempo(120)
{
}

MetaTempoEvent::MetaTempoEvent(unsigned long t, short temp) :
    MetaEvent(t), tempo(temp)
{

	if (temp == WC_TEMPO)
		SetWildcard(wc_tempo);
}

MetaTempoEvent::MetaTempoEvent(
    const MetaTempoEvent &e) : MetaEvent(e), tempo(e.tempo)
{
}

MetaTempoEvent &
MetaTempoEvent::operator=(const MetaTempoEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	tempo = e.tempo;
	return (*this);
}

char *
MetaTempoEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Tempo: ";
	if (GetWildcard(wc_tempo))
		buf << "*";
	else
		buf << (int)tempo;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
MetaTempoEvent::SMFRead(SMFTrack &t)
{
	long usec;
	const unsigned char *ptr;

	// get and throw away length
	if (t.GetVarValue() != 3)
		return  ("Incomplete MetaTempoEvent - bad length");

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTempoEvent");
	usec = *ptr * 0x10000;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTempoEvent");
	usec += *ptr * 0x100;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTempoEvent");
	usec += *ptr;

	tempo = 60000000L / usec;
	return (0);
}

const char *
MetaTempoEvent::SMFWrite(SMFTrack &t) const
{
	long usec;

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(3))
		return ("Out of memory");
	usec = 60000000L / tempo;
	if (!t.PutByte(usec / 0x10000))
		return ("Out of memory");
	usec %= 0x10000;
	if (!t.PutByte(usec / 0x100))
		return ("Out of memory");
	usec %= 0x100;
	if (!t.PutByte(usec))
		return ("Out of memory");
	return (0);
}

int
MetaTempoEvent::Equal(const Event *e) const
{
	MetaTempoEvent *eptr = (MetaTempoEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_tempo) && !GetWildcard(wc_tempo) &&
	    tempo != eptr->tempo)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaTempoEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

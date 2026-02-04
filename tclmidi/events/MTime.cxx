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
#include "MTime.h"

const int MetaTimeEvent::WC_NUMERATOR = 0;
const int MetaTimeEvent::WC_DENOMINATOR = 0;
const int MetaTimeEvent::WC_CLOCKS_PER_BEAT = 0;
const int MetaTimeEvent::WC_32ND_NOTES_PER_QUARTER_NOTE = 0;

const unsigned long MetaTimeEvent::wc_numerator = (1 << 1);
const unsigned long MetaTimeEvent::wc_denominator = (1 << 2);
const unsigned long MetaTimeEvent::wc_clocks_per_beat = (1 << 3);
const unsigned long MetaTimeEvent::wc_32nd_notes_per_quarter_note
    = (1 << 4);

MetaTimeEvent::MetaTimeEvent() : numerator(4), denominator(4), clocks(24),
    thirty_seconds(8)
{
}

MetaTimeEvent::MetaTimeEvent(unsigned long time, int n, int d, int c, int t)
    : MetaEvent(time), numerator(n), denominator(d), clocks(c),
    thirty_seconds(t)
{

	if (n == WC_NUMERATOR)
		SetWildcard(wc_numerator);
	if (d == WC_DENOMINATOR)
		SetWildcard(wc_denominator);
	if (c == WC_CLOCKS_PER_BEAT)
		SetWildcard(wc_clocks_per_beat);
	if (t == WC_32ND_NOTES_PER_QUARTER_NOTE)
		SetWildcard(wc_32nd_notes_per_quarter_note);
}

MetaTimeEvent::MetaTimeEvent(const MetaTimeEvent &e) : MetaEvent(e),
    numerator(e.numerator), denominator(e.denominator), clocks(e.clocks),
    thirty_seconds(e.thirty_seconds)
{
}

MetaTimeEvent &
MetaTimeEvent::operator=(const MetaTimeEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	numerator = e.numerator;
	denominator = e.denominator;
	clocks = e.clocks;
	thirty_seconds = e.thirty_seconds;
	return (*this);
}

char *
MetaTimeEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Numerator: ";
	if (GetWildcard(wc_numerator))
		buf << "*";
	else
		buf << (int)numerator;
	buf << " Denominator: ";
	if (GetWildcard(wc_denominator))
		buf << "*";
	else
		buf << (int)denominator;
	buf << " Clocks Per Metronome Beat: ";
	if (GetWildcard(wc_clocks_per_beat))
		buf << "*";
	else
		buf << (int)clocks;
	buf << " 32nd Notes Per Quarter Note: ";
	if (GetWildcard(wc_32nd_notes_per_quarter_note))
		buf << "*";
	else
		buf << (int)thirty_seconds;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
MetaTimeEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;
	unsigned char i, powof2;

	// get and throw away length
	if (t.GetVarValue() != 4)
		return ("Incomplete MetaTimeEvent - bad length");
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing numerator");
	numerator = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing denominator");
	powof2 = *ptr;
	denominator = 1;
	for (i = 0; i < powof2; i++)
		denominator *= 2;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing clocks");
	clocks = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaTimeEvent - missing 32nds");
	thirty_seconds = *ptr;
	return (0);
}

const char *
MetaTimeEvent::SMFWrite(SMFTrack &t) const
{
	unsigned char i, powof2;

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(4))
		return ("Out of memory");
	if (!t.PutByte(numerator))
		return ("Out of memory");
	for (i = 0, powof2 = 1; powof2 <= denominator; powof2 *= 2, i++);
	i--;
	if (!t.PutByte(i))
		return ("Out of memory");
	if (!t.PutByte(clocks))
		return ("Out of memory");
	if (!t.PutByte(thirty_seconds))
		return ("Out of memory");
	return (0);
}

int
MetaTimeEvent::Equal(const Event *e) const
{
	MetaTimeEvent *eptr = (MetaTimeEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_numerator) && !GetWildcard(wc_numerator) &&
	    numerator != eptr->numerator)
		return (0);
	if (!eptr->GetWildcard(wc_denominator) && !GetWildcard(wc_denominator)
	    && denominator != eptr->denominator)
		return (0);
	if (!eptr->GetWildcard(wc_clocks_per_beat) &&
	    !GetWildcard(wc_clocks_per_beat) && clocks != eptr->clocks)
		return (0);
	if (!eptr->GetWildcard(wc_32nd_notes_per_quarter_note) &&
	    !GetWildcard(wc_32nd_notes_per_quarter_note) &&
	    thirty_seconds != eptr->thirty_seconds)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaTimeEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

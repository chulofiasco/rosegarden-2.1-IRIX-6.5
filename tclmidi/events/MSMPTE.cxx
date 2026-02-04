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
#include "MSMPTE.h"

const int MetaSMPTEEvent::WC_HOUR = -1;
const int MetaSMPTEEvent::WC_MINUTE = -1;
const int MetaSMPTEEvent::WC_SECOND = -1;
const int MetaSMPTEEvent::WC_FRAME = -1;
const int MetaSMPTEEvent::WC_FRACTIONAL_FRAME = -1;

const unsigned long MetaSMPTEEvent::wc_hour = (1 << 1);
const unsigned long MetaSMPTEEvent::wc_minute = (1 << 2);
const unsigned long MetaSMPTEEvent::wc_second = (1 << 3);
const unsigned long MetaSMPTEEvent::wc_frame = (1 << 4);
const unsigned long MetaSMPTEEvent::wc_fractional_frame = (1 << 5);

MetaSMPTEEvent::MetaSMPTEEvent() : hour(0), minute(0), second(0), frame(0),
    fractional_frame(0)
{
}

MetaSMPTEEvent::MetaSMPTEEvent(unsigned long t, int h, int m, int s, int f,
    int ff) : MetaEvent(t), hour(h), minute(m), second(s), frame(f),
    fractional_frame(ff)
{

	if (h == WC_HOUR)
		SetWildcard(wc_hour);
	if (m == WC_MINUTE)
		SetWildcard(wc_minute);
	if (s == WC_SECOND)
		SetWildcard(wc_second);
	if (f == WC_FRAME)
		SetWildcard(wc_frame);
	if (ff == WC_FRACTIONAL_FRAME)
		SetWildcard(wc_fractional_frame);
}

MetaSMPTEEvent::MetaSMPTEEvent(const MetaSMPTEEvent &e) : MetaEvent(e),
    hour(e.hour), minute(e.minute), second(e.second), frame(e.frame),
    fractional_frame(e.fractional_frame)
{
}

MetaSMPTEEvent &
MetaSMPTEEvent::operator=(const MetaSMPTEEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	hour = e.hour;
	minute = e.minute;
	second = e.second;
	frame = e.frame;
	fractional_frame = e.fractional_frame;
	return (*this);
}

char *
MetaSMPTEEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = MetaEvent::GetEventStr();
	buf << tbuf << " Hour: ";
	if (GetWildcard(wc_hour))
		buf << "*";
	else
		buf << (int)hour;
	buf << " Minute: ";
	if (GetWildcard(wc_minute))
		buf << "*";
	else
		buf << (int)minute;
	buf << " Second: ";
	if (GetWildcard(wc_second))
		buf << "*";
	else
		buf << (int)second;
	buf << " Frame: ";
	if (GetWildcard(wc_frame))
		buf << "*";
	else
		buf << (int)frame;
	buf << " Fractional Frame: ";
	if (GetWildcard(wc_fractional_frame))
		buf << "*";
	else
		buf << (int)fractional_frame;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
MetaSMPTEEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	// get and throw away length
	if (t.GetVarValue() != 5)
		return ("Incomplete metaSMPTEEvent - bad length");

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing hour");
	hour = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing minute");
	minute = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing second");
	second = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing frame");
	frame = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete MetaSMPTEEvent - missing fractional "
		    "frame");
	fractional_frame = *ptr;
	return (0);
}

const char *
MetaSMPTEEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(5))
		return ("Out of memory");
	if (!t.PutByte(hour))
		return ("Out of memory");
	if (!t.PutByte(minute))
		return ("Out of memory");
	if (!t.PutByte(second))
		return ("Out of memory");
	if (!t.PutByte(frame))
		return ("Out of memory");
	if (!t.PutByte(fractional_frame))
		return ("Out of memory");
	return (0);
}

int
MetaSMPTEEvent::Equal(const Event *e) const
{
	MetaSMPTEEvent *eptr = (MetaSMPTEEvent *)e;

	if (!MetaEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_hour) && !GetWildcard(wc_hour) &&
	    hour != eptr->hour)
		return (0);
	if (!eptr->GetWildcard(wc_minute) && !GetWildcard(wc_minute) &&
	    minute != eptr->minute)
		return (0);
	if (!eptr->GetWildcard(wc_second) && !GetWildcard(wc_second) &&
	    second != eptr->second)
		return (0);
	if (!eptr->GetWildcard(wc_frame) && !GetWildcard(wc_frame) &&
	    frame != eptr->frame)
		return (0);
	if (!eptr->GetWildcard(wc_fractional_frame) &&
	    !GetWildcard(wc_fractional_frame) &&
	    fractional_frame != eptr->fractional_frame)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const MetaSMPTEEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

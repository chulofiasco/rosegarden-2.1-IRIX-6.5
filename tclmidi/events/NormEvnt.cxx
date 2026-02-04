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
#include "NormEvnt.h"

const int  NormalEvent::WC_CHANNEL = -1;
const unsigned long NormalEvent::wc_channel = (1 << 1);


NormalEvent::NormalEvent() : channel(0)
{
}

NormalEvent::NormalEvent(unsigned long t, int chan) : Event(t), channel(chan)
{

	if (chan == WC_CHANNEL)
		SetWildcard(wc_channel);
}

NormalEvent::NormalEvent(const NormalEvent &e) : Event(e), channel(e.channel)
{
}

NormalEvent &
NormalEvent::operator=(const NormalEvent &e)
{

	(Event)*this = (Event)e;
	channel = e.channel;
	return (*this);
}

char *
NormalEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *str;

	str = Event::GetEventStr();
	buf << str << " Channel: ";
	if (GetWildcard(wc_channel))
		buf << "*";
	else
		buf << (int)channel;
	buf << ends;
	delete str;
	return (buf.str());
}

int
NormalEvent::Equal(const Event *e) const
{
	NormalEvent *eptr = (NormalEvent *)e;

	if (!Event::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_channel) && !GetWildcard(wc_channel) &&
	    channel != eptr->channel)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const NormalEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

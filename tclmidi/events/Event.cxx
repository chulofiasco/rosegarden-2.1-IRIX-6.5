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
#include "Event.h"

const unsigned long Event::WC_TIME = 0xffffffff;
const unsigned long Event::wc_time = (1 << 0);

Event::Event() : time(0L), wildcard(0), next_event(0), prev_event(0), node(0)
{
}

Event::Event(unsigned long t) : time(t), wildcard(0), next_event(0),
    prev_event(0), node(0)
{

	if (t == WC_TIME)
		SetWildcard(wc_time);
}

Event::Event(const Event &e) : time(e.time), wildcard(e.wildcard),
    next_event(0), prev_event(0), node(0)
{
}

int
Event::operator==(const Event &e) const
{

	if (this == &e)
		return (1);
	if (GetType() != e.GetType())
		return (0);
	return (this->Equal(&e));
}

Event &
Event::operator=(const Event &e)
{

	time = e.time;
	wildcard = e.wildcard;
	return (*this);
}

char *
Event::GetEventStr(void) const
{
	ostrstream buf;

	buf << "Time: ";
	if (GetWildcard(wc_time))
		buf << "*";
	else
		buf << time;
	buf << " Type: " << this->GetTypeStr() << ends;
	return (buf.str());
}

void
Event::Print(ostream &os) const
{
	char *str;

	os << (str = this->GetEventStr());
	delete str;
}

int
Event::Equal(const Event *e) const
{

	if (!e->GetWildcard(wc_time) && !GetWildcard(wc_time) &&
	    time != e->time)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const Event &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

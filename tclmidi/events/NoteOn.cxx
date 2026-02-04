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
#include "NoteOn.h"

const unsigned long NoteOnEvent::WC_DURATION = 0xffffffff;

const unsigned long NoteOnEvent::wc_duration = (1 << 4);

NoteOnEvent::NoteOnEvent()
{
}

NoteOnEvent::NoteOnEvent(unsigned long t, int chan, int pit, int vel,
    const NoteEvent *np) : NoteEvent(t, chan, pit, vel, np)
{

	if (np != 0 && np->GetTime() != WC_TIME && t != WC_TIME)
		duration = np->GetTime() - t;
}

NoteOnEvent::NoteOnEvent(unsigned long t, int chan, int pit, int vel,
    unsigned long dur, const NoteEvent *np) : NoteEvent(t, chan, pit, vel, np),
    duration(dur)
{

	if (dur == WC_DURATION)
		SetWildcard(wc_duration);
}

NoteOnEvent::NoteOnEvent(const NoteOnEvent &e) : NoteEvent(e)
{
}

NoteOnEvent &
NoteOnEvent::operator=(const NoteOnEvent &e)
{

	(NoteEvent)*this = (NoteEvent)e;
	return (*this);
}

char *
NoteOnEvent::GetEventStr(void) const
{

	return (NoteEvent::GetEventStr());
}

int
NoteOnEvent::Equal(const Event *e) const
{
	NoteOnEvent *eptr = (NoteOnEvent *)e;

	if (!NoteEvent::Equal(e))
		return (0);
	if (GetNotePair() != 0 && eptr->GetNotePair() != 0) {
		if (!eptr->GetWildcard(wc_duration) &&
		    !GetWildcard(wc_duration) && duration != eptr->duration)
			return (0);
	}
	return (1);
}

ostream &
operator<<(ostream &os, const NoteOnEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

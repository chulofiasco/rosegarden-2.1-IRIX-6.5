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
#include "MEOT.h"

MetaEndOfTrackEvent::MetaEndOfTrackEvent()
{
}

MetaEndOfTrackEvent::MetaEndOfTrackEvent(unsigned long t) : MetaEvent(t)
{
}

MetaEndOfTrackEvent::MetaEndOfTrackEvent(const MetaEndOfTrackEvent &e) :
    MetaEvent(e)
{
}

MetaEndOfTrackEvent &
MetaEndOfTrackEvent::operator=(const MetaEndOfTrackEvent &e)
{

	(MetaEvent)*this = (MetaEvent)e;
	return (*this);
}

char *
MetaEndOfTrackEvent::GetEventStr(void) const
{

	return (MetaEvent::GetEventStr());
}

const char *
MetaEndOfTrackEvent::SMFRead(SMFTrack &t)
{

	// get length and throw away - will be zero
	if (t.GetVarValue() != 0)
		return ("Incomplete MetaEndOfTrackEvent - bad length");
	return (0);
}

const char *
MetaEndOfTrackEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutFixValue(0))
		return ("Out of memory");
	return (0);
}

ostream &
operator<<(ostream &os, const MetaEndOfTrackEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

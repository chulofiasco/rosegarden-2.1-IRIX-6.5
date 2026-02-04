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
#include "KeyPres.h"

const int KeyPressureEvent::WC_PITCH = -1;
const int KeyPressureEvent::WC_PRESSURE = -1;
const unsigned long KeyPressureEvent::wc_pitch = (1 << 2);
const unsigned long KeyPressureEvent::wc_pressure = (1 << 3);

KeyPressureEvent::KeyPressureEvent() : pitch(0), pressure(0)
{
}

KeyPressureEvent::KeyPressureEvent(unsigned long t, int chan, int pit,
    int pres) : NormalEvent(t, chan), pitch(pit), pressure(pres)
{

	if (pit == WC_PITCH)
		SetWildcard(wc_pitch);
	if (pres == WC_PRESSURE)
		SetWildcard(wc_pressure);
}

KeyPressureEvent::KeyPressureEvent(const KeyPressureEvent &e) : NormalEvent(e),
    pitch(e.pitch), pressure(e.pressure)
{
}

KeyPressureEvent &
KeyPressureEvent::operator=(const KeyPressureEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	pitch = e.pitch;
	pressure = e.pressure;
	return (*this);
}

char *
KeyPressureEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Pitch: ";
	if (GetWildcard(wc_pitch))
		buf << "*";
	else
		buf << (int)pitch;
	buf << " Pressure: ";
	if (GetWildcard(wc_pressure))
		buf << "*";
	else
		buf << (int)pressure;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
KeyPressureEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete KeyPressureEvent - missing pitch");
	pitch = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete KeyPressureEvent - missing pressure");
	pressure = *ptr;
	return (0);
}

const char *
KeyPressureEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(pitch))
		return ("Out of memory");
	if (!t.PutByte(pressure))
		return ("Out of memory");
	return (0);
}

int
KeyPressureEvent::Equal(const Event *e) const
{
	KeyPressureEvent *eptr = (KeyPressureEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_pitch) && !GetWildcard(wc_pitch) &&
	    pitch != eptr->pitch)
		return (0);
	if (!eptr->GetWildcard(wc_pressure) && !GetWildcard(wc_pressure)
	    && pressure != eptr->pressure)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const KeyPressureEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

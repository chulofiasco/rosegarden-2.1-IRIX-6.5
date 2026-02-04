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
#include "Note.h"

const unsigned long NoteEvent::wc_pitch = (1 << 2);
const unsigned long NoteEvent::wc_velocity = (1 << 3);

const int NoteEvent::WC_PITCH = -1;
const int NoteEvent::WC_VELOCITY = -1;

NoteEvent::NoteEvent() : pitch(0), velocity(0), note_pair(0)
{
}

NoteEvent::NoteEvent(unsigned long t, int chan, int pit, int vel,
    const NoteEvent *np) : NormalEvent(t, chan), pitch(pit), velocity(vel),
    note_pair((NoteEvent *)np)
{

	if (pit == WC_PITCH)
		SetWildcard(wc_pitch);
	if (vel == WC_VELOCITY)
		SetWildcard(wc_velocity);
}

NoteEvent::NoteEvent(const NoteEvent &e) : NormalEvent(e),
    pitch(e.pitch), velocity(e.velocity), note_pair(e.note_pair)
{
}

NoteEvent &
NoteEvent::operator=(const NoteEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	pitch = e.pitch;
	velocity = e.velocity;
	note_pair = e.note_pair;
	return (*this);
}

char *
NoteEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Pitch: ";
	if (GetWildcard(wc_pitch))
		buf << "*";
	else
		buf << (int)pitch;
	buf << " Velocity: ";
	if (GetWildcard(wc_velocity))
		buf << "*";
	else
		buf << (int)velocity;
	if (note_pair != 0) {
		buf << " NotePair Time: ";
		if (note_pair->GetTime() == WC_TIME)
			buf << "*";
		else
			buf << note_pair->GetTime();
	}
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
NoteEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete NoteEvent - missing pitch");
	pitch = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete NoteEvent - missing velocity");
	velocity = *ptr;
	return (0);
}

const char *
NoteEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(pitch))
		return ("Out of memory");
	if (!t.PutByte(velocity))
		return ("Out of memory");
	return (0);
}

int
NoteEvent::Equal(const Event *e) const
{
	NoteEvent *eptr = (NoteEvent *)e;

	// make sure note isn't = to noteon
	if ((note_pair == 0 && eptr->note_pair != 0) ||
	    (note_pair != 0 && eptr->note_pair == 0))
		return (0);
	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_pitch) && !GetWildcard(wc_pitch) &&
	    pitch != eptr->pitch)
		return (0);
	if (!eptr->GetWildcard(wc_velocity) && !GetWildcard(wc_velocity) &&
	    velocity != eptr->velocity)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const NoteEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

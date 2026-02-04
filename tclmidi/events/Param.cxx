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
#include "Param.h"

const int ParameterEvent::WC_PARAMETER = -1;
const int ParameterEvent::WC_VALUE = -1;
const unsigned long ParameterEvent::wc_parameter = (1 << 2);
const unsigned long ParameterEvent::wc_value = (1 << 3);

ParameterEvent::ParameterEvent() : parameter(0), value(0)
{
}

ParameterEvent::ParameterEvent(unsigned long t, int chan, int param,
    int val) : NormalEvent(t, chan), parameter(param), value(val)
{

	if (param == WC_PARAMETER)
		SetWildcard(wc_parameter);
	if (val == WC_VALUE)
		SetWildcard(wc_value);
}

ParameterEvent::ParameterEvent(const ParameterEvent &e) : NormalEvent(e),
    parameter(e.parameter), value(e.value)
{
}

ParameterEvent &
ParameterEvent::operator=(const ParameterEvent &e)
{

	(NormalEvent)*this = (NormalEvent)e;
	parameter = e.parameter;
	value = e.value;
	return (*this);
}

char *
ParameterEvent::GetEventStr(void) const
{
	ostrstream buf;
	char *tbuf;

	tbuf = NormalEvent::GetEventStr();
	buf << tbuf << " Parameter: ";
	if (GetWildcard(wc_parameter))
		buf << "*";
	else
		buf << (int)parameter;
	buf << " Value: ";
	if (GetWildcard(wc_value))
		buf << "*";
	else
		buf << (int)value;
	buf << ends;
	delete tbuf;
	return (buf.str());
}

const char *
ParameterEvent::SMFRead(SMFTrack &t)
{
	const unsigned char *ptr;

	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete ParameterEvent - missing parameter");
	parameter = *ptr;
	if ((ptr = t.GetByte()) == 0)
		return ("Incomplete ParameterEvent - missing value");
	value = *ptr;
	return (0);
}

const char *
ParameterEvent::SMFWrite(SMFTrack &t) const
{

	if (IsWildcard())
		return("Can't write wildcard events");
	if (!t.PutByte(parameter))
		return ("Out of memory");
	if (!t.PutByte(value))
		return ("Out of memory");
	return (0);
}

int
ParameterEvent::Equal(const Event *e) const
{
	ParameterEvent *eptr = (ParameterEvent *)e;

	if (!NormalEvent::Equal(e))
		return (0);
	if (!eptr->GetWildcard(wc_parameter) && !GetWildcard(wc_parameter)
	    && parameter != eptr->parameter)
		return (0);
	if (!eptr->GetWildcard(wc_value) && !GetWildcard(wc_value) &&
	    value != eptr->value)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const ParameterEvent &e)
{
	char *str;

	os << (str = e.GetEventStr());
	delete str;
	return (os);
}

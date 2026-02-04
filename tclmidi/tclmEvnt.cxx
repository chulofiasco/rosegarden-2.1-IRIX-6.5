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
extern "C" {
#include <tcl.h>
}
/* Damn Microsoft compiler can't deal with the real name of the include file */
#ifndef _MSC_VER
#include <strstream.h>
#else
#include <strstrea.h>
#endif
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "tclmidi.h"
#include "tclmEvnt.h"

void
Tclm_PrintEvent(ostream &buf, Event *e)
{
	char *str;

	switch (e->GetType()) {
	case NOTEOFF:
		if (((NoteEvent *)e)->GetNotePair() != 0) {
			buf << ends;
			return;
		}
		str = Tclm_PrintNoteOff((NoteOffEvent *)e);
		break;
	case NOTEON:
		if (((NoteEvent *)e)->GetNotePair() == 0)
			str = Tclm_PrintNoteOn((NoteOnEvent *)e);
		else {
			if ((int)((NoteEvent *)e)->GetVelocity() == 0) {
				buf << ends;
				return;
			}
			str = Tclm_PrintNote((NoteOnEvent *)e);
		}
		break;
	case KEYPRESSURE:
		str = Tclm_PrintKeyPressure((KeyPressureEvent *)e);
		break;
	case PARAMETER:
		str = Tclm_PrintParameter((ParameterEvent *)e);
		break;
	case PROGRAM:
		str = Tclm_PrintProgram((ProgramEvent *)e);
		break;
	case CHANNELPRESSURE:
		str = Tclm_PrintChannelPressure((ChannelPressureEvent *)e);
		break;
	case PITCHWHEEL:
		str = Tclm_PrintPitchWheel((PitchWheelEvent *)e);
		break;
	case SYSTEMEXCLUSIVE:
		str = Tclm_PrintSystemExclusive((SystemExclusiveEvent *)e);
		break;
	case METASEQUENCENUMBER:
		str = Tclm_PrintMetaSequenceNumber(
		    (MetaSequenceNumberEvent *)e);
		break;
	case METATEXT:
		str = Tclm_PrintMetaText((MetaTextEvent *)e);
		break;
	case METACOPYRIGHT:
		str = Tclm_PrintMetaCopyright((MetaCopyrightEvent *)e);
		break;
	case METASEQUENCENAME:
		str = Tclm_PrintMetaSequenceName((MetaSequenceNameEvent *)e);
		break;
	case METAINSTRUMENTNAME:
		str = Tclm_PrintMetaInstrumentName(
		    (MetaInstrumentNameEvent *)e);
		break;
	case METALYRIC:
		str = Tclm_PrintMetaLyric((MetaLyricEvent *)e);
		break;
	case METAMARKER:
		str = Tclm_PrintMetaMarker((MetaMarkerEvent *)e);
		break;
	case METACUE:
		str = Tclm_PrintMetaCue((MetaCueEvent *)e);
		break;
	case METACHANNELPREFIX:
		str = Tclm_PrintMetaChannelPrefix((MetaChannelPrefixEvent *)e);
		break;
	case METAPORTNUMBER:
		str = Tclm_PrintMetaPortNumber((MetaPortNumberEvent *)e);
		break;
	case METAENDOFTRACK:
		str = Tclm_PrintMetaEndOfTrack((MetaEndOfTrackEvent *)e);
		break;
	case METATEMPO:
		str = Tclm_PrintMetaTempo((MetaTempoEvent *)e);
		break;
	case METASMPTE:
		str = Tclm_PrintMetaSMPTE((MetaSMPTEEvent *)e);
		break;
	case METATIME:
		str = Tclm_PrintMetaTime((MetaTimeEvent *)e);
		break;
	case METAKEY:
		str = Tclm_PrintMetaKey((MetaKeyEvent *)e);
		break;
	case METASEQUENCERSPECIFIC:
		str = Tclm_PrintMetaSequencerSpecific(
		    (MetaSequencerSpecificEvent *)e);
		break;
	case METAUNKNOWN:
		str = Tclm_PrintMetaUnknown((MetaUnknownEvent *)e);
		break;
	default:
		str = 0;
		break;
	}
	if (e->GetTime() == Event::WC_TIME)
		buf << "* " << str << ends;
	else
		buf << e->GetTime() << " " << str << ends;
	delete str;
}

char *
Tclm_PrintNoteOff(NoteOffEvent *e)
{
	ostrstream buf;
	int c, p, v;

	c = e->GetChannel();
	p = e->GetPitch();
	v = e->GetVelocity();
	buf << "NoteOff ";
	if (c == NoteOffEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (p == NoteOffEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << " ";
	if (v == NoteOffEvent::WC_VELOCITY)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintNoteOn(NoteOnEvent *e)
{
	ostrstream buf;
	int c, p, v;

	c = e->GetChannel();
	p = e->GetPitch();
	v = e->GetVelocity();
	buf << "NoteOn ";
	if (c == NoteOnEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (p == NoteOnEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << " ";
	if (v == NoteOnEvent::WC_VELOCITY)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintNote(NoteOnEvent *e)
{
	ostrstream buf;
	int c, p, v;
	unsigned long d;

	c = e->GetChannel();
	p = e->GetPitch();
	v = e->GetVelocity();
	d = e->GetDuration();
	buf << "Note ";
	if (c == NoteOnEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (p == NoteOnEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << " ";
	if (v == NoteOnEvent::WC_VELOCITY)
		buf << "*";
	else
		buf << v;
	buf << " ";
	if (d == NoteOnEvent::WC_DURATION)
		buf << "*";
	else
		buf << d;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintKeyPressure(KeyPressureEvent *e)
{
	ostrstream buf;
	int c, p;

	c = e->GetChannel();
	p = e->GetPitch();
	buf << "KeyPressure ";
	if (c == KeyPressureEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (p == KeyPressureEvent::WC_PITCH)
		buf << "*";
	else
		buf << p;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintParameter(ParameterEvent *e)
{
	ostrstream buf;
	int c, p, v;

	c = e->GetChannel();
	p = e->GetParameter();
	v = e->GetValue();

	buf << "Parameter ";
	if (c == ParameterEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (p == ParameterEvent::WC_PARAMETER)
		buf << "*";
	else
		buf << p;
	buf << " ";
	if (v == ParameterEvent::WC_VALUE)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintProgram(ProgramEvent *e)
{
	ostrstream buf;
	int c, v;

	c = e->GetChannel();
	v = e->GetValue();
	buf << "Program ";
	if (c == ProgramEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (v == ProgramEvent::WC_VALUE)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintChannelPressure(ChannelPressureEvent *e)
{
	ostrstream buf;
	int c, p;

	c = e->GetChannel();
	p = e->GetPressure();

	buf << "ChannelPressure ";
	if (c == ChannelPressureEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (p == ChannelPressureEvent::WC_PRESSURE)
		buf << "*";
	else
		buf << p;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintPitchWheel(PitchWheelEvent *e)
{
	ostrstream buf;
	int c;
	long v;

	c = e->GetChannel();
	v = e->GetValue();

	buf << "PitchWheel ";
	if (c == PitchWheelEvent::WC_CHANNEL)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (v == PitchWheelEvent::WC_VALUE)
		buf << "*";
	else
		buf << v;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintSystemExclusive(SystemExclusiveEvent *e)
{
	ostrstream buf;
	const unsigned char *d;
	long l;

	d = e->GetData();
	l = e->GetLength();

	buf << "SystemExclusive ";
	if (e->GetContinued() == 1)
		buf << "continued ";
	if (d == SystemExclusiveEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, e->GetData(), e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaSequenceNumber(MetaSequenceNumberEvent *e)
{
	ostrstream buf;
	long l;

	l = e->GetNumber();

	buf << "MetaSequenceNumber ";
	if (l == MetaSequenceNumberEvent::WC_NUMBER)
		buf << "*";
	else
		buf << l;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaText(MetaTextEvent *e)
{
	ostrstream buf;
	const char *s;

	s = e->GetString();

	buf << "MetaText ";
	if (s == MetaTextEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaCopyright(MetaCopyrightEvent *e)
{
	ostrstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaCopyright ";
	if (s == MetaCopyrightEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaSequenceName(MetaSequenceNameEvent *e)
{
	ostrstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaSequenceName ";
	if (s == MetaSequenceNameEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaInstrumentName(MetaInstrumentNameEvent *e)
{
	ostrstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaInstrumentName ";
	if (s == MetaInstrumentNameEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaLyric(MetaLyricEvent *e)
{
	ostrstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaLyric ";
	if (s == MetaLyricEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaMarker(MetaMarkerEvent *e)
{
	ostrstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaMarker ";
	if (s == MetaMarkerEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaCue(MetaCueEvent *e)
{
	ostrstream buf;
	const char *s;

	s = e->GetString();
	buf << "MetaCue ";
	if (s == MetaCueEvent::WC_STRING)
		buf << "*";
	else
		buf << "{" << s << "}";
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaChannelPrefix(MetaChannelPrefixEvent *e)
{
	ostrstream buf;
	const unsigned char *d;

	d = e->GetData();
	buf << "MetaChannelPrefix ";
	if (d == MetaChannelPrefixEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, d, e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaPortNumber(MetaPortNumberEvent *e)
{
	ostrstream buf;
	int p;

	p = e->GetPort();
	buf << "MetaPortNumber ";
	if (p == MetaPortNumberEvent::WC_PORT)
		buf << "*";
	else
		buf << p;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaEndOfTrack(MetaEndOfTrackEvent *e)
{
	ostrstream buf;
	MetaEndOfTrackEvent *dummy;

	// shut up a warning
	dummy = e;

	buf << "MetaEndOfTrack" << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaTempo(MetaTempoEvent *e)
{
	ostrstream buf;
	short t;

	t = e->GetTempo();
	buf << "MetaTempo ";
	if (t == MetaTempoEvent::WC_TEMPO)
		buf << "*";
	else
		buf << t;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaSMPTE(MetaSMPTEEvent *e)
{
	ostrstream buf;
	int h, m, s, f, ff;

	h = e->GetHour();
	m = e->GetMinute();
	s = e->GetSecond();
	f = e->GetFrame();
	ff = e->GetFractionalFrame();
	buf << "MetaSMPTE ";
	if (h == MetaSMPTEEvent::WC_HOUR)
		buf << "*";
	else
		buf << h;
	buf << " ";
	if (m == MetaSMPTEEvent::WC_MINUTE)
		buf << "*";
	else
		buf << m;
	buf << " ";
	if (s == MetaSMPTEEvent::WC_SECOND)
		buf << "*";
	else
		buf << s;
	buf << " ";
	if (f == MetaSMPTEEvent::WC_FRAME)
		buf << "*";
	else
		buf << f;
	buf << " ";
	if (ff == MetaSMPTEEvent::WC_FRACTIONAL_FRAME)
		buf << "*";
	else
		buf << ff;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaTime(MetaTimeEvent *e)
{
	ostrstream buf;
	int n, d, c, t;

	n = e->GetNumerator();
	d = e->GetDenominator();
	c = e->GetClocksPerBeat();
	t = e->Get32ndNotesPerQuarterNote();

	buf << "MetaTime ";
	if (n == MetaTimeEvent::WC_NUMERATOR)
		buf << "*";
	else
		buf << n;
	buf << " ";
	if (d == MetaTimeEvent::WC_DENOMINATOR)
		buf << "*";
	else
		buf << d;
	buf << " ";
	if (c == MetaTimeEvent::WC_CLOCKS_PER_BEAT)
		buf << "*";
	else
		buf << c;
	buf << " ";
	if (t == MetaTimeEvent::WC_32ND_NOTES_PER_QUARTER_NOTE)
		buf << "*";
	else
		buf << t;
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaKey(MetaKeyEvent *e)
{
	ostrstream buf;

	buf << "MetaKey ";
	if (e->GetKey() == MetaKeyEvent::WC_KEY)
		buf << "*";
	else
		buf << "{" << e->GetKeyStr() << "}";
	buf << " ";
	if (e->GetMode() == MetaKeyEvent::WC_MODE)
		buf << "*";
	else
		buf << e->GetModeStr();
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaSequencerSpecific(MetaSequencerSpecificEvent *e)
{
	ostrstream buf;
	const unsigned char *d;

	d = e->GetData();

	buf << "MetaSequencerSpecific ";
	if (d == MetaSequencerSpecificEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, d, e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

char *
Tclm_PrintMetaUnknown(MetaUnknownEvent *e)
{
	ostrstream buf;
	int t;
	const unsigned char *d;

	t = e->GetMetaType();
	d = e->GetData();

	buf << "MetaUnknown ";
	if (t == MetaUnknownEvent::WC_META_TYPE)
		buf << "*";
	else
		buf << t;
	buf << " ";
	if (d == MetaUnknownEvent::WC_DATA)
		buf << "*";
	else {
		buf << "{";
		Tclm_PrintData(buf, d, e->GetLength());
		buf << "}";
	}
	buf << ends;
	return (buf.str());
}

Event *
Tclm_ParseEvent(Tcl_Interp *interp, char *str)
{
	Event *event;
	Event *(*pfunc)(Tcl_Interp *, long, int, char **);
	char **argv, **aptr;;
	char *name;
	long time;
	int argc, i, length;

	if (Tcl_SplitList(interp, str, &argc, &argv) != TCL_OK)
		return (0);
	aptr = argv;

	if (strcmp(argv[0], "*") == 0)
		time = Event::WC_TIME;
	else {
		if (Tcl_GetLong(interp, argv[0], &time) != TCL_OK)
			return (0);
	}

	length = strlen(argv[1]);
	name = new char[length + 1];
	for (i = 0; i < length; i++)
		name[i] = tolower(argv[1][i]);
	name[i] = '\0';

	argv++;
	argc--;
	
	pfunc = 0;
	switch (name[0]) {
	case 'c':
		if (strncmp(name, "channelpressure", length) == 0)
			pfunc = Tclm_ParseChannelPressure;
		break;
	case 'k':
		if (strncmp(name, "keypressure", length) == 0)
			pfunc = Tclm_ParseKeyPressure;
		break;
	case 'm':
		// meta events
		switch (name[4]) {
		case 'c':
			if (strncmp(name, "metachannelprefix", length) == 0)
				pfunc = Tclm_ParseMetaChannelPrefix;
			else if (strncmp(name, "metacopyright", length) == 0)
				pfunc = Tclm_ParseMetaCopyright;
			else if (strncmp(name, "metacue", length) == 0)
				pfunc = Tclm_ParseMetaCue;
			break;
		case 'e':
			if (strncmp(name, "metaendoftrack", length) == 0)
				pfunc = Tclm_ParseMetaEndOfTrack;
			break;
		case 'i':
			if (strncmp(name, "metainstrumentname", length) == 0)
				pfunc = Tclm_ParseMetaInstrumentName;
			break;
		case 'k':
			if (strncmp(name, "metakey", length) == 0)
				pfunc = Tclm_ParseMetaKey;
			break;
		case 'l':
			if (strncmp(name, "metalyric", length) == 0)
				pfunc = Tclm_ParseMetaLyric;
			break;
		case 'm':
			if (strncmp(name, "metamarker", length) == 0)
				pfunc = Tclm_ParseMetaMarker;
			break;
		case 'p':
			if (strncmp(name, "metaportnumber", length) == 0)
				pfunc = Tclm_ParseMetaPortNumber;
			break;
		case 's':
			if (strncmp(name, "metasequencename", length) == 0)
				pfunc = Tclm_ParseMetaSequenceName;
			else if (strncmp(name, "metasequencenumber", length)
			    == 0)
				pfunc = Tclm_ParseMetaSequenceNumber;
			else if (strncmp(name, "metasequencerspecific", length)
			    == 0)
				pfunc = Tclm_ParseMetaSequencerSpecific;
			else if (strncmp(name, "metasmpte", length) == 0)
				pfunc = Tclm_ParseMetaSMPTE;
			break;
		case 't':
			if (strncmp(name, "metatempo", length) == 0)
				pfunc = Tclm_ParseMetaTempo;
			else if (strncmp(name, "metatext", length) == 0)
				pfunc = Tclm_ParseMetaText;
			else if (strncmp(name, "metatime", length) == 0)
				pfunc = Tclm_ParseMetaTime;
			break;
		case 'u':
			if (strncmp(name, "metaunknown", length) == 0)
				pfunc = Tclm_ParseMetaUnknown;
			break;
		}
		break;
	case 'n':
		if (strncmp(name, "note", length) == 0)
			pfunc = Tclm_ParseNote;
		else if (strncmp(name, "noteoff", length) == 0)
			pfunc = Tclm_ParseNoteOff;
		else if (strncmp(name, "noteon", length) == 0)
			pfunc = Tclm_ParseNoteOn;
		break;
	case 'p':
		if (strncmp(name, "parameter", length) == 0)
			pfunc = Tclm_ParseParameter;
		else if (strncmp(name, "pitchwheel", length) == 0)
			pfunc = Tclm_ParsePitchWheel;
		else if (strncmp(name, "program", length) == 0)
			pfunc = Tclm_ParseProgram;
		break;
	case 's':
		if (strncmp(name, "systemexclusive", length) == 0)
			pfunc = Tclm_ParseSystemExclusive;
		break;
	}

	if (pfunc == 0) {
		Tcl_AppendResult(interp, "bad event type ", argv[0], 0);
		Tcl_Ckfree((char *)aptr);
		delete name;
		return (0);
	}
	event = pfunc(interp, time, argc, argv);
	Tcl_Ckfree((char *)aptr);
	delete name;
	return (event);
}

Event *
Tclm_ParseNoteOff(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int channel, pitch, velocity;

	if (argc != 3 && argc != 4) {
		Tcl_SetResult(interp, "bad event: should be \"time NoteOff "
		    "channel pitch ?velocity?\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = NoteOffEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		pitch = NoteOffEvent::WC_PITCH;
	else if (!Tclm_ParseDataByte(interp, argv[2], &pitch))
		return (0);
	if (argc == 3)
		velocity = 0;
	else if (strcmp(argv[3], "*") == 0)
		velocity = NoteOffEvent::WC_VELOCITY;
	else if (!Tclm_ParseDataByte(interp, argv[3], &velocity))
		return (0);

	return (new NoteOffEvent(time, channel, pitch, velocity));
}

Event *
Tclm_ParseNoteOn(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int channel, pitch, velocity;

	if (argc != 4) {
		Tcl_SetResult(interp, "bad event: should be \"time NoteOn "
		    "channel pitch velocity\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = NoteOnEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		pitch = NoteOnEvent::WC_PITCH;
	else if (!Tclm_ParseDataByte(interp, argv[2], &pitch))
		return (0);
	if (strcmp(argv[3], "*") == 0)
		velocity = NoteOnEvent::WC_VELOCITY;
	else if (!Tclm_ParseDataByte(interp, argv[3], &velocity))
		return (0);

	return (new NoteOnEvent(time, channel, pitch, velocity));
}

Event *
Tclm_ParseNote(Tcl_Interp *interp, long time, int argc, char **argv)
{
	NoteOnEvent *event;
	NoteOffEvent *off;
	unsigned long duration;
	int channel, pitch, velocity;

	if (argc != 5) {
		Tcl_SetResult(interp, "bad event: should be \"time Note "
		    "channel pitch velocity duration\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = NoteOnEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		pitch = NoteOnEvent::WC_PITCH;
	else if (!Tclm_ParseDataByte(interp, argv[2], &pitch))
		return (0);
	if (strcmp(argv[3], "*") == 0)
		velocity = NoteOnEvent::WC_VELOCITY;
	else if (!Tclm_ParseDataByte(interp, argv[3], &velocity))
		return (0);
	if (strcmp(argv[4], "*") == 0)
		duration = NoteOnEvent::WC_DURATION;
	else if (Tcl_GetLong(interp, argv[4], (long *)&duration) != TCL_OK)
		return (0);

	event = new NoteOnEvent();
	event->SetTime(time);
	event->SetChannel(channel);
	event->SetPitch(pitch);
	event->SetVelocity(velocity);
	event->SetDuration(duration);

	off = new NoteOffEvent();
	if (duration == NoteOnEvent::WC_DURATION)
		off->SetTime(NoteOffEvent::WC_TIME);
	else
		off->SetTime(time + duration);
	off->SetChannel(channel);
	off->SetPitch(pitch);
	event->SetNotePair(off);
	off->SetNotePair(event);

	return (event);
}

Event *
Tclm_ParseKeyPressure(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int channel, pitch, pressure;

	if (argc != 4) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time KeyPressure channel pitch pressure\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = KeyPressureEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		pitch = KeyPressureEvent::WC_PITCH;
	else if (!Tclm_ParseDataByte(interp, argv[2], &pitch))
		return (0);
	if (strcmp(argv[3], "*") == 0)
		pressure = KeyPressureEvent::WC_PRESSURE;
	else if (!Tclm_ParseDataByte(interp, argv[3], &pressure))
		return (0);

	return (new KeyPressureEvent(time, channel, pitch, pressure));
}

Event *
Tclm_ParseParameter(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int channel, parameter, value;

	if (argc != 4) {
		Tcl_SetResult(interp, "bad event: should be \"time Parameter "
		    "channel parameter value\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = ParameterEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		parameter = ParameterEvent::WC_PARAMETER;
	else if (!Tclm_ParseDataByte(interp, argv[2], &parameter))
		return (0);
	if (strcmp(argv[3], "*") == 0)
		value = ParameterEvent::WC_VALUE;
	else if (!Tclm_ParseDataByte(interp, argv[3], &value))
		return (0);

	return (new ParameterEvent(time, channel, parameter, value));
}

Event *
Tclm_ParseProgram(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int channel, value;

	if (argc != 3) {
		Tcl_SetResult(interp, "bad event: should be \"time Program "
		    "channel value\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = ProgramEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		value = ProgramEvent::WC_VALUE;
	else if (!Tclm_ParseDataByte(interp, argv[2], &value))
		return (0);

	return (new ProgramEvent(time, channel, value));
}

Event *
Tclm_ParseChannelPressure(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int channel, pressure;

	if (argc != 3) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time ChannelPressure channel pressure\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = ChannelPressureEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		pressure = ChannelPressureEvent::WC_PRESSURE;
	else if (!Tclm_ParseDataByte(interp, argv[2], &pressure))
		return (0);

	return (new ChannelPressureEvent(time, channel, pressure));
}

Event *
Tclm_ParsePitchWheel(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int channel;
	long value;

	if (argc != 3) {
		Tcl_SetResult(interp, "bad event: should be \"time PitchWheel "
		    "channel value\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		channel = PitchWheelEvent::WC_CHANNEL;
	else if (!Tclm_ParseDataByte(interp, argv[1], &channel))
		return (0);
	if (strcmp(argv[2], "*") == 0)
		value = PitchWheelEvent::WC_VALUE;
	else if (Tcl_GetLong(interp, argv[2], &value) != TCL_OK)
		return (0);

	return (new PitchWheelEvent(time, channel, value));
}

Event *
Tclm_ParseSystemExclusive(Tcl_Interp *interp, long time, int argc, char **argv)
{
	char **str;
	SystemExclusiveEvent *event;
	unsigned char *data;
	long len;
	int i, val;

	if ((argc != 2 && argc != 3) || (argc == 3 && strncmp(argv[1], "cont",
	    4) != 0)) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time SystemExclusive ?continued? {data ?data ...?}\"",
		    TCL_STATIC);
		return (0);
	}

	if (argc == 2) {
		if (strcmp(argv[1], "*") == 0) {
			data = (unsigned char *)SystemExclusiveEvent::WC_DATA;
			len = SystemExclusiveEvent::WC_LENGTH;
			event = new SystemExclusiveEvent(time, data, len);
			return (event);
		}
		if (Tcl_SplitList(interp, argv[1], (int *)&len, &str) != TCL_OK)
			return (0);
	} else {
		if (strcmp(argv[2], "*") == 0) {
			data = (unsigned char *)SystemExclusiveEvent::WC_DATA;
			len = SystemExclusiveEvent::WC_LENGTH;
			event = new SystemExclusiveEvent(time, data, len);
			event->SetContinued(1);
			return (event);
		}
		if (Tcl_SplitList(interp, argv[2], (int *)&len, &str) != TCL_OK)
			return (0);
	}

	data = new unsigned char[len];
	if (data == 0)
		return (0);

	for (i = 0; i < len; i++) {
		if (Tcl_GetInt(interp, str[i], &val) != TCL_OK)
			return (0);
		data[i] = val;
	}

	Tcl_Ckfree((char *)str);
	event = new SystemExclusiveEvent(time, data, len);
	if (argc == 3)
		event->SetContinued(1);
	delete data;

	return (event);
}

Event *
Tclm_ParseMetaSequenceNumber(Tcl_Interp *interp, long time, int argc,
    char **argv)
{
	long num;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaSequenceNumber number\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		num = MetaSequenceNumberEvent::WC_NUMBER;
	else if (Tcl_GetLong(interp, argv[1], &num) != TCL_OK)
		return (0);

	return (new MetaSequenceNumberEvent(time, num));
}

Event *
Tclm_ParseMetaText(Tcl_Interp *interp, long time, int argc, char **argv)
{
	const char *t;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaText "
		    "string\"", TCL_STATIC);
		return (0);
	}
	if (strcmp(argv[1], "*") == 0)
		t = MetaTextEvent::WC_STRING;
	else
		t = argv[1];

	return (new MetaTextEvent(time, t));
}

Event *
Tclm_ParseMetaCopyright(Tcl_Interp *interp, long time, int argc, char **argv)
{
	const char *t;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaCopyright string\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		t = MetaCopyrightEvent::WC_STRING;
	else
		t = argv[1];

	return (new MetaCopyrightEvent(time, t));
}

Event *
Tclm_ParseMetaSequenceName(Tcl_Interp *interp, long time, int argc,
    char **argv)
{
	const char *t;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaSequenceName string\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		t = MetaSequenceNameEvent::WC_STRING;
	else
		t = argv[1];

	return (new MetaSequenceNameEvent(time, t));
}

Event *
Tclm_ParseMetaInstrumentName(Tcl_Interp *interp, long time, int argc,
    char **argv)
{
	const char *t;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaInstrumentName string\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		t = MetaInstrumentNameEvent::WC_STRING;
	else
		t = argv[1];

	return (new MetaInstrumentNameEvent(time, t));
}

Event *
Tclm_ParseMetaLyric(Tcl_Interp *interp, long time, int argc, char **argv)
{
	const char *t;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaLyric "
		    "string\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		t = MetaLyricEvent::WC_STRING;
	else
		t = argv[1];

	return (new MetaLyricEvent(time, t));
}

Event *
Tclm_ParseMetaMarker(Tcl_Interp *interp, long time, int argc, char **argv)
{
	const char *t;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaMarker "
		    "string\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		t = MetaMarkerEvent::WC_STRING;
	else
		t = argv[1];

	return (new MetaMarkerEvent(time, t));
}

Event *
Tclm_ParseMetaCue(Tcl_Interp *interp, long time, int argc, char **argv)
{
	const char *t;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaCue "
		    "string\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		t = MetaCueEvent::WC_STRING;
	else
		t = argv[1];

	return (new MetaCueEvent(time, t));
}

Event *
Tclm_ParseMetaChannelPrefix(Tcl_Interp *interp, long time, int argc,
    char **argv)
{
	char **str;
	MetaChannelPrefixEvent *event;
	unsigned char *data;
	long len;
	int i, val;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaChannelPrefix {data ?data ...?}\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0) {
		data = (unsigned char *)MetaChannelPrefixEvent::WC_DATA;
		len = MetaChannelPrefixEvent::WC_LENGTH;
		return (new MetaChannelPrefixEvent(time, data, len));
	}
	if (Tcl_SplitList(interp, argv[1], (int *)&len, &str) != TCL_OK)
		return (0);

	data = new unsigned char[len];
	if (data == 0)
		return (0);

	for (i = 0; i < len; i++) {
		if (Tcl_GetInt(interp, str[i], &val) != TCL_OK)
			return (0);
		data[i] = val;
	}

	Tcl_Ckfree((char *)str);
	event = new MetaChannelPrefixEvent(time, data, len);
	delete data;

	return (event);
}

Event *
Tclm_ParseMetaPortNumber(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int port;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaPortNumber port\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		port = MetaPortNumberEvent::WC_PORT;
	else if (Tcl_GetInt(interp, argv[1], &port) != TCL_OK)
		return (0);

	return (new MetaPortNumberEvent(time, port));
}

Event *
Tclm_ParseMetaEndOfTrack(Tcl_Interp *interp, long time, int argc, char **argv)
{
	char *dummy;

	// shut up a warning
	dummy = argv[0];

	if (argc != 1) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaEndOfTrack\"", TCL_STATIC);
		return (0);
	}

	return (new MetaEndOfTrackEvent(time));
}

Event *
Tclm_ParseMetaTempo(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int tempo;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaTempo "
		    "tempo\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		tempo = MetaTempoEvent::WC_TEMPO;
	else if (Tcl_GetInt(interp, argv[1], &tempo) != TCL_OK)
		return (0);

	return (new MetaTempoEvent(time, tempo));
}

Event *
Tclm_ParseMetaSMPTE(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int hour, minute, second, frame, fractional_frame;

	if (argc != 6) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaSMPTE "
		    "hour minute second frame fractional_frame\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		hour = MetaSMPTEEvent::WC_HOUR;
	else if (Tcl_GetInt(interp, argv[1], &hour) != TCL_OK)
		return (0);
	if (strcmp(argv[2], "*") == 0)
		minute = MetaSMPTEEvent::WC_MINUTE;
	else if (Tcl_GetInt(interp, argv[2], &minute) != TCL_OK)
		return (0);
	if (strcmp(argv[3], "*") == 0)
		second = MetaSMPTEEvent::WC_SECOND;
	else if (Tcl_GetInt(interp, argv[3], &second) != TCL_OK)
		return (0);
	if (strcmp(argv[4], "*") == 0)
		frame = MetaSMPTEEvent::WC_FRAME;
	else if (Tcl_GetInt(interp, argv[4], &frame) != TCL_OK)
		return (0);
	if (strcmp(argv[5], "*") == 0)
		fractional_frame = MetaSMPTEEvent::WC_FRACTIONAL_FRAME;
	else if (Tcl_GetInt(interp, argv[5], &fractional_frame) != TCL_OK)
		return (0);

	return (new MetaSMPTEEvent(time, hour, minute, second, frame,
	    fractional_frame));
}

Event *
Tclm_ParseMetaTime(Tcl_Interp *interp, long time, int argc, char **argv)
{
	int numerator, denominator, clocks, thirty_seconds;

	if (argc != 5) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaTime "
		    "numerator denominator clocks/beat 32nds/quarter\"",
		    TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		numerator = MetaTimeEvent::WC_NUMERATOR;
	else if (Tcl_GetInt(interp, argv[1], &numerator) != TCL_OK)
		return (0);
	if (strcmp(argv[2], "*") == 0)
		denominator = MetaTimeEvent::WC_DENOMINATOR;
	else if (Tcl_GetInt(interp, argv[2], &denominator) != TCL_OK)
		return (0);
	if (strcmp(argv[3], "*") == 0)
		clocks = MetaTimeEvent::WC_CLOCKS_PER_BEAT;
	else if (Tcl_GetInt(interp, argv[3], &clocks) != TCL_OK)
		return (0);
	if (strcmp(argv[4], "*") == 0)
		thirty_seconds = MetaTimeEvent::WC_32ND_NOTES_PER_QUARTER_NOTE;
	else if (Tcl_GetInt(interp, argv[4], &thirty_seconds) != TCL_OK)
		return (0);

	return (new MetaTimeEvent(time, numerator, denominator, clocks,
	    thirty_seconds));
}

Event *
Tclm_ParseMetaKey(Tcl_Interp *interp, long time, int argc, char **argv)
{
	Key key;
	Mode mode;
	int match;

	if (argc != 3) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaKey "
		    "key mode\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		key = MetaKeyEvent::WC_KEY;
	else {
		key = StrToKey(argv[1], &match);
		if (!match) {
			Tcl_AppendResult(interp, "bad key: ", argv[1], 0);
			return (0);
		}
	}
	if (strcmp(argv[2], "*") == 0)
		mode = MetaKeyEvent::WC_MODE;
	else {
		mode = StrToMode(argv[2], &match);
		if (!match) {
			Tcl_AppendResult(interp, "bad mode: ", argv[2], 0);
			return (0);
		}
	}

	return (new MetaKeyEvent(time, key, mode));
}

Event *
Tclm_ParseMetaSequencerSpecific(Tcl_Interp *interp, long time, int argc,
    char **argv)
{
	char **str;
	MetaSequencerSpecificEvent *event;
	unsigned char *data;
	long len;
	int i, val;

	if (argc != 2) {
		Tcl_SetResult(interp, "bad event: should be "
		    "\"time MetaSequencerSpecific {data ?data ...?}\"",
		    TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0) {
		data = (unsigned char *)MetaSequencerSpecificEvent::WC_DATA;
		len = MetaSequencerSpecificEvent::WC_LENGTH;
		return (new MetaSequencerSpecificEvent(time, data, len));
	}
	if (Tcl_SplitList(interp, argv[1], (int *)&len, &str) != TCL_OK)
		return (0);

	data = new unsigned char[len];
	if (data == 0)
		return (0);

	for (i = 0; i < len; i++) {
		if (Tcl_GetInt(interp, str[i], &val) != TCL_OK)
			return (0);
		data[i] = val;
	}

	Tcl_Ckfree((char *)str);
	event = new MetaSequencerSpecificEvent(time, data, len);
	delete data;

	return (event);
}

Event *
Tclm_ParseMetaUnknown(Tcl_Interp *interp, long time, int argc, char **argv)
{
	char **str;
	MetaUnknownEvent *event;
	unsigned char *data;
	long len;
	int i, type, val;

	if (argc != 3) {
		Tcl_SetResult(interp, "bad event: should be \"time MetaUnknown "
		    "type {data ?data ...?}\"", TCL_STATIC);
		return (0);
	}

	if (strcmp(argv[1], "*") == 0)
		type = MetaUnknownEvent::WC_META_TYPE;
	else if (Tcl_GetInt(interp, argv[1], &type) != TCL_OK)
		return (0);

	if (strcmp(argv[2], "*") == 0) {
		data = (unsigned char *)MetaUnknownEvent::WC_DATA;
		len = MetaUnknownEvent::WC_LENGTH;
		return (new MetaUnknownEvent(time, data, len, type));
	}
	if (Tcl_SplitList(interp, argv[2], (int *)&len, &str) != TCL_OK)
		return (0);

	data = new unsigned char[len];
	if (data == 0)
		return (0);

	for (i = 0; i < len; i++) {
		if (Tcl_GetInt(interp, str[i], &val) != TCL_OK)
			return (0);
		data[i] = val;
	}

	Tcl_Ckfree((char *)str);
	event = new MetaUnknownEvent(time, data, len, type);
	delete data;

	return (event);
}

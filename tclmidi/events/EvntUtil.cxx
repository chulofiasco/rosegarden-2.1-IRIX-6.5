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
#include "EvntUtil.h"
#include "AllEvent.h"

Event *
ReadEventFromSMFTrack(SMFTrack &track, unsigned long &last_t,
    int use_time, const char *&errstr)
{
	long t;
	Event *event;
	const unsigned char *ptr;
	unsigned char state;

	errstr = 0;
	event = 0;

	if (!use_time) {
		t = 0;
		last_t = 0;
	} else {
		if ((t = track.GetVarValue()) == -1)
			return(0);

		t += last_t;
		last_t = t;
	}

	// try to determine event type
	if ((ptr = track.PeekByte()) == 0) {
		errstr = "Incomplete event";
		return (0);
	}

	if (*ptr & 0x80) {
		// It's a new state - go ahead a get it
		state = *track.GetByte();
		// and change running state
		track.SetRunningState(state);
	} else {
		// we're using the running state
		state = track.GetRunningState();
		if (!(state & 0x80)) {
			errstr = "Making use of invalid running state";
			return (0);
		}
	}

	if (state == 0xf0) {
		// reset running state
		track.SetRunningState(0);
		event = new SystemExclusiveEvent(0);
		if (event == 0) {	
			errstr = "Out of memory";
			return (0);
		}
	} else if (state == 0xf7) {
		// reset running state
		track.SetRunningState(0);
		event = new SystemExclusiveEvent(1);
		if (event == 0) {
			errstr = "Out of memory";
			return (0);
		}
	} else if (state == 0xff) {
		// reset running state
		track.SetRunningState(0);
		// meta event - determined by next byte
		if ((ptr = track.GetByte()) == 0) {
			errstr = "Incomplete Meta event";
			return (0);
		}
		switch (*ptr) {
		case 0x00:
			event = new MetaSequenceNumberEvent();
			break;
		case 0x01: case 0x08: case 0x09: case 0x0a:
		case 0x0b: case 0x0c: case 0x0d: case 0x0e:
		case 0x0f:
			event = new MetaTextEvent();
			break;
		case 0x02:
			event = new MetaCopyrightEvent();
			break;
		case 0x03:
			event = new MetaSequenceNameEvent();
			break;
		case 0x04:
			event = new MetaInstrumentNameEvent();
			break;
		case 0x05:
			event = new MetaLyricEvent();
			break;
		case 0x06:
			event = new MetaMarkerEvent();
			break;
		case 0x07:
			event = new MetaCueEvent();
			break;
		case 0x20:
			event = new MetaChannelPrefixEvent();
			break;
		case 0x21:
			event = new MetaPortNumberEvent();
			break;
		case 0x2f:
			event = new MetaEndOfTrackEvent();
			break;
		case 0x51:
			event = new MetaTempoEvent();
			break;
		case 0x54:
			event = new MetaSMPTEEvent();
			break;
		case 0x58:
			event = new MetaTimeEvent();
			break;
		case 0x59:
			event = new MetaKeyEvent();
			break;
		case 0x7f:
			event = new
			    MetaSequencerSpecificEvent();
			break;
		default:
			event = new MetaUnknownEvent(*ptr);
			break;
		}
		if (event == 0) {
			errstr = "Out of memory";
			return (0);
		}
	} else {
		switch (state & 0xf0) {
		case 0x80:
			event = new NoteOffEvent();
			break;
		case 0x90:
			event = new NoteOnEvent();
			break;
		case 0xa0:
			event = new KeyPressureEvent();
			break;
		case 0xb0:
			event = new ParameterEvent();
			break;
		case 0xc0:
			event = new ProgramEvent();
			break;
		case 0xd0:
			event = new ChannelPressureEvent();
			break;
		case 0xe0:
			event = new PitchWheelEvent();
			break;
		}
		if (event == 0) {
			errstr = "Out of memory";
			return (0);
		}
		((NormalEvent *)event)->SetChannel(state & 0x0f);
	}

	// Now event points to the correct event
	// Set the time
	event->SetTime(t);
	// Now read the event
	if ((errstr = event->SMFRead(track)) == 0)
		return (event);
	else {
		delete event;
		return (0);
	}
}

int
WriteEventToSMFTrack(SMFTrack &track, unsigned long &last_t,
    const Event *event, int use_time, const char *&errstr)
{
	EventType type;
	long t;

	errstr = 0;

	if (use_time) {
		t = event->GetTime() - last_t;
		last_t = event->GetTime();

		if (!track.PutFixValue(t)) {
			errstr = "Out of memory";
			return (0);
		}
	}

	type = event->GetType();
	if (type == SYSTEMEXCLUSIVE) {
		if (((SystemExclusiveEvent *)event)->GetContinued() == 1) {
			if (!track.PutByte(0xf7)) {
				errstr = "Out of memory";
				return (0);
			}
		} else {
			if (!track.PutByte(0xf0)) {
				errstr = "Out of memory";
				return (0);
			}
		}
		// clear running state
		track.SetRunningState(0);
	} else if (type >= NOTEOFF && type <= PITCHWHEEL) {
		unsigned char state;

		state = 0x80 + (type - NOTEOFF) * 0x10 +
		    ((NormalEvent *)event)->GetChannel();

		if (state != track.GetRunningState()) {
			if (!track.PutByte(state)) {
				errstr = "Out of memory";
				return (0);
			}
			track.SetRunningState(state);
		}
	} else {
		// meta events
		unsigned char metatype;

		if (!track.PutByte(0xff)) {
			errstr = "Out of memory";
			return (0);
		}
		if (type >= METATEXT && type <= METACUE)
			metatype = 0x01 + (type - METATEXT);
		else {
			switch (type) {
			case METASEQUENCENUMBER:
				metatype = 0x00;
				break;
			case METACHANNELPREFIX:
				metatype = 0x20;
				break;
			case METAPORTNUMBER:
				metatype = 0x21;
				break;
			case METAENDOFTRACK:
				metatype = 0x2f;
				break;
			case METATEMPO:
				metatype = 0x51;
				break;
			case METASMPTE:
				metatype = 0x54;
				break;
			case METATIME:
				metatype = 0x58;
				break;
			case METAKEY:
				metatype = 0x59;
				break;
			case METASEQUENCERSPECIFIC:
				metatype = 0x7f;
				break;
			case METAUNKNOWN:
				metatype =
				    ((MetaUnknownEvent *)event)->GetMetaType();
				break;
			default:
				/* shut up warning */
				metatype = 0x00;
				break;
			}
		}
		if (!track.PutByte(metatype)) {
			errstr = "Out of memory";
			return (0);
		}
		// clear running state
		track.SetRunningState(0);
	}

	if ((errstr = event->SMFWrite(track)) == 0)
		return (1);
	else
		return (0);
}

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
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <ctype.h>
#include "tclmidi.h"
#include "TclmIntp.h"
#include "tclmEvnt.h"
#include "Song.h"
#ifdef HAVE_SYS_MIDIIOCTL_H
#include "TclmDr75.h"
#endif

#if defined(HAVE_SYS_MIDIIOCTL_H)
static const int DeviceConfigured = 1;
#else
static const int DeviceConfigured = 0;
#endif

static int Tclm_MidiPlay(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiRecord(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiStop(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiWait(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiDevice(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiTime(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiFeature(ClientData client_data, Tcl_Interp *interp,
    int argc, char **argv);
static int Tclm_MidiSend(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiRecv(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);

int
Tclm_PlayInit(Tcl_Interp *interp, TclmInterp *tclm_interp)
{

	Tcl_CreateCommand(interp, "midiplay", Tclm_MidiPlay, tclm_interp, 0);
	Tcl_CreateCommand(interp, "midirecord", Tclm_MidiRecord,
	    tclm_interp, 0);
	Tcl_CreateCommand(interp, "midistop", Tclm_MidiStop, tclm_interp, 0);
	Tcl_CreateCommand(interp, "midiwait", Tclm_MidiWait, tclm_interp, 0);
	Tcl_CreateCommand(interp, "mididevice", Tclm_MidiDevice,
	    tclm_interp, 0);
	Tcl_CreateCommand(interp, "miditime", Tclm_MidiTime, tclm_interp, 0);
	Tcl_CreateCommand(interp, "midifeature", Tclm_MidiFeature,
	    tclm_interp, 0);
	Tcl_CreateCommand(interp, "midisend", Tclm_MidiSend, tclm_interp, 0);
	Tcl_CreateCommand(interp, "midirecv", Tclm_MidiRecv, tclm_interp, 0);
	return (TCL_OK);
}

int
Tclm_MidiPlay(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *song;
	MidiDevice *dev;
	int repeat;

	if (argc != 3 && argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID MidiID ?repeat?\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}

	if ((song = tclm_interp->GetSong(argv[2])) == 0) {
		Tcl_AppendResult(interp, "bad SongID ", argv[2], 0);
		return (TCL_ERROR);
	}

	repeat = 0;
	if (argc == 4 && strlen(argv[3]) != 0) {
		if (strcmp(argv[3], "repeat") == 0)
			repeat = 1;
		else {
			Tcl_AppendResult(interp, "bad repeat option: should "
			    "be \"", argv[0], " DevID MidiID ?repeat?\"", 0);
			return (TCL_ERROR);
		}
	}

	if (!dev->Play(song, repeat)) {
		Tcl_AppendResult(interp, "couldn't play song \n",
		    dev->GetError(), 0);
		return (TCL_ERROR);
	}
	Tcl_SetResult(interp, "1", TCL_STATIC);
	return (TCL_OK);
}

int
Tclm_MidiRecord(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *rsong, *psong;
	MidiDevice *dev;
	int repeat;

	if (argc < 3 || argc > 5) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID RecMidiID ?PlayMidiID ?repeat??\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}

	if ((rsong = tclm_interp->GetSong(argv[2])) == 0) {
		Tcl_AppendResult(interp, "bad SongID ", argv[2], 0);
		return (TCL_ERROR);
	}

	psong = 0;
	repeat = 0;
	if (argc > 3) {
		if ((psong = tclm_interp->GetSong(argv[3])) == 0) {
			Tcl_AppendResult(interp, "bad SongID ", argv[3], 0);
			return (TCL_ERROR);
		}

		if (argc == 5 && strlen(argv[4]) != 0) {
			if (strcmp(argv[4], "repeat") == 0)
				repeat = 1;
			else {
				Tcl_AppendResult(interp, "bad repeat flag: ",
				    argv[4], 0);
				return (TCL_ERROR);
			}
		}
	}

	if (!dev->Record(rsong, psong, repeat)) {
		Tcl_AppendResult(interp, "Couldn't record song\n",
		    dev->GetError(), 0);
		return (TCL_ERROR);
	}
	Tcl_SetResult(interp, "1", TCL_STATIC);
	return (TCL_OK);
}

int
Tclm_MidiStop(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	MidiDevice *dev;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}

	if (!dev->Stop()) {
		Tcl_AppendResult(interp, "Couldn't stop playing/recording\n",
		    dev->GetError(), 0);
		return (TCL_ERROR);
	}
	Tcl_SetResult(interp, "1", TCL_STATIC);
	return (TCL_OK);
}

int
Tclm_MidiWait(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	MidiDevice *dev;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;
	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}
	if (!dev->Wait()) {
		Tcl_AppendResult(interp, "Couldn't wait for playing/recording "
		    "to stop\n", dev->GetError(), 0);
		return (TCL_ERROR);
	}
	Tcl_SetResult(interp, "1", TCL_STATIC);
	return (TCL_OK);
}

int
Tclm_MidiDevice(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	MidiDevice *dev;
	ostrstream *buf;
	unsigned short mask;
	char *dev_key, *str, **sub_argv;
	int con, i, imask, j, mt, sub_argc, value;

	tclm_interp = (TclmInterp *)client_data;

	/*
	 * mididevice with no args means just check for device
	 * support.
	 */
	if (argc == 1) {
		buf = new ostrstream;
		*buf << DeviceConfigured << ends;
		str = buf->str();
		Tcl_SetResult(interp, str, TCL_VOLATILE);
		delete buf;
		delete str;
		return (TCL_OK);
	}

	// Either open or get device
	if (strncmp(argv[1], "device", strlen("device")) == 0) {
		// get an existing device
		dev = tclm_interp->GetDevice(argv[1]);
		if (dev == 0) {
			Tcl_AppendResult(interp, "Bad device id \"",
			    argv[1], "\"", 0);
			return (TCL_ERROR);
		}
		dev_key = argv[1];
	} else {
#ifdef HAVE_SYS_MIDIIOCTL_H
		dev = new TclmDriver75;
		if (dev != 0)
			((TclmDriver75 *)dev)->SetInterp(interp);
		if (dev != 0)
			((TclmDriver75 *)dev)->SetTclmInterp(tclm_interp);
#else
		dev = 0;
#endif
		if (dev == 0) {
			Tcl_SetResult(interp, "Out of memory", TCL_STATIC);
			return (TCL_ERROR);
		}
		if (!dev->Open(argv[1])) {
			Tcl_AppendResult(interp, "Couldn't open ",
			    argv[1], ": ", dev->GetError(), 0);
			return (TCL_ERROR);
		}
		dev_key = tclm_interp->AddDevice(dev);
		if (dev_key == 0) {
			Tcl_SetResult(interp, "Couldn't add device",
			    TCL_STATIC);
			return (TCL_ERROR);
		}
		Tcl_AppendResult(interp, dev_key, 0);
	}

	for (i = 2; i < argc; i++) {
		// loop through each arg and either set or return values
		if (Tcl_SplitList(interp, argv[i], &sub_argc, &sub_argv)
		    != TCL_OK)
			return (TCL_ERROR);
		if (strcmp(sub_argv[0], "close") == 0) {
			if (i != argc - 1) {
				Tcl_SetResult(interp, "Can't perform ops after "
				    "closing device", TCL_STATIC);
				return (TCL_ERROR);
			}
			if (dev_key == 0) {
				Tcl_SetResult(interp, "Can't close until after "
				    "device has been opened", TCL_STATIC);
				return (TCL_ERROR);
			}
			tclm_interp->DeleteDevice(dev_key);
		} else if (strcmp(sub_argv[0], "midithru") == 0) {
			switch (sub_argc) {
			case 1:
				/* return the value */
				buf = new ostrstream;
				mt = dev->GetMidiThru();
				if (mt == -1) {
					Tcl_SetResult(interp,
					    (char *)dev->GetError(),
					    TCL_VOLATILE);
					return (TCL_ERROR);
				}
				*buf << "midithru " << (mt ? "on" : "off")
				    << ends;
				str = buf->str();
				Tcl_AppendElement(interp, str);
				delete str;
				delete buf;
				break;
			case 2:
				/* set the value */
				if (Tcl_GetBoolean(interp, sub_argv[1], &value)
				    != TCL_OK)
					return (TCL_ERROR);
				if (!dev->SetMidiThru(value)) {
					Tcl_SetResult(interp,
					    (char *)dev->GetError(),
					    TCL_VOLATILE);
					return (TCL_ERROR);
				}
				break;
			default:
				/* wrong args */
				Tcl_AppendResult(interp, "Wrong number of "
				    "args: should be \"", argv[0], " DevID "
				     "midithru ?on|off?\"", 0);
				return (TCL_ERROR);
			}
		} else if (strcmp(sub_argv[0], "slave") == 0) {
			MidiDevice *master;
			if (sub_argc != 2) {
				Tcl_AppendResult(interp, "Wrong number of "
				    "args: should be \"", argv[0], " DevID "
				    "slave DevID\"", 0);
				return (TCL_ERROR);
			}
			master = tclm_interp->GetDevice(sub_argv[1]);
			if (master == 0) {
				Tcl_AppendResult(interp, "Bad Master ID \"",
				    sub_argv[1], "\"", 0);
				return (TCL_ERROR);
			}
			if (!dev->Slave(*master)) {
				Tcl_AppendResult(interp, "Couldn't make "
				    "slave: ", dev->GetError(), 0);
				return (TCL_ERROR);
			}
		} else if (strcmp(sub_argv[0], "channel_on") == 0 ||
		    strcmp(sub_argv[0], "channel_off") == 0) {
			if (strcmp(sub_argv[0], "channel_on") == 0)
				con = 1;
			else
				con = 0;

			switch (sub_argc) {
			case 1:
				/* return the value */
				buf = new ostrstream;

				imask = dev->GetChannelMask();
				if (imask == -1) {
					Tcl_SetResult(interp,
					    (char *)dev->GetError(),
					    TCL_VOLATILE);
					return (TCL_ERROR);
				}
				mask = imask;
				if (con)
					*buf << "channel_on ";
				else
					*buf << "channel_off ";
				for (j = 0; j < 16; j++) {
					if (!(mask & (1 << j))) {
						if (con)
							*buf << " " << j;
					} else {
						if (!con)
							*buf << " " << j;
					}
				}
				*buf << ends;
				str = buf->str();
				Tcl_AppendElement(interp, str);
				delete str;
				delete buf;
				break;
			default:
				/* get the current mask */
				imask = dev->GetChannelMask();
				if (imask == -1) {
					Tcl_SetResult(interp,
					    (char *)dev->GetError(),
					    TCL_VOLATILE);
					return (TCL_ERROR);
				}
				mask = imask;

				/* set the value */
				for (j = 1; j < sub_argc; j++) {
					if (Tcl_GetInt(interp, sub_argv[j],
					    &value) != TCL_OK)
						return (TCL_ERROR);
					if (con) {
						mask &= ~(1 << value);
					} else {
						mask |= (1 << value);
					}
				}
				if (!dev->SetChannelMask(mask)) {
					Tcl_SetResult(interp,
					    (char *)dev->GetError(),
					    TCL_VOLATILE);
					return (TCL_ERROR);
				}
				break;
			}
		} else {
			Tcl_AppendResult(interp, "Bad parameter \"",
			    sub_argv[0], "\"", 0);
			return (TCL_ERROR);
		}
		Tcl_Ckfree((char *)sub_argv);
	}
	return (TCL_OK);
}

int
Tclm_MidiTime(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	ostrstream tbuf;
	unsigned long t;
	TclmInterp *tclm_interp;
	MidiDevice *dev;
	char *str;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;
	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}
	if (!dev->GetTime(&t)) {
		Tcl_AppendResult(interp, "Couldn't get time: ",
		    dev->GetError() , 0);
		return (TCL_ERROR);
	}
	tbuf << t << ends;
	str = tbuf.str();
	Tcl_SetResult(interp, str, TCL_VOLATILE);
	delete str;
	return (TCL_OK);
}

int
Tclm_MidiFeature(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	MidiDevice *dev;
	char **sub_argv;
	int i, sub_argc;

	if (argc < 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID ?kernel_timing? ?smpte_timing? "
		    "?mpu401_timing? ?get_smpte?\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;
	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}

	for (i = 2; i < argc; i++) {
		ostrstream *res;
		char *res_str;

		// loop through each arg and either set or return values
		if (Tcl_SplitList(interp, argv[i], &sub_argc, &sub_argv)
		    != TCL_OK)
			return (TCL_ERROR);
		if ((res = dev->Feature(sub_argv[0],
		    (const char **)&sub_argv[1], sub_argc - 1)) == 0) {
			Tcl_AppendResult(interp, "Feature \"", sub_argv[0],
			    "\" failed: ", dev->GetError(), 0);
			return (TCL_ERROR);
		}
		res_str = res->str();
		Tcl_AppendResult(interp, res_str, 0);
		delete res_str;
		delete res;
	}
	return (TCL_OK);
}

int
Tclm_MidiSend(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	MidiDevice *dev;
	Event **events;
	int i, num_events;

	if (argc < 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID event ?event ...?\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}

	events = new Event *[argc - 2];
	if (events == 0) {
		Tcl_SetResult(interp, "Out of memory", TCL_STATIC);
		return (TCL_ERROR);
	}
	num_events = 0;
	for (i = 2; i < argc; i++) {
		events[num_events] = Tclm_ParseEvent(interp, argv[i]);
		if (events[num_events] != 0) {
			if (events[num_events]->GetType() == NOTEON &&
			    ((NoteEvent *)events[num_events])->GetNotePair()
			    != 0) {
				Tcl_AppendResult(interp, "Can't send Note "
				    "events, use separate NoteOn/NoteOff "
				    "events", 0);
				delete ((NoteEvent *)events[num_events])
				    ->GetNotePair();
				delete events[num_events];
			} else {
				num_events++;
			}
		} else {
			if (strlen(interp->result) == 0) {
				Tcl_SetResult(interp, "Out of memory",
				    TCL_STATIC);
				return (TCL_ERROR);
			}
		}
	}
	if (!dev->Send(events, num_events)) {
		Tcl_SetResult(interp, (char *)dev->GetError(), TCL_VOLATILE);
		return (TCL_ERROR);
	}
	for (i = 0; i < num_events; i++)
		delete events[i];
	delete events;
	return (TCL_OK);
}

int
Tclm_MidiRecv(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	ostrstream *buf;
	TclmInterp *tclm_interp;
	MidiDevice *dev;
	Event **events;
	int i, num_events;
	char *str;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " DevID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	dev = tclm_interp->GetDevice(argv[1]);
	if (dev == 0) {
		Tcl_AppendResult(interp, "Bad DevID \"", argv[1], "\"", 0);
		return (TCL_ERROR);
	}

	if (!dev->Recv(&events, &num_events)) {
		Tcl_SetResult(interp, (char *)dev->GetError(), TCL_VOLATILE);
		return (TCL_ERROR);
	}

	for (i = 0; i < num_events; i++) {
		buf = new ostrstream;
		Tclm_PrintEvent(*buf, events[i]);
		str = buf->str();
		if (str != 0 && str[0] != '\0')
			Tcl_AppendElement(interp, str);
		delete str;
		delete buf;
		delete events[i];
	}
	delete events;
	return (TCL_OK);
}

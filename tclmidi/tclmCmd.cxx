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
#include <string.h>
#include <iostream.h>
#include "tclmidi.h"
#include "TclmIntp.h"
#include "Song.h"
#include "tclmEvnt.h"
#include "patchlvl.h"

static int Tclm_MidiMake(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiFree(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiRead(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiWrite(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiConfig(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiRewind(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiGet(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiPut(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiDelete(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiMerge(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiSplit(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiCopy(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_MidiVersion(ClientData client_data, Tcl_Interp *interp,
    int argc, char **argv);
static int Tclm_MidiTrack(ClientData client_data, Tcl_Interp *interp,
    int argc, char **argv);
static int Tclm_MidiGrep(ClientData client_data, Tcl_Interp *interp,
    int argc, char **argv);
static int Tclm_GetTrack(TclmInterp *tclm_interp, Tcl_Interp *interp,
    const char *str, Song **song, int *track);


/*
 * DllEntryPoint --
 *
 *	This wrapper function is used by Windows to invoke the
 *	initialization code for the Dll.  If we are compiling
 *	with Visual C++, this routine will be renamed to DllMain.
 *
 */
#ifdef __WIN32__
BOOL APIENTRY
DllEntryPoint(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{

	return TRUE;
}
#endif

EXPORT(int,Tclmidi_Init)(Tcl_Interp *interp)
{
	TclmInterp *ti;

	ti = new TclmInterp;
	if (ti == 0) {
		Tcl_SetResult(interp, "Out of memory in Tclmidi_Init",
		    TCL_STATIC);
		return (TCL_ERROR);
	}
	Tcl_CreateCommand(interp, "midimake", Tclm_MidiMake, ti, 0);
	Tcl_CreateCommand(interp, "midifree", Tclm_MidiFree, ti, 0);
	Tcl_CreateCommand(interp, "midiread", Tclm_MidiRead, ti, 0);
	Tcl_CreateCommand(interp, "midiwrite", Tclm_MidiWrite, ti, 0);
	Tcl_CreateCommand(interp, "midiconfig", Tclm_MidiConfig, ti, 0);
	Tcl_CreateCommand(interp, "midirewind", Tclm_MidiRewind, ti, 0);
	Tcl_CreateCommand(interp, "midiget", Tclm_MidiGet, ti, 0);
	Tcl_CreateCommand(interp, "midiput", Tclm_MidiPut, ti, 0);
	Tcl_CreateCommand(interp, "mididelete", Tclm_MidiDelete, ti, 0);
	Tcl_CreateCommand(interp, "midimerge", Tclm_MidiMerge, ti, 0);
	Tcl_CreateCommand(interp, "midisplit", Tclm_MidiSplit, ti, 0);
	Tcl_CreateCommand(interp, "midimove", Tclm_MidiCopy, ti, 0);
	Tcl_CreateCommand(interp, "midicopy", Tclm_MidiCopy, ti, 0);
	Tcl_CreateCommand(interp, "midiversion", Tclm_MidiVersion, ti, 0);
	Tcl_CreateCommand(interp, "miditrack", Tclm_MidiTrack, ti, 0);
	Tcl_CreateCommand(interp, "midigrep", Tclm_MidiGrep, ti, 0);

	if (Tclm_PatchInit(interp, ti) != TCL_OK)
		return (TCL_ERROR);

	if (Tclm_PlayInit(interp, ti) != TCL_OK)
		return (TCL_ERROR);
	return (Tcl_PkgProvide(interp, "tclmidi", (char *)TCLMIDI_NUM_VERSION));
}

int
Tclm_MidiMake(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *s;
	char *key;

	if (argc != 1) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], "\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	s = new Song;
	key = tclm_interp->AddSong(s);
	Tcl_SetResult(interp, key, TCL_VOLATILE);
	delete key;
	return (TCL_OK);
}

int
Tclm_MidiFree(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if (!tclm_interp->DeleteSong(argv[1])) {
		Tcl_AppendResult(interp, "Bad key ", argv[1], 0);
		return (TCL_ERROR);
	}
	return (TCL_OK);
}

int
Tclm_MidiRead(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	Tcl_Channel channel;
	int mode;
	TclmInterp *tclm_interp;
	Song *song;
	char *key;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " FileID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	song = new Song;

	channel = Tcl_GetChannel(interp, argv[1], &mode);
	if (channel == 0)
		return (TCL_ERROR);
	if ((mode & TCL_READABLE) == 0) {
		Tcl_AppendResult(interp, "channel \"", argv[1],
		    "\" wasn't opened for reading", 0);
		return (TCL_ERROR);
	}
	Tcl_SetChannelOption(interp, channel, "-translation", "binary");
	if (!song->SMFRead(channel)) {
		Tcl_AppendResult(interp, "couldn't read MIDI file ", argv[1],
		    ": ", song->GetError(), 0);
		delete song;
		return (TCL_ERROR);
	}
	key = tclm_interp->AddSong(song);
	Tcl_SetResult(interp, key, TCL_VOLATILE);
	delete key;
	return (TCL_OK);
}

int
Tclm_MidiWrite(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *song;
	Tcl_Channel channel;
	int mode;

	if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " FileID MidiID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[2])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[2], 0);
		return (TCL_ERROR);
	}

	channel = Tcl_GetChannel(interp, argv[1], &mode);
	if (channel == 0)
		return (TCL_ERROR);
	if ((mode & TCL_WRITABLE) == 0) {
		Tcl_AppendResult(interp, "channel \"", argv[1],
		    "\" wasn't opened for writing", 0);
		return (TCL_ERROR);
	}
	Tcl_SetChannelOption(interp, channel, "-translation", "binary");
	if (!song->SMFWrite(channel)) {
		Tcl_AppendResult(interp, "couldn't write ", argv[2],
		    ": ", song->GetError(), 0);
		delete song;
		return (TCL_ERROR);
	}
	return (TCL_OK);
}

int
Tclm_MidiConfig(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	ostrstream *buf;
	TclmInterp *tclm_interp;
	Song *song;
	char *str, **sub_argv;
	int i, sub_argc, value;

	if (argc < 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID ?{format|division|tracks ?value?} ...?\"",
		    0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[1])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[1], 0);
		return (TCL_ERROR);
	}

	if (argc == 2) {
		// make list of all values
		buf = new ostrstream;
		*buf << "format " << song->GetFormat() << ends;
		str = buf->str();
		Tcl_AppendElement(interp, str);
		delete str;
		delete buf;
		buf = new ostrstream;
		*buf << "division " << song->GetDivision() << ends;
		str = buf->str();
		Tcl_AppendElement(interp, str);
		delete str;
		delete buf;
		buf = new ostrstream;
		*buf << "tracks " << song->GetNumTracks() << ends;
		str = buf->str();
		Tcl_AppendElement(interp, str);
		delete str;
		delete buf;
		return (TCL_OK);
	}
	for (i = 2; i < argc; i++) {
		// loop through each arg and either set or return values
		if (Tcl_SplitList(interp, argv[i], &sub_argc, &sub_argv)
		    != TCL_OK)
			return (TCL_ERROR);
		switch (sub_argc) {
		case 1:
			// return the value
			buf = new ostrstream;
			if (strcmp(sub_argv[0], "format") == 0) {
				*buf << "format " << song->GetFormat() << ends;
			} else if (strcmp(sub_argv[0], "division") == 0) {
				*buf << "division " << song->GetDivision()
				    << ends;
			} else if (strcmp(sub_argv[0], "tracks") == 0) {
				*buf << "tracks " << song->GetNumTracks()
				    << ends;
			} else {
				Tcl_AppendResult(interp, "bad parameter ",
				    sub_argv[0], 0);
				delete buf;
				return (TCL_ERROR);
			}
			str = buf->str();
			Tcl_AppendElement(interp, str);
			delete str;
			delete buf;
			break;
		case 2:
			// set the value
			if (Tcl_GetInt(interp, sub_argv[1], &value) != TCL_OK)
				return (TCL_ERROR);
			if (strcmp(sub_argv[0], "format") == 0) {
				song->SetFormat(value);
			} else if (strcmp(sub_argv[0], "division") == 0) {
				song->SetDivision(value);
			} else if (strcmp(sub_argv[0], "tracks") == 0) {
				song->SetNumTracks(value);
			} else {
				Tcl_AppendResult(interp, "bad parameter ",
				    sub_argv[0], 0);
				return (TCL_ERROR);
			}
			break;
		default:
			Tcl_SetResult(interp, "wrong # args: should be "
			    "{format|division|tracks ?value?}", TCL_STATIC);
			return (TCL_ERROR);
			break;
		}
		Tcl_Ckfree((char *)sub_argv);
	}
	return (TCL_OK);
}

int
Tclm_MidiRewind(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *song;
	int track;

	if (argc != 3 && argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID ?track?\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[1])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[1], 0);
		return (TCL_ERROR);
	}

	if (argc == 2)
		song->RewindEvents();
	else {
		if (Tcl_GetInt(interp, argv[2], &track) != TCL_OK)
			return (TCL_ERROR);
		song->RewindEvents(track);
	}
	return (TCL_OK);
}

int
Tclm_MidiGet(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	ostrstream *buf;
	long time;
	int printable, track;
	TclmInterp *tclm_interp;
	Song *song;
	Event *e, *events;
	char *str;

	if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID track time|next|prev\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[1])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[1], 0);
		return (TCL_ERROR);
	}

	if (Tcl_GetInt(interp, argv[2], &track) != TCL_OK)
		return (TCL_ERROR);

	if (track >= song->GetNumTracks() || track < 0) {
		ostrstream buf;
		char *bstr;

		buf << "bad track value " << track << " (only " <<
		    (int)song->GetNumTracks() << " tracks in song)" << ends;
		bstr = buf.str();
		Tcl_SetResult(interp, bstr, TCL_VOLATILE);
		delete bstr;
		return (TCL_ERROR);
	}

	if (strcmp(argv[3], "next") == 0) {
		printable = 0;
		while (!printable) {
			if ((e = song->NextEvent(track)) == 0) {
				Tcl_SetResult(interp, "EOT", TCL_STATIC);
				printable = 1;
			} else {
				buf = new ostrstream;
				Tclm_PrintEvent(*buf, e);
				str = buf->str();
				if (str != 0 && str[0] != '\0') {
					Tcl_SetResult(interp, str,
					    TCL_VOLATILE);
					printable = 1;
				}
				delete str;
				delete buf;
			}
		}
	} else if (strcmp(argv[3], "prev") == 0) {
		printable = 0;
		while (!printable) {
			if ((e = song->PrevEvent(track)) == 0) {
				Tcl_SetResult(interp, "EOT", TCL_STATIC);
				printable = 1;
			} else {
				buf = new ostrstream;
				Tclm_PrintEvent(*buf, e);
				str = buf->str();
				if (str != 0 && str[0] != '\0') {
					Tcl_SetResult(interp, str,
					    TCL_VOLATILE);
					printable = 1;
				}
				delete str;
				delete buf;
			}
		}
	} else {
		if (Tcl_GetLong(interp, argv[3], &time) != TCL_OK)
			return (TCL_ERROR);
		if ((events = song->GetEvents((short)track, time)) == 0)
			Tcl_SetResult(interp, "EOT", TCL_STATIC);
		else {
			for (e = events; e != 0; e = e->GetNextEvent()) {
				buf = new ostrstream;
				Tclm_PrintEvent(*buf, e);
				str = buf->str();
				if (str != 0 && str[0] != '\0')
					Tcl_AppendElement(interp, str);
				delete str;
				delete buf;
			}
		}
	}
	return (TCL_OK);
}

int
Tclm_MidiPut(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *song;
	NoteEvent *np, *new_e2;
	Event *event, *new_e1;
	int track;

	if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID track event\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[1])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[1], 0);
		return (TCL_ERROR);
	}

	if (Tcl_GetInt(interp, argv[2], &track) != TCL_OK)
		return (TCL_ERROR);

	if (track >= song->GetNumTracks() || track < 0) {
		ostrstream buf;
		char *bstr;

		buf << "bad track value " << track << " (only " <<
		    (int)song->GetNumTracks() << " tracks in song)" << ends;
		bstr = buf.str();
		Tcl_SetResult(interp, bstr, TCL_VOLATILE);
		delete bstr;
		return (TCL_ERROR);
	}

	if ((event = Tclm_ParseEvent(interp, argv[3])) == 0) {
		if (strlen(interp->result) == 0)
			Tcl_SetResult(interp, "No more memory", TCL_STATIC);
		return (TCL_ERROR);
	}
	if (event->IsWildcard()) {
		Tcl_SetResult(interp, "Can't put wildcard events in a song",
		    TCL_STATIC);
		return (TCL_ERROR);
	}
	new_e1 = song->PutEvent(track, *event);
	if (new_e1 == 0) {
		Tcl_SetResult(interp, "Couldn't put event", TCL_STATIC);
		return (TCL_ERROR);
	}
	// check to see if it has a note off too
	np = 0;
	if (event->GetType() == NOTEON)
		np = ((NoteEvent *)event)->GetNotePair();
	if (np != 0) {
		new_e2 = (NoteEvent *)song->PutEvent(track, *np);
		if (new_e2 == 0) {
			Tcl_SetResult(interp, "Couldn't put event",
			    TCL_STATIC);
			return (TCL_ERROR);
		}
		((NoteEvent *)new_e1)->SetNotePair(new_e2);
		new_e2->SetNotePair((NoteEvent *)new_e1);
		delete np;
	}
	delete event;

	return (TCL_OK);
}

int
Tclm_MidiDelete(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *song;
	Event *event, *note_off;
	int track;

	if (argc != 4 && argc != 6) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID track {event | range starttime "
		    "endtime}\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[1])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[1], 0);
		return (TCL_ERROR);
	}

	if (Tcl_GetInt(interp, argv[2], &track) != TCL_OK)
		return (TCL_ERROR);

	if (track >= song->GetNumTracks() || track < 0) {
		ostrstream buf;
		char *bstr;

		buf << "bad track value " << track << " (only " <<
		    (int)song->GetNumTracks() << " tracks in song)" << ends;
		bstr = buf.str();
		Tcl_SetResult(interp, bstr, TCL_VOLATILE);
		delete bstr;
		return (TCL_ERROR);
	}

	if (strcmp(argv[3], "range") == 0) {
		unsigned long start, end;

		if (Tcl_GetLong(interp, argv[4], (long *)&start) != TCL_OK)
			return (TCL_ERROR);
		if (Tcl_GetLong(interp, argv[5], (long *)&end) != TCL_OK)
			return (TCL_ERROR);
		if (!song->DeleteRange(track, start, end)) {
			Tcl_SetResult(interp, "couldn't delete range",
			    TCL_STATIC);
			return (TCL_ERROR);
		}
		Tcl_SetResult(interp, "1", TCL_STATIC);
		return (TCL_OK);
	}

	if ((event = Tclm_ParseEvent(interp, argv[3])) == 0) {
		if (strlen(interp->result) == 0)
			Tcl_SetResult(interp, "No more memory", TCL_STATIC);
		return (TCL_ERROR);
	}
	// If this is a NoteOn/NoteOff pair, get real NoteOff from tree
	// and delete it
	if (event->GetType() == NOTEON && ((NoteEvent *)event)->GetNotePair()
	    != 0) {
		Event *e, *events;

		// find real event in tree
		events = song->GetEvents(track, event->GetTime());
		for (e = events; e != 0 && !(*event == *e);
		    e = e->GetNextEvent());
		if (e == 0) {
			Tcl_SetResult(interp, "0", TCL_STATIC);
			delete ((NoteEvent *)event)->GetNotePair();
			delete event;
			return (TCL_OK);
		}

		// find real NoteOff pair and delete it
		note_off = ((NoteEvent *)e)->GetNotePair();
		if (!song->DeleteEvent(track, *note_off)) {
			Tcl_SetResult(interp, "0", TCL_STATIC);
			delete ((NoteEvent *)event)->GetNotePair();
			delete event;
			return (TCL_OK);
		}
		delete ((NoteEvent *)event)->GetNotePair();
	}
	if (!song->DeleteEvent(track, *event)) {
		Tcl_SetResult(interp, "0", TCL_STATIC);
		if (event->GetType() == NOTEON &&
		   ((NoteEvent *)event)->GetNotePair() != 0)
			delete ((NoteEvent *)event)->GetNotePair();
		delete event;
		return (TCL_OK);
	}
	delete event;
	Tcl_SetResult(interp, "1", TCL_STATIC);
	return (TCL_OK);
}

int
Tclm_MidiMerge(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *dest_song, *src_song;
	int dest_track, i, src_track;

	if (argc < 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " {destMidiID destTrack} {srcMidiID srcTrack} "
		    "?{srcMidiID srcTrack} ...?\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if (Tclm_GetTrack(tclm_interp, interp, argv[1], &dest_song,
	    &dest_track) != TCL_OK)
		return (TCL_ERROR);

	for (i = 2; i < argc; i++) {
		if (Tclm_GetTrack(tclm_interp, interp, argv[i], &src_song,
		    &src_track) != TCL_OK)
			return (TCL_ERROR);
		if (!dest_song->Merge(dest_track, *src_song, src_track)) {
			Tcl_AppendResult(interp, "couldn't merge ",
			    argv[i], " to ", argv[1], 0);
			return (TCL_ERROR);
		}
	}
	return (TCL_OK);
}

int
Tclm_MidiSplit(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *src_song, *meta_song, *normal_song;
	int src_track, meta_track, normal_track;

	if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " {srcMidiID srcTrack} {metaMidiID metaTrack} "
		    "{otherMidiID otherTrack}\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if (Tclm_GetTrack(tclm_interp, interp, argv[1], &src_song, &src_track)
	    != TCL_OK)
		return (TCL_ERROR);
	if (Tclm_GetTrack(tclm_interp, interp, argv[2], &meta_song, &meta_track)
	    != TCL_OK)
		return (TCL_ERROR);
	if (Tclm_GetTrack(tclm_interp, interp, argv[3], &normal_song,
	    &normal_track) != TCL_OK)
		return (TCL_ERROR);

	if (!src_song->Split(src_track, *meta_song, meta_track, *normal_song,
	    normal_track)) {
		Tcl_AppendResult(interp, "Couldn't split track ", argv[1], 0);
		return (TCL_ERROR);
	}
	return (TCL_OK);
}


int
Tclm_MidiCopy(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	EventTree *tmp_track;
	Song *src_song, *dest_song;
	double scalar;
	unsigned long dstart, sstart, send;
	int src_track, dest_track;

	if (argc != 6) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " {destMidiID destTrack} destStartTime "
		    "{srcMidiID srcTrack} srcStartTime srcEndTime\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if (Tclm_GetTrack(tclm_interp, interp, argv[1], &dest_song, &dest_track)
	    != TCL_OK)
		return (TCL_ERROR);
	if (Tcl_GetLong(interp, argv[2], (long *)&dstart) != TCL_OK)
		return (TCL_ERROR);
	if (Tclm_GetTrack(tclm_interp, interp, argv[3], &src_song, &src_track)
	    != TCL_OK)
		return (TCL_ERROR);
	if (Tcl_GetLong(interp, argv[4], (long *)&sstart) != TCL_OK)
		return (TCL_ERROR);
	if (Tcl_GetLong(interp, argv[5], (long *)&send) != TCL_OK)
		return (TCL_ERROR);

	scalar = (double)dest_song->GetDivision() / src_song->GetDivision();

	tmp_track = src_song->GetRange(src_track, sstart, send);
	if (tmp_track == 0) {
		Tcl_AppendResult(interp, "Couldn't get range from: ", argv[3],
		    " to ", argv[4], 0);
		return (TCL_ERROR);
	}
	if (strcmp(argv[0], "midimove") == 0) {
		if (!src_song->DeleteRange(src_track, sstart, send)) {
			Tcl_AppendResult(interp, "Couldn't remove events "
			    "from source track", 0);
			return (TCL_ERROR);
		}
	}
	if (!dest_song->Add(dest_track, *tmp_track, dstart, scalar)) {
		Tcl_AppendResult(interp, "Couldn't add range", 0);
		return (TCL_ERROR);
	}
	delete tmp_track;
	return (TCL_OK);
}

int
Tclm_MidiVersion(ClientData client_data, Tcl_Interp *interp, int argc,
    char *argv[])
{
	ClientData *dummy;

	// shut up a warning
	dummy = &client_data;

	if (argc != 1) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], "\"", 0);
		return (TCL_ERROR);
	}
	Tcl_SetResult(interp, (char *)TCLMIDI_VERSION, TCL_STATIC);
	return (TCL_OK);
}

int
Tclm_MidiTrack(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *song;
	int track;

	if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID track {start|end}\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[1])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[1], 0);
		return (TCL_ERROR);
	}

	if (Tcl_GetInt(interp, argv[2], &track) != TCL_OK)
		return (TCL_ERROR);

	if (track >= song->GetNumTracks() || track < 0) {
		ostrstream buf;
		char *bstr;

		buf << "bad track value " << track << " (only " <<
		    (int)song->GetNumTracks() << " tracks in song)" << ends;
		bstr = buf.str();
		Tcl_SetResult(interp, bstr, TCL_VOLATILE);
		delete bstr;
		return (TCL_ERROR);
	}

	if (strcmp(argv[3], "start") == 0) {
		ostrstream buf;
		char *bstr;

		buf << song->GetTrack(track).GetStartTime() << ends;
		bstr = buf.str();
		Tcl_SetResult(interp, bstr, TCL_VOLATILE);
		delete bstr;
		return (TCL_OK);
	} else if (strcmp(argv[3], "end") == 0) {
		ostrstream buf;
		char *bstr;

		buf << song->GetTrack(track).GetEndTime() << ends;
		bstr = buf.str();
		Tcl_SetResult(interp, bstr, TCL_VOLATILE);
		delete bstr;
		return (TCL_OK);
	} else {
		Tcl_AppendResult(interp, "bad ", argv[0], " command \"",
		    argv[3], "\"", 0);
		return (TCL_ERROR);
	}
}

int
Tclm_MidiGrep(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;
	Song *song;
	int i, num_events, num_matched, track;
	Event **events, **matched;
	NoteEvent *np;
	ostrstream *buf;
	char *str;

	if (argc < 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID track event ?event?\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if ((song = tclm_interp->GetSong(argv[1])) == 0) {
		Tcl_AppendResult(interp, "bad key ", argv[1], 0);
		return (TCL_ERROR);
	}

	if (Tcl_GetInt(interp, argv[2], &track) != TCL_OK)
		return (TCL_ERROR);

	if (track >= song->GetNumTracks() || track < 0) {
		ostrstream buf;
		char *bstr;

		buf << "bad track value " << track << " (only " <<
		    (int)song->GetNumTracks() << " tracks in song)" << ends;
		bstr = buf.str();
		Tcl_SetResult(interp, bstr, TCL_VOLATILE);
		delete bstr;
		return (TCL_ERROR);
	}

	num_events = argc - 3;
	events = new Event *[num_events];
	if (events == 0) {
		Tcl_SetResult(interp, "No more memory", TCL_STATIC);
		return (TCL_ERROR);
	}
	for (i = 0; i < num_events; i++) {
		// parse events
		events[i] = Tclm_ParseEvent(interp, argv[i + 3]);
		if (events[i] == 0) {
			if (strlen(interp->result) == 0)
				Tcl_SetResult(interp, "No more memory",
				    TCL_STATIC);
			i--;
			for (; i >= 0; i--) {
				if (events[i]->GetType() == NOTEON) {
					np = ((NoteEvent *)events[i])->
					    GetNotePair();
					if (np != 0)
						delete np;
				}
				delete events[i];
			}
			delete events;
			return (TCL_ERROR);
		}
	}
	if (!song->Grep(track, events, num_events, &matched, &num_matched)) {
		Tcl_SetResult(interp, "midigrep failed", TCL_STATIC);
		return (TCL_ERROR);
	}
	for (i = 0; i < num_events; i++) {
		if (events[i]->GetType() == NOTEON) {
			np = ((NoteEvent *)events[i])->GetNotePair();
			if (np != 0)
				delete np;
		}
		delete events[i];
	}
	delete events;
	for (i = 0; i < num_matched; i++) {
		buf = new ostrstream;
		Tclm_PrintEvent(*buf, matched[i]);
		str = buf->str();
		if (str != 0 && str[0] != '\0')
			Tcl_AppendElement(interp, str);
		delete str;
		delete buf;
	}
	delete matched;
	return (TCL_OK);
}


int
Tclm_GetTrack(TclmInterp *tclm_interp, Tcl_Interp *interp, const char *str,
    Song **song, int *track)
{
	char **sub_argv;
	int sub_argc;

	if (Tcl_SplitList(interp, (char *)str, &sub_argc, &sub_argv) != TCL_OK)
		return (TCL_ERROR);
	if (sub_argc != 2) {
		Tcl_SetResult(interp, "bad track designation: "
		    "should be \"{MidiID Track}\"", TCL_STATIC);
		Tcl_Ckfree((char *)sub_argv);
		return (TCL_ERROR);
	}
	if ((*song = tclm_interp->GetSong(sub_argv[0])) == 0) {
		Tcl_AppendResult(interp, "bad MidiID ", str, 0);
		Tcl_Ckfree((char *)sub_argv);
		return (TCL_ERROR);
	}
	if (Tcl_GetInt(interp, sub_argv[1], track) != TCL_OK) {
		Tcl_AppendResult(interp, "bad track ", str, 0);
		Tcl_Ckfree((char *)sub_argv);
		return (TCL_ERROR);
	}
	if (*track < 0 || *track >= (*song)->GetNumTracks()) {
		ostrstream buf;
		char *s;

		buf << "Bad track value " << str << " (only "
		    << (*song)->GetNumTracks() << " tracks in song)" << ends;
		s = buf.str();
		Tcl_SetResult(interp, s, TCL_VOLATILE);
		delete s;
		Tcl_Ckfree((char *)sub_argv);
		return (TCL_ERROR);
	}
	Tcl_Ckfree((char *)sub_argv);
	return (TCL_OK);
}

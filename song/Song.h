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
#ifndef SONG_H
#define SONG_H

extern "C" {
#include <tcl.h>
}
#include "EvntTree.h"

class Song {
	friend ostream &operator<<(ostream &os, const Song &s);
public:
	Song();
	Song(const Song &s);
	Song(short num);
	Song(short form, short div, short num);
	~Song();

	Event *GetEvents(short track, unsigned long time);
	Event *NextEvent(short track);
	Event *PrevEvent(short track);
	short GetFormat(void) const {return (format);}
	short GetDivision(void) const {return (division);}
	short GetNumTracks(void) const {return (num_tracks);}
	EventTree &GetTrack(short track);

	Event *PutEvent(short track, const Event &event);
	void RewindEvents(void);
	void RewindEvents(short track);
	int DeleteEvent(short track, Event &event);
	void SetFormat(short form) {format = form;}
	void SetDivision(short div) {division = div;}
	void SetNumTracks(short num);

	int Add(short track, EventTree &et, unsigned long start,
	    double scalar = 1.0);
	EventTree *GetRange(short track, unsigned long start,
	    unsigned long end) const;
	int DeleteRange(short track, unsigned long start, unsigned long end);

	int Grep(short track, Event **events, int num_event, Event ***matched,
	    int *num_matched);

	int Merge(short dest_track, const Song &song, short src_track);
	int Split(short src_track, Song &meta_song, short meta_track,
	    Song &normal_song, short normal_track) const;

	Song &operator=(const Song &s);

	int SMFRead(int fd);
	int SMFWrite(int fd);
	int SMFRead(Tcl_Channel channel);
	int SMFWrite(Tcl_Channel channel);
	const char *GetError(void) const;

	void SetNotePair(int track_num, Event *event);
private:
	short format;
	short division;
	short num_tracks;
	EventTree **tracks;
	const char *errstr;
};
#endif

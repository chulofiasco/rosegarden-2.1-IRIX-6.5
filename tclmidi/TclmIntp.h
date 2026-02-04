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
#ifndef TCLMINTERP_H
#define TCLMINTERP_H

#include <iostream.h>
#include "Song.h"
#include "MidiDev.h"
#include "GusPatch.h"

extern "C" {
#include <tcl.h>
}

class TclmInterp {
public:
	TclmInterp();
	TclmInterp(const TclmInterp &ti);
	~TclmInterp();

	Song *GetSong(const char *key) const;
	char *AddSong(const Song *song);
	int DeleteSong(const char *key);
	MidiDevice *GetDevice(const char *key) const;
	char *AddDevice(const MidiDevice *dev);
	int DeleteDevice(const char *key);
	GusPatchFile *GetPatch(const char *key) const;
	char *AddPatch(const GusPatchFile *dev);
	int DeletePatch(const char *key);

	TclmInterp &operator=(const TclmInterp &ti);
private:
	Tcl_HashTable song_ht;
	Tcl_HashTable dev_ht;
	Tcl_HashTable patch_ht;
	int current_song;
	int current_dev;
	int current_patch;
	Event *next_event;
};
#endif

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
#include <assert.h>
/* Damn Microsoft compiler can't deal with the real name of the include file */
#ifndef _MSC_VER
#include <strstream.h>
#else
#include <strstrea.h>
#endif
#include "TclmIntp.h"

TclmInterp::TclmInterp() : current_song(0), current_dev(0), current_patch(0),
    next_event(0)
{

	Tcl_InitHashTable(&song_ht, TCL_STRING_KEYS);
	Tcl_InitHashTable(&dev_ht, TCL_STRING_KEYS);
	Tcl_InitHashTable(&patch_ht, TCL_STRING_KEYS);
}

TclmInterp::TclmInterp(const TclmInterp &ti)
{
	ostrstream *buf;
	Tcl_HashSearch search;
	Tcl_HashEntry *entry, *new_entry;
	Song *song, *new_song;
	MidiDevice *dev, *new_dev;
	GusPatchFile *patch, *new_patch;
	char *key;
	int repeat;

	current_song = 0;
	current_dev = 0;
	Tcl_InitHashTable(&song_ht, TCL_STRING_KEYS);
	entry = Tcl_FirstHashEntry((Tcl_HashTable *)&ti.song_ht, &search);
	while (entry != 0) {
		song = (Song *)Tcl_GetHashValue(entry);

		// make a new entry
		buf = new ostrstream;
		*buf << "song" << current_song << ends;
		key = buf->str();
		new_entry = Tcl_CreateHashEntry(&song_ht, key, &repeat);
		new_song = new Song(*song);
		assert(new_song != 0);
		Tcl_SetHashValue(new_entry, new_song);
		delete key;
		delete buf;
		current_song++;

		// find next entry
		entry = Tcl_NextHashEntry(&search);
	}

	Tcl_InitHashTable(&dev_ht, TCL_STRING_KEYS);
	entry = Tcl_FirstHashEntry((Tcl_HashTable *)&ti.dev_ht, &search);
	while (entry != 0) {
		dev = (MidiDevice *)Tcl_GetHashValue(entry);

		// make a new entry
		buf = new ostrstream;
		*buf << "device" << current_dev << ends;
		key = buf->str();
		new_entry = Tcl_CreateHashEntry(&dev_ht, key, &repeat);
		new_dev = dev->Dup();
		assert(new_dev != 0);
		Tcl_SetHashValue(new_entry, new_dev);
		delete key;
		delete buf;
		current_dev++;

		// find next entry
		entry = Tcl_NextHashEntry(&search);
	}

	Tcl_InitHashTable(&patch_ht, TCL_STRING_KEYS);
	entry = Tcl_FirstHashEntry((Tcl_HashTable *)&ti.patch_ht, &search);
	while (entry != 0) {
		patch = (GusPatchFile *)Tcl_GetHashValue(entry);

		// make a new entry
		buf = new ostrstream;
		*buf << "patch" << current_patch << ends;
		key = buf->str();
		new_entry = Tcl_CreateHashEntry(&patch_ht, key, &repeat);
		new_patch = patch->Dup();
		assert(new_patch != 0);
		Tcl_SetHashValue(new_entry, new_patch);
		delete key;
		delete buf;
		current_patch++;

		// find next entry
		entry = Tcl_NextHashEntry(&search);
	}
}

TclmInterp::~TclmInterp()
{
	Tcl_HashSearch search;
	Tcl_HashEntry *entry;
	Song *song;
	MidiDevice *dev;
	GusPatchFile *patch;

	// delete existing entries
	entry = Tcl_FirstHashEntry(&song_ht, &search);
	while (entry != 0) {
		song = (Song *)Tcl_GetHashValue(entry);
		delete song;
		Tcl_DeleteHashEntry(entry);
		entry = Tcl_NextHashEntry(&search);
	}

	entry = Tcl_FirstHashEntry(&dev_ht, &search);
	while (entry != 0) {
		dev = (MidiDevice *)Tcl_GetHashValue(entry);
		delete dev;
		Tcl_DeleteHashEntry(entry);
		entry = Tcl_NextHashEntry(&search);
	}

	entry = Tcl_FirstHashEntry(&patch_ht, &search);
	while (entry != 0) {
		patch = (GusPatchFile *)Tcl_GetHashValue(entry);
		delete patch;
		Tcl_DeleteHashEntry(entry);
		entry = Tcl_NextHashEntry(&search);
	}

	// delete table
	Tcl_DeleteHashTable(&song_ht);
	Tcl_DeleteHashTable(&dev_ht);
	Tcl_DeleteHashTable(&patch_ht);
}

Song *
TclmInterp::GetSong(const char *key) const
{
	Tcl_HashEntry *entry;

	entry = Tcl_FindHashEntry((Tcl_HashTable *)&song_ht, (char *)key);
	if (entry == 0)
		return (0);
	return ((Song *)Tcl_GetHashValue(entry));
}

char *
TclmInterp::AddSong(const Song *song)
{
	ostrstream buf;
	char *key;
	Tcl_HashEntry *entry;
	int repeat;

	buf << "song" << current_song++ << ends;
	key = buf.str();
	entry = Tcl_CreateHashEntry(&song_ht, key, &repeat);
	Tcl_SetHashValue(entry, song);
	return (key);
}

int
TclmInterp::DeleteSong(const char *key)
{
	Tcl_HashEntry *entry;
	Song *song;

	entry = Tcl_FindHashEntry(&song_ht, (char *)key);
	if (entry == 0)
		return (0);
	song = (Song *)Tcl_GetHashValue(entry);
	delete song;
	Tcl_DeleteHashEntry(entry);
	return (1);
}

MidiDevice *
TclmInterp::GetDevice(const char *key) const
{
	Tcl_HashEntry *entry;

	entry = Tcl_FindHashEntry((Tcl_HashTable *)&dev_ht, (char *)key);
	if (entry == 0)
		return (0);
	return ((MidiDevice *)Tcl_GetHashValue(entry));
}

char *
TclmInterp::AddDevice(const MidiDevice *dev)
{
	ostrstream buf;
	char *key;
	Tcl_HashEntry *entry;
	int repeat;

	buf << "device" << current_dev++ << ends;
	key = buf.str();
	entry = Tcl_CreateHashEntry(&dev_ht, key, &repeat);
	Tcl_SetHashValue(entry, dev);
	return (key);
}

int
TclmInterp::DeleteDevice(const char *key)
{
	Tcl_HashEntry *entry;
	MidiDevice *dev;

	entry = Tcl_FindHashEntry(&dev_ht, (char *)key);
	if (entry == 0)
		return (0);
	dev = (MidiDevice *)Tcl_GetHashValue(entry);
	delete dev;
	Tcl_DeleteHashEntry(entry);
	return (1);
}

GusPatchFile *
TclmInterp::GetPatch(const char *key) const
{
	Tcl_HashEntry *entry;

	entry = Tcl_FindHashEntry((Tcl_HashTable *)&patch_ht, (char *)key);
	if (entry == 0)
		return (0);
	return ((GusPatchFile *)Tcl_GetHashValue(entry));
}

char *
TclmInterp::AddPatch(const GusPatchFile *patch)
{
	ostrstream buf;
	char *key;
	Tcl_HashEntry *entry;
	int repeat;

	buf << "patch" << current_patch++ << ends;
	key = buf.str();
	entry = Tcl_CreateHashEntry(&patch_ht, key, &repeat);
	Tcl_SetHashValue(entry, patch);
	return (key);
}

int
TclmInterp::DeletePatch(const char *key)
{
	Tcl_HashEntry *entry;
	GusPatchFile *patch;

	entry = Tcl_FindHashEntry(&patch_ht, (char *)key);
	if (entry == 0)
		return (0);
	patch = (GusPatchFile *)Tcl_GetHashValue(entry);
	delete patch;
	Tcl_DeleteHashEntry(entry);
	return (1);
}

TclmInterp &
TclmInterp::operator=(const TclmInterp &ti)
{
	ostrstream *buf;
	Tcl_HashSearch search;
	Tcl_HashEntry *entry, *new_entry;
	Song *song, *new_song;
	MidiDevice *dev, *new_dev;
	GusPatchFile *patch, *new_patch;
	char *key;
	int repeat;

	// delete existing entries
	entry = Tcl_FirstHashEntry(&song_ht, &search);
	while (entry != 0) {
		song = (Song *)Tcl_GetHashValue(entry);
		delete song;
		Tcl_DeleteHashEntry(entry);
		entry = Tcl_NextHashEntry(&search);
	}
	entry = Tcl_FirstHashEntry(&dev_ht, &search);
	while (entry != 0) {
		dev = (MidiDevice *)Tcl_GetHashValue(entry);
		delete dev;
		Tcl_DeleteHashEntry(entry);
		entry = Tcl_NextHashEntry(&search);
	}
	entry = Tcl_FirstHashEntry(&patch_ht, &search);
	while (entry != 0) {
		patch = (GusPatchFile *)Tcl_GetHashValue(entry);
		delete patch;
		Tcl_DeleteHashEntry(entry);
		entry = Tcl_NextHashEntry(&search);
	}

	current_song = 0;
	entry = Tcl_FirstHashEntry((Tcl_HashTable *)&ti.song_ht, &search);
	while (entry != 0) {
		song = (Song *)Tcl_GetHashValue(entry);

		// make a new entry
		buf = new ostrstream;
		*buf << "song" << current_song << ends;
		key = buf->str();
		new_entry = Tcl_CreateHashEntry(&song_ht, key, &repeat);
		new_song = new Song(*song);
		assert(new_song != 0);
		Tcl_SetHashValue(new_entry, new_song);
		delete key;
		delete buf;
		current_song++;

		// find next entry
		entry = Tcl_NextHashEntry(&search);
	}

	current_dev = 0;
	entry = Tcl_FirstHashEntry((Tcl_HashTable *)&ti.dev_ht, &search);
	while (entry != 0) {
		dev = (MidiDevice *)Tcl_GetHashValue(entry);

		// make a new entry
		buf = new ostrstream;
		*buf << "device" << current_dev << ends;
		key = buf->str();
		new_entry = Tcl_CreateHashEntry(&dev_ht, key, &repeat);
		new_dev = dev->Dup();
		assert(new_dev != 0);
		Tcl_SetHashValue(new_entry, new_dev);
		delete key;
		delete buf;
		current_dev++;

		// find next entry
		entry = Tcl_NextHashEntry(&search);
	}

	current_patch = 0;
	entry = Tcl_FirstHashEntry((Tcl_HashTable *)&ti.patch_ht, &search);
	while (entry != 0) {
		patch = (GusPatchFile *)Tcl_GetHashValue(entry);

		// make a new entry
		buf = new ostrstream;
		*buf << "patch" << current_patch << ends;
		key = buf->str();
		new_entry = Tcl_CreateHashEntry(&patch_ht, key, &repeat);
		new_patch = patch->Dup();
		assert(new_patch != 0);
		Tcl_SetHashValue(new_entry, new_patch);
		delete key;
		delete buf;
		current_patch++;

		// find next entry
		entry = Tcl_NextHashEntry(&search);
	}

	return (*this);
}

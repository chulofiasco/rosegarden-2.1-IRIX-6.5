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
#ifndef EVENTTREE_H
#define EVENTTREE_H

#include <iostream.h>
/* Microsoft compiler can't deal with the real name - are we surprised? */
#ifndef _MSC_VER
#include <strstream.h>
#else
#include <strstrea.h>
#endif
#include <iomanip.h>
#include <assert.h>

#include "Event.h"
#include "EvTrDefs.h"

extern "C" {
#include "rb.h"
}


class EventTree {
	friend ostream &operator<<(ostream &os, const EventTree &t);
public:
	EventTree();
	EventTree(const EventTree &t);
	~EventTree();

	Event *GetEvents(unsigned long time);
	Event *GetEventsNoMod(unsigned long time) const;
	Event *NextEvent(void);
	Event *NextEvent(const Event *e) const;
	Event *NextEvents(void);
	Event *NextEvents(const Event *e) const;
	Event *PrevEvent(void);
	Event *PrevEvent(const Event *e) const;
	Event *PrevEvents(void);
	Event *PrevEvents(const Event *e) const;
	Event *GetFirstEvent(void);
	Event *GetFirstEvents(void);
	Event *GetLastEvent(void);
	Event *GetLastEvents(void);
	Event *GetFirstEvent(void) const;
	Event *GetFirstEvents(void) const;
	Event *GetLastEvent(void) const;
	Event *GetLastEvents(void) const;
	unsigned long GetStartTime(void);
	unsigned long GetEndTime(void);

	int Add(const EventTree &et, unsigned long start, double scalar = 1);
	EventTree *GetRange(unsigned long start, unsigned long end) const;
	int DeleteRange(unsigned long start, unsigned long end);

	void Grep(Event **events, int num_event, Event ***matched,
	    int *num_matched) const;

	Event *PutEvent(const Event &event);
	void RewindEvents(void) {curr_event = 0;}
	int DeleteEvent(Event &event);

	EventTree &operator=(const EventTree &t);
private:
	void DeleteTree(void);
	void CopyTree(const EventTree &t);

	EventTreeHead *head;
	Event *curr_event;
};

#endif

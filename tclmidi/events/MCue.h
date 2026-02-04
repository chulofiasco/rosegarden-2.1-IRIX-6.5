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
#ifndef METACUE_H
#define METACUE_H

#include "MText.h"

class MetaCueEvent : public MetaTextEvent {
	friend ostream &operator<<(ostream &os, const MetaCueEvent &e);
public:
	MetaCueEvent();
	MetaCueEvent(unsigned long t, const char *str);
	MetaCueEvent(const MetaCueEvent &e);
	virtual ~MetaCueEvent();
	virtual Event *Dup(void) const {return (new MetaCueEvent(*this));}

	virtual EventType GetType(void) const {return (METACUE);}
	virtual char *GetTypeStr(void) const {return ("MetaCueEvent");}
	virtual char *GetEventStr(void) const;

	MetaCueEvent &operator=(const MetaCueEvent &e);

	virtual const char *SMFRead(SMFTrack &t)
	    {return (MetaTextEvent::SMFRead(t));}
	virtual const char *SMFWrite(SMFTrack &t) const
	    {return (MetaTextEvent::SMFWrite(t));}
protected:
	virtual int Equal(const Event *e) const
	    {return (MetaTextEvent::Equal(e));}
};
#endif

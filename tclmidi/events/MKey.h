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
#ifndef METAKEY_H
#define METAKEY_H

#include "MEvent.h"

typedef enum {KEY_CFLAT = -7, KEY_GFLAT = -6, KEY_DFLAT = -5, KEY_AFLAT = -4,
    KEY_EFLAT = -3, KEY_BFLAT = -2, KEY_F = -1, KEY_C = 0, KEY_G = 1,
    KEY_D = 2, KEY_A = 3, KEY_E = 4, KEY_B = 5, KEY_FSHARP = 6,
    KEY_CSHARP = 7, KEY_WC} Key;

typedef enum {MODE_MAJOR = 0, MODE_MINOR = 1, MODE_WC} Mode;

Key IntToKey(int i);
int KeyToInt(Key k);
Mode IntToMode(int i);
int ModeToInt(Mode m);

extern Key StrToKey(const char *str, int *match);
extern Mode StrToMode(const char *str, int *match);

class MetaKeyEvent : public MetaEvent {
	friend ostream &operator<<(ostream &os, const MetaKeyEvent &e);
public:
	MetaKeyEvent();
	MetaKeyEvent(unsigned long t, Key k = KEY_C, Mode m = MODE_MAJOR);
	MetaKeyEvent(const MetaKeyEvent &e);
	virtual Event *Dup(void) const {return (new MetaKeyEvent(*this));}

	virtual EventType GetType(void) const {return (METAKEY);}
	virtual char *GetTypeStr(void) const {return ("MetaKeyEvent");}
	virtual char *GetEventStr(void) const;
	Key GetKey(void) const {
		if (GetWildcard(wc_key))
			return (KEY_WC);
		else
			return (key);
	}
	const char *GetKeyStr(void) const;
	Mode GetMode(void) const {
		if (GetWildcard(wc_mode))
			return (MODE_WC);
		else
			return (mode);
	}
	const char *GetModeStr(void) const;

	void SetKey(Key k) {
		if (k == KEY_WC)
			SetWildcard(wc_key);
		else {
			key = k;
			ClearWildcard(wc_key);
		}
	}
	void SetMode(Mode m) {
		if (m == MODE_WC)
			SetWildcard(wc_mode);
		else {
			mode = m;
			ClearWildcard(wc_mode);
		}
	}

	MetaKeyEvent &operator=(const MetaKeyEvent &e);

	virtual const char *SMFRead(SMFTrack &t);
	virtual const char *SMFWrite(SMFTrack &t) const;

	static const Key WC_KEY;
	static const Mode WC_MODE;
protected:
	virtual int Equal(const Event *e) const;
private:
	static const unsigned long wc_key;
	static const unsigned long wc_mode;
	Key key;
	Mode mode;
};
#endif

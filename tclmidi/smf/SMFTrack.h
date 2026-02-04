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
#ifndef SMFTRACK_H
#define SMFTRACK_H

extern "C" {
#include <tcl.h>
}

#include <iostream.h>
#include <iomanip.h>
/* Microsoft compiler can't deal with the real name - are we surprised? */
#ifndef _MSC_VER
#include <strstream.h>
#else
#include <strstrea.h>
#endif

const long StreamBlockSize = 256;

class SMFTrack {
	friend ostream &operator<<(ostream &os, const SMFTrack &t);
public:
	SMFTrack();
	SMFTrack(unsigned char *data, long len);
	SMFTrack(const SMFTrack &t);
	~SMFTrack();

	void StaticBuffer(unsigned char *data, long len, int init);
	void DynamicBuffer(void);

	long GetLength(void) const {return (length);}
	unsigned char GetRunningState(void) const {return (run_state);}
	const unsigned char *GetByte(void);
	const unsigned char *PeekByte(void) const;
	const unsigned char *GetData(long len);
	long GetVarValue(void);

	void SetRunningState(unsigned char rs) {run_state = rs;}
	int PutByte(unsigned char b);
	int PutData(unsigned char *data, long len);
	int PutFixValue(long val);

	void Empty(void);
	int Read(int fd);
	int Write(int fd) const;
	int Read(Tcl_Channel channel);
	int Write(Tcl_Channel channel) const;

	SMFTrack &operator=(const SMFTrack &t);
private:
	int IncreaseSize(long len);

	long allocated;
	long length;
	int static_buf;
	unsigned char run_state;
	unsigned char *start;
	unsigned char *pos;
	unsigned char *end;
};
#endif

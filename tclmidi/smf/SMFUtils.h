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
#ifndef SMFUTILS_H
#define SMFUTILS_H

extern "C" {
#include <tcl.h>
}

/*
 * convert midi order to host order and vice-versa
 * Trying a new inline function version.  This generates silly warnings,
 * but I believe it is clearer and faster.
 */
#if WORDS_BIGENDIAN

inline long
mtohl(long l)
{

	return (l);
}

inline long
htoml(long l)
{

	return (l);
}

inline short
mtohs(short s)
{

	return (s);
}

inline short
htoms(short s)
{

	return (s);
}

#else

inline long
mtohl(long l)
{
	union {
		long l;
		char c[4];
	} swap;
	char tmp_c;

	swap.l = l;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[3];
	swap.c[3] = tmp_c;
	tmp_c = swap.c[1];
	swap.c[1] = swap.c[2];
	swap.c[2] = tmp_c;
	return (swap.l);
}

inline long
htoml(long l)
{

	union {
		long l;
		char c[4];
	} swap;
	char tmp_c;

	swap.l = l;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[3];
	swap.c[3] = tmp_c;
	tmp_c = swap.c[1];
	swap.c[1] = swap.c[2];
	swap.c[2] = tmp_c;
	return (swap.l);
}

inline short
mtohs(short s)
{
	union {
		short s;
		char c[2];
	} swap;
	char tmp_c;

	swap.s = s;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[1];
	swap.c[1] = tmp_c;
	return (swap.s);
}

inline short
htoms(short s)
{

	union {
		short s;
		char c[2];
	} swap;
	char tmp_c;

	swap.s = s;
	tmp_c = swap.c[0];
	swap.c[0] = swap.c[1];
	swap.c[1] = tmp_c;
	return (swap.s);
}
#endif


extern long MRead(int fd, char *data, long len);
extern long MWrite(int fd, char *data, long len);
extern long MRead(Tcl_Channel channel, char *data, long len);
extern long MWrite(Tcl_Channel channel, char *data, long len);
extern long VarToFix(unsigned char *var, int *len);
extern int FixToVar(long fix, unsigned char *var);
#endif

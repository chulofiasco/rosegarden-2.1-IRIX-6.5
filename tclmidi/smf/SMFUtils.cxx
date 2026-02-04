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
#include <stdlib.h>
#if HAVE_UNISTD_H
# include <sys/types.h>
# include <unistd.h>
#else
# ifdef _MSC_VER
#  include <io.h>
# endif
#endif
#include "SMFUtils.h"


long
MRead(int fd, char *data, long len)
{
	int num_read;
	int total_read;

	total_read = 0;
	do {
		if ((num_read = read(fd, data, len - total_read)) == -1)
			return (-1);
		if (num_read == 0)
			break;
		total_read += num_read;
		data += num_read;
	} while (len > total_read);
	return (total_read);
}

long
MWrite(int fd, char *data, long len)
{
	int num_written;
	int total_written;

	total_written = 0;
	do {
		if ((num_written = write(fd, data, len - total_written)) == -1)
			return (-1);
		if (num_written == 0)
			break;
		total_written += num_written;
		data += num_written;
	} while (len > total_written);
	return (total_written);
}

long
MRead(Tcl_Channel channel, char *data, long len)
{
	int num_read;
	int total_read;

	total_read = 0;
	do {
		if ((num_read = Tcl_Read(channel, data, len - total_read))
		    == -1)
			return (-1);
		if (num_read == 0)
			break;
		total_read += num_read;
		data += num_read;
	} while (len > total_read);
	return (total_read);
}

long
MWrite(Tcl_Channel channel, char *data, long len)
{
	int num_written;
	int total_written;

	total_written = 0;
	do {
		if ((num_written = Tcl_Write(channel, data,
		    len - total_written)) == -1)
			return (-1);
		if (num_written == 0)
			break;
		total_written += num_written;
		data += num_written;
	} while (len > total_written);
	return (total_written);
}


long
VarToFix(unsigned char *var, int *len)
{
	long fix;

	fix = 0;
	*len = 0;
	if (*var & 0x80)
		do {
			fix = (fix << 7) + (*var & 0x7f);
			(*len)++;
		} while (*var++ & 0x80);
	else {
		fix = *var++;
		(*len)++;
	}

	return (fix);
}


int
FixToVar(long fix, unsigned char *var)
{
	int i;
	unsigned char buf[4];
	unsigned char *bptr;

	buf[0] = buf[1] = buf[2] = buf[3] = 0;
	bptr = buf;
	*bptr++ = fix & 0x7f;
	while ((fix >>= 7) > 0) {
		*bptr |= 0x80;
		*bptr++ += (fix & 0x7f);
	}

	i = 0;
	do {
		*var++ = *--bptr;
		i++;
	} while (bptr != buf);

	return (i);
}


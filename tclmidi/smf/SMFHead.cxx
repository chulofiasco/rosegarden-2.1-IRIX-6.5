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
#include <string.h>
#include "SMFHead.h"
#include "SMFUtils.h"

SMFHead::SMFHead() : format(0), num_tracks(1), division(120)
{
}

SMFHead::SMFHead(short form, short num, short div) : format(form),
    num_tracks(num), division(div)
{
}

SMFHead::SMFHead(const SMFHead &h) : format(h.format), num_tracks(h.num_tracks),
    division(h.division)
{
}

SMFHead &
SMFHead::operator=(const SMFHead &h)
{

	format = h.format;
	num_tracks = h.num_tracks;
	division = h.division;
	return (*this);
}

int
SMFHead::Read(int fd)
{
	long length;
	char id[4];

	if (MRead(fd, id, 4) != 4)
		return (0);
	if (strncmp(id, "MThd", 4) != 0)
		return (0);
	if (MRead(fd, (char *)&length, 4) != 4)
		return (0);
	length = mtohl(length);
	/* length should be 6, but who really cares */
	if (MRead(fd, (char *)&format, 2) != 2)
		return (0);
	format = mtohs(format);
	if (MRead(fd, (char *)&num_tracks, 2) != 2)
		return (0);
	num_tracks = mtohs(num_tracks);
	if (MRead(fd, (char *)&division, 2) != 2)
		return (0);
	division = mtohs(division);
	/* a little sanity checking */
	if (format == 0 && num_tracks != 1)
		return (0);
	return (1);
}

int
SMFHead::Write(int fd) const
{
	long length;
	short s;

	if (MWrite(fd, "MThd", 4) != 4)
		return (0);
	length = htoml(6L);
	if (MWrite(fd, (char *)&length, 4) != 4)
		return (0);
	s = htoms(format);
	if (MWrite(fd, (char *)&s, 2) != 2)
		return (0);
	s = htoms(num_tracks);
	if (MWrite(fd, (char *)&s, 2) != 2)
		return (0);
	s = htoms(division);
	if (MWrite(fd, (char *)&s, 2) != 2)
		return (0);
	return (1);
}

int
SMFHead::Read(Tcl_Channel channel)
{
	long length;
	char id[4];

	if (MRead(channel, id, 4) != 4)
		return (0);
	if (strncmp(id, "MThd", 4) != 0)
		return (0);
	if (MRead(channel, (char *)&length, 4) != 4)
		return (0);
	length = mtohl(length);
	/* length should be 6, but who really cares */
	if (MRead(channel, (char *)&format, 2) != 2)
		return (0);
	format = mtohs(format);
	if (MRead(channel, (char *)&num_tracks, 2) != 2)
		return (0);
	num_tracks = mtohs(num_tracks);
	if (MRead(channel, (char *)&division, 2) != 2)
		return (0);
	division = mtohs(division);
	/* a little sanity checking */
	if (format == 0 && num_tracks != 1)
		return (0);
	return (1);
}

int
SMFHead::Write(Tcl_Channel channel) const
{
	long length;
	short s;

	if (MWrite(channel, "MThd", 4) != 4)
		return (0);
	length = htoml(6L);
	if (MWrite(channel, (char *)&length, 4) != 4)
		return (0);
	s = htoms(format);
	if (MWrite(channel, (char *)&s, 2) != 2)
		return (0);
	s = htoms(num_tracks);
	if (MWrite(channel, (char *)&s, 2) != 2)
		return (0);
	s = htoms(division);
	if (MWrite(channel, (char *)&s, 2) != 2)
		return (0);
	return (1);
}

ostream &
operator<<(ostream &os, const SMFHead &h)
{

	os << "Format: " << h.format << " Num. Tracks: " << h.num_tracks
	    << " Division: " << h.division;
	return (os);
}

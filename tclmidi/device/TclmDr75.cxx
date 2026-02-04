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
#ifdef HAVE_SYS_MIDIIOCTL_H
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strstream.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include <sys/ioctl.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif

#if HAVE_LINUX_TERMIOS_H
#include <linux/termios.h>
#endif

#include <sys/midiioctl.h>

#include "TclmDr75.h"
#include "EvntUtil.h"
#include "Note.h"
#include "GusPatch.h"

const int MaxEventSize = 256;

TclmDriver75::TclmDriver75() : last_play_time(0), last_rec_time(0), curr_event(0),
    play_song(0), rec_song(0), fd(-1), finished(0), last_record_rs(0),
    interp(0)
{
}

TclmDriver75::TclmDriver75(const TclmDriver75 &td) : last_play_time(0),
    last_rec_time(0), curr_event(0), play_song(0), rec_song(0), fd(-1),
    finished(0), last_record_rs(0), interp(0)
{
	const TclmDriver75 *dummy;

	// shut up a warning
	dummy = &td;
}

TclmDriver75::~TclmDriver75()
{

	if (fd != -1) {
		Stop();
		Close();
	}
	if (curr_event != 0) {
		delete curr_event;
		curr_event = 0;
	}
}

int
TclmDriver75::Open(const char *dev)
{
	ostrstream err;
	char *str;

	if (fd != -1) {
		SetError("Device already open");
		return (0);
	}
	SetName(dev);
	fd = open(dev, O_RDWR);
	if (fd == -1) {
		err << "open failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}
	tcl_file = Tcl_GetFile((ClientData)fd, TCL_UNIX_FD);
	return (1);
}

int
TclmDriver75::Close(void)
{

	if (fd != -1) {
		if (!Stop())
			return (0);
		Tcl_FreeFile(tcl_file);
		close(fd);
	}
	if (curr_event != 0) {
		delete curr_event;
		curr_event = 0;
	}
	return (1);
}

int
TclmDriver75::Play(Song *s, int r)
{
	ostrstream err;
	char *str;
	int arg, i, raw;

	if (fd == -1) {
		SetError("Device is not open");
		return (0);
	}

	if (ioctl(fd, MGRAW, &raw) == -1) {
		err << "MGRAW ioctl failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}

	if (raw) {
		SetError("Can't play song to raw device - use timed device "
		    "instead.");
		return (0);
	}

	if (rec_song != 0) {
		SetError("Already recording");
		return (0);
	}
	if (play_song != 0) {
		SetError("Already playing");
		return (0);
	}

	// set repeat
	SetRepeat(r);

	play_song = s;

	// initialize track/event info
	last_play_time = 0;
	finished = 0;
	curr_event = new Event *[play_song->GetNumTracks()];
	if (curr_event == 0) {
		SetError("No more memory");
		return (0);
	}
	for (i = 0; i < play_song->GetNumTracks(); i++)
		curr_event[i] = play_song->GetTrack(i).GetEventsNoMod(0);

	Tcl_CreateFileHandler(tcl_file, TCL_READABLE | TCL_WRITABLE |
	    TCL_EXCEPTION, TclmDriver75Event, this);

	// Reset the device so the timer is close to current
	if (ioctl(fd, MRESET, 0) == -1) {
		err << "ioctl MRESET failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		Tcl_DeleteFileHandler(tcl_file);
		return (0);
	}

	// set division
	arg = play_song->GetDivision();
	if (ioctl(fd, MSDIVISION, &arg) == -1) {
		err << "ioctl MSDIVISION failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		Tcl_DeleteFileHandler(tcl_file);
		return (0);
	}

	arg = GetMidiThru();
	if (ioctl(fd, MTHRU, &arg) == -1) {
		err << "ioctl MTHRU failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		Tcl_DeleteFileHandler(tcl_file);
		return (0);
	}

	// I don't know why I need to add this.
	Tcl_DoOneEvent(0);

	return (1);
}

int
TclmDriver75::Record(Song *rs, Song *ps, int r)
{
	ostrstream err;
	char *str;
	int arg, i, raw;

	if (fd == -1) {
		SetError("Device is not open");
		return (0);
	}

	if (ioctl(fd, MGRAW, &raw) == -1) {
		err << "MGRAW ioctl failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}

	if (raw) {
		SetError("Can't record song from raw device - use timed device "
		    "instead.");
		return (0);
	}

	if (rec_song != 0) {
		SetError("Already recording");
		return (0);
	}
	if (play_song != 0) {
		SetError("Already playing");
		return (0);
	}

	Tcl_CreateFileHandler(tcl_file, TCL_READABLE | TCL_WRITABLE |
	    TCL_EXCEPTION, TclmDriver75Event, this);

	// Reset the device so the timer is close to current
	if (ioctl(fd, MRESET, 0) == -1) {
		err << "ioctl MRESET failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		Tcl_DeleteFileHandler(tcl_file);
		return (0);
	}

	rec_song = rs;
	last_play_time = 0;
	last_rec_time = 0;
	last_record_rs = 0;
	if (ps == 0) {
		play_song = 0;
		arg = rec_song->GetDivision();
		if (ioctl(fd, MSDIVISION, &arg) == -1) {
			err << "ioctl MSDIVISION failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			Tcl_DeleteFileHandler(tcl_file);
			return (0);
		}
	} else {
		play_song = ps;

		// initialize track/event info
		curr_event = new Event *[play_song->GetNumTracks()];
		if (curr_event == 0) {
			SetError("No more memory");
			Tcl_DeleteFileHandler(tcl_file);
			return (0);
		}
		for (i = 0; i < play_song->GetNumTracks(); i++)
			curr_event[i] =
			    play_song->GetTrack(i).GetEventsNoMod(0);

		// set division
		arg = play_song->GetDivision();
		if (ioctl(fd, MSDIVISION, &arg) == -1) {
			err << "ioctl MSDIVISION failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			Tcl_DeleteFileHandler(tcl_file);
			return (0);
		}

		// start record timer when first event is scheduled to play
		arg = 1;
		if (ioctl(fd, MRECONPLAY, &arg) == -1) {
			err << "ioctl MRECONPLAY failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			Tcl_DeleteFileHandler(tcl_file);
			return (0);
		}

		finished = 0;
	}

	// set repeat
	SetRepeat(r);

	arg = GetMidiThru();
	if (ioctl(fd, MTHRU, &arg) == -1) {
		err << "ioctl MTHRU failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		Tcl_DeleteFileHandler(tcl_file);
		return (0);
	}

	// I don't know why I need to add this.
	Tcl_DoOneEvent(0);

	return (1);
}

int
TclmDriver75::Stop(void)
{
	ostrstream err;
	char *str;
	int arg;

	finished = 1;
	if (fd == -1)
		return (1);

	arg = 0;
	if (ioctl(fd, MASYNC, &arg) == -1) {
		err << "ioctl MASYNC failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}

	if (ioctl(fd, MGPLAYQ, &arg) == -1) {
		err << "ioctl MGPLAYQ failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}
	// flush queues
	if (arg != 0) {
		if (ioctl(fd, MDRAIN, NULL) == -1) {
			err << "ioctl MDRAIN failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (0);
		}
	}

	Tcl_DeleteFileHandler(tcl_file);

	if (curr_event != 0) {
		delete curr_event;
		curr_event = 0;
	}
	play_song = 0;
	rec_song = 0;
	return (1);
}

int
TclmDriver75::Wait(void)
{
	int found_event;

	finished = 0;
	found_event = 1;
	while (!finished && found_event)
		found_event = Tcl_DoOneEvent(0);
	return (1);
}

int
TclmDriver75::GetMidiThru(void)
{
	ostrstream err;
	const char *name;
	char *str;
	int d, mt;

	// open the device
	if (fd != -1)
		d = fd;
	else {
		if ((name = GetName()) == 0) {
			SetError("No device set");
			return (-1);
		}
		// open device
		if ((d = open(name, O_RDONLY)) == -1) {
			err << "open failed: " << strerror(errno) << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (-1);
		}
	}
	if (ioctl(d, MGTHRU, &mt) == -1) {
		err << "ioctl failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (-1);
	}
	if (fd == -1)
		close(d);
	return (mt);
}

int
TclmDriver75::SetMidiThru(int mt)
{
	ostrstream err;
	const char *name;
	char *str;
	int d;

	// open the device
	if (fd != -1)
		d = fd;
	else {
		if ((name = GetName()) == 0) {
			SetError("No device set");
			return (0);
		}
		// open device
		if ((d = open(name, O_RDONLY)) == -1) {
			err << "open failed: " << strerror(errno) << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (0);
		}
	}
	if (ioctl(d, MTHRU, &mt) == -1) {
		err << "ioctl failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}
	if (fd == -1)
		close(d);
	return (1);
}

ostrstream *
TclmDriver75::Feature(const char *command, const char *argv[], int argc)
{
	struct midi_feature feature;
	struct SMPTE_frame frame;
	ostrstream err, *res;
	char *str;

	if (strcmp(command, "kernel_timing") == 0) {
		feature.type = MFEAT_KERNEL_TIMING;
		feature.data = 0;
		if (ioctl(fd, MFEATURE, &feature) == -1) {
			err << "feature ioctl failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (0);
		}
		res = new ostrstream;
		*res << ends;
	} else if (strcmp(command, "smpte_timing") == 0) {
		feature.type = MFEAT_SMPTE_TIMING;
		feature.data = 0;
		if (ioctl(fd, MFEATURE, &feature) == -1) {
			err << "feature ioctl failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (0);
		}
		res = new ostrstream;
		*res << ends;
	} else if (strcmp(command, "mpu401_timing") == 0) {
		feature.type = MFEAT_MPU401_TIMING;
		feature.data = 0;
		if (ioctl(fd, MFEATURE, &feature) == -1) {
			err << "feature ioctl failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (0);
		}
		res = new ostrstream;
		*res << ends;
	} else if (strcmp(command, "get_smpte") == 0) {
		feature.type = MFEAT_GET_SMPTE;
		feature.data = &frame;
		if (ioctl(fd, MFEATURE, &feature) == -1) {
			err << "feature ioctl failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (0);
		}
		res = new ostrstream;
		/* convert data we received from SMPTE command*/
		if (!(frame.status & SMPTE_SYNC))
			*res << "NOSYNC";
		else
			*res << setw(2) << setfill('0') << (int)frame.hour << ":"
			    << setw(2) << setfill('0') << (int)frame.minute << ":"
			    << setw(2) << setfill('0') << (int)frame.second << ":"
			    << setw(2) << setfill('0') << (int)frame.frame << ":"
			    << setw(2) << setfill('0') << (int)frame.fraction;
		*res << ends;
	} else if (strcmp(command, "flush_patches") == 0) {
		feature.type = MFEAT_FLUSH_PATCHES;
		feature.data = 0;
		if (ioctl(fd, MFEATURE, &feature) == -1) {
			err << "feature ioctl failed: " << strerror(errno)
			    << ends;
			str = err.str();
			SetError(str);
			delete str;
			return (0);
		}
		res = new ostrstream;
		*res << ends;
	} else if (strcmp(command, "load_patch") == 0) {
		if (!LoadPatch(argc, argv))
			return (0);
		res = new ostrstream;
		*res << ends;
	} else if (strcmp(command, "delete_patch") == 0) {
		if (!DeletePatch(argc, argv))
			return (0);
		res = new ostrstream;
		*res << ends;
	} else {
		err << "unsupported feature: " << command << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}
	return (res);
}

int
TclmDriver75::Slave(const MidiDevice &md)
{
	ostrstream err;
	int id;
	TclmDriver75 *master;
	char *str;

	master = (TclmDriver75 *)&md;

	/* get the master's id */
	if (ioctl(master->fd, MGETID, &id) == -1) {
		err << "id ioctl failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}

	/* now make ourselves its slave */
	if (ioctl(fd, MSLAVE, &id) == -1) {
		err << "slave ioctl failed: " << strerror(errno) << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}

	return (1);
}

int
TclmDriver75::GetTime(unsigned long *t)
{
	ostrstream err;
	char *str;

	if (ioctl(fd, MGSMFTIME, t) == -1) {
		err << "ioctl MGSMFTIME failed: " << strerror(errno)
		    << ends;
		str = err.str();
		SetError(str);
		delete str;
		return (0);
	}
	return (1);
}

int
TclmDriver75::Send(Event **events, int num)
{
	SMFTrack smf;
	ostrstream buf;
	unsigned long junk;
	long bytes_written, len;
	int i, raw;
	const char *errstr;
	char *str;


	if (fd == -1) {
		SetError("Device not open");
		return (0);
	}

	if (ioctl(fd, MGRAW, &raw) == -1) {
		buf << "MGRAW ioctl failed: " << strerror(errno) << ends;
		str = buf.str();
		SetError(str);
		delete str;
		return (0);
	}

	if (!raw) {
		SetError("Can't send events to timed device - use raw device "
		    "instead.");
		return (0);
	}

	for (i = 0; i < num; i++) {
		if (!WriteEventToSMFTrack(smf, junk, events[i], 0, errstr)) {
			SetError(errstr);
			return (0);
		}
	}

	len = smf.GetLength();
	bytes_written = write(fd, smf.GetData(len), len);
	if (bytes_written != len) {
		buf << "TclmDriver75: write wrote wrong amount";
		if (bytes_written == -1)
			buf << ": " << strerror(errno) << ends;
		else
			buf << " " << bytes_written <<
			    " of " << len << ends;
		str = buf.str();
		SetError(str);
		delete str;
		return (0);
	}
	return (1);
}

int
TclmDriver75::Recv(Event ***events, int *num)
{
	SMFTrack track, dup_track;
	Event **event_ptr;
	ostrstream buf;
	unsigned long junk;
	int i, num_events, num_read, raw;
	unsigned char event[MaxEventSize];
	const char *errstr;
	char *str;

	if (fd == -1) {
		SetError("Device not open");
		return (0);
	}

	if (ioctl(fd, MGRAW, &raw) == -1) {
		buf << "MGRAW ioctl failed: " << strerror(errno) << ends;
		str = buf.str();
		SetError(str);
		delete str;
		return (0);
	}

	if (!raw) {
		SetError("Can't recv events to timed device - use raw device "
		    "instead.");
		return (0);
	}

	do {
		if ((num_read = read(fd, event, MaxEventSize)) == -1) {
			buf << "error reading TclmDriver75 event: " <<
			    strerror(errno) << ends;
			str = buf.str();
			SetError(str);
			delete str;
			return (0);
		}
		track.PutData(event, num_read);
	} while (num_read == MaxEventSize);

	dup_track = track;

	// count how many events are in the track;
	num_events = 0;
	while (ReadEventFromSMFTrack(dup_track, junk, 0, errstr) != 0)
		num_events++;

	event_ptr = new Event *[num_events];
	if (event_ptr == 0) {
		SetError("Out of memory");
		return (0);
	}

	// get the actual events
	for (i = 0; i < num_events; i++)
		event_ptr[i] = ReadEventFromSMFTrack(track, junk, 0, errstr);

	*events = event_ptr;
	*num = num_events;
	return (1);
}

int
TclmDriver75::SetChannelMask(int mask)
{
	ostrstream buf;
	char *str;
	unsigned short us_mask;

	if (fd == -1) {
		SetError("Device not open");
		return (0);
	}
	us_mask = mask;
	if (ioctl(fd, MSCMASK, &us_mask) == -1) {
		buf << "MSCMASK ioctl failed: " << strerror(errno) << ends;
		str = buf.str();
		SetError(str);
		delete str;
		return (0);
	}
	return (1);
}

int
TclmDriver75::GetChannelMask(void)
{
	ostrstream buf;
	char *str;
	unsigned short us_mask;

	if (fd == -1) {
		SetError("Device not open");
		return (-1);
	}
	if (ioctl(fd, MGCMASK, &us_mask) == -1) {
		buf << "MGCMASK ioctl failed: " << strerror(errno) << ends;
		str = buf.str();
		SetError(str);
		delete str;
		return (-1);
	}
	return (us_mask);
}

void
TclmDriver75Event(ClientData cd, int mask)
{
	int res;
	TclmDriver75 *mpu;

	mpu = (TclmDriver75 *)cd;

	if (mask & TCL_READABLE) {
		if (!mpu->ReadEvents()) {
			if (mpu->interp == 0)
				cerr << mpu->GetError() << "\n";
			else {
				Tcl_SetResult(mpu->interp,
				    (char *)mpu->GetError(), TCL_VOLATILE);
				Tcl_BackgroundError(mpu->interp);
			}
		}
	}
	if (mask & TCL_WRITABLE) {
		if ((res = mpu->WriteEvents()) == 0)
			mpu->finished = 1;
		else if (res == -1) {
			if (mpu->interp == 0)
				cerr << mpu->GetError() << "\n";
			else {
				Tcl_SetResult(mpu->interp,
				    (char *)mpu->GetError(), TCL_VOLATILE);
				Tcl_BackgroundError(mpu->interp);
			}
		}
	}
	if (mask & TCL_EXCEPTION) {
		if (!mpu->TimeChanged()) {
			if (mpu->interp == 0)
				cerr << mpu->GetError() << "\n";
			else {
				Tcl_SetResult(mpu->interp,
				    (char *)mpu->GetError(), TCL_VOLATILE);
				Tcl_BackgroundError(mpu->interp);
			}
		}
	}
}

int
TclmDriver75::TimeChanged()
{
	unsigned long new_time;
	int i;

	if (ioctl(fd, MGSMFTIME, &new_time) == -1) {
		/*
		 * we can't update, but we still need
		 * to send events
		 */
		if (WriteEvents() == -1)
			return (0);
	}

	/* redo curr_event pointers based on new time */
	for (i = 0; i < play_song->GetNumTracks(); i++)
		curr_event[i] = play_song->GetTrack(i).GetEventsNoMod(new_time);
	last_play_time = new_time;
	last_rec_time = new_time;
	/* now write the new events */
	if (WriteEvents() == -1)
		return (0);
	finished = 0;
	return (1);
}

int
TclmDriver75::WriteEvents(void)
{
	SMFTrack smf;
	unsigned long low_time;
	long len;
	int bytes_written, i, low_index, numtowrite, numwritten, pq;
	const char *errstr;
	ostrstream buf;
	char *str;

	if (ioctl(fd, MGQAVAIL, &numtowrite) == -1) {
		buf << "TclmDriverThread: ioctl MGQAVAIL failed: "
		    << strerror(errno) << ends;
		str = buf.str();
		SetError(str);
		delete str;
		return (-1);
	}
	for (numwritten = 0; numwritten < numtowrite;) {
		// find track with lowest timed event
		low_index = -1;
		low_time = 0;
		for (i = 0; i < play_song->GetNumTracks(); i++) {
			if (curr_event[i] == 0)
				continue;
			if (low_index == -1) {
				low_time = curr_event[i]->GetTime();
				low_index = i;
				continue;
			} else {
				if (curr_event[i]->GetTime() < low_time) {
					low_time = curr_event[i]->GetTime();
					low_index = i;
				}
			}
		}
		// Check to see if we're done
		if (low_index == -1) {
			if (!GetRepeat()) {
				// write any remaining events
				if (numwritten != 0) {
					len = smf.GetLength();
					bytes_written = write(fd,
					    smf.GetData(len), len);
					if (bytes_written != len) {
						buf << "TclmDriver75: write "
						    "wrote wrong amount";
						if (bytes_written == -1)
							buf << ": " <<
							    strerror(errno) <<
							    ends;
						else {
							buf << " " <<
							    bytes_written
							    << " of " <<
							    len << ends;
						}
						str = buf.str();
						SetError(str);
						delete str;
						return (-1);
					}
					return (numwritten);
				} else {
					/*
					 * if play queue is empty, we're
					 * done, otherwise do nothing
					 */
					if (ioctl(fd, MGPLAYQ, &pq) == -1) {
						buf << "TclmDriver75: MGPLAYQ "
						    "failed: " <<
						    strerror(errno) << ends;
						str = buf.str();
						SetError(str);
						delete str;
						return (-1);
					}
					if (pq == 0)
						return (0);
					else
						return (-1);
				}
			}
			// otherwise reset event pointers
			for (i = 0; i < play_song->GetNumTracks(); i++)
				curr_event[i] = play_song->GetTrack(i).
				    GetEventsNoMod(0);
			last_play_time = 0;
			continue;
		}
		// Write events until time changes
		while (curr_event[low_index] != 0 &&
		    curr_event[low_index]->GetTime() == low_time &&
		    numwritten < numtowrite) {
			if (!WriteEventToSMFTrack(smf, last_play_time,
			    curr_event[low_index], 1, errstr)) {
				buf << "error playing: " << errstr << ends;
				str = buf.str();
				SetError(str);
				delete str;
				return (-1);
			}
			numwritten++;
			curr_event[low_index] = play_song->GetTrack(low_index).
			    NextEvent(curr_event[low_index]);
		}
	}
	if (numwritten != 0) {
		len = smf.GetLength();
		bytes_written = write(fd, smf.GetData(len), len);
		if (bytes_written != len) {
			buf << "TclmDriver75: write wrote wrong amount";
			if (bytes_written == -1)
				buf << ": " << strerror(errno) << ends;
			else
				buf << " " << bytes_written <<
				    " of " << len << ends;
			str = buf.str();
			SetError(str);
			delete str;
			return (-1);
		}
	}
	return (numwritten);
}

int
TclmDriver75::ReadEvents(void)
{
	SMFTrack track;
	int num_read;
	EventType etype;
	unsigned char event[MaxEventSize];
	const char *errstr;
	Event *e, *first_e;
	ostrstream buf;
	char *str;

	do {
		if ((num_read = read(fd, event, MaxEventSize)) == -1) {
			buf << "error reading TclmDriver75 event: " <<
			    strerror(errno) << ends;
			str = buf.str();
			SetError(str);
			delete str;
			return (0);
		}
		track.PutData(event, num_read);
	} while (num_read == MaxEventSize);

	track.SetRunningState(last_record_rs);
	first_e = 0;
	while ((e = ReadEventFromSMFTrack(track, last_rec_time, 1, errstr))
	    != 0) {
		Event *new_e;

		if (first_e == 0)
			first_e = e;
		new_e = rec_song->PutEvent(0, *e);
		delete e;
		if (new_e == 0)
			continue;
		etype = new_e->GetType();
		// put links on noteoffs
		if ((etype == NOTEON && ((NoteEvent *)new_e)->GetVelocity()
		    == 0) || etype == NOTEOFF)
			rec_song->SetNotePair(0, new_e);
	}
	if (errstr != 0) {
		buf << "Error while recording: " << errstr << ends;
		str = buf.str();
		SetError(str);
		delete str;
		return (0);
	}
	last_record_rs = track.GetRunningState();
	return (1);
}

int
TclmDriver75::LoadPatch(int argc, const char *argv[])
{
	ostrstream err;
	struct gravis_patch p;
	struct midi_feature feature;
	int i, m;
	GusPatchFile *patch;
	const GusWave *w;
	char *serr;

	if (argc != 3) {
		SetError("wrong # args: should be \"{load_patch PatchID "
		    "PatchType ProgramNum}\"");
		return (0);
	}
	patch = tclm_interp->GetPatch(argv[0]);
	if (patch == 0) {
		err << "Bad PatchID \"" << argv[0] << "\"" << ends;
		serr = err.str();
		SetError(serr);
		delete serr;
		return (0);
	}

	if (strcmp(argv[1], "drum") == 0)
		p.drum = 1;
	else if (strcmp(argv[1], "melodic") == 0)
		p.drum = 0;
	else {
		SetError("bad PatchType.  Must be \"drum\" or \"melodic\"");
		return (0);
	}

	if (Tcl_GetInt(interp, (char *)argv[2], &p.program) != TCL_OK) {
		err << "Bad program number: \"" << argv[2] << "\"" << ends;
		serr = err.str();
		SetError(serr);
		delete serr;
		return (0);
	}

	p.num_waves = patch->GetNumWaves();
	p.waves = new struct gravis_wave [p.num_waves];
	if (p.waves == 0) {
		SetError("Out of memory");
		return (0);
	}

	w = patch->GetWaves();
	for (i = 0; i < p.num_waves; i++) {
		p.waves[i].len = w[i].GetWaveSize();
		p.waves[i].loop_start = w[i].GetStartLoop();
		p.waves[i].loop_end = w[i].GetEndLoop();
		p.waves[i].volume = patch->GetHeader()->GetMasterVolume();
		p.waves[i].low_freq = w[i].GetLowFrequency();
		p.waves[i].high_freq = w[i].GetHighFrequency();
		p.waves[i].root_freq = w[i].GetRootFrequency();
		p.waves[i].sr = w[i].GetSampleRate();
		m = w[i].GetModes();
		if (m & GUS_WAVE_16_BIT)
			p.waves[i].data_8_bit = 0;
		else
			p.waves[i].data_8_bit = 1;
		if (m & GUS_WAVE_UNSIGNED)
			p.waves[i].data_unsigned = 1;
		else
			p.waves[i].data_unsigned = 0;
		if (m & GUS_WAVE_LOOPING)
			p.waves[i].looping = 1;
		else
			p.waves[i].looping = 0;
		p.waves[i].data = new unsigned char [p.waves[i].len];
		if (p.waves[i].data == 0) {
			for (; i >= 0; i--)
				delete p.waves[i].data;
			delete p.waves;
			SetError("Out of memory");
			return (0);
		}
		memcpy(p.waves[i].data, w[i].GetWaveData(), p.waves[i].len);
	}

	feature.type = MFEAT_LOAD_PATCH;
	feature.data = &p;
	if (ioctl(fd, MFEATURE, &feature) == -1) {
		err << "feature ioctl failed: " << strerror(errno) << ends;
		serr = err.str();
		SetError(serr);
		delete serr;
		for (i = 0; i < p.num_waves; i++)
			delete p.waves[i].data;
		delete p.waves;
		return (0);
	}
	return (1);
}

int
TclmDriver75::DeletePatch(int argc, const char *argv[])
{
	ostrstream err;
	struct gravis_patch p;
	struct midi_feature feature;
	char *serr;

	if (argc != 2) {
		SetError("wrong # args: should be \"delete_patch "
		    "PatchType ProgramNum\"");
		return (0);
	}

	if (strcmp(argv[0], "drum") == 0)
		p.drum = 1;
	else if (strcmp(argv[0], "melodic") == 0)
		p.drum = 0;
	else {
		SetError("bad PatchType.  Must be \"drum\" or \"melodic\"");
		return (0);
	}

	if (Tcl_GetInt(interp, (char *)argv[1], &p.program) != TCL_OK) {
		err << "Bad program number: \"" << argv[1] << "\"" << ends;
		serr = err.str();
		SetError(serr);
		delete serr;
		return (0);
	}

	p.num_waves = 0;
	p.waves = 0;

	feature.type = MFEAT_DELETE_PATCH;
	feature.data = &p;
	if (ioctl(fd, MFEATURE, &feature) == -1) {
		err << "feature ioctl failed: " << strerror(errno) << ends;
		serr = err.str();
		SetError(serr);
		delete serr;
		return (0);
	}
	return (1);
}

ostream &
operator<<(ostream &os, const TclmDriver75 &mpu)
{

	os << mpu.GetName();
	if (mpu.GetName() != 0)
		os << "Open " << mpu.GetName();
	if (mpu.play_song != 0)
		os << " Playing";
	if (mpu.rec_song != 0)
		os << " Recording";
	return (os);
}
#endif

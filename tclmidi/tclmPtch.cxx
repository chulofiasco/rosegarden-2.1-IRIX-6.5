/*-
 * Copyright (c) 1996 Michael B. Durian.  All rights reserved.
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
extern "C" {
#include <tcl.h>
}
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include "tclmidi.h"
#include "TclmIntp.h"

static int Tclm_PatchRead(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);
static int Tclm_PatchFree(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv);

int
Tclm_PatchInit(Tcl_Interp *interp, TclmInterp *tclm_interp)
{

	Tcl_CreateCommand(interp, "patchread", Tclm_PatchRead, tclm_interp, 0);
	Tcl_CreateCommand(interp, "patchfree", Tclm_PatchFree, tclm_interp, 0);
	return (TCL_OK);
}

int
Tclm_PatchRead(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	Tcl_Channel channel;
	int mode;
	ostrstream err;
	TclmInterp *tclm_interp;
	GusPatchFile *patch;
	char *key, *serr;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be\"",
		    argv[0], " FileID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	patch = new GusPatchFile;

	channel = Tcl_GetChannel(interp, argv[1], &mode);
	if (channel == 0)
		return (TCL_ERROR);
	if ((mode & TCL_READABLE) == 0) {
		Tcl_AppendResult(interp, "channel \"", argv[1],
		    "\" wasn't opened for reading", 0);
		return (TCL_ERROR);
	}
	Tcl_SetChannelOption(interp, channel, "-translation", "binary");
	if (!patch->Read(channel, err)) {
		serr = err.str();
		Tcl_AppendResult(interp, "couldn't read GUS patch file ",
		    argv[1], ": ", serr, 0);
		delete serr;
		delete patch;
		return (TCL_ERROR);
	}
	key = tclm_interp->AddPatch(patch);
	Tcl_SetResult(interp, key, TCL_VOLATILE);
	delete key;
	return (TCL_OK);
}

int
Tclm_PatchFree(ClientData client_data, Tcl_Interp *interp, int argc,
    char **argv)
{
	TclmInterp *tclm_interp;

	if (argc != 2) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " MidiID\"", 0);
		return (TCL_ERROR);
	}
	tclm_interp = (TclmInterp *)client_data;

	if (!tclm_interp->DeletePatch(argv[1])) {
		Tcl_AppendResult(interp, "Bad key ", argv[1], 0);
		return (TCL_ERROR);
	}
	return (TCL_OK);
}

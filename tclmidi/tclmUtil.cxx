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
extern "C" {
#include <tcl.h>
}
#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/*
 * This is lifted from Tcl_GetInt in tclGet.c - I just want to make
 * sure that there is a case that correctly handles longs.  Yes,
 * on most UNIX systems an int is a long, but it is not guaranteed,
 * and I don't know about other OSs.
 */
int
Tcl_GetLong(Tcl_Interp *interp, const char *string, long *longPtr)
{
    const char *p;
    char *end;
    long i;

    /*
     * Note: use strtoul instead of strtol for integer conversions
     * to allow full-size unsigned numbers, but don't depend on strtoul
     * to handle sign characters;  it won't in some implementations.
     */

    for (p = string; isspace(*p); p++) {
	/* Empty loop body. */
    }
    if (*p == '-') {
	i = -strtoul(p+1, &end, 0);
    } else if (*p == '+') {
	i = strtoul(p+1, &end, 0);
    } else {
	i = strtoul(p, &end, 0);
    }
    while ((*end != '\0') && isspace(*end)) {
	end++;
    }
    if ((end == string) || (*end != 0)) {
	Tcl_AppendResult(interp, "expected integer but got \"", string,
		"\"", (char *) NULL);
	return TCL_ERROR;
    }
    *longPtr = i;
    return TCL_OK;
}

int
Tclm_ParseDataByte(Tcl_Interp *interp, const char *str, int *val)
{

	if (Tcl_GetInt(interp, (char *)str, val) != TCL_OK)
		return (0);
	if (*val < 0) {
		Tcl_AppendResult(interp, "value ", str, " too small, must "
		    "be between 0 and 127 inclusive", 0);
		return (0);
	}
	if (*val > 127) {
		Tcl_AppendResult(interp, "value ", str, " too large, must "
		    "be between 0 and 127 inclusive", 0);
		return (0);
	}
	return (1);
}

void
Tclm_PrintData(ostream &buf, const unsigned char *data, long length)
{
	long i;

	buf.setf(ios::showbase | ios::internal);
	buf << hex << setw(4) << setfill('0') << (int)data[0];
	for (i = 1; i < length; i++)
		buf << " " << hex << setw(4) << setfill('0') << (int)data[i];
}

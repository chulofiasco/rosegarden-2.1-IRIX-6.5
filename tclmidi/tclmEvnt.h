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
#ifndef TCLMEVENT_H
#define TCLMEVENT_H

#include <tcl.h>
#include "AllEvent.h"

extern void Tclm_PrintEvent(ostream &buf, Event *e);
extern char *Tclm_PrintNoteOff(NoteOffEvent *e);
extern char *Tclm_PrintNoteOn(NoteOnEvent *e);
extern char *Tclm_PrintNote(NoteOnEvent *e);
extern char *Tclm_PrintKeyPressure(KeyPressureEvent *e);
extern char *Tclm_PrintParameter(ParameterEvent *e);
extern char *Tclm_PrintProgram(ProgramEvent *e);
extern char *Tclm_PrintChannelPressure(ChannelPressureEvent *e);
extern char *Tclm_PrintPitchWheel(PitchWheelEvent *e);
extern char *Tclm_PrintSystemExclusive(SystemExclusiveEvent *e);
extern char *Tclm_PrintMetaSequenceNumber(MetaSequenceNumberEvent *e);
extern char *Tclm_PrintMetaText(MetaTextEvent *e);
extern char *Tclm_PrintMetaCopyright(MetaCopyrightEvent *e);
extern char *Tclm_PrintMetaSequenceName(MetaSequenceNameEvent *e);
extern char *Tclm_PrintMetaInstrumentName(MetaInstrumentNameEvent *e);
extern char *Tclm_PrintMetaLyric(MetaLyricEvent *e);
extern char *Tclm_PrintMetaMarker(MetaMarkerEvent *e);
extern char *Tclm_PrintMetaCue(MetaCueEvent *e);
extern char *Tclm_PrintMetaChannelPrefix(MetaChannelPrefixEvent *e);
extern char *Tclm_PrintMetaPortNumber(MetaPortNumberEvent *e);
extern char *Tclm_PrintMetaEndOfTrack(MetaEndOfTrackEvent *e);
extern char *Tclm_PrintMetaTempo(MetaTempoEvent *e);
extern char *Tclm_PrintMetaSMPTE(MetaSMPTEEvent *e);
extern char *Tclm_PrintMetaTime(MetaTimeEvent *e);
extern char *Tclm_PrintMetaKey(MetaKeyEvent *e);
extern char *Tclm_PrintMetaSequencerSpecific(MetaSequencerSpecificEvent *e);
extern char *Tclm_PrintMetaUnknown(MetaUnknownEvent *e);
extern Event *Tclm_ParseEvent(Tcl_Interp *interp, char *str);
extern Event *Tclm_ParseNoteOff(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseNoteOn(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseNote(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseKeyPressure(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseParameter(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseProgram(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseChannelPressure(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParsePitchWheel(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseSystemExclusive(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaSequenceNumber(Tcl_Interp *interp, long time,
    int argc, char **argv);
extern Event *Tclm_ParseMetaText(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaCopyright(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaSequenceName(Tcl_Interp *interp, long time,
    int argc, char **argv);
extern Event *Tclm_ParseMetaInstrumentName(Tcl_Interp *interp, long time,
    int argc, char **argv);
extern Event *Tclm_ParseMetaLyric(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaMarker(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaCue(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaChannelPrefix(Tcl_Interp *interp, long time,
    int argc, char **argv);
extern Event *Tclm_ParseMetaPortNumber(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaEndOfTrack(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaTempo(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaSMPTE(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaTime(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaKey(Tcl_Interp *interp, long time, int argc,
    char **argv);
extern Event *Tclm_ParseMetaSequencerSpecific(Tcl_Interp *interp, long time,
    int argc, char **argv);
extern Event *Tclm_ParseMetaUnknown(Tcl_Interp *interp, long time, int argc,
    char **argv);
#endif

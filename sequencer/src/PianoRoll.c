/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           PianoRoll.c
 *
 *    Description:    Code to manage piano roll style subwindows for display
 *                    tracks in the MIDI file.
 *
 *
 *    Author:         AJG
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     15/03/94        AJG             File Created.
 *    002     13/10/96        rwb             Scrollable Piano Roll with keyboard,
 *                                            velocity bars and zooming.
 *
 *
 */


#include <MidiXInclude.h>
#include <MidiFile.h>
#include <MidiErrorHandler.h>

#include "Globals.h"
#include "Consts.h"
#include "Main.h"
#include "PianoRoll.h"
#include "PianoRollMenu.h"
#include "EventListMenu.h"
#include "Parse.h"
#include "Menu.h"
#include "Clipboard.h"
#include "Undo.h"

#include <Debug.h>

/********************************/
/* Private function prototypes. */
/********************************/

void Midi_PianoRollDrawBackground(PRWindowList ParentWindow);
void Midi_PianoRollDrawNotes(PRWindowList ParentWindow);

PRWindowList    MIDIPianoRollWindows = NULL;

extern char **TrackListEntries;

short Midi_NoteSpacingTable[] = { 0,1,0,1,1,0,1,0,1,0,1,1 };
short Midi_NoteSharpsTable[]  = { 0,1,0,1,0,0,1,0,1,0,1,0 };

int Midi_PianoRollCalculateNoteYPos(byte Note, float Zoom)
{
int     Octave;
int     PitchMod;
int     Counter;
float    YCalc;
float   KeyboardNoteY;

BEGIN("Midi_PianoRollCalculateNoteYPos");

    Octave   = Note / 12;
    PitchMod = Note % 12;

    Counter = 0;

    KeyboardNoteY = (int) ( PIANO_ROLL_OCTAVE_YSIZE / 7 );

    YCalc = PIANO_ROLL_TOP_STAVE_OFFSET - 4 * PIANO_ROLL_OCTAVE_YSIZE +
            ( 2.5 * KeyboardNoteY );

    YCalc += ( (11 - Octave) * 7 * KeyboardNoteY);
    while(PitchMod)
    {
        YCalc -= ( KeyboardNoteY * Midi_NoteSpacingTable[Counter++] ) ;
        --PitchMod;
    }

RETURN_INT(YCalc);
}

void Midi_PianoRollSetupGCs(PRWindowList NewWindow)
{
XGCValues GCSetupBuffer;
unsigned long   GCMask = GCFunction | GCPlaneMask | 
                         GCFillStyle | GCStipple |
                         GCLineWidth;

BEGIN("Midi_PianoRollSetupGCs");

  /* cc, 6/96: only create one set of GCs and share, instead of
     creating five per PianoRoll window without ever freeing them */

  if (MIDIPianoRollWindows)
    {
      MIDIPianoRollWindows = (PRWindowList)First(MIDIPianoRollWindows);
      NewWindow->Clear      = MIDIPianoRollWindows->Clear;
      NewWindow->DrawForce  = MIDIPianoRollWindows->DrawForce;
      NewWindow->DrawXor    = MIDIPianoRollWindows->DrawXor;
      NewWindow->LightGrey  = MIDIPianoRollWindows->LightGrey;
      NewWindow->Grey       = MIDIPianoRollWindows->Grey;
    }
  else
    {

      XGetGCValues(XtDisplay(topLevel), 
           DefaultGCOfScreen(XtScreen(topLevel)), 
           GCMask,
           &GCSetupBuffer);

      GCSetupBuffer.line_width = 0;
      GCSetupBuffer.stipple    = LightGrey;

      NewWindow->Clear = YCreateGC(GCMask, &GCSetupBuffer, NoShade, True);

      GCSetupBuffer.function = GXcopy;
    
      NewWindow->DrawForce = YCreateGC(GCMask, &GCSetupBuffer, NoShade, False);
    
      GCSetupBuffer.function = GXxor;
    
      NewWindow->DrawXor = YCreateGC(GCMask, &GCSetupBuffer, NoShade, False);
    
      GCSetupBuffer.function   = GXcopy;
      GCSetupBuffer.fill_style = FillOpaqueStippled;
      
      NewWindow->LightGrey = YCreateGC(GCMask, &GCSetupBuffer, NoShade, False);
      
      GCSetupBuffer.stipple = Grey;
      
      NewWindow->Grey = YCreateGC(GCMask, &GCSetupBuffer, NoShade, False);

    }

END;
}

static XExposeEvent event = { Expose, 0L, True, NULL, NULL, 0, 0, 300, 200, 0 };

void Midi_PianoRollScrollbarSetThumbPos(PRWindowList);

void Midi_PianoRollForwardCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PianoRollForwardCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    if (ParentWindow->BarNumber * 4 * MIDIHeaderBuffer.Timing.Division >= 
        ParentWindow->LastEvtTime) END;

    ++ParentWindow->BarNumber;

    Midi_PianoRollDrawBackground(ParentWindow);
    Midi_PianoRollDrawNotes(ParentWindow);

    event.display = XtDisplay(ParentWindow->PianoRollLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollLabel),
           XtWindow(ParentWindow->PianoRollLabel),  False,
           ExposureMask, (XEvent *)&event);

    event.display = XtDisplay(ParentWindow->PianoRollStrengthLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollStrengthLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollStrengthLabel),
           XtWindow(ParentWindow->PianoRollStrengthLabel),  False,
           ExposureMask, (XEvent *)&event);

    Midi_PianoRollScrollbarSetThumbPos(ParentWindow);
END;
}


void Midi_PianoRollBackwardCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;

BEGIN("Midi_PianoRollBackwardCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    if (ParentWindow->BarNumber == 0) END;

    --ParentWindow->BarNumber;

    Midi_PianoRollDrawBackground(ParentWindow);
    Midi_PianoRollDrawNotes(ParentWindow);

    event.display = XtDisplay(ParentWindow->PianoRollLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollLabel),
           XtWindow(ParentWindow->PianoRollLabel),  False,
           ExposureMask, (XEvent *)&event);

    event.display = XtDisplay(ParentWindow->PianoRollStrengthLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollStrengthLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollStrengthLabel),
           XtWindow(ParentWindow->PianoRollStrengthLabel),  False,
           ExposureMask, (XEvent *)&event);

    Midi_PianoRollScrollbarSetThumbPos(ParentWindow);
END;
}


void Midi_PianoRollFfwdCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;
int         FfwdDist;
float        Zoom;

BEGIN("Midi_PianoRollFfwdCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    Zoom = ParentWindow->Zoom;
    FfwdDist = (ParentWindow->PianoRollWidth / PIANO_ROLL_DEFAULT_BAR_WIDTH);
    ParentWindow->BarNumber  += FfwdDist;

    if (ParentWindow->BarNumber * 4 * MIDIHeaderBuffer.Timing.Division >= 
        ParentWindow->LastEvtTime)
    {
        ParentWindow->BarNumber -= FfwdDist;
        END;
    }

    Midi_PianoRollDrawBackground(ParentWindow);
    Midi_PianoRollDrawNotes(ParentWindow);

    event.display = XtDisplay(ParentWindow->PianoRollLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollLabel),
           XtWindow(ParentWindow->PianoRollLabel),  False,
           ExposureMask, (XEvent *)&event);

    event.display = XtDisplay(ParentWindow->PianoRollStrengthLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollStrengthLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollStrengthLabel),
           XtWindow(ParentWindow->PianoRollStrengthLabel),  False,
           ExposureMask, (XEvent *)&event);

    Midi_PianoRollScrollbarSetThumbPos(ParentWindow);
END;
}


void Midi_PianoRollRewindCB(Widget w, XtPointer a, XtPointer b)
{
PRWindowList ParentWindow;
float Zoom;

BEGIN("Midi_PianoRollRewindCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    Zoom = ParentWindow->Zoom;
    ParentWindow->BarNumber -= (ParentWindow->PianoRollWidth
                                /PIANO_ROLL_DEFAULT_BAR_WIDTH);

    if (ParentWindow->BarNumber < 0) ParentWindow->BarNumber = 0;

    Midi_PianoRollDrawBackground(ParentWindow);
    Midi_PianoRollDrawNotes(ParentWindow);

    event.display = XtDisplay(ParentWindow->PianoRollLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollLabel),
           XtWindow(ParentWindow->PianoRollLabel),  False,
           ExposureMask, (XEvent *)&event);

    event.display = XtDisplay(ParentWindow->PianoRollStrengthLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollStrengthLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollStrengthLabel),
           XtWindow(ParentWindow->PianoRollStrengthLabel),  False,
           ExposureMask, (XEvent *)&event);

    Midi_PianoRollScrollbarSetThumbPos(ParentWindow);
END;
}


void Midi_PianoRollScrollbarInit(PRWindowList ParentWindow)
{
float      ThumbSize = 1.0;
Arg      arg;
XtArgVal *ld;
float     Zoom;


BEGIN("Midi_PianoRollScrollbarInit");

        Zoom = ParentWindow->Zoom;

        if (ParentWindow->LastEvtTime > 0) {

      ThumbSize = ((float)(ParentWindow->PianoRollWidth / PIANO_ROLL_DEFAULT_BAR_WIDTH) *
               4 * MIDIHeaderBuffer.Timing.Division) / ParentWindow->LastEvtTime;
    } else ThumbSize = 1.0;

    if (ThumbSize < 0) ThumbSize = 0.0;
    if (ThumbSize > 1) ThumbSize = 1.0;

    if (sizeof(float) > sizeof(XtArgVal)) 
    {
        XtSetArg(arg, XtNshown, &ThumbSize);
    }
    else 
    {
            ld = (XtArgVal *)&ThumbSize;
            XtSetArg(arg, XtNshown, *ld);
    }

    XtSetValues(ParentWindow->Scrollbar, &arg, 1);


END;
}

void Midi_PianoRollScrollbarSetThumbPos(PRWindowList ParentWindow)
{
Arg       arg;
XtArgVal *ld;
float      Top;

BEGIN("Midi_PianoRollScrollbarSetThumbPos");

        if (ParentWindow->LastEvtTime > 0) {

      Top = ((float)ParentWindow->BarNumber * (4 * MIDIHeaderBuffer.Timing.Division) / 
         ParentWindow->LastEvtTime);

    } else Top = 0;

    if (sizeof(float) > sizeof(XtArgVal)) XtSetArg(arg, XtNtopOfThumb, &Top);
    else
    {
        ld = (XtArgVal *)&Top;
        XtSetArg(arg, XtNtopOfThumb, *ld);
    }

    XtSetValues(ParentWindow->Scrollbar, &arg, 1);


    ParentWindow->Dragging          = False;
    ParentWindow->DragStartCoord = 0;
    ParentWindow->DragEndCoord   = 0;

END;
}

void Midi_PianoRollScrollbarJumpCB(Widget w, XtPointer a, XtPointer CBdata)
{
float         position;
PRWindowList    ParentWindow;
int        BarNumber;

BEGIN("Midi_PianoRollScrollbarJumpCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    position = *((float *)CBdata);

    
    BarNumber = position * ParentWindow->LastEvtTime / (4 * MIDIHeaderBuffer.Timing.Division);

    if (ParentWindow->BarNumber == BarNumber) END;

    ParentWindow->BarNumber = BarNumber;

    Midi_PianoRollDrawBackground(ParentWindow);
    Midi_PianoRollDrawNotes(ParentWindow);

    event.display = XtDisplay(ParentWindow->PianoRollLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollLabel),
           XtWindow(ParentWindow->PianoRollLabel),  False,
           ExposureMask, (XEvent *)&event);

    event.display = XtDisplay(ParentWindow->PianoRollStrengthLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollStrengthLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollStrengthLabel),
           XtWindow(ParentWindow->PianoRollStrengthLabel),  False,
           ExposureMask, (XEvent *)&event);

END;
}



void Midi_PianoRollScrollbarScrollCB(Widget w, XtPointer a, XtPointer CBdata)
{
PRWindowList ParentWindow;
Dimension Length;
int Delta;
int Pos;
float Zoom;

BEGIN("Midi_PianoRollScrollbarScrollCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    Zoom = ParentWindow->Zoom;
    if (!ParentWindow) END;
    Pos = (int)CBdata;

    YGetValue(ParentWindow->Scrollbar, XtNlength, &Length);

    Delta = (ParentWindow->PianoRollWidth / PIANO_ROLL_DEFAULT_BAR_WIDTH) * -((float)Pos / Length);

    ParentWindow->BarNumber += Delta;

    if (ParentWindow->BarNumber < 0) ParentWindow->BarNumber = 0;

    if (ParentWindow->BarNumber * 4 * MIDIHeaderBuffer.Timing.Division >= 
        ParentWindow->LastEvtTime)
    {
        ParentWindow->BarNumber = ParentWindow->LastEvtTime / (4 * MIDIHeaderBuffer.Timing.Division);
    }

    Midi_PianoRollScrollbarSetThumbPos(ParentWindow);
    Midi_PianoRollDrawBackground(ParentWindow);
    Midi_PianoRollDrawNotes(ParentWindow);

    event.display = XtDisplay(ParentWindow->PianoRollLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollLabel),
           XtWindow(ParentWindow->PianoRollLabel),  False,
           ExposureMask, (XEvent *)&event);

    event.display = XtDisplay(ParentWindow->PianoRollStrengthLabel);
    event.window  =  XtWindow(ParentWindow->PianoRollStrengthLabel);

    XSendEvent(XtDisplay(ParentWindow->PianoRollStrengthLabel),
           XtWindow(ParentWindow->PianoRollStrengthLabel),  False,
           ExposureMask, (XEvent *)&event);

    ParentWindow->Dragging          = False;
    ParentWindow->DragStartCoord = 0;
    ParentWindow->DragEndCoord   = 0;


END;
}


int Midi_GetPitch(int PositionX, int PositionY, float Zoom)
{
int       Ytemp;
int       Counter = 0;
int       Pitch = 0;
float       KeyboardNoteY;

    BEGIN("Midi_GetPitch");

    /* Extract the Note position from the Pointer Position */
    KeyboardNoteY = (int) ( PIANO_ROLL_OCTAVE_YSIZE / 7 );

    Ytemp = PositionY;
    Ytemp -= PIANO_ROLL_TOP_STAVE_OFFSET - 4 * PIANO_ROLL_OCTAVE_YSIZE + ( 3.5 * KeyboardNoteY ) + 1;
    while ((Ytemp - ( KeyboardNoteY * 7 )) > 0)
    {
        Pitch += 12;
        Ytemp -= ( KeyboardNoteY * 7 );
    }

    while((Ytemp - (KeyboardNoteY)) > 0)
    {
        Ytemp -= KeyboardNoteY;
        Counter++;
        Pitch++;
    }

    /* add in sharp conversion factors for the white notes */
    switch(Counter)
    {
        case 0:
            break;
        case 4:
            Pitch += 3;
            break;
        case 5:
            Pitch += 4;
            break;
        case 6:
            Pitch += 5;
            break;
        default:
            Pitch += Counter;
            break;
    }

    /* fraction of a black note */
    if ( Ytemp<= (KeyboardNoteY/3) && !(Counter == 4 || Counter == 6 || !Counter ) )
    {
       Pitch--;
    }
    else if ( Ytemp >=(2 * KeyboardNoteY/3) && !( Counter == 3 || !Counter ) )
    {
       Pitch++;
    }

    /* say 131 is the highest note we'll ever get on the down count
       we reverse this from the bottom C to the top G (127) */

    Pitch = 131 - Pitch;

    /* make sure we ignore all callbacks above 127 and below 0 */
    if ( Pitch < 0 || Pitch > 127 )
        RETURN_INT(-1);

#ifdef RWB_DEBUG
    fprintf(stderr," Pitch = %d, Remainder = %d, Counter = %d\n", 131 - Pitch, Ytemp, Counter);
#endif

    RETURN_INT(Pitch);
}
    


void Midi_PianoRollCalculateInNote(PRWindowList ParentWindow, int PositionX, int PositionY)
{
float     Zoom = ParentWindow->Zoom;
int       Pitch = 0;
long      NoteTime;
EventList CurrEvtPtr;

BEGIN("Midi_PianoRollCalculateInNote");

    /* get the pitch of any possible note */
    if ( ( Pitch = Midi_GetPitch(PositionX, PositionY, Zoom) ) < 0 )
        END;

    /* work out the X position */
    if ( ( NoteTime = PIANO_ROLL_TIMING_INFO(PositionX, ParentWindow->BarNumber) ) < 0 )
        END;

#ifdef RWB_DEBUG
    fprintf(stderr,"  at %d\n",(int)NoteTime);
#endif

    CurrEvtPtr = MIDITracks[ParentWindow->TrackNum];

    while(CurrEvtPtr)
    {
        if (MessageType(CurrEvtPtr->Event.EventCode) == MIDI_NOTE_ON &&
            NoteTime >= CurrEvtPtr->Event.DeltaTime &&
            ( NoteTime <= CurrEvtPtr->Event.DeltaTime + CurrEvtPtr->Event.EventData.Note.Duration ) &&
            CurrEvtPtr->Event.EventData.Note.Note == Pitch)
        {
            break;
        }
        else CurrEvtPtr = (EventList)Next(CurrEvtPtr);
    }

    /* dependent on the mode we can do things  - edit, add, delete etc. */

    if (!CurrEvtPtr) 
    {
        fprintf(stderr,"NEW!\n");
    }
    else
        fprintf(stderr,"HIT!!\n");

END;
}


void Midi_PianoRollCalculateInVelocity()
{
}


void Midi_PianoRollDrawSelectionBars(PRWindowList ParentWindow, GC drawGC)
{
Dimension LabelHeight;
Dimension Offset;
float     Zoom;

BEGIN("Midi_PianoRollDrawSelectionBars");
    if (!ParentWindow->DragStartCoord && !ParentWindow->DragEndCoord) END;

        Zoom = ParentWindow->Zoom;

    Offset = 0;
    YGetValue(ParentWindow->PianoRollLabel, XtNheight, &LabelHeight);

    if (LabelHeight > PIANO_ROLL_PIXMAP_HEIGHT)
    {
        Offset = (LabelHeight - PIANO_ROLL_PIXMAP_HEIGHT) / 2;
    }

/*

        if ( button press + release )
          if ( in_note )
             unhighlight any current note;
             highlight selected note;

          else 
             quantize x and y and display cursor;

        else if ( drag )
          if ( in_note )
            see if drag constraints mean note size change;
          else
            drag out an area for highlight
*/

        /* quantize the X position */
        /* quantize the Y position */

    if (IsMono && (ParentWindow->DragStartCoord != ParentWindow->DragEndCoord))
    {
/*
        XDrawLine(display, ParentWindow->LabelPixmap, ParentWindow->DrawXor,
              ParentWindow->DragStartCoord, 4, ParentWindow->DragEndCoord, 4);

        XDrawLine(display, ParentWindow->LabelPixmap, ParentWindow->DrawXor,
              ParentWindow->DragEndCoord, 4, 
              ParentWindow->DragEndCoord, PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, ParentWindow->LabelPixmap, ParentWindow->DrawXor,
              ParentWindow->DragEndCoord, PIANO_ROLL_PIXMAP_HEIGHT - 4,
              ParentWindow->DragStartCoord, PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, ParentWindow->LabelPixmap, ParentWindow->DrawXor,
              ParentWindow->DragStartCoord, PIANO_ROLL_PIXMAP_HEIGHT - 4,
              ParentWindow->DragStartCoord, 4);


        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), ParentWindow->DrawXor,
              ParentWindow->DragStartCoord, Offset + 4, 
              ParentWindow->DragEndCoord,   Offset + 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), ParentWindow->DrawXor,
              ParentWindow->DragEndCoord, Offset + 4, 
              ParentWindow->DragEndCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), ParentWindow->DrawXor,
              ParentWindow->DragEndCoord,   Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4,
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), ParentWindow->DrawXor,
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4,
              ParentWindow->DragStartCoord, Offset + 4);

        END;
*/
    }


    else if (ParentWindow->DragStartCoord != ParentWindow->DragEndCoord)
    {
/*
        XDrawLine(display, ParentWindow->LabelPixmap, drawGC, 
              ParentWindow->DragStartCoord, 8, ParentWindow->DragStartCoord, 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC, 
              ParentWindow->DragStartCoord, 8, ParentWindow->DragStartCoord, 4);

        XDrawLine(display, ParentWindow->LabelPixmap, drawGC, 
              ParentWindow->DragStartCoord, 4, ParentWindow->DragEndCoord, 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC, 
              ParentWindow->DragStartCoord, 4, ParentWindow->DragEndCoord, 4);

        XDrawLine(display, ParentWindow->LabelPixmap, drawGC, 
              ParentWindow->DragEndCoord, 8, ParentWindow->DragEndCoord, 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC, 
              ParentWindow->DragEndCoord, 8, ParentWindow->DragEndCoord, 4);


        XDrawLine(display, ParentWindow->LabelPixmap, drawGC, 
              ParentWindow->DragStartCoord, PIANO_ROLL_PIXMAP_HEIGHT - 8, 
                     ParentWindow->DragStartCoord, PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC, 
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 8, 
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, ParentWindow->LabelPixmap, drawGC, 
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4,
              ParentWindow->DragEndCoord,   Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC, 
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4,
              ParentWindow->DragEndCoord,   Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, ParentWindow->LabelPixmap, drawGC, 
              ParentWindow->DragEndCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 8,
              ParentWindow->DragEndCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC, 
              ParentWindow->DragEndCoord, PIANO_ROLL_PIXMAP_HEIGHT - 8, 
              ParentWindow->DragEndCoord, PIANO_ROLL_PIXMAP_HEIGHT - 4);
*/
    }
    else
    {
                /* select note */

/*
                Midi_PianoRollCalculateInNote();

        XDrawLine(display, ParentWindow->LabelPixmap, drawGC,
              ParentWindow->DragStartCoord, 8, ParentWindow->DragStartCoord - 4, 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC,
              ParentWindow->DragStartCoord,     Offset + 8, 
              ParentWindow->DragStartCoord - 4, Offset + 4);

        XDrawLine(display, ParentWindow->LabelPixmap, drawGC,
              ParentWindow->DragStartCoord, 8, ParentWindow->DragStartCoord + 4, 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC,
              ParentWindow->DragStartCoord,     Offset + 8, 
              ParentWindow->DragStartCoord + 4, Offset + 4);



        XDrawLine(display, ParentWindow->LabelPixmap, drawGC,
              ParentWindow->DragStartCoord, PIANO_ROLL_PIXMAP_HEIGHT - 8,
              ParentWindow->DragStartCoord - 4, PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC,
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 8,
              ParentWindow->DragStartCoord - 4, Offset + PIANO_ROLL_PIXMAP_HEIGHT -  4);

        XDrawLine(display, ParentWindow->LabelPixmap, drawGC,
              ParentWindow->DragStartCoord, PIANO_ROLL_PIXMAP_HEIGHT - 8,
              ParentWindow->DragStartCoord + 4, PIANO_ROLL_PIXMAP_HEIGHT - 4);

        XDrawLine(display, XtWindow(ParentWindow->PianoRollLabel), drawGC,
              ParentWindow->DragStartCoord, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 8,
               ParentWindow->DragStartCoord + 4, Offset + PIANO_ROLL_PIXMAP_HEIGHT - 4);
*/
    }

END;
}


void Midi_PRCheckResizeCB(Widget w, XtPointer ClientData, XEvent *e, Boolean *cont)
{
PRWindowList ParentWindow;
Dimension    CurrentWidth;
Dimension    CurrentHeight;
float        ZoomFactor, Zoom;

BEGIN("Midi_PRCheckResizeCB");

    if ( ( ParentWindow = (PRWindowList)ClientData ) == 0 )
            ParentWindow = Midi_PRGetWindowFromWidget(w);
        
        Zoom = ParentWindow->Zoom;

    YGetValue(ParentWindow->PianoRollLabel, XtNwidth,  &CurrentWidth);
    YGetValue(ParentWindow->PianoRollLabel, XtNheight, &CurrentHeight);

    if (CurrentWidth  == ParentWindow->PianoRollWidth &&
        CurrentHeight == ParentWindow->PianoRollHeight)
    {
            if ( e->type != -1 )
        END;
    }

        ZoomFactor = ParentWindow->Zoom / PIANO_ROLL_ZOOM_DEFAULT;

    if ( (CurrentWidth != ParentWindow->PianoRollWidth) || ( ParentWindow->Zoom > PIANO_ROLL_ZOOM_DEFAULT ) )
    {

        XFreePixmap(XtDisplay(ParentWindow->PianoRollLabel), ParentWindow->LabelPixmap);
        XFreePixmap(XtDisplay(ParentWindow->PianoRollLabel), ParentWindow->StrengthLabelPixmap);

        ParentWindow->PianoRollWidth  = CurrentWidth;

                if ( ParentWindow->Zoom < PIANO_ROLL_ZOOM_DEFAULT )
                {
            ParentWindow->LabelPixmap = XCreatePixmap(XtDisplay(ParentWindow->PianoRollLabel), 
                                         RootWindowOfScreen(XtScreen(ParentWindow->PianoRollLabel)),
                                        ParentWindow->PianoRollWidth * ZoomFactor,
                                        (PIANO_ROLL_PIXMAP_HEIGHT / ZoomFactor),
                                        DefaultDepthOfScreen(XtScreen(ParentWindow->PianoRollLabel)));
                }
                else
                {
            ParentWindow->LabelPixmap = XCreatePixmap(XtDisplay(ParentWindow->PianoRollLabel), 
                                         RootWindowOfScreen(XtScreen(ParentWindow->PianoRollLabel)),
                                        (ParentWindow->PianoRollWidth * ZoomFactor),
                                        (PIANO_ROLL_PIXMAP_HEIGHT),
                                        DefaultDepthOfScreen(XtScreen(ParentWindow->PianoRollLabel)));
                }

            ParentWindow->StrengthLabelPixmap = XCreatePixmap(XtDisplay(ParentWindow->PianoRollStrengthLabel), 
                                 RootWindowOfScreen(XtScreen(ParentWindow->PianoRollStrengthLabel)),
                                 (int)(ParentWindow->PianoRollWidth * ZoomFactor),
                                 (int)(PIANO_ROLL_VELOCITY_HEIGHT / ZoomFactor),
                                 DefaultDepthOfScreen(XtScreen(ParentWindow->PianoRollStrengthLabel)));

    }

    ParentWindow->PianoRollHeight = CurrentHeight;

    Midi_PianoRollDrawBackground(ParentWindow);
    Midi_PianoRollDrawNotes(ParentWindow);
    YSetValue(ParentWindow->PianoRollLabel, XtNbitmap, ParentWindow->LabelPixmap);
    YSetValue(ParentWindow->PianoRollStrengthLabel, XtNbitmap, ParentWindow->StrengthLabelPixmap);

    ParentWindow->Dragging          = False;
    ParentWindow->DragStartCoord = 0;
    ParentWindow->DragEndCoord   = 0;

END;
}


void Midi_PianoRollButtonPressCB(Widget w, XtPointer clientData, XEvent *e, Boolean *cont)
{
PRWindowList  ParentWindow;
XButtonEvent *ButtonEvent;

BEGIN("Midi_PianoRollButtonPressCB");
        
    ParentWindow = (PRWindowList)clientData;
    ButtonEvent = (XButtonEvent *)e;

    if (ParentWindow->Dragging || ButtonEvent->type != ButtonPress || ButtonEvent->button != Button1) END;

    Midi_PianoRollDrawSelectionBars(ParentWindow, ParentWindow->Clear);

    ParentWindow->Dragging = True;    

    ParentWindow->DragStartCoord = ButtonEvent->x;
    ParentWindow->DragEndCoord   = ButtonEvent->x;

        Midi_PianoRollCalculateInNote(ParentWindow, ButtonEvent->x, ButtonEvent->y);

    Midi_PianoRollDrawSelectionBars(ParentWindow, ParentWindow->DrawForce);

END;
}


void Midi_PianoRollZoomInCB(Widget w, XtPointer a, XtPointer b)
{
char temp[30];
XEvent i;
PRWindowList  ParentWindow;
BEGIN("Midi_PianoRollZoomInCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    if ( ParentWindow->Zoom >= PIANO_ROLL_ZOOM_MAX )
        END;

    ParentWindow->Zoom += PIANO_ROLL_ZOOM_STEP;
    sprintf(temp, "Zoom : %3d", (int)ParentWindow->Zoom);
    strcat(temp,"%");

    YSetValue(ParentWindow->ZoomValue, XtNlabel, temp);

    i.type = -1;
    Midi_PRCheckResizeCB(w, NULL, &i, NULL);
END;
}

void Midi_PianoRollZoomOutCB(Widget w, XtPointer a, XtPointer b)
{
char temp[30];
XEvent i;
PRWindowList  ParentWindow;
BEGIN("Midi_PianoRollZoomOutCB");

    ParentWindow = Midi_PRGetWindowFromWidget(w);

    if ( ParentWindow->Zoom <= PIANO_ROLL_ZOOM_MIN )
        END;

    ParentWindow->Zoom -= PIANO_ROLL_ZOOM_STEP;
    sprintf(temp, "Zoom : %3d", (int)ParentWindow->Zoom);
    strcat(temp,"%");

    YSetValue(ParentWindow->ZoomValue, XtNlabel, temp);

    i.type = -1;
    Midi_PRCheckResizeCB(w, NULL, &i, NULL);
END;
}

void Midi_PianoRollButtonReleaseCB(Widget w, XtPointer clientData, XEvent *e, Boolean *cont)
{
PRWindowList  ParentWindow;
XButtonEvent *ButtonEvent;

BEGIN("Midi_PianoRollButtonReleaseCB");

    ButtonEvent  = (XButtonEvent *)e;
    ParentWindow = (PRWindowList)clientData;

    if (!ParentWindow->Dragging || ButtonEvent->type != ButtonRelease || ButtonEvent->button != Button1) END;

    ParentWindow->Dragging = False;
    ParentWindow->DragEndCoord = ButtonEvent->x;

    if (ParentWindow->DragStartCoord != ParentWindow->DragEndCoord)
    {
        Midi_ELLeaveMenuMode((ELWindowList)ParentWindow,
                     NothingSelectedMode);
    }
    else Midi_ELEnterMenuMode((ELWindowList)ParentWindow,
                  NothingSelectedMode);
END;
}



void Midi_PianoRollPointerMotionCB(Widget w, XtPointer clientData, XEvent *e, Boolean *cont)
{
PRWindowList  ParentWindow;
XMotionEvent *MotionEvent;

BEGIN("Midi_PianoRollPointerMotionCB");

    MotionEvent  = (XMotionEvent *)e;
    ParentWindow = (PRWindowList)clientData;

    if (!ParentWindow->Dragging) END;

    Midi_PianoRollDrawSelectionBars(ParentWindow, ParentWindow->Clear);

    ParentWindow->DragEndCoord = MotionEvent->x;

    Midi_PianoRollDrawSelectionBars(ParentWindow, ParentWindow->DrawForce);

END;
}


void Midi_PianoRollQuitAction(Widget w, XEvent *e, String *s, Cardinal *c)
{
  PRWindowList windows;
  Begin("Midi_PianoRollQuitAction");

  windows = MIDIPianoRollWindows;

  while (windows) {
    if (windows->Shell == w) break;
    windows = (PRWindowList)Next(windows);
  }

  if (!windows) End;

  Midi_RemovePianoRollWindow(windows->TrackNum);
  End;
}


Widget Midi_PianoRollLocateWindowShell(int TrackNum)
{
PRWindowList Windows;
BEGIN("Midi_PianoRollLocateWindowShell");

    Windows = MIDIPianoRollWindows;

    while(Windows)
      {
    if (TrackNum == Windows->TrackNum)
      {
        RETURN_PTR(Windows->Shell);
      }
    
    Windows = (PRWindowList)Next(Windows);
      }

RETURN_PTR(NULL);
} 



void Midi_PianoRollWindowCreate(int TrackNum)
{
PRWindowList NewWindow;
Dimension    h1, h2;
char         TitleBuffer[256];
Atom         WmProtocols;
float        Zoom;

Widget         BottomBox,
         HelpButton,
         RewindButton,
         BackButton,
         ForwardButton,
         FfwdButton,
         ZoomInButton,
         ZoomOutButton;

Widget   WindowShell;

Widget   ViewScroll;

BEGIN("Midi_PianoRollWindowCreate");

    WindowShell = Midi_PianoRollLocateWindowShell(TrackNum);
    if (WindowShell) {
      XMapRaised(XtDisplay(WindowShell), XtWindow(WindowShell));
      END;
    }

    NewWindow = (PRWindowList)NewList(sizeof(PRWindowListElt));

    if (NewWindow == NULL)
    {
        Error(FATAL, "Unable to allocate space for new window.");
    }

    NewWindow->TrackNum   = TrackNum;
    NewWindow->BarNumber  = 0;
    if (MIDITracks[TrackNum])
    {
        NewWindow->LastEvtTime =
            ((EventList)Last(MIDITracks[TrackNum]))->Event.DeltaTime;
    }
    else
        NewWindow->LastEvtTime = 0;

    NewWindow->Shell = XtAppCreateShell("Rosegarden Sequencer Piano Roll",
                           ProgramName,
                           applicationShellWidgetClass, display, NULL, 0);

    NewWindow->Pane = YCreateWidget("Piano Roll", 
                           panedWidgetClass, NewWindow->Shell);

    NewWindow->TopBox = YCreateShadedWidget("Top Box", formWidgetClass,
                          NewWindow->Pane, MediumShade);

    NewWindow->MenuBar = YCreateShadedWidget("Menu Bar", boxWidgetClass, 
                               NewWindow->TopBox, MediumShade);

    NewWindow->HelpBox = YCreateShadedWidget("Help Box", boxWidgetClass,
                         NewWindow->TopBox, MediumShade);

    NewWindow->Toolbar = YCreateToolbar(NewWindow->Pane);

    XtVaSetValues(NewWindow->TopBox, XtNdefaultDistance, 0, NULL);

    XtVaSetValues(NewWindow->MenuBar,
              XtNleft,   XawChainLeft,   XtNright,  XawChainRight,
              XtNtop,    XawChainTop,    XtNbottom, XawChainTop,
              XtNhorizDistance, 0,       XtNvertDistance, 0,
              XtNborderWidth, 0, NULL);

    XtVaSetValues(NewWindow->HelpBox,
              XtNfromHoriz, NewWindow->MenuBar, XtNleft, XawChainRight,
              XtNright,  XawChainRight,  XtNtop, XawChainTop,
              XtNbottom, XawChainTop,    XtNhorizDistance, 0,
              XtNvertDistance, 0,        XtNborderWidth, 0, NULL);

    NewWindow->ViewPort = YCreateWidget("View Port", viewportWidgetClass,
                                        NewWindow->Pane);

    NewWindow->PianoRollLabel = YCreateWidget("Label", labelWidgetClass,
                                              NewWindow->ViewPort);

    NewWindow->PianoRollStrengthLabel = YCreateWidget("Strength Label",
                                              labelWidgetClass,
                                              NewWindow->Pane);

    NewWindow->Scrollbar = YCreateWidget("Scrollbar", scrollbarWidgetClass,
                                              NewWindow->Pane);

    NewWindow->Zoom = ( Zoom = PIANO_ROLL_ZOOM_DEFAULT );

    BottomBox = YCreateShadedWidget("Bottom Box", boxWidgetClass,
                                     NewWindow->Pane, MediumShade);

    RewindButton  = YCreateSurroundedWidget("Rewind",  repeaterWidgetClass, 
                        BottomBox, SurroundShade, NoShade);

    BackButton    = YCreateSurroundedWidget("Back",    repeaterWidgetClass, 
                        BottomBox, SurroundShade, NoShade);

    NewWindow->CounterDisplay = YCreateLabel("  0.00", BottomBox);

    ForwardButton = YCreateSurroundedWidget("Forward", repeaterWidgetClass, 
                        BottomBox, SurroundShade, NoShade);

    FfwdButton    = YCreateSurroundedWidget("Ffwd",    repeaterWidgetClass, 
                        BottomBox, SurroundShade, NoShade);

    NewWindow->ZoomValue = YCreateLabel("Zoom : 100%", BottomBox);

    ZoomInButton    = YCreateSurroundedWidget("Zoom In",    repeaterWidgetClass, 
                        BottomBox, SurroundShade, NoShade);

    ZoomOutButton    = YCreateSurroundedWidget("Zoom Out",  repeaterWidgetClass, 
                        BottomBox, SurroundShade, NoShade);


    XtAddCallback(ForwardButton, XtNcallback, Midi_PianoRollForwardCB,  NULL);
    XtAddCallback(BackButton,    XtNcallback, Midi_PianoRollBackwardCB, NULL);
    XtAddCallback(FfwdButton,    XtNcallback, Midi_PianoRollFfwdCB,     NULL);
    XtAddCallback(RewindButton,  XtNcallback, Midi_PianoRollRewindCB,   NULL);

    XtAddCallback(ZoomInButton,  XtNcallback, Midi_PianoRollZoomInCB,   NULL);
    XtAddCallback(ZoomOutButton, XtNcallback, Midi_PianoRollZoomOutCB,  NULL);

    XtAddCallback(NewWindow->Scrollbar, XtNscrollProc,
                  Midi_PianoRollScrollbarScrollCB, NULL);
    XtAddCallback(NewWindow->Scrollbar, XtNjumpProc,
                  Midi_PianoRollScrollbarJumpCB,   NULL);

/*   

    XtAddEventHandler(NewWindow->PianoRollStrengthLabel, ButtonPressMask, 
              True, Midi_PianoRollButtonPressCB, NewWindow);

    XtAddEventHandler(NewWindow->PianoRollStrengthLabel, ButtonReleaseMask,
              True, Midi_PianoRollButtonReleaseCB, NewWindow);

    XtAddEventHandler(NewWindow->PianoRollStrengthLabel, PointerMotionMask, 
              True, Midi_PianoRollPointerMotionCB, NewWindow);

    XtAddEventHandler(NewWindow->PianoRollLabel, ButtonPressMask, 
              True, Midi_PianoRollButtonPressCB, NewWindow);

    XtAddEventHandler(NewWindow->PianoRollLabel, ButtonReleaseMask,
              True, Midi_PianoRollButtonReleaseCB, NewWindow);

    XtAddEventHandler(NewWindow->PianoRollLabel, PointerMotionMask, 
              True, Midi_PianoRollPointerMotionCB, NewWindow);
*/

    NewWindow->Dragging = False;
      
    YSetValue(RewindButton,  XtNbitmap, Rewind);
    YSetValue(BackButton,    XtNbitmap, Back);
    YSetValue(ForwardButton, XtNbitmap, Forward);
    YSetValue(FfwdButton,    XtNbitmap, Ffwd);
    YSetValue(ZoomInButton,  XtNbitmap, ZoomIn);
    YSetValue(ZoomOutButton, XtNbitmap, ZoomOut);

    sprintf(TitleBuffer, "%s Sequencer Piano Roll - %s", ProgramName,
            TrackListEntries[TrackNum]);

    YSetValue(NewWindow->Shell, XtNtitle,        TitleBuffer);
    YSetValue(NewWindow->Shell, XtNiconName,   TitleBuffer);
    YSetValue(NewWindow->Shell, XtNiconPixmap, RoseMap);
    YSetValue(NewWindow->Shell, XtNiconMask,   RoseMask);

    YSetValue(NewWindow->ViewPort, XtNforceBars,  True);
    YSetValue(NewWindow->ViewPort, XtNallowHoriz, False);

    YSetValue(NewWindow->PianoRollStrengthLabel, XtNmin,
              PIANO_ROLL_VELOCITY_HEIGHT + 10);

    YSetValue(NewWindow->PianoRollStrengthLabel, XtNmax,
              PIANO_ROLL_VELOCITY_HEIGHT + 10);

    YSetValue(NewWindow->Scrollbar, XtNorientation, XtorientHorizontal);
    YSetValue(NewWindow->Scrollbar, XtNmin, PIANO_ROLL_SCROLLBAR_OFFSET);
    YSetValue(NewWindow->Scrollbar, XtNmax, PIANO_ROLL_SCROLLBAR_OFFSET);

    NewWindow->TrackMenuButton = YCreateMenuButton("Track", NewWindow->MenuBar);
    NewWindow->EditMenuButton  = YCreateMenuButton("Edit", NewWindow->MenuBar);

    /* removed until we get around to actually implementing it... */
    NewWindow->EventMenuButton = YCreateMenuButton("Event", NewWindow->MenuBar);
    XtUnmanageChild(XtParent(NewWindow->EventMenuButton));

    HelpButton = YCreateCommand("Help", NewWindow->HelpBox);

    XtVaSetValues(NewWindow->MenuBar, XtNleft, XawChainLeft, XtNright,
              XawChainRight, XtNtop, XawChainTop, XtNbottom,
              XawChainTop, NULL);

    if (!appData.interlockWindow) YSetValue(HelpButton, XtNsensitive, False);

    XtAddCallback(HelpButton, XtNcallback, Midi_HelpCallback,
                  "Track - Show Piano Roll");

    Midi_PRInstallMenus(NewWindow);

    XtSetMappedWhenManaged(NewWindow->Shell, False);
  
    XtRealizeWidget(NewWindow->Shell);
  
    YGetValue(NewWindow->TrackMenuButton, XtNheight, &h1);
    YGetValue(RewindButton, XtNheight, &h2);
    YGetValue(NewWindow->ViewPort,  XtNwidth,  &NewWindow->PianoRollWidth);
    YGetValue(NewWindow->PianoRollLabel,  XtNheight,
              &NewWindow->PianoRollHeight);

    XtUnrealizeWidget(NewWindow->Shell);

    YSetValue(NewWindow->ViewPort, XtNallowVert, True);
    YSetValue(NewWindow->PianoRollLabel, XtNwidth, NewWindow->PianoRollWidth);

    YSetValue(NewWindow->PianoRollStrengthLabel, XtNwidth,
              NewWindow->PianoRollWidth);

    YSetValue(NewWindow->PianoRollLabel, XtNheight, PIANO_ROLL_PIXMAP_HEIGHT);

    XtSetMappedWhenManaged(NewWindow->Shell, True);

    NewWindow->LabelPixmap = XCreatePixmap(
                     XtDisplay(NewWindow->PianoRollLabel),
                     RootWindowOfScreen(XtScreen(NewWindow->PianoRollLabel)),
                     NewWindow->PianoRollWidth,
                     PIANO_ROLL_PIXMAP_HEIGHT,
                     DefaultDepthOfScreen(XtScreen(NewWindow->PianoRollLabel)));

    NewWindow->StrengthLabelPixmap = XCreatePixmap(
                     XtDisplay(NewWindow->PianoRollStrengthLabel), 
                     RootWindowOfScreen(XtScreen(NewWindow->
                          PianoRollStrengthLabel)),
                     NewWindow->PianoRollWidth,
                     PIANO_ROLL_VELOCITY_HEIGHT,
                     DefaultDepthOfScreen(
                          XtScreen(NewWindow->PianoRollStrengthLabel)));

    XtAddEventHandler(NewWindow->PianoRollLabel, ExposureMask,
                      True, Midi_PRCheckResizeCB, NewWindow);

    XtAddEventHandler(NewWindow->PianoRollStrengthLabel, ExposureMask,
                      True, Midi_PRCheckResizeCB, NewWindow);

    Midi_PianoRollSetupGCs(NewWindow);
    Midi_PianoRollDrawBackground(NewWindow);
    Midi_PianoRollDrawNotes(NewWindow);

    YSetValue(NewWindow->TopBox, XtNmin, h1 + 15);
    YSetValue(NewWindow->TopBox, XtNmax, h1 + 15);

    YSetValue(BottomBox, XtNmin, h2 + 15);
    YSetValue(BottomBox, XtNmax, h2 + 15);

    YSetValue(ZoomOutButton, XtNright, NULL);
    YSetValue(ZoomInButton,  XtNfromHoriz, NULL);

    YSetValue(NewWindow->PianoRollLabel, XtNbitmap, NewWindow->LabelPixmap);
    YSetValue(NewWindow->PianoRollStrengthLabel, XtNbitmap,
              NewWindow->StrengthLabelPixmap);

    XtRealizeWidget(NewWindow->Shell);

    WmProtocols = XInternAtom(XtDisplay(NewWindow->Shell),
                  "WM_DELETE_WINDOW", False);
    XtOverrideTranslations(NewWindow->Shell,
         XtParseTranslationTable("<Message>WM_PROTOCOLS: pianoroll-wm-quit()"));

    XSetWMProtocols(XtDisplay(NewWindow->Shell),
            XtWindow(NewWindow->Shell), &WmProtocols, 1);

    if (MIDIPianoRollWindows)
    {
        Nconc(MIDIPianoRollWindows, NewWindow);
    }
    else MIDIPianoRollWindows = NewWindow;

    Midi_PREnterMenuMode(NewWindow, NothingSelectedMode);

    if (Midi_ClipboardIsEmpty())
    {
        Midi_PREnterMenuMode(NewWindow, NothingCutMode);
    }

    if (Midi_UndoIsBufferEmpty())
    {
        Midi_PREnterMenuMode(NewWindow, NothingDoneMode);
    }

    Midi_PianoRollScrollbarInit(NewWindow);

    ViewScroll = XtNameToWidget(NewWindow->ViewPort, "vertical");
    YSetValue(ViewScroll, XtNthumb, LightGrey);
    YSetValue(NewWindow->Scrollbar, XtNthumb, LightGrey);

    NewWindow->DragStartCoord = 0;
    NewWindow->DragEndCoord   = 0;
    NewWindow->SelectStartEvt = NULL;
    NewWindow->SelectEndEvt   = NULL;

    XtInstallAccelerators(NewWindow->ViewPort, NewWindow->Shell);
END;
}





void Midi_PianoRollDrawBackground(PRWindowList ParentWindow)
{
int i;
float y, KeyboardNoteY;
int keyboard[] = { 1, 1, 0, 1, 1, 1, 0 };
int KeyboardWidth = 50;
float ZoomFactor, Zoom;

BEGIN("Midi_PianoRollDrawBackground");

    Zoom = ParentWindow->Zoom;

    ZoomFactor = Zoom / PIANO_ROLL_ZOOM_DEFAULT;

    XFillRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
               ParentWindow->Clear,
               0,
                       0,
                       ParentWindow->PianoRollWidth,
                       ParentWindow->PianoRollHeight);

    XFillRectangle(XtDisplay(topLevel), ParentWindow->StrengthLabelPixmap,
                   ParentWindow->Clear,
                   0,
                   0,
                   ParentWindow->PianoRollWidth + PIANO_ROLL_SCROLLBAR_OFFSET,
                   PIANO_ROLL_VELOCITY_HEIGHT / ZoomFactor);

    XDrawLine(XtDisplay(topLevel), ParentWindow->StrengthLabelPixmap,
                   ParentWindow->DrawForce,
                   (int) KeyboardWidth + PIANO_ROLL_SCROLLBAR_OFFSET + 11,
                   ( ( PIANO_ROLL_VELOCITY_HEIGHT / ZoomFactor ) - 10 ),
                   (int) ParentWindow->PianoRollWidth,
                   ( ( PIANO_ROLL_VELOCITY_HEIGHT / ZoomFactor )  - 10 ) );

    /* Insert the Bar Lines */
    i = PIANO_ROLL_CLEF_WIDTH;

        while (i < ParentWindow->PianoRollWidth)
    {
        XDrawLine(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                  ParentWindow->DrawForce,
                  i,
                  0,
                  i,
                  PIANO_ROLL_PIXMAP_HEIGHT);

/*   Velocity Bar Lines
                XDrawLine(XtDisplay(topLevel),
                          ParentWindow->StrengthLabelPixmap,
                          ParentWindow->DrawForce,
                          i + PIANO_ROLL_SCROLLBAR_OFFSET - 1,
                          2,
                          i + PIANO_ROLL_SCROLLBAR_OFFSET - 1,
                          PIANO_ROLL_VELOCITY_HEIGHT  - 11);
*/
        i += PIANO_ROLL_DEFAULT_BAR_WIDTH;
    }

    KeyboardNoteY = (int)( PIANO_ROLL_OCTAVE_YSIZE / 7 );

    /* NB :  PIANO_ROLL_PIXMAP_HEIGHT is the working height of the
       pixmap - not always the same as the actual height */

    y = PIANO_ROLL_TOP_STAVE_OFFSET - 4 * PIANO_ROLL_OCTAVE_YSIZE -
        ( 0.5 * KeyboardNoteY );

    while ( y < PIANO_ROLL_PIXMAP_HEIGHT )
    {
            
        XDrawRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                       ParentWindow->DrawForce,
                       0,
                       y,
                       KeyboardWidth,
                       (int)KeyboardNoteY); 

        if ( ( (int)( y / KeyboardNoteY) % 7 ) == 3 )
        {
            XDrawLine(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                      ParentWindow->DrawForce,
                      KeyboardWidth + 10,
                      y,
                      ParentWindow->PianoRollWidth,
                      y);
        }

        y += KeyboardNoteY;
    }

    /* and the black notes */
    y = PIANO_ROLL_TOP_STAVE_OFFSET - 4 * PIANO_ROLL_OCTAVE_YSIZE -
        ( 0.5 * KeyboardNoteY );

    i = 0;

    while ( y < PIANO_ROLL_PIXMAP_HEIGHT )
    {
        if ( i && ( keyboard[ (int)(y / KeyboardNoteY ) % 7 ]  ) )
        {
            XFillRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                           ParentWindow->DrawForce,
                           0,
                           y + (  0.65 * KeyboardNoteY ),
                           0.62 * KeyboardWidth,
                           0.7 * KeyboardNoteY);

            XFillRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                          ParentWindow->Clear,
                          0.56 * KeyboardWidth,
                          y + (  0.65 * KeyboardNoteY ),
                          0.06 * KeyboardWidth,
                          0.7 * KeyboardNoteY);

            XDrawRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                           ParentWindow->DrawForce,
                           0,
                           y + (  0.65 * KeyboardNoteY ),
                           0.62 * KeyboardWidth,
                           0.7 * KeyboardNoteY);
        }

        y += KeyboardNoteY;

        if (!i)
            i = 1;
    }
            

END;
}


void Midi_PianoRollDrawNotes(PRWindowList ParentWindow)
{
EventList CurrEvtPtr;
MIDIEvent CurrentEvent;
float       Ypos, Xpos, NoteHeight;
int      NoteBoxLength;
long      StartDispTime;
long      EndDispTime;
float      Resolution;
GC      NoteShade;
char      FormatBuffer[16];
float     ZoomFactor, Zoom;
float     VelocityWidth;

BEGIN("Midi_PianoRollDrawNotes");

    CurrEvtPtr = MIDITracks[ParentWindow->TrackNum];

        Zoom = ParentWindow->Zoom;

    StartDispTime = ParentWindow->BarNumber * 4 *
                           MIDIHeaderBuffer.Timing.Division;

    EndDispTime   = StartDispTime + (ParentWindow->PianoRollWidth /
                    PIANO_ROLL_DEFAULT_BAR_WIDTH + 1) *
                    4 * MIDIHeaderBuffer.Timing.Division;

    Resolution = ((float)PIANO_ROLL_DEFAULT_BAR_WIDTH) /
                 (4 * MIDIHeaderBuffer.Timing.Division);

    sprintf(FormatBuffer,"%d.00", (ParentWindow->BarNumber * 4));
    YSetValue(ParentWindow->CounterDisplay, XtNlabel, FormatBuffer);

    /*************************************/
    /* Zip along to the next note event. */
    /*************************************/

    while(CurrEvtPtr)
    {
        if (MessageType(CurrEvtPtr->Event.EventCode) == MIDI_NOTE_ON &&
            CurrEvtPtr->Event.DeltaTime +
            CurrEvtPtr->Event.EventData.Note.Duration >= StartDispTime)
        {
            break;
        }
        else CurrEvtPtr = (EventList)Next(CurrEvtPtr);
    }

    if (!CurrEvtPtr) END;

        ZoomFactor = ParentWindow->Zoom / PIANO_ROLL_ZOOM_DEFAULT;

    while (CurrEvtPtr && CurrEvtPtr->Event.DeltaTime < EndDispTime)
    {
        CurrentEvent = &CurrEvtPtr->Event;
    
        Ypos = Midi_PianoRollCalculateNoteYPos 
                       (CurrentEvent->EventData.Note.Note, ParentWindow->Zoom);

        Xpos =  ( (CurrentEvent->DeltaTime - StartDispTime) * Resolution ) +
                  PIANO_ROLL_CLEF_WIDTH;

        NoteBoxLength = CurrentEvent->EventData.Note.Duration * Resolution;

        if (Midi_NoteSharpsTable[CurrentEvent->EventData.Note.Note % 12])
        {
            NoteShade = ParentWindow->LightGrey;
                        Ypos -= ( 0.5 * (int) PIANO_ROLL_OCTAVE_YSIZE / 7 );
        }
        else NoteShade = ParentWindow->Grey;

        NoteHeight =  ( (int) PIANO_ROLL_OCTAVE_YSIZE / 7 );

        VelocityWidth = ( NoteHeight );

        if ( (int)NoteHeight <= 3 )
            NoteHeight = 3;

        if ( (int)VelocityWidth <= 3 )
            VelocityWidth = 3;

        if ( Xpos >= PIANO_ROLL_CLEF_WIDTH )
        {
            XFillRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                   ParentWindow->Clear,
                   Xpos-1, Ypos-1, NoteBoxLength+2, NoteHeight - 1);

            XFillRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                   NoteShade,
                   Xpos, Ypos, NoteBoxLength, NoteHeight - 2 );

            XDrawRectangle(XtDisplay(topLevel), ParentWindow->LabelPixmap,
                   ParentWindow->DrawForce,
                   Xpos, Ypos, NoteBoxLength, NoteHeight - 2 ); 

                    /* Velocity Bars */
            XFillRectangle(XtDisplay(topLevel),
                   ParentWindow->StrengthLabelPixmap,
                   ParentWindow->Clear,
                   Xpos + PIANO_ROLL_SCROLLBAR_OFFSET - 1,
                   ( PIANO_ROLL_VELOCITY_HEIGHT / ZoomFactor ) -
                   (  CurrentEvent->EventData.Note.Velocity / 2 ) - 11,
                   VelocityWidth,
                   ( CurrentEvent->EventData.Note.Velocity / 2 ) - 1);

            XFillRectangle(XtDisplay(topLevel),
                   ParentWindow->StrengthLabelPixmap,
                   NoteShade,
                   Xpos + PIANO_ROLL_SCROLLBAR_OFFSET,
                   ( PIANO_ROLL_VELOCITY_HEIGHT / ZoomFactor ) -
                   (  CurrentEvent->EventData.Note.Velocity / 2 ) - 10,
                   VelocityWidth - 1,
                   ( CurrentEvent->EventData.Note.Velocity / 2 ) - 1);

            XDrawRectangle(XtDisplay(topLevel),
                   ParentWindow->StrengthLabelPixmap,
                   ParentWindow->DrawForce,
                   Xpos + PIANO_ROLL_SCROLLBAR_OFFSET,
                   ( PIANO_ROLL_VELOCITY_HEIGHT / ZoomFactor ) -
                   (  CurrentEvent->EventData.Note.Velocity / 2 ) - 10,
                   VelocityWidth - 1,
                   (CurrentEvent->EventData.Note.Velocity / 2 ) - 1);
        }

        do
        {
            CurrEvtPtr = (EventList)Next(CurrEvtPtr);
        }
        while(CurrEvtPtr && MessageType(CurrEvtPtr->Event.EventCode)
                  != MIDI_NOTE_ON);
    }
END;
}



void Midi_UpdatePianoRollWindow(int TrackNum)
{
PRWindowList SubWindow;
char         TitleBuffer[256];

BEGIN("Midi_UpdatePianoRollWindow");

    SubWindow = MIDIPianoRollWindows;

    while (SubWindow)
    {
        if (SubWindow->TrackNum == TrackNum)
        {
            SubWindow->DragStartCoord = 0;
            SubWindow->DragEndCoord   = 0;
            SubWindow->SelectStartEvt = NULL;
            SubWindow->SelectEndEvt   = NULL;

            Midi_PianoRollDrawSelectionBars(SubWindow, SubWindow->Clear);
            Midi_PianoRollDrawBackground(SubWindow);
            Midi_PianoRollDrawNotes(SubWindow);
            event.display = XtDisplay(SubWindow->PianoRollLabel);
            event.window  =  XtWindow(SubWindow->PianoRollLabel);

            XSendEvent(XtDisplay(SubWindow->PianoRollLabel),
                   XtWindow(SubWindow->PianoRollLabel),  False,
                   ExposureMask, (XEvent *)&event);

            sprintf(TitleBuffer, "%s Sequencer Piano Roll - %s",
                ProgramName, TrackListEntries[TrackNum]);
            YSetValue(SubWindow->Shell, XtNtitle,        TitleBuffer);
            YSetValue(SubWindow->Shell, XtNiconName,   TitleBuffer);
            END;
        }
        SubWindow = (PRWindowList)Next(SubWindow);
    }
END;
}


void Midi_RemovePianoRollWindow(int TrackNum)
{
PRWindowList SubWindow;

BEGIN("Midi_RemovePianoRollWindow");

    SubWindow = MIDIPianoRollWindows;

    while(SubWindow)
    {
      if (SubWindow->TrackNum == TrackNum)
        {
          if (!Prev(SubWindow) && !Next(SubWindow)) {
        XFreeGC(XtDisplay(topLevel), SubWindow->Clear);
        XFreeGC(XtDisplay(topLevel), SubWindow->DrawForce);
        XFreeGC(XtDisplay(topLevel), SubWindow->DrawXor);
        XFreeGC(XtDisplay(topLevel), SubWindow->LightGrey);
        XFreeGC(XtDisplay(topLevel), SubWindow->Grey);
          }

          XFreePixmap(XtDisplay(topLevel), SubWindow->LabelPixmap);
          XFreePixmap(XtDisplay(topLevel), SubWindow->StrengthLabelPixmap);
          XtFree(SubWindow->TrackMenu);
          XtFree(SubWindow->EditMenu);
          XtFree(SubWindow->EventMenu);
          YDestroyToolbar(SubWindow->Toolbar);
          XtDestroyWidget(SubWindow->Shell);
          MIDIPianoRollWindows = (PRWindowList)First(Remove(SubWindow));
          break;
        }
      SubWindow = (PRWindowList)Next(SubWindow);
    }
END;
}
            
void Midi_PianoRollDeleteAllWindows()
{
BEGIN("Midi_PianoRollDeleteAllWindows");

        if (MIDIPianoRollWindows) {
      XFreeGC(XtDisplay(topLevel), MIDIPianoRollWindows->Clear);
      XFreeGC(XtDisplay(topLevel), MIDIPianoRollWindows->DrawForce);
      XFreeGC(XtDisplay(topLevel), MIDIPianoRollWindows->DrawXor);
      XFreeGC(XtDisplay(topLevel), MIDIPianoRollWindows->LightGrey);
      XFreeGC(XtDisplay(topLevel), MIDIPianoRollWindows->Grey);
    }

    while(MIDIPianoRollWindows)
    {
        XFreePixmap(XtDisplay(topLevel), MIDIPianoRollWindows->LabelPixmap);
        XtFree(MIDIPianoRollWindows->TrackMenu);
        XtFree(MIDIPianoRollWindows->EditMenu);
        XtFree(MIDIPianoRollWindows->EventMenu);
        YDestroyToolbar(MIDIPianoRollWindows->Toolbar);
        XtDestroyWidget(MIDIPianoRollWindows->Shell);
        MIDIPianoRollWindows = (PRWindowList)First(Remove(MIDIPianoRollWindows));
    }
END;
}

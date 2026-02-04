/*
 *    Rosegarden MIDI Sequencer
 *
 *    File:           MainWindow.h
 *
 *    Description:    Functions that build up the sequencer main window.
 *
 *
 *    Author:         AJG
 *    History:
 *
 *    Update  Date            Programmer      Comments
 *    ======  ====            ==========      ========
 *    001     16/02/94        AJG             File Created.
 *
 *
 */

#include "Types.h"
#include <MidiFile.h>
#include "Globals.h"
#include <MidiXInclude.h>
#include "MainWindow.h"
#include "TrackList.h"
#include "Message.h"
#include "Menu.h"
#include "Main.h"

#include <Version.h>

#include <ILClient.h>
#include <MidiErrorHandler.h>
#include <Debug.h>
#include <rose.xbm>
#include <rose_mask.xbm>

#include <up.xbm>
#include <down.xbm>
#include <rewind.xbm>
#include <back.xbm>
#include <forward.xbm>
#include <ffwd.xbm>
#include <lightgrey.xbm>
#include <darkgrey.xbm>
#include <zoomin.xbm>
#include <zoomout.xbm>

#include <clef_treble.xbm>
#include <clef_bass.xbm>

#include <notemod_sharp.xbm>


YMessageString aboutText[] = {
  { "Rosegarden",                    YMessageBold,       },
  { " ",                             YMessageNormal,     },
  { "MIDI Sequencer",                YMessageNormal,     },
  { " ",                             YMessageNormal,     },
  { ROSEGARDEN_VERSION,              YMessageNormal,     },
  { "Andy Green & Richard Brown",    YMessageBold,       },
  { " ",                             YMessageNormal,     },
  { "IRIX 6.5 fixes: @chulofiasco, SGUG",  YMessageBold,       },
  { "with thanks to ctc, jpff and others", YMessageItalic, },
};


Pixmap  RoseMap, 
        RoseMask,
        UpMap,
        DownMap,
        TrebleClef,
        BassClef,
        Rewind,
        Back,
        Forward,
        Ffwd,
        Sharp,
        LightGrey,
        Grey,
        ZoomIn,
        ZoomOut;

Widget  MainForm;
Widget  OuterPane;
Widget  TrackListBox;
Widget  FileButton, 
        EditButton, 
        TrackButton, 
        MidiButton, 
        FilterButton,
        HelpButton, 
        RoseLabel, 
        MsgLabel;

Widget  TimeDisplay;
Widget  EndofTrackLabel;



Cursor  ListBoxCursor;

Dimension MsgLabelWidth;

Boolean IsMono;

/*********************************************************************/
/* Midi_AboutButton: Callback function to produce a nice 'About' box */
/* should someone be stupid enough to click on the Rosegarden logo.  */
/*********************************************************************/

void Midi_AboutButton(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_AboutButton");

    YMessage(XtParent(w), "About Rosegarden", "Enough!", aboutText,
                                                    XtNumber(aboutText));

END;
}



/*****************************************************/
/* Midi_AcknowledgeHelp: Callback to acknowledge the */
/* completion of a Help interlock request.           */
/*****************************************************/

void Midi_AcknowledgeHelp(IL_ReturnCode Woo)
{
BEGIN("Midi_AcknowledgeHelp");

END;
}




/*********************************************************************/
/* Midi_RequestHelp: Callback for the Help button on the menu bar.   */
/* A call is made to the Help service over the interlock connection. */
/*********************************************************************/
 
void Midi_RequestHelp(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_RequestHelp");

    if (appData.interlockWindow)
    {
        IL_RequestService(ILS_HELP_SERVICE,
			  Midi_AcknowledgeHelp, "Sequencer", 10);
    }
    else 
    {
        YQuery(topLevel, 
        "Sorry, no help is available if the Top Box isn't present.\n",
                                                      1, 0, 0, "OK", NULL);
    }

END;
}

void Midi_CreateMainWindow()
{
    Widget TopBox;
    Widget MenuBar;
    Widget HelpBar;
    Widget Toolbar;
    Widget PlayBar;
    Widget CounterLabel;
    
    Widget MainViewPort;
    Widget MsgViewPort;
    
    Dimension h1, h2, w1, w2;

    BEGIN("Midi_CreateMainWindow");
    
    if (appData.interlockWindow)
    {
        YInitialise(topLevel, Midi_DemandHelp);
    }
    else YInitialise(topLevel, NULL);
    
    YShouldWarpPointer(appData.shouldWarpPointer);

    OuterPane = YCreateWidget("Sequencer", panedWidgetClass, topLevel);

    TopBox = YCreateShadedWidget("Top Box", formWidgetClass, OuterPane,
                                                                  MediumShade);

    MenuBar = YCreateShadedWidget("Menu Bar", boxWidgetClass,
                                                    TopBox, MediumShade);

    HelpBar = YCreateShadedWidget("Help Bar", boxWidgetClass, TopBox,
                                                                  MediumShade);

    Toolbar = YCreateToolbar(OuterPane);

    MainForm = YCreateShadedWidget("Main Form", formWidgetClass, OuterPane,
                                                                  LightShade);

    MainViewPort = YCreateWidget("View Port", viewportWidgetClass, MainForm);


    TrackListBox = YCreateWidget("Track List", listWidgetClass, MainViewPort);

    PlayBar = YCreateShadedWidget("Play Bar", boxWidgetClass, OuterPane,
                                                                  MediumShade);

    MsgViewPort = YCreateWidget("Message View Port", viewportWidgetClass,
                                                                   OuterPane);

    MsgLabel = YCreateSurroundedWidget("Message Label", labelWidgetClass,
                                               MsgViewPort, NoShade, NoShade);
    
    RoseLabel   = YCreateSurroundedWidget("Rosegarden Logo",
                         commandWidgetClass, MenuBar, SurroundShade, NoShade);



    FileButton  = YCreateMenuButton("File", MenuBar);
    EditButton  = YCreateMenuButton("Edit", MenuBar);
    TrackButton = YCreateMenuButton("Track", MenuBar);
    MidiButton  = YCreateMenuButton("Midi", MenuBar);
    FilterButton = YCreateMenuButton("Filter", MenuBar);
    HelpButton  = YCreateCommand("Help", HelpBar);

    CounterLabel = YCreateLabel("Position", PlayBar);
    TimeDisplay = YCreateLabel(" 00000000", PlayBar);
    EndofTrackLabel = YCreateLabel(" ( 00000000 )", PlayBar);

    XtVaSetValues(TopBox, XtNdefaultDistance, 0, NULL);
    
    XtVaSetValues(MenuBar,
                  XtNleft,   XawChainLeft,   XtNright,  XawChainRight,
                  XtNtop,    XawChainTop,    XtNbottom, XawChainTop,
                  XtNhorizDistance, 0,       XtNvertDistance, 0,
                  XtNborderWidth, 0, NULL);

    XtVaSetValues(HelpBar,
                  XtNfromHoriz, MenuBar,     XtNleft,   XawChainRight,
                  XtNright,  XawChainRight,  XtNtop,    XawChainTop,
                  XtNbottom, XawChainTop,    XtNhorizDistance, 0,
                  XtNvertDistance, 0,        XtNborderWidth, 0, NULL);

    XtVaSetValues(PlayBar, XtNfromVert, TrackListBox,
                  XtNhorizDistance, 0, XtNborderWidth, 0, NULL);

    YGetValue(TimeDisplay, XtNheight, &h1);
    YGetValue(PlayBar, XtNwidth,  &w1);  
    YSetValue(PlayBar, XtNmin, h1+15);
    YSetValue(PlayBar, XtNmax, h1+15);


    YMenuInitialise(RoseLabel, appData.acceleratorTable);
    
    XtAddCallback(HelpButton, XtNcallback, Midi_RequestHelp, NULL);
    XtAddCallback(RoseLabel,  XtNcallback, Midi_AboutButton, NULL);
    XtAddCallback(TrackListBox, XtNcallback, Midi_TrackListCB, NULL);
    XtAddEventHandler(TrackListBox, ButtonReleaseMask, True,
                      Midi_TrackListSelectionCB, NULL);

    if (!appData.interlockWindow) YSetValue(HelpButton, XtNsensitive, False);
    
    Midi_InstallFileMenu(FileButton);
    Midi_InstallEditMenu(EditButton);
    Midi_InstallTrackMenu(TrackButton);
    Midi_InstallMidiMenu(MidiButton);
    Midi_InstallFilterMenu(FilterButton);
    
    XtInstallAccelerators(MainViewPort, RoseLabel);
    
    /* changed back from YCreatePixmapFromData calls, cc 2/95: */

    RoseMap     = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       rose_bits, rose_width, rose_height);

    RoseMask    = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       rose_mask_bits, rose_mask_width, rose_mask_height);

    UpMap      = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       up_bits, up_width, up_height);

    DownMap    = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       down_bits, down_width, down_height);

    Rewind     = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       rewind_bits, rewind_width, rewind_height);

    Back       = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       back_bits, back_width, back_height);

    Forward    = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       forward_bits, forward_width, forward_height);

    Ffwd       = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       ffwd_bits, ffwd_width, ffwd_height);
    
    ZoomIn       = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       zoomin_bits, zoomin_width, zoomin_height);
    
    ZoomOut      = XCreateBitmapFromData
      (XtDisplay(topLevel), RootWindowOfScreen(XtScreen(topLevel)),
       zoomout_bits, zoomout_width, zoomout_height);
    
    LightGrey  = XCreateBitmapFromData( XtDisplay(topLevel),
                        RootWindowOfScreen(XtScreen(topLevel)),
                        lightgrey_bits, lightgrey_width, lightgrey_height);
    
    Grey       = XCreateBitmapFromData(XtDisplay(topLevel),
                        RootWindowOfScreen(XtScreen(topLevel)),
                        darkgrey_bits,  darkgrey_width,  darkgrey_height);
    
    Sharp   = YCreatePixmapFromData(notemod_sharp_bits, notemod_sharp_width, 
                                               notemod_sharp_height, NoShade);
    
    TrebleClef = YCreatePixmapFromData(clef_treble_bits, clef_treble_width, 
                                                 clef_treble_height, NoShade);

    BassClef   = YCreatePixmapFromData(clef_bass_bits, clef_bass_width,
                                                   clef_bass_height, NoShade);

    YMessageInitialise(RoseMap, appData.aboutTextFont);


    YSetValue(RoseLabel, XtNbitmap, RoseMap);
    YSetValue(RoseLabel, "shadowWidth", 0); /* in case we're 3d */
    YSetValue(TrackListBox, XtNsensitive, False);
    YSetValue(TrackListBox, XtNverticalList, True);

    XtSetMappedWhenManaged(topLevel, False);
    
    XtRealizeWidget(topLevel);
    
    YGetValue(RoseLabel, XtNheight, &h1);
    YGetValue(MsgLabel,  XtNheight, &h2);
    YGetValue(MenuBar,   XtNwidth,  &w1);  
    YGetValue(HelpBar,   XtNwidth,  &w2);
    
    XtUnrealizeWidget(topLevel);
    
    XtSetMappedWhenManaged(topLevel, True);
    
    YSetValue(TopBox,        XtNmin, h1 + 15);
    YSetValue(TopBox,        XtNmax, h1 + 15);
    
    YSetValue(MsgViewPort, XtNmin, h2 + 4);
    YSetValue(MsgViewPort, XtNmax, h2 + 4);
    YSetValue(MsgLabel,    XtNwidth, w1 + w2 - 6);

    MsgLabelWidth = w1 + w2 - 6;
    
    YSetValue(topLevel, XtNiconPixmap, RoseMap);
    YSetValue(topLevel, XtNiconMask,   RoseMask);
    
    XtRealizeWidget(topLevel);
    
    YGetValue(TrackListBox, XtNcursor, &ListBoxCursor);
    
    Midi_EnterMenuMode(NoFileLoadedMode    | NothingDoneMode | 
                       NothingSelectedMode | NothingCutMode  |
		       NotPlayingMode);

    Midi_LeaveMenuMode(PlaybackMode);
    
    Midi_DisplayPermanentMessage("");
    
    ErrorHandlerInitialise(topLevel);
    
    MIDIHeaderBuffer.Format    = MIDI_NO_FILE_LOADED;
    MIDIHeaderBuffer.NumTracks = 0;
    
    IsMono = (DefaultDepthOfScreen(XtScreen(topLevel)) == 1);
    
    Midi_TrackListSetup();
    END;
}

void Midi_SetBusy(Boolean State)
{
BEGIN("Midi_SetBusy");

    if (State)
    {
        XDefineCursor(display, XtWindow(TrackListBox), HourglassCursor);
        YSetValue(TrackListBox, XtNsensitive, False);
        /*YSetValue(TrackListBox, XtNcursor, HourglassCursor);*/
    }
    else
    {
        XUndefineCursor(display, XtWindow(TrackListBox));
        YSetValue(TrackListBox, XtNsensitive, True);
        /*YSetValue(TrackListBox, XtNcursor, ListBoxCursor);*/
    }

    XSync(display, False);

END;
}

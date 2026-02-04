/*
 * remerged from system dependent files but still requires
 * some tweaking depending on the systems and what functionality
 * can be supported
 */

#include "Mapper.h"
#include <MidiFile.h>

#include "Globals.h"
#include "EventDlgs.h"
#include "Sequence.h"
#include "Main.h"
#include "Menu.h"
#include "MidiSetupDlgs.h"
#include "EventListWindow.h"
#include "TrackList.h"
#include "EventListMenu.h"

#include <ctype.h>
#include <Debug.h>


extern int MidiEventBufferSize;
int MenuSelection;
Widget DeviceDlg;
Widget PatchDlg;
Widget ChangeDlg;
Widget PatchValueField[16];
Widget TrackingMenu;

extern int Playing;
extern int SynthType;
extern Widget topLevel;
extern void Midi_SeqAllNotesOff();

extern byte InitialPatches[16];
extern TrackMetaInfoElement     Tracks;
extern DeviceMetaInfoElement    Devices;

void Midi_PatchOKCB();

Boolean SetupSync;
Boolean MaintainTempo;

Widget SetupDlg;
Widget SetupEventBufSizeField;
Widget DevicePlay;
Widget SetupDeviceField;

#ifndef SYSTEM_OSS
Widget SetupPortNameField;
extern Boolean MidiPortSync;
extern Boolean MidiMaintainTempo;
#endif


/*
 * service the sensitivities of all open event list windows.
 *
 */
void
Midi_ServiceEventListGUIs(int Mode)
{
int i;
ELWindowList thing;
BEGIN("Midi_ServiceEventListGUIs");

    /* refresh all the open event lists */
    for (i = 0; i < MIDIHeaderBuffer.NumTracks; i++)
    {
        if ((thing = (ELWindowList)Midi_ELGetWindowFromTrack(i)) != NULL)
        {
            YSetValue(thing->TrackDeviceLabel, XtNsensitive, Mode);
            YSetValue(thing->DeviceMenu, XtNsensitive, Mode);
            YSetValue(thing->TrackPlaybackStatus, XtNsensitive, Mode);
            YSetValue(thing->TrackPlaybackLabel, XtNsensitive, Mode);
        }
    }

    while(XtAppPending(appContext)) XtAppProcessEvent(appContext, XtIMAll);
    XFlush(display);

END;
}


void
Midi_ChangesAllTracksCB(Widget w, XtPointer a, XtPointer b)
{
/* ELWindowList ThisWindow = (ELWindowList)a; */
BEGIN("Midi_ChangesAllTracksCB");

     MenuSelection  = (int)b;

END;
}

void
Midi_ChangeCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_ChangeCancelCB");

    XtDestroyWidget(ChangeDlg);

END;
}

void
Midi_ChangeOKCB(Widget w, XtPointer a, XtPointer b)
{
int i;
ELWindowList thing;
String NewDeviceLabel;

BEGIN("Midi_ChangeOKCB");

    /* refresh all the open event lists */
    for (i = 0; i < MIDIHeaderBuffer.NumTracks; i++)
    {
        Mapper_GetTrack(i)->Device = MenuSelection;
        if ((thing = (ELWindowList)Midi_ELGetWindowFromTrack(i)) != NULL)
        {
            NewDeviceLabel = XtNewString
                (Mapper_GetDevice(MenuSelection)->Device.Data.Midi.name);

            YSetValue(thing->DeviceMenu, XtNlabel, NewDeviceLabel);
        }
    }

    Midi_TrackListSetup();

    XtDestroyWidget(ChangeDlg);

END;
}


void
Midi_ChangeTracksToDeviceCB(Widget w, XtPointer a, XtPointer b)
{
Widget ChangePane;
Widget ChangeTopBox;
Widget ChangeLabel;
Widget ChangeForm;
Widget ChangeBottomBox;
Widget ChangeOK;
Widget ChangeCancel;
Widget ChangeDeviceMenu;
XPoint op;
/* Dimension LabelWidth; */
String *ChangeDeviceMenuLabels;
int i;

BEGIN("Midi_ChangeTracksToDeviceCB");

    ChangeDlg = XtCreatePopupShell("Change Setup", transientShellWidgetClass, 
                                     topLevel, NULL, 0);

    ChangePane = YCreateWidget("Change Setup Pane",panedWidgetClass, ChangeDlg);

    ChangeTopBox = YCreateShadedWidget("Change Setup Title Box", boxWidgetClass,
                                              ChangePane, MediumShade);


    ChangeForm = YCreateShadedWidget("Change Device Form", formWidgetClass,
                                              ChangePane, LightShade);

    ChangeLabel = YCreateLabel("Change Device for all Tracks", ChangeForm);

    ChangeBottomBox = YCreateShadedWidget("Change Setup Button Box",
                           boxWidgetClass, ChangePane, MediumShade);

    ChangeOK = YCreateCommand("OK", ChangeBottomBox);
    ChangeCancel = YCreateCommand("Cancel", ChangeBottomBox);

    /* set up the device menu */

    ChangeDeviceMenuLabels = (String *)XtMalloc
          (Devices.MaxDevices * sizeof(String));

    for ( i = 0; i < Devices.MaxDevices; i++ )
    {
        ChangeDeviceMenuLabels[i] = XtNewString
            (Mapper_GetDevice(i)->Device.Data.Midi.name);
    }

    if (Devices.MaxDevices == 0)
    {
        ChangeDeviceMenu = YCreateLabel("(no devices)", ChangeForm);
    }
    else
    {
        ChangeDeviceMenu = YCreateOptionMenu(ChangeForm,
                              ChangeDeviceMenuLabels,
                              Devices.MaxDevices,
                              MenuSelection,
                              Midi_ChangesAllTracksCB,
                              (XtPointer)ChangeForm);
    }

    YSetValue(XtParent(ChangeDeviceMenu), XtNfromVert, XtParent(ChangeLabel));
    YSetValue(XtParent(ChangeOK), XtNfromVert, XtParent(ChangeDeviceMenu));
    YSetValue(XtParent(ChangeCancel), XtNfromHoriz, XtParent(ChangeOK));

    XtAddCallback(ChangeOK, XtNcallback, Midi_ChangeOKCB, NULL);
    XtAddCallback(ChangeCancel, XtNcallback, Midi_ChangeCancelCB, NULL);

    if (Devices.MaxDevices > 0)
    {
        YFixOptionMenuLabel(ChangeDeviceMenu);
    }

    op = YPlacePopupAndWarp(ChangeDlg, XtGrabNonexclusive, ChangeOK,
                            ChangeCancel);

END;
}

void
Midi_MuteAllTracksCB(Widget w, XtPointer a, XtPointer b)
{
int i;
ELWindowList thing;
Widget button;
Dimension ButtonWidth;

BEGIN("Midi_MuteAllTracksCB");

    /* refresh all the open event lists */
    for (i = 0; i < MIDIHeaderBuffer.NumTracks; i++)
    {
        Mapper_GetTrack(i)->PlaybackStatus = Playback_Muted;
        if ((thing = (ELWindowList)Midi_ELGetWindowFromTrack(i)) != NULL)
        {
            button = thing->TrackPlaybackStatus;
            YGetValue(button, XtNwidth, &ButtonWidth);
            YSetValue(button, XtNlabel, MIDI_TRACK_MUTE_TEXT);
            YSetValue(button, XtNwidth, ButtonWidth);
        }
    }

    Midi_TrackListSetup();

END;
}


void
Midi_ActivateAllTracksCB(Widget w, XtPointer a, XtPointer b)
{

int i;
ELWindowList thing;
Widget button;
Dimension ButtonWidth;

BEGIN("Midi_ActivateAllTracksCB");

    for (i = 0; i < MIDIHeaderBuffer.NumTracks; i++)
    {
        Mapper_GetTrack(i)->PlaybackStatus = Playback_Enabled;
        if ((thing = (ELWindowList)Midi_ELGetWindowFromTrack(i)) != NULL)
        {
            button = thing->TrackPlaybackStatus;
            YGetValue(button, XtNwidth, &ButtonWidth);
            YSetValue(button, XtNlabel, MIDI_TRACK_PB_TEXT);
            YSetValue(button, XtNwidth, ButtonWidth);
        }
    }

    Midi_TrackListSetup();

END;
}



void Midi_SetupDlg(void);

void Midi_SetupCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_SetupCB");

    Midi_SetupDlg();

END;
}

void Midi_SetupCancelCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_SetupCancelCB");

    YDestroyOptionMenu(TrackingMenu);
    XtDestroyWidget(SetupDlg);

END;
}

void Midi_SetupSyncCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_SetupSyncCB");

    SetupSync = !SetupSync;

END;
}


void Midi_SetupMaintainTempoCB(Widget w, XtPointer a, XtPointer b)
{
BEGIN("Midi_SetupMaintainTempoCB");

        MaintainTempo = !MaintainTempo;

END;
}


/*
 * Function:     Midi_SetupOKCB
 *
 * Description:  Gets any return values from the Dialog.
 *
 */
void Midi_SetupOKCB(Widget w, XtPointer a, XtPointer b)
{
char *EventBufStr;
int   EventBufSize;
char *DeviceStr;

BEGIN("Midi_SetupOKCB");

    YGetValue(SetupEventBufSizeField, XtNstring, &EventBufStr);

    EventBufSize = atoi(EventBufStr);

    if (EventBufSize < 1) 
    {
        YQuery(SetupDlg, "Invalid Event Buffer Size", 1, 0, 0, "Continue",
                                                                       NULL);
        END;
    }

    MidiEventBufferSize = EventBufSize;

    YGetValue(SetupDeviceField, XtNstring, &DeviceStr);
    strcpy(Devices.FileDescriptor, DeviceStr);

    if ( !Mapper_DeviceQuery() )
    {
        YQuery(SetupDlg, "Device Invalid or Busy", 1, 0, 0, "Continue", NULL);
        END;
    }

#ifndef SYSTEM_OSS
    MidiPortSync = SetupSync;
#ifdef SYSTEM_ZILOG
    MidiMaintainTempo = MaintainTempo;
#endif /* SYSTEM_ZILOG */
#endif /* not SYSTEM_OSS */

    YDestroyOptionMenu(TrackingMenu);
    XtDestroyWidget(SetupDlg);

END;
}

    
void Midi_SetupDlg(void)
{
    Widget SetupPane;
    Widget SetupTopBox;
    Widget SetupLabel;

    Widget SetupForm;
    Widget RecordDeviceLabel;
    Widget SetupEventBufSizeLabel;
    Widget SetupBottomBox;
    Widget SetupOK;
    Widget SetupCancel;
    Widget SetupHelp = 0;

    Widget SetupDeviceLabel;
    Widget RecordDeviceField;
    Dimension LabelWidth;

#ifndef SYSTEM_OSS
    Widget SetupPortNameLabel;
    Widget SetupPortSyncButton;
#ifdef SYSTEM_ZILOG
    Widget SetupMaintainTempoButton;
#endif /* SYSTEM_ZILOG */
#endif /* not SYSTEM_OSS */

    XPoint op;
    char   TextBuffer[32];

BEGIN("Midi_SetupDlg");

    SetupDlg = XtCreatePopupShell("Midi Setup", transientShellWidgetClass, 
                                     topLevel, NULL, 0);

    SetupPane = YCreateWidget("Midi Setup Pane", panedWidgetClass, SetupDlg);

    SetupTopBox = YCreateShadedWidget("Midi Setup Title Box", boxWidgetClass,
                                              SetupPane, MediumShade);

    SetupLabel = YCreateLabel("Configure MIDI Setup", SetupTopBox);

    
    SetupForm = YCreateShadedWidget("Midi Setup Form", formWidgetClass,
                                              SetupPane, LightShade);
#ifndef SYSTEM_OSS
    SetupPortSyncButton = YCreateToggle("Synchronous Mode", SetupForm,
                                        Midi_SetupSyncCB);
#ifdef SYSTEM_ZILOG
    SetupMaintainTempoButton = YCreateToggle("Maintain Tempo", SetupForm,
                                             Midi_SetupMaintainTempoCB);
#endif /* SYSTEM_ZILOG */
#endif /* not SYSTEM_OSS */

    SetupDeviceLabel = YCreateLabel("Device : ", SetupForm);

    /* we're using the MidiPort as the Device i.e. /dev/sequencer */
    SetupDeviceField = YCreateSurroundedWidget("Device Setup Field",
                           asciiTextWidgetClass, SetupForm, NoShade, NoShade);
 
    if ( Tracks.RecordDevice == -1 )
        strcpy(TextBuffer, "(no device)" );
    else
    {
        if (Tracks.Track != NULL)
        {
            strcpy(TextBuffer, Mapper_GetDevice(Tracks.RecordDevice)->
                                              Device.Data.Midi.name);
        }
        else
        {
            strcpy(TextBuffer, "(null device)");
        }
    }

    RecordDeviceField = YCreateLabel ( TextBuffer, SetupForm);

    RecordDeviceLabel = YCreateLabel("Record Device :", SetupForm);

    SetupEventBufSizeLabel = YCreateLabel("Event Buffer Size:", SetupForm);

    SetupEventBufSizeField = YCreateSurroundedWidget("Event Buffer Size",
                           asciiTextWidgetClass, SetupForm, NoShade, NoShade);

    TrackingMenu = Midi_PlayTrackingMenuButton(SetupForm);

    SetupBottomBox = YCreateShadedWidget("Midi Setup Button Box",
                           boxWidgetClass, SetupPane, MediumShade);

    SetupOK     = YCreateCommand("OK", SetupBottomBox);
    SetupCancel = YCreateCommand("Cancel", SetupBottomBox);

    if (appData.interlockWindow)
    {
        SetupHelp   = YCreateCommand("Help", SetupBottomBox);
        XtAddCallback(SetupHelp,   XtNcallback, Midi_HelpCallback,
                                               "Midi - MIDI Setup");
    }


    XtAddCallback(SetupOK, XtNcallback, Midi_SetupOKCB, NULL);
    XtAddCallback(SetupCancel, XtNcallback, Midi_SetupCancelCB, NULL);

/************************************/
/* Format the form widget contents. */
/************************************/

#ifndef SYSTEM_OSS
#ifdef SYSTEM_ZILOG
    YSetValue(XtParent(SetupMaintainTempoButton), XtNfromVert,
                                         XtParent(SetupPortSyncButton));
    YSetValue(XtParent(SetupDeviceLabel), XtNfromVert,
                                         XtParent(SetupMaintainTempoButton));
    YSetValue(XtParent(SetupDeviceField), XtNfromVert,
                                         XtParent(SetupMaintainTempoButton));
#else /* !SYSTEM_ZILOG */
    YSetValue(XtParent(SetupDeviceLabel), XtNfromVert,
                                         XtParent(SetupPortSyncButton));
    YSetValue(XtParent(SetupDeviceField), XtNfromVert,
                                         XtParent(SetupPortSyncButton));
#endif /* !SYSTEM_ZILOG */
#endif /* !SYSTEM_OSS */

    YSetValue(XtParent(SetupEventBufSizeLabel), XtNfromVert,
                                         XtParent(RecordDeviceLabel));
    YSetValue(XtParent(SetupEventBufSizeField), XtNfromHoriz,
                                         XtParent(SetupEventBufSizeLabel));
    YSetValue(XtParent(SetupEventBufSizeField), XtNfromVert,
                                         XtParent(RecordDeviceLabel));

    YSetValue(XtParent(TrackingMenu), XtNfromVert,
          XtParent(SetupEventBufSizeLabel));

    YSetValue(XtParent(SetupDeviceField), XtNfromHoriz,
                                             XtParent(SetupDeviceLabel));

    YSetValue(XtParent(RecordDeviceLabel), XtNfromVert,
                                             XtParent(SetupDeviceLabel));

    YSetValue(XtParent(RecordDeviceField), XtNfromHoriz,
                                             XtParent(RecordDeviceLabel));

    YSetValue(XtParent(RecordDeviceField), XtNfromVert,
                                             XtParent(SetupDeviceField));

    YSetValue(SetupEventBufSizeField, XtNeditType, XawtextEdit);
    sprintf(TextBuffer, "%d", MidiEventBufferSize);
    YSetValue(SetupEventBufSizeField, XtNstring, TextBuffer);

    YSetValue(SetupDeviceField, XtNeditType, XawtextEdit);
    sprintf(TextBuffer, "%s", Devices.FileDescriptor);
    YSetValue(SetupDeviceField, XtNstring, TextBuffer);

#ifndef SYSTEM_OSS
    SetupSync = MidiPortSync;
    YSetToggleValue(SetupPortSyncButton, SetupSync);
#ifdef SYSTEM_ZILOG
    MaintainTempo = MidiMaintainTempo;
    YSetToggleValue(SetupMaintainTempoButton, MaintainTempo);
#endif /* SYSTEM_ZILOG */
#endif /* not SYSTEM_OSS */

    YGetValue(SetupDeviceField, XtNwidth, &LabelWidth);
    YSetValue(SetupDeviceField, XtNwidth, ((int)LabelWidth * 2 ));
    YSetValue(SetupEventBufSizeField, XtNwidth, ((int)LabelWidth * 2 ));
    YSetValue(RecordDeviceField, XtNwidth, ((int)LabelWidth * 2 ));

    YSetValue(SetupEventBufSizeLabel, XtNjustify, XtJustifyLeft);
    YSetValue(SetupDeviceLabel, XtNjustify, XtJustifyLeft);
    YSetValue(RecordDeviceLabel, XtNjustify, XtJustifyLeft);

    YGetValue(SetupEventBufSizeLabel, XtNwidth, &LabelWidth);
    YSetValue(SetupDeviceLabel, XtNwidth, LabelWidth);
    YSetValue(RecordDeviceLabel, XtNwidth, LabelWidth);

    op = YPlacePopupAndWarp(SetupDlg, XtGrabNonexclusive, SetupOK, SetupCancel);

    YFixOptionMenuLabel(TrackingMenu);
    YAssertDialogueActions(SetupDlg, SetupOK, SetupCancel, SetupHelp);
END;
}


/*
 * Still unstable - no checking for length or integrity of fields
 * 
 */
void
Midi_InitialPatchesDlg(void)
{
    Widget PatchPane;
    Widget PatchTopBox;
    Widget PatchLabel;
    Widget PatchForm;
    Widget PatchBottomBox;
    Widget PatchOK;
    Widget PatchValueLabel[16];
    XPoint op;
    Dimension LabelWidth;

    char TextBuffer[32];
    char Patches[16][3];
    int i;

    PatchDlg = XtCreatePopupShell("Patch Setup", transientShellWidgetClass, 
                                     topLevel, NULL, 0);

    PatchPane = YCreateWidget("Patch Setup Pane", panedWidgetClass, PatchDlg);

    PatchTopBox = YCreateShadedWidget("Patch Setup Title Box", boxWidgetClass,
                                              PatchPane, MediumShade);

    PatchLabel = YCreateLabel("Initial External MIDI Patches", PatchTopBox);

    
    PatchForm = YCreateShadedWidget("Patch Setup Form", formWidgetClass,
                                              PatchPane, LightShade);
    for ( i = 0; i < 16; i++ )
    {
        sprintf(TextBuffer,"MIDI Channel %d",i);
        PatchValueLabel[i] = YCreateLabel(TextBuffer, PatchForm);

        sprintf(TextBuffer,"MIDI Patch Channel %d",i);
        PatchValueField[i] = YCreateSurroundedWidget(TextBuffer,
                     asciiTextWidgetClass, PatchForm, NoShade, NoShade);

        if ( i > 0 )
        {
            YSetValue(XtParent(PatchValueField[i]),XtNfromVert,
                                XtParent(PatchValueField[i-1]));

            YSetValue(XtParent(PatchValueLabel[i]),XtNfromVert,
                                XtParent(PatchValueLabel[i-1]));
        }

        YSetValue(PatchValueLabel[i], XtNjustify, XtJustifyLeft);

        YSetValue(XtParent(PatchValueField[i]),XtNfromHoriz,
                             XtParent(PatchValueLabel[i]));

        /* write the patch values */
        sprintf(Patches[i],"%d",InitialPatches[i]);
        YSetValue(PatchValueField[i], XtNeditType, XawtextEdit);
        YSetValue(PatchValueField[i], XtNstring, Patches[i]);
    }

    /* reset all the label widths to the last (widest) */
    YGetValue(PatchValueLabel[15], XtNwidth, &LabelWidth);
    for ( i = 0; i <15; i++ )
        YSetValue(PatchValueLabel[i], XtNwidth, LabelWidth);

    PatchBottomBox = YCreateShadedWidget("Patch Setup Button Box",
                           boxWidgetClass, PatchPane, MediumShade);
                           
    PatchOK = YCreateCommand("OK", PatchBottomBox);

    XtAddCallback(PatchOK, XtNcallback, Midi_PatchOKCB, NULL);

    op = YPlacePopupAndWarp(PatchDlg, XtGrabNonexclusive, PatchOK, PatchOK);

    YAssertDialogueActions(PatchDlg, PatchOK, PatchOK, PatchOK);

}


void
Midi_PatchOKCB(Widget w, XtPointer a, XtPointer b)
{
    int i;
    char *Patches;
    BEGIN("Midi_PatchOKCB");

    /* write the patches back from their temporary strings */
    for ( i = 0; i < 16; i++ )
    {
        YGetValue(PatchValueField[i], XtNstring, &Patches);
        InitialPatches[i] = (byte) atoi(Patches);
        if ( InitialPatches[i] > 127 )
        {
            YQuery(PatchDlg, "Invalid Patch Number", 1, 0, 0, "Continue",
                                                                       NULL);
            END;
        }
    }

    XtDestroyWidget(PatchDlg);

END;
}

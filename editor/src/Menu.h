
/*
   Musical Notation Editor for X, Chris Cannam 1994

   Headers for Menu creation/use files
*/


#define CALLBACK(X) void X (Widget w, XtPointer a, XtPointer b)


#ifndef _MUSIC_MENU_H_
#define _MUSIC_MENU_H_


#include "General.h"
#include "Tags.h"
#include "Stave.h"
#include "Yawn.h"



extern void    IssueMenuComplaint(String);

extern void    EnterMenuMode(MenuMode);
extern void    LeaveMenuMode(MenuMode);
extern Boolean QueryMenuMode(MenuMode);

extern void    InstallFileMenu(Widget);
extern void    InstallEditMenu(Widget);
extern void    InstallChordMenu(Widget);
extern void    InstallGroupMenu(Widget);
extern void    InstallTextMenu(Widget);
extern void    InstallBarMenu(Widget);
extern void    InstallPaletteMenu(Widget);
extern void    InstallMarkMenu(Widget);
extern void    InstallStaveMenu(Widget);
extern void    InstallFilterMenu(Widget);
extern void    MenuCleanUp(void);
extern void    FileMenuCleanUp(void);
extern void    FilterMenuCleanUp(void);

extern CALLBACK( Unimplemented );



extern YMenuId fileMenuId;

extern void    AddStaveToFileMenu  (MajorStave, String, String);
extern String  GetStaveName        (MajorStave);
extern String  GetStaveFileName    (MajorStave);

extern void    FileMenuMarkChanged (MajorStave, Boolean);
extern Boolean FileMenuIsChanged   (MajorStave);

extern Boolean FileStaveExists     (String);
extern void    FileChangeToStave   (String);
extern void    FileCloseStave      (String);

/* The following are all in MenuTools.c, for historical reasons */

extern CALLBACK( FileMenuWriteMidi  );
extern CALLBACK( FileMenuSequence   );
extern CALLBACK( FileMenuPlayMidi   );
extern CALLBACK( FileMenuImportMidi );

extern CALLBACK( FileMenuExportMusicTeX  );
extern CALLBACK( FileMenuExportOpusTeX   );
extern CALLBACK( FileMenuExportPMX       );

extern void TrackingCleanUp(void);
extern void FileMenuEditILCallback(String);
extern void FileMenuFollowILCallback(String);



extern CALLBACK( EditMenuUndo           ); /* in Undo.c */
extern CALLBACK( EditMenuRedo           ); /* in Undo.c */
extern CALLBACK( EditMenuDelete         );
extern CALLBACK( EditMenuCopy           );
extern CALLBACK( EditMenuCut            );
extern CALLBACK( EditMenuPaste          );
extern CALLBACK( EditMenuShowClipboard  );
extern CALLBACK( EditMenuSelectBar      );
extern CALLBACK( EditMenuSelectStaff    );




extern CALLBACK( ChordMenuChangeChord      );
extern CALLBACK( ChordMenuSearchReplace    );
extern CALLBACK( ChordMenuChord            );
extern CALLBACK( ChordMenuUnchord          );
extern CALLBACK( ChordMenuCreateChord      );
extern CALLBACK( ChordMenuNameChord        );
extern CALLBACK( ChordMenuUnnameChord      );
extern CALLBACK( ChordMenuHideNames        );
extern CALLBACK( ChordMenuShowNames        );
extern CALLBACK( ChordMenuShowAllNames     );
extern CALLBACK( ChordMenuShowAllNoteNames );


extern CALLBACK( StaveMenuRenameStave  );
extern CALLBACK( StaveMenuNewStave     );
extern CALLBACK( StaveMenuDeleteStave  );
extern CALLBACK( StaveMenuEmptyStave   );
extern CALLBACK( StaveMenuFillToEnd    );
extern CALLBACK( StaveMenuConnectBelow );
extern CALLBACK( StaveMenuDisconnect   );
extern CALLBACK( StaveMenuAssignPatch  );
extern CALLBACK( StaveMenuShowPatches  );

extern void FillStaffsToEnd(MajorStave, int, int);



extern CALLBACK( GroupMenuTie          );
extern CALLBACK( GroupMenuSlur         );
extern CALLBACK( GroupMenuCrescendo    );
extern CALLBACK( GroupMenuDecrescendo  );
extern CALLBACK( GroupMenuRemove       );
extern CALLBACK( GroupMenuBeam         );
extern CALLBACK( GroupMenuSimpleTuplet );
extern CALLBACK( GroupMenuTuplet       );
extern CALLBACK( GroupMenuGrace        );
extern CALLBACK( GroupMenuBreakGroup   );
extern CALLBACK( GroupMenuTranspose    );
extern CALLBACK( GroupMenuAutoBeam     );
extern CALLBACK( GroupMenuRetrograde   );
extern CALLBACK( GroupMenuInvert       );



extern void KeyTransposeItemIntoNewKey(Item *, Key *, Key *);

extern CALLBACK( BarMenuTimeSignature  );
extern CALLBACK( BarMenuKeySignature   );
extern CALLBACK( BarMenuMetronome      );
extern CALLBACK( BarMenuRemoveMet      );
extern CALLBACK( BarMenuPlain          );
extern CALLBACK( BarMenuDoubleBar      );
extern CALLBACK( BarMenuRepeatAtStart  );
extern CALLBACK( BarMenuRepeatAtEnd    );
extern CALLBACK( BarMenuEndHere        );
extern CALLBACK( BarMenuRemoveBarlines );
extern CALLBACK( BarMenuRefreshBarlines);

/* BarMenuLabel and BarMenuUnlabel are in MenuText.c, */
/* because they're textual operations.                */




extern CALLBACK( TextMenuTextAbove     );
extern CALLBACK( TextMenuTextAboveBig  );
extern CALLBACK( TextMenuTextBelow     );
extern CALLBACK( TextMenuTextBelowI    );
extern CALLBACK( TextMenuDynamic       );
extern CALLBACK( TextMenuUnlabelGroup  );
extern CALLBACK( TextMenuUnlabelBar    );
extern CALLBACK( TextMenuUnlabelStaff  );

/* and holding these for MenuBar.c -- the implementation's easier */
/* in the context of a textual manipulation file like this one -- */

extern CALLBACK( BarMenuLabel          );
extern CALLBACK( BarMenuUnlabel        );

/* and this is here just for calling from IO.c -- it's in MenuText.c */

extern void MaybeConvertDynamics(MajorStave, int); /* bit grimy */



extern CALLBACK( MarkMenuMark    );
extern CALLBACK( MarkMenuUnmark  );



extern CALLBACK( FilterMenuDumpStaff      );
extern CALLBACK( FilterMenuDumpAllStaves  );
extern CALLBACK( FilterMenuReadStaff      );
extern CALLBACK( FilterMenuApplyFilter    );


#endif /* _MUSIC_MENU_H_ */


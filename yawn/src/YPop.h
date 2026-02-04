
/* YAWN callback/action management header */
/* Chris Cannam, March 1995               */

/* This is a private header */

#ifndef _YPOP_H_
#define _YPOP_H_

extern Boolean _yShouldWarp;

extern void _YDialogueDefaultAction (Widget, XEvent *, String *, Cardinal *);
extern void _YDialogueCancelAction  (Widget, XEvent *, String *, Cardinal *);
extern void _YDialogueHelpAction    (Widget, XEvent *, String *, Cardinal *);

#endif /* _YPOP_H_ */

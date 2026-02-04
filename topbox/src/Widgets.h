
#ifndef _MUSIC_WIDGETS_
#define _MUSIC_WIDGETS_

extern Boolean serverStarted;

extern void    CreateApplicationWidgets(void);
extern Widget  CreateButton(String, Widget, WidgetClass);
				            /* name, parent, class */
extern void    ClientFinished(char *);

extern Widget aboutButton;	/* hacky IL client */

#endif /* _MUSIC_WIDGETS_ */



#ifndef _YAWN_TOOLBAR_H_
#define _YAWN_TOOLBAR_H_

#include <Yawn.h>

extern void _YAddToolbarButton(char *,          /* widget name */ 
			       Widget,          /* menuButton  */
			       unsigned char *, /* bitmap      */
			       XtCallbackProc,  /* callback    */
			       XtPointer,       /* client_data */
			       unsigned long);  /* mode mask   */

extern void _YToolbarMode(Widget, unsigned long); /* menuButton, mode */

#endif

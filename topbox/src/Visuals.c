
/* Musical Notation Editor for X, Chris Cannam 1994 */
/* Visual record setup functions */


/*
   For many of the Visual lists in this file, the order in which the
   entries appear _is_ important.  Frequently, other functions simply
   reference these by index, and would begin to get things wrong if
   you changed the ordering.  If you must do so, check very carefully
   for references in other files before you do.
*/


#include "General.h"
#include "Visuals.h"
#include "Yawn.h"

#include "rose.xbm"
#include "rose_mask.xbm"
#include "grey.xbm"
#include "lightgrey.xbm"

Pixmap roseMap      = 0;
Pixmap roseMaskMap  = 0;
Pixmap greyMap      = 0;
Pixmap lightGreyMap = 0;


Result InitialiseVisuals(void)
{
  Window w;

  Begin("InitialiseVisuals");

  w = RootWindowOfScreen(XtScreen(topLevel));

  /* changed back from YCreatePixmapFromData, cc 2/95 */

  roseMap = XCreateBitmapFromData
    (XtDisplay(topLevel),w, rose_bits, rose_width, rose_height);

  roseMaskMap = XCreateBitmapFromData
    (XtDisplay(topLevel),w, rose_mask_bits, rose_mask_width, rose_mask_height);

  greyMap = YCreatePixmapFromData
    (grey_bits, grey_width, grey_height, NoShade);

  lightGreyMap = YCreatePixmapFromData
    (lightgrey_bits, lightgrey_width, lightgrey_height, NoShade);

  Return(Succeeded);
}


void CleanUpVisuals(void)
{
  Begin("CleanUpVisuals");

  if (!display) End;

  if (     roseMap) XFreePixmap(display,      roseMap);
  if ( roseMaskMap) XFreePixmap(display,  roseMaskMap);
  if (     greyMap) XFreePixmap(display,      greyMap);
  if (lightGreyMap) XFreePixmap(display, lightGreyMap);

  roseMap = roseMaskMap = greyMap = lightGreyMap = 0;

  End;
}


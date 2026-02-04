
/* GC.c */
/* Musical Notation Editor, Chris Cannam 1994: Graphics Context Routines */

/* {{{ Includes */

#include "General.h"
#include "Yawn.h"

/* }}} */
/* {{{ Declarations */

XFontStruct *timeSigFont = NULL;
XFontStruct *bigFont     = NULL;
XFontStruct *littleFont  = NULL;
XFontStruct *tinyFont    = NULL;
XFontStruct *italicFont  = NULL;
XFontStruct *chordFont   = NULL;
XFontStruct *dynamicFont = NULL;

GC     drawingGC;
GC      dashedGC;
GC         xorGC;
GC        beamGC;
GC    greyFillGC;
GC   lightFillGC;
GC       clearGC;
GC        copyGC;
GC     timeSigGC;
GC     bigTextGC;
GC  littleTextGC;
GC    tinyTextGC;
GC  italicTextGC;
GC dynamicTextGC;
GC   chordNameGC;

Boolean oneD;

/* }}} */

/* {{{ Create */

void CreateGCs(void)
{
  XGCValues     values;
  unsigned long fullGCMask;
  static char   dashes[] = { (char)1, (char)3 };
  static char   greyBits[] = { (char)0x08, (char)0x02 };
  static char   lightGreyBits[] = { (char)0x08, (char)0x00,
				    (char)0x02, (char)0x00 };
  Pixmap        grey, lightGrey;

  Begin("CreateGCs");

  oneD = (DefaultDepthOfScreen(XtScreen(topLevel)) == 1);

  fullGCMask = GCFunction | GCLineWidth;

  if (XGetGCValues(display,
		   DefaultGCOfScreen(XtScreen(topLevel)),
		   fullGCMask, &values) == 0)
    Error("Could not get default graphics context values");

  values.function = GXcopy;
  copyGC       = YCreateGC(fullGCMask, &values, NoShade, False);
  clearGC      = YCreateGC(fullGCMask, &values, NoShade, True);

  values.line_width = 0;
  drawingGC    = YCreateGC(fullGCMask, &values, NoShade, False);

  values.line_style = LineOnOffDash;
  dashedGC     = YCreateGC(fullGCMask | GCLineStyle, &values, NoShade, False);
  XSetDashes(display, dashedGC, 0, dashes, 2);

  values.function = GXinvert;
  xorGC        = YCreateGC(fullGCMask, &values, MediumShade, False);

  values.function = GXcopy;
  values.line_width = 3;
  beamGC       = YCreateGC(fullGCMask, &values, NoShade, False);

  values.fill_style = FillStippled;

  grey = XCreateBitmapFromData
    (display, RootWindowOfScreen(XtScreen(topLevel)), greyBits, 4, 2); 

  values.stipple = grey;
  greyFillGC   = YCreateGC(fullGCMask | GCFillStyle | GCStipple,
			   &values, NoShade, False);

  lightGrey = XCreateBitmapFromData
    (display, RootWindowOfScreen(XtScreen(topLevel)), lightGreyBits, 4, 4);

  values.stipple = lightGrey;
  lightFillGC   = YCreateGC(fullGCMask | GCFillStyle | GCStipple,
			    &values, NoShade, False);

  fullGCMask   |= GCFont;

  timeSigFont   = YLoadQueryFont(appData.timeSignatureFont);
  bigFont       = YLoadQueryFont(appData.bigTextFont);
  littleFont    = YLoadQueryFont(appData.littleTextFont);
  tinyFont      = YLoadQueryFont(appData.tinyTextFont);
  italicFont    = YLoadQueryFont(appData.italicTextFont);
  chordFont     = YLoadQueryFont(appData.chordNameFont);
  dynamicFont   = YLoadQueryFont(appData.dynamicTextFont);

  values.font   = timeSigFont->fid;
  timeSigGC     = YCreateGC(fullGCMask, &values, NoShade, False);
  values.font   = bigFont->fid;
  bigTextGC     = YCreateGC(fullGCMask, &values, NoShade, False);
  values.font   = littleFont->fid;
  littleTextGC  = YCreateGC(fullGCMask, &values, NoShade, False);
  values.font   = tinyFont->fid;
  tinyTextGC    = YCreateGC(fullGCMask, &values, NoShade, False);
  values.font   = italicFont->fid;
  italicTextGC  = YCreateGC(fullGCMask, &values, NoShade, False);
  values.font   = chordFont->fid;
  chordNameGC   = YCreateGC(fullGCMask, &values, NoShade, False);
  values.font   = dynamicFont->fid;
  dynamicTextGC = YCreateGC(fullGCMask, &values, NoShade, False);

  End;
}

/* }}} */
/* {{{ Destroy */

void GCCleanUp()
{
  Begin("GCClearUp");

  if ( timeSigFont ) XFreeFont(display, timeSigFont);
  if (     bigFont ) XFreeFont(display,     bigFont);
  if (  littleFont ) XFreeFont(display,  littleFont);
  if (    tinyFont ) XFreeFont(display,    tinyFont);
  if (  italicFont ) XFreeFont(display,  italicFont);
  if (   chordFont ) XFreeFont(display,   chordFont);

  End;
}

/* }}} */


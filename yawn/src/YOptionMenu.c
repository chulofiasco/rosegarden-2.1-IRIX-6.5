

#ifndef DEBUG_PLUS_PLUS
#undef DEBUG
#endif


#include <Yawn.h>

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Debug.h>
#include <SysDeps.h>


typedef struct _YOptionMenuListRec
{
  Widget         button;
  XtCallbackProc callback;
  XtPointer      client_data;
  int            count;
  YMenuId        id;
  YMenuElement  *rec;
  int            deft;
} YOptionMenuListRec, *YOptionMenuList;

static YOptionMenuList optionMenus = NULL;
static int optionMenuCount = 0;


void _YOptionMenuCallback(Widget w, XtPointer a, XtPointer b)
{
  YMenuElement *elt = (YMenuElement *)a;
  int i, j;

  Begin("YOptionMenuCallback");

  /* find the button the laborious way */

  for (i = 0; i < optionMenuCount; ++i) {
    for (j = 0; j < optionMenus[i].count; ++j) {
      if (&optionMenus[i].rec[j] == elt) {
	
	XtVaSetValues(optionMenus[i].button, XtNlabel, elt->label, NULL);
	if (optionMenus[i].callback) {
	  optionMenus[i].callback(optionMenus[i].button,
				  optionMenus[i].client_data, (XtPointer)j);
	}

	End;
      }
    }
  }

  fprintf(stderr, "Warning: _YOptionMenuCallback: couldn't locate elt %p\n", a);

  End;
}


int YGetCurrentOption(Widget w)
{
  int i, j;
  String label;

  Begin("YGetCurrentOption");

  for (i = 0; i < optionMenuCount; ++i) {
    if (optionMenus[i].button == w) {

      XtVaGetValues(w, XtNlabel, &label, NULL);

      for (j = 0; j < optionMenus[i].count; ++j) {
	if (!strcmp(optionMenus[i].rec[j].label, label)) Return(j);
      }

      fprintf(stderr, "Warning: YGetCurrentOption: couldn't locate \"%s\"\n",
	      label);
      Return(-1);
    }
  }

  fprintf(stderr, "Warning: YGetCurrentOption: couldn't locate widget %p\n", w);
  Return(-1);
}


void YSetCurrentOption(Widget w, int option)
{
  int i;

  Begin("YSetCurrentOption");

  for (i = 0; i < optionMenuCount; ++i) {
    if (optionMenus[i].button == w) {

      XtVaSetValues(w, XtNlabel, optionMenus[i].rec[option].label, NULL);
      End;
    }
  }

  fprintf(stderr, "Warning: YGetCurrentOption: couldn't locate widget %p\n", w);
  End;
}


void YDestroyOptionMenu(Widget w)
{
  int i, j;
  Begin("YDestroyOptionMenu");

  for (i = 0; i < optionMenuCount; ++i) {
    if (optionMenus[i].button == w) {

      YDestroyMenu(optionMenus[i].id);

      for (j = 0; j < optionMenus[i].count; ++j) {
	XtFree((void *)optionMenus[i].rec[j].label);
      }

      XtFree((void *)optionMenus[i].rec);
      XtDestroyWidget(optionMenus[i].button);

      if (i < optionMenuCount - 1) {
	memcpy(&optionMenus[i], &optionMenus[i+1],
	       (optionMenuCount - i - 1) * sizeof(YOptionMenuListRec));
      }

      --optionMenuCount;
      End;
    }
  }

  fprintf(stderr, "Warning: YDestroyOptionMenu: couldn't locate widget %p\n",w);
  End;
}


void YFixOptionMenuLabel(Widget button)
{
  int i;
  Begin("YFixOptionMenuLabel");

  for (i = 0; i < optionMenuCount; ++i) {
    if (optionMenus[i].button == button) {
      YSetValue(button, XtNlabel,
		optionMenus[i].rec[optionMenus[i].deft].label);
      End;
    }
  }

  fprintf(stderr, "Warning: YFixOptionMenuLabel: couldn't locate button %p\n",
	  button);
  End;
}


Widget YCreateOptionMenu(Widget parent, String *options, int optcount, int deft,
			 XtCallbackProc callback, XtPointer client_data)
{
  int longest = -1;
  YMenuElement *menu;
  Widget button;
  int i;

  Begin("YCreateOptionMenu");

  menu = (YMenuElement *)XtMalloc(optcount * sizeof(YMenuElement));

  for (i = 0; i < optcount; ++i) {
    menu[i].label = XtNewString(options[i]);
    menu[i].insensitive_mode_mask = 0L;
    menu[i].callback = _YOptionMenuCallback;
    menu[i].toolbar_bitmap = 0;
    menu[i].widget = 0;

    if (longest < 0 || strlen(menu[i].label) > strlen(menu[longest].label)) {
      longest = i;
    }
  }

  button = YCreateMenuButton(menu[longest].label, parent);

  optionMenus = (YOptionMenuList)XtRealloc
    ((char *)optionMenus, (optionMenuCount+1) * sizeof(YOptionMenuListRec));

  optionMenus[optionMenuCount].button = button;
  optionMenus[optionMenuCount].count = optcount;
  optionMenus[optionMenuCount].callback = callback;
  optionMenus[optionMenuCount].client_data = client_data;
  optionMenus[optionMenuCount].rec = menu;
  optionMenus[optionMenuCount].deft  = deft;
  optionMenus[optionMenuCount].id =
    YCreateMenu(button, "option menu", optcount, menu);
  ++optionMenuCount;

  Return(button);
}


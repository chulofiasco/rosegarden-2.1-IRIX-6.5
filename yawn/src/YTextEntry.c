
#include <Yawn.h>

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>

#include <Debug.h>
#include <SysDeps.h>

Widget YCreateTextEntryField(Widget parent, String text, String deft)
{
  Widget form;
  Widget label;
  Widget field;

  Begin("YCreateTextEntryField");

  form = YCreateShadedWidget("YTextEntryFieldForm", formWidgetClass,
			     parent, LightShade);

  label = YCreateLabel("YTextEntryFieldLabel", form);
  YSetValue(label, XtNlabel, text);

  field = YCreateSurroundedWidget("YTextEntryFieldField", asciiTextWidgetClass,
				  form, NoShade, NoShade);
  YSetValue(field, XtNstring, deft);
  YSetValue(XtParent(field), XtNfromHoriz, XtParent(label));

  Return(field);
}


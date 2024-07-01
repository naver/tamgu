/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubutton.cxx
 Date       : 2017/09/01
 Purpose    : buttons (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgubutton.h"
#include "tamgubitmap.h"
#include "tamguimage.h"
#include "atoms.h"
#include "instructions.h"

//We need to declare once again our local definitions.
basebin_hash<buttonMethod>  Tamgubutton::methods;



short Tamgubutton::idtype = 0;


static void button_callback(Fl_Widget *w, void *data) {
	Tamgubutton* button = (Tamgubutton*)data;
	Tamgu* func = button->function;
	TamguCallFunction kfunc(func);

	button->Setreference();
	button->object->Setreference();
	kfunc.arguments.push_back(button);
	kfunc.arguments.push_back(button->object);
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	button->Resetreference();
	button->object->Resetreference();

}



//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubutton::AddMethod(TamguGlobal* global, string name, buttonMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgubutton::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgubutton::InitialisationModule(global,"");
}

bool Tamgubutton::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamgubutton::idtype = global->Getid("button");

	Tamgubutton::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a callback object");
	Tamgubutton::AddMethod(global, "create", &Tamgubutton::MethodCreate, P_SEVEN | P_SIX | P_FIVE, "create(int x, int y, int w, int h, string type, string shape, string label): Create a button. If type is not provided then it is: FL_Regular.");
	Tamgubutton::AddMethod(global, "when", &Tamgubutton::MethodWhen, 65534, "when(string when1 string when2...): Type of event for a button which triggers the callback");
	Tamgubutton::AddMethod(global, "shortcut", &Tamgubutton::MethodShortcut, P_ONE, "shortcut(string keycode): Set a shortcut to activate the button from the keyboard");
	Tamgubutton::AddMethod(global, "color", &Tamgubutton::MethodColor, P_ONE, "color(string code|int code): Set the color of the button");
	Tamgubutton::AddMethod(global, "value", &Tamgubutton::MethodValue, P_ONE | P_NONE, "value(): return the value of the current button");
	Tamgubutton::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the button label alignment");
	Tamgubutton::AddMethod(global, "image", &Tamgubutton::MethodImage, P_ONE | P_TWO | P_THREE, "image(image im, string label, int labelalign): Use the image as a button image");
	Tamgubutton::AddMethod(global, "bitmap", &Tamgubutton::MethodBitmap, P_TWO | P_THREE | P_FOUR, "bitmap(bitmap im, int color, string label, int labelalign): Use the bitmap as a button image");
	Tamgubutton::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamgubutton::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamgubutton::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamgubutton::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamgubutton::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamgubutton::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamgubutton::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamgubutton::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamgubutton::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamgubutton::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamgubutton::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamgubutton::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamgubutton::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamgubutton::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");



	global->newInstance[Tamgubutton::idtype] = new Tamgubutton(global);
	global->RecordCompatibilities(Tamgubutton::idtype);

	return true;
}


Tamgu* Tamgubutton::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgubutton::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamgubutton:: Newiteration(bool direction) {
	return aITERNULL;
}


Tamgu* Tamgubutton::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* h = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* ktype = NULL;
	Tamgu* shape = NULL;
	Tamgu* label;
	if (callfunc->Size() == 7) {
		ktype = callfunc->Evaluate(4, contextualpattern, idthread);
		shape = callfunc->Evaluate(5, contextualpattern, idthread);
		label = callfunc->Evaluate(6, contextualpattern, idthread);
	}
	else {
		if (callfunc->Size() == 6) {
			shape = callfunc->Evaluate(4, contextualpattern, idthread);
			label = callfunc->Evaluate(5, contextualpattern, idthread);
		}
		else
			label = callfunc->Evaluate(4, contextualpattern, idthread);
	}
	if (button != NULL)
		return Returningerror("WND(101): button already created", idthread);
	int shap;
	if (shape == NULL)
		shap = 0;
	else {
		if (shape->Type() == a_string) {
			if (buttonShapes.find(shape->String()) == buttonShapes.end()) {
				string mes = "WND(675): Unknwown button shape: ";
				mes += shape->String();
				return globalTamgu->Returnerror(mes, idthread);
			}
			shap = buttonShapes[shape->String()];
		}
		else
			shap = shape->Integer();
	}
	string thetxt = label->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	if (ktype == NULL)
		thetype = 0;
	else
	if (ktype->Type() == a_string) {
		string btype = ktype->String();
		if (buttontypes.find(btype) == buttontypes.end()) {
			string mes = "WND(675): Unknwown button type: ";
			mes += btype;
			return globalTamgu->Returnerror(mes, idthread);
		}
		thetype = buttontypes[btype];
	}
	else
		thetype = ktype->Integer();

	switch (thetype) {
	case 1: //check
		button = new Fl_Check_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 2://light
		button = new Fl_Light_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer());
		break;
	case 3://Repeat
		button = new Fl_Repeat_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 4://Return
		button = new Fl_Return_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 5://Round
		button = new Fl_Round_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		break;
	case 6://Image
		button = new ButtonImage(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
		image = true;
		break;
	default://regular
		button = new Fl_Button(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	}
	button->type(shap);
	if (function != NULL)
		button->callback(button_callback, this);

	SaveItem(this);
	return aTRUE;
}

Tamgu* Tamgubutton::MethodWhen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (button == NULL)
		return Returningerror("WND(805): Button not initialized", idthread);
	Tamgu* ke;
	int action = 0;
	for (int i = 0; i < callfunc->Size(); i++) {
		ke = callfunc->Evaluate(i, contextualpattern, idthread);
		if (ke->Type() == a_string) {
			if (buttonAction.find(ke->String()) == buttonAction.end()) {
				string mess = "WND(654): Unknown action";
				return globalTamgu->Returnerror(mess, idthread);
			}
			action |= buttonAction[ke->String()];
		}
		else
			action |= ke->Integer();
	}
	button->when(action);
	return aTRUE;
}

Tamgu* Tamgubutton::MethodShortcut(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (button == NULL)
		return Returningerror("WND(805): Button not initialized", idthread);
	//0 is the first parameter and so on...
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		string shortcut = ke->String();
		if (keyCodes.find(shortcut) == keyCodes.end()) {
			shortcut = "WND(656): Unknown key code";
			return globalTamgu->Returnerror(shortcut, idthread);
		}
		button->shortcut(keyCodes[shortcut]);
	}
	else
		button->shortcut(ke->Integer());
	return aTRUE;
}

Tamgu* Tamgubutton::MethodColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (button == NULL)
		return Returningerror("WND(805): Button not initialized", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string shortcut = ke->String();
		if (colors.find(shortcut) == colors.end()) {
			shortcut = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(shortcut, idthread);
		}
		button->color(colors[shortcut]);
	}
	else {
		unsigned int c = ke->Integer();
		button->color(c);
	}
	return aTRUE;
}

Tamgu* Tamgubutton::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (button == NULL)
		return Returningerror("WND(805): Button not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(button->value());
	Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
	button->value(v->Integer());
	return aTRUE;
}

Tamgu* Tamgubutton::MethodImage(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (button == NULL)
		return Returningerror("WND(805): Button not initialized", idthread);
	if (!image)
		return Returningerror("WND(809): Image button required", idthread);
	Tamgu* kimage = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kimage->Type() != Tamguimage::idtype)
		return Returningerror("WND(813): image object required", idthread);
	Fl_Image* fl = ((Tamguimage*)kimage)->image;
	if (callfunc->Size() >= 2) {
		((ButtonImage*)button)->mytext = callfunc->Evaluate(1, contextualpattern, idthread)->String();
		if (callfunc->Size() == 3)
			((ButtonImage*)button)->myalign = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	}
	((ButtonImage*)button)->myimage = fl;
	return aTRUE;
}

Tamgu* Tamgubutton::MethodBitmap(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (button == NULL)
		return Returningerror("WND(805): Button not initialized", idthread);
	if (!image)
		return Returningerror("WND(809): Image button required", idthread);
	Tamgu* kbitmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kbitmap->Type() != Tamgubitmap::idtype)
		return Returningerror("WND(813): image object required", idthread);
	Fl_Bitmap* fl = ((Tamgubitmap*)kbitmap)->bitmap;
	Tamgu* kcolor = callfunc->Evaluate(1, contextualpattern, idthread);
	unsigned int c = FL_BLACK;
	if (kcolor->Type() == a_string) {
		//0 is the first parameter and so on...
		string shortcut = kcolor->String();
		if (colors.find(shortcut) == colors.end()) {
			shortcut = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(shortcut, idthread);
		}
		c = colors[shortcut];
	}
	else
		c = kcolor->Integer();
	if (callfunc->Size() >= 3) {
		((ButtonImage*)button)->mytext = callfunc->Evaluate(2, contextualpattern, idthread)->String();
		if (callfunc->Size() == 4)
			((ButtonImage*)button)->myalign = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
	}
	((ButtonImage*)button)->thecolor = c;
	((ButtonImage*)button)->isbitmap = true;
	((ButtonImage*)button)->myimage = fl;
	return aTRUE;
}





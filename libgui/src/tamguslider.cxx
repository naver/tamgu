/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguslider.cxx
 Date       : 2017/09/01
 Purpose    : slider widget (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguslider.h"
#include "tamguint.h"
#include "instructions.h"

//We need to declare once again our local definitions.
basebin_hash<sliderMethod>  Tamguslider::methods;



short Tamguslider::idtype = 0;

static void slider_callback(Fl_Widget*, void* data) {
	Tamguslider* slid = (Tamguslider*)data;
	Tamgu* func = slid->function;
	TamguCallFunction kfunc(func);
	slid->Setreference();
	slid->object->Setreference();

	kfunc.arguments.push_back(slid);
	kfunc.arguments.push_back(slid->object);
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	slid->Resetreference();
	slid->object->Resetreference();

}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguslider::AddMethod(TamguGlobal* global, string name, sliderMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguslider::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguslider::InitialisationModule(global,"");
}

bool Tamguslider::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamguslider::idtype = global->Getid("slider");

	Tamguslider::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Tamguslider::AddMethod(global, "create", &Tamguslider::MethodCreate, P_SEVEN, "create(int x, int y, int w, int h, int alignment, bool valueslider, string label): Create a slider or a valueslider");
	Tamguslider::AddMethod(global, "resize", &Tamguslider::MethodResize, P_FOUR, "resize(object): make the object resizable");
	Tamguslider::AddMethod(global, "bounds", &Tamguslider::MethodBoundary, P_TWO, "bounds(int x, int y): defines the slider boundary");
	Tamguslider::AddMethod(global, "type", &Tamguslider::MethodType, P_ONE, "type(int x): Value slider type");
	Tamguslider::AddMethod(global, "boxtype", &Tamguslider::MethodBoxType, P_ONE, "boxtype(int x): Define the slider box type");
	Tamguslider::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the slider alignement");
	Tamguslider::AddMethod(global, "step", &Tamguslider::MethodStep, P_ONE, "step(int): define the slider step");
	Tamguslider::AddMethod(global, "value", &Tamguslider::MethodValue, P_ONE | P_NONE, "value(int): define the value for the slider or return its value");
	Tamguslider::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamguslider::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamguslider::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamguslider::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamguslider::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamguslider::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamguslider::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamguslider::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamguslider::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamguslider::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamguslider::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamguslider::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamguslider::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamguslider::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");



	global->newInstance[Tamguslider::idtype] = new Tamguslider(global);
	global->RecordCompatibilities(Tamguslider::idtype);

	return true;
}


Tamgu* Tamguslider::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguslider::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamguslider::Newiteration(bool direction) {
	return aITERNULL;
}


Tamgu* Tamguslider::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (slider != NULL)
		return Returningerror("WND(100): Cannot recreate an existing slider", idthread);
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* align = callfunc->Evaluate(4, contextualpattern, idthread);
	bool valueslider = callfunc->Evaluate(5, contextualpattern, idthread)->Boolean();
	string label = callfunc->Evaluate(6, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	if (valueslider)
		slider = new Fl_Value_Slider(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	else
		slider = new Fl_Slider(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	slider->align(align->Integer());
	slider->type(1);
	slider->step(1);
	
	if (function != NULL)
		slider->callback(slider_callback, this);
	SaveItem(this);
	return aTRUE;
}

Tamgu* Tamguslider::MethodResize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (slider == NULL)
		return Returningerror("WND(101): No slider available", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	slider->resize(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Tamgu* Tamguslider::MethodBoundary(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (slider == NULL)
		return Returningerror("WND(101): No slider available", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	slider->bounds(x->Integer(), y->Integer());
	return aTRUE;
}

Tamgu* Tamguslider::MethodType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (slider == NULL)
		return Returningerror("WND(101): No slider available", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	if (x->Type() == a_string) {
		string buf = x->String();
		if (slidertype.find(buf) == slidertype.end())
			return Returningerror("WND(206): Wrong slider type", idthread);
		slider->type(slidertype[buf]);
	}
	else
		slider->type(x->Integer());
	return aTRUE;
}

Tamgu* Tamguslider::MethodBoxType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (slider == NULL)
		return Returningerror("WND(101): No slider available", idthread);
	Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	if (pos->Type() == a_string) {
		if (boxTypes.find(pos->String()) == boxTypes.end()) {
			string mes = "WND(767): Unknown type:";
			mes += pos->String();
			return globalTamgu->Returnerror(mes, idthread);
		}
		else
			slider->box((Fl_Boxtype)boxTypes[pos->String()]);
	}
	else
		slider->box((Fl_Boxtype)pos->Integer());
	return aTRUE;
}

Tamgu* Tamguslider::MethodStep(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (slider == NULL)
		return Returningerror("WND(101): No slider available", idthread);
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	int i = x->Integer();
	slider->step(i);
	return aTRUE;
}

Tamgu* Tamguslider::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (slider == NULL)
		return Returningerror("WND(101): No slider available", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(slider->value());
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	slider->value(x->Integer());
	return aTRUE;
}






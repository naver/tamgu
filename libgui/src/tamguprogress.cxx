/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprogress.cxx
 Date       : 2017/09/01
 Purpose    : progress bar (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguprogress.h"
#include "instructions.h"

static void progress_callback(Fl_Widget*, void* data) {
	Tamguprogress* cnt = (Tamguprogress*)data;
	Tamgu* func = cnt->function;
	TamguCallFunction kfunc(func);

	cnt->Setreference();
	cnt->object->Setreference();
	kfunc.arguments.push_back(cnt);
	kfunc.arguments.push_back(cnt->object);
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	cnt->Resetreference();
	cnt->object->Resetreference();
}


//We need to declare once again our local definitions.
basebin_hash<progressMethod>  Tamguprogress::methods;



short Tamguprogress::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprogress::AddMethod(TamguGlobal* global, string name, progressMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguprogress::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguprogress::InitialisationModule(global,"");
}

bool Tamguprogress::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamguprogress::idtype = global->Getid("progress");

	Tamguprogress::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");

	Tamguprogress::AddMethod(global, "create", &Tamguprogress::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a progress bar");
	Tamguprogress::AddMethod(global, "minimum", &Tamguprogress::MethodMinimum, P_ONE | P_NONE, "minimum(float x): defines the minimum or return the minimum");
	Tamguprogress::AddMethod(global, "maximum", &Tamguprogress::MethodMaximum, P_ONE | P_NONE, "maximum(float x): defines the maximum or return the maximum");
	Tamguprogress::AddMethod(global, "value", &Tamguprogress::MethodValue, P_ONE | P_NONE, "value(float): define the value for the progress bar or return its value");
	Tamguprogress::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamguprogress::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamguprogress::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamguprogress::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamguprogress::AddMethod(global, "barcolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "barcolor(int color): Color for the progression bar");
	Tamguprogress::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamguprogress::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamguprogress::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamguprogress::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");



	global->newInstance[Tamguprogress::idtype] = new Tamguprogress(global);
	global->RecordCompatibilities(Tamguprogress::idtype);

	return true;
}


Tamgu* Tamguprogress::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguprogress::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamguprogress:: Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamguprogress::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (progress != NULL)
		return Returningerror("WND(220): Cannot recreate an existing wprogress bar", idthread);
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	progress = new Fl_Progress(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	
	if (function != NULL) {
		if (function->Size() != 2)
			return globalTamgu->Returnerror("TAMGUI(881): Error, wrong function arguments", idthread);
		progress->callback(progress_callback, this);
	}
	return aTRUE;
}

Tamgu* Tamguprogress::MethodMinimum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (progress == NULL)
		return Returningerror("WND(221): No wprogress bar available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = progress->minimum();
		return globalTamgu->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	progress->minimum(a);
	return aTRUE;
}

Tamgu* Tamguprogress::MethodMaximum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (progress == NULL)
		return Returningerror("WND(221): No wprogress bar available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = progress->maximum();
		return globalTamgu->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	progress->maximum(a);
	return aTRUE;
}

Tamgu* Tamguprogress::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (progress == NULL)
		return Returningerror("WND(221): No wprogress bar available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = progress->value();
		return globalTamgu->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	progress->value(a);
	return aTRUE;
}






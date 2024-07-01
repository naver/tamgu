/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgucounter.cxx
 Date       : 2017/09/01
 Purpose    : counter display (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgucounter.h"
#include "instructions.h"
#include "tamgufvector.h"

static void counter_callback(Fl_Widget*, void* data) {
	Tamgucounter* cnt = (Tamgucounter*)data;
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
basebin_hash<counterMethod>  Tamgucounter::methods;



short Tamgucounter::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgucounter::AddMethod(TamguGlobal* global, string name, counterMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgucounter::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgucounter::InitialisationModule(global,"");
}

bool Tamgucounter::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamgucounter::idtype = a_counter;

	Tamgucounter::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Tamgucounter::AddMethod(global, "create", &Tamgucounter::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a wcounter");
	Tamgucounter::AddMethod(global, "bounds", &Tamgucounter::MethodBoundary, P_NONE | P_TWO, "bounds(float x, float y): defines the wcounter boundary");
	Tamgucounter::AddMethod(global, "type", &Tamgucounter::MethodType, P_ONE, "type(bool normal): if 'true' then normal wcounter or simple wcounter");
	Tamgucounter::AddMethod(global, "step", &Tamgucounter::MethodStep, P_ONE, "step(double): define the wcounter step");
	Tamgucounter::AddMethod(global, "lstep", &Tamgucounter::MethodLargeStep, P_ONE, "lstep(double): define the large wcounter step");
	Tamgucounter::AddMethod(global, "steps", &Tamgucounter::MethodSteps, P_TWO, "steps(double): define the wcounter steps normal and large.");
	Tamgucounter::AddMethod(global, "value", &Tamgucounter::MethodValue, P_ONE | P_NONE, "value(float): define the value for the wcounter or return its value");
	Tamgucounter::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamgucounter::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamgucounter::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamgucounter::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamgucounter::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamgucounter::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamgucounter::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamgucounter::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamgucounter::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamgucounter::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamgucounter::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamgucounter::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamgucounter::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamgucounter::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamgucounter::AddMethod(global, "textsize", &Tamgucounter::MethodFontSize, P_ONE | P_NONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
	Tamgucounter::AddMethod(global, "font", &Tamgucounter::MethodFont, P_ONE | P_NONE, "font(int s): set or return the text font");
	Tamgucounter::AddMethod(global, "textcolor", &Tamgucounter::MethodTextColor, P_ONE | P_NONE, "textcolor(string code|int code): Set the color of the text");


	global->newInstance[Tamgucounter::idtype] = new Tamgucounter(global);
	global->RecordCompatibilities(Tamgucounter::idtype);

	return true;
}


Tamgu* Tamgucounter::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgucounter::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamgucounter:: Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamgucounter::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (counter != NULL)
		return Returningerror("WND(210): Cannot recreate an existing wcounter", idthread);
	
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	counter = new Fl_Counter(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	if (function != NULL) {
		if (function->Size() != 2)
			return globalTamgu->Returnerror("TAMGUI(881): Error, wrong function arguments", idthread);
		counter->callback(counter_callback, this);
	}
	return aTRUE;
}

Tamgu* Tamgucounter::MethodBoundary(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	double a, b;
	if (callfunc->Size() == 0) {
		a = counter->minimum();
		b = counter->maximum();
        Tamgufvector* v = globalTamgu->Providefvector();
		v->values.push_back(a);
		v->values.push_back(b);
		return v;
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	b = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	counter->bounds(a, b);
	return aTRUE;
}

Tamgu* Tamgucounter::MethodType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	bool nstep = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (nstep)
		counter->type(FL_NORMAL_COUNTER);
	else
		counter->type(FL_SIMPLE_COUNTER);
	return aTRUE;
}

Tamgu* Tamgucounter::MethodStep(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	double kstep = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	counter->step(kstep);
	return aTRUE;
}

Tamgu* Tamgucounter::MethodLargeStep(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	double kstep = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	counter->lstep(kstep);
	return aTRUE;
}

Tamgu* Tamgucounter::MethodSteps(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	double nstep = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	double Lstep = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	counter->step(nstep, Lstep);
	return aTRUE;
}

Tamgu* Tamgucounter::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	double a;
	if (callfunc->Size() == 0) {
		a = counter->value();
		return globalTamgu->Providefloat(a);
	}
	a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
	counter->value(a);
	return aTRUE;
}

Tamgu* Tamgucounter::MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(counter->textsize());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	counter->textsize(ke->Integer());
	return aTRUE;
}

Tamgu* Tamgucounter::MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	if (callfunc->Size() == 0) {
		int i = counter->textfont();
		if (ifonts.find(i) != ifonts.end())
			return globalTamgu->Providestring(ifonts[i]);
		return globalTamgu->Provideint(i);
	}
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		string buf = ke->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		counter->textfont(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return Returningerror("WND(658): unknown font", idthread);
		counter->textfont(font);
	}
	return aTRUE;
}

Tamgu* Tamgucounter::MethodTextColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (counter == NULL)
		return Returningerror("WND(211): No wcounter available", idthread);
	if (callfunc->Size() == 0) {
		Fl_Color color = counter->textcolor();
		return globalTamgu->Provideint(color);
	}
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		counter->textcolor(colors[buf]);
	}
	else {
		unsigned int c = ke->Integer();
		counter->textcolor(c);
	}
	return aTRUE;
}






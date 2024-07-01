/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubrowser.cxx
 Date       : 2017/09/01
 Purpose    : generic browser (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgubrowser.h"
#include "instructions.h"

static void browser_callback(Fl_Widget*, void* data) {
	Tamgubrowser* cnt = (Tamgubrowser*)data;
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
basebin_hash<browserMethod>  Tamgubrowser::methods;



short Tamgubrowser::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubrowser::AddMethod(TamguGlobal* global, string name, browserMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgubrowser::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgubrowser::InitialisationModule(global,"");
}

bool Tamgubrowser::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamgubrowser::idtype = global->Getid("browser");

	Tamgubrowser::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Tamgubrowser::AddMethod(global, "create", &Tamgubrowser::MethodCreate, P_FIVE, "create(xywhlabel): Create a browser");
	Tamgubrowser::AddMethod(global, "add", &Tamgubrowser::MethodAdd, P_ONE, "add(label): Add a string to the browser");
	Tamgubrowser::AddMethod(global, "load", &Tamgubrowser::MethodLoad, P_ONE, "load(filename): Load a file into the browser");
	Tamgubrowser::AddMethod(global, "insert", &Tamgubrowser::MethodInsert, P_TWO, "insert(llabel): Insert a label before line l");
	Tamgubrowser::AddMethod(global, "clear", &Tamgubrowser::MethodClear, P_NONE, "clear(): Clear the browser from all values");
	Tamgubrowser::AddMethod(global, "value", &Tamgubrowser::MethodValue, P_NONE, "value(): return the current selected value");
	Tamgubrowser::AddMethod(global, "select", &Tamgubrowser::MethodSelect, P_ONE | P_NONE, "select(): Return selected , string.\rselect(int i): Return string in position i");
	Tamgubrowser::AddMethod(global, "deselect", &Tamgubrowser::MethodDeselect, P_NONE | P_ONE, "deselect(): Deselect all items\rdeselect(int i): Deselect item i");
	Tamgubrowser::AddMethod(global, "columnchar", &Tamgubrowser::MethodColumnchar, P_ONE | P_NONE, "columnchar(): Return the column char separator.\rcolumnchar(string): Set the column char separator");
	Tamgubrowser::AddMethod(global, "formatchar", &Tamgubrowser::MethodFormatchar, P_ONE | P_NONE, "formatchar(): Return the format char.\rformatchar(string): Set the format char");
	Tamgubrowser::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamgubrowser::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamgubrowser::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamgubrowser::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamgubrowser::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamgubrowser::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamgubrowser::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamgubrowser::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamgubrowser::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamgubrowser::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamgubrowser::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamgubrowser::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamgubrowser::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamgubrowser::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamgubrowser::AddMethod(global, "copy", &Tamguwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Tamgubrowser::AddMethod(global, "paste", &Tamguwidget::MethodPaste, P_NONE, "paste(): paste string");
	Tamgubrowser::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");



	global->newInstance[Tamgubrowser::idtype] = new Tamgubrowser(global);
	global->RecordCompatibilities(Tamgubrowser::idtype);

	return true;
}


Tamgu* Tamgubrowser::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgubrowser::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamgubrowser:: Newiteration(bool direction) {
	return aITERNULL;
}


Tamgu* Tamgubrowser::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (browser != NULL)
		return Returningerror("WND(100): Cannot recreate an existing browser", idthread);
	//0 is the first parameter and so on...

	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	if (function != NULL) {
		browser = new Fl_Select_Browser(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
		browser->callback(browser_callback, this);
	}
	else
		browser = new Fl_Browser(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	return aTRUE;
}

Tamgu* Tamgubrowser::MethodAdd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	string v = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	browser->add(STR(v));
	return aTRUE;
}

Tamgu* Tamgubrowser::MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	string v = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	browser->load(STR(v));
	return aTRUE;
}

Tamgu* Tamgubrowser::MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	int i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	string v = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	browser->insert(i, STR(v), 0);
	return aTRUE;
}

Tamgu* Tamgubrowser::MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	browser->clear();
	return aTRUE;
}

Tamgu* Tamgubrowser::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	string v;
	int i = browser->value();
	return globalTamgu->Provideint(i);
}

Tamgu* Tamgubrowser::MethodSelect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	string v;
	int select;
	if (callfunc->Size() == 0)
		select = browser->value();
	else
		select = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (select >= 1 && select <= browser->size())
		v = browser->text(select);
	return globalTamgu->Providestring(v);
}

Tamgu* Tamgubrowser::MethodDeselect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	if (callfunc->Size() == 0)
		browser->deselect();
	else {
		int select = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
		browser->deselect(select);
	}
	return aTRUE;
}

Tamgu* Tamgubrowser::MethodColumnchar(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	string select;
	if (callfunc->Size() == 0) {
		select = browser->column_char();
		return globalTamgu->Providestring(select);
	}
	select = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (select.size() != 0)
		browser->column_char(select[0]);
	return aTRUE;
}

Tamgu* Tamgubrowser::MethodFormatchar(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (browser == NULL)
		return Returningerror("WND(121): Browser widget has not been created yet", idthread);
	string select;
	if (callfunc->Size() == 0) {
		select = browser->format_char();
		return globalTamgu->Providestring(select);
	}
	select = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (select.size() != 0)
		browser->format_char(select[0]);
	return aTRUE;
}






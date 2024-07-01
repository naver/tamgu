/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutabs.cxx
 Date       : 2017/09/01
 Purpose    : window tabs (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutabs.h"
#include "tamgugroup.h"

//We need to declare once again our local definitions.
basebin_hash<tabsMethod>  Tamgutabs::methods;



short Tamgutabs::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutabs::AddMethod(TamguGlobal* global, string name, tabsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgutabs::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgutabs::InitialisationModule(global,"");
}

bool Tamgutabs::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamgutabs::idtype = global->Getid("tabs");

	Tamgutabs::AddMethod(global, "begin", &Tamgutabs::MethodBegin, P_FIVE, "begin(int x, int y, int w , int h, string title): Create a tab window and begin initialisation");
	Tamgutabs::AddMethod(global, "end", &Tamgutabs::MethodEnd, P_NONE, "end(): end tab creation");
	Tamgutabs::AddMethod(global, "current", &Tamgutabs::MethodCurrent, P_NONE | P_ONE, "current(): Return the current active tab");
	Tamgutabs::AddMethod(global, "add", &Tamgutabs::MethodAdd, P_ONE, "add(wgroup g): Add dynamically a new tab");
	Tamgutabs::AddMethod(global, "remove", &Tamgutabs::MethodRemove, P_ONE, "remove(wgroup g): Remove a tab");
	Tamgutabs::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamgutabs::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamgutabs::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamgutabs::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamgutabs::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamgutabs::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamgutabs::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamgutabs::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamgutabs::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamgutabs::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamgutabs::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamgutabs::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamgutabs::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamgutabs::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamgutabs::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");


	global->newInstance[Tamgutabs::idtype] = new Tamgutabs(global);
	global->RecordCompatibilities(Tamgutabs::idtype);

	return true;
}


Tamgu* Tamgutabs::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgutabs::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamgutabs::Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamgutabs::MethodBegin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Tamgu* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (tabs != NULL)
		return Returningerror("WND(101): tabs already created", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	tabs = new Fl_Tabs(x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
	SaveItem(this);
	end = false;
	return aTRUE;
}

Tamgu* Tamgutabs::MethodEnd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (tabs == NULL)
		return Returningerror("WND(101): tabs not created", idthread);
	end = true;
	tabs->end();
	return aTRUE;
}

Tamgu* Tamgutabs::MethodCurrent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (tabs == NULL)
		return Returningerror("WND(101): tabs not created", idthread);
	if (callfunc->Size() == 0) {
		Tamgu_Group* flgroup = (Tamgu_Group*)tabs->value();
		if (flgroup == NULL)
			return Returningerror("WND(800): No active tab", idthread);
		return flgroup->associate;
	}
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() != Tamgugroup::idtype)
		return Returningerror("WND(105): You can only select wgroup object", idthread);
	Tamgugroup* kgroup = (Tamgugroup*)ke;
	if (kgroup->group == NULL)
		return Returningerror("WND(506): Empty group", idthread);
	if (kgroup->group->parent() != tabs)
		return Returningerror("WND(507): This group does not belong to that tab", idthread);
	tabs->value(kgroup->group);
	return aTRUE;
}

Tamgu* Tamgutabs::MethodAdd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (tabs == NULL)
		return Returningerror("WND(101): tabs not created", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == Tamgugroup::idtype) {
		Tamgugroup* kgroup = (Tamgugroup*)ke;
		if (kgroup->group == NULL)
			return Returningerror("WND(506): Empty group", idthread);
		tabs->add(kgroup->group);
		tabs->value(kgroup->group);
		tabs->hide();
		tabs->show();
		return aTRUE;
	}
	return Returningerror("WND(105): You can only add wgroup object", idthread);
}

Tamgu* Tamgutabs::MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (tabs == NULL)
		return Returningerror("WND(101): tabs not created", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == Tamgugroup::idtype) {
		Tamgugroup* kgroup = (Tamgugroup*)ke;
		if (kgroup->group == NULL)
			return Returningerror("WND(506): Empty group", idthread);
		if (kgroup->group->parent() != tabs)
			return Returningerror("WND(507): This group does not belong to that tab", idthread);
		kgroup->group->hide();
		tabs->remove(kgroup->group);
		delete kgroup->group;
		tabs->hide();
		tabs->show();
		return aTRUE;
	}
	return Returningerror("WND(505): You can only remove wgroup object", idthread);
}






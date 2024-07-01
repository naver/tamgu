/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubox.cxx
 Date       : 2017/09/01
 Purpose    : boxes (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgubox.h"

//We need to declare once again our local definitions.
basebin_hash<boxMethod>  Tamgubox::methods;



short Tamgubox::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubox::AddMethod(TamguGlobal* global, string name, boxMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgubox::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgubox::InitialisationModule(global,"");
}

bool Tamgubox::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamgubox::idtype = global->Getid("box");


	Tamgubox::AddMethod(global, "create", &Tamgubox::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create a box");
	Tamgubox::AddMethod(global, "type", &Tamgubox::MethodType, P_ONE, "type(string|int boxtype): Define the box type");
	Tamgubox::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamgubox::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamgubox::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamgubox::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamgubox::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamgubox::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamgubox::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamgubox::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamgubox::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamgubox::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamgubox::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamgubox::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamgubox::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamgubox::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");


	global->newInstance[Tamgubox::idtype] = new Tamgubox(global);
	global->RecordCompatibilities(Tamgubox::idtype);

	return true;
}


Tamgu* Tamgubox::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgubox::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamgubox:: Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamgubox::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Tamgu* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (box != NULL)
		return Returningerror("WND(101): box already created", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	box = new Fl_Box(x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
	SaveItem(this);
	return aTRUE;
}

Tamgu* Tamgubox::MethodType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (box == NULL)
		return Returningerror("WND(102): box not created", idthread);
	Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
	if (pos->Type() == a_string) {
		if (boxTypes.find(pos->String()) == boxTypes.end()) {
			string mes = "WND(767): Unknown type:";
			mes += pos->String();
			return globalTamgu->Returnerror(mes, idthread);
		}
		else
			box->box((Fl_Boxtype)boxTypes[pos->String()]);
	}
	else
		box->box((Fl_Boxtype)pos->Integer());
	return aTRUE;
}






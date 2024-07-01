/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguchoice.cxx
 Date       : 2017/09/01
 Purpose    : choice window (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguchoice.h"
#include "atoms.h"
#include "instructions.h"

static void choice_callback(Fl_Widget*, void* data) {
	ChoiceCallback* wn = (ChoiceCallback*)data;
	Tamguchoice* kwnd = wn->choice;
	Tamgu* func = wn->function;
	TamguCallFunction kfunc(func);

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.arguments.push_back(kwnd);
	kfunc.arguments.push_back(wn->object);
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	kwnd->Resetreference();
	wn->object->Resetreference();
}

//We need to declare once again our local definitions.

//We need to declare once again our local definitions.
basebin_hash<choiceMethod>  Tamguchoice::methods;



short Tamguchoice::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguchoice::AddMethod(TamguGlobal* global, string name, choiceMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguchoice::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguchoice::InitialisationModule(global,"");
}

bool Tamguchoice::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamguchoice::idtype = global->Getid("choice");

	Tamguchoice::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
	Tamguchoice::AddMethod(global, "create", &Tamguchoice::MethodCreate, P_FIVE, "create(int x, int y, int w, int h, string label): Create an choice");
	Tamguchoice::AddMethod(global, "value", &Tamguchoice::MethodValue, P_ONE | P_NONE, "value(int s): set the choice initialization value");
	Tamguchoice::AddMethod(global, "font", &Tamguchoice::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
	Tamguchoice::AddMethod(global, "menu", &Tamguchoice::MethodMenu, P_ONE, "menu(vector s): Initialize the menu");
	Tamguchoice::AddMethod(global, "fontsize", &Tamguchoice::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Tamguchoice::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamguchoice::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamguchoice::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamguchoice::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamguchoice::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamguchoice::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamguchoice::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamguchoice::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamguchoice::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamguchoice::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamguchoice::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamguchoice::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamguchoice::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamguchoice::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamguchoice::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");



	global->newInstance[Tamguchoice::idtype] = new Tamguchoice(global);
	global->RecordCompatibilities(Tamguchoice::idtype);

	return true;
}


Tamgu* Tamguchoice::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguchoice::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamguchoice:: Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamguchoice::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (choice != NULL)
		return Returningerror("WND(100): Cannot recreate an existing choice", idthread);
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	string label = callfunc->Evaluate(4, contextualpattern, idthread)->String();
	strcpy_s(txt, 1024, label.c_str());
	choice = new Fl_Choice(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt);
	SaveItem(this);
	return aTRUE;
}

Tamgu* Tamguchoice::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (choice == NULL)
		return Returningerror("WND(678): choice not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(choice->value());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	choice->value(ke->Integer());
	return aTRUE;
}

Tamgu* Tamguchoice::MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (choice == NULL)
		return Returningerror("WND(678): choice not initialized", idthread);
	if (callfunc->Size() == 0) {
		int i = choice->textfont();
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
		font = fonts[buf];
	}
	else
		font = ke->Integer();
	if (font >= fontmax)
		return Returningerror("WND(658): unknown font", idthread);
	choice->textfont(font);
	return aTRUE;
}

Tamgu* Tamguchoice::MethodMenu(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (choice == NULL)
		return Returningerror("WND(303): No window available", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//we expect a vector
	if (ke->isVectorContainer() == false)
		return Returningerror("WND(109): Wrong parameter, vector expected", idthread);
	Tamgu* menu = (Tamgu*)ke;
	vector<Fl_Menu_Item*> elements;
	Fl_Menu_Item* fl;
	for (int i = 0; i < menu->Size(); i++) {
		//First level
		//each element is a vector
		ke = menu->getvalue(i);
		if (ke->isVectorContainer() == false)
			return Returningerror("WND(109): Wrong element, vector expected", idthread);
		Tamgu* v = ke;
		//then we analyse the sub-elements, the next element should be a vector
		if (v->Size() != 3)
			return Returningerror("WND(109): Wrong element, missing field in a sub-menu", idthread);
		string nn = v->getvalue(0)->String();
		char* n = strdup(STR(nn));
		//second element shoudl be a Callback
		Tamgu* function = v->getvalue(1);
		if (!function->isFunction())
			return Returningerror("WND(109): Wrong element, third field in a sub-menu should be a function", idthread);
		Tamgu* object = v->getvalue(2);
		if (!object->isCallVariable())
			object = object->Eval(aNULL, aNULL, idthread);

		fl = new Fl_Menu_Item;
		fl->labeltype_ = 0;
		fl->labelfont_ = font;
		fl->labelsize_ = fontsize;
		fl->labelcolor_ = fontcolor;
		fl->text = n;
		fl->shortcut_ = 0;
		fl->callback_ = choice_callback;
		fl->user_data_ = new ChoiceCallback(function, object, this);
		fl->flags = 0;
		elements.push_back(fl);
	}
	flmenu = new Fl_Menu_Item[elements.size() + 1];
	flnbelements = elements.size();
	int i;
	for (i = 0; i < elements.size(); i++) {
		flmenu[i] = *elements[i];
		delete elements[i];
	}
	flmenu[i].text = 0;
	flmenu[i].shortcut_ = 0;
	flmenu[i].callback_ = NULL;
	flmenu[i].user_data_ = 0;
	flmenu[i].flags = 0;
	flmenu[i].labeltype_ = 0;
	flmenu[i].labelfont_ = 0;
	flmenu[i].labelsize_ = 12;
	flmenu[i].labelcolor_ = FL_BLACK;
	choice->menu(flmenu);
	return aTRUE;
}

Tamgu* Tamguchoice::MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (choice == NULL)
		return Returningerror("WND(678): choice not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(choice->textsize());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	fontsize = ke->Integer();
	return aTRUE;
}






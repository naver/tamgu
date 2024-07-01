/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwoutput.cxx
 Date       : 2017/09/01
 Purpose    : window output widget (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguwoutput.h"
#include "atoms.h"

//We need to declare once again our local definitions.
basebin_hash<woutputMethod>  Tamguwoutput::methods;



short Tamguwoutput::idtype = 0;



//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguwoutput::AddMethod(TamguGlobal* global, string name, woutputMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguwoutput::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguwoutput::InitialisationModule(global,"");
}

bool Tamguwoutput::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamguwoutput::idtype = global->Getid("woutput");

	Tamguwoutput::AddMethod(global, "create", &Tamguwoutput::MethodCreate, P_SIX, "create(int x, int y, int w, int h, boolean multiline, string label): Create an output");
	Tamguwoutput::AddMethod(global, "value", &Tamguwoutput::MethodValue, P_NONE | P_ONE, "value(string s): set the output buffer");
	Tamguwoutput::AddMethod(global, "wrap", &Tamguwoutput::MethodWrap, P_ONE, "wrap(bool): Set the wrapping mode");
	Tamguwoutput::AddMethod(global, "color", &Tamguwoutput::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
	Tamguwoutput::AddMethod(global, "font", &Tamguwoutput::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
	Tamguwoutput::AddMethod(global, "fontsize", &Tamguwoutput::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Tamguwoutput::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamguwoutput::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamguwoutput::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamguwoutput::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamguwoutput::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamguwoutput::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamguwoutput::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamguwoutput::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamguwoutput::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamguwoutput::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamguwoutput::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamguwoutput::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamguwoutput::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamguwoutput::AddMethod(global, "copy", &Tamguwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Tamguwoutput::AddMethod(global, "paste", &Tamguwidget::MethodPaste, P_NONE, "paste(): paste string");
	Tamguwoutput::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");



	global->newInstance[Tamguwoutput::idtype] = new Tamguwoutput(global);
	global->RecordCompatibilities(Tamguwoutput::idtype);

	return true;
}


Tamgu* Tamguwoutput::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguwoutput::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamguwoutput::Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamguwoutput::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* h = callfunc->Evaluate(3, contextualpattern, idthread);
	multiline = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
	Tamgu* label = callfunc->Evaluate(5, contextualpattern, idthread);
	if (output != NULL)
		return Returningerror("WND(101): output already created", idthread);
	string thetxt = label->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	if (multiline)
		output = new Fl_Multiline_Output(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	else
		output = new Fl_Output(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	output->cursor_color(FL_WHITE);
	SaveItem(this);
	return aTRUE;
}

Tamgu* Tamguwoutput::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    
	if (output == NULL)
		return Returningerror("WND(679): Output not initialized", idthread);
    
	if (callfunc->Size() == 0) {
		buf = output->value();
		return globalTamgu->Providestring(buf);
	}
	buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	output->value(STR(buf));
	return aTRUE;
}

Tamgu* Tamguwoutput::MethodWrap(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (output == NULL)
		return Returningerror("WND(679): Output not initialized", idthread);
	bool mode = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (mode)
		output->align(FL_ALIGN_WRAP);
	else
		output->align(FL_ALIGN_NOWRAP);
	return aTRUE;
}

Tamgu* Tamguwoutput::MethodColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (output == NULL)
		return Returningerror("WND(679): Output not initialized", idthread);
	if (callfunc->Size() == 0) {
		Fl_Color color = output->textcolor();
		return globalTamgu->Provideint(color);
	}
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		output->textcolor(colors[buf]);
	}
	else {
		unsigned int c = ke->Integer();
		output->textcolor(c);
	}
	return aTRUE;
}

Tamgu* Tamguwoutput::MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (output == NULL)
		return Returningerror("WND(679): Output not initialized", idthread);
	if (callfunc->Size() == 0) {
		int i = output->textfont();
		if (ifonts.find(i) != ifonts.end())
			return globalTamgu->Providestring(ifonts[i]);
		return globalTamgu->Provideint(i);
	}
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	if (ke->Type() == a_string) {
		buf = ke->String();
		if (fonts.find(buf) == fonts.end()) {
			buf = "WND(657): Unknown font code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		output->textfont(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return Returningerror("WND(658): unknown font", idthread);
		output->textfont(font);
	}
	return aTRUE;
}

Tamgu* Tamguwoutput::MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (output == NULL)
		return Returningerror("WND(679): Output not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(output->textsize());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	output->textsize(ke->Integer());
	return aTRUE;
}






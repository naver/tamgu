/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwinput.cxx
 Date       : 2017/09/01
 Purpose    : window input widget (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguwinput.h"
#include "atoms.h"
#include "instructions.h"
//We need to declare once again our local definitions.
basebin_hash<winputMethod>  Tamguwinput::methods;



short Tamguwinput::idtype = 0;

static void input_callback(Fl_Widget *w, void *data) {
	Tamguwinput* input = (Tamguwinput*)data;
	Tamgu* func = input->function;

	TamguCallFunction kfunc(func);
	input->Setreference();
	input->object->Setreference();

	kfunc.arguments.push_back(input);
	kfunc.arguments.push_back(input->object);
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());

	input->Resetreference();
	input->object->Resetreference();
}


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguwinput::AddMethod(TamguGlobal* global, string name, winputMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguwinput::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguwinput::InitialisationModule(global,"");
}

bool Tamguwinput::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamguwinput::idtype = global->Getid("winput");

	Tamguwinput::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");

	Tamguwinput::AddMethod(global, "create", &Tamguwinput::MethodCreate, P_SIX, "create(int x, int y, int w, int h, boolean multiline, string label): Create an input");
	Tamguwinput::AddMethod(global, "value", &Tamguwinput::MethodValue, P_ONE | P_NONE, "value()|(string v): return the input buffer or set the initial buffer");
	Tamguwinput::AddMethod(global, "insert", &Tamguwinput::MethodInsert, P_TWO, "insert(string s, int p): insert s at position p in the input");
	Tamguwinput::AddMethod(global, "selection", &Tamguwinput::MethodSelection, P_NONE, "selection(): return the selected text in the input");
	Tamguwinput::AddMethod(global, "color", &Tamguwinput::MethodColor, P_ONE | P_NONE, "color(string c|int c): set or return the text color");
	Tamguwinput::AddMethod(global, "word", &Tamguwinput::MethodWord, P_ONE, "word(int pos): Return the word at position pos");
	Tamguwinput::AddMethod(global, "font", &Tamguwinput::MethodFont, P_ONE | P_NONE, "font(string s): set or return the text font");
	Tamguwinput::AddMethod(global, "fontsize", &Tamguwinput::MethodFontSize, P_ONE | P_NONE, "fontsize(int c): set or return the text font size");
	Tamguwinput::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamguwinput::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamguwinput::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamguwinput::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamguwinput::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamguwinput::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamguwinput::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamguwinput::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamguwinput::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamguwinput::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamguwinput::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamguwinput::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamguwinput::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamguwinput::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamguwinput::AddMethod(global, "copy", &Tamguwidget::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Tamguwinput::AddMethod(global, "paste", &Tamguwidget::MethodPaste, P_NONE, "paste(): paste string");
	Tamguwinput::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");


	global->newInstance[Tamguwinput::idtype] = new Tamguwinput(global);
	global->RecordCompatibilities(Tamguwinput::idtype);

	return true;
}


Tamgu* Tamguwinput::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguwinput::Eval(Tamgu* context, Tamgu* idx, short idthread) {
	if (input == NULL)
		return this;

	if (idx == NULL || idx == aNULL)
		return this;

	string value = input->value();
	long ileft, iright;
	char res = StringIndexes(value, idx, ileft, iright, idthread);

	if (res == 0) {
		if (globalTamgu->erroronkey)
			globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
		return aNOELEMENT;
	}

	if (res == 1)
		return globalTamgu->Providestring(c_char_get(USTR(value), ileft));

	return globalTamgu->Providestring(value.substr(ileft, iright - ileft));
}

TamguIteration* Tamguwinput::Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamguwinput::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* w = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* h = callfunc->Evaluate(3, contextualpattern, idthread);
	multiline = callfunc->Evaluate(4, contextualpattern, idthread)->Boolean();
	Tamgu* label = callfunc->Evaluate(5, contextualpattern, idthread);
	if (input != NULL)
		return Returningerror("WND(101): input already created", idthread);
	string thetxt = label->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	if (multiline)
		input = new Fl_Multiline_Input(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);
	else
		input = new Fl_Input(x->Integer(), y->Integer(), w->Integer(), h->Integer(), txt);

	

	if (function != NULL)
		input->callback(input_callback, this);

	SaveItem(this);
	return aTRUE;
}

Tamgu* Tamguwinput::MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (input == NULL)
		return Returningerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0) {
		buf = input->value();
		return globalTamgu->Providestring(buf);
	}
	buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	input->value(buf.c_str());
	return aTRUE;
}

Tamgu* Tamguwinput::MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (input == NULL)
		return Returningerror("WND(677): Input not initialized", idthread);
	buf = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	Tamgu* pos = callfunc->Evaluate(1, contextualpattern, idthread);
	input->insert(buf.c_str(), pos->Integer());
	return aTRUE;
}

Tamgu* Tamguwinput::MethodSelection(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (input == NULL)
		return Returningerror("WND(677): Input not initialized", idthread);
	int pos = input->position();
	int nb = input->mark();
	buf = input->value();
	buf = buf.substr(pos, nb);
	return globalTamgu->Providestring(buf);
}

Tamgu* Tamguwinput::MethodColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (input == NULL)
		return Returningerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0) {
		Fl_Color color = input->textcolor();
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
		input->textcolor(colors[buf]);
	}
	else {
		unsigned int c = ke->Integer();
		input->textcolor(c);
	}
	return aTRUE;
}

Tamgu* Tamguwinput::MethodWord(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (input == NULL)
		return Returningerror("WND(677): Input not initialized", idthread);
	//first we get our position
	long pos = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	//then we try to extract the word string
	long start = pos;
	const char* ibuf = input->value();
	string res;
	while (start > 0 && c_is_alpha((uchar*)ibuf, pos)) {
		start--;
		pos = start;
	}
	pos = ++start;
	while (pos <= input->size() && c_is_alpha((uchar*)ibuf, start)) {
		res += c_char_get((uchar*)ibuf, pos);
		pos++;
		start = pos;
	}
	return globalTamgu->Providestring(res);
}

Tamgu* Tamguwinput::MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (input == NULL)
		return Returningerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0) {
		int i = input->textfont();
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
		input->textfont(fonts[buf]);
	}
	else {
		int font = ke->Integer();
		if (font >= fontmax)
			return Returningerror("WND(658): unknown font", idthread);
		input->textfont(font);
	}
	return aTRUE;
}

Tamgu* Tamguwinput::MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (input == NULL)
		return Returningerror("WND(677): Input not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(input->textsize());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//0 is the first parameter and so on...
	input->textsize(ke->Integer());
	return aTRUE;
}





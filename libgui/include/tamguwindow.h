/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwindow.h
 Date       : 2017/07/06
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguwindow_h
#define tamguwindow_h

#include "tamguwidget.h"

class Doublewindow;
class Fl_Widget;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguwindow;
//This typedef defines a type "windowMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguwindow::*windowMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------
class TamguCanvas : public Fl_Widget {
public:
	Tamgu* function;
	Tamguwindow* window;
	Tamgu* object;
	int x, y, w, h;
	int sw, sh;

	TamguCanvas(int SW, int SH, int X, int Y, int W, int H) : Fl_Widget(X, Y, W, H, NULL) {
		sw = SW;
		sh = SH;
		x = X;
		y = Y;
		w = W;
		h = H;
		function = NULL;
		window = NULL;
		object = aNULL;
	}
	void draw();
};

class Tamguwindow : public Tamguwidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static basebin_hash<windowMethod> methods;
	
	
	static basebin_hash<bool> nothreads;


	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Doublewindow* window;
	TamguCanvas* scrollcanvas;
	Fl_Scroll* scroll;

	char txt[1024];
	string label;
	bool end;
	vector<int> items;
	Fl_Menu_Item* flmenu;
	int flmenunbelements;
	FLCallback* onclose;
	FLCallback* timeout;
	int ionclose;
	int itimeout;
	bool closed;

	//---------------------------------------------------------------------------------------------------------------------
	Tamguwindow(TamguGlobal* g, Tamgu* parent = NULL);
	~Tamguwindow();

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* index, Tamgu* window, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* index, short idthread);

	short Type() {
		return Tamguwindow::idtype;
	}

	string Typename() {
		return "window";
	}

	Tamgu* Atom(bool forced = false) {
		return this;
	}

	Fl_Widget* Widget() {
		if (scrollcanvas != NULL)
			return scrollcanvas;
		return window;
	}

    void ResetWidget() {
        if (window != NULL)
            delete window;
        window = NULL;
    }

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		globalTamgu->windowmode = true;
		Tamguwindow* w = new Tamguwindow(globalTamgu);
		w->function = f;
		return w;
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, windowMethod func, unsigned long arity, string infos);

	static bool InitialisationModule(TamguGlobal* g, string version);


	
     void Setidtype(TamguGlobal* global);
     void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[idtype])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[idtype].find(n) !=  globalTamgu->infomethods[idtype].end())
              return globalTamgu->infomethods[idtype][n];
             return "Unknown method";
	}

	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isContainer() {
		return false;
	}

	virtual bool iswindowContainer() {
		return false;
	}

	void itemclose();

	bool Update() {
		return update;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodPost(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodBegin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEnd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRun(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodOnClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOntime(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRedrawing(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSetFonts(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGetFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGetFontSizes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodNbFonts(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPlot(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* PlotCoords(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAsk(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAlert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPassword(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInput(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMenu(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodResizable(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSizerange(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodScrollbar(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodModal(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBorder(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBackgroundColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDrawColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRectangle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodVertex(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRectangleFill(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLineShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLoop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLineRotation(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPushMatrix(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPopMatrix(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodScale(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTranslate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRotate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMultmatrix(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTransform_x(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTransform_y(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTransform_dx(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTransform_dy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTransform_vertex(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBeginPoints(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEndPoints(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBeginLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEndLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBeginLoop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEndLoop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBeginPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEndPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBeginComplexPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGap(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEndComplexPolygon(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodArc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCurve(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPushClip(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPopClip(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPie(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPoint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCircle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTextMeasure(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDrawText(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnDragDrop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnKey(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCursorStyle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWindowHide(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFlush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPosition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAwake(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodUnlock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodImage(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBitmap(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		if (globalTamgu->Error(idthread))
			return globalTamgu->Errorobject(idthread);
		
		if (Stopall())
			return aNULL;

		///Specific section to handle threads... One exception the redraw function...
		if (idthread && callfunc->Size() != 0 && !nothreads.check(idname)) {
			//In this case, we do not want to process the method, we postpone its activation
			RecordGUIObject(callfunc, idthread);
			return aNULL;
		}

		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
	}

	void Clear() {
		//To set a variable back to empty

	}

	void Clean() {
		//Your cleaning code
	}

	long Size() {
		return 0;
	}

	uchar Byte() {

		return 0;
	}

	long Integer() {

		return 0;
	}

	double Float() {

		return 0;
	}

	string String() {

		return label;
	}


	bool Boolean() {
        if (window)
            return true;
        return false;
	}

	Tamgu* same(Tamgu* a) {
		if (a->Type() == Tamguwindow::idtype && ((Tamguwindow*)a)->window == window)
			return aTRUE;
		return aFALSE;
	}
};

#endif









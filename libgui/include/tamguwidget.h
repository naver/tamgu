/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwidget.h
 Date       : 2017/07/28
 Purpose    :
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguwidget_h
#define tamguwidget_h

#include "tamgugui.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguwidget;
//This typedef defines a type "widgetMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguwidget::*widgetMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguwidget : public TamguReference {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<widgetMethod> methods;
    
    
    static short idtype; //see Tamguwindow for its implementation...

    static vector<long> idwidgets;
    static vector<Tamguwidget*> pwidgets;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	Tamgu* function;
	Tamgu* object;
    vector<char> begins;
    short calledthread;
	bool update;
    bool del;

    //---------------------------------------------------------------------------------------------------------------------
	Tamguwidget(TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
		calledthread = 0;
		update = true;
		function = NULL;
		object = aNULL;
        del = false;
		g->RecordInTrackerProtected(this);
        idwidgets.push_back(idtracker);
        pwidgets.push_back(this);
	}

	void Addfunction(Tamgu* a) {
		function = a;
	}

	virtual void itemclose() {}

	void clean() {
		Setprotect(true);
		Resetreference();
	}

	short Yourowntype() {
		return Tamguwidget::idtype;
	}

	//For widget, we do not need to store them in a variable...
	bool duplicateForCall() {
		return false;
	}

    void Resetreference(short r = 1);

    virtual void ResetWidget() {}

	//----------------------------------------------------------------------------------------------------------------------

	void RecordGUIObject(TamguCall* callfunc, short idthread);

	virtual Fl_Widget* Widget() {
		return NULL;
	}

    virtual unsigned long CallBackArity() {
        return P_TWO;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//We are interested by the variable...
		object = callfunc->arguments[0];
		if (!object->isCallVariable())
			object = callfunc->Evaluate(0, aNULL, idthread);
		return aTRUE;
	}


	Tamgu* MethodRedraw(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTextFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRGBColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodShow(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFocus(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAlign(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCreated(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCopy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPaste(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCoordinates(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSelectionColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLabel(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLabelType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLabelColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLabelFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLabelSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDrawText(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
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
	Tamgu* MethodTooltip(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodHide(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBackgroundColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------

    Tamgu* Returningerror(string msg, short idthread);
    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.

	virtual Tamgu* same(Tamgu* a) {
		if (a == this)
			return aTRUE;
		return aFALSE;
	}
};

#endif


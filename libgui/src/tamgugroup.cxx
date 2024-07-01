/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgugroup.cxx
 Date       : 2017/09/01
 Purpose    : Widget grouping (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgugroup.h"
#include "instructions.h"

Tamgu_Group::Tamgu_Group(Tamgugroup* kg, int x, int y, int w, int h, const char* txt) : associate(kg), Fl_Group(x, y, w, h, txt) { 
	iassociate = globalTamgu->RecordInTrackerProtected(kg);
}

Tamgu_Group::~Tamgu_Group() {
	if (associate != NULL) {
		if (associate->end == false)
			end();
		associate->itemclose();
		associate->group = NULL;
	}
}

void Tamgu_Group::draw() {
	if (globalTamgu->Checktracker(associate,iassociate)) {
		if (associate->update)
			Fl_Group::draw();
	}

	if (Stopall())
		return;


	if (associate->function != NULL) {
#ifndef WIN32
		Locking _lock(globalTamgu->_printlock);
#endif
		Tamgu* func = associate->function;
		TamguCallFunction kfunc(func);

		associate->Setreference();
		associate->object->Setreference();
		kfunc.arguments.push_back(associate);
		kfunc.arguments.push_back(associate->object);
		kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
		associate->Resetreference();
		associate->object->Resetreference();		
	}
}

//We need to declare once again our local definitions.
basebin_hash<groupMethod>  Tamgugroup::methods;



short Tamgugroup::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgugroup::AddMethod(TamguGlobal* global, string name, groupMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgugroup::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgugroup::InitialisationModule(global,"");
}

bool Tamgugroup::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	

	Tamgugroup::idtype = global->Getid("group");

	Tamgugroup::AddMethod(global, "begin", &Tamgugroup::MethodBegin, P_FIVE, "begin(int x, int y, int w , int h, string title): Create a group within a tab and begin initialisation");
	Tamgugroup::AddMethod(global, "end", &Tamgugroup::MethodEnd, P_NONE, "end(): end group creation");
	Tamgugroup::AddMethod(global, "selectioncolor", &Tamguwidget::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamgugroup::AddMethod(global, "tooltip", &Tamguwidget::MethodTooltip, P_ONE, "tooltip(string msg): Add a tooltip message to the widget");
	Tamgugroup::AddMethod(global, "activate", &Tamgugroup::MethodActivate, P_NONE, "activate(): Active this tab");
	Tamgugroup::AddMethod(global, "show", &Tamguwidget::MethodShow, P_NONE, "show(): Show the widget");
	Tamgugroup::AddMethod(global, "focus", &Tamguwidget::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamgugroup::AddMethod(global, "hide", &Tamguwidget::MethodHide, P_NONE, "hide(): Hide the widget");
	Tamgugroup::AddMethod(global, "labelsize", &Tamguwidget::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamgugroup::AddMethod(global, "labelfont", &Tamguwidget::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamgugroup::AddMethod(global, "labelcolor", &Tamguwidget::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamgugroup::AddMethod(global, "labeltype", &Tamguwidget::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamgugroup::AddMethod(global, "label", &Tamguwidget::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamgugroup::AddMethod(global, "boundaries", &Tamguwidget::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamgugroup::AddMethod(global, "drawcolor", &Tamgugroup::MethodDrawColor, P_ONE, "drawcolor(int c|string c): set the color for the next drawings");
	Tamgugroup::AddMethod(global, "rectangle", &Tamgugroup::MethodRectangle, P_FOUR | P_FIVE, "rectangle(int x, int y, int w , int h , string c|int c): Draw a rectangle with optional color c");
	Tamgugroup::AddMethod(global, "rectanglefill", &Tamgugroup::MethodRectangleFill, P_FOUR | P_FIVE, "rectanglefill(int x, int y, int w , int h , string c|int c): Fill a rectangle with optional color c");
	Tamgugroup::AddMethod(global, "lineshape", &Tamgugroup::MethodLineShape, P_TWO, "lineshape(string type, int width): Select the line shape and its thikness");
	Tamgugroup::AddMethod(global, "polygon", &Tamgugroup::MethodPolygon, P_SIX | P_EIGHT, "polygon(int x, int y, int x1 , int y1, int x2 , int y2 , int x3 , int y3): Draw a polygon x3 and y3 are optional");
	Tamgugroup::AddMethod(global, "line", &Tamgugroup::MethodLine, P_FOUR | P_SIX, "line(int x, int y, int x1 , int y1, int x2 , int y2): Draw a line between points x2 and y2 are optional");
	Tamgugroup::AddMethod(global, "rotation", &Tamgugroup::MethodLineRotation, P_FOUR | P_FIVE, "rotation(float x, float y, float distance , float angle , bool draw): Compute the coordinate of a rotated point from point xy using a distance and an angle. Return a vector of floats with the new coordinates.");
	Tamgugroup::AddMethod(global, "pushmatrix", &Tamgugroup::MethodPushMatrix, P_NONE, "pushmatrix(): Save the current transformation");
	Tamgugroup::AddMethod(global, "popmatrix", &Tamgugroup::MethodPopMatrix, P_NONE, "popmatrix(): Restore the current transformation");
	Tamgugroup::AddMethod(global, "scale", &Tamgugroup::MethodScale, P_TWO | P_ONE, "scale(float x, float y)|scale(float x): Scale the current transformation");
	Tamgugroup::AddMethod(global, "translate", &Tamgugroup::MethodTranslate, P_TWO, "translate(float x, float y): translate the current transformation");
	Tamgugroup::AddMethod(global, "rotate", &Tamgugroup::MethodRotate, P_ONE, "rotate(float d): rotate of degree d the current transformation");
	Tamgugroup::AddMethod(global, "multmatrix", &Tamgugroup::MethodMultmatrix, P_SIX, "multmatrix(float a , float b , float c , float d , float x , float y): combine transformations");
	Tamgugroup::AddMethod(global, "transformx", &Tamgugroup::MethodTransform_x, P_TWO, "transformx(float x , float y): Transform a coordinate X using the current transformation matrix.");
	Tamgugroup::AddMethod(global, "transformy", &Tamgugroup::MethodTransform_y, P_TWO, "transformy(float x , float y): Transform a coordinate Y using the current transformation matrix.");
	Tamgugroup::AddMethod(global, "transformdx", &Tamgugroup::MethodTransform_dx, P_TWO, "transformdx(float x , float y): Transform a distance DX using the current transformation matrix.");
	Tamgugroup::AddMethod(global, "transformdy", &Tamgugroup::MethodTransform_dy, P_TWO, "transformdy(float x , float y): Transform a distance DY using the current transformation matrix.");
	Tamgugroup::AddMethod(global, "transformedvertex", &Tamgugroup::MethodTransform_x, P_TWO, "transformedvertex(float x , float y): add transformations to vertices list.");
	Tamgugroup::AddMethod(global, "beginpoints", &Tamgugroup::MethodBeginPoints, P_NONE, "beginpoints(): Start accumulating vertices");
	Tamgugroup::AddMethod(global, "endpoints", &Tamgugroup::MethodEndPoints, P_NONE, "endpoints(): Stop accumulating vertices");
	Tamgugroup::AddMethod(global, "beginline", &Tamgugroup::MethodBeginLine, P_NONE, "beginline(): Start drawing lines.");
	Tamgugroup::AddMethod(global, "endline", &Tamgugroup::MethodEndLine, P_NONE, "endline(): End drawing lines.");
	Tamgugroup::AddMethod(global, "beginloop", &Tamgugroup::MethodBeginLoop, P_NONE, "beginloop(): Start drawing a closed sequence of lines.");
	Tamgugroup::AddMethod(global, "endloop", &Tamgugroup::MethodEndLoop, P_NONE, "endloop(): End drawing a closed sequence of lines.");
	Tamgugroup::AddMethod(global, "beginpolygon", &Tamgugroup::MethodBeginPolygon, P_NONE, "beginpolygon(): Start drawing a convex filled polygon.");
	Tamgugroup::AddMethod(global, "endpolygon", &Tamgugroup::MethodEndPolygon, P_NONE, "endpolygon(): End drawing a convex filled polygon.");
	Tamgugroup::AddMethod(global, "begincomplexpolygon", &Tamgugroup::MethodBeginComplexPolygon, P_NONE, "begincomplexpolygon(): Start drawing a complex filled polygon.");
	Tamgugroup::AddMethod(global, "gap", &Tamgugroup::MethodGap, P_NONE, "gap(): Fill the gap while drawing a complex polygon.");
	Tamgugroup::AddMethod(global, "endcomplexpolygon", &Tamgugroup::MethodEndComplexPolygon, P_NONE, "endcomplexpolygon(): End drawing a complex filled polygon.");
	Tamgugroup::AddMethod(global, "arc", &Tamgugroup::MethodArc, P_FIVE | P_SIX, "arc(int x, int y, int x1 , int y1 , float a1 , float a2): Draw an arc.\rarc(float xfloat yfloat radfloat a1float a2): Add a series of points to the current path on the arc of a circle;");
	Tamgugroup::AddMethod(global, "curve", &Tamgugroup::MethodCurve, P_EIGHT, "curve(float x, float y, float x1, float y1, float x1, float y2, float x3, float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at xy and x3y3.");
	Tamgugroup::AddMethod(global, "pushclip", &Tamgugroup::MethodPushClip, P_FOUR, "pushclip(int x, int y, int w , int h): Insert a clip region with the following coordinates");
	Tamgugroup::AddMethod(global, "popclip", &Tamgugroup::MethodPopClip, P_NONE, "popclip(): Release a clip region");
	Tamgugroup::AddMethod(global, "pie", &Tamgugroup::MethodPie, P_SIX, "pie(int x, int y, int x1 , int y1 , float a1 , float a2): Draw a pie");
	Tamgugroup::AddMethod(global, "point", &Tamgugroup::MethodPoint, P_FOUR, "po, int(int x, int y): Draw a pixel");
	Tamgugroup::AddMethod(global, "circle", &Tamgugroup::MethodCircle, P_THREE | P_FOUR, "circle(int x, int y, int r, int color): Draw a circle. 'color' is optional.");
	Tamgugroup::AddMethod(global, "textsize", &Tamgugroup::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
	Tamgugroup::AddMethod(global, "drawtext", &Tamgugroup::MethodDrawText, P_THREE, "drawtext(string l, int x, int y): Put a text at position xy");
	Tamgugroup::AddMethod(global, "redraw", &Tamgugroup::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamgugroup::AddMethod(global, "rgbcolor", &Tamgugroup::MethodRGBColor, P_ONE | P_THREE, "rgbcolor(string|int color)|(int r, int g, int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined");
	Tamgugroup::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE, "redraw(): Redraw the window");
	Tamgugroup::AddMethod(global, "backgroundcolor", &Tamguwidget::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamgugroup::AddMethod(global, "created", &Tamguwidget::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamgugroup::AddMethod(global, "align", &Tamguwidget::MethodAlign, P_ONE, "align(int): define the label alignment");




	global->newInstance[Tamgugroup::idtype] = new Tamgugroup(global);
	global->RecordCompatibilities(Tamgugroup::idtype);

	return true;
}


Tamgu* Tamgugroup::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgugroup::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

TamguIteration* Tamgugroup:: Newiteration(bool direction) {
	return aITERNULL;
}

Tamgu* Tamgugroup::MethodBegin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {	
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	Tamgu* x1 = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* x2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* y2 = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* text = callfunc->Evaluate(4, contextualpattern, idthread);
	if (group != NULL)
		return Returningerror("WND(101): group already created", idthread);
	string thetxt = text->String();
	if (thetxt.size() < 1024)
		strcpy_s(txt, 1024, thetxt.c_str());
	{
		Locking _lock(this);
		group = new Tamgu_Group(this, x1->Integer(), y1->Integer(), x2->Integer(), y2->Integer(), txt);
	}
	
	SaveItem(this);	
	return aTRUE;
}

Tamgu* Tamgugroup::MethodEnd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (group == NULL)
		return Returningerror("WND(101): group not created", idthread);
	group->end();
	end = true;	
	return aTRUE;
}

Tamgu* Tamgugroup::MethodActivate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (group == NULL)
		return Returningerror("WND(101): group not created", idthread);
	Fl_Widget* tabs = group->parent();
	if (tabs == NULL)
		return Returningerror("WND(507): This group does not belong to a tab structure", idthread);
	((Fl_Tabs*)tabs)->value(group);
	return aTRUE;
}






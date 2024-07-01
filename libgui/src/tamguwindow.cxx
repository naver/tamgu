/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwindow.cxx
 Date       : 2017/09/01
 Purpose    : window implementation (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguwindow.h"
#include "tamguwidget.h"
#include "atoms.h"
#include "vectors.h"
#include "tamgubitmap.h"
#include "tamguimage.h"
#include "instructions.h"

#ifdef FLTK14
#include "Fl/Fl_Sys_Menu_Bar.H"
#endif

#ifdef WIN32
void FlResetTimer();
#endif


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
//-----------------------------------------------------------------------------------

bool Ready();
//-------------------------------------------------------------------
//This index is used to detect the redraw method
short redraw_index = 0;
//-------------------------------------------------------------------
static vector<Tamguwindow*> current_window;
static ThreadLock windowLock;
static Tamguwindow* mainwindow = NULL;
static list<FLCallback> time_data;

FLCallback& Timedata(Tamgu* func, Tamgu* o, Tamguwindow* win) {
    FLCallback t;
    time_data.push_back(t);
    time_data.back().Set(func, o, win);
    return time_data.back();
}

void Cleartimedata() {
    while (!time_data.front().used)
        time_data.pop_front();
}

void Clearwindows() {
	Locking _lock(windowLock);
	mainwindow = NULL;
}

void TerminateMainWindow() {
	Locking _lock(windowLock);
	if (mainwindow != NULL) {
		if (mainwindow->window != NULL) {
			delete mainwindow->window;
		}
		mainwindow = NULL;
	}		
}

//-------------------------------------------------------------------
static void menu_callback(Fl_Widget *w, void *data) {
	FLCallback* wn = (FLCallback*)data;
	Tamguwindow* kwnd = wn->window;
	TamguCallFunction kfunc(wn->function);

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.arguments.push_back(kwnd);
	kfunc.arguments.push_back(wn->object);
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	kwnd->Resetreference();
	wn->object->Resetreference();
}

static void close_callback(Fl_Widget *w, void *data) {
	Locking _lock(windowLock);
	Tamguwindow* kwnd = (Tamguwindow*)data;
    if (kwnd->window!=NULL) {
		delete kwnd->window;
        kwnd->window = NULL;
    }
}

static void window_callback(Fl_Widget *w, void *data) {
	Locking _lock(windowLock);
	FLCallback* wn = (FLCallback*)data;
	Tamguwindow* kwnd = wn->window;
	TamguCallFunction kfunc(wn->function);

	kwnd->Setreference();
	wn->object->Setreference();
	kfunc.arguments.push_back(kwnd);
	kfunc.arguments.push_back(wn->object);
	Tamgu* ret = kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	kwnd->Resetreference();
	wn->object->Resetreference();

	if (ret->Boolean() == false)
		return;

	kwnd->closed = true;
	delete kwnd->window;
}

static void timeout_callback(void *data) {
	if (Stopall())
		return;

	FLCallback* wn = (FLCallback*)data;
	if (!wn->used || !globalTamgu->Checktracker(wn->window, wn->iwindow))
		return;

	Tamguwindow* kwnd = wn->window;
	if (!Ready()) {
		Fl::repeat_timeout(wn->timevalue, timeout_callback, data);
		return;
	}

	Cleartimedata();

	if (Stopall())
		return;

	wn->window->window->redraw();
	Fl::repeat_timeout(wn->timevalue, timeout_callback, data);
	return;
}

void SaveItem(Tamgu* e) {
	Locking _lock(windowLock);
	if (current_window.size() != 0) {
		Tamguwindow* w = current_window.back();
		long id = e->idtracker;
		w->items.push_back(id);
	}
}

//-------------------------------------------------------------------
//We need to declare once again our local definitions.
basebin_hash<windowMethod>  Tamguwindow::methods;


basebin_hash<bool> Tamguwindow::nothreads;


short Tamguwindow::idtype = 0;
short Tamguwidget::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguwindow::AddMethod(TamguGlobal* global, string name, windowMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguwindow::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamguwindow::InitialisationModule(global,"");
}

bool Tamguwindow::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	
	
	nothreads.clear();
	
	Tamguwindow::idtype = global->Getid("window");
	Tamguwidget::idtype = global->Getid("widget");
	Initstopall(false);
	mainwindow = NULL;

	//We record a specific id for the redraw method (used everywhere, but whose behaviour depends on whether we are in a thread)
	redraw_index = globalTamgu->Getid("redraw");

	Tamguwindow::AddMethod(global, "_initial", &Tamguwidget::MethodInitial, P_ONE, "_initial(variable): Creation of a window with a call back object");
    Tamguwindow::AddMethod(global, "post", &Tamguwindow::MethodPost, P_ATLEASTONE, "post(function, ...): post a function execution within a thread.");
	Tamguwindow::AddMethod(global, "begin", &Tamguwindow::MethodBegin, P_THREE | P_FIVE, "begin(int x,int y,int w, int h,string title): Create a window and begin initialisation, w and h are optional");
    Tamguwindow::AddMethod(global, "end", &Tamguwindow::MethodEnd, P_NONE, "end(): end creation");
	Tamguwindow::AddMethod(global, "run", &Tamguwindow::MethodRun, P_NONE, "run(): Launch the GUI");
	Tamguwindow::AddMethod(global, "create", &Tamguwindow::MethodCreate, P_THREE | P_FIVE, "create(int x,int y,int w, int h,string title): Create a window without widgets w and h are optional");

	Tamguwindow::AddMethod(global, "onclose", &Tamguwindow::MethodOnClose, P_TWO, "onclose(function, object): Callback to call on close");
	Tamguwindow::AddMethod(global, "redrawing", &Tamguwindow::MethodRedrawing, P_ONE, "redrawing(double timelapse): Set a time out to redraw the window every 'timelapase'");
	Tamguwindow::AddMethod(global, "ontime", &Tamguwindow::MethodOntime, P_THREE, "ontime(function,double timelapse, object): Set a time out function");
	Tamguwindow::AddMethod(global, "initializefonts", &Tamguwindow::MethodSetFonts, P_NONE, "initializefonts(): load fonts from system. Return the number of available fonts");
	Tamguwindow::AddMethod(global, "getfont", &Tamguwindow::MethodGetFont, P_ONE, "getfont(int num): get font name.");
	Tamguwindow::AddMethod(global, "getfontsizes", &Tamguwindow::MethodGetFontSizes, P_ONE, "getfontsizes(int num): return a vector of available font sizes.");
	Tamguwindow::AddMethod(global, "fontnumber", &Tamguwindow::MethodNbFonts, P_NONE, "fontnumber(): Return the number of available fonts.");
	Tamguwindow::AddMethod(global, "plot", &Tamguwindow::MethodPlot, P_TWO | P_THREE, "plot(fvector xy,int thickness,fvector landmarks): Plot a graph from a table of successive xy points according to window size. If thickness===0 then points are continuously plotted else defines the diameter of the point. Return a float vector which is used with plotcoords. The landmark vector is optional it is has the following structure: [XmaxWindow,YmaxWindow,XminValue,YminValue,XmaxValue,YmaxValue,incX,incY]. incX,incY are also optional.");
	Tamguwindow::AddMethod(global, "plotcoords", &Tamguwindow::PlotCoords, P_THREE, "plotcoords(fvector inits,float x,float y): Compute the coordinates of a point(xy) according to the previous scale computed with plot. Returns a vector of two elements [xsys] corresponding to the screen coordinates in the current window.");
	Tamguwindow::AddMethod(global, "ask", &Tamguwindow::MethodAsk, P_THREE | P_FOUR | P_FIVE, "ask(string msg,string buttonmsg1,string buttonmsg2,string buttonmsg3): Pop up window to pose a question");
	Tamguwindow::AddMethod(global, "alert", &Tamguwindow::MethodAlert, P_ONE, "alert(string msg): Pop up window to display an alert");
	Tamguwindow::AddMethod(global, "password", &Tamguwindow::MethodPassword, P_ONE, "password(string msg): Pop up window to input a password");
	Tamguwindow::AddMethod(global, "get", &Tamguwindow::MethodInput, P_NONE | P_ONE, "get(string msg): Pop up window to get a value from the user");
	Tamguwindow::AddMethod(global, "menu", &Tamguwindow::MethodMenu, P_FIVE, "menu(vector,int x,int y,int w, int h): initialize a menu with its callback functions");
	Tamguwindow::AddMethod(global, "resizable", &Tamguwindow::MethodResizable, P_ONE | P_NONE, "resizable(object): make the object resizable");
	Tamguwindow::AddMethod(global, "sizerange", &Tamguwindow::MethodSizerange, P_FOUR, "sizerange(int minw,int minh, int maxw,int maxh): define range in which the size of the window can evolve");
	Tamguwindow::AddMethod(global, "scrollbar", &Tamguwindow::MethodScrollbar, P_SIX, "scrollbar(int minw,int minh, int maxw,int maxh,int maxcanvasw, int maxcanvash): Insert scroll bars");
	Tamguwindow::AddMethod(global, "modal", &Tamguwindow::MethodModal, P_ONE | P_NONE, "modal(bool b): If true make the window modal. If no parameter return if the window is modal");
	Tamguwindow::AddMethod(global, "border", &Tamguwindow::MethodBorder, P_ONE | P_NONE, "border(bool b): If true add or remove borders. If no parameter return if the window has borders");
	Tamguwindow::AddMethod(global, "backgroundcolor", &Tamguwindow::MethodBackgroundColor, P_ONE | P_NONE, "backgroundcolor(int c|string c): set the background color");
	Tamguwindow::AddMethod(global, "drawcolor", &Tamguwidget::MethodDrawColor, P_ONE, "drawcolor(int c|string c): set the color for the next drawings");
	Tamguwindow::AddMethod(global, "rectangle", &Tamguwidget::MethodRectangle, P_FOUR | P_FIVE, "rectangle(int x, int y, int w , int h , string c|int c): Draw a rectangle with optional color c");
	Tamguwindow::AddMethod(global, "vertex", &Tamguwidget::MethodVertex, P_TWO, "vertex(float x, float y): Add a vertex to a complex structure");
	Tamguwindow::AddMethod(global, "rectanglefill", &Tamguwidget::MethodRectangleFill, P_FOUR | P_FIVE, "rectanglefill(int x, int y, int w , int h , string c|int c): Fill a rectangle with optional color c");
	Tamguwindow::AddMethod(global, "lineshape", &Tamguwidget::MethodLineShape, P_TWO, "lineshape(string type, int width): Select the line shape and its thikness");
	Tamguwindow::AddMethod(global, "polygon", &Tamguwidget::MethodPolygon, P_SIX | P_EIGHT, "polygon(int x, int y, int x1, int y1,int x2, int y2, int x3, int y3): Draw a polygon x3 and y3 are optional");
	Tamguwindow::AddMethod(global, "loop", &Tamguwidget::MethodLoop, P_SIX | P_EIGHT, "loop(int x, int y, int x1, int y1,int x2, int y2, int x3, int y3): Draw a series of lines x3 and y3 are optional");
	Tamguwindow::AddMethod(global, "line", &Tamguwidget::MethodLine, P_FOUR | P_SIX, "line(int x, int y, int x1, int y1,int x2, int y2): Draw a line between points x2 and y2 are optional");
	Tamguwindow::AddMethod(global, "rotation", &Tamguwidget::MethodLineRotation, P_FOUR | P_FIVE, "rotation(float x, float y, float distance , float angle , bool draw): Compute the coordinate of a rotated point from point xy using a distance and an angle. Return a vector of floats with the new coordinates.");
	Tamguwindow::AddMethod(global, "pushmatrix", &Tamguwidget::MethodPushMatrix, P_NONE, "pushmatrix(): Save the current transformation");
	Tamguwindow::AddMethod(global, "popmatrix", &Tamguwidget::MethodPopMatrix, P_NONE, "popmatrix(): Restore the current transformation");
	Tamguwindow::AddMethod(global, "scale", &Tamguwidget::MethodScale, P_TWO | P_ONE, "scale(float x, float y)|scale(float x): Scale the current transformation");
	Tamguwindow::AddMethod(global, "translate", &Tamguwidget::MethodTranslate, P_TWO, "translate(float x, float y): translate the current transformation");
	Tamguwindow::AddMethod(global, "rotate", &Tamguwidget::MethodRotate, P_ONE, "rotate(float d): rotate of degree d the current transformation");
	Tamguwindow::AddMethod(global, "multmatrix", &Tamguwidget::MethodMultmatrix, P_SIX, "multmatrix(float a , float b , float c , float d , float x , float y): combine transformations");
	Tamguwindow::AddMethod(global, "transformx", &Tamguwidget::MethodTransform_x, P_TWO, "transformx(float x , float y): Transform a coordinate X using the current transformation matrix.");
	Tamguwindow::AddMethod(global, "transformy", &Tamguwidget::MethodTransform_y, P_TWO, "transformy(float x , float y): Transform a coordinate Y using the current transformation matrix.");
	Tamguwindow::AddMethod(global, "transformdx", &Tamguwidget::MethodTransform_dx, P_TWO, "transformdx(float x , float y): Transform a distance DX using the current transformation matrix.");
	Tamguwindow::AddMethod(global, "transformdy", &Tamguwidget::MethodTransform_dy, P_TWO, "transformdy(float x , float y): Transform a distance DY using the current transformation matrix.");
	Tamguwindow::AddMethod(global, "transformedvertex", &Tamguwidget::MethodTransform_vertex, P_TWO, "transformedvertex(float x , float y): add transformations to vertices list.");
	Tamguwindow::AddMethod(global, "beginpoints", &Tamguwidget::MethodBeginPoints, P_NONE, "beginpoints(): Start accumulating vertices");
	Tamguwindow::AddMethod(global, "endpoints", &Tamguwidget::MethodEndPoints, P_NONE, "endpoints(): Stop accumulating vertices");
	Tamguwindow::AddMethod(global, "beginline", &Tamguwidget::MethodBeginLine, P_NONE, "beginline(): Start drawing lines.");
	Tamguwindow::AddMethod(global, "endline", &Tamguwidget::MethodEndLine, P_NONE, "endline(): End drawing lines.");
	Tamguwindow::AddMethod(global, "beginloop", &Tamguwidget::MethodBeginLoop, P_NONE, "beginloop(): Start drawing a closed sequence of lines.");
	Tamguwindow::AddMethod(global, "endloop", &Tamguwidget::MethodEndLoop, P_NONE, "endloop(): End drawing a closed sequence of lines.");
	Tamguwindow::AddMethod(global, "beginpolygon", &Tamguwidget::MethodBeginPolygon, P_NONE, "beginpolygon(): Start drawing a convex filled polygon.");
	Tamguwindow::AddMethod(global, "endpolygon", &Tamguwidget::MethodEndPolygon, P_NONE, "endpolygon(): End drawing a convex filled polygon.");
	Tamguwindow::AddMethod(global, "begincomplexpolygon", &Tamguwidget::MethodBeginComplexPolygon, P_NONE, "begincomplexpolygon(): Start drawing a complex filled polygon.");
	Tamguwindow::AddMethod(global, "gap", &Tamguwidget::MethodGap, P_NONE, "gap(): Fill the gap while drawing a complex polygon.");
	Tamguwindow::AddMethod(global, "endcomplexpolygon", &Tamguwidget::MethodEndComplexPolygon, P_NONE, "endcomplexpolygon(): End drawing a complex filled polygon.");
	Tamguwindow::AddMethod(global, "arc", &Tamguwidget::MethodArc, P_FIVE | P_SIX, "arc(int x, int y, int x1, int y1, float a1 , float a2): Draw an arc.\rarc(float xfloat yfloat radfloat a1float a2): Add a series of points to the current path on the arc of a circle;");
	Tamguwindow::AddMethod(global, "curve", &Tamguwidget::MethodCurve, P_EIGHT, "curve(float x, float y, float x1, float y1, float x1, float y2, float x3, float y3): Add a series of points on a Bezier curve to the path. The curve ends (and two of the points) are at xy and x3y3.");
	Tamguwindow::AddMethod(global, "pushclip", &Tamguwidget::MethodPushClip, P_FOUR, "pushclip(int x, int y, int w , int h): Insert a clip region with the following coordinates");
	Tamguwindow::AddMethod(global, "popclip", &Tamguwidget::MethodPopClip, P_NONE, "popclip(): Release a clip region");
	Tamguwindow::AddMethod(global, "pie", &Tamguwidget::MethodPie, P_SIX, "pie(int x, int y, int x1, int y1, float a1 , float a2): Draw a pie");
	Tamguwindow::AddMethod(global, "point", &Tamguwidget::MethodPoint, P_TWO, "point(int x, int y): Draw a pixel");
	Tamguwindow::AddMethod(global, "circle", &Tamguwidget::MethodCircle, P_THREE | P_FOUR, "circle(int x, int y, int r, int color): Draw a circle. 'color' is optional.");
	Tamguwindow::AddMethod(global, "textsize", &Tamguwidget::MethodTextMeasure, P_ONE, "textsize(string l): Return a map with w and h as key to denote width and height of the string in pixels");
	Tamguwindow::AddMethod(global, "drawtext", &Tamguwidget::MethodDrawText, P_THREE, "drawtext(string l,int x,int y): Put a text at position xy");
	Tamguwindow::AddMethod(global, "redraw", &Tamguwidget::MethodRedraw, P_NONE | P_ONE, "redraw(): Redraw the window");
	Tamguwindow::AddMethod(global, "rgbcolor", &Tamguwindow::MethodRGBColor, P_ONE | P_THREE, "rgbcolor(string|int color)|(int r, int g, int b)|(vector color): either return a vector of the color decomposition or return the value of rgb values combined");
	Tamguwindow::AddMethod(global, "font", &Tamguwindow::MethodTextFont, P_TWO, "font(string f,int sz): Set the font name and its size");
	Tamguwindow::AddMethod(global, "ondragdrop", &Tamguwindow::MethodOnDragDrop, P_TWO, "ondragdrop(function, object): Set the call back function for drag and drop action.");
	Tamguwindow::AddMethod(global, "onmouse", &Tamguwindow::MethodOnMouse, P_THREE, "onmouse(int action function, object): Set the call back function on a mouse action with a given object as parameter");
	Tamguwindow::AddMethod(global, "onkey", &Tamguwindow::MethodOnKey, P_THREE, "onkey(int action function, object): Set the call back function on a keyboard action with a given object as parameter");
	Tamguwindow::AddMethod(global, "cursorstyle", &Tamguwindow::MethodCursorStyle, P_THREE, "cursorstyle(int cursortype , int color, int color): Set the cursor shape");
	Tamguwindow::AddMethod(global, "hide", &Tamguwindow::MethodWindowHide, P_ONE, "hide(bool h): Hide the window if h is true");
	Tamguwindow::AddMethod(global, "show", &Tamguwindow::MethodShow, P_NONE, "show(): Show the window");
	Tamguwindow::AddMethod(global, "focus", &Tamguwindow::MethodFocus, P_NONE, "focus(): Get the focus");
	Tamguwindow::AddMethod(global, "flush", &Tamguwindow::MethodFlush, P_NONE, "flush(): force redrawing of window");
	Tamguwindow::AddMethod(global, "position", &Tamguwindow::MethodPosition, P_NONE | P_TWO, "position()|(int x, int y): Return the window position or set the window position");
	Tamguwindow::AddMethod(global, "size", &Tamguwindow::MethodSize, P_NONE | P_FOUR, "size()|(int x, int y, int w , int h): Return the window size or set the window size");
	Tamguwindow::AddMethod(global, "image", &Tamguwindow::MethodImage, P_FIVE, "image(image image, int x , int y , int w , int h): Display an image");
	Tamguwindow::AddMethod(global, "bitmap", &Tamguwindow::MethodBitmap, P_FOUR | P_SIX, "bitmap(bitmap image, int color, int x , int y , int w , int h): Display a bitmap");
	Tamguwindow::AddMethod(global, "labelsize", &Tamguwindow::MethodLabelSize, P_ONE | P_NONE, "labelsize(int c): set or return the label font size");
	Tamguwindow::AddMethod(global, "labelfont", &Tamguwindow::MethodLabelFont, P_ONE | P_NONE, "labelfont(int c): set or return the label font");
	Tamguwindow::AddMethod(global, "labelcolor", &Tamguwindow::MethodLabelColor, P_ONE | P_NONE, "labelcolor(int c): set or return the label color");
	Tamguwindow::AddMethod(global, "labeltype", &Tamguwindow::MethodLabelType, P_ONE | P_NONE, "labeltype(int c): set or return the label type");
	Tamguwindow::AddMethod(global, "label", &Tamguwindow::MethodLabel, P_ONE | P_NONE, "label(string s): set or return the label text");
	Tamguwindow::AddMethod(global, "selectioncolor", &Tamguwindow::MethodSelectionColor, P_ONE | P_NONE, "selectioncolor(int color): Color for the selected elements");
	Tamguwindow::AddMethod(global, "boundaries", &Tamguwindow::MethodCoordinates, P_NONE | P_ONE | P_FOUR, "boundaries(ivector crds): return a vector of the widget coordinates");
	Tamguwindow::AddMethod(global, "created", &Tamguwindow::MethodCreated, P_NONE, "created(): Return true if the object has been created");
	Tamguwindow::AddMethod(global, "copy", &Tamguwindow::MethodCopy, P_ONE, "copy(string): Copy string to clipboard");
	Tamguwindow::AddMethod(global, "paste", &Tamguwindow::MethodPaste, P_NONE, "paste(): paste string");
	Tamguwindow::AddMethod(global, "align", &Tamguwindow::MethodAlign, P_ONE, "align(int): define the label alignment");
    Tamguwindow::AddMethod(global, "close", &Tamguwindow::MethodClose, P_NONE, "close(): close window");


	nothreads[global->Getid("getfont")] = true; //tamguwindow
	nothreads[global->Getid("getfontsizes")] = true; //tamguwindow
	nothreads[global->Getid("textsize")] = true; //tamguwindow
	nothreads[global->Getid("rgbcolor")] = true; //tamguwindow

	global->newInstance[Tamguwindow::idtype] = new Tamguwindow(global);
	global->RecordCompatibilities(Tamguwindow::idtype);

	return true;
}

void TamguCanvas::draw() {	
	if (function != NULL) {
		fl_color(FL_BLACK); //we set FL_BLACK as the default color, it can be modified with drawcolor in the code...
		TamguCallFunction call(function);
		call.arguments.push_back(window);
		call.arguments.push_back(object);
		Tamgu* a = call.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
		a->Release();		
		window->scroll->scrollbar.redraw();
		window->scroll->hscrollbar.redraw();
	}
}

Tamguwindow::Tamguwindow(TamguGlobal* g, Tamgu* parent) : Tamguwidget(g, parent) {
	//Do not forget your variable initialisation
	window = NULL;
	scrollcanvas = NULL;
	scroll = NULL;
	end = false;
	flmenu = NULL;
	flmenunbelements = 0;
	onclose = NULL;
	timeout = NULL;
	closed = false;
	Setstopall(false);
}

Tamguwindow::~Tamguwindow() {
    Locking _lock(windowLock);
	itemclose();
    if (window != NULL) {
        if (!end)
            window->end();
		delete window;
        window = NULL;
    }
}


void Tamguwindow::itemclose() {
    if (mainwindow == this)
        Setstopall(true);

	if (flmenunbelements) {
		for (int i = 0; i < flmenunbelements; i++) {
			if (flmenu[i].text != NULL) {
				free((char*)flmenu[i].text);
				delete (FLCallback*)flmenu[i].user_data();
			}
		}
		delete[] flmenu;
		flmenunbelements = 0;
	}

	Tamgu* ke;
	for (int i = 0; i < items.size(); i++) {
		ke = globalTamgu->GetFromTracker(items[i]);
		if (ke != NULL)
			((Tamguwidget*)ke)->itemclose();
	}
	
	items.clear();
	
	if (onclose != NULL)
		delete onclose;
	
    if (timeout != NULL) {
		timeout->Reset();
#ifdef WIN32
        FlResetTimer();
#else
        Fl::remove_timeout(NULL, NULL);
#endif

        timeout = NULL;
    }
	
	if (scrollcanvas != NULL)
		delete scrollcanvas;
	
	if (scroll != NULL)
		delete scroll;

	scroll = NULL;
	scrollcanvas = NULL;
	onclose = NULL;
	function = NULL;
}


Tamgu* Tamguwindow::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamguwindow::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

Tamgu* Tamguwindow::MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (window == NULL)
        return aFALSE;
    itemclose();
    delete window;
    window = NULL;
    return aTRUE;
}

Tamgu* Tamguwindow::MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (window != NULL)
		return globalTamgu->Returnerror("Cannot recreate an existing window", idthread);

	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, aNULL, idthread);
	Tamgu* y = callfunc->Evaluate(1, aNULL, idthread);
	if (callfunc->Size() == 3) {
		label = callfunc->Evaluate(2, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), x->Integer() + 100, y->Integer() + 100, txt, function, object, contextualpattern, this);
		window->size_range(x->Integer(), y->Integer());
	}
	else {
		Tamgu* wx = callfunc->Evaluate(2, aNULL, idthread);
		Tamgu* hy = callfunc->Evaluate(3, aNULL, idthread);
		label = callfunc->Evaluate(4, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt, function, object, contextualpattern, this);
	}

	current_window.push_back(this);
	end = true;
	window->callback(close_callback, this);
	window->end();
#ifdef FLTK14
    Fl_Sys_Menu_Bar::window_menu_style(Fl_Sys_Menu_Bar::no_window_menu);
#endif

	window->show();
	return aTRUE;
}

Tamgu* Tamguwindow::MethodRun(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return globalTamgu->Returnerror("No window available", idthread);

	if (end == false) {
		end = true;
		window->end();
		window->show();
	}
	
	if (mainwindow == NULL)
		mainwindow = this;

	if (timeout != NULL) {
#ifdef WIN32
		FlResetTimer();
#endif
		Fl::add_timeout(timeout->timevalue, timeout_callback, (void*)timeout);
	}

	Fltkrun(idthread);
	mainwindow = NULL;
	return aTRUE;
}

Tamgu* Tamguwindow::MethodEnd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return globalTamgu->Returnerror("No window available", idthread);
	if (end)
		return globalTamgu->Returnerror("Window already instantiated", idthread);

	end = true;

	window->end();
	if (scrollcanvas != NULL)
		window->resizable(scrollcanvas);

#ifdef FLTK14
    Fl_Sys_Menu_Bar::window_menu_style(Fl_Sys_Menu_Bar::no_window_menu);
#endif

	window->show();
	current_window.pop_back();

	return aTRUE;
}

Tamgu* Tamguwindow::MethodBegin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window != NULL)
		return globalTamgu->Returnerror("Cannot recreate an existing window", idthread);

	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, aNULL, idthread);
	Tamgu* y = callfunc->Evaluate(1, aNULL, idthread);

	if (callfunc->Size() == 3) {
		label = callfunc->Evaluate(2, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), txt, function, object, contextualpattern, this);
		window->size_range(x->Integer(), y->Integer());
	}
	else {
		Tamgu* wx = callfunc->Evaluate(2, aNULL, idthread);
		Tamgu* hy = callfunc->Evaluate(3, aNULL, idthread);
		label = callfunc->Evaluate(4, aNULL, idthread)->String();
		strcpy_s(txt, 1024, label.c_str());
		window = new Doublewindow(x->Integer(), y->Integer(), wx->Integer(), hy->Integer(), txt, function, object, contextualpattern, this);
	}
	current_window.push_back(this);
	window->callback(close_callback, this);
	return aTRUE;

}

Tamgu* Tamguwindow::MethodBitmap(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	Tamgu* kbitmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kbitmap->Type() != Tamgubitmap::idtype)
		return Returningerror("WND(169): Unknown bitmap", idthread);
	Tamgu* ke = callfunc->Evaluate(1, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	int x = callfunc->Evaluate(2, contextualpattern, idthread)->Integer();
	int y = callfunc->Evaluate(3, contextualpattern, idthread)->Integer();
	int w = ((Tamgubitmap*)kbitmap)->szw;
	int h = ((Tamgubitmap*)kbitmap)->szh;
	if (callfunc->Size() == 6) {
		w = callfunc->Evaluate(4, contextualpattern, idthread)->Integer();
		h = callfunc->Evaluate(5, contextualpattern, idthread)->Integer();
	}
	Fl_Bitmap* fl = ((Tamgubitmap*)kbitmap)->bitmap;
	if (fl == NULL)
		return Returningerror("WND(169): Unknown bitmap", idthread);
	if (!end) {
		Fl_Box* box = new Fl_Box(x, y, w, h);
		box->labelcolor(color);
		box->image(fl);
		return aTRUE;
	}
	fl_color(color);
	fl->draw(x, y, w, h);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodImage(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	Tamgu* kimage = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kimage->Type() != Tamguimage::idtype)
		return Returningerror("WND(167): Unknown image", idthread);
	Tamgu* kx = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* ky = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* kw = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* kh = callfunc->Evaluate(4, contextualpattern, idthread);
	Fl_Image* fl = ((Tamguimage*)kimage)->image;
	if (fl == NULL)
		return Returningerror("WND(168): Unloaded image", idthread);
	if (!end) {
		Fl_Box* box = new Fl_Box(kx->Integer(), ky->Integer(), kw->Integer(), kh->Integer());
		box->image(fl);
		return aTRUE;
	}
	fl->draw(kx->Integer(), ky->Integer(), kw->Integer(), kh->Integer());
	return aTRUE;
}


Tamgu* Tamguwindow::MethodAwake(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	Fl::awake();
	return aTRUE;
}

Tamgu* Tamguwindow::MethodLock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	Fl::lock();
	return aTRUE;
}

Tamgu* Tamguwindow::MethodUnlock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//In our example, we have only two parameters
	Fl::unlock();
	return aTRUE;
}

Tamgu* Tamguwindow::MethodSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	//In our example, we have only two parameters
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (callfunc->Size() == 0) {
        Tamguivector* kvect = globalTamgu->Provideivector();
		kvect->values.push_back(window->x());
		kvect->values.push_back(window->y());
		kvect->values.push_back(window->w());
		kvect->values.push_back(window->h());
		return kvect;
	}
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(3, contextualpattern, idthread);
	window->resize(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	return aTRUE;
}

Tamgu* Tamguwindow::MethodPosition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	//In our example, we have only two parameters
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (callfunc->Size() == 0) {
        Tamguivector* kvect = globalTamgu->Provideivector();
		kvect->values.push_back(window->x_root());
		kvect->values.push_back(window->y_root());
		return kvect;
	}
	//0 is the first parameter and so on...
	Tamgu* x = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(1, contextualpattern, idthread);
	window->position(x->Integer(), y->Integer());
	return aTRUE;
}

Tamgu* Tamguwindow::MethodFlush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//If we are in the main thread, well business as usual
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	window->flush();
	return aTRUE;
}

Tamgu* Tamguwindow::MethodWindowHide(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	//0 is the first parameter and so on...
	bool kaction = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (kaction == true) {
		if (window->shown())
			window->hide();
	}
	else
	if (!window->shown())
		window->show();
	return aTRUE;
}


Tamgu* Tamguwindow::MethodOnKey(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (end)
		return Returningerror("WND(102): Window already instantiated", idthread);
	//0 is the first parameter and so on...
	Tamgu* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* function = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Eval(aNULL, aNULL, idthread);

	int act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (actions.find(action) == actions.end())
			return Returningerror("WND(344): Unknown keyboard action", idthread);
		act = actions[action];
	}
	else
		act = kaction->Integer();
	if (window->Setkeyaction(act, function, object) == false)
		return Returningerror("WND(343): Wrong parameter in your keyboard callback", idthread);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodCursorStyle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	//0 is the first parameter and so on...
	Tamgu* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* kcolor1 = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* kcolor2 = callfunc->Evaluate(2, contextualpattern, idthread);
	Fl_Cursor act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (cursors.find(action) == cursors.end())
			return Returningerror("WND(344): Unknown keyboard action", idthread);
		act = (Fl_Cursor)actions[action];
	}
	else
		act = (Fl_Cursor)kaction->Integer();
	window->cursor(act, kcolor1->Integer(), kcolor2->Integer());
	return aTRUE;
}

Tamgu* Tamguwindow::MethodOnDragDrop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	//0 is the first parameter and so on...
	Tamgu* function = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* object = callfunc->arguments[1];
	if (!object->isCallVariable())
		object = object->Eval(aNULL, aNULL, idthread);

	window->Setdragdrop(function, object);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodOnMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (end)
		return Returningerror("WND(102): Window already instantiated", idthread);
	//0 is the first parameter and so on...
	Tamgu* kaction = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* function = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Eval(aNULL, aNULL, idthread);

	int act;
	if (kaction->Type() == a_string) {
		string action = kaction->String();
		if (actions.find(action) == actions.end())
			return Returningerror("WND(341): Unknown mouse action", idthread);
		act = actions[action];
	}
	else
		act = kaction->Integer();
	if (window->Setmouseaction(act, function, object) == false)
		return Returningerror("WND(342): Wrong parameter in your mouse callback", idthread);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodMenu(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (end)
		return Returningerror("WND(102): Window already instantiated", idthread);
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	//we expect a vector
	if (ke->isVectorContainer() == false)
		return Returningerror("WND(109): Wrong parameter, vector expected", idthread);
	Tamgu* menu = ke;
	vector<Fl_Menu_Item*> elements;
	Fl_Menu_Item* fl;
	for (int i = 0; i < menu->Size(); i++) {
		//First level
		//each element is a vector
		ke = menu->getvalue(i);
		if (ke->isVectorContainer() == false)
			return Returningerror("WND(109): Wrong element, vector expected", idthread);
		Tamgu* element = ke;
		//the first element should be the name of our element
		if (element->Size() != 0) {
			ke = element->getvalue(0);
			string nn = ke->String();
			char* n = strdup(STR(nn));
			fl = new Fl_Menu_Item;
			fl->text = n;
			fl->shortcut_ = 0;
			fl->callback_ = NULL;
			fl->user_data_ = 0;
			fl->flags = FL_SUBMENU;
			fl->labeltype_ = 0;
			fl->labelfont_ = 0;
			fl->labelsize_ = 12;
			fl->labelcolor_ = FL_BLACK;
			elements.push_back(fl);
			//then we analyse the sub-elements, the next element should be a vector
			for (int i = 1; i < element->Size(); i++) {
				if (element->getvalue(i)->isVectorContainer() == false)
					return Returningerror("WND(109): Wrong element, vector expected", idthread);
				Tamgu* v = element->getvalue(i);
				//First element should be the name
				if (v->Size() != 5)
					return Returningerror("WND(109): Wrong element, missing field in a sub-menu", idthread);
				nn = v->getvalue(0)->String();
				n = strdup(STR(nn));
				//second element shoudl be a vector
				if (v->getvalue(1)->isVectorContainer() == false)
					return Returningerror("WND(109): Wrong element, second field in a sub-menu should be a vector", idthread);
				long code = 0;
				Tamgu* vv = v->getvalue(1);
				for (int j = 0; j < vv->Size(); j++) {
					Tamgu* kcom = vv->getvalue(j);
					if (kcom->Type() == a_string) {
						string scom = kcom->String();
						if (commands.find(scom) != commands.end())
							code += commands[scom];
						else
							code += scom[0];
					}
					else
						code += kcom->Integer();
				}
				Tamgu* function = v->getvalue(2);
				if (!function->isFunction())
					return Returningerror("WND(109): Wrong element, third field in a sub-menu should be a function", idthread);
				Tamgu* object = v->getvalue(3);
				if (!object->isCallVariable())
					object = object->Eval(aNULL, aNULL, idthread);

				bool sep = v->getvalue(4)->Boolean();
				fl = new Fl_Menu_Item;
				fl->labeltype_ = 0;
				fl->labelfont_ = 0;
				fl->labelsize_ = 12;
				fl->labelcolor_ = FL_BLACK;
				fl->text = n;
				fl->shortcut_ = code;
				fl->callback_ = menu_callback;
				fl->user_data_ = new FLCallback(function, object, this);
				fl->flags = 0;
				if (sep == true)
					fl->flags = FL_MENU_DIVIDER;
				elements.push_back(fl);
			}
		}
		elements.push_back(NULL);
	}
	elements.push_back(NULL);
	flmenunbelements = elements.size();
	flmenu = new Fl_Menu_Item[flmenunbelements];
	int i;
	for (i = 0; i < flmenunbelements; i++) {
		if (elements[i] != NULL)
			flmenu[i] = *elements[i];
		else {
			flmenu[i].text = 0;
			flmenu[i].shortcut_ = 0;
			flmenu[i].callback_ = NULL;
			flmenu[i].user_data_ = 0;
			flmenu[i].flags = 0;
			flmenu[i].labeltype_ = 0;
			flmenu[i].labelfont_ = 0;
			flmenu[i].labelsize_ = 12;
			flmenu[i].labelcolor_ = FL_BLACK;
		}
		delete elements[i];
	}
	Tamgu* x = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* y = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* wx = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* hy = callfunc->Evaluate(4, contextualpattern, idthread);
	Fl_Menu_Bar *m = new Fl_Menu_Bar(x->Integer(), y->Integer(), wx->Integer(), hy->Integer());
	m->copy(flmenu);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodResizable(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (end == false)
		return Returningerror("WND(123): Operation not available", idthread);
	if (callfunc->Size() == 0) {
		window->resizable(NULL);
		return aTRUE;
	}
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	if (ke->Yourowntype() == Tamguwidget::idtype) {
		Fl_Widget* fl = ((Tamguwidget*)ke)->Widget();
		if (fl == NULL)
			return Returningerror("WND(134): Widget has not been created yet", idthread);
		window->resizable(fl);
		return aTRUE;
	}
	return Returningerror("WND(133): Widget expected as parameter", idthread);
}

Tamgu* Tamguwindow::MethodSizerange(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (end)
		return Returningerror("WND(102): Window already instantiated", idthread);
	Tamgu* minw = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* minh = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* maxw = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* maxh = callfunc->Evaluate(3, contextualpattern, idthread);
	window->size_range(minw->Integer(), minh->Integer(), maxw->Integer(), maxh->Integer());
	return aTRUE;
}

Tamgu* Tamguwindow::MethodScrollbar(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (end)
		return Returningerror("WND(102): Window already instantiated", idthread);
	if (scrollcanvas != NULL)
		return Returningerror("WND(108): Scrollbar already instantiated", idthread);

	Tamgu* minw = callfunc->Evaluate(0, contextualpattern, idthread);
	Tamgu* minh = callfunc->Evaluate(1, contextualpattern, idthread);
	Tamgu* maxw = callfunc->Evaluate(2, contextualpattern, idthread);
	Tamgu* maxh = callfunc->Evaluate(3, contextualpattern, idthread);
	Tamgu* maxcw = callfunc->Evaluate(4, contextualpattern, idthread);
	Tamgu* maxch = callfunc->Evaluate(5, contextualpattern, idthread);
	scroll = new Fl_Scroll(minw->Integer(), minh->Integer(), maxw->Integer(), maxh->Integer());
	scrollcanvas = new TamguCanvas(maxw->Integer(), maxh->Integer(), minw->Integer(), minh->Integer(), maxcw->Integer(), maxch->Integer());
	scrollcanvas->window = this;
	scrollcanvas->function = function;
	scrollcanvas->object = object;
	function = NULL;
	scroll->end();	
	return aTRUE;
}

Tamgu* Tamguwindow::MethodModal(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (callfunc->Size() == 0) {
		if (window->modal() == 0)
			return aFALSE;
		return aTRUE;
	}
	bool m = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	if (m)
		window->set_modal();
	else
		window->set_non_modal();
	return aTRUE;
}

Tamgu* Tamguwindow::MethodBorder(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	if (end)
		return Returningerror("WND(102): Window already instantiated", idthread);
	if (callfunc->Size() == 0) {
		if (window->border() == 0)
			return aFALSE;
		return aTRUE;
	}
	bool m = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
	window->border(m);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodBackgroundColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

	Fl_Widget* window = Widget();
	if (window == NULL)
		return Returningerror("WND(805): Widget not initialized", idthread);
	if (callfunc->Size() == 0)
		return globalTamgu->Provideint(window->color());
	Tamgu* ke = callfunc->Evaluate(0, contextualpattern, idthread);
	Fl_Color color;
	if (ke->Type() == a_string) {
		//0 is the first parameter and so on...
		string buf = ke->String();
		if (colors.find(buf) == colors.end()) {
			buf = "WND(657): Unknown color code";
			return globalTamgu->Returnerror(buf, idthread);
		}
		color = colors[buf];
	}
	else
		color = ke->Integer();
	window->color(color);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodPlot(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	Tamgu* points = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!points->isVectorContainer())
		return Returningerror("WND(872): We expect a vector as first parameter.", idthread);
	long thickness = callfunc->Evaluate(1, contextualpattern, idthread)->Integer();
	double x, y, a = 0.0, b = 0.0;
	double maxX=0, maxY=0, minX=0, minY=0;
	double minx = 0;
	double miny = 0;
	double maxx = window->w();
	double maxy = window->h();
	double incx = 0.0;
	double incy = 0.0;
	char action = 0;
	Tamgu* klandmark = NULL;
	if (callfunc->Size() == 3) {
		klandmark = callfunc->Evaluate(2, contextualpattern, idthread);
		if (!klandmark->isVectorContainer())
			return Returningerror("WND(873): We expect a vector as third parameter", idthread);
		if (klandmark->Size() >= 4) {
			minx = klandmark->getfloat(0);
			miny = klandmark->getfloat(1);
			maxx = klandmark->getfloat(2);
			maxy = klandmark->getfloat(3);
			if (klandmark->Size() >= 8) {
				action = 1;
				minX = klandmark->getfloat(4);
				minY = klandmark->getfloat(5);
				maxX = klandmark->getfloat(6);
				maxY = klandmark->getfloat(7);
				if (klandmark->Size() >= 10) {
					action = 2;
					incx = klandmark->getfloat(8);
					incy = klandmark->getfloat(9);
				}
			}
		}
	}

	Tamgu* table = points;
	long sz = points->Size();
	Tamgu* kvect = Selectafvector(contextualpattern);

	long i;

	if (points->Type() == a_vector && points->getvalue(0)->isVectorContainer()) {
        Tamgufvector* fv = globalTamgu->Providefvector();
		Tamgu* a;
		for (i = 0; i < sz; i++) {
			a = points->getvalue(i);
			if (!a->isVectorContainer() || a->Size() != 2) {
				fv->Release();
				return Returningerror("WND(871): The vector should contain vectors of two elements.", idthread);
			}

			x = a->getfloat(0);
			y = a->getfloat(1);

			fv->values.push_back(x);
			fv->values.push_back(y);
			if (!action) {
				if (!i) {
					maxX = x;
					minX = x;
					minY = y;
					maxY = y;
				}
				else {
					maxX = max(x, maxX);
					minX =  min(x, minX);
					minY =  min(y, minY);
					maxY = max(y, maxY);
				}
			}
		}		
		
		if (maxX == minX || maxY == minY) {
			fv->Release();
			return kvect;
		}

		table = fv;
		sz = fv->values.size();
	}
	else {
		if (sz % 2 != 0)
			return Returningerror("WND(871): The vector should contain an even number of elements.", idthread);

		if (!action) {
			for (i = 0; i < sz; i += 2) {
				x = table->getfloat(i);
				y = table->getfloat(i + 1);
				if (!i) {
					maxX = x;
					minX = maxX;
					minY = x;
					maxY = minY;
				}
				else {
					minX =  min(minX, x);
					maxX = max(maxX, x);
					minY =  min(minY, y);
					maxY = max(maxY, y);
				}
			}
			if (maxX == minX || maxY == minY)
				return kvect;
		}
	}

	kvect->storevalue(minx); //0
	kvect->storevalue(miny); //1
	kvect->storevalue(maxx); //2
	kvect->storevalue(maxy); //3
	if (thickness > 1) {
		minx += thickness;
		miny += thickness >> 1;
		maxx -= thickness;
		maxy -= (thickness << 1) - (thickness >> 1);
	}
	if (action != 2) {
		incx = (maxx - minx) / (maxX - minX);
		incy = (maxy - miny) / (maxY - minY);
	}

	for (i = 0; i < sz; i += 2) {
		x = table->getfloat(i);
		y = table->getfloat(i + 1);

		x = minx + incx*x - incx*minX;
		y = miny + maxy - incy*y + incy*minY;
		if (!thickness) {
			if (i)
				fl_line((int)a, (int)b, (int)x, (int)y);
			a = x;
			b = y;			
		}
		else {
			if (thickness == 1)
				fl_point((int)x, (int)y);
			else
				fl_circle((int)x, (int)y, thickness);
		}
	}

	kvect->storevalue(minX); //4
	kvect->storevalue(minY); //5
	kvect->storevalue(maxX); //6
	kvect->storevalue(maxY); //7
	kvect->storevalue(incx); //8
	kvect->storevalue(incy); //9
	kvect->storevalue((double)thickness); //10

	if (table != points)
		table->Release();

	return kvect;
}

Tamgu* Tamguwindow::PlotCoords(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Tamgu* kvect = callfunc->Evaluate(0, contextualpattern, idthread);
	if (kvect->Size() != 11)
		return Returningerror("WND(862): Wrong values to compute coordinates", idthread);
	double x = callfunc->Evaluate(1, contextualpattern, idthread)->Float();
	double y = callfunc->Evaluate(2, contextualpattern, idthread)->Float();
	double minx = kvect->getfloat(0);
	double miny = kvect->getfloat(1);
	double maxy = kvect->getfloat(3);
	double minX = kvect->getfloat(4);
	double minY = kvect->getfloat(5);
	double incx = kvect->getfloat(8);
	double incy = kvect->getfloat(9);
	int thickness = kvect->getfloat(10);
	if (thickness > 1) {
		minx += thickness;
		miny += thickness >> 1;
		maxy -= (thickness << 1) - (thickness >> 1);
	}
	x = minx + incx*x - incx*minX;
	y = miny + maxy - incy*y + incy*minY;
	Tamgu* kres = Selectafvector(contextualpattern);
	kres->storevalue(x);
	kres->storevalue(y);
	return kres;
}

Tamgu* Tamguwindow::MethodAsk(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	int a;
	string msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string msg1 = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	string msg2 = callfunc->Evaluate(2, contextualpattern, idthread)->String();
	if (callfunc->Size() == 3)
		a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), NULL);
	else {
		string msg3 = callfunc->Evaluate(3, contextualpattern, idthread)->String();
		if (callfunc->Size() == 4)
			a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), msg3.c_str());
		else {
			string msg4 = callfunc->Evaluate(4, contextualpattern, idthread)->String();
			a = fl_choice(msg.c_str(), msg1.c_str(), msg2.c_str(), msg3.c_str(), msg4.c_str());
		}
	}
	return globalTamgu->Provideint(a);
}

Tamgu* Tamguwindow::MethodAlert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	string msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	fl_alert(msg.c_str());
	return aTRUE;
}

Tamgu* Tamguwindow::MethodPassword(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	string msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	const char* pass = fl_password(msg.c_str());
	if (pass == NULL)
		return aNULL;
	return globalTamgu->Providestring(pass);
}

Tamgu* Tamguwindow::MethodInput(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {


	string msg;
	if (callfunc->Size() == 1)
		msg = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	else
		msg = "get";
	const char* ret = fl_input(msg.c_str());
	if (ret == NULL)
		return aNULL;
	return globalTamgu->Providestring(ret);
}

Tamgu* Tamguwindow::MethodPost(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (window == NULL)
        return Returningerror("WND(303): No window available", idthread);
    
    Tamgu* function = callfunc->arguments[0];
    if (!function->isFunction())
        return Returningerror("WND(109): Wrong element, the first parameter should be a function", idthread);

    TamguCallFunction kfunc(function);
    Tamgu* a;
    long i;
    for (i = 1; i < callfunc->Size(); i++) {
        a = callfunc->arguments[i];
        a->Setreference();
        kfunc.arguments.push_back(a);
    }
    
    kfunc.Eval(aNULL, aNULL, idthread);
    
    for (i = 0; i < kfunc.arguments.last; i++)
        kfunc.arguments[i]->Resetreference();
    
    return aTRUE;
}

Tamgu* Tamguwindow::MethodOnClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	Tamgu* function = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!function->isFunction())
		return Returningerror("WND(109): Wrong element, the first parameter should be a function", idthread);

	Tamgu* object = callfunc->arguments[1];
	if (!object->isCallVariable())
		object = object->Eval(aNULL, aNULL, idthread);

	onclose = new FLCallback(function, object, this);
	window->callback(window_callback, onclose);
	return aTRUE;
}

Tamgu* Tamguwindow::MethodOntime(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);
	Tamgu* function = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!function->isFunction())
		return Returningerror("WND(109): Wrong element, the first parameter should be a function", idthread);
	Tamgu* object = callfunc->arguments[2];
	if (!object->isCallVariable())
		object = object->Eval(aNULL, aNULL, idthread);

	if (timeout == NULL)
        timeout = &Timedata(function, object, this);
    else
        timeout->Set(function, object, this);

	timeout->timevalue = callfunc->Evaluate(1, contextualpattern, idthread)->Float();

	return aTRUE;
}

Tamgu* Tamguwindow::MethodRedrawing(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (window == NULL)
		return Returningerror("WND(303): No window available", idthread);

    if (timeout == NULL)
        timeout = &Timedata(aNULL, aNULL, this);
    else
        timeout->Set(aNULL, aNULL, this);

	timeout->timevalue = callfunc->Evaluate(0, contextualpattern, idthread)->Float();

	return aTRUE;
}

Tamgu* Tamguwindow::MethodSetFonts(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	fontmax = Fl::set_fonts("*");
	return globalTamgu->Provideint(fontmax);
}

Tamgu* Tamguwindow::MethodGetFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	int i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (i >= fontmax)
		return Returningerror("WND(431): Unavailable font id", idthread);
	return globalTamgu->Providestring(Fl::get_font_name(i));
}

Tamgu* Tamguwindow::MethodGetFontSizes(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	int i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
	if (i >= fontmax)
		return Returningerror("WND(431): Unavailable font id", idthread);
	int* sizes;
	int nb = Fl::get_font_sizes(i, sizes);
	Tamgu* kvect = Selectaivector(contextualpattern);
	kvect->Clear();
	if (nb == 0)
		return kvect;
	for (int i = 0; i < nb; i++)
		kvect->storevalue((long)sizes[i]);
	return kvect;
}

Tamgu* Tamguwindow::MethodNbFonts(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	return globalTamgu->Provideint(fontmax);
}





/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgugui.h
 Date       : 2017/07/14
 Purpose    :
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef doublewindow_h
#define doublewindow_h

#ifdef uchar
#undef uchar
#endif

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Table_Row.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Select_Browser.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Progress.H>

#ifndef FLTKNOJPEG
#include <FL/Fl_JPEG_Image.H>
#endif

//--------------------------------------------------------------------------------------
class Tamguwidget;
//--------------------------------------------------------------------------------------
int Fltkrun(short idthread);
bool Stopall();
void Initstopall(bool s);
void Setstopall(bool s);
//--------------------------------------------------------------------------------------
void fltkinitialisationtype();
bool fltk_create_variables(TamguGlobal* global);
void SaveItem(Tamgu* e);
//--------------------------------------------------------------------------------------
extern int fontmax;
extern short redraw_index;
extern map<int, string> ifonts;

extern map<string, unsigned int> boxTypes;
extern map<string, unsigned int> fonts;
extern map<string, unsigned int> buttonShapes;
extern map<string, unsigned int> buttonAction;
extern map<string, unsigned int> keyCodes;
extern map<string, Fl_Color> colors;
extern map<string, unsigned int> aligns;
extern map<string, unsigned int> lineshape;
extern map<string, unsigned int> slidertype;
extern map<string, unsigned int> actions;
extern map<string, unsigned int> labeltypes;
extern map<string, unsigned int> buttontypes;
extern map<string, unsigned int> cursors;
extern map<string, unsigned long> commands;

class widge {
public:
	Fl_Widget* wdg;
	short idthread;
	long id;
	Tamguwidget* kw;

	widge(Fl_Widget* w, Tamguwidget* k, long idg, int i) {
		wdg = w;
		kw = k;
		id = idg;
		idthread = i;
	}

	void redraw();
};

void flredraw(void* data);
//--------------------------------------------------------------------------------------

class Idcouple {
public:

	Tamgu* o;
	long id;
	long count;

	Idcouple(Tamgu* e = NULL, bool set = false) {
		count = 0;
		o = e;
		if (e != NULL) {
			id = globalTamgu->RecordInTrackerProtected(e);
			if (set) {
				count++;
				o->Setreference();
			}
		}
		else
			id = -1;
	}

	void Use(Tamgu* e);

	void Set() {
		if (o != NULL) {
			count++;
			o->Setreference();
		}
	}

	void Reset() {
		if (o != NULL) {
			while (count > 0) {
				globalTamgu->ResetWithTracker(o, id);
				count--;
			}
		}
	}

	TamguFunction* Function() {
		return (TamguFunction*)o;
	}

	Tamgu* Object() {
		return o;
	}
};

class Tamguwindow;
class Doublewindow : public Fl_Double_Window {
public:
	Tamgu* function;
	Tamgu* object;
	bool ready;
	Tamguwindow* window;
	long iwindow;

	hmap<int, Idcouple> functionmouse;
	hmap<int, Idcouple> functionkey;
	hmap<int, Idcouple> objectmouse;
	hmap<int, Idcouple> objectkey;
	bool dragdrop;
	Idcouple functiondragdrop;
	Idcouple objectdragdrop;
	string eventtext;
	TamguGlobal* global;

	Doublewindow(int x, int y, const char* l, Tamgu* f, Tamgu* o, Tamgu* c, Tamguwindow* wn);
	Doublewindow(int x, int y, int w, int h, const char* l, Tamgu* f, Tamgu* o, Tamgu* c, Tamguwindow* wn);

	~Doublewindow();

	int mouse_callback(int e);
	int key_callback(int e);
	int dragdropcall_back();

	bool Setdragdrop(Tamgu* f, Tamgu* o) {
		functiondragdrop.Use(f);
		objectdragdrop.Use(o);
		return true;
	}

	bool Setmouseaction(int e, Tamgu* f, Tamgu* object);

	bool Setkeyaction(int e, Tamgu* f, Tamgu* object);

	void draw();
	int handle(int e);

};

class FLCallback {
public:

	Tamgu* function;
	Tamguwindow* window;
 long iwindow;
	Tamgu* object;
	long iobject;
	double timevalue;
    bool used;

    FLCallback() {
        used = false;
        timevalue = 0;
        function = NULL;
        window = NULL;
        iwindow = -1;
        object = NULL;
        iobject = -1;
    }

	FLCallback(Tamgu* func, Tamgu* o, Tamguwindow* win) {
        used = true;
		timevalue = 0;
		function = func;
		window = win;
        iwindow = ((Tamgu*)window)->Idtracker();
		object = o;
		object->Setreference();
		iobject = globalTamgu->RecordInTrackerProtected(object);
	}

	void Set(Tamgu* func, Tamgu* o, Tamguwindow* win) {
        if (object != NULL && iobject != -1)
            globalTamgu->ResetWithTracker(object, iobject);

        used = true;
		timevalue = 0;
		function = func;
		window = win;
        iwindow = ((Tamgu*)window)->Idtracker();
		object = o;
		object->Setreference();
		iobject = globalTamgu->RecordInTrackerProtected(object);
	}

    void Reset() {
        globalTamgu->ResetWithTracker(object, iobject);
        timevalue = 0;
        function = NULL;
        window = NULL;
        iwindow = -1;
        object = NULL;
        iobject = -1;
        used = false;
    }
};


#endif


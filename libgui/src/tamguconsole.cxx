/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguconsole.cxx
 Date       : 2017/09/01
 Purpose    : console to edit and debug tamgu programs.
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

//Use tam_DLL only in Windows, if you link to the tmg DLL
//#define tam_DLL

#ifdef UNIX
#include <unistd.h>
#endif

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tabs.H>

#include "tamgu.h"
#include "tamguversion.h"
#include "globaltamgu.h"
#include "compilecode.h"
#include "tamgudebug.h"
#include "x_tokenize.h"

#include <thread>

static const char* tmgwhile = "while";
static const char* tmgfor = "for";
static const char* tmgswitch = "switch";
static const char* tmgif = "if";
static const char* tmgelse = "else";
static const char* tmgelif = "elif";

static x_coloringrule tok_colorparse;
static tokenizer_result<string> x_colorparse;
static tokenizer_automaton token_auto;

string Tamgudate();

#ifdef APPLE
#define FLCTRL FL_COMMAND
#define TESTCOMBI Fl::event_command
#define TAILLE_NORMALE FL_NORMAL_SIZE
#else
#define FLCTRL FL_CTRL
#define TESTCOMBI Fl::event_ctrl
#ifdef UNIX
#define TAILLE_NORMALE FL_NORMAL_SIZE-1
#else
#define TAILLE_NORMALE FL_NORMAL_SIZE
#endif
#endif

#ifdef C11
using std::map;
#endif

extern "C" {
	Exporting bool TamguLaunching();
}

//---------------------------------------------------------------------------------------------------------
class TamguLocalEditor;
//---------------------------------------------------------------------------------------------------------
static bool Tamgurun(TamguLocalEditor* e, string filename, string codeinit, ostringstream* os, bool asdebug, bool threadmode, bool display);
//------------------------------------------------------------------------------------
long VirtualIndentation(string& codestr);
void SetBlankSize(long sz);
long GetBlankSize();
//------------------------------------------------------------------------------------------
void ResetGetEditor(); //when in a console, we use a window version of the get...
//------------------------------------------------------------------------------------------
static hmap<short, vector<string> > thevectorstrings;
static hmap<short, map<string, string> > thevectorinfos;
static hmap<string, short> tmgtypes;
static hmap<short, short> equivalences;

static bool double_display = false;

class TamguLocalEditorMain;
static TamguLocalEditorMain* mainwindow;
static Fl_Double_Window* w_mainwindow;
static Fl_Group* maingroup;
static Fl_Tabs* tabwindows;

static vector<TamguLocalEditor*> editors;

//---------------------------------------------------------------------------------------------------------------------------
static hmap<string, vector<long> > i_breakpoints;

void NextLine(Fl_Widget *w, void *data);
void Shortname(Fl_Widget *w, void *data);
void Gotonextbreak(Fl_Widget *w, void *data);
void StopExecution(Fl_Widget *w, void *data);
void Gotoend(Fl_Widget *w, void *data);
void Getin(Fl_Widget *w, void *data);
void Getout(Fl_Widget *w, void *data);



static Debuginfo infodebug;


class DebugEditor;

static Tamgu* Debug_callback(vector<Tamgu*>& stack, short idthread, void* data);

class DebugWindow {
public:

	Fl_Window*    debugwindow;
	DebugEditor*      localvariables;
	DebugEditor*      allvariables;
	DebugEditor*      stack;
	Fl_Output*           currentline;
#if defined(WIN32) || defined(APPLE)
	Fl_Return_Button*   nextline;
#else
	Fl_Button*		   nextline;
#endif
	Fl_Button*		   stop;
	Fl_Button*		   go;
	Fl_Button*          upToTheEnd;
	Fl_Button*          infunction;
	Fl_Button*          outfunction;
	Fl_Check_Button*    shortname;
	Fl_Output*		   ofilename;
	string localstack;
	string variables;
	string allthevariables;
	char ch[50];
	TamguLocalEditor* editor;
	long vcurrentline;
	LockedThread loquet;
	string vfilename;
	agnostring localline;


	DebugWindow(TamguLocalEditor* e);

	~DebugWindow() {
		infodebug.clearall();
	}

	bool Endofexecution() {
		if (globalTamgu->executionbreak)
			return true;
		return false;
	}

	void update(TamguLocalEditor* e, string filename, string thevariables, string allvars, string thestack, long thecurrentline);

	void updatefields();

	void Release() {
		loquet.Released();
	}

	void show() {
		debugwindow->show();
	}

	void hide() {
		debugwindow->hide();
	}
};

static DebugWindow* old_current_debug_display = NULL;
static DebugWindow* current_debug_display = NULL;

//---------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//Implemented in FLTK...

const int EDITORMAXBUFFER = 250000;

static bool stopmainloop = false;

string Removetrailing(string& res) {
	string chaine(res);
	long d, f;
	for (d = 0; d<chaine.size(); d++) {
		if ((uchar)chaine[d]>32)
			break;
	}

	for (f = chaine.size() - 1; f >= 0; f--) {
		if ((uchar)chaine[f] > 32)
			break;
	}
	long lg = f - d + 1;
	if (lg >= 1)
		chaine = chaine.substr(d, lg);
	else
		chaine = "";
	return chaine;
}

//---------------------------------------------------------------------------------------------------------

static bool comparewith(string& s, string with) {
	if (s.size() <= with.size())
		return false;
	for (int i = 0; i < with.size(); i++)
	if (s[i] != with[i])
		return false;
	return true;
}

static bool isinstruction(string& s) {
	char c = s[s.size() - 1];
	if (c == ')') {
		if (comparewith(s, tmgwhile))
			return true;
		if (comparewith(s, tmgfor))
			return true;
		if (comparewith(s, tmgswitch))
			return true;
		if (comparewith(s, tmgif))
			return true;
	}
	return false;
}

static void call_choice(Fl_Widget* w, void* x);

static void local_filebrowser_callback(Fl_File_Chooser *fc, void* data);
static void local_filebrowser_callback_open(Fl_File_Chooser *fc, void* data);

static Fl_Text_Display::Style_Table_Entry localstyletable[14];// Style table

static void InitStyleTable(int unicodefont, int unicodefont_italic) {
	localstyletable[0] =  { FL_BLACK, unicodefont, TAILLE_NORMALE }; // A - Plain
	localstyletable[1] =  { FL_DARK_RED, unicodefont, TAILLE_NORMALE }; // B - numbers
	localstyletable[2] =  { FL_DARK_GREEN, unicodefont, TAILLE_NORMALE }; // C - Comments
	localstyletable[3] =  { FL_RED, unicodefont, TAILLE_NORMALE }; // D - Strings (double)
	localstyletable[4] = { fl_rgb_color(130, 130, 240), unicodefont, TAILLE_NORMALE }; // E - Strings (simple)
	localstyletable[5] =  { FL_BLACK, unicodefont, TAILLE_NORMALE }; // F - Keywords
	localstyletable[6] =  { FL_BLUE, unicodefont, TAILLE_NORMALE }; // G - Keywords function, thread, if, switch,else,elif,try, catch
	localstyletable[7] = { fl_rgb_color(160, 100, 10), unicodefont, TAILLE_NORMALE }; // H - Keywords preceded with a '.'
	localstyletable[8] =  { FL_DARK_RED, unicodefont_italic, TAILLE_NORMALE }; // I - Breakpoints
	localstyletable[9] = { fl_rgb_color(135, 135, 135), unicodefont, TAILLE_NORMALE }; // J - Long comments
	localstyletable[10] =  { FL_DARK_MAGENTA, unicodefont, TAILLE_NORMALE }; // K -  variables
	localstyletable[11] = { fl_rgb_color(20, 30, 180), unicodefont, TAILLE_NORMALE }; // L -  features
	localstyletable[12] = { FL_DARK_RED, unicodefont, TAILLE_NORMALE }; // M - console
    localstyletable[13] = { fl_rgb_color(120, 80, 10), unicodefont, TAILLE_NORMALE }; // N - Keywords followed with a '('
}

static map<string, bool> localkeywords;

class e_modified {
public:

	string buff;
	int pos;
	int nb;
	char inserted;

	e_modified(int p, char i, int n, string& s) : buff(s) {
		pos = p;
		nb = n;
		inserted = i;
	}

	bool add(int p, int n, string& s) {
#ifdef WIN32
		unsigned char c = s.back();
#else
		unsigned char c = s[s.size() - 1];
#endif
		if (!inserted || c<'0' || (c>'9' && c<'A') || (c>'Z' && c < 'a'))
			return false;
		if (p == pos + nb) {
			nb += n;
			buff += s;
			return true;
		}
		return false;
	}
};


class TamguEditorTextBuf : public Fl_Text_Buffer {
public:
	char* wbuffer;
	int internalsize;
	bool donotprocess;
	list<e_modified> doing;
	list<e_modified> undoing;


	TamguEditorTextBuf() : Fl_Text_Buffer(EDITORMAXBUFFER) {
		internalsize = 1024;
		wbuffer = new char[1024];
		donotprocess = false;
		//canUndo(1);
	}

	~TamguEditorTextBuf() {
		delete[] wbuffer;
	}

	void addinsertion(int p, int n, string s, int nDeleted) {
		if (doing.size() && doing.back().add(p, n, s))
			return;

		e_modified e(p, 1, n, s);
		if (nDeleted)
			e.inserted = 2;
		if (doing.size() >= 1000)
			doing.pop_front();
		doing.push_back(e);
	}

	void adddeletion(int p, int n, string s) {
		e_modified e(p, 0, n, s);
		if (doing.size() >= 1000)
			doing.pop_front();
		doing.push_back(e);
	}

	void insertlocaltext(int pos, string& a) {
		a = s_replacestrings(a, "\r\n", "\n");
		insert(pos, STR(a));
	}

	void clear() {
		donotprocess = true;
		text("");
	}

	void setstyle(int start, int end, uchar style) {
		int l = end - start;
		if (l >= internalsize) {
			delete[] wbuffer;
			internalsize = l + 100;
			wbuffer = new char[internalsize];
		}
		memset(wbuffer, style, l);
		wbuffer[l] = 0;
		replace(start, end, wbuffer);
	}

	//Extract a word from position s
	//s should be the last character of the word
	int wordfromend(int e) {
		int c = char_at(--e);
		while (e >= 0 && c > 32) {
			c = char_at(--e);
		}
		return e + 1;
	}

	string subrange(int s, int e) {
		if (s > length() || e > length() || (e - s) <= 0)
			return "";
		string code;
		char* str = text_range(s, e);
		code = str;
		free(str);
		return code;
	}
};


struct Local_Style_Arg {
	TamguLocalEditor* e;
	TamguEditorTextBuf* s;
};

static void local_style_update(int pos,          // I - Position of update
	int        nInserted,    // I - Number of inserted chars
	int        nDeleted,     // I - Number of deleted chars
	int        nRestyled,    // I - Number of restyled chars
	const char *deletedText, // I - Text that was deleted
	void       *stylarg);

static void localdisplay(string s, void* o);

static void local_number_callback(Fl_Widget *w, void *data);

static void local_close_callback(Fl_Widget *w, void *data);
static void find_next_button_callback(Fl_Widget *w, void *data);
static void find_cancel_button_callback(Fl_Widget *w, void *data);


static void Menu_CB(Fl_Widget*w, void*data);


class tmgFindEditor : public Fl_Text_Editor {
public:
	TamguEditorTextBuf* textbuf;

	tmgFindEditor(int x, int y, int h, int w, char* f) : Fl_Text_Editor(x, y, h, w, f) {
		textbuf = new TamguEditorTextBuf();
		buffer(textbuf);
	}

	void value(string& s) {
		textbuf->text(STR(s));
		textbuf->select(0, textbuf->length());
	}

	string value() {
		char* txt = textbuf->text();
		string s(txt);
		free(txt);
		return s;
	}
};




class TamguLocalFind {
public:

	Fl_Window          find_dlg;
	Fl_Input           find_find;
	Fl_Return_Button   find_next;
	Fl_Button          find_cancel;
	Fl_Check_Button	   find_case;
	TamguLocalEditor* editor;
	int start, end;
	bool found;


	TamguLocalFind(TamguLocalEditor* e, string& tofind) :
		find_dlg(300, 105, 450, 120, "Find"),
		find_find(70, 10, 350, 25, "String:"),
		find_next(20, 70, 130, 25, "Next"),
		find_cancel(290, 70, 130, 25, "Cancel"),
		find_case(290, 40, 60, 25, "Case")	{

		find_dlg.set_modal();
		find_next.callback(find_next_button_callback, (void*)this);
		find_cancel.callback(find_cancel_button_callback, (void*)this);
		if (tofind.size() > 0) {
			find_find.value(STR(tofind));
			find_find.position(0, tofind.size());
		}
		find_case.value(false);
		editor = e;
		find_dlg.show();
		start = 0;
		end = 0;
		found = true;
	}
};

static void replace_all_button_callback(Fl_Widget *w, void *data);
static void replace_next_button_callback(Fl_Widget *w, void *data);
static void replace_cancel_button_callback(Fl_Widget *w, void *data);
static void close_choice(Fl_Widget *w, void *data);

class TamguLocalReplace {
public:

	Fl_Window           replace_dlg;
	Fl_Input            replace_find;
	Fl_Input			replace_with;
	Fl_Button			replace_all;
	Fl_Return_Button	replace_next;
	Fl_Button          replace_cancel;
	Fl_Check_Button	   replace_case;
	TamguLocalEditor* editor;
	int start, end;
	bool found;


	TamguLocalReplace(TamguLocalEditor* e, string& tofind, string& toreplace) :
		replace_dlg(300, 105, 600, 130, "Replace"),
		replace_find(80, 10, 500, 25, "Find:"),
		replace_with(80, 40, 500, 25, "Replace:"),

		replace_all(10, 90, 170, 25, "Replace All"),
		replace_next(200, 90, 170, 25, "Replace Next"),
		replace_cancel(390, 90, 100, 25, "Cancel"),
		replace_case(520, 90, 100, 25, "Case")	{

		replace_dlg.set_modal();
		replace_all.callback(replace_all_button_callback, (void*)this);
		replace_next.callback(replace_next_button_callback, (void*)this);
		replace_cancel.callback(replace_cancel_button_callback, (void*)this);
		if (tofind.size() > 0) {
			replace_find.value(STR(tofind));
			replace_find.position(0, tofind.size());
		}
		replace_with.value(STR(toreplace));
		replace_case.value(false);
		editor = e;
		replace_dlg.show();
		start = 0;
		end = 0;
		found = false;
	}
};

class tmgSuggest {
public:
	Fl_Window localwnd;
	Fl_Choice wchoice;
	Fl_Menu_Item* flm;
	TamguLocalEditor* edt;
	long nb;

	tmgSuggest(TamguLocalEditor* e, string& code, int x, int y, int, int, bool);
	~tmgSuggest();
};

class tmgChoiceItem {
public:
	tmgSuggest* wnd;
	TamguLocalEditor* editor;
	Fl_Menu_Item* fli;
	int pos;
	int idx;

	tmgChoiceItem(TamguLocalEditor* e, int i, int p, Fl_Menu_Item* f, tmgSuggest* w) {
		editor = e;
		idx = i + 1;
		fli = f;
		wnd = w;
		pos = p;
	}
};

class TamguLineNumber : public Fl_Input {
public:

	TamguLineNumber(int x, int y, int w, int h, TamguLocalEditor* ae) : Fl_Input(x, y, w, h, "") {
		callback(local_number_callback, ae);
		when(FL_WHEN_ENTER_KEY);
	}


	int handle(int e) {
		switch (e) {
		case FL_KEYUP:
			return 1;
		}
		return Fl_Input::handle(e);
	}

};

class TamguLocalEditor : public Fl_Text_Editor {
public:
	TamguEditorTextBuf* textbuf;
	string filename;
	string tabname;
	TamguEditorTextBuf* stylebuf;
	Local_Style_Arg stylearg;
	TamguLocalEditor* main;
	Fl_Menu_Bar* menubar;
	Fl_Menu_Item menuitems[3];
	string searchstring;
	string searchreplace;
	TamguLocalFind* findwindow;
	TamguLocalReplace* replacewindow;
	char filetype;
	bool utf8;
	int rmain, rthread, rdebug;
	int idwindow;

	int idwnd;
	string code;
	TamguLineNumber o_linenumber;
	Fl_Check_Button o_utf8;
	Fl_Group* wgroup;

	char lastop;
	bool issaved;
	bool lastcasesensitive;
	int nbelements;
	vector<tmgChoiceItem*> choicemenuitems;
	string str_choicemenuitems;

	hmap<string, string> declared_variables;

	void Testtypefile() {
		filetype = 0;
		if (filename != "" && filename.size() > 4) {
			if (filename.substr(filename.size() - 3, 3) == ".py")
				filetype = 1;
		}
		string sep;
#ifdef WIN32
		sep = '\\';
#else
		sep='/';
#endif
		int ps = filename.rfind(sep, filename.size() - 1);
		if (ps != string::npos)
			tabname = filename.substr(ps + 1, filename.size() - ps - 1);
		else
			tabname = filename;
	}

	TamguLocalEditor(TamguLocalEditor* m, int x, int y, int w, int h, const char* f) : Fl_Text_Editor(x, y, w, h, ""), o_linenumber(w - 80, 35, 55, 20, this), o_utf8(w - 160, 35, 45, 20, "UTF8") {
		utf8 = true;
		idwindow = 0;
		o_utf8.value(true);
		lastcasesensitive = false;
		issaved = true;
		idwnd = 0;
		lastop = 0;
		menubar = NULL;
		if (!strcmp(f, "Console"))
			filename = f;
		else
			filename = NormalizeFileName(f);
		main = m;
		textbuf = new TamguEditorTextBuf();
		stylebuf = new TamguEditorTextBuf();
		buffer(textbuf);
		rmain = 0; rthread = 0; rdebug = 0;
		nbelements = sizeof(localstyletable) / sizeof(localstyletable[0]);
		highlight_data(stylebuf, localstyletable, nbelements, 'A', NULL, 0);
		stylearg.s = stylebuf;
		stylearg.e = this;
		textbuf->add_modify_callback(local_style_update, &stylearg);
		Testtypefile();
		if (tabname == "")
			tabname = "Noname";
	}

	void Loadfromfile(string pathname) {
		ifstream load(STR(pathname), openMode);
		if (load.fail()) {
			code = "Creating a new file:";
			code += pathname;
			Display(code, -1);
			code.clear();
			return;
		}
		code.clear();
		string s;
		bool first = true;
		while (!load.eof()) {
			getline(load, s);
			if (first) {
				//we check the utf8 signature
				if (s.size() >= 3 && (uchar)(s[0]) == 239 && (uchar)(s[1]) == 187 && (uchar)(s[2]) == 191)
					s = s.substr(3, s.size() - 3);
				first = false;
			}
			code += s;
			code += '\n';
		}
		Setvalue(code);
		issaved = true;
	}

	bool Tosave() {
		return issaved;
	}

	virtual int currentline(int pos = -1) {
		int linenum = -1;
		if (pos == -1)
			pos = insert_position();
		position_to_line(pos, &linenum);
		linenum += get_absolute_top_line_number();
		return linenum;
	}

	void Updateline() {
		int linenum = currentline();
		char ch[20];
		sprintf_s(ch, 20, "%d", linenum);
		o_linenumber.value(ch);
	}

	void Line(int pos) {
		int linenum = currentline();
		char ch[20];
		sprintf_s(ch, 20, "%d", linenum);
		o_linenumber.value(ch);
	}

	void Selectline(long line, bool sel) {
		long start = 0;
		long end = 0;
		while (line > 1) {
			end = line_end(start, 1);
			start = end + 1;
			line--;
		}
		if (sel == false) {
			insert_position(start);
			Updateline();
		}
		else {
			end = line_end(start, 1);
			insert_position(start);
			textbuf->select(start, end);
		}
		show_insert_position();
	}

	void Getline(int line, agnostring& localline) {
		int start = 0;
		int end = 0;
		while (line > 1) {
			end = line_end(start, 1);
			start = end + 1;
			line--;
		}
		end = line_end(start, 1);
		insert_position(start);
		textbuf->select(start, end);
		localline = textbuf->subrange(start, end);
		show_insert_position();
	}

	void Gotoline() {
		const char* val = fl_input("Line", "");
		if (val != NULL && strlen(val) > 0) {
			long line = atoi(val);
			Selectline(line, false);
		}
	}

	void SetBlanks() {
		long sz = GetBlankSize();
		char ch[10];
		sprintf_s(ch, 10, "%d", sz);
		const char* val = fl_input("Blanks Size", ch);
		if (val != NULL && strlen(val) > 0) {
			sz = atoi(val);
			SetBlankSize(sz);
		}
	}

	bool Find() {
		const char* val;
		lastop = 0;
		val = (const char*)searchstring.c_str();
		if (val != NULL && strlen(val) > 0) {
			lastop = 1;
			int start = insert_position();
			int end = start;
			if (textbuf->search_forward(start, val, &end, lastcasesensitive) == 1) {
				textbuf->select(end, end + searchstring.size());
				insert_position(end + searchstring.size());
				show_insert_position();
				Updateline();
				return true;
			}

			if (textbuf->search_forward(0, val, &end, lastcasesensitive) == 1) {
				textbuf->select(end, end + searchstring.size());
				insert_position(end + searchstring.size());
				show_insert_position();
				Updateline();
				return true;
			}
		}
		return false;
	}

	bool Findfirst(int& start, int& end, bool casesensitive) {
		if (searchstring.size() == 0)
			return false;
		lastcasesensitive = casesensitive;
		const char* tofind = (const char*)searchstring.c_str();
		start = insert_position();
		end = start;
		int l = searchstring.size();

		if (textbuf->search_forward(start, tofind, &end, casesensitive) == 1) {
			textbuf->select(end, end + l);
			start = end;
			end += l;
			insert_position(end);
			show_insert_position();
			Updateline();
			return true;
		}

		if (textbuf->search_forward(0, tofind, &end, casesensitive) == 1) {
			textbuf->select(end, end + l);
			start = end;
			end += l;
			insert_position(end);
			show_insert_position();
			Updateline();
			return true;
		}
		return false;
	}

	void Replaceone(int start, int end, bool casesensitive) {
		lastop = 0;
		if (searchstring.size() == 0)
			return;
		lastcasesensitive = casesensitive;
		const char* tofind = (const char*)searchstring.c_str();
		const char* toreplace = (const char*)searchreplace.c_str();
		long l = searchstring.size();
		long lrep = searchreplace.size();
		lastop = 2;
		if (textbuf->search_forward(start, tofind, &end, casesensitive) == 1) {
			textbuf->replace(end, end + l, toreplace);
			insert_position(end + lrep);
			show_insert_position();
		}
	}

	void Replaceall(bool casesensitive) {
		lastop = 0;
		if (searchstring.size() == 0)
			return;
		lastcasesensitive = casesensitive;
		const char* tofind = (const char*)searchstring.c_str();
		const char* toreplace = (const char*)searchreplace.c_str();
		long l = searchstring.size();
		int start = 0;
		int end = 0;
		while (textbuf->search_forward(start, tofind, &end, casesensitive) == 1) {
			start = end + l;
			textbuf->replace(end, start, toreplace);
			start = end;
		}
	}

	void FindWindow() {
		findwindow = new TamguLocalFind(this, searchstring);
	}

	void ReplaceWindow() {
		replacewindow = new TamguLocalReplace(this, searchstring, searchreplace);
	}

	virtual void Execute(TamguLocalEditor* e, string thecode, int start, bool debugmode, bool threadmode) {}

	virtual string Value() {
		char* str = textbuf->text();
		string code = str;
		free(str);
		return code;
	}

	virtual void Display(string a, int end) {
		main->Display(a, end);
	}

	virtual void Findbreakpoints(hmap<string, vector<long> >& breaks) {
		if (filename == "")
			return;

		string f = NormalizeFileName(filename);

		int mx = stylebuf->length();
		for (int i = 0; i < mx; i++) {
			if (stylebuf->byte_at(i) == 'I') {
				breaks[f].push_back(i);
				i = line_end(i, 1);
			}
		}
	}

	void Savecontent() {
		string c = Value();
		utf8 = o_utf8.value();
		string s;
		if (!utf8) {
			if (!s_is_utf8(USTR(c), c.size())) {
				s = s_utf8_to_latin(USTR(c), c.size());
				c = s;
			}
		}

		ofstream save(STR(filename), ios::binary);
		save << STR(c);
		save.close();

		SyntaxColor(0, 0);
		issaved = true;
	}

	void SyntaxColor(int start, int max) {

		int rastart, raend;
		string localcode;

		i_breakpoints.clear();
		if (start == 0 && max == 0)  {
			Findbreakpoints(i_breakpoints);
			localcode = Value();
			rastart = 0;
			raend = stylebuf->length();
			stylebuf->setstyle(0, raend, 'A');
		}
		else {
			rastart = 0;
			raend = max;
			localcode = textbuf->subrange(start, max);
			stylebuf->setstyle(start, max, 'A');
		}

		tok_colorparse.tokenize<string>(localcode, x_colorparse);

		int end = 0;
        long l;
		char* v;
		int i, r;
        string sub;

		for (i = 0; i<x_colorparse.stack.size(); i++) {
			l = x_colorparse.stack[i].size();
			v = (char*)x_colorparse.stack[i].c_str();

			r = textbuf->search_forward(start, (char*)v, &end, 0);
			start = end + l;
			if (r == 0)
                continue;
            switch (x_colorparse.stacktype[i]) {
                case 1:
                    stylebuf->setstyle(end, start, 'D');
                    break;
                case 2:
                    stylebuf->setstyle(end, start, 'E');
                    break;
                case 3:
                    if (textbuf->byte_at(start) == '>')
                        stylebuf->setstyle(end, start + 1, 'B');
                    else
                        stylebuf->setstyle(end, start, 'B');
                    break;
                case 4:
                    if (localkeywords.find(x_colorparse.stack[i]) != localkeywords.end())
                        stylebuf->setstyle(end, start, 'G');
                    else
                        stylebuf->setstyle(end, start, 'F');
                    break;
                case 5:
                    stylebuf->setstyle(end, start, 'J');
                    break;
                case 6:
                    stylebuf->setstyle(end, start, 'K');
                    break;
                case 9:
                    stylebuf->setstyle(end, start, 'C');
                    break;
                case 11: //.label(
                    stylebuf->setstyle(end+1, start-1, 'H');
                    break;
                case 12:
                    sub=x_colorparse.stack[i].substr(0,x_colorparse.stack[i].size()-1);
                    if (localkeywords.find(sub) != localkeywords.end())
                        stylebuf->setstyle(end, start-1, 'G');
                    else
                        stylebuf->setstyle(end, start-1, 'N');
                    break;
                case 13:
                    stylebuf->setstyle(end, start, 'N');
                    break;
            }
        }

		for (i = 0; i < i_breakpoints[filename].size(); i++) {
			start = i_breakpoints[filename][i];
			end = line_end(start, 1);
			stylebuf->setstyle(start, end, 'I');
		}
		redisplay_range(rastart, raend);
	}

	void Indente() {
		if (filetype)
			return;
		string c = Value();
		code.clear();
		int pos = insert_position();
		IndentCode(c, code, GetBlankSize());
		e_modified e(0, 3, c.size(), c);
		textbuf->doing.push_back(e);
		textbuf->text(code.c_str());
		textbuf->doing.pop_back();
		insert_position(pos);
		show_insert_position();
		Updateline();
		SyntaxColor(0, 0);
	}

	virtual int Start() {
		return -1;
	}

	virtual void Setvalue(string& c) {
		if (c == "")
			utf8 = true;
		else {
			c = s_replacestrings(c, "\r\n", "\n");
			utf8 = s_is_utf8(USTR(c), c.size());
			string s;
			if (!utf8) {
				s_latin_to_utf8(s, USTR(c), c.size());
				c = s;
			}
			textbuf->text(c.c_str());
		}

		o_utf8.value(utf8);
		SyntaxColor(0, 0);
	}

	void getcodewithnospace(int start, int end, string& c) {
		start = line_start(start);
		while (start > 0) {
			if ((uchar)textbuf->byte_at(start) > 32)
				break;
			start = line_start(start - 1);
		}

		c = textbuf->subrange(start, end);
	}

    int justewhite(int start) {
        start = line_start(start);
        int compte = 0;
        while (start > 0) {
            if ((uchar)textbuf->byte_at(start) != 32)
                break;
            compte++;
            start = line_start(start - 1);
        }
        cerr <<"COMPTE:" << compte << endl;
        return compte;
    }


	int justwhite(string& lastline) {
		int compte = 0;
		for (int i = 0; i < lastline.size() - 1; i++) {
			if (lastline[i] != 32)
				return compte;
			compte++;
		}
		return compte;
	}

	//-------------------------------------------------
	virtual int key_callback_up(int e) {
		int key = Fl::event_key();
		int ekey = Fl::event_state();
		if (key == 65289 && (ekey & FL_CTRL) == FL_CTRL) { //ctrl+tab
			int idx = idwnd + 1;
			if (idx >= editors.size())
				idx = 0;
			tabwindows->value(editors[idx]->wgroup);
			return 1;
		}

		if (!filetype) {
			const char* tkey = Fl::event_text();
			if (tkey[0] == '}' || tkey[0] == ')') {
				int start = insert_position();
				start = line_start(start);
				int end = line_end(start, 1);
				code = textbuf->subrange(start, end);
                string cremove = Removetrailing(code);
				if (cremove == "}" || cremove == ")") {
					int startcolor = line_start(start - 1);
					int endcolor = line_end(startcolor, 1);
					getcodewithnospace(startcolor, endcolor, code);
					long blanc = VirtualIndentation(code);
                    blanc -= GetBlankSize();
                    if (blanc) {
                        string blanks(blanc, ' ');
                        blanks += cremove;
                        textbuf->replace(start, end, STR(blanks));
                    }
                    else {
                        code = cremove;
                        textbuf->replace(start, end, STR(code));
                    }
				}
				return 1;
			}

			if (key == 65293) {//carriage return...

				Fl::lock();
				if (str_choicemenuitems != "") {
					str_choicemenuitems = "";
					Fl::unlock();
					return 1;
				}
				Fl::unlock();

				int start = insert_position(), startcolor, endcolor, end;
				start = line_start(start);
				end = line_end(start, 1);

				startcolor = line_start(start - 1);
				endcolor = line_end(startcolor, 1);
				SyntaxColor(startcolor, endcolor);
				getcodewithnospace(startcolor, end, code);
				long blanc = VirtualIndentation(code);
                if (blanc) {
                    string blanks(blanc, ' ');                    
                    textbuf->insertlocaltext(end, blanks);
                    insert_position(line_start(end) + blanks.size());
                }
                removebreaks();
				Updateline();
				return 1;
			}
		}
		else {
			if (key == 65293) {//carriage return...

				Fl::lock();
				if (str_choicemenuitems != "") {
					str_choicemenuitems = "";
					Fl::unlock();
					return 1;
				}
				Fl::unlock();

				int start = insert_position(), startcolor, endcolor, end;
				start = line_start(start);
				end = line_end(start, 1);

				startcolor = line_start(start - 1);
				endcolor = line_end(startcolor, 1);
				SyntaxColor(startcolor, endcolor);

				char cwhite = 32;
				int blanc = startcolor;
				uchar c = (uchar)textbuf->byte_at(blanc);
				while (c == 9 || c == 32) {
					cwhite = c;
					blanc++;
					c = (uchar)textbuf->byte_at(blanc);
				}
				code = "";
				for (int i = startcolor; i<blanc; i++)
					code += cwhite;
				if (code.size()>0) {
					textbuf->insertlocaltext(end, code);
					insert_position(end + code.size());
				}

				Updateline();
				return 1;
			}
		}
		Updateline();
		return Fl_Text_Editor::handle(e);
	}

	virtual bool ismain() {
		return false;
	}

	virtual int handle(int e) {
		switch (e) {
		case FL_RELEASE:
			Updateline();
			break;
		case FL_KEYDOWN:
			if (Fl::event_key() == 122 && TESTCOMBI())
				return 0;
			break;
		case FL_KEYUP:
			if (key_callback_up(e) == 1)
				return 1;
			break;
		}
		return Fl_Text_Editor::handle(e);
	}
	//-----------------------------------------------------

	virtual TamguLocalEditor* Editor(string pathname);

	virtual void removebreaks() {
		stylebuf->setstyle(0, stylebuf->length(), 'A');
		SyntaxColor(0, 0);
	}

	int Findcorrespondance() {
		int linepos = currentline() - 1;
		int start = line_start(insert_position());
		int end = line_end(start, 1);
		code = textbuf->subrange(start, end);
		bool search = false;
		int direction = 1;

		if (code.find("}") != string::npos) {
			code = textbuf->subrange(0, end);
			direction = -1;
			search = true;
		}
		else
		if (code.find("{") != string::npos) {
			code = textbuf->subrange(start, textbuf->length());
			direction = 1;
			search = true;
		}
		if (search == false)
			return -1;

		token_auto.tokenize<string>(code, x_colorparse);

		int count = 0;

        if (direction == 1) {
			for (start = 0; x_colorparse.stack.size(); start++) {
				if (x_colorparse.stack[start] == "{")
					count++;
				else
				if (x_colorparse.stack[start] == "}") {
					count--;
					if (count == 0)
						return x_colorparse.stackln[start] + linepos;
				}
			}
		}
		else {
			for (start = x_colorparse.stack.size() - 1; start >= 0; start--) {
				if (x_colorparse.stack[start] == "}")
					count++;
				else
				if (x_colorparse.stack[start] == "{") {
					count--;
					if (count == 0)
						return x_colorparse.stackln[start];
				}
			}
		}
		return -1;
	}

	virtual void EvaluateCommand();


	virtual void Setfilename(string f) {
		filename = f;
		Testtypefile();
		wgroup->label(STR(tabname));
	}

	virtual void Addmenu() {
		const char* msg = "Two 'runs'.\n\tThe first runs in the main frame and CANNOT BE stopped.\n\tThe second runs in a thread and CAN BE stopped.";
		menubar = new Fl_Menu_Bar(2, 30, 450, 30);
		menubar->tooltip(msg);
		menubar->add("File/Open", FLCTRL + 'o', Menu_CB, (void*)this);    // userdata is always 'this'
		menubar->add("File/Save", FLCTRL + 's', Menu_CB, (void*)this);
		menubar->add("File/Save as", FLCTRL + FL_SHIFT + 's', Menu_CB, (void*)this);
		menubar->add("File/Close", FLCTRL + 'w', Menu_CB, (void*)this);
#ifdef WIN32
		menubar->add("Edit/Undo", FLCTRL + 'z', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'z', Menu_CB, (void*)this, 0x80);
#else
		menubar->add("Edit/Undo", FLCTRL + 'u', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'u', Menu_CB, (void*)this, 0x80);
#endif
		menubar->add("Edit/List", FLCTRL + FL_SHIFT + 'l', Menu_CB, (void*)this);
		menubar->add("Edit/Matching", FLCTRL + 'j', Menu_CB, (void*)this);
		menubar->add("Edit/Indent", FLCTRL + 'i', Menu_CB, (void*)this);
		menubar->add("Edit/(Un)comment", FLCTRL + 'k', Menu_CB, (void*)this);
		menubar->add("Edit/Goto line", FLCTRL + 'l', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Set blanks", 0, Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Find", FLCTRL + 'f', Menu_CB, (void*)this);
		menubar->add("Edit/Replace", FLCTRL + 'h', Menu_CB, (void*)this);
		menubar->add("Edit/Next", FLCTRL + 'n', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Font larger", FLCTRL + 'y', Menu_CB, (void*)this);
		menubar->add("Edit/Font smaller", FLCTRL + FL_SHIFT + 'y', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Cut", FLCTRL + 'x', Menu_CB, (void*)this);
		menubar->add("Edit/Copy", FLCTRL + 'c', Menu_CB, (void*)this);
		menubar->add("Edit/Paste", FLCTRL + 'v', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Negation", FL_ALT + FL_SHIFT + 'n', Menu_CB, (void*)this);
        menubar->add("Edit/Rule left arrow", FL_ALT + FL_SHIFT + 'a', Menu_CB, (void*)this);
        menubar->add("Edit/Rule right arrow", FL_CTRL + FL_ALT + FL_SHIFT + 'a', Menu_CB, (void*)this);
        menubar->add("Edit/Different", FL_ALT + FL_SHIFT + 'd', Menu_CB, (void*)this);
		menubar->add("Edit/Disjunction", FL_ALT + 'o', Menu_CB, (void*)this);
		menubar->add("Edit/Conjunction", FL_ALT + 'a', Menu_CB, (void*)this);
        menubar->add("Edit/Pi", FL_ALT + 'p', Menu_CB, (void*)this);
        menubar->add("Edit/Tau", FL_ALT + 't', Menu_CB, (void*)this);
		menubar->add("Edit/Euler", FL_ALT + 'e', Menu_CB, (void*)this);
		menubar->add("Edit/Golden", FL_ALT + 'g', Menu_CB, (void*)this);
		menubar->add("Edit/Multiply", FL_ALT + 'm', Menu_CB, (void*)this);
		menubar->add("Edit/Divide", FL_ALT + 'd', Menu_CB, (void*)this);
		menubar->add("Edit/Sum", FL_ALT + FL_SHIFT + 's', Menu_CB, (void*)this);
		menubar->add("Edit/Product", FL_ALT + FL_SHIFT + 'p', Menu_CB, (void*)this);
		menubar->add("Edit/Square", FL_ALT + 's', Menu_CB, (void*)this);
		menubar->add("Edit/Cube", FL_ALT + 'c', Menu_CB, (void*)this);
		menubar->add("Edit/Square Root", FL_ALT + 'r', Menu_CB, (void*)this);
		menubar->add("Edit/Cubic Root", FL_ALT + 'u', Menu_CB, (void*)this);
		rmain = menubar->add("Command/Run (main thread)", FLCTRL + 'r', Menu_CB, (void*)this);
		rthread = menubar->add("Command/Run (threaded)", FLCTRL + 'e', Menu_CB, (void*)this);
		menubar->add("Command/Stop", FL_ALT + 'b', Menu_CB, (void*)this, 0x80);
		rdebug = menubar->add("Command/Debug", FLCTRL + 'd', Menu_CB, (void*)this);
		menubar->add("Command/Toggle Breakpoint", FLCTRL + 'b', Menu_CB, (void*)this);
		menubar->add("Command/Remove All Breakpoints", FLCTRL + FL_SHIFT + 'b', Menu_CB, (void*)this);
	}

	virtual void Deactivate() {
		Fl_Menu_Item* items = (Fl_Menu_Item*)menubar->menu();
		items[rthread].deactivate();
		items[rdebug].deactivate();
		items[rmain].deactivate();
	}

	virtual void Reactivate() {
		Fl_Menu_Item* items = (Fl_Menu_Item*)menubar->menu();
		items[rthread].activate();
		items[rdebug].activate();
		items[rmain].activate();
	}

	virtual TamguLocalEditor* tmgCreateWindow(string path, bool fullnew = false) {
		return this;
	}
};

//------------------------------------------------------------------------------------------------------------------
static void call_choice(Fl_Widget* w, void* x) {
	tmgChoiceItem* fli = (tmgChoiceItem*)x;
	TamguLocalEditor* edt = fli->editor;
	Fl::lock();
	edt->str_choicemenuitems = fli->fli->text;
	int pos = edt->str_choicemenuitems.find("(");
	if (pos == -1)
		edt->str_choicemenuitems += "(";
	else
		edt->str_choicemenuitems = edt->str_choicemenuitems.substr(0, pos + 1);
	pos = fli->pos;
	edt->insert(STR(edt->str_choicemenuitems));
	Fl::unlock();
	delete fli->wnd;
}

static void close_choice(Fl_Widget* w, void* x) {
	delete (tmgSuggest*)x;
}

tmgSuggest::tmgSuggest(TamguLocalEditor* e, string& code, int x, int y, int abspos, int thetype, bool info) : wchoice(30, 10, 170, 30, ""),
localwnd(x + e->window()->x() + 10, y + e->window()->y() + 10, 220, 50, STR(code)) {

	edt = e;
	nb = thevectorstrings[thetype].size() + 2;
	flm = new Fl_Menu_Item[nb];
	Fl_Menu_Item* fl;

	fl = new Fl_Menu_Item;
	fl->labeltype_ = 0;
	fl->labelfont_ = 0;
	fl->labelsize_ = 12;
	fl->labeltype_ = 0;
	fl->labelcolor_ = FL_BLUE;
	fl->text = "Choose";
	fl->shortcut_ = 0;
	fl->callback_ = 0;
	fl->user_data_ = 0;
	fl->flags = 0;
	flm[0] = *fl;

	int i;
	char* v;
	e->choicemenuitems.clear();
	tmgChoiceItem* kci;
	string val;
	string descript;
	for (i = 0; i < thevectorstrings[thetype].size(); i++) {
		val = thevectorstrings[thetype][i];
		if (thevectorinfos[thetype].find(val) != thevectorinfos[thetype].end()) {
			descript = thevectorinfos[thetype][val];
			if (info)
				val = descript;
		}
		else
			descript = "No description available";

		v = strdup(STR(val));
		if (v != NULL) {
			fl = new Fl_Menu_Item;
			fl->labeltype_ = 0;
			fl->labelfont_ = 0;
			fl->labelsize_ = 12;
			fl->labeltype_ = 0;
			fl->labelcolor_ = FL_BLACK;
			fl->text = v;
			fl->shortcut_ = 0;
			fl->callback_ = call_choice;
			kci = new tmgChoiceItem(e, i, abspos, fl, this);
			fl->user_data((void*)kci);
			e->choicemenuitems.push_back(kci);
			fl->flags = 0;
			flm[i + 1] = *fl;
		}
	}

	i++;
	flm[i].text = 0;
	flm[i].shortcut_ = 0;
	flm[i].callback_ = NULL;
	flm[i].user_data_ = 0;
	flm[i].flags = 0;
	flm[i].labeltype_ = 0;
	flm[i].labelfont_ = 0;
	flm[i].labelsize_ = 12;
	flm[i].labelcolor_ = FL_BLACK;

	wchoice.menu(flm);
	wchoice.value(0);
	localwnd.callback(close_choice, (void*)this);
	localwnd.set_modal();
	Fl::focus(&wchoice);
	localwnd.end();
	localwnd.show();
	const Fl_Menu_Item* cfl = wchoice.menu()->pulldown(wchoice.x(), wchoice.y(), wchoice.w(), wchoice.h(), wchoice.mvalue(), &wchoice);
	if (!cfl || cfl->submenu()) {
		delete this;
		return;
	}
	wchoice.picked(cfl);
}

tmgSuggest::~tmgSuggest() {
	int i;
	for (i = 0; i < edt->choicemenuitems.size(); i++) {
		free((char*)edt->choicemenuitems[i]->fli->text);
		delete edt->choicemenuitems[i];
	}
	edt->choicemenuitems.clear();
	delete[] flm;
}

//------------------------------------------------------------------------------------------------------------------


static void local_style_update(int pos,          // I - Position of update
	int        nInserted,    // I - Number of inserted chars
	int        nDeleted,     // I - Number of deleted chars
	int        nRestyled,    // I - Number of restyled chars
	const char *deletedText, // I - Text that was deleted
	void       *stylarg) {     // I - Callback data



	TamguEditorTextBuf *stylebuf = ((struct Local_Style_Arg*)stylarg)->s;
	TamguLocalEditor* editor = ((struct Local_Style_Arg*)stylarg)->e;
	unsigned char currentstyle = 'A';

	// If this is just a selection change, just unselect the style buffer...
	if (nInserted == 0 && nDeleted == 0) {
		stylebuf->unselect();
		return;
	}

	TamguEditorTextBuf *styletext = NULL;
	if (editor != NULL) {
		editor->issaved = false;
		styletext = editor->textbuf;
		if (styletext->donotprocess)
			styletext = NULL;
	}

	if (nDeleted > 0) {
		// Just delete characters in the style buffer...
		if (styletext != NULL) {
			styletext->adddeletion(pos, nDeleted, deletedText);
			styletext->undoing.clear();
		}
	}

	// Track changes in the text buffer...
	if (nInserted > 0) {
		// Insert characters into the style buffer...
		if (pos > 0) {
			char* cstyle = stylebuf->text_range(pos - 1, pos);
			currentstyle = cstyle[0];
			free(cstyle);
		}
		if (pos == 0 && styletext != NULL && nInserted == styletext->length())
			styletext = NULL;

		char* style = new char[nInserted + 1];
		memset(style, currentstyle, nInserted);
		style[nInserted] = '\0';
		stylebuf->replace(pos, pos + nDeleted, style);
		delete[] style;
		if (styletext) {
			styletext->addinsertion(pos, nInserted, styletext->subrange(pos, pos + nInserted), nDeleted);
			styletext->undoing.clear();
		}
	}
	else
		stylebuf->remove(pos, pos + nDeleted);
}

//-------------------------------------------------------------------------------------------------------
void find_next_button_callback(Fl_Widget *w, void *data) {
	TamguLocalFind* r = (TamguLocalFind*)data;
	if (r->found == false) {
		delete r;
		return;
	}

	r->editor->searchstring = r->find_find.value();
	r->editor->lastcasesensitive = r->find_case.value();
	if (r->editor->Find() == false)
		delete r;
}

void find_cancel_button_callback(Fl_Widget *w, void *data) {
	TamguLocalFind* r = (TamguLocalFind*)data;
	delete r;
}

void replace_all_button_callback(Fl_Widget *w, void *data) {
	TamguLocalReplace* r = (TamguLocalReplace*)data;
	r->editor->searchstring = r->replace_find.value();
	r->editor->searchreplace = r->replace_with.value();
	r->editor->Replaceall(r->replace_case.value());
	delete r;
}

void replace_next_button_callback(Fl_Widget *w, void *data) {
	TamguLocalReplace* r = (TamguLocalReplace*)data;
	r->editor->searchstring = r->replace_find.value();
	r->editor->searchreplace = r->replace_with.value();
	if (r->start == 0 && r->end == 0)
		r->found = r->editor->Findfirst(r->start, r->end, r->replace_case.value());

	if (r->found) {
		r->editor->Replaceone(r->start, r->end, r->replace_case.value());
		r->found = r->editor->Findfirst(r->start, r->end, r->replace_case.value());
	}
	if (r->found == false)
		delete r;
}

void replace_cancel_button_callback(Fl_Widget *w, void *data) {
	TamguLocalReplace* r = (TamguLocalReplace*)data;
	delete r;
}
//------------------------------------------------------------------------------------------------
class TamguLocalEditorMain : public TamguLocalEditor {
public:
	bool startingdisplay;
	int count;
	string intermediarycode;
	int pos;
	ostringstream* os;
	Fl_Output o_tam_version;
	int numnew;


	TamguLocalEditorMain(string fn, int x, int y, int h, int w, const char* txt) : TamguLocalEditor(NULL, x, y, h, w, txt), o_tam_version(800, 5, 180, 20, "") {
		numnew = 0;
		os = NULL;
		char buff[100];
		sprintf_s(buff, 100, "%s", (char*)TamguVersion());
		o_tam_version.value(buff);
		pos = 100;
		main = this;
		count = 0;
	}

	~TamguLocalEditorMain() {
		if (os != NULL)
			delete os;
	}


	void Setfilename(string f) {
		Testtypefile();
	}

	void Deactivate() {}
	void Reactivate() {}

	void Addmenu() {
		menubar = new Fl_Menu_Bar(2, 30, 450, 30);
		menubar->add("File/New", FLCTRL + FL_SHIFT + 'n', Menu_CB, (void*)this);    // userdata is always 'this'
		menubar->add("File/Open", FLCTRL + 'o', Menu_CB, (void*)this);    // userdata is always 'this'
		menubar->add("File/Save", FLCTRL + 's', Menu_CB, (void*)this, 0x80);
		menubar->add("File/Quit", FLCTRL + 'q', Menu_CB, (void*)this);
#ifdef WIN32
		menubar->add("Edit/Undo", FLCTRL + 'z', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'z', Menu_CB, (void*)this, 0x80);
#else
		menubar->add("Edit/Undo", FLCTRL + 'u', Menu_CB, (void*)this);
		menubar->add("Edit/Redo", FLCTRL + FL_SHIFT + 'u', Menu_CB, (void*)this, 0x80);
#endif
		menubar->add("Edit/Cut", FLCTRL + 'x', Menu_CB, (void*)this);
		menubar->add("Edit/Copy", FLCTRL + 'c', Menu_CB, (void*)this);
		menubar->add("Edit/Paste", FLCTRL + 'v', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Mirror", FLCTRL + FL_SHIFT + 'm', Menu_CB, (void*)this, FL_MENU_TOGGLE);
		menubar->add("Edit/List", FLCTRL + FL_SHIFT + 'l', Menu_CB, (void*)this);
		menubar->add("Edit/Editor", FLCTRL + FL_SHIFT + 'e', Menu_CB, (void*)this);
		menubar->add("Edit/Clean", FLCTRL + 'k', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Font larger", FLCTRL + 'y', Menu_CB, (void*)this);
		menubar->add("Edit/Font smaller", FLCTRL + FL_SHIFT + 'y', Menu_CB, (void*)this, 0x80);
		menubar->add("Edit/Negation", FL_ALT + FL_SHIFT + 'n', Menu_CB, (void*)this);
        menubar->add("Edit/Rule left arrow", FL_ALT + FL_SHIFT + 'a', Menu_CB, (void*)this);
        menubar->add("Edit/Rule right arrow", FL_CTRL + FL_ALT + FL_SHIFT + 'a', Menu_CB, (void*)this);
		menubar->add("Edit/Disjunction", FL_ALT + 'o', Menu_CB, (void*)this);
		menubar->add("Edit/Conjunction", FL_ALT + 'a', Menu_CB, (void*)this);
		menubar->add("Edit/Pi", FL_ALT + 'p', Menu_CB, (void*)this);
        menubar->add("Edit/Tau", FL_ALT + 't', Menu_CB, (void*)this);
		menubar->add("Edit/Euler", FL_ALT + 'e', Menu_CB, (void*)this);
		menubar->add("Edit/Golden", FL_ALT + 'g', Menu_CB, (void*)this);
		menubar->add("Edit/Multiply", FL_ALT + 'm', Menu_CB, (void*)this);
		menubar->add("Edit/Divide", FL_ALT + 'd', Menu_CB, (void*)this);
		menubar->add("Edit/Sum", FL_ALT + FL_SHIFT + 's', Menu_CB, (void*)this);
		menubar->add("Edit/Product", FL_ALT + FL_SHIFT + 'p', Menu_CB, (void*)this);
		menubar->add("Edit/Square", FL_ALT + 's', Menu_CB, (void*)this);
		menubar->add("Edit/Cube", FL_ALT + 'c', Menu_CB, (void*)this);
		menubar->add("Edit/Square Root", FL_ALT + 'r', Menu_CB, (void*)this);
		menubar->add("Edit/Cubic Root", FL_ALT + 'u', Menu_CB, (void*)this);
	}

	void EvaluateCommand() {
		char picked[1024];
		menubar->item_pathname(picked, sizeof(picked)-1);
		Fl_File_Chooser* filebrowser;

		if (!strcmp(picked, "File/New")) {
			tmgCreateWindow("", true);
			return;
		}


		if (!strcmp(picked, "File/Open")) {
			filebrowser = new Fl_File_Chooser(".", "*.tmg", Fl_File_Chooser::SINGLE, "Load your file");
			filebrowser->show();
			while (filebrowser->shown())
				Fl::wait();
			if (filebrowser->value() != NULL)
				local_filebrowser_callback_open(filebrowser, this);
			delete filebrowser;
			return;
		}

		if (!strcmp(picked, "File/Quit")) {
			local_close_callback(window(), NULL);
			return;
		}

		string codeindente;
		if (!strcmp(picked, "File/Save")) {
			if (filename == "") {
				filebrowser = new Fl_File_Chooser(".", "*.tmg", Fl_File_Chooser::CREATE, "Save your file");
				filebrowser->show();
				while (filebrowser->shown())
					Fl::wait();
				if (filebrowser->value() != NULL)
					local_filebrowser_callback(filebrowser, this);
				delete filebrowser;
			}
			else
				Savecontent();
			return;
		}

		if (!strcmp(picked, "Edit/List")) {
			code = TamguListing();
			if (!filetype) {
				string codeindente;
				IndentCode(code, codeindente, GetBlankSize());
				Display(codeindente, -1);
			}
			else
				Display(code, -1);
			return;
		}

		if (!strcmp(picked, "Edit/Editor")) {
			tmgCreateWindow("");
			return;
		}

		if (!strcmp(picked, "Edit/Mirror")) {
			double_display = 1 - double_display;
			return;
		}

		if (!strcmp(picked, "Edit/Clean")) {
			intermediarycode.clear();
			count = 0;
			code.clear();
			filename.clear();
			code = "cleaned\n";
			Display(code, -1);
			code.clear();
			main->textbuf->clear();
			return;
		}

		if (!strcmp(picked, "Edit/Font larger")) {
			int sz = localstyletable[0].size;
			sz += 1;
			for (int i = 0; i < nbelements; i++)
				localstyletable[i].size = sz;
			Fl::redraw();
			return;
		}

		if (!strcmp(picked, "Edit/Font smaller")) {
			int sz = localstyletable[0].size;
			if (sz <= 12)
				return;
			sz -= 1;
			for (int i = 0; i<nbelements; i++)
				localstyletable[i].size = sz;
			Fl::redraw();
			return;
		}

		if (!strcmp(picked, "Edit/Undo")) {
			//we remove the top of the list in textbuf doing
			if (textbuf->doing.size()>0) {
				textbuf->donotprocess = true;
				e_modified& e = textbuf->doing.back();
				if (!e.inserted) {
					textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
					insert_position(e.pos + e.nb);
				}
				else {
					textbuf->remove(e.pos, e.pos + e.nb);
					insert_position(e.pos);
				}
				textbuf->undoing.push_back(e);
				textbuf->doing.pop_back();
				textbuf->donotprocess = false;
			}
			return;
		}

		if (!strcmp(picked, "Edit/Redo")) {
			if (textbuf->undoing.size() > 0) {
				textbuf->donotprocess = true;
				e_modified& e = textbuf->undoing.back();
				if (e.inserted) {
					textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
					insert_position(e.pos + e.nb);
				}
				else {
					textbuf->remove(e.pos, e.pos + e.nb);
					insert_position(e.pos);
				}
				textbuf->doing.push_back(e);
				textbuf->undoing.pop_back();
				textbuf->donotprocess = false;
			}
			return;
		}

		if (!strcmp(picked, "Edit/Cut")) {
			Fl_Text_Editor::kf_cut(0, this);
			return;
		}

		if (!strcmp(picked, "Edit/Copy")) {
			Fl_Text_Editor::kf_copy(0, this);
			return;
		}

		if (!strcmp(picked, "Edit/Paste")) {
			Fl_Text_Editor::kf_paste(0, this);
			return;
		}

        if (!strcmp(picked, "Edit/Negation")) {
            long i = insert_position();
            textbuf->insert(i, "¬");
            insert_position(i + 2);
            return;
        }

        if (!strcmp(picked, "Edit/Rule left arrow")) {
            long i = insert_position();
            textbuf->insert(i, "←");
            insert_position(i + 3);
            return;
        }

        if (!strcmp(picked, "Edit/Rule right arrow")) {
            long i = insert_position();
            textbuf->insert(i, "→");
            insert_position(i + 3);
            return;
        }

		if (!strcmp(picked, "Edit/Different")) {
			long i = insert_position();
			textbuf->insert(i, "≠");
			insert_position(i + 3);
			return;
		}

		if (!strcmp(picked, "Edit/Disjunction")) {
			long i = insert_position();
			textbuf->insert(i, "∨");
			insert_position(i + 3);
			return;
		}

		if (!strcmp(picked, "Edit/Conjunction")) {
			long i = insert_position();
			textbuf->insert(i, "∧");
			insert_position(i + 3);
			return;
		}

        if (!strcmp(picked, "Edit/Pi")) {
            long i = insert_position();
            textbuf->insert(i, "π");
            insert_position(i + 2);
            return;
        }

        if (!strcmp(picked, "Edit/Tau")) {
            long i = insert_position();
            textbuf->insert(i, "τ");
            insert_position(i + 2);
            return;
        }

		if (!strcmp(picked, "Edit/Euler")) {
			long i = insert_position();
			textbuf->insert(i, "ℯ");
			insert_position(i + 3);
			return;
		}

		if (!strcmp(picked, "Edit/Golden")) {
			long i = insert_position();
			textbuf->insert(i, "φ");
			insert_position(i + 2);
			return;
		}

		if (!strcmp(picked, "Edit/Multiply")) {
			long i = insert_position();
			textbuf->insert(i, "×");
			insert_position(i + 2);
			return;
		}

		if (!strcmp(picked, "Edit/Divide")) {
			long i = insert_position();
			textbuf->insert(i, "÷");
			insert_position(i + 2);
			return;
		}

		if (!strcmp(picked, "Edit/Sum")) {
			long i = insert_position();
			textbuf->insert(i, "∑(");
			insert_position(i + 4);
			return;
		}

		if (!strcmp(picked, "Edit/Product")) {
			long i = insert_position();
			textbuf->insert(i, "∏(");
			insert_position(i + 4);
			return;
		}

		if (!strcmp(picked, "Edit/Square")) {
			long i = insert_position();
			textbuf->insert(i, "²");
			insert_position(i + 2);
			return;
		}

		if (!strcmp(picked, "Edit/Cube")) {
			long i = insert_position();
			textbuf->insert(i, "³");
			insert_position(i + 2);
			return;
		}

		if (!strcmp(picked, "Edit/Square Root")) {
			long i = insert_position();
			textbuf->insert(i, "√(");
			insert_position(i + 4);
			return;
		}

		if (!strcmp(picked, "Edit/Cubic Root")) {
			long i = insert_position();
			textbuf->insert(i, "∛(");
			insert_position(i + 4);
			return;
		}
	}


	void Findbreakpoints(hmap<string, vector<long> >& breaks) {
		for (int i = 1; i < editors.size(); i++) {
			if (editors[i] != NULL)
				editors[i]->Findbreakpoints(breaks);
		}
	}

	void removebreaks() {
		for (int i = 1; i < editors.size(); i++) {
			if (editors[i] != NULL)
				editors[i]->removebreaks();
		}
	}

	TamguLocalEditor* Editor(string pathname) {
		if (pathname == "")
			return NULL;
		for (int i = 1; i < editors.size(); i++) {
			if (editors[i] != NULL && editors[i]->filename == pathname)
				return editors[i];
		}
		return NULL;
	}

	//In this case, the buffer can wrap, the line position is much more complex then...
	//we cannot use position_to_line
	int currentline(int pos = -1) {
		int linenum = 1;
		if (pos == -1)
			pos = insert_position();

		if (pos > textbuf->length())
			pos = textbuf->length();

		for (int i = 0; i < pos; i++) {
			if (textbuf->byte_at(i) == '\n')
				linenum++;
		}
		return linenum;
	}

	TamguLocalEditor* tmgCreateWindow(string pathname, bool fullnew = false) {
		TamguLocalEditor* e = Editor(pathname);
		if (e != NULL) {
			e->show();
			return e;
		}

		long xgr = maingroup->x();
		long ygr = maingroup->y();
		long wgr = maingroup->w();
		long hgr = maingroup->h();

		Fl_Group* w = new Fl_Group(xgr, ygr, wgr, hgr, STR(pathname));
		e = new TamguLocalEditor(this, 1, 60, wgr - 1, hgr - 31, STR(pathname));
		e->wgroup = w;
		e->idwnd = editors.size();
		editors.push_back(e);
		e->Addmenu();
        if (fullnew) {
            string dte=Tamgudate();
            dte=dte.substr(0,10);
            code="/@\nDate: ";
            code+=dte;
            code+="\nAuthor: \nDescription: \n@/\n\n";
        }

        e->Setvalue(code);
		e->Line(0);
		//w->size_range(10, 10, 0, 0);
		if (pathname == "")
			e->issaved = false;
		else {
			e->filename = NormalizeFileName(pathname);
			e->Testtypefile();
			e->issaved = true;
		}

		w->label(STR(e->tabname));
		w->end();
		w->resizable(e);

		tabwindows->add(w);
		tabwindows->value(w);
		tabwindows->hide();
		tabwindows->show();
		return e;
	}

	int Start() {
		return textbuf->length();
	}

	void Execute(TamguLocalEditor* e, string thecode, int start, bool debugmode, bool threadmode) {
		string pathname = e->filename;
		bool asdebug = false;
		if (debugmode)
			asdebug = true;
		else {
			if (pathname == "")
				pathname = "none";
		}

		window()->cursor(FL_CURSOR_WAIT);
		if (os != NULL)
			delete os;
		os = new ostringstream;
		*os << "\n";
		TamguExtinguish();
		Tamgurun(e, pathname, thecode, os, asdebug, threadmode, false);
		if (os->str().size() > 1)
			Display(os->str(), start);
		code = thecode;
		window()->cursor(FL_CURSOR_DEFAULT);
	}

	void Display(string a, int end) {
		int start;

		if (end >= EDITORMAXBUFFER || (textbuf->length() + a.size()) > EDITORMAXBUFFER) {
			textbuf->text(STR(a));
			end = a.size();
		}
		else
		if (end == -1) {
			end = textbuf->length();
			textbuf->append(STR(a));
		}
		else
			textbuf->insertlocaltext(end, a);

		start = end;
		end = textbuf->length();
		stylebuf->setstyle(start, end, 'M');
		insert_position(end);
		show_insert_position();
		Updateline();
		redraw();
		tabwindows->value(mainwindow->wgroup);
	}

	void Setvalue(string& c) {
		utf8 = s_is_utf8(USTR(c), c.size());
		string s;
		if (!utf8) {
			s_latin_to_utf8(s, USTR(c), c.size());
			code = s;
		}
		else
			code = c;
		o_utf8.value(utf8);
	}

	string Value() {
		return code;
	}

	int key_callback_up(int e) {
		int key = Fl::event_key();
		int ekey = Fl::event_state();
		if (key == 65289 && (ekey & FL_CTRL) == FL_CTRL) { //ctrl+tab
			int idx = idwnd + 1;
			if (idx >= editors.size())
				idx = 0;
			tabwindows->value(editors[idx]->wgroup);
			return 1;
		}

		if (key == 65293) {//carriage return...
			int start = line_start(insert_position());
			int end = line_end(start, 1);
			string c = intermediarycode;
			int blanc = VirtualIndentation(c);
            if (blanc) {
                string blank(blanc, ' ');
                textbuf->insertlocaltext(end, blank);
                insert_position(line_start(end) + blank.size());
            }
			return 1;
		}
		return Fl_Text_Editor::handle(e);
	}

	int key_callback_down(int e) {
		textbuf->donotprocess = false;
		int key = Fl::event_key();
		if (key != 65293) {
			Updateline();
			return Fl_Text_Editor::handle(e);
		}

		if (os != NULL)
			delete os;
		os = new ostringstream;

		int start = 0;
		int end = 0;
		bool execution = false;
		bool display = false;
		bool add = false;
		textbuf->selection_position(&start, &end);
		string s;
		char c;
		if (start != end) {
			s = textbuf->selection_text();
			textbuf->select(end, end);
			s = Trim(s);
			if (s != "") {
				intermediarycode.clear();
				execution = true;
				c = s[s.size() - 1];
				if (c != ';')
					display = true;
				intermediarycode = s;
			}
		}
		else {
			start = insert_position();
			start = line_start(start);
			end = line_end(start, 1);
			s = textbuf->subrange(start, end);
			if (s.size() > 5 && s[s.size() - 1] == '}' && justwhite(s) != 0) {
				s[s.size() - 6] = '}';
				s[s.size() - 1] = ' ';
				textbuf->replace(start, end, STR(s));
			}
			s = Trim(s);
			SyntaxColor(start, end);
		}


		if (s != "") {
			if (!execution) {
				execution = false;
				c = s[s.size() - 1];
				if (isinstruction(s)) {
					intermediarycode += s;
					intermediarycode += "\n";
					add = true;
				}
				else {
					if (c == '{') {
						intermediarycode += s;
						intermediarycode += "\n";
						count++;
						add = true;
					}
					else {
						if (c == '}') {
							intermediarycode += s;
							intermediarycode += "\n";
							count--;
							add = true;
							if (count < 0) {
								intermediarycode.clear();
								count = 0;
								*os << "Wrong instruction" << endl;
							}
							else {
								if (count == 0) {
									execution = true;
									*os << "\nrecorded\n";
								}
							}
						}
						else {
							intermediarycode += s;
							intermediarycode += "\n";
							add = true;
							execution = true;
						}
					}
				}
			}
		}

		if (execution && !count) {
			if (add)
				code += intermediarycode;
			startingdisplay = true;
			*os << "\n";
			Tamgurun(NULL, "console", intermediarycode, os, false, false, true);
			intermediarycode.clear();
		}

		if (os->str().size() > 1)
			Display(os->str(), end);

		end = textbuf->length();
		if (end > 0)
			stylebuf->setstyle(end - 1, end, 'A');
		Updateline();
		return Fl_Text_Editor::handle(e);
	}

	int handle(int e) {
		switch (e) {
		case FL_KEYDOWN:
			if (Fl::event_key() == 122 && TESTCOMBI())
				return 0;
			if (key_callback_down(e) == 1)
				return 1;
			break;
		case FL_RELEASE:
			Updateline();
			break;
		case FL_KEYUP:
			return key_callback_up(e);
		}
		return Fl_Text_Editor::handle(e);
	}

	bool ismain() {
		return true;
	}

};
//--------------------------------------------------------------------------------------------
void Menu_CB(Fl_Widget*w, void*data) {
	TamguLocalEditor* o = (TamguLocalEditor*)data;
	o->EvaluateCommand();
}

void local_number_callback(Fl_Widget *w, void *data) {
	TamguLocalEditor* local = (TamguLocalEditor*)data;
	Fl_Input* d = (Fl_Input*)w;
	const char* val = d->value();
	if (val != NULL && strlen(val) > 0) {
		long line = atoi(val);
		local->Selectline(line, true);
	}
}

void local_close_callback(Fl_Widget *w, void *data) {
	if (data == NULL) {
		stopmainloop = true;
		delete w;
		return;
	}

	TamguLocalEditor* e = (TamguLocalEditor*)data;
	bool asked = false;

	for (int i = 1; i < editors.size(); i++) {
		e = editors[i];
		if (e != NULL) {
			if (e->Tosave() == false) {
				ostringstream message;
				message << "File:'" << e->filename << "' not saved... Do you want to quit? ";
				asked = true;
				if (fl_ask(STR(message.str())) != 1)
					return;
			}
		}
	}

	if (asked == false) {
		if (fl_ask("Do you want to quit?") != 1)
			return;
	}

	stopmainloop = true;
	delete w;
}

static void local_filebrowser_callback(Fl_File_Chooser *fc, void* data) {
	TamguLocalEditor* e = (TamguLocalEditor*)data;
	e->filename = fc->value();
	e->Testtypefile();
	bool add = true;
	if (e->filename.size() > 4) {
		add = false;
		string sub = e->filename.substr(e->filename.size() - 4, 4);
		if (sub != ".tmg")
			add = true;
	}
	if (add)
		e->filename += ".tmg";

	e->Setfilename(e->filename);
	e->Savecontent();
}

static void local_filebrowser_callback_open(Fl_File_Chooser *fc, void* data) {
	if (!fc->shown()) {
		TamguLocalEditor* e = (TamguLocalEditor*)data;
		e->filename = fc->value();
		e = e->tmgCreateWindow(e->filename);
		e->Loadfromfile(e->filename);
	}
}

//---------------------------------------------------------------------------------------------------------------------------



#define FOREVER 1e05
static bool indebug = false;
static int debugstatus = -1;
static void displaythestrings();
static bool available = false;

#ifdef MACUI
void fltk_reinit();
#endif

#ifdef WIN32
void FlResetTimer(); //see Tamguwindow
void Flresetmainthread();
void Fltkclearmainthread();
#endif

static int TamguEditorrun() {
	int tt = 1;

#ifdef WIN32
	int unicodefont = -1;
	int unicodefont_italic = -1;

	long fontmax = Fl::set_fonts("*");
	//we select the Arial Unicode MS font by default...
	string fontname;
	bool found = false;
	for (int i = 1; i < fontmax; i++) {
		fontname = Fl::get_font_name(i);
		if (fontname == "Arial Unicode MS") {
			unicodefont = i;
			if (found)
				break;
			found = true;
		}
		else {
			if (fontname == "Arial Unicode MS italic") {
				unicodefont_italic = i;
				if (found)
					break;
				found = true;
			}
		}
	}
	if (!found || unicodefont == -1 || unicodefont_italic == -1)
		InitStyleTable(FL_COURIER, FL_COURIER_ITALIC);
	else
		InitStyleTable(unicodefont, unicodefont_italic);
#else
	InitStyleTable(FL_COURIER, FL_COURIER_ITALIC);
#endif


#ifdef MACUI
    fltk_reinit();
#endif


#ifdef WIN32
    Fltkclearmainthread();
#endif

    Fl::lock();

#ifdef WIN32
    Fl::unlock();
    Flresetmainthread();
    Fl::lock();
#endif

	while (!stopmainloop) {
		if (available)
			displaythestrings();
		tt = Fl::wait(FOREVER);
	}
	Fl::unlock();
	return 0;
}


//this method is used to display the content of the debugger in a window
//-------------------------------------------------------------------------------------
TamguLocalEditor* TamguLocalEditor::Editor(string pathname) {
	return mainwindow->Editor(pathname);
}

void TamguLocalEditor::EvaluateCommand() {
	char picked[80];
	menubar->item_pathname(picked, sizeof(picked)-1);

	Fl_File_Chooser* filebrowser;
	// How to handle callbacks..

	string codeindente;
	if (!strcmp(picked, "File/Close")) {
		if (wgroup != NULL) {
			for (int i = 0; i < editors.size(); i++) {
				if (editors[i] == this) {
					editors.erase(editors.begin() + i);
					break;
				}
			}

			tabwindows->remove(wgroup);
			delete wgroup;
			w_mainwindow->redraw();
		}
		return;
	}

	if (!strcmp(picked, "File/Open")) {
		filebrowser = new Fl_File_Chooser(".", "*.tmg", Fl_File_Chooser::SINGLE, "Load your file");
		filebrowser->show();
		while (filebrowser->shown())
			Fl::wait();
		if (filebrowser->value() != NULL)
			local_filebrowser_callback_open(filebrowser, mainwindow);
		delete filebrowser;
		return;
	}

	if (!strcmp(picked, "File/Quit")) {
		local_close_callback(window(), NULL);
		return;
	}


	if (!strcmp(picked, "File/Save")) {
		if (filename == "") {
			filebrowser = new Fl_File_Chooser(".", "*.tmg", Fl_File_Chooser::CREATE, "Save your file");
			filebrowser->show();
			while (filebrowser->shown())
				Fl::wait();
			if (filebrowser->value() != NULL)
				local_filebrowser_callback(filebrowser, this);
			delete filebrowser;
		}
		else
			Savecontent();
		return;
	}

	if (!strcmp(picked, "File/Save as")) {
		filename = "";
		filebrowser = new Fl_File_Chooser(".", "*.tmg", Fl_File_Chooser::CREATE, "Save your file as");
		filebrowser->show();
		while (filebrowser->shown())
			Fl::wait();
		if (filebrowser->value() != NULL)
			local_filebrowser_callback(filebrowser, this);
		delete filebrowser;
		return;
	}

	if (!strcmp(picked, "Edit/Find")) {
		FindWindow();
		return;
	}

	if (!strcmp(picked, "Edit/Replace")) {
		ReplaceWindow();
		return;
	}

	if (!strcmp(picked, "Edit/Goto line")) {
		Gotoline();
		return;
	}

	if (!strcmp(picked, "Edit/Set blanks")) {
		SetBlanks();
		return;
	}

	if (!strcmp(picked, "Edit/Font larger")) {
		int sz = localstyletable[0].size;
		sz += 1;
		for (int i = 0; i < nbelements; i++)
			localstyletable[i].size = sz;
		Fl::redraw();
		return;
	}

	if (!strcmp(picked, "Edit/List")) {
		Setvalue(main->code);
		Indente();
		return;
	}

	if (!strcmp(picked, "Edit/Font smaller")) {
		int sz = localstyletable[0].size;
		if (sz <= 12)
			return;
		sz -= 1;
		for (int i = 0; i < nbelements; i++)
			localstyletable[i].size = sz;
		Fl::redraw();
		return;
	}

	if (!strcmp(picked, "Edit/Next")) {
		if (lastop == 1)
			Find();
		else {
			if (lastop == 2) {
				int start, end;
				if (Findfirst(start, end, lastcasesensitive))
					Replaceone(start, end, lastcasesensitive);
			}
		}

		return;
	}

	if (!strcmp(picked, "Edit/Undo")) {
		//we remove the top of the list in textbuf doing
		if (textbuf->doing.size() > 0) {
			textbuf->donotprocess = true;
			e_modified& e = textbuf->doing.back();
			char ins = e.inserted;
			if (ins == 3) {//Full replacement
				string c = Value();
				e_modified nv(0, 3, c.size(), c);
				textbuf->text(STR(e.buff));
				textbuf->undoing.push_back(nv);
				textbuf->doing.pop_back();
				textbuf->donotprocess = false;
				return;
			}

			if (!ins) {
				textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
				insert_position(e.pos + e.nb);
			}
			else {
				textbuf->remove(e.pos, e.pos + e.nb);
				insert_position(e.pos);
			}
			textbuf->undoing.push_back(e);
			textbuf->doing.pop_back();
			if (ins == 2) { //connected actions
				e_modified& eb = textbuf->doing.back();
				textbuf->insert(eb.pos, eb.buff.c_str()); //removed, we reinsert it
				insert_position(eb.pos + eb.nb);
				textbuf->undoing.push_back(eb);
				textbuf->doing.pop_back();
			}
			textbuf->donotprocess = false;
		}
		return;
	}

	if (!strcmp(picked, "Edit/Redo")) {
		if (textbuf->undoing.size() > 0) {
			textbuf->donotprocess = true;
			e_modified& e = textbuf->undoing.back();
			if (e.inserted == 3) {//Full replacement
				string c = Value();
				e_modified nv(0, 3, c.size(), c);
				textbuf->doing.push_back(nv);
				textbuf->text(STR(e.buff));
				textbuf->undoing.pop_back();
				textbuf->donotprocess = false;
				return;
			}

			if (e.inserted) {
				textbuf->insert(e.pos, e.buff.c_str()); //removed, we reinsert it
				insert_position(e.pos + e.nb);
			}
			else {
				textbuf->remove(e.pos, e.pos + e.nb);
				insert_position(e.pos);
			}
			textbuf->doing.push_back(e);
			textbuf->undoing.pop_back();
			if (textbuf->undoing.size()) {
				e_modified& eb = textbuf->undoing.back();
				if (eb.inserted == 2) {
					//second connected action
					textbuf->insert(eb.pos, eb.buff.c_str()); //removed, we reinsert it
					insert_position(eb.pos + eb.nb);
					textbuf->doing.push_back(eb);
					textbuf->undoing.pop_back();
				}
			}
			textbuf->donotprocess = false;
		}
		return;
	}

	if (!strcmp(picked, "Edit/Matching")) {
		long line = Findcorrespondance();
		if (line != -1)
			Selectline(line + 1, true);
		return;
	}

	//we need to get the selected lines
	if (!strcmp(picked, "Edit/(Un)comment")) {
		int start, end;
		int i = textbuf->selection_position(&start, &end);
		string s;
		if (!i) {
			start = insert_position();
			s = textbuf->subrange(start, start + 2);
			if (s == "//")
				textbuf->remove(start, start + 2);
			else
				textbuf->insert(start, "//");
			return;
		}

		bool add = false;
		char* str = textbuf->selection_text();
		int sz = strlen(str);
		if (str[sz - 1] == 10) {
			str[sz - 1] = 0;
			sz--;
			add = true;
		}
		s = str;
		free(str);

		//we uncomment, if it is already commented
		if (s[0] == '/' && s[1] == '/') {
			s = s.substr(2, sz - 2);
			string rep;
			string sub;
			rep = 10;
			sub = 10;
			sub += "//";
			s = s_replacestring(s, sub, rep);
			if (add)
				s += 10;
			textbuf->replace_selection(STR(s));
			return;
		}
		s.insert(0, "//");
		for (i = 0; i < sz; i++) {
			if (s[i] == 10)
				s.insert(i + 1, "//");
		}
		if (add)
			s += 10;
		textbuf->replace_selection(STR(s));
		return;
	}

	if (!strcmp(picked, "Edit/Indent")) {
		Indente();
		return;
	}

	if (!strcmp(picked, "Command/Run (main thread)")) {
		Deactivate();
		main->textbuf->clear();
		main->Execute(this, Value(), main->Start(), false, false);
		return;
	}

	if (!strcmp(picked, "Command/Run (threaded)")) {
		Deactivate();
		main->textbuf->clear();
		main->Execute(this, Value(), main->Start(), false, true);
		return;
	}

	if (!strcmp(picked, "Command/Debug")) {
		if (filename == "") {
			main->Display("Save this window first", -1);
			return;
		}
		Deactivate();
		i_breakpoints.clear();
		int linenum = -1;
		main->Findbreakpoints(i_breakpoints);
		int start;


		hmap<string, vector<long> >::iterator it;
		for (it = i_breakpoints.begin(); it != i_breakpoints.end(); it++) {
			vector<long>& localbreaks = it->second;
			string fname = it->first;
			TamguLocalEditor* e = main->Editor(fname);
			if (e == NULL)
				continue;
			for (int i = 0; i < localbreaks.size(); i++) {
				start = localbreaks[i];
				e->position_to_line(start, &linenum);
				linenum += e->get_absolute_top_line_number();
				infodebug.breakpoints[fname][linenum] = true;
			}
		}

		main->Execute(this, Value(), main->Start(), true, false);
		return;
	}

	if (!strcmp(picked, "Command/Stop")) {
		TamguStop();
		Reactivate();
		return;
	}

	if (!strcmp(picked, "Command/Toggle Breakpoint")) {
		int start = insert_position();
		start = line_start(start);
		int end = line_end(start, 1);
		if (stylebuf->byte_at(start) == 'I') {
			stylebuf->setstyle(start, end, 'A');
			SyntaxColor(start, end);
		}
		else {
			stylebuf->setstyle(start, end, 'I');
			redisplay_range(start, end);
		}
		return;
	}

	if (!strcmp(picked, "Edit/Cut")) {
		Fl_Text_Editor::kf_cut(0, this);
		return;
	}

	if (!strcmp(picked, "Edit/Copy")) {
		Fl_Text_Editor::kf_copy(0, this);
		return;
	}

	if (!strcmp(picked, "Edit/Paste")) {
		Fl_Text_Editor::kf_paste(0, this);
		return;
	}

	if (!strcmp(picked, "Edit/Negation")) {
		long i = insert_position();
		textbuf->insert(i, "¬");
		insert_position(i + 2);
		return;
	}

    if (!strcmp(picked, "Edit/Rule left arrow")) {
        long i = insert_position();
        textbuf->insert(i, "←");
        insert_position(i + 3);
        return;
    }

    if (!strcmp(picked, "Edit/Rule right arrow")) {
        long i = insert_position();
        textbuf->insert(i, "→");
        insert_position(i + 3);
        return;
    }

	if (!strcmp(picked, "Edit/Different")) {
		long i = insert_position();
		textbuf->insert(i, "≠");
		insert_position(i + 3);
		return;
	}

	if (!strcmp(picked, "Edit/Disjunction")) {
		long i = insert_position();
		textbuf->insert(i, "∨");
		insert_position(i + 3);
		return;
	}

	if (!strcmp(picked, "Edit/Conjunction")) {
		long i = insert_position();
		textbuf->insert(i, "∧");
		insert_position(i + 3);
		return;
	}

    if (!strcmp(picked, "Edit/Pi")) {
        long i = insert_position();
        textbuf->insert(i, "π");
        insert_position(i + 2);
        return;
    }

    if (!strcmp(picked, "Edit/Tau")) {
        long i = insert_position();
        textbuf->insert(i, "τ");
        insert_position(i + 2);
        return;
    }

	if (!strcmp(picked, "Edit/Euler")) {
		long i = insert_position();
		textbuf->insert(i, "ℯ");
		insert_position(i + 3);
		return;
	}

	if (!strcmp(picked, "Edit/Golden")) {
		long i = insert_position();
		textbuf->insert(i, "φ");
		insert_position(i + 2);
		return;
	}

	if (!strcmp(picked, "Edit/Multiply")) {
		long i = insert_position();
		textbuf->insert(i, "×");
		insert_position(i + 2);
		return;
	}

	if (!strcmp(picked, "Edit/Divide")) {
		long i = insert_position();
		textbuf->insert(i, "÷");
		insert_position(i + 2);
		return;
	}

	if (!strcmp(picked, "Edit/Sum")) {
		long i = insert_position();
		textbuf->insert(i, "∑(");
		insert_position(i + 4);
		return;
	}

	if (!strcmp(picked, "Edit/Product")) {
		long i = insert_position();
		textbuf->insert(i, "∏(");
		insert_position(i + 4);
		return;
	}

	if (!strcmp(picked, "Edit/Square")) {
		long i = insert_position();
		textbuf->insert(i, "²");
		insert_position(i + 2);
		return;
	}

	if (!strcmp(picked, "Edit/Cube")) {
		long i = insert_position();
		textbuf->insert(i, "³");
		insert_position(i + 2);
		return;
	}

	if (!strcmp(picked, "Edit/Square Root")) {
		long i = insert_position();
		textbuf->insert(i, "√(");
		insert_position(i + 4);
		return;
	}

	if (!strcmp(picked, "Edit/Cubic Root")) {
		long i = insert_position();
		textbuf->insert(i, "∛(");
		insert_position(i + 4);
		return;
	}

	if (!strcmp(picked, "Command/Remove All Breakpoints")) {
		main->removebreaks();
		return;
	}
}
//--------------------------------------------------------------------------------------------

void whenallisfinished(TamguGlobal*);
static TamguLocalEditor* currentexecutionwindow;

static vector<string> stringtodisplay;


void clearstringtodisplay() {
	Locking _lock(GlobalTamgu()->_printlock);
	stringtodisplay.clear();
	available = false;
}

static void addstringtodisplay(string& s) {
	Locking _lock(GlobalTamgu()->_printlock);
	stringtodisplay.push_back(s);
	available = true;
}

#ifdef WIN32
void __stdcall executethread(short arg) {
	if (!TamguRun(arg, true))
		addstringtodisplay(TamguErrorMessage());
}
#else
void executethread(short arg) {
	if (!TamguRun(arg, true)) {
		string msg = TamguErrorMessage();
		addstringtodisplay(msg);
	}
}
#endif

static void displaythestrings() {
	static string res;
	{
		Locking _lock(GlobalTamgu()->_printlock);
        res.clear();
		for (int i = 0; i < stringtodisplay.size(); i++)
			res += stringtodisplay[i];
		stringtodisplay.clear();
		available = false;
	}

	mainwindow->Display(res, -1);
}

static void display_results(string s, void* object) {
	addstringtodisplay(s);
}

static bool Tamgurun(TamguLocalEditor* e, string filename, string code, ostringstream* os, bool asdebug, bool threadmode, bool display) {
	//we normalize the filename to its full version...
	currentexecutionwindow = e;

	TamguGlobal* global = GlobalTamgu();
	if (global == NULL) {
		TamguLaunching();
		global = GlobalTamgu();
	}

	clearstringtodisplay();

	globalTamgu->doubledisplay = double_display;
	globalTamgu->os = os;
	globalTamgu->displayfunction = NULL;

	ResetGetEditor();

	VECTE<Tamgu*> keepstack;

    if (asdebug) {
        globalTamgu->Setdebugmode(true);
        globalTamgu->Setdebugfunction(Debug_callback);
    }
    else {
        globalTamgu->Setdebugmode(false);
        globalTamgu->Setdebugfunction(NULL);
    }

	keepstack = globalTamgu->threads[0].stack;
	globalTamgu->threads[0].stack.clear();

	code = conversion_latin_to_utf8(code);
	//We evaluate in kstart...
	globalTamgu->linereference = 1;
	short idcode;
	try {
		idcode = TamguCompile(code, filename, display);
	}
	catch (TamguRaiseError* err) {
		if (e != NULL)
			e->Reactivate();

		*os << err->message;
		globalTamgu->Cleanerror(0);
		return false;
	}

	if (idcode == -1) {
		*os << TamguErrorMessage();

		globalTamgu->threads[0].stack = keepstack;

		if (e != NULL) {
			long l = TamguCurrentLine();
			string filename = TamguCurrentFilename();
			TamguLocalEditor* lc = mainwindow->Editor(filename);
			if (lc != NULL)
				lc->Selectline(l, true);
			e->Reactivate();
		}

		return false;
	}

	if (asdebug) {
		infodebug.clearall();

		if (old_current_debug_display != NULL)
			current_debug_display = old_current_debug_display;
		else {
			Fl::lock();
			current_debug_display = new DebugWindow(mainwindow);
			Fl::unlock();
		}

		current_debug_display->show();
		threadmode = true;
	}

	bool noerror = true;

	if (threadmode) {
		globalTamgu->doubledisplay = false;
		if (asdebug) {
			globalTamgu->displayfunction = NULL;
			globalTamgu->doubledisplay = true;
		}
		else
			globalTamgu->displayfunction = display_results;
		globalTamgu->terminationfunction = whenallisfinished;
		std::thread* thid = new std::thread(executethread, idcode);
	}
	else {
		globalTamgu->doubledisplay = double_display;
		if (!TamguRun(idcode)) {
			*os << TamguErrorMessage();
			noerror = false;
			if (e != NULL) {
				long l = TamguCurrentLine();
				e->Selectline(l, true);
			}
		}

		globalTamgu->threads[0].stack = keepstack;
		if (e != NULL)
			e->Reactivate();
	}

	return noerror;
}

//------------------------------------------------------------------------------------------
static void display_callback(void* str) {
	if (str != NULL) {
		string* s = (string*)str;
		mainwindow->Display(*s, -1);
		delete s;
	}
	mainwindow->textbuf->donotprocess = false;
}

void whenallisfinished(TamguGlobal* global) {
	globalTamgu->terminationfunction = NULL;
	string serr;

	if (currentexecutionwindow != NULL)
		currentexecutionwindow->Reactivate();
	currentexecutionwindow = NULL;
	if (globalTamgu->GenuineError(0) == true) {
		serr = TamguErrorMessage();
		long l = TamguCurrentLine();
		string filename = TamguCurrentFilename();
		TamguLocalEditor* lc = mainwindow->Editor(filename);
		if (lc != NULL)
			lc->Selectline(l, true);

		if (globalTamgu->doubledisplay)
			*mainwindow->os << serr;
		else
			addstringtodisplay(serr);
		globalTamgu->Cleanerror(0);
	}

	if (globalTamgu->doubledisplay) {
		serr = mainwindow->os->str();
		Fl::awake(display_callback, new string(serr));
	}
	else
		Fl::awake(display_callback, NULL);
}

//---------------------------------------------------------------------------------------------------------------------------
///----------------------------------------------------------------------DEBUG SECTION---------------------------------------
class DebugEditor : public Fl_Text_Editor {
public:

	TamguEditorTextBuf* textbuf;
	TamguEditorTextBuf* stylebuf;
	Local_Style_Arg stylearg;

	DebugEditor(int x, int y, int h, int w, const char* f) : Fl_Text_Editor(x, y, h, w, f) {
		align(FL_ALIGN_LEFT);
		textbuf = new TamguEditorTextBuf();
		stylebuf = new TamguEditorTextBuf();
		buffer(textbuf);
		wrap_mode(WRAP_AT_BOUNDS, 0);
		int nbelements = sizeof(localstyletable) / sizeof(localstyletable[0]);
		highlight_data(stylebuf, localstyletable, nbelements, 'A', NULL, 0);
		stylearg.s = stylebuf;
		stylearg.e = NULL;
		textbuf->add_modify_callback(local_style_update, &stylearg);
	}

	void value(string& s) {
		textbuf->text(STR(s));
	}

	void SyntaxColor() {

		int start = 0;
		int rastart, raend;
		char* texte = textbuf->text();
		string localcode = texte;
		free(texte);
		rastart = 0;
		raend = stylebuf->length();
		stylebuf->setstyle(0, raend, 'A');

		tok_colorparse.tokenize<string>(localcode, x_colorparse);

		int end = 0, l;
		const char* v;
        string sub;

		int i;
        for (i = 0; i < x_colorparse.stack.size(); i++) {
            l = x_colorparse.stack[i].size();
            v = (const char*)x_colorparse.stack[i].c_str();
            textbuf->search_forward(start, v, &end, 0);
            start = end + l;
            switch (x_colorparse.stacktype[i]) {
                case 1:
                    stylebuf->setstyle(end, start, 'D');
                    break;
                case 2:
                    stylebuf->setstyle(end, start, 'E');
                    break;
                case 3:
                    stylebuf->setstyle(end, start, 'B');
                    break;
                case 4:
                    if (localkeywords.find(x_colorparse.stack[i]) != localkeywords.end())
                        stylebuf->setstyle(end, start, 'G');
                    else
                        stylebuf->setstyle(end, start, 'F');
                    break;
                case 9:
                    stylebuf->setstyle(end, start, 'C');
                    break;
                case 11:
                    stylebuf->setstyle(end+1, start-1, 'H');
                    break;
                case 12:
                    sub=x_colorparse.stack[i].substr(0,x_colorparse.stack[i].size()-1);
                    if (localkeywords.find(sub) != localkeywords.end())
                        stylebuf->setstyle(end, start-1, 'G');
                    else
                        stylebuf->setstyle(end, start-1, 'N');
                case 13:
                    stylebuf->setstyle(end, start, 'N');
                    break;
            }
        }

		redisplay_range(rastart, raend);
	}

};

static void close_debug(Fl_Widget *w, void *data) {
	DebugWindow* dw = (DebugWindow*)data;
	globalTamgu->Setdebugmode(false);
	globalTamgu->Setdebugfunction(NULL);
	if (!globalTamgu->executionbreak) {
		current_debug_display->Release();
		globalTamgu->executionbreak = true;
	}
	delete dw->debugwindow;
	dw->debugwindow = NULL;
}

DebugWindow::DebugWindow(TamguLocalEditor* e)  {
	debugwindow = new Fl_Window(100, 100, 800, 700, "Debugger");
	currentline = new Fl_Output(750, 15, 40, 25, "Ln");
	shortname = new Fl_Check_Button(690, 600, 60, 25, "Short value");

	localvariables = new DebugEditor(50, 15, 675, 180, "Locals");
	allvariables = new DebugEditor(50, 200, 675, 180, "Alls");
	stack = new DebugEditor(50, 385, 675, 205, "Stack");

	ofilename = new Fl_Output(45, 600, 625, 25, "Path");
	nextline = new Fl_Return_Button(25, 650, 75, 25, "Next");
	go = new Fl_Button(115, 650, 75, 25, "Goto");
	infunction = new Fl_Button(200, 650, 75, 25, "In");
	outfunction = new Fl_Button(285, 650, 75, 25, "Out");
	stop = new Fl_Button(390, 650, 75, 25, "Stop");
	upToTheEnd = new Fl_Button(475, 650, 75, 25, "To End");

	shortname->value(true);
	nextline->callback(NextLine, (void*)this);
	go->callback(Gotonextbreak, (void*)this);
	shortname->callback(Shortname, (void*)this);
	stop->callback(StopExecution, (void*)this);
	upToTheEnd->callback(Gotoend, (void*)this);
	infunction->callback(Getin, (void*)this);
	outfunction->callback(Getout, (void*)this);

	debugwindow->end();
	debugwindow->resizable(localvariables);
	debugwindow->callback(close_debug, this);
	editor = e;
	vcurrentline = 0;
}

class localeditupdate {
private:
    TamguLocalEditor* editor;
    DebugWindow* debug;
    long currentline;
    string filename;
    string thevariables;
    string allvars;
    string thestack;

public:
    localeditupdate(TamguLocalEditor* e, DebugWindow* d, string f, string v, string a, string s, long c) {
        editor=e;
        currentline=c;
        debug=d;
        filename=f;
        thevariables=v;
        allvars=a;
        thestack=s;
    }

    void select() {
        debug->localstack = thestack;
        debug->variables = thevariables;
        debug->allthevariables = allvars;
        debug->vcurrentline = currentline;
        debug->vfilename = filename;
        debug->updatefields();
        editor->Selectline(currentline, true);
        if (tabwindows->value() != editor->wgroup)
            tabwindows->value(editor->wgroup);
    }
};

static void debug_update(void* e) {
    localeditupdate* ed=(localeditupdate*)e;
    ed->select();
    delete ed;
}

void DebugWindow::updatefields() {
    ofilename->value(STR(vfilename));
    sprintf_s(ch, 50, "%ld", vcurrentline);
    currentline->value(ch);
    stack->value(localstack);
    localvariables->value(variables);
    allvariables->value(allthevariables);

    localvariables->SyntaxColor();
    allvariables->SyntaxColor();

}

void DebugWindow::update(TamguLocalEditor* e, string filename, string thevariables, string allvars, string thestack, long thecurrentline) {
    Fl::awake(debug_update, new localeditupdate(e,this,filename,thevariables,allvars,thestack,thecurrentline));
	loquet.Blocked();
}

//this method is used to display the content of the debugger in a window
bool displaydebug(string thevariables, string allvariables, string thestack, string thefilename, long thecurrentline) {
	infodebug.clear();

	if (current_debug_display == NULL || current_debug_display->editor == NULL) {
		return false;
	}
	TamguLocalEditor* e = current_debug_display->editor->Editor(thefilename);
	if (e == NULL)
		return false;
	current_debug_display->update(e, thefilename, thevariables, allvariables, thestack, thecurrentline);
	return true;
}

void Shortname(Fl_Widget *w, void *data) {
	infodebug.shortname = ((Fl_Check_Button*)w)->value();
}


void NextLine(Fl_Widget *w, void *data) {
	if (globalTamgu->executionbreak) {
		current_debug_display->ofilename->value("End Of Process");
		return;
	}

	infodebug.nextline = true;
	current_debug_display->Release();
}


void Gotonextbreak(Fl_Widget *w, void *data) {
	if (globalTamgu->executionbreak) {
		current_debug_display->ofilename->value("End Of Process");
		return;
	}
    infodebug.getintofunction = false;
    infodebug.getoutfunction = false;
    infodebug.nextline = false;
	current_debug_display->Release();
}

void Getin(Fl_Widget *w, void *data) {
	if (globalTamgu->executionbreak) {
		current_debug_display->ofilename->value("End Of Process");
		return;
	}
	infodebug.getintofunction = true;
	infodebug.getoutfunction = false;
	infodebug.nextline = true;
	current_debug_display->Release();
}

void Getout(Fl_Widget *w, void *data) {
	if (globalTamgu->executionbreak) {
		current_debug_display->ofilename->value("End Of Process");
		return;
	}
	infodebug.lastfunction = -1;
	infodebug.getintofunction = false;
	infodebug.getoutfunction = true;
	infodebug.nextline = true;
	current_debug_display->Release();
}


void StopExecution(Fl_Widget *w, void *data) {
	infodebug.clear();
	globalTamgu->Setdebugmode(false);
	globalTamgu->Setdebugfunction(NULL);
	globalTamgu->executionbreak = true;
	current_debug_display->Release();
}


void Gotoend(Fl_Widget *w, void *data) {
	if (globalTamgu->executionbreak) {
		current_debug_display->ofilename->value("End Of Process");
		return;
	}
	infodebug.clear();
	globalTamgu->Setdebugmode(false);
	globalTamgu->Setdebugfunction(NULL);
	current_debug_display->Release();
}

static Tamgu* Debug_callback(vector<Tamgu*>& stack, short idthread, void* data) {
    Tamgu* res = infodebug.debugger(stack, idthread, data);
    if (res == aTRUE)
        displaydebug(STR(infodebug.localvariables), STR(infodebug.allvariables), STR(infodebug.sstack), STR(infodebug.filename), infodebug.currentline);
    return res;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern "C" {
	Exporting bool window_editor(string filename) {
		inittableutf8();

		vector<string> vs;
		TamguLaunching();
		TamguAllObjects(vs);

		for (size_t i = 0; i < vs.size(); i++)
			localkeywords[vs[i]] = 0;

		TamguExtinguish();

		stopmainloop = false;

		w_mainwindow = new Fl_Double_Window(50, 50, 1001, 850, "tmg");
		w_mainwindow->size_range(10, 10, 0, 0);

		tabwindows = new Fl_Tabs(0, 0, 1001, 850, "");

		maingroup = new Fl_Group(0, 30, 1000, 800, "Console");
		mainwindow = new TamguLocalEditorMain(filename, 1, 60, 999, 769, "Console");
		editors.push_back(mainwindow);

		mainwindow->wgroup = maingroup;
		w_mainwindow->callback(local_close_callback, mainwindow);

		mainwindow->Addmenu();
		mainwindow->wrap_mode(3, -1);
		maingroup->end();

		tabwindows->end();
		tabwindows->hide();
		tabwindows->show();
		w_mainwindow->end();
		w_mainwindow->resizable(mainwindow);
		w_mainwindow->show();
		if (filename != "") {
			TamguLocalEditor* elocal = mainwindow->tmgCreateWindow(filename);
			elocal->Loadfromfile(filename);
		}
		TamguEditorrun();
		return true;
	}

    Exporting bool window_editor_code(string filename, string& code) {
        inittableutf8();

        vector<string> vs;
        TamguAllObjects(vs);

        for (size_t i = 0; i < vs.size(); i++)
            localkeywords[vs[i]] = 0;

        TamguExtinguish();

        stopmainloop = false;

        w_mainwindow = new Fl_Double_Window(50, 50, 1001, 850, "tmg");
        w_mainwindow->size_range(10, 10, 0, 0);

        tabwindows = new Fl_Tabs(0, 0, 1001, 850, "");

        maingroup = new Fl_Group(0, 30, 1000, 800, "Console");
        mainwindow = new TamguLocalEditorMain(filename, 1, 60, 999, 769, "Console");
        editors.push_back(mainwindow);

        mainwindow->wgroup = maingroup;
        w_mainwindow->callback(local_close_callback, mainwindow);

        mainwindow->Addmenu();
        mainwindow->wrap_mode(3, -1);
        maingroup->end();

        tabwindows->end();
        tabwindows->hide();
        tabwindows->show();
        w_mainwindow->end();
        w_mainwindow->resizable(mainwindow);
        w_mainwindow->show();
        if (filename != "") {
            TamguLocalEditor* elocal = mainwindow->tmgCreateWindow(filename);
            elocal->Setvalue(code);
            elocal->issaved = true;
        }
        TamguEditorrun();
        return true;
    }

}


/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgueditor.h
 Date       : 2017/07/02
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgueditor_h
#define tamgueditor_h

#include "tamguwidget.h"

class TamguTextEditor;
struct Stylearg {
	TamguTextEditor* editor;
	Fl_Text_Buffer *stylebuf;
	char currentstyle;
};


class TamguTextBuf : public Fl_Text_Buffer {
public:
	char* wbuffer;
	int internalsize;

	TamguTextBuf() : Fl_Text_Buffer() {
		internalsize = 1024;
		wbuffer = new char[1024];
		canUndo(1);
	}

	~TamguTextBuf() {
		delete[] wbuffer;
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

	char getstyle(int pos) {
		if (pos < 0 || pos >= length())
			return 'A';
		return byte_at(pos);
	}

	unsigned char* internalbuffer() {
		if (internalsize < length()) {
			delete[] wbuffer;
			internalsize = length() + 100;
			wbuffer = new char[internalsize];
		}
		char* t = text();
		strcpy(wbuffer, t);
		free(t);
		return (uchar*)wbuffer;
	}

	string subrange(int s, int e) {
		if (s > length())
			return "";
		string code;
		char* str = text_range(s, e);
		code = str;
		free(str);
		return code;
	}
};

class Tamgueditor;
class TamguTextEditor : public Fl_Text_Editor {
public:
	bool modified;
	Tamgu* functionvscroll;
	Tamgu* objectvscroll;
	Tamgu* functionhscroll;
	Tamgu* objecthscroll;
	Tamgueditor* editor;
	map<int, Idcouple> functionmouse;
	map<int, Idcouple> functionkey;
	map<int, Idcouple> objectmouse;
	map<int, Idcouple> objectkey;

	static void loc_v_scrollbar_cb(Fl_Scrollbar* b, TamguTextEditor* textD);
	static void loc_h_scrollbar_cb(Fl_Scrollbar* b, TamguTextEditor* textD);

	TamguTextEditor(Tamgueditor* e, int x, int y, int w, int h, const char* txt) : Fl_Text_Editor(x, y, w, h, txt) {
		modified = true;
		editor = e;
		mVScrollBar->callback((Fl_Callback*)loc_v_scrollbar_cb, this);
		mHScrollBar->callback((Fl_Callback*)loc_h_scrollbar_cb, this);
		functionvscroll = NULL;
		objectvscroll = aNULL;
		functionhscroll = NULL;
		objecthscroll = aNULL;
	}


	int Firstchar() {
		return mFirstChar;
	}

	int Line(int pos) {
		int linenum = -1;
		position_to_line(pos, &linenum);
		linenum += get_absolute_top_line_number();
		return linenum;
	}

	int Topline() {
		return mTopLineNum;
	}

	int PositionFromLine(Fl_Text_Buffer* buf, int maxLines) {
		int retPos;
		int retLines;
		int retLineStart;
		int retLineEnd;
		wrapped_line_counter(buf, 0, buf->length(), maxLines, true, 0, &retPos, &retLines, &retLineStart, &retLineEnd, true);
		return retPos;
	}

	int mouse_callback(int e);
	int key_callback(int e);

	bool Setmouseaction(int e, Tamgu* f, Tamgu* object);
	bool Setkeyaction(int e, Tamgu* f, Tamgu* object);

	int handle(int e);
	void draw_cursor(int X, int Y);
};

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgueditor;
//This typedef defines a type "editorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgueditor::*editorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgueditor : public Tamguwidget {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static basebin_hash<editorMethod> methods;
	
	
	static bin_hash<bool> nothreads;

	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	TamguTextEditor* editor;
	

	string buf;
	int nbstyle;
	TamguTextBuf* textbuf;
	TamguTextBuf* stylebuf;
	string lowercase;
	struct Stylearg stylearg;
	map<string, string> stylekeys;
	map<string, string> keystyles;
	map<string, string> messagestyle;
	map<string, int> messagecolor;
	Fl_Text_Display::Style_Table_Entry* styletable;
	Fl_Double_Window* sticky;
	char txt[1024];

	Fl_Widget* Widget() {
		return editor;
	}
    
    
	//---------------------------------------------------------------------------------------------------------------------
	Tamgueditor(TamguGlobal* g, Tamgu* parent = NULL) : Tamguwidget(g, parent) {
		//Do not forget your variable initialisation
		editor = NULL;
		stylebuf = NULL;
		nbstyle = 0;
		styletable = NULL;
		sticky = NULL;
        textbuf = NULL;
        stylebuf = NULL;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* index, short idthread);


	short Type() {
		return Tamgueditor::idtype;
	}

	string Typename() {
		return "editor";
	}

	Tamgu* Atom(bool forced = false) {
		return this;
	}

	void itemclose() {
		if (editor != NULL)
			delete editor;
		editor = NULL;
		clean();
	}

    void ResetWidget() {
        if (editor != NULL)
            delete editor;
        editor = NULL;
    }

    unsigned long CallBackArity() {
        return P_SEVEN;
    }

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		Tamgueditor* a = new Tamgueditor(globalTamgu);
		a->function = f;
		return a;
	}

	TamguIteration* Newiteration(bool direction);

	static void AddMethod(TamguGlobal* g, string name, editorMethod func, unsigned long arity, string infos);

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

	virtual bool isValueContainer() {
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	bool InitializeItemMenu(string ckey, Tamgu* kvect, unsigned char* key, int& ikey);
	Tamgu* AddStyle(short idthread, long start, long end, uchar stylecode);
	Tamgu* MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSelection(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSetStyle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSave(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReplace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReplaceAll(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodHighlight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodUnHighlight(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCursorStyle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnMouseClick(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnKey(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnVScroll(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnHScroll(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGetStyle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAddStyles(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDelete(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCut(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCopy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPaste(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSelect(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLineBoundaries(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLineBoundariesChar(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCursor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCursorChar(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGotoLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAnnotate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWord(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWrap(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTextMeasure(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAppend(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBytePosition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCharacterPosition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	void Annotate(short idthread, char* buf, char* sfind, long sz, unsigned char stylecode, int matchcase) {
		if (sz == 0)
			return;

		long i = 0;
		char* ret = strstr(buf, sfind);
		while (ret != NULL) {
			i = ret - buf;
			AddStyle(idthread, i, i + sz, stylecode);
			ret = strstr(++ret, sfind);
		}
	}



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
		return String().size();
	}

	long Integer() {

		return String().size();
	}

	double Float() {

		return String().size();
	}

	string String() {
        if (textbuf == NULL)
            return "";
        
		return (char*)textbuf->internalbuffer();
	}


	bool Boolean() {
        if (editor != NULL)
            return true;
        return false;
	}

};

#endif









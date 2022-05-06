/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusys.cxx
 Date       : 2017/09/01
 Purpose    :
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamgusys.h"
#include "atoms.h"
#include "tamgumap.h"
#include "tamgudate.h"
#include "instructions.h"
#include "tamguivector.h"

#ifdef WIN32
#include "Windows.h"
#include <io.h>
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#ifdef WIN32
#include <conio.h>
#include <ctype.h>

//Handling console tune up
static HANDLE hStdout = 0;
static HANDLE hStdin = 0;
static DWORD lpMode = 0;
static DWORD fdwSaveOldMode;
static UINT codepage = 0;
#else
#include <unistd.h>   //_getch
#include <termios.h>  //_getch
#include <sys/ioctl.h>
#endif

#include <signal.h>
#include <iomanip>


#include "directorylisting.h"
#include "terminalstrings.h"

#ifdef WIN32
#define DUP _dup
#define DUP2 _dup2
#define FILENO _fileno
#else
#include <unistd.h>
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#endif

//We need to declare once again our local definitions.
Exporting basebin_hash<sysMethod>  Tamgusys::methods;

Exporting short Tamgusys::idtype = 0;

static TamguConstsys* _mysys = NULL;

#ifdef WIN32
char* Getenv(char* name);
#else
#define Getenv getenv
#endif

static void _clearscreen() {
#ifdef WIN32
	system("cls");
#else
	cout << sys_clear << sys_clear_scrolling << sys_home;
#endif
}

static long row_size = -1;
static long col_size = -1;

//------------------------------------------------------------------------------------------
//This function is called whenever a resize signal is sent
bool checkresize();
static void callresize(int theSignal) {
    static bool calling = true;
	if (!calling || _mysys == NULL || _mysys->function == NULL || !checkresize())
		return;
    
    calling = false;
	TamguCallFunction2 kfunc(_mysys->function);

	kfunc.arguments.push_back(globalTamgu->ProvideConstint(row_size));
	kfunc.arguments.push_back(globalTamgu->ProvideConstint(col_size));
	
	kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());

    calling = true;
    fflush(stdout);
}

//------------------------------------------------------------------------------------------

#ifdef WIN32
static long size_row = 0;
static long size_col = 0;

void Returnscreensize(long& rs, long& cs, long& sr, long& sc) {
	rs = row_size;
	cs = col_size;
	sr = size_row;
	sc = size_col;
}

bool checkresize() {
	static CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	long rs = csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top;
	long cs = csbiInfo.srWindow.Right - csbiInfo.srWindow.Left;

	if (rs != row_size || cs != col_size) {
		row_size = rs;
		col_size = cs;

		size_row = csbiInfo.dwMaximumWindowSize.X;
		size_col = csbiInfo.dwMaximumWindowSize.Y;
		return true;
	}
	return false;
}

void ResetWindowsConsole() {
	SetConsoleCP(codepage);
	SetConsoleOutputCP(codepage);
	SetConsoleMode(hStdout, lpMode);
	SetConsoleMode(hStdin, fdwSaveOldMode);
}

Tamgusys::~Tamgusys() {
	ResetWindowsConsole();
}

static char check_size_utf8(int utf) {
    unsigned char check = utf & 0xF0;
    
    switch (check) {
        case 0xC0:
            return 1;
        case 0xE0:
            return 2;
        case 0xF0:
            return 3;
    }
    return 0;
}

void getcursor(int& xcursor, int& ycursor) {
	static CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	xcursor = csbiInfo.dwCursorPosition.X;
	ycursor = csbiInfo.dwCursorPosition.Y;
}

//We generate the Unix strings in Windows, to keep the whole code constant across platform
string MouseEventProc(MOUSE_EVENT_RECORD mer) {
	stringstream stre;

	stre << "\033[";
	int x = 0, y = 0;
	COORD mousep = mer.dwMousePosition;

	mousep.X++;
	mousep.Y++;

	long wheel = mer.dwButtonState;

	switch (mer.dwEventFlags)
	{
	case 0:
		if (!mer.dwButtonState) {
			stre << 35 << ";" << mousep.X << ";" << mousep.Y << "M";
			_mysys->tracking = false;
		}
		else if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			stre << 32 << ";" << mousep.X << ";" << mousep.Y<< "M";
			_mysys->tracking = true;
		}
		else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
		{
			stre << 34 << ";" << mousep.X << ";" << mousep.Y<< "M";
			_mysys->tracking = true;
		}
		else
		{
			stre << 34 << ";" << mousep.X << ";" << mousep.Y<< "M";
			_mysys->tracking = true;
		}
		break;
	case MOUSE_HWHEELED:
		if (wheel < 0)
			stre << 97 << ";" << mousep.X << ";" << mousep.Y << "M";
		else
			stre << 96 << ";" << mousep.X << ";" << mousep.Y << "M";
		break;
	case MOUSE_MOVED:
		if (_mysys->tracking)
			stre << 64 << ";" << mousep.X << ";" << mousep.Y << "M";
		else
			stre << 67 << ";" << mousep.X << ";" << mousep.Y << "M";
		break;
	case MOUSE_WHEELED:
		if (wheel < 0)
			stre << 97 << ";" << mousep.X << ";" << mousep.Y<< "M";
		else
			stre << 96 << ";" << mousep.X << ";" << mousep.Y<< "M";
		break;
	default:
		stre << 67 << ";" << mousep.X << ";" << mousep.Y << "M";
	}
	return stre.str();
}

string getwinchar(void(*f)()) {
	static hmap<int, bool> keys;
	static bool init = false;
	if (!init) {
		//this is the list of control keys that we actually care for
		keys[77] = true;
		keys[75] = true;
		keys[80] = true;
		keys[72] = true;
		keys[83] = true;
		keys[71] = true;
		keys[79] = true;
		keys[119] = true;
		keys[117] = true;
		keys[73] = true;
		keys[81] = true;
		keys[116] = true;
		keys[115] = true;
		init = true;
	}

	wstring w;
	string s;
	DWORD cNumRead;
	WCHAR kar;
	INPUT_RECORD irInBuf[512];
	bool stop = false;
	int i;

	//This is the most important trick here, you need to reset the flags at each call...
	DWORD fdwMode = ENABLE_WINDOW_INPUT;
	SetConsoleMode(hStdin, fdwMode);

	kar = 0;
	while (s == "") {
		ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			512,         // size of read buffer 
			&cNumRead); // number of records read 

		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT: {// keyboard input 
				KEY_EVENT_RECORD& key = irInBuf[i].Event.KeyEvent;
				if (key.bKeyDown) {
					kar = key.uChar.UnicodeChar;
					if (!kar) {
						if (keys.find(key.wVirtualScanCode) != keys.end()) {
							s = (uchar)224;
							s += key.wVirtualScanCode;
							return s;
						}
					}
					else {
						w = kar;
						s_unicode_to_utf8(s, w);
						return s;
					}
				}
				break;
			}
			case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
				if (checkresize())
					(*f)();
				break;
			}
		}
	}

	return s;
}

string getcharacter() {
	static hmap<int, bool> keys;
	static bool init = false;
	if (!init) {
		//this is the list of control keys that we actually care for
		keys[77] = true;
		keys[75] = true;
		keys[80] = true;
		keys[72] = true;
		keys[83] = true;
		keys[71] = true;
		keys[79] = true;
		keys[119] = true;
		keys[117] = true;
		keys[73] = true;
		keys[81] = true;
		keys[116] = true;
		keys[115] = true;
		init = true;
	}

	string s;

	DWORD cNumRead;
	WCHAR kar;
	wstring w;
	INPUT_RECORD irInBuf[512];
	bool stop = false;
	int i;
	DWORD fdwMode;

	//This is the most important trick here, you need to reset the flags at each call...
	if (_mysys == NULL || _mysys->mouseenabled == false)
		fdwMode = ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS;
	else
		fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;

	SetConsoleMode(hStdin, fdwMode);

	kar = 0;
	while (s == "") {
		ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			512,         // size of read buffer 
			&cNumRead); // number of records read 

		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT: {// keyboard input 
				KEY_EVENT_RECORD& key = irInBuf[i].Event.KeyEvent;
				if (key.bKeyDown) {
					kar = key.uChar.UnicodeChar;
					if (!kar) {
						if (keys.find(key.wVirtualScanCode) != keys.end()) {
							s = (uchar)224;
							s += key.wVirtualScanCode;
							return s;
						}
					}
					else {
						w = kar;
						s_unicode_to_utf8(s, w);
						return s;
					}
				}
				break;
			}
			case MOUSE_EVENT: // mouse input 
				return MouseEventProc(irInBuf[i].Event.MouseEvent);
			case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
                    callresize(0);
			}
		}
	}

    return s;
}

void sendstring(string s) {
    cout << s;
}

#else
bool checkresize() {
	struct winsize wns;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wns);
	if (row_size != wns.ws_row || col_size != wns.ws_col) {
		struct winsize wns;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &wns);
		row_size = wns.ws_row; //we need to keep a final line on screen for operators
		col_size = wns.ws_col;
		return true;
	}
	return false;
}

static const short _getbuffsize = 128;

void sendstring(string s) {
    fflush(stdout);
    //If you need to get the absolute cursor position, you can decomment these lines
    cout << s;
}

void getcursor(int& xcursor, int& ycursor) {
    fflush(stdout);
    //If you need to get the absolute cursor position, you can decomment these lines
    cout << cursor_position;
    scanf("\033[%d;%dR", &xcursor, &ycursor);
}

string getcharacter(){
    static char buf[_getbuffsize+2];
    memset(buf,0, _getbuffsize);
    fflush(stdout);
        
    string res;
    long nb;
    
    do {
        nb = read(0,buf,_getbuffsize);
        if (nb < 0)
            perror("read()");
        buf[nb] = 0;
        res += buf;
        memset(buf,0, _getbuffsize);
    }
    while (nb == _getbuffsize);
    
    
    return res;
}

Tamgusys::~Tamgusys() {
    Reset();
}
#endif

void Getscreensizes() {
#ifdef WIN32
    static CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

    if (row_size == -1 && col_size == -1) {
		_clearscreen();
		DWORD fdwMode;

		hStdin = GetStdHandle(STD_INPUT_HANDLE);

		// Save the current input mode, to be restored on exit. 

		if (!GetConsoleMode(hStdin, &fdwSaveOldMode)) {
			printf("GetConsoleMode (%d)\n", GetLastError());
			return;
		}


		// Enable the window and mouse input events. 
		fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;;
		if (!SetConsoleMode(hStdin, fdwMode)) {
			printf("SetConsoleMode (%d)\n", GetLastError());
			return;
		}

        codepage = GetConsoleOutputCP();
        //UTF8 setting
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);

        // Get the current screen buffer size and window position.
        hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        //Selecting a different font to display all Unicode characters
        CONSOLE_FONT_INFOEX info = { 0 };
        info.cbSize = sizeof(info);
        info.dwFontSize.Y = 18; // leave X as zero
        info.FontWeight = FW_NORMAL;
        wcscpy(info.FaceName, L"Consolas");
        SetCurrentConsoleFontEx(hStdout, NULL, &info);

        //We set the specific mode to handle terminal commands
        GetConsoleMode(hStdout, &lpMode);
        SetConsoleMode(hStdout, lpMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

		if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
		{
			printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
			return;
		}
		row_size = csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top;
		col_size = csbiInfo.srWindow.Right - csbiInfo.srWindow.Left;

		size_row = csbiInfo.dwMaximumWindowSize.X;
		size_col = csbiInfo.dwMaximumWindowSize.Y;

	}


#else
    struct winsize wns;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wns);
    row_size = wns.ws_row; //we need to keep a final line on screen for operators
    col_size = wns.ws_col;
#endif
}


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgusys::AddMethod(TamguGlobal* global, string name, sysMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamgusys::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgusys::InitialisationModule(global,"");
}


bool Tamgusys::InitialisationModule(TamguGlobal* global, string version) {
    Getscreensizes();
    
    methods.clear();
    
    
    
    Tamgusys::idtype = global->Getid("sys");
    
    Tamgusys::AddMethod(global, "command", &Tamgusys::MethodCommand, P_ONE | P_TWO, "command(string com, string outputfile): execute the command 'com'. outputfile is optional and can be used to redirect output to a file.");
    Tamgusys::AddMethod(global, "isdirectory", &Tamgusys::MethodisDirectory, P_ONE, "isdirectory(string path): return 'true' if path is a directory");
    Tamgusys::AddMethod(global, "createdirectory", &Tamgusys::MethodCreateDirectory, P_ONE, "createdirectory(string path): create the directory of path 'path'");
    Tamgusys::AddMethod(global, "mkdir", &Tamgusys::MethodCreateDirectory, P_ONE, "mkdir(string path): create the directory of path 'path'");
    Tamgusys::AddMethod(global, "listdirectory", &Tamgusys::MethodListDirectory, P_ONE, "listdirectory(string path): return the content of the directory 'path'");
    Tamgusys::AddMethod(global, "ls", &Tamgusys::MethodListDirectory, P_ONE, "ls(string path): return the content of the directory 'path'");
    Tamgusys::AddMethod(global, "realpath", &Tamgusys::MethodRealPath, P_ONE, "realpath(path relpath): return the actual path matching 'realpath'");
    Tamgusys::AddMethod(global, "fileinfo", &Tamgusys::MethodFileInfo, P_ONE, "fileinfo(string pathname): returns the info of the file 'pathname'");
    Tamgusys::AddMethod(global, "env", &Tamgusys::MethodEnv, P_ONE | P_TWO, "env(string var)|env(string varstring val): return or set the content of the environment variable 'var'");
    Tamgusys::AddMethod(global, "pipe", &Tamgusys::MethodPopen, P_ONE, "pipe(string command): execute a command and store the result in a svector.");
    Tamgusys::AddMethod(global, "getchar", &Tamgusys::MethodGETCH, P_NONE, "getchar(): return the last character (or group of characters) keyed in.");
    Tamgusys::AddMethod(global, "reset", &Tamgusys::MethodReset, P_NONE, "reset(): reset the getchar context (Unix only).");
    Tamgusys::AddMethod(global, "colors", &Tamgusys::MethodCOLORS, P_THREE | P_FOUR, "colors(int style, int code1, int code2, bool disp): set text color or return the color code");
    Tamgusys::AddMethod(global, "foregroundcolor", &Tamgusys::MethodFGCOLORS, P_ONE, "foregroundcolor(int color): set foreground text color");
    Tamgusys::AddMethod(global, "backgroundcolor", &Tamgusys::MethodBGCOLORS, P_ONE, "backgroundcolor(int color): set background text color");
    Tamgusys::AddMethod(global, "rgbforegroundcolor", &Tamgusys::MethodRGBFGCOLORS, P_THREE, "rgbforegroundcolor(int red, int green, int blue): set rgb foreground text color");
    Tamgusys::AddMethod(global, "rgbbackgroundcolor", &Tamgusys::MethodRGBBGCOLORS, P_THREE, "rgbbackgroundcolor(int red, int green, int blue): set rgb background text color");

    Tamgusys::AddMethod(global, "scrollmargin", &Tamgusys::MethodSCROLLMARGIN, P_TWO, "scrollmargin(int top, int bottom): defines margin area");
    Tamgusys::AddMethod(global, "deletechar", &Tamgusys::MethodDELETECHAR, P_ONE, "deletechar(int nb): delete nb char");
    Tamgusys::AddMethod(global, "up", &Tamgusys::MethodUP, P_ONE, "up(int nb): move up nb line");
    Tamgusys::AddMethod(global, "down", &Tamgusys::MethodDOWN, P_ONE, "down(int nb): move down nb line");
    Tamgusys::AddMethod(global, "right", &Tamgusys::MethodRIGHT, P_ONE, "right(int nb): move right nb characters");
    Tamgusys::AddMethod(global, "left", &Tamgusys::MethodLEFT, P_ONE, "left(int nb): move left nb characters");
    Tamgusys::AddMethod(global, "next_line", &Tamgusys::MethodNEXT_LINE, P_ONE, "next_line(int nb): move to nb next line down");
    Tamgusys::AddMethod(global, "previous_line", &Tamgusys::MethodPREVIOUS_LINE, P_ONE, "previous_line(int nb): move nb previous line up");
    Tamgusys::AddMethod(global, "column", &Tamgusys::MethodCOLUMN, P_ONE, "column(int nb): move to column nb");
    Tamgusys::AddMethod(global, "row_column", &Tamgusys::MethodROW_COLUMN, P_TWO, "row_column(int row, int column): move to row/column");
    Tamgusys::AddMethod(global, "home", &Tamgusys::MethodHOME, P_NONE, "home(): move cursor to home");
    Tamgusys::AddMethod(global, "cls", &Tamgusys::MethodCLS, P_NONE, "cls(): clear screen and return to home position");
    Tamgusys::AddMethod(global, "hor_vert", &Tamgusys::MethodHOR_VERT, P_TWO, "hor_vert(int hor, int vert): move to hor/vert");
    Tamgusys::AddMethod(global, "clearscreen", &Tamgusys::MethodCLEARSCREEN, P_ONE, "clearscreen(int nb): nb=0, 1, 2, 3 for partial or full screen clearing");
    Tamgusys::AddMethod(global, "clear", &Tamgusys::MethodCLEAR, P_NONE, "clear(): clear screen ");
    Tamgusys::AddMethod(global, "eraseline", &Tamgusys::MethodERASE_LINE, P_ONE, "eraseline(int nb): nb =0, 1 or 2 for line erasement");
    Tamgusys::AddMethod(global, "scroll_up", &Tamgusys::MethodSCROLL_UP, P_ONE, "scroll_up(int nb): scrolling up nb characters");
    Tamgusys::AddMethod(global, "scroll_down", &Tamgusys::MethodSCROLL_DOWN, P_ONE, "scroll_down(int nb): scrolling down nb characters");
    
    Tamgusys::AddMethod(global, "screensize", &Tamgusys::MethodCoordinates, P_NONE, "screensizes(): return the screen size for row and column");
    Tamgusys::AddMethod(global, "hasresized", &Tamgusys::MethodScreenHasResized, P_NONE, "hasresized(): return 'true', if the screen size has changed");
    Tamgusys::AddMethod(global, "cursor", &Tamgusys::MethodCursorPosition, P_NONE, "cursor(): return the cursor position");
    Tamgusys::AddMethod(global, "inmouse", &Tamgusys::MethodInitMouse, P_NONE, "inmouse(): enable mouse tracking");
    Tamgusys::AddMethod(global, "outmouse", &Tamgusys::MethodCloseMouse, P_NONE, "outmouse(): disable mouse tracking");

    Tamgusys::AddMethod(global, "mousexy", &Tamgusys::MethodPositionMouse, P_ONE, "mousexy(string key): return mouse position");
    Tamgusys::AddMethod(global, "mousescrollup", &Tamgusys::MethodPositionScrollingUp, P_ONE, "mousescrollup(string key): return mouse position when scrolling up");
    Tamgusys::AddMethod(global, "mousescrolldown", &Tamgusys::MethodPositionScrollingDown, P_ONE, "mousescrolldown(string key): return mouse position when scrolling down");
    Tamgusys::AddMethod(global, "mousedown1", &Tamgusys::MethodClickFirstMouseDown, P_ONE, "mousedown1(string key): return mouse position when primary click");
    Tamgusys::AddMethod(global, "mousedown2", &Tamgusys::MethodClickSecondMouseDown, P_ONE, "mousedown2(string key): return mouse position when secondary click");
    Tamgusys::AddMethod(global, "mouseup", &Tamgusys::MethodClickMouseUp, P_ONE, "mouseup(string key): return mouse position when button up");
    Tamgusys::AddMethod(global, "mousetrack", &Tamgusys::MethodMouseTrack, P_ONE, "mousetrack(string key): return mouse position when mouse is moving while pressed");
    Tamgusys::AddMethod(global, "ismouseaction", &Tamgusys::MethodIsActionMouse, P_ONE, "ismouseaction(string key): return true if it is a mouse action");    
    Tamgusys::AddMethod(global, "isescapesequence", &Tamgusys::MethodisEscapeSequence, P_ONE, "isescapesequence(string key): return true if it is an escape sequence");
    
    Tamgusys::AddMethod(global, "showcursor", &Tamgusys::MethodShowCursor, P_ONE, "showcursor(bool show): show or hide the cursor");
    
	Tamgusys::AddMethod(global, "resizecallback", &Tamgusys::MethodResizeCallBack, P_ONE, "resizecallback(function f): set the callback function that is called when the terminal window is resized");
    
	if (version != "") {
        global->newInstance[Tamgusys::idtype] = new Tamgusys(global);
        global->RecordCompatibilities(Tamgusys::idtype);
        
        _mysys = new TamguConstsys(global);
        Tamgu* a = new TamguSystemVariable(global, _mysys, global->Createid("_sys"), Tamgusys::idtype);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyright), global->Createid("_sys_keyright"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyleft), global->Createid("_sys_keyleft"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keydown), global->Createid("_sys_keydown"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyup), global->Createid("_sys_keyup"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keydel), global->Createid("_sys_keydel"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyhomekey), global->Createid("_sys_keyhomekey"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyendkey), global->Createid("_sys_keyendkey"), a_string);
#ifdef WIN32
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyc_homekey), global->Createid("_sys_keyc_homekey"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyc_endkey), global->Createid("_sys_keyc_endkey"), a_string);
#endif
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyc_up), global->Createid("_sys_keyc_up"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyc_down), global->Createid("_sys_keyc_down"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyc_right), global->Createid("_sys_keyc_right"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_keyc_left), global->Createid("_sys_keyc_left"), a_string);
        
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_backspacekey), global->Createid("_sys_keybackspace"), a_string);
        a = new TamguSystemVariable(global, new TamguConstString((char*)sys_escapekey), global->Createid("_sys_keyescape"), a_string);
    }
    return true;
}

void SetEnvironmentVariable(string command, string val) {
#ifdef WIN32
    command += "=";
    command += val;
    _putenv(STR(command));
#else
    setenv(command.c_str(), val.c_str(), 1);
#endif
}

Tamgu* Tamgusys::Put(Tamgu* idx, Tamgu* kval, short idthread) {
    if (idx != NULL && !idx->isConst()) {
        lastcommand = idx->String();
        string val = kval->String();
#ifdef WIN32
        lastcommand += "=";
        lastcommand += val;
        _putenv(STR(lastcommand));
#else
        setenv(lastcommand.c_str(), val.c_str(), 1);
#endif
        
    }
    return aTRUE;
}

Tamgu* Tamgusys::Eval(Tamgu* context, Tamgu* idx, short idthread) {
    if (idx != NULL && !idx->isConst()) {
        lastcommand = idx->String();
        char* rep = Getenv(STR(lastcommand));
        if (rep == NULL)
            return globalTamgu->Providestring("");
        return globalTamgu->Providestring(rep);
    }
    return this;
}

void Reseting_system_environment_for_getchar() {
    _mysys->getcharhasbeenused=false;
#ifndef WIN32
    sendstring(showcursor);
    sendstring(disablemouse);

    struct termios old={0};
    if(tcgetattr(0, &old)<0) {
        perror("tcsetattr()");
    }
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    old.c_cc[VSTART] = _mysys->vstart;
    old.c_cc[VSTOP] = _mysys->vstop;
    old.c_cc[VSUSP] = _mysys->vsusp;
    
    tcsetattr(0, TCSADRAIN, &old);
#endif
}

void Tamgusys::Reset() {
#ifdef WIN32
	if (getcharhasbeenused) {
		ResetWindowsConsole();
		getcharhasbeenused = false;
	}
#else
    sendstring(showcursor);
    if (mouseenabled) {
        sendstring(disablemouse);
        mouseenabled = false;
    }
    
    if (getcharhasbeenused) {
        struct termios old={0};
        if(tcgetattr(0, &old)<0) {
            perror("tcsetattr()");
        }
        old.c_lflag|=ICANON;
        old.c_lflag|=ECHO;
        old.c_cc[VSTART] = vstart;
        old.c_cc[VSTOP] = vstop;
        old.c_cc[VSUSP] = vsusp;
        
        tcsetattr(0, TCSADRAIN, &old);
        getcharhasbeenused = false;
    }
#endif
    lastcommand="";
}

#ifdef WIN32
#define PATH_MAX 4096
#endif

Tamgu* Tamgusys::MethodPopen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //0 is the first parameter and so on...
    Tamgu* kcmd = callfunc->Evaluate(0, aNULL, idthread);
    contextualpattern =  Selectasvector(contextualpattern);
    
    FILE *fp;
    int status;
    
    char path[PATH_MAX];
    
    string cmd = kcmd->String();
    
#ifdef WIN32
    fp = _popen(STR(cmd), "r");
#else
    fp = popen(STR(cmd), "r");
    
#endif
    if (fp == NULL)
        return globalTamgu->Returnerror("Error while opening 'popen' stream");
    
    while (fgets(path, PATH_MAX, fp) != NULL) {
        cmd  = path;
        contextualpattern->storevalue(cmd);
    }
    
    
#ifdef WIN32
    status = _pclose(fp);
#else
    status = pclose(fp);
#endif
    
    if (status == -1)
        return globalTamgu->Returnerror("Error while closing 'popen' stream");
    
    
    return contextualpattern;
}

Tamgu* Tamgusys::MethodCommand(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //0 is the first parameter and so on...
    Tamgu* kcmd = callfunc->Evaluate(0, aNULL, idthread);
    string filename;
    if (callfunc->Size() == 2)
        filename = callfunc->Evaluate(1, aNULL, idthread)->String();
    lastcommand = kcmd->String();
    if (lastcommand != "") {
        if (filename != "") {
            int o = DUP(1);
            FILE* stream = fopen(STR(filename), "w");
            if (stream == NULL)
                return globalTamgu->Returnerror("SYS(001): Cannot open output file", idthread);
            DUP2(FILENO(stream), 1);
            system(lastcommand.c_str());
            fflush(stdout);
            fclose(stream);
            DUP2(o, 1);
            stream = fopen(STR(filename), "r");
            string content;
            char buffer[2049];
            size_t nb;
            while (!feof(stream)) {
                nb = fread(buffer, 1, 2048, stream);
                buffer[nb] = 0;
                content += buffer;
            }
            fclose(stream);
            return globalTamgu->Providewithstring(content);
        }
        else {
            int res = system(lastcommand.c_str());
            return globalTamgu->ProvideConstint(res);
        }
    }
    //you may return any value of course...
    return aTRUE;
}

Tamgu* Tamgusys::MethodisDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string dirName_in = callfunc->Evaluate(0, aNULL, idthread)->String();
#ifdef WIN32
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return aFALSE;  //something is wrong with your path!
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return aTRUE;   // this is a directory!
#else
    struct stat st;
    if (stat(STR(dirName_in), &st) == 0)
        if ((st.st_mode & S_IFMT) == S_IFDIR)
            return aTRUE;
#endif
    return aFALSE;    // this is not a directory!
}

Tamgu* Tamgusys::MethodCreateDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    string path = kpath->String();
    if (DirectoryCreation(STR(path), 0) == false)
        return aFALSE;
    return aTRUE;
}

Tamgu* Tamgusys::MethodListDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    string path = kpath->String();
    directorylisting dir(path);
    bool go = dir.initial();
    
    if (contextualpattern->isNumber()) {
        long i = 0;
        if (go) {
            while (dir.getnext()) {
                i++;
            }
        }
        return globalTamgu->ProvideConstint(i);
    }
    
    Tamgu* kvect = Selectasvector(contextualpattern);
    
    if (!go)
        return kvect;
    
    string name;
    while (dir.getnext(name)) {
        if (kvect->Type() == a_svector)
            ((Tamgusvector*)kvect)->values.push_back(name);
        else
            kvect->Push(globalTamgu->Providewithstring(name));
    }
    return kvect;
}

Tamgu* Tamgusys::MethodRealPath(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    char localpath[4096];
    string path = kpath->String();
#ifdef WIN32
    _fullpath(localpath, STR(path), 4096);
#else
    realpath(STR(path), localpath);
#endif
    return globalTamgu->Providestring(localpath);
}

Tamgu* Tamgusys::MethodFileInfo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* kpath = callfunc->Evaluate(0, aNULL, idthread);
    string filename = kpath->String();
#ifdef WIN32
    struct _stat scible;
    int stcible = _stat(STR(filename), &scible);
#else
    struct stat scible;
    int stcible = stat(STR(filename), &scible);
#endif
    if (stcible >= 0) {
        Tamgu* size = globalTamgu->ProvideConstint(scible.st_size);
        Tamgu* change = new Tamgudate(scible.st_mtime);
        Tamgu* adate = new Tamgudate(scible.st_atime);
        Tamgu* cdate = new Tamgudate(scible.st_ctime);
        kpath = globalTamgu->Providewithstring(filename);
        Tamgumap* mp = (Tamgumap*)Selectamap(contextualpattern);
        mp->Push("size", size);
        mp->Push("date", change);
        mp->Push("access", adate);
        mp->Push("change", cdate);
        mp->Push("pathname", kpath);
        if ((scible.st_mode & S_IFMT) == S_IFDIR)
            mp->Push("directory", aTRUE);
        else
            mp->Push("directory", aFALSE);
        return mp;
    }
    return aNULL;
}

Tamgu* Tamgusys::MethodEnv(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //0 is the first parameter and so on...
    Tamgu* kname = callfunc->Evaluate(0, aNULL, idthread);
    lastcommand = kname->String();
    if (callfunc->Size() == 1) {
        if (lastcommand != "") {
            char* rep = Getenv(STR(lastcommand));
            if (rep == NULL)
                return globalTamgu->Providestring("");
            return globalTamgu->Providestring(rep);
        }
        //you may not return any value of course...
        return aNULL;
    }
    else {
        Tamgu* kvalue = callfunc->Evaluate(1, aNULL, idthread);
        if (lastcommand != "") {
            string val = kvalue->String();
#ifdef WIN32
            lastcommand += "=";
            lastcommand += val;
            _putenv(STR(lastcommand));
#else
            setenv(lastcommand.c_str(), val.c_str(), 1);
#endif
            return aTRUE;
        }
    }
    //you may return any value of course...
    return aFALSE;
}

#ifdef WIN32
Tamgu* Tamgusys::MethodGETCH(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return globalTamgu->Providestring(getcharacter());
}
#else
Tamgu* Tamgusys::MethodGETCH(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!getcharhasbeenused) {
        struct termios old={0};
        if(tcgetattr(0, &old)<0) {
            perror("tcsetattr()");
            return globalTamgu->Returnerror("tcsetattr ICANON", idthread);
        }
        
        old.c_lflag&=~ICANON;
        old.c_lflag&=~ECHO;
        old.c_cc[VMIN]=1;
        old.c_cc[VTIME]=0;
        
        vstart = old.c_cc[VSTART];
        vstop = old.c_cc[VSTOP];
        vsusp = old.c_cc[VSUSP];
        
        old.c_cc[VSTART] = 0;
        old.c_cc[VSTOP] = 0;
        old.c_cc[VSUSP] = 0;
        
        if(tcsetattr(0, TCSANOW, &old)<0) {
            perror("tcsetattr ICANON");
            return globalTamgu->Returnerror("tcsetattr ICANON", idthread);
        }
        getcharhasbeenused = true;
        //We might be after a reset
        if (mouseenabled) {
            sendstring(enablemouse);
        }
    }
    
    return globalTamgu->Providestring(getcharacter());
}
#endif

Tamgu* Tamgusys::MethodCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[16] ;
    long v;
    memcpy(buff, sys_colors,15);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    v = callfunc->Evaluate(1, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[6] = n999[v][0];
    buff[7] = n999[v][1];
    buff[8] = n999[v][2];
    v = callfunc->Evaluate(2, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[10] = n999[v][0];
    buff[11] = n999[v][1];
    buff[12] = n999[v][2];
    bool disp = true;
    if (callfunc->Size()==4)
        disp = callfunc->Evaluate(3, aNULL, idthread)->Boolean();
    if (disp)
        cout << buff;
    return globalTamgu->Providestring(buff);
}

Tamgu* Tamgusys::MethodFGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[13];
    memcpy(buff, sys_foreground_colors, 11);
    long v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[7] = n999[v][0];
    buff[8] = n999[v][1];
    buff[9] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodBGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[13];
    memcpy(buff, sys_background_colors, 11);
    long v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[7] = n999[v][0];
    buff[8] = n999[v][1];
    buff[9] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodRGBFGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[21] ;
    long v;
    memcpy(buff, sys_rgb_foreground_colors,20);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[7] = n999[v][0];
    buff[8] = n999[v][1];
    buff[9] = n999[v][2];
    v = callfunc->Evaluate(1, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[12] = n999[v][0];
    buff[13] = n999[v][1];
    buff[14] = n999[v][2];
    v = callfunc->Evaluate(2, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[16] = n999[v][0];
    buff[17] = n999[v][1];
    buff[18] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodRGBBGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[21] ;
    long v;
    memcpy(buff, sys_rgb_background_colors,20);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[7] = n999[v][0];
    buff[8] = n999[v][1];
    buff[9] = n999[v][2];
    v = callfunc->Evaluate(1, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[12] = n999[v][0];
    buff[13] = n999[v][1];
    buff[14] = n999[v][2];
    v = callfunc->Evaluate(2, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[16] = n999[v][0];
    buff[17] = n999[v][1];
    buff[18] = n999[v][2];
    cout << buff;
    return aTRUE;
}


Tamgu* Tamgusys::MethodSCROLLMARGIN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[12] ;
    long v;
    memcpy(buff, sys_scrollmargin,11);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    v = callfunc->Evaluate(1, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[6] = n999[v][0];
    buff[7] = n999[v][1];
    buff[8] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodDELETECHAR(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_deletechar,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodUP(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_up,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodDOWN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_down,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodRIGHT(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_right,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodLEFT(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_left,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodNEXT_LINE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_next_line,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodPREVIOUS_LINE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_previous_line,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodCOLUMN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_column,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodROW_COLUMN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[12] ;
    long v;
    memcpy(buff, sys_row_column,11);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    v = callfunc->Evaluate(1, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[6] = n999[v][0];
    buff[7] = n999[v][1];
    buff[8] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodHOME(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    cout << sys_home;
    return aTRUE;
}

Tamgu* Tamgusys::MethodCLS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    _clearscreen();
    return aTRUE;
}

Tamgu* Tamgusys::MethodHOR_VERT(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[12] ;
    long v;
    memcpy(buff, sys_hor_vert,11);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    v = callfunc->Evaluate(1, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[6] = n999[v][0];
    buff[7] = n999[v][1];
    buff[8] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodCLEARSCREEN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_partial_clear,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    if (v > 4 || v  < 0)
        v = 0;
    buff[2] = v+48;
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodCLEAR(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    cout << sys_clear << sys_clear_scrolling << sys_home;
    return aTRUE;
}

Tamgu* Tamgusys::MethodERASE_LINE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_partial_line_clear,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    if (v > 3 || v  < 0)
        v = 0;
    buff[2] = v+48;
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodSCROLL_UP(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_scroll_up,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodSCROLL_DOWN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    char buff[8] ;
    long v;
    memcpy(buff, sys_scroll_down,7);
    v = callfunc->Evaluate(0, aNULL, idthread)->Integer();
    v = maxlocal(v,0);
    v = minlocal(v,999);
    buff[2] = n999[v][0];
    buff[3] = n999[v][1];
    buff[4] = n999[v][2];
    cout << buff;
    return aTRUE;
}

Tamgu* Tamgusys::MethodCoordinates(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguivector* iv = (Tamguivector*)Selectaivector(contextualpattern);
    iv->values.push_back(row_size);
    iv->values.push_back(col_size);
    return iv;
}

Tamgu* Tamgusys::MethodScreenHasResized(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (checkresize())
        return aTRUE;
    return aFALSE;
}

Tamgu* Tamgusys::MethodCursorPosition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //If you need to get the absolute cursor position, you can decomment these lines
    int xcursor = 0;
    int ycursor = 0;
    getcursor(xcursor,ycursor);
    Tamgu* vect = Selectaivector(contextualpattern);
    vect->storevalue((long)xcursor);
    vect->storevalue((long)ycursor);
    return vect;
}

Tamgu* Tamgusys::MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Reset();
    return aTRUE;
}

Tamgu* Tamgusys::MethodInitMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (mouseenabled)
        return aFALSE;
#ifdef WIN32
	tracking = false;
#else
	sendstring(enablemouse);
#endif
    mouseenabled = true;
    return aTRUE;
}

Tamgu* Tamgusys::MethodCloseMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!mouseenabled)
        return aFALSE;
#ifdef WIN32
	tracking = false;
	SetConsoleMode(hStdin, fdwSaveOldMode);
#else
    sendstring(disablemouse);
#endif
    mouseenabled = false;
    return aTRUE;
}

Tamgu* Tamgusys::MethodPositionMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    int action, mxcursor, mycursor;
    Tamgu* vect = Selectaivector(contextualpattern);
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
        //This a move
        sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
        if (action == 67) {
            vect->storevalue((long)mxcursor);
            vect->storevalue((long)mycursor);
        }
    }
    return vect;
}

Tamgu* Tamgusys::MethodPositionScrollingUp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    int action, mxcursor, mycursor;
    Tamgu* vect = Selectaivector(contextualpattern);
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
        //This a move
        sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
        if (action == 96) {
            vect->storevalue((long)mxcursor);
            vect->storevalue((long)mycursor);
        }
    }
    return vect;
}

Tamgu* Tamgusys::MethodPositionScrollingDown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    int action, mxcursor, mycursor;
    Tamgu* vect = Selectaivector(contextualpattern);
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
        //This a move
        sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
        if (action == 97) {
            vect->storevalue((long)mxcursor);
            vect->storevalue((long)mycursor);
        }
    }
    return vect;
}

Tamgu* Tamgusys::MethodClickFirstMouseDown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    int action, mxcursor, mycursor;
    Tamgu* vect = Selectaivector(contextualpattern);
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
        //This a move
        sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
        if (action == 32) {
            vect->storevalue((long)mxcursor);
            vect->storevalue((long)mycursor);
        }
    }
    return vect;
}

Tamgu* Tamgusys::MethodClickSecondMouseDown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    int action, mxcursor, mycursor;
    Tamgu* vect = Selectaivector(contextualpattern);
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
        //This a move
        sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
        if (action == 34) {
            vect->storevalue((long)mxcursor);
            vect->storevalue((long)mycursor);
        }
    }
    return vect;
}

Tamgu* Tamgusys::MethodClickMouseUp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    int action, mxcursor, mycursor;
    Tamgu* vect = Selectaivector(contextualpattern);
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
        //This a move
        sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
        if (action == 35) {
            vect->storevalue((long)mxcursor);
            vect->storevalue((long)mycursor);
        }
    }
    return vect;
}

Tamgu* Tamgusys::MethodMouseTrack(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    int action, mxcursor, mycursor;
    Tamgu* vect = Selectaivector(contextualpattern);
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[') {
        //This a move
        sscanf(STR(mousectrl), "\033[%d;%d;%dM", &action, &mycursor, &mxcursor);
        if (action == 64) {
            vect->storevalue((long)mxcursor);
            vect->storevalue((long)mycursor);
        }
    }
    return vect;
}

Tamgu* Tamgusys::MethodIsActionMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    if (mousectrl.size() >= 8 && mousectrl.back() == 'M' && mousectrl[0] == 27 && mousectrl[1] == '[')
        return aTRUE;
    return aFALSE;
}


Tamgu* Tamgusys::MethodisEscapeSequence(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string mousectrl = callfunc->Evaluate(0, aNULL, idthread)->String();
    if (mousectrl.size() >= 8 && mousectrl[0] == 27 && mousectrl[1] == '[')
        return aTRUE;
    return aFALSE;
}

Tamgu* Tamgusys::MethodShowCursor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    bool cursor = callfunc->Evaluate(0, aNULL, idthread)->Boolean();
    if (cursor)
        sendstring(showcursor);
    else
        sendstring(hidecursor);

    return aTRUE;
}


Tamgu* Tamgusys::MethodResizeCallBack(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Tamgu* func = callfunc->Evaluate(0, aNULL, idthread);
	if (func == aNULL) {
		function = NULL;
#ifndef WIN32
		signal(SIGWINCH, NULL);
#endif
		return aTRUE;
	}

	if (!func->isFunction() || func->Size() != 2)
		globalTamgu->Returnerror("Expecting a function of arity: 2", idthread);
	function = func;
#ifndef WIN32
	signal(SIGWINCH, callresize);
#endif
	return aTRUE;
}


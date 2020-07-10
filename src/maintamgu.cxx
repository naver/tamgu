/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : maintamgu.cxx
 Date       : 2017/07/11
 Purpose    : Main of the Tamgu executable
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifdef WIN32
#include <windows.h>
#endif

#include "tamgu.h"
#include "tamguversion.h"
#include "globaltamgu.h"
#include "tamgustring.h"
#include "tamgudebug.h"
#include "instructions.h"
#include "tamgulisp.h"
#include "tamgusys.h"

#ifdef WIN32
#define PATH_MAX 4096
#include <conio.h>
#endif
#include <stdio.h>

#ifdef UNIX
#ifndef APPLE
#include <unistd.h>
#include <linux/limits.h>
#endif
#endif

#include <iomanip>

#ifdef WIN32
char* Getenv(char* name);
#else
#define Getenv getenv
#endif

//------------------------------------------------------------------------
static string _variable_declaration("bool a,b,c; date d; int i,j,k; float f,g,h; string s,t,u; map m; vector v; self x,y,z;");
static wstring _wvariable_declaration(L"bool a,b,c; date d; int i,j,k; float f,g,h; string s,t,u; map m; vector v; self x,y,z;");
//------------------------------------------------------------------------
void Reseting_system_environment_for_getchar();
static short current_thread_id;

void RetrieveThroughVariables(string& declaration);
void RetrieveThroughVariables(wstring& decl);
bool CheckThroughVariables();
Tamgu* ProcEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

static bool lispmode = false;

#ifdef WITHCONSOLE
extern "C" {
    Exporting bool TamguLaunching();
    Exporting bool window_editor(string filename);
    Exporting bool window_editor_code(string filename, string& code);
}
#else
void TamguLaunching() {
    TamguCreate();
}
#endif

static vector<string> arguments;


#ifdef UNIX
#ifndef APPLE
string get_exe_path( )
{
    char arg1[20];
    char exepath[PATH_MAX + 1] = {0};
    
    sprintf( arg1, "/proc/%d/exe", getpid() );
    readlink( arg1, exepath, sizeof(exepath) );
    return string( exepath );
}
#endif
#endif

string Normalizefilename(string path) {
    char localpath[4096];
    #ifdef WIN32
    _fullpath(localpath, STR(path), 4096);
    #else
    realpath(STR(path), localpath);
    #endif
    return localpath;
}

#ifdef WIN32
static char THEMAIN[] = "C:\\MAIN\0";
#else
//The specific version for Unix platforms that implement a complete character editor
static char THEMAIN[] = "/MAIN\0";

#include <unistd.h>   //_getch
#include <termios.h>  //_getch
#include <sys/ioctl.h>
#endif

#include <signal.h>
#include <iomanip>
#include "jag.h"

static void displayhelp(string wh) {
    cout << endl << STR(wh) << Endl << Endl;
    #ifdef WITHCONSOLE
    cout << "-console ('filename'): open the console editor with an optional filename" << endl << endl;
    #endif
#ifndef WIN321
    cout << "-e 'filename' load filename into the terminal console in edit mode" << endl << endl;
    cout << "-l 'filename' load filename into the terminal console" << endl << endl;
    cout << "-lisp activate lisp mode" << endl << endl;
#endif
    cout << "-a 'stdin data' available in tamgu via _args" << endl;
    cout << "-a 'source code' with _args fully piped" << endl  << endl;
    cout << "-c 'source code' without piped data" << endl << endl;
    cout << "-p 'source code' with _args piped one string at a time" << endl;
    cout << "   for code without a ';' such as -p 'l[:\".\"]', a 'println' is automatically inserted." << endl;
    cout << "   Hence: -p '\"touch\", l[:\".\"]' is a valid expression." << endl;
    cout << "   It is rewritten: println(\"touch\", l[:\".\"]);" << endl;
    cout << "-pb 'source code' inserted before the code with -p" << endl;
    cout << "-pe 'source code' added after the code with -p" << endl;
    cout << "-p: Pre-declared variables:" << endl;
    cout << "\tl: current line from stdin (also l0)" << endl;
    cout << "\t_args: split the current line along spaces and store each field separatly. _args[0] is the full line" << endl;
    cout << "\t_line: current line number (starts at 1)" << endl;
    cout << "\t_size: number of fields (_args size) in a line from stdin" << endl;
    cout << "\tl1-l99: each variable matches a field in a line from stdin (ln == _args[n])" << endl << endl;
    cout << "-i Predeclared variables:" << endl << endl;
    cout << "\t";
    cout << "_args: argument vector" << endl;
    cout << "\t";
    cout << "_paths: _paths[0] is the current directory" << endl;
    cout << "\t";
    cout << "a,b,c: bool" << endl;
    cout << "\t";
    cout << "d: date" << endl;
    cout << "\t";
    cout << "i,j,k: int" << endl;
    cout << "\t";
    cout << "f,g,h: float" << endl;
    cout << "\t";
    cout << "s,t,u: string" << endl;
    cout << "\t";
    cout << "m: map" << endl;
    cout << "\t";
    cout << "v: vector" << endl;
    cout << "\t";
    cout << "x,y,z: self" << endl;
    cout << endl << endl;
    exit(-1);
}

///------------------------------------------------------------------------------------
static Debuginfo debuginfo;
class tamgu_editor;

Tamgu* debuginfo_callback(vector<Tamgu*>& stack, short idthread, void* data);
void debuggerthread(tamgu_editor* call);

///------------------------------------------------------------------------------------
#ifdef WIN32
BOOL WINAPI handle_ctrl_c(_In_ DWORD dwCtrlType);
#else
static void handle_ctrl_c(int theSignal);
#endif

///------------------------------------------------------------------------------------
static void displaychar(string& bf) {
    for (int i=0; i < bf.size(); i++)
        cout << (int)(uchar)bf[i] << " ";
    cout << endl;
}
//------------------------------------------------------------------------
static string _metacharacters = "× ÷ ← → ¬ ≠ ∨ ∧ π τ ℯ φ ∏ ∑ √ ∛ ² ³";
//------------------------------------------------------------------------
class tamgu_editor : public jag_editor {
    hmap<string, short> filenames;
    vector<string> ifilenames;
    vector<wstring> codes;
    
    vector<editor_keep> editors_undos;
    vector<editor_keep> editors_redos;
    vector<long> lastlines;

    vector<string> displaying;
    
    string historyfilename;
    wstring curlyspace;

    long currentfileid;
    long curlypos;
    long nbcurly;
    long pcursor;
    long idCode;
    long lastline;
    
    bool editmode;
    

    
public:
    LockedThread editor_loquet;
    bool debugmode;

    tamgu_editor() {
        currentfileid = -1;
        editmode = false;
        curlypos = -1;
        nbcurly = 0;
        pcursor = -1;
        curlyspace = L"";
        debugmode = false;
        idCode = -1;
        lastline = 0;
    }
    
    bool emode() {
        if (editmode && option == x_none)
            return true;
        return false;
    }
    
    void displaythehelp(long i) {
        cout << m_clear << m_home;
        cerr << endl << m_redbold << "Commands:" << m_current << endl << endl;
        
        if (!i || i == 1) {
            cerr << "   - " << m_redbold << "1. Programs:" << m_current << endl;
            cerr << "   \t- " << m_redbold << "create filename:" << m_current << " create a file space with a specific file name" << endl;
            cerr << "   \t- " << m_redbold << "open filename:" << m_current << " load a program (use "<< m_redital << "run" << m_current <<" to execute it)" << endl;
            cerr << "   \t- " << m_redbold << "open:" << m_current << " reload the program (use "<< m_redital << "run" << m_current <<" to execute it)" << endl;
            cerr << "   \t- " << m_redbold << "save filename:" << m_current << " save the buffer content in a file" << endl;
            cerr << "   \t- " << m_redbold << "save:" << m_current << " save the buffer content with the current filename" << endl;
            cerr << "   \t- " << m_redbold << "run filename:" << m_current << " load and run a program filename" << endl;
            cerr << "   \t- " << m_redbold << "run:" << m_current << " execute the buffer content" << endl;
            cerr << "   \t- " << m_redbold << "lisp:" << m_current << " (de)activate the lisp mode" << endl;
            cerr << "   \t- " << m_redbold << "debug:" << m_current << " debug mode" << endl;
            cerr << "   \t\t- " << m_redbold << "n!:" << m_current << " next line" << endl;
            cerr << "   \t\t- " << m_redbold << "l!:" << m_current << " display local variables" << endl;
            cerr << "   \t\t- " << m_redbold << "a!:" << m_current << " display all active variables" << endl;
            cerr << "   \t\t- " << m_redbold << "s!:" << m_current << " display the stack" << endl;
            cerr << "   \t\t- " << m_redbold << "g!:" << m_current << " go to next breakpoint" << endl;
            cerr << "   \t\t- " << m_redbold << "e!:" << m_current << " execute up to the end" << endl;
            cerr << "   \t\t- " << m_redbold << "S!:" << m_current << " stop the execution" << endl;
            cerr << "   \t\t- " << m_redbold << "name:" << m_current << " display the content of a variable (just type its name)" << endl;
            cerr << "   \t\t- " << m_redbold << "h!:" << m_current << " display debug help" << endl;
            cerr << "   \t- " << m_redbold << "args v1 v2...vn:" << m_current << " set a list of values to the argument list" << endl;
            cerr << "   \t- " << m_redbold << "filename:" << m_current << " display the current filename" << endl << endl;
        }
        
        if (!i || i == 2) {
            cerr << "   - " << m_redbold << "2. Command line mode:" << m_current << endl;
            cerr << "   \t- " << m_redbold << "help:" << m_current << " display the help" << endl;
            cerr << "   \t- " << m_redbold << "help n:" << m_current << " display one of the help sections (from 1 to 5)" << endl;
            cerr << "   \t- " << m_redbold << "history:" << m_current << " display the command history" << endl;
            cerr << "   \t- " << m_redbold << "load filename:" << m_current << " load the command history from a file" << endl;
            cerr << "   \t- " << m_redbold << "store filename:" << m_current << " store the command history in a file" << endl;
            cerr << "   \t- " << m_redbold << "filename:" << m_current << " display the current file name" << endl;
            cerr << "   \t- " << m_redbold << "spaces:" << m_current << " display all the files stored in memory with their file space id" << endl;
            cerr << "   \t- " << m_redbold << "select space:" << m_current << " select a file space" << endl;
            cerr << "   \t- " << m_redbold << "create filename:" << m_current << " create a new empty file space" << endl;
            cerr << "   \t- " << m_redbold << "metas:" << m_current << " display meta-characters" << endl;
            cerr << "   \t- " << m_redbold << "rm:" << m_current << " clear the buffer content" << endl;
            cerr << "   \t- " << m_redbold << "rm b:e:" << m_current << " remove the lines from b to e (b: or :e is also possible)" << endl;
            cerr << "   \t- " << m_redbold << "list:" << m_current << " list the buffer content" << endl;
            cerr << "   \t- " << m_redbold << "list b:e:" << m_current << " display the buffer content from line b to e (b: or :e is also possible)" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-t:" << m_current << " up in the code listing" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-g:" << m_current << " down in the code listing" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-f:" << m_current << " force the current line to be appended at the end of the code" << endl;
            cerr << "   \t- " << m_redbold << "?method:" << m_current << " return a description for 'method'" << m_current << endl;
            cerr << "   \t- " << m_redbold << "?o.method:" << m_current << " return a description for 'method' for variable 'o'" << m_current << endl << endl;
        }
        
        if (!i || i == 3) {
            cerr << "   - " << m_redbold << "3. edit (space):" << m_current << " edit mode. You can optionally select also a file space" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-b:" << m_current << " toggle breakpoint" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-k:" << m_current << " delete from cursor up to the end of the line" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-d:" << m_current << " delete a full line" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-u:" << m_current << " undo last modification" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-r:" << m_current << " redo last modification" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-f:" << m_current << " find a string" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-n:" << m_current << " find next" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-g:" << m_current << " move to a specific line, '$' is the end of the code" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-l:" << m_current << " toggle between top and bottom of the screen" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-t:" << m_current << " reindent the code" << endl;
#ifdef WIN32
			cerr << "   \t- " << m_redbold << "Ctrl+Alt-h:" << m_current << " local help" << endl;
#else
			cerr << "   \t- " << m_redbold << "Ctrl-h:" << m_current << " local help" << endl;
#endif
			cerr << "   \t- " << m_redbold << "Ctrl-w:" << m_current << " write file to disk" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-c:" << m_current << " exit the editor" << endl << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-x:" << m_redital << " Combined Commands" << m_current << endl;
            cerr << "   \t\t- " << m_redital << "C:" << m_current << " count a pattern" << endl;
            cerr << "   \t\t- " << m_redital << "H:" << m_current << " convert HTML entities to Unicode characters" << endl;
            cerr << "   \t\t- " << m_redital << "D:" << m_current << " delete a bloc of lines" << endl;
            cerr << "   \t\t- " << m_redital << "n:" << m_current << " hide/display line numbers" << endl;
            cerr << "   \t\t- " << m_redital << "c:" << m_current << " copy a bloc of lines" << endl;
            cerr << "   \t\t- " << m_redital << "x:" << m_current << " cut a bloc of lines" << endl;
            cerr << "   \t\t- " << m_redital << "v:" << m_current << " paste a bloc of lines" << endl;
            cerr << "   \t\t- " << m_redital << "d:" << m_current << " run in debug mode" << endl;
            cerr << "   \t\t- " << m_redital << "r:" << m_current << " run the code" << endl;
            cerr << "   \t\t- " << m_redital << "w:" << m_current << " write and quit" << endl;
            cerr << "   \t\t- " << m_redital << "l:" << m_current << " load a file" << endl;
            cerr << "   \t\t- " << m_redital << "m:" << m_current << " display meta-characters" << endl;
            cerr << "   \t\t- " << m_redital << "h:" << m_current << " full help" << endl;
            cerr << "   \t\t- " << m_redital << "q:" << m_current << " quit" << endl << endl;
        }
        
        if (!i || i == 4) {
            cerr << "   - " << m_redbold << "4. Environment:" << m_current << endl;
#ifdef WITHCONSOLE
            cerr << "   \t- " << m_redbold << "console:" << m_current << " launch the GUI" << endl;
#endif
            cerr << "   \t- " << m_redbold << "cls:" << m_current << " clear screen" << endl;
            cerr << "   \t- " << m_redbold << "color:" << m_current << " display terminal colors" << endl;
            cerr << "   \t- " << m_redbold << "color att fg bg:" << m_current << " display a color out of attribute, foreground, background" << endl;
            cerr << "   \t- " << m_redbold << "colors:" << m_current << " display the colors for code coloring" << endl;
            cerr << "   \t- " << m_redbold << "colors name att fg bg:" << m_current << " modify the color for one of the following denominations = ";
            cerr << m_redital  << "strings, methods, keywords, functions, comments" << m_current << endl << endl;
        }
        
        if (!i || i == 5) {
            cerr << "   - " << m_redbold << "5. System:" << m_current << endl;
            cerr << "   \t- " << m_redbold << "!unix:" << m_current << " what follows the " << m_redital << "'!'" << m_current << " will be executed as a Unix command (ex: "<< m_redital << "!ls" << m_current << ")" << endl;
            cerr << "   \t- " << m_redbold << "!vs=unix:" << m_current << " what follows the " << m_redital << "'='" << m_current << " will be executed as a Unix command (vs must be a svector:"
                 << m_redital << "!vs=ls" << m_current << ")" << endl;
            cerr << "   \t- " << m_redbold << "clear (space):" << m_current << " clear the current environment or a specifc file space" << endl;
            cerr << "   \t- " << m_redbold << "reinit:" << m_current << " clear the buffer content and initialize predeclared variables" << endl;
            cerr << "   \t- " << m_redbold << "Ctrl-d:" << m_current << " end the session and exit tamgu" << endl << endl;
            cerr << "   \t- " << m_redbold << "exit:" << m_current << " end the session and exit tamgu" << endl << endl;
        }
        
        if (!i || i == 6) {
            cerr << "   " << m_redbold << "6. Regular expressions for " << m_redital << "'find'" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%d" << m_current <<"\t\tstands for any digit" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%x" << m_current <<"\t\tstands for a hexadecimal digit (abcdef0123456789ABCDEF)" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%p" << m_current <<"\t\tstands for any punctuation" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%c" << m_current <<"\t\tstands for any lower case letter" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%C" << m_current <<"\t\tstands for any upper case letter" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%a" << m_current <<"\t\tstands for any letter" << m_current << endl;
            cerr << "   \t- " << m_redbold << "?" << m_current <<"\t\tstands for any character" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%?" << m_current <<"\t\tstand for the character “?” itself" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%%" << m_current <<"\t\tstand for the character “%” itself" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%s" << m_current <<"\t\tstand for any space character include the non-breaking space" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%r" << m_current <<"\t\tstand a carriage return" << m_current << endl;
            cerr << "   \t- " << m_redbold << "%n" << m_current <<"\t\tstand for a non-breaking space" << m_current << endl;
            cerr << "   \t- " << m_redbold << "~" << m_current <<"\t\tnegation" << m_current << endl;
            cerr << "   \t- " << m_redbold << "\\x" << m_current <<"\t\tescape character" << m_current << endl;
            cerr << "   \t- " << m_redbold << "\\ddd" << m_current <<"\t\tcharacter code across 3 digits exactly" << m_current << endl;
            cerr << "   \t- " << m_redbold << "\\xFFFF" << m_current <<"\tcharacter code across 4 hexas exactly" << m_current << endl;
            cerr << "   \t- " << m_redbold << "{a-z}" << m_current <<"\t\tbetween a and z included" << m_current << endl;
            cerr << "   \t- " << m_redbold << "[a-z]" << m_current <<"\t\tsequence of characters" << m_current << endl;
            cerr << "   \t- " << m_redbold << "^" << m_current <<"\t\tthe expression should start at the beginning of the string" << m_current << endl;
            cerr << "   \t- " << m_redbold << "$" << m_current <<"\t\tthe expression should match up to the end of the string" << m_current << endl << endl;
            
            cerr << "   " << m_redbold << "Examples:" << m_current << endl;
            cerr << "   \t- " << m_redbold << "dog%c    matches dogs or dogg" << m_current << endl;
            cerr << "   \t- " << m_redbold << "m%d      matches m0, m1,…,m9" << m_current << endl;
            cerr << "   \t- " << m_redbold << "{%dab}   matches 1, a, 2, b" << m_current << endl;
            cerr << "   \t- " << m_redbold << "{%dab}+  matches 1111a, a22a90ab" << m_current << endl;
        }
        
        cerr << endl;
    }

    //------------------------------------------------------------------------------------------------
    //Cursor movements...
    //------------------------------------------------------------------------------------------------
    void printdebug(long pos) {
        clearscreen();
        cout << back << m_dore << "▶▶" << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[pos] << "> " << m_current << coloringline(lines[pos]) << endl;
        pos++;
        short nb = 0;
        while (nb !=  25 && pos < (lines.size() - 1)) {
            cout << back << m_dore << "▶▶" << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[pos] << "> " << m_current << coloringline(lines[pos]) << endl;
            ++pos;
            ++nb;
        }
        
        if (displaying.size()) {
            cout << endl;
            vector<long> errors;
            long i;
            for (i = 0; i < displaying.size(); i++) {
                if (!evallocalcode(displaying[i], true))
                    errors.push_back(i);
            }
            
            for (i = errors.size()-1; i>= 0; i--) {
                displaying.erase(displaying.begin()+errors[i]);
            }
            cout << endl;
        }
        
        cout << back << m_dore << "▶▶" << m_current;
        cout.flush();
    }
    
    void printline(long n, string l) {
        if (debugmode && debuginfo.running)
            cout << back << m_dore << prefix << m_current << l;
        else {
            if (!emode()) {
                if (prefixesize(n) > prefixsize)
                    setprefixesize(n);
            }
            if (noprefix)
                cout << back << l;
            else
                cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << n << "> " << m_current << l;
        }
    }
    
    void printline(long n) {
        if (debugmode && debuginfo.running)
            cout << back << m_dore << prefix << m_current;
        else {
            if (!emode()) {
                if (prefixesize(n) > prefixsize)
                    setprefixesize(n);
            }
            if (noprefix)
                cout << back;
            else
                cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << n << "> " << m_current;
        }
    }

    void printline(long n, wstring& l, long i = -1) {
        if (debugmode && debuginfo.running)
            cout << back << m_dore << prefix << m_current << convert(l);
        else {
            if (noprefix)
                cout << back << coloringline(l, i);
            else
                cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << n << "> " << m_current << coloringline(l, i);
        }
    }

    //this is the method to handle up and down key strokes
    bool updown(char drt, long& pos) {
        if (emode())
            return jag_editor::updown(drt, pos);
        
        if (curlypos == -1) {
            if (drt == is_up) {
                if (poscommand > 0 && poscommand <= commandlines.size()) {
                    clearline();
                    line = commandlines[--poscommand];
                    posinstring = linesize();
                    printline(pos+1, line);
                }
                return false;
            }
            
            clearline();
            long i = commandlines.size();
            if (poscommand >= 0 && poscommand < i - 1) {
                line = commandlines[++poscommand];
                posinstring = linesize();
                printline(pos+1, line);
            }
            else {
                line = L"";
                poscommand = i;
                posinstring = 0;
                printline(pos+1);
            }
            return false;
        }

        if (drt == is_up) { // we are going up
            if (pos > curlypos) {
                --pos;
                cout << m_up;
                line = lines[pos];
                posinstring = linesize();
                movetoposition();
                return true;
            }
            return false;
        }
        
        if (pos < (lines.size() - 1)) {
            ++pos;
            cout << m_down;
            line = lines[pos];
            posinstring = linesize();
            movetoposition();
            return true;
        }
        
        return false;
    }

    void displaygo(bool full) {
        if (option != x_none) {
            jag_editor::displaygo(full);
            return;
        }

        if (full) {
            if (editmode) {
                if (lines.status[pos] == concat_line) {
                    if (noprefix)
                        cout << back << coloringline(line, pos);
                    else {
                        string space(prefixe(), ' ');
                        cout << back << space << coloringline(line, pos);
                    }
                }
                else {
                    if (!lines.check(pos) || noprefix)
                        printline(lines.numeros[pos], line, pos);
                    else {
                        string prf = prefix;
#ifdef WIN32
						prefix = ">>";
#else
						prefix = "▶▶";
#endif
                        printline(lines.numeros[pos], line, pos);
                        prefix = prf;
                    }
                }
            }
            else
                printline(pos+1, line);
        }
    }

    //We detect long commented lines or long strings
    void scanforlonglines() {
        longstrings.clear();
        //We check for comments and long lines
        long r;
        for (long i = 0; i < lines.size(); i++) {
            if (lines[i].find(L"//") != -1) {
                longstrings.push_back(l_com_one);
                continue;
            }
            
            r = lines[i].find(L"/@");
            if (r != -1) {
                longstrings.push_back(l_com);
                if (lines[i].find(L"@/", r) != -1)
                    continue;
                i++;
                while (i < lines.size() && lines[i].find(L"@/") == -1) {
                    longstrings.push_back(l_com);
                    i++;
                }
                longstrings.push_back(l_com);
                continue;
            }
            
            r = lines[i].find(L"@\"");
            if (r != -1) {
                if (lines[i].find(L"\"@", r) != -1) {
                    longstrings.push_back(l_str);
                    continue;
                }
                
                longstrings.push_back((r+1)*-1);
                i++;
                while (i < lines.size() && lines[i].find(L"\"@") == -1) {
                    longstrings.push_back(l_str);
                    i++;
                }
                longstrings.push_back(l_str);
                continue;
            }

            longstrings.push_back(0);
        }
    }
    
    void displaylist(long beg, long end) {
		if (!lines.size()) {
			clearline();
			if (!noprefix)
				cout << back << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << "1> " << endl;
			return;
		}

        stringstream blk;
        
        if (beg < 0)
            beg = 0;
        
        if (modified) {
            scanforlonglines();
            modified = false;
        }

        if (emode()) {
            long nb = 0;
            x_option g = option;
            option = x_none;
            
            poslines.clear();
            
            lines.updatesize();
            
            for (long i = beg; i < lines.size(); i++) {
                string space(prefixe(), ' ');
                if (lines.status[i] == concat_line)
                    blk << space << coloringline(lines[i], i) << endl;
                else {
                    if (lines.check(i))
                        blk << m_dore << "▶▶" << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[i] << "> " << m_current << coloringline(lines[i], i) << endl;
                    else
                        blk << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[i] << "> " << m_current << coloringline(lines[i], i) << endl;
                }
				poslines.push_back(i);
				if (poslines.size() > row_size)
					break;
            }
            option = g;
            clearscreen();
            cout << blk.str();
            return;
        }
        
        if (end >= lines.size())
            end = lines.size() - 1;
        
        long lastsz = 0;
        long thesz, asz;

        if (beg)
            beg = lines.getlinenumber(beg)+1;
        
        if (end)
            end = lines.getlinenumber(end)+1;

        for (long i = beg; i <= end; i++) {
            string space(prefixe(), ' ');
            if (lines.status[i] == concat_line)
                blk << space << coloringline(lines[i], i) << endl;
            else {
                if (lines.check(i))
                    blk << m_dore << "▶▶" << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[i] << "> " << m_current << coloringline(lines[i], i);
                else
                    blk << m_dore << prefix << m_current << m_lightgray << std::setw(prefixsize) << lines.numeros[i] << "> " << m_current << coloringline(lines[i], i);
            }
            asz = blk.str().size();
            thesz = col_size - (asz - lastsz);
            if (thesz > 0) {
                string space(thesz,' ');
                blk << space;
            }
            blk << endl;
            lastsz = asz;
        }
        cout << back << blk.str();
    }
    

    //------------------------------------------------------------------------------------------------
    //Deletion methods...
    //------------------------------------------------------------------------------------------------
    long prefixego() {
        if (option != x_none)
            return jag_editor::prefixego();
        
        if (debugmode && debuginfo.running)
            return 2;
        return prefixe();
    }

    void clearcurlybuffer() {
        nbcurly = 0;
        curlypos = -1;
#ifdef WIN32
		prefix = "<>";
#else
		prefix = "◀▶";
#endif
		pcursor = -1;
        curlyspace = L"";
    }
    
    void setpcursor() {
        getcursor();
        pcursor = xcursor;
    }
    
    long Pcursor() {
        getcursor();
        return (xcursor - pcursor)+curlypos;
    }

    //------------------------------------------------------------------------------------------------
    //formating method...
    //------------------------------------------------------------------------------------------------

    //------------------------------------------------------------------------------------------------
    //search method...
    //------------------------------------------------------------------------------------------------
    
    static bool isempty(wstring c) {
        Trim(c);
        if (c == L"")
            return true;
        return false;
    }
    
    static bool isempty(string c) {
        Trim(c);
        if (c == "")
            return true;
        return false;
    }
    
    //We create an empty space
    void addspace(string filename, bool fromwrite = false) {
        thecurrentfilename = Trim(filename);
        if (thecurrentfilename == "")
            return;
        
        thecurrentfilename = Normalizefilename(thecurrentfilename);
        currentfileid = ifilenames.size();
        ifilenames.push_back(thecurrentfilename);
        codes.push_back(L"");
        editor_keep kp;
        editors_undos.push_back(kp);
        editors_redos.push_back(kp);
        lastlines.push_back(0);
        filenames[thecurrentfilename] = currentfileid;
        if (!fromwrite) {
            //We create a local undos/redos section..
            cerr << m_redbold << "Create file space: " << thecurrentfilename << m_current <<" (" << currentfileid << ")" << endl;
            wstring c = L"\n";
            TamguSetCode(c);
            lines.setcode(c);
        }
    }
    
    bool loadfile(wstring& name) {
        return loadfile(convert(name));
    }

    //We keep different files in memory...
    bool loadfile(string filename) {
        wstring code;
        thecurrentfilename = Trim(filename);
        if (thecurrentfilename == "")
            return false;
        
        thecurrentfilename = Normalizefilename(thecurrentfilename);
        if (filenames.find(thecurrentfilename) != filenames.end()) {
            if (currentfileid != filenames[thecurrentfilename]) {
                //We backup our current undo/redo buffer
                undos.storein(editors_undos[currentfileid]);
                redos.storein(editors_redos[currentfileid]);
                
                currentfileid = filenames[thecurrentfilename];
                
                //We then activate the redo/undo in hold...
                editors_undos[currentfileid].storein(undos);
                editors_redos[currentfileid].storein(redos);
                code = codes[currentfileid];
                TamguSetCode(code);
            }
        }
        else {
            ifstream rd(thecurrentfilename, openMode);
            if (rd.fail()) {
                cerr << m_redbold << "Cannot load: " << thecurrentfilename << m_current << endl;
                return false;
            }
            string ln;
            string cde;
            while (!rd.eof()) {
                getline(rd, ln);
                ln = Trimright(ln);
                cde += ln + "\n";
            }
            TamguSetCode(cde);
            code = TamguUListing();
            if (currentfileid != -1) {
                //We backup our current undo/redo buffer
                undos.storein(editors_undos[currentfileid]);
                redos.storein(editors_redos[currentfileid]);
                //This is a new loading, with no undo/redo in hold
                undos.clear();
                redos.clear();
            }
            currentfileid = ifilenames.size();
            ifilenames.push_back(thecurrentfilename);
            codes.push_back(code);
            
            //We create a new redo/undo section
            editor_keep kp;
            editors_undos.push_back(kp);
            editors_redos.push_back(kp);
            filenames[thecurrentfilename] = currentfileid;
        }
        line = L"";
        posinstring = 0;
        lines.setcode(code);
        return true;

    }
    
    bool writetofile() {
        wstring code = lines.code();
        
        ofstream wd(thecurrentfilename, ios::binary);
        if (wd.fail())
            return false;
        wd << convert(code);
        wd.close();
        if (currentfileid == -1) {
            addspace(thecurrentfilename, true);
            codes[0] = code;
        }
        tobesaved = false;
        return true;
    }

    void cleardebug() {
        debugmode = false;
        debuginfo.stopexecution();
    }
    
    bool evallocalcode(string code, bool disp=false) {
        Trim(code);
        string avariable;
        if (code.find("[") == -1 && code.find("(") == -1) {
            //Pure variable
            short idname = globalTamgu->Getid(code);
            if (idname != -1) {
                Tamgu* a = globalTamgu->Getvariable(current_thread_id, idname);
                if (a != NULL) {
                    if (disp)
                        cout << code << ": ";
                    cout << m_redbold << a->String() << m_current << endl;
                    return true;
                }
            }
        }
        if (code.back() != ';') {
            avariable = "println(";
            avariable += code;
            avariable += ".json());";
        }
        else
            avariable = code;
        
        Tamgustring _arg(avariable);
        _arg.reference=100;
        _arg.protect = false;
        TamguCall func(0);
        func.arguments.push_back(&_arg);
        globalTamgu->debugmode = false;
        bool gL = globalTamgu->threadMODE;
        globalTamgu->threadMODE = false;
        if (disp)
            cout << code << ": ";
        cout << m_redbold;
        Tamgu* e = ProcEval(aNULL,current_thread_id,&func);
        cout << m_current;
        globalTamgu->debugmode = true;
        globalTamgu->threadMODE = gL;
        if (e->isError()) {
            globalTamgu->Cleanerror(0);
            return false;
        }
        e->Releasenonconst();
        return true;
    }
    
    long handlingcommands(long pos, bool& dsp, char initvar) {
        typedef enum {cmd_none, cmd_args, cmd_filename, cmd_spaces, cmd_select, cmd_edit, cmd_run, cmd_cls, cmd_echo, cmd_console, cmd_help, cmd_list,
            cmd_metas, cmd_rm, cmd_break, cmd_history, cmd_open, cmd_create, cmd_save, cmd_exit, cmd_load_history, cmd_store_history, cmd_colors, cmd_color, cmd_clear, cmd_reinit,
            cmd_debug, cmd_next, cmd_keep, cmd_remove, cmd_locals, cmd_all, cmd_stack, cmd_goto, cmd_in, cmd_out, cmd_stop, cmd_short_name, cmd_to_end, cmd_lispmode} thecommands;

        static bool init = false;
        static hmap<wstring, thecommands> commands;
        std::thread* tid;
        
        if (!init) {
            init = true;
            commands[L"args"] = cmd_args;
            commands[L"filename"] = cmd_filename;
            commands[L"spaces"] = cmd_spaces;
            commands[L"edit"] = cmd_edit;
            commands[L"select"] = cmd_select;
            commands[L"run"] = cmd_run;
            commands[L"cls"] = cmd_cls;
            commands[L"echo"] = cmd_echo;
            commands[L"console"] = cmd_console;
            commands[L"help"] = cmd_help;
            commands[L"list"] = cmd_list;
            commands[L"metas"] = cmd_metas;
            commands[L"rm"] = cmd_rm;
            commands[L"history"] = cmd_history;
            commands[L"load"] = cmd_load_history;
            commands[L"store"] = cmd_store_history;
            commands[L"open"] = cmd_open;
            commands[L"create"] = cmd_create;
            commands[L"save"] = cmd_save;
            commands[L"exit"] = cmd_exit;
            commands[L"colors"] = cmd_colors;
            commands[L"color"] = cmd_color;
            commands[L"clear"] = cmd_clear;
            commands[L"reinit"] = cmd_reinit;
            commands[L"debug"] = cmd_debug;
            commands[L"n!"] = cmd_next;
            commands[L"k!"] = cmd_keep;
            commands[L"r!"] = cmd_remove;
            commands[L"h!"] = cmd_help;
            commands[L"l!"] = cmd_locals;
            commands[L"a!"] = cmd_all;
            commands[L"s!"] = cmd_stack;
            commands[L"g!"] = cmd_goto;
            commands[L"i!"] = cmd_in;
            commands[L"o!"] = cmd_out;
            commands[L"S!"] = cmd_stop;
            commands[L"e!"] = cmd_to_end;
            commands[L"break"] = cmd_break;
            commands[L"short"] = cmd_short_name;
            commands[L"lisp"] = cmd_lispmode;
        }
        
        cout << endl;
        wstring code;
        
        long i, idcode;
        
        vector<wstring> v;
        vsplit(line, L" ", v);
        if (v.size() == 0)
            return pos;
        
        thecommands command = cmd_none;
        if (commands.find(v[0]) != commands.end()) {
            command = commands[v[0]];
        }
        else {
            if (line[0] == '!' || line[0] == '?') {
                if (debugmode && debuginfo.running)
                    return pos;

                if (line[0] == '!') {
                    addcommandline(line);

                    //We launch a Unix command...
                    code = line.substr(1, line.size() -1);
                    long iquote = line.find(L"\"");
                    long iequal = line.find(L"=");
                    if (iequal != -1 && (iquote == -1 || iequal < iquote)) {
                        code = line.substr(iequal+1, line.size()-iequal);
                        line = line.substr(1, iequal);
                        if (iquote == -1) {
                            line += L"_sys.pipe(\"";
                            line += code;
                            line += L"\");";
                        }
                        else {
                            line += L"_sys.pipe('";
                            line += code;
                            line += L"');";
                        }
                    }
                    else {
                        if (iquote != -1) {
                            line = L"_sys.command('";
                            line += code;
                            line += L"');";
                        }
                        else {
                            line = L"_sys.command(\"";
                            line += code;
                            line += L"\");";
                        }
                    }
                    
                    Executesomecode(line);
                    
                    code = TamguUListing();
                    lines.setcode(code);
                    lines.pop_back();
                    code = lines.code();
                    TamguSetCode(code);
                    return lines.size();
                }
                
                if (line[0] == '?') {
                    line = Trim(line);
                    line.erase(0, 1);
                    i = line.rfind(L".");
                    if (i != -1) {
                        code = line.substr(i+1 , line.size() - 2);
                        line.erase(i, string::npos);
                        line += L".info('";
                        line += code;
                        line += L"')";
                        Executesomecode(line);
                    }
                    else {
                        code = L"printjln(_info('" + line + L"'), \"\\n\", ' for type: ');";
                        Executesomecode(code);
                    }
                    return pos;
                }
            }
        }
        
        switch(command) {
            case cmd_none:
                break;
            case cmd_args: //args
            {
                arguments.clear();
                string arg;
                for (i = 1; i < v.size(); i++) {
                    s_unicode_to_utf8(arg, v[i]);
                    arguments.push_back(arg);
                }
                
                if (arguments.size())
                    TamguSetArguments(arguments);
            }
                return pos;
            case cmd_filename:
                cout << back << m_redbold << "Filename: " << m_red << thecurrentfilename << m_current << endl;
                return pos;
            case cmd_spaces:
                if (v.size() == 1) {
                    cout << back << m_redbold << "File Space:" << endl << endl;
                    for (i = 0; i < ifilenames.size(); i++) {
                        if (i == currentfileid)
                            cout << back << m_redbold << "Filename " << i <<": " << m_red << ifilenames[i] << m_current << " <<< " << endl;
                        else
                            cout << back << m_redbold << "Filename " << i <<": " << m_red << ifilenames[i] << m_current << endl;
                    }
                    cout << endl;
                    return pos;
                } //if a value is provided, then it works as a select...
            case cmd_select:
                addcommandline(line);
                if (debugmode && debuginfo.running)
                    return pos;
                if (v.size() == 1) {
                    cout << back << m_redbold << "Missing file space id" << endl;
                    return pos;
                }
                i = convertinteger(v[1]);
                if (i < 0 || i >= ifilenames.size()) {
                    cout << back << m_redbold << "File space: " << i << " does not exist" << endl;
                    return pos;
                }
                if (i != currentfileid) {
                    //We backup our current undo/redo section
                    undos.storein(editors_undos[currentfileid]);
                    redos.storein(editors_redos[currentfileid]);
                    
                    currentfileid = i;
                    code = codes[i];
                    thecurrentfilename = ifilenames[i];
                    
                    //we now reactivate the current undo/redo section
                    editors_undos[i].storein(undos);
                    editors_redos[i].storein(redos);
                    
                    lines.setcode(code);
                    TamguSetCode(code);
                    posinstring = 0;
                    line = L"";
                    cout << "File space is now: " << thecurrentfilename << " (" << i << ")" << endl;
                    pos = 0;
                    modified = true;
                }
                return pos;
           case cmd_edit:
                if (v.size() == 2) {
                    i = convertinteger(v[1]);
                    if (i < 0 || i >= ifilenames.size()) {
                        cout << back << m_redbold << "File space: " << i <<" does not exist" << endl;
                        return pos;
                    }
                    if (i != currentfileid) {
                        //We backup our current undo/redo section
                        undos.storein(editors_undos[currentfileid]);
                        redos.storein(editors_redos[currentfileid]);
                        lastlines[currentfileid] = lastline;
                        
                        currentfileid = i;
                        code = codes[i];
                        thecurrentfilename = ifilenames[i];
                        lastline = lastlines[i];
                        
                        //we now reactivate the current undo/redo section
                        editors_undos[i].storein(undos);
                        editors_redos[i].storein(redos);
                        
                        lines.setcode(code);
                        modified = true;
                    }
                }
                
                addcommandline(line);
                if (debugmode && debuginfo.running)
                    return pos;
#ifdef WIN32
				prefix = "<>";
#else
				prefix = "작";
#endif

                if (lines.size() == 0) {
                    lines.push(L"");
                    poslines.clear();
                    poslines.push_back(0);
                }

                editmode = true;
                currentline = 0;
                dsp = false;
                
                pos = lastline;
                option = x_none;
                posinstring = 0;
                line = lines[lastline];
                displaylist(lastline, row_size);
                movetoline(currentline);
                movetobeginning();
                return pos;
            case cmd_run:
                addcommandline(line);
                if (v.size() == 1) {
                    string cd = TamguListing();
                    if (isempty(cd))
                        return pos;

                    TamguExtinguish();
                    TamguLaunching();
                    if (arguments.size())
                        TamguSetArguments(arguments);

                    line = L"";
                    editmode = false;

                    if (debugmode) {
                        current_thread_id = 0;
                        globalTamgu->Setdebugmode(true);
                        globalTamgu->Setdebugfunction(debuginfo_callback, this);
                        debuginfo.clearall();
#ifdef WIN32
						prefix = ">>";
#else
						prefix = "▶▶";
#endif
                        tid = new std::thread(debuggerthread, this);
                    }
                    else {
                        globalTamgu->Setdebugmode(false);
                        globalTamgu->Setdebugfunction(NULL);
                    }
                    
                    if (thecurrentfilename == "")
                        idCode = TamguCompile(cd, THEMAIN, false);
                    else
                        idCode = TamguCompile(cd, thecurrentfilename, false);

                    if (idCode == -1) {
                        cerr << m_redbold << TamguErrorMessage() << m_current << endl;
                        globalTamgu->Cleanerror(0);
                        return pos;
                    }
                    
                    runcode(debugmode);
                    return pos;
                }
                
                if (loadfile(v[1])) {
                    idcode = TamguLoad(thecurrentfilename);
                    
                    if (idcode != -1) {
                        cout << m_red;
                        if (!TamguRun(idcode))
                            cerr << m_redbold << TamguErrorMessage() << endl;
                        cout << m_current;
                    }
                }
                else
                    cerr << m_redbold << "Cannot load: " << thecurrentfilename << m_current << endl;
                return pos;
            case cmd_cls:
                clearscreen();
                return pos;
            case cmd_echo:
                if (debugmode && debuginfo.running)
                    return pos;
                echochar = true;
                return pos;
            case cmd_console:
                if (debugmode && debuginfo.running)
                    return pos;
#ifdef WITHCONSOLE
                addcommandline(line);
            {
                string s = TamguListing();
                if (thecurrentfilename == "")
                    window_editor_code("Unknown", s);
                else
                    window_editor_code(thecurrentfilename, s);
                exit(0);
            }
#else
                cerr << m_redbold << "Not implemented" << endl;
#endif
                return pos;

            case cmd_help:
                if (debugmode) {
                    cerr << "- " << m_redbold << "a!:" << m_current << " display all active variables" << endl;
                    cerr << "- " << m_redbold << "e!:" << m_current << " execute up to the end" << endl;
                    cerr << "- " << m_redbold << "g!:" << m_current << " go to next breakpoint" << endl;
                    cerr << "- " << m_redbold << "h!:" << m_current << " display debug help" << endl;
                    cerr << "- " << m_redbold << "i!:" << m_current << " get into a function" << endl;
                    cerr << "- " << m_redbold << "k!: var" << m_current << " display variable at each iteration" << endl;
                    cerr << "- " << m_redbold << "l!:" << m_current << " display local variables" << endl;
                    cerr << "- " << m_redbold << "n!:" << m_current << " next line (also carriage return)" << endl;
                    cerr << "- " << m_redbold << "o!:" << m_current << " get out of a function" << endl;
                    cerr << "- " << m_redbold << "r!: var|all" << m_current << " delete var from display or all variables kept for display" << endl;
                    cerr << "- " << m_redbold << "s!:" << m_current << " display the stack" << endl;
                    cerr << "- " << m_redbold << "S!:" << m_current << " stop the execution" << endl;
                    cerr << "- " << m_redbold << "name:" << m_current << " display the content of a variable (just type its name)" << endl;
                    return pos;
                }
                i = 0;
                if (v.size() == 2) {
                    i = v[1][0] - 48;
                    if (i < 1 || i > 6)
                        i = 0;
                }
                
                displaythehelp(i);
                return pos;
            case cmd_metas:
                if (debugmode && debuginfo.running)
                    return pos;
                cout << m_redbold << _metacharacters << endl;
                return pos;
            case cmd_list:
            case cmd_rm:
            case cmd_break:
                addcommandline(line);
                if (debugmode && debuginfo.running)
                    return pos;
            {
                code = TamguUListing();
                if (isempty(code))
                    return pos;
                
                if (v.size() >= 2 && v[1].size() > 1) {
                    wstring c = v[1];
                    long ps = c.find(L":");
                    if (ps != -1) {
                        long sz = c.size();
                        sz--;
                        v.pop_back();
                        if (ps == 0) { //:10
                            v.push_back(L":");
                            c.erase(0,1);
                            v.push_back(c);
                        }
                        else {
                            if (ps == sz) {//the end...
                                c.erase(sz, 1);
                                v.push_back(c);
                                v.push_back(L":");
                            }
                            else {//10:20
                                v.push_back(c.substr(0, ps));
                                v.push_back(L":");
                                v.push_back(c.substr(ps+1, c.size()));
                            }
                        }
                    }
                }
                
                string codeindente;
                i = 3;
                string cd = convert(code);
                codeindente = "";
                IndentCode(cd, codeindente, i, isLispmode());
                code = wconvert(codeindente);
                lines.setcode(code);
                
                if (lines.size() == 0)
                    return pos;
                
                bool md = false;
                long backpos = pos;
                long lastline = lines.numeros.back();
                long beg = 0, end = lastline;
                if (v.size() >= 2) {
                    md = true;
                    if (v[1] == L":") { //list :20
                                        //we display up to the next element (if it part of it...
                        if (v.size() >= 3) {
                            if (v[2] == L"$")
                                end = lastline;
                            else {
                                end = convertinteger(v[2]) - 1;
                                if (end > lastline)
                                    end = lastline;
                            }
                            pos = lines.getlinenumber(end);
                        }
                    }
                    else {
                        if (v[1] == L"$")
                            beg = lines.size();
                        else {
                            beg = convertinteger(v[1]) - 1;
                            if (beg < 0)
                                beg = 0;
                            if (beg > lastline)
                                beg = lastline;
                        }
                        pos = lines.getlinenumber(beg);
                        if (v.size() >= 3) {
                            if (v[2] == L":") { //list 23:
                                if (v.size() >= 4)
                                    end = convertinteger(v[3]) - 1; //list 23:40
                            }
                            else
                                end = convertinteger(v[2]) - 1; //list 10 23
                            
                            if (end > lastline)
                                end = lastline;
                            pos = lines.getlinenumber(end);
                        }
                        else
                            end = beg; // we display one line only
                    }
                }
                else
                    pos = lines.getlinenumber(lastline);
                
                if (command == cmd_break) {
                    if (v.size() >= 2 && v[1] == L"clear") {
                        debuginfo.breakpoints.clear();
                        lines.checks.clear();
                        pos = backpos;
                        return pos;
                    }
                    
                    if (!md) {
                        cout << endl;
                        for (auto& b: debuginfo.breakpoints) {
                            for (auto& c: b.second) {
                                cout << b.first << ":" << c.first << endl;
                            }
                        }
                        return pos;
                    }
                    
                    string buffer = Normalizefilename(thecurrentfilename);
                    
                    if (lines.check(pos)) {
                        debuginfo.breakpoints[buffer].erase(lines.numeros[pos]);
                        lines.checks.erase(pos);
                        cout << "breakpoint removed: " << beg+1 << endl;
                    }
                    else {
                        debuginfo.breakpoints[buffer][lines.numeros[pos]] = true;
                        lines.checks[pos] = true;
                        cout << "breakpoint added: " << beg+1 << endl;
                    }
                    
                    line = L"";
                    posinstring = 0;
                    return pos;
                }
                if (command == cmd_list) { //list
                    cout << endl;
                    displaylist(beg, end);
                    cout << endl;
                }
                else {
                    beg = lines.getlinenumber(beg)+1;
                    end = lines.getlinenumber(end)+2;
                    lines.erase(beg, end);
                    code = lines.code();
                    TamguSetCode(code);
                }
            }
                line = L"";
                posinstring = 0;
                return pos;
            case cmd_history:
                if (debugmode && debuginfo.running)
                    return pos;

                cerr << endl;
                if (historyfilename != "")
                    cerr << m_redbold << "History:" << historyfilename << m_current << endl;
                
                for (i = 0; i < commandlines.size(); i++)
                    cerr << i+1 << " = " << coloringline(commandlines[i]) << endl;
                cerr << endl;
                addcommandline(v[0]);
                return pos;
            case cmd_load_history: {
                if (debugmode && debuginfo.running)
                    return pos;

                if (v.size() != 2) {
                    if (historyfilename == "") {
                        cerr << m_redbold << "Missing file name.." << m_current << endl;
                        return pos;
                    }
                }
                else
                    historyfilename = Normalizefilename(convert(v[1]));

                    
                ifstream ld(historyfilename, openMode);
                if (ld.fail()) {
                    cerr << m_redbold << "Cannot load:" << historyfilename << m_current << endl;
                    return pos;
                }
                string s;
                while (!ld.eof()) {
                    getline(ld, s);
                    s=Trim(s);
                    if (s!="") {
                        code = wconvert(s);
                        commandlines.push_back(code);
                    }
                }
                return pos;
            }
            case cmd_store_history: {
                if (debugmode && debuginfo.running)
                    return pos;

                if (v.size() != 2) {
                    if (historyfilename == "") {
                        cerr << m_redbold << "Missing file name.." << m_current << endl;
                        return pos;
                    }
                }
                else
                    historyfilename = Normalizefilename(convert(v[1]));
                ofstream st(historyfilename, std::ios::binary);
                for (i = 0; i < commandlines.size(); i++)
                    st << convert(commandlines[i]) << endl;
                return pos;
            }
            case cmd_open:
                if (debugmode && debuginfo.running)
                    return pos;
                addcommandline(line);

                if (v.size() == 1) {
                    if (thecurrentfilename == "") {
                        cerr << m_redbold << "Missing file name.." << m_current << endl;
                        return pos;
                    }
                }
                if (loadfile(v[1]))
                    cerr << m_red << "ok." << m_current << endl;
                return pos;
            case cmd_create:
                if (debugmode && debuginfo.running)
                    return pos;
                
                addcommandline(line);
                if (v.size() == 1) {
                    cerr << m_redbold << "Missing file name.." << m_current << endl;
                    return pos;
                }
                addspace(convert(v[1]));
                line = L"";
                return pos;
            case cmd_save:
                if (debugmode && debuginfo.running)
                    return pos;
                addcommandline(line);
                if (v.size() == 1) {
                    if (thecurrentfilename == "") {
                        cerr << m_redbold << "Missing file name.." << m_current << endl;
                        return pos;
                    }
                }
                else {
                    thecurrentfilename = convert(v[1]);
                    thecurrentfilename = Trim(thecurrentfilename);
                    thecurrentfilename = Normalizefilename(thecurrentfilename);
                }
            {
                string codeindente;
                string cd = TamguListing();
                i = 3;
                IndentCode(cd, codeindente, i, isLispmode());
                ofstream wd(thecurrentfilename, ios::binary);
                wd << codeindente;
                wd.close();
                
                //if this is a first saving of this code...
                if (filenames.find(thecurrentfilename) == filenames.end()) {
                    currentfileid = ifilenames.size();
                    ifilenames.push_back(thecurrentfilename);
                    filenames[thecurrentfilename] = currentfileid;
                    code = wconvert(cd);
                    codes.push_back(code);
                    
                    //We also backup our undo/redo section
                    editors_undos.push_back(undos);
                    editors_redos.push_back(redos);
                }
            }
                return pos;
            case cmd_exit:
                return !terminate();
            case cmd_colors:
                if (debugmode && debuginfo.running)
                    return pos;
                addcommandline(line);

                if (v.size() == 1) {
                    int j;
                    cerr << endl << m_redbold << "Denomination\t" << "att\tfg\tbg" <<endl;
                    for (i = 0; i <= 4; i++) {
                        cerr << colors[i] << colordenomination[i] << ":\t";
                        j = 2;
                        while (colors[i][j] != 'm') {
                            if (colors[i][j] == ';')
                                cerr << "\t";
                            else
                                cerr << colors[i][j];
                            j++;
                        }
                        cerr << m_current << endl;
                    }
                    cerr << endl;
                    return pos;
                }
                if (v.size() == 5) {
                    char buffer[100];
                        //the second word is the denomination
                    string s;
                    for (i = 0; i <= 4; i++) {
                        s_unicode_to_utf8(s, v[1]);
                        if (colordenomination[i] == s) {
                            long att = convertinteger(v[2]);
                            long fg = convertinteger(v[3]);
                            long bg = convertinteger(v[4]);
                            sprintf(buffer,"\033[%ld;%ld;%ldm", att, fg, bg);
                            colors[i] = buffer;
                            cout << buffer << s << " " << att << " " << fg << " " << bg << m_current << endl;
                            return pos;
                        }
                    }
                }
                
                cerr << m_redbold << "colors takes four parameters: denomination attribute forground background" << m_current << endl;
                return pos;
            case cmd_color:
                if (debugmode && debuginfo.running)
                    return pos;
                addcommandline(line);
                
            {
                char buffer[100];
                if (v.size() == 4) {
                    long att = convertinteger(v[1]);
                    long fg = convertinteger(v[2]);
                    long bg = convertinteger(v[3]);
                    sprintf(buffer,"\033[%ld;%ld;%ldm", att, fg, bg);
                    printf("%sdisplaying color%s\n",buffer, m_current);
                }
                else {
                    for (int att = 0; m_attr[att] != -1; att++) {
                        for (int fg = 0; m_clfg[fg]; fg++) {
                            for (int bg = 0; m_clbg[bg]; bg++) {
                                sprintf(buffer,"\033[%d;%d;%dm", m_attr[att], m_clfg[fg], m_clbg[bg]);
                                printf("%s%d,%d,%d:\t%s displaying color%s\n",m_current,m_attr[att], m_clfg[fg], m_clbg[bg], buffer, m_current);
                            }
                        }
                    }
                    cout << m_current;
                }
            }
                return pos;
            case cmd_clear:
                if (debugmode && debuginfo.running)
                    return pos;
                addcommandline(line);
                if (v.size() == 2) {
                    i = convertinteger(v[1]);
                    if (i < 0 || i >= ifilenames.size()) {
                        cout << back << m_redbold << "file space does not exist" << endl;
                        return pos;
                    }
                    //In this case, we clear one space...
                    string current =  ifilenames[i];
                    filenames.erase(current);
                    ifilenames.erase(ifilenames.begin()+i);
                    codes.erase(codes.begin()+i);
                    editors_undos.erase(editors_undos.begin()+i);
                    editors_redos.erase(editors_redos.begin()+i);

                    //We need to resynchronize the id for each file...
                    hmap<string, short>:: iterator it;
                    for (it = filenames.begin(); it != filenames.end(); it++) {
                        if (it->second > i)
                            --it->second;
                    }
                    if (ifilenames.size() != 0) {
                        //if i points to currentfileid, we clear it and replaces it, with position 0
                        if (i == currentfileid) {
                            currentfileid = 0; //we select the first one, by default...
                            code = codes[currentfileid];
                            thecurrentfilename = ifilenames[currentfileid];
                            undos.storein(editors_undos[currentfileid]);
                            redos.storein(editors_redos[currentfileid]);
                            lines.setcode(code);
                            TamguSetCode(code);
                            posinstring = 0;
                            line = L"";
                            cout << "File space is now: " << currentfileid << endl;
                            pos = 0;
                            modified = true;
                        }
                    }
                }
                else {
                    filenames.clear();
                    ifilenames.clear();
                    codes.clear();
                    editors_undos.clear();
                    editors_redos.clear();
                }

                if (!ifilenames.size()) {
                    thecurrentfilename = "";
                    lines.clear();
                }
                
                clearcurlybuffer();
                line = L"";
                posinstring = 0;
                TamguExtinguish();
                TamguLaunching();
                if (arguments.size())
                    TamguSetArguments(arguments);
                
                if (initvar == 1 || CheckThroughVariables()) {
                    string cde;
                    if (initvar == 1)
                        cde = _variable_declaration;
                    if (CheckThroughVariables())
                        RetrieveThroughVariables(cde);
                    idcode = TamguCompile(cde, THEMAIN);
                    TamguRun(idcode);
                    code = TamguUListing();
                    lines.setcode(code);
                }

                pos = 0;
                return pos;
            case cmd_reinit:
                if (debugmode && debuginfo.running)
                    return pos;
                addcommandline(line);

                thecurrentfilename = "";
                lines.clear();
                clearcurlybuffer();
                line = L"";
                posinstring = 0;
                TamguExtinguish();
                TamguLaunching();
                if (arguments.size())
                    TamguSetArguments(arguments);
            {
                string cde = _variable_declaration;
                if (CheckThroughVariables())
                    RetrieveThroughVariables(cde);
                idcode = TamguCompile(cde, THEMAIN);
            }
                TamguRun(idcode);
                code = TamguUListing();
                lines.setcode(code);
                pos = 0;
                return pos;
            case cmd_debug:
                if (debugmode && debuginfo.running)
                    return pos;
                addcommandline(line);
                
                debugmode = 1 - debugmode;
                updateline = true;
                if (debugmode)
                    cout << m_red << "debugger on" << endl;
                else
                    cout << m_red << "debugger off" << endl;
                return pos;
            case cmd_keep:
                if (debugmode && debuginfo.running) {
                    string var = convert(v[1]);
                    for (i = 0; i < displaying.size(); i++) {
                        if (displaying[i] == var)
                            return pos;
                    }
                    displaying.push_back(var);
                }
                return pos;
            case cmd_remove:
                if (debugmode && debuginfo.running) {
                    string var = convert(v[1]);
                    if (var == "all") {
                        displaying.clear();
                        return pos;
                    }
                    for (i = 0; i < displaying.size(); i++) {
                        if (displaying[i] == var) {
                            displaying.erase(displaying.begin()+i);
                            return pos;
                        }
                    }
                }
                return pos;
            case cmd_next:
                if (debugmode && debuginfo.running) {
                    if (!debuginfo.next())
                        cleardebug();
                    else
                        Reseting_system_environment_for_getchar();
                }
                return pos;
            case cmd_in:
                if (debugmode && debuginfo.running) {
                    if (!debuginfo.getin())
                        cleardebug();
                    else
                        Reseting_system_environment_for_getchar();
                }
                return pos;
            case cmd_out:
                if (debugmode && debuginfo.running) {
                    if (debuginfo.getout())
                        cleardebug();
                    else
                        Reseting_system_environment_for_getchar();
                }
                return pos;
            case cmd_goto:
                if (debugmode && debuginfo.running) {
                    if (!debuginfo.gotonext())
                        cleardebug();
                    else
                        Reseting_system_environment_for_getchar();
                }
                return pos;
            case cmd_locals:
                if (debugmode && debuginfo.running)
                    cout << debuginfo.localvariables;
                else
                    Reseting_system_environment_for_getchar();
                return pos;
            case cmd_all:
                if (debugmode && debuginfo.running)
                    cout << debuginfo.allvariables;
                else
                    Reseting_system_environment_for_getchar();
                return pos;
            case cmd_stack:
                if (debugmode && debuginfo.running)
                    cout << debuginfo.sstack;
                else
                    Reseting_system_environment_for_getchar();
                return pos;
            case cmd_short_name:
                if (debugmode && debuginfo.running) {
                    if (debuginfo.shortdisplay())
                        cout << "short display on" << endl;
                    else
                        cout << "short display off" << endl;
                }
                return pos;
            case cmd_stop:
                if (debugmode && debuginfo.running) {
                    if (!debuginfo.stopexecution())
                        cleardebug();
                    else
                            Reseting_system_environment_for_getchar();
                }
                return pos;
            case cmd_to_end:
                if (debugmode && debuginfo.running) {
                    if (!debuginfo.gotoend())
                        cleardebug();
                    else
                        Reseting_system_environment_for_getchar();
                }
                return pos;
            case cmd_lispmode:
                if (v.size() == 1) {
                    lispmode = 1 - lispmode;
                    Setlispmode(lispmode);
                    if (lispmode)
                        cout << "Lisp mode activated" << endl;
                    else
                        cout << "Lisp mode deactivated" << endl;
                    return pos;
                }
        }
        
        if (debugmode && debuginfo.running) {
            if (line != L"") {
                addcommandline(line);
                evallocalcode(convert(line));
            }
            else {
                if (!debuginfo.next())
                    cleardebug();
            }
            return pos;
        }
        
        //Adding a line into the code
        if (line.size()) {
            if (curlypos != -1) {
                //if the last line is not a blank line then it means that we did not "enter" the last line in the list...
                //We did not press ENTER on this line
                //we will only check if pos is not the last line.
                //if the last line is a blank line, then it means that the last line was appended with ENTER, we do not want to take it into
                //account. The insertion is only taken into account when we skip it hence nb==2
                long nb = 2;
                wstring l = lines.back();
                for (i = 0; i < l.size(); i++) {
                    if (l[i] != 32) {
                        nb = 1;
                        break;
                    }
                }
                
                if (pos >= curlypos && pos < lines.size() - nb) {
                    //The position is at a known line...
                    //we insert a new line
                    pos++;
                    line = lines[pos];
                    for (i = 0; i < line.size(); i++) {
                        if (line[i] != L' ')
                            break;
                    }
                    line = L"";
                    posinstring = 0;
                    if (i > 0) {
                        line = wstring(i, L' ');
                        posinstring = line.size();
                    }
                    
                    lines.inserting(pos, line);
                    
                    displaylist(pos, lines.size());
                    i = pos;
                    while (i != lines.size()) {
                        cout << m_up;
                        i++;
                    }
                    dsp = false;
                    return pos;
                }
            }
            
            char lastchar = line.back();

            if (lastchar == '}') {
                if (nbcurly > 0) {
                    nbcurly--;
                    if (line.size() > 4) {
                        curlyspace = curlyspace.substr(0, curlyspace.size()-4);
                        string space(line.size()+prefixe()+1, ' ');
                        line = line.substr(4, line.size()-4);
                        lines[pos] = line;
                        cout << m_up << back << space << back;
                        displaygo(true);
                        cout << m_down << back;
                    }
                    else
                        curlyspace = L"";
                }
            }
            else {
                if (lastchar == '{') {
                    curlyspace += L"    ";
                    nbcurly++;
                    if (curlypos == -1) {
                        curlypos = pos;
                        prefix = "[]";
                        cout << m_up << back << m_dore << prefix << m_current << m_down << back;
                        if (!curlypos)
                            lines[0] = line;
                        else
                            lines.push_back(line);
                        pos = lines.size();
                        lines.push(L"");
                        return pos;
                    }
                }
            }
            
            if (curlypos != -1) {
                //if we are here, then it means that the last line is a blank line, no need to add one more...
                if (pos == lines.size() - 2)
                    return pos+1;
                
                pos = lines.size();
                //otherwise, we prepare our list to welcome a future code insertion
                lines.push(L"");
                return pos;
            }
            
            Executesomecode(line);
            code = TamguUListing();
            
            lines.setcode(code);
            pos = lines.size();
            return pos;
        }
        
        if (curlypos != -1 && !nbcurly) {
            code = L"";
            while (curlypos < pos) {
                code += lines[curlypos++] + L"\n";
            }
            
            Executesomecode(code);
            clearcurlybuffer();
            code = TamguUListing();
            lines.setcode(code);
            pos = lines.size();
        }

        return pos;
    }
    
    void init() {
        lines.clear();
        lines.push(L"");
        poslines.clear();
        poslines.push_back(0);
        displaylist(0,1);
        pos = 0;
        posinstring = 0;
        line = L"";
        kbuffer = L"";
        currentline = 0;
        clearscreen();
        printline(1);
    }

    bool terminate() {
        replaceall = false;
        if (tobesaved) {
            tobesaved = false;
            if (emode())
                displayonlast("File not saved... same command again to quit", true);
            else
                printline(pos+1, "File not saved... ctrl-d again to quit");
            return false;
        }
        
        movetolastline();
        clearline();
        cout << back << m_redbold << "bye!!!" << m_current << endl;
        
        if (debugmode && debuginfo.running)
            debuginfo.stopexecution();
        
        fflush(stdout);
		resetterminal();
        exit(0);
        return true;
    }
    
    void clear() {
        echochar = false;
        pos = lines.size();
        if (!editmode) {
            cout << "^C" << endl;
            printline(pos+1);
        }
        else {
            lastline = poslines[0];
            editmode = false;
            currentline = 0;
            
            wstring code = lines.code();
            if (currentfileid != -1)
                codes[currentfileid] = code;
            TamguSetCode(code);
            movetolastline();
            clearline();
            string l = m_red;
            l += "exit editor";
            l += m_current;
#ifdef WIN32
			prefix = "<>";
#else
			prefix = "◀▶";
#endif
			printline(pos+1, l);
            cout << endl;
            clearline();
            printline(pos+1);
        }
        
        if (debugmode && debuginfo.running)
            cleardebug();
        else
            TamguStop();

        fflush(stdout);
        line = L"";
        posinstring = 0;
        clearcurlybuffer();
        currentline = 0;
    }

    bool checkcommand(char c) {
        switch (c) {
            case 'm':
                displayonlast(_metacharacters, true);
                return true;
            case 'd':
                displayonlast("", true);
                debugmode = true;
            case 'r':
                if (emode()) {
                    lastline = poslines[0];
                    if (lines.size()) {
                        bool dsp = true;
                        handle_ctrl_c(0);
                        line = L"run";
                        posinstring = linesize();
                        pos = 0;
                        editmode = true;
                        clearscreen();
                        
                        if (c == 'r')
                            debugmode = false;
                        else
                            cout << m_redital << "Debug on" << m_current << endl;
                        
                        handlingcommands(pos, dsp, false);
                        editmode = false;
                        posinstring = 0;
                        pos = lines.size()-1;
                        line = L"";
                        printline(pos+1);
                    }
                }
                return true;
            default:
                return jag_editor::checkcommand(c);
        }
        return false;
    }
    
    void ls(string path, vector<wstring>& paths) {
        FILE *fp;
        int status;

        char chemin[PATH_MAX];
        
        string cmd = "ls -1 -p ";
        cmd += path;

#ifdef WIN32
		fp = _popen(STR(cmd), "r");
#else
		fp = popen(STR(cmd), "r");
#endif
		if (fp == NULL)
            return;
        
        wstring l;
        while (fgets(chemin, PATH_MAX, fp) != NULL) {
            cmd = chemin;
            cmd = Trim(cmd);
            l = wconvert(cmd);
            paths.push_back(l);
        }
                
#ifdef WIN32
		status = _pclose(fp);
#else
		status = pclose(fp);
#endif
	}
    
    bool checkpath() {
        //The first part should be a command such as open or load...
        long pos = line.rfind(' ');
        if (pos == -1)
            return false;
        
        wstring root = line.substr(0, pos);
        wstring name;
        wstring path = line.substr(pos, line.size());
        path = Trim(path);
        //Two cases, we have a "/" in it...
        pos = path.rfind(L"/");
        if (pos != -1) {
            name = path.substr(pos+1, path.size()-pos);
            path = path.substr(0, pos+1);
        }
        else {
            name = path;
            path = L".";
        }
        vector<wstring> paths;
        vector<wstring> targets;
        ls(convert(path), paths);
        //Now we look for continuation
        long i;
        for (i = 0; i < paths.size(); i++) {
            if (paths[i].substr(0, name.size()) == name)
                targets.push_back(paths[i]);
        }
        if (path == L".")
            path = L"";
        
        if (targets.size() == 0)
            return false;
        
        if (targets.size() == 1) {
            line = root;
            line += L" ";
            line += path;
            line += targets[0];
            clearline();
            displaygo(true);
            posinstring = line.size();
            movetoposition();
            return true;
        }
        
        wstring common;
        long ln  = name.size();
        bool end = false;
        while (!end) {
            //We add one letter from the targets and see if it is common to all targets
            for (i = 0; i < targets.size(); i++) {
                if (ln >= targets[i].size()) {
                    end = true;
                    break;
                }
            }
            if (!end) {
                ++ln;
                common = targets[0].substr(0, ln);
                for (i = 1; i < targets.size(); i++) {
                    if (targets[i].substr(0, ln) != common) {
                        end = true;
                        break;
                    }
                }
                if (!end)
                    name = common;
            }
        }
        
        
        cerr << endl << endl << m_redital;
        for (i = 0; i < targets.size(); i++)
            cerr << convert(targets[i]) << " ";
        cerr << m_current << endl << endl;
        
        line = root;
        line += L" ";
        line += path;
        line += name;
        clearline();
        displaygo(true);
        posinstring = line.size();
        movetoposition();
        return true;
    }
    
    bool checkkeyboard(string& buff, long& first, long& last, bool& dsp, char noinit) {
        switch ((uchar)buff[0]) {
            case 2: //ctrl-b run/breakpoint
                if (emode()) {
                    string buffer = Normalizefilename(thecurrentfilename);
                    if (lines.check(pos)) {
                        debuginfo.breakpoints[buffer].erase(lines.numeros[pos]);
                        lines.checks.erase(pos);
                    }
                    else {
                        debuginfo.breakpoints[buffer][lines.numeros[pos]] = true;
                        lines.checks[pos] = true;
                    }
                    displaygo(true);
                }
                return true;
#ifdef WIN32
			case 3: //ctrl-c, only here on Windows
				clear();
				return true;
#endif
            case 4: //ctrl-d exiting
                if (emode()) { //we delete a line
                    deleteline(0);
                    return true;
                }
                if (debugmode && debuginfo.running) {
                    debuginfo.stopexecution();
#ifdef WIN32
					prefix = "<>";
#else
					prefix = "◀▶";
#endif
					return true;
                }
                return !terminate();
            case 9:
                if (emode())
                    return false;
                //We try to interpret the string as a path
                return checkpath();
#ifdef WIN32
            case 13: //this is a carriage return
#else
			case 10:
#endif
                if (option != x_none) {
                    checkaction(buff, first, last, isLispmode());
                    return true;
                }
                if (editmode) {
                    pos = handlingeditorline(true);
                    return true;
                }
                pos = handlingcommands(pos, dsp, noinit);
                if (dsp) {
                    if (curlypos != -1) {
                        line = curlyspace;
                        lines[pos] = line;
                        printline(pos+1, line);
                    }
                    else {
                        line = L"";
                        printline(pos+1);
                    }
                    posinstring = line.size();
                }
                else
                    movetoposition();
                return true;
            case 11: //ctrl-k: delete trailing characters
                if (emode()) {
                    deleteallafter();
                    return true;
                }
                
                clearline();
                kbuffer = line.substr(posinstring, line.size());
                line = line.substr(0, posinstring);
                if (pos < lines.size())
                    lines[pos] = line;
                if (option != x_none)
                    displaygo(true);
                else
                    printline(pos+1, line);
                return true;
            case 17:
                if (emode()) {
                    clear();
                    return true;
                }
                return checkaction(buff, first, last, isLispmode());
#ifdef WIN32
			case 224:
#else
			case 27: //Escape...
#endif
                     //we clear the current line if it is the only character...
                if (buff.size() == 1) {
                    if (option != x_none || tooglehelp) {
                        displayonlast("", true);
                        tooglehelp = false;
                        option = x_none;
                        return true;
                    }
                    
                    if (editmode)
                        return true;
                    
                    clearline();
                    printline(pos+1);
                    clearcurlybuffer();
                    line = L"";
                    posinstring = 0;
                    return true;
                }
                
                evaluateescape(buff);
                return true;
            default:
                return checkaction(buff, first, last, isLispmode());
        }
        return false;
    }
    
    void launchterminal(char noinit) {
		clearscreen();

        localhelp << m_red<< "^b" << m_current << ":breakpoint " << m_red<< "^c/q" << m_current << ":cmd line " << m_red << "^xq" << m_current << ":exit";
        
        option = x_none;
#ifdef WIN32
		prefix = "<>";
		cerr << endl << m_redbold << TamguVersion() << m_current << endl;
#else
		prefix = "작";
		cerr << endl << m_redbold << TamguVersion() << "(탐구)" << m_current << endl;
#endif
        
        cerr << "Copyright 2019-present NAVER Corp." << endl;
        cerr << "64 bits" << endl;
        
#ifdef WIN32
		SetConsoleCtrlHandler(handle_ctrl_c, TRUE);
#else
        signal(SIGINT,handle_ctrl_c);
#endif

        bool dsp = true;
        
        if (ifilenames.size() > 1) {
            currentfileid = 0;
            thecurrentfilename = ifilenames[0];
            lines.setcode(codes[0]);
            TamguSetCode(codes[0]);
        }

        switch (noinit) {
            case 1:
#ifdef WIN32
				prefix = "<>";
#else
				prefix = "◀▶";
#endif
				pos = 1;
                line = _wvariable_declaration;
                lines.push_back(line);
                poslines.push_back(0);
                line = L"";
                cerr << endl << m_red << "help: display available commands" << m_current << endl << endl;
                printline(pos+1);
                break;
            case 2:
#ifdef WIN32
				prefix = "<>";
#else
				prefix = "◀▶";
#endif
				cerr << endl << m_red << "help: display available commands" << m_current << endl << endl;
                TamguLaunching();
                if (arguments.size())
                    TamguSetArguments(arguments);
                
                lines.push(L"");
                poslines.push_back(0);
                line = L"";
                printline(1);
                break;
            case 3:
                //switch to edit mode
                pos = 0;
                line = L"edit";
                pos = handlingcommands(pos, dsp, noinit);
                break;
            case 4:
                pos = 1;
                line = _wvariable_declaration;
                lines.push_back(line);
                poslines.push_back(0);
                line = L"edit";
                noinit = 1;
                pos = handlingcommands(pos, dsp, noinit);
                break;
            default:
#ifdef WIN32
				prefix = "<>";
#else
				prefix = "◀▶";
#endif
				cerr << endl << m_red << "help: display available commands" << m_current << endl << endl;
                printline(pos+1);
        }
        

        clearst();
        wstring code;
        wstring b;
        string buffer;
        string buff;

        long first = 0, last;

        bool inbuffer = false;
        bool instring = false;

        while (1) {
            buff = getch();
            
            if (echochar) {
                displaychar(buff);
                continue;
            }
            
            dsp = true;
            if (checkkeyboard(buff, first, last, dsp, noinit))
                continue;
#ifdef WIN32
			if (!buff[0] && buff[1] == '#') {
				//Special case for ctrl+alt+h (ctrl-h is backdelete on windows
				if (emode()) {
					option = x_none;
					if (!tooglehelp)
						displayonlast(localhelp.str(), true);
					else {
						jag_editor::displaylist(poslines[0]);
						movetoline(currentline);
						movetoposition();
					}
					tooglehelp = 1 - tooglehelp;
				}
				continue;
			}
#endif
            if (inbuffer) {
                buffer += buff;
                buff = buffer;
                inbuffer = false;
            }
            
            if (buff.size() == getbuffsize())
                inbuffer = check_utf8(buff, buffer);
            
            code = wconvert(buff);
            
            cleanheaders(code);
            
            //We keep track of the initial form of the line...
            if (emode())
                undo(lines[pos],pos, u_modif); //The value is negative to indicate a deletion
            
            //Only one character to add, no need for further inspection, no CR in the string as well
            if (code.size() == 1 || buff.find(10) == -1)
                addabuffer(code, instring);
            else {
                for (long j = 0; j < code.size(); j++) {
                    b = code[j];
                    if (b[0] == 10) {
                        pos = handlingeditorline(false);
                        continue;
                    }
                    addabuffer(b, instring);
                }
            }
            
            if (curlypos != -1)
                lines[pos] = line;
        }
    }
    
    void setcode(string& code) {
        TamguSetCode(code);
        wstring cd = wconvert(code);
        lines.setcode(cd);
        pos = lines.size();
    }
    
    bool Executesomecode(wstring& c) {
        long idcode;
        string code = convert(c);
        
        Setlispmode(lispmode);
        
        if (thecurrentfilename == "")
            idcode = TamguCompile(code, THEMAIN, true);
        else
            idcode = TamguCompile(code, thecurrentfilename, true);

        if (!editmode)
            addcommandline(c);
        
        line = L"";
        posinstring = 0;
        
        if (idcode == -1) {
            cerr << m_redbold << TamguErrorMessage() << m_current << endl;
            globalTamgu->Cleanerror(0);
            return false;
        }
        
        cout << m_red;
        if (!TamguRun(idcode)) {
            cerr << m_redbold << TamguErrorMessage() << endl;
            globalTamgu->Cleanerror(0);
        }
        cout << m_current;
        
        lispmode=isLispmode();
        
        return true;
    }

    bool runcode(bool glock) {
        cout << m_red;
        if (!TamguRun(idCode, glock)) {
            cerr << m_redbold << TamguErrorMessage() << endl;
            globalTamgu->Cleanerror(0);
        }
        
        cout << m_current;
        
        if (debugmode) {
            cleardebug();
            editor_loquet.Released();
            updateline = true;
#ifdef WIN32
			prefix = "<>";
#else
			prefix = "◀▶";
#endif
		}
            
        return true;
    }
    

    void addcommandline(wstring& w) {
        commandlines.push_back(w);
        poscommand = commandlines.size();
    }
    
    void resetscreen() {
        screensizes();
        if (editmode && option == x_none) {
            modified = true;
            wstring code = lines.code();
            lines.setcode(code);
            displaylist(poslines[0], poslines.back());
            movetoline(currentline);
            posinstring = 0;
            movetobeginning();
        }
    }

};

///------------------------------------------------------------------------------------

#ifdef WIN32
BOOL WINAPI handle_ctrl_c(_In_ DWORD dwCtrlType) {
	if (JAGEDITOR != NULL)
		JAGEDITOR->clear();
	return true;
}
#else
static void handle_ctrl_c(int theSignal) {
    if (JAGEDITOR != NULL)
        JAGEDITOR->clear();
}
#endif

//This is a specific case, when the use of _getchar on UNix may interfere with the debuggger
//We force getchar in Tamgusys to reinitialize itself
Tamgu* debuginfo_callback(vector<Tamgu*>& stack, short idthread, void* data) {
    current_thread_id = idthread;
    tamgu_editor* te = (tamgu_editor*)data;
    Tamgu* res = debuginfo.debugger(stack, idthread, data);
    if (res == aTRUE) {
        long pos = te->lines.getlinenumber(debuginfo.currentline);
        if (debuginfo.filename != te->thecurrentfilename) {
            //We jump to another file... We need to reload it...
            te->loadfile(debuginfo.filename);
            cout << m_redital << "entering:" << m_redbold << debuginfo.filename << m_current << endl;
        }
        te->printdebug(pos);
        te->editor_loquet.Released();
        debuginfo.loquet.Blocked();
    }
    return res;
}


void debuggerthread(tamgu_editor* call) {
    bool dsp = true;
    long first = 0, last;
    string buff;
    wstring b;
    wstring code;
    
    call->updateline = false;
    //We force the new threadid to be id 0
    globalTamgu->SetThreadid();
    
    while (debuginfo.running) {
        call->editor_loquet.Blocked();
        call->posinstring = 0;
        call->line = L"";

        while (debuginfo.running) {
            buff = call->getch();
            
            dsp = true;
            if (call->checkkeyboard(buff, first, last, dsp, 0)) {
                if (buff == "" || buff == "n!")
                    break;
                continue;
            }
            code = call->wconvert(buff);
            
            //Only one character to add, no need for further inspection, no CR in the string as well
            if (code.size() == 1 || buff.find(10) == -1)
                call->addabuffer(code, false);
            else {
                for (long j = 0; j < code.size(); j++) {
                    b = code[j];
                    if (b[0] == 10) {
                        call->pos = call->handlingeditorline(false);
                        continue;
                    }
                    call->addabuffer(b, false);
                }
            }
        }
    }
    call->updateline = true;
}


#ifndef WIN32
void purejagmode(int argc, char *argv[]) {
    JAGEDITOR = new jag_editor;

    if (argc == 2) {
        string cmd = argv[1];
        if (cmd == "-h" || cmd == "-help" || cmd == "--help" || cmd == "--h") {
            cout << m_clear << m_home;
            cerr << m_red << "jag(작): micro text processor (version: experimental)" << m_current << endl;
            cerr << m_red << "Copyright 2019-present NAVER Corp." << m_current << endl;
            cerr << m_redital << "Ctrl-xh:" << m_current << " to display this help from within" << endl << endl;
            JAGEDITOR->displaythehelp(true);
            cerr << endl;
            exit(0);
        }

        if (JAGEDITOR->loadfile(cmd))
            JAGEDITOR->launchterminal(true);
        else
            JAGEDITOR->launchterminal(false);
    }
    else
        JAGEDITOR->launchterminal(false);

}
#endif

int main(int argc, char *argv[]) {
	string lnstr;

    current_thread_id = 0;
    
	string wh = TamguVersion();

	char* v = Getenv("TAMGULIBS");
    if (v != NULL)
        lnstr = v;
    
	if (lnstr == "") {
#ifdef WIN32
		WCHAR path[MAX_PATH];
		GetModuleFileNameW(NULL, path, MAX_PATH);
		wstring wpath = path;
		long posinpath = wpath.rfind('\\');
		if (posinpath != -1)
			wpath = wpath.substr(0, posinpath + 1);
		string spath;
		s_unicode_to_utf8(spath, wpath);
		spath = "TAMGULIBS=" + spath;
		_putenv(STR(spath));
#endif
#ifdef APPLE
		char path[2048];
		strcpy(path, "/usr/local/lib/tamgu");
		setenv("TAMGULIBS", path, 1);
#else
#ifdef UNIX
		if (getenv("TAMGULIBS") == NULL) {
			string upath = get_exe_path();
			setenv("TAMGULIBS", STR(upath), 1);
	}
#endif
#endif
}
	//No arguments , we launch the shell now
	if (argc <= 1) {
		JAGEDITOR = new tamgu_editor;
		JAGEDITOR->launchterminal(2);
	}

	lnstr = "";
	//Arguments to be analysed
    char piped = -1;
    //argv[1] is the file name
    string name = argv[1];
    string code = "";
    string codepipe = "";
    string codeafter = "";
    string codebefore = "";
    string filename;
    bool console = false;
    string predeclarations;
    bool initconsoleterminal = true;
    short idcode;

    string args;
    long i;
    for (i = 1; i < argc; i++) {
        args = argv[i];
        if (args[0] != '-') {
            filename = args;
            i++;
            if (i < argc) {
                args = argv[i];
                if (args == "-a") {
                    while (!cin.eof()) {
                        getline(cin, lnstr);
                        if (lnstr.size())
                            arguments.push_back(lnstr);
                        lnstr = "";
                    }
                    i++;
                }
                while (i < argc) {
                    arguments.push_back(argv[i]);
                    i++;
                }
            }
            break;
        }
        
        if (console) {
            cerr << "Unknown command:" << args << endl;
            exit(-1);
        }

        if (args == "-h") {
            displayhelp(wh);
            exit(-1);
        }

        if (args == "-i") {
            predeclarations = _variable_declaration;
            initconsoleterminal = false;
            continue;
        }

        if (args == "-lisp") {
            if (i < argc - 1)
                name = argv[i+1];
            lispmode = true;
            continue;
        }
        
        if (args == "-l" || args == "-e") {
            vector<string> names;
            if (JAGEDITOR == NULL)
                JAGEDITOR =  new tamgu_editor;

            if (i < argc - 1) {
                i++;
                JAGEDITOR->setpathname(argv[i]);
                names.push_back(argv[i]);
                i++;
                while (i < argc) {
                    lnstr = argv[i];
                    if (lnstr == "-l") {
                        i++;
                        if (i < argc)
                            names.push_back(argv[i]);
                        i++;
                        continue;
                    }
                    if (lnstr == "-a") {
                        lnstr = "";
                        while (!cin.eof()) {
                            getline(cin, lnstr);
                            if (lnstr.size())
                                arguments.push_back(lnstr);
                            lnstr = "";
                        }
                        i++;
                        continue;
                    }
                    break;
                }
                
                while (i < argc) {
                    arguments.push_back(argv[i]);
                    i++;
                }
                
                TamguLaunching();
                if (arguments.size())
                    TamguSetArguments(arguments);
                

                char init = 3;
                for (i = 0; i < names.size(); i++) {
                    if (!JAGEDITOR->loadfile(names[i])) {
                        if (i > 0)
                            exit(-1);
                        //This is the creation of a file
                        ((tamgu_editor*)JAGEDITOR)->addspace(names[0]);
                        
                        if (!initconsoleterminal) {
                            init = 4;
                            break;
                        }
                    }
                    else {
                        wstring w;
                        string line = "read ";
                        line += names[i];
                        sc_utf8_to_unicode(w, USTR(line), line.size());
                        JAGEDITOR->addcommandline(w);
                        line = "";
                    }
                }
                
                if (args == "-e")
                    JAGEDITOR->launchterminal(init);
                else
                    JAGEDITOR->launchterminal(0);
            }
            else {
                cerr << endl << "Missing filename" << endl << endl;
                exit(-1);
            }
        }

        #ifdef DOSOUTPUT
        if (args == "-dos") {
            Setdosoutput(true);
            continue;
        }
        #endif

        #ifdef WITHCONSOLE
        if (args == "-console") {
            console = true;
            continue;
        }
        #endif
        
        if (args == "-a") {
            piped = 1;
            while (!cin.eof()) {
                getline(cin, lnstr);
                if (lnstr.size())
                    arguments.push_back(lnstr);
                lnstr = "";
            }
        }
        else
            if (args == "-p" || args == "-pb" || args == "-pe") {
                piped = 2;
            }
            else
                if (args == "-c") {
                    piped = 0;
                }

        if (piped != -1) {
            if (i + 1 < argc) {
                if (argv[i + 1][0] != '-')
                    code = argv[++i];
            }

            if (code == "") {
                if (piped == 1) {
                    predeclarations = _variable_declaration;
                    initconsoleterminal = false;
                    continue;
                }
                
                cerr << "Error: missing code or unknown argument: " << args << endl;
                exit(-1);
            }
            
            if (args == "-p") {
                codepipe = code;
                continue;
            }
            
            if (args == "-pb") {
                codebefore = code;
                continue;
            }
            
            if (args == "-pe") {
                codeafter = code;
                continue;
            }
            break;
        }

        cerr << "Unknown command:" << args << endl;
        exit(-1);
    }

    TamguLaunching();
    TamguSetArguments(arguments);

    #ifdef WITHCONSOLE
    //If the console has been requested...
    if (console) {
        //We record the libraries...
        window_editor(filename);
        exit(0);
    }
    #endif

    //executing a file
    if (code == "") {
        if (name[0] == '-') {
            if (JAGEDITOR == NULL)
                JAGEDITOR =  new tamgu_editor;
            
            if (!initconsoleterminal) {
                TamguLaunching();
                if (arguments.size())
                    TamguSetArguments(arguments);

                idcode = TamguCompile(predeclarations, THEMAIN);
                TamguRun(idcode);
                initconsoleterminal = 1;
            }
            
            JAGEDITOR->launchterminal(initconsoleterminal);
        }

        name = Normalizefilename(name);
        try {
            idcode = TamguLoad(name);
        }
        catch (TamguRaiseError* err) {
            cerr << err->message << " at line: " << err->left << endl;
            delete err;
            exit(-1);
        }

        if (idcode == -1) {
            cerr << "Unknown filename: " << name << endl;
            exit(-1);
        }

        if (!TamguRun(idcode))
            cerr << TamguErrorMessage() << endl;

#ifdef GARBAGEFORDEBUG
        TamguExtinguish();
#endif
        exit(0);
    }

    if (lispmode) {
        predeclarations="(setq a true)\n\
(setq b true)\n\
(setq c true)\n\
(setq d (date))\n\
(setq i 0)\n\
(setq j 0)\n\
(setq k 0)\n\
(setq f 0.0)\n\
(setq g 0.0)\n\
(setq h 0.0)\n\
(setq s \"\")\n\
(setq t \"\")\n\
(setq u \"\")\n\
(setq m {})\n\
(setq v ())\n\
(setq _line 0)\n\
(setq _size 0)\n";

        predeclarations += "(setq l0 \"\") (setq l1 \"\") (setq l2 \"\") (setq l3 \"\") (setq l4 \"\") (setq l5 \"\") (setq l6 \"\") (setq l7 \"\") \
        (setq l8 \"\") (setq l9 \"\") (setq l10 \"\") (setq l11 \"\") (setq l12 \"\") (setq l13 \"\") (setq l14 \"\")\
        (setq l15 \"\") (setq l16 \"\") (setq l17 \"\") (setq l18 \"\") (setq l19 \"\") (setq l20 \"\") (setq l21 \"\") \
        (setq l22 \"\") (setq l23 \"\") (setq l24 \"\") (setq l25 \"\") (setq l26 \"\") (setq l27 \"\") (setq l28 \"\") \
        (setq l29 \"\") (setq l30 \"\") (setq l31 \"\") (setq l32 \"\") (setq l33 \"\") (setq l34 \"\") (setq l35 \"\") \
        (setq l36 \"\") (setq l37 \"\") (setq l38 \"\") (setq l39 \"\") (setq l40 \"\") (setq l41 \"\") (setq l42 \"\") \
        (setq l43 \"\") (setq l44 \"\") (setq l45 \"\") (setq l46 \"\") (setq l47 \"\") (setq l48 \"\") (setq l49 \"\") \
        (setq l50 \"\") (setq l51 \"\") (setq l52 \"\") (setq l53 \"\") (setq l54 \"\") (setq l55 \"\") (setq l56 \"\") \
        (setq l57 \"\") (setq l58 \"\") (setq l59 \"\") (setq l60 \"\") (setq l61 \"\") (setq l62 \"\") (setq l63 \"\") \
        (setq l64 \"\") (setq l65 \"\") (setq l66 \"\") (setq l67 \"\") (setq l68 \"\") (setq l69 \"\") (setq l70 \"\") \
        (setq l71 \"\") (setq l72 \"\") (setq l73 \"\") (setq l74 \"\") (setq l75 \"\") (setq l76 \"\") (setq l77 \"\") \
        (setq l78 \"\") (setq l79 \"\") (setq l80 \"\") (setq l81 \"\") (setq l82 \"\") (setq l83 \"\") (setq l84 \"\") \
        (setq l85 \"\") (setq l86 \"\") (setq l87 \"\") (setq l88 \"\") (setq l89 \"\") (setq l90 \"\") (setq l91 \"\") \
        (setq l92 \"\") (setq l93 \"\") (setq l94 \"\") (setq l95 \"\") (setq l96 \"\") (setq l97 \"\") (setq l98 \"\") (setq l99 \"\")";
        
        if (codebefore != "") {
            predeclarations += "\n";
            predeclarations += codebefore;
            predeclarations += "\n";
        }
        predeclarations += "\n(defun _Internal_Function(l) \n";
        Trim(codepipe);
        if (codepipe.back() != ';' && codepipe.find("{") == string::npos && codepipe.find("print") == string::npos) {
            codepipe="(println "+codepipe+")";
        }
        predeclarations += codepipe;
        predeclarations += "\n)\n";
        if (codeafter != "") {
            predeclarations += "\n\n(defun _Ending_Internal_Function()\n";
            predeclarations += codeafter;
            predeclarations += "\n)\n";
        }
        code = predeclarations;
        Setlispmode(true);
    }
    else {
        predeclarations = _variable_declaration;
        predeclarations += "int _size; int _line;";
        predeclarations += "self l0; self l1; self l2; self l3; self l4; self l5; self l6; self l7; self l8;\
        self l9; self l10; self l11; self l12; self l13; self l14; self l15; self l16; self l17; \
        self l18; self l19; self l20; self l21; self l22; self l23; self l24; self l25; self l26; \
        self l27; self l28; self l29; self l30; self l31; self l32; self l33; self l34; self l35; \
        self l36; self l37; self l38; self l39; self l40; self l41; self l42; self l43; self l44; \
        self l45; self l46; self l47; self l48; self l49; self l50; self l51; self l52; self l53; \
        self l54; self l55; self l56; self l57; self l58; self l59; self l60; self l61; self l62; \
        self l63; self l64; self l65; self l66; self l67; self l68; self l69; self l70; self l71; \
        self l72; self l73; self l74; self l75; self l76; self l77; self l78; self l79; self l80; \
        self l81; self l82; self l83; self l84; self l85; self l86; self l87; self l88; self l89; \
        self l90; self l91; self l92; self l93; self l94; self l95; self l96; self l97; self l98; self l99;";

        if (piped == 2) {
            if (codebefore != "") {
                predeclarations += "\n";
                predeclarations += codebefore;
                predeclarations += "\n";
            }
            predeclarations += "function _Internal_Function(string l) {\n";
            Trim(codepipe);
            if (codepipe.back() != ';' && codepipe.find("{") == string::npos && codepipe.find("print") == string::npos) {
                codepipe="println("+codepipe+");";
            }
            predeclarations += codepipe;
            predeclarations += "\n}";
            if (codeafter != "") {
                predeclarations += "\n\nfunction _Ending_Internal_Function() {\n";
                predeclarations += codeafter;
                predeclarations += "\n}";
            }
            code = predeclarations;
        }
        else
            code = predeclarations + code;
    }
    try {
        idcode = TamguCompile(code, THEMAIN);
    }
    catch (TamguRaiseError* err) {
        cerr << err->message << endl;
        delete err;
        exit(-1);
    }

    if (idcode == -1) {
        cerr << TamguErrorMessage() << endl;
        exit(-1);
    }
    
    if (!TamguRun(idcode))
        cerr << TamguErrorMessage() << endl;

    //just a bit of code...
    if (piped == 2) {
        TamguCode* tcode = TamguCodeSpace(idcode);
        
        Setlispmode(false);
        
        //We need to store the variable pointers...
        Tamgu* ret;
        short idname;

        vector<Tamgu*> vars;
        vector<short> names;
        
        Tamguint* _size = globalTamgu->Provideint();
        _size->Setreference();
        idname = globalTamgu->Getid("_size");
        globalTamgu->Replacevariable(0, idname, _size);

        Tamguint* _line = globalTamgu->Provideint(1);
        _line->Setreference();
        idname = globalTamgu->Getid("_line");
        globalTamgu->Replacevariable(0, idname, _line);
        
        for (i = 0; i < 100; i++) {
            predeclarations = "l"+convertfromnumber(i);
            idname = globalTamgu->Getid(predeclarations);
            if (lispmode) {
                names.push_back(idname);
                ret = globalTamgu->Getvariable(0, idname);
                vars.push_back(ret);
            }
            else {
                ret = globalTamgu->Provideself();
                ret->Putvalue(globalTamgu->Providestring(), 0);
                ret->Setreference();
                vars.push_back(ret);
                globalTamgu->Replacevariable(0, idname, ret);
            }
        }
        
        vector<Tamgu*> params;
        Tamgustring ts("");
        ts.reference = 100;
        params.push_back(&ts);
        lnstr = "";
        string cut=" ";
        long sz;
        long maxsize = 0;
        double v;
        short l;
        short thetype;
        //lnstr = "drwxr-xr-x    9 roux  1222672855    288  5 mar 10:24 tamguconsole";
        while (!cin.eof()) {
            getline(cin, lnstr);
            if (lnstr.size()) {
                ts.value =  lnstr;
                arguments.clear();
                arguments.push_back(lnstr);

                s_split(lnstr,cut,arguments, false);
                sz = arguments.size();
                _size->value = sz-1;
                
                sz = (sz < 100) ? sz : 100;
                if (sz > maxsize)
                    maxsize = sz;
                
                                
                for (i = 0; i < sz; i++) {
                    lnstr = arguments[i];
                    v = conversionfloathexa(STR(lnstr), l);
                    if (lispmode) {
                        if (l == lnstr.size()) {
                            if (lnstr.find('.') != -1)
                                thetype = a_float;
                            else
                                thetype = a_int;
                        }
                        else
                            thetype = a_string;
                        
                        if (thetype != vars[i]->Type()) {
                            switch (thetype) {
                                case a_float:
                                    ret = globalTamgu->Providefloat(v);
                                    break;
                                case a_int:
                                    ret = globalTamgu->Provideint(v);
                                    break;
                                default:
                                    ret = globalTamgu->Providewithstring(lnstr);
                                    break;
                            }
                            globalTamgu->Replacevariable(0, names[i], ret);
                            vars[i]->Resetreference();
                            ret->Setreference();
                            vars[i] = ret;
                        }
                        else
                            switch (thetype) {
                                case a_float:
                                    vars[i]->storevalue(v);
                                    break;
                                case a_int:
                                    vars[i]->storevalue((long)v);
                                    break;
                                default:
                                    vars[i]->storevalue(lnstr);
                                    break;
                            }
                    }
                    else {
                        if (l == lnstr.size()) {
                            if (lnstr.find('.') != -1)
                                vars[i]->storevalue(v);
                            else
                                vars[i]->storevalue((long)v);
                        }
                        else
                            vars[i]->storevalue(lnstr);
                    }
                }
                
                for (;i < maxsize; i++)
                    vars[i]->storevalue("");
                
                executionbreak = false;
                globalTamgu->running = true;
                globalTamgu->threadMODE = false;
                globalTamgu->isthreading = false;
                globalTamgu->threadcounter = 0;
                globalTamgu->Initarguments(arguments);
                ret = TamguExecute(tcode, "_Internal_Function", params);
                if (ret->isError()) {
                    cerr << ret->String() << endl;
                    exit(-1);
                }
                ret->Release();
                lnstr = "";
                _line->value++;
            }
        }
        if (codeafter != "") {
            params.clear();
            ret = TamguExecute(tcode, "_Ending_Internal_Function", params);
            ret->Release();
        }
    }
}

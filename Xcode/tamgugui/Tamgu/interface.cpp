/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : interface.cpp
 Date       : 2017/09/01
 Purpose    : Functions and Methods to communicate with Tamgu API
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include <stdio.h>

#include <set>
#include "tamgu.h"
#include "compilecode.h"
#include "x_node.h"
#include "globaltamgu.h"
#include "tamgustring.h"
#include "x_tokenize.h"
#include "tamgudebug.h"

const char* tmgelse ="else";

extern "C" {
    void Rappel(char threading, const char* txt);
    void displaydebug(const char* localvariables, const char* allvariables, const char* sstack, const char* filename, long currentline);
    const char* Inputtext(const char* msg);
    char WindowModeActivated(void);
    void Inittamgulibspath();
}

//------------------------------------------------------------------------------------------------DEBUG
static Debuginfo infodebug;

static string displaybuffer;

Tamgu* Debug_callback(vector<Tamgu*>& stack, short idthread, void* data) {
    Tamgu* res = infodebug.debugger(stack, idthread, data);
    if (res == aTRUE) {
        //We release our main debugger window...
        displaydebug(STR(infodebug.localvariables), STR(infodebug.allvariables), STR(infodebug.sstack), STR(infodebug.filename), infodebug.currentline);
        infodebug.loquet.Blocked();
    }
    return res;
}


//The following functions are called from within the GUI to handle debugging
static bool debugmode = false;
extern "C" {

    void Shortname(char v) {
        infodebug.shortname=v;
    }
    
    void addabreakpoint(const char* filename, long numline, char add) {
        infodebug.addabreakpoint(filename, numline, add);
    }
    
    const char* Getcurrentfilename(void) {
        return STR(infodebug.filename);
    }
    
    void clearallbreakpoints(void) {
        infodebug.clearbreakpoints();
    }
    
    void Setdebugmode(char d) {
        infodebug.displaymode = false;
        debugmode = d;
    }
    
    char NextLine(void) {
        return infodebug.next();
    }

    char Gotonextbreak(void) {
        return infodebug.gotonext();
    }
    
    char Getin(void) {
        return infodebug.getin();
    }
    
    char Getout(void) {
        return infodebug.getout();
    }
    
    char StopDebug(void) {
        infodebug.stopexecution();
        return 1;
    }
    
    
    char Gotoend(void) {
        return infodebug.gotoend();
    }
    
    void Blocked(void) {
        infodebug.loquet.Blocked();
    }
    
    void Released(void) {
        if (executionbreak)
            return;
        
        infodebug.loquet.Released();
    }
    
}

//------------------------------------------------------------------------------------------------RUN AND COMPILE

static void display_result(string s, void* object) {
    string e;
    s_latin_to_utf8(e, USTR(s), s.size());
    displaybuffer+=e;
}

static void send_result(string s, void* object) {
    //we cannot have both console and windows accessed...
    s_latin_to_utf8(displaybuffer, USTR(s), s.size());
    if (WindowModeActivated())
        return;
    
    char threading=*((bool*)object);
    Rappel(threading, STR(displaybuffer));
}

static void final_display() {
    Rappel(1, STR(displaybuffer));
}

//In the case of a mac GUI, we need to call a specific version of kget, which is based on alert...
//This function is only called if we are not implementing a FLTK GUI, in that case we use ProcEditor (in tamgufltk.cxx)
Tamgu* ProcMacEditor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    string label("Enter your text:");
    if (callfunc->Size())
        label = callfunc->Evaluate(0, aNULL, idthread)->String();
    
    
    
    const char* buff = Inputtext(STR(label));
    if (buff == NULL)
        return aNULL;
    
    string res=buff;
    return globalTamgu->Providestring(res);
}

extern "C" {
    
    char IsRunning(void) {
        return TamguRunning();
    }
    
    ///Users/roux/Documents/GitHub/TAMGU/bin/Examples/word2vec
    void settamgupath(const char* homepath, const char* v) {
        char* start=(char*)v;
        
        while (start[0]<=32) start++;
        long lg=strlen(start)-1;
        while (start[lg]<=32)lg--;
        start[lg+1]=0;

        setenv("TAMGULIBS",start,1);
        char path[4096];
        sprintf(path,"%s/.tamgu",homepath);
        ofstream savepath(path);
        savepath.write(start,strlen(start));
        savepath.write("\n",1);
        savepath.close();
    }
    
    void inittamgupath(const char* homepath) {
        char path[4096];
        sprintf(path,"%s/.tamgu",homepath);
        ifstream getpath(path);
        if (getpath.fail()) {
            getpath.close();
            ofstream savepath(path);
            strcpy(path,"/usr/local/lib/tamgu");
            savepath.write(path,strlen(path));
            savepath.write("\n",1);
            savepath.close();
            setenv("TAMGULIBS",path,1);
            return;
        }
        memset(path,0,4096);
        getpath.read(path,4096);
        char* start=path;
        
        while (start[0]<=32) start++;
        long lg=strlen(start)-1;
        while (start[lg]<=32)lg--;
        start[lg+1]=0;
        setenv("TAMGULIBS",start,1);
    }
    
    const char* Listing(void) {
        displaybuffer=TamguListing();
        return STR(displaybuffer);
    }
    
    const char* Readfile(const char* path) {
        ifstream f(path, openMode);
        if (f.fail())
            return NULL;
        displaybuffer="";
        string line;
        while (!f.eof()) {
            getline(f,line);
            displaybuffer+=line;
            displaybuffer += "\n";
        }
        s_latin_to_utf8(line, USTR(displaybuffer), displaybuffer.size());
        displaybuffer=line;
        cr_normalise(displaybuffer);
        return STR(displaybuffer);
    }
    
    char StopExecution(void) {
        return TamguStop();
    }
    
    void InitialisationDisplay(short id, char th) {
        static bool threading;
        
        threading=th;
        displaybuffer="";
        TamguGlobal* g = GlobalTamgu(id);
        if (g!=NULL) {
            g->displayfunction = send_result;
            g->displayobject = &threading;
            g->doubledisplay = false;
        }
    }
    
    void Initdisplay(short id) {
        displaybuffer="";
        TamguGlobal* g = GlobalTamgu(id);
        if (g!=NULL) {
            g->displayfunction = display_result;
            g->doubledisplay = false;
        }
    }
    
    const char* Getdisplay(void) {
        static string buff;
        
        buff=displaybuffer;
        buff+="\r";
        displaybuffer="";
        return STR(buff);
    }
    
    void CleanGlobal(void) {
        TamguDeleteGlobal(0);
    }
    
    int Compilecode(const char* cde, const char* filename, char console) {
        Inittamgulibspath();
        
        if (cde==NULL)
            return -1;
        
        if (!TamguSelectglobal(0)) {
            TamguCreateGlobal();
        }
        
        if (debugmode == true) {
            globalTamgu->Setdebugmode(true);
            globalTamgu->Setdebugfunction(Debug_callback);
        }
        else {
            globalTamgu->Setdebugmode(false);
            globalTamgu->Setdebugfunction(NULL);
        }
        
        int idcode=-1;
        string code=cde;
        try {
            globalTamgu->threads[0].currentinstruction = NULL;
            idcode = TamguCompile(code, filename, console);
            if (idcode==-1)
                displaybuffer=TamguErrorMessage();
        }
        catch (TamguRaiseError* err) {
            displaybuffer = err->message;
            globalTamgu->Cleanerror(0);
            return -1;
        }
        
        return idcode;
    }
    
    long CurrentLine(void) {
        return TamguCurrentLine();
    }
    
    const char* Currentfilename(void) {
        static string filename;
        filename=TamguCurrentFilename();
        return filename.c_str();
    }
    
    char Run(short idcode) {
#ifdef MULTIGLOBALTAMGU
        //In that case, especially when we are running from within a thread, we need to activate our local thread version of globalTamgu...
        TamguSelectglobal(0);
#endif
        bool finaldisplay = false;
        if (WindowModeActivated()) {
            finaldisplay = true;
        }
        else
            globalTamgu->RecordOneProcedure("kget", ProcMacEditor, P_NONE | P_ONE);

        if (debugmode == true) {
            globalTamgu->Setdebugmode(true);
            globalTamgu->Setdebugfunction(Debug_callback);
            infodebug.clearall();
        }
        
        TamguCode* a = globalTamgu->Getcode(idcode);
        if (a == NULL)
            return false;
        globalTamgu->spaceid = idcode;
        
        a->Run(debugmode);
        
        if (globalTamgu->GenuineError(0)) {
            displaybuffer=TamguErrorMessage();
            return false;
        }
        
        if (finaldisplay)
            final_display();

        return true;
    }
    
    char** Getnames(int* nb) {
        bool tobecleaned=false;
        if (!TamguSelectglobal(0)) {
            TamguCreateGlobal();
            tobecleaned=true;
        }
        
        vector<string> vs;
        TamguAllObjects(vs);
        if (tobecleaned)
            TamguDeleteGlobal(0);
        
        if (vs.size() == 0)
            return NULL;
        
        char** liste=new char*[vs.size()];
        for (int i = 0; i < vs.size(); i++) {
            liste[i] = new char[vs[i].size()+1];
            strcpy(liste[i], STR(vs[i]));
        }
        *nb= (int)vs.size();
        return liste;
    }
    
    void TamguFinalClean(void) {
        TamguExtinguish();
    }

    long indentationVirtuel(char* cr, char* acc) {
        if (cr == NULL)
            return 0;

        string codestr(cr);
        return VirtualIndentation(codestr);
    }
    
    const char* lindentation(char* basecode, int blancs) {
        static string codeindente;
        
        string codestr = basecode;
        Trimright(codestr);
        codestr+="\n";
        bool lisp = false;
        if (codestr[0] == '(' && codestr[1] == ')')
            lisp = true;

        cr_normalise(codestr);
        codeindente = "";
        IndentationCode(codestr, codeindente, lisp);
        if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
            cr_normalise(codeindente);
        codeindente += "\n";
        return codeindente.c_str();
    }
    
    
    void Keywwords(hmap<wstring,bool>& names) {
        bool tobecleaned=false;
        if (!TamguSelectglobal(0)) {
            TamguCreateGlobal();
            tobecleaned=true;
        }
        
        vector<string> vs;
        TamguAllObjects(vs);
        if (tobecleaned)
            TamguDeleteGlobal(0);
        
        
        wstring token;
        for (int i = 0; i < vs.size(); i++) {
            sc_utf8_to_unicode(token, USTR(vs[i]), vs[i].size());
            names[token]=true;
        }
    }
    
    void Keywords(std::set<wstring>& names) {
        bool tobecleaned=false;
        if (!TamguSelectglobal(0)) {
            TamguCreateGlobal();
            tobecleaned=true;
        }
        
        vector<string> vs;
        TamguAllObjects(vs);
        if (tobecleaned)
            TamguDeleteGlobal(0);
        
        wstring w;
        for (int i = 0; i < vs.size(); i++) {
            sc_utf8_to_unicode(w, USTR(vs[i]), vs[i].size());
            names.insert(w);
        }
    }
    
    long* colorparser(uint16_t* text, long from, long upto) {
        static std::set<wstring> keys;
        static vector<long> limits;
        static x_wcoloringrule xr;
        static bool init=false;
        
        if (!init) {
            init=true;
            Keywords(keys);
        }
        
        long i;
        wstring txt;
        for (long i = 0; i < upto; i++)
            txt += (wchar_t)text[i];
        
        xr.tokenize(txt, true);
        
        char type;
        long gauche = 0,droite = 0;
        long sz=xr.stack.size();
        wstring sub;
        limits.clear();
        
        for (i=0;i<sz;i++) {
            type=xr.stacktype[i];
            if (!type)
                continue;
            gauche=xr.cpos[i];
            
            if (type == 5) {
                if (xr.stack[i][1] == '/') {
                    if (gauche < from || gauche > upto)
                        continue;
                }
            }
            else {
                if (type != 3 && (gauche < from || gauche > upto))
                    continue;
            }
            
            //The strings in the mac GUI are encoded in UTF16
            droite = xr.stack[i].size();
                        
            switch(type) {
                case 1:
                case 2:
                case 3:
                    //strings
                    limits.push_back(type);
                    limits.push_back(gauche);
                    limits.push_back(droite);
                    break;
                case 4://regular token
                    if (keys.find(xr.stack[i])!=keys.end()) {
                        limits.push_back(5);
                        limits.push_back(gauche);
                        limits.push_back(droite);
                        break;
                    }
                    break;
                case 5://comments
                    limits.push_back(7);
                    limits.push_back(gauche);
                    limits.push_back(droite);
                    break;
                 case 10:
                    //special variables: #d+ ?label $d+
                    limits.push_back(8);
                    limits.push_back(gauche);
                    limits.push_back(droite);
                    break;
                case 11:
                    // .method(
                    limits.push_back(4);
                    limits.push_back(gauche+1);
                    limits.push_back(droite-2);
                    break;
                case 12:
                    // function(
                    sub=xr.stack[i].substr(0,xr.stack[i].size()-1);
                    if (keys.find(sub)!=keys.end()) {
                        limits.push_back(5);
                        limits.push_back(gauche);
                        limits.push_back(droite-1);
                        break;
                    }
                    limits.push_back(6);
                    limits.push_back(gauche);
                    limits.push_back(droite-1);
                    break;
                case 13:
                    // <function
                    sub=xr.stack[i].substr(1,xr.stack[i].size()-1);
                    if (keys.find(sub)!=keys.end()) {
                        limits.push_back(5);
                        limits.push_back(gauche);
                        limits.push_back(droite-1);
                        break;
                    }
                    limits.push_back(6);
                    limits.push_back(gauche+1);
                    limits.push_back(droite);
                    break;
                case 14:
                    //annotation lexicon head rule
                    sub=xr.stack[i];
                    limits.push_back(6);
                    limits.push_back(gauche);
                    limits.push_back(droite);
                    break;
            }
        }
        
        long* res=new long[limits.size()+1];
        for (i=0;i<limits.size();i++)
            res[i]=limits[i];
        res[limits.size()]=-1;
        return res;
    }
    
    void deletion(long* l) {
        delete[] l;
    }
}

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : globaltamgu.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/


//This code is only used in the context of a GUI...
#include "tamgu.h"
#include "compilecode.h"
#include "codeparse.h"

static string _fullcode;
//----------------------------------------------------------------------------------
static vector<TamguGlobal*> globals;
static ThreadLock globalLock;

class JtamguGlobalLocking {
private:
    std::lock_guard<std::recursive_mutex>* g;
    
public:
    JtamguGlobalLocking() {
        g = new std::lock_guard<std::recursive_mutex>(*globalLock.lock);
    }
    
    ~JtamguGlobalLocking() {
        delete g;
    }
};

static int Storeglobal(TamguGlobal* g) {
    JtamguGlobalLocking _lock;
	for (int idx = 0; idx < globals.size(); idx++) {
		if (globals[idx] == NULL) {
			g->idglobal = idx;
			globals[idx] = g;
			return idx;
		}
	}
	
	g->idglobal = globals.size();
	globals.push_back(g);
	return g->idglobal;
}

Exporting int TamguCreateGlobal(long nbthreads) {
	_fullcode = "";
	TamguGlobal* global = new TamguGlobal(nbthreads);
	global->linereference = 1;
    globalTamgu = global;
	return Storeglobal(global);
}

Exporting bool TamguDeleteGlobal(int idx) {
    JtamguGlobalLocking _lock;
	if (idx <0 || idx >= globals.size() || globals[idx] == NULL)
		return false;

    bool cleanit=false;
    if (globalTamgu == globals[idx])
        cleanit = true;

	delete globals[idx];
	globals[idx] = NULL;
    
    if (cleanit)
        globalTamgu = NULL;
        
 	return true;
}

Exporting bool TamguSelectglobal(int idx) {
    JtamguGlobalLocking _lock;
	if (idx <0 || idx >= globals.size() || globals[idx] == NULL)
		return false;

	globals[idx]->SetThreadid();
	globalTamgu = globals[idx];
	return true;
}

Exporting bool TamguReleaseglobal(int idx) {
    JtamguGlobalLocking _lock;
	if (idx <0 || idx >= globals.size() || globals[idx] == NULL)
		return false;

	globals[idx]->ResetThreadid();
	return true;
}

Exporting void TamguCleanAllGlobals() {
    JtamguGlobalLocking _lock;
	for (int idx = 0; idx < globals.size(); idx++) {
		if (globals[idx] != NULL)
			delete globals[idx];
	}
	globals.clear();
}

Exporting TamguGlobal* GlobalTamgu(int idx) {
    JtamguGlobalLocking _lock;
    if (idx <0 || idx >= globals.size() || globals[idx] == NULL)
        return NULL;
    
    return globals[idx];
}

//----------------------------------------------------------------------------------

Exporting TamguGlobal* TamguCreate(long nbthreads) {
	if (globalTamgu != NULL)
		return NULL;

	_fullcode = "";
	globalTamgu = new TamguGlobal(nbthreads, true);
	globalTamgu->linereference = 1;
	Storeglobal(globalTamgu);
	return globalTamgu;
}


Exporting bool TamguExtinguish() {
	if (globalTamgu != NULL) {
		if (globalTamgu->isRunning())
			return false;
        
        JtamguGlobalLocking _lock;
        globals[globalTamgu->idglobal] = NULL;
		delete globalTamgu;		
		globalTamgu = NULL;		
	}
	return true;
}

Exporting void TamguAllObjects(vector<string>& vs) {
	if (globalTamgu!=NULL)
		globalTamgu->TamguAllObjects(vs);
}

Exporting short TamguLoad(string filename) {
	if (globalTamgu == NULL)
		return -1;

	//The system variables...
	globalTamgu->SystemInitialization(filename);

	TamguCode* a = globalTamgu->Loadfile(filename);
	if (a == NULL)
		return -1;
	return a->idcode;
}

Exporting short TamguCompile(string& codeinit, string filename, bool dsp) {
    static x_reading xr;
    static bnf_tamgu bnf;

	TamguCode* a = globalTamgu->Getcode(0);
	if (a == NULL)
		a = globalTamgu->GetNewCodeSpace(filename);
		
	//The system variables...
	globalTamgu->SystemInitialization(filename);
    globalTamgu->Cleanerror(0);
    
	bool add = true;
	string code;	
	if (dsp) {
		code = Trim(codeinit);
		if (code.size() == 0)
			return -1;

        xr.tokenize(code);
        if (!xr.size())
            return -1;
        
        bool puredisplay=true;
        if (xr.stack[xr.stack.size()-1] == ";") {
            puredisplay = false;
        }
        else {
            for (long pcode=0; pcode<xr.stack.size(); pcode++) {
                if (xr.stack[pcode] == "{") {
                    puredisplay = false;
                    break;
                }
            }
        }
        
		if (puredisplay) {
			if (code[0] == '<' && code.back() == '>') {
				//Is it a function description

				x_node* x = bnf.x_parsing(&xr, FULL);
				if (x == NULL) {
					globalTamgu->Returnerror("Cannot parse this string", 0);
					return -1;
				}
				x_node* xn = x;
				while (xn != NULL && xn->token != "telque" && xn->nodes.size()) xn = xn->nodes[0];

				if (xn != NULL) {
					for (int i = 0; i<xn->nodes.size(); i++) {
						if (xn->nodes[i]->token == "taskell") {
							dsp = false;
							break;
						}
					}
				}
				delete x;
			}

			if (dsp) {
				add = false;
				code = "println";
				code += "(";
				code += codeinit;
				code += ");";
			}
		}
	}
	else
		code = codeinit;

	if (!a->Compile(code))
		return -1;
	
	if (add) {
		_fullcode += code;
		_fullcode += Endl;
	}

	return a->idcode;
}


Exporting string TamguListing() {
	return _fullcode;
}

Exporting wstring TamguUListing() {
    wstring s;
    s_utf8_to_unicode(s, USTR(_fullcode), _fullcode.size());
    return s;
}

Exporting void TamguSetCode(wstring& code) {
    sc_unicode_to_utf8(_fullcode, code);
}


Exporting void TamguSetCode(string& code) {
    _fullcode = code;
}

Exporting size_t TamguCompile(string& code, short idcode) {
	TamguCode* a = globalTamgu->Getcode(idcode);
	if (a == NULL || !a->Compile(code))
		return -1;
	return a->firstinstruction;
}

Exporting bool TamguLoading(short idcode) {
	TamguCode* a = globalTamgu->Getcode(idcode);
	if (a == NULL)
		return false;
	globalTamgu->spaceid = idcode;

	a->Loading();

	if (globalTamgu->GenuineError(0))
		return false;
	return true;
}

Exporting bool TamguRun(short idcode, bool glock) {
	TamguCode* a = globalTamgu->Getcode(idcode);
	if (a == NULL)
		return false;
	globalTamgu->spaceid = idcode;

	a->Run(glock);

	if (globalTamgu->GenuineError(0))
		return false;
	return true;
}

Exporting short TamguLastCodeSpace() {
	return (short)globalTamgu->spaceid;
}

Exporting TamguCode* TamguCurrentSpace() {
	if (globalTamgu->spaceid == -1)
		return NULL;
	return globalTamgu->spaces[globalTamgu->spaceid];
}

Exporting TamguCode* TamguCodeSpace(short idcode) {
    JtamguGlobalLocking _lock;
	if (globalTamgu == NULL || idcode >= globalTamgu->spaces.size() || idcode < 0)
		return NULL;

	return globalTamgu->spaces[idcode];
}

Exporting string TamguCurrentFilename() {
	if (globalTamgu == NULL)
		return "";

	TamguCode* c = globalTamgu->Getcurrentcode();
	if (c == NULL)
		return "";
	return c->filename;
}

Exporting long TamguCurrentLine() {
	if (globalTamgu == NULL)
		return 0;

	TamguCode* c = globalTamgu->Getcurrentcode();
	if (c == NULL)
		return 0;
	return c->Getcurrentline();
}

Exporting void TamguSetinitline(long i) {
    if (globalTamgu==NULL)
        return;
    globalTamgu->linereference = i;
}

static ostringstream* gl_os=NULL;
Exporting void TamguSetos() {
    if (globalTamgu==NULL)
        return;
    if (gl_os==NULL)
        delete gl_os;
    gl_os=new std::ostringstream;
    globalTamgu->os = gl_os;
}

Exporting string TamguDisplayValue() {
    if (globalTamgu==NULL || gl_os==NULL)
        return "";
    return gl_os->str();
}

Exporting string TamguErrorMessage() {
	if (globalTamgu->errorraised[0] != NULL) {
		string filename = globalTamgu->Getcurrentfile();
		stringstream message;		
		message << globalTamgu->errorraised[0]->String() << " at " << globalTamgu->Getinstructionline(0);
		string msg = message.str();
		if (msg.find(filename) == string::npos)
			message << " in " << filename;

		return message.str();
	}
	return globalTamgu->threads[0].message.str();
}

Exporting TamguGlobal* GlobalTamgu() {
	return globalTamgu;
}

Exporting bool TamguRunning() {
	if (globalTamgu == NULL)
		return false;

	return globalTamgu->isRunning();
}

Exporting short TamguCurrentThreadId() {
	return globalTamgu->GetThreadid();
}

Exporting string TamguIndentation(string& codestr,string blanc) {	
	vector <long> iblancs;
	vector<string> vargs;
	cr_normalise(codestr);
	v_split_indent(codestr, vargs);
	string codeindente = "";
	IndentationCode(codeindente, vargs, iblancs, (int)blanc.size(), true, "else", "elif", "if");
	Trimright(codeindente);
	codeindente += "\n";
	if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
		cr_normalise(codeindente);
	return codeindente.c_str();
}


Exporting bool TamguStop() {
	if (globalTamgu == NULL)
		return true;
    if (globalTamgu->isRunning()) {
        executionbreak = true;
        globalTamgu->Releasevariables();
    }
	return true;
}

Exporting void TamguDisplayFunction(DisplayFunctionCall call, void* o) {
	if (globalTamgu == NULL)
		return;
	globalTamgu->displayfunction = call;
	globalTamgu->displayobject = o;
}


Exporting void TamguSetArguments(string args) {
	if (globalTamgu == NULL)
		return;
	globalTamgu->Setarguments(args);
}


Exporting void TamguSetArguments(vector<string>& args) {
	if (globalTamgu == NULL)
		return;
	globalTamgu->arguments = args;
}

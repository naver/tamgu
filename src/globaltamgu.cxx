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
#include "tamgustring.h"

static string _fullcode;
//----------------------------------------------------------------------------------
bool TamguDeleteInThread(short idglobal);
Exchanging vector<TamguGlobal*> globalTamguPool;
static ThreadLock globalLock;
static long max_instances = 0;

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

static short Storeglobal(TamguGlobal* g) {
    JtamguGlobalLocking _lock;
    for (short idglobal = 0; idglobal < globalTamguPool.size(); idglobal++) {
        if (globalTamguPool[idglobal] == NULL) {
            g->idglobal = idglobal;
            globalTamguPool[idglobal] = g;
            return idglobal;
        }
    }
    
    g->idglobal = globalTamguPool.size();
    globalTamguPool.push_back(g);
    return g->idglobal;
}

Exporting long Countinstances() {
    JtamguGlobalLocking _lock;
    long nb = 0;
    for (short idglobal = 0; idglobal < globalTamguPool.size(); idglobal++) {
        if (globalTamguPool[idglobal] != NULL) {
            nb += globalTamguPool[idglobal]->spaces.size();
        }
    }
    
    max_instances = std::max(max_instances, nb);
    return nb;
}

Exporting long Maxcountinstances() {
    return max_instances;
}


Exporting short TamguCreateGlobal(long nbthreads) {
    _fullcode = "";
    TamguGlobal* global = new TamguGlobal(nbthreads, true);
    global->linereference = 1;
    return Storeglobal(global);
}

Exporting bool TamguDeleteGlobal(short idglobal) {
    TamguGlobal* g = NULL;
    
    {
        JtamguGlobalLocking _lock;
        if (idglobal <0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
            return false;
        
        g = globalTamguPool[idglobal];
        globalTamguPool[idglobal] = NULL;
    }
    
    if (g != NULL)
        delete g;
    
    return true;
}

//We detect all globalTamgu objects that have been idle for more than diff seconds
Exporting void TamguIdle(long diff, vector<long>& idles) {
    
    std::chrono::system_clock::time_point currenttime = std::chrono::system_clock::now();
    auto add = std::chrono::seconds(diff);
    
    currenttime -= add;
    
    JtamguGlobalLocking _lock;
    for (short idglobal = 0; idglobal < globalTamguPool.size(); idglobal++) {
        if (globalTamguPool[idglobal] != NULL) {
            if (globalTamguPool[idglobal]->last_execution < currenttime)
                idles.push_back(idglobal);
        }
    }
}

Exporting bool TamguDeleteGlobalinThread(short idglobal) {
    JtamguGlobalLocking _lock;
    return TamguDeleteInThread(idglobal);
}

Exporting string TamguEventString(short idglobal) {
    if (idglobal <0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
        return "NONE";
    TamguGlobal* g = globalTamguPool[idglobal];
    if (g->event_variable != NULL)
        return g->event_variable->String();
    return "NONE";
}

Exporting bool TamguDeleteSpace(short idglobal, short idcode) {
    JtamguGlobalLocking _lock;
    if (idglobal <0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
        return false;
    TamguGlobal* g = globalTamguPool[idglobal];
    if (g->spaces[idcode] != NULL) {
        delete g->spaces[idcode];
        g->spaces[idcode] = NULL;
        return true;
    }
    return false;
}

Exporting bool TamguSelectglobal(short idglobal) {
    JtamguGlobalLocking _lock;
    if (idglobal <0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
        return false;

    globalTamguPool[idglobal]->SetThreadid();
    globalTamgu = globalTamguPool[idglobal];
    return true;
}

Exporting bool TamguReleaseglobal(short idglobal) {
    JtamguGlobalLocking _lock;
    if (idglobal <0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
        return false;

    globalTamguPool[idglobal]->ResetThreadid();
    return true;
}

Exporting void TamguCleanAllGlobals() {
    JtamguGlobalLocking _lock;
    for (short idglobal = 0; idglobal < globalTamguPool.size(); idglobal++) {
        if (globalTamguPool[idglobal] != NULL)
            delete globalTamguPool[idglobal];
    }
    globalConstants.clean();
    globalTamguPool.clear();
    clean_utf8_handler();
}

Exporting TamguGlobal* getGlobalTamgu(short idglobal) {
    JtamguGlobalLocking _lock;
    if (idglobal <0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
        return NULL;
    
    return globalTamguPool[idglobal];
}

//----------------------------------------------------------------------------------

Exporting TamguGlobal* TamguCreate(long nbthreads) {
    if (globalTamgu != NULL)
        return NULL;

    _fullcode = "";
    TamguGlobal* global = new TamguGlobal(nbthreads, true);
    global->linereference = 1;
    Storeglobal(global);
    return global;
}


Exporting bool TamguExtinguish() {
    if (globalTamgu != NULL) {
        if (globalTamgu->isRunning())
            return false;
        
        JtamguGlobalLocking _lock;
        short idglobal = globalTamgu->idglobal;
        delete globalTamgu;
        globalTamguPool[idglobal] = NULL;
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

Exporting short TamguCompile(string& codeinit, string filename, char dsp) {
    static tokenizer_result<string> xr;
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
        globalTamgu->store_in_code_lines = true;
        globalTamgu->codelines.clear();
        
        globalTamgu->codelines.push_back("");
        v_split(codeinit, "\n", globalTamgu->codelines);

        code = Trim(codeinit);
        if (code.size() == 0)
            return -1;

        globalTamgu->tamgu_tokenizer.tokenize<string>(code, xr);
        if (!xr.size())
            return -1;
        
        bool puredisplay=true;
        if (xr.stack.back() == ";") {
            puredisplay = false;
        }
        else {
            if (xr.stack[0] != "\\(") {
                for (long pcode=0; pcode<xr.stack.size(); pcode++) {
                    if (xr.stack[pcode] == "{") {
                        puredisplay = false;
                        break;
                    }
                }
            }
        }
        
        if (puredisplay) {
            if (code[0] == '<' && code.back() == '>') {
                //Is it a function description

                x_node* x = bnf.x_parsing(&xr, FULL);
                if (x == NULL) {
                    globalTamgu->Returnerror(e_cannot_parse_this, 0);
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

            if (dsp == 2) {
                code = "return";
                code += "(";
                code += codeinit;
                code += ");";
            }
            else if (dsp) {
                add = false;
                if (isLispmode()) {
                    _fullcode += codeinit;
                    _fullcode += Endl;
                    code = "(println ";
                    code += codeinit;
                    code += ")";
                }
                else {
                    code = "println";
                    code += "(";
                    code += codeinit;
                    code += ");";
                }
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

Exporting short TamguCompileMain(string& codeinit, string filename) {
    TamguCode* a = globalTamgu->Getcode(0);
    if (a == NULL) {
        a = globalTamgu->GetNewCodeSpace(filename);
    }
    
    //The system variables...
    globalTamgu->SystemInitialization(filename);
    globalTamgu->Cleanerror(0);
    
    //This is
    globalTamgu->store_in_code_lines = true;
    globalTamgu->codelines.clear();
    
    globalTamgu->codelines.push_back("");
    v_split(codeinit, "\n", globalTamgu->codelines);

    if (!a->Compile(codeinit))
        return -1;
    
    return a->idcode;
}


Exporting short TamguCompileWithNewSpace(string& codeinit, string filename) {
    if (filename == "") {
        long sz = globalTamgu->pathnames.size();
        ostringstream os;
        os << "%FILE_INTERNAL" << sz;
        filename = os.str();
    }
    else {
        if (globalTamgu->pathnames.find(filename) != globalTamgu->pathnames.end()) {
            long sz = globalTamgu->pathnames.size();
            ostringstream os;
            os << filename << "_" << sz;
            filename = os.str();
        }
    }
    
    TamguCode* a = globalTamgu->GetNewCodeSpace(filename);

    if (!a->Compile(codeinit))
        return -1;
    
    return a->idcode;
}

Exporting short TamguCompileNewSpace(string& codeinit, string filename) {
    TamguCode* a = globalTamgu->GetNewCodeSpace(filename);

    if (!a->Compile(codeinit))
        return -1;
    
    return a->idcode;
}

Exporting bool TamguCheckCompile(string& codeinit, string filename, vector<TamguFullError*>& errors) {
    filename = NormalizeFileName(filename);
    TamguGlobal* g = globalTamgu;
    globalTamgu = new TamguGlobal(1);
    TamguCode* a = globalTamgu->GetNewCodeSpace(filename);
    a->CompileFull(codeinit, errors);
    delete globalTamgu;
    globalTamgu = g;
    return !errors.size();
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

Exporting void TamguAddCode(wstring& code) {
    string cde;
    sc_unicode_to_utf8(cde, code);
    _fullcode += cde;
    _fullcode += Endl;
}

Exporting void TamguAddCode(string& code) {
    _fullcode += code;
    _fullcode += Endl;
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

    a->Executing();

    if (globalTamgu->GenuineError(0))
        return false;
    return true;
}

Exporting long TamguLastInstruction(short idcode) {
    TamguCode* code = globalTamgu->Getcode(idcode);
    if (code == NULL)
        return -1;
    
    return code->mainframe.instructions.size();
}

Exporting Tamgu* TamguEval(short idcode, long begininstruction) {
    TamguCode* code = globalTamgu->Getcode(idcode);
    if (code == NULL)
        return NULL;
    
    globalTamgu->spaceid = idcode;

    Tamgu* a = code->Eval(begininstruction);

    if (globalTamgu->GenuineError(0))
        return NULL;
    
    return a;
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

Exporting TamguCode* TamguCodeSpace(short idglobal, short idcode) {
    JtamguGlobalLocking _lock;
    if (idglobal < 0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
        return NULL;
    
    TamguGlobal* g = globalTamguPool[idglobal];
    if (idcode < g->spaces.size() && g->spaces[idcode] != NULL)
        return g->spaces[idcode];
    
    return NULL;
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

Exporting bool TamguErrorVector(TamguGlobal* global,
                                std::vector<string>& errorlines,
                                std::vector<string>& files,
                                std::vector<long>& linenumbers) {
    if (global->errorraised[0] != NULL) {
        string filename = global->Getcurrentfile();
        long line = globalTamgu->Getcurrentline();
        stringstream message;
        global->Getstack(errorlines, files, linenumbers);
        errorlines.push_back(global->errorraised[0]->String());
        linenumbers.push_back(line);
        files.push_back(filename);
        return true;
    }
    return false;

}

Exporting string TamguErrorMessage() {
    if (globalTamgu->event_variable != NULL)
        return globalTamgu->event_variable->String();
    
    if (globalTamgu->errorraised[0] != NULL) {
        string filename = globalTamgu->Getcurrentfile();
        long line = globalTamgu->Getcurrentline();
        stringstream message;

        globalTamgu->Getstack(message);
        message << globalTamgu->errorraised[0]->String() << " at " << line;
        if (!globalTamgu->store_in_code_lines) {
            if (filename != "")
                message << " in " << filename;
        }
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

Exporting void TamguSpaceInit(string filename) {
    TamguCode* a = globalTamgu->Getcode(0);
    if (a == NULL)
        a = globalTamgu->GetNewCodeSpace(filename);
    
        //The system variables...
    globalTamgu->SystemInitialization(filename);
    globalTamgu->Cleanerror(0);
    globalTamgu->Pushstack(&a->mainframe);
}

Exporting string TamguIndentation(string& codestr,string blanc) {
    bool lisp = false;
    if (codestr[0] == '(' && codestr[1] == ')') {
        lisp = true;
    }

    cr_normalise(codestr);
    string codeindente;
    IndentationCode(codestr, codeindente, lisp, true);
    Trimright(codeindente);
    codeindente += "\n";
    if (codeindente.find("/@") != string::npos || codeindente.find("@\"") != string::npos)
        cr_normalise(codeindente);
    return codeindente.c_str();
}

Exporting bool TamguStop(short idglobal) {
    if (idglobal <0 || idglobal >= globalTamguPool.size() || globalTamguPool[idglobal] == NULL)
        return false;
    
    TamguGlobal* global = globalTamguPool[idglobal];
    if (global->isRunning()) {
        global->executionbreak = true;
        global->Releasevariables();
    }
    
    return true;
}

Exporting void TamguStopAll() {
    JtamguGlobalLocking _lock;
    for (short idglobal = 0; idglobal < globalTamguPool.size(); idglobal++) {
        if (globalTamguPool[idglobal] == NULL) {
            TamguStop(idglobal);
        }
    }
}

Exporting bool TamguStop() {
    if (globalTamgu == NULL)
        return true;
    
    if (globalTamgu->isRunning()) {
        globalTamgu->executionbreak = true;
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


void jsoning(stringstream& res, string value) {
    if (value.find("\"") == -1)
        res << "\"" << value << "\"";
    else {
        string v = s_replacestrings(value, "\"", "\\\"");
        res << "\"" << v << "\"";
    }
}

Tamgu* ProcAllDefinitions(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

Exporting string TamguAllObjectWithInfo(string code) {
    if (code != "") {
        TamguCode* a = globalTamgu->Getcode(0);
        if (a == NULL)
            a = globalTamgu->GetNewCodeSpace("FLOW");
        vector<TamguFullError*> errors;
        a->CompileFull(code, errors);
        for (auto& a: errors)
            delete a;
        
    }
    
    Tamgu* m = ProcAllDefinitions(aNULL,0,NULL);
    string json = m->JSonString();
    m->Release();
    return json;
}

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgu.cxx
 Date       : 2017/09/01
 Purpose    : The place of all initializations...
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */


#include "tamgu.h"
#include "compilecode.h"
#include "tamgutamgu.h"
#include "constobjects.h"
#include "tamgubyte.h"
#include "tamguint.h"
#include "tamgufloat.h"
#include "tamgustring.h"
#include "tamguustring.h"
#include "tamgufraction.h"
#include "tamgudecimal.h"
#include "instructions.h"
#include "tamgumapss.h"
#include "tamgutreemap.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgusvector.h"
#include "tamguuvector.h"
#include "tamguprimemap.h"
#include "predicate.h"
#include "tamguautomaton.h"
#include "tamguannotator.h"
#include <memory>
#include "vecte.h"
#include "tamgutaskell.h"
#include "tamgulisp.h"
#include "tamgucomplex.h"
#include "tamgusocket.h"
#include "tamgudate.h"
//----------------------------------------------------------------------------------
const char* tamgu_version = "Tamgu 1.2024.10.28.17";

#ifdef UNIX
#include <sys/resource.h>
#endif

extern "C" {
Exporting const char* TamguVersion(void) {
    return tamgu_version;
}
}

//specific hack to compile with visual 2017
#if (_MSC_VER >= 1900)
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
#endif

//----------------------------------------------------------------------------------
#ifdef MULTIGLOBALTAMGU
localthread TamguGlobal* globalTamgu = NULL;
#else
Exchanging TamguGlobal* globalTamgu = NULL;
#endif
//----------------------------------------------------------------------------------
vector<string> TamguGlobal::arguments;

Tamgu* aNULL = NULL;

void GlobalConstants::clean() {
    if (gNULL != NULL) {
        delete gITERNULL;
        delete gUNIVERSAL;
        delete gTRUE;
        delete gFALSE;
        delete gEMPTYLISP;
        delete gEMPTYSTRING;
        delete gEMPTYUSTRING;
        delete gBREAK;
        delete gCONTINUE;
        delete gRETURN;
        delete gPIPE;
        delete gNOELEMENT;
        delete gDEFAULT;
        delete gEND;
        delete gRAISEERROR;
        delete gBREAKFALSE;
        delete gBREAKTRUE;
        delete gBREAKZERO;
        delete gBREAKONE;
        delete gASSIGNMENT;
        delete gFAIL;
        delete gPREDICATETRUE;
        delete gTERMINAL;
        delete gCUTFALSE;
        delete gCUT;
        delete gSTOP;
        delete gHASKELL;
        delete gNULLDECLARATION;
        delete gNULLLet;
        delete gNOTHING;
        
        gNULL= NULL;
        gUNIVERSAL= NULL;
        gTRUE= NULL;
        gFALSE= NULL;
        
        gMINUSONE= NULL;
        gZEROPOINTZERO= NULL;
        gZERO= NULL;
        gONE= NULL;
        
        gEMPTYLISP= NULL;
        gEMPTYSTRING= NULL;
        gEMPTYUSTRING= NULL;
        gBREAK= NULL;
        gCONTINUE= NULL;
        gRETURN= NULL;
        gPIPE= NULL;
        gNOELEMENT= NULL;
        gDEFAULT= NULL;
        gEND= NULL;
        gRAISEERROR= NULL;
        gBREAKFALSE= NULL;
        gBREAKTRUE= NULL;
        gBREAKZERO= NULL;
        gBREAKONE= NULL;
        
        gASSIGNMENT= NULL;
        gITERNULL= NULL;
        gFAIL= NULL;
        gPREDICATETRUE = NULL;
        gTERMINAL= NULL;
        gCUTFALSE= NULL;
        gCUT= NULL;
        gSTOP= NULL;
        gHASKELL= NULL;
        gNULLDECLARATION= NULL;
        gNULLLet= NULL;
        gNOTHING= NULL;
    }
}

GlobalConstants globalConstants;

Tamgu* booleantamgu[2];

//----------------------------------------------------------------------------------
// Debug area, to detect potential memory leaks...
#if defined(GARBAGESCAN) || defined(GARBAGEINDEBUG)
ThreadLock _garbaging;
static bool keep_track_of_garbage = false;
static vector<Tamgu*> _tamgu_garbage;
static long _tamgu_garbage_id = 0;

Exporting void Set_keep_track_garbage(bool v) {
    keep_track_of_garbage = v;
    if (v) {
        _tamgu_garbage.clear();
        _tamgu_garbage_id = 0;
    }
}

bool Is_keep_track_garbage() {
    return keep_track_of_garbage;
}

Exporting Tamgu::Tamgu() {
    idtracker = -1;
    investigate = is_none;
    
    if (keep_track_of_garbage) {
        Locking _lock(_garbaging);
        
#ifdef GARBAGEINDEBUG
        iddebug = _tamgu_garbage.size();
        _tamgu_garbage.push_back(this);
        _tamgu_garbage_id = iddebug;
#else
        long sz = _tamgu_garbage.size();
        if (_tamgu_garbage_id < sz && _tamgu_garbage[_tamgu_garbage_id] == NULL) {
            iddebug = _tamgu_garbage_id;
            _tamgu_garbage[iddebug] = this;
            _tamgu_garbage_id++;
            //We find the next NULL slot in _tamgu_garbage...
            while (_tamgu_garbage_id < sz && _tamgu_garbage[_tamgu_garbage_id] != NULL) _tamgu_garbage_id++;
        }
        else {
            iddebug = sz;
            _tamgu_garbage.push_back(this);
            _tamgu_garbage_id = iddebug;
        }
#endif
    }
    globalTamgu->Storeingarbageif_add(this);
}

Exporting Tamgu::~Tamgu() {
    Locking _lock(_garbaging);
    if (idtracker != -1)
        globalTamgu->RemoveFromTracker(idtracker);
    
    if (keep_track_of_garbage) {
        Locking _lock(_garbaging);
        _tamgu_garbage[iddebug] = NULL;
        if (_tamgu_garbage_id > iddebug)
            _tamgu_garbage_id = iddebug;
    }
    
    globalTamgu->Removefromlocalgarbageif_add(-1,-1,this);
}

void Garbaging(vector<Tamgu*>& issues, vector<long>& idissues) {
    issues.clear();
    idissues.clear();
    Tamgu* g;
    Locking _lock(_garbaging);
    for (size_t i = 0; i < _tamgu_garbage.size(); i++) {
        g = _tamgu_garbage[i];
        if (g != NULL) {
            issues.push_back(g);
            idissues.push_back(i);
        }
    }
}

Exporting void Garbaging(hmap<std::string, long>& issues) {
    issues.clear();
    Tamgu* g;
    string key;
    char buffer[100];
    Locking _lock(_garbaging);
    for (size_t i = 0; i < _tamgu_garbage.size(); i++) {
        g = _tamgu_garbage[i];
        if (g != NULL) {
            key = g->Typestring();
            if (key == "") {
                sprintf_s(buffer, 100, "%d", g->Type());
                key = buffer;
            }
            issues[key] += 1;
        }
    }
}

Exporting void Garbaging(hmap<long, long>& issues) {
    issues.clear();
    Tamgu* g;
    Locking _lock(_garbaging);
    for (size_t i = 0; i < _tamgu_garbage.size(); i++) {
        g = _tamgu_garbage[i];
        if (g != NULL) {
            if (g->idtracker == -1 && g->Reference() == 0) {
                issues[g->Type()] += 1;
            }
        }
    }
}

#else
//This is a mode that is only activated when _eval is called...
Exporting void Set_keep_track_garbage(bool v) {}
Exporting void Garbaging(hmap<std::string, long>& issues) {}
Exporting void Garbaging(hmap<long, long>& issues) {}

Tamgu::Tamgu() {
    idtracker = -1;
    investigate = is_none;
    globalTamgu->Storeingarbageif_add(this);
}

Tamgu::~Tamgu() {
    globalTamgu->Removefromlocalgarbageif_add(-1,-1,this);
}

#endif

void TamguGlobal::Removefromlocalgarbage(short idthread, long i, Tamgu* a) {
    //If has already been cleaned or it was not created in this thread
    if (a->idtracker == -555)
        return;
    
    //if idthread == -1, it comes from ~Tamgu directly
    if (idthread == -1) {
        idthread = GetThreadid();
        //We look for a place to clean it...
        threads[idthread].Cleaningarbage(a);
    }
    else
        threads[idthread].localgarbage[i] = NULL;
    
    if (a->idtracker != -1)
        RemoveFromTracker(a->idtracker);
    a->idtracker = -555;
    
}

void ThreadStruct::set_base_address(uintptr_t b) {
#ifdef WIN32
    ULONG_PTR lowLimit;
    ULONG_PTR highLimit;
    
    GetCurrentThreadStackLimits(&lowLimit, &highLimit);
    max_inner_stack = fabs(highLimit - lowLimit);
    max_inner_stack -= max_inner_stack * 0.002;
#else
#ifdef TAMGUWASM
    max_inner_stack = 8360000;
#else
    struct rlimit rl;
    getrlimit(RLIMIT_STACK, &rl);
    max_inner_stack = rl.rlim_cur;
    max_inner_stack -= max_inner_stack * 0.002;
#endif
#endif
    baseaddress = b;
}

void ThreadStruct::Cleanfromgarbageposition(Tamgu* declaration,
                                            short idthread,
                                            long p,
                                            Tamgu* keep,
                                            long lastrecorded,
                                            long maxrecorded) {
    Tamgu* e;
    long i;
    
    if (maxrecorded == -1)
        maxrecorded = localgarbage.size();
    
    for (i = p; i < maxrecorded; i++) {
        e = localgarbage[i];
        if (e == keep || e == NULL || e->isConst() || (e->idtracker != -1 && e->idtracker < lastrecorded))
            continue;
        
        if (declaration->FindAndClean(e)) {
            globalTamgu->Removefromlocalgarbage(idthread,i,  e);
            delete e;
        }
    }
    
    if (p)
        localgarbage.erase(localgarbage.begin()+p, localgarbage.end());
    else
        localgarbage.clear();
    in_eval = false;
}

//----------------------------------------------------------------------------------
Exporting long ThreadLock::ids = 0;
//----------------------------------------------------------------------------------
// Locked variables...

Exporting void LockedThread::Blocked() {
    std::unique_lock<std::mutex> lck(mtx);
    released = 1;
    lock.wait(lck);
    released = 2;
}

Exporting void LockedThread::Released() {
    if (released == 1)
        lock.notify_all();
}

Exporting LockedThread::~LockedThread() {
    if (released == 1)
        lock.notify_all();
}

//----------------------------------------------------------------------------------

TamguRaiseError::TamguRaiseError(stringstream& mes, string file, size_t l, size_t r, size_t l_p, size_t r_p) {
    message = mes.str();
    
    filename = file;
    left = l;
    right = r;
    left_pos = l_p;
    right_pos = r_p;
}

//----------------------------------------------------------------------------------
ThreadStruct::ThreadStruct() : stack(1000), variables(false) {
    max_inner_stack = 0;
    baseaddress = 0;
    fstcompanion = NULL;
    prologstack = 0;
    returnvalue = NULL;
    previousinstruction=NULL;
    currentinstruction = NULL;
    used = false;
    parentthreads = -1;
    handle = _GETTHREADID();
    nbjoined = 0;
    in_eval = false;
    embedded_try = 0;
}

void ThreadStruct::Update(short idthread) {
    variables = globalTamgu->threads[idthread].variables;
}

void ThreadStruct::Clear() {
    stackinstructions.clear();
    embedded_try = 0;
    in_eval = false;
    localgarbage.clear();
    nbjoined = 0;
    prologstack = 0;
    stack.clear();
    stacklisp.clear();
    debugstack.clear();
    fibres.clear();
    message.str("");
    variables.clear();
    returnvalue = aNULL;
    used = false;
}

threadhandle ThreadStruct::Initialization() {
    stackinstructions.clear();
    embedded_try = 0;
    nbjoined = 0;
    prologstack = 0;
    returnvalue = aNULL;
    used = true;
    
    locks.clear();
    
    handle = _GETTHREADID();
    message.str("");
    debugstack.clear();
    previousinstruction=NULL;
    currentinstruction = NULL;
    return handle;
}

void TamguGlobal::Clearknowledgebase() {
    hmap<short, vector<TamguPredicate*> >::iterator itk;
    Tamgu* k;
    for (itk = knowledgebase.begin(); itk != knowledgebase.end(); itk++) {
        for (size_t i = 0; i < itk->second.size(); i++) {
            k = itk->second[i];
            if (elements_to_delete.find(k) == elements_to_delete.end()) {
                if (k->Candelete()) {
                    elements_to_delete.insert(k);
                }
            }
        }
    }
    
    knowledgebase.clear();
    knowledgebase_on_first.clear();
    knowledgebase_on_second.clear();
    knowledgebase_on_third.clear();
}

//----------------------------------------------------------------------------------
TamguGlobal::TamguGlobal(long nb, bool setglobal) :
idSymbols(false), methods(false), compatibilities(false), strictcompatibilities(false),
operator_strings(false), terms(false), booleanlocks(true), tracked(NULL, true), trackerslots(-1, true) {
    
    TamguGlobal* globalSave = globalTamgu;
    globalTamgu = this;
    
    short_string = 0;
    last_execution = std::chrono::system_clock::now();
    
#ifdef TAMGULOOSEARGUMENTCOMPATIBILITIES
    loosecompability = true;
#else
    loosecompability = false;
#endif
    
    global_constants = &globalConstants;
    
    handler_on_utf8 = create_utf8_handler();
    
    event_variable = NULL;
    add_to_tamgu_garbage = false;
    number_of_current_eval = 0;
    
    threadcounter = 0;
    store_in_code_lines = false;
    
    SetThreadid();
    
    ThreadLock::ids = 10;
    
    booleanlocks[a_int] = new ThreadLock;
    booleanlocks[a_float] = new ThreadLock;
    booleanlocks[a_decimal] = new ThreadLock;
    booleanlocks[a_long] = new ThreadLock;
    booleanlocks[a_short] = new ThreadLock;
    booleanlocks[a_byte] = new ThreadLock;
    booleanlocks[a_boolean] = new ThreadLock;
    
    maxrange = 100000;
    idglobal = 0;
    
    conceptfunction = NULL;
    rolefunction = NULL;
    propertyfunction = NULL;
    
    threadMODE = false;
    isthreading = false;
        
    debugmode = false;
    currentbnf = NULL;
    maxthreads = nb;
    erroronkey = false;
    windowmode = false;
    spaceid = -1;
    running = false;
    waitingonfalse = false;
    executionbreak = false;
    
    
    threads = new ThreadStruct[maxthreads];
    errors = new bool[maxthreads];
    errorraised = new TamguError*[maxthreads];
    
    long i;
    for (i = 0; i < maxthreads; i++) {
        threads[i].idthread = i;
        errors[i] = false;
        errorraised[i] = NULL;
    }
    
    predicateContainer = NULL;
    
    linereference = 0;
    lineerror = -1;
    
    kstd = NULL;
    kerr = NULL;
    
    RecordConstantNames();
    RecordSystemVariables();
    
    RecordCompileFunctions();
    RecordObjects();
    RecordContainers();
    RecordProcedures();
    RecordPredicates();
    RecordPredicateFunctions();
    
    RecordMathFunctions();
    RecordCompatibilities();
    //---------------------------------
    //We use it as a way to deflect printing to a string...
    stringbuffer = NULL;
    stringbuffererror = NULL;
    os = &cout;
    doubledisplay = false;
    displayfunction = NULL;
    displayobject = NULL;
    lispe = NULL;
    //---------------------------------
    debugfunction = NULL;
    debugdata = NULL;
    terminationfunction = NULL;
    //---------------------------------
    gTheAnnotationRules=NULL;
    gAutomatons=NULL;
    //---------------------------------
    long mx = 50;
    //---------------------------------
    for (i = 0; i < mx; i++) {
        lispreservoire.push_back(new Tamgulisp(i));
        intreservoire.push_back(new Tamguintbuff(i));
        longreservoire.push_back(new Tamgulongbuff(i));
        floatreservoire.push_back(new Tamgufloatbuff(i));
        stringreservoire.push_back(new Tamgustringbuff(i));
        ustringreservoire.push_back(new Tamguustringbuff(i));
        declarationreservoire.push_back(new TamguDeclarationLocal(i));
        declarationcleanreservoire.push_back(new TamguDeclarationAutoClean(i));
        primemapreservoire.push_back(new Tamguprimemapbuff(i));
    }
    
    lispidx = 0;
    intidx = 0;
    longidx = 0;
    slfidx = 0;
    floatidx = 0;
    stringidx = 0;
    ustringidx = 0;
    declarationidx = 0;
    declarationcleanidx = 0;
    
    mapidx = 0;
    primemapidx = 0;
    treemapidx = 0;
    mapssidx = 0;
    vectoridx = 0;
    ivectoridx = 0;
    fvectoridx = 0;
    svectoridx = 0;
    uvectoridx = 0;
    
    //---------------------------------
    mx >>= 3;
    for (long i = 0; i < mx; i++) {
        slfreservoire.push_back(new Tamguselfbuff(i));
        vectorreservoire.push_back(new Tamguvectorbuff(i));
        mapreservoire.push_back(new Tamgumapbuff(i));
        treemapreservoire.push_back(new Tamgutreemapbuff(i));
        mapssreservoire.push_back(new Tamgumapssbuff(i));
        ivectorreservoire.push_back(new Tamguivectorbuff(i));
        fvectorreservoire.push_back(new Tamgufvectorbuff(i));
        svectorreservoire.push_back(new Tamgusvectorbuff(i));
        uvectorreservoire.push_back(new Tamguuvectorbuff(i));
    }
    
    if (!setglobal)
        globalTamgu = globalSave;
}

void Tamgu::Deletion(std::set<Tamgu*>& elements_to_delete) {
    if (idtracker != -1)
        return;
    
    if (Candelete())
        elements_to_delete.insert(this);
    
    if (isObjectContainer() && Size() && Type() != a_java_vector) {
        Tamgu* k;
        TamguIteration* itr = Newiteration(true);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            k = itr->Value();
            if (k!= NULL && elements_to_delete.find(k) == elements_to_delete.end())
                k->Deletion(elements_to_delete);
        }
        delete itr;
    }
    
    Finalclean(elements_to_delete);
}

TamguGlobal::~TamguGlobal() {
    TamguGlobal* resetToNUll = globalTamgu;
    if (globalTamgu != this)
        globalTamgu = this;
    
    if (event_variable != NULL)
        event_variable->Resetreference();
    
    if (stringbuffer != NULL)
        stringbuffer->Resetreference();
    if (stringbuffererror != NULL)
        stringbuffererror->Resetreference();
    
    stringbuffer = NULL;
    stringbuffererror = NULL;
    event_variable = NULL;
    
    for (auto& a: integer_pool) {
        if (a.second->idtracker == -1)
            elements_to_delete.insert(a.second);
    }
    
    for (auto& a: float_pool) {
        if (a.second->idtracker == -1)
            elements_to_delete.insert(a.second);
    }
    
    for (auto& a : string_pool) {
        if (a.second->idtracker == -1)
            elements_to_delete.insert(a.second);
    }
    
    string_pool.clear();
    
    
    clear_pattern();
    clear_automates();
    
    delete[] threads;
    delete[] errors;
    delete[] errorraised;
    
    size_t i;
    
    delete booleanlocks[a_int];
    delete booleanlocks[a_float];
    delete booleanlocks[a_decimal];
    delete booleanlocks[a_long];
    delete booleanlocks[a_short];
    delete booleanlocks[a_byte];
    delete booleanlocks[a_boolean];
    
    for (auto & it : rules)
        delete it.second;
    
    for (i = 0; i < spaces.size(); i++) {
        if (spaces[i] != NULL)
            delete spaces[i];
    }
    
    Clearknowledgebase();
    spaces.clear();
    
    //For the sockets, we do it in advance
    tracked.clean(elements_to_delete);
    
    for (i = 0; i < mapreservoire.size(); i++)
        delete mapreservoire[i];
    
    for (i = 0; i < mapssreservoire.size(); i++)
        delete mapssreservoire[i];
    
    for (i = 0; i < vectorreservoire.size(); i++)
        delete vectorreservoire[i];
    
    for (i = 0; i < ivectorreservoire.size(); i++)
        delete ivectorreservoire[i];
    for (i = 0; i < fvectorreservoire.size(); i++)
        delete fvectorreservoire[i];
    for (i = 0; i < svectorreservoire.size(); i++)
        delete svectorreservoire[i];
    for (i = 0; i < uvectorreservoire.size(); i++)
        delete uvectorreservoire[i];
    
    for (i = 0; i < slfreservoire.size(); i++)
        delete slfreservoire[i];
    
    for (i = 0; i < intreservoire.size(); i++)
        delete intreservoire[i];
    
    for (i = 0; i < floatreservoire.size(); i++)
        delete floatreservoire[i];
    
    for (i = 0; i < stringreservoire.size(); i++)
        delete stringreservoire[i];
    
    for (i = 0; i < ustringreservoire.size(); i++)
        delete ustringreservoire[i];
    
    for (i = 0; i < lispreservoire.size(); i++)
        delete lispreservoire[i];
    
    for (i = 0; i < longreservoire.size(); i++)
        delete longreservoire[i];
    
    for (i = 0; i < treemapreservoire.size(); i++)
        delete treemapreservoire[i];
    
    for (i = 0; i < primemapreservoire.size(); i++)
        delete primemapreservoire[i];
    
    for (i = 0; i < declarationreservoire.size(); i++)
        delete declarationreservoire[i];
    
    for (i = 0; i < declarationcleanreservoire.size(); i++)
        delete declarationcleanreservoire[i];
    
    for (i = 0; i < pvireservoire.size(); i++)
        delete pvireservoire[i];
    
    for (auto k : elements_to_delete)
        delete k;
    
    atomic_iterator<string, ThreadLock*> itlocktables(locktables);
    atomic_iterator<string, LockedThread*> itwaitstrings(waitstrings);
    atomic_iterator<long, LockedThread*> itthreadvariables(threadvariables);
    
    while (!itlocktables.end()) {
        delete itlocktables.second;
        itlocktables.next();
    }
    
    while (!itwaitstrings.end()) {
        delete itwaitstrings.second;
        itwaitstrings.next();
    }
    
    while (!itthreadvariables.end()) {
        delete itthreadvariables.second;
        itthreadvariables.next();
    }
    
    pathnames.clear();
    locktables.clear();
    waitstrings.clear();
    idSymbols.clear();
    symbolIds.clear();
    lispactions.clear();
    string_operators.clear();
    operator_strings.clear();
    atanOperatorMath.clear();
    parseFunctions.clear();
    
    newInstance.clear();
    methods.clear();
    allmethods.clear();
    framemethods.clear();
    functions.clear();
    procedures.clear();
    commons.clear();
    
    arities.clear();
    frames.clear();
    
    systems.clear();
    
    compatibilities.clear();
    fullcompatibilities.clear();
    strictcompatibilities.clear();
    hierarchy.clear();
    
    if (gAutomatons!=NULL)
        delete gAutomatons;
    
    globalTamgu = resetToNUll;
    
#ifdef GARBAGEINDEBUG
    vector<Tamgu*> issues;
    vector<long> idissues;
    Garbaging(issues, idissues);
    if (issues.size())
        cerr << e_no_fully_cleaned << issues.size() << endl;
    else
        cerr << "No dangling values. All has been cleaned." << endl;
#endif
}

static void getname(string& f) {
#ifdef WIN32
    long pos = f.rfind("\\");
#else
    long pos = f.rfind("/");
#endif
    
    if (pos != -1)
        f = f.substr(pos + 1, f.size() - pos - 1);
}

Exporting void TamguGlobal::Getdebuginfo(string& localvariables, string& allvariables, string& stackstring, bool shortname, long sz, short idthread) {
    
    vector<Tamgu*>& stack = threads[idthread].debugstack;
    
    localvariables = "";
    allvariables = "";
    stackstring = "";
    
    short idcode = stack.back()->Currentspace();
    if (idcode == -1)
        return;
    
    debugmode=false;
    
    short fileid = stack.back()->Currentfile();
    string filename = Getfilename(fileid);
    long currentline = stack.back()->Currentline();
    getname(filename);
    
    VECTE<Tamgu*>& threadstack = threads[idthread].stack;
    long i;
    stringstream sstack;
    sstack << currentline << " in " << filename << "\n";
    
    string funcname;
    
    Tamgu* a =threads[idthread].currentinstruction;
    threads[idthread].currentinstruction = NULL;
    if (a!=NULL) {
        vector<short> vars;
        a->ScanVariables(vars);
        
        if (threads[idthread].previousinstruction!=NULL)
            threads[idthread].previousinstruction->ScanVariables(vars);
        
        threads[idthread].previousinstruction=a;
        
        hmap<short,bool> m;
        for (i=0;i<vars.size();i++)
            m[vars[i]]=true;
        vars.clear();
        for (const auto& it : m)
            vars.push_back(it.first);
        
        stringstream var;
        
        for (size_t j = 0; j < vars.size(); j++) {
            a = Getdefinition(vars[j], idthread, aNULL);
            var << Getsymbol(vars[j]) << " = ";
            if (!shortname)
                var << a->String();
            else {
                string s = a->String();
                if (s.size() >= sz) {
                    s = s.substr(0, sz);
                    s += "...";
                    //we count both ' and "
                    int dq = 0;
                    int sq = 0;
                    for (long i = 0; i < s.size(); i++) {
                        if (s[i] == '\'')
                            sq++;
                        else
                            if (s[i] == '\"')
                                dq++;
                    }
                    if (dq && (dq % 2) != 0)
                        s += "\"";
                    else
                        if (sq && (sq % 2) != 0)
                            s += "'";
                }
                var << s;
            }
            
            var << "\n";
        }
        
        if (var.str().size())
            localvariables += var.str();
    }
    
    Tamgu* stackline;
    for (i = (long)threadstack.size() - 1; i >= 0; i--) {
        stringstream var;
        vector<short> vars;
        stackline=threadstack[i];
        funcname = "";
        short nm = stackline->Name();
        
        if (nm > 0)
            funcname = Getsymbol(nm);
        
        
        stackline->Variables(vars);
        for (size_t j = 0; j < vars.size(); j++) {
            //a = stackline->Declaration(vars[j]);
            a = globalTamgu->Getavariable(idthread, vars[j]);
            var << Getsymbol(vars[j]) << " = ";
            if (!shortname)
                var << a->String();
            else {
                string s = a->String();
                Trimright(s);
                if (s.size() >= sz) {
                    s = s.substr(0, sz);
                    s += "...";
                    //we count both ' and "
                    int dq = 0;
                    int sq = 0;
                    for (long l = 0; l < s.size(); l++) {
                        if (s[l] == '\'')
                            sq++;
                        else
                            if (s[l] == '\"')
                                dq++;
                    }
                    if (dq && (dq % 2) != 0)
                        s += "\"";
                    else
                        if (sq && (sq % 2) != 0)
                            s += "'";
                }
                var << s;
            }
            
            if (nm > 0)
                var << " //" << funcname;
            
            var << "\n";
        }
        
        if (var.str().size())
            allvariables += var.str();
    }
    
    for (i = (long)stack.size() - 1; i >= 0; i--) {
        stackline=stack[i];
        if (!stackline->isaFunction())
            continue;
        
        short nm = stackline->Name();
        
        if (nm > 0) {
            funcname = Getsymbol(nm);
            a = Getdefinition(nm, idthread);
            
            short idc = stackline->Currentspace();
            if (idc == -1)
                sstack << "\n";
            else {
                stringstream aline;
                
                fileid = stackline->Currentfile();
                filename = Getfilename(fileid);
                getname(filename);
                long cline = stackline->Currentline();
                aline << cline << " in " << filename << " (" << funcname << ")\n";
                sstack << aline.str();
            }
        }
    }
    
    stackstring = sstack.str();
    debugmode=true;
}

void TamguGlobal::Releasevariables() {
    Locking _lock(_threadvariableslock);
    //This method is called when we need to stop an Tamgu program
    //We then need to remove any wait on variables still active...
    atomic_iterator<string, LockedThread*> itwaitstrings(waitstrings);
    atomic_iterator<long, LockedThread*> itthreadvariables(threadvariables);
    
    while (!itwaitstrings.end()) {
        itwaitstrings.second->Released();
        itwaitstrings.next();
    }
    
    while (!itthreadvariables.end()) {
        itthreadvariables.second->Released();
        itthreadvariables.next();
    }
}

void TamguGlobal::Triggeronfalse(Tamgu* a) {
    if (!waitingonfalse)
        return;
    
    if (a->idtracker != -1 && threadvariables.check(a->idtracker)) {
        if (!a->Protectedboolean())
            threadvariables.getpointer(a->idtracker)->Released();
    }
}

void TamguGlobal::Setarguments(string args) {
    arguments.clear();
    if (args.size() != 0)
        v_split(args, " ", arguments);
}

Exporting void TamguGlobal::RecordMathFunctions() {
    mathFunctions[Getid("abs")] = fabs;
    mathFunctions[Getid("acos")] = acos;
    mathFunctions[Getid("acosh")] = acosh;
    mathFunctions[Getid("asin")] = asin;
    mathFunctions[Getid("asinh")] = asinh;
    mathFunctions[Getid("atan")] = atan;
    mathFunctions[Getid("atanh")] = atanh;
    mathFunctions[Getid("cbrt")] = cbrt;
    mathFunctions[Getid("∛")] = cbrt;
    mathFunctions[Getid("cos")] = cos;
    mathFunctions[Getid("cosh")] = cosh;
    mathFunctions[Getid("erf")] = erf;
    mathFunctions[Getid("erfc")] = erfc;
    mathFunctions[Getid("exp")] = exp;
    mathFunctions[Getid("exp2")] = exp2;
    mathFunctions[Getid("expm1")] = expm1;
    mathFunctions[Getid("floor")] = floor;
    mathFunctions[Getid("lgamma")] = lgamma;
    mathFunctions[Getid("ln")] = log;
    mathFunctions[Getid("log")] = log10;
    mathFunctions[Getid("log1p")] = log1p;
    mathFunctions[Getid("log2")] = log2;
    mathFunctions[Getid("logb")] = logb;
    mathFunctions[Getid("nearbyint")] = nearbyint;
    mathFunctions[Getid("rint")] = rint;
    mathFunctions[Getid("round")] = round;
    mathFunctions[Getid("sin")] = sin;
    mathFunctions[Getid("sinh")] = sinh;
    mathFunctions[Getid("sqrt")] = sqrt;
    mathFunctions[Getid("tan")] = tan;
    mathFunctions[Getid("tanh")] = tanh;
    mathFunctions[Getid("tgamma")] = tgamma;
    mathFunctions[Getid("trunc")] = trunc;
    mathFunctions[Getid("√")] = sqrt;
}

void TamguGlobal::Cleanerror(short idthread) {
    TamguError* err = errorraised[idthread];
    if (err != NULL && err != aRAISEERROR && err != aEND) {
        RemoveFromTracker(err->idtracker);
        delete err;
    }
    errorraised[idthread] = NULL;
    errors[idthread] = false;
}


Exporting Tamgu* TamguGlobal::Errorobject(short idthread) {
    return (executionbreak)?aEND:(errors[idthread])?errorraised[idthread]:aNULL;
}

string TamguGlobal::Errorstring(short idthread) {
    if (executionbreak)
        return "Stop";
    
    if (errors[idthread])
        return errorraised[idthread]->String();
    
    return "";
}

Exporting Tamgu* TamguGlobal::Returnerror(Tamgu* err, short idthread) {
    if (errors[idthread])
        return errorraised[idthread];
    
    if (!threads[idthread].embedded_try)
        stack_error = threads[idthread].stackinstructions;
    
    errorraised[idthread] = (TamguError*)err;
    errors[idthread] = true;
    return err;
}

Exporting Tamgu* TamguGlobal::Returnerror(string msgerr) {
    short idthread = GetThreadid();
    if (!errors[idthread]) {
        if (!threads[idthread].embedded_try)
            stack_error = threads[idthread].stackinstructions;
        
        errorraised[idthread] = new TamguError(msgerr);
        errors[idthread] = true;
    }
    return errorraised[idthread];
}

Exporting Tamgu* TamguGlobal::Returnerror(string msgerr, short idthread) {
    if (!errors[idthread]) {
        if (!threads[idthread].embedded_try)
            stack_error = threads[idthread].stackinstructions;
        
        errorraised[idthread] = new TamguError(msgerr);
        errors[idthread] = true;
    }
    return errorraised[idthread];
}

Exporting Tamgu* TamguGlobal::Returnerror(const char* msgerr, short idthread) {
    if (!errors[idthread]) {
        if (!threads[idthread].embedded_try)
            stack_error = threads[idthread].stackinstructions;
        
        errorraised[idthread] = new TamguError(msgerr);
        errors[idthread] = true;
    }
    return errorraised[idthread];
}

Exporting Tamgu* TamguGlobal::Returnstackoverflow(short idthread) {
    if (!errors[idthread]) {
        if (!threads[idthread].embedded_try)
            stack_error = threads[idthread].stackinstructions;
        
        errorraised[idthread] = new TamguError(e_stack_overflow);
        errors[idthread] = true;
    }
    return errorraised[idthread];
}

Exporting void TamguGlobal::Seterror(string msgerr, short idthread) {
    if (!errors[idthread]) {
        if (!threads[idthread].embedded_try)
            stack_error = threads[idthread].stackinstructions;
        
        errorraised[idthread] = new TamguError(msgerr);
        errors[idthread] = true;
    }
}

void ThreadStruct::pushtracked(Tamgu* a) {
    currentinstruction = (a->isTracked())?a:currentinstruction;
    stack.push_back(a);
}

Tamgu* TamguGlobal::GetTopFrame() {
    for (long i=threads[0].stack.size()-1; i>=0; i--) {
        if (threads[0].stack[i]->isFrame())
            return threads[0].stack[i];
    }
    return NULL;
}

Tamgu* ProcCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

void TamguGlobal::set_full_compatibilities(void) {
    fullcompatibilities = compatibilities;
    
    bin_hash<Tamgu*>::iterator it;
    
    int i, j;
    vector<short> numbertypes;
    vector<short> stringtypes;
    short ty;
    for (it = newInstance.begin(); it != newInstance.end(); it++) {
        if (it->second->isFrame()) //this type is only used to produce frames...
            continue;
        
        ty = it->second->Type();
        
        if (it->second->isNumber()) {
            numbertypes.push_back(ty);
            continue;
        }
        
        if (it->second->isString()) {
            stringtypes.push_back(ty);
            continue;
        }
    }
    
    for (i = 0; i < numbertypes.size(); i++) {
        //We enable a loose compatibility for function arguments between strings and numbers
        //When this flag is set to one, a function with a string parameter can be called with a number argument
        //By default, this flag is set to 0
        //Note that there is another flag: TAMGUSTRICTCOMPARISON, which deals with strict or loose comparisons between elements
        //When TAMGUSTRICTCOMPARISON is set to 1, a comparison between a string and a number returns false.
        //Else, the second element of the comparison is converted into the type of the first element.
        for (j = 0; j < stringtypes.size(); j++)
            fullcompatibilities[numbertypes[i]][stringtypes[j]] = true;
    }
    
    for (i = 0; i < stringtypes.size(); i++) {
        //We enable a loose compatibility for function arguments between strings and numbers
        //When this flag is set to one, a function with a string parameter can be called with a number argument
        //Note that there is another flag: TAMGUSTRICTCOMPARISON, which deals with strict or loose comparisons between elements
        fullcompatibilities[stringtypes[i]][Tamgudate::idtype] = true;
        for (j = 0; j < numbertypes.size(); j++)
            fullcompatibilities[stringtypes[i]][numbertypes[j]] = true;
    }
}

void TamguGlobal::set_loose_compatibilities(void) {
    if (loosecompability) {
        bin_hash<Tamgu*>::iterator it;
        
        int i, j;
        vector<short> numbertypes;
        vector<short> stringtypes;
        short ty;
        for (it = newInstance.begin(); it != newInstance.end(); it++) {
            if (it->second->isFrame()) //this type is only used to produce frames...
                continue;
            
            ty = it->second->Type();
            
            if (it->second->isNumber()) {
                numbertypes.push_back(ty);
                continue;
            }
            
            if (it->second->isString()) {
                stringtypes.push_back(ty);
                continue;
            }
        }
        
        for (i = 0; i < numbertypes.size(); i++) {
            //We enable a loose compatibility for function arguments between strings and numbers
            //When this flag is set to one, a function with a string parameter can be called with a number argument
            //By default, this flag is set to 0
            //Note that there is another flag: TAMGUSTRICTCOMPARISON, which deals with strict or loose comparisons between elements
            //When TAMGUSTRICTCOMPARISON is set to 1, a comparison between a string and a number returns false.
            //Else, the second element of the comparison is converted into the type of the first element.
            for (j = 0; j < stringtypes.size(); j++)
                compatibilities[numbertypes[i]][stringtypes[j]] = true;
        }
        
        for (i = 0; i < stringtypes.size(); i++) {
            //We enable a loose compatibility for function arguments between strings and numbers
            //When this flag is set to one, a function with a string parameter can be called with a number argument
            //Note that there is another flag: TAMGUSTRICTCOMPARISON, which deals with strict or loose comparisons between elements
            for (j = 0; j < numbertypes.size(); j++)
                compatibilities[stringtypes[i]][numbertypes[j]] = true;
        }
    }
}

Exporting void TamguGlobal::RecordCompatibilities() {
    bin_hash<Tamgu*>::iterator it;
    
    vector<short> numbertypes;
    vector<short> stringtypes;
    vector<short> maptypes;
    vector<short> vectortypes;
    
    equto[a_plusequ] = a_plus;
    equto[a_minusequ] = a_minus;
    equto[a_multiplyequ] = a_multiply;
    equto[a_divideequ] = a_divide;
    equto[a_divideintegerequ] = a_divideinteger;
    equto[a_powerequ] = a_power;
    equto[a_shiftleftequ] = a_shiftleft;
    equto[a_shiftrightequ] = a_shiftright;
    equto[a_modequ] = a_mod;
    equto[a_mergeequ] = a_merge;
    equto[a_combineequ] = a_combine;
    equto[a_addequ] = a_add;
    equto[a_orequ] = a_or;
    equto[a_xorequ] = a_xor;
    equto[a_andequ] = a_and;
    equto[a_plus] = a_plus;
    equto[a_minus] = a_minus;
    equto[a_multiply] = a_multiply;
    equto[a_divide] = a_divide;
    equto[a_power] = a_power;
    equto[a_shiftleft] = a_shiftleft;
    equto[a_shiftright] = a_shiftright;
    equto[a_mod] = a_mod;
    equto[a_merge] = a_merge;
    equto[a_combine] = a_combine;
    equto[a_add] = a_add;
    equto[a_or] = a_or;
    equto[a_xor] = a_xor;
    equto[a_and] = a_and;
    
    compatibilities[a_call][a_call] = true;
    compatibilities[a_call][a_callfunction] = true;
    compatibilities[a_call][a_callmethod] = true;
    compatibilities[a_call][a_callprocedure] = true;
    compatibilities[a_call][a_calltaskell] = true;
    compatibilities[a_call][a_lambda] = true;
    compatibilities[a_call][a_let] = true;
    compatibilities[a_call][a_self] = true;
    compatibilities[a_call][a_none] = true;
    
    compatibilities[a_let][a_call] = true;
    compatibilities[a_let][a_function] = true;
    compatibilities[a_let][a_callfunction] = true;
    compatibilities[a_let][a_callmethod] = true;
    compatibilities[a_let][a_callprocedure] = true;
    compatibilities[a_let][a_calltaskell] = true;
    compatibilities[a_let][a_lambda] = true;
    compatibilities[a_let][a_none] = true;
    
    compatibilities[a_self][a_call] = true;
    compatibilities[a_self][a_function] = true;
    compatibilities[a_self][a_callfunction] = true;
    compatibilities[a_self][a_callmethod] = true;
    compatibilities[a_self][a_callprocedure] = true;
    compatibilities[a_self][a_calltaskell] = true;
    compatibilities[a_self][a_lambda] = true;
    compatibilities[a_self][a_none] = true;
    
    compatibilities[a_self][a_const] = true;
    compatibilities[a_let][a_const] = true;
    
    compatibilities[a_boolean][a_const] = true;
    compatibilities[a_boolean][a_predicatelaunch] = true;
    compatibilities[a_predicate][a_predicatevar] = true;
    compatibilities[a_predicatevar][a_predicate] = true;
    compatibilities[a_predicatevar][a_predicatelaunch] = true;
    
    //The difference between strict and not strict (see TamguCallFunctionArgsTaskell::Get) is that
    //in regular compatibility, string and integer can be accepted as compatible, while in a strict compatibility it is not the case
    //second, vector will be compatible with ivector, fvector etc... but the reverse will be false. The same for maps.
    strictcompatibilities[a_call][a_call] = true;
    strictcompatibilities[a_call][a_function] = true;
    strictcompatibilities[a_call][a_callfunction] = true;
    strictcompatibilities[a_call][a_callmethod] = true;
    strictcompatibilities[a_call][a_callprocedure] = true;
    strictcompatibilities[a_call][a_calltaskell] = true;
    strictcompatibilities[a_call][a_lambda] = true;
    strictcompatibilities[a_call][a_let] = true;
    strictcompatibilities[a_call][a_self] = true;
    strictcompatibilities[a_call][a_none] = true;
    
    strictcompatibilities[a_universal][a_call] = true;
    strictcompatibilities[a_universal][a_function] = true;
    strictcompatibilities[a_universal][a_callfunction] = true;
    strictcompatibilities[a_universal][a_callmethod] = true;
    strictcompatibilities[a_universal][a_callprocedure] = true;
    strictcompatibilities[a_universal][a_calltaskell] = true;
    strictcompatibilities[a_universal][a_lambda] = true;
    strictcompatibilities[a_universal][a_none] = true;
    
    strictcompatibilities[a_let][a_call] = true;
    strictcompatibilities[a_let][a_function] = true;
    strictcompatibilities[a_let][a_callfunction] = true;
    strictcompatibilities[a_let][a_callmethod] = true;
    strictcompatibilities[a_let][a_callprocedure] = true;
    strictcompatibilities[a_let][a_calltaskell] = true;
    strictcompatibilities[a_let][a_lambda] = true;
    strictcompatibilities[a_let][a_none] = true;
    
    strictcompatibilities[a_self][a_call] = true;
    strictcompatibilities[a_self][a_function] = true;
    strictcompatibilities[a_self][a_callfunction] = true;
    strictcompatibilities[a_self][a_callmethod] = true;
    strictcompatibilities[a_self][a_callprocedure] = true;
    strictcompatibilities[a_self][a_calltaskell] = true;
    strictcompatibilities[a_self][a_lambda] = true;
    strictcompatibilities[a_self][a_none] = true;
    
    strictcompatibilities[a_self][a_const] = true;
    strictcompatibilities[a_let][a_const] = true;
    
    strictcompatibilities[a_boolean][a_const] = true;
    strictcompatibilities[a_boolean][a_predicatelaunch] = true;
    strictcompatibilities[a_predicate][a_predicatevar] = true;
    strictcompatibilities[a_predicatevar][a_predicate] = true;
    strictcompatibilities[a_predicatevar][a_predicatelaunch] = true;
    
    maptypes.push_back(a_constmap);
    vectortypes.push_back(a_constvector);
    
    short ty;
    for (it = newInstance.begin(); it != newInstance.end(); it++) {
        if (it->second->isFrame()) //this type is only used to produce frames...
            continue;
        
        ty = it->second->Type();
        
        //we record each object as being its own procedure... These procedures (actually calling ProcCreate)
        //return an object of this type...
        RecordOneProcedure(Getsymbol(ty), "Procedure as Predicate", ProcCreate, P_NONE | P_ONE);
        returntypes[ty] = ty;
        
        compatibilities[ty][ty] = true;
        
        compatibilities[a_self][ty] = true;
        compatibilities[a_let][ty] = true;
        compatibilities[ty][a_self] = true;
        compatibilities[ty][a_let] = true;

        compatibilities[ty][a_lisp] = true;

        compatibilities[ty][a_call] = true;
        compatibilities[ty][a_function] = true;
        compatibilities[ty][a_callfunction] = true;
        compatibilities[ty][a_callmethod] = true;
        compatibilities[ty][a_callprocedure] = true;
        compatibilities[ty][a_calltaskell] = true;
        compatibilities[ty][a_lambda] = true;
        compatibilities[ty][a_instructions] = true;
        compatibilities[ty][a_none] = true;
        compatibilities[ty][a_predicatevar] = true;
        
        compatibilities[a_boolean][ty] = true;
        
        strictcompatibilities[ty][ty] = true;
        
        strictcompatibilities[ty][a_lisp] = true;
        
        strictcompatibilities[a_self][ty] = true;
        strictcompatibilities[a_let][ty] = true;
        strictcompatibilities[a_universal][ty] = true;
        strictcompatibilities[ty][a_self] = true;
        strictcompatibilities[ty][a_let] = true;
        strictcompatibilities[ty][a_universal] = true;
        
        strictcompatibilities[ty][a_call] = true;
        strictcompatibilities[ty][a_function] = true;
        strictcompatibilities[ty][a_callfunction] = true;
        strictcompatibilities[ty][a_callmethod] = true;
        strictcompatibilities[ty][a_callprocedure] = true;
        strictcompatibilities[ty][a_calltaskell] = true;
        strictcompatibilities[ty][a_lambda] = true;
        strictcompatibilities[ty][a_instructions] = true;
        strictcompatibilities[ty][a_none] = true;
        strictcompatibilities[ty][a_predicatevar] = true;
        
        if (it->second->isVectorContainer()) {
            vectortypes.push_back(ty);
            continue;
        }
        if (it->second->isMapContainer()) {
            maptypes.push_back(ty);
            continue;
        }
        
        if (it->second->isNumber()) {
            numbertypes.push_back(ty);
            continue;
        }
        
        if (it->second->isString()) {
            stringtypes.push_back(ty);
            continue;
        }
    }
    
    int i, j;
    for (i = 0; i < numbertypes.size(); i++) {
        for (j = 0; j < numbertypes.size(); j++) {
            compatibilities[numbertypes[i]][numbertypes[j]] = true;
            strictcompatibilities[numbertypes[i]][numbertypes[j]] = true;
        }
        //We enable a loose compatibility for function arguments between strings and numbers
        //When this flag is set to one, a function with a string parameter can be called with a number argument
        //By default, this flag is set to 0
        //Note that there is another flag: TAMGUSTRICTCOMPARISON, which deals with strict or loose comparisons between elements
        //When TAMGUSTRICTCOMPARISON is set to 1, a comparison between a string and a number returns false.
        //Else, the second element of the comparison is converted into the type of the first element.
        if (loosecompability) {
            for (j = 0; j < stringtypes.size(); j++)
                compatibilities[numbertypes[i]][stringtypes[j]] = true;
        }
        
        compatibilities[numbertypes[i]][a_const] = true;
        strictcompatibilities[numbertypes[i]][a_const] = true;
    }
    
    for (i = 0; i < stringtypes.size(); i++) {
        for (j = 0; j < stringtypes.size(); j++) {
            compatibilities[stringtypes[i]][stringtypes[j]] = true;
            strictcompatibilities[stringtypes[i]][stringtypes[j]] = true;
        }
        //We enable a loose compatibility for function arguments between strings and numbers
        //When this flag is set to one, a function with a string parameter can be called with a number argument
        //Note that there is another flag: TAMGUSTRICTCOMPARISON, which deals with strict or loose comparisons between elements
        if (loosecompability) {
            for (j = 0; j < numbertypes.size(); j++)
                compatibilities[stringtypes[i]][numbertypes[j]] = true;
        }
        
        compatibilities[stringtypes[i]][a_const] = true;
        strictcompatibilities[stringtypes[i]][a_const] = true;
    }
    
    strictcompatibilities[a_sloop][a_svector] = true;
    strictcompatibilities[a_sloop][a_uvector] = true;
    strictcompatibilities[a_uloop][a_svector] = true;
    strictcompatibilities[a_uloop][a_uvector] = true;
    strictcompatibilities[a_sloop][a_vector] = true;
    strictcompatibilities[a_uloop][a_vector] = true;
    
    compatibilities[a_sloop][a_svector] = true;
    compatibilities[a_sloop][a_uvector] = true;
    compatibilities[a_uloop][a_svector] = true;
    compatibilities[a_uloop][a_uvector] = true;
    compatibilities[a_sloop][a_vector] = true;
    compatibilities[a_uloop][a_vector] = true;
    
    for (i = 0; i < vectortypes.size(); i++) {
        for (j = 0; j < vectortypes.size(); j++) {
            compatibilities[vectortypes[i]][vectortypes[j]] = true;
            compatibilities[vectortypes[i]][a_string] = true;
            compatibilities[vectortypes[i]][a_ustring] = true;
        }
        
        if (vectortypes[i] != a_svector && vectortypes[i] != a_uvector ) {
            compatibilities[a_bloop][vectortypes[i]] = true;
            compatibilities[a_iloop][vectortypes[i]] = true;
            compatibilities[a_lloop][vectortypes[i]] = true;
            compatibilities[a_dloop][vectortypes[i]] = true;
            compatibilities[a_floop][vectortypes[i]] = true;
            strictcompatibilities[a_bloop][vectortypes[i]] = true;
            strictcompatibilities[a_iloop][vectortypes[i]] = true;
            strictcompatibilities[a_lloop][vectortypes[i]] = true;
            strictcompatibilities[a_dloop][vectortypes[i]] = true;
            strictcompatibilities[a_floop][vectortypes[i]] = true;
        }
        
        compatibilities[vectortypes[i]][a_predicatelaunch] = true;
        strictcompatibilities[a_vector][vectortypes[i]] = true;
        strictcompatibilities[vectortypes[i]][a_vector] = true;
        compatibilities[vectortypes[i]][a_const] = true;
        strictcompatibilities[vectortypes[i]][a_const] = true;
    }
    
    for (i = 0; i < maptypes.size(); i++) {
        for (j = 0; j < maptypes.size(); j++) {
            compatibilities[maptypes[i]][maptypes[j]] = true;
            compatibilities[maptypes[i]][a_string] = true;
            compatibilities[maptypes[i]][a_ustring] = true;
        }
        
        strictcompatibilities[a_map][maptypes[i]] = true;
        strictcompatibilities[a_primemap][maptypes[i]] = true;
        strictcompatibilities[a_treemap][maptypes[i]] = true;
        strictcompatibilities[a_binmap][maptypes[i]] = true;
        
        strictcompatibilities[maptypes[i]][a_map] = true;
        strictcompatibilities[maptypes[i]][a_primemap] = true;
        strictcompatibilities[maptypes[i]][a_treemap] = true;
        strictcompatibilities[maptypes[i]][a_binmap] = true;
        
        compatibilities[maptypes[i]][a_const] = true;
        strictcompatibilities[maptypes[i]][a_const] = true;
    }
    set_full_compatibilities();
}

void TamguGlobal::SetCompatibilities(short ty) {
    
    compatibilities[ty][ty] = true;
    
    compatibilities[a_self][ty] = true;
    compatibilities[a_let][ty] = true;
    compatibilities[ty][a_self] = true;
    compatibilities[ty][a_let] = true;
    
    compatibilities[ty][a_call] = true;
    compatibilities[ty][a_function] = true;
    compatibilities[ty][a_callfunction] = true;
    compatibilities[ty][a_callmethod] = true;
    compatibilities[ty][a_callprocedure] = true;
    compatibilities[ty][a_calltaskell] = true;
    compatibilities[ty][a_lambda] = true;
    compatibilities[ty][a_instructions] = true;
    compatibilities[ty][a_none] = true;
    compatibilities[ty][a_predicatevar] = true;
    
    compatibilities[a_boolean][ty] = true;
    
    
    strictcompatibilities[ty][ty] = true;
    strictcompatibilities[a_self][ty] = true;
    strictcompatibilities[a_let][ty] = true;
    strictcompatibilities[a_universal][ty] = true;
    strictcompatibilities[ty][a_self] = true;
    strictcompatibilities[ty][a_let] = true;
    strictcompatibilities[ty][a_universal] = true;
    
    strictcompatibilities[ty][a_call] = true;
    strictcompatibilities[ty][a_function] = true;
    strictcompatibilities[ty][a_callfunction] = true;
    strictcompatibilities[ty][a_callmethod] = true;
    strictcompatibilities[ty][a_callprocedure] = true;
    strictcompatibilities[ty][a_calltaskell] = true;
    strictcompatibilities[ty][a_lambda] = true;
    strictcompatibilities[ty][a_instructions] = true;
    strictcompatibilities[ty][a_none] = true;
    strictcompatibilities[ty][a_predicatevar] = true;
    
    fullcompatibilities[ty][ty] = true;
    
    fullcompatibilities[a_self][ty] = true;
    fullcompatibilities[a_let][ty] = true;
    fullcompatibilities[ty][a_self] = true;
    fullcompatibilities[ty][a_let] = true;
    
    fullcompatibilities[ty][a_call] = true;
    fullcompatibilities[ty][a_function] = true;
    fullcompatibilities[ty][a_callfunction] = true;
    fullcompatibilities[ty][a_callmethod] = true;
    fullcompatibilities[ty][a_callprocedure] = true;
    fullcompatibilities[ty][a_calltaskell] = true;
    fullcompatibilities[ty][a_lambda] = true;
    fullcompatibilities[ty][a_instructions] = true;
    fullcompatibilities[ty][a_none] = true;
    fullcompatibilities[ty][a_predicatevar] = true;
    
    fullcompatibilities[a_boolean][ty] = true;
    
    bin_hash<Tamgu*>::iterator it;
    short type;
    Tamgu* ta;
    for (it = newInstance.begin(); it != newInstance.end(); it++) {
        ta = it->second;
        if (ta->isContainer() && ta->Frame() == NULL && !ta->isValueContainer()) {
            type = ta->Type();
            compatibilities[type][ty];
            strictcompatibilities[type][ty];
            fullcompatibilities[type][ty];
        }
    }
}

long TamguGlobal::Getinstructionline(short idthread) {
    if (threads[idthread].currentinstruction == NULL)
        return lineerror;
    return threads[idthread].currentinstruction->Currentline();
}

long TamguGlobal::Getinstructionfile(short idthread) {
    if (threads[idthread].currentinstruction == NULL)
        return lineerror;
    return threads[idthread].currentinstruction->Currentfile();
}


//--------------------------------------------------------------------------------------------
//These three methods are the way to go to create new objets in an external library, which are declared internally within Tamgu
Tamgu* TamguGlobal::Provideinstance(short type, short idthread) {
    return newInstance.get(type)->Newinstance(idthread);
}

Tamgu* TamguGlobal::Provideinstance(string thetypename, short idthread) {
    return newInstance.get(globalTamgu->Getid(thetypename))->Newinstance(idthread);
}

short TamguGlobal::Typeinstance(string thetypename) {
    return Getid(thetypename);
}

Tamgu* TamguGlobal::Provideinstance(Tamgu* p, long i) {
    return p->anInstance(i);
}
//--------------------------------------------------------------------------------------------

//This function is a hack which is necessary to get these variables a value in a DLL
Exporting void TamguGlobal::Update() {
    globalTamgu = this;
    globalConstants.set(globalTamgu->global_constants);
    booleantamgu[0] = aFALSE;
    booleantamgu[1] = aTRUE;
    set_utf8_handler(handler_on_utf8);
}


Exporting void TamguGlobal::RecordConstantNames() {
    string_operators["=="] = a_same;
    string_operators["<"] = a_less;
    string_operators[">"] = a_more;
    string_operators[":<"] = a_less;
    string_operators[":>"] = a_more;
    string_operators[">="] = a_moreequal;
    string_operators["<="] = a_lessequal;
    string_operators["="] = a_assignement;
    string_operators["is"] = a_assignement;
    string_operators[":="] = a_forcedaffectation;
    string_operators["isnot"] = a_different;
    string_operators["<>"] = a_different;
    string_operators["~="] = a_different;
    string_operators["!="] = a_different;
    string_operators["≠"] = a_different;
    string_operators["+"] = a_plus;
    string_operators["-"] = a_minus;
    string_operators["*"] = a_multiply;
    string_operators["×"] = a_multiply;
    string_operators["/"] = a_divide;
    string_operators["÷"] = a_divide;
    string_operators["%"] = a_mod;
    string_operators["%%"] = a_divideinteger;
    string_operators["^^"] = a_power;
    string_operators["**"] = a_power;
    string_operators["|"] = a_or;
    string_operators["^"] = a_xor;
    string_operators["&"] = a_and;
    string_operators["<<"] = a_shiftleft;
    string_operators[">>"] = a_shiftright;
    string_operators[":<<"] = a_shiftleft; //in Taskell mode to avoid confusion with Taskell brackets...
    string_operators[":>>"] = a_shiftright; //in Taskell mode to avoid confusion with Taskell brackets...
    string_operators["<<<"] = a_stream;
    string_operators["+="] = a_plusequ;
    string_operators["++"] = a_plusplus;
    string_operators["-="] = a_minusequ;
    string_operators["--"] = a_minusminus;
    string_operators["&&&"] = a_merge;
    string_operators["&&&="] = a_mergeequ;
    string_operators["|||"] = a_combine;
    string_operators["|||="] = a_combineequ;
    string_operators["::"] = a_add;
    string_operators["::="] = a_addequ;
    string_operators["×="] = a_multiplyequ;
    string_operators["*="] = a_multiplyequ;
    string_operators["//="] = a_divideintegerequ;
    string_operators["/="] = a_divideequ;
    string_operators["÷="] = a_divideequ;
    string_operators["%="] = a_modequ;
    string_operators["^^="] = a_powerequ;
    string_operators["**="] = a_powerequ;
    string_operators["<<="] = a_shiftleftequ;
    string_operators[">>="] = a_shiftrightequ;
    string_operators[":<<="] = a_shiftleftequ; //for consistency...
    string_operators[":>>="] = a_shiftrightequ;
    string_operators["|="] = a_orequ;
    string_operators["^="] = a_xorequ;
    string_operators["&="] = a_andequ;
    string_operators["||"] = a_booleanor;
    string_operators["&&"] = a_booleanand;
    string_operators["or"] = a_booleanor;
    string_operators["xor"] = a_booleanxor;
    string_operators["ifnot"] = a_ifnot;
    string_operators["and"] = a_booleanand;
    string_operators["in"] = a_in;
    string_operators["notin"] = a_notin;
    string_operators["²"] = a_square;
    string_operators["³"] = a_cube;
    string_operators["∧"] = a_conjunction;
    string_operators["∨"] = a_disjunction;
    
    
    operator_strings[a_same] = "==";
    operator_strings[a_less] = "<";
    operator_strings[a_more] = ">";
    operator_strings[a_moreequal] = ">=";
    operator_strings[a_lessequal] = "<=";
    operator_strings[a_assignement] = "=";
    operator_strings[a_forcedaffectation] = ":=";
    operator_strings[a_different] = "<>";
    operator_strings[a_different] = "~=";
    operator_strings[a_different] = "!=";
    operator_strings[a_plus] = "+";
    operator_strings[a_minus] = "-";
    operator_strings[a_multiply] = "*";
    operator_strings[a_divide] = "/";
    operator_strings[a_mod] = "%";
    operator_strings[a_power] = "^^";
    operator_strings[a_or] = "|";
    operator_strings[a_xor] = "^";
    operator_strings[a_and] = "&";
    operator_strings[a_stream] = "<<<";
    operator_strings[a_shiftleft] = "<<";
    operator_strings[a_shiftright] = ">>";
    operator_strings[a_plusequ] = "+=";
    operator_strings[a_plusplus] = "++";
    operator_strings[a_minusequ] = "-=";
    operator_strings[a_minusminus] = "--";
    operator_strings[a_multiplyequ] = "*=";
    operator_strings[a_divideintegerequ] = "//=";
    operator_strings[a_divideequ] = "/=";
    operator_strings[a_modequ] = "%=";
    operator_strings[a_powerequ] = "^^=";
    operator_strings[a_shiftleftequ] = "<<=";
    operator_strings[a_shiftrightequ] = ">>=";
    operator_strings[a_orequ] = "|=";
    operator_strings[a_xorequ] = "^=";
    operator_strings[a_andequ] = "&=";
    operator_strings[a_booleanor] = "||";
    operator_strings[a_booleanand] = "&&";
    operator_strings[a_in] = "in";
    operator_strings[a_notin] = "notin";
    operator_strings[a_square] = "²";
    operator_strings[a_cube] = "³";
    operator_strings[a_conjunction] = "∧";
    operator_strings[a_disjunction] = "∨";
    operator_strings[a_merge] = "&&&";
    operator_strings[a_combine] = "|||";
    operator_strings[a_add] = "::";
    operator_strings[a_mergeequ] = "&&&=";
    operator_strings[a_combineequ] = "|||=";
    operator_strings[a_addequ] = "::=";
    operator_strings[a_quote] = "quote";
    
    atanOperatorMath[a_plus] = true;
    atanOperatorMath[a_minus] = true;
    atanOperatorMath[a_multiply] = true;
    atanOperatorMath[a_divide] = true;
    atanOperatorMath[a_mod] = true;
    atanOperatorMath[a_power] = true;
    atanOperatorMath[a_or] = true;
    atanOperatorMath[a_xor] = true;
    atanOperatorMath[a_and] = true;
    atanOperatorMath[a_shiftleft] = true;
    atanOperatorMath[a_shiftright] = true;
    
    if (aMINUSONE == NULL) {
        aMINUSONE = new TamguConstInt(-1);
        aZERO = new TamguConstInt(0);
        aZEROPOINTZERO = new TamguConstFloat(0.0);
        aONE = new TamguConstInt(1);
        
        //They are created once for all...
        //Constants and ids
        aPIPE = new TamguConstPipe(a_pipe, ":", NULL);
        
        aTRUE = new TamguConstBool(true, NULL);
        aFALSE = new TamguConstBool(false, NULL);
        
        aEMPTYLISP = new Tamgulispcode(NULL);
        aEMPTYSTRING = new TamguConstString("", NULL);
        aEMPTYUSTRING = new TamguConstUString(L"", NULL);
        
        aEND = new TamguError(NULL, "END");
        aRAISEERROR = new TamguError(NULL, "ERROR");
        aBREAKTRUE = new TamguCallBreak(aTRUE, NULL);
        aBREAKFALSE = new TamguCallBreak(aFALSE, NULL);
        aBREAKONE = new TamguCallBreak(aONE, NULL);
        aBREAKZERO = new TamguCallBreak(aZERO, NULL);
        
        aNULLDECLARATION = new TamguDeclaration((TamguGlobal*)NULL);
        aNULLLet = new TamguLet(NULL);
        
        
        //Since we need to set aNULL in vecte, we keep local instantiation of aNULL, which is related to gNULL
        aNULL = new TamguConst(Createid("null"), "null", NULL); //0 --> a_null
        //a hack to avoid this value to be stored in the tracker, when creating the corresponding system variable (see RecordSystemVariables() in codeexecute.cxx)
        
        globalConstants.gNULL = aNULL;
        
        aITERNULL = new TamguConstiteration(false, NULL);
        aITERNULL->Update(aNULL); //Horrible hack to have aITERNULL being deleted before aNULL, but benefiting from aNULL still
        
        aMINUSONE->Protectfromtracker();
        aZERO->Protectfromtracker();
        aZEROPOINTZERO->Protectfromtracker();
        aONE->Protectfromtracker();
        
        aITERNULL->Protectfromtracker();
        aPIPE->Protectfromtracker();
        
        aTRUE->Protectfromtracker();
        aFALSE->Protectfromtracker();
        
        aEMPTYLISP->Protectfromtracker();
        aEMPTYSTRING->Protectfromtracker();
        aEMPTYUSTRING->Protectfromtracker();
        
        aEND->Protectfromtracker();
        aRAISEERROR->Protectfromtracker();
        aBREAKTRUE->Protectfromtracker();
        aBREAKFALSE->Protectfromtracker();
        aBREAKONE->Protectfromtracker();
        aBREAKZERO->Protectfromtracker();
        
        aNULLDECLARATION->Protectfromtracker();
        aNULLLet->Protectfromtracker();
        
        aNULL->Protectfromtracker();
        
        booleantamgu[0] = aFALSE;
        booleantamgu[1] = aTRUE;
    }
    else
        Createid("null"); //a_null
    
    Setlispmode(false);
    
    
    Createid("true"); //1 --> a_true
    Createid("false"); //2 --> a_false
    
    Createid("zero"); //3 --> a_zero
    Createid("one"); //4 --> a_one
    
    Createid("minusone"); //5 --> a_minusone
    
    Createid("bool");//6 -->a_boolean
    Createid("byte"); //7 --> a_byte
    Createid("short"); //8 --> a_short
    Createid("int"); //9 --> a_int
    Createid("long"); //10 --> a_long
    Createid("decimal");//11 --> a_decimal
    Createid("fraction");//12 --> a_fraction
    Createid("complex"); //13 a_complex
    Createid("float");//14 --> a_float
    
    Createid("bloop");//15 --> a_bloop
    Createid("iloop");//16 --> a_iloop
    Createid("lloop");//17 --> a_lloop
    Createid("dloop");//18 --> a_dloop
    Createid("floop");//19 --> a_floop
    
    Createid("ithrough"); //20 --> a_intthrough
    Createid("lthrough"); //21 --> a_longthrough
    Createid("dthrough"); //22 --> a_decimalthrough
    Createid("fthrough"); //23 --> a_floatthrough
    Createid("sthrough"); //24 --> a_stringthrough
    Createid("uthrough"); //25 --> a_ustringthrough
    Createid("vthrough"); //26 --> a_vectorthrough
    Createid("mthrough"); //27 --> a_mapthrough
    
    Createid("string");//28 --> a_string
    Createid("ustring");//29 --> a_ustring
    
    Createid("sloop"); //30 --> a_sloop
    Createid("uloop");//31 --> a_uloop
    
    
    
    Createid("constvector"); //32 --> a_constvector
    Createid("vector");//33 --> a_vector
    Createid("%frame_vector%");//34 --> a_framevector
    Createid("bvector");//35 --> a_bvector
    Createid("fvector");//36 --> a_fvector
    Createid("ivector");//37 --> a_ivector
    Createid("hvector");//38 --> a_hvector
    Createid("svector");//39 --> a_svector
    Createid("uvector");//40 --> a_uvector
    Createid("dvector");//41 --> a_dvector
    Createid("lvector");//42 --> a_lvector
    Createid("list"); //43 --> a_list
    
    
    Createid("constmap"); //44 --> a_constmap
    Createid("map");//45 --> a_map
    Createid("treemap"); //46 --> a_treemap
    
    Createid("primemap"); //47 --> a_primemap
    Createid("binmap"); //48 --> a_binmap
    Createid("mapss"); //49 --> a_mapss
    
    Createid("mapi");//50 --> a_mapi
    Createid("treemapi"); //51 --> a_treemapi
    Createid("mapf");//52 --> a_mapi
    
    Createid("%frame_map%");//53 --> a_framemap
    Createid("%frame_treemap%");//54 --> a_frametreemap
    Createid("%frame_primemap%");//55 --> a_frameprimemap
    Createid("%frame_mapi%");//56 --> a_framemapi
    Createid("%frame_treemapi%");//57 --> a_frametreemapi
    Createid("%frame_mapf%");//58 --> a_framemapf
    
    Createid("&error");//59 --> a_error
    
    Createid("const"); //60 --> a_const
    Createid("tam_none"); //61 --> a_none
    Createid(":"); //62 --> a_pipe
    
    if (aBREAK == NULL) {
        aBREAK = new TamguConstBreak(Createid("break"), "break", NULL); //63 --> a_break
        aCONTINUE = new TamguConstContinue(Createid("continue"), "continue", NULL); //64 --> a_continue
        aRETURN = new TamguConst(Createid("return"), "return",  NULL); //65 --> a_return
        aNOELEMENT = new TamguConst(Createid("empty"), "empty", NULL); //66 --> a_empty
        
        aBREAK->Protectfromtracker();
        aCONTINUE->Protectfromtracker();
        aRETURN->Protectfromtracker();
        aNOELEMENT->Protectfromtracker(); //a hack to avoid this value to be stored in the tracker...
    }
    else {
        Createid("break");
        Createid("continue");
        Createid("return");
        Createid("empty");
    }
    
    Createid("_MAIN"); //67 --> a_mainframe
    
    Createid("call"); //68 --> a_call
    Createid("callfunction");//69 --> a_callfunction
    Createid("callthread");//70 --> a_callthread
    Createid("callmethod"); //71 --> a_callmethod
    Createid("callprocedure"); //72 --> a_callprocedure
    Createid("callindex"); //73 --> a_callindex
    Createid("calltaskell");//74 --> a_calltaskell
    Createid("lambda");//75 --> a_lambda
    
    Createid("variable"); //76 --> a_variable
    Createid("declarations"); //77 --> a_declaration
    Createid("instructions"); //78 --> a_instructions
    Createid("function"); //79 --> a_function
    Createid("frame"); //80 --> a_frame
    Createid("frameinstance"); //81 --> a_frameinstance
    Createid("extension"); //82 --> a_extension
    Createid("_initial"); //83 --> a_initial
    Createid("iterator"); //84 --> a_iteration
    
    if (aDEFAULT == NULL) {
        aDEFAULT = new TamguConst(Createid("&default"), "&default", NULL); //85 --> a_default
        aDEFAULT->Protectfromtracker();
    }
    else
        Createid("&default");
    
    Createid("forinrange"); //86 --> a_forinrange
    Createid("sequence"); //87 --> a_sequence
    Createid("self"); //88 --> a_self
    Createid("&return;");//89 --> a_idreturnvariable
    Createid("&breaktrue"); //90 --> a_breaktrue
    Createid("&breakfalse"); //91 --> a_breakfalse
    
    Createid("constvectormerge"); //92 --> a_vectormerge
    Createid("instructionequ"); //93 --> a_instructionequ
    
    //let is used in Taskell expression, it corresponds to a self variable...
    Createid("let"); //94 --> a_let
    
    if (aASSIGNMENT == NULL) {
        aASSIGNMENT = new TamguConst(Createid("&assign"), "&assign", NULL); //95 --> a_assign
        aASSIGNMENT->Protectfromtracker();
    }
    else
        Createid("&assign");
    
    Createid("tamgu");//96 --> a_tamgu
    Createid("this");//97 --> a_this
    Createid("[]");//98 --> a_index
    Createid("[:]");//99 --> a_interval
    Createid("type");//100 --> a_type
    Createid("_final");//101 --> a_final
    Createid("&inifinitive"); //102 --> a_infinitive
    Createid("&cycle"); //103 --> a_cycle
    Createid("&replicate"); //104 --> a_replicate
    Createid("fail");//105 --> a_fail
    Createid("&~!"); //106 --> a_cutfalse
    
    
    Createid("!"); //107 --> a_cut
    Createid("stop");//108 --> a_stop
    
    
    Createid("&PREDICATEENTREE"); //109 --> a_predicateentree
    
    if (aUNIVERSAL == NULL) {
        aUNIVERSAL = new TamguConstUniversal(Createid("_"), "_", NULL); //110 --> a_universal
        aUNIVERSAL->Protectfromtracker(); //a hack to avoid this value to be stored in the tracker...
    }
    else
        Createid("_");
    
    Createid("asserta"); //111 --> a_asserta
    Createid("assertz"); //112 --> a_assertz
    Createid("retract"); //113 --> a_retract
    Createid("findall"); //114 --> a_findall
    Createid("&dependency_asserta&"); //115 --> a_dependency_asserta
    Createid("&dependency_assertz&"); //116 --> a_dependency_assertz
    Createid("&dependency_retract&"); //117 --> a_dependency_retract
    Createid("&dependency_remove&"); //118 --> a_dependency_remove
    Createid("predicatemethod"); //119 --> a_predicatemethod
    Createid("predicatevar"); //120 --> a_predicatevar
    Createid("predicate"); //121 --> a_predicate
    Createid("term"); //122 --> a_term
    Createid("?_"); //123 --> a_instance
    Createid("p_predicateruleelement"); //124 --> a_predicateruleelement
    Createid("p_predicatecontainer"); //125 --> a_predicatecontainer
    Createid("p_predicaterule"); //126 --> a_predicaterule
    Createid("p_predicateinstruction"); //127 --> a_predicateinstruction
    Createid("p_knowledgebase"); //128 --> a_knowledgebase
    Createid("p_dependencybase"); //129 --> a_dependencybase
    Createid("p_predicatedomain"); //130 --> a_predicatedomain
    Createid("p_launch"); //131 --> a_predicatelaunch
    Createid("p_predicateelement"); //132 --> a_predicateelement
    Createid("p_parameterpredicate"); //133 --> a_parameterpredicate
    Createid("p_predicateevaluate"); //134 --> a_predicateevaluate
    Createid("dependency"); //135 --> a_dependency
    
    Createid("tam_stream"); //136 --> a_stream
    Createid("setq"); //137 --> a_assignement
    
    Createid("tam_plusequ"); //138 --> a_plusequ
    Createid("tam_minusequ"); //139 --> a_minusequ
    Createid("tam_multiplyequ"); //140 --> a_multiplyequ
    Createid("tam_divideequ"); //141 --> a_divideequ
    Createid("tam_divideintegerequ"); //142 --> a_divideintegerequ
    Createid("tam_modequ"); //143 --> a_modequ
    Createid("tam_powerequ"); //144 --> a_powerequ
    Createid("tam_shiftleftequ"); //145 --> a_shiftleftequ
    Createid("tam_shiftrightequ"); //146 --> a_shiftrightequ
    Createid("tam_orequ"); //147 --> a_orequ
    Createid("tam_xorequ"); //148 --> a_xorequ
    Createid("tam_andequ"); //149 --> a_andequ
    Createid("tam_mergeequ"); //150 --> a_mergeequ
    Createid("tam_combineequ"); //151 --> a_combineequ
    Createid("tam_addequ"); //152 --> a_addequ
    
    //Operators
    
    Createid("+"); //153 --> a_plus
    Createid("-"); //154 --> a_minus
    Createid("*"); //155 --> a_multiply
    Createid("/"); //156 --> a_divide
    Createid("%%"); //157 --> a_divideinteger
    Createid("^^"); //158 --> a_power
    Createid("<<"); //159 --> a_shiftleft
    Createid(">>"); //160 --> a_shiftright
    Createid("%"); //161 --> a_mod
    Createid("|"); //162 --> a_or
    Createid("^"); //163 --> a_xor
    Createid("&"); //164 --> a_and
    Createid("&&&"); //165 --> a_merge
    Createid("|||"); //166 --> a_combine
    Createid("::"); //167 --> a_add
    Createid("∧"); //168 --> a_conjunction
    Createid("∨"); //169 --> a_disjunction
    Createid("<"); //170
    Createid(">"); //171
    Createid("=="); //172
    Createid("!="); //173
    Createid("<="); //174
    Createid(">="); //175
    Createid("++"); //176
    Createid("--"); //177
    Createid("in"); //178
    Createid("notin"); //179
    
    
    Createid("tam_match"); //180
    Createid("tam_bloc"); //181
    Createid("tam_blocloopin"); //182
    Createid("tam_filein"); //183
    Createid("tam_blocboolean"); //184
    Createid("tam_parameter"); //185
    Createid("if"); //186
    Createid("tam_try"); //187
    Createid("tam_switch"); //188
    Createid("while"); //189
    Createid("for"); //190
    Createid("tam_catchbloc"); //191
    Createid("tam_booleanand"); //192
    Createid("tam_booleanor"); //193
    
    if (aHASKELL == NULL) {
        aHASKELL = new TamguConst(Createid("&taskell"), "&taskell", NULL); //194
        aHASKELL->Protectfromtracker();
    }
    else
        Createid("&taskell");
    
    Createid("tam_forcedaffectation"); //195
    
    Createid("²"); //196
    Createid("³"); //197
    Createid("&counter;"); //198
    
    Createid("synode"); //199
    //when we want to modify a dependency...
    Createid("&modify_dependency"); //200
    
    Createid("&action_var"); //201
    Createid("&taskelldeclaration;"); //202
    Createid("&drop;"); //203
    
    Createid("concept"); //204
    Createid("~"); //205
    Createid("taskellinstruction"); //206
    Createid("methods"); //207
    Createid("treg"); //208
    Createid("table"); //209
    Createid("ifnot"); //210
    
    if (aNOTHING == NULL) {
        aNOTHING = new TamguConst(Createid("Nothing"), "Nothing", NULL); //211 --> a_Nothing
        aNOTHING->Protectfromtracker();
    }
    else
        Createid("Nothing");
    
    Createid("preg"); //212
    Createid("&a_rules;"); //213
    Createid("tam_iftaskell"); //214
    Createid("tam_casetaskell"); //215
    Createid("size"); //216
    Createid("post"); //217
    Createid("fibre"); //218
    Createid("tam_booleanxor"); //219
    Createid("push"); //220
    
    Createid("quote"); //221 a_quote
    Createid("cons"); //222 a_cons
    Createid("cond"); //223 a_cond
    Createid("atom"); //224 a_atom
    Createid("eq"); //225 a_eq
    Createid("cadr"); //226 a_cadr
    Createid("defun"); //227 a_defun
    Createid("label"); //228 a_label
    Createid("atomp"); //229 a_atomp
    Createid("numberp"); //230 a_numberp
    Createid("consp"); //231 a_consp
    Createid("zerop"); //232 a_zerop,
    Createid("nullp"); //233 a_nullp,
    Createid("block"); //234 a_block,
    Createid("eval"); //235 a_eval,
    Createid("key"); //236 a_key,
    Createid("keys"); //237 a_keys,
    Createid("load"); //238 a_load,
    Createid("body"); //239 a_body,
    Createid("apply"); //240 a_apply,
    Createid("pair"); //241 a_pair,
    Createid("calllisp"); //242 a_calllisp
    Createid("callcommon"); //243 a_callcommon
    Createid("insert"); //244 a_insert
    Createid("_map"); //245 _map
    Createid("_filter"); //246 _filter
    Createid("_takewhile"); //247 _takewhile
    Createid("_dropwhile"); //248 _dropwhile
    Createid("_zip"); //249 _zip
    Createid("_zipwith"); //250 _zipwith
    Createid("_foldl"); //251 folding and scaning operations in Lisp
    Createid("_scanl"); //252
    Createid("_foldr"); //253
    Createid("_scanr"); //254
    Createid("_foldl1"); //255
    Createid("_scanl1"); //256
    Createid("_foldr1"); //257
    Createid("_scanr1"); //258
    Createid("iterator_java"); //259 --> a_iteration_java
    Createid("java_vector"); //260 --> a_java_vector
    Createid("&predicate_terminal&"); //261 --> a_terminal
    Createid("_iferror"); //262 --> a_iferror
    Createid("frametype"); //263 --> a_frametype
    Createid("waitonjoined"); //264 --> a_waitonjoined
    Createid("lisp"); //265 a_lisp
    
    
    //This is a simple hack to handle "length" a typical Haskell operator as "size"...
    //Note that there will be a useless index
    
    Createid("length"); //245
    symbolIds["length"] = a_size;
    
    Createid("not"); //246
    symbolIds["not"] = a_negation;
    
    symbolIds["and"] = a_booleanand;
    symbolIds["or"] = a_booleanor;
    symbolIds["xor"] = a_booleanxor;
    symbolIds["append"] = a_merge;
    symbolIds["auto"] = a_self;
    
    symbolIds["?:-"] = a_predicatevar;
    
    atomics[a_boolean] = a_boolean;
    atomics[a_byte] = a_boolean;
    atomics[a_short] = a_short;
    atomics[a_int] = a_int;
    atomics[a_long] = a_long;
    atomics[a_decimal] = a_decimal;
    atomics[a_float] = a_float;
    atomics[a_string] = a_string;
    atomics[a_ustring] = a_ustring;
    atomics[a_fraction] = a_ustring;
    
    throughs[a_intthrough] = a_int;
    throughs[a_longthrough] = a_long;
    throughs[a_decimalthrough] = a_decimal;
    throughs[a_floatthrough] = a_float;
    throughs[a_stringthrough] = a_string;
    throughs[a_ustringthrough] = a_ustring;
    throughs[a_vectorthrough] = a_svector;
    throughs[a_mapthrough] = a_mapss;
    
    actions[a_plus] = new TamguAction(this, a_plus);
    actions[a_minus] = new TamguAction(this, a_minus);
    actions[a_multiply] = new TamguAction(this, a_multiply);
    actions[a_power] = new TamguAction(this, a_power);
    actions[a_shiftleft] = new TamguAction(this, a_shiftleft);
    actions[a_shiftright] = new TamguAction(this, a_shiftright);
    actions[a_divide] = new TamguAction(this, a_divide);
    actions[a_divideinteger] = new TamguAction(this, a_divideinteger);
    actions[a_mod] = new TamguAction(this, a_mod);
    actions[a_or] = new TamguAction(this, a_or);
    actions[a_xor] = new TamguAction(this, a_xor);
    actions[a_and] = new TamguAction(this, a_and);
    actions[a_merge] = new TamguAction(this, a_merge);
    actions[a_combine] = new TamguAction(this, a_combine);
    actions[a_add] = new TamguAction(this, a_add);
}

//------------------- Threads-------------------------------------
bool TamguGlobal::isFile(Tamgu* a) {
    if (!newInstance.check(a->Typevariable()))
        return false;
    return newInstance[a->Typevariable()]->isFile();
}

bool TamguGlobal::isString(Tamgu* a) {
    if (!newInstance.check(a->Typevariable()))
        return false;
    return newInstance[a->Typevariable()]->isString();
}

bool TamguGlobal::isNumber(Tamgu* a) {
    if (!newInstance.check(a->Typevariable()))
        return false;
    return newInstance[a->Typevariable()]->isNumber();
}

bool TamguGlobal::isContainer(Tamgu* a) {
    if (!newInstance.check(a->Typevariable()))
        return false;
    return newInstance[a->Typevariable()]->isContainer();
}

bool TamguGlobal::isVectorContainer(Tamgu* a) {
    if (!newInstance.check(a->Typevariable()))
        return false;
    return newInstance[a->Typevariable()]->isVectorContainer();
}

bool TamguGlobal::isMapContainer(Tamgu* a) {
    if (!newInstance.check(a->Typevariable()))
        return false;
    return newInstance[a->Typevariable()]->isMapContainer();
}

short TamguGlobal::Type(Tamgu* a) {
    if (!newInstance.check(a->Typevariable()))
        return false;
    return newInstance[a->Typevariable()]->Type();
}

//------------------- Threads-------------------------------------
Tamgu* ProcThreadhandle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    threadhandle tid = _GETTHREADID();
    stringstream s;
    s << tid;
    return globalTamgu->Providestring(s.str());
}

Exporting short TamguGlobal::GetThreadid() {
    if (!isthreading)
        return 0;
    
    int idthread;
    threadids.get(_GETTHREADID(), idthread);
    return idthread;
}

short TamguGlobal::InitThreadid(short idthread) {
    threadids.set(threads[idthread].Initialization(),idthread);
    return idthread;
}

Tamgu* TamguGlobal::Getmainframe(size_t idcode) {
    return spaces[idcode]->Mainframe();
}

void TamguGlobal::DeclareInMainframe(size_t idcode, short n, Tamgu* a) {
    spaces[idcode]->Mainframe()->Declare(n ,a);
}

Tamgu* TamguGlobal::DeclarationInMainframe(size_t idcode, short n) {
    return spaces[idcode]->Mainframe()->Declaration(n);
}

void TamguGlobal::DeclareTopstack(short idthread, short n, Tamgu* a) {
    threads[idthread].stack.back()->Declare(n, a);
}

Tamgu* TamguGlobal::DeclarationTopstack(short idthread, short n) {
    return threads[idthread].stack.back()->Declaration(n);
}

bool TamguGlobal::isDeclaredTopstack(short idthread, short n) {
    return threads[idthread].stack.back()->isDeclared(n);
}


bool TamguGlobal::TopstackisMainframe() {
    return threads[0].stack.back()->isMainFrame();
}


short TamguGlobal::SelectThreadid() {
    short i;
    for (i = 1; i < maxthreads; i++) {
        if (!threads[i].used) {
            threads[i].used = true;
            threadcounter++;
            return i;
        }
    }
    return -1;
}

void TamguGlobal::EraseThreadid(short id) {
    threadids.erase(threads[id].handle);
    threads[id].Clear();
    errors[id] = false;
    errorraised[id] = NULL;
    threadcounter--;
}

//------------------- TamguCode management-------------------------------------

TamguCode* TamguGlobal::GetNewCodeSpace(string filename) {
    filename = NormalizeFileName(filename);
    
    try {
        return pathnames.at(filename);
    }
    catch(const std::out_of_range& oor) {
        TamguCode* a = new TamguCode((short)spaces.size(), filename, this);
        spaces.push_back(a);
        pathnames[filename] = a;
        return a;
    }
}

//----------------------------------------------------------------------------------
Exporting void TamguGlobal::RecordInTracker(Tamgu* a) {
    long idx = trackerslots.backpop();
    if (idx == -1)
        a->Setid(tracked.push_back(a));
    else
        a->Setid(tracked.put(a, idx));
}

Exporting long TamguGlobal::RecordInTrackerProtected(Tamgu* a) {
    if (a->idtracker != -1)
        return a->idtracker;
    
    long idx = trackerslots.backpop();
    if (idx == -1)
        return a->Setidreturn(tracked.push_back(a));
    
    return a->Setidreturn(tracked.put(a, idx));
}

Exporting void TamguGlobal::ResetWithTracker(Tamgu* a, long idx, long inc) {
    if (!tracked.check(idx, a))
        return;
    a->Resetreference(inc);
}


Exporting TamguSystemVariable* TamguGlobal::CreateSystemVariable(Tamgu* v, short n, short t) {
    if (systems.check(n))
        return systems[n];
    
    return new TamguSystemVariable(this, v, n, t);
}

Exporting TamguSystemVariable* TamguGlobal::CreateSystemVariable(Tamgu* v, string nm, short t) {
    short n = Getid(nm);
    
    if (systems.check(n))
        return systems[n];
    
    return new TamguSystemVariable(this, v, n, t);
}

//----------------------------------------------------------------------------------
bool TamguGlobal::Loadcontent(string content) {
    //We directly parse the content...
    static tokenizer_result<string> xr;
    
    tamgu_tokenizer.tokenize<string>(content, xr);
    TamguCode* a = GetNewCodeSpace("SCRIPT");
    return a->Load(xr);
}

TamguCode* TamguGlobal::Loadfile(string filename) {
    tokenizer_result<string> xr;
    filename = NormalizeFileName(filename);
    
    string code;
    ifstream file(filename, openMode);
    
    if (file.fail())
        return NULL;
    
    char c = file.get();
    if (!file.eof()) {
        do {
            code += c;
            c = file.get();
            
        } while (!file.eof());
    }
    
    file.close();

    if (code[0] == '(' && code[1] == ')') {
        xr.lispmode = true;
        code[0] = '/';
        code[1] = '/';
    }

    code += Endl;
    
    long sp = spaceid;
    spaceid=spaces.size();
    TamguCode* a = GetNewCodeSpace(filename);

    std::chrono::high_resolution_clock::time_point before;
    std::chrono::high_resolution_clock::time_point after;
    
    before = std::chrono::high_resolution_clock::now();
    tamgu_tokenizer.tokenize<string>(code, xr);
    after = std::chrono::high_resolution_clock::now();
    double dtok = std::chrono::duration_cast<std::chrono::milliseconds>( after - before ).count();
    
    before = std::chrono::high_resolution_clock::now();
    a->Load(xr);
    after = std::chrono::high_resolution_clock::now();
    double dparse = std::chrono::duration_cast<std::chrono::milliseconds>( after - before ).count();
    
    TamguSystemVariable* vs = globalTamgu->systems[globalTamgu->Getid("_internals")];
    vs->value->storevalue((long)xr.stack.size());
    vs->value->storevalue(dtok);
    vs->value->storevalue(dparse);
    
    spaceid=sp;
    return a;
}

void TamguGlobal::TamguAllObjects(vector<string>& vs) {
    vs.clear();
    
    bin_hash<Tamgu*>::iterator it;
    string n;
    for (it = newInstance.begin(); it != newInstance.end(); it++) {
        n = Getsymbol(it->first);
        vs.push_back(n);
    }
    
    bin_hash<TamguProcedure>::iterator itp;
    for (itp = procedures.begin(); itp != procedures.end(); itp++) {
        n = Getsymbol(itp->first);
        vs.push_back(n);
    }
    
    bin_hash<unsigned long>::iterator itl;
    for (itl = allmethods.begin(); itl != allmethods.end(); itl++) {
        n = Getsymbol(itl->first);
        vs.push_back(n);
    }
    
    for (itl = framemethods.begin(); itl != framemethods.end(); itl++) {
        n = Getsymbol(itl->first);
        vs.push_back(n);
    }
    
    for (itp = commons.begin(); itp != commons.end(); itp++) {
        n = Getsymbol(itp->first);
        vs.push_back(n);
    }
    
    
    vs.push_back("alias");
    vs.push_back("a_change");
    vs.push_back("a_delete");
    vs.push_back("a_first");
    vs.push_back("a_insert");
    vs.push_back("a_switch");
    vs.push_back("a_nocase");
    vs.push_back("a_offsets");
    vs.push_back("a_features");
    vs.push_back("a_surface");
    vs.push_back("a_repetition");
    vs.push_back("a_vowel");
    vs.push_back("a_longest");
    vs.push_back("absent");
    vs.push_back("all");
    vs.push_back("and");
    vs.push_back("any");
    vs.push_back("append");
    vs.push_back("assert");
    vs.push_back("asserta");
    vs.push_back("assertz");
    vs.push_back("autorun");
    vs.push_back("block");
    vs.push_back("break");
    vs.push_back("case");
    vs.push_back("catch");
    vs.push_back("common");
    vs.push_back("concept");
    vs.push_back("role");
    vs.push_back("const");
    vs.push_back("continue");
    vs.push_back("cycle");
    vs.push_back("data");
    vs.push_back("default");
    vs.push_back("defun");
    vs.push_back("deriving");
    vs.push_back("do");
    vs.push_back("drop");
    vs.push_back("dropWhile");
    vs.push_back("e_latin_we");
    vs.push_back("e_latin_ce");
    vs.push_back("e_latin_se");
    vs.push_back("e_latin_ne");
    vs.push_back("e_cyrillic");
    vs.push_back("e_arabic");
    vs.push_back("e_greek");
    vs.push_back("e_hebrew");
    vs.push_back("e_turkish");
    vs.push_back("e_nordic");
    vs.push_back("e_thai");
    vs.push_back("e_baltic");
    vs.push_back("e_celtic");
    vs.push_back("e_latin_ffe");
    vs.push_back("e_latin_see");
    vs.push_back("e_windows");
    vs.push_back("e_cp1252");
    vs.push_back("elif");
    vs.push_back("else");
    vs.push_back("empty");
    vs.push_back("exclusive");
    vs.push_back("exit");
    vs.push_back("extension");
    vs.push_back("fail");
    vs.push_back("false");
    vs.push_back("fibre");
    vs.push_back("filter");
    vs.push_back("flip");
    vs.push_back("foldl");
    vs.push_back("foldr");
    vs.push_back("for");
    vs.push_back("frame");
    vs.push_back("from");
    vs.push_back("function");
    vs.push_back("get");
    vs.push_back("if");
    vs.push_back("iferror");
    vs.push_back("ifnot");
    vs.push_back("If");
    vs.push_back("IF");
    vs.push_back("in");
    vs.push_back("is");
    vs.push_back("iterator");
    vs.push_back("java_vector");
    vs.push_back("joined");
    vs.push_back("label");
    vs.push_back("lambda");
    vs.push_back("cons");
    vs.push_back("cond");
    vs.push_back("eq");
    vs.push_back("let");
    vs.push_back("length");
    vs.push_back("map");
    vs.push_back("Maybe");
    vs.push_back("Nothing");
    vs.push_back("namespace");
    vs.push_back("not");
    vs.push_back("notin");
    vs.push_back("null");
    vs.push_back("of");
    vs.push_back("ontology");
    vs.push_back("or");
    vs.push_back("otherwise");
    vs.push_back("polynomial");
    vs.push_back("ponder");
    vs.push_back("private");
    vs.push_back("property");
    vs.push_back("protected");
    vs.push_back("remove");
    vs.push_back("repeat");
    vs.push_back("replicate");
    vs.push_back("retract");
    vs.push_back("return");
    vs.push_back("scanl");
    vs.push_back("scanr");
    vs.push_back("setq");
    vs.push_back("static");
    vs.push_back("stop");
    vs.push_back("store");
    vs.push_back("strict");
    vs.push_back("switch");
    vs.push_back("table");
    vs.push_back("take");
    vs.push_back("takeWhile");
    vs.push_back("this");
    vs.push_back("thread");
    vs.push_back("true");
    vs.push_back("try");
    vs.push_back("where");
    vs.push_back("while");
    vs.push_back("with");
    vs.push_back("xor");
    vs.push_back("zip");
    vs.push_back("zipWith");
    
}

//----------------------------------------------------------------------------------------
Tamgu* Tamgu::Complexe() {
    return new Tamgucomplexe((double)Float(), 0);
}

Exporting Tamgu* Tamgu::Succ() {
    long v = Integer();
    return globalTamgu->ProvideConstint(v + 1);
}

Exporting Tamgu* Tamgu::Pred() {
    long v = Integer();
    return globalTamgu->ProvideConstint(v - 1);
}

Exporting void Tamgu::addustringto(wstring ws) {
    Tamgu* a = globalTamgu->Providewithustring(ws);
    Push(a);
    a->Release();
}

Exporting void Tamgu::addstringto(string s) {
    Tamgu* a = globalTamgu->Providewithstring(s);
    Push(a);
    a->Release();
}

Exporting void Tamgu::addstringto(wchar_t s) {
    wstring w;
    w = s;
    Tamgu* a = globalTamgu->Providewithustring(w);
    Push(a);
    a->Release();
    
}

Exporting void Tamgu::addustringto(wstring ws, int i) {
    if (Size() == 0)
        return;
    
    Tamgu* ke;
    if (i < 0)
        ke = Last();
    else {
        if (i >= Size())
            return;
        ke = getvalue(i);
    }
    
    ke->addustringto(ws);
}

Exporting void Tamgu::addstringto(string s, int i) {
    if (Size() == 0)
        return;
    
    Tamgu* ke;
    if (i < 0)
        ke = Last();
    else {
        if (i >= Size())
            return;
        ke = getvalue(i);
    }
    
    ke->addstringto(s);
}

Exporting void Tamgu::addstringto(wchar_t s, int i) {
    if (Size() == 0)
        return;
    
    Tamgu* ke;
    if (i < 0)
        ke = Last();
    else {
        if (i >= Size())
            return;
        ke = getvalue(i);
    }
    
    ke->addstringto(s);
}

Exporting void Tamgu::storevalue(Tamgu* a, long beg, long end) {
    for (; beg < end; beg++)
        Push(a->getvalue(beg));
    a->Release();
}

Exporting void Tamgu::storevalue(string u) {
    Tamgu* a = globalTamgu->Providewithstring(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Providewithustring(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(long u) {
    Push(globalTamgu->ProvideConstint(u));
}

Exporting void Tamgu::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(BLONG u) {
    Push(globalTamgu->ProvideConstlong(u));
}

Exporting void Tamgu::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Providewithustring(w);
    Push(a);
    a->Release();
}

//--------------------------------------------------------------------
// Some necessary declarations to record code
Exporting TamguTracked::TamguTracked(short t, TamguGlobal* g, Tamgu* parent) {
    investigate = is_tracked;
    idtype = t;
    idinfo = -1;
    
    if (parent != NULL)
        parent->AddInstruction(this);
    
    if (g != NULL) {
        g->RecordInTracker(this);
        long line = g->Getcurrentline();
        short idcode = (short)g->spaceid;
        short idfile = -1;
        if (idcode != -1)
            idfile = g->spaces[idcode]->currentfileid;
        idinfo = g->Addinfo(idcode, idfile, line);
    }
}

Exporting TamguTracked::TamguTracked(short t, TamguTracked* copie, TamguGlobal* g, Tamgu* parent) {
    investigate = is_tracked;
    idtype = t;
    idinfo = copie->idinfo;
    
    
    if (parent != NULL)
        parent->AddInstruction(this);
    
    if (g != NULL)
        g->RecordInTracker(this);
}

Exporting TamguTracked::TamguTracked(string t, TamguGlobal* g, Tamgu* parent) {
    investigate = is_tracked;
    
    idtype = 0;
    idinfo = -1;
    
    if (parent != NULL)
        parent->AddInstruction(this);
    
    if (g != NULL) {
        idtype = g->Getid(t);
        g->RecordInTracker(this);
        long line = g->Getcurrentline();
        short idcode = (short)g->spaceid;
        short idfile = -1;
        if (idcode != -1)
            idfile = g->spaces[idcode]->currentfileid;
        idinfo = g->Addinfo(idcode, idfile, line);
    }
}

//---------------------------------------------------------------------------------------------
void TamguLet::Storevalue(string& u) {
    if (value != aNOELEMENT)
        value->Storevalue(u);
    else {
        value = globalTamgu->Providewithstring(u);
        value->Setreference(reference);
    }
}

void TamguLet::Storevalue(wstring& u) {
    if (value != aNOELEMENT)
        value->Storevalue(u);
    else {
        value = globalTamgu->Providewithustring(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(string u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providewithstring(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(float u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = new Tamgudecimal(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(short u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = new Tamgushort(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(wstring u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providewithustring(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(long u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Provideint(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(BLONG u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providelong(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(double u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providefloat(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(unsigned char u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = new Tamgubyte(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(wchar_t u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        wstring w;
        w = u;
        value = globalTamgu->Providewithustring(w);
        value->Setreference(reference);
    }
}

//-------------------------------------------------------------------
void TamguSelf::Storevalue(string& u) {
    locking();
    if (typevalue == a_string)
        value->Storevalue(u);
    else {
        value = globalTamgu->Providewithstring(u);
        value->Setreference(reference);
        typevalue = a_string;
    }
    unlocking();
}

void TamguSelf::Storevalue(wstring& u) {
    locking();
    if (typevalue == a_ustring)
        ((Tamguustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Providewithustring(u);
        value->Setreference(reference);
        typevalue = a_ustring;
    }
    unlocking();
}


void TamguSelf::storevalue(string u) {
    locking();
    if (typevalue == a_string)
        ((Tamgustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Providewithstring(u);
        value->Setreference(reference);
        typevalue = a_string;
    }
    unlocking();
}

void TamguSelf::storevalue(float u) {
    locking();
    if (typevalue == a_decimal)
        ((Tamgudecimal*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = new Tamgudecimal(u);
        value->Setreference(reference);
        typevalue = a_decimal;
    }
    unlocking();
}

void TamguSelf::storevalue(short u) {
    locking();
    if (typevalue == a_short)
        ((Tamgushort*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = new Tamgushort(u);
        value->Setreference(reference);
        typevalue = a_short;
    }
    unlocking();
}

void TamguSelf::storevalue(wstring u) {
    locking();
    if (typevalue == a_ustring)
        ((Tamguustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Providewithustring(u);
        value->Setreference(reference);
        typevalue = a_ustring;
    }
    unlocking();
}

void TamguSelf::storevalue(long u) {
    locking();
    if (typevalue == a_int)
        ((Tamguint*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Provideint(u);
        value->Setreference(reference);
        typevalue = a_int;
    }
    unlocking();
}

void TamguSelf::storevalue(BLONG u) {
    locking();
    if (typevalue == a_long)
        ((Tamgulong*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Providelong(u);
        value->Setreference(reference);
        typevalue = a_long;
    }
    unlocking();
}

void TamguSelf::storevalue(double u) {
    locking();
    if (typevalue == a_float)
        ((Tamgufloat*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Providefloat(u);
        value->Setreference(reference);
        typevalue = a_float;
    }
    unlocking();
}

void TamguSelf::storevalue(unsigned char u) {
    locking();
    if (typevalue == a_byte)
        ((Tamgubyte*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = new Tamgubyte(u);
        value->Setreference(reference);
        typevalue = a_byte;
    }
    unlocking();
}

void TamguSelf::storevalue(wchar_t u) {
    locking();
    if (typevalue == a_ustring)
        ((Tamguustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        wstring w;
        w = u;
        value = globalTamgu->Providewithustring(w);
        value->Setreference(reference);
        typevalue = a_ustring;
    }
    unlocking();
}
//---------------------------------------------------------------------------------------------
void TamguRawSelf::Storevalue(string& u) {
    if (value != aNOELEMENT)
        value->Storevalue(u);
    else {
        value = globalTamgu->Providewithstring(u);
    }
}

void TamguRawSelf::Storevalue(wstring& u) {
    if (value != aNOELEMENT)
        value->Storevalue(u);
    else {
        value = globalTamgu->Providewithustring(u);
    }
}

void TamguRawSelf::storevalue(string u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providewithstring(u);
    }
}

void TamguRawSelf::storevalue(float u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = new Tamgudecimal(u);
    }
}

void TamguRawSelf::storevalue(short u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = new Tamgushort(u);
    }
}

void TamguRawSelf::storevalue(wstring u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providewithustring(u);
    }
}

void TamguRawSelf::storevalue(long u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Provideint(u);
    }
}

void TamguRawSelf::storevalue(BLONG u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providelong(u);
    }
}

void TamguRawSelf::storevalue(double u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providefloat(u);
    }
}

void TamguRawSelf::storevalue(unsigned char u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = new Tamgubyte(u);
    }
}

void TamguRawSelf::storevalue(wchar_t u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        wstring w;
        w = u;
        value = globalTamgu->Providewithustring(w);
    }
}
//---------------------------------------------------------------------------------------------
Exporting Tamgu* Tamgu::Push(Tamgu*, Tamgu*) {
    return this;
}

Exporting Tamgu* Tamgu::Push(BLONG k, Tamgu* v) {
    return Push(globalTamgu->ProvideConstlong(k), v);
}

Exporting Tamgu* Tamgu::Push(ushort k, Tamgu* v) {
    Tamgu* a = new Tamgushort(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(long k, Tamgu* v) {
    return Push(globalTamgu->ProvideConstint(k), v);
}

Exporting Tamgu* Tamgu::Push(double k, Tamgu* v) {
    Tamgu* a = globalTamgu->Providefloat(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(string k, Tamgu* v) {
    Tamgu* a = globalTamgu->Providewithstring(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(wstring k, Tamgu* v) {
    Tamgu* a = globalTamgu->Providewithustring(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Pushinmap(Tamgu* a, short idhtread) {
    if (a->isMapContainer()) {
        std::unique_ptr<TamguIteration> itr(a->Newiteration(false));
        itr->Begin();
        while (!itr->End()->Boolean()) {
            Push(itr->IteratorKey(), itr->IteratorValue());
            itr->Next();
        }
        return aTRUE;
    }
    return Push(Size(), a);
}

Exporting Tamgufraction* Tamgu::Fraction() {
    return new Tamgufraction(Float());
}

Exporting Tamgu* Tamgu::Invert(bool autoself) {
    Tamgu* f = new Tamgufraction(Float());
    Tamgu* res = f->Invert(true);
    if (res->isError()) {
        f->Release();
        return res;
    }
    return res;
}

//--------------------------------------------------------------
//--------------------------------------------------------------

Exporting Tamgu* TamguConstString::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!globalTamgu->checkarity(a_string,idname, callfunc->Size())) {
        string mess("'");
        mess += globalTamgu->Getsymbol(idname);
        mess += "' : Unknown method or Wrong number of arguments for type: 'string'";
        return globalTamgu->Returnerror(mess, idthread);
    }
    
    Tamgustring v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstUString::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!globalTamgu->checkarity(a_ustring,idname, callfunc->Size())) {
        string mess("'");
        mess += globalTamgu->Getsymbol(idname);
        mess += "' : Unknown method or Wrong number of arguments for type: 'ustring'";
        return globalTamgu->Returnerror(mess, idthread);
    }
    
    Tamguustring v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstInt::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    if (!globalTamgu->checkarity(a_int,idname, callfunc->Size())) {
        string mess("'");
        mess += globalTamgu->Getsymbol(idname);
        mess += "' : Unknown method or Wrong number of arguments for type: 'int'";
        return globalTamgu->Returnerror(mess, idthread);
    }
    Tamguint v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstShort::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    if (!globalTamgu->checkarity(a_short,idname, callfunc->Size())) {
        string mess("'");
        mess += globalTamgu->Getsymbol(idname);
        mess += "' : Unknown method or Wrong number of arguments for type: 'short'";
        return globalTamgu->Returnerror(mess, idthread);
    }
    
    Tamgushort v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstDecimal::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    if (!globalTamgu->checkarity(a_decimal,idname, callfunc->Size())) {
        string mess("'");
        mess += globalTamgu->Getsymbol(idname);
        mess += "' : Unknown method or Wrong number of arguments for type: 'decimal'";
        return globalTamgu->Returnerror(mess, idthread);
    }
    Tamgudecimal v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstFloat::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    if (!globalTamgu->checkarity(a_float,idname, callfunc->Size())) {
        string mess("'");
        mess += globalTamgu->Getsymbol(idname);
        mess += "' : Unknown method or Wrong number of arguments for type: 'float'";
        return globalTamgu->Returnerror(mess, idthread);
    }
    Tamgufloat v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstLong::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (!globalTamgu->checkarity(a_long,idname, callfunc->Size())) {
        string mess("'");
        mess += globalTamgu->Getsymbol(idname);
        mess += "' : Unknown method or Wrong number of arguments for type: 'long'";
        return globalTamgu->Returnerror(mess, idthread);
    }
    Tamgulong v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

void TamguConstString::Methods(Tamgu* vs) {
    Tamgustring v("");
    v.Methods(vs);
}

void TamguConstUString::Methods(Tamgu* vs) {
    Tamguustring v(L"");
    v.Methods(vs);
}

void TamguConstInt::Methods(Tamgu* vs)  {
    Tamguint v(0);
    v.Methods(vs);
}

void TamguConstShort::Methods(Tamgu* vs)  {
    Tamgushort v(0);
    v.Methods(vs);
}

void TamguConstDecimal::Methods(Tamgu* vs)  {
    Tamgudecimal v(0);
    v.Methods(vs);
}

void TamguConstFloat::Methods(Tamgu* vs)  {
    Tamgufloat v(0);
    v.Methods(vs);
}

void TamguConstLong::Methods(Tamgu* vs) {
    Tamgulong v(0);
    v.Methods(vs);
}

void TamguConstBool::Methods(Tamgu* vs) {
    Tamguboolean v(true);
    v.Methods(vs);
}

string TamguConstString::Info(string s) {
    Tamgustring v("");
    return v.Info(s);
}

string TamguConstUString::Info(string s) {
    Tamguustring v(L"");
    return v.Info(s);
}

string TamguConstInt::Info(string s)  {
    Tamguint v(0);
    return v.Info(s);
}

string TamguConstShort::Info(string s)  {
    Tamgushort v(0);
    return v.Info(s);
}

string TamguConstDecimal::Info(string s)  {
    Tamgudecimal v(0);
    return v.Info(s);
}

string TamguConstFloat::Info(string s)  {
    Tamgufloat v(0);
    return v.Info(s);
}

string TamguConstLong::Info(string s) {
    Tamgulong v(0);
    return v.Info(s);
}

string TamguConstBool::Info(string s) {
    Tamguboolean v(true);
    return v.Info(s);
}
//------------------------------------------------------------------------------------------------------------------------
Exporting Tamgumap* TamguGlobal::Providemap() {
    if (threadMODE)
        return new Tamgumap;
    
    Tamgumapbuff* ke;
    
    if (mapempties.last > 0) {
        ke = mapreservoire[mapempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = mapreservoire.size();
    
    while (mapidx < mx) {
        if (!mapreservoire[mapidx]->used) {
            mapreservoire[mapidx]->used = true;
            ke = mapreservoire[mapidx++];
            ke->Enablelock(0);
            return ke;
        }
        mapidx++;
    }
    
    long sz = mx >> 2;
    mapreservoire.resize(mx + sz);
    mapidx = mx + sz;
    for (long i = mx; i < mapidx; i++)
        mapreservoire[i] = (Tamgumapbuff*)Provideinstance(mapreservoire[0], i);
    
    mapidx = mx;
    mapreservoire[mapidx]->used = true;
    ke = mapreservoire[mapidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamgutreemap* TamguGlobal::Providetreemap() {
    if (threadMODE)
        return new Tamgutreemap;
    
    Tamgutreemapbuff* ke;
    
    if (treemapempties.last > 0) {
        ke = treemapreservoire[treemapempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = treemapreservoire.size();
    
    while (treemapidx < mx) {
        if (!treemapreservoire[treemapidx]->used) {
            treemapreservoire[treemapidx]->used = true;
            ke = treemapreservoire[treemapidx++];
            ke->Enablelock(0);
            return ke;
        }
        treemapidx++;
    }
    
    long sz = mx >> 2;
    treemapreservoire.resize(mx + sz);
    treemapidx = mx + sz;
    for (long i = mx; i < treemapidx; i++)
        treemapreservoire[i] = (Tamgutreemapbuff*)Provideinstance(treemapreservoire[0], i);
    
    treemapidx = mx;
    treemapreservoire[treemapidx]->used = true;
    ke = treemapreservoire[treemapidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamguprimemap* TamguGlobal::Provideprimemap() {
    if (threadMODE)
        return new Tamguprimemap;
    
    Tamguprimemapbuff* ke;
    
    if (primemapempties.last > 0) {
        ke = primemapreservoire[primemapempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = primemapreservoire.size();
    
    while (primemapidx < mx) {
        if (!primemapreservoire[primemapidx]->used) {
            primemapreservoire[primemapidx]->used = true;
            ke = primemapreservoire[primemapidx++];
            ke->Enablelock(0);
            return ke;
        }
        primemapidx++;
    }
    
    long sz = mx >> 2;
    primemapreservoire.resize(mx + sz);
    primemapidx = mx + sz;
    for (long i = mx; i < primemapidx; i++)
        primemapreservoire[i] = (Tamguprimemapbuff*)Provideinstance(primemapreservoire[0], i);
    
    primemapidx = mx;
    primemapreservoire[primemapidx]->used = true;
    ke = primemapreservoire[primemapidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamgumapss* TamguGlobal::Providemapss() {
    if (threadMODE)
        return new Tamgumapss;
    
    Tamgumapssbuff* ke;
    
    if (mapssempties.last > 0) {
        ke = mapssreservoire[mapssempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = mapssreservoire.size();
    
    while (mapssidx < mx) {
        if (!mapssreservoire[mapssidx]->used) {
            mapssreservoire[mapssidx]->used = true;
            ke = mapssreservoire[mapssidx++];
            ke->Enablelock(0);
            return ke;
        }
        mapssidx++;
    }
    
    long sz = mx >> 2;
    mapssreservoire.resize(mx + sz);
    mapssidx = mx + sz;
    for (long i = mx; i < mapssidx; i++)
        mapssreservoire[i] = (Tamgumapssbuff*)Provideinstance(mapssreservoire[0], i);
    
    mapssidx = mx;
    mapssreservoire[mapssidx]->used = true;
    ke = mapssreservoire[mapssidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamguvector* TamguGlobal::Providevector() {
    if (threadMODE)
        return new Tamguvector;
    
    Tamguvectorbuff* ke;
    
    if (vectorempties.last > 0) {
        ke = vectorreservoire[vectorempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = vectorreservoire.size();
    
    while (vectoridx < mx) {
        if (!vectorreservoire[vectoridx]->used) {
            vectorreservoire[vectoridx]->used = true;
            ke = vectorreservoire[vectoridx++];
            ke->Enablelock(0);
            return ke;
        }
        vectoridx++;
    }
    
    long sz = mx >> 2;
    vectorreservoire.resize(mx + sz);
    vectoridx = mx + sz;
    for (long i = mx; i < vectoridx; i++)
        vectorreservoire[i] = (Tamguvectorbuff*)Provideinstance(vectorreservoire[0], i);
    
    vectoridx = mx;
    vectorreservoire[vectoridx]->used = true;
    ke = vectorreservoire[vectoridx++];
    ke->Enablelock(0);
    return ke;
}


Exporting Tamguivector* TamguGlobal::Provideivector() {
    if (threadMODE)
        return new Tamguivector;
    
    Tamguivectorbuff* ke;
    
    if (ivectorempties.last > 0) {
        ke = ivectorreservoire[ivectorempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = ivectorreservoire.size();
    
    while (ivectoridx < mx) {
        if (!ivectorreservoire[ivectoridx]->used) {
            ivectorreservoire[ivectoridx]->used = true;
            ke = ivectorreservoire[ivectoridx++];
            ke->Enablelock(0);
            return ke;
        }
        ivectoridx++;
    }
    
    long sz = mx >> 2;
    ivectorreservoire.resize(mx + sz);
    ivectoridx = mx + sz;
    for (long i = mx; i < ivectoridx; i++)
        ivectorreservoire[i] = (Tamguivectorbuff*)Provideinstance(ivectorreservoire[0], i);
    
    ivectoridx = mx;
    ivectorreservoire[ivectoridx]->used = true;
    ke = ivectorreservoire[ivectoridx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamgufvector* TamguGlobal::Providefvector() {
    if (threadMODE)
        return new Tamgufvector;
    
    Tamgufvectorbuff* ke;
    if (fvectorempties.last > 0) {
        ke = fvectorreservoire[fvectorempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = fvectorreservoire.size();
    
    while (fvectoridx < mx) {
        if (!fvectorreservoire[fvectoridx]->used) {
            fvectorreservoire[fvectoridx]->used = true;
            ke = fvectorreservoire[fvectoridx++];
            ke->Enablelock(0);
            return ke;
        }
        fvectoridx++;
    }
    
    long sz = mx >> 2;
    fvectorreservoire.resize(mx + sz);
    fvectoridx = mx + sz;
    for (long i = mx; i < fvectoridx; i++)
        fvectorreservoire[i] = (Tamgufvectorbuff*)Provideinstance(fvectorreservoire[0], i);
    
    fvectoridx = mx;
    fvectorreservoire[fvectoridx]->used = true;
    ke = fvectorreservoire[fvectoridx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamgusvector* TamguGlobal::Providesvector() {
    if (threadMODE)
        return new Tamgusvector;
    
    Tamgusvectorbuff* ke;
    if (svectorempties.last > 0) {
        ke = svectorreservoire[svectorempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = svectorreservoire.size();
    
    while (svectoridx < mx) {
        if (!svectorreservoire[svectoridx]->used) {
            svectorreservoire[svectoridx]->used = true;
            ke = svectorreservoire[svectoridx++];
            ke->Enablelock(0);
            return ke;
        }
        svectoridx++;
    }
    
    long sz = mx >> 2;
    svectorreservoire.resize(mx + sz);
    svectoridx = mx + sz;
    for (long i = mx; i < svectoridx; i++)
        svectorreservoire[i] = (Tamgusvectorbuff*)Provideinstance(svectorreservoire[0], i);
    
    svectoridx = mx;
    svectorreservoire[svectoridx]->used = true;
    ke = svectorreservoire[svectoridx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamguuvector* TamguGlobal::Provideuvector() {
    if (threadMODE)
        return new Tamguuvector;
    
    Tamguuvectorbuff* ke;
    if (uvectorempties.last > 0) {
        ke = uvectorreservoire[uvectorempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = uvectorreservoire.size();
    
    while (uvectoridx < mx) {
        if (!uvectorreservoire[uvectoridx]->used) {
            uvectorreservoire[uvectoridx]->used = true;
            ke = uvectorreservoire[uvectoridx++];
            ke->Enablelock(0);
            return ke;
        }
        uvectoridx++;
    }
    
    long sz = mx >> 2;
    uvectorreservoire.resize(mx + sz);
    uvectoridx = mx + sz;
    for (long i = mx; i < uvectoridx; i++)
        uvectorreservoire[i] = (Tamguuvectorbuff*)Provideinstance(uvectorreservoire[0], i);
    
    uvectoridx = mx;
    uvectorreservoire[uvectoridx]->used = true;
    ke = uvectorreservoire[uvectoridx++];
    ke->Enablelock(0);
    return ke;
}

Exporting TamguSelf* TamguGlobal::Provideself() {
    if (threadMODE)
        return new TamguSelf;
    
    Tamguselfbuff* ke;
    
    if (slfempties.last > 0) {
        ke = slfreservoire[slfempties.backpop()];
        ke->used = true;
        ke->value = aNOELEMENT;
        ke->typevalue = a_const;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = slfreservoire.size();
    
    while (slfidx < mx) {
        if (!slfreservoire[slfidx]->used) {
            ke = slfreservoire[slfidx++];
            ke->used = true;
            ke->value = aNOELEMENT;
            ke->typevalue = a_const;
            ke->Enablelock(0);
            return ke;
        }
        slfidx++;
    }
    
    long sz = mx >> 2;
    slfreservoire.resize(mx + sz);
    slfidx = mx + sz;
    for (long i = mx; i < slfidx; i++)
        slfreservoire[i] = (Tamguselfbuff*)Provideinstance(slfreservoire[0], i);
    
    slfidx = mx;
    slfreservoire[slfidx]->used = true;
    ke = slfreservoire[slfidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamgu* TamguGlobal::ProvideConstfloat(double v) {
    Tamgu* value;
    if (threadMODE) {
        _numberlock.Locking();
        value = float_pool[v];
        if (value == NULL) {
            value = new TamguConstFloat(v);
            float_pool[v] = value;
        }
        _numberlock.Unlocking();
        return value;
    }
    
    value = float_pool[v];
    if (value == NULL) {
        value = new TamguConstFloat(v);
        float_pool[v] = value;
    }
    
    return value;
}

Exporting Tamgu* TamguGlobal::ProvideConstString(string v, Tamgu* parent) {
    Tamgu* vstr = string_pool[v];
    if (vstr != NULL) {
        if (parent != NULL)
            parent->AddInstruction(vstr);
        return vstr;
    }
    
    vstr = new TamguConstString(v,parent);
    string_pool[v] = (TamguConstString*)vstr;
    return vstr;
}


Exporting Tamgu* TamguGlobal::ProvideConstint(long v) {
    Tamgu* value;
    if (threadMODE) {
        _numberlock.Locking();
        value = integer_pool[v];
        if (value == NULL) {
            value = new TamguConstInt(v);
            integer_pool[v] = value;
        }
        _numberlock.Unlocking();
        return value;
    }
    
    value = integer_pool[v];
    if (value == NULL) {
        value = new TamguConstInt(v);
        integer_pool[v] = value;
    }
    return value;
}

Exporting Tamgu* TamguGlobal::ProvideConstlong(BLONG v) {
    Tamgu* value;
    if (threadMODE) {
        _numberlock.Locking();
        value = integer_pool[v];
        if (value == NULL) {
            value = new TamguConstInt(v);
            integer_pool[v] = value;
        }
        _numberlock.Unlocking();
        return value;
    }
    
    value = integer_pool[v];
    if (value == NULL) {
        value = new TamguConstInt(v);
        integer_pool[v] = value;
    }
    return value;
}


Exporting Tamguint* TamguGlobal::Provideint(long v) {
    if (threadMODE)
        return new Tamguint(v);
    
    Tamguintbuff* ke;
    
    if (iempties.last > 0) {
        ke = intreservoire[iempties.backpop()];
        ke->used = true;
        ke->value = v;
        return ke;
    }
    
    long mx = intreservoire.size();
    
    while (intidx < mx) {
        if (!intreservoire[intidx]->used) {
            intreservoire[intidx]->used = true;
            intreservoire[intidx]->value = v;
            ke = intreservoire[intidx++];
            return ke;
        }
        intidx++;
    }
    
    long sz = mx >> 2;
    intreservoire.resize(mx + sz);
    intidx = mx + sz;
    for (long i = mx; i < intidx; i++)
        intreservoire[i] = (Tamguintbuff*)Provideinstance(intreservoire[0], i);
    
    intidx = mx;
    intreservoire[intidx]->used = true;
    intreservoire[intidx]->value = v;
    ke = intreservoire[intidx++];
    return ke;
}


Exporting Tamgulong* TamguGlobal::Providelong(BLONG v) {
    if (threadMODE)
        return new Tamgulong(v);
    
    Tamgulongbuff* ke;
    
    if (lgempties.last > 0) {
        ke = longreservoire[lgempties.backpop()];
        ke->used = true;
        ke->value = v;
        return ke;
    }
    
    long mx = longreservoire.size();
    
    while (longidx < mx) {
        if (!longreservoire[longidx]->used) {
            longreservoire[longidx]->used = true;
            longreservoire[longidx]->value = v;
            ke = longreservoire[longidx++];
            return ke;
        }
        longidx++;
    }
    
    long sz = mx >> 2;
    longreservoire.resize(mx + sz);
    longidx = mx + sz;
    for (long i = mx; i < longidx; i++)
        longreservoire[i] = (Tamgulongbuff*)Provideinstance(longreservoire[0], i);
    
    longidx = mx;
    longreservoire[longidx]->used = true;
    longreservoire[longidx]->value = v;
    ke = longreservoire[longidx++];
    return ke;
}

Exporting Tamgufloat* TamguGlobal::Providefloat(double v) {
    if (threadMODE)
        return new Tamgufloat(v);
    
    Tamgufloatbuff* ke;
    if (fempties.last > 0) {
        ke = floatreservoire[fempties.backpop()];
        ke->used = true;
        ke->value = v;
        return ke;
    }
    
    long mx = floatreservoire.size();
    
    while (floatidx < mx) {
        if (!floatreservoire[floatidx]->used) {
            floatreservoire[floatidx]->used = true;
            floatreservoire[floatidx]->value = v;
            ke = floatreservoire[floatidx++];
            return ke;
        }
        floatidx++;
    }
    
    long sz = mx >> 2;
    floatreservoire.resize(mx + sz);
    floatidx = mx + sz;
    for (long i = mx; i < floatidx; i++)
        floatreservoire[i] = (Tamgufloatbuff*)Provideinstance(floatreservoire[0], i);
    
    floatidx = mx;
    floatreservoire[floatidx]->used = true;
    floatreservoire[floatidx]->value = v;
    ke = floatreservoire[floatidx++];
    return ke;
}

Exporting TamguDeclarationLocal* TamguGlobal::Providedeclaration(short idt) {
    TamguDeclarationLocal* ke;
    
    if (threadMODE) {
        ke = new TamguDeclarationLocal(-1);
        ke->idthread = idt;
        return ke;
    }
    
    if (declempties.last > 0) {
        ke = declarationreservoire[declempties.backpop()];
        ke->used = true;
        ke->idthread = idt;
        return ke;
    }
    
    long mx = declarationreservoire.size();
    
    while (declarationidx < mx) {
        if (!declarationreservoire[declarationidx]->used) {
            
            ke = declarationreservoire[declarationidx++];
            ke->used = true;
            ke->idthread = idt;
            return ke;
        }
        declarationidx++;
    }
    
    long sz = mx >> 2;
    declarationreservoire.resize(mx + sz);
    declarationidx = mx + sz;
    for (long i = mx; i < declarationidx; i++)
        declarationreservoire[i] = new TamguDeclarationLocal(i);
    
    declarationidx = mx;
    ke = declarationreservoire[declarationidx++];
    ke->used = true;
    ke->idthread = idt;
    return ke;
}

Exporting Tamgulisp* TamguGlobal::Providelisp() {
    if (threadMODE)
        return new Tamgulisp(-1);
    
    Tamgulisp* ke;
    if (lempties.last > 0) {
        ke = lispreservoire[lempties.backpop()];
        ke->used = true;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = lispreservoire.size();
    
    while (lispidx < mx) {
        if (!lispreservoire[lispidx]->used) {
            lispreservoire[lispidx]->used = true;
            ke = lispreservoire[lispidx++];
            ke->Enablelock(0);
            return ke;
        }
        lispidx++;
    }
    
    long sz = mx >> 2;
    lispreservoire.resize(mx + sz);
    lispidx = mx + sz;
    for (long i = mx; i < lispidx; i++)
        lispreservoire[i] = new Tamgulisp(i);
    
    lispidx = mx;
    lispreservoire[lispidx]->used = true;
    ke = lispreservoire[lispidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamgustring* TamguGlobal::Providestring(string v) {
    if (threadMODE)
        return new Tamgustring(v);
    
    Tamgustringbuff* ke;
    if (sempties.last > 0) {
        ke = stringreservoire[sempties.backpop()];
        ke->used = true;
        ke->value = v;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = stringreservoire.size();
    
    while (stringidx < mx) {
        if (!stringreservoire[stringidx]->used) {
            stringreservoire[stringidx]->used = true;
            stringreservoire[stringidx]->value = v;
            ke = stringreservoire[stringidx++];
            ke->Enablelock(0);
            return ke;
        }
        stringidx++;
    }
    
    long sz = mx >> 2;
    stringreservoire.resize(mx + sz);
    stringidx = mx + sz;
    for (long i = mx; i < stringidx; i++)
        stringreservoire[i] = (Tamgustringbuff*)Provideinstance(stringreservoire[0], i);
    
    stringidx = mx;
    stringreservoire[stringidx]->used = true;
    stringreservoire[stringidx]->value = v;
    ke = stringreservoire[stringidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamguustring* TamguGlobal::Provideustring(wstring v) {
    if (threadMODE)
        return new Tamguustring(v);
    
    Tamguustringbuff* ke;
    if (uempties.last > 0) {
        ke = ustringreservoire[uempties.backpop()];
        ke->used = true;
        ke->value = v;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = ustringreservoire.size();
    
    while (ustringidx < mx) {
        if (!ustringreservoire[ustringidx]->used) {
            ustringreservoire[ustringidx]->value = v;
            ustringreservoire[ustringidx]->used = true;
            ke = ustringreservoire[ustringidx++];
            ke->Enablelock(0);
            return ke;
        }
        ustringidx++;
    }
    
    long sz = mx >> 2;
    ustringreservoire.resize(mx + sz);
    ustringidx = mx + sz;
    for (long i = mx; i < ustringidx; i++)
        ustringreservoire[i] = (Tamguustringbuff*)Provideinstance(ustringreservoire[0], i);
    
    ustringidx = mx;
    ustringreservoire[ustringidx]->used = true;
    ustringreservoire[ustringidx]->value = v;
    ke = ustringreservoire[ustringidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamgustring* TamguGlobal::Providewithstring(string& v) {
    if (threadMODE)
        return new Tamgustring(v);
    
    Tamgustringbuff* ke;
    if (sempties.last > 0) {
        ke = stringreservoire[sempties.backpop()];
        ke->used = true;
        ke->value = v;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = stringreservoire.size();
    
    while (stringidx < mx) {
        if (!stringreservoire[stringidx]->used) {
            stringreservoire[stringidx]->used = true;
            stringreservoire[stringidx]->value = v;
            ke = stringreservoire[stringidx++];
            ke->Enablelock(0);
            return ke;
        }
        stringidx++;
    }
    
    long sz = mx >> 2;
    stringreservoire.resize(mx + sz);
    stringidx = mx + sz;
    for (long i = mx; i < stringidx; i++)
        stringreservoire[i] = (Tamgustringbuff*)Provideinstance(stringreservoire[0], i);
    
    stringidx = mx;
    stringreservoire[stringidx]->used = true;
    stringreservoire[stringidx]->value = v;
    ke = stringreservoire[stringidx++];
    ke->Enablelock(0);
    return ke;
}

Exporting Tamguustring* TamguGlobal::Providewithustring(wstring& v) {
    if (threadMODE)
        return new Tamguustring(v);
    
    Tamguustringbuff* ke;
    if (uempties.last > 0) {
        ke = ustringreservoire[uempties.backpop()];
        ke->used = true;
        ke->value = v;
        ke->Enablelock(0);
        return ke;
    }
    
    long mx = ustringreservoire.size();
    
    while (ustringidx < mx) {
        if (!ustringreservoire[ustringidx]->used) {
            ustringreservoire[ustringidx]->value = v;
            ustringreservoire[ustringidx]->used = true;
            ke = ustringreservoire[ustringidx++];
            ke->Enablelock(0);
            return ke;
        }
        ustringidx++;
    }
    
    long sz = mx >> 2;
    ustringreservoire.resize(mx + sz);
    ustringidx = mx + sz;
    for (long i = mx; i < ustringidx; i++)
        ustringreservoire[i] = (Tamguustringbuff*)Provideinstance(ustringreservoire[0], i);
    
    ustringidx = mx;
    ustringreservoire[ustringidx]->used = true;
    ustringreservoire[ustringidx]->value = v;
    ke = ustringreservoire[ustringidx++];
    ke->Enablelock(0);
    return ke;
}

//--------------------------------------------------------------------
string TamguGlobal::ErrorMessage() {
    if (errorraised[0] != NULL) {
        string filename = Getcurrentfile();
        long line = Getcurrentline();
        stringstream message;
        
        Getstack(message);
        message << errorraised[0]->String() << " at " << line;
        if (!store_in_code_lines) {
            if (filename != "")
                message << " in " << filename;
        }
        return message.str();
    }
    return threads[0].message.str();
}

void TamguGlobal::PushErrorInEvent(Tamgu* a) {
    if (a->isError() && event_variable != NULL) {
        if (event_variable->Size())
            event_variable->addstringonly("\n");
        Tamgu* str = Providestring(ErrorMessage());
        event_variable->Push(str);
        str->Release();
    }
}

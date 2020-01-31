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
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgusvector.h"
#include "tamguuvector.h"
#include "predicate.h"
#include "tamguautomaton.h"
#include "tamguannotator.h"
#include <memory>
#include "vecte.h"
#include "tamgutaskell.h"

//----------------------------------------------------------------------------------
const char* tamgu_version = "Tamgu 1.2020.01.31";

Tamgu* booleantamgu[2];

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
//----------------------------------------------------------------------------------
static bool globalconstantes = false;
bool executionbreak = false;

Exchanging Tamgu* aNULL  = NULL;
Exchanging Tamgu* aUNIVERSAL  = NULL;
Exchanging Tamgu* aTRUE  = NULL;
Exchanging Tamgu* aFALSE  = NULL;

Exchanging Tamgu* aMINUSTEN  = NULL;
Exchanging Tamgu* aMINUSNINE  = NULL;
Exchanging Tamgu* aMINUSEIGHT  = NULL;
Exchanging Tamgu* aMINUSSEVEN  = NULL;
Exchanging Tamgu* aMINUSSIX  = NULL;
Exchanging Tamgu* aMINUSFIVE  = NULL;
Exchanging Tamgu* aMINUSFOUR  = NULL;
Exchanging Tamgu* aMINUSTHREE  = NULL;
Exchanging Tamgu* aMINUSTWO  = NULL;
Exchanging Tamgu* aMINUSONE  = NULL;
Exchanging Tamgu* aZERO  = NULL;
Exchanging Tamgu* aZEROPOINTZERO = NULL;
Exchanging Tamgu* aONE  = NULL;
Exchanging Tamgu* aTWO  = NULL;
Exchanging Tamgu* aTHREE  = NULL;
Exchanging Tamgu* aFOUR  = NULL;
Exchanging Tamgu* aFIVE  = NULL;
Exchanging Tamgu* aSIX  = NULL;
Exchanging Tamgu* aSEVEN  = NULL;
Exchanging Tamgu* aEIGHT  = NULL;
Exchanging Tamgu* aNINE  = NULL;
Exchanging Tamgu* aTEN  = NULL;
Exchanging Tamgu* aELEVEN  = NULL;
Exchanging Tamgu* aTWELVE  = NULL;
Exchanging Tamgu* aTHIRTEEN  = NULL;
Exchanging Tamgu* aFOURTEEN  = NULL;
Exchanging Tamgu* aFIFTEEN  = NULL;
Exchanging Tamgu* aSIXTEEN  = NULL;
Exchanging Tamgu* aSEVENTEEN  = NULL;
Exchanging Tamgu* aEIGHTEEN  = NULL;
Exchanging Tamgu* aNINETEEN  = NULL;
Exchanging Tamgu* aTWENTY  = NULL;
Exchanging Tamgu* aTHIRTYTWO  = NULL;
Exchanging Tamgu* aSIXTYFOUR  = NULL;

Exchanging Tamgu* aEMPTYSTRING  = NULL;
Exchanging Tamgu* aEMPTYUSTRING  = NULL;
Exchanging Tamgu* aBREAK  = NULL;
Exchanging Tamgu* aCONTINUE  = NULL;
Exchanging Tamgu* aRETURN  = NULL;
Exchanging Tamgu* aPIPE  = NULL;
Exchanging Tamgu* aNOELEMENT  = NULL;
Exchanging Tamgu* aDEFAULT  = NULL;
Exchanging Tamgu* aEND  = NULL;
Exchanging Tamgu* aRAISEERROR  = NULL;
Exchanging TamguCallBreak* aBREAKFALSE  = NULL;
Exchanging TamguCallBreak* aBREAKTRUE  = NULL;
Exchanging TamguCallBreak* aBREAKONE  = NULL;
Exchanging TamguCallBreak* aBREAKZERO  = NULL;
Exchanging Tamgu* aAFFECTATION  = NULL;
Exchanging TamguConstiteration* aITERNULL  = NULL;
Exchanging TamguPredicate* aFAIL  = NULL;
Exchanging TamguPredicate* aCUT  = NULL;
Exchanging TamguPredicate* aSTOP  = NULL;
Exchanging Tamgu* aHASKELL  = NULL;
Exchanging TamguDeclaration* aNULLDECLARATION  = NULL;
Exchanging TamguLet* aNULLLet  = NULL;
Exchanging Tamgu* aNOTHING  = NULL;


//For those who cannot tolerate non cleaned global variables...
void FinalTamguConstantCleaning(void) {
    if (globalconstantes) {

        delete aITERNULL ;

        delete  aNULL ;
        delete  aUNIVERSAL ;
        delete  aTRUE ;
        delete  aFALSE ;

        delete  aMINUSTEN ;
        delete  aMINUSNINE ;
        delete  aMINUSEIGHT ;
        delete  aMINUSSEVEN ;
        delete  aMINUSSIX ;
        delete  aMINUSFIVE ;
        delete  aMINUSFOUR ;
        delete  aMINUSTHREE ;
        delete  aMINUSTWO ;
        delete  aMINUSONE ;
        delete  aZERO ;
        delete  aZEROPOINTZERO ;
        delete  aONE ;
        delete  aTWO ;
        delete  aTHREE ;
        delete  aFOUR ;
        delete  aFIVE ;
        delete  aSIX ;
        delete  aSEVEN ;
        delete  aEIGHT ;
        delete  aNINE ;
        delete  aTEN ;
        delete aELEVEN;
        delete aTWELVE;
        delete aTHIRTEEN;
        delete aFOURTEEN;
        delete aFIFTEEN;
        delete aSIXTEEN;
        delete aSEVENTEEN;
        delete aEIGHTEEN;
        delete aNINETEEN;
        delete aTWENTY;
        delete aTHIRTYTWO;
        delete aSIXTYFOUR;


        delete  aEMPTYSTRING ;
        delete  aEMPTYUSTRING ;
        delete  aBREAK ;
        delete  aCONTINUE ;
        delete  aRETURN ;
        delete  aPIPE ;
        delete  aNOELEMENT ;
        delete  aDEFAULT ;
        delete  aEND ;
        delete  aRAISEERROR ;
        delete aBREAKFALSE ;
        delete aBREAKTRUE ;
        delete aBREAKZERO ;
        delete aBREAKONE ;
        delete  aAFFECTATION ;
        delete aFAIL ;
        delete aCUT ;
        delete aSTOP ;
        delete  aHASKELL ;
        delete aNULLDECLARATION ;
        delete aNULLLet ;
        delete aNOTHING;

        aNULL  = NULL;
        aUNIVERSAL  = NULL;
        aTRUE  = NULL;
        aFALSE  = NULL;

        aMINUSTEN  = NULL;
        aMINUSNINE  = NULL;
        aMINUSEIGHT  = NULL;
        aMINUSSEVEN  = NULL;
        aMINUSSIX  = NULL;
        aMINUSFIVE  = NULL;
        aMINUSFOUR  = NULL;
        aMINUSTHREE  = NULL;
        aMINUSTWO  = NULL;
        aMINUSONE  = NULL;
        aZERO  = NULL;
        aZEROPOINTZERO  = NULL;
        aONE  = NULL;
        aTWO  = NULL;
        aTHREE  = NULL;
        aFOUR  = NULL;
        aFIVE  = NULL;
        aSIX  = NULL;
        aSEVEN  = NULL;
        aEIGHT  = NULL;
        aNINE  = NULL;
        aTEN  = NULL;
        aELEVEN  = NULL;
        aTWELVE  = NULL;
        aTHIRTEEN  = NULL;
        aFOURTEEN  = NULL;
        aFIFTEEN  = NULL;
        aSIXTEEN  = NULL;
        aSEVENTEEN  = NULL;
        aEIGHTEEN  = NULL;
        aNINETEEN  = NULL;
        aTWENTY  = NULL;
        aTHIRTYTWO  = NULL;
        aSIXTYFOUR  = NULL;

        aEMPTYSTRING  = NULL;
        aEMPTYUSTRING  = NULL;
        aBREAK  = NULL;
        aCONTINUE  = NULL;
        aRETURN  = NULL;
        aPIPE  = NULL;
        aNOELEMENT  = NULL;
        aDEFAULT  = NULL;
        aEND  = NULL;
        aRAISEERROR  = NULL;
        aBREAKFALSE  = NULL;
        aBREAKTRUE  = NULL;
        aBREAKZERO  = NULL;
        aBREAKONE  = NULL;
        aAFFECTATION  = NULL;
        aITERNULL  = NULL;
        aFAIL  = NULL;
        aCUT  = NULL;
        aSTOP  = NULL;
        aHASKELL  = NULL;
        aNULLDECLARATION  = NULL;
        aNULLLet  = NULL;
        aNOTHING = NULL;

        globalconstantes = false;
    }
}

//----------------------------------------------------------------------------------

// Debug area, to detect potential memory leaks...
#ifdef GARBAGEFORDEBUG

static vector<Tamgu*> garbage;
ThreadLock _garbaging;

Exporting Tamgu::Tamgu() {
    idtracker = -1;
    investigate = is_none;

    if (globalTamgu != NULL && globalTamgu->globalLOCK) {
        Locking _lock(_garbaging);
        iddebug = garbage.size();
        garbage.push_back(this);
    }
    else {
        iddebug = garbage.size();
        garbage.push_back(this);
    }
}

Exporting Tamgu::~Tamgu() {
    Locking _lock(_garbaging);
    garbage[iddebug] = NULL;
}

void Garbaging(vector<Tamgu*>& issues, vector<long>& idissues) {
    issues.clear();
    idissues.clear();
    Tamgu* g;
    Locking _lock(_garbaging);
    for (size_t i = 0; i < garbage.size(); i++) {
        g = garbage[i];
        if (g != NULL) {
            if (g->idtracker == -1 && g->Reference() == 0) {
                issues.push_back(g);
                idissues.push_back(i);
            }
        }
    }
}


#endif

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

TamguRaiseError::TamguRaiseError(stringstream& mes, string file, size_t l, size_t r) {
    message = mes.str();

    filename = file;
    left = l;
    right = r;
}

//----------------------------------------------------------------------------------
ThreadStruct::ThreadStruct() : stack(1000), variables(false) {
    fstcompanion = NULL;
    prologstack = 0;
    returnvalue = NULL;
    previousinstruction=NULL;
    currentinstruction = NULL;
    used = false;
    parentthreads = -1;
    handle = _GETTHREADID();
    nbjoined = 0;
}

void ThreadStruct::Update(short idthread) {
    variables = globalTamgu->threads[idthread].variables;
}

void ThreadStruct::Clear() {
    nbjoined = 0;
    prologstack = 0;
    stack.clear();
    message.str("");
    variables.clear();
    returnvalue = aNULL;
    used = false;
}

threadhandle ThreadStruct::Initialization() {
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


void ThreadStruct::Setknowledgebase() {
    Locking* _lock = _getlock(globalTamgu->_knowledgelock);
    if (globalTamgu->threads[0].knowledgebase.size()) {
        long i;

        basebinn_hash<vector<TamguPredicate*> >::iterator it;

        for (it = globalTamgu->threads[0].knowledgebase.begin(); it != globalTamgu->threads[0].knowledgebase.end(); it++) {
            for (i = 0; i < it->second.size(); i++) {
                knowledgebase[it->first].push_back(it->second[i]);
                it->second[i]->Setreference();
            }
        }
    }
    _cleanlock(_lock);
}


void ThreadStruct::Clearknowledgebase() {
    basebinn_hash<vector<TamguPredicate*> >::iterator itk;
    for (itk = knowledgebase.begin(); itk != knowledgebase.end(); itk++) {
        for (size_t i = 0; i < itk->second.size(); i++) {
            itk->second[i]->Setprotect(false);
            itk->second[i]->Resetreference();
        }
    }

    knowledgebase.clear();
}

Exporting ThreadStruct::~ThreadStruct() {
    Clearknowledgebase();
    if (fstcompanion!=NULL)
        delete fstcompanion;
}

//----------------------------------------------------------------------------------
TamguGlobal::TamguGlobal(long nb, bool setglobal) :
idSymbols(false), methods(false), compatibilities(false), strictcompatibilities(false),
    operator_strings(false), terms(false), booleanlocks(true), tracked(NULL, true) {

#ifdef GARBAGEFORDEBUG
        garbage.clear();
#endif
        
        threadcounter = 0;
        trackerslotfilled = 0;

        waitingonfalse = false;

        SetThreadid();

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

        globalLOCK = false;
        isthreading = false;

        ThreadLock::ids = 0;

        maxstack = 1000;

        debugmode = false;
        inittableutf8();
        currentbnf = NULL;
        maxthreads = nb;
        erroronkey = false;
        windowmode = false;
        spaceid = -1;
        running = false;


        threads = new ThreadStruct[maxthreads];
        errors = new bool[maxthreads];
        errorraised = new TamguError*[maxthreads];

        long i;
        for (i = 0; i < maxthreads; i++) {
            errors[i] = false;
            errorraised[i] = NULL;
        }

        predicateContainer = NULL;

        linereference = 0;
        lineerror = -1;

        kstd = NULL;
        kerr = NULL;
		if (setglobal)
			globalTamgu = this;

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
        executionbreak = false;
        //---------------------------------
        debugfunction = NULL;
        debugdata = NULL;
        terminationfunction = NULL;

        //---------------------------------
        gTheAnnotationRules=NULL;
        gAutomatons=NULL;
        //---------------------------------
        long mx = 1000;
        //---------------------------------
        for (i = 0; i < mx; i++) {
            intreservoire.push_back(new Tamguintbuff(i));
            floatreservoire.push_back(new Tamgufloatbuff(i));
            stringreservoire.push_back(new Tamgustringbuff(i));
            ustringreservoire.push_back(new Tamguustringbuff(i));
            declarationreservoire.push_back(new TamguDeclarationLocal(i));
            declarationcleanreservoire.push_back(new TamguDeclarationAutoClean(i));
        }

        intidx = 0;
        slfidx = 0;
        floatidx = 0;
        stringidx = 0;
        ustringidx = 0;
        declarationidx = 0;
        declarationcleanidx = 0;

        indexidx = 0;
        mapidx = 0;
        mapssidx = 0;
        vectoridx = 0;
        ivectoridx = 0;
        fvectoridx = 0;
        svectoridx = 0;
        uvectoridx = 0;

        //---------------------------------
        mx >>= 3;
        for (long i = 0; i < mx; i++) {
            indexreservoire.push_back(new TamguIndexbuff(i));
            slfreservoire.push_back(new Tamguselfbuff(i));
            vectorreservoire.push_back(new Tamguvectorbuff(i));
            mapreservoire.push_back(new Tamgumapbuff(i));
            mapssreservoire.push_back(new Tamgumapssbuff(i));
            ivectorreservoire.push_back(new Tamguivectorbuff(i));
            fvectorreservoire.push_back(new Tamgufvectorbuff(i));
            svectorreservoire.push_back(new Tamgusvectorbuff(i));
            uvectorreservoire.push_back(new Tamguuvectorbuff(i));
        }
    }

void Tamgu::Deletion() {
    if (idtracker != -1)
        return;

    if (isObjectContainer()) {
        TamguIteration* itr = Newiteration(true);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next())
            itr->Value()->Deletion();
        delete itr;
    }

    if (Candelete())
        delete this;
}

TamguGlobal::~TamguGlobal() {

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
    spaces.clear();
    tracked.clean();

    for (i = 0; i < indexreservoire.size(); i++)
        delete indexreservoire[i];

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

    for (i = 0; i < declarationreservoire.size(); i++)
        delete declarationreservoire[i];

    for (i = 0; i < declarationcleanreservoire.size(); i++)
        delete declarationcleanreservoire[i];

    for (i = 0; i < pvireservoire.size(); i++)
        delete pvireservoire[i];

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

    codes.clear();
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
    strictcompatibilities.clear();
    hierarchy.clear();

    if (gAutomatons!=NULL)
        delete gAutomatons;

    //FinalTamguConstantCleaning();
    
    #ifdef GARBAGEFORDEBUG
    vector<Tamgu*> issues;
    vector<long> idissues;
    Garbaging(issues, idissues);
    garbage.clear();
    if (issues.size())
        cerr << "No fully cleaned:" << issues.size() << endl;
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
        for (auto& it : m)
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
    for (i = threadstack.size() - 1; i >= 0; i--) {
        stringstream var;
        vector<short> vars;
        stackline=threadstack[i];
        funcname = "";
        short nm = stackline->Name();

        if (nm > 0)
            funcname = Getsymbol(nm);


        stackline->Variables(vars);
        for (size_t j = 0; j < vars.size(); j++) {
            a = stackline->Declaration(vars[j]);

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

    for (i = stack.size() - 1; i >= 0; i--) {
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
    if (executionbreak)
        return aEND;

    if (errors[idthread])
        return errorraised[idthread];

    return aNULL;
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

    errorraised[idthread] = (TamguError*)err;
    errors[idthread] = true;
    return err;
}

Exporting Tamgu* TamguGlobal::Returnerror(string msgerr) {
    short idthread = GetThreadid();
    if (errors[idthread])
        return errorraised[idthread];

    errorraised[idthread] = new TamguError(msgerr);
    errors[idthread] = true;
    return errorraised[idthread];
}

Exporting Tamgu* TamguGlobal::Returnerror(string msgerr, short idthread) {
    if (errors[idthread])
        return errorraised[idthread];

    errorraised[idthread] = new TamguError(msgerr);
    errors[idthread] = true;
    return errorraised[idthread];
}

void TamguGlobal::Pushstack(Tamgu* a, short idthread) {
    if (a->isTracked())
        threads[idthread].currentinstruction = a;

    threads[idthread].stack.push_back(a);
    if (threads[idthread].Size() >= maxstack)
        Returnerror("Stack overflow", idthread);
}

Tamgu* TamguGlobal::GetTopFrame() {
    for (long i=threads[0].stack.size()-1; i>=0; i--) {
        if (threads[0].stack[i]->isFrame())
            return threads[0].stack[i];
    }
    return NULL;
}

Tamgu* ProcCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
Exporting void TamguGlobal::RecordCompatibilities() {
    bin_hash<Tamgu*>::iterator it;

    vector<short> numbers;
    vector<short> strings;
    vector<short> maps;
    vector<short> vectors;

    equto[a_plusequ] = a_plus;
    equto[a_minusequ] = a_minus;
    equto[a_multiplyequ] = a_multiply;
    equto[a_divideequ] = a_divide;
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
    compatibilities[a_predicate][a_predicatevar] = true;
    compatibilities[a_predicatevar][a_predicate] = true;

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
    strictcompatibilities[a_predicate][a_predicatevar] = true;
    strictcompatibilities[a_predicatevar][a_predicate] = true;

    maps.push_back(a_constmap);
    vectors.push_back(a_constvector);

    short ty;
    for (it = newInstance.begin(); it != newInstance.end(); it++) {
        if (it->second->isFrame()) //this type is only used to produce frames...
        continue;
        ty = it->second->Type();

        //we record each object as being its own procedure... These procedures (actually calling ProcCreate)
        //return an object of this type...
        RecordOneProcedure(Getsymbol(ty), ProcCreate, P_NONE | P_ONE);
        returntypes[ty] = ty;

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

        if (it->second->isVectorContainer()) {
            vectors.push_back(ty);
            continue;
        }
        if (it->second->isMapContainer()) {
            maps.push_back(ty);
            continue;
        }

        if (it->second->isNumber()) {
            numbers.push_back(ty);
            continue;
        }

        if (it->second->isString()) {
            strings.push_back(ty);
            continue;
        }
    }

    int i, j;
    for (i = 0; i < numbers.size(); i++) {
        for (j = 0; j < numbers.size(); j++) {
            compatibilities[numbers[i]][numbers[j]] = true;
            strictcompatibilities[numbers[i]][numbers[j]] = true;
        }
        for (j = 0; j < strings.size(); j++)
            compatibilities[numbers[i]][strings[j]] = true;
        compatibilities[numbers[i]][a_const] = true;
        strictcompatibilities[numbers[i]][a_const] = true;
    }

    for (i = 0; i < strings.size(); i++) {
        for (j = 0; j < strings.size(); j++) {
            compatibilities[strings[i]][strings[j]] = true;
            strictcompatibilities[strings[i]][strings[j]] = true;
        }
        for (j = 0; j < numbers.size(); j++)
            compatibilities[strings[i]][numbers[j]] = true;
        compatibilities[strings[i]][a_const] = true;
        strictcompatibilities[strings[i]][a_const] = true;
    }

    for (i = 0; i < vectors.size(); i++) {
        for (j = 0; j < vectors.size(); j++)
            compatibilities[vectors[i]][vectors[j]] = true;

        strictcompatibilities[a_vector][vectors[i]] = true;
        strictcompatibilities[vectors[i]][a_vector] = true;
        compatibilities[vectors[i]][a_const] = true;
        strictcompatibilities[vectors[i]][a_const] = true;
    }

    for (i = 0; i < maps.size(); i++) {
        for (j = 0; j < maps.size(); j++)
            compatibilities[maps[i]][maps[j]] = true;

        strictcompatibilities[a_map][maps[i]] = true;
        strictcompatibilities[a_primemap][maps[i]] = true;
        strictcompatibilities[a_treemap][maps[i]] = true;
        strictcompatibilities[a_binmap][maps[i]] = true;

        strictcompatibilities[maps[i]][a_map] = true;
        strictcompatibilities[maps[i]][a_primemap] = true;
        strictcompatibilities[maps[i]][a_treemap] = true;
        strictcompatibilities[maps[i]][a_binmap] = true;

        compatibilities[maps[i]][a_const] = true;
        strictcompatibilities[maps[i]][a_const] = true;
    }
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
}

long TamguGlobal::Getinstructionline(short idthread) {
    if (threads[idthread].currentinstruction == NULL)
        return lineerror;
    return threads[idthread].currentinstruction->Currentline();
}

//This function is hack which is necessary to get these variables a value in a DLL
Exporting void TamguGlobal::Update() {
    globalTamgu = this;

    if (aNULL == NULL) {
        aNULL = gNULL;
        aUNIVERSAL = gUNIVERSAL;
        aTRUE = gTRUE;
        aFALSE = gFALSE;
        aMINUSTEN = gMINUSTEN;
        aMINUSNINE = gMINUSNINE;
        aMINUSEIGHT = gMINUSEIGHT;
        aMINUSSEVEN = gMINUSSEVEN;
        aMINUSSIX = gMINUSSIX;
        aMINUSFIVE = gMINUSFIVE;
        aMINUSFOUR = gMINUSFOUR;
        aMINUSTHREE = gMINUSTHREE;
        aMINUSTWO = gMINUSTWO;
        aMINUSONE = gMINUSONE;
        aZERO = gZERO;
        aZEROPOINTZERO = gZEROPOINTZERO;
        aONE = gONE;
        aTWO = gTWO;
        aTHREE = gTHREE;
        aFOUR = gFOUR;
        aFIVE = gFIVE;
        aSIX = gSIX;
        aSEVEN = gSEVEN;
        aEIGHT = gEIGHT;
        aNINE = gNINE;
        aTEN = gTEN;
        aELEVEN  = gELEVEN;
        aTWELVE  = gTWELVE;
        aTHIRTEEN  = gTHIRTEEN;
        aFOURTEEN  = gFOURTEEN;
        aFIFTEEN  = gFIFTEEN;
        aSIXTEEN  = gSIXTEEN;
        aSEVENTEEN  = gSEVENTEEN;
        aEIGHTEEN  = gEIGHTEEN;
        aNINETEEN  = gNINETEEN;
        aTWENTY  = gTWENTY;
        aTHIRTYTWO  = gTHIRTYTWO;
        aSIXTYFOUR  = gSIXTYFOUR;



        aEMPTYSTRING = gEMPTYSTRING;
        aEMPTYUSTRING = gEMPTYUSTRING;
        aBREAK = gBREAK;
        aCONTINUE = gCONTINUE;
        aRETURN = gRETURN;
        aPIPE = gPIPE;
        aNOELEMENT = gNOELEMENT;
        aDEFAULT = gDEFAULT;
        aEND = gEND;
        aRAISEERROR = gRAISEERROR;
        aBREAKFALSE = gBREAKFALSE;
        aBREAKTRUE = gBREAKTRUE;
        aBREAKZERO = gBREAKZERO;
        aBREAKONE = gBREAKONE;
        aAFFECTATION = gAFFECTATION;
        aITERNULL = gITERNULL;
        aFAIL = gFAIL;
        aCUT = gCUT;
        aSTOP = gSTOP;
        aHASKELL = gHASKELL;
        aNULLDECLARATION = gNULLDECLARATION;
        aNULLLet = gNULLLet;
        aNOTHING = gNOTHING;

        booleantamgu[0] = aFALSE;
        booleantamgu[1] = aTRUE;
    }

    //We need to set the idtype back for tamgu internal objects
    if (!Tamguint::idtype) {
        RecordObjectsId();
        RecordContainersId();
    }
}


Exporting void TamguGlobal::RecordConstantNames() {
    string_operators["=="] = a_same;
    string_operators["<"] = a_less;
    string_operators[">"] = a_more;
    string_operators[":<"] = a_less;
    string_operators[":>"] = a_more;
    string_operators[">="] = a_moreequal;
    string_operators["<="] = a_lessequal;
    string_operators["="] = a_affectation;
    string_operators["is"] = a_affectation;
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
    operator_strings[a_affectation] = "=";
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

    if (!globalconstantes) {
		//They are created once for all...
        //Constants and ids
        aITERNULL = new TamguConstiteration(false, NULL);
        aPIPE = new TamguConstPipe(a_pipe, ":", NULL);

        aTRUE = new TamguConstBool(true, NULL);
        aFALSE = new TamguConstBool(false, NULL);

        aMINUSTEN = new TamguConstShort(-10, NULL);
        aMINUSNINE = new TamguConstShort(-9, NULL);
        aMINUSEIGHT = new TamguConstShort(-8, NULL);
        aMINUSSEVEN = new TamguConstShort(-7, NULL);
        aMINUSSIX = new TamguConstShort(-6, NULL);
        aMINUSFIVE = new TamguConstShort(-5, NULL);
        aMINUSFOUR = new TamguConstShort(-4, NULL);
        aMINUSTHREE = new TamguConstShort(-3, NULL);
        aMINUSTWO = new TamguConstShort(-2, NULL);
        aMINUSONE = new TamguConstShort(-1, NULL);
        aZERO = new TamguConstShort(0, NULL);
        aZEROPOINTZERO = new TamguConstFloat(0.0, NULL);
        aONE = new TamguConstShort(1, NULL);
        aTWO = new TamguConstShort(2, NULL);
        aTHREE = new TamguConstShort(3, NULL);
        aFOUR = new TamguConstShort(4, NULL);
        aFIVE = new TamguConstShort(5, NULL);
        aSIX = new TamguConstShort(6, NULL);
        aSEVEN = new TamguConstShort(7, NULL);
        aEIGHT = new TamguConstShort(8, NULL);
        aNINE = new TamguConstShort(9, NULL);
        aTEN = new TamguConstShort(10, NULL);
        aELEVEN  = new TamguConstShort(11 ,NULL);
        aTWELVE  = new TamguConstShort(12 ,NULL);
        aTHIRTEEN  = new TamguConstShort(13 ,NULL);
        aFOURTEEN  = new TamguConstShort(14 ,NULL);
        aFIFTEEN  = new TamguConstShort(15 ,NULL);
        aSIXTEEN  = new TamguConstShort(16 ,NULL);
        aSEVENTEEN  = new TamguConstShort(17 ,NULL);
        aEIGHTEEN  = new TamguConstShort(18 ,NULL);
        aNINETEEN  = new TamguConstShort(19 ,NULL);
        aTWENTY  = new TamguConstShort(20 ,NULL);
        aTHIRTYTWO  = new TamguConstShort(32 ,NULL);
        aSIXTYFOUR  = new TamguConstShort(64 ,NULL);

        //We protect these values, to avoid their storage in the tracker...
        aMINUSTEN->Protectfromtracker();
        aMINUSNINE->Protectfromtracker();
        aMINUSEIGHT->Protectfromtracker();
        aMINUSSEVEN->Protectfromtracker();
        aMINUSSIX->Protectfromtracker();
        aMINUSFIVE->Protectfromtracker();
        aMINUSFOUR->Protectfromtracker();
        aMINUSTHREE->Protectfromtracker();
        aMINUSTWO->Protectfromtracker();
        aMINUSONE->Protectfromtracker();
        aZERO->Protectfromtracker();
        aZEROPOINTZERO->Protectfromtracker();
        aONE->Protectfromtracker();
        aTWO->Protectfromtracker();
        aTHREE->Protectfromtracker();
        aFOUR->Protectfromtracker();
        aFIVE->Protectfromtracker();
        aSIX->Protectfromtracker();
        aSEVEN->Protectfromtracker();
        aEIGHT->Protectfromtracker();
        aNINE->Protectfromtracker();
        aTEN->Protectfromtracker();
        aELEVEN->Protectfromtracker();
        aTWELVE->Protectfromtracker();
        aTHIRTEEN->Protectfromtracker();
        aFOURTEEN->Protectfromtracker();
        aFIFTEEN->Protectfromtracker();
        aSIXTEEN->Protectfromtracker();
        aSEVENTEEN->Protectfromtracker();
        aEIGHTEEN->Protectfromtracker();
        aNINETEEN->Protectfromtracker();
        aTWENTY->Protectfromtracker();
        aTHIRTYTWO->Protectfromtracker();
        aSIXTYFOUR->Protectfromtracker();

        aEMPTYSTRING = new TamguConstString("", NULL);
        aEMPTYUSTRING = new TamguConstUString(L"", NULL);
        aEMPTYSTRING->Protectfromtracker();
        aEMPTYUSTRING->Protectfromtracker();

        aNULL = new TamguConst(Createid("null"), "null", NULL); //0 --> a_null
        //a hack to avoid this value to be stored in the tracker, when creating the corresponding system variable (see RecordSystemVariables() in codeexecute.cxx)
        aNULL->Protectfromtracker();
        aITERNULL->Update(aNULL); //Horrible hack to have aITERNULL being deleted before aNULL, but benefiting from aNULL still
    }
    else
        Createid("null"); //a_null

    booleantamgu[0] = aFALSE;
    booleantamgu[1] = aTRUE;

    Createid("true"); //1 --> a_true
    Createid("false"); //2 --> a_false

    Createid("zero"); //3 --> a_zero
    Createid("one"); //4 --> a_one

    Createid("bool");//5 --> a_boolean
    Createid("minusone"); //6 --> a_minusone

    Createid("byte"); //7 --> a_byte
    Createid("short"); //8 --> a_short
    Createid("int"); //9 --> a_int
    Createid("long"); //10 --> a_long
    Createid("decimal");//11 --> a_decimal
    Createid("fraction");//12 --> a_fraction
    Createid("float");//13 --> a_float

    Createid("bloop");//14 --> a_bloop
    Createid("iloop");//15 --> a_iloop
    Createid("lloop");//16 --> a_lloop
    Createid("dloop");//17 --> a_dloop
    Createid("floop");//18 --> a_floop

    Createid("ithrough"); //19 --> a_intthrough
    Createid("lthrough"); //20 --> a_longthrough
    Createid("dthrough"); //21 --> a_decimalthrough
    Createid("fthrough"); //22 --> a_floatthrough
    Createid("sthrough"); //23 --> a_stringthrough
    Createid("uthrough"); //24 --> a_ustringthrough
    Createid("vthrough"); //25 --> a_vectorthrough
    Createid("mthrough"); //26 --> a_mapthrough

    Createid("string");//27 --> a_string
    Createid("ustring");//28 --> a_ustring

    Createid("sloop"); //29 --> a_sloop
    Createid("uloop");//30 --> a_uloop


    Createid("constvector"); //31 --> a_constvector
    Createid("vector");//32 --> a_vector
    Createid("bvector");//33 --> a_bvector
    Createid("fvector");//34 --> a_fvector
    Createid("ivector");//35 --> a_ivector
    Createid("hvector");//36 --> a_hvector
    Createid("svector");//37 --> a_svector
    Createid("uvector");//38 --> a_uvector
    Createid("dvector");//39 --> a_dvector
    Createid("lvector");//40 --> a_lvector
    Createid("list"); //41 --> a_list

    Createid("constmap"); //42 --> a_constmap
    Createid("map");//43 --> a_map
    Createid("treemap"); //44 --> a_treemap
    Createid("primemap"); //45 --> a_primemap
    Createid("binmap"); //46 --> a_binmap
    Createid("mapss"); //47 --> a_mapss

    Createid("&error");//48 --> a_error

    if (!globalconstantes) {
        aEND = new TamguError(NULL, "END");
        aRAISEERROR = new TamguError(NULL, "ERROR");
    }

    Createid("const"); //49 --> a_const
    Createid("tam_none"); //50 --> a_none
    Createid(":"); //51 --> a_pipe

    if (!globalconstantes) {
        aBREAK = new TamguConstBreak(Createid("break"), "break", NULL); //52 --> a_break
        aCONTINUE = new TamguConstContinue(Createid("continue"), "continue", NULL); //53 --> a_continue
        aRETURN = new TamguConst(Createid("return"), "return",  NULL); //54 --> a_return
        aNOELEMENT = new TamguConst(Createid("empty"), "empty", NULL); //55 --> a_empty
        aNOELEMENT->Protectfromtracker(); //a hack to avoid this value to be stored in the tracker...
    }
    else {
        Createid("break");
        Createid("continue");
        Createid("return");
        Createid("empty");
    }

    Createid("_MAIN"); //56 --> a_mainframe

    Createid("call"); //57 --> a_call
    Createid("callfunction");//58 --> a_callfunction
    Createid("callthread");//59 --> a_callthread
    Createid("callmethod"); //60 --> a_callmethod
    Createid("callprocedure"); //61 --> a_callprocedure
    Createid("callindex"); //62 --> a_callindex
    Createid("calltaskell");//63 --> a_calltaskell
    Createid("lambda");//64 --> a_lambda

    Createid("variable"); //65 --> a_variable
    Createid("declarations"); //66 --> a_declaration
    Createid("instructions"); //67 --> a_instructions
    Createid("function"); //68 --> a_function
    Createid("frame"); //69 --> a_frame
    Createid("frameinstance"); //70 --> a_frameinstance
    Createid("extension"); //71 --> a_extension
    Createid("_initial"); //72 --> a_initial
    Createid("iterator"); //73 --> a_iteration

    if (!globalconstantes)
        aDEFAULT = new TamguConst(Createid("&default"), "&default", NULL); //74 --> a_default
    else
        Createid("&default");

    Createid("forinrange"); //75 --> a_forinrange
    Createid("sequence"); //76 --> a_sequence
    Createid("self"); //77 --> a_self
    Createid("&return;");//78 --> a_idreturnvariable
    Createid("&breaktrue"); //79 --> a_breaktrue
    Createid("&breakfalse"); //80 --> a_breakfalse

    if (!globalconstantes) {
        aBREAKTRUE = new TamguCallBreak(aTRUE, NULL);
        aBREAKFALSE = new TamguCallBreak(aFALSE, NULL);
        aBREAKONE = new TamguCallBreak(aONE, NULL);
        aBREAKZERO = new TamguCallBreak(aZERO, NULL);
    }

    Createid("constvectormerge"); //81 --> a_vectormerge
    Createid("instructionequ"); //82 --> a_instructionequ

    //let is used in Taskell expression, it corresponds to a self variable...
    Createid("let"); //83 --> a_let

    if (!globalconstantes)
        aAFFECTATION = new TamguConst(Createid("&assign"), "&assign", NULL); //84 --> a_assign
    else
        Createid("&assign");

    Createid("tamgu");//85 --> a_tamgu
    Createid("this");//86 --> a_this
    Createid("[]");//87 --> a_index
    Createid("[:]");//88 --> a_interval
    Createid("type");//89 --> a_type
    Createid("_final");//90 --> a_final
    Createid("&inifinitive"); //91 --> a_infinitive
    Createid("&cycle"); //92 --> a_cycle
    Createid("&replicate"); //93 --> a_replicate
    Createid("fail");//94 --> a_fail
    Createid("!"); //95 --> a_cut
    Createid("stop"); //96 --> a_stop
    Createid("&PREDICATEENTREE"); //97 --> a_predicateentree

    if (!globalconstantes) {
        aUNIVERSAL = new TamguConstUniversal(Createid("_"), "_", NULL); //98 --> a_universal
        aUNIVERSAL->Protectfromtracker(); //a hack to avoid this value to be stored in the tracker...
    }
    else
        Createid("_");

    Createid("asserta"); //99 --> a_asserta
    Createid("assertz"); //100 --> a_assertz
    Createid("retract"); //101 --> a_retract
    Createid("&remove&"); //102 --> a_remove
    Createid("predicatevar"); //103 --> a_predicatevar
    Createid("predicate"); //104 --> a_predicate
    Createid("term"); //105 --> a_term
    Createid("p_instance"); //106 --> a_instance
    Createid("p_predicateruleelement"); //107 --> a_predicateruleelement
    Createid("p_predicatecontainer"); //108 --> a_predicatecontainer
    Createid("p_predicaterule"); //109 --> a_predicaterule
    Createid("p_predicateinstruction"); //110 --> a_predicateinstruction
    Createid("p_knowledgebase"); //111 --> a_knowledgebase
    Createid("p_dependencybase"); //112 --> a_dependencybase
    Createid("p_predicatedomain"); //113 --> a_predicatedomain
    Createid("p_launch"); //114 --> a_predicatelaunch
    Createid("p_predicateelement"); //115 --> a_predicateelement
    Createid("p_parameterpredicate"); //116 --> a_parameterpredicate
    Createid("p_predicateevaluate"); //117 --> a_predicateevaluate
    Createid("dependency"); //118 --> a_dependency
    
    Createid("tam_stream"); //119 --> a_stream
    Createid("tam_affectation"); //120 --> a_affectation
   
    Createid("tam_plusequ"); //121 --> a_plusequ
    Createid("tam_minusequ"); //122 --> a_minusequ
    Createid("tam_multiplyequ"); //123 --> a_multiplyequ
    Createid("tam_divideequ"); //124 --> a_divideequ
    Createid("tam_modequ"); //125 --> a_modequ
    Createid("tam_powerequ"); //126 --> a_powerequ
    Createid("tam_shiftleftequ"); //127 --> a_shiftleftequ
    Createid("tam_shiftrightequ"); //128 --> a_shiftrightequ
    Createid("tam_orequ"); //129 --> a_orequ
    Createid("tam_xorequ"); //130 --> a_xorequ
    Createid("tam_andequ"); //131 --> a_andequ
    Createid("tam_mergeequ"); //132 --> a_mergeequ
    Createid("tam_combineequ"); //133 --> a_combineequ
    Createid("tam_addequ"); //134 --> a_addequ

    //Operators

    Createid("+"); //135 --> a_plus
    Createid("-"); //136 --> a_minus
    Createid("*"); //137 --> a_multiply
    Createid("/"); //138 --> a_divide
    Createid("^^"); //139 --> a_power
    Createid("<<"); //140 --> a_shiftleft
    Createid(">>"); //141 --> a_shiftright
    Createid("%"); //142 --> a_mod
    Createid("|"); //143 --> a_or
    Createid("^"); //144 --> a_xor
    Createid("&"); //145 --> a_and
    Createid("&&&"); //146 --> a_merge
    Createid("|||"); //147 --> a_combine
    Createid("::"); //148 --> a_add
    Createid("∧"); //149 --> a_conjunction
    Createid("∨"); //150 --> a_disjunction
    Createid("<"); //151
    Createid(">"); //152
    Createid("=="); //153
    Createid("!="); //154
    Createid("<="); //155
    Createid(">="); //156
    Createid("++"); //157
    Createid("--"); //158
    Createid("in"); //159
    Createid("notin"); //160


    Createid("tam_match"); //161
    Createid("tam_bloc"); //162
    Createid("tam_blocloopin"); //163
    Createid("tam_filein"); //164
    Createid("tam_blocboolean"); //165
    Createid("tam_parameter"); //166
    Createid("if"); //167
    Createid("tam_try"); //168
    Createid("tam_switch"); //169
    Createid("while"); //170
    Createid("for"); //171
    Createid("tam_catchbloc"); //172
    Createid("tam_booleanand"); //173
    Createid("tam_booleanor"); //174

    if (!globalconstantes)
        aHASKELL = new TamguConst(Createid("&taskell"), "&taskell", NULL); //175
    else
        Createid("&taskell");

    Createid("tam_forcedaffectation"); //176

    if (!globalconstantes) {
        aNULLDECLARATION = new TamguDeclaration(this);
        aNULLLet = new TamguLet(NULL);
    }

    Createid("²"); //177
    Createid("³"); //178
    Createid("&counter;"); //179

    Createid("synode"); //180
    //when we want to modify a dependency...
    Createid("&modify_dependency"); //181

    Createid("&action_var"); //182
    Createid("&taskelldeclaration;"); //183
    Createid("&drop;"); //184

    Createid("concept"); //185
    Createid("~"); //186
    Createid("taskellinstruction"); //187
    Createid("methods"); //188
    Createid("treg"); //189
    Createid("table"); //190
    Createid("ifnot"); //191

    if (!globalconstantes) {
        aNOTHING = new TamguConst(Createid("Nothing"), "Nothing", NULL); //192 --> a_Nothing
        aNOTHING->Protectfromtracker();
    }
    else
        Createid("Nothing");

    Createid("preg"); //193
    Createid("&a_rules;"); //194
    Createid("tam_iftaskell"); //195
    Createid("tam_casetaskell"); //196
    Createid("size"); //197
    Createid("post"); //198
    Createid("fibre"); //199
    Createid("tam_booleanxor"); //200
    Createid("push"); //201
    
    Createid("quote"); //202 a_quote
    Createid("cons"); //203 a_cons
    Createid("cond"); //204 a_cond
    Createid("atom"); //205 a_atom
    Createid("eq"); //206 a_eq
    Createid("cadr"); //207 a_cadr
    Createid("defun"); //208 a_defun
    Createid("label"); //209 a_label
    Createid("atomp"); //210 a_atomp
    Createid("numberp"); //211 a_numberp
    Createid("consp"); //212 a_consp
    Createid("zerop"); //213 a_zerop,
    Createid("nullp"); //214 a_nullp,
    Createid("block"); //215 a_block,
    Createid("setq"); //216 a_setq,
    Createid("append"); //217 a_append,
    Createid("lisp"); //218 a_lisp

    //This is a simple hack to handle "length" a typical Haskell operator as "size"...
    //Note that there will be a useless index

    Createid("length"); //219
    symbolIds["length"] = a_size;

    Createid("not"); //217
    symbolIds["not"] = a_negation;

    symbolIds["and"] = a_booleanand;
    symbolIds["or"] = a_booleanor;
    symbolIds["xor"] = a_booleanxor;
    
    dependenciesvariable[a_modifydependency] = a_modifydependency;


    atomics[a_short] = a_short;
    atomics[a_int] = a_int;
    atomics[a_long] = a_long;
    atomics[a_decimal] = a_decimal;
    atomics[a_float] = a_float;
    atomics[a_string] = a_string;
    atomics[a_ustring] = a_ustring;

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
    actions[a_mod] = new TamguAction(this, a_mod);
    actions[a_or] = new TamguAction(this, a_or);
    actions[a_xor] = new TamguAction(this, a_xor);
    actions[a_and] = new TamguAction(this, a_and);
    actions[a_merge] = new TamguAction(this, a_merge);
    actions[a_combine] = new TamguAction(this, a_combine);
    actions[a_add] = new TamguAction(this, a_add);

    globalconstantes=true;

    gNULL = aNULL;
    gUNIVERSAL = aUNIVERSAL;
    gTRUE = aTRUE;
    gFALSE = aFALSE;
    gMINUSTEN = aMINUSTEN;
    gMINUSNINE = aMINUSNINE;
    gMINUSEIGHT = aMINUSEIGHT;
    gMINUSSEVEN = aMINUSSEVEN;
    gMINUSSIX = aMINUSSIX;
    gMINUSFIVE = aMINUSFIVE;
    gMINUSFOUR = aMINUSFOUR;
    gMINUSTHREE = aMINUSTHREE;
    gMINUSTWO = aMINUSTWO;
    gMINUSONE = aMINUSONE;
    gZERO = aZERO;
    gZEROPOINTZERO = aZEROPOINTZERO;
    gONE = aONE;
    gTWO = aTWO;
    gTHREE = aTHREE;
    gFOUR = aFOUR;
    gFIVE = aFIVE;
    gSIX = aSIX;
    gSEVEN = aSEVEN;
    gEIGHT = aEIGHT;
    gNINE = aNINE;
    gTEN = aTEN;
    gELEVEN  = aELEVEN;
    gTWELVE  = aTWELVE;
    gTHIRTEEN  = aTHIRTEEN;
    gFOURTEEN  = aFOURTEEN;
    gFIFTEEN  = aFIFTEEN;
    gSIXTEEN  = aSIXTEEN;
    gSEVENTEEN  = aSEVENTEEN;
    gEIGHTEEN  = aEIGHTEEN;
    gNINETEEN  = aNINETEEN;
    gTWENTY  = aTWENTY;
    gTHIRTYTWO  = aTHIRTYTWO;
    gSIXTYFOUR  = aSIXTYFOUR;


    gEMPTYSTRING = aEMPTYSTRING;
    gEMPTYUSTRING = aEMPTYUSTRING;
    gBREAK = aBREAK;
    gCONTINUE = aCONTINUE;
    gRETURN = aRETURN;
    gPIPE = aPIPE;
    gNOELEMENT = aNOELEMENT;
    gDEFAULT = aDEFAULT;
    gEND = aEND;
    gRAISEERROR = aRAISEERROR;
    gBREAKFALSE = aBREAKFALSE;
    gBREAKTRUE = aBREAKTRUE;
    gBREAKZERO = aBREAKZERO;
    gBREAKONE = aBREAKONE;
    gAFFECTATION = aAFFECTATION;
    gITERNULL = aITERNULL;
    gFAIL = aFAIL;
    gCUT = aCUT;
    gSTOP = aSTOP;
    gHASKELL = aHASKELL;
    gNULLDECLARATION = aNULLDECLARATION;
    gNULLLet = aNULLLet;
    gNOTHING = aNOTHING;

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

    int id;
    threadids.get(_GETTHREADID(), id);
    return id;
}

short TamguGlobal::InitThreadid(short id) {
    threadids.set(threads[id].Initialization(),id);
    return id;
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

    if (codes.find(filename) != codes.end())
        return codes[filename];

    TamguCode* a = new TamguCode((short)spaces.size(), filename, this);
    spaces.push_back(a);
    codes[filename] = a;
    return a;
}

//----------------------------------------------------------------------------------
Exporting void TamguGlobal::RecordInTracker(Tamgu* a) {
    if (trackerslotfilled) {
        long idx = trackerslots.frontpop();
        --trackerslotfilled;
        a->Setid(tracked.put(a, idx));
    }
    else
        a->Setid(tracked.push_back(a));
}

Exporting long TamguGlobal::RecordInTrackerProtected(Tamgu* a) {
    if (a->idtracker != -1)
        return a->idtracker;

    if (globalLOCK) {
        long idx = -1;
        if (trackerslotfilled) {
            _trackerlock.Locking();
            if (trackerslotfilled) {
                idx = trackerslots.frontpop();
                --trackerslotfilled;
            }
            _trackerlock.Unlocking();
        }

        if (idx == -1)
            return a->Setidreturn(tracked.push_back(a));

        return a->Setidreturn(tracked.put(a, idx));
    }

    if (trackerslotfilled) {
        long idx = trackerslots.frontpop();
        --trackerslotfilled;

        return a->Setidreturn(tracked.put(a, idx));
    }

    return a->Setidreturn(tracked.push_back(a));
}

Exporting void TamguGlobal::ResetWithTracker(Tamgu* a, long id, long inc) {
    if (!tracked.check(id, a))
        return;
    a->Resetreference(inc);
}

//----------------------------------------------------------------------------------
bool TamguGlobal::Loadcontent(string content) {
    //We directly parse the content...
    static x_reading xr;

    xr.tokenize(content);
    TamguCode* a = GetNewCodeSpace("SCRIPT");
    return a->Load(xr);
}

TamguCode* TamguGlobal::Loadfile(string filename) {
    x_reading xr;
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

    long sp = spaceid;
    spaceid=spaces.size();
    TamguCode* a = GetNewCodeSpace(filename);

    xr.tokenize(code);
    a->Load(xr);
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


    vs.push_back("a_first");
    vs.push_back("a_change");
    vs.push_back("a_delete");
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
    vs.push_back("ifnot");
    vs.push_back("If");
    vs.push_back("IF");
    vs.push_back("in");
    vs.push_back("is");
    vs.push_back("iterator");
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
Exporting Tamgu* Tamgu::Succ() {
    long v = Integer();
    return globalTamgu->Provideint(v + 1);
}

Exporting Tamgu* Tamgu::Pred() {
    long v = Integer();
    return globalTamgu->Provideint(v - 1);
}

Exporting void Tamgu::addustringto(wstring ws) {
    Tamgu* a = globalTamgu->Provideustring(ws);
    Push(a);
    a->Release();
}

Exporting void Tamgu::addstringto(string s) {
    Tamgu* a = globalTamgu->Providestring(s);
    Push(a);
    a->Release();
}

Exporting void Tamgu::addstringto(wchar_t s) {
    wstring w;
    w = s;
    Tamgu* a = globalTamgu->Provideustring(w);
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

Exporting void Tamgu::storevalue(string u) {
    Tamgu* a = globalTamgu->Providestring(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(long u) {
    Tamgu* a = globalTamgu->Provideint(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    Push(a);
    a->Release();
}

Exporting void Tamgu::storevalue(BLONG u) {
    Tamgu* a = new Tamgulong(u);
    Push(a);
    a->Release();
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
    Tamgu* a = globalTamgu->Provideustring(w);
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
        value = globalTamgu->Providestring(u);
        value->Setreference(reference);
    }
}

void TamguLet::Storevalue(wstring& u) {
    if (value != aNOELEMENT)
        value->Storevalue(u);
    else {
        value = globalTamgu->Provideustring(u);
        value->Setreference(reference);
    }
}

void TamguLet::storevalue(string u) {
    if (value != aNOELEMENT)
        value->storevalue(u);
    else {
        value = globalTamgu->Providestring(u);
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
        value = globalTamgu->Provideustring(u);
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
        value = new Tamgulong(u);
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
        value = globalTamgu->Provideustring(w);
        value->Setreference(reference);
    }
}

//-------------------------------------------------------------------
void TamguSelf::Storevalue(string& u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_string)
        value->Storevalue(u);
    else {
        value = globalTamgu->Providestring(u);
        value->Setreference(reference);
        typevalue = a_string;
    }
    _cleanlock(_lock);
}

void TamguSelf::Storevalue(wstring& u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_ustring)
        ((Tamguustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Provideustring(u);
        value->Setreference(reference);
        typevalue = a_ustring;
    }
    _cleanlock(_lock);
}


void TamguSelf::storevalue(string u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_string)
        ((Tamgustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Providestring(u);
        value->Setreference(reference);
        typevalue = a_string;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(float u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_decimal)
        ((Tamgudecimal*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = new Tamgudecimal(u);
        value->Setreference(reference);
        typevalue = a_decimal;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(short u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_short)
        ((Tamgushort*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = new Tamgushort(u);
        value->Setreference(reference);
        typevalue = a_short;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(wstring u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_ustring)
        ((Tamguustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Provideustring(u);
        value->Setreference(reference);
        typevalue = a_ustring;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(long u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_int)
        ((Tamguint*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Provideint(u);
        value->Setreference(reference);
        typevalue = a_int;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(BLONG u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_long)
        ((Tamgulong*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = new Tamgulong(u);
        value->Setreference(reference);
        typevalue = a_long;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(double u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_float)
        ((Tamgufloat*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = globalTamgu->Providefloat(u);
        value->Setreference(reference);
        typevalue = a_float;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(unsigned char u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_byte)
        ((Tamgubyte*)value)->value = u;
    else {
        value->Resetreference(reference);
        value = new Tamgubyte(u);
        value->Setreference(reference);
        typevalue = a_byte;
    }
    _cleanlock(_lock);
}

void TamguSelf::storevalue(wchar_t u) {
    Locking* _lock = _getlock(this);
    if (typevalue == a_ustring)
        ((Tamguustring*)value)->value = u;
    else {
        value->Resetreference(reference);
        wstring w;
        w = u;
        value = globalTamgu->Provideustring(w);
        value->Setreference(reference);
        typevalue = a_ustring;
    }
    _cleanlock(_lock);
}
//---------------------------------------------------------------------------------------------
Exporting Tamgu* Tamgu::Push(BLONG k, Tamgu* v) {
    Tamgu* a = new Tamgulong(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(ushort k, Tamgu* v) {
    Tamgu* a = new Tamgushort(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(long k, Tamgu* v) {
    Tamgu* a = globalTamgu->Provideint(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(double k, Tamgu* v) {
    Tamgu* a = globalTamgu->Providefloat(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(string k, Tamgu* v) {
    Tamgu* a = globalTamgu->Providestring(k);
    v = Push(a, v);
    a->Release();
    return v;
}

Exporting Tamgu* Tamgu::Push(wstring k, Tamgu* v) {
    Tamgu* a = globalTamgu->Provideustring(k);
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
    Tamgustring v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstUString::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguustring v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstInt::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    Tamguint v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstShort::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    Tamgushort v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstDecimal::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    Tamgudecimal v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstFloat::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc)  {
    Tamgufloat v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

Exporting Tamgu* TamguConstLong::CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgulong v(value);
    contextualpattern = v.CallMethod(idname, contextualpattern, idthread, callfunc);
    if (contextualpattern == &v)
        return this;
    return contextualpattern;
}

//------------------------------------------------------------------------------------------------------------------------
Exporting Tamgumap* TamguGlobal::Providemap() {
    if (globalLOCK)
        return new Tamgumap;

    Tamgumapbuff* ke;

    if (mapempties.last > 0) {
        ke = mapreservoire[mapempties.backpop()];
        ke->used = true;
        return ke;
    }

    long mx = mapreservoire.size();

    while (mapidx < mx) {
        if (!mapreservoire[mapidx]->used) {
            mapreservoire[mapidx]->used = true;
            ke = mapreservoire[mapidx++];
            return ke;
        }
        mapidx++;
    }

    long sz = mx >> 2;
    mapreservoire.resize(mx + sz);
    mapidx = mx + sz;
    for (long i = mx; i < mapidx; i++)
        mapreservoire[i] = new Tamgumapbuff(i);

    mapidx = mx;
    mapreservoire[mapidx]->used = true;
    ke = mapreservoire[mapidx++];
    return ke;
}


Exporting void TamguIndexbuff::Rollback() {
    left->Release();
    if (right != NULL)
        right->Release();

    used = false;
    if (!globalTamgu->globalLOCK)
        globalTamgu->indexempties.push_back(idx);
}

Exporting TamguIndex* TamguGlobal::Provideindex(TamguIndex* init, short idthread) {
    if (globalLOCK)
        return new TamguIndexthread(init, idthread);

    TamguIndexbuff* ke;

    if (indexempties.last > 0) {
        ke = indexreservoire[indexempties.backpop()];
        ke->used = true;
        ke->Set(init, idthread);
        return ke;
    }

    long mx = indexreservoire.size();

    while (indexidx < mx) {
        if (!indexreservoire[indexidx]->used) {
            indexreservoire[indexidx]->used = true;
            indexreservoire[indexidx]->Set(init, idthread);
            ke = indexreservoire[indexidx++];
            return ke;
        }
        indexidx++;
    }

    long sz = mx >> 2;
    indexreservoire.resize(mx + sz);
    indexidx = mx + sz;
    for (long i = mx; i < indexidx; i++)
        indexreservoire[i] = new TamguIndexbuff(i);

    indexidx = mx;
    indexreservoire[indexidx]->used = true;
    indexreservoire[indexidx]->Set(init, idthread);
    ke = indexreservoire[indexidx++];
    return ke;
}

Exporting Tamgumapss* TamguGlobal::Providemapss() {
    if (globalLOCK)
        return new Tamgumapss;

    Tamgumapssbuff* ke;

    if (mapssempties.last > 0) {
        ke = mapssreservoire[mapssempties.backpop()];
        ke->used = true;
        return ke;
    }

    long mx = mapssreservoire.size();

    while (mapssidx < mx) {
        if (!mapssreservoire[mapssidx]->used) {
            mapssreservoire[mapssidx]->used = true;
            ke = mapssreservoire[mapssidx++];
            return ke;
        }
        mapssidx++;
    }

    long sz = mx >> 2;
    mapssreservoire.resize(mx + sz);
    mapssidx = mx + sz;
    for (long i = mx; i < mapssidx; i++)
        mapssreservoire[i] = new Tamgumapssbuff(i);

    mapssidx = mx;
    mapssreservoire[mapssidx]->used = true;
    ke = mapssreservoire[mapssidx++];
    return ke;
}

Exporting Tamguvector* TamguGlobal::Providevector() {
    if (globalLOCK)
        return new Tamguvector;

    Tamguvectorbuff* ke;

    if (vectorempties.last > 0) {
        ke = vectorreservoire[vectorempties.backpop()];
        ke->used = true;
        return ke;
    }

    long mx = vectorreservoire.size();

    while (vectoridx < mx) {
        if (!vectorreservoire[vectoridx]->used) {
            vectorreservoire[vectoridx]->used = true;
            ke = vectorreservoire[vectoridx++];
            return ke;
        }
        vectoridx++;
    }

    long sz = mx >> 2;
    vectorreservoire.resize(mx + sz);
    vectoridx = mx + sz;
    for (long i = mx; i < vectoridx; i++)
        vectorreservoire[i] = new Tamguvectorbuff(i);

    vectoridx = mx;
    vectorreservoire[vectoridx]->used = true;
    ke = vectorreservoire[vectoridx++];
    return ke;
}


Exporting Tamguivector* TamguGlobal::Provideivector() {
    if (globalLOCK)
        return new Tamguivector;

    Tamguivectorbuff* ke;

    if (ivectorempties.last > 0) {
        ke = ivectorreservoire[ivectorempties.backpop()];
        ke->used = true;
        return ke;
    }

    long mx = ivectorreservoire.size();

    while (ivectoridx < mx) {
        if (!ivectorreservoire[ivectoridx]->used) {
            ivectorreservoire[ivectoridx]->used = true;
            ke = ivectorreservoire[ivectoridx++];
            return ke;
        }
        ivectoridx++;
    }

    long sz = mx >> 2;
    ivectorreservoire.resize(mx + sz);
    ivectoridx = mx + sz;
    for (long i = mx; i < ivectoridx; i++)
        ivectorreservoire[i] = new Tamguivectorbuff(i);

    ivectoridx = mx;
    ivectorreservoire[ivectoridx]->used = true;
    ke = ivectorreservoire[ivectoridx++];
    return ke;
}

Exporting Tamgufvector* TamguGlobal::Providefvector() {
    if (globalLOCK)
        return new Tamgufvector;

    Tamgufvectorbuff* ke;
    if (fvectorempties.last > 0) {
        ke = fvectorreservoire[fvectorempties.backpop()];
        ke->used = true;
        return ke;
    }

    long mx = fvectorreservoire.size();

    while (fvectoridx < mx) {
        if (!fvectorreservoire[fvectoridx]->used) {
            fvectorreservoire[fvectoridx]->used = true;
            ke = fvectorreservoire[fvectoridx++];
            return ke;
        }
        fvectoridx++;
    }

    long sz = mx >> 2;
    fvectorreservoire.resize(mx + sz);
    fvectoridx = mx + sz;
    for (long i = mx; i < fvectoridx; i++)
        fvectorreservoire[i] = new Tamgufvectorbuff(i);

    fvectoridx = mx;
    fvectorreservoire[fvectoridx]->used = true;
    ke = fvectorreservoire[fvectoridx++];
    return ke;
}

Exporting Tamgusvector* TamguGlobal::Providesvector() {
    if (globalLOCK)
        return new Tamgusvector;

    Tamgusvectorbuff* ke;
    if (svectorempties.last > 0) {
        ke = svectorreservoire[svectorempties.backpop()];
        ke->used = true;
        return ke;
    }

    long mx = svectorreservoire.size();

    while (svectoridx < mx) {
        if (!svectorreservoire[svectoridx]->used) {
            svectorreservoire[svectoridx]->used = true;
            ke = svectorreservoire[svectoridx++];
            return ke;
        }
        svectoridx++;
    }

    long sz = mx >> 2;
    svectorreservoire.resize(mx + sz);
    svectoridx = mx + sz;
    for (long i = mx; i < svectoridx; i++)
        svectorreservoire[i] = new Tamgusvectorbuff(i);

    svectoridx = mx;
    svectorreservoire[svectoridx]->used = true;
    ke = svectorreservoire[svectoridx++];
    return ke;
}

Exporting Tamguuvector* TamguGlobal::Provideuvector() {
    if (globalLOCK)
        return new Tamguuvector;

    Tamguuvectorbuff* ke;
    if (uvectorempties.last > 0) {
        ke = uvectorreservoire[uvectorempties.backpop()];
        ke->used = true;
        return ke;
    }

    long mx = uvectorreservoire.size();

    while (uvectoridx < mx) {
        if (!uvectorreservoire[uvectoridx]->used) {
            uvectorreservoire[uvectoridx]->used = true;
            ke = uvectorreservoire[uvectoridx++];
            return ke;
        }
        uvectoridx++;
    }

    long sz = mx >> 2;
    uvectorreservoire.resize(mx + sz);
    uvectoridx = mx + sz;
    for (long i = mx; i < uvectoridx; i++)
        uvectorreservoire[i] = new Tamguuvectorbuff(i);

    uvectoridx = mx;
    uvectorreservoire[uvectoridx]->used = true;
    ke = uvectorreservoire[uvectoridx++];
    return ke;
}

Exporting TamguSelf* TamguGlobal::Provideself() {
    if (globalLOCK)
        return new TamguSelf;

    Tamguselfbuff* ke;

    if (slfempties.last > 0) {
        ke = slfreservoire[slfempties.backpop()];
        ke->used = true;
        ke->value = aNOELEMENT;
        ke->typevalue = a_const;
        return ke;
    }

    long mx = slfreservoire.size();

    while (slfidx < mx) {
        if (!slfreservoire[slfidx]->used) {
            ke = slfreservoire[slfidx++];
            ke->used = true;
            ke->value = aNOELEMENT;
            ke->typevalue = a_const;
            return ke;
        }
        slfidx++;
    }

    long sz = mx >> 2;
    slfreservoire.resize(mx + sz);
    slfidx = mx + sz;
    for (long i = mx; i < slfidx; i++)
        slfreservoire[i] = new Tamguselfbuff(i);

    slfidx = mx;
    slfreservoire[slfidx]->used = true;
    ke = slfreservoire[slfidx++];
    return ke;
}

Exporting Tamguint* TamguGlobal::Provideint(long v) {
    if (globalLOCK)
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
        intreservoire[i] = new Tamguintbuff(i);

    intidx = mx;
    intreservoire[intidx]->used = true;
    intreservoire[intidx]->value = v;
    ke = intreservoire[intidx++];
    return ke;
}

Exporting Tamgufloat* TamguGlobal::Providefloat(double v) {
    if (globalLOCK)
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
        floatreservoire[i] = new Tamgufloatbuff(i);

    floatidx = mx;
    floatreservoire[floatidx]->used = true;
    floatreservoire[floatidx]->value = v;
    ke = floatreservoire[floatidx++];
    return ke;
}

Exporting TamguDeclarationLocal* TamguGlobal::Providedeclaration(Tamgu* ins, short idt, bool p) {
    TamguDeclarationLocal* ke;

    if (globalLOCK) {
        ke = new TamguDeclarationLocal(-1);
        ke->idthread = idt;

        if (p) {
            Pushstack(ke, idt);
            ke->pushed = true;
        }

        if (debugmode && ins->isTracked()) {
            ke->idinfo = ins->Currentinfo();
        }

        return ke;
    }

    if (declempties.size()) {

        ke = declarationreservoire[declempties.frontpop()];
        ke->used = true;
        ke->idthread = idt;

        if (p) {
            Pushstack(ke, idt);
            ke->pushed = true;
        }

        if (debugmode && ins->isTracked()) {
            ke->idinfo = ins->Currentinfo();
        }

        return ke;
    }

    long mx = declarationreservoire.size();

    while (declarationidx < mx) {
        if (!declarationreservoire[declarationidx]->used) {

            ke = declarationreservoire[declarationidx++];
            ke->used = true;
            ke->idthread = idt;

            if (p) {
                Pushstack(ke, idt);
                ke->pushed = true;
            }

            if (debugmode && ins->isTracked()) {
                ke->idinfo = ins->Currentinfo();
            }
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
    if (p) {
        Pushstack(ke, idt);
        ke->pushed = true;
    }

    if (debugmode && ins->isTracked()) {
        ke->idinfo = ins->Currentinfo();
    }
    return ke;
}

Exporting Tamgustring* TamguGlobal::Providestring(string v) {
    if (globalLOCK)
        return new Tamgustring(v);

    Tamgustringbuff* ke;
    if (sempties.last > 0) {
        ke = stringreservoire[sempties.backpop()];
        ke->used = true;
        ke->value = v;
        return ke;
    }

    long mx = stringreservoire.size();

    while (stringidx < mx) {
        if (!stringreservoire[stringidx]->used) {
            stringreservoire[stringidx]->used = true;
            stringreservoire[stringidx]->value = v;
            ke = stringreservoire[stringidx++];
            return ke;
        }
        stringidx++;
    }

    long sz = mx >> 2;
    stringreservoire.resize(mx + sz);
    stringidx = mx + sz;
    for (long i = mx; i < stringidx; i++)
        stringreservoire[i] = new Tamgustringbuff(i);

    stringidx = mx;
    stringreservoire[stringidx]->used = true;
    stringreservoire[stringidx]->value = v;
    ke = stringreservoire[stringidx++];
    return ke;
}

Exporting Tamguustring* TamguGlobal::Provideustring(wstring v) {
    if (globalLOCK)
        return new Tamguustring(v);

    Tamguustringbuff* ke;
    if (uempties.last > 0) {
        ke = ustringreservoire[uempties.backpop()];
        ke->used = true;
        ke->value = v;
        return ke;
    }

    long mx = ustringreservoire.size();

    while (ustringidx < mx) {
        if (!ustringreservoire[ustringidx]->used) {
            ustringreservoire[ustringidx]->value = v;
            ustringreservoire[ustringidx]->used = true;
            ke = ustringreservoire[ustringidx++];
            return ke;
        }
        ustringidx++;
    }

    long sz = mx >> 2;
    ustringreservoire.resize(mx + sz);
    ustringidx = mx + sz;
    for (long i = mx; i < ustringidx; i++)
        ustringreservoire[i] = new Tamguustringbuff(i);

    ustringidx = mx;
    ustringreservoire[ustringidx]->used = true;
    ustringreservoire[ustringidx]->value = v;
    ke = ustringreservoire[ustringidx++];
    return ke;
}


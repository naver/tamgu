/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguglobal.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguglobal_h
#define tamguglobal_h

//--------------------------------------------------------------------
//TamguGlobal is one unique object in which the threads and other ids are stored...
//--------------------------------------------------------------------
#include <thread>
#include <mutex>
#include <condition_variable>
#include <set>
#include <chrono>
#include "vecte.h"

#include "x_node.h"
#include "tokens.h"

#define threadhandle std::thread::id
#define THREADPTR std::thread*
#define _GETTHREADID() std::this_thread::get_id()

#include "atomicmap.h"

//--------------------------------------------------------------------
class TamguError;
class TamguLet;
class Tamgustring;
class TamguPredicateVariable;
class TamguPredicateFunction;
class TamguPredicate;
class TamguPredicateRule;
class TamguPredicate;
class TamguPredicateContainer;
class TamguCallBreak;
class TamguObject;
class Tamgulongbuff;
class Tamgulong;
class Tamguintbuff;
class Tamguint;
class Tamgufloatbuff;
class Tamgufloat;
class Tamgustringbuff;
class Tamgustring;
class Tamguustringbuff;
class Tamguustring;
class Tamguvector;
class Tamguvectorbuff;
class Tamguivector;
class Tamguivectorbuff;
class Tamgufvector;
class Tamgufvectorbuff;
class Tamgusvector;
class Tamgusvectorbuff;
class Tamguuvector;
class Tamguuvectorbuff;
class Tamgumap;
class Tamgutreemap;
class Tamguprimemap;
class Tamguprimemapbuff;
class Tamgutreemapbuff;
class Tamgumapbuff;
class Tamgumapss;
class Tamgumapssbuff;
class TamguIndex;
class bnf_tamgu;
class An_rules;
class Au_automatons;
class Au_automaton;
class TamguDeclarationLocal;
class TamguDeclarationAutoClean;
class TamguActionVariable;
class TamguSelf;
class Tamguselfbuff;
class FstCompanion;
class TamguDoubleSideAutomaton;
class TamguCallFibre;
class TamguPredicateVariableInstance;
class TamguPredicateVariableInstancebuff;
class Tamgulisp;
class UTF8_Handler;
class TamguConstString;
class TamguThreadCall;

//--------------------------------------------------------------------
typedef bool(*TamguExternalModule)(TamguGlobal*, string);
//--------------------------------------------------------------------
typedef double(*mathFunc)(double);
//--------------------------------------------------------------------
class GlobalConstants {
public:
    
    Tamgu* gNULL;
    Tamgu* gUNIVERSAL;
    Tamgu* gTRUE;
    Tamgu* gFALSE;
    
    Tamgu* gMINUSONE;
    Tamgu* gZEROPOINTZERO;
    Tamgu* gZERO;
    Tamgu* gONE;

    Tamgu* gEMPTYLISP;
    Tamgu* gEMPTYSTRING;
    Tamgu* gEMPTYUSTRING;
    Tamgu* gBREAK;
    Tamgu* gCONTINUE;
    Tamgu* gRETURN;
    Tamgu* gPIPE;
    Tamgu* gNOELEMENT;
    Tamgu* gDEFAULT;
    Tamgu* gEND;
    Tamgu* gRAISEERROR;
    TamguCallBreak* gBREAKFALSE;
    TamguCallBreak* gBREAKTRUE;
    TamguCallBreak* gBREAKZERO;
    TamguCallBreak* gBREAKONE;

    Tamgu* gASSIGNMENT;
    TamguConstiteration* gITERNULL;
    TamguPredicate* gFAIL;
    TamguPredicate* gTERMINAL;
    TamguPredicate* gCUTFALSE;
    TamguPredicate* gCUT;
    TamguPredicate* gSTOP;
    Tamgu* gHASKELL;
    TamguDeclaration* gNULLDECLARATION;
    TamguLet* gNULLLet;
    Tamgu* gNOTHING;
    
    GlobalConstants() {
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
        gTERMINAL= NULL;
        gCUTFALSE= NULL;
        gCUT= NULL;
        gSTOP= NULL;
        gHASKELL= NULL;
        gNULLDECLARATION= NULL;
        gNULLLet= NULL;
        gNOTHING= NULL;
        aNULL = NULL;
    }
    
    void set(GlobalConstants* g) {
        gNULL = g->gNULL;
        aNULL = gNULL;
        gUNIVERSAL = g->gUNIVERSAL;
        gTRUE = g->gTRUE;
        gFALSE = g->gFALSE;
        gMINUSONE = g->gMINUSONE;
        gZEROPOINTZERO = g->gZEROPOINTZERO;
        gZERO = g->gZERO;
        gONE = g->gONE;
        gEMPTYLISP = g->gEMPTYLISP;
        gEMPTYSTRING = g->gEMPTYSTRING;
        gEMPTYUSTRING = g->gEMPTYUSTRING;
        gBREAK = g->gBREAK;
        gCONTINUE = g->gCONTINUE;
        gRETURN = g->gRETURN;
        gPIPE = g->gPIPE;
        gNOELEMENT = g->gNOELEMENT;
        gDEFAULT = g->gDEFAULT;
        gEND = g->gEND;
        gRAISEERROR = g->gRAISEERROR;
        gBREAKFALSE = g->gBREAKFALSE;
        gBREAKTRUE = g->gBREAKTRUE;
        gBREAKZERO = g->gBREAKZERO;
        gBREAKONE = g->gBREAKONE;
        gASSIGNMENT = g->gASSIGNMENT;
        gITERNULL = g->gITERNULL;
        gFAIL = g->gFAIL;
        gTERMINAL = g->gTERMINAL;
        gCUTFALSE = g->gCUTFALSE;
        gCUT = g->gCUT;
        gSTOP = g->gSTOP;
        gHASKELL = g->gHASKELL;
        gNULLDECLARATION = g->gNULLDECLARATION;
        gNULLLet = g->gNULLLet;
        gNOTHING = g->gNOTHING;
    }
    void clean();
};

//--------------------------------------------------------------------
extern GlobalConstants globalConstants;

#define aUNIVERSAL globalConstants.gUNIVERSAL
#define aUNIVERSAL globalConstants.gUNIVERSAL
#define aTRUE globalConstants.gTRUE
#define aFALSE globalConstants.gFALSE
#define aMINUSONE globalConstants.gMINUSONE
#define aZEROPOINTZERO globalConstants.gZEROPOINTZERO
#define aZERO globalConstants.gZERO
#define aONE globalConstants.gONE
#define aEMPTYLISP globalConstants.gEMPTYLISP
#define aEMPTYSTRING globalConstants.gEMPTYSTRING
#define aEMPTYUSTRING globalConstants.gEMPTYUSTRING
#define aBREAK globalConstants.gBREAK
#define aCONTINUE globalConstants.gCONTINUE
#define aRETURN globalConstants.gRETURN
#define aPIPE globalConstants.gPIPE
#define aNOELEMENT globalConstants.gNOELEMENT
#define aDEFAULT globalConstants.gDEFAULT
#define aEND globalConstants.gEND
#define aRAISEERROR globalConstants.gRAISEERROR
#define aBREAKFALSE globalConstants.gBREAKFALSE
#define aBREAKTRUE globalConstants.gBREAKTRUE
#define aBREAKZERO globalConstants.gBREAKZERO
#define aBREAKONE globalConstants.gBREAKONE
#define aASSIGNMENT globalConstants.gASSIGNMENT
#define aITERNULL globalConstants.gITERNULL
#define aFAIL globalConstants.gFAIL
#define aTERMINAL globalConstants.gTERMINAL
#define aCUTFALSE globalConstants.gCUTFALSE
#define aCUT globalConstants.gCUT
#define aSTOP globalConstants.gSTOP
#define aHASKELL globalConstants.gHASKELL
#define aNULLDECLARATION globalConstants.gNULLDECLARATION
#define aNULLLet globalConstants.gNULLLet
#define aNOTHING globalConstants.gNOTHING
//--------------------------------------------------------------------

class ThreadLock {
public:
	static Exchanging long ids;

	std::recursive_mutex* lock;

	long id;

    inline ThreadLock() {
        id = ids++;
        lock = new std::recursive_mutex;
    }
    
    inline void Locking() {
        lock->lock();
    }
    
    inline void Unlocking() {
        lock->unlock();
    }
    
    ~ThreadLock() {
        delete lock;
    }
    
};

class Tamgulonglist : public std::list<long> {
public:

    long frontpop() {
        long i = front();
        pop_front();
        return i;
    }
};

class LockedThread {
public:
	std::condition_variable lock;
	std::mutex mtx;
	char released;

	LockedThread() {
		released = 0; 
	}

	Exporting ~LockedThread();
	Exporting void Blocked();
	Exporting void Released();
};


//The definition of parsing function...
typedef Tamgu* (TamguCode::*ParseElement)(x_node*, Tamgu*);

class ThreadStruct {
public:
	VECTE<Tamgu*> stack;
    VECTE<Tamgu*> stacklisp;
    VECTE<Tamgu*> stackinstructions;
    VECTE<short> basepredicatename;
    VECTE<TamguCallFibre*> fibres;
    vector<Tamgu*> debugstack;
    vector<Tamgu*> localgarbage;
	bin_hash<VECTE<Tamgu*> > variables;
    hmap<string, ThreadLock*> locks;

	string nonblockingerror;

	stringstream message;
	threadhandle handle;
    
    std::atomic<short> nbjoined;

    FstCompanion* fstcompanion;
	Tamgu* returnvalue;
    Tamgu* previousinstruction;
    Tamgu* currentinstruction;

    long embedded_try;
    long parentthreads;

    short idthread;
    short gpredicatename;
	short prologstack;

	bool used;
    bool in_eval;

	ThreadStruct();
	Exporting ~ThreadStruct();

    FstCompanion* Companion();
    
    
    void Pushinstruction(Tamgu* a) {
        currentinstruction = a;
        stackinstructions.push_back(a);
    }
    
    void Popinstruction() {
        stackinstructions.pop_back();
    }
    
    void SetPredicateVariableFlags(short base) {
        gpredicatename = base;
        basepredicatename.clear();
    }

    void Returnname(short name) {
        basepredicatename.push_back(name);
    }
    
    short GetName() {
        if (basepredicatename.last)
            return basepredicatename.backpop();
        gpredicatename++;
        return gpredicatename;
    }

    inline long Size() {
		return (prologstack + stack.size() + stacklisp.size());
	}
    
    bool pushtracked(Tamgu* a, long mx);
    
    inline bool push(Tamgu* a, long mx) {
        stack.push_back(a);
        return (stack.size() >= mx);
    }
    
	void Update(short idthread);

    inline void Storeingarbage(Tamgu* a) {
        if (in_eval)
            localgarbage.push_back(a);
    }
    
    inline void Cleaningarbage(Tamgu* a) {
        if (in_eval) {
            for (long i = 0; i < localgarbage.size(); i++) {
                if (localgarbage[i] ==  a) {
                    localgarbage[i] = NULL;
                    return;
                }
            }
        }
    }
    
    inline long Garbagesize() {
        in_eval = true;
        return localgarbage.size();
    }
    
    void Cleanfromgarbageposition(Tamgu* declaration, short idthread, long p, Tamgu* keep, long lastrecorded, long maxrecorded);
	
    inline void Removevariable(short n) {
		if (variables.check(n)) {
			VECTE<Tamgu*>& v = variables.get(n);
			if (v.last)
				v.pop_back();
		}
	}

	inline void Replacevariable(short name, Tamgu* var) {
		if (variables.check(name)) {
			VECTE<Tamgu*>& v = variables.get(name);
			if (v.last) {
				v.vecteur[v.last - 1] = var;
                return;
			}
		}

		variables[name].push_back(var);
	}

	void Clear();
	threadhandle Initialization();

	Tamgu* Getdefinition(short id);
    Tamgu* Declaration(short id);
	Tamgu* Declarator(short id);
	bool isDeclared(short id);
	Tamgu* GetTopFrame();

};

//-----------------------------------------------------------------------------------
//------------- MAIN GLOBAL OBJECT --------------------------------------------------
//-----------------------------------------------------------------------------------
class TamguGlobal;
class TamguSystemVariable;
typedef void(*DisplayFunctionCall)(string s, void* object);
typedef Tamgu* (*TamguProcedure)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
typedef Tamgu* (*TamguDebug)(vector<Tamgu*>& stack, short idthread, void* data);
typedef void (*TamguTermination)(TamguGlobal* global);

class Tamguinfo {
public:
    
    long line;
    short idcode;
    short idfile;
    
    Tamguinfo() {
        idcode = -1;
        idfile = -1;
        line = -1;
    }
    
    Tamguinfo(short c, short f, long l) {
        idcode = c;
        idfile = f;
        line = l;
    }
    
    bool compare(short c, short f, long l) {
        return (c == idcode && f == idfile && l == line);
    }
};

class TamguGlobal {
private:

	//The specific objects which we want to track (not exactly a garbage)
    
	long maxthreads;
    
public:

    atomic_vector<Tamgu*> tracked;
    std::set<Tamgu*> elements_to_delete;

    short idglobal;
	long maxrange;

	//All programmes stored in memory
	vector<TamguCode*> spaces;
	hmap<string, TamguCode*> pathnames;
    hmap<string, TamguCode*> codes;

    std::chrono::system_clock::time_point last_execution;
    
	ThreadStruct* threads;
    bool* errors;
    TamguError** errorraised;
    UTF8_Handler* handler_on_utf8;

    atomic_map<threadhandle, int> threadids;
    hmap<short, hmap<string, string> > infomethods;

    hmap<short, vector<TamguPredicate*> > knowledgebase;
    hmap<string,vector<TamguPredicate*> > knowledgebase_on_first;
    hmap<string,vector<TamguPredicate*> > knowledgebase_on_second;
    hmap<string,vector<TamguPredicate*> > knowledgebase_on_third;
    
    std::vector<std::string> codelines;
    bool store_in_code_lines;
    
    VECTE<Tamgu*> stack_error;
    std::vector<TamguThreadCall*> thread_pool;

    tokenizer_automaton tamgu_tokenizer;

	//This variable is set to TRUE in thread mode... It allows then for the actual creation of locks...
    bool threadMODE;
    bool isthreading;
    
    std::atomic<bool> waitingonfalse;
    std::atomic<bool> running;
    std::atomic<short> threadcounter;

	atomic_map<string, ThreadLock*> locktables;
	atomic_map<string, LockedThread*> waitstrings;
	atomic_map<long, LockedThread*> threadvariables;

	void Releasevariables();
	void Triggeronfalse(Tamgu* a);

	bool erroronkey;
    bool loosecompability;
    short short_string;

	//-----------------------------------
	vector<string> filenames;
	//-----------------------------------
	hmap<short, mathFunc> mathFunctions;
	//-----------------------------------
	hmap<string, bool> systemfunctions;
	//-----------------------------------
	basebin_hash<short> throughs;
    //-----------------------------------
    basebin_hash<short> atomics;
    //-----------------------------------
    basebin_hash<bool> minimal_indexes;
    //-----------------------------------
	basebin_hash<Tamgu*> actions;
	//-----------------------------------
	basebin_hash<Tamgu*> concepts;
	basebin_hash<Tamgu*> roles;
	basebin_hash<Tamgu*> properties;
	basebin_hash<basebin_hash<bool> > hierarchy;
	//-----------------------------------

	TamguFunction* conceptfunction;
	TamguFunction* rolefunction;
	TamguFunction* propertyfunction;

	//Displaying stuff on screen or into a variable...


	Tamgustring* stringbuffer;
	Tamgustring* stringbuffererror;
    
    Tamgu* event_variable;
    
	ostream* os;
	bool doubledisplay;
	DisplayFunctionCall displayfunction;
	void* displayobject;
	Tamgu* kstd;
	Tamgu* kerr;

	//-------------------------------
	long linereference;
	long lineerror;
	bnf_tamgu* currentbnf;
    
    //infocode is used to record all information about a given instruction: its space, its file and its line in the file
    //we then return an idinfo, which is stored in a TamguTracked derived object.
    //Since, some instructions may share the same information, we only record them once...
    vector<Tamguinfo> infocode;
    
    long Addinfo(short c, short f, long l) {
        long ps = infocode.size();
        if (!ps || !infocode.back().compare(c,f,l)) {
            Tamguinfo i(c,f,l);
            infocode.push_back(i);
            return ps;
        }
        
        return ps-1;
    }

	//-------------------------------
	TamguDebug debugfunction;
	void* debugdata;

	TamguTermination terminationfunction;
	//-------------------------------
	long spaceid;
	bool debugmode;
	bool windowmode;
	
	//We convert all our strings into short id...
	bin_hash<string> idSymbols;
	
    hmap<string, short> symbolIds;
    bin_hash<unsigned long> lispactions;
    bin_hash<Tamgu*> lispsymbols;
    bin_hash<Tamgu*> lispoperators;
    
    Tamgu* Providelispsymbols(string& n, Tamgu* parent = NULL);
    Tamgu* Providelispsymbols(short n);
    Tamgu* Providelispsymbols(short n, short a);
    Tamgu* Providelispoperators(short n);
    
    Tamgu* Provideinstance(short type, short idthread);
    Tamgu* Provideinstance(string type, short idthread);
    Tamgu* Provideinstance(Tamgu* p, long i);
    
    short Typeinstance(string s);
    
    void set_full_compatibilities(void);
    void set_loose_compatibilities(void);
    
    hmap<string, short> string_operators;
	bin_hash<string> operator_strings;
	bin_hash<bool> atanOperatorMath;
	bin_hash<short> equto;

	bin_hash<short> returntypes;
    bin_hash<short> returnindextypes;

    bool checkoperator(short a) {
        return operator_strings.check(a);
    }

    //--------------------------------
	//Constant
	Exporting Tamgu* ProvideConstint(long v);
	Exporting Tamgu* ProvideConstfloat(double v);
	Exporting Tamgu* ProvideConstlong(BLONG v);
    Exporting Tamgu* ProvideConstString(string v, Tamgu* parent = NULL);    
	//--------------------------------
	//Buffers...
    vector<TamguPredicateVariableInstancebuff*> pvireservoire;
    VECTE<long> pviempties;
    long pviidx;
    Exporting TamguPredicateVariableInstance* Providevariableinstance(short, short);

	vector<Tamgumapssbuff*> mapssreservoire;
	VECTE<long> mapssempties;
	long mapssidx;
	Exporting Tamgumapss* Providemapss();

	vector<Tamgumapbuff*> mapreservoire;
	VECTE<long> mapempties;
	long mapidx;
	Exporting Tamgumap* Providemap();

    vector<Tamguprimemapbuff*> primemapreservoire;
    VECTE<long> primemapempties;
    long primemapidx;
    Exporting Tamguprimemap* Provideprimemap();

    vector<Tamgutreemapbuff*> treemapreservoire;
    VECTE<long> treemapempties;
    long treemapidx;
    Exporting Tamgutreemap* Providetreemap();

	vector<Tamguvectorbuff*> vectorreservoire;
	VECTE<long> vectorempties;
	long vectoridx;
	Exporting Tamguvector* Providevector();

	vector<Tamguivectorbuff*> ivectorreservoire;
	VECTE<long> ivectorempties;
	long ivectoridx;
	Exporting Tamguivector* Provideivector();

	vector<Tamgufvectorbuff*> fvectorreservoire;
	VECTE<long> fvectorempties;
	long fvectoridx;
	Exporting Tamgufvector* Providefvector();

	vector<Tamgusvectorbuff*> svectorreservoire;
	VECTE<long> svectorempties;
	long svectoridx;
	Exporting Tamgusvector* Providesvector();

	vector<Tamguuvectorbuff*> uvectorreservoire;
	VECTE<long> uvectorempties;
	long uvectoridx;
	Exporting Tamguuvector* Provideuvector();

	vector<Tamguintbuff*> intreservoire;
	VECTE<long> iempties;
	long intidx;
	Exporting Tamguint* Provideint(long v = 0);
    
    vector<Tamgulongbuff*> longreservoire;
    VECTE<long> lgempties;
    long longidx;
    Exporting Tamgulong* Providelong(BLONG v = 0);

	vector<Tamgufloatbuff*> floatreservoire;
	VECTE<long> fempties;
	long floatidx;
	Exporting Tamgufloat* Providefloat(double v = 0);

	vector<Tamgustringbuff*> stringreservoire;
	VECTE<long> sempties;
	long stringidx;
    Exporting Tamgustring* Providestring(string v = "");
    Exporting Tamgustring* Providewithstring(string& v);

	vector<Tamguustringbuff*> ustringreservoire;
	VECTE<long> uempties;
	long ustringidx;
    Exporting Tamguustring* Provideustring(wstring v = L"");
    Exporting Tamguustring* Providewithustring(wstring& v);

    vector<Tamgulisp*> lispreservoire;
    VECTE<long> lempties;
    long lispidx;
    Exporting Tamgulisp* Providelisp();

    vector<TamguDeclarationLocal*> declarationreservoire;
    VECTE<long> declempties;
    long declarationidx;
    Exporting TamguDeclarationLocal* Providedeclaration(short idt);

    vector<TamguDeclarationAutoClean*> declarationcleanreservoire;
    VECTE<long> declarationcleanempties;
    long declarationcleanidx;
    Exporting TamguDeclarationAutoClean* Providedeclarationclean(short);

    vector<Tamguselfbuff*> slfreservoire;
    VECTE<long> slfempties;
    long slfidx;
    Exporting TamguSelf* Provideself();
    
    hmap<short, ThreadLock*> booleanlocks;
    
    //--------------------------------
    hmap<std::string, TamguConstString*> string_pool;
    //--------------------------------
    GlobalConstants* global_constants;
    //--------------------------------

    hmap<BLONG, Tamgu*> integer_pool; //recording of number values
    hmap<double, Tamgu*> float_pool;

    An_rules* gTheAnnotationRules;
    Au_automatons* gAutomatons;
        
	short gpredicatedico;
	short gpredicatezone;
	short gpredicatedependency;
	short gpredicatefeature;

	void SetPredicateVariableFlags(short idthread) {
        threads[idthread].SetPredicateVariableFlags(symbolIds.size());
	}

    void Returnname(short idthread, short name) {
        threads[idthread].Returnname(name);
    }
    
    short GetName(short idthread) {
        return threads[idthread].GetName();
    }

    void Setineval(short idthread, bool v) {
        threads[idthread].in_eval = v;
    }
    
    void Storeingarbage(Tamgu* a) {
        threads[GetThreadid()].Storeingarbage(a);
    }
    
    void Removefromlocalgarbage(short idthread, long i, Tamgu* a);
    
    void Storeingarbageif_add(Tamgu* a) {
        if (add_to_tamgu_garbage)
            threads[GetThreadid()].Storeingarbage(a);
    }

    void Removefromlocalgarbageif_add(short idthread, long i, Tamgu* a) {
        if (add_to_tamgu_garbage)
            Removefromlocalgarbage(-1, -1, a);
    }

    void increment_try(short idthread) {
        threads[idthread].embedded_try++;
    }

    void decrement_try(short idthread) {
        threads[idthread].embedded_try--;
    }

    Exporting void Clearfibres(short);
	Exporting void Update();
	//--------------------------------

	//This is a list of functions to actually parse code
	hmap<string, ParseElement> parseFunctions;

	//This is the association between an class id and its instance creation...
	bin_hash<Tamgu*> newInstance;

	hmap<short, hmap<short, Tamgu*> > mapnewInstances;

	bin_hash<basebin_hash<unsigned long> > methods;
	bin_hash<unsigned long> allmethods;
	basebin_hash<unsigned long> framemethods;
	basebin_hash<bool> functions;
	basebin_hash<TamguProcedure> procedures;
	basebin_hash<TamguProcedure> commons;
    hmap<string, string> commoninfos;
    hmap<string, string> procedureinfos;

	bin_hash<unsigned long> arities;
	basebin_hash<TamguFrame*> frames;
    basebin_hash<TamguFrame*> framecontainers;
    basebin_hash<Tamgu*> framevariables;
    hmap<string, short> framecontainer_predeclared;

	basebin_hash<TamguSystemVariable*> systems;
	
    bin_hash<basebin_hash<bool> > compatibilities;
    bin_hash<basebin_hash<bool> > fullcompatibilities;
	bin_hash<basebin_hash<bool> > strictcompatibilities;
	
	basebin_hash<TamguFrame*> extensions;
	basebin_hash<bool> extensionmethods;

	TamguPredicateContainer* predicateContainer;

	hmap<string, Au_automaton*> rules;
    //--------------------------------
    basebin_hash<TamguActionVariable*> globalvariables;

    void TamguAllObjects(vector<string>& vs);

    //----------------------------------------------------------------------------------
    // These two variables are used to manage local garbaging when an eval is executed...
    std::atomic<short> number_of_current_eval;
    bool add_to_tamgu_garbage;
    //--------------------------------
	long maxstack;
	//--------------------------------

	basebin_hash<TamguPredicateVariable*> predicatevariables;
	map<string, Tamgu*> dependencyvariables;
	Tamgu* modifieddependency;
	bin_hash<TamguPredicateFunction*> predicates;
    std::set<short> predicate_definitions;
	bin_hash<string> terms;

	void Setnonblockingerror(string s, short idthread) {
		threads[idthread].nonblockingerror = s;
	}

	string Getnonblockingerror(short idthread) {
		return threads[idthread].nonblockingerror;
	}

    //--------------------------------
    //Predicate Functions
    bool Checkpredicate(short name);    
    void Clearknowledgebase();

    bool TestPredicate(TamguDeclaration* dom, TamguPredicate* p);
    char isaValidPredicate(TamguDeclaration* dom, TamguPredicate* p, hmap<unsigned short, vector<TamguPredicateRule*> >& rulebase);
    bool GetPredicates(TamguDeclaration* dom, TamguPredicate* p, vector<TamguPredicate*>& res, bool cut);
    bool StorePredicate(TamguDeclaration* dom, TamguPredicate* pv, bool last);
    bool RemovePredicates(TamguDeclaration* dom, TamguPredicate* p);
    bool RemoveThePredicate(TamguDeclaration* dom, TamguPredicate* p);
	
	TamguPredicateContainer* Predicatecontainer();

	TamguPredicateContainer* GetPredicatecontainer(short idthread) {
		return predicateContainer;
	}

	bool predicateTrace(short n);


    //--------------------------------
	
	TamguGlobal(long nb, bool setglobal=false);

	~TamguGlobal();

	//--------------------------------
	//Threading...
	ThreadLock _threadlocks;
    
    ThreadLock _trackerlock;
    ThreadLock _locktableslock;
    ThreadLock _waitstringslock;
    ThreadLock _threadvariableslock;
    
	ThreadLock _printlock;
	ThreadLock _knowledgelock;
    ThreadLock _parselock;
    ThreadLock _numberlock;

    void set_garbage_mode(bool v) {
        if (v)
            number_of_current_eval++;
        else
            number_of_current_eval--;
        
        if (!number_of_current_eval)
            add_to_tamgu_garbage = false;
    }

    bool Activategarbage(bool v) {
        bool previous = add_to_tamgu_garbage;
        add_to_tamgu_garbage = v;
        return previous;
    }

    int Addtogarbage() {
        return add_to_tamgu_garbage;
    }

    void clean_from_garbage_position(Tamgu* declaration, short idthread, long p, Tamgu* keep, long lastrecorded, long maxrecorded = -1) {
        threads[idthread].Cleanfromgarbageposition(declaration, idthread, p, keep, lastrecorded, maxrecorded);
        number_of_current_eval--;
        add_to_tamgu_garbage = number_of_current_eval;
    }

    long initialize_local_garbage(short idthread) {
        number_of_current_eval++;
        add_to_tamgu_garbage = true;
        return threads[idthread].Garbagesize();
    }

    inline bool checkmethod(short idtype, short idname) {
        return (methods.check(idtype) && methods.get(idtype).check(idname));
    }
    
    inline bool checkarity(short idtype, short idname, long sz) {
        unsigned long a = 1 << sz;
        return (methods.check(idtype) && methods.get(idtype).check(idname) && a == (methods.get(idtype).get(idname)&a));
    }
    
    inline void Pushinstruction(Tamgu* a, short idthread) {
        threads[idthread].Pushinstruction(a);
    }
    
    inline void Popinstruction(short idthread) {
        threads[idthread].Popinstruction();
    }
    
	bool isRunning() {
		return running;
	}

	Exporting short GetThreadid();

	void SetThreadid() {
		threadids.set(_GETTHREADID(),0);
	}

	void ResetThreadid() {
		threadids.erase(_GETTHREADID());
	}

    void Getstack(vector<string>& lines, vector<string>&, vector<long>&);
    void Getstack(std::stringstream& message);
	void EraseThreadid(short id);
	short InitThreadid(short id);
	short SelectThreadid();

	void Sethreading() {
		threadMODE = true;
        isthreading = true;
	}

	bool isThreading() {
		return isthreading;
	}

    void Cleanthreads(TamguThreadCall* current = NULL);
    
	void Reallocatemaxthreads(short nb) {
		if (nb > maxthreads) {
			delete[] threads;
            delete[] errors;
            delete[] errorraised;
            
			maxthreads = nb;
			threads = new ThreadStruct[maxthreads];
            errors = new bool[maxthreads];
            errorraised = new TamguError*[maxthreads];

            for (long i = 0; i < maxthreads; i++) {
                threads[i].idthread = i;
                errors[i] = false;
                errorraised[i] = NULL;
            }
		}
	}

    //--------------------------------
    atomic_vector<long> trackerslots;
    
    Exporting void RecordInTracker(Tamgu* a);
    Exporting long RecordInTrackerProtected(Tamgu* a);
    Exporting void ResetWithTracker(Tamgu* a, long id, long inc = 1);
    Exporting TamguSystemVariable* CreateSystemVariable(Tamgu* v, short n, short t);
    Exporting TamguSystemVariable* CreateSystemVariable(Tamgu* v, string n, short t);
    
    inline void RemoveFromTracker(long idx) {
        if (idx == -1)
            return;
        
        tracked.erase(idx);

        if (!trackerslots.check(idx)) {
            trackerslots.push_back(idx);
        }
    }
    
    inline bool Checktracker(Tamgu* a, long id) {
        return (id >= 0 && tracked.check(id,a));
    }
    
    inline Tamgu* GetFromTracker(long id) {
        return tracked[id];
    }
    
    inline size_t Trackedsize() {
        return tracked.size();
    }
    
    inline void Cleanfrom(long mx) {
        tracked.cleanfrom(mx);
    }
    
    //--------------------------------

	Exporting void Getdebuginfo(string& localvariables, string& allvariables, string& stack, bool, long sz, short idthread);

	void Pushpredicate(short idthread) {
		threads[idthread].prologstack++;
		if (threads[idthread].Size() >= maxstack)
			Returnerror(e_stack_overflow, idthread);
	}

	void Poppredicate(short idthread) {
		threads[idthread].prologstack--;
	}

    void Cleardebug(short idthread) {
        threads[idthread].previousinstruction=NULL;
    }

    inline bool Checkstack(short idthread) {
        return (threads[idthread].Size() >= maxstack);
    }
    
	//Push on stack a function or a domain
    inline bool Pushstacklisp(Tamgu* a, short idthread) {
        if (threads[idthread].Size() >= maxstack)
            return false;
        
        threads[idthread].stacklisp.push_back(a);
        return true;
    }
    
    inline void Popstacklisp(short idthread) {
        threads[idthread].stacklisp.pop_back();
    }

    inline Tamgu* Topstacklisp(short idthread) {
        if (threads[idthread].stacklisp.size() == 0)
            return NULL;
        return threads[idthread].stacklisp.back();
    }
    
    inline void Pushstack(Tamgu* a, short idthread = 0) {
        if (threads[idthread].pushtracked(a, maxstack))
            Seterror(e_stack_overflow, idthread);
    }

    inline void Pushstackraw(Tamgu* a, short idthread = 0) {
        if (threads[idthread].push(a, maxstack))
            Seterror(e_stack_overflow, idthread);
    }

	inline void Popstack(short idthread = 0) {
		threads[idthread].stack.pop_back();
	}

	inline Tamgu* Topstack(short idthread = 0) {
		return threads[idthread].stack.back();
	}

    Tamgu* GetTopFrame();
    
	inline size_t Stacksize(short idthread = 0) {
		return threads[idthread].stack.size();
	}

	inline Tamgu* Stack(size_t i, short idthread = 0) {
		if (i >= threads[idthread].stack.size())
			return NULL;
		return threads[idthread].stack.vecteur[i];
	}

    inline Tamgu* DStack(size_t i) {
        return threads[0].stack.vecteur[i];
    }


	Tamgu* GetTopFrame(short idthread);

    inline Tamgu* Getdeclarationandcheck(short id, short idthread) {
        if (threads[idthread].variables.check(id))
            return threads[idthread].variables.get(id).back();
        return NULL;
    }

    inline Tamgu* Getdeclaration(short id, short idthread) {
        return threads[idthread].variables.get(id).back();
    }
    
    inline Tamgu* Getmaindeclaration(short id, short idthread) {
        return threads[idthread].variables.get(id).vecteur[1];
    }

    Tamgu* Getframedefinition(short frname, short name, short idthread);

    Tamgu* Declaration(short id, short idthread);

	Tamgu* Getdefinition(short id, short idthread, Tamgu* current = aNULL);
	bool isDeclared(short id, short idthread);
	Tamgu* Declarator(short id, short idthread);

    
	inline void Update(short idthread, short fromidthread) {
		threads[idthread].Update(fromidthread);
	}

    void Displaystack(short idthread) {
        auto& a = threads[idthread].variables;
        bin_iter<short, VECTE<Tamgu*> > iter(a.table, a.indexes, a.tsize);
        
        for (;iter.table; iter++) {
            cerr << Getsymbol(iter->first) << ":" << iter->second.size() << endl;
        }
        cerr << "-------------------------" << endl;
    }
    
    inline void Storevariable(short idthread, short name, Tamgu* var) {
        threads[idthread].variables[name].push_back(var);
    }

    inline void Replacevariable(short idthread, short name, Tamgu* var) {
        threads[idthread].Replacevariable(name, var);
    }

	inline Tamgu* Getvariable(short idthread, short name) {
		return threads[idthread].variables.get(name).back();
	}

    inline Tamgu* Getavariable(short idthread, short name) {
        if (threads[idthread].variables.check(name) && threads[idthread].variables.get(name).size())
            return threads[idthread].variables.get(name).back();
        return aNULL;
    }

    inline void Removevariable(short idthread, short name) {
        threads[idthread].Removevariable(name);
    }

    inline void Removingvariable(short idthread, short name) {
        threads[idthread].variables.get(name).pop_back();
    }

    inline Tamgu* Removetopvariable(short idthread, short name) {
        return threads[idthread].variables.get(name).backpop();
    }

    Tamgu** Currentinstruction(short idthread) {
        return &threads[idthread].currentinstruction;
    }
    
	inline void Current(Tamgu* g, short idthread) {
		threads[idthread].currentinstruction = g;
	}

	inline void Pushdebug(short idthread) {
		if (debugmode) {
			threads[idthread].debugstack.push_back(threads[idthread].currentinstruction);
			if (debugfunction != NULL)
				(*debugfunction)(threads[idthread].debugstack, idthread, debugdata);
		}
	}

	inline void Popdebug(short idthread) {
		if (debugmode)
			threads[idthread].debugstack.pop_back();
	}

	inline void Setdebugmode(bool d) {
		debugmode = d;
	}

	inline void Setdebugfunction(TamguDebug f, void* data = NULL) {
		debugfunction = f;
		debugdata = data;
	}

	long Getinstructionline(short idthread);
    long Getinstructionfile(short idthread);
	//--------------------------------
	Exporting void RecordCompileFunctions();
	Exporting void RecordCompatibilities();
	Exporting void SetCompatibilities(short ty);
	Exporting void RecordObjects();
	Exporting void RecordContainers();
	Exporting void RecordConstantNames();
	Exporting void RecordProcedures();
	Exporting void RecordOneProcedure(string, string, TamguProcedure, unsigned long arity, short rettype = a_null);
	Exporting void RecordCommon(string, string, TamguProcedure, unsigned long arity, short rettype = a_null);
	Exporting void RecordSystemVariables();
	Exporting void RecordPredicates();
	Exporting void RecordPredicateFunctions();
	Exporting void RecordMathFunctions();

	void SystemInitialization(string spath);
    void Initarguments(vector<string>& args);


    Exporting void RecordArity(short type, short name, unsigned long arity);
    Exporting void RecordCompatibilities(short type);
    Exporting void RecordMethods(short type, basebin_hash<unsigned long>& exported);

	Exporting void RecordMethods(short type, short name, unsigned long arity);

//------------------------------ External Libraries ---------------------------------------
	hmap<string, TamguExternalModule> externalLibraries;
	Exporting bool RecordExternalLibrary(string name, TamguExternalModule);
	//--------------------------------Evaluate Function------------------------------------

    Exporting Tamgu* EvaluateParenthetic(string& s, string& o, string& c, bool comma, bool dgt, bool concat, vector<string>& rules, short idthread);
    Exporting Tamgu* EvaluateLisp(Tamgu* contextualpattern, string filename, string& body, short idthread);
    Exporting Tamgu* EvaluateTags(string& s, string& o, string& c, bool comma, bool dgt, bool concat, vector<string>& rules, short idthread);
	Exporting Tamgu* EvaluateVector(string& s, short idthread);
	Exporting Tamgu* EvaluateMap(string& s, short idthread);
    Exporting Tamgu* EvaluateVector(Tamgu* avect, string& s, short idthread);
    Exporting Tamgu* EvaluateMap(Tamgu* amap,string& s, short idthread);
	Exporting Tamgu* EvaluateJSON(string& s, short idthread);
    Exporting An_rules* EvaluateRules(string& s, short idthread);

	bool Checkhierarchy(short c1, short c2) {
        return (c1 == c2 || (hierarchy.check(c1) && hierarchy.get(c1).check(c2)));
	}

	bool Testcompatibility(short r, short v, bool strict) {
		if (strict) {
            return (strictcompatibilities.check(r) && strictcompatibilities.get(r).check(v));
		}

        return (compatibilities.check(r) && compatibilities.get(r).check(v));
	}

	bool Compatible(short r, short v) {
        return (compatibilities.check(r) && compatibilities.get(r).check(v));
	}

    bool Compatiblefull(short r, short v) {
        return (fullcompatibilities.check(r) && fullcompatibilities.get(r).check(v));
    }

	bool Compatiblestrict(short r, short v) {
        return (strictcompatibilities.check(r) && strictcompatibilities.get(r).check(v));
	}

	//--------------------------------
    bool executionbreak;
	static vector<string> arguments;
	void Setarguments(string args);
	//--------------------------------

	void Clear() {
		for (long i = 0; i < maxthreads; i++)
			threads[i].Clear();
	}

	//--------------------------------
    string Getsymbol(short s) {
        if (idSymbols.check(s))
            return idSymbols.get(s);
        return "";
    }

    wstring Getwsymbol(short s) {
        if (idSymbols.check(s)) {
            wstring w;
            string e = idSymbols.get(s);
            s_utf8_to_unicode(w, USTR(e) , e.size());
            return w;
        }
        return L"";
    }

    short Checkid(string s) {
        if (symbolIds.find(s) == symbolIds.end())
            return -1;
        return symbolIds[s];
    }
    
	short Getid(string s) {
		if (symbolIds.find(s) == symbolIds.end()) {
			short id = (short)symbolIds.size();
			symbolIds[s] = id;
			idSymbols[id] = s;
			return id;
		}

		return symbolIds[s];
	}

    short Getid(wstring w) {
        string s;
        s_unicode_to_utf8(s, w);
        if (symbolIds.find(s) == symbolIds.end()) {
            short id = (short)symbolIds.size();
            symbolIds[s] = id;
            idSymbols[id] = s;
            return id;
        }
        
        return symbolIds[s];
    }

	short Createid(string s) {
		short id = (short)symbolIds.size();
		symbolIds[s] = id;
		idSymbols[id] = s;
		return id;
	}

	//--------------------------------
	TamguCode* GetNewCodeSpace(string filename);
	TamguCode* Getcode(size_t idcode) {
		if (idcode >= spaces.size())
			return NULL;
		return spaces[idcode];
	}

	TamguCode* Getcurrentcode() {
		if (spaceid != -1)
			return spaces[spaceid];
		return NULL;
	}

	Tamgu* Getmainframe(size_t idcode);
    
    void DeclareInMainframe(size_t idcode, short n, Tamgu* a);
    Tamgu* DeclarationInMainframe(size_t idcode, short n);

    void DeclareTopstack(short idthread, short n, Tamgu* a);
    Tamgu* DeclarationTopstack(short idthread, short n);
    bool isDeclaredTopstack(short idthread, short n);

    bool TopstackisMainframe();
    
	long Getcurrentline();
	string Getcurrentfile();
	short Getfileid(short& idc, string f);
	Exporting string Getfilename(short fileid);

	Tamgu* EvaluateMainVariable();

	bool Loadcontent(string content);
	TamguCode* Loadfile(string filename);

	//--------------------------------
	inline bool Error(short idthread) {
        return (errors[idthread] || executionbreak);
	}

	inline bool GenuineError(short idthread) {
        return errors[idthread];
	}

	Exporting Tamgu* Errorobject(short idthread);
	string Errorstring(short idthread);
    
    void PushErrorInEvent(Tamgu* a);
    string ErrorMessage();

	Exporting Tamgu* Returnerror(Tamgu* err, short idthread);
	Exporting Tamgu* Returnerror(string err, short idthread);
    Exporting void Seterror(string err, short idthread);
	Exporting Tamgu* Returnerror(string err);

	void Cleanerror(short idthread);

	bool isFile(Tamgu* a);
	bool isString(Tamgu* a);
	bool isNumber(Tamgu* a);
	bool isContainer(Tamgu* a);
	bool isVectorContainer(Tamgu* a);
	bool isMapContainer(Tamgu* a);
	short Type(Tamgu* a);
};

//-----------------------------------------------------------------------

class TamguRaiseError {
public:

	string message;
	string filename;
	size_t left;
	size_t right;
    size_t left_pos;
    size_t right_pos;

	TamguRaiseError(stringstream& mes, string file = "", size_t l = 0, size_t r = 0, size_t l_p = 0, size_t r_p = 0);
};

class Tamgudebug {
public:

	short idthread;
	bool protect;
	bool protectfirst;

	Tamgudebug(short idt, Tamgu* i = NULL) {
		idthread = idt;
		protect = true;
		if (i != NULL) {
            globalTamgu->Current(i, idthread);
			globalTamgu->Pushdebug(idthread);
			protectfirst = false;
		}
		else
			protectfirst = true;
	}

	void push() {
		protect = false;
		globalTamgu->Pushdebug(idthread);
	}

	void pop() {
		globalTamgu->Popdebug(idthread);
		protect = true;
	}

	~Tamgudebug() {
		if (!protect)
			globalTamgu->Popdebug(idthread);
		if (!protectfirst)
			globalTamgu->Popdebug(idthread);
	}

};

#endif






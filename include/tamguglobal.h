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
#include "vecte.h"

#include "x_node.h"

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
class Tamgulisp;

//--------------------------------------------------------------------
typedef bool(*TamguExternalModule)(TamguGlobal*, string);
//--------------------------------------------------------------------
typedef double(*mathFunc)(double);
//--------------------------------------------------------------------
extern bool executionbreak;
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
	vector<Tamgu*> debugstack;
    
    VECTE<TamguCallFibre*> fibres;

    std::atomic<short> nbjoined;
    
	basebinn_hash<vector<TamguPredicate*> > knowledgebase;
	bin_hash<VECTE<Tamgu*> > variables;

	string nonblockingerror;

	stringstream message;
	threadhandle handle;

    FstCompanion* fstcompanion;
	Tamgu* returnvalue;
    Tamgu* previousinstruction;
    Tamgu* currentinstruction;
	hmap<string, ThreadLock*> locks;

	long parentthreads;
	short prologstack;

	bool used;

	ThreadStruct();
	Exporting ~ThreadStruct();

    FstCompanion* Companion();
    
	inline long Size() {
		return (prologstack + stack.size() + stacklisp.size());
	}

	void Update(short idthread);

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
	void Clearknowledgebase();
	void Setknowledgebase();

	Tamgu* Getdefinition(short id);
    Tamgu* Declaration(short id);
	Tamgu* Declarator(short id);
	bool isDeclared(short id);
	Tamgu* GetTopFrame();

	bool TestPredicate(TamguDeclaration* dom, TamguPredicate* p);
	char isaValidPredicate(TamguDeclaration* dom, TamguPredicate* p, hmap<unsigned short, vector<TamguPredicateRule*> >& rulebase);
	bool GetPredicates(TamguDeclaration* dom, TamguPredicate* p, vector<TamguPredicate*>& res, bool cut);
	bool StorePredicate(TamguDeclaration* dom, TamguPredicate* pv, bool last);
	bool RemovePredicates(TamguDeclaration* dom, TamguPredicate* p);
	bool RemoveThePredicate(TamguDeclaration* dom, TamguPredicate* p);

	inline bool Checkpredicate(short name) {
		if (name == a_universal){
			if (knowledgebase.size())
				return true;
			return false;
		}

        return knowledgebase.check(name);
	}

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

    short idglobal;
	long maxrange;

	//All programmes stored in memory
	vector<TamguCode*> spaces;
	hmap<string, TamguCode*> codes;

	ThreadStruct* threads;
    bool* errors;
    TamguError** errorraised;


    atomic_map<threadhandle, int> threadids;


	//This variable is set to TRUE in thread mode... It allows then for the actual creation of locks...
    bool globalLOCK;
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

    short Typeinstance(string s);
    
    hmap<string, short> string_operators;
	bin_hash<string> operator_strings;
	bin_hash<bool> atanOperatorMath;
	bin_hash<short> equto;

	bin_hash<short> returntypes;

	//--------------------------------
	//Buffers...
    vector<TamguPredicateVariableInstance*> pvireservoire;
    VECTE<long> pviempties;
    long pviidx;
    Exporting TamguPredicateVariableInstance* Providepvi(short);

	vector<Tamgumapssbuff*> mapssreservoire;
	VECTE<long> mapssempties;
	long mapssidx;
	Exporting Tamgumapss* Providemapss();

	vector<Tamgumapbuff*> mapreservoire;
	VECTE<long> mapempties;
	long mapidx;
	Exporting Tamgumap* Providemap();

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
    Exporting TamguDeclarationLocal* Providedeclaration(Tamgu* ins, short idt, bool p);

    vector<TamguDeclarationAutoClean*> declarationcleanreservoire;
    VECTE<long> declarationcleanempties;
    long declarationcleanidx;
    Exporting TamguDeclarationAutoClean* Providedeclarationclean(short);

    vector<Tamguselfbuff*> slfreservoire;
    VECTE<long> slfempties;
    long slfidx;
    Exporting TamguSelf* Provideself();
    
    bin_hash<ThreadLock*> booleanlocks;
    
    //--------------------------------
	Tamgu* gNULL;
	Tamgu* gUNIVERSAL;
	Tamgu* gTRUE;
	Tamgu* gFALSE;

	Tamgu* gMINUSTEN;
	Tamgu* gMINUSNINE;
	Tamgu* gMINUSEIGHT;
	Tamgu* gMINUSSEVEN;
	Tamgu* gMINUSSIX;
	Tamgu* gMINUSFIVE;
	Tamgu* gMINUSFOUR;
	Tamgu* gMINUSTHREE;
	Tamgu* gMINUSTWO;
	Tamgu* gMINUSONE;
	Tamgu* gZERO;
    Tamgu* gZEROPOINTZERO;
	Tamgu* gONE;
	Tamgu* gTWO;
	Tamgu* gTHREE;
	Tamgu* gFOUR;
	Tamgu* gFIVE;
	Tamgu* gSIX;
	Tamgu* gSEVEN;
	Tamgu* gEIGHT;
	Tamgu* gNINE;
	Tamgu* gTEN;
    Tamgu* gELEVEN;
    Tamgu* gTWELVE;
    Tamgu* gTHIRTEEN;
    Tamgu* gFOURTEEN;
    Tamgu* gFIFTEEN;
    Tamgu* gSIXTEEN;
    Tamgu* gEIGHTEEN;
    Tamgu* gSEVENTEEN;
    Tamgu* gNINETEEN;
    Tamgu* gTWENTY;
    Tamgu* gTHIRTYTWO;
    Tamgu* gSIXTYFOUR;
    
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
	Tamgu* gAFFECTATION;
	TamguConstiteration* gITERNULL;
	TamguPredicate* gFAIL;
	TamguPredicate* gCUT;
	TamguPredicate* gSTOP;
	Tamgu* gHASKELL;
	TamguDeclaration* gNULLDECLARATION;
    TamguLet* gNULLLet;
    Tamgu* gNOTHING;

    hmap<BLONG, Tamgu*> numbers; //we record the values we've seen so far in order to avoid some overgeneration of constant values...

    An_rules* gTheAnnotationRules;
    Au_automatons* gAutomatons;
    
	unsigned short gpredicatename;
	unsigned short gpredicatedico;
	unsigned short gpredicatezone;
	unsigned short gpredicatedependency;
	unsigned short gpredicatefeature;

	void SetPredicateVariableFlags() {
		gpredicatezone = idSymbols.size();
		gpredicatedico = gpredicatezone + 1;
		gpredicatedependency = gpredicatedico + 1;
		gpredicatefeature = gpredicatedependency + 1;
		gpredicatename = gpredicatefeature + 1;
	}



    Exporting void Clearfibres(short);
	Exporting void Update();
	//--------------------------------

	//This is a list of functions to actually parse code
	hmap<string, ParseElement> parseFunctions;

	//This is the association between an class id and its instance creation...
	bin_hash<Tamgu*> newInstance;

	hmap<short, hmap<short, Tamgu*> > mapnewInstances;

	bin_hash<bin_hash<unsigned long> > methods;
	bin_hash<unsigned long> allmethods;
	bin_hash<unsigned long> framemethods;
	bin_hash<bool> functions;
	bin_hash<TamguProcedure> procedures;
	bin_hash<TamguProcedure> commons;
    hmap<string, string> commoninfos;

	bin_hash<unsigned long> arities;
	bin_hash<TamguFrame*> frames;

	bin_hash<TamguSystemVariable*> systems;
	
	bin_hash<basebin_hash<bool> > compatibilities;
	bin_hash<basebin_hash<bool> > strictcompatibilities;
	
	bin_hash<TamguFrame*> extensions;
	bin_hash<bool> extensionmethods;

	TamguPredicateContainer* predicateContainer;

	hmap<string, Au_automaton*> rules;
    //--------------------------------
    basebin_hash<TamguActionVariable*> globalvariables;

    void TamguAllObjects(vector<string>& vs);

    //--------------------------------
	long maxstack;
	//--------------------------------

	basebin_hash<TamguPredicateVariable*> predicatevariables;
	map<string, Tamgu*> dependencyvariables;
	basebin_hash<short> dependenciesvariable;
	Tamgu* modifieddependency;
	bin_hash<TamguPredicateFunction*> predicates;
	bin_hash<string> terms;

	void Setnonblockingerror(string s, short idthread) {
		threads[idthread].nonblockingerror = s;
	}

	string Getnonblockingerror(short idthread) {
		return threads[idthread].nonblockingerror;
	}

	bool TestPredicate(TamguDeclaration* dom, TamguPredicate* p, short idthread) {
		return threads[idthread].TestPredicate(dom, p);
	}
	
	char isaValidPredicate(TamguDeclaration* dom, TamguPredicate* p, hmap<unsigned short, vector<TamguPredicateRule*> >& rulebase, short idthread) {
		return threads[idthread].isaValidPredicate(dom, p, rulebase);
	}
	
	bool Checkpredicate(short name, short idthread) {
		return threads[idthread].Checkpredicate(name);
	}

	bool GetPredicates(TamguDeclaration* dom, TamguPredicate* p, vector<TamguPredicate*>& res, bool cut, short idthread) {
		return threads[idthread].GetPredicates(dom, p, res, cut);
	}
	
	bool StorePredicate(TamguDeclaration* dom, TamguPredicate* pv, bool last, short idthread) {
		return threads[idthread].StorePredicate(dom, pv, last);
	}
	
	bool RemovePredicates(TamguDeclaration* dom, TamguPredicate* p, short idthread) {
		return threads[idthread].RemovePredicates(dom, p);
	}

	bool RemoveThePredicate(TamguDeclaration* dom, TamguPredicate* p, short idthread) {
		return threads[idthread].RemoveThePredicate(dom, p);
	}
	
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
	ThreadLock _trackerlock;
    
    ThreadLock _locktableslock;
    ThreadLock _waitstringslock;
    ThreadLock _threadvariableslock;
    
	ThreadLock _printlock;
	ThreadLock _knowledgelock;
    ThreadLock _parselock;

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

	void EraseThreadid(short id);
	short InitThreadid(short id);
	short SelectThreadid();

	void Sethreading() {
		globalLOCK = true;
        isthreading = true;
	}

	bool isThreading() {
		return isthreading;
	}

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
                errors[i] = false;
                errorraised[i] = NULL;
            }
		}
	}

    //--------------------------------
    VECTE<long> trackerslots;
    std::atomic<long> trackerslotfilled;
    
    Exporting void RecordInTracker(Tamgu* a);
    Exporting long RecordInTrackerProtected(Tamgu* a);
    Exporting void ResetWithTracker(Tamgu* a, long id, long inc = 1);
    
    inline void RemoveFromTracker(long idx) {
        if (idx == -1)
            return;
        
        tracked.erase(idx);
        
        if (globalLOCK) {
            _trackerlock.Locking();
            trackerslots.push_back(idx);
            ++trackerslotfilled;
            _trackerlock.Unlocking();
        }
        else {
            trackerslots.push_back(idx);
            ++trackerslotfilled;
        }
        
    }
    
    inline bool Checktracker(Tamgu* a, long id) {
        if (id >= 0 && tracked.check(id,a))
            return true;
        return false;
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
			Returnerror("Stack overflow", idthread);
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
    
    void Pushstack(Tamgu* a, short idthread = 0);

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

	inline Tamgu* Getvariable(short idthread, short name) {
		return threads[idthread].variables.get(name).back();
	}

	inline void Storevariable(short idthread, short name, Tamgu* var) {
		threads[idthread].variables[name].push_back(var);
	}

	inline void Removevariable(short idthread, short name) {
		threads[idthread].Removevariable(name);
	}

    inline Tamgu* Removetopvariable(short idthread, short name) {
        return threads[idthread].variables.get(name).backpop();
    }


	inline void Replacevariable(short idthread, short name, Tamgu* var) {
		 threads[idthread].Replacevariable(name, var);
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
	//--------------------------------
	Exporting void RecordCompileFunctions();
	Exporting void RecordCompatibilities();
	Exporting void SetCompatibilities(short ty);
	Exporting void RecordObjects();
	Exporting void RecordContainers();
	Exporting void RecordConstantNames();
	Exporting void RecordProcedures();
	Exporting void RecordOneProcedure(string, TamguProcedure, unsigned long arity, short rettype = a_null);
	Exporting void RecordCommon(string, string, TamguProcedure, unsigned long arity, short rettype = a_null);
	Exporting void RecordSystemVariables();
	Exporting void RecordPredicates();
	Exporting void RecordPredicateFunctions();
	Exporting void RecordMathFunctions();

	void SystemInitialization(string spath);
    void Initarguments(vector<string>& args);


	Exporting void RecordMethods(short type, bin_hash<unsigned long>& exported);

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
	Exporting Tamgu* EvaluateJSON(string& s, short idthread);
    Exporting An_rules* EvaluateRules(string& s, short idthread);

	bool Checkhierarchy(short c1, short c2) {
		if (c1 == c2)
			return true;
		if (hierarchy.check(c1) && hierarchy.get(c1).check(c2))
			return true;
		return false;
	}

	bool Testcompatibility(short r, short v, bool strict) {
		if (strict) {
			if (strictcompatibilities.check(r) && strictcompatibilities.get(r).check(v))
				return true;
			return false;
		}

		if (compatibilities.check(r) && compatibilities.get(r).check(v))
			return true;
		return false;
	}

	bool Compatible(short r, short v) {
		if (compatibilities.check(r) && compatibilities.get(r).check(v))
			return true;
		return false;
	}

	bool Compatiblestrict(short r, short v) {
		if (strictcompatibilities.check(r) && strictcompatibilities.get(r).check(v))
			return true;
		return false;
	}

	//--------------------------------
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
		if (errors[idthread] || executionbreak)
			return true;
		return false;
	}

	inline bool GenuineError(short idthread) {
		if (errors[idthread])
			return true;
		return false;
	}

	Exporting Tamgu* Errorobject(short idthread);
	string Errorstring(short idthread);
	Exporting Tamgu* Returnerror(Tamgu* err, short idthread);
	Exporting Tamgu* Returnerror(string err, short idthread);
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

	TamguRaiseError(stringstream& mes, string file = "", size_t l = 0, size_t r = 0);
};

class Tamgudebug {
public:

	short idthread;
	bool protect;
	bool protectfirst;

	Tamgudebug(short id, Tamgu* i = NULL) {
		idthread = id;
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






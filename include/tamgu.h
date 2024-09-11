/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgu.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgu_h
#define tamgu_h

#include <stdio.h>
#include <string.h>
#include "messages_error.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>


#include "conversion.h"
#include "vecte.h"

class TamguCode;
class TamguGlobal;
class Tamgu;
class TamguCall;
class TamguDeclaration;
class TamguConst;
class TamguFunction;
class TamguFrame;
class TamguIteration;
class TamguInstruction;
class Tamgufraction;
class TamguFunctionLambda;
class TamguIndex;
class TamguPredicateContainer;
class TamguPredicateVariableInstance;
class TamguInstructionEvaluate;
class An_any;

#include "tamguconstants.h"

#ifdef GARBAGESCAN
Exporting void Garbaging(hmap<std::string, long>& issues);
Exporting void Garbaging(hmap<long, long>& issues);
#endif

#ifdef MULTIGLOBALTAMGU
Exporting void Garbaging(hmap<std::string, long>& issues);
Exporting void Garbaging(hmap<long, long>& issues);
#ifdef WIN32
#define localthread __declspec( thread )
#else
#ifdef APPLE
#define localthread thread_local
#else
#define localthread thread_local
#endif
#endif
#else
#define localthread
#endif

//-----------------------------------------------------------------------
bool ToggleLispMode();
bool isLispmode();
void Setlispmode(bool v);
Exporting void Set_keep_track_garbage(bool v);
//-----------------------------------------------------------------------
inline long maxlocal(long x, long y) { return (x > y) ? x : y; }
inline long minlocal(long x, long y) { return (x < y) ? x : y; }
//-----------------------------------------------------------------------
extern Exchanging vector<TamguGlobal*> globalTamguPool;

#define global_tamgu globalTamguPool[idglobal]

//The main variable, which controls the world...
#ifdef MULTIGLOBALTAMGU
extern localthread TamguGlobal* globalTamgu;
#else
extern Exchanging TamguGlobal* globalTamgu;
#endif

#define _getlocktamgu(x) globalTamgu->threadMODE ? (x->hasLock() ? new Locking((TamguObject*)x) : NULL) : NULL
#define _cleanlocktamgu(x) if (x != NULL) delete x

#define _getlock(x) globalTamgu->threadMODE ? new Locking(x) : NULL
#define _getlocks(x,y)  globalTamgu->threadMODE ? new Doublelocking(x,y) : NULL
#define _cleanlock(x) if (globalTamgu->threadMODE) delete x

#define _getlockg(x) threadMODE ? new Locking(x) : NULL
#define _cleanlockg(x) if (threadMODE) delete x

//-----------------------------------------------------------------------
void PrintALine(TamguGlobal* g, string s);
//-----------------------------------------------------------------------
#include "tamguglobal.h"

typedef enum {is_none = 0, is_container = 1, is_constante = 2, is_constcontainer = 3, is_declaration = 4,
    is_tracked = 8, is_null = 0x0A, is_variable = 0x10, is_callvariable = 0x18,
    is_index = 0x1C, is_string = 0x20, is_pure_string = 0x24, is_number =  0x40, is_regular = 0x80,
    is_tobelocked = 0x100, is_noneedforlock = 0x200, is_checkforlock = 0x300, is_frameinstance = 0x400, is_predicate = 0x800, is_predicatemethod = 0x1000,
    is_continue = 0x8002, is_error = 0xC002, is_break = 0xE002, is_return = 0xF002,
    is_noconst = 0xFFFD
} is_investigations;

//Tamgu is the class from which every element descends
class Tamgu {
public:
	long idtracker;
    unsigned short investigate;

#if defined(GARBAGESCAN) || defined(GARBAGEINDEBUG)
	long iddebug;
	Exporting Tamgu();
	Exporting virtual ~Tamgu();
#else
	Tamgu();
	virtual ~Tamgu();
#endif

	virtual Tamgu* Compile(TamguGlobal*, Tamgu* parent) {
		return this;
	}

    void Protectfromtracker() {
        idtracker = -100;
    }

	//These two methods are the most important, they store or get a value from an element
	virtual Tamgu* Put(Tamgu* index, Tamgu* value, short idthread) {
		return this;
	}

    virtual Tamgu* EvalWithSimpleIndex(Tamgu* idx, short idthread, bool) {
        return this;
    }

    virtual Tamgu* EvalIndex(Tamgu* context, TamguIndex* value, short idthread) {
        return this;
    }
    
    virtual Tamgu* Eval_Arguments(TamguDeclarationLocal* context, Tamgu* value, short idthread) {
        return aFALSE;
    }
    
	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
		return this;
	}

    virtual Tamgu* EvalComplexe(Tamgu* context, Tamgu* value, short idthread) {
        return Eval(context, value, idthread);
    }

	virtual Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread) {
		return Eval(environment, value, idthread);
	}
    
	virtual void Setidtype(TamguGlobal* global) {}

    virtual Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return this;
    }
    
    virtual Tamgu* CallTopMethod(short idname, TamguFrame* frame, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return this;
    }
    
    virtual bool Setvalue(Tamgu* index, Tamgu* value, short idthread, bool strict = false) {
		return Put(index, value, idthread)->Boolean();
	}

	virtual Tamgu* Putvalue(Tamgu* value, short idthread) {
		return Put(aNULL, value, idthread);
	}

    virtual Tamgu* Clonevalue(Tamgu* value, short idthread) {
        return Putvalue(value, idthread);
    }

    virtual void Putatomicvalue(Tamgu* value) {
        Put(value, aNULL, globalTamgu->GetThreadid());
    }
    
    inline Tamgu* GetvalueforThread() {
        if (!duplicateForCall())
            return this;
        return Atom();
    }

    virtual Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);

	virtual void Insert(long idx, Tamgu* ke) {}

	//Some building operations...
	//This operation is called whenever an instruction is added to a structure...
	virtual void Subcontext(bool b) {}
    virtual void Setpartialtest(bool b) {}
    
	virtual Tamgu* Parent() {
		return aNULL;
	}
    
    virtual void Getshape(vector<long>& sh) {}
    
    virtual bool isPredicateNameVariable() {
        return false;
    }
    
    virtual short checkTypePredicate() {
        return a_null;
    }
    
    virtual void add(An_any* e) {}
    virtual void push(An_any* e) {}
	virtual void InstructionClear() {}
	virtual Tamgu* Lastinstruction() { return aNULL; }
	virtual void AddInstruction(Tamgu* a) {}
	virtual void Putinstruction(size_t i, Tamgu* ke) {}
	virtual Tamgu* InstructionRemoveLast() { return aNULL; }
	virtual void InstructionRemove(size_t i) {}
	virtual size_t InstructionSize() { return 0; }
	virtual Tamgu* Instruction(size_t i) { return aNULL; }
    
    virtual Tamgu* Lispbody() {return this;}

	virtual void ScanVariables(vector<short>& vars) {}
	virtual void CheckTaskellComposition() {}

    virtual void Setinstruction(short) {}

	virtual void Setinfo(Tamgu* a) {}
	virtual void Getinfo(Tamgu* a) {}
	virtual Tamgu* Initialisation() { 
		return aNULL; 
	}

	virtual bool Checkvariable() {
		return false;
	}

    //We check if all variables are accessible for computing (see where variables Haskell)
    virtual bool Computevariablecheck(short idthread) {
        return false;
    }
    
	virtual void Setchoice(char v) {}

	virtual char Choice() {
		return -1;
	}
        
    virtual Tamgu* car(short idthread) {
        return aNOELEMENT;
    }
    
    virtual Tamgu* cdr(short idthread) {
        return aNOELEMENT;
    }

    virtual char Declarelocal(short idthread, short n, Tamgu* a) {
        return globalTamgu->Topstack()->Declarelocal(idthread, n, a);
    }
    
    virtual void Replacedeclaration(short idthread, short n, Tamgu* a) {
        globalTamgu->Topstack()->Replacedeclaration(idthread, n, a);
        globalTamgu->Replacevariable(idthread, n, a);
    }
	//------------------------------------------------------------------
	//Reference operations
    virtual uchar checkmark() {
        return 0;
    }
    
    virtual void mark(uchar m) {}
    virtual void unmark() {}
    virtual void markloop(uchar m) {}

    virtual void Removecontainerreference(short inc) {}

    virtual void Removereference(short inc = 1) {
        Resetreference(inc);
    }
    
    virtual void Cleanreference(short inc) {}
    
    virtual void Addreference(unsigned short inv, short inc=1) {}
    virtual void Setreference(short r) {}
    virtual void Setreference() {}
	virtual void Resetreference(short r = 1) {}
	virtual void Resetreferencenoprotect(short r = 1) {}
	virtual void Release() {}
	virtual void Protect() {}
	virtual void Remove() {}
	virtual void SetConst() {}

	virtual string Info(string n) {
		return "Unknown method";
	}

	virtual void Methods(Tamgu*) {}

    virtual void Prepare(Tamgu* env, short idthread) {}

    virtual void Setaffectation(bool b){}
	virtual void Setevaluate(bool v) {}

	Exporting virtual Tamgu* Succ();
	Exporting virtual Tamgu* Pred();

	virtual Tamgu* Nextfunction() {
		return NULL;
	}


	virtual Tamgu* Atom(bool forced = false) {
		return this;
	}

	virtual Tamgu* Returned(short idthread) {
		return this;
	}

	virtual Tamgu* Value() {
		return this;
	}

    virtual Tamgu* GetLetValue() {
        return this;
    }
    
	virtual Tamgu* Value(wstring& w) {
		string res;
		s_unicode_to_utf8(res, w);
		return Value(res);
	}

	virtual Tamgu* Value(string&) {
		return aNOELEMENT;
	}

	virtual Tamgu* Value(long) {
		return aNOELEMENT;
	}

	virtual Tamgu* Value(double) {
		return aNOELEMENT;
	}

	virtual Tamgu* Value(Tamgu*) {
		return aNOELEMENT;
	}

	//------------------------------------------------------------------
    virtual bool directCall() {
        return false;
    }
    
    virtual bool isLocalEvaluation() {
        return false;
    }
    
    virtual bool isFunctionVariable() {
        return false;
    }

    virtual bool isaIF() {
        return false;
    }
    
    virtual bool isPartialtest() {
        return false;
    }

    virtual Tamgu* update(short idthread) {
        return this;
    }
    
	Exporting virtual bool Unify(TamguDeclaration* dom, Tamgu* a);
    
	virtual bool isUnified(TamguDeclaration* dom) {
		return true;
	}
	
	virtual bool isComputable(TamguDeclaration* dom) {
		return isUnified(dom);
	}

	virtual void Cleans(Tamgu* v, bool localvalue) {}

	virtual void Leaves(Tamgu* v) {
		v->Push(this);
	}

	virtual Tamgu* Returnobject() { return aNULL; }

	virtual Tamgu* PredicateEvalue(TamguInstructionEvaluate* context, VECTE<Tamgu*>& stack, TamguPredicate* currenthead, long depth) {
		return aFALSE;
	}

	Exporting virtual Tamgu* Predicate(TamguDeclaration* dom, short idthread);
	virtual bool Insertvalue(Tamgu* dom, Tamgu* v, basebin_hash<Tamgu*>&) {
		return true;
	}

	virtual void Variables(vector<short>& vars) {}

	virtual Tamgu* VariableValue(TamguDeclaration* dom, short idthread) {
		return aNULL;
	}

	virtual Tamgu* Variable(TamguDeclaration* dom) {
		return NULL;
	}

	Exporting virtual Tamgu* ExtractPredicateVariables(Tamgu* contextualpattern, TamguDeclaration* dom, Tamgu* c, Tamgu* e, short, bool root);
	Exporting virtual Tamgu* EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread);

	virtual void Setfail(bool) {}
	virtual Tamgu* Last() {
		return aNULL;
	}
    
    virtual bool FindAndClean(Tamgu* a) {
        return true;
    }

    virtual void Setdomainlock() {}
    virtual void Resetdomainlock() {}
    
    //------------------------------------------------------------------
	virtual void SetVariablesWillBeCreated() {}
	virtual void Forcedclean() {}
	virtual void Setforced(bool v) {}
	virtual Tamgu* Newinstance(short idthread, Tamgu* f = NULL) { return this; }
	virtual Tamgu* Newvalue(Tamgu* a, short idthread) {
		Tamgu* v = Newinstance(idthread);
		v->Put(aNULL, a, idthread);
		a = v->Eval(aNULL, aNULL, idthread);
		v->Release();
		return a;
	}

    //This is a specific case to avoid returning an instance through Providexxx...
    virtual Tamgu* Newpureinstance(short idthread) {
        return Newinstance(idthread);
    }
    
    virtual Tamgu* anInstance(long i) {
        return Newinstance(0);
    }
    
    virtual TamguIteration* Newiteration(bool direction) {
        return (TamguIteration*)aITERNULL;
    }

	virtual void Postinstantiation(short idthread, bool setreference) {}

	virtual bool isObject() {
		return false;
	}
    
	virtual long Lockid() {
		return -1;
	}
	
	virtual bool isEQU() {
		return false;
	}

	virtual bool isAffectation() {
		return false;
	}

    virtual bool checkAffectation() {
        return true;
    }
    
	virtual bool isSequence() {
		return false;
	}

	virtual bool hasDeclaration() {
		return false;
	}

	virtual bool isDeclared(short id) {
		return false;
	}

	virtual void Declare(short id, Tamgu* a) {}

	virtual Tamgu* Declaration(short id) {
		return NULL;
	}

    virtual Tamgu* Declared(short id) {
        return Declaration(id);
    }

    virtual uchar BType() {
        return 255;
    }
    
	virtual short Type() {
		return 0;
	}

    virtual string Typestring() {
        return globalTamgu->Getsymbol(Type());
    }
    
	virtual short Typenumber() {
		if (isNumber())
			return Type();
		return 0;
	}

	virtual void Setname(short n) {}

	virtual string Typename() {
        return globalTamgu->Getsymbol(Type());
    }

	void Setid(long i) {
		idtracker = i;
	}

    long Setidreturn(long i) {
        idtracker = i;
        return idtracker;
    }
    
	long Idtracker() {
		return idtracker;
	}

	virtual void Setcodereference(long l, short i) {}
    
    virtual long Currentinfo() {
        return -1;
    }
    
	virtual long Currentline() {
		return -1;
	}

	virtual short Currentspace() {
		return -1;
	}

	virtual short Currentfile() {
		return -1;
	}

	virtual void Setaction(short a) {}
	virtual short Action() {
		return a_none;
	}

	virtual void Addparent(Tamgu*) {}

	virtual Tamgu* Frame() {
		return NULL;
	}

    virtual bool isParent(TamguFrame*) {
        return false;
    }
    

	virtual void Setprotect(bool n) {}

	virtual void Protectcontainervalues() {}

	virtual void Popping() {}
	
    virtual bool Candelete() {
        return true;
    }
    
    virtual void Finalclean(std::set<Tamgu*>& elements_to_delete) {}
    void Deletion(std::set<Tamgu*>& elements_to_delete);
    
	virtual long Setprotect() {
		return -1;
	}

	//-----------------------------------------------
    virtual void searchall(string& txt, vector<long>&) {}
    virtual void searchall(wstring& txt, vector<long>&) {}

    virtual void searchall(string& txt, vector<long>&, long init) {}
    virtual void searchall(wstring& txt, vector<long>&, long init) {}

    virtual bool search(wstring& w, long& f, long& l, long init =0) {
        return false;
    }
    
    virtual bool search(string& w, long& f, long& l, long init = 0) {
        return false;
    }
    
    virtual bool searchlast(wstring& w, long& first, long& last, long init = 0) {
        return false;
    }
    virtual bool searchlast(string& w, long& first, long& last, long init = 0) {
        return false;
    }

    virtual wstring wreplace(wstring& w, wstring& rep) {
        return L"";
    }
    
    virtual string replace(string& w, string& rep) {
        return "";
    }
    
    virtual Tamgu* splitting(Tamgu* contextual, wstring& w) {
        return aNULL;
    }
    
    virtual Tamgu* splitting(Tamgu* contextual, string& w) {
        return aNULL;
    }
    
    //-----------------------------------------------
	virtual void Reserve(long d) {}
	virtual void Clone(TamguInstruction* ki) {}
    virtual Tamgu* Merging(Tamgu*) {
        return aNOELEMENT;
    }
    virtual Tamgu* Combine(Tamgu*) {
        return aNOELEMENT;
    }
	//-----------------------------------------------
	virtual void Setdisjunction(bool v) {}
	virtual basebin_hash<TamguPredicateVariableInstance*>* Dico() {
		return NULL;
	}

	virtual void Setdico(short n, TamguPredicateVariableInstance* a) {}
	virtual TamguPredicateVariableInstance* Getdico(short n) {
		return NULL;
	}

	virtual bool Checkdico(short n) {
		return false;
	}

	virtual void setSucessfull(bool v) {}
	virtual bool hasbeenSuccesfull() {
		return false;
	}

	virtual char Indexes(short idthread, long sz, long& ileft, long& iright) {
		Tamgu* val = Eval(aNULL, aNULL, idthread);
		ileft = val->Integer();
		val->Release();
		if (ileft < 0 || ileft >= sz)
			return 0;
		return 1;
	}

	virtual bool Failed() {
		return false;
	}

	virtual Tamgu* Evalue(TamguDeclaration* dom, short idthread, bool duplicate) {
		return Getvalues(dom, idthread);
	}

	Exporting virtual Tamgu* Getvalues(TamguDeclaration* dom, bool duplicate);
	virtual void Setmerge() {}

	virtual bool Checkprecision(Tamgu* r) {
		return true;
	}

	//-----------------------------------------------
	virtual bool isPredicateVariable() {
		return false;
	}

	virtual bool isDisjunction() {
		return false;
	}

    virtual bool checkDisjunction() {
        return false;
    }

	// Status Functions
	virtual short Reference() {
		return 0;
	}

	virtual bool duplicateForCall() {
		return true;
	}

	virtual bool Subcontext() {
		return false;
	}

	virtual bool isProtected() {
		return false;
	}

	virtual bool isPrivate() {
		return false;
	}

    virtual bool isTail() {
        return false;
    }
        
	virtual bool isCall() {
		return false;
	}

    virtual bool isCallFunction() {
        return false;
    }
    
    virtual char isCallTaskellFunction() {
        return 0;
    }
        
    virtual bool isLisp() {
        return false;
    }
    
	virtual bool isFile() {
		return false;
	}

	virtual bool isLong() {
		return false;
	}
	
	virtual bool isShort() {
		return false;
	}

	virtual bool isFloat() {
		return false;
	}

	virtual bool isInteger() {
		return false;
	}

	virtual bool isDecimal() {
		return false;
	}

	virtual bool isPureCallVariable() {
		return false;
	}

	virtual bool isAssignable() {
		return false;
	}

	virtual bool isBoolean() {
		return false;
	}

	virtual bool isIncrement() {
		return false;
	}

	virtual bool isAtom() {
		return false;
	}

    virtual bool checkAtomType(short ty) {
        return false;
    }

    virtual Tamgu* Atomref() {
        return this;
    }

    virtual Tamgu* AtomNoConst() {
        return Atom();
    }

    virtual bool isTime() {
        return false;
    }
    
    virtual bool isLoop() {
        return false;
    }
    
	virtual bool isJoined() {
		return false;
	}

    virtual bool isObjectContainer() {
        return false;
    }

	virtual bool isContainerClass() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

	virtual bool isVectorValueContainer() {
		return false;
	}

	virtual bool isVectorContainer() {
		return false;
	}

	virtual bool isMerge() {
		return false;
	}

	virtual bool isMapContainer() {
		return false;
	}

	virtual bool isEvaluate() {
		return false;
	}

	virtual bool isEmpty() {
		return true;
	}

	virtual bool isFrame() {
		return false;
	}

    virtual bool isMainFrame() {
        return false;
    }

    virtual bool isInfinite() {
		return false;
	}

	virtual bool isChosen() {
		return false;
	}

	virtual bool isROOTOPERATION() {
		return false;
	}

	virtual bool isOperation() {
		return false;
	}
    
    virtual bool is_e_operation() {
        return false;
    }

	virtual bool isInstruction() {
		return false;
	}

	virtual bool isInterval() {
		return false;
	}

	virtual bool isStop() {
        return false;
    }
    
	virtual short Name() {
		return 0;
	}

    virtual bool isName(short n) {
        return false;
    }
    
	virtual bool isAlias() {
		return false;
	}

    virtual bool isFunction() {
        return false;
    }

    virtual bool isCurryfied() {
        return false;
    }
    
	virtual bool isFunctionParameter() {
		return false;
	}

    virtual bool isaFunction() {
        return false;
    }
    
	virtual bool isThread() {
		return false;
	}

	virtual bool isExclusive() {
		return false;
	}

	virtual bool isTaskellFunction() {
		return false;
	}

	virtual bool Predeclared() {
		return false;
	}

	virtual bool isUsed() {
		return false;
	}

    virtual short Typeframe() {
        return 0;
    }
    
	virtual short Typevariable() {
		return Type();
	}

	virtual short Typevariable(short i) {
		return a_none;
	}

	virtual short Typeinfered() {
		return Typevariable();
	}

	virtual short Yourowntype() {
		return 0;
	}

    inline bool isConstContainer() {
        return (investigate == is_constcontainer); //is_const | is_container
    }
    
    inline bool isVariable() {
        return (investigate == is_variable);
    }
    
    inline bool isCallVariable() {
        return (investigate == is_callvariable);
    }
    
    inline bool isDeclaration() {
        return (is_declaration == (investigate & is_declaration));
    }
    
    inline bool isConst() {
        return ( is_constante == (investigate & is_constante));
    }

    inline bool isNULL() {
        return (investigate == is_null);
    }
    
    inline bool isPredicate() {
        return (is_predicate == (investigate & is_predicate));
    }

    inline bool isPredicateMethod() {
        return (is_predicatemethod == (investigate & is_predicatemethod));
    }

    inline bool isFrameinstance() {
        return (is_frameinstance == (investigate & is_frameinstance));
    }

    inline bool isTracked() {
        return (investigate == is_tracked);
    }

    inline bool isIndex() {
        return (is_index == (investigate & is_noconst));
    }
    
    virtual bool isDirectIndex() {
        return false;
    }
    
    inline bool isPureString() {
        return (is_pure_string == (investigate & is_pure_string));
    }
    
    inline bool isString() {
        return ((is_string == (investigate & is_string)) ? true : (investigate == is_callvariable) ? isCallString() : false);
    }

    inline bool isNumber() {
        return ((is_number == (investigate & is_number)) ? true : (investigate == is_callvariable) ? isCallNumber() : false);
    }

    inline bool isJustString() {
        return (is_string == (investigate & is_string));
    }

    inline bool isJustNumber() {
        return (is_number == (investigate & is_number));
    }

    inline bool isRegular() {
        return (investigate == is_regular);
    }
    
    virtual bool isCallString() {
        return false;
    }
    
    virtual bool isCallNumber() {
        return false;
    }
    
    virtual bool isTerminal() {
        return false;
    }
    
    virtual TamguPredicate* checkTerminal(short n, short sz) {
        return NULL;
    }
    
    virtual void Indexing() {}

    //We try to minimize the use of locks in threads, as they might be quite detrimental to the speed of the interpreter.
    //An object might need to be locked when it is passed as an argument to a thread
    //Global variables are always locked, whenever a threads have been activated
    inline void Enablelock(unsigned short v) {
        investigate |= v;
    }
    
    inline unsigned short isToBelocked() {
        return (investigate & is_tobelocked);
    }

    inline bool hasLock() {
        return (is_tobelocked == (investigate & is_checkforlock));
    }

    inline bool hasalsoLock() {
        return (is_tobelocked == (investigate & is_tobelocked));
    }

    inline bool needFullInvestigate() {
        return (investigate >= is_continue);
    }

    inline bool needInvestigate() {
        return (investigate > is_continue);
    }
    
    inline bool isContainer() {
        return (is_container == (investigate & is_container));
    }

	inline bool isError() {
		return (investigate == is_error);
	}

    inline bool isReturned() {
        return (investigate == is_return);
    }
    
    inline void Releasenonconst() {
        if (!(investigate & is_constante))
            Release();
    }

    inline void Resetnonconst() {
        if (!(investigate & is_constante))
            Resetreference(1);
    }

	virtual bool baseValue() {
		return false;
	}

	virtual bool isConstant() {
		return false;
	}

	virtual bool Alreadydeclared() {
		return false;
	}

    virtual bool isGlobalVariable() {
        return false;
    }
    
	virtual bool isLetSelf() {
		return false;
	}

	virtual bool isConcept() {
		return false;
	}

	virtual bool isNegation() {
		return false;
	}

    virtual bool isContent() {
        return false;
    }
    
	virtual void Setnegation(bool n) {}
    virtual bool Setstopindex() {
        return false;
    }
    
	//-----------------------------------------------
	Exporting virtual Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting virtual Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    Exporting Tamgu* Filterreverse(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init);
	Exporting virtual Tamgu* Filterboolean(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* def);

	virtual void Addargmode() {}
	virtual void Addfunctionmode() {}

	virtual void Addfunction(Tamgu* a) {}
	virtual void Sharedeclaration(TamguFrame* k, bool final) {}
	virtual Tamgu* Getindex() {
		return NULL;
	}

	virtual Tamgu* Function() {
		return NULL;
	}

	virtual Tamgu* Body(short) {
		return this;
	}

	virtual Tamgu* Composition() {
		return aNOELEMENT;
	}

	//-----------------------------------------------
	virtual unsigned long EditDistance(Tamgu* e) {
		if (e->Type() != Type() || !same(e)->Boolean())
			return 1;
		return 0;
	}

	//Vector and Map operations...
	virtual Tamgu* Push(Tamgu*) {
		return this;
	}

	virtual Tamgu* push(Tamgu*) {
		return this;
	}
    
    virtual void pushatom(Tamgu*) {}

	Exporting virtual Tamgu* Pushinmap(Tamgu* a, short idhtread);

    Exporting virtual Tamgu* Push(Tamgu*, Tamgu*);
	Exporting virtual Tamgu* Push(BLONG k, Tamgu* a);
	Exporting virtual Tamgu* Push(long k, Tamgu* v);
	Exporting virtual Tamgu* Push(ushort k, Tamgu* v);
	Exporting virtual Tamgu* Push(double k, Tamgu* v);
	Exporting virtual Tamgu* Push(string k, Tamgu* v);

    virtual Tamgu* push(string k, Tamgu* v) {
        return this;
    }

    virtual void store(long k, Tamgu* v) {}

	Exporting virtual Tamgu* Push(wstring k, Tamgu* v);

	virtual Tamgu* Pop(Tamgu*) {
		return this;
	}

	virtual Tamgu* Vector(short idthread) {
		return aNULL;
	}

	virtual Tamgu* Map(short idthread) {
		return aNULL;
	}

	//--------------------------------------------------

	virtual Tamgu* Parameter(size_t i) {
		return aNULL;
	}

	virtual Tamgu* Argument(size_t i) {
		return aNULL;
	}

	//--------------------------------------------------
	virtual bool Checkarity() {
		return true;
	}

    virtual bool Checktype(short ty) {
        return false;
    }
    
    virtual unsigned long CallBackArity() {
        return P_NONE;
    }
    
	//--------------------------------------------------
	virtual Tamgu* getvalue(long i) {
		return aNOELEMENT;
	}

    Exporting virtual void addstringonly(string s) {}

	Exporting virtual void addustringto(wstring ws);
	Exporting virtual void addstringto(string s);
	Exporting virtual void addstringto(wchar_t s);

	Exporting virtual void addustringto(wstring ws, int i);
	Exporting virtual void addstringto(string s, int i);
	Exporting virtual void addstringto(wchar_t s, int i);


    Exporting virtual void Storevalue(string& u) {
        storevalue(u);
    }
    
    Exporting virtual void Storevalue(wstring& u) {
        storevalue(u);
    }

    Exporting virtual void storevalue(Tamgu* v, long beg, long end);
    Exporting virtual void storevalue(string u);
    Exporting virtual void storevalue(wstring u);
    Exporting virtual void storevalue(float u);
	Exporting virtual void storevalue(short u);
	Exporting virtual void storevalue(long u);
	Exporting virtual void storevalue(BLONG u);
	Exporting virtual void storevalue(double u);
	Exporting virtual void storevalue(unsigned char u);
	Exporting virtual void storevalue(wchar_t u);

	virtual Tamgu* Getvector(Tamgu* value, short idthread, bool skip = false) {
		return Eval(aNULL, aNULL, idthread);
	}

	virtual Tamgu* Getmap(Tamgu* value, short idthread, bool skip = false) {
		return Eval(aNULL, aNULL, idthread);
	}

    virtual long Getinteger0(short idthread) {
        return Getinteger(idthread);
    }
    
	virtual long Getinteger(short idthread) {
		Tamgu* value = Eval(aNULL, aNULL, idthread);
		long v = value->Integer();
		value->Releasenonconst();
		return v;
	}

	virtual BLONG Getlong(short idthread) {
		Tamgu* value = Eval(aNULL, aNULL, idthread);
		BLONG v = value->Long();
		value->Releasenonconst();
		return v;
	}

	virtual short Getshort(short idthread) {
		Tamgu* value = Eval(aNULL, aNULL, idthread);
		short v = value->Short();
		value->Releasenonconst();
		return v;
	}

	virtual float Getdecimal(short idthread) {
		Tamgu* value = Eval(aNULL, aNULL, idthread);
		float v = value->Decimal();
		value->Releasenonconst();
		return v;
	}

	virtual double Getfloat(short idthread) {
		Tamgu* value = Eval(aNULL, aNULL, idthread);
		double v = value->Float();
		value->Releasenonconst();
		return v;
	}

	virtual string Getstring(short idthread) {
		Tamgu* value = Eval(aNULL, aNULL, idthread);
        string v = value->String();
		value->Releasenonconst();
		return v;
	}

	virtual wstring Getustring(short idthread) {
		Tamgu* value = Eval(aNULL, aNULL, idthread);
        wstring v = value->UString();
		value->Releasenonconst();
		return v;
	}

	//----------------------------------------------------------------------

	virtual long getinteger(long i) {
		Tamgu* a = getvalue(i);
		long v = a->Integer();
		a->Release();
		return v;
	}

	virtual BLONG getlong(long i) {
		Tamgu* a = getvalue(i);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

    virtual uchar getbyte(long i) {
        Tamgu* a = getvalue(i);
        uchar v = a->Byte();
        a->Release();
        return v;
    }
    
	virtual short getshort(long i) {
		Tamgu* a = getvalue(i);
		short v = a->Short();
		a->Release();
		return v;
	}

	virtual float getdecimal(long i) {
		Tamgu* a = getvalue(i);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	virtual double getfloat(long i) {
		Tamgu* a = getvalue(i);
		double v = a->Float();
		a->Release();
		return v;
	}

	virtual string getstring(long i) {
		Tamgu* a = getvalue(i);
		string v = a->String();
		a->Release();
		return v;
	}

	virtual wstring getustring(long i) {
		Tamgu* a = getvalue(i);
		wstring v = a->UString();
		a->Release();
		return v;
	}

	//--------------------------------------------------

    //The as... methods are used in comparison to make the comparison strict.
    //For instance: i == null is always false.
    virtual string asString() { return String(); }
    virtual wstring asUString() { return UString(); }
    virtual long asInteger() { return Integer(); }
    virtual double asFloat() { return Float(); }
    virtual float asDecimal() { return Decimal(); }
    virtual BLONG asLong() { return Long(); }
    virtual short asShort() { return Short(); }
    virtual unsigned char asByte() {
        return Byte();
    }

    //--- Some important conversion methods...
    virtual bool Stringpredicatekey(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

    virtual bool Stringpredicatekeysecond(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

    virtual bool Stringpredicatekeythird(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

    virtual string displayString() { return String(); }
	virtual string String() { return ""; }
    virtual void Setstring(string& v, short idthread) { v = String(); }

	virtual string Bytes() {
		return String();
	}

	virtual wstring UString() {
		string s = String();
		wstring res;
		s_utf8_to_unicode(res, USTR(s), s.size());
		return res;
	}

    virtual void Setstring(wstring& v, short idthread) { v = Getustring(idthread); }
    
	virtual string StringToDisplay() {
		return String();
	}

	virtual string JSonString() {
		string value = String();
        string res;
        jstringing(res, value);
        return res;
	}

    int Int() {return (int)Integer();}
	virtual long Integer() { return 0; }
	virtual short Short() { return (short)Integer(); }
    virtual Tamgu* Complexe();
    
	virtual double Float() { return 0; }
	
	virtual float Decimal() {
		return (float)Float();
	}

	virtual bool Boolean() { return false; }
	virtual bool Protectedboolean() { 
		return Boolean();
	}

	virtual BLONG Long() { return 0; }
	virtual unsigned char Byte() {
		return (unsigned char)Integer();
	}

	virtual void Clear() {}
	virtual void Clean() {}
	virtual void Cleaning(short idthread) {}

	virtual long Size() { return 0; }

    virtual long CommonSize() {
        return Size();        
    }
    
	//Basic operations: we add the current value with "a"
	virtual Tamgu* andset(Tamgu* a, bool itself) {
		return this;
	}

	virtual Tamgu* orset(Tamgu* a, bool itself) {
		return this;
	}

	virtual Tamgu* xorset(Tamgu* a, bool itself) {
		return this;
	}

	virtual Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
		return aNULL;
	}

	Exporting virtual Tamgufraction* Fraction();
	Exporting virtual Tamgu* Invert(bool autoself);

	virtual Tamgu* Thesum(long i, long j) {
		return this;
	}

	virtual Tamgu* Theproduct(long i, long j) {
		return this;
	}

	virtual double Sum() {
		return 0;
	}

	virtual double Product() {
		return 0;
	}

	virtual Tamgu* plusplus() {
		return this;
	}

	virtual Tamgu* minusminus() {
		return this;
	}

	virtual Tamgu* plus(Tamgu* a, bool itself) {
		return this;
	}
	virtual Tamgu* minus(Tamgu* a, bool itself) {
		return this;
	}
	virtual Tamgu* multiply(Tamgu* a, bool itself) {
		return this;
	}
    
    virtual Tamgu* divideinteger(Tamgu* a, bool itself) {
        return this;
    }

	virtual Tamgu* divide(Tamgu* a, bool itself) {
		return this;
	}
    virtual Tamgu* mod(Tamgu* a, bool itself) {
        return this;
    }
	virtual Tamgu* power(Tamgu* a, bool itself) {
		return this;
	}
	virtual Tamgu* shiftleft(Tamgu* a, bool itself) {
		return this;
	}
	virtual Tamgu* shiftright(Tamgu* a, bool itself) {
		return this;
	}

	virtual Tamgu* less(Tamgu* a) {
		return aFALSE;
	}

	virtual Tamgu* more(Tamgu* a) {
		return aFALSE;
	}

	virtual Tamgu* same(Tamgu* a) {
		if (a == this)
			return aTRUE;
		return aFALSE;
	}

	virtual Tamgu* different(Tamgu* a) {
		if (same(a) == aFALSE)
			return aTRUE;
		return aFALSE;
	}

	virtual Tamgu* lessequal(Tamgu* a) {
		return aFALSE;
	}

	virtual Tamgu* moreequal(Tamgu* a) {
		return aFALSE;
	}

	virtual Tamgu* Compare(Tamgu* context, Tamgu* value, short idthread) {
		return same(value);
	}

	virtual bool Puretaskelldeclaration() {
		return false;
	}

    virtual void Setreturntype(short t) {}
	virtual short Returntype() {
		return a_none;
	}
};

//-------------------------------------------------------------------------
class TamguError : public Tamgu {
public:
	string error;

	TamguError(TamguGlobal* g, string e) : error(e) {
        investigate = is_error;
        if (g != NULL)
            g->RecordInTracker(this);
	}

	TamguError(string e) : error(e) {
        investigate = is_error;
		globalTamgu->RecordInTrackerProtected(this);
	}

	short Type() {
		return a_error;
	}

    string String() {
        return error;
    }

    wstring UString() {
        wstring err;
        s_utf8_to_unicode(err, USTR(error), error.size());
        return err;
    }

    void Setstring(string& v, short idthread) {
        v = error;
    }
    
    void Setstring(wstring& v, short idthread) {
        sc_utf8_to_unicode(v, USTR(error), error.size());
    }
    
    string Getstring(short idthread) {
        return error;
    }

    wstring Getustring(short idthread) {
        wstring err;
        s_utf8_to_unicode(err, USTR(error), error.size());
        return err;
    }
    
    Tamgu* Getvector(Tamgu* value, short idthread, bool skip = false) {
        return this;
    }

    Tamgu* Getmap(Tamgu* value, short idthread, bool skip = false) {
        return this;
    }

    long Getinteger(short idthread) {
        return -1;
    }

    BLONG Getlong(short idthread) {
        return -1;
    }

    short Getshort(short idthread) {
        return -1;
    }

    float Getdecimal(short idthread) {
        return -1;
    }

    double Getfloat(short idthread) {
        return -1;
    }
	void Resetreference(short inc) {}
	void Release() {}

    Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
        return this;
    }
    
    Tamgu* less(Tamgu* a) {
        return this;
    }
    
    Tamgu* more(Tamgu* a) {
        return this;
    }
    
    Tamgu* same(Tamgu* a) {
        return this;
    }
    
    Tamgu* different(Tamgu* a) {
        return this;
    }
    
    Tamgu* lessequal(Tamgu* a) {
        return this;
    }
    
    Tamgu* moreequal(Tamgu* a) {
        return this;
    }
    
};

class TamguFullError : public TamguError {
public:
    string filename;
    long posbinary;
    long poscharacter;
    long line;
    
    TamguFullError(string err, string& fn, long pb, long pc, long l) : filename(fn), posbinary(pb), poscharacter(pc), line(l), TamguError(NULL, err) {}
    
};


//-------------------------------------------------------------------------
//We need to declare the following other classes
//The TamguReference implements the reference counting mechanism...
class TamguReference : public Tamgu {
public:

    std::atomic<short> reference;
    bool protect;

    TamguReference(TamguGlobal* g, Tamgu* parent = NULL)  {
        //is_noneedforlock is a flag that indicates that the classes deriving from it do not need a lock
        //However, with the exception of TamguObject, which resets this value to zero, since it does introduce a lock
        investigate = is_noneedforlock;
        protect = true;
        reference = 0;
        if (g != NULL)
            g->RecordInTrackerProtected(this);
        if (parent != NULL)
            parent->AddInstruction(this);
    }

    TamguReference()  {
        investigate = is_noneedforlock;
        protect = true;
        reference = 0;
    }

    bool Checktype(short ty) {
        if (!reference && Type() == ty)
            return true;
        
        return false;
    }
    
    virtual short Reference() {
        return reference;
    }
    
    virtual void Setprotect(bool n) {
        protect = n;
    }
    
    virtual void Popping() {
        protect = false;
        if (reference <= 0)
            protect = true;
    }
    
    bool isProtected() {
        return protect;
    }
    
    void Resetreferencenoprotect(short r = 1) {
        Setprotect(false);
        Resetreference(r);
    }
    
    virtual void Addreference(unsigned short inv, short inc=1) {
        investigate |= (inv & is_tobelocked);
        Setreference(inc);
    }

    virtual Tamgu* Atomref() {
        Setreference();
        return this;
    }
    
    virtual void Setreference(short r) {
        reference += r;
        protect = false;
        
    }
    
    virtual void Setreference() {
        ++reference;
        protect = false;
        
    }

    virtual void Resetreference(short r = 1) {
        r = reference - r;
        if (r <= 0) {
            reference.store(0);
            if (!protect) {
                if (idtracker != -1)
                    globalTamgu->RemoveFromTracker(idtracker);            
                delete this;
            }
        }
        else
            reference.store(r);
    }

    virtual void Release() {
        if (reference == 0) {
            protect = false;
            Resetreference(0);
        }
    }
    
    virtual void Protect() {
            //We suppose there have been a Setreference before...
        if (reference >= 2) { //No problem, we simply remove one increment
            Resetreference();
            Setprotect(false); //Should not be any protect left in that case...
            return;
        }
        
            //Else, we decrease our reference, but we protect it with a protect
        Setprotect(true);
        Resetreference();
        Popping(); //and protection only if necessary...
    }

    bool isObject() {
        return true;
    }
};

//This class is used to handle objects, which can be either cleaned or deleted...
class TamguObject : public TamguReference {
public:
 	ThreadLock* _locker;

    TamguObject(TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent)  {
        _locker = new ThreadLock;
        //We need to clear the value for investigate set in TamguReference...
        investigate = is_none;
	}

	TamguObject()  {
        _locker = new ThreadLock;
        //We need to clear the value for investigate set in TamguReference since we now have a lock
        investigate = is_none;
	}

	~TamguObject() {
        delete _locker;
	}

	inline long Lockid() {
		return _locker->id;
	}
    
	inline std::recursive_mutex* Getlock() {
        return _locker->lock;
	}
    
    inline void locking() {
        if (globalTamgu->threadMODE && hasLock())
            _locker->lock->lock();
    }

    inline void unlocking() {
        if (globalTamgu->threadMODE && hasLock())
            _locker->lock->unlock();
    }

};

//--------------------------------------------------------------------
//to mark and unmark a path in self-contained containers...
class TamguCircular {
public:
    Tamgu* container;
    
    TamguCircular(Tamgu* c) {
        container=c;
        container->markloop(1);
    }
    
    ~TamguCircular() {
        container->markloop(0);
    }
};

//This version is for lockfree container...
class TamguContainer : public TamguReference {
public:
    bool affectation;
    
    TamguContainer(TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
        investigate |= is_container;
        affectation = false;
    }
    
    TamguContainer() {
        investigate |= is_container;
        affectation = false;
    }
    
    void Setaffectation(bool b) {
        affectation = b;
    }
    
    bool isAffectation() {
        return affectation;
    }
    
    bool checkAffectation() {
        if (affectation)
            return true;
        affectation = true;
        return false;
    }
    
};

class TamguObjectContainer : public TamguContainer {
public:
    
    std::atomic<short> containerreference;
    bool loopmark;
    unsigned char usermark;
    
    TamguObjectContainer(TamguGlobal* g, Tamgu* parent = NULL) : TamguContainer(g, parent) {
        loopmark=false;
        usermark=0;
        containerreference=0;
    }
    
    TamguObjectContainer() {
        loopmark=false;
        usermark=0;
        containerreference = 0;
    }
    
    uchar checkmark() {
        return usermark;
    }
    
    void mark(uchar m) {
        usermark=m;
    }
    
    void markloop(uchar m) {
        loopmark=m;
    }
    
    void Removecontainerreference(short inc) {
        containerreference-=inc;
    }
    
    void Removereference(short inc = 1) {
        containerreference-=inc;
        
        if (!containerreference && !reference)
            Resetreference(0);
    }
    
    short Reference() {
        return reference+containerreference;
    }
    
    void Addreference(unsigned short inv, short inc=1) {
        investigate |= (inv & is_tobelocked);
        protect=false;
        containerreference+=inc;
    }
    
    virtual void Release() {
        if ((reference+containerreference) == 0) {
            protect = false;
            Resetreference(0);
        }
    }
    
};

class TamguLockContainer : public TamguObject {
public:
    bool affectation;
    
    TamguLockContainer(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        investigate = is_container;
        affectation = false;
    }
    
    TamguLockContainer() {
        investigate = is_container;
        affectation = false;
    }
    
    void Setaffectation(bool b) {
        affectation = b;
    }
    
    bool isAffectation() {
        return affectation;
    }
    
    bool checkAffectation() {
        if (affectation)
            return true;
        affectation = true;
        return false;
    }
    
    virtual Tamgu* car(short idthread);    
    virtual Tamgu* cdr(short idthread);
    
    virtual Tamgu* divideinteger(Tamgu* a, bool itself) {
        return divide(a, itself);
    }
};

class TamguObjectLockContainer : public TamguLockContainer {
public:
    
    std::atomic<short> containerreference;
    std::atomic<bool> loopmark;
    std::atomic<unsigned char> usermark;
    
    TamguObjectLockContainer(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
        loopmark=false;
        usermark=0;
        containerreference=0;
    }
    
    TamguObjectLockContainer() {
        loopmark=false;
        usermark=0;
        containerreference = 0;
    }
    
    uchar checkmark() {
        return usermark;
    }
    
    void mark(uchar m) {
        usermark=m;
    }
    
    void markloop(uchar m) {
        loopmark=m;
    }
    
    inline bool lockingmark() {
        if (globalTamgu->threadMODE && hasLock()) {
            _locker->lock->lock();
            if (loopmark) {
                _locker->lock->unlock();
                return false;
            }
            loopmark = true;
            return true;
        }
        if (loopmark)
            return false;
        loopmark = true;
        return true;
    }

    inline void unlockingmark() {
        loopmark = false;
        if (globalTamgu->threadMODE && hasLock())
            _locker->lock->unlock();
    }

    void Removecontainerreference(short inc) {
        containerreference-=inc;
    }
    
    void Removereference(short inc = 1) {
        containerreference-=inc;
        
        if (!containerreference && !reference)
            Resetreference(0);
    }
    
    short Reference() {
        return reference+containerreference;
    }
    
    void Addreference(unsigned short inv, short inc=1) {
        investigate |= (inv & is_tobelocked);
        protect=false;
        containerreference+=inc;
    }
    
    virtual void Release() {
        if ((reference+containerreference) == 0) {
            protect = false;
            Resetreference(0);
        }
    }
};
//--------------------------------------------------------------------
// These classes are used to declared code, which can be tracked in TamguGlobal
class TamguTracked : public Tamgu {
public:
    long idinfo;
	short idtype;

    Exporting TamguTracked(short t, TamguGlobal* g = NULL, Tamgu* parent = NULL);
    Exporting TamguTracked(short t, TamguTracked* copie, TamguGlobal* g = NULL, Tamgu* parent = NULL);
	Exporting TamguTracked(string t, TamguGlobal* g = NULL, Tamgu* parent = NULL);

	void Remove() {
		globalTamgu->RemoveFromTracker(idtracker);
		delete this;
	}

	short Type() {
		return idtype;
	}

    long Currentinfo() {
        return idinfo;
    }
    
    //idinfo points to a slot in infocode that stores the file, space and line where the instruction is stored.
	long Currentline() {
        if (idinfo == -1)
            return -1;
        return globalTamgu->infocode[idinfo].line;
	}

	short Currentspace() {
        if (idinfo == -1)
            return -1;
        return globalTamgu->infocode[idinfo].idcode;
	}

	short Currentfile() {
        if (idinfo == -1)
            return -1;
        return globalTamgu->infocode[idinfo].idfile;
	}

	virtual short Typeinfered() {
		return a_none;
	}
    
	void Setinfo(Tamgu* ins) {
		if (globalTamgu->debugmode) {
			idinfo = ins->Currentinfo();
		}
	}

	void Getinfo(Tamgu* ins) {
        idinfo = ins->Currentinfo();
	}

};
//------------------------------------------------------------------------
class TamguIteration : public TamguObject {
private:
	Tamgu* prevkey;
	Tamgu* prevvalue;

public:
	bool reverse;

	TamguIteration(bool d, TamguGlobal* g = NULL) : reverse(d) {
		prevkey = prevvalue = NULL;
		if (globalTamgu != NULL)
			prevkey = prevvalue = aNULL;
	}

	void Update(Tamgu* n = aNULL) {
		prevkey = prevvalue = n;		
	}

	virtual short Type() {
		return a_iteration;
	}

	~TamguIteration() {
		prevkey->Resetreference();
		prevvalue->Resetreference();
	}

	Tamgu* IteratorKey() {
		Tamgu* v = Key();
		v->Setreference();
		prevkey->Resetreference();
		prevkey = v;
		return v;
	}

    Tamgu* IteratorValue() {
        Tamgu* v = Value();
        prevvalue->Resetreference();
        
        if (v->isProtected()) {
            v->Setreference();
            prevvalue = v;
        }
        else
            prevvalue = aNULL;
        
        return v;
    }
        
    virtual void Setvalue(Tamgu* recipient, short idthread) {
        Tamgu* v = Value();
        recipient->Setvalue(aNULL,v, idthread);
        v->Release();
    }
    
	virtual Tamgu* Key() {
		return aNULL;
	}

	virtual Tamgu* Value() {
		return aNULL;
	}

	virtual string Keystring() {
		return "";
	}

	virtual string Valuestring() {
		return "";
	}

	virtual wstring Keyustring() {
		string s = Keystring();
		wstring res;
		s_utf8_to_unicode(res, USTR(s), s.size());
		return res;
	}

	virtual wstring Valueustring() {
		string s = Valuestring();
		wstring res;
		s_utf8_to_unicode(res, USTR(s), s.size());
		return res;
	}

	virtual long Keyinteger() {
		return 0;
	}

	virtual short Keyshort() {
		return Key()->Short();
	}

	virtual BLONG Keylong() {
		return IteratorKey()->Long();
	}

	virtual long Valueinteger() {
		return 0;
	}

	virtual short Valueshort() {
		return IteratorValue()->Short();
	}
	
	virtual BLONG Valuelong() {
		return IteratorValue()->Long();
	}

	virtual double Keyfloat() {
		return 0;
	}

	virtual double Valuefloat() {
		return 0;
	}

	virtual float Valuedecimal() {
		return IteratorValue()->Decimal();
	}

	virtual void Next() {}

	virtual Tamgu* End() {
		return aTRUE;
	}

	virtual Tamgu* Begin() {
		return aFALSE;
	}

};

class TamguConstiteration : public TamguIteration {
public:

	TamguConstiteration(bool d, TamguGlobal* g) : TamguIteration(d, g) {
        if (g!=NULL)
            g->RecordInTracker(this);
	}

    void Addreference(unsigned short inv, short inc=1) {}
    void Setreference(short) {}
    void Setreference() {}
	void Resetreference(short r = 1) {}
	void Release() {}
	void Protect() {}
};

typedef Tamgu* (*javaMethod)(void*);
typedef Tamgu* (*javaMethodInput)(void*, long);
typedef Tamgu* (*javaMethodOutput)(void*, long, Tamgu*);

class TamguJavaIteration : public TamguIteration {
public:
    javaMethod java_begin;
    javaMethod java_next;
    javaMethod java_end;
    javaMethod java_iter;
    javaMethod java_value;
    javaMethod java_delete;
    javaMethod java_size;
    javaMethod java_clear;
    javaMethodInput java_get;
    javaMethodOutput java_set;

    void* java_iterator;
        
    TamguJavaIteration(void* jiv, javaMethod b, javaMethod n, javaMethod e, javaMethod iter, javaMethod v, javaMethod d, javaMethod sz, javaMethodInput g, javaMethodOutput o, javaMethod cl) :
    java_delete(d), java_begin(b), java_next(n), java_end(e), java_iter(iter), java_value(v), java_size(sz),
    java_get(g), java_set(o), java_clear(cl), TamguIteration(true) {
        java_iterator = jiv;
    }
        
    TamguIteration* Newiteration(bool direction) {
        return this;
    }
    
    short Type() {
        return a_iteration_java;
    }
    
    long Size() {
        Tamgu* sz = java_size(java_iterator);
        long s_z = sz->Integer();
        sz->Release();
        return s_z;
    }
    
    ~TamguJavaIteration() {
        java_delete(java_iterator);
    }

    Tamgu* getvalue(long i) {
        return java_get(java_iterator, i);
    }

    void storevalue(long i, Tamgu* v) {
        java_set(java_iterator, i, v);
    }
    
    Tamgu* Key() {
        return java_iter(java_iterator);
    }

    Tamgu* Value() {
        return java_value(java_iterator);
    }

    void Next() {
        java_next(java_iterator);
    }

    Tamgu* End() {
        return java_end(java_iterator);
    }
    
    void Clear() {
        java_clear(java_iterator);
    }

    Tamgu* Begin() {
        return java_begin(java_iterator);
    }

    string Keystring() {
        return IteratorKey()->String();
    }

    string Valuestring() {
        return IteratorValue()->String();
    }

    wstring Keyustring() {
        return IteratorKey()->UString();
    }

    wstring Valueustring() {
        return IteratorValue()->UString();
    }

    long Keyinteger() {
        return IteratorKey()->Integer();
    }

    short Keyshort() {
        return IteratorKey()->Short();
    }

    BLONG Keylong() {
        return IteratorKey()->Long();
    }

    long Valueinteger() {
        return IteratorValue()->Integer();
    }

    short Valueshort() {
        return IteratorValue()->Short();
    }
    
    BLONG Valuelong() {
        return IteratorValue()->Long();
    }

    double Keyfloat() {
        return IteratorKey()->Float();
    }

    double Valuefloat() {
        return IteratorValue()->Float();
    }

    float Valuedecimal() {
        return IteratorValue()->Decimal();
    }
    
};

//------------------------------------------------------------------------
//To record objects such as functions or frames...
class TamguDeclaration : public TamguTracked {
public:
	basebin_hash<Tamgu*> declarations;
	short name;

    TamguDeclaration(TamguGlobal* g) : name(-1), TamguTracked(a_declaration) {
        investigate = is_declaration;
    }

	TamguDeclaration(short t = a_declaration, TamguGlobal* g = NULL, Tamgu* parent = NULL) : name(-1), TamguTracked(t, g, parent) {
        investigate = is_declaration;
    }
    
	TamguDeclaration(short n, short t, TamguGlobal* g = NULL, Tamgu* parent = NULL) : name(n), TamguTracked(t, g, parent) {
        investigate = is_declaration;
    }

    bool FindAndClean(Tamgu* a) {
        basebin_hash<Tamgu*>::iterator it;
        for (it = declarations.begin(); it != declarations.end(); it++) {
            if (it->second == a) {
                return false;
            }
        }
        return true;
    }

	bool hasDeclaration() {
		return true;
	}

    void Replacedeclaration(short idthread, short id, Tamgu* a) {
        declarations[id] = a;
        globalTamgu->Replacevariable(idthread, id, a);
    }
    
    char Declarelocal(short idthread, short n, Tamgu* a) {
        if (isDeclared(n))
            return a_declaration;

        Declare(n, a);
        return true;
    }

	bool isEmpty() {
		if (declarations.base == -1)
			return true;
		return false;
	}

	short Name() {
		return name;
	}

	virtual bool isDeclared(short id) {
        return declarations.check(id);
	}

	virtual void Declare(short id, Tamgu* a) {
		declarations[id] = a;
	}

	virtual Tamgu* Declaration(short id) {
		if (declarations.check(id))
            return declarations.get(id);
        return NULL;
	}

	void Variables(vector<short>& vars) {
		basebin_hash<Tamgu*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++) {
			if (it->second->isFunction() || it->second->isFrame() || it->second->isVariable())
				continue;
			vars.push_back(it->first);
		}
	}

	void Cleaning(short idthread);
	virtual void Erase(short id) {
		declarations.erase(id);
	}
};

//--------------------------------------------------------------------

//To store a sequence of instructions
class TamguInstruction : public TamguTracked {
public:
	VECTE<Tamgu*> instructions;
	Tamgu* sibling;
    short returntype;
    
	short action;
	bool negation;
	bool variablesWillBeCreated;

	TamguInstruction(short t = a_instructions, TamguGlobal* g = NULL, Tamgu* parent = NULL) :
		variablesWillBeCreated(false), negation(false), action(a_none), sibling(NULL), returntype(a_null), TamguTracked(t, g, parent) {}

	TamguInstruction(TamguGlobal* g, Tamgu* parent = NULL) :
		variablesWillBeCreated(false), negation(false), action(a_none), sibling(NULL), TamguTracked(a_instructions, g, parent) {}


    void Setreturntype(short t) {
        returntype=t;
    }

    short Returntype() {
        if (returntype == a_null)
            return a_none;
        return returntype;
    }

	virtual void AddInstruction(Tamgu* a) {
		instructions.push_back(a);
	}

    void ScanVariables(vector<short>& vars) {
        for (long i=0; i<instructions.size();i++)
            instructions[i]->ScanVariables(vars);
        
    }
    
    string String() {
        string s = "[";
        s += globalTamgu->Getsymbol(idtype);
        s += "]";
        return s;
    }
    
    virtual string displayString() {
        string s = "[";
        s += globalTamgu->Getsymbol(action);
        for (long i = 0; i < instructions.size(); i++) {
            if (i)
                s += ",";
            s += instructions[i]->displayString();
        }
        s += "]";
        return s;
    }
    
	void SetVariablesWillBeCreated() {
		//This variable is set to true when a variable creation is present in the instruction vector...
		variablesWillBeCreated = true;
	}

    inline void update(short d, short idthread) {
        while (d--) instructions.vecteur[d]->update(idthread);
    }

    Tamgu* Parent() {
		return sibling;
	}

	bool isNegation() {
		return negation;
	}

	void Setnegation(bool n) {
		negation = n;
	}

	void InstructionClear() {
		instructions.clear();
	}

	void Putinstruction(size_t i, Tamgu* ke) {
		if (i < instructions.size())
			instructions.at(i, ke);
		else
			instructions.push_back(ke);
	}

	void Setaction(short a) {
		action = a;
	}

	Tamgu* InstructionRemoveLast() {
		if (instructions.size() == 0)
			return aNULL;
		Tamgu* ret = instructions[instructions.size() - 1];
		instructions.pop_back();
		return ret;
	}

	void InstructionRemove(size_t nb) {
		if (nb > instructions.size())
			nb = instructions.size();
		while (nb > 0) {
			instructions.pop_back();
			nb--;
		}
	}

	bool isInstruction() {
		return true;
	}

	Tamgu* Lastinstruction() {
		if (instructions.size() == 0)
			return aNULL;
		return instructions[instructions.size() - 1];
	}

	size_t InstructionSize() { return instructions.size(); }

	Tamgu* Instruction(size_t i) { return instructions[i]; }

	short Action() {
		return action;
	}

	virtual void Clone(TamguInstruction* ki) {
		ki->instructions = instructions;
		ki->negation = negation;
		ki->sibling = sibling;
        ki->returntype = returntype;
	}

	void Addparent(Tamgu* a) {
		sibling = a;
	}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);
};

class TamguInstructionList : public TamguInstruction {
public:
    TamguInstructionList(short t = a_instructions, TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguInstruction(t,g,parent) {}
    TamguInstructionList(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(g, parent) {}

    bool isObjectContainer() {
        return true;
    }
    
};

class TamguBeforeLast : public TamguInstruction {
public:

	TamguBeforeLast(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_bloc, g, parent) {
		action = a_bloc;
	}

	void AddInstruction(Tamgu* a) {
		if (instructions.last == 0)
			instructions.push_back(a);
		else
			instructions.insert(instructions.last - 1, a);
	}

};

class TamguSequence : public TamguInstruction {
public:
	basebin_hash<Tamgu*> declarations;

    TamguSequence(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_sequence, g, parent) {
        action = a_bloc;
    }

    TamguSequence(short a_code, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_code, g, parent) {
        action = a_bloc;
    }

    void Replacedeclaration(short idthread, short id, Tamgu* a) {
        declarations[id] = a;
        globalTamgu->Replacevariable(idthread, id, a);
    }

    char Declarelocal(short idthread, short n, Tamgu* a) {
        if (isDeclared(n))
            return a_declaration;

        Declare(n, a);
        return true;
    }

	bool hasDeclaration() {
		return true;
	}

	void Variables(vector<short>& vars) {
		basebin_hash<Tamgu*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++) {
			if (it->second->isFunction() || it->second->isFrame() || it->second->isVariable())
				continue;
			vars.push_back(it->first);
		}
	}

	bool isDeclared(short id) {
		if (declarations.check(id))
			return true;
		return false;
	}

	virtual void Declare(short id, Tamgu* a) {
		declarations[id] = a;
	}

	Tamgu* Declaration(short id) {
        if (declarations.check(id))
            return declarations.get(id);
        return NULL;
	}

	void Cleaning(short idthread) {
		bin_hash<Tamgu*>::iterator it;
		for (it = declarations.begin(); it != declarations.end(); it++)
			it->second->Resetreference(1);
	}

	bool isSequence() {
		return true;
	}
};

class TamguHBloc : public TamguSequence {
public:

	TamguHBloc(TamguGlobal* g, Tamgu* parent = NULL) : TamguSequence(g, parent) {
		action = a_bloc;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);
};
//--------------------------------------------------------------------
class TamguAlias : public TamguDeclaration {
public:
    VECTE<Tamgu*> instructions;
    VECTE<short> parameters;
    ThreadLock* _locker;
    bool assignable;

    TamguAlias(short n, TamguGlobal* global) : TamguDeclaration(n, a_lambda, global) {
        _locker = NULL;
        investigate = is_tracked;
        assignable = false;
    }

    ~TamguAlias() {
        if (_locker != NULL)
            delete _locker;
    }

    inline void lock_assignment() {
        if (globalTamgu->threadMODE && hasLock() && _locker)
            _locker->lock->lock();
    }

    inline void unlock_assignment() {
        if (globalTamgu->threadMODE && hasLock() && _locker)
            _locker->lock->unlock();
    }

    bool isAssignable() {
        return assignable;
    }

    size_t InstructionSize() {
        return instructions.last;
    }

    void AddInstruction(Tamgu* a) {
        instructions.push_back(a);
    }

    Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);

    bool isAlias() {
        return true;
    }

    bool isFunction() {
        return true;
    }
    
    long Size() {
        return (long)parameters.size();
    }

    bool isaFunction() {
        return true;
    }
};

//A User Function declaration.
class TamguFunction : public TamguDeclaration {
public:
	VECTE<Tamgu*> instructions;
	VECTE<Tamgu*> parameters;
	TamguFunction* next;

    long instruction_size;
	short returntype;

	char choice;
	bool autorun;
	bool privatefunction;
	bool strict;
	bool used;
	bool nonlimited;
	bool adding;

	TamguFunction(short n, TamguGlobal* global) : returntype(a_null), TamguDeclaration(n, a_function, global) {
        instruction_size = 0;
		adding = true;
		idtype = a_function;
		autorun = false;
		privatefunction = false;
		strict = false;
		used = false;
		nonlimited = false;
		choice = 0;
		next = NULL;
        investigate = is_tracked;
	}

	size_t InstructionSize() { 
		return instructions.last; 
	}

    Tamgu* Lispbody();

	short Returntype() {
		if (returntype == a_null)
			return a_none;
		return returntype;
	}

	short Typeinfered() {
		if (returntype == a_null)
			return a_none;
		return returntype;
	}

	bool isFrame() {
		return false;
	}

	virtual void AddInstruction(Tamgu* a) {
		if (choice == 0)
			parameters.push_back(a);
		else
            if (adding) {
                instructions.push_back(a);
                instruction_size++;
            }
	}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);
    Tamgu* Run(Tamgu* context, short idthread);

	bool isPrivate() {
		return privatefunction;
	}

	bool isFunction() {
		return true;
	}

	bool Predeclared() {
		if (choice == 2)
			return true;
		return false;
	}

	bool isUsed() {
		return used;
	}

	void Addfunction(Tamgu* a) {
		TamguFunction* l = this;
		while (l->next != NULL)
			l = l->next;
		l->next = (TamguFunction*)a;
	}

	bool Alreadydeclared() {
		if (choice == 1)
			return true;
		return false;
	}

	long Size() {
		return (long)parameters.size();
	}

	inline Tamgu* Parameter(size_t i) {
		return parameters[i];
	}

	Tamgu* Nextfunction() {
		return next;
	}

	void Setchoice(char v) {
		choice = v;
	}

	char Choice() {
		return choice;
	}
    
    bool isaFunction() {
        return true;
    }
    
    virtual TamguFunction* FindFunction(short ty) {
        return this;
    }
};

class TamguFunctionMap : public TamguFunction {
public:
    
    basebin_hash<TamguFunction*>* indexes;
    TamguFunctionMap(short n, TamguGlobal* global) : TamguFunction(n, global) {
        indexes = NULL;
    }

    ~TamguFunctionMap() {
        if (indexes != NULL)
            delete indexes;
    }
    
    TamguFunction* FindFunction(short ty) {
        if (indexes == NULL)
            return this;
        
        if (indexes->check(ty))
            return (*indexes)[ty];
    
        basebin_hash<TamguFunction*>::iterator it;
        for (it = indexes->begin(); it != indexes->end(); it++) {
            if (globalTamgu->Testcompatibility(it.first, ty, strict))
                return it.second;
        }
        return NULL;
    }

    void Indexing() {
        if (choice == 1) {
            if (next == NULL)
                return;
            
            short ty;
            TamguFunction* l = this;
            while (l != NULL) {
                if (l->Size()) {
                    ty = l->Parameter(0)->Typevariable();
                    if (indexes == NULL) {
                        indexes = new basebin_hash<TamguFunction*>;
                        (*indexes)[ty] = l;
                    }
                    else {
                        if (!indexes->check(ty))
                            (*indexes)[ty] = l;
                    }
                }
                l = l->next;
            }
        }
    }
};

class TamguThread : public TamguFunction {
public:
	ThreadLock* _locker;
	bool exclusive;
	bool joined;

	TamguThread(short n, TamguGlobal* global, bool j, char p) : joined(j), TamguFunction(n, global) {
		exclusive = false;
		_locker = NULL;
		if (p == 1)
			_locker = new ThreadLock;
		else
		if (p == 2)
			exclusive = true;
	}

	bool isJoined() {
		return joined;
	}

	bool isThread() {
		return true;
	}

	bool isExclusive() {
		return exclusive;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);
};

class TamguProtectedFunction : public TamguThread {
public:

	TamguProtectedFunction(short n, TamguGlobal* global, char protect) : TamguThread(n, global, false, protect) {}
};


class TamguFunctionCall : public TamguObject {
public:
	TamguFunction* body;
    bool clean;

    TamguFunctionCall(TamguGlobal* global = NULL) : TamguObject(global) {
		body = NULL;
        clean = false;
    }
    
    ~TamguFunctionCall() {
        if (clean)
            body->Resetreference();

    }
    
    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new TamguFunctionCall();
    }
    
	short Action() {
		return a_call;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread) {
		if (body == NULL)
			return globalTamgu->Returnerror(e_no_function_assigned, idthread);

		return body->Eval(context, callfunction, idthread);
	}

	Tamgu* Put(Tamgu* context, Tamgu* callfunction, short idthread) {
        if (callfunction->isFunction()) {
            if (body != NULL)
                body->Resetreference();
            
            body = (TamguFunction*)callfunction->Body(idthread);

            //Lisp functions are handled as object...
            body->Setreference();
            //In the case of a lambda, this might be the only place where it is stored...
            if (body->Name() == a_lambda)
                clean = true;
            return aTRUE;
		}

		return globalTamgu->Returnerror(e_expecting_a_function02, idthread);
	}

	bool duplicateForCall() {
		return false;
	}

	Tamgu* Body(short idthread) {
		return body;
	}

	bool isFunction() {
		return true;
	}

	long Size() {
		if (body != NULL)
			return body->Size();
		return -1;
	}

	Tamgu* Parameter(size_t i) {
		if (body == NULL)
			return aNOELEMENT;
		return body->parameters[i];
	}

};

//------------------------------------------------------------------------
//To record frame declarations...
class TamguFrame : public TamguDeclaration {
public:
	basebin_hash<Tamgu*> numbers;
    basebin_hash<short> names;

    VECTE<Tamgu*> instructions;
    VECTE<short> vnames;
	VECTE<Tamgu*> variables;
    
    TamguFrame* topframe;
    unsigned short investigating;
    
    short thetype;
    short theextensionvar;
    short minvar, maxvar;
    
    bool choosemin;
	bool privee;
	bool post;

	TamguFrame(short n, bool p, TamguGlobal* g = NULL, Tamgu* parent = NULL) : topframe(NULL),
		post(false), privee(p), TamguDeclaration(n, a_frame, g) {
            investigate = is_tracked;
            investigating = 0;
            thetype = n;
            minvar = 0;
            maxvar = 0;
            theextensionvar = 0;
            choosemin = true;
        }

	long Size() {
		return instructions.size();
	}

    short Typeframe() {
        return thetype;
    }
    
    void ScanVariables(vector<short>& vars) {
        for (long i=0; i<instructions.size();i++)
            instructions[i]->ScanVariables(vars);
        
    }

    bool Pushdeclaration(TamguFrame* k) {
        if (k->topframe != NULL)
            return false;
        
        basebin_hash<Tamgu*>::iterator it;
        for (it = declarations.begin(); it != declarations.end(); it++) {
            if (it->second->isFunction()) {
                if (!k->declarations.check(it->first))
                    k->declarations[it->first] = it->second;
            }
            else
                k->Declare(it->first, it->second);
        }
        k->topframe = this;
        return true;
    }
    
    void Sharedeclaration(TamguFrame* k, bool final) {
        basebin_hash<Tamgu*>::iterator it;
        //The first time, we copy the functions...
        if (final) {
            for (it = declarations.begin(); it != declarations.end(); it++) {
                if (it->second->isFunction()) {
                    if (!k->declarations.check(it->first))
                        k->declarations[it->first] = it->second;
                }
            }
            return;
        }
        
		//Then the variables...
        for (it = declarations.begin(); it != declarations.end(); it++) {
            if (it->second->isFunction())
                continue;
            k->declarations[it->first] = it->second;
        }
        
        k->investigating = investigating;
        k->theextensionvar = theextensionvar;
        k->variables = variables;
        k->names = names;
        k->vnames = vnames;
        k->numbers = numbers;
        k->post = post;
        k->topframe=this;
        k->minvar =  minvar;
        k->maxvar = maxvar;
        k->choosemin = choosemin;
    }

    void Storename(short idn, short n) {
        vnames.push_back(idn);
        names[idn] = n;
        if (!minvar) {
            minvar = idn;
            maxvar = idn;
        }
        else {
            if (minvar > idn)
                minvar = idn;
            if (maxvar < idn)
                maxvar = idn;
            //Up to 64, this is still interesting to handle frames as an array of 64 elements
            //In that case, the frame instances will be created with a basemin_hash...
            //see Tamguframeseeder::Newinstance for an explanation...
            if ((maxvar-minvar) >= binsize)
                choosemin = false;
        }
    }
    
	void Declare(short idn, Tamgu* a) {
        switch (idn) {
            case a_short:
            case a_int:
            case a_long:
            case a_decimal:
            case a_fraction:
            case a_complex:
            case a_float:
                if (numbers.empty()) {
                    numbers[a_short] = a;
                    numbers[a_int] = a;
                    numbers[a_long] = a;
                    numbers[a_decimal] = a;
                    numbers[a_float] = a;
                }
                else
                    numbers[idn] = a;
                investigating |= is_number;
                break;
            case a_string:
            case a_ustring:
                investigating |= is_string;
                break;
            case a_index:
            case a_interval:
                investigating |= is_container;
        }

		declarations[idn] = a;
		if (a->isVariable()) {
            Storename(idn, variables.size());
			variables.push_back(a);
			if (a->isFrame())
				post = true;
		}
	}
	
    TamguFrame* Topframe() {
        return topframe;
    }
    
    bool isParent(TamguFrame* fr) {
        TamguFrame* p=topframe;
        while (p!=NULL) {
            if (fr==p)
                return true;
            p=p->topframe;
        }
        return false;
    }
    
	void AddInstruction(Tamgu* a) {
		instructions.push_back(a);
	}

	Tamgu* Frame() {
		return this;
	}

	virtual bool isFrame() {
		return true;
	}

	virtual bool isDeclared(short id) {
		if (id == a_this)
			return true;

		return declarations.check(id);
	}

    bool isExtension() {
        return (topframe != NULL && topframe->idtype == a_extension);
    }

};

class TamguMainFrame : public TamguFrame {
public:

    basebin_hash<unsigned long> exported;
	ThreadLock _locker;

	TamguMainFrame(short n, bool p, TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguFrame(n, p, g, parent) {}

	Tamgu* Declaration(short id);
	void Declare(short id, Tamgu* a);
	bool isDeclared(short id);
	void cleaning(TamguGlobal* global) {
        basebin_hash<Tamgu*>::iterator it;
        for (it = declarations.begin(); it != declarations.end(); it++) {
            it->second->Deletion(global->elements_to_delete);
        }
	}
    
    char Declarelocal(short idthread, short n, Tamgu* a) {
        if (isDeclared(n))
            return a_mainframe;
        return false;
    }


    bool isMainFrame() {
        return true;
    }

	bool isFrame() {
		return false;
	}
};

//------------------------------------------------------------------------
class TamguArguments : public TamguTracked {
public:
	VECTE<Tamgu*> arguments;
	short name;

	TamguArguments(short t, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguTracked(t, global, parent) {}
	TamguArguments(short n, short t, TamguGlobal* global = NULL, Tamgu* parent = NULL) : name(n), TamguTracked(t, global, parent) {}

	short Name() {
		return name;
	}

	virtual void AddInstruction(Tamgu* a) {
		arguments.push_back(a);
	}

	bool isCall() {
		return true;
	}

	bool isCallFunction() {
		return true;
	}

	long Size() {
		return (long)arguments.size();
	}

	Tamgu* Argument(size_t i) {
		if (i >= arguments.size())
			return aNOELEMENT;
		return arguments[i];
	}
};

//The basic Call, with its arguments...
class TamguCall : public TamguTracked {
public:
	VECTE<Tamgu*> arguments;
	Tamgu* function;
	short name;
	bool addarg;
	bool negation;
    bool curryfied;

	TamguCall(short t, TamguGlobal* global = NULL, Tamgu* parent = NULL) : name(-1), function(NULL), addarg(true), negation(false), curryfied(false), TamguTracked(t, global, parent) {}
	TamguCall(short n, short t, TamguGlobal* global = NULL, Tamgu* parent = NULL) : function(NULL), addarg(true), name(n), negation(false), curryfied(false), TamguTracked(t, global, parent) {}

	short Name() {
		return name;
	}
    
    bool Computevariablecheck(short idthread) {
        vector<short> vars;
        ScanVariables(vars);

        for (long i = 0; i < vars.size(); i++) {
            if (!globalTamgu->threads[idthread].variables.check(vars[i]))
                return false;
            
            VECTE<Tamgu*>& e = globalTamgu->threads[idthread].variables.get(vars[i]);
            if (e.size() == 0)
                return false;
        }
        return true;
    }
    
	bool isNegation() {
		return negation;
	}

	void Setnegation(bool n) {
		negation = n;
	}

    void ScanVariables(vector<short>& vars) {
        for (long i=0; i<arguments.size();i++)
            arguments[i]->ScanVariables(vars);
    }

	void CheckTaskellComposition() {
		Tamgu* a;
		for (short i = 0; i < arguments.size(); i++) {
			a = arguments[i]->Composition();
			if (a != aNOELEMENT) {
				arguments[i]->Remove();
				arguments.vecteur[i] = a;
			}
		}
	}

    virtual string String() {
        string v = "[";
        v += globalTamgu->Getsymbol(name);
        v+="(";
        for (long i = 0; i < arguments.size(); i++) {
            if (i)
                v+=",";
            if (globalTamgu->short_string) {
                string vsub = arguments[i]->String();
                if (vsub.size() < globalTamgu->short_string)
                    v += vsub;
                else
                    v += vsub.substr(0, globalTamgu->short_string) + "..";
            }
            else
                v+=arguments[i]->String();
        }
        v+=")]";
        return v;
    }
    
    void Setstring(string& v, short idthread) {
        v = "[";
        v += globalTamgu->Getsymbol(name);
        v+="(";
        for (long i = 0; i < arguments.size(); i++) {
            if (i)
                v+=",";
            v+=arguments[i]->String();
        }
        v+=")]";
    }
    
    virtual void AddInstruction(Tamgu* a) {
		if (addarg)
			arguments.push_back(a);
		else
			function = a;
	}

	bool isCall() {
		return true;
	}

	bool isCallFunction() {
		return true;
	}

    virtual bool Setstopindex() {
        if (function !=NULL)
            function->Setstopindex();
        return false;
    }

    bool isDirectIndex() {
        return (function != NULL && function->isIndex() && !function->Function());
    }

	Tamgu* Getindex() {
		if (function != NULL)
			return function->Getindex();
		return NULL;
	}

	void Addargmode() {
		addarg = true;
	}

	void Addfunctionmode() {
		addarg = false;
	}

	long Size() {
		return (long)arguments.size();
	}

	Tamgu* Argument(size_t i) {
		if (i >= arguments.size())
			return aNOELEMENT;
		return arguments[i];
	}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread) {
		return this;
	}

	virtual Tamgu* Evaluate(long i, Tamgu* context, short idthread) {
		return arguments[i]->Eval(context, aNULL, idthread);
	}
    
    virtual Tamgu* Evaluatedefault(long i, Tamgu* context, Tamgu* default_value, short idthread) {
        if (i < arguments.size())
            return arguments[i]->Eval(context, aNULL, idthread);
        return default_value;
    }
};

class TamguCallClean : public TamguCall {
public:
	VECTE<Tamgu*> cleaning;
    long i;

	TamguCallClean(short t, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCall(t, global, parent) {}
	TamguCallClean(short n, short t, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCall(n, t, global, parent) {}

	//We use Evaluate to keep a track of the arguments computed out of our list
	Tamgu* Evaluate(long ipar, Tamgu* context, short idthread) {
		context = arguments[ipar]->Eval(context, aNULL, idthread);
        if (!context->isConst())
            cleaning.push_back(context);
		return context;
	}

    Tamgu* Evaluatedefault(long ipar, Tamgu* context, Tamgu* default_value, short idthread) {
        if (ipar < arguments.size()) {
            context = arguments[ipar]->Eval(context, aNULL, idthread);
            if (!context->isConst())
                cleaning.push_back(context);
            return context;
        }
        return default_value;
    }

	//This function will automatically clean all these arguments...
	inline void Releasing(Tamgu* returnobject) {
        if (!cleaning.last)
            return;
        
        if (returnobject->isConst()) {
            for (i = 0; i < cleaning.last; i++)
                cleaning.vecteur[i]->Release();
            cleaning.clear();
            return;
        }
        
        returnobject->Setreference();
		for (i = 0; i < cleaning.last; i++) {
			if (cleaning.vecteur[i] != returnobject)
				cleaning.vecteur[i]->Release();
		}
		cleaning.clear();
        returnobject->Protect();
	}

	void Cleaning(short idthread) {
		for (i = 0; i < cleaning.last; i++)
			cleaning.vecteur[i]->Release();
		cleaning.clear();
	}

	short Returntype() {
		return Typeinfered();
	}

};

//------------------------------------------------------------------------
class TamguShape : public Tamgu {
public:
    vector<Tamgu*> instructions;
    Tamgu* function;
    char stop;
    
    TamguShape(TamguGlobal* global = NULL, Tamgu* parent = NULL) {
        function = NULL;
        stop = false;
        if (parent != NULL)
            parent->AddInstruction(this);
        
        if (global != NULL)
            global->RecordInTracker(this);
        
        investigate = is_index;
    }
    
    void AddInstruction(Tamgu* a) {
        if (stop == 2) {
            function = a;
            stop = false;
        }
        else
            instructions.push_back(a);
    }

    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* Put(Tamgu* context, Tamgu* value, short idthread);

    void ScanVariables(vector<short>& vars) {
        for (long i = 0; i < instructions.size(); i++) {
            if (!instructions[i]->isConst())
                instructions[i]->ScanVariables(vars);
        }
    }

    short Type() {
        return a_callindex;
    }

    short Typeinfered() {
        return a_none;
    }

    bool isCall() {
        return true;
    }

    Tamgu* Function() {
        return function;
    }

    bool isStop() {
        return true;
    }
    
    Tamgu* Getindex() {
        if (function == NULL)
            return this;
        return function->Getindex();
    }
    
    bool Setstopindex() {
        if (function == NULL) {
            stop = true;
           return true;
        }
        stop = function->Setstopindex();
        return true;
    }
    
    bool isDirectIndex() {
        return (function != NULL && function->isIndex() && !function->Function());
    }

    long Size() {
        return instructions.size();
    }
    
    Tamgu* Parameter(size_t i) {
        return instructions[i];
    }

};

class TamguIndex : public Tamgu {
public:
	Tamgu* left;
	Tamgu* right;
	Tamgu* function;
	bool interval, signleft, signright, stop, minimal;

    TamguIndex(bool itv, TamguGlobal* global = NULL, Tamgu* parent = NULL) :
    interval(itv), left(aNOELEMENT),
    signleft(false), signright(false), stop(false),
    function(NULL), right(NULL) {
        minimal = false;
        if (parent != NULL)
            parent->AddInstruction(this);
        
        if (global != NULL)
            global->RecordInTracker(this);
        
        investigate = is_index;
    }
    
    TamguIndex(TamguIndex* idx, short idthread) {
        investigate = is_index;
        interval = idx->interval;
        minimal = idx->minimal;
        signleft = idx->signleft;
        signright = idx->signright;
        left = idx->left;
        right = idx->right;
        
        if (!left->isConst())
            left = left->Eval(aNULL, aNULL, idthread);
        
        if (right != NULL && !right->isConst())
            right = right->Eval(aNULL, aNULL, idthread);
    }

    TamguIndex(Tamgu* l, Tamgu* f) : interval(false), left(l), signleft(false), signright(false), right(NULL) {
        minimal = true;
        function = f;
        stop = function->Setstopindex();
        investigate = is_index | is_constante;
    }

    TamguIndex(Tamgu* l, Tamgu* r, Tamgu* f) : interval(true), left(l),signleft(false), signright(false),right(r) {
        function = f;
        stop = function->Setstopindex();
        investigate = is_index | is_constante;
    }
    
    void Setaffectation(bool b) {
        if (function != NULL)
            function->Setaffectation(b);
    }
    
    bool Checklegit() {
        if (left->isFunctionParameter() || left->isFunction() || (right != NULL && (right->isFunctionParameter() || right->isFunction())))
            return false;
        return true;
    }
    
    void Checkconst() {
        if (left->isConst() && (right == NULL || right->isConst()))
            investigate |= is_constante;
    }
    
    void ScanVariables(vector<short>& vars) {
        if (!left->isConst())
            left->ScanVariables(vars);
        if (right != NULL && !right->isConst())
            right->ScanVariables(vars);
        if (function != NULL)
            function->ScanVariables(vars);
    }

	short Type() {
		return a_callindex;
	}

	short Typeinfered() {
		return a_none;
	}
    
	char Indexes(short idthread, long sz, long& ileft, long& iright) {
		Tamgu* keyleft = left->Eval(aNULL, aNULL, idthread);
		Tamgu* keyright = NULL;
		if (interval == true)
			keyright = right->Eval(aNULL, aNULL, idthread);

		ileft = keyleft->Integer();
		keyleft->Release();
		if (keyright != NULL) {
			iright = keyright->Integer();
            if (keyright != right)
                keyright->Release();
		}

		if (ileft < 0)
			ileft += sz;

		if (ileft < 0 || ileft >= sz)
			return 0;

		if (!interval)
			return 1;

		if (iright <= 0 || keyright == aNULL) {
			iright += sz;
			if (iright<ileft)
				return 0;
		}
		else {
			if (iright>sz)
				return 0;
		}
		return 2;
	}

    TamguIndex* Evaluate(short idthread) {
        if (isConst())
            return this;
        
        return new TamguIndex(this, idthread);
    }
    
    void Rollback() {
        if (isConst())
            return;
        
        left->Releasenonconst();
        if (right != NULL)
            right->Releasenonconst();
        delete this;
    }
    
    void Clear() {
        left->Releasenonconst();
        if (right != NULL)
            right->Releasenonconst();
        if (idtracker != -1)
            globalTamgu->RemoveFromTracker(idtracker);
        if (function != NULL && function->isIndex())
            function->Clear();
        delete this;
    }

    void Set(TamguIndex* idx, short idthread)  {
        interval = idx->interval;
        signleft = idx->signleft;
        signright = idx->signright;
        left = idx->left;
        right = idx->right;
        
        if (!left->isConst())
            left = left->Eval(aNULL, aNULL, idthread);
        
        if (right != NULL && !right->isConst())
            right = right->Eval(aNULL, aNULL, idthread);
    }

	void AddInstruction(Tamgu* a) {
		if (left == aNOELEMENT)
			left = a;
		else if (interval && right == NULL)
			right = a;
		else
		if (function == NULL)
			function = a;
		else
			function->AddInstruction(a);
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* Put(Tamgu* context, Tamgu* value, short idthread);

	Tamgu* Function() {
		return function;
	}

	bool isInterval() {
		return interval;
	}

    bool isStop() {
        return stop;
    }
    
    Tamgu* Getindex() {
        if (stop)
            return this;
        return function->Getindex();
    }
    
    bool Setstopindex() {
        if (function==NULL) {
            stop=true;
           return true;
        }
        
        stop=function->Setstopindex();
        return stop;
    }
    
    bool isDirectIndex() {
        return (function != NULL && function->isIndex() && !function->Function());
    }

	bool isCall() {
		return true;
	}

    string String() {
        return left->String();
    }

    wstring UString() {
        return left->UString();
    }

    void Setstring(string& v, short idthread) {
        left->Setstring(v, idthread);
    }

    void Setstring(wstring& v, short idthread) {
        left->Setstring(v, idthread);
    }

	virtual string Bytes() {
		return left->Bytes();
	}

	string StringToDisplay() {
		return left->StringToDisplay();
	}

	string JSonString() {
		return left->JSonString();
	}

    long Getinteger0(short idthread) {
        return left->Getinteger0(idthread);
    }

	long Getinteger(short idthread) {
		return left->Getinteger(idthread);
	}

	float Getdecimal(short idthread) {
		return left->Getdecimal(idthread);
	}

	short Getshort(short idthread) {
		return left->Getshort(idthread);
	}

	double Getfloat(short idthread) {
		return left->Getfloat(idthread);
	}

	BLONG Getlong(short idthread) {
		return left->Getlong(idthread);
	}

	string Getstring(short idthread) {
		return left->Getstring(idthread);
	}

	wstring Getustring(short idthread) {
		return left->Getustring(idthread);
	}

	long Integer() {
		return left->Integer();
	}

	short Short() {
		return left->Short();
	}

	double Float() {
		return left->Float();
	}

	bool Boolean() {
		return left->Boolean();
	}

	BLONG Long() {
		return left->Long();
	}

	unsigned char Byte() {
		return left->Byte();
	}

    long Size() {
        return (1 + interval);
    }
    
    Tamgu* Parameter(size_t i) {
        if (!i)
            return left;
        return right;
    }
};


//------------------------------------------------------------------------
class Locking {
private:
	std::recursive_mutex* g;

public:
	Locking(TamguGlobal* global) {
        if (global->threadMODE) {
			g = globalTamgu->_trackerlock.lock;
            g->lock();
        }
        else
            g = NULL;
	}

	Locking(Tamgu* d) {
        g = NULL;
        if (globalTamgu->threadMODE && d->hasLock()) {
            g = ((TamguObject*)d)->Getlock();
            g->lock();
        }
	}

	Locking(ThreadLock& d) {
        if (globalTamgu->threadMODE) {
			g = d.lock;
            g->lock();
        }
        else
            g = NULL;
	}
    
	~Locking() {
		if (g != NULL)
			g->unlock();
	}
};

//We impose a global order in which locks are created to reduce deadlock risks...
class Doublelocking {
public:
	std::recursive_mutex* gfirst;
	std::recursive_mutex* glast;

	Doublelocking(TamguObject* d, Tamgu* o) {
        gfirst = NULL;
        glast = NULL;
        if (globalTamgu->threadMODE) {
            if (d->hasLock()) {
                if (o->hasLock()) {
                    TamguObject* dd = (TamguObject*)o;
                    if (d->Lockid() < dd->Lockid()) {
                        gfirst = d->Getlock();
                        glast = dd->Getlock();
                    }
                    else {
                        gfirst = dd->Getlock();
                        glast = d->Getlock();
                    }
                    gfirst->lock();
                    glast->lock();
                }
                else {
                    gfirst = d->Getlock();
                    gfirst->lock();
                }
            }
        }
    }

	~Doublelocking() {
		if (gfirst != NULL)
			gfirst->unlock();
		if (glast != NULL)
			glast->unlock();
	}

};

//------------------------------------------------------------------------
//The difference between let and self, is that in a let the first value defines once for all the type of the variable...
//let i=0; i is then an integer, unless you use the operator :=
// i := "tutu"; becomes a string...
//In a self, the type does not depend on the first assignment.

class TamguLet : public TamguObject {
public:
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	Tamgu* value;

	//---------------------------------------------------------------------------------------------------------------------

	TamguLet(TamguGlobal* g) : TamguObject(g) {
		value = aNOELEMENT;
	}

	TamguLet() {
		//Do not forget your variable initialisation
		value = aNOELEMENT;
	}

	bool isLetSelf() {
		return true;
	}
    
    void Addreference(unsigned short inv, short r=1) {
        investigate |= (inv & is_tobelocked);
        reference += r;
        protect = false;
        value->Addreference(investigate, r);
    }
    
    void Setreference(short r) {
        value->Setreference(r);
        reference += r;
        protect = false;
    }
    
    void Setreference() {
        value->Setreference();
        ++reference;
        protect = false;
    }
    
    void Release() {
        if (protect == true && reference == 0) {
            {
                locking();
                value->Setprotect(false);
                unlocking();
                protect = false;
            }
            Resetreference(1);
        }
    }

    void Resetreference(short r = 1) {
        {
            locking();
            value->Resetreference(r);
            unlocking();
        }
        TamguObject::Resetreference(r);
    }

    
    Tamgu* GetLetValue() {
        return value;
    }
    
	void Setprotect(bool n) {
        value->Setprotect(n);
		protect = n;
	}

	void Popping() {
		{
			locking();
			value->Popping();
            unlocking();
		}

		protect = false;
		if (reference <= 0)
			protect = true;
	}

    Tamgu* Declaration(short id) {
        if (value == aNOELEMENT)
            return aNOELEMENT;
        return value->Declaration(id);
    }

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Newvalue(Tamgu* a, short idthread) {
		if (value != aNOELEMENT)
			return value->Newvalue(a, idthread);
		return a->Atom();
	}
	//----------------------------------------------------------------------------------------------------------------------
    virtual void Storevalue(string& u);
    virtual void Storevalue(wstring& u);
    
    virtual void storevalue(string u);
	virtual void storevalue(float u);
	virtual void storevalue(short u);
	virtual void storevalue(wstring u);
	virtual void storevalue(long u);
	virtual void storevalue(BLONG u);
	virtual void storevalue(double u);
	virtual void storevalue(unsigned char u);
	virtual void storevalue(wchar_t u);
	//----------------------------------------------------------------------------------------------------------------------
	virtual Tamgu* Put(Tamgu* idx, Tamgu* v, short idthread) {
		locking();
        if (value == v) {
            unlocking();
			return aTRUE;
        }

        if (!value->isConst()) {
            value->Put(idx, v, idthread);
            unlocking();
            return aTRUE;
        }
        else {
            if (idx->isIndex()) {
                unlocking();
                return globalTamgu->Returnerror("LET(001): Undefined assignment", idthread);
            }
        }

        if (v->isConst()) {
            if (v == aNOELEMENT) {
                value = aNOELEMENT;
                unlocking();
                return aTRUE;
            }
			value = v->Atom();
            value->Setreference(reference);
            value->Enablelock(isToBelocked());
            unlocking();
            return aTRUE;
        }
        
        value = v->GetLetValue();
		value->Setreference(reference);
        value->Enablelock(isToBelocked());
        unlocking();
		return aTRUE;
	}

	virtual bool Setvalue(Tamgu* index, Tamgu* v, short idthread, bool strict = false) {
        locking();
        if (value == v) {
            unlocking();
			return true;
        }

		if (index->isIndex()) {
            unlocking();
			globalTamgu->Returnerror("LET(001): Undefined assignment", idthread);
			return false;
		}

        if (!value->isConst())
            value->Resetreference(reference);
        
        if (v != aNOELEMENT && v->isConst()) {
            value = v->Atom();
            value->Setreference(reference);
            value->Enablelock(isToBelocked());
            unlocking();
            return aTRUE;
        }
        
        value = v->GetLetValue();
		value->Setreference(reference);
        value->Enablelock(isToBelocked());
        unlocking();
		return true;
	}

    virtual Tamgu* Clonevalue(Tamgu* v, short idthread) {
        locking();
        if (value == v) {
            unlocking();
            return v;
        }

        if (!value->isConst())
            value->Resetreference(reference);

        if (v->isConst())
            v = v->Atom();

        value = v->GetLetValue();
        value->Setreference(reference);
        value->Enablelock(isToBelocked());
        unlocking();
        return value;
    }

    virtual Tamgu* Putvalue(Tamgu* v, short idthread) {
        locking();
        if (value == v) {
            unlocking();
            return v;
        }

        if (!value->isConst())
            value->Resetreference(reference);

        if (v->isConst())
            v = v->Atom();

        value = v->GetLetValue();
        value->Setreference(reference);
        value->Enablelock(isToBelocked());
        unlocking();
        return value;
    }

    void Clean() {
        value =  aNOELEMENT;
    }
    
	virtual void Forcedclean() {
        locking();
		value->Resetreference(reference);
		value = aNOELEMENT;
        unlocking();
	}

	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread) {
		locking();
		v = value->Eval(context, v, idthread);
        unlocking();
        return v;
	}

	virtual short Typevariable() {
        if (value == aNOELEMENT)
            return a_let;
        return value->Typevariable();
	}

	virtual short Type() {
		if (value == aNOELEMENT)
			return a_let;
		return value->Type();
	}

	virtual short Typeinfered() {
		return a_let;
	}

	virtual string Typename() {
        if (value == aNOELEMENT)
            return "let";
        return value->Typename();
	}

	Tamgu* Atom(bool forced = false) {
		return value->Atom(forced);
	}

	bool isFrame() {
		return value->isFrame();
	}
	
	Tamgu* Frame() {
		return value->Frame();
	}

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return value->duplicateForCall();
	}

	Tamgu* Value() {
		return value;
	}

	bool isFunction() {
		return value->isFunction();
	}

	Tamgu* Body(short idthread) {
		return value->Body(idthread);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below

	virtual Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new TamguLet;
	}

	void Clear() {
		//To set a variable back to empty
		value->Clear();
	}


	string String() {
        locking();
        string s = value->String();
        unlocking();
		return s;
	}

	wstring UString() {
        locking();
        wstring s = value->UString();
        unlocking();
        return s;
	}

    void Setstring(string& v, short idthread) {
        locking();
        value->Setstring(v, idthread);
        unlocking();
    }

    void Setstring(wstring& v, short idthread) {
        locking();
        value->Setstring(v, idthread);
        unlocking();
    }

    long Getinteger(short idthread) {
        return value->Getinteger(idthread);
    }

    BLONG Getlong(short idthread) {
        return value->Getlong(idthread);
    }

    short Getshort(short idthread) {
        return value->Getshort(idthread);
    }

    float Getdecimal(short idthread) {
        return value->Getdecimal(idthread);
    }

    double Getfloat(short idthread) {
        return value->Getfloat(idthread);
    }

    string Getstring(short idthread) {
        return value->Getstring(idthread);
    }

    wstring Getustring(short idthread) {
        return value->Getustring(idthread);
    }

	long Integer() {
		locking();
		long v = value->Integer();
        unlocking();
        return v;
	}
	double Float() {
        locking();
        double v = value->Float();
        unlocking();
        return v;
	}
	BLONG Long() {
        locking();
        BLONG v = value->Long();
        unlocking();
        return v;
	}

	bool Boolean() {
        locking();
        bool v = value->Boolean();
        unlocking();
        return v;
	}

    string StringToDisplay() {
        locking();
        string v = value->StringToDisplay();
        unlocking();
        return v;
    }

	string JSonString() {
        locking();
        string v = value->JSonString();
        unlocking();
        return v;
	}

	string Bytes() {
        locking();
        string v = value->Bytes();
        unlocking();
        return v;
	}


	//Basic operations
	long Size() {
        locking();
        long v = value->Size();
        unlocking();
        return v;
	}

    Tamgu* Merging(Tamgu* a) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->Merging(a);
        unlocking();
        return a;
    }
    
    Tamgu* Combine(Tamgu* a) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->Combine(a);
        unlocking();
        return a;
    }
    
	Tamgu* andset(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->andset(a, itself);
        unlocking();
        return a;
	}

	Tamgu* orset(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->orset(a, itself);
        unlocking();
        return a;
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->xorset(a, itself);
        unlocking();
        return a;
	}

	Tamgu* plus(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->plus(a, itself);
        unlocking();
        return a;
	}

	Tamgu* minus(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->minus(a, itself);
        unlocking();
        return a;
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->multiply(a, itself);
        unlocking();
        return a;
	}

    Tamgu* divide(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->divide(a, itself);
        unlocking();
        return a;
    }
    
    Tamgu* divideinteger(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->divideinteger(a, itself);
        unlocking();
        return a;
    }
    
	Tamgu* power(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->power(a, itself);
        unlocking();
        return a;
	}
	Tamgu* shiftleft(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->shiftleft(a, itself);
        unlocking();
        return a;
	}
	Tamgu* shiftright(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->shiftright(a, itself);
        unlocking();
        return a;
	}
	Tamgu* mod(Tamgu* a, bool itself) {
        locking();
        if (value == aNOELEMENT) {
            unlocking();
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->mod(a, itself);
        unlocking();
        return a;
	}

	Tamgu* less(Tamgu* a) {
        locking();
        a = value->less(a);
        unlocking();
        return a;
	}

	Tamgu* more(Tamgu* a) {
        locking();
        a = value->more(a);
        unlocking();
        return a;
	}

	Tamgu* same(Tamgu* a) {
        locking();
        a = value->same(a);
        unlocking();
        return a;
	}

	Tamgu* different(Tamgu* a) {
        locking();
        a = value->different(a);
        unlocking();
        return a;
	}

	Tamgu* lessequal(Tamgu* a) {
        locking();
        a = value->lessequal(a);
        unlocking();
        return a;
	}

	Tamgu* moreequal(Tamgu* a) {
        locking();
        a = value->moreequal(a);
        unlocking();
        return a;
	}

};

//In a 'self' we clear the value before setting it back...
class TamguSelf : public TamguLet {
public:
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	//---------------------------------------------------------------------------------------------------------------------

    short typevalue;
    
	TamguSelf(TamguGlobal* g) : TamguLet(g) {
        typevalue = a_const;
    }

	TamguSelf() {
        typevalue = a_const;
    }

	//------------------------------------------------------------------------
	//--------------------------------------------------------------------
    void Storevalue(string& u);
    void Storevalue(wstring& u);

    void storevalue(string u);
	void storevalue(float u);
	void storevalue(short u);
	void storevalue(wstring u);
	void storevalue(long u);
	void storevalue(BLONG u);
	void storevalue(double u);
	void storevalue(unsigned char u);
	void storevalue(wchar_t u);
    
	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* idx, Tamgu* v, short idthread) {
        locking();
        if (value == v) {
            unlocking();
			return aTRUE;
        }

		if (!value->isConst()) {
            if (idx->isIndex()) {
                unlocking();
				return value->Put(idx, v, idthread);
            }

			if (typevalue == v->Type() && v->duplicateForCall()) {
				value->Put(idx, v, idthread);
                unlocking();
				return aTRUE;
			}

			value->Resetreference(reference);
		}
        else {
            if (idx->isIndex()) {
                unlocking();
                return globalTamgu->Returnerror("LET(001): Undefined assignment", idthread);
            }
        }

        if (v->isConst()) {
            if (v == aNOELEMENT) {
                value = aNOELEMENT;
                typevalue = a_const;
                unlocking();
                return aTRUE;
            }
            value = v->Atom();
            value->Setreference(reference);
            value->Enablelock(isToBelocked());
            typevalue = value->Type();
            unlocking();
            return aTRUE;
        }

        value = v->GetLetValue();
		value->Setreference(reference);
        value->Enablelock(isToBelocked());
        typevalue = value->Type();
        unlocking();
		return aTRUE;
	}

    bool Setvalue(Tamgu* index, Tamgu* v, short idthread, bool strict = false) {
        locking();
        if (value == v) {
            unlocking();
            return true;
        }
        
        if (index->isIndex()) {
            globalTamgu->Returnerror("LET(001): Undefined assignment", idthread);
            unlocking();
            return false;
        }
        
        if (!value->isConst())
            value->Resetreference(reference);
        
        if (v != aNOELEMENT && v->isConst()) {
            value = v->Atom();
            value->Setreference(reference);
            value->Enablelock(isToBelocked());
            typevalue = value->Type();
            unlocking();
            return aTRUE;
        }
        
        value = v->GetLetValue();
        value->Setreference(reference);
        value->Enablelock(isToBelocked());
        typevalue = value->Type();
        unlocking();
        return true;
    }
    
    Tamgu* Putvalue(Tamgu* v, short idthread) {
        if (globalTamgu->threadMODE) {
            locking();
            if (value == v) {
                unlocking();
                return value;
            }
            
            if (!value->isConst())
                value->Resetreference(reference);
            
            if (v->isConst())
                v = v->Atom();
            
            value = v->GetLetValue();
            value->Setreference(reference);
            value->Enablelock(isToBelocked());
            typevalue = value->Type();
            unlocking();
            return value;
        }
        
        if (value == v)
            return value;
        
        if (!value->isConst())
            value->Resetreference(reference);
        
        if (v->isConst())
            v = v->Atom();
        
        value = v->GetLetValue();
        value->Setreference(reference);
        value->Enablelock(isToBelocked());
        typevalue = value->Type();

        return value;
    }

    Tamgu* Clonevalue(Tamgu* v, short idthread) {
        if (globalTamgu->threadMODE) {
            locking();
            if (value == v) {
                unlocking();
                return value;
            }
            
            if (!value->isConst())
                value->Resetreference(reference);
            
            if (v->isConst())
                v = v->Atom();
            
            value = v->GetLetValue();
            value->Setreference(reference);
            value->Enablelock(isToBelocked());
            typevalue = value->Type();
            unlocking();
            return value;
        }
        
        if (value == v)
            return value;
        
        if (!value->isConst())
            value->Resetreference(reference);
        
        if (v->isConst())
            v = v->Atom();
        
        value = v->GetLetValue();
        value->Setreference(reference);
        value->Enablelock(isToBelocked());
        typevalue = value->Type();

        return value;
    }

    void Forcedclean() {
        locking();
        value->Resetreference(reference);
        value = aNOELEMENT;
        typevalue = a_const;
        unlocking();
    }

    void Postinstantiation(short idthread, bool setreference) {
        if (value->isConst()) {
            value = value->Atom();
            value->Setreference();
        }
        typevalue = value->Type();
    }

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return globalTamgu->Provideself();
	}

	short Typevariable() {
		return a_self;
	}

	short Type() {
		if (value == aNOELEMENT)
			return a_self;
		return typevalue;
	}

	short Typeinfered() {
		return a_self;
	}

	string Typename() {
        if (value == aNOELEMENT)
            return "auto";
        return value->Typename();
	}

};

class Tamguselfbuff : public TamguSelf {
public:
    long idx;
    bool used;
    
    Tamguselfbuff(long i) {
        //Do not forget your variable initialisation
        idx = i;
        used = false;
    }
    
    bool Candelete() {
        return false;
    }
    
    Tamgu* anInstance(long i) {
        return new Tamguselfbuff(i);
    }
    
    void Resetreference(short r) {
        value->Resetreference(r);
        r = reference - r;
        if (r <= 0) {
            reference.store(0);
            if (!protect) {
                protect = true;

                typevalue = a_const;
                value = aNOELEMENT;
                if (!globalTamgu->threadMODE && used)
                    globalTamgu->slfempties.push_back(idx);
                used = false;
            }
        }
        else
            reference.store(r);
    }
};

//------------------------------------------------------------------------
//The difference between let and self, is that in a let the first value defines once for all the type of the variable...
//let i=0; i is then an integer, unless you use the operator :=
// i := "tutu"; becomes a string...
//In a self, the type does not depend on the first assignment.

class TamguRawSelf : public Tamgu {
public:
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Tamgu* value;

    //---------------------------------------------------------------------------------------------------------------------

    TamguRawSelf() {
        //Do not forget your variable initialisation
        value = aNOELEMENT;
    }

    bool isLetSelf() {
        return true;
    }
    
    Tamgu* GetLetValue() {
        return value;
    }
    
    //----------------------------------------------------------------------------------------------------------------------
    void Storevalue(string& u);
    void Storevalue(wstring& u);
    
    void storevalue(string u);
    void storevalue(float u);
    void storevalue(short u);
    void storevalue(wstring u);
    void storevalue(long u);
    void storevalue(BLONG u);
    void storevalue(double u);
    void storevalue(unsigned char u);
    void storevalue(wchar_t u);
    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* idx, Tamgu* v, short idthread) {
        if (value == v) {
            return aTRUE;
        }

        if (!value->isConst()) {
            value->Put(idx, v, idthread);
            return aTRUE;
        }
        else {
            if (idx->isIndex()) {
                return globalTamgu->Returnerror("LET(001): Undefined assignment", idthread);
            }
        }

        if (v->isConst()) {
            if (v == aNOELEMENT) {
                value = aNOELEMENT;
                return aTRUE;
            }
            value = v->Atom();
            value->Enablelock(isToBelocked());
            return aTRUE;
        }
        
        value = v->GetLetValue();
        value->Enablelock(isToBelocked());
        value->Setreference();
        return aTRUE;
    }

    bool Setvalue(Tamgu* index, Tamgu* v, short idthread, bool strict = false) {
        if (value == v) {
            return true;
        }

        if (index->isIndex()) {
            globalTamgu->Returnerror("LET(001): Undefined assignment", idthread);
            return false;
        }

        if (!value->isConst())
            value->Resetreference();
        
        if (v != aNOELEMENT && v->isConst()) {
            value = v->Atom();
            value->Enablelock(isToBelocked());
            return aTRUE;
        }
        
        value = v->GetLetValue();
        value->Enablelock(isToBelocked());
        return true;
    }

    Tamgu* Clonevalue(Tamgu* v, short idthread) {
        if (value == v) {
            return v;
        }

        if (!value->isConst())
            value->Resetreference();

        if (v->isConst())
            v = v->Atom();

        value = v->GetLetValue();
        value->Enablelock(isToBelocked());
        return value;
    }

    Tamgu* Putvalue(Tamgu* v, short idthread) {
        if (value == v) {
            return v;
        }

        if (!value->isConst())
            value->Resetreference();

        if (v->isConst())
            v = v->Atom();

        value = v->GetLetValue();
        value->Enablelock(isToBelocked());
        return value;
    }

    void Clean() {
        value =  aNOELEMENT;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread) {
        return value->Eval(context, v, idthread);
    }

    short Typevariable() {
        if (value == aNOELEMENT)
            return a_let;
        return value->Typevariable();
    }

    short Type() {
        if (value == aNOELEMENT)
            return a_let;
        return value->Type();
    }

    short Typeinfered() {
        return a_let;
    }

    string Typename() {
        if (value == aNOELEMENT)
            return "let";
        return value->Typename();
    }

    Tamgu* Atom(bool forced = false) {
        return value->Atom(forced);
    }

    bool isFrame() {
        return value->isFrame();
    }
    
    Tamgu* Frame() {
        return value->Frame();
    }

    //If it is too complex to duplicate an element (for instance when passed to a function)
    //then you can use return false...  Usually in that case, Atom should always be just: return this;
    bool duplicateForCall() {
        return value->duplicateForCall();
    }

    Tamgu* Value() {
        return value;
    }

    bool isFunction() {
        return value->isFunction();
    }

    Tamgu* Body(short idthread) {
        return value->Body(idthread);
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below

    void Clear() {
        //To set a variable back to empty
        value->Clear();
    }


    string String() {
        return value->String();
    }

    wstring UString() {
        return value->UString();
    }

    void Setstring(string& v, short idthread) {
        value->Setstring(v, idthread);
    }

    void Setstring(wstring& v, short idthread) {
        value->Setstring(v, idthread);
    }

    long Getinteger(short idthread) {
        return value->Getinteger(idthread);
    }

    BLONG Getlong(short idthread) {
        return value->Getlong(idthread);
    }

    short Getshort(short idthread) {
        return value->Getshort(idthread);
    }

    float Getdecimal(short idthread) {
        return value->Getdecimal(idthread);
    }

    double Getfloat(short idthread) {
        return value->Getfloat(idthread);
    }

    string Getstring(short idthread) {
        return value->Getstring(idthread);
    }

    wstring Getustring(short idthread) {
        return value->Getustring(idthread);
    }

    long Integer() {
        return value->Integer();
    }
    double Float() {
        return value->Float();
    }
    BLONG Long() {
        return value->Long();
    }

    bool Boolean() {
        return value->Boolean();
    }

    string StringToDisplay() {
        return value->StringToDisplay();
    }

    string JSonString() {
        return value->JSonString();
    }

    string Bytes() {
        return value->Bytes();
    }


    //Basic operations
    long Size() {
        return value->Size();
    }

    Tamgu* Merging(Tamgu* a) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->Merging(a);
        return a;
    }
    
    Tamgu* Combine(Tamgu* a) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->Combine(a);
        return a;
    }
    
    Tamgu* andset(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->andset(a, itself);
        return a;
    }

    Tamgu* orset(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->orset(a, itself);
        return a;
    }

    Tamgu* xorset(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        a = value->xorset(a, itself);
        return a;
    }

    Tamgu* plus(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->plus(a, itself);
    }

    Tamgu* minus(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->minus(a, itself);
    }

    Tamgu* multiply(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->multiply(a, itself);
    }

    Tamgu* divide(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->divide(a, itself);
    }

    Tamgu* divideinteger(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->divideinteger(a, itself);
    }

    Tamgu* power(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->power(a, itself);
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->shiftleft(a, itself);
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->shiftright(a, itself);
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        if (value == aNOELEMENT) {
            return globalTamgu->Returnerror(e_uninitialized_self_variable);
        }
        return value->mod(a, itself);
    }

    Tamgu* less(Tamgu* a) {
        return value->less(a);
    }

    Tamgu* more(Tamgu* a) {
        return value->more(a);
    }

    Tamgu* same(Tamgu* a) {
        return value->same(a);
    }

    Tamgu* different(Tamgu* a) {
        return value->different(a);
    }

    Tamgu* lessequal(Tamgu* a) {
        return value->lessequal(a);
    }

    Tamgu* moreequal(Tamgu* a) {
        return value->moreequal(a);
    }

};

#endif




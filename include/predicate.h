/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : predicate.h
 Date       : 2017/09/01
 Purpose    : Predicate Implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
   */


#ifndef i_predicate
#define i_predicate

#include "tamgu.h"
#include "tamgumap.h"
#include "tamgumapss.h"
#include "tamguvector.h"
#include "tamgusvector.h"
#include "instructions.h"

class TamguPredicateRule;

typedef enum { SEARCHONE, FULLSEARCH, STOPSEARCH } predicatesearch;

class TamguDeclarationPredicate : public TamguDeclaration {
public:
    ThreadLock* _locker;
	bool fail;

	TamguDeclarationPredicate() : fail(false), _locker(NULL) {}

    ~TamguDeclarationPredicate() {
        if (_locker != NULL)
            delete _locker;
    }

    void Setdomainlock() {
        if (_locker)
            _locker->lock->lock();
    }

    void Resetdomainlock() {
        if (_locker)
            _locker->lock->unlock();
    }

    void Initlock() {
        if (_locker == NULL)
            _locker = new ThreadLock;
    }
    
	void Setfail(bool v) {
		fail = v;
	}

	bool Failed() {
		return fail;
	}
    
    short Type() {
        return a_predicatedomain;
    }

};

//A PredicateVariable such as: ?val
class TamguBasePredicateVariable : public TamguReference {
public:

	short name;

	TamguBasePredicateVariable(TamguGlobal* g, short n, Tamgu* parent = NULL) : TamguReference(g, parent) {
		name = n;
	}
	
	TamguBasePredicateVariable(short n) {
		name = n;
	}
    
    virtual bool Stringpredicatekey(string& v) {
        return false;
    }

    virtual bool Stringpredicatekeysecond(string& v) {
        return false;
    }

    virtual bool Stringpredicatekeythird(string& v) {
        return false;
    }

	bool isPredicateVariable() {
		return true;
	}

	bool isAssignable() {
		return true;
	}

	bool duplicateForCall() {
		return false;
	}

	virtual short Type() {
		return a_predicatevar;
	}

	void Setname(short n) {
		name = n;
	}

	short Name() {
        return name;
    }
    
    bool isName(short n) {
        return (n == name || n == a_universal);
    }

	virtual string String();

	virtual bool Unify(TamguDeclaration* dom, Tamgu* a) {
		return false;
	}

	virtual bool isUnified(TamguDeclaration* dom);
	
	virtual Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguBasePredicateVariable(globalTamgu, name);
	}

	virtual Tamgu* same(Tamgu* a) {
		return aTRUE;
	}
	
	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	virtual Tamgu* Put(Tamgu* idx, Tamgu* value, short idthread);
	virtual Tamgu* ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* val, Tamgu* e, short, bool root);
	virtual Tamgu* EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread);
};

class TamguPredicateVariable : public TamguBasePredicateVariable {
public:
	Tamgu* function;
	bool affectation;

	TamguPredicateVariable(TamguGlobal* g, short n, Tamgu* parent = NULL) : TamguBasePredicateVariable(g, n, parent) {
		affectation = false;
		function = NULL;
	}

	void AddInstruction(Tamgu* a) {
		function = a;
	}

	void Setaffectation(bool v) {
		affectation = v;
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

	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    virtual Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);
    
	Tamgu* Function() {
		return function;
	}

};

class TamguPredicateVariableExchange : public TamguPredicateVariable {
public:
    Tamgu* exchange;
    
    TamguPredicateVariableExchange(short n, Tamgu* v, TamguGlobal* g, Tamgu* parent = NULL) : exchange(v), TamguPredicateVariable(g, n, parent) {}
    Tamgu* ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* val, Tamgu* e, short, bool root);
};

class TamguPredicateVariableInstance : public TamguBasePredicateVariable {
public:
	Tamgu* value;
	short labelname;
    short thread_id;
	bool merge;

    TamguPredicateVariableInstance() : thread_id(-1), labelname(-1), value(aNOELEMENT), merge(false), TamguBasePredicateVariable(-1) {}
    TamguPredicateVariableInstance(TamguGlobal* global) : thread_id(-1), labelname(-1), value(aNOELEMENT), merge(false), TamguBasePredicateVariable(global, -2) {}
	TamguPredicateVariableInstance(short n, short ln, short idthread) : thread_id(idthread), labelname(ln), value(aNOELEMENT), merge(false), TamguBasePredicateVariable(n) {}

    
    ~TamguPredicateVariableInstance() {
        if (thread_id != -1) {
            globalTamgu->Returnname(thread_id, name);
        }
    }
    
	bool isToMerge() {
		return merge;
	}
    
	short Type() {
		return a_instance;
	}

	string String();

	bool Unify(TamguDeclaration* dom, Tamgu* a);
	bool isUnified(TamguDeclaration* dom);

	Tamgu* VariableValue(TamguDeclaration* dom, short idthread);

	Tamgu* Variable(TamguDeclaration* dom);


	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguPredicateVariableInstance(name, labelname, idthread);
	}

	bool Insertvalue(Tamgu* dom, Tamgu* v, basebin_hash<Tamgu*>&);

    bool Stringpredicatekey(string& v) {
        return value->Stringpredicatekey(v);
    }

    bool Stringpredicatekeysecond(string& v) {
        return value->Stringpredicatekeysecond(v);
    }

    bool Stringpredicatekeythird(string& v) {
        return value->Stringpredicatekeythird(v);
    }
    
	virtual void Setreference(short inc) {
		if (value != aNOELEMENT)
			value->Setreference(inc);
		reference += inc;
		protect = false;
	}

    virtual void Setreference() {
        if (value != aNOELEMENT)
            value->Setreference();
        ++reference;
        protect = false;
    }

    virtual void Resetreference(short inc = 1);

	void affiche();

	void Cleans(Tamgu* v, bool localvalue) {
		if (localvalue) {
			value->Resetreference(reference);
			value = v;
		}
		else
			v->Resetreference(reference);
	}

	Tamgu* Value(TamguDeclaration* dom);

	Tamgu* Value() {
		return value;
	}

	Tamgu* same(Tamgu* a);

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	Tamgu* Put(Tamgu* idx, Tamgu* value, short idthread);

	bool Setvalue(Tamgu* index, Tamgu* val, short idthread, bool initial) {
        if (value == val)
            return false;
        
		if (!initial)
			value->Resetreference(reference);
		value = val;
		return true;
	}

	Tamgu* ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* val, Tamgu* e, short, bool root);
	Tamgu* EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread);
	virtual Tamgu* Getvalues(TamguDeclaration* dom, bool duplicate);

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		value->Resetreference(reference);
		value = a->Atom();
		value->Setreference(reference);
		return value;
	}

	Tamgu* plusplus() {
		return value->plusplus();
	}

	Tamgu* minusminus() {
		return value->plusplus();
	}

	Tamgu* plus(Tamgu* a, bool itself) {
		return value->plus(a, itself);
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		return value->minus(a, itself);
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		return value->multiply(a, itself);
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		return value->divide(a, itself);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		return value->power(a, itself);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		return value->shiftleft(a, itself);
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		return value->shiftright(a, itself);
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		return value->mod(a, itself);
	}

	Tamgu* less(Tamgu* a) {
		return value->less(a);
	}

	Tamgu* more(Tamgu* a) {
		return value->more(a);
	}

	Tamgu* lessequal(Tamgu* a) {
		return value->lessequal(a);
	}

	Tamgu* moreequal(Tamgu* a) {
		return value->moreequal(a);
	}

};

class TamguPredicateVariableInstanceExchange : public TamguPredicateVariableInstance {
public:
    Tamgu* domain;
    Tamgu* exchange;
    TamguPredicateVariableInstanceExchange(Tamgu* d, Tamgu* e, short n, short ln, short idthread) :
        domain(d),exchange(e),TamguPredicateVariableInstance(n, ln, idthread) {}
    Tamgu* Getvalues(TamguDeclaration* dom, bool duplicate);
};

class TamguPredicateVariableInstancebuff : public TamguPredicateVariableInstance {
public:
    long idx;
    char used;

    TamguPredicateVariableInstancebuff(long i) : idx(i) {
        used = false;
    }
    
    bool Candelete() {
        return false;
    }

    void Resetreference(short inc = 1);
    void Resetreferencenoprotect(short r = 1);
    void Removereference(short inc = 1);
  
    void Setreference(short inc) {
        if (value != aNOELEMENT)
            value->Setreference(inc);
        reference += inc;
        protect = false;
    }
    
    void Setreference() {
        if (value != aNOELEMENT)
            value->Setreference();
        ++reference;
        protect = false;
    }
};


class TamguPredicateVariableInstanceForCleaning : public TamguReference {
public:
	TamguPredicateVariableInstance* variable;

	TamguPredicateVariableInstanceForCleaning(TamguPredicateVariableInstance* v) {
		variable = v;
	}

	void Resetreference(short inc = 1) {
		variable->value->Resetreference(variable->reference);
		variable->value = aNOELEMENT;
		TamguReference::Resetreference(inc);
	}
};

class TamguPredicateTerm : public TamguBasePredicateVariable {
public:

	vector<Tamgu*> parameters;

	TamguPredicateTerm(TamguGlobal* g, short n, Tamgu* parent = NULL) : TamguBasePredicateVariable(g, n, parent) {}

	bool Insertvalue(Tamgu* dom, Tamgu* v, basebin_hash<Tamgu*>&);

	virtual Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguPredicateTerm(globalTamgu, name);
	}

	short Type() {
		return a_term;
	}

	void Set(short n) {
		name = n;
		globalTamgu->RecordInTrackerProtected(this);
	}

	Tamgu* Value() {
		return this;
	}

	void Leaves(Tamgu* v) {
		for (size_t i = 0; i < parameters.size(); i++)
			parameters[i]->Leaves(v);
	}

	Tamgu* ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* val, Tamgu* e, short, bool root);
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread);

	Tamgu* Put(Tamgu* idx, Tamgu* value, short idthread);

	bool Unify(TamguDeclaration* dom, Tamgu* a);
	bool isUnified(TamguDeclaration* dom);

	Tamgu* Vector(short idthread);
	Tamgu* Map(short idthread);

	Tamgu* Parameter(size_t i) {
		return parameters[i];
	}

	virtual void Release() {
		if (reference == 0) {
			protect = false;
			for (size_t i = 0; i < parameters.size(); i++)
				parameters[i]->Release();
			TamguReference::Resetreference();
		}
	}

    void Setreference(short inc);
    void Setreference();
	void Resetreference(short inc = 1);
	void Setprotect(bool);
	void Popping();


	virtual Tamgu* Getvalues(TamguDeclaration* dom, bool duplicate);

	virtual void AddInstruction(Tamgu* e) {
		parameters.push_back(e);
	}

	string String();
    void Setstring(string& v, short idthread);
    
    bool Stringpredicatekey(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

    bool Stringpredicatekeysecond(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

    bool Stringpredicatekeythird(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }


	BLONG Long() {
		return parameters.size();
	}

	long Integer() {
		return (long)parameters.size();
	}
	double Float() {
		return (double)parameters.size();
	}

	Tamgu* same(Tamgu* a);

	long Size() {
		return parameters.size();
	}

	Tamgu* MethodName(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providestring(globalTamgu->Getsymbol(name));
	}

	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return MethodName(contextualpattern, idthread, callfunc);
	}

};

class TamguPredicateConcept : public TamguBasePredicateVariable {
public:
	VECTE<Tamgu*> parameters;
	VECTE<Tamgu*> args;
	TamguFunctionLambda* fconcept;
	Tamgu* value;
	
	TamguPredicateConcept(TamguGlobal* g, short n, Tamgu* parent = NULL);

	bool Insertvalue(Tamgu* dom, Tamgu* v, basebin_hash<Tamgu*>&);

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguPredicateConcept(globalTamgu, name);
	}

	bool isConcept() {
		return true;
	}


	Tamgu* MethodName(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providestring(globalTamgu->Getsymbol(name));
	}

	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return MethodName(contextualpattern, idthread, callfunc);
	}


	short Type() {
		return a_concept;
	}

	void Set(short n) {
		name = n;
		globalTamgu->RecordInTrackerProtected(this);
	}

	Tamgu* Value() {
		return this;
	}

	void Leaves(Tamgu* v) {
		for (size_t i = 0; i < parameters.size(); i++)
			parameters[i]->Leaves(v);
	}

	Tamgu* ExtractPredicateVariables(Tamgu* context, TamguDeclaration* dom, Tamgu* val, Tamgu* e, short, bool root);
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread);

	Tamgu* Put(Tamgu* idx, Tamgu* value, short idthread);

	bool Unify(TamguDeclaration* dom, Tamgu* a);
	bool isUnified(TamguDeclaration* dom);

	Tamgu* Vector(short idthread);
	Tamgu* Map(short idthread);

	Tamgu* Parameter(size_t i) {
		return parameters[i];
	}

	void Release() {
		if (reference == 0) {
			protect = false;
			for (size_t i = 0; i < parameters.size(); i++)
				parameters[i]->Release();
			TamguReference::Resetreference();
		}
	}

    void Setreference(short inc);
    void Setreference();
	void Resetreference(short inc = 1);
	void Setprotect(bool);
	void Popping();


	Tamgu* Evalue(TamguDeclaration* dom, short idthread, bool duplicate);
	Tamgu* Getvalues(TamguDeclaration* dom, bool duplicate);

	void AddInstruction(Tamgu* e) {
		parameters.push_back(e);
	}

	string String();
    void Setstring(string& v, short idthread);
    
    bool Stringpredicatekey(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

    bool Stringpredicatekeysecond(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

    bool Stringpredicatekeythird(string& v) {
        Setstring(v, 0);
        return !v.empty();
    }

	BLONG Long() {
		return parameters.size();
	}

	long Integer() {
		return (long)parameters.size();
	}
	double Float() {
		return (double)parameters.size();
	}

	Tamgu* same(Tamgu* a);

	long Size() {
		return parameters.size();
	}
};

class TamguPredicate;
//This typedef defines a type "predicateMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (TamguPredicate::*predicateMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//The instance. It can contain predicate variables in its parameters: ?val
class TamguPredicate : public TamguReference {
public:
	short ptype;
	vector<Tamgu*> parameters;

	short name;

    bool terminal;
	bool negation;
	bool disjunction;
    bool shared;

	static Exchanging basebin_hash<predicateMethod> methods;

	TamguPredicate(short n, TamguGlobal* g = NULL, short t = a_predicate, Tamgu* parent = NULL);
	TamguPredicate(TamguGlobal* g, short n);

	void Leaves(Tamgu* v) {
		for (size_t i = 0; i < parameters.size(); i++)
			parameters[i]->Leaves(v);
	}

	virtual bool Setinvariable(Tamgu* v, TamguDeclaration* dom, short idthread) {
		return false;
	}

	virtual void Resetintvariable(TamguDeclaration* dom, short idthread) {}

	bool duplicateForCall() {
		return false;
	}
    
    bool isTerminal() {
        return terminal;
    }

    TamguPredicate* checkTerminal(short n, short sz) {
        return (terminal && name == n && parameters.size() == sz)?this:NULL;
    }
    
    void Setidtype(TamguGlobal* global);

	void Setnegation(bool v) {
		negation = v;
	}

    short Name() {
        return name;
    }
    bool isName(short n) {
        return (n == name || n == a_universal);
    }
    string Namestring() {
        return globalTamgu->Getsymbol(name);
    }
        
	~TamguPredicate() {
		if (idtracker != -1 && globalTamgu->Checktracker(this, idtracker))
			globalTamgu->RemoveFromTracker(idtracker);
	}

	virtual bool Checkparameters(TamguDeclaration*) {
		return true;
	}

	virtual long Idrule() {
		return -1;
	}

	virtual void Setidrule(long i) {}

	virtual Tamgu* Features() {
		return aNULL;
	}

	virtual void Setfeatures(Tamgu* feats) {}


	virtual short Idvar() {
		return 0;
	}
    
    virtual char setPredicateNameVariable(TamguPredicate*, short idthread) {
        return true;
    }
    
    virtual void resetPredicateNameVariable(short idthread) {}

	virtual void Setname(short n) {
		name = n;
	}

	virtual short kbaction() {
		return 0;
	}

	virtual void Setchosen(char v) {}

	virtual bool isDependency() {
		return false;
	}

	virtual short Type() {
		return ptype;
	}

	void Setdisjunction(bool v) {
		disjunction = v;
	}

	void set(bool n, bool d) {
		negation = n;
		disjunction = d;
	}

	bool isNegation() {
		return negation;
	}

	bool isDisjunction() {
		return disjunction;
	}

	bool Unify(TamguDeclaration* dom, Tamgu* a);
	bool isUnified(TamguDeclaration* dom);

	Tamgu* Vector(short idthread);
	Tamgu* Map(short idthread);


	virtual TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* d, short idthread);
	bool Copyfrom(TamguPredicate* p, Tamgu* context, TamguDeclaration* d, TamguPredicate* h, short);

	Tamgu* Parameter(size_t i) {
		return parameters[i];
	}

	Tamgu* Vector(Tamgu*, Tamguvector* itself = NULL);
	Tamgu* Map(Tamgu*, Tamgumap* itself = NULL);


    virtual void Setreference(short inc);
    virtual void Setreference();
	virtual void Resetreference(short inc = 1);
	virtual void Setprotect(bool);
	void Popping();

	virtual void Clear() {
		for (long i = 0; i < parameters.size(); i++) {
			if (!parameters[i]->isInstruction()) {
				parameters[i]->Setprotect(0);
				parameters[i]->Resetreference(reference);
			}
		}
		parameters.clear();
	}

	virtual void AddInstruction(Tamgu* e) {
		parameters.push_back(e);
	}

	bool Basic() {
		return true;
	}

	bool Simple() {
		return true;
	}

	bool Statique() {
		return true;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);
	virtual Tamgu* Put(Tamgu* context, Tamgu* callfunction, short idthread);

	virtual Tamgu* Newinstance(short idthread, Tamgu* f = NULL);

	virtual string String();
	virtual string JSonString();
    virtual void Setstring(string& v, short idthread);

    bool Stringpredicatekey(string& v) {
        return parameters[0]->Stringpredicatekey(v);
    }
    
    bool Stringpredicatekeysecond(string& v) {
        if (parameters.size() > 1)
            return parameters[1]->Stringpredicatekeysecond(v);
        return false;
    }

    bool Stringpredicatekeythird(string& v) {
        if (parameters.size() > 2)
            return parameters[2]->Stringpredicatekeythird(v);
        return false;
    }

	bool Boolean();

	Tamgu* same(Tamgu* a);

	long Size() {
		return parameters.size();
	}

    void Replaceparameter(short i, Tamgu* value, short idthread) {
        if (parameters[i] == value || value == aNOELEMENT)
            return;
        if (parameters[i]->isPredicateVariable()) {
            if (parameters[i]->Setvalue(aNULL,value, idthread, false))
                value->Setreference(parameters[i]->Reference());
        }
        else {
            parameters[i]->Resetreference();
            parameters[i] = value;
            value->Setreference(reference);
        }
    }
    
	//--------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}


	void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[a_predicate])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[a_predicate].find(n) !=  globalTamgu->infomethods[a_predicate].end())
              return globalTamgu->infomethods[a_predicate][n];
             return "Unknown method";
	}
	//---------------------------------------------------------------------------------------------------------------------
	Tamgu* MethodRuleid(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFeatures(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLeaves(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* v = Selectavector(contextualpattern);
		Leaves(v);
		return v;
	}
	Tamgu* MethodName(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providestring(globalTamgu->Getsymbol(name));
	}
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTrace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	static void AddMethod(TamguGlobal* g, string name, predicateMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(TamguGlobal* g, string version);
	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*TamguPredicate::methods.get(idname))(contextualpattern, idthread, callfunc);
	}
};

class TamguPredicateFunction : public TamguPredicate {
public:
	Tamgu* function;
	Tamgu* object;
	bool trace;
	TamguPredicateFunction(TamguGlobal* g, Tamgu* f, short n) : TamguPredicate(n, g, a_predicate) {
		function = f;
		object = aNULL;
		trace = false;
	}
    
	Tamgu* Function() {
		return function;
	}
	string String() {
		return globalTamgu->Getsymbol(name);
	}
    
    void Setstring(string& v, short idthread) {
        v = globalTamgu->Getsymbol(name);
    }

    void Setstring(wstring& v, short idthread) {
        v = globalTamgu->Getwsymbol(name);
    }

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguPredicateFunction(globalTamgu, function, name);
	}
	void Resetreference(short inc = 1);

	Tamgu* Returnobject() {
		return object;
	}
	void Setname(short n);
};

class TamguConstPredicate : public TamguPredicate {
public:
	TamguConstPredicate(TamguGlobal* g, short n) : TamguPredicate(n, g, a_predicate) {
        investigate |= is_constante;
    }
    
	string String() {
		return globalTamgu->Getsymbol(name);
	}
    
    void Setstring(string& v, short idthread) {
        v = globalTamgu->Getsymbol(name);
    }

    void Setstring(wstring& v, short idthread) {
        v = globalTamgu->Getwsymbol(name);
    }

    short checkTypePredicate() {
        return name;
    }
    
	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return this;
	}
	void Resetreference(short inc = 1) {}
    
};

class TamguPredicateAsVariable : public TamguPredicate {
public:
    short idvar;
    Tamgu* predicate_name;

    TamguPredicateAsVariable(TamguGlobal* g, Tamgu* parent, Tamgu* var, short n, short idv) : TamguPredicate(n, g, a_predicate, parent) {
        idvar = idv;
        predicate_name = var;
    }
    
    TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* d, short idthread);
    bool isPredicateNameVariable() {
        return true;
    }
    char setPredicateNameVariable(TamguPredicate*, short idthread);
    void resetPredicateNameVariable(short idthread);
    
    virtual void Popping() {}
    virtual void Setreference(short inc) {}
    virtual void Setreference() {}
    virtual void Resetreference(short inc = 1) {}
    virtual void Setprotect(bool) {}
    
};

class TamguPredicateAsVariableInstance : public TamguPredicateAsVariable {
public:

    TamguPredicateAsVariableInstance(short n, short idv) : TamguPredicateAsVariable(NULL, NULL, NULL, n, idv) {}
    
    void Popping();
    void Setreference(short inc);
    void Setreference();
    void Resetreference(short inc = 1);
    void Setprotect(bool);

    void Clear() {
        predicate_name->Resetreference(reference);
        for (long i = 0; i < parameters.size(); i++) {
            if (!parameters[i]->isInstruction()) {
                parameters[i]->Setprotect(0);
                parameters[i]->Resetreference(reference);
            }
        }
        parameters.clear();
    }


};

class TamguDependency : public TamguPredicate {
public:
	Tamgu* features;
	long idrule;
	short idvar;	
	char chosen;


	TamguDependency(TamguGlobal* g, Tamgu* f, short n, short id);

	short Idvar();

	long Idrule() {
		return idrule;
	}

	void Setidrule(long i) {
		idrule = i;
	}

	bool Setinvariable(Tamgu* v, TamguDeclaration* dom, short idthread);

	void Resetintvariable(TamguDeclaration* dom, short idthread);

	TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* d, short idthread);
	bool Unify(TamguDeclaration* dom, Tamgu* a);

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguDependency(NULL, aNULL, name, idvar);
	}

	void AddInstruction(Tamgu* e) {
		if (chosen == 2)
			features = e;
		else
			parameters.push_back(e);
	}

	bool isChosen() {
		return chosen;
	}

	void Setchosen(char v) {
		chosen = v;
	}
	
	bool isDependency() {
		return true;
	}

    void Setreference(short inc) {
        features->Setreference(inc);
        TamguPredicate::Setreference(inc);
    }
    
    void Setreference() {
        features->Setreference();
        TamguPredicate::Setreference();
    }
    
	void Resetreference(short inc = 1) {
		features->Resetreference(inc);
		TamguPredicate::Resetreference(inc);
	}

	void Setprotect(bool v) {
		features->Setprotect(v);
		TamguPredicate::Setprotect(v);
	}

	string String();
    void Setstring(string& v, short idthread);

    bool Stringpredicatekey(string& v) {
        return false;
    }

    bool Stringpredicatekeysecond(string& v) {
        return false;
    }

    bool Stringpredicatekeythird(string& v) {
        return false;
    }

	string JSonString();

	Tamgu* Vector(short idthread) {
		Tamguvector* v = new Tamguvector;
		v->Push(new Tamgustring(globalTamgu->Getsymbol(name)));
		v->Push(features);
		for (long i = 0; i < parameters.size(); i++) {
			v->Push(parameters[i]->Map(idthread));
		}
		return v;
	}

	Tamgu* Features() {
		return features;
	}

	void Setfeatures(Tamgu* feats);
};

class TamguPredicateVar : public TamguPredicate {
public:


	TamguPredicateVar(TamguGlobal* g, short n) : TamguPredicate(n, g, a_predicate) {}

	Tamgu* Put(Tamgu* dom, Tamgu* val, short idthread);
	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguPredicateVar(globalTamgu, name);
	}

};

class TamguPredicateKnowledgeBase : public TamguPredicate {
public:
	bool add;

	TamguPredicateKnowledgeBase(TamguGlobal* g, short n, Tamgu* parent = NULL, short t = a_knowledgebase) : TamguPredicate(n, g, t, parent) {
		add = false;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguPredicateKnowledgeBase(globalTamgu, name);
	}
    
};

class TamguDependencyKnowledgeBase : public TamguPredicateKnowledgeBase {
public:
	Tamgu* features;

	TamguDependencyKnowledgeBase(TamguGlobal* g, short n, Tamgu* parent = NULL) : TamguPredicateKnowledgeBase(g, n, parent, a_dependencybase) {
		features = aNULL;
	}

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		return new TamguDependencyKnowledgeBase(globalTamgu, name);
	}

	void AddInstruction(Tamgu* e) {
		if (add)
			features = e;
		else
			parameters.push_back(e);
	}
};

//The actions are: retract, asserta, assertz
class TamguPredicateAction : public TamguPredicate {
public:
	short action;

	TamguPredicateAction(TamguGlobal* g, short n, Tamgu* parent = NULL) : action(n), TamguPredicate(n, g, a_predicate, parent) {}

	short kbaction() {
		return action;
	}

    virtual short checkTypePredicate() {
        return action;
    }
    
	string String();
    void Setstring(string& v, short idthread);

	void AddInstruction(Tamgu* e) {
		TamguPredicate* p = (TamguPredicate*)e;
		name = p->name;
		parameters = p->parameters;
		negation = p->negation;
		p->parameters.clear();
	}

	TamguPredicate* Duplicate(Tamgu* context, TamguDeclaration* d, short idthread);

	virtual Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		TamguPredicateAction* p = new TamguPredicateAction(globalTamgu, action);
		p->name = name;
		p->negation = negation;
		return p;
	}

};

//The actions are: retract, asserta, assertz, remove
class TamguDependencyAction : public TamguPredicateAction {
public:
	Tamgu* features;
	short idvar;
	long idrule;

	TamguDependencyAction(TamguGlobal* g, short n, long idr, Tamgu* parent = NULL) : idrule(idr), TamguPredicateAction(g, n, parent) {
        //we then transform the different actions:
        //a_asserta becomes a_dependency_asserta
        //a_assertz becomes a_dependency_assertz
        //a_retract becomes a_dependency_retract
        switch (action) {
            case a_asserta:
                action = a_dependency_asserta;
                break;
            case a_assertz:
                action = a_dependency_assertz;
                break;
            case a_retract:
                action = a_dependency_retract;
                break;
        }
		features = aNULL;
		idvar = 0;
	}

	short Idvar();

	long Idrule() {
		return idrule;
	}

	bool Setinvariable(Tamgu* v, TamguDeclaration* dom, short idthread);

	void Resetintvariable(TamguDeclaration* dom, short idthread);

	bool isDependency() {
		return true;
	}

	Tamgu* Features() {
		return features;
	}

	void AddInstruction(Tamgu* e) {
		TamguDependency* p = (TamguDependency*)e;
		name = p->name;
		parameters = p->parameters;
		negation = p->negation;
		features = p->features;
		idvar = p->idvar;
	}

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		TamguDependencyAction* p = new TamguDependencyAction(globalTamgu, action, idrule);
		p->name = name;
		p->features = features;
		p->negation = negation;
		p->idvar = idvar;
		return p;
	}

    void Setreference(short inc) {
        features->Setreference(inc);
        TamguPredicate::Setreference(inc);
    }
    
    void Setreference() {
        features->Setreference();
        TamguPredicate::Setreference();
    }
    
	Tamgu* same(Tamgu* a);
};

//this class is used to record an entry within the knowledge base such as: toto(x,y) :- true; 
//with toto(x,y):- false; the element is removed.
class TamguInstructionPredicate : public TamguSequence {
public:
	short name;
	TamguInstructionPredicate(short n, TamguGlobal* g, Tamgu* parent = NULL) : TamguSequence(g, parent) {
		name = n;
	}

	Tamgu* Eval(Tamgu*, Tamgu*, short);
	short Type() {
		return a_predicateinstruction;
	}
};

//This is a most important class, as it contains the element which is used as a trigger to force the analyse of rules...
class TamguInstructionEvaluate : public Tamgu {
public:
	hmap<unsigned short, vector<TamguPredicateRule*> > rules;
	basebin_hash<TamguPredicateVariableInstance*>* dico;	
	VECTE<TamguPredicate*> results;

	TamguPredicate* head;
	TamguPredicate* headrule;
	TamguDeclarationPredicate* dom;
	Tamgu* value;
	TamguCallFunction* callfunc;

	short threadowner;

	bool count;
	bool trace;
    bool dependency_mode;
	predicatesearch fulltraversal;


	TamguInstructionEvaluate(TamguPredicate* h, short idthread, bool tr, bool d_m = false) : results(10) {
        dependency_mode = d_m;
		fulltraversal = SEARCHONE;
        threadowner = idthread;
		callfunc = NULL;
		value = aNULL;
		trace = tr;
		count = false;
		headrule = NULL;
		head = h;
		dom = NULL;
	}

	~TamguInstructionEvaluate() {
		if (idtracker != -1 && globalTamgu->Checktracker(this, idtracker))
			globalTamgu->RemoveFromTracker(idtracker);
	}

	short Type() {
		return a_predicateevaluate;
	}

    unsigned long CallBackArity() {
        return P_TWO;
    }

	basebin_hash<TamguPredicateVariableInstance*>* Dico() {
		return dico;
	}

	void Setdico(short n, TamguPredicateVariableInstance* a) {
		(*dico)[n] = a;
	}

	TamguPredicateVariableInstance* Getdico(short n) {
		return (*dico)[n];
	}

	bool Checkdico(short n) {
		return dico->check(n);
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* PredicateEvalue(VECTE<Tamgu*>& goals, TamguPredicate* currenthead, long depth);
	TamguPredicate* PredicateUnification(VECTE<Tamgu*>& goals, long& posreplace, long& from);
	Tamgu* PredicateCreate(TamguPredicate* headrule, long depth);

};


class TamguInstructionLaunch : public TamguTracked{
public:
	TamguPredicate* head;

	TamguInstructionLaunch(TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_predicatelaunch, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	short Type() {
		return a_predicatelaunch;
	}

    short Typeinfered() {
        return a_predicatelaunch;
    }
    
	void AddInstruction(Tamgu* e) {
		head = (TamguPredicate*)e;
	}
};

//definition of a rule element, which can be anything from another predicate to a function call...
//contextualpattern is ALWAYS a TamguInstructionEvaluate
class TamguPredicateRuleElement : public TamguTracked {
public:
	vector<Tamgu*> instructions;
	bool negation;
	bool disjunction;

	TamguPredicateRuleElement(TamguGlobal* g, Tamgu* parent = NULL) : disjunction(false), negation(false), TamguTracked(a_predicateruleelement, g, parent) {}


	short Type() {
		return a_predicateruleelement;
	}

	short Action() {
		return a_predicateelement;
	}

	void AddInstruction(Tamgu* ke) {
		instructions.push_back(ke);
	}

	void Setnegation(bool n) {
		negation = n;
	}

	void Setdisjunction(bool v) {
		disjunction = v;
	}

	void Addtail(TamguPredicateRule*, long& disj);
};


class TamguPredicateContainer : public TamguReference {
public:
	hmap<unsigned short, vector<TamguPredicateRule*> > rules;


	TamguPredicateContainer(TamguGlobal* g) : TamguReference(g) {}

	bool duplicateForCall() {
		return false;
	}

	short Type() {
		return a_predicatecontainer;
	}

};


//Definition of a predicate rule
class TamguPredicateRule : public TamguTracked {
public:
	vector<Tamgu*> instructions;
	TamguPredicate* head;
	short name;
	char disjunction;

	short Name() {
		return name;
	}
    bool isName(short n) {
        return (n == name || n == a_universal);
    }

	TamguPredicateRule(short n, TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguTracked(a_predicaterule, g, parent) {
		name = n;
		disjunction = 0;
		head = NULL;
	}

	~TamguPredicateRule() {
		head = NULL;
	}

	short Type() {
		return a_predicaterule;
	}

	void Reorderdisjunction(long);
	void Addtail(TamguPredicateContainer* kcont, TamguPredicateRuleElement* elements) {
		if (elements != NULL) {
			long disj = -1;
			kcont->rules[name].push_back(this);
			elements->Addtail(this, disj);
			if (disj != -1)
				Reorderdisjunction(disj);
		}
	}
	Tamgu* Eval(Tamgu* c, Tamgu* v, short idthread);
	void addfinal(TamguPredicateContainer* kpc);
};

class TamguPredicateRuleItem : public TamguTracked {
public:
	Tamgu* item;
	long position;
	bool negation;
	bool disjunction;

	TamguPredicateRuleItem(TamguGlobal* g, Tamgu* parent, bool n, bool d, long i);

	short Type() {
		return a_predicateruleelement;
	}

	void Setdisjunction(bool v) {
		item->Setdisjunction(v);
		disjunction = v;
	}

	bool isNegation() {
		return negation;
	}

	bool isDisjunction() {
		return disjunction;
	}

    Tamgu* Unify(Tamgu* context, TamguDeclaration* dom, basebin_hash<TamguPredicateVariableInstance*>* dico, short idthread);
};

class TamguPredicateLocalInstruction : public TamguReference {
public:
	basebin_hash<TamguPredicateVariableInstance*>* dico;
	Tamgu* instruction;
	bool negation;
	bool success;
	bool disjunction;
    bool keep;


	TamguPredicateLocalInstruction(TamguGlobal* g, basebin_hash<TamguPredicateVariableInstance*>* context, Tamgu* e, bool n, bool d)  {
		dico = context;
		negation = n;
		disjunction = d;
		instruction = e;
		success = false;
	}

	short Type() {
		return a_predicateevaluate;
	}
    
    bool isThread() {
        return instruction->isThread();
    }

	basebin_hash<TamguPredicateVariableInstance*>* Dico() {
		return dico;
	}

	void Setdico(short n, TamguPredicateVariableInstance* a) {
		(*dico)[n] = a;
	}

	TamguPredicateVariableInstance* Getdico(short n) {
		return (*dico)[n];
	}

	bool Checkdico(short n) {
		return dico->check(n);
	}

    bool checkDisjunction() {
        return disjunction;
    }

	void Setdisjunction(bool v) {
		disjunction = v;
	}


	bool hasbeenSuccesfull() {
		return success;
	}

	void setSucessfull(bool v) {
		success = v;
	}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	short Action() {
		return instruction->Action();
	}

	short Name() {
		return instruction->Name();
	}

	string String();
};


#endif






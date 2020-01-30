/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : instructions.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef instructions_h
#define instructions_h

#include "tamgushort.h"
#include "tamguint.h"
#include "tamgudecimal.h"
#include "tamgufloat.h"
#include "tamgulong.h"

class Tamgudecimal;

class Tamgutamgu;

#define DIVIDEDBYZERO -101

//--------------------------------------------------------------------
class TamguDeclarationLocal : public TamguTracked {
public:

	VECTE<short> names;
	VECTE<Tamgu*> declarations;

    short idx;
    short idthread;
    short i;
    bool pushed;
    bool used;

    TamguDeclarationLocal(short ix) : idx(ix), used(false), pushed(false), idthread(-1), TamguTracked(NULL) {
        investigate = is_declaration;
    }

	bool hasDeclaration() {
		return true;
	}

	bool isEmpty() {
		if (!declarations.last)
			return true;
		return false;
	}

	bool isDeclared(short id) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id)
				return true;
		}

		return false;
	}

    void Replacedeclaration(short idthread, short id, Tamgu* a) {
        for (i = 0; i < names.last; i++) {
            if (names.vecteur[i] == id) {
                declarations.vecteur[i] = a;
                break;
            }
        }
        globalTamgu->Replacevariable(idthread, id, a);
    }

    char Declarelocal(short idthread, short n, Tamgu* a) {
        if (isDeclared(n))
            return a_declaration;
        Declare(n, a);
        return true;
    }

	void Variables(vector<short>& vars) {
		for (i = 0; i < names.last; i++)
			vars.push_back(names[i]);
	}

	void Declare(short id, Tamgu* a) {
		names.push_back(id);
		declarations.push_back(a);
	}

	Tamgu* Declaration(short id) {
		for (i = 0; i < names.last; i++) {
			if (names.vecteur[i] == id)
				return declarations[i];
		}
		return NULL;
	}


	void Cleaning() {
		for (i = 0; i < names.last; i++) {
			declarations.vecteur[i]->Resetreference();
			globalTamgu->Removevariable(idthread, names.vecteur[i]);
		}

		names.last = 0;
		declarations.last = 0;
	}

    void Release() {
        Cleaning();
        if (pushed)
            globalTamgu->Popstack(idthread);
        pushed = false;
        idthread = -1;
        if (used) {
            used = false;
            if (!globalTamgu->globalLOCK)
                globalTamgu->declempties.push_back(idx);
            return;
        }
        delete this;
    }

    void Releasing() {
        Cleaning();
        if (pushed)
            globalTamgu->Popstack(idthread);
        pushed = false;
        idthread = -1;
        if (used) {
            used = false;
            if (!globalTamgu->globalLOCK)
                globalTamgu->declempties.push_back(idx);
            return;
        }
        delete this;
    }
    

    void Info(Tamgu* ins, short idt, bool p) {
        idthread = idt;
        if (p) {
            globalTamgu->Pushstack(this, idthread);
            pushed = p;
        }
        
        if (globalTamgu->debugmode && ins->isTracked()) {
            idinfo = ins->Currentinfo();
        }
    }

};

//--------------------------------------------------------------------
//This function call is used to call methods associated to objects such as i.log()
class TamguCallMethod : public TamguCallClean {
public:
    
    bool Checkarity();
    TamguCallMethod(short m, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallClean(m, a_callmethod, global, parent) {}
    
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* object, short idthread);
    Exporting Tamgu* Put(Tamgu* context, Tamgu* object, short idthread);
    
    short Typeinfered() {
        if (function != NULL)
            return function->Typevariable();
        
        if (!globalTamgu->returntypes.check(name))
            return a_none;
        
        return globalTamgu->returntypes[name];
    }
    
};


class TamguCallFromCall : public TamguCallMethod {
public:

	TamguCallFromCall(short m, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallMethod(m, global, parent) {}

	Exporting Tamgu* Eval(Tamgu* context, Tamgu* object, short idthread);
    Exporting Tamgu* Put(Tamgu* context, Tamgu* object, short idthread);

};

class TamguCallCommonMethod : public TamguCallFromCall {
public:

	bool Checkarity();

	TamguCallCommonMethod(short m, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallFromCall(m, global, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* object, short idthread);

	Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread) {
		return TamguCallFromCall::Eval(environment, value, idthread);
	}

};

//This function call is used to call user declared functions
class TamguCallFunction : public TamguCall {
public:
	Tamgu* body;
	bool nonlimited;

	Exporting bool Checkarity();

	TamguCallFunction(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : body(b), TamguCall(a_callfunction, global, parent) {
		nonlimited = false;
		if (b != NULL)
			name = b->Name();
	}

    TamguCallFunction(TamguCallFunction& c) : TamguCall(a_callfunction) {
        body=c.body;
        nonlimited=c.nonlimited;
        name=c.name;
        arguments=c.arguments;
    }
    
	Exporting virtual Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);

	short Name() {
		return name;
	}

	Tamgu* Body(short idthread) {
		return body->Body(idthread);
	}

	short Typeinfered() {
		if (body != NULL)
			return body->Typeinfered();
		return a_none;
	}
};

class TamguCallFunction0 : public TamguCallFunction {
public:
    TamguCallFunction0(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallFunction(b, global, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
};

class TamguCallFunction1 : public TamguCallFunction {
public:
    TamguCallFunction1(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallFunction(b, global, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
};

class TamguCallFunction2 : public TamguCallFunction {
public:
    TamguCallFunction2(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallFunction(b, global, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
};

class TamguCallFunction3 : public TamguCallFunction {
public:
    TamguCallFunction3(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallFunction(b, global, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
};


class TamguCallFunction4 : public TamguCallFunction {
public:
    TamguCallFunction4(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallFunction(b, global, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
};


class TamguCallFunction5 : public TamguCallFunction {
public:
    TamguCallFunction5(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallFunction(b, global, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
};


class TamguFunctionDeclarationCall : public TamguCallFunction {
public:

	TamguFunctionDeclarationCall(short n, TamguGlobal* global, Tamgu* parent) : TamguCallFunction(NULL, global, parent) { name = n; }
	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread) {
		body = globalTamgu->Getdeclaration(name, idthread);
		body = body->Body(idthread);
		if (body == NULL)
			return globalTamgu->Returnerror("Cannot execute this function", idthread);
		return TamguCallFunction::Eval(context, callfunction, idthread);
	}

};


class TamguCallThread : public TamguCall {
public:
	Tamgu* body;
	Tamgu* recipient;
	bool nonlimited;

	bool Checkarity();
	TamguCallThread(Tamgu* b, TamguGlobal* global = NULL, Tamgu* parent = NULL) : 
		nonlimited(false),recipient(NULL), body(b), TamguCall(a_callthread, global, parent) {}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);

	short Name() {
		return body->Name();
	}

	Tamgu* Body(short idthread) {
		return body->Body(idthread);
	}

	Tamgu* Push(Tamgu* v) {
		recipient = v;
		recipient->Setreference();
		return aTRUE;
	}
};


//This function call is used to call user declared functions
class TamguThreadCall : public TamguCallFunction {
public:

    TamguGlobal* global;

    Tamgu* domain;
	Tamgu* recipient;
    std::atomic<bool> joined;

    long idomain;
	short parentid;
	short idthread;
	bool cleandom;
	bool exclusive;
	std::thread* thid;
	threadhandle tid;

	TamguThreadCall(Tamgu* b, Tamgu* r, Tamgu* d, bool c, bool e, short id, short pid) : TamguCallFunction(b) {
        
        global = globalTamgu;

        recipient = r;
		parentid = pid;
		joined = false;
		idtype = a_callthread;
		domain = d;
		idthread = id;
		cleandom = c;
		exclusive = e;
		idomain = -1;
		if (cleandom)
			idomain = d->Setprotect();
	}

	Tamgu* Eval(Tamgu* domain, Tamgu* value, short idthread);

	short Name() {
		return body->Name();
	}

};

//This function call is used to call user declared functions
class TamguCallFrameFunction : public TamguCallClean {
public:
    TamguFrame* frame;
    
    bool Checkarity();
    
    TamguCallFrameFunction(TamguFrame* f, short n, TamguGlobal* global = NULL, Tamgu* parent = NULL) :
    frame(f), TamguCallClean(n, a_callfunction, global, parent) {}
    
	TamguCallFrameFunction(short n) : frame(NULL), TamguCallClean(n, a_callfunction) {}

    virtual Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
	virtual Tamgu* Put(Tamgu* context, Tamgu* domain, short idthread);

    Tamgu* Frame() {
        return frame;
    }

	short Typeinfered() {
		if (function != NULL)
			return function->Typeinfered();

		return frame->Declaration(name)->Typeinfered();
	}

};

//This function call is used to call user declared functions
class TamguCallTopFrameFunction : public TamguCallFrameFunction {
public:
    
    TamguCallTopFrameFunction(TamguFrame* f, short n, TamguGlobal* global = NULL, Tamgu* parent = NULL) :
    TamguCallFrameFunction(f,n, global, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);
	Tamgu* Put(Tamgu* context, Tamgu* domain, short idthread);

    Tamgu* Frame() {
        return frame;
    }
};

//This function call is used to call procedures such as print, println etc...
class TamguCallProcedure : public TamguCallClean {
public:

	bool Checkarity();

	TamguCallProcedure(short m, TamguGlobal* global = NULL, Tamgu* parent = NULL) : TamguCallClean(m, a_callprocedure, global, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	short Typeinfered() {
		if (globalTamgu->returntypes.check(name))
			return globalTamgu->returntypes[name];
		return a_none;
	}

};

class TamguCallReturn : public TamguTracked {
public:
	Tamgu* argument;

	TamguCallReturn(TamguGlobal* global = NULL, Tamgu* parent = NULL) : argument(aNOELEMENT), TamguTracked(a_return, global, parent) {
        investigate = is_return;
    }

    Tamgu* DirectEval(Tamgu* context, Tamgu* v, short idthread) {
        globalTamgu->threads[idthread].returnvalue = argument->Eval(context, aNULL, idthread);
        return this;
    }
    
	virtual Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread) {
		globalTamgu->threads[idthread].returnvalue = argument->Eval(context, aNULL, idthread);
		return this;
	}

	virtual Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
		globalTamgu->threads[idthread].returnvalue = v;
		return aTRUE;
	}

	bool isCall() {
		return true;
	}

	bool isCallFunction() {
		return true;
	}

	long Size() {
		if (argument == NULL)
			return 0;
		return 1;
	}

	Tamgu* Argument(size_t i) {
		if (argument == aNOELEMENT)
			return NULL;

		return argument;
	}

	Tamgu* Returned(short idthread) {
		return globalTamgu->threads[idthread].returnvalue;
	}

	virtual void AddInstruction(Tamgu* a) {
		if (argument != aNOELEMENT)
			return;
		argument = a;
	}
};

//To handle aBREAKTRUE et aBREAKFALSE
class TamguCallBreak : public TamguCallReturn {
public:

	TamguCallBreak(Tamgu* a, TamguGlobal* global) : TamguCallReturn(global) {
		argument = a;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread) {
		globalTamgu->threads[idthread].returnvalue = argument;
		return this;
	}

    Tamgu* DirectEval(Tamgu* context, Tamgu* v, short idthread) {
        globalTamgu->threads[idthread].returnvalue = argument;
        return this;
    }

	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
		return aTRUE;
	}

	void AddInstruction() {}
};
//----------------------------------------------------------------------
//Variables are declarations, which are executed through a Put
class TamguVariableDeclaration : public TamguTracked {
public:
	Tamgu* initialization;
	Tamgu* function;

	short name;
	short typevariable;

	bool isprivate;
	bool choice;
	bool isframe;
	bool isconstant;

	TamguVariableDeclaration(TamguGlobal* g, short n, short t, bool p, bool c, Tamgu* parent);
	TamguVariableDeclaration(TamguGlobal* g, short n, short t, Tamgu* parent = NULL);

	//When we call this function, we actually will create an element of type value
	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	virtual Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
	bool Setvalue(Tamgu* index, Tamgu* value, short idthread, bool strict = false);

    bool Checkarity();
    
	bool isFrame() {
		return isframe;
	}

	bool isConstant() {
		return isconstant;
	}

    void Copiing(TamguVariableDeclaration* a) {
        initialization = a->initialization;
        function = a->function;
        Getinfo(a);
    }
    
    void ScanVariables(vector<short>& vars) {
        vars.push_back(name);
        if (initialization!=NULL)
            initialization->ScanVariables(vars);
    }

	bool isFunction() {
		//We can check whether it is a TamguFunctionCall object, then it can be a function
		if (typevariable == a_call)
			return true;
		return false;
	}

	Tamgu* Initialisation() {
		return initialization;
	}

	virtual void AddInstruction(Tamgu* a) {
		if (!choice)
			initialization = a;
		else
			function = a;
	}

	bool Private() {
		return isprivate;
	}

	short Typevariable() {
		return typevariable;
	}

	short Typeinfered() {
		return typevariable;
	}

	Tamgu* Frame() {
		if (!globalTamgu->newInstance.check(typevariable))
			return NULL;
		return globalTamgu->newInstance[typevariable]->Frame();
	}

	Tamgu* Function() {
		return function;
	}

    bool Setstopindex() {
        if (function !=NULL)
            function->Setstopindex();
        return false;
    }

	short Name() {
		return name;
	}
};

class TamguAtomicVariableDeclaration : public TamguVariableDeclaration {
public:
    Tamgu* constant;
    Tamgu* reference;
    char directcall;
    
    TamguAtomicVariableDeclaration(TamguGlobal* g, short n, short t, bool p, bool c, Tamgu* parent) : TamguVariableDeclaration(g,n,t,p,c,parent) {
        reference = globalTamgu->newInstance.get(typevariable);
        constant = aNULL;
        switch (typevariable) {
            case a_string:
                constant = aEMPTYSTRING;
                break;
            case a_ustring:
                constant = aEMPTYUSTRING;
                break;
            case a_decimal:
            case a_float:
                constant = aZEROPOINTZERO;
                break;
            case a_int:
            case a_long:
            case a_short:
                constant = aZERO;
        }
        
        directcall=false;
    }
    TamguAtomicVariableDeclaration(TamguGlobal* g, short n, short t, Tamgu* parent = NULL) : TamguVariableDeclaration(g,n,t,parent) {
        reference = globalTamgu->newInstance.get(typevariable);
        directcall=false;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);

    void AddInstruction(Tamgu* a) {
        if (!choice) {
            initialization = a;
            directcall = true;
            if (initialization->isConst())
                directcall = 2;
        }
        else
            function = a;
    }
};

class TamguTaskellVariableDeclaration : public TamguVariableDeclaration {
public:
    char directcall;

    TamguTaskellVariableDeclaration(TamguGlobal* g, short n, short t, bool p, bool c, Tamgu* parent) : directcall(1),TamguVariableDeclaration(g, n, t, p, c, parent) {}
    TamguTaskellVariableDeclaration(TamguGlobal* g, short n, short t, Tamgu* parent = NULL) : directcall(1),TamguVariableDeclaration(g, n, t, parent) {}

    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguTaskellSelfVariableDeclaration : public TamguVariableDeclaration {
public:
    char directcall;
    
    TamguTaskellSelfVariableDeclaration(TamguGlobal* g, short n, short tid = a_self, Tamgu* parent = NULL) : directcall(1),TamguVariableDeclaration(g, n, tid, false, false, parent) {}
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);

    bool Computevariablecheck(short idthread) {
        if (initialization == NULL)
            return true;
        return initialization->Computevariablecheck(idthread);
    }
};

class TamguFrameVariableDeclaration : public TamguVariableDeclaration {
public:
	bool common;

	TamguFrameVariableDeclaration(TamguGlobal* g, short n, short t, bool p, bool c, Tamgu* parent) : common(c), TamguVariableDeclaration(g, n, t, p, false, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

};

class TamguFrameAtomicVariableDeclaration : public TamguAtomicVariableDeclaration {
public:
    bool common;

    TamguFrameAtomicVariableDeclaration(TamguGlobal* g, short n, short t, bool p, bool c, Tamgu* parent) : common(c), TamguAtomicVariableDeclaration(g, n, t, p, false, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguGlobalVariableDeclaration : public TamguVariableDeclaration {
public:
	bool alreadydeclared;

	TamguGlobalVariableDeclaration(TamguGlobal* g, short n, short t, bool p, bool c, Tamgu* parent) : alreadydeclared(false), TamguVariableDeclaration(g, n, t, p, c, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    bool isGlobalVariable() {
        return true;
    }

};

class TamguThroughVariableDeclaration : public TamguVariableDeclaration {
public:
    string sname;
    string tname;

	TamguThroughVariableDeclaration(short n, short t, string sn, string tn, Tamgu* parent) : sname(sn), tname(tn), TamguVariableDeclaration(NULL, n, t, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	void Resetreference(short i = 1) {}

    bool Candelete() {
        return false;
    }

};

class TamguSystemVariable : public TamguVariableDeclaration {
public:

	Tamgu* value;
	Exporting TamguSystemVariable(TamguGlobal* g, Tamgu* v, short n, short t);

    Tamgu* Atom(bool forced = false) {
        return value->Newvalue(value, 0);
    }

    Tamgu* plus(Tamgu* a, bool itself) {
        return value->plus(a, false);
    }
    
    Tamgu* minus(Tamgu* a, bool itself) {
        return value->minus(a, false);
    }
    
    Tamgu* multiply(Tamgu* a, bool itself) {
        return value->multiply(a, true);
    }

    Tamgu* divide(Tamgu* a, bool itself) {
        return value->divide(a, true);
    }

    Tamgu* power(Tamgu* a, bool itself) {
        return value->power(a, true);
    }

    Tamgu* shiftleft(Tamgu* a, bool itself) {
        return value->shiftleft(a, true);
    }

    Tamgu* shiftright(Tamgu* a, bool itself) {
        return value->shiftright(a, true);
    }

    Tamgu* mod(Tamgu* a, bool itself) {
        return value->mod(a, true);
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

class TamguSelfVariableDeclaration : public TamguVariableDeclaration {
public:

	TamguSelfVariableDeclaration(TamguGlobal* g, short n, short tid = a_self, Tamgu* parent = NULL) : TamguVariableDeclaration(g, n, tid, false, false, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);

    short Checklettype() {
        //The type of a let variable is set at the beginning
        if (typevariable == a_let && initialization != NULL) {
            short ty = initialization->Typevariable();
            //We only accept basic instantiation...
            if (!globalTamgu->returntypes.check(ty)) {
                ty = initialization->Typeinfered();
                if (ty == a_none || !globalTamgu->returntypes.check(ty) || globalTamgu->frames.check(ty))
                    return a_let;
            }

            switch (ty) {
                case a_short:
                    return a_int;
                case a_decimal:
                    return a_float;
                case a_constvector:
                    return a_vector;
                case a_constmap:
                    return a_map;
            }
            
            return ty;
        }
        
        return typevariable;
    }
};

//------------------------------------------------
class TamguCallVariable : public TamguTracked {
public:
	Tamgu* call;

	short name;
	short typevariable;

	bool affectation;
	bool forced;
    bool directcall;
	
	TamguCallVariable(short n, short idt, TamguGlobal* g = NULL, Tamgu* parent = NULL) : directcall(false), forced(false), affectation(false), typevariable(idt), name(n), call(NULL), TamguTracked(a_variable, g, parent) {
        investigate = is_callvariable;
    }

	void Setforced(bool v) {
		forced = v;
	}
    
    bool directCall() {
        return directcall;
    }

	virtual short Action() {
		if (call != NULL)
			return a_none;
		return a_variable;
	}
	
	bool isFloat() {
		if (typevariable == a_float || typevariable == a_decimal)
			return true;
		return false;
	}

	bool isString() {
		if (typevariable == a_string || typevariable == a_ustring)
			return true;
		return false;
	}

	bool isNumber() {
		switch (typevariable) {
		case a_int:
		case a_long:
		case a_short:
		case a_decimal:
		case a_float:
			return true;
		}
		return false;
	}

    void ScanVariables(vector<short>& vars) {
        vars.push_back(name);
        if (call != NULL)
            call->ScanVariables(vars);
    }

	bool isFunction() {
		if (typevariable == a_call)
			return true;
		return false;
	}

	void AddInstruction(Tamgu* a) {
		if (call == NULL)
			call = a;
		else
			call->AddInstruction(a);
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

	short Typevariable() {
		return typevariable;
	}

	short Typeinfered() {
		if (call != NULL) {
			if (call->isIndex() || call->isIncrement())
				return a_none;
			return call->Typeinfered();
		}
		return typevariable;
	}

	bool isPureCallVariable() {
		if (call == NULL)
			return true;
		return false;
	}

	bool isAssignable() {
		return true;
	}

	bool Checkvariable() {
		return true;
	}

	bool isFrame() {
        return globalTamgu->frames.check(typevariable);
    }

	Tamgu* Frame() {
        if (globalTamgu->frames.check(typevariable))
            return globalTamgu->frames.get(typevariable);
        return NULL;
	}

	Tamgu* Function() {
		return call;
	}
    
    bool Setstopindex() {
        if (call !=NULL)
            call->Setstopindex();
        return false;
    }

    Tamgu* Getindex() {
        if (call != NULL)
            return call->Getindex();
        return NULL;
    }

	short Name() {
		return name;
	}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	virtual Tamgu* Put(Tamgu* context, Tamgu* value, short idthread);

	Tamgu* Body(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Body(idthread);
	}

	virtual long Getinteger(short idthread) {
		if (call == NULL)
			return globalTamgu->threads[idthread].variables.get(name).back()->Integer();

		Tamgu* a = Eval(aNULL, aNULL, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	virtual BLONG Getlong(short idthread) {
		if (call == NULL)
			return globalTamgu->threads[idthread].variables.get(name).back()->Long();

		Tamgu* a = Eval(aNULL, aNULL, idthread);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	virtual short Getshort(short idthread) {
		if (call == NULL)
			return globalTamgu->threads[idthread].variables.get(name).back()->Short();

		Tamgu* a = Eval(aNULL, aNULL, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	virtual float Getdecimal(short idthread) {
		if (call == NULL)
			return globalTamgu->threads[idthread].variables.get(name).back()->Decimal();

		Tamgu* a = Eval(aNULL, aNULL, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	virtual double Getfloat(short idthread) {
		if (call == NULL)
			return globalTamgu->threads[idthread].variables.get(name).back()->Float();

		Tamgu* a = Eval(aNULL, aNULL, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	virtual string Getstring(short idthread) {
		if (call == NULL)
			return globalTamgu->threads[idthread].variables.get(name).back()->String();

		Tamgu* a = Eval(aNULL, aNULL, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	virtual wstring Getustring(short idthread) {
		if (call == NULL)
			return globalTamgu->threads[idthread].variables.get(name).back()->UString();

		Tamgu* a = Eval(aNULL, aNULL, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}

    virtual Tamgu* plus(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->plus(a, itself);
    }

    virtual Tamgu* minus(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->minus(a, itself);
    }

    virtual Tamgu* multiply(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->multiply(a, itself);
    }

    virtual Tamgu* divide(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->divide(a, itself);
    }

    virtual Tamgu* power(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->power(a, itself);
    }

    virtual Tamgu* shiftleft(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->shiftleft(a, itself);
    }

    virtual Tamgu* shiftright(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->shiftright(a, itself);
    }

    virtual Tamgu* mod(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->mod(a, itself);
    }

};
//----------------------------------------------------------------------
class TamguCallGlobalVariable : public TamguCallVariable {
public:

    Tamgu* value;
    uchar first;
    
    TamguCallGlobalVariable(short n, short idt, TamguGlobal* g = NULL, Tamgu* parent = NULL) : first(true), value(NULL), TamguCallVariable(n,idt,g,parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    bool isGlobalVariable() {
        return true;
    }
};

//----------------------------------------------------------------------
class TamguCallConstantVariable : public TamguCallVariable {
public:

	TamguCallConstantVariable(short n, short idt, TamguGlobal* g, Tamgu* parent) : TamguCallVariable(n, idt, g, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* Put(Tamgu* context, Tamgu* value, short idthread) {
		return aNULL;
	}

	long Getinteger(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Integer();
	}

	BLONG Getlong(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Long();
	}

	short Getshort(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Short();
	}

	float Getdecimal(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Decimal();
	}

	double Getfloat(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Float();
	}

	string Getstring(short idthread) {
		return Eval(aNULL, aNULL, idthread)->String();
	}

	wstring Getustring(short idthread) {
		return Eval(aNULL, aNULL, idthread)->UString();
	}

	short Action() {
		return a_none;
	}
    
    Tamgu* plus(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->plus(a, false);
    }

    Tamgu* minus(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->minus(a, false);
    }

    Tamgu* multiply(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->multiply(a, false);
    }

    Tamgu* divide(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->divide(a, false);
    }

    Tamgu* power(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->power(a, false);
    }

    Tamgu* shiftleft(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->shiftleft(a, false);
    }

    Tamgu* shiftright(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->shiftright(a, false);
    }

    Tamgu* mod(Tamgu* a, bool itself) {
        Tamgu* value = Eval(aNULL, aNULL, globalTamgu->GetThreadid());
        return value->mod(a, false);
    }
};

class TamguCallThroughVariable : public TamguCallVariable {
public:

    string sname;
	TamguCallThroughVariable(short n, short idt, string sn, Tamgu* parent = NULL) : sname(sn), TamguCallVariable(n, idt, NULL, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	long Getinteger(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Integer();
	}

	BLONG Getlong(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Long();
	}

	short Getshort(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Short();
	}

	float Getdecimal(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Decimal();
	}

	double Getfloat(short idthread) {
		return Eval(aNULL, aNULL, idthread)->Float();
	}

	string Getstring(short idthread) {
		return Eval(aNULL, aNULL, idthread)->String();
	}

	wstring Getustring(short idthread) {
		return Eval(aNULL, aNULL, idthread)->UString();
	}

	short Action() {
		return a_none;
	}


};

//----------------------------------------------------------------------

class TamguCallThis : public TamguCallVariable {
public:

	TamguCallThis(short idt, TamguGlobal* g, Tamgu* parent) : TamguCallVariable(a_this, idt, g, parent) {}

    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        return call->Eval(context, value, idthread);
    }

	long Getinteger(short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back()->Integer();
        Tamgu* value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = call->Eval(aNULL, value, idthread);
        long v = value->Integer();
        value->Release();
        return v;
	}

	BLONG Getlong(short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back()->Long();
        Tamgu* value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = call->Eval(aNULL, value, idthread);
        BLONG v = value->Long();
        value->Release();
        return v;
	}

	short Getshort(short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back()->Short();
        Tamgu* value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = call->Eval(aNULL, value, idthread);
        short v = value->Short();
        value->Release();
        return v;
	}

	float Getdecimal(short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back()->Decimal();
        Tamgu* value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = call->Eval(aNULL, value, idthread);
        float v = value->Decimal();
        value->Release();
        return v;
	}

	double Getfloat(short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back()->Float();
        Tamgu* value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = call->Eval(aNULL, value, idthread);
        double v = value->Float();
        value->Release();
        return v;
	}

	string Getstring(short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back()->String();
        Tamgu* value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = call->Eval(aNULL, value, idthread);
        string v = value->String();
        value->Release();
        return v;
	}

	wstring Getustring(short idthread) {
        if (call == NULL)
            return globalTamgu->threads[idthread].variables.get(typevariable).back()->UString();
        Tamgu* value = globalTamgu->threads[idthread].variables.get(typevariable).back();
        value = call->Eval(aNULL, value, idthread);
        wstring v = value->UString();
        value->Release();
        return v;
	}

	short Action() {
		return a_none;
	}

};

class TamguCallFrameVariable : public TamguCallVariable {
public:
	TamguFrame* frame;
    short frame_name, call_name;
    char nocall;

	TamguCallFrameVariable(short n, TamguFrame* f, short idt, TamguGlobal* g = NULL, Tamgu* parent = NULL) :
    frame(f), nocall(10), TamguCallVariable(n, idt, g, parent) {
        frame_name = f->Name();
    }

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* Put(Tamgu* context, Tamgu* value, short idthread);

	short Action() {
		return a_none;
	}

	long Getinteger(short idthread) {
        switch (nocall) {
            case 1:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Integer();
            case 2:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Integer();
            case 3: {
                Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                a = call->Eval(aNULL, a, idthread);
                long v = a->Integer();
                a->Release();
                return v;
            }
            case 4:
                return globalTamgu->Getdeclaration(frame_name, idthread)->Integer();
            case 10:
                if (call == NULL) {
                    if (frame_name ==  name) {
                        nocall = 4;
                        return globalTamgu->Getdeclaration(frame_name, idthread)->Integer();
                    }
                    nocall = 1;
                    return globalTamgu->Getframedefinition(frame_name, name, idthread)->Integer();
                }
                else
                    if (call->Function() == NULL && call->isCallVariable()) {
                        nocall = 2; call_name = call->Name();
                        return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Integer();
                    }
                    else
                        if (frame_name ==  name) {
                            nocall = 3;
                            Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                            a = call->Eval(aNULL, a, idthread);
                            long v = a->Integer();
                            a->Release();
                            return v;
                        }

                nocall=0;
        }

        Tamgu* a = globalTamgu->Getframedefinition(frame_name, name, idthread);
		a = call->Eval(aNULL, a, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	BLONG Getlong(short idthread) {
        switch (nocall) {
            case 1:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Long();
            case 2:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Long();
            case 3: {
                Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                a = call->Eval(aNULL, a, idthread);
                BLONG v = a->Long();
                a->Release();
                return v;
            }
            case 4:
                return globalTamgu->Getdeclaration(frame_name, idthread)->Long();
            case 10:
                if (call == NULL) {
                    if (frame_name ==  name) {
                        nocall = 4;
                        return globalTamgu->Getdeclaration(frame_name, idthread)->Long();
                    }
                    return globalTamgu->Getframedefinition(frame_name, name, idthread)->Long();
                }
                else
                    if (call->Function() == NULL && call->isCallVariable()) {
                        nocall = 2; call_name = call->Name();
                        return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Long();
                    }
                    else
                        if (frame_name ==  name) {
                            nocall = 3;
                            Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                            a = call->Eval(aNULL, a, idthread);
                            BLONG v = a->Long();
                            a->Release();
                            return v;
                        }
                nocall=0;
        }
        
        Tamgu* a = globalTamgu->Getframedefinition(frame_name, name, idthread);
        a = call->Eval(aNULL, a, idthread);
        BLONG v = a->Long();
        a->Release();
        return v;
	}

	short Getshort(short idthread) {
        switch (nocall) {
            case 1:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Short();
            case 2:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Short();
            case 3: {
                Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                a = call->Eval(aNULL, a, idthread);
                short v = a->Short();
                a->Release();
                return v;
            }
            case 4:
                return globalTamgu->Getdeclaration(frame_name, idthread)->Short();
            case 10:
                if (call == NULL) {
                    if (frame_name ==  name) {
                        nocall = 4;
                        return globalTamgu->Getdeclaration(frame_name, idthread)->Short();
                    }
                    return globalTamgu->Getframedefinition(frame_name, name, idthread)->Short();
                }
                else
                    if (call->Function() == NULL && call->isCallVariable()) {
                        nocall = 2; call_name = call->Name();
                        return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Short();
                    }
                    else
                        if (frame_name ==  name) {
                            nocall = 3;
                            Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                            a = call->Eval(aNULL, a, idthread);
                            short v = a->Short();
                            a->Release();
                            return v;
                        }
                nocall=0;
        }
		Tamgu* a = globalTamgu->Getframedefinition(frame_name, name, idthread);
		a = call->Eval(aNULL, a, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	float Getdecimal(short idthread) {
        switch (nocall) {
            case 1:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Decimal();
            case 2:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Decimal();
            case 3: {
                Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                a = call->Eval(aNULL, a, idthread);
                float v = a->Decimal();
                a->Release();
                return v;
            }
            case 4:
                return globalTamgu->Getdeclaration(frame_name, idthread)->Decimal();
            case 10:
                if (call == NULL) {
                    if (frame_name ==  name) {
                        nocall = 4;
                        return globalTamgu->Getdeclaration(frame_name, idthread)->Decimal();
                    }
                    return globalTamgu->Getframedefinition(frame_name, name, idthread)->Decimal();
                }
                else
                    if (call->Function() == NULL && call->isCallVariable()) {
                        nocall = 2; call_name = call->Name();
                        return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Decimal();
                    }
                    else
                        if (frame_name ==  name) {
                            nocall = 3;
                            Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                            a = call->Eval(aNULL, a, idthread);
                            float v = a->Decimal();
                            a->Release();
                            return v;
                        }
                nocall=0;
        }

		Tamgu* a = globalTamgu->Getframedefinition(frame_name, name, idthread);
		a = call->Eval(aNULL, a, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	double Getfloat(short idthread) {
        switch (nocall) {
            case 1:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Float();
            case 2:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Float();
            case 3: {
                Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                a = call->Eval(aNULL, a, idthread);
                double v = a->Float();
                a->Release();
                return v;
            }
            case 4:
                return globalTamgu->Getdeclaration(frame_name, idthread)->Float();
            case 10:
                if (call == NULL) {
                    if (frame_name ==  name) {
                        nocall = 4;
                        return globalTamgu->Getdeclaration(frame_name, idthread)->Float();
                    }
                    return globalTamgu->Getframedefinition(frame_name, name, idthread)->Float();
                }
                else
                    if (call->Function() == NULL && call->isCallVariable()) {
                        nocall = 2; call_name = call->Name();
                        return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->Float();
                    }
                    else
                        if (frame_name ==  name) {
                            nocall = 3;
                            Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                            a = call->Eval(aNULL, a, idthread);
                            double v = a->Float();
                            a->Release();
                            return v;
                        }
                nocall=0;
        }

		Tamgu* a = globalTamgu->Getframedefinition(frame_name, name, idthread);
		a = call->Eval(aNULL, a, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	string Getstring(short idthread) {
        switch (nocall) {
            case 1:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->String();
            case 2:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->String();
            case 3: {
                Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                a = call->Eval(aNULL, a, idthread);
                string v = a->String();
                a->Release();
                return v;
            }
            case 4:
                return globalTamgu->Getdeclaration(frame_name, idthread)->String();
            case 10:
                if (call == NULL) {
                    if (frame_name ==  name) {
                        nocall = 4;
                        return globalTamgu->Getdeclaration(frame_name, idthread)->String();
                    }
                    return globalTamgu->Getframedefinition(frame_name, name, idthread)->String();
                }
                else
                    if (call->Function() == NULL && call->isCallVariable()) {
                        nocall = 2; call_name = call->Name();
                        return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->String();
                    }
                    else
                        if (frame_name ==  name) {
                            nocall = 3;
                            Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                            a = call->Eval(aNULL, a, idthread);
                            string v = a->String();
                            a->Release();
                            return v;
                        }
                nocall=0;
        }

		Tamgu* a = globalTamgu->Getframedefinition(frame_name, name, idthread);
		a = call->Eval(aNULL, a, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	wstring Getustring(short idthread) {
        switch (nocall) {
            case 1:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->UString();
            case 2:
                return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->UString();
            case 3: {
                Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                a = call->Eval(aNULL, a, idthread);
                wstring v = a->UString();
                a->Release();
                return v;
            }
            case 4:
                return globalTamgu->Getdeclaration(frame_name, idthread)->UString();
            case 10:
                if (call == NULL) {
                    if (frame_name ==  name) {
                        nocall = 4;
                        return globalTamgu->Getdeclaration(frame_name, idthread)->UString();
                    }
                    return globalTamgu->Getframedefinition(frame_name, name, idthread)->UString();
                }
                else
                    if (call->Function() == NULL && call->isCallVariable()) {
                        nocall = 2; call_name = call->Name();
                        return globalTamgu->Getframedefinition(frame_name, name, idthread)->Declaration(call_name)->UString();
                    }
                    else
                        if (frame_name ==  name) {
                            nocall = 3;
                            Tamgu* a = globalTamgu->Getdeclaration(frame_name, idthread);
                            a = call->Eval(aNULL, a, idthread);
                            wstring v = a->UString();
                            a->Release();
                            return v;
                        }
                nocall=0;
        }

		Tamgu* a = globalTamgu->Getframedefinition(frame_name, name, idthread);
		a = call->Eval(aNULL, a, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}

};

class TamguCallFromFrameVariable : public TamguCallVariable {
public:
	TamguCallFromFrameVariable(short n, short idt, TamguGlobal* g = NULL, Tamgu* parent = NULL) :
		TamguCallVariable(n, idt, g, parent) {}

    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* Put(Tamgu* context, Tamgu* value, short idthread);
    
	long Getinteger(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	BLONG Getlong(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	short Getshort(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	float Getdecimal(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	double Getfloat(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	string Getstring(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	wstring Getustring(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}
	short Action() {
		return a_none;
	}

};

class TamguCallTamguVariable : public TamguCallVariable {
public:
	Tamgutamgu* aa;

	TamguCallTamguVariable(short n, Tamgutamgu* f, TamguGlobal* g = NULL, Tamgu* parent = NULL);
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	short Action() {
		return a_none;
	}
    
    Tamgu* Declaration(short id);
    
    short Typevariable() {
        return a_tamgu;
    }
};

class TamguCallFunctionVariable : public TamguCallVariable {
public:

	TamguCallFunctionVariable(short n, short idt, TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguCallVariable(n, idt, g, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    bool isFunctionVariable() {
        return true;
    }
};

class TamguCallSelfVariable : public TamguCallVariable {
public:

    TamguCallSelfVariable(short n, short tid, TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguCallVariable(n, tid, g, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	Tamgu* Put(Tamgu* context, Tamgu* value, short idthread);

	long Getinteger(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		long v = a->Integer();
		a->Release();
		return v;
	}

	BLONG Getlong(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		BLONG v = a->Long();
		a->Release();
		return v;
	}

	short Getshort(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		short v = a->Short();
		a->Release();
		return v;
	}

	float Getdecimal(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		float v = a->Decimal();
		a->Release();
		return v;
	}

	double Getfloat(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		double v = a->Float();
		a->Release();
		return v;
	}

	string Getstring(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		string v = a->String();
		a->Release();
		return v;
	}

	wstring Getustring(short idthread) {
		Tamgu* a = Eval(aNULL, aNULL, idthread);
		wstring v = a->UString();
		a->Release();
		return v;
	}

};

class TamguCallSystemVariable : public TamguCallVariable {
public:

	TamguCallSystemVariable(short n, short t, TamguGlobal* g, Tamgu* parent) : TamguCallVariable(n, t, g, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
		value = globalTamgu->systems[name]->value;

		if (call != NULL)
			return call->Eval(context, value, idthread);

		return value;
	}

	long Getinteger(short idthread) {
		Tamgu* value = globalTamgu->systems[name]->value;

        if (call != NULL)
			value = call->Eval(aNULL, value, idthread);

		return value->Integer();
	}

	BLONG Getlong(short idthread) {
		Tamgu* value = globalTamgu->systems[name]->value;

        if (call != NULL)
            value = call->Eval(aNULL, value, idthread);

		return value->Long();
	}

	short Getshort(short idthread) {
		Tamgu* value = globalTamgu->systems[name]->value;

        if (call != NULL)
            value = call->Eval(aNULL, value, idthread);

		return value->Short();
	}

	float Getdecimal(short idthread) {
		Tamgu* value = globalTamgu->systems[name]->value;

        if (call != NULL)
            value = call->Eval(aNULL, value, idthread);

		return value->Decimal();
	}

	double Getfloat(short idthread) {
		Tamgu* value = globalTamgu->systems[name]->value;

        if (call != NULL)
            value = call->Eval(aNULL, value, idthread);

		return value->Float();
	}

	string Getstring(short idthread) {
		Tamgu* value = globalTamgu->systems[name]->value;

        if (call != NULL)
            value = call->Eval(aNULL, value, idthread);

		return value->String();
	}

	wstring Getustring(short idthread) {
		Tamgu* value = globalTamgu->systems[name]->value;

        if (call != NULL)
            value = call->Eval(aNULL, value, idthread);

		return value->UString();
	}

	short Action() {
		return a_none;
	}

};

//----------------------------------------------------------------------
class TamguBreak : public TamguTracked {
public:

	TamguBreak(TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguTracked(a_instructions, g, parent) {
        investigate = is_break;
    }
    
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
		return aBREAK;
	}

};

class TamguContinue : public TamguTracked {
public:

	TamguContinue(TamguGlobal* g = NULL, Tamgu* parent = NULL) : TamguTracked(a_instructions, g, parent) {
        investigate = is_continue;
    }

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
		return aCONTINUE;
	}
};

//----------------------------------------------------------------------
class TamguInstructionAFFECTATION : public TamguInstruction {
public:

	TamguInstructionAFFECTATION(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	string String() {
		string v = instructions[0]->String();
		v += "=";
		v += instructions[1]->String();
		return v;
	}
};

class TamguInstructionSTREAM : public TamguInstructionAFFECTATION {
public:
    
    TamguInstructionSTREAM(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstructionAFFECTATION(g, parent) {
        idtype=a_stream;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionAtomicAFFECTATION : public TamguInstruction {
public:
    Tamgu* variable;
    short name;
    char directcall;
    
    TamguInstructionAtomicAFFECTATION(TamguGlobal* g, Tamgu* var, Tamgu* parent = NULL) : directcall(false), TamguInstruction(a_affectation, g, parent) {
        name = var->Name();
        variable = var;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    string String() {
        string v = instructions[0]->String();
        v += "=";
        v += instructions[1]->String();
        return v;
    }
};

class TamguInstructionGlobalVariableAFFECTATION : public TamguInstruction {
public:
    Tamgu* variable;
    short varname;
    bool first;
    
    TamguInstructionGlobalVariableAFFECTATION(TamguGlobal* g, short n, Tamgu* parent = NULL) : varname(n), variable(aNULL), first(true), TamguInstruction(a_affectation, g, parent) {}
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    string String() {
        string v = instructions[0]->String();
        v += "=";
        v += instructions[1]->String();
        return v;
    }
};

class TamguInstructionVariableAFFECTATION : public TamguInstruction {
public:
    
    TamguInstructionVariableAFFECTATION(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_affectation, g, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    string String() {
        string v = instructions[0]->String();
        v += "=";
        v += instructions[1]->String();
        return v;
    }
};

class TamguInstructionFunctionVariableAFFECTATION : public TamguInstruction {
public:
    short name;
    
    TamguInstructionFunctionVariableAFFECTATION(TamguGlobal* g, Tamgu* var, Tamgu* parent = NULL) : TamguInstruction(a_affectation, g, parent) {
        name = var->Name();
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    string String() {
        string v = instructions[0]->String();
        v += "=";
        v += instructions[1]->String();
        return v;
    }
};



//----------------------------------------------------------------------

class TamguAction : public Tamgu {
public:

	short action;

    TamguAction(TamguGlobal* g, short op) : action(op) {
        g->RecordInTracker(this);
    }
    

	TamguAction(short op) : action(op) {
		globalTamgu->RecordInTracker(this);
	}

	short Action() {
		return action;
	}

	string String() {
		return globalTamgu->Getsymbol(action);
	}

};

class TamguActionVariable : public Tamgu {
public:

	short name;
	short typevar;

	TamguActionVariable(short n, short t) : name(n), typevar(t) {
		globalTamgu->RecordInTracker(this);
	}

	short Name() {
		return name;
	}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back();
	}

	short Action() {
		return a_variable;
	}

	short Typeinfered() {
		return typevar;
	}

	short Type() {
		return a_actionvariable;
	}

    virtual long Integer() {
        return Getinteger(globalTamgu->GetThreadid());
    }
    
    virtual BLONG Long(short idthread) {
        return Getlong(globalTamgu->GetThreadid());
    }
    
    virtual short Short() {
        return Getshort(globalTamgu->GetThreadid());
    }
    
    virtual float Decimal() {
        return Getdecimal(globalTamgu->GetThreadid());
    }
    
    virtual double Float() {
        return Getfloat(globalTamgu->GetThreadid());
    }
    
    virtual string String() {
        return Getstring(globalTamgu->GetThreadid());
    }
    
    virtual wstring UString() {
        return Getustring(globalTamgu->GetThreadid());
    }

	virtual long Getinteger(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Integer();
	}

	virtual BLONG Getlong(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Long();
	}

	virtual short Getshort(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Short();
	}

	virtual float Getdecimal(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Decimal();
	}

	virtual double Getfloat(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Float();
	}

	virtual string Getstring(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->String();
	}

	virtual wstring Getustring(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->UString();
	}

    void Remove() {
        globalTamgu->RemoveFromTracker(idtracker);
        delete this;
    }

    Tamgu* update(short idthread);

};

class TamguActionVariableInt : public TamguActionVariable {
public:
    
    TamguActionVariableInt(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back();
    }
    
    long Getinteger(short idthread) {
        return ((Tamguint*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    BLONG Getlong(short idthread) {
        return ((Tamguint*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    short Getshort(short idthread) {
        return ((Tamguint*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    float Getdecimal(short idthread) {
        return ((Tamguint*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    double Getfloat(short idthread) {
        return ((Tamguint*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->UString();
    }
};

class TamguActionVariableShort : public TamguActionVariable {
public:
    
    TamguActionVariableShort(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back();
    }
    
    long Getinteger(short idthread) {
        return ((Tamgushort*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    BLONG Getlong(short idthread) {
        return ((Tamgushort*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    short Getshort(short idthread) {
        return ((Tamgushort*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    float Getdecimal(short idthread) {
        return ((Tamgushort*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    double Getfloat(short idthread) {
        return ((Tamgushort*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->UString();
    }
};

class TamguActionVariableDecimal : public TamguActionVariable {
public:
    
    TamguActionVariableDecimal(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back();
    }
    
    long Getinteger(short idthread) {
        return ((Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    BLONG Getlong(short idthread) {
        return ((Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    short Getshort(short idthread) {
        return ((Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    float Getdecimal(short idthread) {
        return ((Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    double Getfloat(short idthread) {
        return ((Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->UString();
    }
};

class TamguActionVariableFloat : public TamguActionVariable {
public:
    
    TamguActionVariableFloat(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back();
    }
    
    long Getinteger(short idthread) {
        return ((Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    BLONG Getlong(short idthread) {
        return ((Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    short Getshort(short idthread) {
        return ((Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    float Getdecimal(short idthread) {
        return ((Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    double Getfloat(short idthread) {
        return ((Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->UString();
    }
};

class TamguActionVariableString : public TamguActionVariable {
public:
    
    TamguActionVariableString(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back();
    }
    
    long Getinteger(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Integer();
    }
    
    BLONG Getlong(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Long();
    }
    
    short Getshort(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Short();
    }
    
    float Getdecimal(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Decimal();
    }
    
    double Getfloat(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Float();
    }
    
    string Getstring(short idthread) {
        return ((Tamgustring*)globalTamgu->threads[idthread].variables.get(name).back())->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->UString();
    }
};

class TamguActionVariableUString : public TamguActionVariable {
public:
    
    TamguActionVariableUString(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back();
    }
    
    long Getinteger(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Integer();
    }
    
    BLONG Getlong(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Long();
    }
    
    short Getshort(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Short();
    }
    
    float Getdecimal(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Decimal();
    }
    
    double Getfloat(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->Float();
    }
    
    wstring Getustring(short idthread) {
        return ((Tamguustring*)globalTamgu->threads[idthread].variables.get(name).back())->UString();
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->String();
    }
};

class TamguActionVariableLong : public TamguActionVariable {
public:
    
    TamguActionVariableLong(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back();
    }
    
    long Getinteger(short idthread) {
        return ((Tamgulong*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    BLONG Getlong(short idthread) {
        return ((Tamgulong*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    short Getshort(short idthread) {
        return ((Tamgulong*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    float Getdecimal(short idthread) {
        return ((Tamgulong*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    double Getfloat(short idthread) {
        return ((Tamgulong*)globalTamgu->threads[idthread].variables.get(name).back())->value;
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).back()->UString();
    }
};

class TamguActionLetVariable : public TamguActionVariable {
public:

	TamguActionLetVariable(short n, short t) : TamguActionVariable(n, t) {}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value();
	}

	long Getinteger(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value()->Integer();
	}

	BLONG Getlong(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value()->Long();
	}

	short Getshort(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value()->Short();
	}

	float Getdecimal(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value()->Decimal();
	}

	double Getfloat(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value()->Float();
	}

	string Getstring(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value()->String();
	}

	wstring Getustring(short idthread) {
		return globalTamgu->threads[idthread].variables.get(name).back()->Value()->UString();
	}

};


class TamguActionGlobalVariableInt : public TamguActionVariable {
public:
    
    bool first;
    Tamguint* val;
    
    TamguActionGlobalVariableInt(short n, short t) : TamguActionVariable(n,t), first(true) {}
    
    
    long Integer() {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Long() {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Short() {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Decimal() {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Float() {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string String() {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring UString() {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }

    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val;
    }
    
    long Getinteger(short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Getlong(short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Getshort(short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Getfloat(short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string Getstring(short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring Getustring(short idthread) {
        if (first) {
            val = (Tamguint*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }
};

class TamguActionGlobalVariableShort : public TamguActionVariable {
public:
    bool first;
    Tamgushort* val;
    
    TamguActionGlobalVariableShort(short n, short t) : TamguActionVariable(n,t), first(true) {}
    
    
    long Integer() {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Long() {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Short() {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Decimal() {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Float() {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string String() {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring UString() {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }

    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val;
    }
    
    long Getinteger(short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Getlong(short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Getshort(short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Getfloat(short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string Getstring(short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring Getustring(short idthread) {
        if (first) {
            val = (Tamgushort*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }
};

class TamguActionGlobalVariableDecimal : public TamguActionVariable {
public:
    
    bool first;
    Tamgudecimal* val;
    
    TamguActionGlobalVariableDecimal(short n, short t) : TamguActionVariable(n,t), first(true) {}
    
    
    long Integer() {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Long() {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Short() {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Decimal() {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Float() {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string String() {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring UString() {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val;
    }
    
    long Getinteger(short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Getlong(short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Getshort(short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Getfloat(short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string Getstring(short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring Getustring(short idthread) {
        if (first) {
            val = (Tamgudecimal*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }

};

class TamguActionGlobalVariableFloat : public TamguActionVariable {
public:
    
    bool first;
    Tamgufloat* val;
    
    TamguActionGlobalVariableFloat(short n, short t) : TamguActionVariable(n,t), first(true) {}
    
    long Integer() {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Long() {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Short() {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Decimal() {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Float() {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string String() {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring UString() {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val;
    }
    
    long Getinteger(short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Getlong(short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Getshort(short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Getfloat(short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string Getstring(short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring Getustring(short idthread) {
        if (first) {
            val = (Tamgufloat*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }

};


class TamguActionGlobalVariableLong : public TamguActionVariable {
public:
    bool first;
    Tamgulong* val;
    
    TamguActionGlobalVariableLong(short n, short t) : TamguActionVariable(n,t), first(true) {}
    
    long Integer() {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Long() {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Short() {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Decimal() {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Float() {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string String() {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring UString() {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val;
    }
    
    long Getinteger(short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    BLONG Getlong(short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    short Getshort(short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    double Getfloat(short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    string Getstring(short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return convertfromnumber(val->value);
    }
    
    wstring Getustring(short idthread) {
        if (first) {
            val = (Tamgulong*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return wconvertfromnumber(val->value);
    }

};

class TamguActionGlobalVariableString : public TamguActionVariable {
public:
    bool first;
    Tamgustring* val;
    
    TamguActionGlobalVariableString(short n, short t) : TamguActionVariable(n,t), first(true) {}
    
    long Integer() {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Integer();
    }
    
    BLONG Long() {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Long();
    }
    
    short Short() {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Short();
    }
    
    float Decimal() {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Decimal();
    }
    
    double Float() {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Float();
    }
    
    string String() {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    wstring UString() {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->UString();
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val;
    }
    
    long Getinteger(short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Integer();
    }
    
    BLONG Getlong(short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Long();
    }
    
    short Getshort(short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Short();
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Decimal();
    }
    
    double Getfloat(short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Float();
    }
    
    wstring Getustring(short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->UString();
    }
    
    string Getstring(short idthread) {
        if (first) {
            val = (Tamgustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        Locking _lock(val);
        return val->value;
    }
};

class TamguActionGlobalVariableUString : public TamguActionVariable {
public:
    bool first;
    Tamguustring* val;
    
    TamguActionGlobalVariableUString(short n, short t) : TamguActionVariable(n,t), first(true) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val;
    }
    
    long Integer() {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Integer();
    }
    
    BLONG Long() {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Long();
    }
    
    short Short() {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Short();
    }
    
    float Decimal() {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Decimal();
    }
    
    double Float() {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Float();
    }
    
    string String() {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->String();
    }
    
    wstring UString() {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[globalTamgu->GetThreadid()].variables.get(name).vecteur[1];
            first = false;
        }
        return val->value;
    }
    
    long Getinteger(short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Integer();
    }
    
    BLONG Getlong(short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Long();
    }
    
    short Getshort(short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Short();
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Decimal();
    }
    
    double Getfloat(short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->Float();
    }
    
    wstring Getustring(short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        Locking _lock(val);
        return val->value;
    }
    
    string Getstring(short idthread) {
        if (first) {
            val = (Tamguustring*)globalTamgu->threads[idthread].variables.get(name).vecteur[1];
            first = false;
        }
        return val->String();
    }
};

class TamguActionGlobalVariable : public TamguActionVariable {
public:
    
    TamguActionGlobalVariable(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1];
    }
    
    long Getinteger(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Integer();
    }
    
    BLONG Getlong(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Long();
    }
    
    short Getshort(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Short();
    }
    
    float Getdecimal(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Decimal();
    }
    
    double Getfloat(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Float();
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->UString();
    }
    
    Tamgu* update(short idthread) {
        switch(typevar) {
            case a_short:
                return new TamguActionGlobalVariableShort(name, typevar);
            case a_int:
                return new TamguActionGlobalVariableInt(name, typevar);
            case a_long:
                return new TamguActionGlobalVariableLong(name, typevar);
            case a_decimal:
                return new TamguActionGlobalVariableDecimal(name, typevar);
            case a_float:
                return new TamguActionGlobalVariableFloat(name, typevar);
            case a_string:
                return new TamguActionGlobalVariableString(name, typevar);
            case a_ustring:
                return new TamguActionGlobalVariableUString(name, typevar);
        }
        return this;
    }
};

class TamguActionGlobalLetVariable : public TamguActionVariable {
public:
    
    TamguActionGlobalLetVariable(short n, short t) : TamguActionVariable(n,t) {}
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value();
    }
    
    long Getinteger(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value()->Integer();
    }
    
    BLONG Getlong(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value()->Long();
    }
    
    short Getshort(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value()->Short();
    }
    
    float Getdecimal(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value()->Decimal();
    }
    
    double Getfloat(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value()->Float();
    }
    
    string Getstring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value()->String();
    }
    
    wstring Getustring(short idthread) {
        return globalTamgu->threads[idthread].variables.get(name).vecteur[1]->Value()->UString();
    }
};


class TamguInstructionAPPLYOPERATIONROOT : public TamguInstruction {
public:
	
	basebin_hash<TamguActionVariable*> variables;

	long size;
	uchar thetype;
	uchar head;
	uchar alls;
	bool sub;
	bool fraction;

	TamguInstructionAPPLYOPERATIONROOT(TamguGlobal* g, Tamgu* parent = NULL, short a_i = a_instructions) : thetype(0), head(0), alls(0), TamguInstruction(a_i, g, parent) {
		size = 0; 
		sub = false;
		fraction = false;
	}

	bool isROOTOPERATION() {
		return true;
	}

	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		TamguInstructionAPPLYOPERATIONROOT* root = new TamguInstructionAPPLYOPERATIONROOT(NULL);
		root->instructions = instructions;
		root->size = size;
		return root;
	}

	void Setsize() {
		size = instructions.size();		
	}

	long Size() {
		return size;
	}
	
	bool Stacking(Tamgu* ins, char top);
	
	Tamgu* Returnlocal(TamguGlobal* g, Tamgu* parent = NULL);

	string String() {
		string v;
		for (long i = size - 1; i >= 0; i--) {
			if (i < size - 1)
				v += " ";
			if (instructions[i] == this)
				v += globalTamgu->Getsymbol(action);
			else
				v += instructions[i]->String();
		}
		return v;
	}

	long Getinteger(short idthread) {
        Tamgu* r;
        if (size == 1)
            r = instructions[0]->Eval(aNULL, aNULL, idthread);
        else {
            short d = (short)size - 1;
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO) {
                globalTamgu->Errorobject(idthread);
                return 0;
            }
        }

		long w = r->Integer();
		r->Release();
		return w;
	}

	BLONG Getlong(short idthread) {
        Tamgu* r;
        if (size == 1)
            r = instructions[0]->Eval(aNULL, aNULL, idthread);
        else {
            short d = (short)size - 1;
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO) {
                globalTamgu->Errorobject(idthread);
                return 0;
            }
        }

		BLONG w = r->Long();
		r->Release();
		return w;
	}

	short Getshort(short idthread) {
        Tamgu* r;
        if (size == 1)
            r = instructions[0]->Eval(aNULL, aNULL, idthread);
        else {
            short d = (short)size - 1;
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO) {
                globalTamgu->Errorobject(idthread);
                return 0;
            }
        }

		short w = r->Short();
		r->Release();
		return w;
	}

	float Getdecimal(short idthread) {
        Tamgu* r;
        if (size == 1)
            r = instructions[0]->Eval(aNULL, aNULL, idthread);
        else {
            short d = (short)size - 1;
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO) {
                globalTamgu->Errorobject(idthread);
                return 0;
            }
        }

		float w = r->Decimal();
		r->Release();
		return w;
	}

	double Getfloat(short idthread) {
        Tamgu* r;
        if (size == 1)
            r = instructions[0]->Eval(aNULL, aNULL, idthread);
        else {
            short d = (short)size - 1;
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO) {
                globalTamgu->Errorobject(idthread);
                return 0;
            }
        }

		double w = r->Float();
		r->Release();
		return w;
	}

	string Getstring(short idthread) {
        Tamgu* r;
        if (size == 1)
            r = instructions[0]->Eval(aNULL, aNULL, idthread);
        else {
            short d = (short)size - 1;
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO) {
                globalTamgu->Errorobject(idthread);
                return "";
            }
        }

		string w = r->String();
		r->Release();
		return w;
	}

    wstring Getustring(short idthread) {
        Tamgu* r;
        if (size == 1)
            r = instructions[0]->Eval(aNULL, aNULL, idthread);
        else {
            short d = (short)size - 1;
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO) {
                globalTamgu->Errorobject(idthread);
                return L"";
            }
        }
        
		wstring w = r->UString();
		r->Release();
		return w;
	}

	Tamgu* Eval(Tamgu* res, Tamgu* r, short idthread) {
		bool store = true;
		if (r != aAFFECTATION)
			store = false;

        if (size == 1)
            r = instructions[0]->Eval(res, aNULL, idthread);
        else {
            short d = (short)size - 1;
            
            r = ccompute(idthread, true, d);
            
            if (d == DIVIDEDBYZERO)
                return globalTamgu->Errorobject(idthread);
        }
        
		if (store) {			
			res->Put(aNULL, r, idthread);
			r->Release();
			return res;
		}

		return r;
	}

	Tamgu* ccompute(short idthread, uchar top, short& d);

	bool isOperation() {
		return false;
	}

};

class TamguInstructionAPPLYOPERATIONEQU : public TamguInstructionAPPLYOPERATIONROOT {
public:

	Tamgu* recipient;
	Tamgu* instruction;

	TamguInstructionAPPLYOPERATIONEQU(TamguGlobal* g, Tamgu* parent = NULL) : recipient(aNULL), TamguInstructionAPPLYOPERATIONROOT(g, parent, a_instructionequ) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    Tamgu* update(uchar btype);

	bool isEQU() {
		return true;
	}
};

class TamguInstructionAPPLYOPERATION : public TamguInstruction {
public:
	Tamgu* root;
	bool subcontext;

	TamguInstructionAPPLYOPERATION(TamguGlobal* g, Tamgu* parent = NULL, short ins = a_instructions) :
		TamguInstruction(ins, g, parent), subcontext(false), root(NULL) {
	}

	void Subcontext(bool b) { subcontext = b; }

	bool Subcontext() {
		return subcontext;
	}

	bool isOperation() {
		return true;
	}

	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	virtual Tamgu* Compile(Tamgu* parent);
};

class TamguInstructionOperationIfnot : public TamguInstruction {
public:
    
    TamguInstructionOperationIfnot(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

};


//----------------------------------------------------------------------------------
class TamguPLUSPLUS : public TamguTracked {
public:

	TamguPLUSPLUS(TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_plusplus, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	bool isIncrement() {
		return true;
	}

	string String() {
		return "++";
	}
};

class TamguMINUSMINUS : public TamguTracked {
public:

	TamguMINUSMINUS(TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_minusminus, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	bool isIncrement() {
		return true;
	}

	string String() {
		return "--";
	}

};

class TamguSQUARE : public TamguTracked {
public:

	TamguSQUARE(TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_square, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	bool isIncrement() {
		return true;
	}

    wstring UString();
};

class TamguCallSQUARE : public TamguTracked {
public:
	short name;

	TamguCallSQUARE(TamguGlobal* g, short n, Tamgu* parent = NULL) : name(n), TamguTracked(a_square, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	bool isIncrement() {
		return true;
	}

    wstring UString();
};


class TamguCUBE : public TamguTracked {
public:

	TamguCUBE(TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_cube, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	bool isIncrement() {
		return true;
	}

    wstring UString();
};

class TamguCallCUBE : public TamguTracked {
public:
	short name;

	TamguCallCUBE(TamguGlobal* g, short n, Tamgu* parent = NULL) : name(n), TamguTracked(a_cube, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
	bool isIncrement() {
		return true;
	}

    wstring UString();
};

class TamguInstructionOR : public TamguInstruction {
public:
    
    TamguInstructionOR(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguInstructionXOR : public TamguInstruction {
public:
    
    TamguInstructionXOR(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguInstructionAND : public TamguInstruction {
public:

	TamguInstructionAND(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionDisjunction : public TamguInstruction {
public:

	TamguInstructionDisjunction(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionConjunction : public TamguInstruction {
public:

	TamguInstructionConjunction(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionIN : public TamguInstruction {
public:

	TamguInstructionIN(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguInstructionIF : public TamguInstruction {
public:

	TamguInstructionIF(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {
		idtype = a_if;
	}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isaIF() {
        return true;
    }

};

class TamguInstructionFOR : public TamguSequence {
public:

	TamguInstructionFOR(TamguGlobal* g, Tamgu* parent = NULL) : TamguSequence(g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionSWITCH : public TamguInstruction {
public:
	hmap<string, int> keys;
    Tamgu* call;
	char usekeys;


	TamguInstructionSWITCH(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {
        call = NULL;
		usekeys = 0;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionWHILE : public TamguSequence {
public:

	TamguInstructionWHILE(TamguGlobal* g, Tamgu* parent = NULL) : TamguSequence(g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionUNTIL : public TamguSequence {
public:

	TamguInstructionUNTIL(TamguGlobal* g, Tamgu* parent = NULL) : TamguSequence(g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFILEIN : public TamguInstruction {
public:

	TamguInstructionFILEIN(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

//-----------------------------------------------------------------------------------------------------------------------------
class TamguInstructionFORINRANGE : public TamguInstruction {
public:

	TamguInstructionFORINRANGE(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_forinrange, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

	Tamgu* ExecuteInteger(Tamguint* value, Tamgu* context, short idthread);
	Tamgu* ExecuteDecimal(Tamgudecimal* value, Tamgu* context, short idthread);
	Tamgu* ExecuteFloat(Tamgufloat* value, Tamgu* context, short idthread);
	Tamgu* ExecuteLong(Tamgulong* value, Tamgu* context, short idthread);
	Tamgu* ExecuteShort(Tamgushort* value, Tamgu* context, short idthread);

};

class TamguInstructionFORINRANGESHORT : public TamguTracked {
public:
	Tamgu* recipient;
	Tamgu* init;
	Tamgu* test;
	Tamgu* increment;
	Tamgu* instruction;

	TamguInstructionFORINRANGESHORT(Tamgu* r, TamguInstruction* i, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent) {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGEINTEGER : public TamguTracked {
public:
	Tamgu* recipient;
	Tamgu* init;
	Tamgu* test;
	Tamgu* increment;
	Tamgu* instruction;

	TamguInstructionFORINRANGEINTEGER(Tamgu* r, TamguInstruction* i, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGELONG : public TamguTracked {
public:
	Tamgu* recipient;
	Tamgu* init;
	Tamgu* test;
	Tamgu* increment;
	Tamgu* instruction;

	TamguInstructionFORINRANGELONG(Tamgu* r, TamguInstruction* i, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGEDECIMAL : public TamguTracked {
public:
	Tamgu* recipient;
	Tamgu* init;
	Tamgu* test;
	Tamgu* increment;
	Tamgu* instruction;

	TamguInstructionFORINRANGEDECIMAL(Tamgu* r, TamguInstruction* i, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGEFLOAT : public TamguTracked {
public:
	Tamgu* recipient;
	Tamgu* init;
	Tamgu* test;
	Tamgu* increment;
	Tamgu* instruction;

	TamguInstructionFORINRANGEFLOAT(Tamgu* r, TamguInstruction* i, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
		init = i->instructions.vecteur[0]->Instruction(1);
		test = i->instructions.vecteur[0]->Instruction(2);
		increment = i->instructions.vecteur[0]->Instruction(3);
		instruction = i->instructions.vecteur[1];
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

//------------------------------------------------------------------------------

class TamguInstructionFORINRANGECONSTSHORT : public TamguTracked {
public:
    Tamgu* recipient;
    Tamgu* instruction;
    short V, t, i;

    
    TamguInstructionFORINRANGECONSTSHORT(Tamgu* r, TamguInstruction* ins, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent) {
        V =  ins->instructions.vecteur[0]->Instruction(1)->Short();
        t = ins->instructions.vecteur[0]->Instruction(2)->Short();
        i = ins->instructions.vecteur[0]->Instruction(3)->Short();
        instruction = ins->instructions.vecteur[1];
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGECONSTINTEGER : public TamguTracked {
public:
    Tamgu* recipient;
    Tamgu* instruction;
    long V,t,i;
    
    TamguInstructionFORINRANGECONSTINTEGER(Tamgu* r, TamguInstruction* ins, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
        V =  ins->instructions.vecteur[0]->Instruction(1)->Integer();
        t = ins->instructions.vecteur[0]->Instruction(2)->Integer();
        i = ins->instructions.vecteur[0]->Instruction(3)->Integer();
        instruction = ins->instructions.vecteur[1];
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGECONSTLONG : public TamguTracked {
public:
    Tamgu* recipient;
    Tamgu* instruction;
    BLONG V,t,i;
    
    TamguInstructionFORINRANGECONSTLONG(Tamgu* r, TamguInstruction* ins, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
        V =  ins->instructions.vecteur[0]->Instruction(1)->Long();
        t = ins->instructions.vecteur[0]->Instruction(2)->Long();
        i = ins->instructions.vecteur[0]->Instruction(3)->Long();
        instruction = ins->instructions.vecteur[1];
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGECONSTDECIMAL : public TamguTracked {
public:
    Tamgu* recipient;
    Tamgu* instruction;
    float V,t,i;
    
    TamguInstructionFORINRANGECONSTDECIMAL(Tamgu* r, TamguInstruction* ins, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
        V =  ins->instructions.vecteur[0]->Instruction(1)->Decimal();
        t = ins->instructions.vecteur[0]->Instruction(2)->Decimal();
        i = ins->instructions.vecteur[0]->Instruction(3)->Decimal();
        instruction = ins->instructions.vecteur[1];
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINRANGECONSTFLOAT : public TamguTracked {
public:
    Tamgu* recipient;
    Tamgu* instruction;
    double V,t,i;

    TamguInstructionFORINRANGECONSTFLOAT(Tamgu* r, TamguInstruction* ins, TamguGlobal* g, Tamgu* parent = NULL) : recipient(r), TamguTracked(a_forinrange, g, parent)  {
        V =  ins->instructions.vecteur[0]->Instruction(1)->Float();
        t = ins->instructions.vecteur[0]->Instruction(2)->Float();
        i = ins->instructions.vecteur[0]->Instruction(3)->Float();
        instruction = ins->instructions.vecteur[1];
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};
//-----------------------------------------------------------------------------------------------------------------------------
class TamguInstructionFORVECTORIN : public TamguInstruction {
public:

	TamguInstructionFORVECTORIN(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguInstructionFORMAPIN : public TamguInstruction {
public:

	TamguInstructionFORMAPIN(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguInstructionFORIN : public TamguInstruction {
public:

	TamguInstructionFORIN(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionFORINVECTOR : public TamguInstructionFORIN {
public:

	TamguInstructionFORINVECTOR(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstructionFORIN(g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguInstructionFORINVALUECONTAINER : public TamguInstruction {
public:

	TamguInstructionFORINVALUECONTAINER(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};



class TamguInstructionTRY : public TamguInstruction {
public:

	TamguInstructionTRY(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};

class TamguInstructionCATCH : public TamguInstruction {
public:

	TamguInstructionCATCH(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {
		action = a_catchbloc;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


class TamguContent : public TamguReference {
public:
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	TamguIndex* idx;
	Tamgu* container;

	//---------------------------------------------------------------------------------------------------------------------

	TamguContent(TamguIndex* i, Tamgu* c) {
		//Do not forget your variable initialisation
		idx = i;
		container = c;
	}

	~TamguContent() {
		idx->Rollback();
	}

	Tamgu* Put(Tamgu* e, Tamgu* v, short idthread) {
		container->Put(idx, v, idthread);
		return aTRUE;
	}

	Tamgu* Atom(bool forced = false) {
		return this;
	}

    void Setreference(short r) {
        container->Setreference(r);
        TamguReference::Setreference(r);
    }
    
    void Setreference() {
        container->Setreference();
        TamguReference::Setreference();
    }
    
	void Resetreference(short r = 1) {
		container->Resetreference(r);
		TamguReference::Resetreference(r);
	}

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}
    
    bool isContent() {
        return true;
    }
};

#endif








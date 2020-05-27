/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutaskell.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgutaskell_h
#define tamgutaskell_h

class TamguCallReturn;
class TamguInstructionTaskellIF;

//----------------------------------------------------------------------------------
class TamguDeclarationAutoClean : public Tamgu {
public:

	VECTE<short> names;
	VECTE<Tamgu*> declarations;
    Tamgu* a;
    short idthread;
    short idx;
	short i;
    bool used;

    TamguDeclarationAutoClean(short ix) : idx(ix), idthread(-1), used(false) {}

	void Replacedeclaration(short id, Tamgu* a) {
		i = names.search(id);
		if (i != -1) {
			if (a != declarations.vecteur[i]) {
				if (declarations.vecteur[i]->isLetSelf())
					declarations.vecteur[i]->Putvalue(a, idthread);
				else {
					globalTamgu->Replacevariable(idthread, id, a);
					declarations.vecteur[i]->Resetreference();
					declarations.vecteur[i] = a;
					a->Setreference();
				}
			}
		}
		else {
			names.push_back(id);
			declarations.push_back(a);
		}
	}

    void Initializevariable(short idt) {
        for (i = 0; i < names.last; i++)
            globalTamgu->Storevariable(idt, names.vecteur[i], declarations.vecteur[i]);
        idthread = idt;
    }

    void Removevariable(short idt) {
        for (i = 0; i < names.last; i++)
            globalTamgu->Removevariable(idt, names.vecteur[i]);
    }
    
	void Declare(short id, Tamgu* a) {
        i = names.search(id);
        if (i != -1) {
            globalTamgu->Removevariable(idthread, id);
            declarations.vecteur[i]->Resetreference();
            declarations.vecteur[i] = a;
            return;
        }

		names.push_back(id);
		declarations.push_back(a);
	}

	bool hasDeclaration() {
		return true;
	}

    char Declarelocal(short idthread, short id, Tamgu* a) {
        if (names.search(id) != -1)
            return a_declaration;

        names.push_back(id);
        declarations.push_back(a);
        return true;
    }

    void Replacedeclaration(short idthread, short id, Tamgu* a) {
        i = names.search(id);
        declarations.vecteur[i] = a;
        globalTamgu->Replacevariable(idthread, id, a);
    }

	void Cleaning() {
		for (i = 0; i < names.last; i++) {
			declarations.vecteur[i]->Resetreference();
			globalTamgu->Removevariable(idthread, names.vecteur[i]);
		}

		names.last = 0;
		declarations.last = 0;
	}

    void Cleanenvironment() {
        for (i = 0; i < names.last; i++) {
            declarations.vecteur[i]->Resetreference();
            globalTamgu->Removevariable(idthread, names.vecteur[i]);
        }
        
        names.last = 0;
        declarations.last = 0;

        if (used) {
            used = false;
            if (!globalTamgu->threadMODE)
                globalTamgu->declarationcleanempties.push_back(idx);
            return;
        }
        delete this;
    }

    void Cleanforfibre() {
        //In this case, the variables are removed at each step...
        for (i = 0; i < names.last; i++)
            declarations.vecteur[i]->Resetreference();
        
        names.last = 0;
        declarations.last = 0;

        if (used) {
            used = false;
            if (!globalTamgu->threadMODE)
                globalTamgu->declarationcleanempties.push_back(idx);
            return;
        }
        delete this;
    }
    
    void Release() {
        Cleanenvironment();
    }
    
    Tamgu* Declaration(short id) {
        i = names.search(id);
        if (i != -1)
            return declarations.vecteur[i];
        return NULL;
    }

	bool isEmpty() {
		if (!names.last)
			return true;
		return false;
	}

	short Type() {
		return a_taskelldeclaration;
	}
    
    Tamgu* Declarationself(short id) {
        i = names.search(id);
        if (i == -1) {
            a = globalTamgu->Provideself();
            globalTamgu->Storevariable(idthread, id, a);
            a->Setreference();
            names.push_back(id);
            declarations.push_back(a);
            return a;
        }

        return declarations.vecteur[i];
    }
    
    Tamgu* Declarationvalue(short name, Tamgu* v, short typevariable) {
        i = names.search(name);
        if (i != -1) {
            a = declarations.vecteur[i];
            a->Putvalue(v, idthread);
            v->Releasenonconst();
            return a;
        }
        
        if (!v->Checktype(typevariable)) {
            a = globalTamgu->newInstance.get(typevariable)->Newinstance(idthread);
            a->Setreference(1);
            if (!v->isNULL()) {
                a->Putvalue(v, idthread);
                v->Releasenonconst();
            }
            names.push_back(name);
            declarations.push_back(a);
            globalTamgu->Storevariable(idthread, name, a);
            a->Setaffectation(true);
            return a;
        }
        
        names.push_back(name);
        declarations.push_back(v);
        globalTamgu->Storevariable(idthread, name, v);
        v->Setreference(1);
        v->Setaffectation(true);
        return v;
    }

    void Setidthread(short idt) {
        idthread = idt;
    }
};

//------------------------------------------------------------------------
class TamguInstructionHASKELLCASE : public TamguInstruction {
public:

	TamguInstructionHASKELLCASE(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_match, g, parent) {
		idtype = a_match;
	}

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
};


//------------------------------------------------------------------------
/// When a function, a method or a procedure are passed as parameter...
class TamguParameterFunction : public TamguTracked {
public:

	short name;

	TamguParameterFunction(short n, short ty, TamguGlobal* g, Tamgu* parent) : name(n), TamguTracked(ty, g, parent) {}

	short Name() {
		return name;
	}

	string String();
    void Setstring(string& v, short idthread);

	bool isFunctionParameter() {
		return true;
	}

	bool duplicateForCall() {
		return false;
	}

	short Typeinfered() {
		return a_calltaskell;
	}
};


class TamguFunctionTaskellParameter : public TamguParameterFunction {
public:


	TamguFunctionTaskellParameter(short n, TamguGlobal* g, Tamgu* parent) : TamguParameterFunction(n, a_calltaskell, g, parent) {}

	Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);
    Tamgu* Eval(Tamgu* environment, Tamgu* value, short idthread);

    bool isTaskellFunction() {
        return true;
    }

    long Size();
};


class TamguFunctionParameter : public TamguParameterFunction {
public:

	TamguFunctionParameter(short n, TamguGlobal* g, Tamgu* parent) : TamguParameterFunction(n, a_callfunction, g, parent) {}

	Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);

	Tamgu* Eval(Tamgu* context, Tamgu* val, short idthread);

};

class TamguMethodParameter : public TamguParameterFunction {
public:


	TamguMethodParameter(short n, TamguGlobal* g, Tamgu* parent) : TamguParameterFunction(n, a_callmethod, g, parent) {}

	Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);
    Tamgu* Eval(Tamgu* environment, Tamgu* value, short idthread);

};

class TamguOperatorParameter : public TamguParameterFunction {
public:
    VECTE<Tamgu*> instructions;
    short op;
    bool left;
    
    TamguOperatorParameter(bool lr, TamguGlobal* g, Tamgu* parent) : TamguParameterFunction(a_null, a_callmethod, g, parent) {
        left = lr;
        op = 0;
    }
    
    void Setaction(short a) {
        op = a;
    }
    
    void AddInstruction(Tamgu* a) {
        instructions.push_back(a);
    }

    Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);

};

class TamguFrameMethodParameter : public TamguParameterFunction {
public:


	TamguFrameMethodParameter(short n, TamguGlobal* g, Tamgu* parent) : TamguParameterFunction(n, a_callmethod, g, parent) {}

	Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);
    Tamgu* Eval(Tamgu* environment, Tamgu* value, short idthread);

};


class TamguCommonParameter : public TamguParameterFunction {
public:

	TamguCommonParameter(short n, TamguGlobal* g, Tamgu* parent) : TamguParameterFunction(n, a_callmethod, g, parent) {}

	Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);
    Tamgu* Eval(Tamgu* environment, Tamgu* value, short idthread);

};

class TamguProcedureParameter : public TamguParameterFunction {
public:


	TamguProcedureParameter(short n, TamguGlobal* g, Tamgu* parent) : TamguParameterFunction(n, a_callprocedure, g, parent) {}

	Tamgu* Execute(Tamgu* environment, Tamgu* value, short idthread);
    Tamgu* Eval(Tamgu* environment, Tamgu* value, short idthread);

};

//-------------------------------------------------------------------------------------------
class TamguGetMethod : public TamguArguments {
public:


	TamguGetMethod(short n, TamguGlobal* g, Tamgu* parent) : TamguArguments(n, a_self, g, parent) {}


	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);

};

class TamguGetCommon : public TamguArguments {
public:
    bool is_size;

	TamguGetCommon(short n, TamguGlobal* g, Tamgu* parent) : is_size(false), TamguArguments(n, a_self, g, parent) {}


	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);

    void AddInstruction(Tamgu* a) {
        if (name == a_size && a->isCallTaskellFunction())
            is_size=true;
        arguments.push_back(a);
    }

};

class TamguGetFunction : public TamguArguments {
public:


	TamguGetFunction(short n, TamguGlobal* g, Tamgu* parent) : TamguArguments(n, a_self, g, parent) {}


	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);

};

class TamguGetFunctionThrough : public TamguArguments {
public:
	Tamgu* call;

	TamguGetFunctionThrough(Tamgu* c, TamguGlobal* g, Tamgu* parent) : call(c), TamguArguments(c->Name(), a_call, g, parent) {}


	Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);

};


//-------------------------------------------------------------------------------------------
class TamguLambdaDomain : public TamguSequence {
public:
	short name;
	bool local;
	bool adding;

	TamguLambdaDomain(TamguGlobal* g = NULL, Tamgu* parent = NULL) : local(false), name(1), adding(true), TamguSequence(g, parent)  {
		name = 1;
	}

	void AddInstruction(Tamgu* a) {
		if (adding)
			instructions.push_back(a);
	}

};

class Taskelldeclaration{
public:
	short atype;
    bool maybe;

	Taskelldeclaration(short w, bool m) : atype(w), maybe(m) {}

	short Type() {
		return atype;
	}

	virtual void Push(short) {}

	virtual short Size() {
		return 0;
	}

	virtual short Typevariable(short i) { return a_none; }

	virtual Taskelldeclaration* copy() {
        return new Taskelldeclaration(atype, maybe);
	}
};

class SubTaskelldeclaration : public Taskelldeclaration {
public:
	vector<short> types;

	SubTaskelldeclaration(short w) : Taskelldeclaration(w, false) {}

	void Push(short t) {
		types.push_back(t);
	}

	short Size() {
		return types.size();
	}

	short Typevariable(short i) {
		return types[i];
	}

	Taskelldeclaration* copy() {
		SubTaskelldeclaration* sub=new SubTaskelldeclaration(atype);
		sub->types = types;
		return sub;
	}

};

class TamguFunctionLambda : public TamguFunction {
public:
    VECTE<Tamgu*> localvariables;
    VECTE<Tamgu*> wherevariables;
    VECTE<short> names;
    
	TamguLambdaDomain lambdadomain;	
	vector<Taskelldeclaration*> taskelldeclarations;
    TamguCallReturn* instructionreturn;
    TamguInstructionTaskellIF* instructionif;
    unsigned long constantparameters;
    short firstconstant;
    char returnonly;
	bool hdeclared, store, maybe;
    bool storage;

    TamguFunctionLambda(short n, TamguGlobal* g = NULL) : instructionreturn(NULL),returnonly(0), maybe(false), hdeclared(false), store(false), TamguFunction(n, g) {
		idtype = a_lambda;
        constantparameters = 0;
        firstconstant = -1;
        instructionif = NULL;
        storage = false;
	}

	~TamguFunctionLambda() {
		for (int i = 0; i < taskelldeclarations.size(); i++)
			delete taskelldeclarations[i];
	}

    inline void Checkreturnonly() {
        if (instructions.last == 1) {
            if (instructions[0]->investigate == is_return) {
                instructionreturn = (TamguCallReturn*)instructions[0];
                returnonly = 1;
                return;
            }
            
            if (instructions.vecteur[0]->Type() == a_iftaskell) {
                instructionif = (TamguInstructionTaskellIF*)instructions.vecteur[0];
                returnonly = 2;
                return;
            }
            returnonly = 3;
            return;
        }
        
        if (instructions.back()->investigate == is_return) {
            instructionreturn = (TamguCallReturn*)instructions.back();
            returnonly = 4;
            instructions.last--;
            return;
        }
        returnonly = 5;
    }
    
    void AddInstruction(Tamgu* a) {
        if (storage)
            return;
        
        if (choice == 0)
            parameters.push_back(a);
        else
            if (adding)
                instructions.push_back(a);
    }

	void settaskelldeclarations(vector<Taskelldeclaration*>& m) {
		for (int i = 0; i < m.size(); i++)
			taskelldeclarations.push_back(m[i]->copy());
	}

	TamguLambdaDomain* Thedomain() {
		return &lambdadomain;
	}

	TamguFunctionLambda* Following() {
		return (TamguFunctionLambda*)next;
	}

	bool isTaskellFunction() {
		return true;
	}

	Tamgu* DirectEval(Tamgu* context, Tamgu* callfunction, short idthread);	
    Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread);

	bool Puretaskelldeclaration() {
		if (instructions.size() == 0 && hdeclared)
			return true;
		return false;
	}

    bool checkconstant(short i) {
        if (i == firstconstant)
            return true;
        
        unsigned long a=1<<i;
        if ((constantparameters&a) == a)
            return true;
        return false;
    }
    
    void Constanttaskelldeclaration() {
        for (long i = 0; i < parameters.size(); i++) {
            if (!parameters[i]->Name() && !parameters[i]->isConstContainer() && parameters[i]->isConst()) {
                constantparameters |= 1 << i;
                if (firstconstant==-1)
                    firstconstant=i;
                return;
            }
        }
    }

    bool isDeclared(short idname) {
        if (names.check(idname))
            return true;
        
        return TamguFunction::isDeclared(idname);
    }
    
    void Declare(short idname, Tamgu* a) {
        if (storage) {
            long i = names.search(idname);
            if (i == -1) {
                localvariables.push_back(a);
                names.push_back(idname);
            }
            else
                localvariables.vecteur[i] = a;
        }
        else
            TamguFunction::Declare(idname, a);
    }
    
    Tamgu* Declaration(short idname) {
        long i = names.search(idname);
        if (i != -1)
            return localvariables[i];
        
        return TamguFunction::Declaration(idname);
    }

};

class TamguGetFunctionLambda : public TamguTracked {
public:

	TamguFunctionLambda* function;
	TamguGetFunctionLambda(TamguFunctionLambda* f, TamguGlobal* g) : function(f), TamguTracked(a_calltaskell, g) {}

	Tamgu* Eval(Tamgu* c, Tamgu* cc, short idthread) {
		return function;
	}
};

//-----------------------------------------------------------------------
class TaskellLoop {
public:
	vector<Tamgu*> recipients;
	vector<Tamgu*> vects;
	vector<TamguIteration*> loops;
	vector<Tamgu*> recreate;
	Tamgu* environment;
	TaskellLoop* next;
	short idthread;
    bool maprecipient;

	TaskellLoop(short id, Tamgu* e) : maprecipient(false), next(NULL), idthread(id), environment(e) {}

	~TaskellLoop() {
		for (int i = 0; i < loops.size(); i++) {
            if (loops[i] != NULL)
                loops[i]->Release();
			vects[i]->Release();
		}
		if (next != NULL)
			delete next;
	}

	void push(Tamgu* r, Tamgu* v, TamguIteration* l) {
		recipients.push_back(r);
		loops.push_back(l);
		if (l == aITERNULL) {
			recreate.push_back(v);
			vects.push_back(aNULL);
		}
		else {
			recreate.push_back(NULL);
			vects.push_back(v);
		}
	}

	void add(TaskellLoop* h) {
		TaskellLoop* a = this;
		while (a->next != NULL)
			a = a->next;
		a->next = h;
	}

	bool end() {
		for (size_t l = 0; l < loops.size(); l++) {
			if (loops[l]->End()->Boolean())
				return true;
		}
		return false;
	}
    
    void clean() {
        for (long i = 0; i < loops.size(); i++) {
            if (loops[i] != NULL) {
                loops[i]->Release();
                loops[i] = NULL;
            }
        }
        if (next != NULL)
            next->clean();
    }

	void execute(short idthread) {
		for (size_t l = 0; l < loops.size(); l++) {
            if (maprecipient)
                //if the recipient is a map constant: {x:y}
                recipients[l]->Setvalue(loops[l], aNULL, idthread);
            else
                loops[l]->Setvalue(recipients[l], idthread);
			loops[l]->Next();
		}
	}

	void reset() {
		for (size_t l = 0; l < loops.size(); l++) {
			if (recreate[l] != NULL) {
				Tamgu* a = recreate[l]->Eval(environment, aNULL, idthread);
				if (a != aNOELEMENT) {
					vects[l]->Release();
					loops[l]->Release();
					vects[l] = a;
					loops[l] = a->Newiteration(false);
					loops[l]->Begin();
				}
			}
            else {
                if (loops[l] == NULL)
                    loops[l] = vects[l]->Newiteration(false);
				loops[l]->Begin();
            }
		}
	}
};
//-----------------------------------------------------------------------
class TamguCallFunctionTaskell : public TamguCall {
public:
	TamguFunctionLambda* body;
    short returntype;
	char taskellchoice;

	TamguLambdaDomain*  Thedomain() {
		return &body->lambdadomain;
	}

	TamguFunction* Body(short idthread) {
		return body;
	}

	bool hasDeclaration() {
		return true;
	}

	virtual Tamgu* Composition();

	virtual bool Checkarity();

	short Typeinfered();

	short Returntype() {
        if (returntype != a_null)
            return returntype;
        
		return body->Returntype();
	}

    short Typevariable() {
        if (returntype == a_null)
            return a_none;
        
        return returntype;
    }
    
    unsigned long CallBackArity() {
        return P_TWO;
    }

    virtual char isCallTaskellFunction() {
        return 3;
    }
    
	TamguCallFunctionTaskell(TamguGlobal* global, Tamgu* parent = NULL, TamguFunctionLambda* b = NULL, short n = 1) : TamguCall(a_calltaskell, global, parent) {
		body = b;
		name = n;
		taskellchoice = 1;
        returntype = a_null;
	}

	TamguCallFunctionTaskell(TamguFunctionLambda* func, TamguGlobal* global, Tamgu* parent) : TamguCall(func->Name(), a_calltaskell, global, parent) {
		body = func;
		taskellchoice = 1;
        returntype = a_null;
	}

	TamguCallFunctionTaskell(TamguFunctionLambda* func) : TamguCall(func->Name(), a_calltaskell) {
		body = func;
		taskellchoice = 1;
        returntype = a_null;
	}


	virtual Tamgu* Eval(Tamgu*context, Tamgu* value, short idthread);
	Tamgu* GetTaskell2(Tamgu* context, Tamgu* env, TamguFunctionLambda*, short idthread);
	Tamgu* GetTaskell3(Tamgu* context, Tamgu* env, TamguFunctionLambda*, short idthread);
	Tamgu* GetTaskell5(Tamgu* context, Tamgu* env, TamguFunctionLambda*, short idthread);
	Tamgu* GetTaskellN(Tamgu* context, Tamgu* env, TamguFunctionLambda*, short idthread);

	Tamgu* Process(TaskellLoop* loop, Tamgu* context, Tamgu* env, TamguFunctionLambda* af, short idthread);

	void Init(Tamgu* f) {
		if (body != NULL)
			return;

		if (f == NULL)
			body = new TamguFunctionLambda(1, globalTamgu);
		else
			body = (TamguFunctionLambda*)f;
	}

	void AddInstruction(Tamgu* ke) {
		switch (taskellchoice) {
		case 1:
			arguments.push_back(ke);
			break;
		case 2:
			function = (TamguCall*)ke;
			break;
		case 3:
			body->lambdadomain.AddInstruction(ke);
			break;
		case 4:
			body->AddInstruction(ke);
			break;
		}
	}

	bool isDeclared(short idname) {
		if (body == NULL)
			return false;
		return body->lambdadomain.isDeclared(idname);
	}

    char Declarelocal(short idthread, short n, Tamgu* a) {
        if (isDeclared(n))
            return a_declaration;

        Declare(n, a);
        return true;
    }

	void Declare(short idname, Tamgu* a) {
		if (body == NULL)
			return;
		body->lambdadomain.Declare(idname, a);
	}

	Tamgu* Declaration(short idname) {
		if (body == NULL)
			return NULL;
		return body->lambdadomain.Declaration(idname);
	}

};

class TamguCallFunctionArgsTaskell : public TamguCallFunctionTaskell {
public:
	bool hdeclared;

	TamguCallFunctionArgsTaskell(TamguGlobal* global, Tamgu* parent = NULL) : hdeclared(false), TamguCallFunctionTaskell(global, parent) {}
	TamguCallFunctionArgsTaskell(TamguFunctionLambda* func, TamguGlobal* global, Tamgu* parent) : hdeclared(false), TamguCallFunctionTaskell(func, global, parent) {
        if (func != NULL) {
			hdeclared = func->hdeclared;
            if (hdeclared)
                returntype = func->Returntype();
        }
	}

	TamguCallFunctionArgsTaskell(TamguFunctionLambda* func) : hdeclared(false), TamguCallFunctionTaskell(func) {
		hdeclared = func->hdeclared;
	}

	virtual Tamgu* Eval(Tamgu*context, Tamgu* value, short idthread);

	Tamgu* Composition() {
		return aNOELEMENT;
	}

	short Typeinfered();

    char isCallTaskellFunction() {
        return 4;
    }
};

class TaskellLoopLinkedList {
public:
    static long idx;
    long id;
    TaskellLoop* loop;
    vector<TaskellLoop*> stack;
    
    Tamgu* recipient;
    TaskellLoopLinkedList* next;
    TamguFunctionLambda* current;
    TamguDeclarationAutoClean* environment;
    Tamgu** args;
    short* idargs;
    short idrecipient;
    long sz;
    
    std::atomic<bool> block;
    bool first;

    TaskellLoopLinkedList() {
        id = idx++;
        loop = NULL;
        recipient = aNULL;
        next = NULL;
        args = NULL;
        environment = NULL;
        first = true;
        idargs = NULL;
        idrecipient = -1;
        block = false;
    }
    
    void set(TaskellLoop* l, Tamgu* r, TamguFunctionLambda* func, Tamgu* e, Tamgu** a, long s) {
        environment = (TamguDeclarationAutoClean*)e;
        current = func;
        loop = l;
        recipient = r;
        args = a;
        sz = s;
        idargs = new short[sz];
        for (long i = 0; i < sz; i++)
            idargs[i] = globalTamgu->RecordInTrackerProtected(args[i]);
        idrecipient = globalTamgu->RecordInTrackerProtected(recipient);
    }
    
    void clean() {
        if (loop != NULL)
            loop->clean();
        stack.clear();
    }
    
    ~TaskellLoopLinkedList() {
        if (loop != NULL)
            delete loop;
        
        //The real problem is that in a final cleaning (when deleting globalTamgu)
        //The recipient might have been declared before the TamguCallFibre and can have been freed before
        //in the overall cleaning process...
        if (idrecipient > 0 && !globalTamgu->Checktracker(recipient, idrecipient))
            recipient = aNULL;
        
        if (args != NULL) {
            for (long i = 0; i < sz; i++) {
                if (recipient != args[i]) {
                    if (globalTamgu->Checktracker(args[i], idargs[i]))
                        args[i]->Resetreference();
                }
            }
            
            delete[] args;
            delete[] idargs;
        }

        if (environment != NULL) {
            if (!environment->isEmpty()) {
                recipient->Setreference();
                environment->Cleanforfibre();
                recipient->Protect();
            }            
        }
        
        recipient->Resetreference();
    }
    
    bool isempty() {
        return (loop == NULL);
    }
    
    Tamgu* Process(short idthread);
};

class TamguCallFibre;
    //This typedef defines a type "bvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (TamguCallFibre::*tamguCallFibre)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

class TamguCallFibre : public TamguCallFunctionArgsTaskell {
public:
    
    static Exchanging basebin_hash<tamguCallFibre> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;
    static Exchanging short idtype;

    ThreadLock _lock;
    ThreadLock _addlock;
    
    TaskellLoopLinkedList head;
    TaskellLoopLinkedList* tail;
    TaskellLoopLinkedList* block;
    Tamgu* variable;
    short idfibre;
    short threadid;
    std::atomic<bool> reset;
    
    static void AddMethod(TamguGlobal* g, string name, tamguCallFibre func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    TamguCallFibre(TamguGlobal* global) : TamguCallFunctionArgsTaskell(global) {
        tail = &head;
        variable = NULL;
        idfibre = -1;
        threadid = -1;
        block = NULL;
        reset = false;
    }

    TamguCallFibre(TamguGlobal* global, Tamgu* parent) : TamguCallFunctionArgsTaskell(global, parent) {
        idfibre = -1;
        tail = &head;
        variable = NULL;
        block = NULL;
        reset = false;
    }

    void Setidtype(TamguGlobal*);
    
    short Type() {
        return a_fibre;
    }
    
    void AddInstruction(Tamgu* a) {
        if (variable == NULL)
            variable = a;
        else
            TamguCallFunctionArgsTaskell::AddInstruction(a);
    }
    
    Tamgu* Eval(Tamgu*context, Tamgu* value, short idthread);
    
    Tamgu* Put(Tamgu* context, Tamgu* v, short idthread);

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return this;
    }

    Tamgu* GetTaskelFibres(Tamgu* context, TamguDeclarationAutoClean* env, TamguFunctionLambda*, Tamgu** args, short idthread);
    
    void add(TaskellLoop* l, Tamgu* r, TamguFunctionLambda* func, Tamgu* e, Tamgu** a, long sz) {
        TaskellLoopLinkedList* tl = new TaskellLoopLinkedList;
        tl->set(l,r, func, e, a, sz);
        
        Locking addlock(_addlock);
        tail->next = tl;
        tail = tl;
    }
    
    bool Checkarity() {
        return true;
    }
    
    Tamgu* Run(short idthread);
    
    Tamgu* MethodRun(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Run(idthread);
    }

    Tamgu* MethodBlock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking addlock(_addlock);
        if (block != NULL)
            block->block = false;
        block = NULL;
        
        if (head.next != NULL) {
            block = tail;
            block->block = true;
            reset = true;
            return aTRUE;
        }
        return aFALSE;
    }

    Tamgu* MethodUnblock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking addlock(_addlock);
        if (block != NULL) {
            block->block = false;
            block = NULL;
            return aTRUE;
        }
        return aFALSE;
    }

    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
            //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    bool Boolean() {
        if (head.next == NULL)
            return false;
        return true;
    }

    long Size() {
        Locking addlock(_addlock);
        TaskellLoopLinkedList* h = head.next;
        long nb = 0;
        while (h != NULL) {
            nb++;
            h=h->next;
        }
        return nb;
    }
    
    double Float() {
        return Size();
    }
    
    BLONG Long() {
        return Size();
    }
    
    long Integer() {
        return Size();
    }
    
    float Decimal() {
        return Size();
    }
    
    short Short() {
        return Size();
    }
    
    void clean() {
        Locking lock(_addlock);
        TaskellLoopLinkedList* h = head.next;
        while (h != NULL) {
            h->clean();
            h = h->next;
        }
        idfibre = -1;
    }
    
    ~TamguCallFibre() {
        if (idfibre != -1) {
            globalTamgu->threads[threadid].fibres.vecteur[idfibre] = NULL;
        }
        TaskellLoopLinkedList* h = head.next;
        while (h != NULL) {
            tail = h->next;
            delete h;
            h = tail;
        }
    }
    
};

class TamguGetCurryfiedFunction : public TamguReference {
public:
    VECTE<Tamgu*> arguments;
    TamguFunction* body;
    short name;
    
    TamguGetCurryfiedFunction(TamguFunction* bd) : body(bd), name(bd->Name()) {}
    
    long Size() {
        return arguments.size();
    }
    
    short Name() {
        return name;
    }
    
    Tamgu* Function() {
        return body;
    }
    
    short Type() {
        return a_call;
    }
    
    short Typevariable() {
        return a_call;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* callfunction, short idthread) {
        return this;
    }
    
    void Resetreference(short inc = 1) {
        for (long i = 0; i < arguments.size(); i++)
            arguments[i]->Resetreference(inc);
        TamguReference::Resetreference(inc);
    }
    
    void Setreference(short inc)  {
        for (long i = 0; i < arguments.size(); i++)
            arguments[i]->Setreference(inc);
        TamguReference::Setreference(inc);
    }
    
    void Setreference()  {
        for (long i = 0; i < arguments.size(); i++)
            arguments[i]->Setreference();
        TamguReference::Setreference();
    }
    
    bool isFunction() {
        return true;
    }
    
    bool isCurryfied() {
        return true;
    }
};


class TamguInstructionTaskellIF : public TamguInstruction {
public:
    TamguCallReturn* returncall;
    bool callreturn;
    
	bool compiled;
	TamguInstructionTaskellIF(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {
		idtype = a_iftaskell;
		compiled = false;
        returncall = NULL;
        callreturn = false;
	}
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* DirectEval(Tamgu* context, Tamgu* value, short idthread);

    void AddInstruction(Tamgu* a) {
        if (instructions.size() == 1 && a->investigate == is_return) {
            callreturn = true;
            returncall = (TamguCallReturn*)a;
        }
        
        instructions.push_back(a);
    }

    bool isaIF() {
        return true;
    }

};

class TamguInstructionTaskellMainCASE : public TamguInstruction {
public:
	long other;

	TamguInstructionTaskellMainCASE(TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {
		idtype = a_casetaskell;
		other = 0;
	}
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isaIF() {
        return true;
    }
    
};

class TamguFrameParameter : public TamguObject {
public:
	short framename;
	vector<short> equivalence;
    vector<Tamgu*> values;

	TamguFrameParameter(short fn, TamguGlobal* g, Tamgu* parent) : TamguObject(g, parent) { 
		framename = fn; 
	}

	Tamgu* Compare(Tamgu* env, Tamgu* v, short idthread);

	void Declare(short id, Tamgu* var) {
		equivalence.push_back(id);
        if (var->Name() != 0)
            equivalence.push_back(var->Name());
        else {
            equivalence.push_back(values.size());
            values.push_back(var);
        }
	}
    
	short Type() {
		return framename;
	}
};

class TamguCallFrameMethod : public TamguCallClean {
public:
	bool Checkarity();

	TamguCallFrameMethod(short n, TamguGlobal* global = NULL, Tamgu* parent = NULL) :
		TamguCallClean(n, a_callfunction, global, parent) {}

	Tamgu* Eval(Tamgu* context, Tamgu* domain, short idthread);

	short Typeinfered() {
		if (function != NULL)
			return function->Typeinfered();
		return a_none;
	}

};

#endif

/*
*  Tamgu (탐구)
    *
* Copyright 2019-present NAVER Corp.
* under BSD 3-clause
*/
/* --- CONTENTS ---
Project    : Tamgu (탐구)
    Version    : See tamgu.cxx for the version number
filename   : lisp.h
Date       : 2017/09/01
Purpose    :
Programmer : Claude ROUX (claude.roux@naverlabs.com)
    Reviewer   :
*/

#ifndef tamgulisp_h
#define tamgulisp_h

#include "tamguvector.h"


class Tamgusymbol : public Tamgu {
public:
    
    short action;
    short name;
    
    Tamgusymbol(short symb, TamguGlobal* g) {
        name = symb;
        if (g->lispactions.check(name))
            action = name;
        else
            action = a_atom;
        g->RecordInTracker(this);
    }

    Tamgusymbol(short symb, short act, TamguGlobal* g) {
        name = symb;
        action = act;
        g->RecordInTracker(this);
    }
    

    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);

    short Action() {
        return action;
    }
    
    short Name() {
        return name;
    }
    
    short Type() {
        return a_atom;
    }
    
    string JSonString() {
        string v;
        v = '"';
        v += globalTamgu->Getsymbol(name);
        v += '"';
        return(v);
    }

    string String() {
        return globalTamgu->Getsymbol(name);
    }
    
    wstring UString() {
        return globalTamgu->Getwsymbol(name);
    }
    
    void Setstring(string& v, short idthread) {
        v = globalTamgu->Getsymbol(name);
    }

    void Setstring(wstring& v, short idthread) {
        v = globalTamgu->Getwsymbol(name);
    }

    Tamgu* andset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* orset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* xorset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* plus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* minus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* multiply(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* divide(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* power(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }

    Tamgu* same(Tamgu* a) {
        return booleantamgu[name == a->Name()];
    }

    Tamgu* different(Tamgu* a) {
        return booleantamgu[name != a->Name()];
    }

    Tamgu* less(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

    Tamgu* more(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

    Tamgu* lessequal(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

    Tamgu* moreequal(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

};

class Tamgulispvariable : public TamguReference {
public:
    Tamgu* function;
    short name;
    
    Tamgulispvariable(string& symb, TamguGlobal* g, Tamgu* parent) : function(NULL), TamguReference(g, parent) {
        name = g->Getid(symb);
    }

    void Setreference() {
        if (function != NULL)
            function->Setreference();
        TamguReference::Setreference();
    }    

    void Setreference(short inc) {
        if (function != NULL)
            function->Setreference(inc);
        TamguReference::Setreference(inc);
    }
    
    void Resetreference(short inc = 1) {
        if ((reference - inc) <= 0 && function != NULL) {
            if (function->isIndex())
                function->Clear();
            function = NULL;
        }
        TamguReference::Resetreference(inc);
    }
    
    void AddInstruction(Tamgu* e) {
        function = e;
    }
    
    Tamgu* Function() {
        return function;
    }

    bool isStop() {
        return function->isStop();
    }
    
    Tamgu* Getindex() {
        return function->Getindex();
    }
    
    bool Setstopindex() {
        return function->Setstopindex();
    }
    
    bool isDirectIndex() {
        return (function != NULL && function->isIndex() && !function->Function());
    }

    bool isCall() {
        return true;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);

    short Action() {
        return a_atom;
    }
    
    short Name() {
        return name;
    }
    
    short Type() {
        return a_variable;
    }
    
    string JSonString() {
        string v;
        v = '"';
        v += globalTamgu->Getsymbol(name);
        v += function->String();
        v += '"';
        return(v);
    }

    string String() {
        string v = globalTamgu->Getsymbol(name);
        v += function->String();
        return v;
    }
    
    wstring UString() {
        return globalTamgu->Getwsymbol(name);
    }
    
    void Setstring(string& v, short idthread) {
        v = globalTamgu->Getsymbol(name);
    }

    void Setstring(wstring& v, short idthread) {
        v = globalTamgu->Getwsymbol(name);
    }

    Tamgu* andset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* orset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* xorset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* plus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* minus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* multiply(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* divide(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* power(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror(e_cannot_compute_with);
    }

    Tamgu* same(Tamgu* a) {
        return booleantamgu[name == a->Name()];
    }

    Tamgu* different(Tamgu* a) {
        return booleantamgu[name != a->Name()];
    }

    Tamgu* less(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

    Tamgu* more(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

    Tamgu* lessequal(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

    Tamgu* moreequal(Tamgu* a) {
        return globalTamgu->Returnerror(e_meaningless_comparison_with);
    }

};

class Tamgucadr : public Tamgu {
public:
    string action;
    
    Tamgucadr(string& a, TamguGlobal* g, Tamgu* parent) {
        action = a;
        g->RecordInTracker(this);
        parent->AddInstruction(this);
    }
    
    short Action() {
        return a_cadr;
    }
    
    uchar getbyte(long i) {
        return action[i];
    }
    
    long Size() {
        return action.size();
    }

    string String() {
        string s = "c";
        s += action;
        s += "r";
        return s;
    }

    void Setstring(string& s, short idthread) {
        s = "c";
        s += action;
        s += "r";
    }

    void Setstring(wstring& s, short idthread) {
        s = L"c";
        for (uchar i = 0; i < action.size(); i++)
            s += (wchar_t)action[i];
        s += L"r";
    }

};

class Tamguoperator : public Tamgu {
public:
    short action;
    
    Tamguoperator(short id, TamguGlobal* g) {
        action = id;
        g->RecordInTracker(this);
    }

    short Action() {
        return action;
    }
    
    short Name() {
        return action;
    }
    
    string String() {
        return globalTamgu->operator_strings[action];
    }

    void Setstring(string& v, short idthread) {
        v = globalTamgu->operator_strings[action];
    }
};

//---------------------------------------------------------------------------------------------------------------------
class Tamgulisp;
typedef Tamgu* (Tamgulisp::*lispMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

class Tamgulisp : public Tamguvector {
    public:
    static Exchanging basebin_hash<lispMethod> methods;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    bool used;
    long idinfo;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgulisp(TamguGlobal* g, Tamgu* parent) : used(false), Tamguvector(g, parent) {}

    Tamgulisp(long i) : used(false), idinfo(i) {}

    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);
    Tamgu* Put(Tamgu* c, Tamgu* v, short idthread);
    Tamgu* Convert(Tamgu* c, short idthread);

    bool isLisp() {
        return true;
    }
    
    bool Candelete() {
        return false;
    }

    virtual void Resetreference(short r);

    virtual short Type() {
        return a_lisp;
    }

    virtual string Typename() {
        return "lisp";
    }

    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: duplicateForCall should return true...
    //However if your object is complex and probably unique through out the code
    //Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    
    void Setaction(short a) {
        Tamgu* op = globalTamgu->Providelispoperators(a);
        Push(op);
    }

    short Action() {
        return a_lisp;
    }

    Tamgu* car(short idthread) {
        if (!values.size())
            return aNOELEMENT;
        return values[0]->Atom();
    }

    virtual Tamgu* Atom(bool forced = false) {
        Tamgulisp* v = globalTamgu->Providelisp();
        for (size_t i = 0; i < values.size(); i++)
            v->push(values[i]);
        return v;
    }

    virtual Tamgu* cdr(short idthread);
    
        //---------------------------------------------------------------------------------------------------------------------
    static void AddMethod(TamguGlobal* g, string name, lispMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    Tamgu* MethodEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*Tamgulisp::methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return globalTamgu->Providelisp();
    }

    string String();
    void Setstring(string& v, short idthread);

    Tamgu* Localreverse() {
        vector<Tamgu*> v;
        for (long i = (long)values.size() - 1; i >= 0; i--)
            v.push_back(values[i]);
        values = v;
        return this;
    }
};


class Tamgulispcode : public Tamgulisp {
public:

    Tamgulispcode(TamguGlobal* g, Tamgu* parent = NULL);
    
    void Cleanreference(short inc) {}
    
    void Addreference(unsigned short inv, short r=1) {}
    void Setreference(short r) {}
    void Setreference() {}
    void Resetreference(short r = 1) {}

    void Setinstruction(short idthread) {
        globalTamgu->Current(this, idthread);
    }

    Tamgu* Atom(bool forced=false) {
        return this;
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

    void Setinfo(Tamgu* ins) {
        if (globalTamgu->debugmode) {
            idinfo = ins->Currentinfo();
        }
    }

    void Getinfo(Tamgu* ins) {
        idinfo = ins->Currentinfo();
    }
};

class Tamgulispair : public Tamgulisp {
public:
    
    Tamgulispair() : Tamgulisp(-1) {}
    
    short Type() {
        return a_pair;
    }

    string Typename() {
        return "pair";
    }

    Tamgu* Atom(bool forced = false) {
        Tamgulispair* v = new Tamgulispair();
        for (size_t i = 0; i < values.size(); i++)
            v->push(values[i]);
        return v;
    }

    Tamgu* cdr(short idthread);
    string String();
    void Setstring(string& v, short idthread);
};


class TamguLispFunction : public TamguFunction {
public:
    std::atomic<short> reference;
    bool protect;

    TamguLispFunction(short n, TamguGlobal* g = NULL) : TamguFunction(n, g) {
        protect = true;
        reference = 0;
    }

    ~TamguLispFunction() {
        if (idtracker != -1)
            globalTamgu->RemoveFromTracker(idtracker);

        Tamgu* tmg = globalTamgu->Getmainframe(0);
        if (tmg->isDeclared(name))
            tmg->Declare(name, aNULL);

        //This is a case that occurs when creating lambdas and functions in Lisp with eval
        for (long i = 0; i < parameters.size(); i++)
            delete parameters[i];
        if (instructions.size() == 1) {
            Tamgu* v = instructions[0]->Argument(0);
            v->Resetreference();
            delete instructions[0];
        }
    }
    
    short Reference() {
        return reference;
    }
    
    void Setprotect(bool n) {
        protect = n;
    }
    
    void Popping() {
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
    
    void Addreference(unsigned short inv, short inc=1) {
        investigate |= (inv & is_tobelocked);
        Setreference(inc);
    }

    void Setreference(short r) {
        reference += r;
        protect = false;
        
    }
    
    void Setreference() {
        ++reference;
        protect = false;
        
    }

    void Resetreference(short r = 1) {
        r = reference - r;
        if (r <= 0) {
            reference.store(0);
            if (!protect) {
                delete this;
            }
        }
        else
            reference.store(r);
    }

    void Release() {
        if (reference == 0) {
            protect = false;
            Resetreference(0);
        }
    }
    
    void Protect() {
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
};

#endif

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
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* orset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* xorset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* plus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* minus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* multiply(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* divide(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* power(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }

    Tamgu* same(Tamgu* a) {
        return booleantamgu[name == a->Name()];
    }

    Tamgu* different(Tamgu* a) {
        return booleantamgu[name != a->Name()];
    }

    Tamgu* less(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
    }

    Tamgu* more(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
    }

    Tamgu* lessequal(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
    }

    Tamgu* moreequal(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
    }

};

class Tamgulispvariable : public Tamgu {
public:
    Tamgu* call;
    short name;
    
    Tamgulispvariable(string& symb, TamguGlobal* g, Tamgu* parent) {
        name = g->Getid(symb);
        parent->AddInstruction(this);
        g->RecordInTracker(this);
    }

    void AddInstruction(Tamgu* e) {
        call = e;
    }
    
    Tamgu* Function() {
        return call;
    }

    bool isStop() {
        return call->isStop();
    }
    
    Tamgu* Getindex() {
        return call->Getindex();
    }
    
    bool Setstopindex() {
        return call->Setstopindex();
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
        v += call->String();
        v += '"';
        return(v);
    }

    string String() {
        string v = globalTamgu->Getsymbol(name);
        v += call->String();
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
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* orset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* xorset(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* plus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* minus(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* multiply(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* divide(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* power(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        return globalTamgu->Returnerror("Cannot compute with symbols");
    }

    Tamgu* same(Tamgu* a) {
        return booleantamgu[name == a->Name()];
    }

    Tamgu* different(Tamgu* a) {
        return booleantamgu[name != a->Name()];
    }

    Tamgu* less(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
    }

    Tamgu* more(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
    }

    Tamgu* lessequal(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
    }

    Tamgu* moreequal(Tamgu* a) {
        return globalTamgu->Returnerror("Meaningless comparison with symbols");
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
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

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
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return globalTamgu->Providelisp();
    }

    string String();
    void Setstring(string& v, short idthread);
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


#endif

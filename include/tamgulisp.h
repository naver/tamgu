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
    
    string String() {
        return globalTamgu->operator_strings[action];
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
    long idinfo;
    
    //---------------------------------------------------------------------------------------------------------------------
    Tamgulisp(TamguGlobal* g, Tamgu* parent = NULL);
    Tamgulisp() : idinfo(-1), Tamguvector() {}

    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);

    bool isLisp() {
        return true;
    }
    
    short Type() {
        return a_lisp;
    }

    string Typename() {
        return "lisp";
    }

    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: duplicateForCall should return true...
    //However if your object is complex and probably unique through out the code
    //Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    Tamgu* Atom(bool forced=false) {
        return this;
    }
    
    void Setaction(short a) {
        Tamgu* op = globalTamgu->Providelispoperators(a);
        Push(op);
    }

    short Action() {
        return a_lisp;
    }

        //---------------------------------------------------------------------------------------------------------------------
    static void AddMethod(TamguGlobal* g, string name, lispMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    Tamgu* MethodEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamgulisp;
    }

    string String();

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
#endif

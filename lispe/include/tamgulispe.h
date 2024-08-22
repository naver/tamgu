/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : lispe.h
 Date       : 2017/09/01
 Purpose    :
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgulispe_h
#define tamgulispe_h
#include "lispe.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgulispe;
//This typedef defines a type "lispeMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgulispe::*lispeMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamgulispe : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<lispeMethod> methods;
    static short idtype;

    LispE* lisp;

    vector<Tamgu*> buffers;
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...

    //---------------------------------------------------------------------------------------------------------------------
    Tamgulispe(TamguGlobal* g, Tamgu* parent = NULL) : lisp(NULL), TamguObject(g, parent) {}
    Tamgulispe();
    ~Tamgulispe();
    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);

    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgulispe::idtype;
    }

    void Setidtype(TamguGlobal* global);

    string Typename() {
        return "lispe";
    }

    //The next two functions work in conjunction...
    //If your object is very atomic (such as a number or a string)
    //Then when it is passed to a function or stored into a container,
    //it might be worth duplicating it.
    // In that case: duplicateForCall should return true...

    //However if your object is complex and probably unique through out the code
    //Then duplicateForCall should return false, and Atom should always reduce to a "return this;" only...
    Tamgu* Atom(bool forced=false) {
        if (forced || !protect)
            return new  Tamgulispe;
        return this;
    }

    bool duplicateForCall() {
        return true;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamgulispe;
    }

    TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, lispeMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    void Methods(Tamgu* v) {
        for (const auto& it : globalTamgu->infomethods[idtype])
            v->storevalue(it.first);
    }

    string Info(string n) {
        if (globalTamgu->infomethods[idtype].find(n) !=  globalTamgu->infomethods[idtype].end())
            return globalTamgu->infomethods[idtype][n];
        return "Unknown method";
    }

    Tamgu* convert_to_tamgu(Element* e, short idthread);
    Element* convert_to_lispe(Tamgu* e, short idthread);
    Tamgu* eval(Element* l, short);
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is a sample  of a function that could be implemented for your needs.

    void build(Tamgu* contextualpattern, TamguCall* callfunc, short idthread, string& code);
    Tamgu* MethodExecute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodEval(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*Tamgulispe::methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear(TamguGlobal* g) {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        return "lispe";
    }

    wstring UString() {
        return L"lispe";
    }

    long Integer() { return 0; }
    short Short() { return 0;}
    double Float() { return 0; }
    bool Boolean() { return false; }
    BLONG Long() { return 0; }
    unsigned char Byte() {
        return 0;
    }

    long Size() { return 0; }

    //Basic operations: we add the current value with "a"
    Tamgu* andset(Tamgu* a, bool itself) {
        return this;
    }

    Tamgu* orset(Tamgu* a, bool itself) {
        return this;
    }

    Tamgu* xorset(Tamgu* a, bool itself) {
        return this;
    }

    Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
        return aNULL;
    }

    Tamgu* plusplus() {
        return this;
    }

    Tamgu* minusminus() {
        return this;
    }

    double Sum() {
        return 0;
    }

    double Product() {
        return 1;
    }

    Tamgu* plus(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* minus(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* multiply(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* divide(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* divideinteger(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* power(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        return this;
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        return this;
    }

    Tamgu* less(Tamgu* a) {
        return aFALSE;
    }

    Tamgu* more(Tamgu* a) {
        return aFALSE;
    }

    Tamgu* same(Tamgu* a) {
        if (a == this)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* different(Tamgu* a) {
        if (same(a) == aFALSE)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* lessequal(Tamgu* a) {
        return aFALSE;
    }

    Tamgu* moreequal(Tamgu* a) {
        return aFALSE;
    }


};
#endif

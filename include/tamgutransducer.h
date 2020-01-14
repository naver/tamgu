/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutransducer.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgutransducer_h
#define tamgutransducer_h

#include "automaton.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgutransducer;
//This typedef defines a type "transducerMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgutransducer::*transducerMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgutransducer : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<transducerMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	TamguDoubleSideAutomaton* automaton;
	string name;
    bool modified;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgutransducer(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		automaton = NULL;
        modified = false;
    }

    Tamgutransducer() {
        //Do not forget your variable initialisation
		automaton = NULL;
        modified = false;
    }

	~Tamgutransducer() {
		if (automaton != NULL)
			delete automaton;
    }

	void Methods(Tamgu* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}


	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgutransducer::idtype;
    }

    

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "transducer";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
    }

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Tamgu* Atom(bool forced=false) {
        return this;
    }

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamgutransducer;
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, transducerMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodAdd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBuild(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodStore(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCompilergx(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLookup(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLookdown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodProcess(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodFactorize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    bool compilergx(wstring& w, wstring& feat);
    bool parse(string& words, Tamguvector* kvect, short idthread, long threshold, short flags, bool option=false);
    void merge(Tamgutransducer* t) {
        if (automaton==NULL) {
            automaton=t->automaton;
            t->automaton=NULL;
            return;
        }
        if (t->automaton==NULL)
            return;
        
        t->automaton->clearmarks();
        vector<TamguState*> marks;
        automaton->start.mergein(&t->automaton->start, *automaton, marks);
        for (long i = 0; i < marks.size(); i++)
            marks[i]->mark = false;
    }
    
    void factorize(long first) {
        if (automaton == NULL)
            return;
        automaton->factorize(first);
    }
    
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition

		Locking _lock(this);
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    

    string String() {
        Locking _lock(this);
        return name;
    }



    bool Boolean() {
        Locking _lock(this);
		if (automaton == NULL)
			return false;
		return true;
	}


};

#endif




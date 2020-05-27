/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguchrono.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguchrono_h
#define tamguchrono_h

#include <chrono>

#include "tamgufloat.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguchrono;
//This typedef defines a type "chronoMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguchrono::*chronoMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguchrono : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<chronoMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...

    std::chrono::high_resolution_clock::time_point value;
    long unit;

    //---------------------------------------------------------------------------------------------------------------------
	Tamguchrono(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		value = std::chrono::high_resolution_clock::now();
        unit=1;
    }

	Tamguchrono() {
		//Do not forget your variable initialisation
        value = std::chrono::high_resolution_clock::now();
        unit=1;
	}

	Tamguchrono(std::chrono::high_resolution_clock::time_point& t) {
		//Do not forget your variable initialisation
		value = t;
        unit=1;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamguchrono::idtype;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "chrono";
    }

    

    

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    bool isTime() {
        return true;
    }
    

    Tamgu* Atom(bool forced=false) {
        if (forced || !protect || reference)
            return new  Tamguchrono(value);
        return this;
    }

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
        return new Tamguchrono;
    }

    TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, chronoMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

	void Methods(Tamgu* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}


	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

    Tamgu* MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodUnit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    

	string String();

	double Float() {
        return value.time_since_epoch().count();
	}

	long Integer() {
		return Float();
	}
    
    BLONG Long() {
        return Float();
    }
    
    short Short() {
        return Float();
    }
    
	bool Boolean() {
		return true;
	}

    Tamgu* minus(Tamgu* bb, bool autoself);


	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float() < a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float()>a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float() == a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif
		if (Float() != a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float() <= a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float() >= a->Float())
			return aTRUE;
		return aFALSE;
	}

    long Size() {
		return Integer();
	}
};

#endif




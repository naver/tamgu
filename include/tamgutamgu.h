/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutamgu.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgutamgu_h
#define tamgutamgu_h
#include "compilecode.h"
#include "tamgusvector.h"
#include "instructions.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgutamgu;
//This typedef defines a type "tamguMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgutamgu::*tamguMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgutamgu : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<tamguMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

	static Exchanging hmap<string, Tamgutamgu*> recorded;

    short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    TamguCode* acode;
	string filename;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgutamgu(TamguGlobal* g, Tamgu* parent = NULL);

    Tamgutamgu(string fn, TamguCode* a, TamguGlobal* g);

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

    bool Candelete() {
        return false;
    }

	short Type() {
		return a_tamgu;
	}

	short Typevariable() {
		return idtype;
	}

	string Typename() {
        return "tamgu";
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

	bool isDeclared(short id) {
		return acode->isDeclared(id);
	}

	Tamgu* Declaration(short id) {
		return acode->Declaration(id);
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgutamgu(globalTamgu);
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, tamguMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(TamguGlobal* g, string version);
	
	Tamgu* Load();

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

    Tamgu* MethodMethod(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {        
		Tamgu* domain = acode->Mainframe();
		Tamgu* func = domain->Declaration(callfunc->Name());
		if (func == NULL || !func->isFunction() || func->isPrivate()) {
			string message = "Unknown function: '";
			message += globalTamgu->Getsymbol(callfunc->Name());
			message += "'";
			return globalTamgu->Returnerror(message, idthread);
		}

		TamguCallFunction cfunc(func);
		cfunc.arguments = callfunc->arguments;
		globalTamgu->Pushstack(domain, idthread);
		Tamgu* a = cfunc.Eval(domain, aNULL, idthread);
		globalTamgu->Popstack(idthread);
		return a;
	}

    //Return the list of all methods available in the external file called through a tamgu variable...
    Tamgu* MethodMethods(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        TamguDeclaration* domain = (TamguDeclaration*)acode->Mainframe();
        Tamgusvector* methods = (Tamgusvector*)Selectasvector(contextualpattern);
        
        basebin_hash<Tamgu*>::iterator it;
        for (it = domain->declarations.begin(); it != domain->declarations.end(); it++) {
            if (it->second->isFunction() && !it->second->isPrivate())
                methods->values.push_back(globalTamgu->Getsymbol(it->first));
        }
        
        return methods;
    }


    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return MethodMethod(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    

    string String() {        
		return filename;
    }

    void Setstring(string& v, short idthread) {
        v = filename;
    }

};

Tamgu* TamguRecordedFiles(string filename);
Tamgu* TamguRecordFile(string filename, TamguCode* a, TamguGlobal* g);

#endif





/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguiterator.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguiterator_h
#define tamguiterator_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguiterator;
//This typedef defines a type "iteratorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguiterator::*iteratorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguiterator : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<iteratorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    TamguIteration* value;

    //---------------------------------------------------------------------------------------------------------------------
    Tamguiterator(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		value = NULL;
    }

    Tamguiterator() {
        //Do not forget your variable initialisation
        value=NULL;
    }

    void Setreference(short r) {
        reference += r;
        if (value != NULL)
            value->Setreference(r);
    }
    
    void Setreference() {
        ++reference;
        if (value != NULL)
            value->Setreference();
    }
    
	void Resetreference(short r) {
		if (value != NULL)
			value->Resetreference(r);
		TamguObject::Resetreference(r);
	}

    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* idx, Tamgu* v, short idthread) {
		if (value != NULL)
			value->Resetreference(reference);
		value = v->Newiteration(false);
		value->Setreference(reference);
        return this;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread) {        
        return this;
    }

    short Type() {
        return Tamguiterator::idtype;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "iterator";
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

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamguiterator(NULL);
    }

    TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, iteratorMethod func, unsigned long arity, string infos);
    
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
	Tamgu* MethodBegin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		return value->Begin();
	}

	Tamgu* MethodKey(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		return value->Key();
	}

	Tamgu* MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		return value->Value();
	}

	Tamgu* MethodNext(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (value == NULL)
			return aNOELEMENT;
		value->Next();
		return aTRUE;
	}

	Tamgu* MethodEnd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (value == NULL)
			return aTRUE;
		return value->End();
	}
	
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

	Tamgu* Key() {
		if (value == NULL)
			return aNOELEMENT;
		return value->Key();
	}

	Tamgu* Value() {
		if (value == NULL)
			return aNOELEMENT;
		return value->Value();
	}

	string Keystring() {
		if (value == NULL)
			return "";
		return value->Keystring();
	}

	wstring Valueustring() {
		if (value == NULL)
			return L"";
		return value->Valueustring();
	}

	string Valuestring() {
		if (value == NULL)
			return "";
		return value->Valuestring();
	}

	long Keyinteger() {
		if (value == NULL)
			return 0;
		return value->Keyinteger();
	}

	long Valueinteger() {
		if (value == NULL)
			return 0;
		return value->Valueinteger();
	}

	double Keyfloat() {
		if (value == NULL)
			return 0;
		return value->Keyfloat();
	}

	double Valuefloat() {
		if (value == NULL)
			return 0;
		return value->Valuefloat();
	}

	void Next() {
		if (value != NULL)
			value->Next();
	}

	Tamgu* End() {
		if (value == NULL)
			return aTRUE;

		if (value->End() == aTRUE) {
			delete value;
			value = NULL;
			return aTRUE;
		}


		return aFALSE;
	}

	void Begin() {
		if (value != NULL)
			value->Begin();
	}

	Tamgu* plusplus() {
		Next();
		return Key();
	}

    void Clear() {
        //To set a variable back to empty
		if (value != NULL)
			delete value;
		value = NULL;
    }

    

    string String() {
        return "";
    }

    wstring UString() {
        return L"";
    }

    void Setstring(string& v, short idthread) {
        v = "";
    }
    
    void Setstring(wstring& v, short idthread) {
        v = L"";
    }
    
    long Integer() {
        
        return 0;
    }
    double Float() {
        
        return 0;
    }
    BLONG Long() {
        
        return 0;
    }

    bool Boolean() {
        
        return false;
    }

    
    //Basic operations
    long Size() {
        
        return 0;
    }
    
    Tamgu* andset(Tamgu* a, bool itself) {
        
        return this;
    }

    Tamgu* orset(Tamgu* a, bool itself) {
        
        return this;
    }

    Tamgu* xorset(Tamgu* a, bool itself) {
        
        return this;
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








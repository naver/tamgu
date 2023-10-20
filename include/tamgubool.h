/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubool.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguboolean_h
#define tamguboolean_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguboolean;
//This typedef defines a type "boolMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguboolean::*boolMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguboolean : public TamguReference {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<boolMethod> methods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	bool value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamguboolean(bool v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Tamguboolean(bool v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
		value = v->Boolean();
		return this;
	}

    Tamgu* Putvalue(Tamgu* v, short idthread) {
        value = v->Boolean();
        return this;
    }

    Tamgu* Clonevalue(Tamgu* v, short idthread) {
        value = v->Boolean();
        return this;
    }

    void Putatomicvalue(Tamgu* v) {
        value = v->Boolean();
    }

	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
		return this;
	}


	short Type() {
        return a_boolean;
	}

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "bool";
	}

	

	

	bool isAtom() {
		return true;
	}

    bool checkAtomType(short ty) {
        return (ty == a_boolean);
    }

	bool isBoolean() {
		return false;
	}

	bool isFrame() {
		return false;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Tamguboolean(value);
		return this;
	}

    Tamgu* Atomref() {
        TamguReference* r;
        if (!protect || reference)
            r = new  Tamguboolean(value);
        else
            r = this;
        r->reference = 1;
        r->protect = false;
        return r;
    }

	Tamgu* Succ() {
        return booleantamgu[!value];
	}

	Tamgu* Pred() {
        return booleantamgu[!value];
	}

    bool Protectedboolean() {
        Locking _lock(*globalTamgu->booleanlocks[a_boolean]);
        return value;
    }

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamguboolean(0);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return new Tamguboolean(a->Boolean());
	}


	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, boolMethod func, unsigned long arity, string infos);
	
	static bool InitialisationModule(TamguGlobal* g, string version);

	
	void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[a_boolean])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[a_boolean].find(n) !=  globalTamgu->infomethods[a_boolean].end())
              return globalTamgu->infomethods[a_boolean][n];
             return "Unknown method";
	}



	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return booleantamgu[!value];
	}

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
		value = false;
	}
	
    wstring UString() {
        if (value)
            return L"true";
        return L"false";
    }

	string String() {
		if (value)
			return "true";
		return "false";
	}

    void Setstring(string& v, short idthread) {
        if (value)
            v = "true";
        else
            v = "false";
    }

    string JSonString() {
        if (value)
            return "true";
        return "false";
    }

    void Setstring(wstring& v, short idthread) {
        if (value)
            v = L"true";
        else
            v = L"false";
    }

	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	bool Boolean() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	Tamgu* same(Tamgu* a) {
		if (value == a->Boolean())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
		if (value != a->Boolean())
			return aTRUE;
		return aFALSE;
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		if (a->Boolean() || value)
			return aTRUE;

		return aFALSE;
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		if (value) {
			if (a->Boolean())
				return aFALSE;
			return aTRUE;
		}

		if (a->Boolean())
			return aTRUE;

		return aFALSE;
	}

};

    //We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
    //of how to declare a new method.
class Tamguatomicbool;
    //This typedef defines a type "atomicboolMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguatomicbool::*atomicboolMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

class Tamguatomicbool : public TamguReference {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<atomicboolMethod> methods;
    
    
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    std::atomic<bool> value;
    
        //---------------------------------------------------------------------------------------------------------------------
    Tamguatomicbool(bool v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
            //Do not forget your variable initialisation
        value = v;
    }
    
    Tamguatomicbool(bool v) {
            //Do not forget your variable initialisation
        value = v;
    }
    
	void Setidtype(TamguGlobal* global) {
		Tamguatomicbool::InitialisationModule(global, "");
	}
	
        //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
        value = v->Boolean();
        return this;
    }
    
    Tamgu* Putvalue(Tamgu* v, short idthread) {
        value = v->Boolean();
        return this;
    }
    
    Tamgu* Clonevalue(Tamgu* v, short idthread) {
        value = v->Boolean();
        return this;
    }
    
    void Putatomicvalue(Tamgu* v) {
        value = v->Boolean();
    }

    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        return this;
    }
    
    
    short Type() {
        return a_boolean;
    }
    
    string Typename() {
        return "bool";
    }
    
    
    
    
    
    bool isAtom() {
        return true;
    }
    
    bool checkAtomType(short ty) {
        return (ty == a_boolean);
    }

    bool isBoolean() {
        return false;
    }
    
    bool isFrame() {
        return false;
    }
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference)
            return new  Tamguatomicbool(value);
        return this;
    }
    
    Tamgu* Atomref() {
        TamguReference* r;
        if (!protect || reference)
            r = new  Tamguatomicbool(value);
        else
            r = this;
        r->reference = 1;
        r->protect = false;
        return r;
    }

    Tamgu* Succ() {
        return booleantamgu[!value];
    }
    
    Tamgu* Pred() {
        return booleantamgu[!value];
    }
    
    bool Protectedboolean() {
        return value;
    }
    
        //---------------------------------------------------------------------------------------------------------------------
        //Declaration
        //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }
    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamguboolean(0);
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        return new Tamguboolean(a->Boolean());
    }
    
    
    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }
    
    static void AddMethod(TamguGlobal* g, string name, atomicboolMethod func, unsigned long arity, string infos);
    
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
    
    
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return booleantamgu[!value];
    }
    
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
        value = false;
    }   
    
    wstring UString() {
        if (value)
            return L"true";
        return L"false";
    }

    string String() {
        if (value)
            return "true";
        return "false";
    }
    
    string JSonString() {
        if (value)
            return "true";
        return "false";
    }
    
    void Setstring(string& v, short idthread) {
        if (value)
            v = "true";
        else
            v = "false";
    }

    void Setstring(wstring& v, short idthread) {
        if (value)
            v = L"true";
        else
            v = L"false";
    }

    unsigned char asByte() {
        return value;
    }

    long asInteger() {
        return value;
    }

    float asDecimal() {
        return (float)value;
    }

    double asFloat() {
        return value;
    }

    BLONG asLong() {
        return value;
    }

    short asShort() {
        return value;
    }

    long Integer() {
        return (long)value;
    }
    
    long Size() {
        return (long)value;
    }
    
    bool Boolean() {
        return value;
    }
    
    float Decimal() {
        return (float)value;
    }
    
    double Float() {
        return (double)value;
    }
    
    BLONG Long() {
        return (BLONG)value;
    }
    
    Tamgu* same(Tamgu* a) {
        if (value == a->Boolean())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* different(Tamgu* a) {
        if (value != a->Boolean())
            return aTRUE;
        return aFALSE;
    }
    
    
    Tamgu* plus(Tamgu* a, bool itself) {
        if (a->Boolean() || value)
            return aTRUE;
        
        return aFALSE;
    }
    
    Tamgu* minus(Tamgu* a, bool itself) {
        if (value) {
            if (a->Boolean())
                return aFALSE;
            return aTRUE;
        }
        
        if (a->Boolean())
            return aTRUE;
        
        return aFALSE;
    }
    
};


class TamguLoopBoolean : public Tamguboolean {
public:

	TamguLoopBoolean() : Tamguboolean(true) {}

    Tamgu* Put(Tamgu* context, Tamgu* ke, short) {
        value = ke->Boolean();
        return aTRUE;
    }
    
    bool isLoop() {
        return true;
    }
    
    Tamgu* Putvalue(Tamgu* ke, short) {
        value = ke->Boolean();
        return aTRUE;
    }
    
    Tamgu* Clonevalue(Tamgu* ke, short) {
        value = ke->Boolean();
        return aTRUE;
    }
    
	short Type() {
		return a_bloop;
	}

	void Next() {
		value = true - value;
	}

	Tamgu* plusplus() {
		value = true - value;
		return this;
	}

	Tamgu* minusminus() {
		value = true - value;
		return this;
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new TamguLoopBoolean;
	}

};

#endif










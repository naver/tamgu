/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubyte.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgubyte_h
#define tamgubyte_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgubyte;
//This typedef defines a type "byteMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgubyte::*byteMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgubyte : public TamguReference {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<byteMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    unsigned char value;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgubyte(unsigned char v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
        //Do not forget your variable initialisation
        value=v;
    }

    Tamgubyte(unsigned char v) {
        //Do not forget your variable initialisation
        value=v;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* idx, Tamgu* v, short idthread) {
		value = v->Byte();
        return this;
    }
    
	Tamgu* Putvalue(Tamgu* v, short idthread) {
		value = v->Byte();
		return this;
	}


    Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread) {        
        return this;
    }

	bool Checkprecision(Tamgu* r) {
		if (r->Type() <= a_byte)
			return true;

		return false;
	}

    short Type() {
        return Tamgubyte::idtype;
    }

    

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "byte";
    }

    Tamgu* Atom(bool forced=false) {
        if (forced || !protect || reference)
            return new  Tamgubyte(value);
        return this;
    }

    Tamgu* Atomref() {
        TamguReference* r;
        if (!protect || reference)
            r = new  Tamgubyte(value);
        else
            r = this;
        r->reference++;
        r->protect = false;
        return r;
    }

	string Bytes() {
		char buff[] = { (char)value, 0 };
		return buff;
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
        return new Tamgubyte(0);
    }

    TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, byteMethod func, unsigned long arity, string infos);
    
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
	bool isString() {
		return false;
	}
	bool isAtom() {
		return true;
	}
    
    bool checkAtomType(short ty) {
        return (ty == a_byte);
    }

	bool isNumber() {
		return true;
	}

	Tamgu* Succ() {
		return new Tamgubyte(value + 1);
	}

	Tamgu* Pred() {
		return new Tamgubyte(value - 1);
	}

	//---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgubyte(value + 1);
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgubyte(value - 1);
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
		value = 0;
    }
    
	unsigned char Byte() {
		return value;
	}
	
	long Integer() {
		return value;
	}
	
	float Decimal() {
		return value;
	}
	
	double Float() {
		return value;
	}

	BLONG Long() {
		return value;
	}

	short Sort() {
		return value;
	}

	wstring UString() {
		wstring s(L" ");
		s[0] = value;
		return s;
	}

	wstring Getustring(short idthread) {
		wstring s(L" ");
		s[0] = value;
		return s;
	}

	string Getstring(short idthread) {
		string s(" ");
		s[0] = value;
		return s;
	}

	long Getinteger(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	string String() {
		string s(" ");
		s[0] = value;
		return s;
	}
	void storevalue(short u) {
		value = u;
	}
	void storevalue(long u) {
		value = (uchar)u;
	}
	
	void storevalue(double u) {
		value = (uchar)u;
	}
	void storevalue(unsigned char u) {
		value = u;
	}
	void storevalue(string u) {
		if (u.size())
			value = u[0];
		else
			value = 0;
	}
	
	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}
	bool Protectedboolean() {
		Locking _lock(*globalTamgu->booleanlocks[a_byte]);
		if (value == 0)
			return false;
		return true;
	}
    
    //Basic operations    
	Tamgu* andset(Tamgu* a, bool autoself) {
		if (autoself) {
			value &= a->Byte();
			return this;
		}
		unsigned char i = value & a->Byte();
		return new Tamgubyte(i);
	}

	Tamgu* orset(Tamgu* a, bool autoself) {
		if (autoself) {
			value |= a->Byte();
			return this;
		}
		unsigned char i = value | a->Byte();
		return new Tamgubyte(i);

	}

	Tamgu* xorset(Tamgu* a, bool autoself) {
		if (autoself) {
			value ^= a->Byte();
			return this;
		}
		unsigned char i = value ^ a->Byte();
		return new Tamgubyte(i);

	}

	Tamgu* plus(Tamgu* a, bool autoself) {
		if (autoself) {
			value += a->Byte();
			return this;
		}
		unsigned char i = value + a->Byte();
		return new Tamgubyte(i);

	}
	Tamgu* minus(Tamgu* a, bool autoself) {
		if (autoself) {
			value -= a->Byte();
			return this;
		}
		unsigned char i = value - a->Byte();
		return new Tamgubyte(i);
	}

	Tamgu* multiply(Tamgu* a, bool autoself) {
		if (autoself) {
			value *= a->Byte();
			return this;
		}
		unsigned char i = value * a->Byte();
		return new Tamgubyte(i);
	}

	Tamgu* divide(Tamgu* a, bool autoself);
	Tamgu* mod(Tamgu* a, bool autoself);

	Tamgu* shiftleft(Tamgu* a, bool autoself) {
		if (autoself) {
			value <<= a->Byte();
			return this;
		}
		unsigned char i = value << a->Byte();
		return new Tamgubyte(i);
	}


	Tamgu* shiftright(Tamgu* a, bool autoself) {
		if (autoself) {
			value >>= a->Byte();
			return this;
		}
		unsigned char i = value >> a->Byte();
		return new Tamgubyte(i);
	}

	Tamgu* power(Tamgu* a, bool autoself);

	Tamgu* less(Tamgu* a) {
		if (value<a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* more(Tamgu* a) {
		if (value>a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* same(Tamgu* a) {
		if (value == a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* different(Tamgu* a) {
		if (value != a->Byte())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
		if (value <= a->Byte())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* moreequal(Tamgu* a) {
		if (value >= a->Byte())
			return aTRUE;
		return aFALSE;
	}
	long Size() {
		return sizeof(value);
	}

	Tamgu* plusplus() {
		value++;
		return this;
	}

	Tamgu* minusminus() {
		value--;
		return this;
	}
};

#endif








/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgushort.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgushort_h
#define tamgushort_h

#include "tamgufloat.h"
#include "tamgustring.h"
#include "tamguustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgushort;
//This typedef defines a type "shortMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgushort::*shortMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgushort : public TamguReference {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<shortMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    short value;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgushort(short v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
        investigate |= is_number;
        //Do not forget your variable initialisation
        value=v;
    }

    Tamgushort(short v) {
        investigate |= is_number;
        //Do not forget your variable initialisation
        value=v;
    }

    //----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->Short();
    }
    
	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
		value = v->Short();
        return this;
    }
    
	Tamgu* Putvalue(Tamgu* v, short idthread) {
		value = v->Short();
		return this;
	}


    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        
        return this;
    }

	bool Checkprecision(Tamgu* r) {
		if (r->Type() <= a_short)
			return true;

		return false;
	}

    short Type() {
        return a_short;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "short";
    }

	bool isInteger() {
		return true;
	}

	bool isAtom() {
		return true;
	}

    bool checkAtomType(short ty) {
        return (ty == a_short);
    }

	short Typenumber() {
		return a_short;
	}

    

	bool isShort() {
		return true;
	}

    Tamgu* Atom(bool forced=false) {
        if (forced || !protect || reference)
            return new  Tamgushort(value);
        return this;
    }

    Tamgu* Atomref() {
        TamguReference* r;
        if (!protect || reference)
            r = new Tamgushort(value);
        else
            r = this;
        r->reference = 1;
        r->protect = false;
        return r;
    }


	void storevalue(short u) {
		value = u;
	}

	void storevalue(long u) {
		value = (short)u;
	}

	void storevalue(BLONG u) {
		value = (short)u;
	}

	void storevalue(double u) {
		value = (short)u;
	}

	void storevalue(unsigned char u) {
		value = (short)u;
	}

	void storevalue(string u) {
		value = (short)convertinteger(u);
	}

	void storevalue(wstring u) {
		value = (short)convertinteger(u);
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		value = (short)convertinteger(w);
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
        return new Tamgushort(0);
    }

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return new Tamgushort(a->Short());
	}

    TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, shortMethod func, unsigned long arity, string infos);
    
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


	Tamgu* Succ() {
		return new Tamgushort(value + 1);
	}

	Tamgu* Pred() {
		return new Tamgushort(value - 1);
	}

	//---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgushort(value + 1);
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgushort(value - 1);
	}

	Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamguustring* r = globalTamgu->Provideustring(L"");
		r->value = (wchar_t)value;
		return r;
	}

	Tamgu* MethodPrimefactors(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* kvect = Selectaivector(contextualpattern);
		short n = value;
		short dec = 2;
		short ni = n;
		while (n > 1 && (dec*dec) <= ni) {
			if (n%dec == 0) {
				kvect->storevalue((long)dec);
				n /= dec;
			}
			else
				dec++;
		}

		if (n > 1)
			kvect->storevalue((long)n);
		return kvect;
	}

	Tamgu* MethodPrime(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		short n = value;
		if (!n)
			return aFALSE;
		if (n == 2 || n == 3 || n == 5 || n == 7 || n == 11)
			return aTRUE;

		if (!n || (n % 2) == 0 || (n % 3) == 0 || (n % 5) == 0 || (n % 7) == 0 || (n % 11) == 0)
			return aFALSE;

		short dec = 13;
		short ni = n;

		while (n > 1 && (dec*dec) <= ni) {
			if (n%dec == 0)
				return aFALSE;
			dec += 4;
			if (n%dec == 0)
				return aFALSE;
			dec += 2;
			if (n%dec == 0)
				return aFALSE;
			dec += 2;
			if (n%dec == 0)
				return aFALSE;
			dec += 2;
		}

		return aTRUE;
	}

	Tamgu* Methodeven(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if ((value % 2) == 0)
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Methododd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if ((value % 2) == 0)
			return aFALSE;
		return aTRUE;
	}

	Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Methodabs(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(fabs(v));
	}

	Tamgu* Methodacos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(acos(v));
	}

	Tamgu* Methodacosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(acosh(v));
	}

	Tamgu* Methodasin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(asin(v));
	}

	Tamgu* Methodasinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(asinh(v));
	}

	Tamgu* Methodatan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(atan(v));
	}

	Tamgu* Methodatanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(atanh(v));
	}

	Tamgu* Methodcbrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(cbrt(v));
	}

	Tamgu* Methodcos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(cos(v));
	}

	Tamgu* Methodcosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(cosh(v));
	}

	Tamgu* Methoderf(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(erf(v));
	}

	Tamgu* Methoderfc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(erfc(v));
	}

	Tamgu* Methodexp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(exp(v));
	}

	Tamgu* Methodexp2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(exp2(v));
	}

	Tamgu* Methodexpm1(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(expm1(v));
	}

	Tamgu* Methodfloor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(floor(v));
	}

	Tamgu* Methodlgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(lgamma(v));
	}

	Tamgu* Methodln(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(log(v));
	}

	Tamgu* Methodlog(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(log10(v));
	}

	Tamgu* Methodlog1p(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(log1p(v));
	}

	Tamgu* Methodlog2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(log2(v));
	}

	Tamgu* Methodlogb(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(logb(v));
	}

	Tamgu* Methodnearbyint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(nearbyint(v));
	}

	Tamgu* Methodrint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(rint(v));
	}

	Tamgu* Methodround(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(round(v));
	}

	Tamgu* Methodsin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(sin(v));
	}

	Tamgu* Methodsinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(sinh(v));
	}

	Tamgu* Methodsqrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(sqrt(v));
	}

	Tamgu* Methodtan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(tan(v));
	}

	Tamgu* Methodtanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(tanh(v));
	}

	Tamgu* Methodtgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(tgamma(v));
	}

	Tamgu* Methodtrunc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		double v = (double)value;
		return globalTamgu->Providefloat(trunc(v));
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

    

	string String() {
		return convertfromnumber(value);		
	}

    wstring UString() {
        return wconvertfromnumber(value);        
    }

    void Setstring(string& v, short idthread) {
        v = convertfromnumber(value);
    }

    void Setstring(wstring& v, short idthread) {
        v = wconvertfromnumber(value);
    }


	short Short() {
		return value;
	}

	long Integer() {
		return value;
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

	wstring Getustring(short idthread) {
		
		return wconvertfromnumber(value);
		
	}

	string Getstring(short idthread) {
		
		return convertfromnumber(value);
		
	}

	long Getinteger(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	bool Protectedboolean() {
        Locking _lock(*globalTamgu->booleanlocks[a_short]);
        if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return value;
	}

	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
		if (itself) {
			value &= a->Short();
			return this;
		}
		return new Tamgushort(value & a->Short());
	}

	Tamgu* orset(Tamgu* a, bool itself) {

		if (itself) {
			value |= a->Short();
			return this;
		}
		return new Tamgushort(value | a->Short());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		if (itself) {
			value ^= a->Short();
			return this;
		}
		return new Tamgushort(value ^ a->Short());
	}


	Tamgu* plus(Tamgu* a, bool itself);
	Tamgu* multiply(Tamgu* a, bool itself);
	Tamgu* shiftleft(Tamgu* a, bool itself);

	Tamgu* minus(Tamgu* a, bool itself) {
		if (itself) {
			value -= a->Short();
			return this;
		}
		return new Tamgushort(value - a->Short());
	}


	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		v = (double)value / v;
		return globalTamgu->Providefloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		if (itself) {
			v = pow(v, a->Float());
			value = (short)v;
			return this;
		}
		v = pow(v, a->Float());
		return globalTamgu->Providefloat(v);
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		if (itself) {
			value >>= a->Short();
			return this;
		}
		return new Tamgushort(value >> a->Short());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		short v = a->Short();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");

		if (itself) {
			value %= a->Short();
			return this;
		}

		v = value % v;
		return new Tamgushort(v);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value < a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value > a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value == a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif
		if (value != a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value <= a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->Short())
			return aTRUE;
		return aFALSE;
	}

};

#endif










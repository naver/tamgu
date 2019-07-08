/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgulong128.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgulong_h
#define tamgulong_h

#ifdef INTELINTRINSICS
#ifdef WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "tamgufloat.h"
#include "tamgustring.h"


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgulong128;
//This typedef defines a type "longMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgulong128::*longMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgulong128 : public TamguReference {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<longMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	__m128 value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgulong128(__m128 v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Tamgulong128(__m128 v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        if (v->Type() == idtype) {
            value = ((Tamgulong128*)v)->value;
        }
        else {
            value = _mm_cvtsi64_si128(v->Long());
        }
    }
    
	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
        if (v->Type() == idtype) {
            value = ((Tamgulong128*)v)->value;
        }
        else {
            value = _mm_cvtsi64_si128(v->Long());
        }
		return this;
	}

	Tamgu* Putvalue(Tamgu* v, short idthread) {
        if (v->Type() == idtype) {
            value = ((Tamgulong128*)v)->value;
        }
        else {
            value = _mm_cvtsi64_si128(v->Long());
        }
		return this;
	}


	Tamgu* Get(Tamgu* context, Tamgu* v, short idthread) {

		return this;
	}

	short Type() {
		return Tamgulong128::idtype;
	}

	string Typename() {
		return "long128";
	}

	bool isInteger() {
		return true;
	}

	bool isAtom() {
		return true;
	}

	short Typenumber() {
        return Tamgulong128::idtype;
	}

	bool isNumber() {
		return true;
	}

	bool isLong() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Tamgulong128(value);
		return this;
	}

	void storevalue(short u) {
        value = _mm_cvtsi64_si128(u);
	}

	void storevalue(long u) {
        value = _mm_cvtsi64_si128(u);
	}

	void storevalue(BLONG u) {
        value = _mm_cvtsi64_si128(u);
	}

	void storevalue(double u) {
        value = _mm_cvtsi64_si128((BLONG)u);
	}

	void storevalue(unsigned char u) {
        value = _mm_cvtsi64_si128((BLONG)u);
	}

	void storevalue(string u) {
        value = _mm_cvtsi64_si128(convertinteger(u));
	}

	void storevalue(wstring u) {
        value = _mm_cvtsi64_si128(convertinteger(u));
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
        value = _mm_cvtsi64_si128(convertinteger(w));
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
		return new Tamgulong128(_mm_setzero_si128());
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
        if (a->Type() == idtype) {
            return new Tamgulong128(((Tamgulong128*)a)->value);
        }
        else {
            __m128 v = _mm_cvtsi64_si128(a->Long());
            return new Tamgulong128(v);
        }
	}


	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, longMethod func, unsigned long arity, string infos);
	
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
		return new Tamgulong128(value + 1);
	}

	Tamgu* Pred() {
		return new Tamgulong128(value - 1);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgulong128(value + 1);
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgulong128(value - 1);
	}

	Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


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
        value = _mm_setzero_si128();
	}

	


	string String() {
		
		return convertfromnumber(value);
		
	}

	long Integer() {
		return (long)value;
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
		return (long)value;
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

	wstring UString() {
		
		return wconvertfromnumber(value);
		
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	bool Protectedboolean() {
        Locking _lock(*globalTamgu->booleanlocks[a_long]);
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
			value &= a->Long();
			return this;
		}
		return new Tamgulong128(value & a->Long());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		if (itself) {
			value |= a->Long();
			return this;
		}
		return new Tamgulong128(value | a->Long());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		if (itself) {
			value ^= a->Long();
			return this;
		}
		return new Tamgulong128(value ^ a->Long());
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		if (itself) {
			value += a->Long();
			return this;
		}

		if (a->isFloat()) {
			double v = value;
			return globalTamgu->Providefloat(v + a->Float());
		}

		return new Tamgulong128(value + a->Long());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		if (itself) {
			value -= a->Long();
			return this;
		}
		return new Tamgulong128(value - a->Long());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		if (itself) {
			value *= a->Long();
			return this;
		}
		
		if (a->isFloat()) {
			double v = value;
			return globalTamgu->Providefloat(v * a->Float());
		}

		return new Tamgulong128(value * a->Long());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		v = (double)value / v;
		
		if (itself) {
			value = v;
			return this;
		}

		return globalTamgu->Providefloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = (double)value;
		v = pow(v, a->Float());
		if (itself) {
			value = v;
			return this;
		}
		return globalTamgu->Providefloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		if (itself) {
			value <<= a->Long();
			return this;
		}
		return new Tamgulong128(value << a->Long());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		if (itself) {
			value >>= a->Long();
			return this;
		}
		return new Tamgulong128(value >> a->Long());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		BLONG v = a->Long();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		if (itself) {
			value %= v;
			return this;
		}

		v = value % v;
		return new Tamgulong128(v);
	}

	Tamgu* less(Tamgu* a) {
		if (value < a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
		if (value > a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
		if (value == a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
		if (value != a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
		if (value <= a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
		if (value >= a->Long())
			return aTRUE;
		return aFALSE;
	}
};


class TamguLoopLong : public Tamgulong128 {
public:

	vector<BLONG> interval;
	long position;
	Tamgu* function;

	TamguLoopLong(TamguGlobal* g) : Tamgulong128(0) {

		position = 0;
		function = NULL;
	}

	short Type() {
		return a_lloop;
	}


	void Addfunction(Tamgu* f) {
		function = f;
	}

	void Callfunction();

    unsigned long CallBackArity() {
        return P_TWO;
    }

	Tamgu* Put(Tamgu*, Tamgu*, short);
	Tamgu* Vector(short idthread);
    Tamgu* Putvalue(Tamgu* ke, short idthread) {
        return Put(aNULL, ke, idthread);
    }

	long Size() {
		return interval.size();
	}

	void Next() {
		if (interval.size() == 0)
			return;

		position++;
		if (position >= interval.size()) {
			if (function != NULL)
				Callfunction();
			position = 0;
		}
		value = interval[position];
	}

	Tamgu* plusplus() {
		if (interval.size() == 0)
			return this;
		position++;
		if (position >= interval.size()) {
			if (function != NULL)
				Callfunction();
			position = 0;
		}
		value = interval[position];
		return this;
	}

	Tamgu* minusminus() {
		if (interval.size() == 0)
			return this;
		position--;
		if (position < 0) {
			if (function != NULL)
				Callfunction();
			position = interval.size() - 1;
		}
		value = interval[position];
		return this;
	}


	Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
		TamguLoopLong* a=new TamguLoopLong(globalTamgu);
		a->function = f;
		return a;
	}

	Tamgu* plus(Tamgu* b, bool autoself) {
		if (interval.size() == 0)
			return this;

		if (autoself) {
			position += b->Integer();

			position = abs(position) % interval.size();

			value = interval[position];
			return this;
		}

		return Tamgulong128::plus(b, autoself);
	}

	Tamgu* minus(Tamgu* b, bool autoself) {
		if (interval.size() == 0)
			return this;

		if (autoself) {
			position -= b->Integer();

			position = (interval.size() - abs(position)) % interval.size();

			value = interval[position];
			return this;
		}

		return Tamgulong128::minus(b, autoself);
	}
};

#endif
#endif










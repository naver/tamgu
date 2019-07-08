/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgulong.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgulong_h
#define tamgulong_h

#include "tamgufloat.h"
#include "tamgustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgulong;
//This typedef defines a type "longMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgulong::*longMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgulong : public TamguReference {
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
	BLONG value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgulong(BLONG v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Tamgulong(BLONG v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->Long();
    }
    
	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
		value = v->Long();
		return this;
	}

	Tamgu* Putvalue(Tamgu* v, short idthread) {
		value = v->Long();
		return this;
	}


	Tamgu* Get(Tamgu* context, Tamgu* v, short idthread) {

		return this;
	}

	bool Checkprecision(Tamgu* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}

	short Type() {
		return Tamgulong::idtype;
	}

	

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "long";
	}

	bool isInteger() {
		return true;
	}

	bool isAtom() {
		return true;
	}

	short Typenumber() {
		return a_long;
	}

	bool isNumber() {
		return true;
	}

	bool isLong() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Tamgulong(value);
		return this;
	}

	string Bytes() {
		u_tamgulong x(value);
		unsigned char buff[u_tamgulongsz + 1];
		memcpy(buff, x.t, u_tamgulongsz);
		buff[u_tamgulongsz] = 0;
		return (char*)buff;
	}

	void storevalue(short u) {
		value = u;
	}

	void storevalue(long u) {
		value = u;
	}

	void storevalue(BLONG u) {
		value = u;
	}

	void storevalue(double u) {
		value = (BLONG)u;
	}

	void storevalue(unsigned char u) {
		value = (BLONG)u;
	}

	void storevalue(string u) {
		value = convertinteger(u);
	}

	void storevalue(wstring u) {
		value = convertinteger(u);
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		value = convertinteger(w);
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
		return new Tamgulong(0);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return new Tamgulong(a->Long());
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
		return new Tamgulong(value + 1);
	}

	Tamgu* Pred() {
		return new Tamgulong(value - 1);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgulong(value + 1);
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgulong(value - 1);
	}

	Tamgu* MethodPrimefactors(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Tamgu* kvect = Selectaivector(contextualpattern);
		BLONG n = value;
		long dec = 2;
		BLONG ni = n;
		while (n > 1 && (dec*dec) <= ni) {
			if (n%dec == 0) {
				kvect->storevalue(dec);
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
		BLONG n = value;
		if (!n)
			return aFALSE;
		if (n == 2 || n == 3 || n == 5 || n == 7 || n == 11)
			return aTRUE;

		if (!n || (n % 2) == 0 || (n % 3) == 0 || (n % 5) == 0 || (n % 7) == 0 || (n % 11) == 0)
			return aFALSE;

		BLONG dec = 13;
		BLONG ni = n;

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
		return new Tamgulong(value & a->Long());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		if (itself) {
			value |= a->Long();
			return this;
		}
		return new Tamgulong(value | a->Long());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		if (itself) {
			value ^= a->Long();
			return this;
		}
		return new Tamgulong(value ^ a->Long());
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

		return new Tamgulong(value + a->Long());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		if (itself) {
			value -= a->Long();
			return this;
		}
		return new Tamgulong(value - a->Long());
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

		return new Tamgulong(value * a->Long());
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
		return new Tamgulong(value << a->Long());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		if (itself) {
			value >>= a->Long();
			return this;
		}
		return new Tamgulong(value >> a->Long());
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
		return new Tamgulong(v);
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


class TamguLoopLong : public Tamgulong {
public:

	vector<BLONG> interval;
	long position;
	Tamgu* function;

	TamguLoopLong(TamguGlobal* g) : Tamgulong(0) {

		position = 0;
		function = NULL;
	}

    bool isLoop() {
        return true;
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

		return Tamgulong::plus(b, autoself);
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

		return Tamgulong::minus(b, autoself);
	}
};

#endif










/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgudecimal.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef Tamgudecimal_h
#define Tamgudecimal_h

#include "tamguint.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgudecimal;
//This typedef defines a type "floatMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgudecimal::*decimalMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgudecimal : public TamguReference {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<decimalMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	float value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgudecimal(float v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
     investigate |= is_number;
		//Do not forget your variable initialisation
		value = v;
	}

	Tamgudecimal(float v) {
     investigate |= is_number;
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->Decimal();
    }

	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
		value = v->Decimal();
		return this;
	}

	Tamgu* Putvalue(Tamgu* v, short idthread) {
		value = v->Decimal();
		return this;
	}


	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {

		return this;
	}

	bool Checkprecision(Tamgu* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}


	short Type() {
        return a_decimal;
	}

	

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "float";
	}

	bool isAtom() {
		return true;
	}

    bool checkAtomType(short ty) {
        return (ty == a_decimal);
    }

	short Typenumber() {
		return a_decimal;
	}

	

	bool isDecimal() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Tamgudecimal(value);
		return this;
	}

	void storevalue(short u) {
		value = (short)u;
	}

	void storevalue(long u) {
		value = (float)u;
	}

	void storevalue(float u) {
		value = u;
	}

	void storevalue(double u) {
		value = (float)u;
	}

	void storevalue(unsigned char u) {
		value = (float)u;
	}

	void storevalue(string u) {
		value = convertfloat(u);
	}

	void storevalue(wstring u) {
		value = convertfloat(u);
	}

	void storevalue(wchar_t u) {
		wstring w;
		w = u;
		value = convertfloat(w);
	}

	Tamgu* Succ() {
		return new Tamgudecimal(value + 1);
	}

	Tamgu* Pred() {
		return new Tamgudecimal(value - 1);
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
		return new Tamgudecimal(0);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return new Tamgudecimal(a->Decimal());
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, decimalMethod func, unsigned long arity, string infos, short typereturn);
	
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

	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(value + 1);
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(value - 1);
	}

    Tamgu* MethodBits(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (callfunc->Size() == 1) {
            float32 f(0);
            f.bits = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
            return new Tamgudecimal(f.v);
        }
        float32 f(value);
        return globalTamgu->Provideint(f.v);
    }
    
    Tamgu* MethodMantissa(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        float result;
        int n = 0;
        
        result = frexp (value , &n);
        return new Tamgudecimal(result);
    }
    
    Tamgu* MethodExponent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        int n = 0;
        
        frexp (value , &n);
        return globalTamgu->Provideint((long)n);
    }

	Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return Invert(true);
	}


	Tamgu* Methodeven(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		long v = value;
		if ((v % 2) == 0)
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Methododd(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		long v = value;
		if ((v % 2) == 0)
			return aFALSE;
		return aTRUE;
	}

	Tamgu* Methodabs(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(fabs(value));
	}

	Tamgu* Methodacos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(acos(value));
	}

	Tamgu* Methodacosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(acosh(value));
	}

	Tamgu* Methodasin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(asin(value));
	}

	Tamgu* Methodasinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(asinh(value));
	}

	Tamgu* Methodatan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(atan(value));
	}

	Tamgu* Methodatanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(atanh(value));
	}

	Tamgu* Methodcbrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(cbrt(value));
	}

	Tamgu* Methodcos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(cos(value));
	}

	Tamgu* Methodcosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(cosh(value));
	}

	Tamgu* Methoderf(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(erf(value));
	}

	Tamgu* Methoderfc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(erfc(value));
	}

	Tamgu* Methodexp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(exp(value));
	}

	Tamgu* Methodexp2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(exp2(value));
	}

	Tamgu* Methodexpm1(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(expm1(value));
	}

	Tamgu* Methodfloor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(floor(value));
	}

	Tamgu* Methodlgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(lgamma(value));
	}

	Tamgu* Methodln(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(log(value));
	}

	Tamgu* Methodlog(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(log10(value));
	}

	Tamgu* Methodlog1p(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(log1p(value));
	}

	Tamgu* Methodlog2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(log2(value));
	}

	Tamgu* Methodlogb(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(logb(value));
	}

	Tamgu* Methodnearbyint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(nearbyint(value));
	}

	Tamgu* Methodrint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(rint(value));
	}

	Tamgu* Methodround(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(round(value));
	}

	Tamgu* Methodsin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(sin(value));
	}

	Tamgu* Methodsinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(sinh(value));
	}

	Tamgu* Methodsqrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(sqrt(value));
	}

	Tamgu* Methodtan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(tan(value));
	}

	Tamgu* Methodtanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(tanh(value));
	}

	Tamgu* Methodtgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(tgamma(value));
	}

	Tamgu* Methodtrunc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgudecimal(trunc(value));
	}

	Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDegree(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRadian(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

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

	

	wstring UString() {
		return wconvertfromnumber(value);
	}

	string String() {
		return convertfromnumber(value);
	}

    void Setstring(string& v, short idthread) {
        v = convertfromnumber(value);
    }

    void Setstring(wstring& v, short idthread) {
        v = wconvertfromnumber(value);
    }

	long Integer() {
		return (long)value;
	}

	double Float() {
		return value;
	}

	float Decimal() {
		return value;
	}

	BLONG Long() {
		return (BLONG)value;
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
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	bool Protectedboolean() {
        Locking _lock(*globalTamgu->booleanlocks[a_decimal]);
        if (value == 0)
			return false;
		return true;
	}

	//Basic operations
	virtual long Size() {
		return sizeof(value);
	}

	virtual Tamgu* plusplus() {
		value++;
		return this;
	}

	virtual Tamgu* minusminus() {
		value--;
		return this;
	}


	Tamgu* andset(Tamgu* a, bool itself) {
        if (itself) {
            value = FloatAnd(value, a->Decimal());
            return this;
        }
        
        return new Tamgudecimal(FloatAnd(value, a->Decimal()));
	}

	Tamgu* orset(Tamgu* a, bool itself) {
        if (itself) {
            value = FloatOr(value, a->Decimal());
            return this;
        }
        
        return new Tamgudecimal(FloatOr(value, a->Decimal()));
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
        if (itself) {
            value = DoubleXor(value, a->Decimal());
            return this;
        }
        
        return new Tamgudecimal(FloatXor(value, a->Decimal()));
	}


	virtual Tamgu* plus(Tamgu* a, bool itself) {
		if (itself) {
			value += a->Decimal();
			return this;
		}
		return new Tamgudecimal(value + a->Decimal());
	}

	virtual Tamgu* minus(Tamgu* a, bool itself) {
		if (itself) {
			value -= a->Decimal();
			return this;
		}
		return new Tamgudecimal(value - a->Decimal());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		if (itself) {
			value *= a->Decimal();
			return this;
		}
		return new Tamgudecimal(value * a->Decimal());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		float v = a->Decimal();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		if (itself) {
			value /= v;
			return this;
		}

		v = value / v;
		return new Tamgudecimal(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {		
		if (itself) {
			value = pow(value, a->Decimal());
			return this;
		}

		double v = pow(value, a->Decimal());
		return new Tamgudecimal(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
        float32 d(value);
        d.bits <<= a->Short();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return new Tamgudecimal(d.v);
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
        float32 d(value);
        d.bits >>= a->Short();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return new Tamgudecimal(d.v);
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		if (itself) {
			value = (long)value % v;
			return this;
		}

		v = (long)value % v;
		return new Tamgudecimal(v);
	}

	Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value < a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value > a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value == a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aTRUE;
#endif
		if (value != a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value <= a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->Decimal())
			return aTRUE;
		return aFALSE;
	}
};

class TamguLoopDecimal : public Tamgudecimal {
public:

	vector<float> interval;
	long position;
	Tamgu* function;

	TamguLoopDecimal(TamguGlobal* g) : Tamgudecimal(0) {

		position = 0;
		function = NULL;
	}

    bool isLoop() {
        return true;
    }
    
	void Addfunction(Tamgu* f) {
		function = f;
	}

	void Callfunction();

	short Type() {
		return a_dloop;
	}

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
		TamguLoopDecimal* a= new TamguLoopDecimal(globalTamgu);
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

		return Tamgudecimal::plus(b, autoself);
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

		return Tamgudecimal::minus(b, autoself);
	}
};

#endif









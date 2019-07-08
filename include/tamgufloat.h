/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufloat.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgufloat_h
#define tamgufloat_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgufloat;
//This typedef defines a type "floatMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgufloat::*floatMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgufloat : public TamguReference {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<floatMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	double value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgufloat(double v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
		//Do not forget your variable initialisation
		value = v;
	}

	Tamgufloat(double v) {
		//Do not forget your variable initialisation
		value = v;
	}

	//----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->Float();
    }

	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
		value = v->Float();
		return this;
	}

	Tamgu* Putvalue(Tamgu* v, short idthread) {
		value = v->Float();
		return this;
	}

	Tamgu* Get(Tamgu* context, Tamgu* value, short idthread) {

		return this;
	}


	short Type() {
		return Tamgufloat::idtype;
	}

	

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "float";
	}

	bool isAtom() {
		return true;
	}

	short Typenumber() {
		return a_float;
	}

	bool isNumber() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return globalTamgu->Providefloat(value);
		return this;
	}

	void storevalue(short u) {
		value = (short)u;
	}

	void storevalue(long u) {
		value = (double)u;
	}

	void storevalue(BLONG u) {
		value = (double)u;
	}
	
	void storevalue(double u) {
		value = u;
	}

	void storevalue(unsigned char u) {
		value = (double)u;
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
		return globalTamgu->Providefloat(value + 1);
	}

	Tamgu* Pred() {
		return globalTamgu->Providefloat(value - 1);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

    Tamgu* Newpureinstance(short idthread) {
        return new Tamgufloat(0);
    }

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Providefloat(0);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->Providefloat(a->Float());
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, floatMethod func, unsigned long arity, string infos);
	
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

    Tamgu* MethodBits(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodMantissa(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double result;
        int n = 0;
        
        result = frexp (value , &n);
        return globalTamgu->Providefloat(result);
    }

    Tamgu* MethodExponent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return Invert(true);
	}

	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(value + 1);
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(value - 1);
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
		return globalTamgu->Providefloat(fabs(value));
	}

	Tamgu* Methodacos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(acos(value));
	}

	Tamgu* Methodacosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(acosh(value));
	}

	Tamgu* Methodasin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(asin(value));
	}

	Tamgu* Methodasinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(asinh(value));
	}

	Tamgu* Methodatan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(atan(value));
	}

	Tamgu* Methodatanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(atanh(value));
	}

	Tamgu* Methodcbrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(cbrt(value));
	}

	Tamgu* Methodcos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(cos(value));
	}

	Tamgu* Methodcosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(cosh(value));
	}

	Tamgu* Methoderf(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(erf(value));
	}

	Tamgu* Methoderfc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(erfc(value));
	}

	Tamgu* Methodexp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(exp(value));
	}

	Tamgu* Methodexp2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(exp2(value));
	}

	Tamgu* Methodexpm1(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(expm1(value));
	}

	Tamgu* Methodfloor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(floor(value));
	}

	Tamgu* Methodlgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(lgamma(value));
	}

	Tamgu* Methodln(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(log(value));
	}

	Tamgu* Methodlog(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(log10(value));
	}

	Tamgu* Methodlog1p(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(log1p(value));
	}

	Tamgu* Methodlog2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(log2(value));
	}

	Tamgu* Methodlogb(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(logb(value));
	}

	Tamgu* Methodnearbyint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(nearbyint(value));
	}

	Tamgu* Methodrint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(rint(value));
	}

	Tamgu* Methodround(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(round(value));
	}

	Tamgu* Methodsin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(sin(value));
	}

	Tamgu* Methodsinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(sinh(value));
	}

	Tamgu* Methodsqrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(sqrt(value));
	}

	Tamgu* Methodtan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(tan(value));
	}

	Tamgu* Methodtanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(tanh(value));
	}

	Tamgu* Methodtgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(tgamma(value));
	}

	Tamgu* Methodtrunc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providefloat(trunc(value));
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

	wstring UString() {
		
		return wconvertfromnumber(value);
		
	}

	string String() {
		
		return convertfromnumber(value);
		
	}

	long Integer() {

		return (long)value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	BLONG Long() {
		return (long)value;
	}

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	bool Protectedboolean() {
        Locking _lock(*globalTamgu->booleanlocks[a_float]);
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
            value = DoubleAnd(value, a->Float());
            return this;
		}

		return globalTamgu->Providefloat(DoubleAnd(value, a->Float()));
	}

	Tamgu* orset(Tamgu* a, bool itself) {
        if (itself) {
            value = DoubleOr(value, a->Float());
            return this;
        }
        
        return globalTamgu->Providefloat(DoubleOr(value, a->Float()));
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
        if (itself) {
            value = DoubleXor(value, a->Float());
            return this;
        }
        
        return globalTamgu->Providefloat(DoubleXor(value, a->Float()));
	}


	virtual Tamgu* plus(Tamgu* a, bool itself) {
		if (itself) {
			value += a->Float();
			return this;
		}
		return globalTamgu->Providefloat(value + a->Float());
	}

	virtual Tamgu* minus(Tamgu* a, bool itself) {
		if (itself) {
			value -= a->Float();
			return this;
		}
		return globalTamgu->Providefloat(value - a->Float());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		if (itself) {
			value *= a->Float();
			return this;
		}
		return globalTamgu->Providefloat(value * a->Float());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		if (itself) {
			value /= v;
			return this;
		}

		v = value / v;
		return globalTamgu->Providefloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {		
		if (itself) {
			value = pow(value, a->Float());
			return this;
		}

		double v = pow(value, a->Float());
		return globalTamgu->Providefloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
        double64 d(value);
        d.bits <<= a->Long();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return globalTamgu->Providefloat(d.v);
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
        double64 d(value);
        d.bits >>= a->Long();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return globalTamgu->Providefloat(d.v);
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
		return globalTamgu->Providefloat(v);
	}

	Tamgu* less(Tamgu* a) {
		if (value < a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
		if (value > a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
		if (value == a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
		if (value != a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
		if (value <= a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
		if (value >= a->Float())
			return aTRUE;
		return aFALSE;
	}
};

//---------------------------------------------------------------------------------------------------------------------
class Tamgufloatbuff : public Tamgufloat {
public:
	long idx;
	bool used;

	Tamgufloatbuff(long i) : Tamgufloat(0) {
		//Do not forget your variable initialisation
		idx = i;
		used = false;
	}

    bool Candelete() {
        return false;
    }
    
	void Resetreference(short r) {
        reference -= r;
        if (reference <= 0) {
            if (!protect) {
                reference = 0;
                protect = true;
   
                used = false;
                value = 0;
                globalTamgu->fempties.push_back(idx);
            }
        }
	}
};

class TamguLoopFloat : public Tamgufloat {
public:
	
	vector<double> interval;
	long position;
	Tamgu* function;

	TamguLoopFloat(TamguGlobal* g) : Tamgufloat(0) {
		
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
		return a_floop;
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
		TamguLoopFloat* a = new TamguLoopFloat(globalTamgu);
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

		return Tamgufloat::plus(b, autoself);
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

		return Tamgufloat::minus(b, autoself);
	}
};


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguatomicfloat;
    //This typedef defines a type "floatMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguatomicfloat::*atomicfloatMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

class Tamguatomicfloat : public TamguReference {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<atomicfloatMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    std::atomic<double> value;
    
        //---------------------------------------------------------------------------------------------------------------------
    Tamguatomicfloat(double v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
            //Do not forget your variable initialisation
        value = v;
    }
    
    Tamguatomicfloat(double v) {
            //Do not forget your variable initialisation
        value = v;
    }
    
        //----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->Float();
    }
    
    Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
        value = v->Float();
        return this;
    }
    
    Tamgu* Putvalue(Tamgu* v, short idthread) {
        value = v->Float();
        return this;
    }
    
    Tamgu* Get(Tamgu* context, Tamgu* value, short idthread) {
        
        return this;
    }
    
    
    short Type() {
        return Tamgufloat::idtype;
    }
    
    string Typename() {
        return "a_float";
    }
    
    bool isAtom() {
        return true;
    }
    
    short Typenumber() {
        return a_float;
    }
    
    bool isNumber() {
        return true;
    }
    
    bool isFloat() {
        return true;
    }
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference)
            return new Tamguatomicfloat(value);
        return this;
    }
    
    void storevalue(short u) {
        value = (short)u;
    }
    
    void storevalue(long u) {
        value = (double)u;
    }
    
    void storevalue(BLONG u) {
        value = (double)u;
    }
    
    void storevalue(double u) {
        value = u;
    }
    
    void storevalue(unsigned char u) {
        value = (double)u;
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
        return new Tamguatomicfloat(value + 1);
    }
    
    Tamgu* Pred() {
        return new Tamguatomicfloat(value - 1);
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
        return new Tamguatomicfloat(0);
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        return new Tamguatomicfloat(a->Float());
    }
    
    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }
    
    static void AddMethod(TamguGlobal* g, string name, atomicfloatMethod func, unsigned long arity, string infos);

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
    
    Tamgu* MethodBits(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodMantissa(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double result;
        int n = 0;
        
        result = frexp (value , &n);
        return new Tamguatomicfloat(result);
    }
    
    Tamgu* MethodExponent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Invert(true);
    }
    
    Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(value + 1);
    }
    
    Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(value - 1);
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
        return new Tamguatomicfloat(fabs(value));
    }
    
    Tamgu* Methodacos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(acos(value));
    }
    
    Tamgu* Methodacosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(acosh(value));
    }
    
    Tamgu* Methodasin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(asin(value));
    }
    
    Tamgu* Methodasinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(asinh(value));
    }
    
    Tamgu* Methodatan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(atan(value));
    }
    
    Tamgu* Methodatanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(atanh(value));
    }
    
    Tamgu* Methodcbrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(cbrt(value));
    }
    
    Tamgu* Methodcos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(cos(value));
    }
    
    Tamgu* Methodcosh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(cosh(value));
    }
    
    Tamgu* Methoderf(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(erf(value));
    }
    
    Tamgu* Methoderfc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(erfc(value));
    }
    
    Tamgu* Methodexp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(exp(value));
    }
    
    Tamgu* Methodexp2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(exp2(value));
    }
    
    Tamgu* Methodexpm1(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(expm1(value));
    }
    
    Tamgu* Methodfloor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(floor(value));
    }
    
    Tamgu* Methodlgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(lgamma(value));
    }
    
    Tamgu* Methodln(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(log(value));
    }
    
    Tamgu* Methodlog(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(log10(value));
    }
    
    Tamgu* Methodlog1p(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(log1p(value));
    }
    
    Tamgu* Methodlog2(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(log2(value));
    }
    
    Tamgu* Methodlogb(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(logb(value));
    }
    
    Tamgu* Methodnearbyint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(nearbyint(value));
    }
    
    Tamgu* Methodrint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(rint(value));
    }
    
    Tamgu* Methodround(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(round(value));
    }
    
    Tamgu* Methodsin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(sin(value));
    }
    
    Tamgu* Methodsinh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(sinh(value));
    }
    
    Tamgu* Methodsqrt(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(sqrt(value));
    }
    
    Tamgu* Methodtan(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(tan(value));
    }
    
    Tamgu* Methodtanh(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(tanh(value));
    }
    
    Tamgu* Methodtgamma(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(tgamma(value));
    }
    
    Tamgu* Methodtrunc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicfloat(trunc(value));
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
    
    wstring UString() {
        
        return wconvertfromnumber(value);
        
    }
    
    string String() {
        
        return convertfromnumber(value);
        
    }
    
    long Integer() {
        
        return (long)value;
    }
    
    float Decimal() {
        return (float)value;
    }
    
    double Float() {
        return value;
    }
    
    BLONG Long() {
        return (long)value;
    }
    
    bool Boolean() {
        if (value == 0)
            return false;
        return true;
    }
    
    bool Protectedboolean() {
        if (value == 0)
            return false;
        return true;
    }
    
        //Basic operations
    virtual long Size() {
        return sizeof(value);
    }
    
    virtual Tamgu* plusplus() {
        value = value + 1;
        return this;
    }
    
    virtual Tamgu* minusminus() {
        value = value - 1;
        return this;
    }
    
    
    Tamgu* andset(Tamgu* a, bool itself) {
        double64 d(value);
        d.bits &= a->Long();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return new Tamguatomicfloat(d.v);
    }
    
    Tamgu* orset(Tamgu* a, bool itself) {
        double64 d(value);
        d.bits |= a->Long();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return new Tamguatomicfloat(d.v);
    }
    
    Tamgu* xorset(Tamgu* a, bool itself) {
        double64 d(value);
        d.bits ^= a->Long();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return new Tamguatomicfloat(d.v);
    }
    
    
    virtual Tamgu* plus(Tamgu* a, bool itself) {
        if (itself) {
            value = value + a->Float();
            return this;
        }
        return new Tamguatomicfloat(value + a->Float());
    }
    
    virtual Tamgu* minus(Tamgu* a, bool itself) {
        if (itself) {
            value = value - a->Float();
            return this;
        }
        return new Tamguatomicfloat(value - a->Float());
    }
    
    Tamgu* multiply(Tamgu* a, bool itself) {
        if (itself) {
            value = value * a->Float();
            return this;
        }
        return new Tamguatomicfloat(value * a->Float());
    }
    
    Tamgu* divide(Tamgu* a, bool itself) {
        double v = a->Float();
        if (v == 0)
            return globalTamgu->Returnerror("Error: Divided by 0");
        if (itself) {
            value = value / v;
            return this;
        }
        
        v = value / v;
        return new Tamguatomicfloat(v);
    }
    
    Tamgu* power(Tamgu* a, bool itself) {
        if (itself) {
            value = pow(value, a->Float());
            return this;
        }
        
        double v = pow(value, a->Float());
        return new Tamguatomicfloat(v);
    }
    
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        double64 d(value);
        d.bits <<= a->Long();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return new Tamguatomicfloat(d.v);
    }
    
    Tamgu* shiftright(Tamgu* a, bool itself) {
        double64 d(value);
        d.bits >>= a->Long();
        
        if (itself) {
            value = d.v;
            return this;
        }
        
        return new Tamguatomicfloat(d.v);
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
        return new Tamguatomicfloat(v);
    }
    
    Tamgu* less(Tamgu* a) {
        if (value < a->Float())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* more(Tamgu* a) {
        if (value > a->Float())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* same(Tamgu* a) {
        if (value == a->Float())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* different(Tamgu* a) {
        if (value != a->Float())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* lessequal(Tamgu* a) {
        if (value <= a->Float())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* moreequal(Tamgu* a) {
        if (value >= a->Float())
            return aTRUE;
        return aFALSE;
    }
};

#endif









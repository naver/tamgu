/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgucomplex.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef Tamgucomplex_h
#define Tamgucomplex_h
#include <complex>
#include <cmath>
#include "tamguint.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgucomplex;
//This typedef defines a type "floatMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgucomplex::*complexMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgucomplex : public TamguReference {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<complexMethod> methods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
    std::complex<double> value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgucomplex(double d, double i, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent), value(d,i) {
        investigate |= is_number;
	}

    Tamgucomplex(double d, double i) : value(d,i) {
        investigate |= is_number;
    }

	Tamgucomplex(std::complex<double> v) : value(v) {
     investigate |= is_number;
		//Do not forget your variable initialisation
	}

	//----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        if (v->Type() == a_complex)
            value = ((Tamgucomplex*)v)->value;
        else
            value = v->Float();
    }

	Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
        if (v->Type() == a_complex)
            value = ((Tamgucomplex*)v)->value;
        else
            value = v->Float();
		return this;
	}

	Tamgu* Putvalue(Tamgu* v, short idthread) {
        if (v->Type() == a_complex)
            value = ((Tamgucomplex*)v)->value;
        else
            value = v->Float();
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
        return a_complex;
	}

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "complex";
	}

	bool isAtom() {
		return true;
	}

    bool checkAtomType(short ty) {
        return (ty == a_complex);
    }

	short Typenumber() {
		return a_complex;
	}

	bool isFloat() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new Tamgucomplex(value);
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
		return new Tamgucomplex(value + 1.0);
	}

	Tamgu* Pred() {
		return new Tamgucomplex(value - 1.0);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgucomplex(0, 0);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
        if (a->Type() == a_complex)
            return new Tamgucomplex(((Tamgucomplex*)a)->value);
        
		return new Tamgucomplex(a->Float());
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, complexMethod func, unsigned long arity, string infos, short typereturn);
	
	static bool InitialisationModule(TamguGlobal* g, string version);
	
	void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[a_complex])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[a_complex].find(n) !=  globalTamgu->infomethods[a_complex].end())
              return globalTamgu->infomethods[a_complex][n];
             return "Unknown method";
	}



	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.

	Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgucomplex(value + 1.0);
	}

	Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		return new Tamgucomplex(value - 1.0);
	}

    Tamgu* MethodUnit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamgucomplex(value.imag(), value.real());
	}

    Tamgu* MethodReal(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return globalTamgu->Providefloat(value.real());
    }

    Tamgu* MethodImaginary(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return globalTamgu->Providefloat(value.imag());
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

    wstring UString() {
        wstring r = L"0c";
        r += wconvertfromnumber(value.real());
        r += L":";
        if (value.imag() == -1)
            r += L"-";
        else
            if (value.imag() != 1)
                r += wconvertfromnumber(value.imag());
        r += L"i";
        return r;
	}

    string String() {
        string r = "0c";
        r += convertfromnumber(value.real());
        r += ":";
        if (value.imag() == -1)
            r += "-";
        else
            if (value.imag() != 1)
                r += convertfromnumber(value.imag());
        r += "i";
        return r;
    }

	string JSonString() {
        return String();
	}

    void Setstring(string& r, short idthread) {
        r = String();
    }

    void Setstring(wstring& r, short idthread) {
        r = UString();
    }

    wstring Getustring(short idthread) {
        return UString();
    }

    string Getstring(short idthread) {
        return String();
    }

	long Integer() {
		return (long)value.real();
	}

	double Float() {
		return value.real();
	}

	float Decimal() {
		return value.real();
	}

    Tamgu* Complex() {
        return this;
    }
    
	BLONG Long() {
		return (BLONG)value.real();
	}

    long asInteger() {
        return value.real();
    }

    float asDecimal() {
        return value.real();
    }

    double asFloat() {
        return value.real();
    }

    BLONG asLong() {
        return value.real();
    }

    short asShort() {
        return value.real();
    }

	long Getinteger(short idthread) {
		return (long)value.real();
	}

	double Getfloat(short idthread) {
		return value.real();
	}

	float Getdecimal(short idthread) {
		return value.real();
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value.real();
	}

	bool Boolean() {
        return (value != 0.0);
	}

	bool Protectedboolean() {
        Locking _lock(*globalTamgu->booleanlocks[a_complex]);
        return (value != 0.0);
    }

	//Basic operations
	virtual long Size() {
		return sizeof(value);
	}

	virtual Tamgu* plusplus() {
        value.real(value.real()+1);
		return this;
	}

	virtual Tamgu* minusminus() {
        value.real(value.real()-1);
		return this;
	}


	Tamgu* plus(Tamgu* a, bool itself) {
        if (a->Type() == a_complex) {
            if (itself) {
                value += ((Tamgucomplex*)a)->value;
                return this;
            }
            return new Tamgucomplex(value + ((Tamgucomplex*)a)->value);
        }

		if (itself) {
			value += a->Float();
			return this;
		}
		return new Tamgucomplex(value + a->Float());
	}

	virtual Tamgu* minus(Tamgu* a, bool itself) {
        if (a->Type() == a_complex) {
            if (itself) {
                value -= ((Tamgucomplex*)a)->value;
                return this;
            }
            return new Tamgucomplex(value - ((Tamgucomplex*)a)->value);
        }

        if (itself) {
			value -= a->Float();
			return this;
		}
		return new Tamgucomplex(value - a->Float());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
        if (a->Type() == a_complex) {
            if (itself) {
                value *= ((Tamgucomplex*)a)->value;
                return this;
            }
            return new Tamgucomplex(value * ((Tamgucomplex*)a)->value);
        }

		if (itself) {
            value *= ((Tamgucomplex*)a)->value;
			return this;
		}
		return new Tamgucomplex(value * a->Float());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
        if (a->Type() == a_complex) {
            Tamgucomplex* c = (Tamgucomplex*)a;
            if (c->value == 0.0)
                return globalTamgu->Returnerror(e_error_divided_by);
            if (itself) {
                value /= c->value;
                return this;
            }
            return new Tamgucomplex(value / c->value);
        }

        double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);
		if (itself) {
			value /= v;
			return this;
		}

        std::complex<double> c;
		c = value / v;
		return new Tamgucomplex(c);
	}

	Tamgu* less(Tamgu* a) {
        return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
        if (a->Type() != a_complex)
            return aFALSE;
        return booleantamgu[value == ((Tamgucomplex*)a)->value];
	}

	Tamgu* different(Tamgu* a) {
        if (a->Type() != a_complex)
            return aTRUE;
		if (value != ((Tamgucomplex*)a)->value)
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









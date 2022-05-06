/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufraction.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgufraction_h
#define tamgufraction_h
#include "tamguint.h"
#include "tamgulong.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgufraction;
//This typedef defines a type "fractionMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgufraction::*fractionMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgufraction : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<fractionMethod> methods;
	
	

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	BLONG numerator;
	BLONG denominator;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgufraction(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
     investigate |= is_number;
		//Do not forget your variable initialisation
		numerator = 0;
		denominator = 1;
	}

	Tamgufraction(BLONG n, BLONG d) {
     investigate |= is_number;
		//Do not forget your variable initialisation
		numerator = n;
		denominator = d;
	}

	Tamgufraction(double n) {
     investigate |= is_number;
		Evaluatefraction(n);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Putvalue(Tamgu* v, short idthread) {
		if (v->Type() == idtype) {
			Tamgufraction* val = (Tamgufraction*)v;
			numerator = val->numerator;
			denominator = val->denominator;
		}
		else
			Evaluatefraction(v->Float());
		return aTRUE;
	}

	Tamgu* Put(Tamgu* idx, Tamgu* v, short idthread) {
		if (v->Type() == idtype) {
			Tamgufraction* val = (Tamgufraction*)v;
			numerator = val->numerator;
			denominator = val->denominator;
		}
		else
			Evaluatefraction(v->Float());
		return aTRUE;
	}

	Tamgu* Invert(bool autoself) {
		if (numerator == 0)
			return globalTamgu->Returnerror("Cannot invert, numerator is 0");
		
		if (autoself)  {
			ND(denominator, numerator);
			return this;
		}

		return new Tamgufraction(denominator, numerator);
	}

	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread) {
		return this;
	}

	short Type() {
        return a_fraction;
	}

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "fraction";
	}

	short Typenumber() {
		return a_float;
	}

    bool checkAtomType(short ty) {
        return (ty == a_fraction);
    }

	bool isFloat() {
		return true;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Tamgufraction(numerator, denominator);
		return this;
	}


	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in aexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgufraction(0, 1);
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, fractionMethod func, unsigned long arity, string infos);
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
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		if (callfunc->Size() == 1)
			Evaluatefraction(callfunc->Evaluate(0, aNULL, idthread)->Float());
		else {
			numerator = callfunc->Evaluate(0, aNULL, idthread)->Long();
			denominator = callfunc->Evaluate(1, aNULL, idthread)->Long();
			if (denominator == 0)
				return globalTamgu->Returnerror("Denominator cannot be 0", idthread);
			Simplify();
		}
		return this;
	}

	Tamgu* MethodSimplify(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		Simplify();
		return this;
	}

	//---------------------------------------------------------------------------------------------------------------------


	BLONG N() {
		return numerator;
	}

	BLONG D() {
		return denominator;
	}

	void ND(BLONG n, BLONG d) {
		numerator = n;
		denominator = d;
	}

	//---------------------------------------------------------------------------------------------------------------------
	BLONG euclidian(BLONG a, BLONG b) {
		if (b == 0)
			return 0;

		if (a < b)
			return 1;

		BLONG remain = a%b;
		if (remain == 0)
			return b;
		return euclidian(b, remain);
	}

	BLONG pureeuclidian(BLONG a, BLONG b) {
		if (b == 0)
			return 0;
		if (a<b)
			return 1;
		BLONG remain = a%b;
		if (remain == 0)
			return b;
		return pureeuclidian(b, remain);
	}

	BLONG computeEuclidian(BLONG a, BLONG b) {
		if (a>b)
			return pureeuclidian(a, b);
		return pureeuclidian(b, a);
	}

	bool Simplify() {
		if (denominator == 0)
			return false;

		if (denominator == 1 || numerator == 0)
			return true;

		if (denominator == numerator) {
			denominator = 1;
			numerator = 1;
			return true;
		}


		BLONG res;
		bool nneg = false;
		bool dneg = false;
		if (numerator < 0) {
			numerator *= -1;
			if (denominator < 0)
				denominator *= -1;
			else
				nneg = true;
		}
		else {
			if (denominator < 0) {
				denominator *= -1;
				dneg = true;
			}
		}

		if (denominator>numerator)
			res = euclidian(denominator, numerator);
		else
			res = euclidian(numerator, denominator);

		if (res == 1) {
			if (dneg)
				denominator *= -1;
			if (nneg)
				numerator *= -1;
			return true;
		}

		numerator /= res;
		denominator /= res;
		if (dneg)
			denominator *= -1;
		if (nneg)
			numerator *= -1;
		return true;
	}

	bool Evaluatefraction(double value) {
		BLONG denom = (long)value;
		if (denom == value) {
			ND(denom, 1);
			return true;
		}

		double mantissa = value;
		//nb contains the number of digits of value...
		BLONG nb = log10(fabs(value));
		BLONG num = 1;
		//We limit the calculus to 29 digits after the point.
		//The mantissa of a double is encoded on 52 bits, which is roughly 15 digits...
		while (nb < 15 && mantissa != (long)mantissa)  {
			mantissa *= 10;
			nb++;
			num *= 10;
		}
		denom = mantissa;
		//In the case of a full mantissa, we use another version of the transformation
		//namely we try to approximate with the smallest fraction possible...
		char ddenom = 0;
		char dnum = 0;
		if (nb == 15) {
			BLONG p1 = computeEuclidian(denom, num);
			BLONG p2 = computeEuclidian(denom + 1, num);
			if (p1 < p2) {
				ddenom = 1;
				dnum = 0;
				p1 = p2;
			}
			p2 = computeEuclidian(denom, num + 1);
			if (p1 < p2) {
				ddenom = 0;
				dnum = 1;
				p1 = p2;
			}
			p2 = computeEuclidian(denom - 1, num);
			if (p1 < p2) {
				ddenom = -1;
				dnum = 0;
				p1 = p2;
			}
			p2 = computeEuclidian(denom, num - 1);
			if (p1 < p2) {
				ddenom = 0;
				dnum = -1;
			}
			denom += ddenom;
			num += dnum;
		}

		ND(denom, num);
		return Simplify();
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
	}

	

    string String() {
        stringstream res;
        res << numerator << "/" << denominator;
        return res.str();
    }

    wstring UString() {
        std::wstringstream res;
        res << numerator << L"/" << denominator;
        return res.str();
    }

    void Setstring(string& v, short idthread) {
        v = convertfromnumber(numerator);
        v += "/";
        v += convertfromnumber(denominator);
    }

    void Setstring(wstring& v, short idthread) {
        v = wconvertfromnumber(numerator);
        v += L"/";
        v += wconvertfromnumber(denominator);
    }

	long Integer() {
		double v = numerator / denominator;
		return (long)v;
	}

	double Float() {
		double v = numerator / denominator;
		return v;
	}
	BLONG Long() {
		double v = numerator / denominator;
		return (BLONG)v;
	}

	bool Boolean() {
        return numerator;
	}


	//Basic operations
	long Size() {
		return Integer();
	}

	Tamgufraction* Fraction() {
		return this;
	}

	Tamgu* plus(Tamgu* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Tamgufraction* b = bb->Fraction();
		BLONG nub = b->N();
		BLONG deb = b->D();
		if (b != bb)
			b->Release();

		if (deb != dea) {
			nua *= deb;
			nub *= dea;
			dea *= deb;
		}

		nua += nub;
		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Tamgufraction(nua, dea);
	}

	Tamgu* minus(Tamgu* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Tamgufraction* b = bb->Fraction();
		BLONG nub = b->N();
		BLONG deb = b->D();
		if (b != bb)
			b->Release();

		if (deb != dea) {
			nua *= deb;
			nub *= dea;
			dea *= deb;
		}

		nua -= nub;
		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Tamgufraction(nua, dea);
	}

	Tamgu* multiply(Tamgu* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Tamgufraction* b = bb->Fraction();
		if (b != bb)
			b->Release();

		nua *= b->N();
		dea *= b->D();

		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Tamgufraction(nua, dea);
	}

	Tamgu* divide(Tamgu* bb, bool autoself) {
		BLONG dea = denominator;
		BLONG nua = numerator;

		Tamgufraction* b = bb->Fraction();
		BLONG deb = b->D();
		if (b != bb)
			b->Release();

		nua *= deb;
		dea *= b->N();

		if (dea == 0)
			return globalTamgu->Returnerror("MATH(100): Error wrong fraction: denominator is 0");

		if (autoself) {
			numerator = nua;
			denominator = dea;
			Simplify();
			return this;
		}

		return new Tamgufraction(nua, dea);
	}

	Tamgu* mod(Tamgu* b, bool autoself) {
		BLONG ib = b->Long();
		if (ib == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");

		BLONG i = Long() % ib;
		return globalTamgu->Providelong(i);
	}

	Tamgu* shiftleft(Tamgu* bb, bool autoself) {
		BLONG i = Long() << bb->Long();
		return globalTamgu->Providelong(i);
	}

	Tamgu* shiftright(Tamgu* bb, bool autoself) {
		BLONG i = Long() >> bb->Long();
		return globalTamgu->Providelong(i);
	}

	Tamgu* power(Tamgu* b, bool autoself) {
		double i = pow(Float(), b->Float());
		return globalTamgu->ProvideConstint(i);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float()<a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float()>a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float() == a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif
		if (Float() != a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float() <= a->Float())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (Float() >= a->Float())
			return aTRUE;
		return aFALSE;
	}

};

#endif





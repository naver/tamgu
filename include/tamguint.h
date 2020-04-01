/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguint.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamguint_h
#define tamguint_h

#include "tamgufloat.h"

    //We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
    //of how to declare a new method.
class Tamguint;
    //This typedef defines a type "intMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguint::*intMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

class Tamguint : public TamguReference {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<intMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    long value;
    
        //---------------------------------------------------------------------------------------------------------------------
    Tamguint(long v, TamguGlobal* g, Tamgu* parent = NULL) : TamguReference(g, parent) {
     investigate |= is_number;
            //Do not forget your variable initialisation
        value = v;
    }
    
    Tamguint(long v) {
     investigate |= is_number;
            //Do not forget your variable initialisation
        value = v;
    }
    
        //----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->Integer();
    }
    
    Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
        value = v->Integer();
        return this;
    }
    
    Tamgu* Putvalue(Tamgu* v, short idthread) {
        value = v->Integer();
        return this;
    }
    
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        
        return this;
    }
    
    bool Checkprecision(Tamgu* r) {
        if (idtype < r->Typenumber())
            return false;
        
        return true;
    }
    
    short Type() {
        return Tamguint::idtype;
    }
    
    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "int";
    }
    
    bool isInteger() {
        return true;
    }
    
    bool isAtom() {
        return true;
    }
    
    bool checkAtomType(short ty) {
        return (ty == a_int);
    }

    short Typenumber() {
        return a_int;
    }
    
    
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference)
            return globalTamgu->Provideint(value);
        return this;
    }
    
    Tamgu* Atomref() {
        TamguReference* r;
        if (!protect || reference)
            r = globalTamgu->Provideint(value);
        else
            r = this;
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Bytes() {
        u_tamguinteger x(value);
        unsigned char buff[u_tamguintsz + 1];
        memcpy(buff, x.t, u_tamguintsz);
        buff[u_tamguintsz] = 0;
        return (char*)buff;
    }
    
    void storevalue(short u) {
        value = u;
    }
    
    void storevalue(long u) {
        value = u;
    }
    
    void storevalue(BLONG u) {
        value = (long)u;
    }
    
    void storevalue(double u) {
        value = (long)u;
    }
    
    void storevalue(unsigned char u) {
        value = (long)u;
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
    
    Tamgu* Newpureinstance(short idthread) {
        return new Tamguint(0);
    }
    
    virtual Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return globalTamgu->Provideint(0);
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        return globalTamgu->Provideint(a->Integer());
    }
    
    
    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }
    
    static void AddMethod(TamguGlobal* g, string name, intMethod func, unsigned long arity, string infos);
    
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
        return globalTamgu->Provideint(value + 1);
    }
    
    Tamgu* Pred() {
        return globalTamgu->Provideint(value - 1);
    }
    
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodBit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Invert(true);
    }
    
    Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return globalTamgu->Provideint(value + 1);
    }
    
    Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return globalTamgu->Provideint(value - 1);
    }
    
    Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodPrimefactors(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* kvect = Selectaivector(contextualpattern);
        long n = value;
        long dec = 2;
        long ni = n;
        while (n > 1 && (dec*dec) <= ni) {
            if (n%dec == 0) {
                kvect->storevalue(dec);
                n /= dec;
            }
            else
                dec++;
        }
        
        if (n > 1)
            kvect->storevalue(n);
        return kvect;
    }
    
    Tamgu* MethodPrime(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long n = value;
        if (!n)
            return aFALSE;
        if (n == 2 || n == 3 || n == 5 || n == 7 || n == 11)
            return aTRUE;
        
        if (!n || (n % 2) == 0 || (n % 3) == 0 || (n % 5) == 0 || (n % 7) == 0 || (n % 11) == 0)
            return aFALSE;
        
        long dec = 13;
        long ni = n;
        
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
    
    void Setstring(string& v, short idthread) {
        convertnumber(value,v);
    }

    void Setstring(wstring& v, short idthread) {
        convertnumber(value,v);
    }

    long Integer() {
        return value;
    }
    
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
    
    wstring UString() {
        
        return wconvertfromnumber(value);
        
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
        Locking _lock(*globalTamgu->booleanlocks[a_int]);
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
            value &= a->Integer();
            return this;
        }
        return globalTamgu->Provideint(value & a->Integer());
    }
    
    Tamgu* orset(Tamgu* a, bool itself) {
        if (itself) {
            value |= a->Integer();
            return this;
        }
        return globalTamgu->Provideint(value | a->Integer());
    }
    
    Tamgu* xorset(Tamgu* a, bool itself) {
        if (itself) {
            value ^= a->Integer();
            return this;
        }
        return globalTamgu->Provideint(value ^ a->Integer());
    }
    
    
    Exporting virtual Tamgu* plus(Tamgu* a, bool itself);
    
    virtual Tamgu* minus(Tamgu* a, bool itself) {
        if (itself) {
            value -= a->Integer();
            return this;
        }
        return globalTamgu->Provideint(value - a->Integer());
    }
    
    Exporting Tamgu* multiply(Tamgu* a, bool itself);
    
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
            value = (long)v;
            return this;
        }
        v = pow(v, a->Float());
        return globalTamgu->Providefloat(v);
    }
    
    Exporting Tamgu* shiftleft(Tamgu* a, bool itself);
    
    Tamgu* shiftright(Tamgu* a, bool itself) {
        if (itself) {
            value >>= a->Integer();
            return this;
        }
        return globalTamgu->Provideint(value >> a->Integer());
    }
    
    Tamgu* mod(Tamgu* a, bool itself) {
        long v = a->Integer();
        if (v == 0)
            return globalTamgu->Returnerror("Error: Divided by 0");
        
        if (itself) {
            value %= a->Integer();
            return this;
        }
        
        v = value % v;
        return globalTamgu->Provideint(v);
    }
    
    //Comparison
    Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
        if (value < a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
        if (value > a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
        if (value == a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aTRUE;
#endif
        if (value != a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
        
        if (value <= a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
        
        if (value >= a->Integer())
            return aTRUE;
        return aFALSE;
    }
};

    //---------------------------------------------------------------------------------------------------------------------
class Tamguintbuff : public Tamguint {
public:
    long idx;
    bool used;
    
    Tamguintbuff(long i) : Tamguint(0) {
            //Do not forget your variable initialisation
        idx = i;
        used = false;
    }
    
    bool Candelete() {
        return false;
    }
    
    void Resetreference(short r) {
        if ((reference-=r) <= 0) {
            reference = 0;
            if (!protect) {
                protect = true;
                
                used = false;
                value = 0;
                if (!globalTamgu->globalLOCK)
                    globalTamgu->iempties.push_back(idx);
            }
        }
    }
    
};
    //---------------------------------------------------------------------------------------------------------------------

class Tamguatomicint;
    //This typedef defines a type "intMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguatomicint::*atomicintMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

class Tamguatomicint : public TamguReference {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<atomicintMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    std::atomic<long> value;
    
        //---------------------------------------------------------------------------------------------------------------------
    Tamguatomicint(long v, TamguGlobal* g) : TamguReference(g) {
     investigate |= is_number;
        value = v;
    }


    Tamguatomicint(long v) {
     investigate |= is_number;
        value = v;
    }
    
        //----------------------------------------------------------------------------------------------------------------------
    void Putatomicvalue(Tamgu* v) {
        value = v->Integer();
    }
    
    Tamgu* Put(Tamgu* index, Tamgu* v, short idthread) {
        value = v->Integer();
        return this;
    }
    
    Tamgu* Putvalue(Tamgu* v, short idthread) {
        value = v->Integer();
        return this;
    }
    
    
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread) {
        
        return this;
    }
    
    bool Checkprecision(Tamgu* r) {
        if (idtype < r->Typenumber())
            return false;
        
        return true;
    }
    
    short Type() {
        return Tamguatomicint::idtype;
    }
    
	void Setidtype(TamguGlobal* global) {
		Tamguatomicint::InitialisationModule(global, "");
	}

    string Typename() {
        return "a_int";
    }
    
    bool isInteger() {
        return true;
    }
    
    bool isAtom() {
        return true;
    }
    
    bool checkAtomType(short ty) {
        return (ty == a_int);
    }

    short Typenumber() {
        return a_int;
    }
    
    
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference)
            return new Tamguatomicint(value);
        return this;
    }
    
    Tamgu* Atomref() {
        TamguReference* r;
        if (!protect || reference)
            r = new Tamguatomicint(value);
        else
            r = this;
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Bytes() {
        u_tamguinteger x(value);
        unsigned char buff[u_tamguintsz + 1];
        memcpy(buff, x.t, u_tamguintsz);
        buff[u_tamguintsz] = 0;
        return (char*)buff;
    }
    
    void storevalue(short u) {
        value = u;
    }
    
    void storevalue(long u) {
        value = u;
    }
    
    void storevalue(BLONG u) {
        value = (long)u;
    }
    
    void storevalue(double u) {
        value = (long)u;
    }
    
    void storevalue(unsigned char u) {
        value = (long)u;
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
    
    virtual Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamguatomicint((long)0);
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        return new Tamguatomicint(a->Integer());
    }
    
    
    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }
    
    static void AddMethod(TamguGlobal* g, string name, atomicintMethod func, unsigned long arity, string infos);
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
        return new Tamguatomicint((long)(value + 1));
    }
    
    Tamgu* Pred() {
        return new Tamguatomicint((long)(value - 1));
    }
    
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodBit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Invert(true);
    }
    
    Tamgu* MethodSucc(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicint((long)(value + 1));
    }
    
    Tamgu* MethodPred(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return new Tamguatomicint((long)(value - 1));
    }
    
    Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodPrimefactors(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* kvect = Selectaivector(contextualpattern);
        long n = value;
        long dec = 2;
        long ni = n;
        while (n > 1 && (dec*dec) <= ni) {
            if (n%dec == 0) {
                kvect->storevalue(dec);
                n /= dec;
            }
            else
                dec++;
        }
        
        if (n > 1)
            kvect->storevalue(n);
        return kvect;
    }
    
    Tamgu* MethodPrime(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long n = value;
        if (!n)
            return aFALSE;
        if (n == 2 || n == 3 || n == 5 || n == 7 || n == 11)
            return aTRUE;
        
        if (!n || (n % 2) == 0 || (n % 3) == 0 || (n % 5) == 0 || (n % 7) == 0 || (n % 11) == 0)
            return aFALSE;
        
        long dec = 13;
        long ni = n;
        
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
    
    void Setstring(string& v, short idthread) {
        convertnumber(value,v);
    }

    void Setstring(wstring& v, short idthread) {
        convertnumber(value,v);
    }

    long Integer() {
        return value;
    }
    
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
    
    wstring UString() {
        
        return wconvertfromnumber(value);
        
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
            value &= a->Integer();
            return this;
        }
        long v = value & a->Integer();
        return new Tamguatomicint(v);
    }
    
    Tamgu* orset(Tamgu* a, bool itself) {
        if (itself) {
            value |= a->Integer();
            return this;
        }
        long v = value | a->Integer();
        return new Tamguatomicint(v);
    }
    
    Tamgu* xorset(Tamgu* a, bool itself) {
        if (itself) {
            value ^= a->Integer();
            return this;
        }
        long v = value ^ a->Integer();
        return new Tamguatomicint(v);
    }
    
    
    Exporting virtual Tamgu* plus(Tamgu* a, bool itself);
    
    virtual Tamgu* minus(Tamgu* a, bool itself) {
        if (itself) {
            value -= a->Integer();
            return this;
        }
        long v = value - a->Integer();
        return new Tamguatomicint(v);
    }
    
    Exporting Tamgu* multiply(Tamgu* a, bool itself);
    
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
            value = (long)v;
            return this;
        }
        v = pow(v, a->Float());
        return globalTamgu->Providefloat(v);
    }
    
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        if (itself) {
            value = value << a->Integer();
            return this;
        }
        long v = value << a->Integer();
        return new Tamguatomicint(v);
    }
    
    Tamgu* shiftright(Tamgu* a, bool itself) {
        if (itself) {
            value = value >> a->Integer();
            return this;
        }
        long v = value >> a->Integer();
        return new Tamguatomicint(v);
    }
    
    Tamgu* mod(Tamgu* a, bool itself) {
        long v = a->Integer();
        if (v == 0)
            return globalTamgu->Returnerror("Error: Divided by 0");
        
        if (itself) {
            value = value % a->Integer();
            return this;
        }
        
        v = value % v;
        return new Tamguatomicint(v);
    }
    
    Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value < a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value == a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->Integer())
            return aTRUE;
        return aFALSE;
    }
    
    Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value >= a->Integer())
            return aTRUE;
        return aFALSE;
    }
};

class TamguTaskellCounter : public Tamguint {
public:
    
    TamguTaskellCounter(long v = 0) : Tamguint(v) {
     investigate |= is_number;
        investigate = is_container;
    }
    
    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return this;
    }
    
    Tamgu* Push(Tamgu* v) {
        value++;
        return this;
    }
    
    Tamgu* Pushinmap(Tamgu* v, short idthread) {
        value++;
        return this;
    }
    
    short Type() {
        return a_counter;
    }
    
    short Typevariable() {
        return a_counter;
    }
    
    void Insert(long idx, Tamgu* ke) {
        value++;
    }
    
    long Size() {
        return 0;
    }
    
    bool isAffectation() {
        return true;
    }
    
    bool checkAffectation() {
        return true;
    }
    
};

class TamguLoopInteger : public Tamguint {
public:
    
    vector<long> interval;
    long position;
    Tamgu* function;
    
    TamguLoopInteger(TamguGlobal* g) : Tamguint(0) {
        
        position = 0;
        function = NULL;
    }

    bool isLoop() {
        return true;
    }
    

    short Type() {
        return a_iloop;
    }
    
    
    void Addfunction(Tamgu* f) {
        function = f;
    }
    
    void Callfunction();
    
    unsigned long CallBackArity() {
        return P_TWO;
    }
    
    Tamgu* Put(Tamgu*, Tamgu*, short);
    Tamgu* Putvalue(Tamgu* v, short idthread) {
        return Put(aNULL, v, idthread);
    }
    
    Tamgu* Vector(short idthread);
    
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
        TamguLoopInteger* a = new TamguLoopInteger(globalTamgu);
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
        
        return Tamguint::plus(b, autoself);
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
        
        return Tamguint::minus(b, autoself);
    }
};


#endif









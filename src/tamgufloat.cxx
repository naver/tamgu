/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufloat.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufloat.h"
#include "tamgustring.h"
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#include "tamgufvector.h"
#include "instructions.h"
#include "constobjects.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<floatMethod>  Tamgufloat::methods;
Exporting hmap<string, string> Tamgufloat::infomethods;
Exporting bin_hash<unsigned long> Tamgufloat::exported;

Exporting short Tamgufloat::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgufloat::AddMethod(TamguGlobal* global, string name, floatMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

Exporting basebin_hash<atomicfloatMethod>  Tamguatomicfloat::methods;
Exporting hmap<string, string> Tamguatomicfloat::infomethods;
Exporting bin_hash<unsigned long> Tamguatomicfloat::exported;

Exporting short Tamguatomicfloat::idtype = 0;


    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguatomicfloat::AddMethod(TamguGlobal* global, string name, atomicfloatMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}


static const double M_GOLDEN = 1.61803398874989484820458683436563811772030917980576286213544862270526046281890244970720720418939113748475;



    void Tamgufloat::Setidtype(TamguGlobal* global) {
        Tamgufloat::idtype = global->Getid("float");
    }

   bool Tamgufloat::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgufloat::idtype = global->Getid("float");

    Tamgufloat::AddMethod(global, "chr", &Tamgufloat::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
    Tamgufloat::AddMethod(global, "invert", &Tamgufloat::MethodInvert, P_NONE, "invert(): value inversion as a fraction");
    Tamgufloat::AddMethod(global, "succ", &Tamgufloat::MethodSucc, P_NONE, "succ(): Return a successor of the current value");
    Tamgufloat::AddMethod(global, "pred", &Tamgufloat::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");

    Tamgufloat::AddMethod(global, "format", &Tamgufloat::MethodFormat, P_ONE, "format(string pattern): Return a string matching the C pattern.");
    Tamgufloat::AddMethod(global, "radian", &Tamgufloat::MethodRadian, P_NONE, "radian(): return the radian value out of a degree value");
    Tamgufloat::AddMethod(global, "degree", &Tamgufloat::MethodDegree, P_NONE, "degree(): return the degree value out of a radian value");

    Tamgufloat::AddMethod(global, "bits", &Tamgufloat::MethodBits, P_NONE|P_ONE, "bits(): return the bit representation or initialize a float number with a bit representation (bits(long v))");
    Tamgufloat::AddMethod(global, "mantissa", &Tamgufloat::MethodMantissa, P_NONE, "mantissa(): return the mantissa value as a float");
    Tamgufloat::AddMethod(global, "exponent", &Tamgufloat::MethodExponent, P_NONE, "exponent(): return the exponent value as an int");

    Tamgufloat::AddMethod(global, "abs", &Tamgufloat::Methodabs, P_NONE, "abs(): call fabs on the value");
    Tamgufloat::AddMethod(global, "acos", &Tamgufloat::Methodacos, P_NONE, "acos(): call acos on the value");
    Tamgufloat::AddMethod(global, "acosh", &Tamgufloat::Methodacosh, P_NONE, "acosh(): call acosh on the value");
    Tamgufloat::AddMethod(global, "asin", &Tamgufloat::Methodasin, P_NONE, "asin(): call asin on the value");
    Tamgufloat::AddMethod(global, "asinh", &Tamgufloat::Methodasinh, P_NONE, "asinh(): call asinh on the value");
    Tamgufloat::AddMethod(global, "atan", &Tamgufloat::Methodatan, P_NONE, "atan(): call atan on the value");
    Tamgufloat::AddMethod(global, "atanh", &Tamgufloat::Methodatanh, P_NONE, "atanh(): call atanh on the value");
    Tamgufloat::AddMethod(global, "cbrt", &Tamgufloat::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
    Tamgufloat::AddMethod(global, "cos", &Tamgufloat::Methodcos, P_NONE, "cos(): call cos on the value");
    Tamgufloat::AddMethod(global, "cosh", &Tamgufloat::Methodcosh, P_NONE, "cosh(): call cosh on the value");
    Tamgufloat::AddMethod(global, "erf", &Tamgufloat::Methoderf, P_NONE, "erf(): call erf on the value");
    Tamgufloat::AddMethod(global, "erfc", &Tamgufloat::Methoderfc, P_NONE, "erfc(): call erfc on the value");
    Tamgufloat::AddMethod(global, "exp", &Tamgufloat::Methodexp, P_NONE, "exp(): call exp on the value");
    Tamgufloat::AddMethod(global, "exp2", &Tamgufloat::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
    Tamgufloat::AddMethod(global, "expm1", &Tamgufloat::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
    Tamgufloat::AddMethod(global, "floor", &Tamgufloat::Methodfloor, P_NONE, "floor(): call floor on the value");
    Tamgufloat::AddMethod(global, "lgamma", &Tamgufloat::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
    Tamgufloat::AddMethod(global, "ln", &Tamgufloat::Methodln, P_NONE, "ln(): call log on the value");
    Tamgufloat::AddMethod(global, "log", &Tamgufloat::Methodlog, P_NONE, "log(): call log10 on the value");
    Tamgufloat::AddMethod(global, "log1p", &Tamgufloat::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
    Tamgufloat::AddMethod(global, "log2", &Tamgufloat::Methodlog2, P_NONE, "log2(): call log2 on the value");
    Tamgufloat::AddMethod(global, "logb", &Tamgufloat::Methodlogb, P_NONE, "logb(): call logb on the value");
    Tamgufloat::AddMethod(global, "nearbyint", &Tamgufloat::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
    Tamgufloat::AddMethod(global, "rint", &Tamgufloat::Methodrint, P_NONE, "rint(): call rint on the value");
    Tamgufloat::AddMethod(global, "round", &Tamgufloat::Methodround, P_NONE, "round(): call round on the value");
    Tamgufloat::AddMethod(global, "sin", &Tamgufloat::Methodsin, P_NONE, "sin(): call sin on the value");
    Tamgufloat::AddMethod(global, "sinh", &Tamgufloat::Methodsinh, P_NONE, "sinh(): call sinh on the value");
    Tamgufloat::AddMethod(global, "sqrt", &Tamgufloat::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
    Tamgufloat::AddMethod(global, "tan", &Tamgufloat::Methodtan, P_NONE, "tan(): call tan on the value");
    Tamgufloat::AddMethod(global, "tanh", &Tamgufloat::Methodtanh, P_NONE, "tanh(): call tanh on the value");
    Tamgufloat::AddMethod(global, "tgamma", &Tamgufloat::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
    Tamgufloat::AddMethod(global, "trunc", &Tamgufloat::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
    Tamgufloat::AddMethod(global, "even", &Tamgufloat::Methodeven, P_NONE, "even(): return true is the value is even");
    Tamgufloat::AddMethod(global, "odd", &Tamgufloat::Methododd, P_NONE, "odd(): return true is the value is odd");

    short idreal = global->Getid("real");

    global->newInstance[Tamgufloat::idtype] = new Tamgufloat(0, global);
    global->newInstance[idreal] = new Tamgufloat(0, global);
    global->newInstance[a_floatthrough] = global->newInstance[Tamgufloat::idtype];

    global->RecordMethods(Tamgufloat::idtype, Tamgufloat::exported);
    global->RecordMethods(idreal, Tamgufloat::exported);
    global->RecordMethods(a_floatthrough, Tamgufloat::exported);

    global->RecordMethods(a_floop, Tamgufloat::exported);

    Tamgu* a = new TamguSystemVariable(global, new TamguConstFloat(M_PI), global->Createid("_pi"), a_float);

    a = new TamguSystemVariable(global, new TamguConstFloat(M_PI), global->Createid("π"), a_float);
    
    a = new TamguSystemVariable(global, new TamguConstFloat(2*M_PI), global->Createid("_tau"), a_float);
    a = new TamguSystemVariable(global, new TamguConstFloat(2*M_PI), global->Createid("τ"), a_float);
    
    a = new TamguSystemVariable(global, new TamguConstFloat(M_E), global->Createid("_e"), a_float);
    a = new TamguSystemVariable(global, new TamguConstFloat(M_E), global->Createid("ℯ"), a_float);

    a = new TamguSystemVariable(global, new TamguConstFloat(M_GOLDEN), global->Createid("_phi"), a_float);
    a = new TamguSystemVariable(global, new TamguConstFloat(M_GOLDEN), global->Createid("φ"), a_float);
    
    Tamguatomicfloat::InitialisationModule(global, version);
    
    return true;
}


bool Tamguatomicfloat::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    Tamguatomicfloat::idtype = global->Getid("a_float");
    
    Tamguatomicfloat::AddMethod(global, "chr", &Tamguatomicfloat::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
    Tamguatomicfloat::AddMethod(global, "invert", &Tamguatomicfloat::MethodInvert, P_NONE, "invert(): value inversion as a fraction");
    Tamguatomicfloat::AddMethod(global, "succ", &Tamguatomicfloat::MethodSucc, P_NONE, "succ(): Return a successor of the current value");
    Tamguatomicfloat::AddMethod(global, "pred", &Tamguatomicfloat::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");
    
    Tamguatomicfloat::AddMethod(global, "format", &Tamguatomicfloat::MethodFormat, P_ONE, "format(string pattern): Return a string matching the C pattern.");
    Tamguatomicfloat::AddMethod(global, "radian", &Tamguatomicfloat::MethodRadian, P_NONE, "radian(): return the radian value out of a degree value");
    Tamguatomicfloat::AddMethod(global, "degree", &Tamguatomicfloat::MethodDegree, P_NONE, "degree(): return the degree value out of a radian value");
    
    Tamguatomicfloat::AddMethod(global, "bits", &Tamguatomicfloat::MethodBits, P_NONE|P_ONE, "bits(): return the bit representation or initialize a float number with a bit representation (bits(long v))");
    Tamguatomicfloat::AddMethod(global, "mantissa", &Tamguatomicfloat::MethodMantissa, P_NONE, "mantissa(): return the mantissa value as a float");
    Tamguatomicfloat::AddMethod(global, "exponent", &Tamguatomicfloat::MethodExponent, P_NONE, "exponent(): return the exponent value as an int");
    
    Tamguatomicfloat::AddMethod(global, "abs", &Tamguatomicfloat::Methodabs, P_NONE, "abs(): call fabs on the value");
    Tamguatomicfloat::AddMethod(global, "acos", &Tamguatomicfloat::Methodacos, P_NONE, "acos(): call acos on the value");
    Tamguatomicfloat::AddMethod(global, "acosh", &Tamguatomicfloat::Methodacosh, P_NONE, "acosh(): call acosh on the value");
    Tamguatomicfloat::AddMethod(global, "asin", &Tamguatomicfloat::Methodasin, P_NONE, "asin(): call asin on the value");
    Tamguatomicfloat::AddMethod(global, "asinh", &Tamguatomicfloat::Methodasinh, P_NONE, "asinh(): call asinh on the value");
    Tamguatomicfloat::AddMethod(global, "atan", &Tamguatomicfloat::Methodatan, P_NONE, "atan(): call atan on the value");
    Tamguatomicfloat::AddMethod(global, "atanh", &Tamguatomicfloat::Methodatanh, P_NONE, "atanh(): call atanh on the value");
    Tamguatomicfloat::AddMethod(global, "cbrt", &Tamguatomicfloat::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
    Tamguatomicfloat::AddMethod(global, "cos", &Tamguatomicfloat::Methodcos, P_NONE, "cos(): call cos on the value");
    Tamguatomicfloat::AddMethod(global, "cosh", &Tamguatomicfloat::Methodcosh, P_NONE, "cosh(): call cosh on the value");
    Tamguatomicfloat::AddMethod(global, "erf", &Tamguatomicfloat::Methoderf, P_NONE, "erf(): call erf on the value");
    Tamguatomicfloat::AddMethod(global, "erfc", &Tamguatomicfloat::Methoderfc, P_NONE, "erfc(): call erfc on the value");
    Tamguatomicfloat::AddMethod(global, "exp", &Tamguatomicfloat::Methodexp, P_NONE, "exp(): call exp on the value");
    Tamguatomicfloat::AddMethod(global, "exp2", &Tamguatomicfloat::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
    Tamguatomicfloat::AddMethod(global, "expm1", &Tamguatomicfloat::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
    Tamguatomicfloat::AddMethod(global, "floor", &Tamguatomicfloat::Methodfloor, P_NONE, "floor(): call floor on the value");
    Tamguatomicfloat::AddMethod(global, "lgamma", &Tamguatomicfloat::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
    Tamguatomicfloat::AddMethod(global, "ln", &Tamguatomicfloat::Methodln, P_NONE, "ln(): call log on the value");
    Tamguatomicfloat::AddMethod(global, "log", &Tamguatomicfloat::Methodlog, P_NONE, "log(): call log10 on the value");
    Tamguatomicfloat::AddMethod(global, "log1p", &Tamguatomicfloat::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
    Tamguatomicfloat::AddMethod(global, "log2", &Tamguatomicfloat::Methodlog2, P_NONE, "log2(): call log2 on the value");
    Tamguatomicfloat::AddMethod(global, "logb", &Tamguatomicfloat::Methodlogb, P_NONE, "logb(): call logb on the value");
    Tamguatomicfloat::AddMethod(global, "nearbyint", &Tamguatomicfloat::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
    Tamguatomicfloat::AddMethod(global, "rint", &Tamguatomicfloat::Methodrint, P_NONE, "rint(): call rint on the value");
    Tamguatomicfloat::AddMethod(global, "round", &Tamguatomicfloat::Methodround, P_NONE, "round(): call round on the value");
    Tamguatomicfloat::AddMethod(global, "sin", &Tamguatomicfloat::Methodsin, P_NONE, "sin(): call sin on the value");
    Tamguatomicfloat::AddMethod(global, "sinh", &Tamguatomicfloat::Methodsinh, P_NONE, "sinh(): call sinh on the value");
    Tamguatomicfloat::AddMethod(global, "sqrt", &Tamguatomicfloat::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
    Tamguatomicfloat::AddMethod(global, "tan", &Tamguatomicfloat::Methodtan, P_NONE, "tan(): call tan on the value");
    Tamguatomicfloat::AddMethod(global, "tanh", &Tamguatomicfloat::Methodtanh, P_NONE, "tanh(): call tanh on the value");
    Tamguatomicfloat::AddMethod(global, "tgamma", &Tamguatomicfloat::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
    Tamguatomicfloat::AddMethod(global, "trunc", &Tamguatomicfloat::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
    Tamguatomicfloat::AddMethod(global, "even", &Tamguatomicfloat::Methodeven, P_NONE, "even(): return true is the value is even");
    Tamguatomicfloat::AddMethod(global, "odd", &Tamguatomicfloat::Methododd, P_NONE, "odd(): return true is the value is odd");
    
    global->newInstance[Tamguatomicfloat::idtype] = new Tamguatomicfloat(0, global);
    global->RecordMethods(Tamguatomicfloat::idtype, Tamguatomicfloat::exported);
    
    return true;
}

Tamgu* Tamgufloat::Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguustring* r = globalTamgu->Provideustring(L"");
    r->value = (wchar_t)value;
    return r;
}


Tamgu* Tamgufloat::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    char buffer[101];
    Tamgu* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
    string sformat = kformat->String();

    #ifdef WIN32
    _set_invalid_parameter_handler(wrongSprintf);

    errorsprintf = false;
    sprintf_s(buffer, 100, STR(sformat), Float());
    if (errorsprintf == true)
        return globalTamgu->Returnerror("Incorrect format specifier or size too long", idthread);
    #else
    int spres;
    spres = sprintf_s(buffer, 100, STR(sformat), Float());
    if (spres<0)
        return globalTamgu->Returnerror("TAMGUI(124):Incorrect format specifier or size too long",idthread);
    #endif

    return globalTamgu->Providestring(buffer);
}


Tamgu* Tamgufloat::MethodRadian(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    double num = value;
    num = M_PI*(num / 180);
    return globalTamgu->Providefloat(num);
}

Tamgu* Tamgufloat::MethodDegree(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    double num = value;
    num = (num * 180) / M_PI;
    return globalTamgu->Providefloat(num);
}

//-------------------------------------------------------
Tamgu* Tamguatomicfloat::Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguustring* r = globalTamgu->Provideustring(L"");
    r->value = (wchar_t)value;
    return r;
}

Tamgu* Tamguatomicfloat::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    char buffer[101];
    Tamgu* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
    string sformat = kformat->String();
    
#ifdef WIN32
    _set_invalid_parameter_handler(wrongSprintf);
    
    errorsprintf = false;
    sprintf_s(buffer, 100, STR(sformat), Float());
    if (errorsprintf == true)
        return globalTamgu->Returnerror("Incorrect format specifier or size too long", idthread);
#else
    int spres;
    spres = sprintf_s(buffer, 100, STR(sformat), Float());
    if (spres<0)
        return globalTamgu->Returnerror("TAMGUI(124):Incorrect format specifier or size too long",idthread);
#endif
    
    return globalTamgu->Providestring(buffer);
}


Tamgu* Tamguatomicfloat::MethodRadian(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    double num = value;
    num = M_PI*(num / 180);
    return new Tamguatomicfloat(num);
}

Tamgu* Tamguatomicfloat::MethodDegree(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    double num = value;
    num = (num * 180) / M_PI;
    return new Tamguatomicfloat(num);
}

void TamguLoopFloat::Callfunction() {

    TamguCallFunction kfunc(function);

    Tamguint* ki = globalTamgu->Provideint(position);
    ki->Setreference();
    kfunc.arguments.push_back(this);
    kfunc.arguments.push_back(ki);
    Tamgu* ke = kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
    ke->Release();
    ki->Resetreference();
}

Tamgu* TamguLoopFloat::Put(Tamgu* context, Tamgu* ke, short idthread) {
    if (ke->isVectorContainer()) {
        position = 0;
        interval.clear();
        for (int i = 0; i < ke->Size(); i++)
            interval.push_back(ke->getfloat(i));
        value = interval[0];
        return aTRUE;
    }

    if (ke->Type() == a_floop) {
        TamguLoopFloat* kl = (TamguLoopFloat*)ke;
        interval = kl->interval;
        value = kl->value;
        position = kl->position;
        return aTRUE;
    }

    if (interval.size() == 0) {
        position = 0;
        value = 0;
        return aTRUE;
    }

    position = ke->Integer();
    if (position >= interval.size())
        position = position % interval.size();
    else
        if (position<0)
            position = (interval.size() + position) % interval.size();
    value = interval[position];
    return aTRUE;
}


Tamgu* TamguLoopFloat::Vector(short idthread) {
    Tamgufvector* kvect = globalTamgu->Providefvector();
    kvect->values = interval;
    return kvect;
}

Tamgu* Tamgufloat::MethodExponent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int n = 0;
    
    frexp (value , &n);
    return globalTamgu->Provideint((long)n);
}

Tamgu* Tamgufloat::MethodBits(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 1) {
        double64 f(0);
        f.bits = callfunc->Evaluate(0, contextualpattern, idthread)->Long();
        return globalTamgu->Providefloat(f.v);
    }
    double64 d(value);
    return new Tamgulong(d.bits);
}

Tamgu* Tamguatomicfloat::MethodExponent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int n = 0;
    
    frexp (value , &n);
    return new Tamguatomicint((long)n);
}

Tamgu* Tamguatomicfloat::MethodBits(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (callfunc->Size() == 1) {
        double64 f(0);
        f.bits = callfunc->Evaluate(0, contextualpattern, idthread)->Long();
        return globalTamgu->Providefloat(f.v);
    }
    double64 d(value);
    return new Tamgulong(d.bits);
}


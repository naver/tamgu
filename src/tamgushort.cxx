/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgushort.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgushort.h"
#include "tamguint.h"
#include "tamgulong.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<shortMethod>  Tamgushort::methods;
Exporting hmap<string, string> Tamgushort::infomethods;
Exporting basebin_hash<unsigned long> Tamgushort::exported;

Exporting short Tamgushort::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgushort::AddMethod(TamguGlobal* global, string name, shortMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgushort::Setidtype(TamguGlobal* global) {
    Tamgushort::InitialisationModule(global,"");
}


   bool Tamgushort::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgushort::idtype = global->Getid("short");

    Tamgushort::AddMethod(global, "succ", &Tamgushort::MethodSucc, P_NONE, "succ(): return the successor of a short");
    Tamgushort::AddMethod(global, "pred", &Tamgushort::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");

    Tamgushort::AddMethod(global, "chr", &Tamgushort::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
    Tamgushort::AddMethod(global, "format", &Tamgushort::MethodFormat, P_ONE, "format(string pattern): Return a string matching the C pattern.");
    Tamgushort::AddMethod(global, "isprime", &Tamgushort::MethodPrime, P_NONE, "prime(): return true is the number is a prime");
    Tamgushort::AddMethod(global, "factors", &Tamgushort::MethodPrimefactors, P_NONE, "factors(): return the list of prime factors");

    Tamgushort::AddMethod(global, "abs", &Tamgushort::Methodabs, P_NONE, "abs(): call fabs on the value");
    Tamgushort::AddMethod(global, "acos", &Tamgushort::Methodacos, P_NONE, "acos(): call acos on the value");
    Tamgushort::AddMethod(global, "acosh", &Tamgushort::Methodacosh, P_NONE, "acosh(): call acosh on the value");
    Tamgushort::AddMethod(global, "asin", &Tamgushort::Methodasin, P_NONE, "asin(): call asin on the value");
    Tamgushort::AddMethod(global, "asinh", &Tamgushort::Methodasinh, P_NONE, "asinh(): call asinh on the value");
    Tamgushort::AddMethod(global, "atan", &Tamgushort::Methodatan, P_NONE, "atan(): call atan on the value");
    Tamgushort::AddMethod(global, "atanh", &Tamgushort::Methodatanh, P_NONE, "atanh(): call atanh on the value");
    Tamgushort::AddMethod(global, "cbrt", &Tamgushort::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
    Tamgushort::AddMethod(global, "cos", &Tamgushort::Methodcos, P_NONE, "cos(): call cos on the value");
    Tamgushort::AddMethod(global, "cosh", &Tamgushort::Methodcosh, P_NONE, "cosh(): call cosh on the value");
    Tamgushort::AddMethod(global, "erf", &Tamgushort::Methoderf, P_NONE, "erf(): call erf on the value");
    Tamgushort::AddMethod(global, "erfc", &Tamgushort::Methoderfc, P_NONE, "erfc(): call erfc on the value");
    Tamgushort::AddMethod(global, "exp", &Tamgushort::Methodexp, P_NONE, "exp(): call exp on the value");
    Tamgushort::AddMethod(global, "exp2", &Tamgushort::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
    Tamgushort::AddMethod(global, "expm1", &Tamgushort::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
    Tamgushort::AddMethod(global, "floor", &Tamgushort::Methodfloor, P_NONE, "floor(): call floor on the value");
    Tamgushort::AddMethod(global, "lgamma", &Tamgushort::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
    Tamgushort::AddMethod(global, "ln", &Tamgushort::Methodln, P_NONE, "ln(): call log on the value");
    Tamgushort::AddMethod(global, "log", &Tamgushort::Methodlog, P_NONE, "log(): call log10 on the value");
    Tamgushort::AddMethod(global, "log1p", &Tamgushort::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
    Tamgushort::AddMethod(global, "log2", &Tamgushort::Methodlog2, P_NONE, "log2(): call log2 on the value");
    Tamgushort::AddMethod(global, "logb", &Tamgushort::Methodlogb, P_NONE, "logb(): call logb on the value");
    Tamgushort::AddMethod(global, "nearbyint", &Tamgushort::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
    Tamgushort::AddMethod(global, "rint", &Tamgushort::Methodrint, P_NONE, "rint(): call rint on the value");
    Tamgushort::AddMethod(global, "round", &Tamgushort::Methodround, P_NONE, "round(): call round on the value");
    Tamgushort::AddMethod(global, "sin", &Tamgushort::Methodsin, P_NONE, "sin(): call sin on the value");
    Tamgushort::AddMethod(global, "sinh", &Tamgushort::Methodsinh, P_NONE, "sinh(): call sinh on the value");
    Tamgushort::AddMethod(global, "sqrt", &Tamgushort::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
    Tamgushort::AddMethod(global, "tan", &Tamgushort::Methodtan, P_NONE, "tan(): call tan on the value");
    Tamgushort::AddMethod(global, "tanh", &Tamgushort::Methodtanh, P_NONE, "tanh(): call tanh on the value");
    Tamgushort::AddMethod(global, "tgamma", &Tamgushort::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
    Tamgushort::AddMethod(global, "trunc", &Tamgushort::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
    Tamgushort::AddMethod(global, "even", &Tamgushort::Methodeven, P_NONE, "even(): return true is the value is even");
    Tamgushort::AddMethod(global, "odd", &Tamgushort::Methododd, P_NONE, "odd(): return true is the value is odd");




    if (version != "") {
        global->newInstance[Tamgushort::idtype] = new Tamgushort(0, global);
        global->RecordMethods(Tamgushort::idtype,Tamgushort::exported);
    }

    return true;
}

Tamgu* Tamgushort::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    char buffer[101];
    Tamgu* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
    string sformat = kformat->String();

    #ifdef WIN32
    _set_invalid_parameter_handler(wrongSprintf);

    errorsprintf = false;
    sprintf_s(buffer, 100, STR(sformat), Short());
    if (errorsprintf == true)
        return globalTamgu->Returnerror("Incorrect format specifier or size too long", idthread);
    #else
    int spres;
    spres = sprintf_s(buffer, 100, STR(sformat), Short());
    if (spres<0)
        return globalTamgu->Returnerror("TAMGUI(124):Incorrect format specifier or size too long", idthread);
    #endif

    return globalTamgu->Providestring(buffer);
}



Tamgu* Tamgushort::plus(Tamgu* a, bool itself) {
    if (!itself && a->isFloat()) {
        double v = value;
        return globalTamgu->Providefloat(v + a->Float());
    }

    BLONG v = value;
    v += a->Long();

    if (IsLong(v))
        return new Tamgulong(v);

    if (!IsShort(v))
        return globalTamgu->Provideint(v);


    if (itself) {
        value = v;
        return this;
    }

    return new Tamgushort(v);
}

Tamgu* Tamgushort::multiply(Tamgu* a, bool itself) {
    if (!itself && a->isFloat()) {
        double v = value;
        return globalTamgu->Providefloat(v + a->Float());
    }

    BLONG v = value;
    v *= a->Long();

    if (IsLong(v))
        return new Tamgulong(v);

    if (!IsShort(v))
        return globalTamgu->Provideint(v);


    if (itself) {
        value = v;
        return this;
    }

    return new Tamgushort(v);
}

Tamgu* Tamgushort::shiftleft(Tamgu* a, bool itself) {
    if (!itself && a->isFloat()) {
        double v = value;
        return globalTamgu->Providefloat(v + a->Float());
    }

    BLONG v = value;
    v <<= a->Long();

    if (IsLong(v))
        return new Tamgulong(v);

    if (!IsShort(v))
        return globalTamgu->Provideint(v);

    if (itself) {
        value = v;
        return this;
    }

    return new Tamgushort(v);
}

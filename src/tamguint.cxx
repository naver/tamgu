/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguint.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguint.h"
#include "tamgustring.h"
#include "tamguustring.h"
#include "tamguivector.h"
#include "instructions.h"
#include "tamgulong.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<intMethod>  Tamguint::methods;
Exporting hmap<string, string> Tamguint::infomethods;
Exporting bin_hash<unsigned long> Tamguint::exported;

Exporting short Tamguint::idtype = 0;

Exporting basebin_hash<atomicintMethod>  Tamguatomicint::methods;
Exporting hmap<string, string> Tamguatomicint::infomethods;
Exporting bin_hash<unsigned long> Tamguatomicint::exported;

Exporting short Tamguatomicint::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguint::AddMethod(TamguGlobal* global, string name, intMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

void Tamguatomicint::AddMethod(TamguGlobal* global, string name, atomicintMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguint::Setidtype(TamguGlobal* global) {
        Tamguint::idtype = global->Getid("int");
    }

   bool Tamguint::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamguint::idtype = global->Getid("int");

    Tamguint::AddMethod(global, "isemoji", &Tamguint::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters");
    Tamguint::AddMethod(global, "emoji", &Tamguint::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji");
    global->returntypes[global->Getid("emoji")] = a_string;

    Tamguint::AddMethod(global, "chr", &Tamguint::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
    Tamguint::AddMethod(global, "succ", &Tamguint::MethodSucc, P_NONE, "succ(): return the successor of an integer");
    Tamguint::AddMethod(global, "pred", &Tamguint::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");

    Tamguint::AddMethod(global, "isprime", &Tamguint::MethodPrime, P_NONE, "prime(): return true is the number is a prime");
    Tamguint::AddMethod(global, "factors", &Tamguint::MethodPrimefactors, P_NONE, "factors(): return the list of prime factors");
    global->returntypes[global->Getid("factors")] = a_ivector;
    Tamguint::AddMethod(global, "bit", &Tamguint::MethodBit, P_ONE, "bit(i): check if the ith bit is 0 or 1");
    Tamguint::AddMethod(global, "invert", &Tamguint::MethodInvert, P_NONE, "");

    Tamguint::AddMethod(global, "format", &Tamguint::MethodFormat, P_ONE, "format(pattern): Return a string matching the C pattern.");

    Tamguint::AddMethod(global, "abs", &Tamguint::Methodabs, P_NONE, "abs(): call fabs on the value");
    Tamguint::AddMethod(global, "acos", &Tamguint::Methodacos, P_NONE, "acos(): call acos on the value");
    Tamguint::AddMethod(global, "acosh", &Tamguint::Methodacosh, P_NONE, "acosh(): call acosh on the value");
    Tamguint::AddMethod(global, "asin", &Tamguint::Methodasin, P_NONE, "asin(): call asin on the value");
    Tamguint::AddMethod(global, "asinh", &Tamguint::Methodasinh, P_NONE, "asinh(): call asinh on the value");
    Tamguint::AddMethod(global, "atan", &Tamguint::Methodatan, P_NONE, "atan(): call atan on the value");
    Tamguint::AddMethod(global, "atanh", &Tamguint::Methodatanh, P_NONE, "atanh(): call atanh on the value");
    Tamguint::AddMethod(global, "cbrt", &Tamguint::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
    Tamguint::AddMethod(global, "cos", &Tamguint::Methodcos, P_NONE, "cos(): call cos on the value");
    Tamguint::AddMethod(global, "cosh", &Tamguint::Methodcosh, P_NONE, "cosh(): call cosh on the value");
    Tamguint::AddMethod(global, "erf", &Tamguint::Methoderf, P_NONE, "erf(): call erf on the value");
    Tamguint::AddMethod(global, "erfc", &Tamguint::Methoderfc, P_NONE, "erfc(): call erfc on the value");
    Tamguint::AddMethod(global, "exp", &Tamguint::Methodexp, P_NONE, "exp(): call exp on the value");
    Tamguint::AddMethod(global, "exp2", &Tamguint::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
    Tamguint::AddMethod(global, "expm1", &Tamguint::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
    Tamguint::AddMethod(global, "floor", &Tamguint::Methodfloor, P_NONE, "floor(): call floor on the value");
    Tamguint::AddMethod(global, "lgamma", &Tamguint::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
    Tamguint::AddMethod(global, "ln", &Tamguint::Methodln, P_NONE, "ln(): call log on the value");
    Tamguint::AddMethod(global, "log", &Tamguint::Methodlog, P_NONE, "log(): call log10 on the value");
    Tamguint::AddMethod(global, "log1p", &Tamguint::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
    Tamguint::AddMethod(global, "log2", &Tamguint::Methodlog2, P_NONE, "log2(): call log2 on the value");
    Tamguint::AddMethod(global, "logb", &Tamguint::Methodlogb, P_NONE, "logb(): call logb on the value");
    Tamguint::AddMethod(global, "nearbyint", &Tamguint::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
    Tamguint::AddMethod(global, "rint", &Tamguint::Methodrint, P_NONE, "rint(): call rint on the value");
    Tamguint::AddMethod(global, "round", &Tamguint::Methodround, P_NONE, "round(): call round on the value");
    Tamguint::AddMethod(global, "sin", &Tamguint::Methodsin, P_NONE, "sin(): call sin on the value");
    Tamguint::AddMethod(global, "sinh", &Tamguint::Methodsinh, P_NONE, "sinh(): call sinh on the value");
    Tamguint::AddMethod(global, "sqrt", &Tamguint::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
    Tamguint::AddMethod(global, "tan", &Tamguint::Methodtan, P_NONE, "tan(): call tan on the value");
    Tamguint::AddMethod(global, "tanh", &Tamguint::Methodtanh, P_NONE, "tanh(): call tanh on the value");
    Tamguint::AddMethod(global, "tgamma", &Tamguint::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
    Tamguint::AddMethod(global, "trunc", &Tamguint::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
    Tamguint::AddMethod(global, "even", &Tamguint::Methodeven, P_NONE, "even(): return true is the value is even");
    Tamguint::AddMethod(global, "odd", &Tamguint::Methododd, P_NONE, "odd(): return true is the value is odd");


    global->newInstance[Tamguint::idtype] = new Tamguint(0, global);
    global->newInstance[a_intthrough] = global->newInstance[Tamguint::idtype];
    global->RecordMethods(Tamguint::idtype, Tamguint::exported);
    global->RecordMethods(a_intthrough, Tamguint::exported);
    global->RecordMethods(a_iloop, Tamguint::exported);

    Tamguatomicint::InitialisationModule(global, version);
    
    return true;
}

bool Tamguatomicint::InitialisationModule(TamguGlobal* global, string version) {

    methods.clear();
    infomethods.clear();
    exported.clear();
    
    Tamguatomicint::idtype = global->Getid("a_int");

    Tamguatomicint::AddMethod(global, "isemoji", &Tamguatomicint::MethodIsemoji, P_NONE, "isemoji(): Test if a string only contains emoji characters");
    Tamguatomicint::AddMethod(global, "emoji", &Tamguatomicint::MethodEmoji, P_NONE, "emoji(): Return the textual description of an emoji");
    global->returntypes[global->Getid("emoji")] = a_string;
    
    Tamguatomicint::AddMethod(global, "chr", &Tamguatomicint::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
    Tamguatomicint::AddMethod(global, "succ", &Tamguatomicint::MethodSucc, P_NONE, "succ(): return the successor of an integer");
    Tamguatomicint::AddMethod(global, "pred", &Tamguatomicint::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");
    
    Tamguatomicint::AddMethod(global, "isprime", &Tamguatomicint::MethodPrime, P_NONE, "prime(): return true is the number is a prime");
    Tamguatomicint::AddMethod(global, "factors", &Tamguatomicint::MethodPrimefactors, P_NONE, "factors(): return the list of prime factors");
    global->returntypes[global->Getid("factors")] = a_ivector;
    Tamguatomicint::AddMethod(global, "bit", &Tamguatomicint::MethodBit, P_ONE, "bit(i): check if the ith bit is 0 or 1");
    Tamguatomicint::AddMethod(global, "invert", &Tamguatomicint::MethodInvert, P_NONE, "");
    
    Tamguatomicint::AddMethod(global, "format", &Tamguatomicint::MethodFormat, P_ONE, "format(pattern): Return a string matching the C pattern.");
    
    Tamguatomicint::AddMethod(global, "abs", &Tamguatomicint::Methodabs, P_NONE, "abs(): call fabs on the value");
    Tamguatomicint::AddMethod(global, "acos", &Tamguatomicint::Methodacos, P_NONE, "acos(): call acos on the value");
    Tamguatomicint::AddMethod(global, "acosh", &Tamguatomicint::Methodacosh, P_NONE, "acosh(): call acosh on the value");
    Tamguatomicint::AddMethod(global, "asin", &Tamguatomicint::Methodasin, P_NONE, "asin(): call asin on the value");
    Tamguatomicint::AddMethod(global, "asinh", &Tamguatomicint::Methodasinh, P_NONE, "asinh(): call asinh on the value");
    Tamguatomicint::AddMethod(global, "atan", &Tamguatomicint::Methodatan, P_NONE, "atan(): call atan on the value");
    Tamguatomicint::AddMethod(global, "atanh", &Tamguatomicint::Methodatanh, P_NONE, "atanh(): call atanh on the value");
    Tamguatomicint::AddMethod(global, "cbrt", &Tamguatomicint::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
    Tamguatomicint::AddMethod(global, "cos", &Tamguatomicint::Methodcos, P_NONE, "cos(): call cos on the value");
    Tamguatomicint::AddMethod(global, "cosh", &Tamguatomicint::Methodcosh, P_NONE, "cosh(): call cosh on the value");
    Tamguatomicint::AddMethod(global, "erf", &Tamguatomicint::Methoderf, P_NONE, "erf(): call erf on the value");
    Tamguatomicint::AddMethod(global, "erfc", &Tamguatomicint::Methoderfc, P_NONE, "erfc(): call erfc on the value");
    Tamguatomicint::AddMethod(global, "exp", &Tamguatomicint::Methodexp, P_NONE, "exp(): call exp on the value");
    Tamguatomicint::AddMethod(global, "exp2", &Tamguatomicint::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
    Tamguatomicint::AddMethod(global, "expm1", &Tamguatomicint::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
    Tamguatomicint::AddMethod(global, "floor", &Tamguatomicint::Methodfloor, P_NONE, "floor(): call floor on the value");
    Tamguatomicint::AddMethod(global, "lgamma", &Tamguatomicint::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
    Tamguatomicint::AddMethod(global, "ln", &Tamguatomicint::Methodln, P_NONE, "ln(): call log on the value");
    Tamguatomicint::AddMethod(global, "log", &Tamguatomicint::Methodlog, P_NONE, "log(): call log10 on the value");
    Tamguatomicint::AddMethod(global, "log1p", &Tamguatomicint::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
    Tamguatomicint::AddMethod(global, "log2", &Tamguatomicint::Methodlog2, P_NONE, "log2(): call log2 on the value");
    Tamguatomicint::AddMethod(global, "logb", &Tamguatomicint::Methodlogb, P_NONE, "logb(): call logb on the value");
    Tamguatomicint::AddMethod(global, "nearbyint", &Tamguatomicint::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
    Tamguatomicint::AddMethod(global, "rint", &Tamguatomicint::Methodrint, P_NONE, "rint(): call rint on the value");
    Tamguatomicint::AddMethod(global, "round", &Tamguatomicint::Methodround, P_NONE, "round(): call round on the value");
    Tamguatomicint::AddMethod(global, "sin", &Tamguatomicint::Methodsin, P_NONE, "sin(): call sin on the value");
    Tamguatomicint::AddMethod(global, "sinh", &Tamguatomicint::Methodsinh, P_NONE, "sinh(): call sinh on the value");
    Tamguatomicint::AddMethod(global, "sqrt", &Tamguatomicint::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
    Tamguatomicint::AddMethod(global, "tan", &Tamguatomicint::Methodtan, P_NONE, "tan(): call tan on the value");
    Tamguatomicint::AddMethod(global, "tanh", &Tamguatomicint::Methodtanh, P_NONE, "tanh(): call tanh on the value");
    Tamguatomicint::AddMethod(global, "tgamma", &Tamguatomicint::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
    Tamguatomicint::AddMethod(global, "trunc", &Tamguatomicint::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
    Tamguatomicint::AddMethod(global, "even", &Tamguatomicint::Methodeven, P_NONE, "even(): return true is the value is even");
    Tamguatomicint::AddMethod(global, "odd", &Tamguatomicint::Methododd, P_NONE, "odd(): return true is the value is odd");
    
    
    global->newInstance[Tamguatomicint::idtype] = new Tamguatomicint(0, global);
    
    global->RecordMethods(Tamguatomicint::idtype, Tamguatomicint::exported);

    return true;
}


Tamgu* Tamguint::MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return booleantamgu[c_is_emoji((BLONG)value)];
}

Tamgu* Tamguint::MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string s = Emoji((BLONG)value);
    return globalTamgu->Providestring(s);
}


Tamgu* Tamguint::Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguustring* r = globalTamgu->Provideustring(L"");
    r->value = (wchar_t)value;
    return r;
}

Tamgu* Tamguint::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    char buffer[101];
    Tamgu* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
    string sformat = kformat->String();

    #ifdef WIN32
    _set_invalid_parameter_handler(wrongSprintf);

    errorsprintf = false;
    sprintf_s(buffer, 100, STR(sformat), Integer());
    if (errorsprintf == true)
        return globalTamgu->Returnerror("Incorrect format specifier or size too long", idthread);
    #else
    int spres;
    spres = sprintf_s(buffer, 100, STR(sformat), Integer());
    if (spres<0)
        return globalTamgu->Returnerror("TAMGUI(124):Incorrect format specifier or size too long", idthread);
    #endif

    return globalTamgu->Providestring(buffer);
}

Tamgu* Tamguint::MethodBit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long ith = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    long v = value;
    if (ith < 0 || ith >= sizeof(long)* 8)
        return aFALSE;
    v &= 1 << ith;
    if (!v)
        return aFALSE;
    return aTRUE;
}


//-----------------------------------------

Tamgu* Tamguatomicint::MethodIsemoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return booleantamgu[c_is_emoji((BLONG)value)];
}

Tamgu* Tamguatomicint::MethodEmoji(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string s = Emoji((BLONG)value);
    return globalTamgu->Providestring(s);
}


Tamgu* Tamguatomicint::Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguustring* r = globalTamgu->Provideustring(L"");
    r->value = (wchar_t)value;
    return r;
}

Tamgu* Tamguatomicint::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    
    char buffer[101];
    Tamgu* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
    string sformat = kformat->String();
    
#ifdef WIN32
    _set_invalid_parameter_handler(wrongSprintf);
    
    errorsprintf = false;
    sprintf_s(buffer, 100, STR(sformat), Integer());
    if (errorsprintf == true)
        return globalTamgu->Returnerror("Incorrect format specifier or size too long", idthread);
#else
    int spres;
    spres = sprintf_s(buffer, 100, STR(sformat), Integer());
    if (spres<0)
        return globalTamgu->Returnerror("TAMGUI(124):Incorrect format specifier or size too long", idthread);
#endif
    
    return globalTamgu->Providestring(buffer);
}

Tamgu* Tamguatomicint::MethodBit(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long ith = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
    long v = value;
    if (ith < 0 || ith >= sizeof(long)* 8)
        return aFALSE;
    v &= 1 << ith;
    if (!v)
        return aFALSE;
    return aTRUE;
}


void TamguLoopInteger::Callfunction() {

    TamguCallFunction kfunc(function);

    Tamguint* ki = globalTamgu->Provideint(position);
    ki->Setreference();
    kfunc.arguments.push_back(this);
    kfunc.arguments.push_back(ki);
    Tamgu* ke = kfunc.Get(aNULL, aNULL, globalTamgu->GetThreadid());
    ke->Release();
    ki->Resetreference();
}

Tamgu* TamguLoopInteger::Put(Tamgu* context, Tamgu* ke, short idthread) {
    if (ke->isVectorContainer()) {
        position = 0;
        interval.clear();
        for (int i = 0; i < ke->Size(); i++)
            interval.push_back(ke->getinteger(i));
        value = interval[0];
        return aTRUE;
    }

    if (ke->Type() == a_iloop) {
        TamguLoopInteger* kl = (TamguLoopInteger*)ke;
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


Tamgu* TamguLoopInteger::Vector(short idthread) {
    Tamguivector* kvect = globalTamgu->Provideivector();
    kvect->values = interval;
    return kvect;
}

Exporting Tamgu* Tamguint::plus(Tamgu* a, bool itself) {
    if (!itself && a->isFloat()) {
        double v = value;
        return globalTamgu->Providefloat(v + a->Float());
    }

    BLONG v = value;
    v += a->Long();

    if (IsLong(v))
        return new Tamgulong(v);

    if (itself) {
        value = v;
        return this;
    }


    return globalTamgu->Provideint(v);
}

Exporting Tamgu* Tamguint::multiply(Tamgu* a, bool itself) {
    if (!itself && a->isFloat()) {
        double v = value;
        return globalTamgu->Providefloat(v*a->Float());
    }

    BLONG v = value;
    v *= a->Long();

    if (IsLong(v))
        return new Tamgulong(v);


    if (itself) {
        value = v;
        return this;
    }

    return globalTamgu->Provideint(v);
}

Exporting Tamgu* Tamguint::shiftleft(Tamgu* a, bool itself) {
    BLONG v = value;
    v <<= a->Integer();
    if (IsLong(v))
        return new Tamgulong(v);

    if (itself) {
        value = v;
        return this;
    }


    return globalTamgu->Provideint(v);
}

Exporting Tamgu* Tamguatomicint::plus(Tamgu* a, bool itself) {
    if (!itself && a->isFloat()) {
        double v = value;
        return globalTamgu->Providefloat(v + a->Float());
    }
    
    BLONG v = value;
    v += a->Long();
    
    if (IsLong(v))
        return new Tamgulong(v);
    
    if (itself) {
        value = v;
        return this;
    }
    
    
    return globalTamgu->Provideint(v);
}

Exporting Tamgu* Tamguatomicint::multiply(Tamgu* a, bool itself) {
    if (!itself && a->isFloat()) {
        double v = value;
        return globalTamgu->Providefloat(v*a->Float());
    }
    
    BLONG v = value;
    v *= a->Long();
    
    if (IsLong(v))
        return new Tamgulong(v);
    
    
    if (itself) {
        value = v;
        return this;
    }
    
    return new Tamguatomicint(v);
}


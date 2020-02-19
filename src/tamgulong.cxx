/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgulong.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgulong.h"
#include "instructions.h"
#include "tamgulvector.h"
#include "tamguustring.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<longMethod>  Tamgulong::methods;
Exporting hmap<string, string> Tamgulong::infomethods;
Exporting bin_hash<unsigned long> Tamgulong::exported;

Exporting short Tamgulong::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgulong::AddMethod(TamguGlobal* global, string name, longMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgulong::Setidtype(TamguGlobal* global) {
        Tamgulong::idtype = global->Getid("long");
    }

   bool Tamgulong::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgulong::idtype = global->Getid("long");

    Tamgulong::AddMethod(global, "chr", &Tamgulong::Methodchr, P_NONE, "chr(): return the character matching the unicode code");
    Tamgulong::AddMethod(global, "format", &Tamgulong::MethodFormat, P_ONE, "format(string pattern): Return a string matching the C pattern.");
    Tamgulong::AddMethod(global, "isprime", &Tamgulong::MethodPrime, P_NONE, "prime(): return true is the number is a prime");
    Tamgulong::AddMethod(global, "factors", &Tamgulong::MethodPrimefactors, P_NONE, "factors(): return the list of prime factors");

    Tamgulong::AddMethod(global, "succ", &Tamgulong::MethodSucc, P_NONE, "succ(): return the successor of an integer");
    Tamgulong::AddMethod(global, "pred", &Tamgulong::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.");


    Tamgulong::AddMethod(global, "abs", &Tamgulong::Methodabs, P_NONE, "abs(): call fabs on the value");
    Tamgulong::AddMethod(global, "acos", &Tamgulong::Methodacos, P_NONE, "acos(): call acos on the value");
    Tamgulong::AddMethod(global, "acosh", &Tamgulong::Methodacosh, P_NONE, "acosh(): call acosh on the value");
    Tamgulong::AddMethod(global, "asin", &Tamgulong::Methodasin, P_NONE, "asin(): call asin on the value");
    Tamgulong::AddMethod(global, "asinh", &Tamgulong::Methodasinh, P_NONE, "asinh(): call asinh on the value");
    Tamgulong::AddMethod(global, "atan", &Tamgulong::Methodatan, P_NONE, "atan(): call atan on the value");
    Tamgulong::AddMethod(global, "atanh", &Tamgulong::Methodatanh, P_NONE, "atanh(): call atanh on the value");
    Tamgulong::AddMethod(global, "cbrt", &Tamgulong::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value");
    Tamgulong::AddMethod(global, "cos", &Tamgulong::Methodcos, P_NONE, "cos(): call cos on the value");
    Tamgulong::AddMethod(global, "cosh", &Tamgulong::Methodcosh, P_NONE, "cosh(): call cosh on the value");
    Tamgulong::AddMethod(global, "erf", &Tamgulong::Methoderf, P_NONE, "erf(): call erf on the value");
    Tamgulong::AddMethod(global, "erfc", &Tamgulong::Methoderfc, P_NONE, "erfc(): call erfc on the value");
    Tamgulong::AddMethod(global, "exp", &Tamgulong::Methodexp, P_NONE, "exp(): call exp on the value");
    Tamgulong::AddMethod(global, "exp2", &Tamgulong::Methodexp2, P_NONE, "exp2(): call exp2 on the value");
    Tamgulong::AddMethod(global, "expm1", &Tamgulong::Methodexpm1, P_NONE, "expm1(): call expm1 on the value");
    Tamgulong::AddMethod(global, "floor", &Tamgulong::Methodfloor, P_NONE, "floor(): call floor on the value");
    Tamgulong::AddMethod(global, "lgamma", &Tamgulong::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value");
    Tamgulong::AddMethod(global, "ln", &Tamgulong::Methodln, P_NONE, "ln(): call log on the value");
    Tamgulong::AddMethod(global, "log", &Tamgulong::Methodlog, P_NONE, "log(): call log10 on the value");
    Tamgulong::AddMethod(global, "log1p", &Tamgulong::Methodlog1p, P_NONE, "log1p(): call log1p on the value");
    Tamgulong::AddMethod(global, "log2", &Tamgulong::Methodlog2, P_NONE, "log2(): call log2 on the value");
    Tamgulong::AddMethod(global, "logb", &Tamgulong::Methodlogb, P_NONE, "logb(): call logb on the value");
    Tamgulong::AddMethod(global, "nearbyint", &Tamgulong::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value");
    Tamgulong::AddMethod(global, "rint", &Tamgulong::Methodrint, P_NONE, "rint(): call rint on the value");
    Tamgulong::AddMethod(global, "round", &Tamgulong::Methodround, P_NONE, "round(): call round on the value");
    Tamgulong::AddMethod(global, "sin", &Tamgulong::Methodsin, P_NONE, "sin(): call sin on the value");
    Tamgulong::AddMethod(global, "sinh", &Tamgulong::Methodsinh, P_NONE, "sinh(): call sinh on the value");
    Tamgulong::AddMethod(global, "sqrt", &Tamgulong::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value");
    Tamgulong::AddMethod(global, "tan", &Tamgulong::Methodtan, P_NONE, "tan(): call tan on the value");
    Tamgulong::AddMethod(global, "tanh", &Tamgulong::Methodtanh, P_NONE, "tanh(): call tanh on the value");
    Tamgulong::AddMethod(global, "tgamma", &Tamgulong::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value");
    Tamgulong::AddMethod(global, "trunc", &Tamgulong::Methodtrunc, P_NONE, "trunc(): call trunc on the value");
    Tamgulong::AddMethod(global, "even", &Tamgulong::Methodeven, P_NONE, "even(): return true is the value is even");
    Tamgulong::AddMethod(global, "odd", &Tamgulong::Methododd, P_NONE, "odd(): return true is the value is odd");



    global->newInstance[Tamgulong::idtype] = new Tamgulong(0, global);
    global->newInstance[a_longthrough] = global->newInstance[Tamgulong::idtype];
    global->RecordMethods(Tamgulong::idtype,Tamgulong::exported);
    global->RecordMethods(a_longthrough, Tamgulong::exported);
    global->RecordMethods(a_lloop, Tamgulong::exported);
    
    return true;
}

Tamgu* Tamgulong::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    char buffer[101];
    Tamgu* kformat = callfunc->Evaluate(0, contextualpattern, idthread);
    string sformat = kformat->String();

    #ifdef WIN32
    _set_invalid_parameter_handler(wrongSprintf);

    errorsprintf = false;
    sprintf_s(buffer, 100, STR(sformat), Long());
    if (errorsprintf == true)
        return globalTamgu->Returnerror("Incorrect format specifier or size too long", idthread);
    #else
    int spres;
    spres = sprintf_s(buffer, 100, STR(sformat), Long());
    if (spres<0)
        return globalTamgu->Returnerror("TAMGUI(124):Incorrect format specifier or size too long", idthread);
    #endif

    return globalTamgu->Providestring(buffer);
}



Tamgu* Tamgulong::Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguustring* r = globalTamgu->Provideustring(L"");
    r->value = (wchar_t)value;
    return r;
}

//--------------------------------------------------------------------------------------------

void TamguLoopLong::Callfunction() {

    TamguCallFunction kfunc(function);

    Tamguint* ki = globalTamgu->Provideint(position);
    ki->Setreference();
    kfunc.arguments.push_back(this);
    kfunc.arguments.push_back(ki);
    Tamgu* ke = kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
    ke->Release();
    ki->Resetreference();
}

Tamgu* TamguLoopLong::Put(Tamgu* context, Tamgu* ke, short idthread) {
    if (ke->isVectorContainer()) {
        position = 0;
        interval.clear();
        for (int i = 0; i < ke->Size(); i++)
            interval.push_back(ke->getlong(i));
        value = interval[0];
        return aTRUE;
    }

    if (ke->Type() == a_lloop) {
        TamguLoopLong* kl = (TamguLoopLong*)ke;
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


Tamgu* TamguLoopLong::Vector(short idthread) {
    Tamgulvector* kvect = new Tamgulvector;
    kvect->values = interval;
    return kvect;
}

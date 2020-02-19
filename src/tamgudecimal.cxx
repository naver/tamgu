/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgudecimal.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgudecimal.h"
#include "tamgustring.h"
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif

#include "tamgudvector.h"
#include "instructions.h"
#include "constobjects.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<decimalMethod>  Tamgudecimal::methods;
Exporting hmap<string, string> Tamgudecimal::infomethods;
Exporting bin_hash<unsigned long> Tamgudecimal::exported;

Exporting short Tamgudecimal::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgudecimal::AddMethod(TamguGlobal* global, string name, decimalMethod func, unsigned long arity, string infos, short typereturn) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
    if (typereturn != a_null)
        global->returntypes[idname] = typereturn;
}



    void Tamgudecimal::Setidtype(TamguGlobal* global) {
        Tamgudecimal::idtype = global->Getid("decimal");
    }

   bool Tamgudecimal::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    Tamgudecimal::idtype = global->Getid("decimal");

    Tamgudecimal::AddMethod(global, "chr", &Tamgudecimal::Methodchr, P_NONE, "chr(): return the character matching the unicode code", a_ustring);
    Tamgudecimal::AddMethod(global, "invert", &Tamgudecimal::MethodInvert, P_NONE, "", a_float);
    Tamgudecimal::AddMethod(global, "succ", &Tamgudecimal::MethodSucc, P_NONE, "succ(): Return a successor of the current value", a_float);
    Tamgudecimal::AddMethod(global, "pred", &Tamgudecimal::MethodPred, P_NONE, "pred(): Return the predecessor of a byte.", a_float);

    Tamgudecimal::AddMethod(global, "format", &Tamgudecimal::MethodFormat, P_ONE, "format(string pattern): Return a string matching the C pattern.", a_string);
    Tamgudecimal::AddMethod(global, "radian", &Tamgudecimal::MethodRadian, P_NONE, "abs(): radian(): return the radian value out of a degree value", a_float);
    Tamgudecimal::AddMethod(global, "degree", &Tamgudecimal::MethodDegree, P_NONE, "degree(): return the degree value out of a radian value", a_float);

    Tamgudecimal::AddMethod(global, "bits", &Tamgudecimal::MethodBits, P_NONE|P_ONE, "bits(): return the bit representation or initialize a float number with a bit representation (bits(int v))", a_none);
    Tamgudecimal::AddMethod(global, "mantissa", &Tamgudecimal::MethodMantissa, P_NONE, "mantissa(): return the mantissa value as a float", a_float);
    Tamgudecimal::AddMethod(global, "exponent", &Tamgudecimal::MethodExponent, P_NONE, "exponent(): return the exponent value as an int", a_int);

    Tamgudecimal::AddMethod(global, "abs", &Tamgudecimal::Methodabs, P_NONE, "abs(): call fabs on the value", a_float);
    Tamgudecimal::AddMethod(global, "acos", &Tamgudecimal::Methodacos, P_NONE, "acos(): call acos on the value", a_float);
    Tamgudecimal::AddMethod(global, "acosh", &Tamgudecimal::Methodacosh, P_NONE, "acosh(): call acosh on the value", a_float);
    Tamgudecimal::AddMethod(global, "asin", &Tamgudecimal::Methodasin, P_NONE, "asin(): call asin on the value", a_float);
    Tamgudecimal::AddMethod(global, "asinh", &Tamgudecimal::Methodasinh, P_NONE, "asinh(): call asinh on the value", a_float);
    Tamgudecimal::AddMethod(global, "atan", &Tamgudecimal::Methodatan, P_NONE, "atan(): call atan on the value", a_float);
    Tamgudecimal::AddMethod(global, "atanh", &Tamgudecimal::Methodatanh, P_NONE, "atanh(): call atanh on the value", a_float);
    Tamgudecimal::AddMethod(global, "cbrt", &Tamgudecimal::Methodcbrt, P_NONE, "cbrt(): call cbrt on the value", a_float);
    Tamgudecimal::AddMethod(global, "cos", &Tamgudecimal::Methodcos, P_NONE, "cos(): call cos on the value", a_float);
    Tamgudecimal::AddMethod(global, "cosh", &Tamgudecimal::Methodcosh, P_NONE, "cosh(): call cosh on the value", a_float);
    Tamgudecimal::AddMethod(global, "erf", &Tamgudecimal::Methoderf, P_NONE, "erf(): call erf on the value", a_float);
    Tamgudecimal::AddMethod(global, "erfc", &Tamgudecimal::Methoderfc, P_NONE, "erfc(): call erfc on the value", a_float);
    Tamgudecimal::AddMethod(global, "exp", &Tamgudecimal::Methodexp, P_NONE, "exp(): call exp on the value", a_float);
    Tamgudecimal::AddMethod(global, "exp2", &Tamgudecimal::Methodexp2, P_NONE, "exp2(): call exp2 on the value", a_float);
    Tamgudecimal::AddMethod(global, "expm1", &Tamgudecimal::Methodexpm1, P_NONE, "expm1(): call expm1 on the value", a_float);
    Tamgudecimal::AddMethod(global, "floor", &Tamgudecimal::Methodfloor, P_NONE, "floor(): call floor on the value", a_float);
    Tamgudecimal::AddMethod(global, "lgamma", &Tamgudecimal::Methodlgamma, P_NONE, "lgamma(): call lgamma on the value", a_float);
    Tamgudecimal::AddMethod(global, "ln", &Tamgudecimal::Methodln, P_NONE, "ln(): call log on the value", a_float);
    Tamgudecimal::AddMethod(global, "log", &Tamgudecimal::Methodlog, P_NONE, "log(): call log10 on the value", a_float);
    Tamgudecimal::AddMethod(global, "log1p", &Tamgudecimal::Methodlog1p, P_NONE, "log1p(): call log1p on the value", a_float);
    Tamgudecimal::AddMethod(global, "log2", &Tamgudecimal::Methodlog2, P_NONE, "log2(): call log2 on the value", a_float);
    Tamgudecimal::AddMethod(global, "logb", &Tamgudecimal::Methodlogb, P_NONE, "logb(): call logb on the value", a_float);
    Tamgudecimal::AddMethod(global, "nearbyint", &Tamgudecimal::Methodnearbyint, P_NONE, "nearbyint(): call nearbyint on the value", a_float);
    Tamgudecimal::AddMethod(global, "rint", &Tamgudecimal::Methodrint, P_NONE, "rint(): call rint on the value", a_float);
    Tamgudecimal::AddMethod(global, "round", &Tamgudecimal::Methodround, P_NONE, "round(): call round on the value", a_float);
    Tamgudecimal::AddMethod(global, "sin", &Tamgudecimal::Methodsin, P_NONE, "sin(): call sin on the value", a_float);
    Tamgudecimal::AddMethod(global, "sinh", &Tamgudecimal::Methodsinh, P_NONE, "sinh(): call sinh on the value", a_float);
    Tamgudecimal::AddMethod(global, "sqrt", &Tamgudecimal::Methodsqrt, P_NONE, "sqrt(): call sqrt on the value", a_float);
    Tamgudecimal::AddMethod(global, "tan", &Tamgudecimal::Methodtan, P_NONE, "tan(): call tan on the value", a_float);
    Tamgudecimal::AddMethod(global, "tanh", &Tamgudecimal::Methodtanh, P_NONE, "tanh(): call tanh on the value", a_float);
    Tamgudecimal::AddMethod(global, "tgamma", &Tamgudecimal::Methodtgamma, P_NONE, "tgamma(): call tgamma on the value", a_float);
    Tamgudecimal::AddMethod(global, "trunc", &Tamgudecimal::Methodtrunc, P_NONE, "trunc(): call trunc on the value", a_float);
    Tamgudecimal::AddMethod(global, "even", &Tamgudecimal::Methodeven, P_NONE, "even(): return true is the value is even", a_float);
    Tamgudecimal::AddMethod(global, "odd", &Tamgudecimal::Methododd, P_NONE, "odd(): return true is the value is odd", a_float);



    global->newInstance[Tamgudecimal::idtype] = new Tamgudecimal(0, global);
    global->newInstance[a_decimalthrough] = global->newInstance[Tamgudecimal::idtype];
    global->RecordMethods(Tamgudecimal::idtype, Tamgudecimal::exported);
    global->RecordMethods(a_decimalthrough, Tamgudecimal::exported);
    global->RecordMethods(a_dloop, Tamgudecimal::exported);
    
    return true;
}

Tamgu* Tamgudecimal::Methodchr(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamguustring* r = globalTamgu->Provideustring(L"");
    r->value = (wchar_t)value;
    return r;
}


Tamgu* Tamgudecimal::MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

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


Tamgu* Tamgudecimal::MethodRadian(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    float num = value;
    num = M_PI*(num / 180);
    return new Tamgudecimal(num);
}

Tamgu* Tamgudecimal::MethodDegree(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    float num = value;
    num = (num * 180) / M_PI;
    return new Tamgudecimal(num);
}

void TamguLoopDecimal::Callfunction() {

    TamguCallFunction kfunc(function);

    Tamguint* ki = globalTamgu->Provideint(position);
    ki->Setreference();
    kfunc.arguments.push_back(this);
    kfunc.arguments.push_back(ki);
    Tamgu* ke = kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
    ke->Release();
    ki->Resetreference();
}

Tamgu* TamguLoopDecimal::Put(Tamgu* context, Tamgu* ke, short idthread) {
    if (ke->isVectorContainer()) {
        position = 0;
        interval.clear();
        for (int i = 0; i < ke->Size(); i++)
            interval.push_back(ke->getdecimal(i));
        value = interval[0];
        return aTRUE;
    }

    if (ke->Type() == a_dloop) {
        TamguLoopDecimal* kl = (TamguLoopDecimal*)ke;
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


Tamgu* TamguLoopDecimal::Vector(short idthread) {
    Tamgudvector* kvect = new Tamgudvector;
    kvect->values = interval;
    return kvect;
}

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : codeoperations.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamgubool.h"
#include "instructions.h"
#include "tamguustring.h"
#include "tamgudecimal.h"
#include "tamguvector.h"
#include "tamgumap.h"
#include "equationtemplate.h"
#include "comparetemplate.h"


#ifdef UNIX
#define swprintf_s swprintf
#endif

Tamgu* TamguPLUSPLUS::Eval(Tamgu* value, Tamgu* variable, short idthread) {
    if (!globalTamgu->isthreading)
        return variable->plusplus();
    
    value = variable->plusplus();
    globalTamgu->Triggeronfalse(variable);
    return value;
}

Tamgu* TamguMINUSMINUS::Eval(Tamgu* value, Tamgu* variable, short idthread) {
    if (!globalTamgu->isthreading)
        return variable->minusminus();
    
    value = variable->minusminus();
    globalTamgu->Triggeronfalse(variable);
    return value;
}

Tamgu* TamguSQUARE::Eval(Tamgu* value, Tamgu* variable, short idthread) {
    value = variable->multiply(variable, false);
    globalTamgu->Triggeronfalse(variable);
    return value;
}

Tamgu* TamguCUBE::Eval(Tamgu* value, Tamgu* variable, short idthread) {
    value = variable->multiply(variable, false);
    variable = value->multiply(variable, true);
    if (variable != value)
        value->Releasenonconst();
    
    globalTamgu->Triggeronfalse(variable);
    return variable;
}


Tamgu* TamguInstructionAPPLYOPERATIONEQU::Eval(Tamgu* context, Tamgu* value, short idthread) {
    Tamgu* variable = value = recipient->Eval(context, aNULL, idthread);
    
    Tamgu* idx = recipient->Function();
    bool putback = false;
    bool relvar = false;
    
    if (idx != NULL) {
        idx = idx->Getindex();
        if (idx != recipient->Function())
            relvar = true;
        
        if (idx != NULL) {
            value = variable->Eval(context, idx, idthread);
            if (value == aNOELEMENT) {
                if (variable->isValueContainer()) {
                    value = idx->Put(variable, aNULL, idthread);
                    if (value->isError()) {
                        if (relvar)
                            variable->Release();
                        return value;
                    }
                    
                    value = variable->Eval(context, idx, idthread);
                    putback = true;
                }
                else {
                    if (relvar)
                        variable->Release();
                    return globalTamgu->Returnerror("No value at this position in the container", idthread);
                }
            }
            else
                if (variable->isValueContainer())
                    putback = true;
        }
    }
    
    //The position in the expression of our variable is the second from the bottom...
    Tamgu* res = instruction->Eval(value, aNULL, idthread);
    
    Tamgu* v=NULL;
    switch (action) {
        case a_plus:
            v = value->plus(res, true);
            break;
        case a_minus:
            v = value->minus(res, true);
            break;
        case a_multiply:
            v = value->multiply(res, true);
            break;
        case a_divide:
            v = value->divide(res, true);
            if (v->isError()) {
                value->Releasenonconst();
                res->Release();
                return globalTamgu->Errorobject(idthread);
            }
            break;
        case a_power:
            v = value->power(res, true);
            break;
        case a_shiftleft:
            v = value->shiftleft(res, true);
            break;
        case a_shiftright:
            v = value->shiftright(res, true);
            break;
        case a_mod:
            v = value->mod(res, true);
            if (v->isError()) {
                value->Releasenonconst();
                res->Release();
                return globalTamgu->Errorobject(idthread);
            }
            break;
        case a_or:
            v = value->orset(res, true);
            break;
        case a_xor:
            v = value->xorset(res, true);
            break;
        case a_and:
            v = value->andset(res, true);
            break;
        case a_merge:
            v = value->Merging(res);
            break;
        case a_combine:
            v = value->Combine(res);
            break;
        case a_add:
            if (!value->isVectorContainer())
                return globalTamgu->Returnerror("Cannot '::' these two elements", idthread);
            value->Merging(res);
            v = value;
    }
    
    if (v != value) {
        value->Put(aNULL, v, idthread);
        v->Release();
    }
    
    if (putback) {
        idx->Put(variable, value, idthread);
        value->Release();
    }
    
    res->Release();
    
    if (globalTamgu->isthreading)
        globalTamgu->Triggeronfalse(variable);

    if (relvar)
        variable->Release();
    
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);
    
    return aTRUE;
}

Tamgu* TamguInstructionAPPLYEQUSHORT::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    
    short v = value->Short();
    short res = instruction->Getshort(idthread);
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    switch (action) {
        case a_plus:
            v += res;
            break;
        case a_minus:
            v -= res;
            break;
        case a_multiply:
            v *= res;
            break;
        case a_divide:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v /= res;
            break;
        case a_power:
            v = pow((double)v, (double)res);
            break;
        case a_shiftleft:
            v <<= res;
            break;
        case a_shiftright:
            v >>= res;
            break;
        case a_mod:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v %= res;
            break;
        case a_or:
            v |= res;
            break;
        case a_xor:
            v ^= res;
            break;
        case a_and:
            v &= res;
            break;
    }
    value->storevalue(v);
    return aTRUE;
}


Tamgu* TamguInstructionAPPLYEQUINT::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    
    long v = value->Integer();
    long res = instruction->Getinteger(idthread);
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    switch (action) {
        case a_plus:
            v += res;
            break;
        case a_minus:
            v -= res;
            break;
        case a_multiply:
            v *= res;
            break;
        case a_divide:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v /= res;
            break;
        case a_power:
            v = pow((double)v, (double)res);
            break;
        case a_shiftleft:
            v <<= res;
            break;
        case a_shiftright:
            v >>= res;
            break;
        case a_mod:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v %= res;
            break;
        case a_or:
            v |= res;
            break;
        case a_xor:
            v ^= res;
            break;
        case a_and:
            v &= res;
            break;
    }
    value->storevalue(v);
    return aTRUE;
}

Tamgu* TamguInstructionAPPLYEQULONG::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    
    BLONG v = value->Long();
    BLONG res = instruction->Getlong(idthread);
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    switch (action) {
        case a_plus:
            v += res;
            break;
        case a_minus:
            v -= res;
            break;
        case a_multiply:
            v *= res;
            break;
        case a_divide:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v /= res;
            break;
        case a_power:
            v = pow((double)v, (double)res);
            break;
        case a_shiftleft:
            v <<= res;
            break;
        case a_shiftright:
            v >>= res;
            break;
        case a_mod:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v %= res;
            break;
        case a_or:
            v |= res;
            break;
        case a_xor:
            v ^= res;
            break;
        case a_and:
            v &= res;
            break;
    }
    value->storevalue(v);
    return aTRUE;
}


Tamgu* TamguInstructionAPPLYEQUDECIMAL::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    
    float v = value->Decimal();
    float res = instruction->Getdecimal(idthread);
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    switch (action) {
        case a_plus:
            v += res;
            break;
        case a_minus:
            v -= res;
            break;
        case a_multiply:
            v *= res;
            break;
        case a_divide:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v /= res;
            break;
        case a_power:
            v = pow(v, res);
            break;
        case a_shiftleft:
            v = FloatShiftleft(v, res);
            break;
        case a_shiftright:
            v = FloatShiftright(v, res);
            break;
        case a_mod:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v = (int)v % (int)res;
            break;
        case a_or:
            v = FloatOr(v,res);
            break;
        case a_xor:
            v = FloatXor(v,res);
            break;
        case a_and:
            v = FloatAnd(v,res);
            break;
    }
    value->storevalue(v);
    return aTRUE;
}

Tamgu* TamguInstructionAPPLYEQUFLOAT::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    
    double v = value->Float();
    double res = instruction->Getfloat(idthread);
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);
    
    switch (action) {
        case a_plus:
            v += res;
            break;
        case a_minus:
            v -= res;
            break;
        case a_multiply:
            v *= res;
            break;
        case a_divide:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v /= res;
            break;
        case a_power:
            v = pow(v,res);
            break;
        case a_shiftleft:
            v = DoubleShiftleft(v, res);
            break;
        case a_shiftright:
            v = DoubleShiftright(v, res);
            break;
        case a_mod:
            if (res == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            v = (int)v % (int)res;
            break;
        case a_or:
            v = DoubleOr(v,res);
            break;
        case a_xor:
            v = DoubleXor(v,res);
            break;
        case a_and:
            v = DoubleAnd(v,res);
            break;
    }
    
    value->storevalue(v);
    return aTRUE;
}

Tamgu* TamguInstructionAPPLYEQUSTRING::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);

    string v = value->String();
    string res = instruction->Getstring(idthread);
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    switch (action) {
    case a_plus:
        v += res;
        break;
    case a_minus:
            v = StringMinus(v, res);
        break;
    }
    value->Storevalue(v);
    return aTRUE;
}

Tamgu* TamguInstructionAPPLYEQUUSTRING::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);

    wstring v = value->UString();
    wstring res = instruction->Getustring(idthread);
    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);

    switch (action) {
    case a_plus:
        v += res;
        break;
    case a_minus:
            v = StringMinus(v, res);
        break;
    }
    value->Storevalue(v);
    return aTRUE;
}

Tamgu* TamguInstructionEQUShort::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    short val = value->Short();

    //The position in the expression of our variable is the second from the bottom...
    short valres = instruction->Getshort(idthread);

    switch (action) {
        case a_plus:
            val += valres;
            break;
        case a_minus:
            val -= valres;
            break;
        case a_multiply:
            val *= valres;
            break;
        case a_divide:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val /= valres;
            break;
        case a_power:
            val = (short)pow((double)val, (double)valres);
            break;
        case a_shiftleft:
            val <<= valres;
            break;
        case a_shiftright:
            val >>= valres;
            break;
        case a_mod:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val %= valres;
            break;
        case a_or:
            val |= valres;
            break;
        case a_xor:
            val ^= valres;
            break;
        case a_and:
            val &= valres;
            break;
    }
    
    value->storevalue(val);
    return aTRUE;
}

Tamgu* TamguInstructionEQUInteger::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    long val = value->Integer();
    
    //The position in the expression of our variable is the second from the bottom...
    long valres = instruction->Getinteger(idthread);

    switch (action) {
        case a_plus:
            val += valres;
            break;
        case a_minus:
            val -= valres;
            break;
        case a_multiply:
            val *= valres;
            break;
        case a_divide:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val /= valres;
            break;
        case a_power:
            val = (long)pow((double)val, (double)valres);
            break;
        case a_shiftleft:
            val <<= valres;
            break;
        case a_shiftright:
            val >>= valres;
            break;
        case a_mod:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val %= valres;
            break;
        case a_or:
            val |= valres;
            break;
        case a_xor:
            val ^= valres;
            break;
        case a_and:
            val &= valres;
            break;
    }
    
    value->storevalue(val);
    return aTRUE;
}

Tamgu* TamguInstructionEQULong::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    BLONG val = value->Long();
    
    //The position in the expression of our variable is the second from the bottom...
    BLONG valres = instruction->Getlong(idthread);

    switch (action) {
        case a_plus:
            val += valres;
            break;
        case a_minus:
            val -= valres;
            break;
        case a_multiply:
            val *= valres;
            break;
        case a_divide:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val /= valres;
            break;
        case a_power:
            val = (BLONG)pow((double)val, (double)valres);
            break;
        case a_shiftleft:
            val <<= valres;
            break;
        case a_shiftright:
            val >>= valres;
            break;
        case a_mod:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val %= valres;
            break;
        case a_or:
            val |= valres;
            break;
        case a_xor:
            val ^= valres;
            break;
        case a_and:
            val &= valres;
            break;
    }
    
    value->storevalue(val);
    return aTRUE;
}

Tamgu* TamguInstructionEQUDecimal::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    float val = value->Decimal();
    
    //The position in the expression of our variable is the second from the bottom...
    float valres = instruction->Getdecimal(idthread);

    switch (action) {
        case a_plus:
            val += valres;
            break;
        case a_minus:
            val -= valres;
            break;
        case a_multiply:
            val *= valres;
            break;
        case a_divide:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val /= valres;
            break;
        case a_power:
            val =pow(val,valres);
            break;
        case a_shiftleft:
            val = (long)val << (long)valres;
            break;
        case a_shiftright:
            val = (long)val >> (long)valres;
            break;
        case a_mod:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val = (long)val % (long)valres;
            break;
        case a_or:
            val = (long)val || (long)valres;
            break;
        case a_xor:
            val = (long)val ^ (long)valres;
            break;
        case a_and:
            val = (long)val & (long)valres;
            break;
    }
    
    value->storevalue(val);
    return aTRUE;
}

Tamgu* TamguInstructionEQUFloat::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    double val = value->Float();
    
    //The position in the expression of our variable is the second from the bottom...
    double valres = instruction->Getfloat(idthread);

    switch (action) {
        case a_plus:
            val += valres;
            break;
        case a_minus:
            val -= valres;
            break;
        case a_multiply:
            val *= valres;
            break;
        case a_divide:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val /= valres;
            break;
        case a_power:
            val =pow(val,valres);
            break;
        case a_shiftleft:
            val = (BLONG)val << (BLONG)valres;
            break;
        case a_shiftright:
            val = (BLONG)val >> (BLONG)valres;
            break;
        case a_mod:
            if (valres == 0)
                return globalTamgu->Returnerror("Cannot divide by 0", idthread);
            val = (BLONG)val % (BLONG)valres;
            break;
        case a_or:
            val = (BLONG)val || (BLONG)valres;
            break;
        case a_xor:
            val = (BLONG)val ^ (BLONG)valres;
            break;
        case a_and:
            val = (BLONG)val & (BLONG)valres;
            break;
    }
    
    value->storevalue(val);
    return aTRUE;
}

Tamgu* TamguInstructionEQUString::Eval(Tamgu* context, Tamgu* value, short idthread) {
    string val;

    value = recipient->Eval(context, aNULL, idthread);
    value->Setstring(val, idthread);
    
    //The position in the expression of our variable is the second from the bottom...
    if (action == a_plus)
        val += instruction->Getstring(idthread);
    else
        val = StringMinus(val, instruction->Getstring(idthread));

    value->Storevalue(val);
    return aTRUE;
}

Tamgu* TamguInstructionEQUUString::Eval(Tamgu* context, Tamgu* value, short idthread) {
    wstring val;
    value = recipient->Eval(context, aNULL, idthread);
    value->Setstring(val, idthread);

    //The position in the expression of our variable is the second from the bottom...
    if (action == a_plus)
        val += instruction->Getustring(idthread);
    else
        val = StringMinus(val, instruction->Getustring(idthread));

    value->Storevalue(val);
    return aTRUE;
}

uchar Returnequ(short ty, bool top);
Tamgu* DynamicEvaluatetype(uchar thetype, uchar ref, Tamgu* a, short action) {
    uchar t = Returnequ(a->Type(), false);
    Tamgu* ret = NULL;
    
    if (action == a_divide || action == a_power) {
        ret = globalTamgu->Providefloat(a->Float());
        a->Release();
        return ret;
    }

    if ((thetype & b_allnumbers)) {
        if (t & b_allstrings) {
                //return a value according to ref
            switch (ref) {
                case b_short:
                case b_int:
                    ret = globalTamgu->Provideint(a->Integer());
                    break;
                case b_long:
                    ret = new Tamgushort(a->Long());
                    break;
                case b_decimal:
                case b_float:
                    ret = globalTamgu->Providefloat(a->Float());
                    break;
            }
            if (ret != NULL) {
                a->Release();
                return ret;
            }
            return a;
        }
        if (t & b_allnumbers) {
            if (ref <= t)
                return a;
            
            switch (ref) {
                case b_int:
                    ret = globalTamgu->Provideint(a->Integer());
                    break;
                case b_long:
                    ret = new Tamgushort(a->Long());
                    break;
                case b_decimal:
                    ret = new Tamgushort(a->Decimal());
                    break;
                case b_float:
                    ret = globalTamgu->Providefloat(a->Float());
                    break;
            }
            if (ret != NULL) {
                a->Release();
                return ret;
            }
        }
        return a;
    }
    
    if ((thetype & b_allstrings) && (t & b_allnumbers)) {
        if (thetype == a_ustring) {
            ret = globalTamgu->Provideustring(a->UString());
            a->Release();
            return ret;
        }
        ret = globalTamgu->Providestring(a->String());
        a->Release();
        return ret;
    }
    return a;
}

Tamgu* TamguInstructionAPPLYOPERATIONROOT::ccompute(short idthread, uchar top, short& d) {
    Tamgu* a = instructions.vecteur[d];
    bool itself = false;
    
    short act = a->Action();
    
    Tamgu* r = aNULL;
    a = instructions.vecteur[--d];
    switch (a->Action()) {
        case a_const:
            r = a;
            break;
        case a_none:
        case a_variable:
            r = a->Eval(aNULL, aNULL, idthread);
            break;
        default:
            r = ccompute(idthread, false, d);
    }
    
    if (top && thetype != 255)
        r = DynamicEvaluatetype(thetype, head, r, act);
    
    Tamgu* v = aZERO;
    
    //In this case, r is a novel object, which has never been used and can then be used for our own purpose...
    if (r->isProtected())
        itself = true;
    
    while (d > 0) {
        a = instructions.vecteur[--d];
        
        switch (a->Action()) {
            case a_const:
                break;
            case a_none:
                a = a->Eval(aNULL, aNULL, idthread);
                break;
            case a_pipe:
                return r;
            case a_variable:
                a = a->Eval(aNULL, aNULL, idthread);
                break;
            default:
                a = ccompute(idthread, false, d);
        }
        
        if (!r->Checkprecision(a)) {
            v = a->Newvalue(r, idthread);
            r->Release();
            r = v;
            itself = true;
        }
        
        switch (act) {
            case a_plus:
                v = r->plus(a, itself);
                break;
            case a_minus:
                v = r->minus(a, itself);
                break;
            case a_multiply:
                v = r->multiply(a, itself);
                break;
            case a_divide:
                v = r->divide(a, itself);
                if (v->isError()) {
                    r->Release();
                    a->Release();
                    d = DIVIDEDBYZERO;
                    return aRAISEERROR;
                }
                break;
            case a_power:
                v = r->power(a, itself);
                break;
            case a_shiftleft:
                v = r->shiftleft(a, itself);
                break;
            case a_shiftright:
                v = r->shiftright(a, itself);
                break;
            case a_mod:
                v = r->mod(a, itself);
                if (v->isError()) {
                    r->Release();
                    a->Release();
                    d = DIVIDEDBYZERO;
                    return aRAISEERROR;
                }
                break;
            case a_or:
                v = r->orset(a, itself);
                break;
            case a_xor:
                v = r->xorset(a, itself);
                break;
            case a_and:
                v = r->andset(a, itself);
                break;
            case a_merge:
                if (!itself && !r->isProtected())
                    v = r->Atom(true)->Merging(a);
                else
                    v = r->Merging(a);
                break;
            case a_combine:
                if (!itself && !r->isProtected())
                    v = r->Atom(true)->Combine(a);
                else
                    v = r->Combine(a);
                break;
            case a_add:
                if (r->isVectorContainer())
                    v = r->Merging(a);
                else {
                    if (a->isVectorContainer()) {
                        a->Insert(0, r);
                        v = a;
                        a = aNULL;
                    }
                    else {
                        r->Release();
                        a->Release();
                        d = DIVIDEDBYZERO;
                        return globalTamgu->Returnerror("Cannot '::' these two elements", idthread);
                    }
                }
        }
        
        a->Release();
        
        itself = true;
        if (r != v) {
            r->Release();
            r = v;
        }
    }
    
    return r;
}


Tamgu* TamguInstructionFRACTION::cfraction(short idthread, short& d) {
    Tamgu* a = instructions.vecteur[d];
    bool itself = false;
    
    short act = a->Action();
    
    Tamgu* r = aNULL;
    a = instructions.vecteur[--d];
    switch (a->Action()) {
        case a_const:
            r = a;
            break;
        case a_none:
        case a_variable:
            r = a->Eval(aNULL, aNULL, idthread);
            break;
        default:
            r = cfraction(idthread, d);
    }
    
    Tamgu* v = (Tamgu*)r->Fraction();
    
    if (v != r) {
        r->Release();
        r = v;
    }
    
    //In this case, r is a novel object, which has never been used and can then be used for our own purpose...
    if (r->isProtected())
        itself = true;
    
    while (d > 0) {
        a = instructions.vecteur[--d];
        
        switch (a->Action()) {
            case a_const:
                break;
            case a_none:
                a = a->Eval(aNULL, aNULL, idthread);
                break;
            case a_pipe:
                return r;
            case a_variable:
                a = a->Eval(aNULL, aNULL, idthread);
                break;
            default:
                a = cfraction(idthread, d);
        }
        
        v = (Tamgu*)a->Fraction();
        if (v != a) {
            a->Release();
            a = v;
        }
        
        switch (act) {
            case a_plus:
                v = r->plus(a, itself);
                break;
            case a_minus:
                v = r->minus(a, itself);
                break;
            case a_multiply:
                v = r->multiply(a, itself);
                break;
            case a_divide:
                v = r->divide(a, itself);
                if (v->isError()) {
                    r->Release();
                    a->Release();
                    d = DIVIDEDBYZERO;
                    return aRAISEERROR;
                }
                break;
            case a_power:
                v = r->power(a, itself);
                break;
            case a_shiftleft:
                v = r->shiftleft(a, itself);
                break;
            case a_shiftright:
                v = r->shiftright(a, itself);
                break;
            case a_mod:
                v = r->mod(a, itself);
                if (v->isError()) {
                    r->Release();
                    a->Release();
                    d = DIVIDEDBYZERO;
                    return aRAISEERROR;
                }
                break;
        }
        
        a->Release();
        
        itself = true;
        if (r != v) {
            r->Release();
            r = (Tamgu*)v->Fraction();
            if (r != v)
                v->Release();
        }
    }
    
    return r;
}

//--------------------------------------------------------------------------------

Tamgu* TamguInstructionAPPLYOPERATION::Eval(Tamgu* res, Tamgu* inter, short idthread) {
    //This is an expression that escaped the compiler...
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Eval(res, inter, idthread);
}


long TamguInstructionAPPLYOPERATION::Getinteger(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Getinteger(idthread);
}

BLONG TamguInstructionAPPLYOPERATION::Getlong(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Getlong(idthread);
}

short TamguInstructionAPPLYOPERATION::Getshort(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Getshort(idthread);
}

float TamguInstructionAPPLYOPERATION::Getdecimal(short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Getdecimal(idthread);
}

double TamguInstructionAPPLYOPERATION::Getfloat(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Getfloat(idthread);
}

string TamguInstructionAPPLYOPERATION::Getstring(short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Getstring(idthread);
}

wstring TamguInstructionAPPLYOPERATION::Getustring(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    return root->Getustring(idthread);
}

void TamguInstructionAPPLYOPERATION::Setstring(string& v, short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    root->Setstring(v, idthread);
}

void TamguInstructionAPPLYOPERATION::Setstring(wstring& v, short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(NULL);
    
    root->Setstring(v, idthread);
}


//--------------------------------------------------------------------------------

Tamgu* TamguInstructionCOMPARE::Eval(Tamgu* right, Tamgu* res, short idthread) {
    
    Tamgu* left = instructions.vecteur[0]->Eval(right, res, idthread);
    right = instructions.vecteur[1]->Eval(right, res, idthread);
    res = aFALSE;
    
    switch (action) {
        case a_less:
            res = left->less(right);
            break;
        case a_more:
            res = left->more(right);
            break;
        case a_same:
            res = left->same(right);
            break;
        case a_different:
            res = left->different(right);
            break;
        case a_lessequal:
            res = left->lessequal(right);
            break;
        case a_moreequal:
            res = left->moreequal(right);
    }
    
    left->Release();
    right->Release();
    
    if (negation == true) {
        bool vbool = res->Boolean();
        res->Release();
        return booleantamgu[!vbool];
    }
    
    return res;
}


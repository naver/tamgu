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
    
    Tamgu* recipient_variable = recipient->Function();
    bool putback = false;
    
    if (recipient_variable != NULL) {
        Tamgu* idx = recipient_variable->Getindex();
        if (idx != NULL && idx->isIndex()) {
            recipient_variable = idx;
            value = idx->Eval(context, variable, idthread);
            if (value == aNOELEMENT) {
                if (variable->isValueContainer()) {
                    value = idx->Put(variable, aNULL, idthread);
                    if (value->isError())
                        return value;
                    
                    value = idx->Eval(context, variable, idthread);
                    putback = true;
                }
                else
                    return globalTamgu->Returnerror(e_no_value_at, idthread);
            }
            else {
                if (variable->isValueContainer()) {
                    putback = true;
                    value = value->AtomNoConst();
                }
            }
        }
    }
    
    //The position in the expression of our variable is the second from the bottom...
    Tamgu* res = instruction->Eval(value, aNULL, idthread);
    
    Tamgu* v = NULL;
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
        case a_divideinteger:
            v = value->divideinteger(res, true);
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
                return globalTamgu->Returnerror(e_cannot_these_two, idthread);
            value->Merging(res);
            v = value;
    }
    
    if (v != value) {
        if (value->isConst())
            value = v;
        else {
            value->Put(aNULL, v, idthread);
            v->Release();
        }
    }
    
    if (putback) {
        recipient_variable->Put(variable, value, idthread);
        value->Release();
    }
    
    res->Release();
    
    if (globalTamgu->isthreading)
        globalTamgu->Triggeronfalse(variable);

    if (globalTamgu->Error(idthread))
        return globalTamgu->Errorobject(idthread);
    
    return aTRUE;
}

Tamgu* TamguInstructionAPPLYOPERATIONEQUALIAS::Eval(Tamgu* context, Tamgu* value, short idthread) {
    TamguDeclarationLocal* environment = globalTamgu->Providedeclaration(idthread);
    if (globalTamgu->debugmode)
        environment->idinfo = Currentinfo();
    
    //idx is either a TamguIndex or a TamguShape
    //A TamguIndex with an internal interval is evaluated as two different arguments.
    Tamgu* idx = alias->Function();
    
    for (long i = 0; i < idx->Size(); i++) {
        value = idx->Parameter(i)->Eval(context, aNULL, idthread);
        if (value->isError()) {
            environment->Cleaning();
            return value;
        }
        environment->Declaring(alias->body->parameters[i], value);
        globalTamgu->Storevariable(idthread, alias->body->parameters[i],  value);
        value->Setreference();
    }

    globalTamgu->Pushstackraw(environment, idthread);
    //We then apply our function within this environment
    alias->body->lock_assignment();
    body_instruction->Setaffectation(true);
    Tamgu* variable = body_instruction->Eval(environment, aNULL, idthread);
    body_instruction->Setaffectation(false);
    alias->body->unlock_assignment();
    if (variable->isError()) {
        globalTamgu->Popstack(idthread);
        environment->Releasing();
        return variable;
    }

    
    idx = body_instruction->Function();
    bool putback = false;
    
    if (idx != NULL) {
        idx = idx->Getindex();
        if (idx == NULL || !idx->isIndex()) {
            globalTamgu->Popstack(idthread);
            environment->Releasing();
            return globalTamgu->Returnerror(e_cannot_evaluate_this03, idthread);
        }
        
        value = idx->Eval(context, variable, idthread);
        if (value == aNOELEMENT) {
            if (variable->isValueContainer()) {
                value = idx->Put(variable, aNULL, idthread);
                if (value->isError()) {
                    globalTamgu->Popstack(idthread);
                    environment->Releasing();
                    return value;
                }
                
                value = idx->Eval(context, variable, idthread);
                putback = true;
            }
            else {
                globalTamgu->Popstack(idthread);
                environment->Releasing();
                return globalTamgu->Returnerror(e_no_value_at, idthread);
            }
        }
        else
            if (variable->isValueContainer()) {
                putback = true;
                value = value->AtomNoConst();
            }
    }
    
    //The position in the expression of our variable is the second from the bottom...
    Tamgu* res = instruction->Eval(value, aNULL, idthread);
    
    Tamgu* v = NULL;
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
                globalTamgu->Popstack(idthread);
                environment->Releasing();
                return globalTamgu->Errorobject(idthread);
            }
            break;
        case a_divideinteger:
            v = value->divideinteger(res, true);
            if (v->isError()) {
                value->Releasenonconst();
                res->Release();
                globalTamgu->Popstack(idthread);
                environment->Releasing();
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
                globalTamgu->Popstack(idthread);
                environment->Releasing();
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
            if (!value->isVectorContainer()) {
                globalTamgu->Popstack(idthread);
                environment->Releasing();
                return globalTamgu->Returnerror(e_cannot_these_two, idthread);
            }
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

    globalTamgu->Popstack(idthread);
    environment->Releasing();

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
        case a_divideinteger:
            if (res == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
        case a_divideinteger:
            if (res == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
        case a_divideinteger:
            if (res == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            v /= res;
            break;
        case a_divideinteger:
            if (res == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            v /= res;
            v = (long)v;
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            v /= res;
            break;
        case a_divideinteger:
            if (res == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            v /= res;
            v = (long)v;
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
        case a_divideinteger:
            if (valres == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
        case a_divideinteger:
            if (valres == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
        case a_divideinteger:
            if (valres == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            val /= valres;
            break;
        case a_divideinteger:
            if (valres == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            val /= valres;
            val = (long)val;
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            val /= valres;
            break;
        case a_divideinteger:
            if (valres == 0)
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
            val /= valres;
            val = (long)val;
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
                return globalTamgu->Returnerror(e_cannot_divide_by, idthread);
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

Tamgu* TamguInstructionEQUFraction::Eval(Tamgu* context, Tamgu* value, short idthread) {
    value = recipient->Eval(context, aNULL, idthread);
    
    //The position in the expression of our variable is the second from the bottom...
    short d = instruction->Size() - 1;
    Tamgu* val = instruction->Getfraction(context, value, idthread, d, action);
    value->Putvalue(val, idthread);
    val->Release();
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
            case a_divideinteger:
                v = r->divideinteger(a, itself);
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
                        return globalTamgu->Returnerror(e_cannot_these_two, idthread);
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

Tamgu* TamguInstructionFRACTION::Getfraction(Tamgu* r, Tamgu* value, short idthread, short& d, short action) {
    bool itself = false;
    Tamgu* v = aNULL;
    Tamgu* a = instructions.vecteur[d--];
    short act = 0;
    if (d > 0) {
        act = a->Action();
        a = instructions.vecteur[d--];
    }
    
    switch (a->Action()) {
        case a_const:
            r = a;
            break;
        case a_none:
            r = a->Eval(aNULL, aNULL, idthread);
            break;
        case a_pipe:
            return value;
        case a_variable:
            r = a->Eval(aNULL, aNULL, idthread);
            break;
        default:
            r = Getfraction(aNULL, r, idthread, d, a->Action());
    }
    
    v = (Tamgu*)r->Fraction();
    if (v != r) {
        r->Release();
        r = v;
    }

    while (d >= 0) {
        a = instructions.vecteur[d--];
        if (a->Action() == a_pipe)
            break;
        
        switch (a->Action()) {
            case a_const:
                break;
            case a_none:
                a = a->Eval(aNULL, aNULL, idthread);
                break;
            case a_variable:
                a = a->Eval(aNULL, aNULL, idthread);
                break;
            default:
                a = Getfraction(aNULL, r, idthread, d, a->Action());
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
            case a_divideinteger:
                v = r->divideinteger(a, itself);
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
        
        itself = true;
        a->Release();
        if (r != v) {
            r->Release();
            r = (Tamgu*)v->Fraction();
            if (r != v)
                v->Release();
        }
    }
    
    switch (action) {
        case a_plus:
            v = r->plus(value, true);
            break;
        case a_minus:
            v = r->minus(value, true);
            break;
        case a_multiply:
            v = r->multiply(value, true);
            break;
        case a_divide:
            v = r->divide(value, true);
            if (v->isError()) {
                r->Release();
                d = DIVIDEDBYZERO;
                return aRAISEERROR;
            }
            break;
        case a_divideinteger:
            v = r->divideinteger(value, true);
            if (v->isError()) {
                r->Release();
                d = DIVIDEDBYZERO;
                return aRAISEERROR;
            }
            break;
        case a_power:
            v = r->power(value, true);
            break;
        case a_shiftleft:
            v = r->shiftleft(value, true);
            break;
        case a_shiftright:
            v = r->shiftright(value, true);
            break;
        case a_mod:
            v = r->mod(value, true);
            if (v->isError()) {
                r->Release();
                d = DIVIDEDBYZERO;
                return aRAISEERROR;
            }
            break;
    }

    if (r != v) {
        r->Release();
        r = (Tamgu*)v->Fraction();
        if (r != v)
            v->Release();
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
            case a_divideinteger:
                v = r->divideinteger(a, itself);
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
        root = Compile(globalTamgu, NULL);
    
    return root->Eval(res, inter, idthread);
}


long TamguInstructionAPPLYOPERATION::Getinteger(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    return root->Getinteger(idthread);
}

BLONG TamguInstructionAPPLYOPERATION::Getlong(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    return root->Getlong(idthread);
}

short TamguInstructionAPPLYOPERATION::Getshort(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    return root->Getshort(idthread);
}

float TamguInstructionAPPLYOPERATION::Getdecimal(short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    return root->Getdecimal(idthread);
}

double TamguInstructionAPPLYOPERATION::Getfloat(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    return root->Getfloat(idthread);
}

string TamguInstructionAPPLYOPERATION::Getstring(short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    return root->Getstring(idthread);
}

wstring TamguInstructionAPPLYOPERATION::Getustring(short idthread) {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    return root->Getustring(idthread);
}

void TamguInstructionAPPLYOPERATION::Setstring(string& v, short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
    root->Setstring(v, idthread);
}

void TamguInstructionAPPLYOPERATION::Setstring(wstring& v, short idthread)  {
    if (root == NULL)
        //we create it on the fly...
        root = Compile(globalTamgu, NULL);
    
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

//------------------------------------------------------------------------------------------
// Tokenizer Implementation Section
//------------------------------------------------------------------------------------------
template<> void tokenizer_result<u_ustring>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    currentchr = (*buffer)[b_pos++];
    c_pos++;
}

#ifdef WIN32
template<> void tokenizer_result<wstring>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    if (c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], false)) {
        sz_read = 2;
        c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], true);
    }
    c_pos++;
}
#else
template<> void tokenizer_result<wstring>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    currentchr = (*buffer)[b_pos++];
    c_pos++;
}
#endif

template<> void tokenizer_result<u16string>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1;
    if (c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], false)) {
        sz_read = 2;
        c_utf16_to_unicode(currentchr, (*buffer)[b_pos++], true);
    }
    c_pos++;
}

template<> void tokenizer_result<string>::getnext() {
    if (b_pos >= buffer_size) {
        currentchr =  0;
        return;
    }
    
    line += (currentchr == '\n');
    sz_read = 1 + c_utf8_to_unicode(buffer, b_pos, currentchr);
    b_pos += sz_read;
    c_pos++;
}


template<> void tokenizer_result<u_ustring>::store_currentchr() {
    stack_ptr->push_back(u_ustring(1, currentchr));
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}

#ifdef WIN32
template<> void tokenizer_result<wstring>::store_currentchr() {
    uint32_t c;
    wstring w;
    if (c_unicode_to_utf16(c, (uint32_t)currentchr)) {
        w = (wchar_t)(c >> 16);
        w += (wchar_t)(c & 0xFFFF);
    }
    else
        w = (wchar_t)c;

    
    stack_ptr->push_back(w);
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}
#else
template<> void tokenizer_result<wstring>::store_currentchr() {
    stack_ptr->push_back(wstring(1, currentchr));
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}
#endif

template<> void tokenizer_result<std::u16string>::store_currentchr() {
    uint32_t c;
    u16string w;
    if (c_unicode_to_utf16(c, (uint32_t)currentchr)) {
        w = (char16_t)(c >> 16);
        w += (char16_t)(c & 0xFFFF);
    }
    else
        w = (char16_t)c;
        
    
    stack_ptr->push_back(w);
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}

template<> void tokenizer_result<string>::store_currentchr() {
    string w = c_unicode_to_utf8(currentchr);
    stack_ptr->push_back(w);
    if (store_all) {
        stacktype.push_back(0);
        stackln.push_back(line);
        bpos.push_back(b_start);
        cpos.push_back(c_start);
    }
    b_start = b_pos;
    c_start = c_pos;
}


template<> void tokenizer_result<u_ustring>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        stack_ptr->push_back(token);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}

#ifdef WIN32
template<> void tokenizer_result<wstring>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        wstring w;
        s_unicode_to_utf16(w, token);
        stack_ptr->push_back(w);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}
#else
template<> void tokenizer_result<wstring>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        stack_ptr->push_back((wstring&)token);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}
#endif

template<> void tokenizer_result<u16string>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        u16string w;
        s_unicode_to_utf16(w, token);
        stack_ptr->push_back(w);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}

template<> void tokenizer_result<string>::store(int32_t label) {
    if (label != -1) {
        if (check_label(token, label))
            return;
        string w;
        s_unicode_to_utf8(w, token);
        stack_ptr->push_back(w);
        token = U"";
        if (store_all) {
            stacktype.push_back((unsigned char)label);
            stackln.push_back(line);
            bpos.push_back(b_start);
            cpos.push_back(c_start);
        }
    }
}

//------------------------------------------------------------
//Display a rule as an automaton
//------------------------------------------------------------

void tokenizer_automaton::display(tokenizer_node* a, int nbblanks, bool top) {
    static std::set<int16_t> found;
    //We start from initial
    if (top) {
        found.clear();
    }
    //This node was already in our set
    bool ret = false;
    if (found.count(a->idpos))
        ret = true;
    else
        found.insert(a->idpos);
    
    string blanks(nbblanks, ' ');
    cout << blanks;
    
    if (a->check_negation())
        cout << "~";
    if (a->check_skip())
        cout << '-';
    if (a->check_start_fail()) {
        if (a->check_fail())
            cout << "<BE>";
        else
            cout << "<B>";
    }
    else {
        if (a->check_fail())
            cout << "<E>";
    }
    
    if (a->action == act_meta) {
        cout << '%';
    }
    
    if (a->action == act_end) {
        if (a->label < 33)
            cout << '$' << (int)a->label;
        else
            cout << '$' << (uchar)a->label;
    }
    else {
        if (!a->label) {
            if (a->action == act_any)
                cout << "ANY";
            else
                cout << "EPSILON";
        }
        else {
            if (a->label < 33)
                cout << (int)a->label;
            else
                cout << (uchar)a->label;
            if (a->endlabel) {
                if (a->endlabel < 33)
                    cout << "-" << (int)a->endlabel;
                else
                    cout << "-" << (uchar)a->endlabel;
            }
        }
    }
    
    if (ret && a->action != act_end) {
        cout << "_SEE_" << a->idpos << endl;
        return;
    }
    
    cout << '_' << a->idpos << endl;

    //we explore the subarcs to give them a position
    for (long i = 0; i < a->arcs.size(); i++) {
        display(a->arcs[i], nbblanks + 2, false);
    }
    if (top)
        cout << endl;
}
//------------------------------------------------------------
//The Rule Compiler
//------------------------------------------------------------

long tokenizer_automaton::compile() {
    /*
     
     The rules are compiled into tokenizer_node arcs and the results are stored in vectorization_table.
     This table contains: 1611 elements (see table_character_size).
     
     When a character code is < 1611, then the associated rules are stored in vectorization_table at character code location.
     For instance, if a rule starts with "A", then the rule will be stored at position 65 (ASCII code for "A")in vectorization_table.
     
     For rules that cannot be indexed, either because they start with an epsilon or because the initial character is > 1611, the
     rule is then stored at position 0.
     
     We have different potential action associated with an arc
     act_char             -> regular character
     act_meta             -> meta-character (%x)
     act_interval         -> interval between two characters: #A-F
     act_any              -> any character
     act_epsilon          -> an epsilon arc, which has created to handle disjunction or optionalaties
     act_end              -> the termination act, associated with a rule identifier, the returned value when the rule has applied
     
     We also have different flags to handle the node status
     flag_negation (1)        -> negation (~)
     flag_skip (2)            -> Character not stored (-)
     flag_fail (4)            -> Set to Element in negated disjunctions. When element is true, rule fails thank to this flag.
     flag_start_fail (8)      -> Set the initial element of a negated disjunction element. Different from flag_fail only for sequences
     flag_gate (16)           -> When this flag is activated, the path always fails
     flag_action (31)          -> flag_negation | flag_skip | flag_fail | flag_start_fail | flag_gate | flag_lisp
     
     //The next flags are used to mark node utilisation
     flag_vectorized (32)   //has been added to vectorization_table as an index based on the first value of the rule
     flag_added (64)       //has been added to vectorization_table at position 0
     flag_postpone (128)    //While removing epsilon, a node that has already been seen but not fully evaluated since
     flag_visited (256)     //A node that has already been visited
     flag_seen (512)       //A node that has already been seen when processing postponed nodes

     -> IMPORTANT: for meta-characters such as %a or %d, we also use each possible value for these meta-characters as index in vectorization_table.
     */
    
    //The meta-characters for which there is a specific semantics
    static char x_actions[]="CHSacdhnoprs";
    
    lisp_detection = false;
    
    map<u_ustring, u_ustring> metalines;
    vector<tokenizer_node*> brackets;
    vector<char> currentbracket;
    
    u_ustring line;
    u_ustring action_id;
    
    tokenizer_node* anode = NULL;
    tokenizer_node* current = NULL;
    tokenizer_node* root = NULL;
    
    u_uchar e;
    u_uchar cc;
    
    int32_t irule, final_action = -1;
    long pos;
    
    bool metarule_available = false;
    bool error = false;
    long begin_nodes;
    bool first;
    bool first_value;
    
    bool tail = false;
    
    
    for (irule = 0; irule < rules.size() && !error; irule++) {
        tail = false;
        line = rules[irule];
        begin_nodes = nodes.size();
        
        if (line[1]==':') { //we detect a meta-rule...
            cc = line[0];
            line=line.c_str()+2;
            action_id = U"%";
            action_id += cc;
            if (metarule_available)
                replacemetas(metalines,line);
            metalines[action_id] = line;
            metarule_available = true;
            continue;
        }
        
        anode = NULL;
        current = NULL;
        disjunction = false;
        initial = NULL;
        root = NULL;
        //We need a dummy value in currentbracket
        currentbracket.clear();
        currentbracket.push_back('*');
        
        //first we look for the = sign at the end of the string...
        
        pos = line.rfind(U"=",line.size()-1);
        if (pos == -1)
            error = true;
        else {
            // =: forces tail recursion
            if (line[pos + 1] == ':') {
                tail = true;
                pos++;
            }
                
            action_id = line.c_str()+pos+1;
            
            switch (action_id[0]) {
                case '!':
                    final_action = -2;
                    break;
                case '#':
                    final_action = -1;
                    break;
                case 'L': //for Lisp detection mode (£)
                    final_action = def_lisp_mode;
                    lisp_detection = true;
                    break;
                default:
                    final_action = convert(action_id);
            }
            
            if (tail)
                line=line.substr(0,pos - 1);
            else
                line=line.substr(0,pos);
            
            //We have some meta rules
            if (metarule_available)
                replacemetas(metalines,line);
        }
        
        first = true;
        first_value = false;
        for (pos = 0; pos < line.size() && !error; pos++) {
            cc = line[pos];
            if (!currentbracket.size()) {
                error = true;
                break;
            }
            
            switch(cc) {
                case '%':
                    //We record the first rule not starting with a character
                    cc = line[pos+1];
                    
                    if (first) {
                        //Note: current = append(current, anode) might seem redundant
                        //but it is important to call it when in a disjunction, as when
                        //the disjunction is the first element, we need to vectorize
                        //for each element that it contains. In that case, append will push
                        //the value in current and returns again current as the main root.
                        switch (cc) {
                            case 'C': //Upper-case characters
                                anode = node(act_meta, cc);
                                for (e = 'A'; e < table_character_size; e++) {
                                    if (c_is_upper(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'H':
                                anode = node(act_meta, cc);
                                current = append(current, anode);
                                break;
                            case 'S': //table space or CR
                                anode = node(act_meta, cc);
                                vectorization(anode, 9);
                                vectorization(anode, 10);
                                vectorization(anode, 13);
                                vectorization(anode, 32);
                                vectorization(anode, 160);
                                vectorization(anode, 0); //for other non-breaking spaces
                                current = append(current, anode);
                                break;
                            case 'a': //Alphabetical characters
                                anode = node(act_meta, cc);
                                vectorization(anode, '_');
                                for (e = 'A'; e < table_character_size; e++) {
                                    if (c_is_alpha(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'c': //Lower-case characters
                                anode = node(act_meta, cc);
                                for (e = 'a'; e < table_character_size; e++) {
                                    if (c_is_lower(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'd':
                                anode = node(act_meta, cc);
                                for (e = '0'; e <= '9'; e++)
                                    vectorization(anode, e);
                                current = append(current, anode);
                                break;
                            case 'h': //Greek characters
                                anode = node(act_meta, cc);
                                for (e = 913; e <= 987; e++) {
                                    if (c_is_alpha(e))
                                        vectorization(anode, e);
                                }
                                current = append(current, anode);
                                break;
                            case 'n':
                                anode = node(act_meta, cc);
                                vectorization(anode, 160);
                                //there are more than one non breaking space
                                vectorization(anode, 0);
                                current = append(current, anode);
                                break;
                            case 'o': {
                                anode = node(act_meta, cc);
                                for (const auto& a : operators)
                                    vectorization(anode, a);
                                current = append(current, anode);
                                break;
                            }
                            case 'p': {
                                anode = node(act_meta, cc);
                                for (const auto& a: vpunctuations())
                                    vectorization(anode, a);
                                current = append(current, anode);
                                break;
                            }
                            case 'r': //CR
                                anode = node(act_meta, cc);
                                vectorization(anode, 10);
                                vectorization(anode, 13);
                                current = append(current, anode);
                                break;
                            case 's': //tab or space or non-breaking spaces
                                anode = node(act_meta, cc);
                                vectorization(anode, 9);
                                vectorization(anode, 32);
                                vectorization(anode, 160);
                                vectorization(anode, 0); //for other non-breaking space characters
                                current = append(current, anode);
                                break;
                            default:
                                anode = node(act_char, cc);
                                vectorization(anode, cc);
                                current = append(current, anode);
                        }
                        pos++;
                        break;
                    }
                    
                    //This is a direct comparison or a meta
                    if (strchr(x_actions,cc) == NULL)
                        anode = node(act_char, cc);
                    else
                        anode = node(act_meta, cc);
                    
                    root = current;
                    current = append(current, anode);
                    pos++;
                    break;
                case '(':
                    //The first element of a rule can never be optional
                    if (first) {
                        currentbracket.push_back('^');
                        first_value = true;
                    }
                    
                    anode = node(act_epsilon, 0);
                    current = append(current, anode);
                    brackets.push_back(current);
                    currentbracket.push_back('(');
                    disjunction = false;
                    break;
                case ')':
                    if (brackets.size()) {
                        root = brackets.back();
                        brackets.pop_back();
                        anode = node(act_epsilon, 0);
                        root->append(anode);
                        current = append(current, anode);
                        currentbracket.pop_back();
                        first_value = false;
                        if (currentbracket.back() == '^') {
                            currentbracket.pop_back();
                            first_value = true;
                        }
                        disjunction = (currentbracket.back() == '{');
                        if (first_value) {
                            first_value = false;
                            first = true;
                            continue;
                        }
                    }
                    else
                        error = true;
                    break;
                case '[': { //recording a sequence
                    //This is a sequence of elements
                    //If we are in a disjunction, then first_value might be true
                    //if this disjunction starts a rule.
                    //We add a specific marker to return to the right value
                    //once the structure analysis is completed.
                    if (first_value)
                        currentbracket.push_back('^');
                    currentbracket.push_back('[');
                    brackets.push_back(current);
                    current = node(act_epsilon, 0);
                    brackets.push_back(current);
                    disjunction = false;
                    break;
                }
                case ']':
                    if (brackets.size()) {
                        anode = brackets.back();
                        brackets.pop_back();
                        current = brackets.back();
                        brackets.pop_back();
                        if (current->check_negation()) {
                            //We mark the last node as a fail arc
                            current->fail_arcs();
                            anode = node(act_any, 0);
                            current->append(anode);
                        }
                        
                        root = current;
                        current->append(anode->arcs[0]);
                        currentbracket.pop_back();
                        if (currentbracket.back() == '^') {
                            //we were in a disjunction, we need to put first_value back to true
                            currentbracket.pop_back();
                            first_value = true;
                        }
                        disjunction = (currentbracket.back() == '{');
                    }
                    else
                        error = true;
                    break;
                case '{':
                    anode = node(act_epsilon, 0);
                    disjunction = false;
                    current = append(current, anode);
                    brackets.push_back(current);
                    disjunction = true;
                    currentbracket.push_back('{');
                    //In this case, we need to dispatch elements to vectorize
                    if (first)
                        first_value = true;
                    break;
                case '}':
                    if (brackets.size()) {
                        first_value = false;
                        disjunction = false;
                        current = brackets.back();
                        brackets.pop_back();
                        root = current;
                        if (current->check_negation()) {
                            current->fail_arcs();
                            anode = node(act_any, 0);
                            current->append(anode);
                        }
                        
                        cc = line[pos+1];
                        if (cc != '+' && cc != '*') {
                            anode = node(act_epsilon, 0);
                            for (e = 0; e < current->size(); e++) {
                                current->arcs[e]->append_final(anode);
                            }
                            if (current->check_negation())
                                current->append(anode);
                            current = anode;
                        }
                        
                        currentbracket.pop_back();
                        disjunction = (currentbracket.back() == '{');
                    }
                    else
                        error = true;
                    break;
                case '+':
                    if (pos) { //only if it is not the first character
                        //append_final appends first current, then anode
                        //on each final nodes in current (to take into account [...])
                        if (check_flag(current->action, act_epsilon)) {
                            anode = node(act_epsilon, 0);
                            for (e = 0; e < current->size(); e++) {
                                current->arcs[e]->append_final(current, anode);
                            }
                            if (current->arcs[0]->check_start_fail())
                                current->append(anode);
                        }
                        else {
                            anode = node(act_epsilon, 0);
                            current->append(current);
                            current = append(current, anode);
                        }
                        current = anode;
                        break;
                    }
                    else
                        error = true;
                    break;
                case '*':
                    if (pos) { //only if it is not the first character
                        //append_final appends first current, then anode
                        //on each final nodes in current (to take into account [...])
                        //We need to add one more layer of node
                        if (root == NULL) {
                            error = true;
                            break;
                        }
                        
                        if (check_flag(current->action, act_epsilon)) {
                            anode = node(act_epsilon, 0);
                            for (e = 0; e < current->size(); e++) {
                                current->arcs[e]->append_final(current, anode);
                            }
                            if (current->arcs[0]->check_start_fail())
                                current->append(anode);
                            current = anode;
                        }
                        else {
                            anode = node(act_epsilon, 0);
                            current->append(current);
                            current = append(current, anode);
                            current = anode;
                        }
                        //We add an arc to skip the whole structure since *
                        //also means 0 case...
                        root->append(anode);
                        break;
                    }
                    else
                        error = true;
                    break;
                case '#': //introduce an interval or a code
                    //interval
                    if (line[pos+2] == '-') {
                        anode = node(act_interval, line[pos+1], line[pos+3]);
                        if (first) {
                            for (u_uchar c = line[pos+1]; c <= line[pos+3]; c++) {
                                vectorization(anode, c);
                            }
                        }
                        root = current;
                        current = append(current, anode);
                        pos += 3;
                    }
                    else {//code
                        e = 0;
                        if (isdigit(line[pos+1])) {
                            pos++;
                            e = line[pos++] - 48;
                            while (pos < line.size() && isdigit(line[pos])) {
                                e *= 10;
                                e += line[pos++] - 48;
                            }
                            pos--;
                            anode = node(act_char, e);
                            if (first)
                                vectorization(anode, e);
                            root = current;
                            current = append(current, anode);
                        }
                        else
                            error = true;
                    }
                    break;
                case '-':
                    if (pos)
                        add_flag(current->flags, flag_skip);
                    else
                        error = true;
                    break;
                case '~':
                    negation = flag_negation;
                    break;
                case '?':
                    anode = node(act_any, cc);
                    root = current;
                    current = append(current, anode);
                    break;
                case ' ': //in disjunction we separate each element with a space
                    if (currentbracket.back() == '{')
                        continue;
                default: {
                    //Elements are separated with blanks in disjunction
                    anode = node(act_char, cc);
                    if (first)
                        vectorization(anode , cc);
                    root = current;
                    current = append(current, anode);
                }
            }
            first = first_value;
            //In this case, we need to index on the first element
            //not the following one...
            if (first_value && (currentbracket.back() == '[' || currentbracket.back() == '('))
                first_value = false;
        }
        
        if (initial == NULL || error || brackets.size())
            return irule;
        
        if (final_action == -2) {
            final_action = 0;
            if (action_id.size() > 1) {
                action_id = action_id.substr(1, action_id.size());
                final_action = convert(action_id);
            }
            add_flag(initial->flags, flag_blocking_gate);
            //This is a gated mechanism. When a rule applies that returns L (see above)
            //Then the Lisp quote reading is activated. Since, this is now the first rule for quotes
            //then quotes will be handled in the Lisp way. We use select_lisp_mode to activate or
            //deactivate the Lisp quote rule, by adding or removing the flag_gate flag.
            if (lisp_detection && initial->label == '\'' && vectorization_table[39]->arcs[0] == initial)
                lisp_quote = initial;
        }
            
        anode = node(act_end, final_action);
        append(current, anode);
                    
        initial->trim_epsilon(nodes);
        if (final_action >= 0)
            indexed_on_label[final_action].push_back(initial);

        if (tail) {
            anode = initial;
            while (anode->arcs.size() == 1)
                anode = anode->arcs[0];
            add_flag(anode->flags, flag_tail);
        }
    }
    
    //Cleaning useless nodes
    //Removing all marks except flag_negation, flag_skip and flag_fail
    for (auto& nd : nodes)
        nd->flags &= flag_action;
    
    //When there is only one rule in vectorization_table, no need to keep the epsilon
    vectorization_table[0]->mark_nodes();
    
    //the position 0 can never be removed, thus we start at 1
    for (e = 1; e < table_character_size; e++) {
        if (vectorization_table[e] != NULL) {
            if (vectorization_table[e]->size() == 1)
                vectorization_table[e] = vectorization_table[e]->arcs[0];
            vectorization_table[e]->mark_nodes();
        }
    }
    
    vector<tokenizer_node*> intermediary;
    for (auto& nd : nodes) {
        if (!nd->check_visited())
            delete nd;
        else {
            nd->idpos = intermediary.size();
            intermediary.push_back(nd);
        }
    }
    
    nodes = intermediary;
    loaded = true;
    return -1;
}

//------------------------------------------------------------
//Cleaning epsilon nodes in tokenize automata
//------------------------------------------------------------

//These nodes have been detected as sub-nodes of themselves. Basically, it is a loop generated out of a Kleene operator (a loop)
void tokenizer_node::processing_postponed_nodes(std::set<long>& visited, vector<tokenizer_node*>& nodes, long idpos) {
    if (check_seen())
        return;
    
    add_flag(flags, flag_seen);
    
    long found = -1;
    for (long i = 0; i < arcs.size(); i++) {
        if (arcs[i]->idpos == idpos) {
            found = i;
        }
        else
            arcs[i]->processing_postponed_nodes(visited, nodes, idpos);
    }

    if (found != -1) {
        vector<tokenizer_node*> local;
        std::set<long> v;
        v.insert(idpos);
        for (long i = 0; i < arcs.size(); i++) {
            if (i == found) {
                for (auto& e : visited) {
                    if (v.find(e) != v.end())
                        continue;
                    local.push_back(nodes[e]);
                    v.insert(e);
                }
            }
            else {
                if (v.find(arcs[i]->idpos) == v.end()) {
                    local.push_back(arcs[i]);
                    v.insert(arcs[i]->idpos);
                }
            }
        }
        arcs = local;
    }
}

//We are looking for epsilon nodes that can be merged...
//When we find an epsilon nodes, we go down in recursion to gather all existing nodes that are not epsilon down the way
//We then replace these epsilon nodes with these nodes.
//When we have an epsilon node that points to a termination node, this epsilon node becomes the new termination node.
void tokenizer_node::remove_epsilon_nodes(std::unordered_map<long,std::set<long> >& visited,
                                          std::set<long>& current,
                                          vector<tokenizer_node*>& nodes, bool epsilon) {
    if (check_visited())
        return;

    add_flag(flags, flag_visited);
    
    std::set<long> locals;
    std::set<long>* vect;
    tokenizer_node* n;
    long i;
    
    long sz = size();
    
    if (sz) {
        //We merge the arcs that share the same definition
        long key;
        std::unordered_map<long, tokenizer_node*> commons;
        vector<tokenizer_node*> nds;
        for (i = 0; i < sz; i++) {
            n = arcs[i];
            if (n->pure_arc()) {
                key = n->action*256 + n->label;
                if (commons.find(key) != commons.end()) {
                    tokenizer_node* nbase = commons[key];
                    for (key = 0; key < n->size(); key++)
                        nbase->arcs.push_back(n->arcs[key]);
                }
                else {
                    commons[key] = n;
                    nds.push_back(n);
                }
            }
            else
                nds.push_back(n);
        }
        //We have found some common arcs
        if (nds.size() != sz) {
            arcs.clear();
            arcs = nds;
            sz = arcs.size();
        }
    }

    if (epsilon)
        vect = &current;
    else
        vect = &locals;
    
    for (i = 0; i < sz; i++) {
        n = arcs[i];
        if (n->is_epsilon()) {
            if (n->size() == 1 && n->arcs[0]->action == act_end) {
                add_flag(n->arcs[0]->flags, flag_visited);
                n->clone(n->arcs[0]);
                n->arcs.clear();
                vect->insert(n->idpos);
            }
            else {
                if (n->check_visited()) {
                    if (visited.find(n->idpos) != visited.end()) {
                        for (auto& e: visited[n->idpos])
                            vect->insert(e);
                    }
                    else {
                        add_flag(n->flags, flag_postpone);
                        vect->insert(n->idpos);
                    }
                }
                else {
                    n->remove_epsilon_nodes(visited, *vect, nodes, true);
                    visited[n->idpos] = *vect;
                    if (n->check_postpone() && visited.find(n->idpos) != visited.end())
                        n->processing_postponed_nodes(visited[n->idpos], nodes, n->idpos);
                }
            }
        }
        else {
            vect->insert(n->idpos);
            n->remove_epsilon_nodes(visited, *vect, nodes, false);
        }
    }
    
    if (locals.size() != 0) {
        arcs.clear();
        for (auto& e : locals) {
            n = nodes[e];
            if (n->pure_arc())
                arcs.push_back(n);
        }
        
        for (auto& e : locals) {
            n = nodes[e];
            if (!n->pure_arc())
                arcs.push_back(n);
        }
        
        for (auto& e : locals) {
            n = nodes[e];
            if (n->check_postpone() && visited.find(n->idpos) != visited.end())
                processing_postponed_nodes(visited[n->idpos], nodes, n->idpos);
        }
    }
}


void tokenizer_automaton::setrules() {
    /*
     a) A metarule is composed of two parts: c:expression, where c is the metacharacter that be accessed through %c and expression is a single body rule.
     
     for instance, we could have encoded %o as:
     rules.push_back(U"o:[≠ ∨ ∧ ÷ × ² ³ ¬]");
     
     IMPORTANT: These rules should be declared with one single operation.
     Their body will replace the call to a %c in other rules  (see the test on metas in the parse section)
     
     If you use a character that is already a meta-character (such as "a" or "d"), then the meta-character will be replaced with
     this new description... However, its content might still use the standard declaration:
     
     rules.push_back(U"a:{%a %d %p}"); "%1 is now a combination of alphabetical characters, digits and punctuations
     
     
     b) A rule is composed of two parts: body = action (action is either an integer or a #.)
     
     body uses the following instructions:
     
     x   is a character that should be recognized
     
     #x     comparison with character of code x...
     #x-y   comparison between x and y. x and y should be ascii characters...
     
     %x  is a meta-character with the following possibilities:
     
     ?  is any character
     %a  is any alphabetical character (including unicode ones such as éè)
     %C  is any uppercase character
     %c  is any lowercase character
     %d  is any digits
     %h  is a Greek letter
     %H  is any hangul character
     %n  is a non-breaking space
     %o  is any operators
     %p  is any punctuations
     %r  is a carriage return both \n and \r
     %s  is a space (32) or a tab (09)
     %S  is both a carriage return or a space (%s or %r)
     
     %nn  you can create new metarules associated with any OTHER characters...
     
     
     (..) is a sequence of optional instructions
     [..] is a sequence of characters in a disjunction
     {..} is a disjunction of meta-characters
     x*   means that the instruction can be repeated zero or n times
     x+   means that the instruction can be repeated at least once
     x-   means that the character should be recognized but not stored in the parsing string
     ~..  means that all character will be recognized except for those in the list after the tilda.
     
     IMPORTANT: spaces are considered characters except in disjunctions
     
     
     Final Action
     ------------
     When a rule applies it returns its final action value, which is usually a numerical value.
     However, there are some special values:
     
     £ : These are characters that trigger the Lisp mode. This is a case when quotes do not encapsulate strings anymore
     !value : Use to add gate keeping in the automatin. For instance, the rule for single quotes is activated.
     #: The rule applies but nothing is stored in the stacks
     
     */
    
    //Spaces, skipped in the parsing string
    rules.push_back(U"%S+=#");                         //0     space (not kept)
    
    
    rules.push_back(U"\\%(=L");  //detecting Lisp Expression
    rules.push_back(U"\\'%(=L");

    //Fast tracks for recurrent punctations
    rules.push_back(U";=0");                         //4     ;
    rules.push_back(U",=0");                         //5     ,
    rules.push_back(U"==0");                         //6     =
    rules.push_back(U"!=0");                         //8     !

    //Special characters that need to be escaped
    rules.push_back(U"%(=0");                         //9     (
    rules.push_back(U"%)=0");                         //10    )
    rules.push_back(U"%[=0");                         //11    [
    rules.push_back(U"%]=0");                         //12    ]
    rules.push_back(U"%{=0");                         //13    {
    rules.push_back(U"%}=0");                         //14    }
    rules.push_back(U"%%=0");                         //21    %
    rules.push_back(U"%?=0");                         //29    ?
    rules.push_back(U"%+=0");                         //18    +
    rules.push_back(U"%-=0");                         //19    -
    rules.push_back(U"%*=0");                         //20    *
    rules.push_back(U"%~=0");                         //7     ~
    rules.push_back(U"%#=0");                         //28    #

    rules.push_back(U"..=0");                        //15    ..
    rules.push_back(U".=0");                         //16    .
    rules.push_back(U"^=0");                         //17    ^
    rules.push_back(U"<=0");                         //22    <
    rules.push_back(U">=0");                         //23    >
    rules.push_back(U"|=0");                         //24    |
    rules.push_back(U"&=0");                         //25    &
    rules.push_back(U":=0");                         //26    :
    rules.push_back(U"$=0");                         //27    $
    rules.push_back(U"\\=0");                        //29    ?
    rules.push_back(U"_%#=4");                       //a special token

    //Comments
    rules.push_back(U"//?*%r=#");                  //30    comments starting with // with no carriage return (CR) inside (not kept)
    rules.push_back(U"/@?*@/=#");                   //32    long comments starting with /@ and finishing with @/ (not kept)
    rules.push_back(U"/=0");                         //34    /
    
    //Strings
    //Double quote
    rules.push_back(U"\"{[\\-\"] ~%r}*\"=:1");     //string "" does not contain CR and can escape characters

    //Single quote
    rules.push_back(U"'=!");                         //4 In the case of Lisp, this rule is activated
    rules.push_back(U"'~%r*'=:2");                    //38    string '' does not contain CR and does not process escape characters
    
    //Long quotes
    rules.push_back(U"@-\"?*\"@-=5");               //40    string @" "@ can contain CR and escape characters (we do not keep the @s)
    
    //tamgu regular expression strings
    rules.push_back(U"r-\"~%r+\"=9");              //42    string r"" tamgu regular expression (we do not keep the r in the parse)
    rules.push_back(U"r-'~%r+'=10");               //42    string r"" tamgu regular expression (we do not keep the r in the parse)
    rules.push_back(U"p-\"~%r+\"=11");             //42    string p"" tamgu posix expression (we do not keep the p in the parse)
    rules.push_back(U"p-'~%r+'=12");               //42    string p"" tamgu posix expression (we do not keep the p in the parse)
    rules.push_back(U"f-\"~%r+\"=13");              //42    string f"" tamgu format expression
    rules.push_back(U"f-'~%r+'=14");              //42    string f'' tamgu format expression

    //Unicode double quote strings
    rules.push_back(U"u-\"{[\\-\"] ~%r}*\"=1");     //string "" does not contain CR and can escape characters
    
    //Unicode single quote strings
    rules.push_back(U"u-'~%r*'=7");                //44    string u'' unicode string
    
    //Unicode long quote strings
    rules.push_back(U"u-@-\"?+\"@-=8");             //46    string u@".."@ unicode string
    
    rules.push_back(U"0b{1 0}+=3");                       //binary numbers
    rules.push_back(U"0c({%- %+})%d+(.%d+)({eE}({%- %+})%d+):({%- %+})(%d+(.%d+)({eE}({%- %+})%d+))i=3");    //complex numbers

    rules.push_back(U"1:{%d #A-F #a-f}");            //2     metarule on 1, for hexadecimal digits
    rules.push_back(U"0x%1+(.%1+)({pP}({%- %+})%d+)=3");  //47 hexadecimal: can handle 0x1.16bca4f9165dep-3
    
    rules.push_back(U"%d+(.%d+)({eE}({%- %+})%d+)=3");    //48    exponential digits

    // Rules start here
    //This character should be interpreted as one
    rules.push_back(U"{%a %d %H}{%a %d %H %#}*=4");               //49    label a combination of alpha, digits and hangul characters
    rules.push_back(U"%n=#");                        //1     non-breaking space (not kept)
    rules.push_back(U"%o=0");                        //51    operators
    rules.push_back(U"%p=0");                        //50    punctuation
    rules.push_back(U"~{%S %p %o}+=4");            //52    An unknown UTF8 token separated with spaces, punctuation or operators...
}


void segmenter_automaton::setrules() {
    rules.push_back(U"#10=!99");                                  //gated arcs to keep carriage or not
    rules.push_back(U"%S+=#");                                  //we skip all spaces
    

    //regular numbers
    rules.push_back(U"%d+{[rd][th][nd][er][ième][ieme]}=1");    //3rd or 4th
    rules.push_back(U"%d(%d)(:%d%d){[am][pm]}=1");              //American hours 6:30am, 9pm
    rules.push_back(U"%d(%d){:h}%d%d=1");                       //European hours 6h30, 21:00

    rules.push_back(U"{%- %+}0b{1 0}+=1");  //binaries
    rules.push_back(U"0b{1 0}+=1");  //binaires
    rules.push_back(U"0c({%- %+})%d+(.%d+)({eE}({%- %+})%d+):({%- %+})(%d+(.%d+)({eE}({%- %+})%d+))i=1");    //complex numbers

    //Tokenizing numbers
    //22 for blocs of 3, with a comma
    //44 for blocs of 3, with a point or a space
    //rules are either 22 or 44 never both
    //33 using a decimal point
    //66 using a decimal comma
    //rules are either 33 or 66 never both
    //The order of the rules is important, if blocs of 3 are to be taken into account
    rules.push_back(U"{%- %+}%d+[,%d%d%d]+=22");          //digits separated with a , by block of 3
    rules.push_back(U"{%- %+}%d+[{.#32}%d%d%d]+=!44");    //digits separated with a . or a space by block of 3
    rules.push_back(U"%d+[,%d%d%d]+=22");  //digits separated with a , by block of 3
    rules.push_back(U"%d+[{.#32}%d%d%d]+=!44"); //digits separated with a . or a space by block of 3

    //We put this meta-rule here to avoid checking all the rules belows with it
    rules.push_back(U"1:{%d #A-F #a-f}");                    //metarule on 1, for hexadecimal digits

    rules.push_back(U"{%- %+}0x%1+(.%1+({pP}({%- %+})%d+))=33");  //hexadecimal with a decimal point
    rules.push_back(U"{%- %+}0x%1+(,%1+({pP}({%- %+})%d+))=!66"); //Gated: hexadecimal with a decimal comma
    rules.push_back(U"{%- %+}%d+(.%d+({eE}({%- %+})%d+))=33");    //Numbers with decimal point
    rules.push_back(U"{%- %+}%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated: Numbers with decimal comma

    rules.push_back(U"0x%1+(.%1+({p P}({%- %+})%d+))=33");   //hexadecimal
    rules.push_back(U"0x%1+(,%1+({p P}({%- %+})%d+))=!66"); //Gated rules
    rules.push_back(U"%d+(.%d+({eE}({%- %+})%d+))=33");      //digits
    rules.push_back(U"%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated rules

    rules.push_back(U"${%a %d}+=1");       //Regular strings
    rules.push_back(U"%#{%a %d}+=1");       //Regular strings
    rules.push_back(U"${%a %d}+=1");       //Regular strings
    
    rules.push_back(U"http(s)://{%a %d . = %# & %? / %- %+}+=1");       //http
    rules.push_back(U"{%a %d . %- %+}+@{%a %d . = & %? %# %- %+}+=1");       //mail address
        
    rules.push_back(U"%o=1");                 //operator
    rules.push_back(U"%p=1");                  //punctuation
    rules.push_back(U"%H{%H %d}*=1");       //Asian characters (Chinese, Korean, Japanese)
    rules.push_back(U"%h{%h %- %d}*=1");       //Greek
    rules.push_back(U"%a{%a %- '’ %d}*=1");       //Regular strings
    rules.push_back(U"~{%S %o %p}+=1");           //Any combination of Unicode characters ending at a space or a punctuation
}

void tags_automaton::setrules() {
    rules.push_back(U"#10=!99");                                  //gated arcs to keep carriage or not
    rules.push_back(U"%S+=#");                                  //we skip all spaces
    
    //regular numbers
    rules.push_back(U"%d+{[rd][th][nd][er][ième][ieme]}=4");    //3rd or 4th
    rules.push_back(U"%d(%d)(:%d%d){[am][pm]}=4");              //American hours 6:30am, 9pm
    rules.push_back(U"%d(%d){:h}%d%d=4");                       //European hours 6h30, 21:00
    
    //Double quote
    rules.push_back(U"\"{[\\-\"] ~%r}*\"=:1");     //string "" does not contain CR and can escape characters

    //Single quote
    rules.push_back(U"'=!");                         //4 In the case of Lisp, this rule is activated
    rules.push_back(U"'~%r*'=:2");                    //38    string '' does not contain CR and does not process escape characters

    
    //We put this meta-rule here to avoid checking all the rules belows with it
    rules.push_back(U"1:{%d #A-F #a-f}");                    //metarule on 1, for hexadecimal digits
    
    //Tokenizing numbers
    //22 for blocs of 3, with a comma
    //44 for blocs of 3, with a point or a space
    //rules are either 22 or 44 never both
    //33 using a decimal point
    //66 using a decimal comma
    //rules are either 33 or 66 never both
    //The order of the rules is important, if blocs of 3 are to be taken into account
    rules.push_back(U"{%- %+}%d+[,%d%d%d]+=22");          //digits separated with a , by block of 3
    rules.push_back(U"{%- %+}%d+[{.#32}%d%d%d]+=!44");    //digits separated with a . or a space by block of 3
    rules.push_back(U"%d+[,%d%d%d]+=22");  //digits separated with a , by block of 3
    rules.push_back(U"%d+[{.#32}%d%d%d]+=!44"); //digits separated with a . or a space by block of 3

    rules.push_back(U"{%- %+}0x%1+(.%1+({pP}({%- %+})%d+))=33");  //hexadecimal with a decimal point
    rules.push_back(U"{%- %+}0x%1+(,%1+({pP}({%- %+})%d+))=!66"); //Gated: hexadecimal with a decimal comma
    rules.push_back(U"{%- %+}%d+(.%d+({eE}({%- %+})%d+))=33");    //Numbers with decimal point
    rules.push_back(U"{%- %+}%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated: Numbers with decimal comma

    rules.push_back(U"0x%1+(.%1+({p P}({%- %+})%d+))=33");   //hexadecimal
    rules.push_back(U"0x%1+(,%1+({p P}({%- %+})%d+))=!66"); //Gated rules
    rules.push_back(U"%d+(.%d+({eE}({%- %+})%d+))=33");      //digits
    rules.push_back(U"%d+(,%d+({eE}({%- %+})%d+))=!66"); //Gated rules

    rules.push_back(U"{%- %+}0b{1 0}+=4");  //binaries
    rules.push_back(U"0b{1 0}+=4");  //binaires
    rules.push_back(U"0c({%- %+})%d+(.%d+)({eE}({%- %+})%d+):({%- %+})(%d+(.%d+)({eE}({%- %+})%d+))i=4");    //complex numbers

    rules.push_back(U"%#{%a %d}+=4");       //Regular strings
    rules.push_back(U"${%a %d}+=4");       //Regular strings
    
    rules.push_back(U"http(s)://{%a %d . = %# & %? / %- %+}+=4");       //http
    rules.push_back(U"{%a %d . %- %+}+@{%a %d . = & %? %# %- %+}+=4");       //mail address
    
    rules.push_back(U"%o=4");                 //operator
    rules.push_back(U"%p=4");                  //punctuation
    rules.push_back(U"%H{%H %d}*=4");       //Asian characters (Chinese, Korean, Japanese)
    rules.push_back(U"%h{%h %- %d}*=4");       //Greek
    rules.push_back(U"%a{%a %- '’ %d}*=4");       //Regular strings
    rules.push_back(U"~{%S %o %p}+=4");           //Any combination of Unicode characters ending at a space or a punctuation
}

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : equationtemplate.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef equationtemplate_h
#define equationtemplate_h

#include "tamgu.h"

//The e_operation objects are a fast way to compute values that are either constant or a single variable, as analyzed by "parse(TamguGlobal* g"
//In the other cases, we use the c"type" function, such as cinteger, cshort etc...
class e_operation : public Tamgu {
public:

    long size;
    Tamgu** values;
    
    e_operation(TamguGlobal* g)  {
        if (g != NULL)
            g->RecordInTracker(this);
        values=NULL;
    }
    
    //We have implemented this method in order to keep the size of values quite small...
    virtual e_operation* update(vector<Tamgu*>& v, short ty) {
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
        return this;
    }

    e_operation* simpleupdate(vector<Tamgu*>& v) {
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
        return this;
    }
    

    virtual string String() {
        string s="(";
        string sym=globalTamgu->Getsymbol(Action());
        
        for (long i=0; i<size;i++) {
            if (i)
                s+=sym;
            s+=values[i]->String();
        }
        s+=")";
        return s;
    }
    
    virtual void Setstring(string& res, short idthread) {
        values[0]->Setstring(res, idthread);
        string u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res += u;
        }
    }
    
    virtual void Setstring(wstring& res, short idthread) {
        values[0]->Setstring(res, idthread);
        wstring u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res += u;
        }
    }
    
    virtual string Getstring(short idthread) {
        string res=values[0]->Getstring(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getstring(idthread);
        return res;
    }
    
    virtual wstring Getustring(short idthread) {
        wstring res=values[0]->Getustring(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getustring(idthread);
        return res;
    }

    void Remove() {
        globalTamgu->RemoveFromTracker(idtracker);
        delete this;
    }

    ~e_operation() {
        if (values != NULL)
            delete[] values;
    }
};


class e_value : public e_operation {
public:
    Tamgu* val;
    
    e_value(TamguGlobal* g, Tamgu* v) : e_operation(g) {
        val = v;
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* t, short idthread) {
        return val->Eval(c,t,idthread);
    }
    
    string String() {
        return val->String();
    }
    
    void Setstring(string& v, short idthread) {
        val->Setstring(v, idthread);
    }
    
    void Setstring(wstring& v, short idthread) {
        val->Setstring(v, idthread);
    }
    
    string Bytes() {
        return val->Bytes();
    }
    
    wstring UString() {
        return val->UString();
    }
    
    string StringToDisplay() {
        return val->StringToDisplay();
    }
    
    string JSonString() {
        return val->JSonString();
    }
    
    long Integer() { return val->Integer(); }
    short Short() { return val->Short(); }
    double Float() { return val->Float(); }
    
    float Decimal() {
        return val->Decimal();
    }
    
    bool Boolean() { return val->Boolean(); }
    
    BLONG Long() { return val->Long(); }
    unsigned char Byte() {
        return val->Byte();
    }
    
    long Getinteger(short idthread) {
        return val->Getinteger(idthread);
    }
    
    short Getshort(short idthread) {
        return val->Getshort(idthread);
    }
    
    double Getfloat(short idthread) {
        return val->Getfloat(idthread);
    }
    
    float Getdecimal(short idthread) {
        return val->Getdecimal(idthread);
    }
    
    
    BLONG Getlong(short idthread) {
        return val->Getlong(idthread);
    }
    
    string Getstring(short idthread) {
        return val->Getstring(idthread);
    }
    
    wstring Getustring(short idthread) {
        return val->Getustring(idthread);
    }

};

class e_const_value : public e_operation {
public:
    Tamgu* val;
    
    e_const_value(TamguGlobal* g, Tamgu* v) : e_operation(g) {
        val = v;
    }
    
    Tamgu* Eval(Tamgu* c, Tamgu* t, short idthread) {
        return val;
    }
    
    string String() {
        return val->String();
    }
  
    void Setstring(string& v, short idthread) {
        val->Setstring(v, idthread);
    }
    
    void Setstring(wstring& v, short idthread) {
        val->Setstring(v, idthread);
    }
    
    string Bytes() {
        return val->Bytes();
    }
    
    wstring UString() {
        return val->UString();
    }
    
    string StringToDisplay() {
        return val->StringToDisplay();
    }
    
    string JSonString() {
        return val->JSonString();
    }
    
    long Integer() { return val->Integer(); }
    short Short() { return val->Short(); }
    double Float() { return val->Float(); }
    
    float Decimal() {
        return val->Decimal();
    }
    
    bool Boolean() { return val->Boolean(); }
    
    BLONG Long() { return val->Long(); }
    unsigned char Byte() {
        return val->Byte();
    }
    
    long Getinteger(short idthread) {
        return val->Integer();
    }
    
    short Getshort(short idthread) {
        return val->Short();
    }
    
    double Getfloat(short idthread) {
        return val->Float();
    }
    
    float Getdecimal(short idthread) {
        return val->Decimal();
    }
    
    
    BLONG Getlong(short idthread) {
        return val->Long();
    }
    
    string Getstring(short idthread) {
        return val->String();
    }
    
    wstring Getustring(short idthread) {
        return val->UString();
    }
    
};


class e_plus_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_plus_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long() + v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)+v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)+v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)+v);
    }
    
    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)+v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)+v);
    }
};

class e_plus_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_plus_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = res->Float() + v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->ProvideConstfloat(r);
    }
    
    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)+v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)+v);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)+v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)+v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)+v);
    }
};

class e_plus_string2 : public e_operation  {
public:
    string v;
    wstring w;
    Tamgu* val;
    
    e_plus_string2(TamguGlobal* g, string s, Tamgu* t) : e_operation(g) {
        v = s;
        val = t;
        s_utf8_to_unicode(w, USTR(v), v.size());
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        string r = res->String() + v;
        if (res->isProtected()) {
            res->Storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providewithstring(r);
    }

    short Action() {
        return a_plus;
    }
    
    string Getstring(short idthread) {
        return (val->Getstring(idthread)+v);
    }
    
    wstring Getustring(short idthread) {
        return (val->Getustring(idthread)+w);
    }
    
    void Setstring(string& s, short idthread) {
        val->Setstring(s, idthread);
        s += v;
    }
    
    void Setstring(wstring& s, short idthread) {
        val->Setstring(s, idthread);
        s += w;
    }
    
};

class e_b_plus_string2 : public e_operation  {
public:
    string v;
    wstring w;
    Tamgu* val;
    
    e_b_plus_string2(TamguGlobal* g, string s, Tamgu* t) : e_operation(g) {
        v = s;
        val = t;
        s_utf8_to_unicode(w, USTR(v), v.size());
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        string r = v;
        r += res->String();
        if (res->isProtected()) {
            res->Storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providewithstring(r);
    }

    string Getstring(short idthread) {
        return (v+val->Getstring(idthread));
    }
    
    wstring Getustring(short idthread) {
        return (w+val->Getustring(idthread));
    }
    
    void Setstring(string& s, short idthread) {
        val->Setstring(s, idthread);
        s = v + s;
    }
    
    void Setstring(wstring& s, short idthread) {
        val->Setstring(s, idthread);
        s = w + s;
    }
    

};

class e_plus_wstring2 : public e_operation  {
public:
    string v;
    wstring w;
    Tamgu* val;
    
    e_plus_wstring2(TamguGlobal* g, wstring s, Tamgu* t) : e_operation(g) {
        w = s;
        val = t;
        s_unicode_to_utf8(v, w);
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        wstring r = res->UString() + w;
        if (res->isProtected()) {
            res->Storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providewithustring(r);
    }

    string Getstring(short idthread) {
        return (val->Getstring(idthread)+v);
    }
    
    wstring Getustring(short idthread) {
        return (val->Getustring(idthread)+w);
    }
    
    void Setstring(string& s, short idthread) {
        val->Setstring(s, idthread);
        s += v;
    }
    
    void Setstring(wstring& s, short idthread) {
        val->Setstring(s, idthread);
        s += w;
    }
    
};

class e_b_plus_wstring2 : public e_operation  {
public:
    string v;
    wstring w;
    Tamgu* val;
    
    e_b_plus_wstring2(TamguGlobal* g, wstring s, Tamgu* t) : e_operation(g) {
        w = s;
        val = t;
        s_unicode_to_utf8(v, w);
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        wstring r = w + res->UString();
        if (res->isProtected()) {
            res->Storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providewithustring(r);
    }

    string Getstring(short idthread) {
        return (v+val->Getstring(idthread));
    }
    
    wstring Getustring(short idthread) {
        return (w+val->Getustring(idthread));
    }
    
    void Setstring(string& s, short idthread) {
        val->Setstring(s, idthread);
        s = v + s;
    }
    
    void Setstring(wstring& s, short idthread) {
        val->Setstring(s, idthread);
        s = w + s;
    }

};

class e_plus2 : public e_operation  {
public:
    
    e_plus2(TamguGlobal* g) : e_operation(g) {}
    
    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (ty == a_string) {
            if (v[0]->isConst())
                return new e_b_plus_string2(globalTamgu, v[0]->String(), v[1]);

            if (v[1]->isConst())
                return new e_plus_string2(globalTamgu, v[1]->String(), v[0]);

            return simpleupdate(v);
        }
        
        if (ty == a_ustring) {
            if (v[0]->isConst())
                return new e_b_plus_wstring2(globalTamgu, v[0]->UString(), v[1]);
            
            if (v[1]->isConst())
                return new e_plus_wstring2(globalTamgu, v[1]->UString(), v[0]);
            
            return simpleupdate(v);
        }

        if (v[0]->isConst()) {
            if (ty == a_float || ty == a_decimal) {
                e_plus_float2* e = new e_plus_float2(globalTamgu);
                e->val = v[1];
                e->v = v[0]->Float();
                return e;
            }
            e_plus_long2* e = new e_plus_long2(globalTamgu);
            e->val = v[1];
            e->v = v[0]->Long();
            return e;
        }
        else {
            if (v[1]->isConst()) {
                if (ty == a_float || ty == a_decimal) {
                    e_plus_float2* e = new e_plus_float2(globalTamgu);
                    e->val = v[0];
                    e->v = v[1]->Float();
                    return e;
                }
                e_plus_long2* e = new e_plus_long2(globalTamgu);
                e->val = v[0];
                e->v = v[1]->Long();
                return e;
            }
        }
        return simpleupdate(v);
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->plus(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->plus(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        return (values[0]->Getinteger(idthread)+values[1]->Getinteger(idthread));
    }
    
    short Getshort(short idthread) {
        return (values[0]->Getshort(idthread)+values[1]->Getshort(idthread));
    }
    
    double Getfloat(short idthread) {
        return (values[0]->Getfloat(idthread)+values[1]->Getfloat(idthread));
    }
    
    float Getdecimal(short idthread) {
        return (values[0]->Getdecimal(idthread)+values[1]->Getdecimal(idthread));
    }
    
    
    BLONG Getlong(short idthread) {
        return (values[0]->Getlong(idthread)+values[1]->Getlong(idthread));
    }
    
    string Getstring(short idthread) {
        return (values[0]->Getstring(idthread)+values[1]->Getstring(idthread));
    }
    
    wstring Getustring(short idthread) {
        return (values[0]->Getustring(idthread)+values[1]->Getustring(idthread));
    }
    
    void Setstring(string& v, short idthread) {
        values[0]->Setstring(v, idthread);
        string s;
        values[1]->Setstring(s, idthread);
        v += s;
    }
    
    void Setstring(wstring& v, short idthread) {
        values[0]->Setstring(v, idthread);
        wstring s;
        values[1]->Setstring(s, idthread);
        v += s;
    }
    
};

class e_multiply_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_multiply_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_multiply;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long() * v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)*v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)*v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)*v);
    }
    
    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)*v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)*v);
    }
    
};

class e_multiply_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_multiply_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_multiply;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = res->Float() * v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->ProvideConstfloat(r);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)*v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)*v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)*v);
    }

    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)*v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)*v);
    }
    
};

class e_minus_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_minus_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_minus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long() - v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)-v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)-v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)-v);
    }
    
    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)-v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)-v);
    }
    
};

class e_minus_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_minus_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_minus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = res->Float() - v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->ProvideConstfloat(r);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)-v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)-v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)-v);
    }
    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)-v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)-v);
    }
    
};

class e_b_minus_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_b_minus_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_minus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = v - res->Long();
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    long Getinteger(short idthread) {
        return (v-val->Getinteger(idthread));
    }
    
    short Getshort(short idthread) {
        return (v-val->Getshort(idthread));
    }
    
    BLONG Getlong(short idthread) {
        return (v-val->Getlong(idthread));
    }

    double Getfloat(short idthread) {
        return (v-val->Getfloat(idthread));
    }
    
    float Getdecimal(short idthread) {
        return (v-val->Getdecimal(idthread));
    }
    
};

class e_b_minus_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_b_minus_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_minus;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = v - res->Float();
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->ProvideConstfloat(r);
    }

    long Getinteger(short idthread) {
        return (v-val->Getinteger(idthread));
    }
    
    short Getshort(short idthread) {
        return (v-val->Getshort(idthread));
    }
    
    BLONG Getlong(short idthread) {
        return (v-val->Getlong(idthread));
    }
    
    double Getfloat(short idthread) {
        return (v-val->Getfloat(idthread));
    }
    
    float Getdecimal(short idthread) {
        return (v-val->Getdecimal(idthread));
    }
    
};

class e_minus2 : public e_operation  {
public:
    
    e_minus2(TamguGlobal* g) : e_operation(g) {}
    
    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (ty == a_string || ty == a_ustring)
            return simpleupdate(v);

        if (v[0]->isConst()) {
            if (ty == a_float || ty == a_decimal) {
                e_b_minus_float2* e = new e_b_minus_float2(globalTamgu);
                e->val = v[1];
                e->v = v[0]->Float();
                return e;
            }
            if (ty == a_short || ty == a_int || ty == a_long) {
                e_b_minus_long2* e = new e_b_minus_long2(globalTamgu);
                e->val = v[1];
                e->v = v[0]->Long();
                return e;
            }
        }
        else {
            if (v[1]->isConst()) {
                if (ty == a_float || ty == a_decimal) {
                    e_minus_float2* e = new e_minus_float2(globalTamgu);
                    e->val = v[0];
                    e->v = v[1]->Float();
                    return e;
                }
                if (ty == a_short || ty == a_int || ty == a_long) {
                    e_minus_long2* e = new e_minus_long2(globalTamgu);
                    e->val = v[0];
                    e->v = v[1]->Long();
                    return e;
                }
                
            }
        }
        return simpleupdate(v);
    }
    
    short Action() {
        return a_minus;
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->minus(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->minus(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        Tamgu* r1 = values[0]->Eval(aNULL, aNULL, idthread);
        if (r1->isTime()) {
            Tamgu* r2 = values[1]->Eval(aNULL, aNULL, idthread);
            Tamgu* v = r1->minus(r2, false);
            r1->Releasenonconst();
            r2->Releasenonconst();
            long vv=  v->Integer();
            v->Releasenonconst();
            return vv;
        }
        long v = r1->Integer();
        r1->Releasenonconst();
        return (v-values[1]->Getinteger(idthread));
    }
    
    short Getshort(short idthread) {
        Tamgu* r1 = values[0]->Eval(aNULL, aNULL, idthread);
        if (r1->isTime()) {
            Tamgu* r2 = values[1]->Eval(aNULL, aNULL, idthread);
            Tamgu* v = r1->minus(r2, false);
            r1->Releasenonconst();
            r2->Releasenonconst();
            short vv=  v->Short();
            v->Releasenonconst();
            return vv;
        }
        short v = r1->Short();
        r1->Releasenonconst();
        return (v-values[1]->Getshort(idthread));
    }
    
    double Getfloat(short idthread) {
        Tamgu* r1 = values[0]->Eval(aNULL, aNULL, idthread);
        if (r1->isTime()) {
            Tamgu* r2 = values[1]->Eval(aNULL, aNULL, idthread);
            Tamgu* v = r1->minus(r2, false);
            r1->Releasenonconst();
            r2->Releasenonconst();
            double vv=  v->Float();
            v->Releasenonconst();
            return vv;
        }
        double v = r1->Float();
        r1->Releasenonconst();
        return (v-values[1]->Getfloat(idthread));
    }
    
    float Getdecimal(short idthread) {
        Tamgu* r1 = values[0]->Eval(aNULL, aNULL, idthread);
        if (r1->isTime()) {
            Tamgu* r2 = values[1]->Eval(aNULL, aNULL, idthread);
            Tamgu* v = r1->minus(r2, false);
            r1->Releasenonconst();
            r2->Releasenonconst();
            float vv=  v->Decimal();
            v->Release();
            return vv;
        }
        float v = r1->Decimal();
        r1->Releasenonconst();
        return (v-values[1]->Getdecimal(idthread));
    }
        
    BLONG Getlong(short idthread) {
        Tamgu* r1 = values[0]->Eval(aNULL, aNULL, idthread);
        
        if (r1->isTime()) {
            Tamgu* r2 = values[1]->Eval(aNULL, aNULL, idthread);
            Tamgu* v = r1->minus(r2, false);
            r1->Releasenonconst();
            r2->Releasenonconst();
            BLONG vv=  v->Long();
            v->Release();
            return vv;
        }
        
        BLONG v = r1->Long();
        r1->Releasenonconst();
        return (v-values[1]->Getlong(idthread));
    }
    
    string Getstring(short idthread) {
        return StringMinus(values[0]->Getstring(idthread),values[1]->Getstring(idthread));
    }
    
    wstring Getustring(short idthread) {
        return StringMinus(values[0]->Getustring(idthread),values[1]->Getustring(idthread));
    }

    void Setstring(string& v, short idthread) {
        values[0]->Setstring(v, idthread);
        string s;
        values[1]->Setstring(s, idthread);
        v = StringMinus(v ,s);
    }
    
    void Setstring(wstring& v, short idthread) {
        values[0]->Setstring(v, idthread);
        wstring s;
        values[1]->Setstring(s, idthread);
        v = StringMinus(v ,s);
    }
    

};

class e_multiply2 : public e_operation  {
public:
    
    e_multiply2(TamguGlobal* g) : e_operation(g) {}
    
    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (v[0]->isConst()) {
            if (ty == a_float || ty == a_decimal) {
                e_multiply_float2* e = new e_multiply_float2(globalTamgu);
                e->val = v[1];
                e->v = v[0]->Float();
                return e;
            }
            if (ty == a_short || ty == a_int || ty == a_long) {
                e_multiply_long2* e = new e_multiply_long2(globalTamgu);
                e->val = v[1];
                e->v = v[0]->Long();
                return e;
            }
        }
        else {
            if (v[1]->isConst()) {
                if (ty == a_float || ty == a_decimal) {
                    e_multiply_float2* e = new e_multiply_float2(globalTamgu);
                    e->val = v[0];
                    e->v = v[1]->Float();
                    return e;
                }
                if (ty == a_short || ty == a_int || ty == a_long) {
                    e_multiply_long2* e = new e_multiply_long2(globalTamgu);
                    e->val = v[0];
                    e->v = v[1]->Long();
                    return e;
                }
                
            }
        }
        return simpleupdate(v);
    }
    short Action() {
        return a_multiply;
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->multiply(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->multiply(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        return (values[0]->Getinteger(idthread)*values[1]->Getinteger(idthread));
    }
    
    short Getshort(short idthread) {
        return (values[0]->Getshort(idthread)*values[1]->Getshort(idthread));
    }
    
    double Getfloat(short idthread) {
        return (values[0]->Getfloat(idthread)*values[1]->Getfloat(idthread));
    }
    
    float Getdecimal(short idthread) {
        return (values[0]->Getdecimal(idthread)*values[1]->Getdecimal(idthread));
    }
    
    
    BLONG Getlong(short idthread) {
        return (values[0]->Getlong(idthread)*values[1]->Getlong(idthread));
    }
    
};

class e_divide_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_divide_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_divide;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long() / v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)/v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)/v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)/v);
    }
    
    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)/v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)/v);
    }
};

class e_divide_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_divide_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_divide;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = res->Float() / v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->ProvideConstfloat(r);
    }
    
    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)/v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)/v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)/v);
    }

    double Getfloat(short idthread) {
        return (val->Getfloat(idthread)/v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getdecimal(idthread)/v);
    }
    
};

class e_b_divide_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_b_divide_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_divide;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long();
        if (!r) {
            res->Releasenonconst();
            return globalTamgu->Returnerror("Cannot divide by 0", idthread);
        }

        r = v / r;
        
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }
    

    long Getinteger(short idthread) {
        long vv = val->Getinteger(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v/vv);
    }
    
    short Getshort(short idthread) {
        short vv = val->Getshort(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v/vv);
    }
    
    BLONG Getlong(short idthread) {
        BLONG vv = val->Getlong(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v/vv);
    }
    
    double Getfloat(short idthread) {
        double vv = val->Getfloat(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v/vv);
    }
    
    float Getdecimal(short idthread) {
        float vv = val->Getdecimal(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (v/vv);
    }

};

class e_b_divide_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_b_divide_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_divide;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = res->Float();
        if (!r) {
            res->Releasenonconst();
            return globalTamgu->Returnerror("Cannot divide by 0", idthread);
        }
        r = v / r;

        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
       return globalTamgu->ProvideConstfloat(r);
    }
    

    double Getfloat(short idthread) {
        double vv = val->Getfloat(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }

        return (v/vv);
    }
    
    float Getdecimal(short idthread) {
        float vv = val->Getdecimal(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (v/vv);
    }
    
    long Getinteger(short idthread) {
        long vv = val->Getinteger(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v/vv);
    }
    
    short Getshort(short idthread) {
        short vv = val->Getshort(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v/vv);
    }
    
    BLONG Getlong(short idthread) {
        BLONG vv = val->Getlong(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v/vv);
    }
    
};

class e_divide2 : public e_operation  {
public:
    
    e_divide2(TamguGlobal* g) : e_operation(g) {}
    
    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (v[0]->isConst()) {
            if (ty == a_float || ty == a_decimal) {
                e_b_divide_float2* e = new e_b_divide_float2(globalTamgu);
                e->val = v[1];
                e->v = v[0]->Float();
                return e;
            }
            if (ty == a_short || ty == a_int || ty == a_long) {
                e_b_divide_long2* e = new e_b_divide_long2(globalTamgu);
                e->val = v[1];
                e->v = v[0]->Long();
                return e;
            }
        }
        else {
            if (v[1]->isConst()) {
                if (ty == a_float || ty == a_decimal) {
                    if (v[1]->Float() == 0) //we postpone the error message
                        return simpleupdate(v);

                    e_divide_float2* e = new e_divide_float2(globalTamgu);
                    e->val = v[0];
                    e->v = v[1]->Float();
                    return e;
                }
                if (ty == a_short || ty == a_int || ty == a_long) {
                    if (v[1]->Long() == 0)
                        return simpleupdate(v);
                    
                    e_divide_long2* e = new e_divide_long2(globalTamgu);
                    e->val = v[0];
                    e->v = v[1]->Long();
                    return e;
                }
                
            }
        }
        return simpleupdate(v);
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        Tamgu* v;
        if (r1->isProtected()) {
            v = r1->divide(r2, true);
            r2->Releasenonconst();
            if (v != r1)
                r1->Releasenonconst();
            return v;
        }
        
        v = r1->divide(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    short Action() {
        return a_divide;
    }

    long Getinteger(short idthread) {
        long v = values[1]->Getinteger(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getinteger(idthread)/v);
    }
    
    short Getshort(short idthread) {
        short v = values[1]->Getshort(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getshort(idthread)/v);
    }
    
    double Getfloat(short idthread) {
        double v = values[1]->Getfloat(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getfloat(idthread)/v);
    }
    
    float Getdecimal(short idthread) {
        float v = values[1]->Getdecimal(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getdecimal(idthread)/v);
    }
    
    
    BLONG Getlong(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getlong(idthread)/v);
    }
};

class e_mod_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_mod_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_mod;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long() % v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)%v);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)%v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)%v);
    }
    
    double Getfloat(short idthread) {
        return (val->Getlong(idthread)%v);
    }
    
    float Getdecimal(short idthread) {
        return (val->Getinteger(idthread)%v);
    }

};

class e_b_mod_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_b_mod_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_mod;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long();
        if (!r) {
            res->Releasenonconst();
            return globalTamgu->Returnerror("Cannot divide by 0", idthread);
        }
        r = v % r;
        
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
       return globalTamgu->Providelong(r);
    }

    long Getinteger(short idthread) {
        long vv = val->Getinteger(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v%vv);
    }
    
    short Getshort(short idthread) {
        short vv = val->Getshort(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v%vv);
    }
    
    BLONG Getlong(short idthread) {
        BLONG vv = val->Getlong(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v%vv);
    }

    double Getfloat(short idthread) {
        BLONG vv = val->Getlong(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v%vv);
    }

    float Getdecimal(short idthread) {
        long vv = val->Getinteger(idthread);
        if (vv == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        
        return (v%vv);
    }
    
};

class e_mod2 : public e_operation  {
public:
    
    e_mod2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_mod;
    }
    
    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (v[0]->isConst()) {
            e_b_mod_long2* e = new e_b_mod_long2(globalTamgu);
            e->val = v[1];
            e->v = v[0]->Long();
            return e;
        }
        else {
            if (v[1]->isConst()) {
                if (v[1]->Long() == 0)
                    return simpleupdate(v);
                
                e_mod_long2* e = new e_mod_long2(globalTamgu);
                e->val = v[0];
                e->v = v[1]->Long();
                return e;
            }
        }
        return simpleupdate(v);
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        Tamgu* v;
        if (r1->isProtected()) {
            v = r1->mod(r2, true);
            r2->Releasenonconst();
            if (v != r1)
                r1->Releasenonconst();
            return v;
        }
        
        v = r1->mod(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }
    short Getshort(short idthread) {
        short v = values[1]->Getshort(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getshort(idthread)%v);
    }
    
    long Getinteger(short idthread) {
        long v = values[1]->Getinteger(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getinteger(idthread)%v);
    }
    
    BLONG Getlong(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getlong(idthread)%v);
    }

    float Getdecimal(short idthread) {
        long v = values[1]->Getinteger(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getinteger(idthread)%v);
    }
    
    double Getfloat(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        if (v == 0) {
            globalTamgu->Returnerror("Cannot divide by 0", idthread);
            return 0;
        }
        return (values[0]->Getlong(idthread)%v);
    }

};

class e_shiftleft_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_shiftleft_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_shiftleft;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long() << v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    short Getshort(short idthread) {
        return (val->Getshort(idthread)<<v);
    }
    
    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)<<v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)<<v);
    }
    
    float Getdecimal(short idthread) {
        float32 f(val->Decimal());
        return (f.bits<<v);
    }
    
    double Getfloat(short idthread) {
        double64 f(val->Float());
        return (f.bits<<v);
    }

};

class e_b_shiftleft_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_b_shiftleft_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_shiftleft;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = v << res->Long();
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }

    short Getshort(short idthread) {
        return (v<<val->Getshort(idthread));
    }
    
    long Getinteger(short idthread) {
        return (v<<val->Getinteger(idthread));
    }
    
    BLONG Getlong(short idthread) {
        return (v<<val->Getlong(idthread));
    }

    float Getdecimal(short idthread) {
        return (v<<val->Getinteger(idthread));
    }
    
    double Getfloat(short idthread) {
        return (v<<val->Getlong(idthread));
    }
};

class e_shiftleft2 : public e_operation  {
public:
    
    e_shiftleft2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_shiftleft;
    }
    
    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (v[0]->isConst()) {
            e_b_shiftleft_long2* e = new e_b_shiftleft_long2(globalTamgu);
            e->val = v[1];
            e->v = v[0]->Long();
            return e;
        }
        else {
            if (v[1]->isConst()) {
                e_shiftleft_long2* e = new e_shiftleft_long2(globalTamgu);
                e->val = v[0];
                e->v = v[1]->Long();
                return e;
            }
        }
        return simpleupdate(v);
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->shiftleft(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->shiftleft(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        long v = values[1]->Getinteger(idthread);
        return (values[0]->Getinteger(idthread)<<v);
    }
    
    short Getshort(short idthread) {
        short v = values[1]->Getshort(idthread);
        return (values[0]->Getshort(idthread)<<v);
    }
    
    BLONG Getlong(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        return (values[0]->Getlong(idthread)<<v);
    }
    
    float Getdecimal(short idthread) {
        float v = values[1]->Getdecimal(idthread);
        return FloatShiftleft(values[0]->Decimal(),v);
    }
    
    double Getfloat(short idthread) {
        double v = values[1]->Getfloat(idthread);
        return DoubleShiftleft(values[0]->Float(),v);
    }

};

class e_shiftright_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_shiftright_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_shiftright;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = res->Long() >> v;
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }
    
    short Getshort(short idthread) {
        return (val->Getshort(idthread)>>v);
    }
    
    long Getinteger(short idthread) {
        return (val->Getinteger(idthread)>>v);
    }
    
    BLONG Getlong(short idthread) {
        return (val->Getlong(idthread)>>v);
    }
    
    float Getdecimal(short idthread) {
        float32 f(val->Decimal());
        return (f.bits>>v);
    }
    
    double Getfloat(short idthread) {
        double64 f(val->Float());
        return (f.bits>>v);
    }

};

class e_b_shiftright_long2 : public e_operation  {
public:
    BLONG v;
    Tamgu* val;
    
    e_b_shiftright_long2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_shiftright;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        BLONG r = v >> res->Long();
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->Providelong(r);
    }
    

    short Getshort(short idthread) {
        return (v>>val->Getshort(idthread));
    }
    
    long Getinteger(short idthread) {
        return (v>>val->Getinteger(idthread));
    }
    
    BLONG Getlong(short idthread) {
        return (v>>val->Getlong(idthread));
    }

    float Getdecimal(short idthread) {
        return (v>>val->Getshort(idthread));
    }
    
    double Getfloat(short idthread) {
        return (v>>val->Getlong(idthread));
    }
};

class e_shiftright2 : public e_operation  {
public:
    
    e_shiftright2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_shiftright;
    }
    
    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (v[0]->isConst()) {
            e_b_shiftright_long2* e = new e_b_shiftright_long2(globalTamgu);
            e->val = v[1];
            e->v = v[0]->Long();
            return e;
        }
        else {
            if (v[1]->isConst()) {
                e_shiftright_long2* e = new e_shiftright_long2(globalTamgu);
                e->val = v[0];
                e->v = v[1]->Long();
                return e;
            }
        }
        return simpleupdate(v);
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->shiftright(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->shiftright(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        long v = values[1]->Getinteger(idthread);
        return (values[0]->Getinteger(idthread)>>v);
    }
    
    short Getshort(short idthread) {
        short v = values[1]->Getshort(idthread);
        return (values[0]->Getshort(idthread)>>v);
    }
    
    BLONG Getlong(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        return (values[0]->Getlong(idthread)>>v);
    }
    
    float Getdecimal(short idthread) {
        float v = values[1]->Getdecimal(idthread);
        return FloatShiftright(values[0]->Decimal(),v);
    }
    
    double Getfloat(short idthread) {
        double v = values[1]->Getfloat(idthread);
        return DoubleShiftright(values[0]->Float(),v);
    }

};

class e_or2 : public e_operation  {
public:
    
    e_or2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_or;
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->orset(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->orset(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        long v = values[1]->Getinteger(idthread);
        return (values[0]->Getinteger(idthread)|v);
    }
    
    short Getshort(short idthread) {
        short v = values[1]->Getshort(idthread);
        return (values[0]->Getshort(idthread)|v);
    }
    
    BLONG Getlong(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        return (values[0]->Getlong(idthread)|v);
    }
    
    float Getdecimal(short idthread) {
        float v = values[1]->Getdecimal(idthread);
        return FloatOr(values[0]->Decimal(),v);
    }
    
    double Getfloat(short idthread) {
        double v = values[1]->Getfloat(idthread);
        return DoubleOr(values[0]->Float(),v);
    }

};

class e_xor2 : public e_operation  {
public:
    
    e_xor2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_xor;
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->xorset(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->xorset(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        long v = values[1]->Getinteger(idthread);
        return (values[0]->Getinteger(idthread)^v);
    }
    
    short Getshort(short idthread) {
        short v = values[1]->Getshort(idthread);
        return (values[0]->Getshort(idthread)^v);
    }
    
    BLONG Getlong(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        return (values[0]->Getlong(idthread)^v);
    }
    
    float Getdecimal(short idthread) {
        float v = values[1]->Getdecimal(idthread);
        return FloatXor(values[0]->Decimal(),v);
    }
    
    double Getfloat(short idthread) {
        double v = values[1]->Getfloat(idthread);
        return DoubleXor(values[0]->Float(),v);
    }

};

class e_and2 : public e_operation  {
public:
    
    e_and2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_and;
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->andset(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->andset(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        long v = values[1]->Getinteger(idthread);
        return (values[0]->Getinteger(idthread)&v);
    }
    
    short Getshort(short idthread) {
        short v = values[1]->Getshort(idthread);
        return (values[0]->Getshort(idthread)&v);
    }
    
    BLONG Getlong(short idthread) {
        BLONG v = values[1]->Getlong(idthread);
        return (values[0]->Getlong(idthread)&v);
    }
    
    float Getdecimal(short idthread) {
        float v = values[1]->Getdecimal(idthread);
        return FloatAnd(values[0]->Decimal(),v);
    }
    
    double Getfloat(short idthread) {
        double v = values[1]->Getfloat(idthread);
        return DoubleAnd(values[0]->Float(),v);
    }

};


class e_power_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_power_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_power;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = pow(res->Float(), v);
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->ProvideConstfloat(r);
    }

    long Getinteger(short idthread) {
        return pow(val->Getfloat(idthread),v);
    }
    
    short Getshort(short idthread) {
        return pow(val->Getfloat(idthread),v);
    }
    
    BLONG Getlong(short idthread) {
        return pow(val->Getfloat(idthread),v);
    }
    
    float Getdecimal(short idthread) {
        return pow(val->Getfloat(idthread),v);
    }

    double Getfloat(short idthread) {
        return pow(val->Getfloat(idthread),v);
    }
    
};

class e_b_power_float2 : public e_operation  {
public:
    double v;
    Tamgu* val;
    
    e_b_power_float2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_power;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        res = val->Eval(aNULL, aNULL, idthread);
        double r = pow(v, res->Float());
        if (res->isProtected()) {
            res->storevalue(r);
            return res;
        }
        res->Releasenonconst();
        return globalTamgu->ProvideConstfloat(r);
    }

    long Getinteger(short idthread) {
        return pow(v, val->Getfloat(idthread));
    }
    
    short Getshort(short idthread) {
        return pow(v, val->Getfloat(idthread));
    }
    
    BLONG Getlong(short idthread) {
        return pow(v, val->Getfloat(idthread));
    }
    
    float Getdecimal(short idthread) {
        return pow(v, val->Getfloat(idthread));
    }
    
    double Getfloat(short idthread) {
        return pow(v, val->Getfloat(idthread));
    }
};

class e_power2 : public e_operation  {
public:
    
    e_power2(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_power;
    }

    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (v[0]->isConst()) {
            e_b_power_float2* e = new e_b_power_float2(globalTamgu);
            e->val = v[1];
            e->v = v[0]->Float();
            return e;
        }
        else {
            if (v[1]->isConst()) {
                e_power_float2* e = new e_power_float2(globalTamgu);
                e->val = v[0];
                e->v = v[1]->Float();
                return e;
            }
        }
        return simpleupdate(v);
    }

    Tamgu* Eval(Tamgu* r1, Tamgu* r2, short idthread) {
        r1 = values[0]->Eval(aNULL, aNULL, idthread);
        r2 = values[1]->Eval(aNULL, aNULL, idthread);
        if (r1->isProtected()) {
            r1->power(r2, true);
            r2->Releasenonconst();
            return r1;
        }
        Tamgu* v = r1->power(r2, false);
        r1->Releasenonconst();
        r2->Releasenonconst();
        return v;
    }

    long Getinteger(short idthread) {
        return (long)pow(values[0]->Getfloat(idthread), values[1]->Getfloat(idthread));
    }
    
    short Getshort(short idthread) {
        return (short)pow(values[0]->Getdecimal(idthread), values[1]->Getdecimal(idthread));
    }
    
    double Getfloat(short idthread) {
        return pow(values[0]->Getfloat(idthread), values[1]->Getfloat(idthread));
    }
    
    float Getdecimal(short idthread) {
        return pow(values[0]->Getdecimal(idthread), values[1]->Getdecimal(idthread));
    }
    
    
    BLONG Getlong(short idthread) {
        return (BLONG)pow(values[0]->Getfloat(idthread), values[1]->Getfloat(idthread));
    }
};

class e_plus_long : public e_operation  {
public:
    BLONG val;
    
    
    e_plus_long(TamguGlobal* g, BLONG vl, vector<Tamgu*>& v) : e_operation(g) {
        val = vl;
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* v, short idthread) {
        BLONG res = val;
        for (long i=0;i<size; i++)
            res += values[i]->Getlong(idthread);
        
        return globalTamgu->Providelong(res);
    }

    long Getinteger(short idthread) {
        long res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getshort(idthread);
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getfloat(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getdecimal(idthread);
        return res;
    }
    
    
    BLONG Getlong(short idthread) {
        BLONG res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getlong(idthread);
        return res;
    }
};

class e_plus_float : public e_operation  {
public:
    double val;
    
    
    e_plus_float(TamguGlobal* g, double vl, vector<Tamgu*>& v) : e_operation(g) {
        val = vl;
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* v, short idthread) {
        double res = val;
        for (long i=0;i<size; i++)
            res += values[i]->Getfloat(idthread);
        
        return globalTamgu->ProvideConstfloat(res);
    }

    long Getinteger(short idthread) {
        long res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getshort(idthread);
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getfloat(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getdecimal(idthread);
        return res;
    }
    
    
    BLONG Getlong(short idthread) {
        BLONG res=val;
        for (long i=0;i<size; i++)
            res += values[i]->Getlong(idthread);
        return res;
    }
};

class e_plus : public e_operation  {
public:

    e_plus(TamguGlobal* g) : e_operation(g) {}

    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (ty == a_short || ty == a_int || ty == a_long) {
            BLONG val=0;
            vector<Tamgu*> vv;
            for (long i = 0; i < v.size(); i++) {
                if (v[i]->isConst())
                    val += v[i]->Long();
                else
                    vv.push_back(v[i]);
            }
            if (vv.size() < v.size())
                return new e_plus_long(globalTamgu, val, vv);
            return simpleupdate(v);
        }
        if (ty == a_float || ty == a_decimal) {
            double val=0;
            vector<Tamgu*> vv;
            for (long i = 0; i < v.size(); i++) {
                if (v[i]->isConst())
                    val += v[i]->Long();
                else
                    vv.push_back(v[i]);
            }
            if (vv.size() < v.size())
                return new e_plus_float(globalTamgu, val, vv);
            return simpleupdate(v);
        }
        
        return simpleupdate(v);
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->plus(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    short Action() {
        return a_plus;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getshort(idthread);
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getfloat(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getdecimal(idthread);
        return res;
    }
    

    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getlong(idthread);
        return res;
    }
    
    string Getstring(short idthread) {
        string res=values[0]->Getstring(idthread);
        for (long i=1;i<size; i++)
            res.append(values[i]->Getstring(idthread));
        return res;
    }

    wstring Getustring(short idthread) {
        wstring res=values[0]->Getustring(idthread);
        for (long i=1;i<size; i++)
            res.append(values[i]->Getustring(idthread));
        return res;
    }

    void Setstring(string& res, short idthread) {
        values[0]->Setstring(res, idthread);
        string u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res.append(u);
        }
    }

    void Setstring(wstring& res, short idthread) {
        values[0]->Setstring(res, idthread);
        wstring u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res.append(u);
        }
    }


};

class e_minus_long : public e_operation  {
public:
    bool first;
    BLONG val;
    
    
    e_minus_long(TamguGlobal* g, bool f, BLONG vl, vector<Tamgu*>& v) : e_operation(g) {
        val = vl;
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
        first = f;
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* v, short idthread) {
        BLONG res = val;
        for (long i=0;i<size; i++)
            res -= values[i]->Getlong(idthread);
        
        return globalTamgu->Providelong(res);
    }
    

    long Getinteger(short idthread) {
        if (first) {
            long res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getinteger(idthread);
            return res;
        }
        
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getinteger(idthread);
        res-=val;
        return res;
    }
    
    short Getshort(short idthread) {
        if (first) {
            short res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getshort(idthread);
            return res;
        }
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getshort(idthread);
        res-=val;
        return res;
        
    }
    
    double Getfloat(short idthread) {
        if (first) {
            double res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getfloat(idthread);
            return res;
        }
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getfloat(idthread);
        res-=val;
        return res;
        
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            float res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getdecimal(idthread);
            return res;
        }
        float res=values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getdecimal(idthread);
        res-=val;
        return res;
        
    }
    
    
    BLONG Getlong(short idthread) {
        if (first) {
            BLONG res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getlong(idthread);
            return res;
        }
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getlong(idthread);
        res-=val;
        return res;
        
    }
};

class e_minus_float : public e_operation  {
public:
    bool first;
    double val;
    
    
    e_minus_float(TamguGlobal* g, bool f, double vl, vector<Tamgu*>& v) : e_operation(g) {
        val = vl;
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
        first = f;
    }
    
    short Action() {
        return a_plus;
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* v, short idthread) {
        double res = val;
        for (long i=0;i<size; i++)
            res -= values[i]->Getfloat(idthread);
        
        return globalTamgu->ProvideConstfloat(res);
    }

    long Getinteger(short idthread) {
        if (first) {
            long res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getinteger(idthread);
            return res;
        }
        
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getinteger(idthread);
        res-=val;
        return res;
    }
    
    short Getshort(short idthread) {
        if (first) {
            short res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getshort(idthread);
            return res;
        }
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getshort(idthread);
        res-=val;
        return res;
        
    }
    
    double Getfloat(short idthread) {
        if (first) {
            double res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getfloat(idthread);
            return res;
        }
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getfloat(idthread);
        res-=val;
        return res;
        
    }
    
    float Getdecimal(short idthread) {
        if (first) {
            float res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getdecimal(idthread);
            return res;
        }
        float res=values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getdecimal(idthread);
        res-=val;
        return res;
        
    }
    
    
    BLONG Getlong(short idthread) {
        if (first) {
            BLONG res=val;
            for (long i=0;i<size; i++)
                res -= values[i]->Getlong(idthread);
            return res;
        }
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getlong(idthread);
        res-=val;
        return res;
        
    }
};

class e_minus : public e_operation  {
public:
    
    e_minus(TamguGlobal* g) : e_operation(g) {}

    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (ty == a_short || ty == a_int || ty == a_long) {
            BLONG val=0;
            vector<Tamgu*> vv;
            bool first=false;
            for (long i = 0; i < v.size(); i++) {
                if (v[i]->isConst()) {
                    if (!i) {
                        first = true;
                        val = v[i]->Long();
                        continue;
                    }
                    if (first)
                        val -= v[i]->Long();
                    else
                        val += v[i]->Long();
                }
                else
                    vv.push_back(v[i]);
            }
            if (vv.size() < v.size())
                return new e_minus_long(globalTamgu, first, val, vv);
            return simpleupdate(v);
        }
        if (ty == a_float || ty == a_decimal) {
            double val=0;
            vector<Tamgu*> vv;
            bool first = false;
            for (long i = 0; i < v.size(); i++) {
                if (v[i]->isConst()) {
                    if (!i) {
                        first = true;
                        val = v[i]->Long();
                        continue;
                    }
                    if (first)
                        val -= v[i]->Long();
                    else
                        val += v[i]->Long();
                }
                else
                    vv.push_back(v[i]);
            }
            if (vv.size() < v.size())
                return new e_minus_float(globalTamgu, first, val, vv);
            return simpleupdate(v);
        }
        
        return simpleupdate(v);
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->minus(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    short Action() {
        return a_minus;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getshort(idthread);
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getfloat(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getdecimal(idthread);
        return res;
    }
    

    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res -= values[i]->Getlong(idthread);
        return res;
    }
    
    string Getstring(short idthread) {
        string res=values[0]->Getstring(idthread);
        for (long i=1;i<size; i++)
            res = StringMinus(res, values[i]->Getstring(idthread));
        return res;
    }

    wstring Getustring(short idthread) {
        wstring res=values[0]->Getustring(idthread);
        for (long i=1;i<size; i++)
            res = StringMinus(res, values[i]->Getustring(idthread));
        return res;
    }

    void Setstring(string& res, short idthread) {
        values[0]->Setstring(res, idthread);
        string u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res = StringMinus(res, u);
        }
    }

    void Setstring(wstring& res, short idthread) {
        values[0]->Setstring(res, idthread);
        wstring u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res = StringMinus(res, u);
        }
    }

};

class e_multiply_long : public e_operation  {
public:
    BLONG val;
    
    
    e_multiply_long(TamguGlobal* g, BLONG vl, vector<Tamgu*>& v) : e_operation(g) {
        val = vl;
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
    }
    
    short Action() {
        return a_multiply;
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* v, short idthread) {
        BLONG res = val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getlong(idthread);
        
        return globalTamgu->Providelong(res);
    }

    
    long Getinteger(short idthread) {
        long res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getshort(idthread);
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getfloat(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getdecimal(idthread);
        return res;
    }
    
    
    BLONG Getlong(short idthread) {
        BLONG res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getlong(idthread);
        return res;
    }
};

class e_multiply_float : public e_operation  {
public:
    double val;
    
    
    e_multiply_float(TamguGlobal* g, double vl, vector<Tamgu*>& v) : e_operation(g) {
        val = vl;
        size = v.size();
        values = new Tamgu*[size];
        for (long i=0;i<size; i++)
            values[i]=v[i];
    }
    
    short Action() {
        return a_multiply;
    }
    
    Tamgu* Eval(Tamgu* r1, Tamgu* v, short idthread) {
        double res = val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getfloat(idthread);
        
        return globalTamgu->ProvideConstfloat(res);
    }

    long Getinteger(short idthread) {
        long res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getshort(idthread);
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getfloat(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getdecimal(idthread);
        return res;
    }
    
    
    BLONG Getlong(short idthread) {
        BLONG res=val;
        for (long i=0;i<size; i++)
            res *= values[i]->Getlong(idthread);
        return res;
    }
};

class e_multiply : public e_operation  {
public:

    e_multiply(TamguGlobal* g) : e_operation(g) {}

    e_operation* update(vector<Tamgu*>& v, short ty) {
        if (ty == a_short || ty == a_int || ty == a_long) {
            BLONG val=1;
            vector<Tamgu*> vv;
            for (long i = 0; i < v.size(); i++) {
                if (v[i]->isConst())
                    val *= v[i]->Long();
                else
                    vv.push_back(v[i]);
            }
            if (vv.size() < v.size())
                return new e_multiply_long(globalTamgu, val, vv);
            return simpleupdate(v);
        }
        if (ty == a_float || ty == a_decimal) {
            double val=1;
            vector<Tamgu*> vv;
            for (long i = 0; i < v.size(); i++) {
                if (v[i]->isConst())
                    val *= v[i]->Long();
                else
                    vv.push_back(v[i]);
            }
            if (vv.size() < v.size())
                return new e_multiply_float(globalTamgu, val, vv);
            return simpleupdate(v);
        }

        return simpleupdate(v);
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->multiply(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    short Action() {
        return a_multiply;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res *= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res *= values[i]->Getshort(idthread);
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res *= values[i]->Getfloat(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res *= values[i]->Getdecimal(idthread);
        return res;
    }
    

    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res *= values[i]->Getlong(idthread);
        return res;
    }
};

class e_divide : public e_operation  {
public:
    
    e_divide(TamguGlobal* g) : e_operation(g) {}

    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->divide(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    short Action() {
        return a_divide;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        long v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getinteger(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res /=v;
        }
            
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        short v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getshort(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res /=v;
        }
        return res;
    }
    
    double Getfloat(short idthread) {
        double res=values[0]->Getfloat(idthread);
        double v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getfloat(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res /=v;
        }
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=values[0]->Getdecimal(idthread);
        float v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getdecimal(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res /=v;
        }
        return res;
    }
    
    
    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        BLONG v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getlong(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res /=v;
        }
        return res;
    }
};

class e_mod : public e_operation  {
public:
    
    e_mod(TamguGlobal* g) : e_operation(g) {}

    short Action() {
        return a_mod;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->mod(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        short v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getshort(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res %=v;
        }
        return res;
    }
    
    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        long v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getinteger(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res %=v;
        }
        
        return res;
    }
    
    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        BLONG v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getlong(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res %=v;
        }
        return res;
    }

    float Getdecimal(short idthread) {
        long res=values[0]->Getinteger(idthread);
        long v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getinteger(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res %=v;
        }
        
        return res;
    }
    
    double Getfloat(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        BLONG v;
        for (long i=1;i<size; i++) {
            v = values[i]->Getlong(idthread);
            if (v == 0) {
                globalTamgu->Returnerror("Cannot divide by 0", idthread);
                return 0;
            }
            res %=v;
        }
        return res;
    }
};

class e_shiftleft : public e_operation  {
public:
  
    e_shiftleft(TamguGlobal* g) : e_operation(g) {}

    short Action() {
        return a_shiftleft;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->shiftleft(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res <<= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res <<= values[i]->Getshort(idthread);
        return res;
    }

    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res <<= values[i]->Getlong(idthread);
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res = values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res = FloatShiftleft(res, values[i]->Getinteger(idthread));
        return res;
    }
    
    double Getfloat(short idthread) {
        double res = values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = DoubleShiftleft(res, values[i]->Getinteger(idthread));
        return res;
    }

};


class e_shiftright : public e_operation  {
public:
 
    e_shiftright(TamguGlobal* g) : e_operation(g) {}

    short Action() {
        return a_shiftright;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->shiftright(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res >>= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res >>= values[i]->Getshort(idthread);
        return res;
    }

    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res >>= values[i]->Getlong(idthread);
        return res;
    }

    float Getdecimal(short idthread) {
        float res = values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res = FloatShiftright(res, values[i]->Getinteger(idthread));
        return res;
    }
    
    double Getfloat(short idthread) {
        double res = values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = DoubleShiftright(res, values[i]->Getinteger(idthread));
        return res;
    }

};

class e_or : public e_operation  {
public:

    e_or(TamguGlobal* g) : e_operation(g) {}

    short Action() {
        return a_or;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->orset(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res |= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res |= values[i]->Getshort(idthread);
        return res;
    }
    
    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res |= values[i]->Getlong(idthread);
        return res;
    }
    
    string Getstring(short idthread) {
        string res=values[0]->Getstring(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getstring(idthread);
        return res;
    }
    
    wstring Getustring(short idthread) {
        wstring res=values[0]->Getustring(idthread);
        for (long i=1;i<size; i++)
            res += values[i]->Getustring(idthread);
        return res;
    }

    void Setstring(string& res, short idthread) {
        values[0]->Setstring(res, idthread);
        string u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res += u;
        }
    }
    
    void Setstring(wstring& res, short idthread) {
        values[0]->Setstring(res, idthread);
        wstring u;
        for (long i=1;i<size; i++) {
            values[i]->Setstring(u, idthread);
            res += u;
        }
    }
    
    float Getdecimal(short idthread) {
        float res = values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res = FloatOr(res, values[i]->Getdecimal(idthread));
        return res;
    }
    
    double Getfloat(short idthread) {
        double res = values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = DoubleOr(res, values[i]->Getfloat(idthread));
        return res;
    }
};



class e_xor : public e_operation  {
public:

    e_xor(TamguGlobal* g) : e_operation(g) {}

    short Action() {
        return a_xor;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->xorset(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res ^= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res ^= values[i]->Getshort(idthread);
        return res;
    }
    
    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res ^= values[i]->Getlong(idthread);
        return res;
    }

    string Getstring(short idthread) {
        string res=values[0]->Getstring(idthread);
        string v;
        string u;
        short m;
        for (long j=1;j<size; j++) {
            v=values[j]->Getstring(idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] != v[i])
                    u += res[i];
            }
            res = u;
        }
        return res;
    }
    
    wstring Getustring(short idthread) {
        wstring res=values[0]->Getustring(idthread);
        wstring v;
        wstring u;
        short m;
        for (long j=1;j<size; j++) {
            v=values[j]->Getustring(idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] != v[i])
                    u += res[i];
            }
            res = u;
        }
        return res;
    }

    void Setstring(string& res, short idthread) {
        values[0]->Setstring(res, idthread);
        string v;
        string u;
        short m;
        for (long j=1;j<size; j++) {
            values[j]->Setstring(v, idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] != v[i])
                    u += res[i];
            }
            res = u;
        }
    }
    
    void Setstring(wstring& res, short idthread) {
        values[0]->Setstring(res, idthread);
        wstring v;
        wstring u;
        short m;
        for (long j=1;j<size; j++) {
            values[j]->Setstring(v, idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] != v[i])
                    u += res[i];
            }
            res = u;
        }
    }

    float Getdecimal(short idthread) {
        float res = values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res = FloatXor(res, values[i]->Getdecimal(idthread));
        return res;
    }
    
    double Getfloat(short idthread) {
        double res = values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = DoubleXor(res, values[i]->Getfloat(idthread));
        return res;
    }

};

class e_and : public e_operation  {
public:
    
    e_and(TamguGlobal* g) : e_operation(g) {}

    short Action() {
        return a_and;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->andset(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

    long Getinteger(short idthread) {
        long res=values[0]->Getinteger(idthread);
        for (long i=1;i<size; i++)
            res &= values[i]->Getinteger(idthread);
        return res;
    }
    
    short Getshort(short idthread) {
        short res=values[0]->Getshort(idthread);
        for (long i=1;i<size; i++)
            res &= values[i]->Getshort(idthread);
        return res;
    }

    BLONG Getlong(short idthread) {
        BLONG res=values[0]->Getlong(idthread);
        for (long i=1;i<size; i++)
            res &= values[i]->Getlong(idthread);
        return res;
    }
    
    string Getstring(short idthread) {
        string res=values[0]->Getstring(idthread);
        string v;
        string u;
        short m;
        for (long j=1;j<size; j++) {
            v=values[j]->Getstring(idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] == v[i])
                    u += res[i];
            }
            res = u;
        }
        return res;
    }
    
    wstring Getustring(short idthread) {
        wstring res=values[0]->Getustring(idthread);
        wstring v;
        wstring u;
        short m;
        for (long j=1;j<size; j++) {
            v=values[j]->Getustring(idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] == v[i])
                    u += res[i];
            }
            res = u;
        }
        return res;
    }
    
    void Setstring(string& res, short idthread) {
        values[0]->Setstring(res, idthread);
        string v;
        string u;
        short m;
        for (long j=1;j<size; j++) {
            values[j]->Setstring(v, idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] == v[i])
                    u += res[i];
            }
            res = u;
        }
    }
    
    void Setstring(wstring& res, short idthread) {
        values[0]->Setstring(res, idthread);
        wstring v;
        wstring u;
        short m;
        for (long j=1;j<size; j++) {
            values[j]->Setstring(v, idthread);
            m = minlocal(res.size(), v.size());
            for (short i = 0; i < m; i++) {
                if (res[i] == v[i])
                    u += res[i];
            }
            res = u;
        }
    }
    

    float Getdecimal(short idthread) {
        float res = values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res = FloatAnd(res, values[i]->Getdecimal(idthread));
        return res;
    }
    
    double Getfloat(short idthread) {
        double res = values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = DoubleAnd(res, values[i]->Getfloat(idthread));
        return res;
    }

};

class e_power : public e_operation  {
public:
    
    e_power(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_power;
    }

    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;
        bool autoself = false;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (res->isProtected())
            autoself = true;
        
        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->power(context, autoself);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }
    
    long Getinteger(short idthread) {
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = pow(res, values[i]->Getfloat(idthread));
        return (long)res;
    }
    
    short Getshort(short idthread) {
        float res=values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res = pow(res, values[i]->Getdecimal(idthread));
        return (short)res;
    }
    
    double Getfloat(short idthread) {
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = pow(res, values[i]->Getfloat(idthread));
        return res;
    }
    
    float Getdecimal(short idthread) {
        float res=values[0]->Getdecimal(idthread);
        for (long i=1;i<size; i++)
            res = pow(res, values[i]->Getdecimal(idthread));
        return res;
    }
    
    
    BLONG Getlong(short idthread) {
        double res=values[0]->Getfloat(idthread);
        for (long i=1;i<size; i++)
            res = pow(res, values[i]->Getfloat(idthread));
        return (BLONG)res;
    }
};

class e_merge : public e_operation {
public:
    e_merge(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_merge;
    }
    
    Tamgu* Eval(Tamgu* a, Tamgu* res, short idthread) {
        Tamgu* v;
        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (!res->isProtected())
            res = res->Atom(true);

        for (long i=1;i<size; i++) {
            a = values[i]->Eval(aNULL, aNULL, idthread);
            v = res->Merging(a);
            a->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }

};

class e_combine : public e_operation {
public:
    e_combine(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_combine;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* res, short idthread) {
        Tamgu* v;

        res=values[0]->Eval(aNULL, aNULL, idthread);
        if (!res->isProtected())
            res = res->Atom(true);

        for (long i=1;i<size; i++) {
            context = values[i]->Eval(aNULL, aNULL, idthread);
            v =  res->Combine(context);
            context->Releasenonconst();
            if (v != res) {
                res->Releasenonconst();
                res = v;
            }
        }
        return res;
    }
    
};

class e_add : public e_operation {
public:
    e_add(TamguGlobal* g) : e_operation(g) {}
    
    short Action() {
        return a_add;
    }
    
    Tamgu* Eval(Tamgu* a, Tamgu* r, short idthread) {
        Tamgu* v;
        
        r=values[0]->Eval(aNULL, aNULL, idthread);
        
        
        for (long i=1;i<size; i++) {
            a = values[i]->Eval(aNULL, aNULL, idthread);
            if (r->isVectorContainer()) {
                v = r->Merging(a);
                a->Releasenonconst();
            }
            else {
                if (a->isVectorContainer()) {
                    a->Insert(0, r);
                    v = a;
                    a = aNULL;
                }
                else {
                    r->Releasenonconst();
                    a->Releasenonconst();
                    return globalTamgu->Returnerror("Cannot '::' these two elements", idthread);
                }
            }
            
            if (r != v) {
                r->Releasenonconst();
                r = v;
            }
        }
        return r;
    }
    
};


//------------------------------------------------------------------ Computing types ---------------------------------
class TamguComputeValue : public TamguTracked {
public:

    e_operation* op;
    short type;
    uchar b_type;
    bool global;
    vector<short> vars;
    
    TamguComputeValue(TamguInstructionAPPLYOPERATIONROOT* r, bool gl, short t, TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_instructions, g, parent) {
        global = gl;
        type = t;
        b_type = 255;
        long i = r->instructions.size()-1;
        if (!i) {
            if (r->instructions[0]->isConst())
                op = new e_const_value(g, r->instructions[0]);
            else
                op = new e_value(g, r->instructions[0]);
        }
        else
            op = building(r,i,g);
    }
    
    uchar BType() {
        return b_type;
    }

    bool Computevariablecheck(short idthread) {        
        for (long i = 0; i < vars.size(); i++) {
            if (!globalTamgu->threads[idthread].variables.check(vars[i]))
                return false;
            
            VECTE<Tamgu*>& e = globalTamgu->threads[idthread].variables.get(vars[i]);
            if (e.size() == 0)
                return false;
        }
        return true;
    }

    void ScanVariables(vector<short>& variables) {
        for (long i = 0; i<  vars.size(); i++)
            variables.push_back(vars[i]);
    }

    e_operation* create(TamguGlobal* g, vector<Tamgu*>& v, short act) {
        e_operation* e = NULL;
        long sz=v.size();
        if (global)
            sz=-1;
        
        switch(act) {
            case a_plus:
                switch(sz) {
                    case 2:
                        e=new e_plus2(g);
                        break;
                    default:
                        e=new e_plus(g);
                }
                break;
            case a_minus:
                if (sz==2)
                    e=new e_minus2(g);
                else
                    e=new e_minus(g);
                break;
            case a_multiply:
                switch(sz) {
                    case 2:
                        e=new e_multiply2(g);
                        break;
                    default:
                        e=new e_multiply(g);
                }
                break;
            case a_divide:
                if (sz==2)
                    e=new e_divide2(g);
                else
                    e=new e_divide(g);
                break;
            case a_mod:
                if (sz==2)
                    e=new e_mod2(g);
                else
                    e=new e_mod(g);
                break;
            case a_shiftright:
                if (sz==2)
                    e=new e_shiftright2(g);
                else
                    e=new e_shiftright(g);
                break;
            case a_shiftleft:
                if (sz==2)
                    e=new e_shiftleft2(g);
                else
                    e=new e_shiftleft(g);
                break;
            case a_or:
                if (sz==2)
                    e=new e_or2(g);
                else
                    e=new e_or(g);
                break;
            case a_xor:
                if (sz==2)
                    e=new e_xor2(g);
                else
                    e=new e_xor(g);
                break;
            case a_and:
                if (sz==2)
                    e=new e_and2(g);
                else
                    e=new e_and(g);
                break;
            case a_power:
                if (sz==2)
                    e=new e_power2(g);
                else
                    e=new e_power(g);
                break;
            case a_merge:
                e = new e_merge(g);
                break;
            case a_combine:
                e = new e_combine(g);
                break;
            case a_add:
                e = new e_add(g);
                break;
            default: {
                stringstream msg;
                msg << "Invalid operation: '" << g->Getsymbol(act) << "'";
                throw new TamguRaiseError(msg);
            }
                
        }
        
        if (global) {
            e->simpleupdate(v);
            return e;
        }
        
        e_operation* ee = e->update(v, type);
        if (ee != e) {
            e->Remove();
            return ee;
        }
        return e;
    }
    
    e_operation* building(TamguInstructionAPPLYOPERATIONROOT* r, long& i, TamguGlobal* g) {
        vector<Tamgu*> v;
        short act = r->instructions[i]->Action();

        Tamgu* a;
        Tamgu* var;
        while (i--) {
            a = r->instructions[i];
            switch(a->Action()) {
                case a_none:
                case a_const:
                    var = a->update(0);
                    v.push_back(var);
                    break;
                case a_variable:
                    var = a->update(0);
                    v.push_back(var);
                    vars.push_back(var->Name());
                    break;
                case a_pipe:
                    return create(g,v,act);
                default:
                    v.push_back(building(r, i,g));
            }
        }
        return create(g,v,act);
    }
    
    bool isOperation() {
        return false;
    }

    string String() {
        return op->String();
    }
    
    void Setstring(string& v, short idthread) {
        op->Setstring(v, idthread);
    }
    
    void Setstring(wstring& v, short idthread) {
        op->Setstring(v, idthread);
    }
    

};


class TamguInstructionCompute : public TamguComputeValue {
public:

    TamguInstructionCompute(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, true,  a_short, g, parent) {}
    
    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        Tamgu* r = op->Eval(res, aNULL, idthread);
        
        if (inter == aASSIGNMENT) {
            res->Put(aNULL, r, idthread);
            r->Releasenonconst();
            return res;
        }
        
        return r;
    }

    long Getinteger(short idthread) {
        return op->Getinteger(idthread);
    }
    
    BLONG Getlong(short idthread) {
        return op->Getlong(idthread);
    }
    short Getshort(short idthread) {
        return op->Getshort(idthread);
    }
    float Getdecimal(short idthread) {
        return op->Getdecimal(idthread);
    }
    double Getfloat(short idthread) {
        return op->Getfloat(idthread);
    }
    
    string Getstring(short idthread) {
        Tamgu* v = op->Eval(aNULL, aNULL, idthread);
        string r = v->String();
        v->Release();
        return r;
    }
    
    wstring Getustring(short idthread) {
        Tamgu* v = op->Eval(aNULL, aNULL, idthread);
        wstring r = v->UString();
        v->Release();
        return r;
    }
    
    void Setstring(string& r, short idthread) {
        Tamgu* v = op->Eval(aNULL, aNULL, idthread);
        v->Setstring(r, idthread);
        v->Release();
    }

    void Setstring(wstring& r, short idthread) {
        Tamgu* v = op->Eval(aNULL, aNULL, idthread);
        v->Setstring(r, idthread);
        v->Release();
    }

};

class TamguInstructionSHORT : public TamguComputeValue {
public:
    TamguInstructionSHORT(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, false,  a_short, g, parent) {
        b_type = b_short;
    }
    
    short Getshort(short idthread) {
        return op->Getshort(idthread);
    }
    
    long Getinteger(short idthread) {
        return op->Getshort(idthread);
    }
    
    BLONG Getlong(short idthread) {
        return op->Getshort(idthread);
    }
    
    float Getdecimal(short idthread) {
        return op->Getshort(idthread);
    }
    double Getfloat(short idthread) {
        return op->Getshort(idthread);
    }

    string Getstring(short idthread) {
        return convertfromnumber((short)op->Getshort(idthread));
    }
    
    wstring Getustring(short idthread) {
        return wconvertfromnumber((short)op->Getshort(idthread));
    }
    
    void Setstring(string& s, short idthread) {
        s = convertfromnumber((short)op->Getshort(idthread));
    }
    
    void Setstring(wstring& s, short idthread) {
        s = wconvertfromnumber((short)op->Getshort(idthread));
    }


    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        short r = op->Getshort(idthread);
        
        if (inter == aASSIGNMENT) {
            res->storevalue(r);
            return res;
        }
        
        return new Tamgushort(r);
    }
    
    short Typevariable() {
        return a_short;
    }
};

class TamguInstructionINTEGER : public TamguComputeValue {
public:
    
    TamguInstructionINTEGER(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, false,  a_int, g, parent) {
        b_type = b_int;
    }
    
    short Getshort(short idthread) {
        return op->Getinteger(idthread);
    }
    
    long Getinteger(short idthread) {
        return op->Getinteger(idthread);
    }
    
    BLONG Getlong(short idthread) {
        return op->Getinteger(idthread);
    }
    
    float Getdecimal(short idthread) {
        return op->Getinteger(idthread);
    }
    
    double Getfloat(short idthread) {
        return op->Getinteger(idthread);
    }

    string Getstring(short idthread) {
        return convertfromnumber((long)op->Getinteger(idthread));
    }
    
    wstring Getustring(short idthread) {
        return wconvertfromnumber((long)op->Getinteger(idthread));
    }
    
    void Setstring(string& s, short idthread) {
        s = convertfromnumber((long)op->Getinteger(idthread));
    }
    
    void Setstring(wstring& s, short idthread) {
        s = wconvertfromnumber((long)op->Getinteger(idthread));
    }

    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        long r = op->Getinteger(idthread);
        
        if (inter == aASSIGNMENT) {
            res->storevalue(r);
            return res;
        }
        
        return globalTamgu->Provideint(r);
    }
    
    short Typevariable() {
        return a_int;
    }
};



class TamguInstructionDECIMAL : public TamguComputeValue {
public:
    
    TamguInstructionDECIMAL(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, false,  a_decimal, g, parent) {
        b_type = b_decimal;
    }
    
    
    short Getshort(short idthread) {
        return op->Getdecimal(idthread);
    }
    
    long Getinteger(short idthread) {
        return op->Getdecimal(idthread);
    }
    
    BLONG Getlong(short idthread) {
        return op->Getdecimal(idthread);
    }
    
    float Getdecimal(short idthread) {
        return op->Getdecimal(idthread);
    }
    
    double Getfloat(short idthread) {
        return op->Getdecimal(idthread);
    }

    string Getstring(short idthread) {
        return convertfromnumber((float)op->Getdecimal(idthread));
    }
    
    wstring Getustring(short idthread) {
        return wconvertfromnumber((float)op->Getdecimal(idthread));
    }
    
    void Setstring(string& s, short idthread) {
        s = convertfromnumber((float)op->Getdecimal(idthread));
    }
    
    void Setstring(wstring& s, short idthread) {
        s = wconvertfromnumber((float)op->Getdecimal(idthread));
    }

    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        float r = op->Getdecimal(idthread);

        if (inter == aASSIGNMENT) {
            res->storevalue(r);
            return res;
        }
        
        return new Tamgudecimal(r);
    }
    short Typevariable() {
        return a_decimal;
    }
};



class TamguInstructionFLOAT : public TamguComputeValue {
public:
    
    TamguInstructionFLOAT(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, false,  a_float, g, parent) {
        b_type = b_float;
    }

    short Getshort(short idthread) {
        return op->Getfloat(idthread);
    }
    
    long Getinteger(short idthread) {
        return op->Getfloat(idthread);
    }
    
    BLONG Getlong(short idthread) {
        return op->Getfloat(idthread);
    }
    
    float Getdecimal(short idthread) {
        return op->Getfloat(idthread);
    }
    
    double Getfloat(short idthread) {
        return op->Getfloat(idthread);
    }

    string Getstring(short idthread) {
        return convertfromnumber((double)op->Getfloat(idthread));
    }
    
    wstring Getustring(short idthread) {
        return wconvertfromnumber((double)op->Getfloat(idthread));
    }
    
    void Setstring(string& s, short idthread) {
        s = convertfromnumber((double)op->Getfloat(idthread));
    }
    
    void Setstring(wstring& s, short idthread) {
        s = wconvertfromnumber((double)op->Getfloat(idthread));
    }

    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        double r = op->Getfloat(idthread);

        if (inter == aASSIGNMENT) {
            res->storevalue(r);
            return res;
        }
        
        return globalTamgu->ProvideConstfloat(r);
    }
    
    short Typevariable() {
        return a_float;
    }
};

class TamguInstructionLONG : public TamguComputeValue {
public:
    
    TamguInstructionLONG(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, false,  a_long, g, parent) {
        b_type = b_long;
    }
    
    short Getshort(short idthread) {
        return op->Getlong(idthread);
    }
    
    long Getinteger(short idthread) {
        return op->Getlong(idthread);
    }
    
    BLONG Getlong(short idthread) {
        return op->Getlong(idthread);
    }
    
    float Getdecimal(short idthread) {
        return op->Getlong(idthread);
    }
    
    double Getfloat(short idthread) {
        return op->Getlong(idthread);
    }

    string Getstring(short idthread) {
        return convertfromnumber((BLONG)op->Getlong(idthread));
    }
    
    wstring Getustring(short idthread) {
        return wconvertfromnumber((BLONG)op->Getlong(idthread));
    }
    
    void Setstring(string& s, short idthread) {
        s = convertfromnumber((BLONG)op->Getlong(idthread));
    }
    
    void Setstring(wstring& s, short idthread) {
        s = wconvertfromnumber((BLONG)op->Getlong(idthread));
    }

    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        BLONG r = op->Getlong(idthread);

        if (inter == aASSIGNMENT) {
            res->storevalue(r);
            return res;
        }
        
        return globalTamgu->Providelong(r);
    }
    
    short Typevariable() {
        return a_long;
    }
};


class TamguInstructionSTRING : public TamguComputeValue {
public:
    
    TamguInstructionSTRING(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, false,  a_string, g, parent) {
        b_type = b_string;
    }
    
    string Getstring(short idthread) {
        return op->Getstring(idthread);
    }

    wstring Getustring(short idthread) {
        string r = op->Getstring(idthread);
        wstring s;
        s_utf8_to_unicode(s,USTR(r), r.size());
        return s;
    }

    void Setstring(string& s, short idthread) {
        op->Setstring(s,idthread);
    }

    void Setstring(wstring& s, short idthread) {
        op->Setstring(s,idthread);
    }

    short Getshort(short idthread) {
        return convertinteger(op->Getstring(idthread));
    }
    
    long Getinteger(short idthread) {
        return convertinteger(op->Getstring(idthread));
    }
    
    BLONG Getlong(short idthread) {
        return convertinteger(op->Getstring(idthread));
    }
    
    float Getdecimal(short idthread) {
        return convertfloat(op->Getstring(idthread));
    }
    
    double Getfloat(short idthread) {
        return convertfloat(op->Getstring(idthread));
    }

    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        string r = op->Getstring(idthread);
        if (inter == aASSIGNMENT) {
            res->Storevalue(r);
            return res;
        }
        return globalTamgu->Providewithstring(r);
    }
    
    short Typevariable() {
        return a_string;
    }
};

class TamguInstructionUSTRING : public TamguComputeValue {
public:

    TamguInstructionUSTRING(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguComputeValue(r, false,  a_ustring, g, parent) {
        b_type = b_ustring;
    }

    string Getstring(short idthread) {
        wstring r = op->Getustring(idthread);
        string s;
        s_unicode_to_utf8(s,r);
        return s;
    }

    wstring Getustring(short idthread) {
        return op->Getustring(idthread);
    }
    
    void Setstring(string& s, short idthread) {
        op->Setstring(s,idthread);
    }

    void Setstring(wstring& s, short idthread) {
        op->Setstring(s,idthread);
    }

    short Getshort(short idthread) {
        return convertinteger(op->Getustring(idthread));
    }
    
    long Getinteger(short idthread) {
        return convertinteger(op->Getustring(idthread));
    }
    
    BLONG Getlong(short idthread) {
        return convertinteger(op->Getustring(idthread));
    }
    
    float Getdecimal(short idthread) {
        return convertfloat(op->Getustring(idthread));
    }
    
    double Getfloat(short idthread) {
        return convertfloat(op->Getustring(idthread));
    }

    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        wstring r = op->Getustring(idthread);
        if (inter == aASSIGNMENT) {
            res->Storevalue(r);
            return res;
        }
        return globalTamgu->Providewithustring(r);
    }
    
    short Typevariable() {
        return a_ustring;
    }

};

class TamguInstructionFRACTION : public TamguInstruction {
public:
    
    long size;
    
    TamguInstructionFRACTION(TamguInstructionAPPLYOPERATIONROOT* r, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructions, g, parent) {
        instructions = r->instructions;
        size = r->Size();
    }
    
    long Size() {
        return size;
    }
    
    double Getfloat(short idthread) {
        short d = (short)size - 1;
        Tamgu* r = cfraction(idthread, d);
        if (r->isError())
            return 0;
        double rv = r->Float();
        r->Releasenonconst();
        return rv;
    }
    
    Tamgu* Eval(Tamgu* res, Tamgu* inter, short idthread) {
        short d = (short)size - 1;
        Tamgu* r = cfraction(idthread, d);
        if (r->isError())
            return r;
        
        if (inter == aASSIGNMENT) {
            res->Putvalue(r, idthread);
            return res;
        }
        
        return r;
    }
    
    Tamgu* cfraction(short idthread, short& d);
    
    bool isOperation() {
        return false;
    }
    
    short Typevariable() {
        return a_float;
    }

};

class TamguInstructionEQUShort : public TamguInstruction {
public:
    Tamgu* recipient;
    TamguInstructionSHORT* instruction;

    TamguInstructionEQUShort(TamguInstructionAPPLYOPERATIONEQU* e, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructionequ, g, parent) {
        recipient = e->recipient;
        instruction = (TamguInstructionSHORT*)e->instruction;
        action = e->action;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isEQU() {
        return true;
    }
    
    short Typevariable() {
        return a_short;
    }

    void ScanVariables(vector<short>& vars) {
        if (recipient != NULL)
            recipient->ScanVariables(vars);
        if (instruction != NULL)
            instruction->ScanVariables(vars);
    }

};

class TamguInstructionEQUInteger : public TamguInstruction {
public:
    Tamgu* recipient;
    TamguInstructionINTEGER* instruction;
    
    TamguInstructionEQUInteger(TamguInstructionAPPLYOPERATIONEQU* e, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructionequ, g, parent) {
        recipient = e->recipient;
        instruction = (TamguInstructionINTEGER*)e->instruction;
        action = e->action;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isEQU() {
        return true;
    }
    
    short Typevariable() {
        return a_int;
    }

    void ScanVariables(vector<short>& vars) {
        if (recipient != NULL)
            recipient->ScanVariables(vars);
        if (instruction != NULL)
            instruction->ScanVariables(vars);
    }

};

class TamguInstructionEQULong : public TamguInstruction {
public:
    Tamgu* recipient;
    TamguInstructionLONG* instruction;

    TamguInstructionEQULong(TamguInstructionAPPLYOPERATIONEQU* e, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructionequ, g, parent) {
        recipient = e->recipient;
        instruction = (TamguInstructionLONG*)e->instruction;
        action = e->action;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isEQU() {
        return true;
    }
    
    short Typevariable() {
        return a_long;
    }

    void ScanVariables(vector<short>& vars) {
        if (recipient != NULL)
            recipient->ScanVariables(vars);
        if (instruction != NULL)
            instruction->ScanVariables(vars);
    }

};

class TamguInstructionEQUDecimal : public TamguInstruction {
public:
    Tamgu* recipient;
    TamguInstructionDECIMAL* instruction;

    TamguInstructionEQUDecimal(TamguInstructionAPPLYOPERATIONEQU* e, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructionequ, g, parent) {
        recipient = e->recipient;
        instruction = (TamguInstructionDECIMAL*)e->instruction;
        action = e->action;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isEQU() {
        return true;
    }
    
    short Typevariable() {
        return a_decimal;
    }

    void ScanVariables(vector<short>& vars) {
        if (recipient != NULL)
            recipient->ScanVariables(vars);
        if (instruction != NULL)
            instruction->ScanVariables(vars);
    }

};

class TamguInstructionEQUFloat : public TamguInstruction {
public:
    Tamgu* recipient;
    TamguInstructionFLOAT* instruction;

    TamguInstructionEQUFloat(TamguInstructionAPPLYOPERATIONEQU* e, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructionequ, g, parent) {
        recipient = e->recipient;
        instruction = (TamguInstructionFLOAT*) e->instruction;
        action = e->action;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isEQU() {
        return true;
    }
    
    short Typevariable() {
        return a_float;
    }

    void ScanVariables(vector<short>& vars) {
        if (recipient != NULL)
            recipient->ScanVariables(vars);
        if (instruction != NULL)
            instruction->ScanVariables(vars);
    }

};

class TamguInstructionEQUString : public TamguInstruction {
public:
    Tamgu* recipient;
    TamguInstructionSTRING* instruction;

    TamguInstructionEQUString(TamguInstructionAPPLYOPERATIONEQU* e, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructionequ, g, parent) {
        recipient = e->recipient;
        instruction = (TamguInstructionSTRING*)e->instruction;
        action = e->action;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isEQU() {
        return true;
    }
    
    short Typevariable() {
        return a_string;
    }

    void ScanVariables(vector<short>& vars) {
        if (recipient != NULL)
            recipient->ScanVariables(vars);
        if (instruction != NULL)
            instruction->ScanVariables(vars);
    }
    
};

class TamguInstructionEQUUString : public TamguInstruction {
public:
    Tamgu* recipient;
    TamguInstructionUSTRING* instruction;

    TamguInstructionEQUUString(TamguInstructionAPPLYOPERATIONEQU* e, TamguGlobal* g, Tamgu* parent = NULL) : TamguInstruction(a_instructionequ, g, parent) {
        recipient = e->recipient;
        instruction = (TamguInstructionUSTRING*)e->instruction;
        action = e->action;
    }
    
    Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    bool isEQU() {
        return true;
    }
    
    short Typevariable() {
        return a_ustring;
    }

    void ScanVariables(vector<short>& vars) {
        if (recipient != NULL)
            recipient->ScanVariables(vars);
        if (instruction != NULL)
            instruction->ScanVariables(vars);
    }

};


#endif



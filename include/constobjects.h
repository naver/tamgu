/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : constobjects.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef constobjects_h
#define constobjects_h

#ifdef UNIX
#include <climits>
#endif

#include "tamgustring.h"
#include "tamgushort.h"
#include "tamguustring.h"
#include "tamguint.h"
#include "tamgufloat.h"
#include "tamgudecimal.h"
#include "tamgulong.h"
#include "tamgubool.h"


//---------------------------------------------------------------------------------
class TamguBaseConst : public Tamgu {
public:
	short idtype;

	TamguBaseConst(short ty, TamguGlobal* g, Tamgu* parent = NULL) : idtype(ty) {
		if (parent != NULL)
			parent->AddInstruction(this);

		if (g != NULL)
			g->RecordInTracker(this);
        investigate = is_constante;
	}

	short Type() {
		return idtype;
	}

	short Typeinfered() {
		return idtype;
	}

	short Typevariable() {
		return idtype;
	}

	virtual short Action() {
		return a_const;
	}

	bool isAtom() {
		return true;
	}

    bool checkAtomType(short ty) {
        return (idtype == ty);
    }
    
	virtual bool baseValue() {
		return true;
	}

    virtual Tamgu* AtomNoConst() {
        return this;
    }
    
    bool Candelete() {
        return false;
    }
};

//---------------------------------------------------------------------------------
class TamguConst : public TamguBaseConst {
public:
	short idconst;
	string name;
    string content;
    wstring wcontent;
	uint64_t value;

	TamguConst(short i, string n, TamguGlobal* g) : idconst(i), name(n), value(LLONG_MAX), TamguBaseConst(a_const, g)  {
        content = "0n1u2l3l4";
        content[0] = 1;
        content[2] = 2;
        content[4] = 3;
        content[6] = 4;
        content[8] = 5;
      
        wcontent = L"0n1u2l3l4";
        wcontent[0] = 1;
        wcontent[2] = 2;
        wcontent[4] = 3;
        wcontent[6] = 4;
        wcontent[8] = 5;

        if (i == a_null || i == a_empty)
            investigate = is_null;
    }

	string JSonString() {
        if (idconst == a_null)
            return name;
		string v;
		v = '"';
		v += name;
		v += '"';
		return(v);
	}

	string Typestring() {
		return name;
	}

	string StringToDisplay() {
		return name;
	}

	bool baseValue() {
		return false;
	}

	bool Boolean() {
		return false;
	}


	Tamgu* Atom(bool forced = false) {
		if (this == aNOELEMENT)
			return aNULL;
		return this;
	}

    void Setstring(string& v, short idthread) {
        v = "";
    }
    
	void Setstring(wstring& v, short idthread) {
        v = L"";
    }

    long Getinteger0(short idthread) {
        return 0;
    }

	long Getinteger(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	short Getshort(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	string Getstring(short idthread) {
		return content;
	}

	wstring Getustring(short idthread) {
		return wcontent;
	}

    string asString() { return content; }
    wstring asUString() { return wcontent; }
	long asInteger() { return value; }
    double asFloat() { return value; }
    float asDecimal() { return value; }
    BLONG asLong() { return value; }
    short asShort() { return value; }
    unsigned char asByte() {
        return 255;
    }
    
    string String() { return ""; }
    wstring UString() { return L""; }
    long Integer() { return 0; }
    double Float() { return 0; }
    float Decimal() { return 0; }
    BLONG Long() { return 0; }
    short Short() { return 0; }
    unsigned char Byte() {
        return 0;
    }

    bool Stringpredicatekey(string& v) {
        return false;
    }

    bool Stringpredicatekeysecond(string& v) {
        return false;
    }

    bool Stringpredicatekeythird(string& v) {
        return false;
    }
};

class TamguConstBreak : public TamguConst {
public:

	TamguConstBreak(short i, string n, TamguGlobal* g) : TamguConst(i, n, g) {
        investigate = is_break;
    }

    short checkTypePredicate() {
        return a_break;
    }

};

class TamguConstContinue : public TamguConst {
public:

	TamguConstContinue(short i, string n, TamguGlobal* g) : TamguConst(i, n, g) {
        investigate = is_continue;
    }
};

class TamguConstPipe : public TamguConst {
public:

    TamguConstPipe(short i, string n, TamguGlobal* g) : TamguConst(i, n, g) {}

	short Action() {
		return a_pipe;
	}

};

class TamguConstUniversal : public TamguConst {
public:
    
    TamguConstUniversal(short i, string n, TamguGlobal* g) : TamguConst(i, "_", g) {}
    
    Tamgu* same(Tamgu*) {
        return aTRUE;
    }
    
};

//---------------------------------------------------------------------------------

class TamguConstString : public TamguBaseConst {
public:
	string value;
    long sz, szc;

	TamguConstString(string v, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_string, NULL, parent) {
        investigate |= is_pure_string;
        sz = v.size();
        szc = size_c(v);
    }

	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* AtomNoConst() {
        return globalTamgu->Providewithstring(value);
    }
	
    Tamgu* Atomref() {
        TamguReference* r = globalTamgu->Providewithstring(value);
        r->reference = 1;
        r->protect = false;
        return r;
    }
    
    string Info(string n);
    void Methods(Tamgu* vs);
    
	short Typeinfered() {
		return a_string;
	}

    Tamgu* Eval(Tamgu* context, Tamgu* idx, short idthread) {
        if (!idx->isIndex() || context == idx)
            return this;
        
        long ileft, iright;
        char res = StringIndexes(value, idx, ileft, iright, idthread);
        
        if (res == 0) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror(e_wrong_key_in, idthread);
            return aNOELEMENT;
        }
        
        if (res == 1)
            idx = globalTamgu->Providestring(c_char_get(USTR(value), ileft));
        else
            idx = globalTamgu->Providestring(value.substr(ileft, iright - ileft));
        return idx;
    }


	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Providewithstring(value);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->Providestring(a->String());
	}

    Tamgu* in(Tamgu* context, Tamgu* a, short idthread);

	TamguIteration* Newiteration(bool direction) {
		return new TamguIterationstring(value, direction);
	}

	Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
		Tamgustring a(value);
		return a.Looptaskell(recipient, context, environment, bd, idthread);
	}

	Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
		Tamgustring a(value);
		return a.Filter(idthread, env, bd, var, kcont, accu, init, direct);
	}

	wstring UString() {
		wstring res;
		s_utf8_to_unicode(res, USTR(value), value.size());
		return res;
	}

    string JSonString() {
        string res;        
        jstringing(res, value);
        return res;
    }

	string String() { return value; }

    void Setstring(string& v, short idthread) {
        v = value;
    }
    
    void Setstring(wstring& v, short idthread) {
        sc_utf8_to_unicode(v, USTR(value), value.size());
    }
    
	string Getstring(short idthread) {
		return value;
	}

	wstring Getustring(short idthread) {
		wstring res;
		s_utf8_to_unicode(res, USTR(value), value.size());
		return res;
	}

    long CommonSize() {
        return szc;
    }

    long Size() {
        return sz;
    }

    bool Boolean() {
        return (value != "");
    }

    unsigned char Byte() {
        return value[0];
    }
    
    unsigned char asByte() {
        return value[0];
    }
    
    long asInteger() {
        return (long)conversionintegerhexa(STR(value));
    }

    float asDecimal() {
        return convertfloat(STR(value));
    }

    double asFloat() {
        return convertfloat(STR(value));
    }

    BLONG asLong() {
        return conversionintegerhexa(STR(value));
    }

    short asShort() {
        return (short)conversionintegerhexa(STR(value));
    }


	long Integer() {
		return (long)conversionintegerhexa(STR(value));
	}

	float Decimal() {
		return convertfloat(STR(value));
	}

	double Float() {
		return convertfloat(STR(value));
	}

	BLONG Long() {
		return conversionintegerhexa(STR(value));
	}

	short Short() {
		return (short)conversionintegerhexa(STR(value));
	}

	Tamgu* Atom(bool forced = false) {
		return globalTamgu->Providewithstring(value);
	}

	Tamgu* Vector(short idthread) {
		return globalTamgu->EvaluateVector(value, idthread);
	}

	Tamgu* Map(short idthread) {
		return globalTamgu->EvaluateMap(value, idthread);
	}

	//we add the current value with a
	Tamgu* plus(Tamgu* a, bool itself) {
		string s = value + a->String();
		return globalTamgu->Providewithstring(s);
	}

	//we remove a from the current value
	Tamgu* minus(Tamgu* a, bool itself) {
		string v = a->String();
		size_t nb = v.size();
		size_t pos = value.find(v);
		if (pos == string::npos)
			return this;

		//we keep our string up to p
		v = value.substr(0, pos);
		//then we skip the nb characters matching the size of v
		pos += nb;
		//then we concatenate with the rest of the string
		v += value.substr(pos, value.size() - pos);
		return globalTamgu->Providewithstring(v);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value < a->asString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value > a->asString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        return booleantamgu[value == a->asString()];
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aTRUE;
#endif
		if (value != a->asString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value <= a->asString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value >= a->asString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		Tamgustring v(value);
		return v.Succ();
	}

	Tamgu* Pred() {
		Tamgustring v(value);
		return v.Pred();
	}

};

//--------------------------------------------------------------------
class TamguConstUString : public TamguBaseConst {
public:
	wstring value;
    long sz, szc;

    TamguConstUString(wstring v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_ustring, g, parent) {
        investigate |= is_pure_string;
        sz = value.size();
#ifdef WSTRING_IS_UTF16
        szc = size_w(value);
#else
        szc = sz;
#endif
    }
    
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* AtomNoConst() {
        return globalTamgu->Providewithustring(value);
    }


    Tamgu* Atomref() {
        TamguReference* r = globalTamgu->Providewithustring(value);
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Info(string n);
    void Methods(Tamgu* vs);
	short Typeinfered() {
		return a_ustring;
	}

    Tamgu* Eval(Tamgu* context, Tamgu* idx, short idthread) {
        if (!idx->isIndex() || context == idx)
            return this;
        
        long ileft, iright;
        char res = StringIndexes(value, idx, ileft, iright, idthread);
        
        if (res == 0) {
            if (globalTamgu->erroronkey)
               return globalTamgu->Returnerror(e_wrong_key_in, idthread);
            return aNOELEMENT;
        }
        
        if (res == 1) {
            Tamguustring* s = globalTamgu->Provideustring(L"");
#ifdef WSTRING_IS_UTF16
            uint32_t c = value[ileft];
            s->value = c;
            if (checklargeutf16(c))
                s->value += value[ileft + 1];
#else
            s->value = value[ileft];
#endif
            return s;
        }
        
        idx = globalTamgu->Provideustring(value.substr(ileft, iright - ileft));
        return idx;
    }
    
	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Providewithustring(value);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->Provideustring(a->UString());
	}

	TamguIteration* Newiteration(bool direction) {
		return new TamguIterationustring(value, direction);
	}

    Tamgu* in(Tamgu* context, Tamgu* a, short idthread);

	Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
		Tamguustring a(value);
		return a.Looptaskell(recipient, context, environment, bd, idthread);
	}

	Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
		Tamguustring a(value);
		return a.Filter(idthread, env, bd, var, kcont, accu, init, direct);
	}

	wstring UString() { 
		return value; 
	}

    string JSonString() {
        string res;
        string v = String();
        jstringing(res, v);
        return res;
    }
    
	string String() {
		string res;
		s_unicode_to_utf8(res, value);
		return res;
	}

    void Setstring(string& v, short idthread) {
        sc_unicode_to_utf8(v, value);
    }
    
    void Setstring(wstring& v, short idthread) {
        v = value;
    }
    
    long CommonSize() {
        return szc;
    }
    
	string Getstring(short idthread) {
		string res;
		s_unicode_to_utf8(res, value);
		return res;
	}

	wstring Getustring(short idthread) {
		return value;
	}

    long Size() {
        return sz;
    }

    bool Boolean() {
        return (value != L"");
    }

    unsigned char Byte() {
        return (unsigned char)value[0];
    }

    unsigned char asByte() {
        return (unsigned char)value[0];
    }

    long asInteger() {
        return (long)conversionintegerhexa(STR(value));
    }

    float asDecimal() {
        return convertfloat(STR(value));
    }

    double asFloat() {
        return convertfloat(STR(value));
    }

    BLONG asLong() {
        return conversionintegerhexa(STR(value));
    }

    short asShort() {
        return (short)conversionintegerhexa(STR(value));
    }


	long Integer() {
		return (long)conversionintegerhexa(STR(value));
	}

	float Decimal() {
		return convertfloat(STR(value));
	}

	double Float() {
		return convertfloat(STR(value));
	}

	BLONG Long() {
		return conversionintegerhexa(STR(value));
	}

	short Short() {
		return (short)conversionintegerhexa(STR(value));
	}

	Tamgu* Atom(bool forced = false) {
		return globalTamgu->Providewithustring(value);
	}

	Tamgu* Vector(short idthread) {
		string res;
		s_unicode_to_utf8(res, value);
		return globalTamgu->EvaluateVector(res, idthread);
	}

	Tamgu* Map(short idthread) {
		string res;
		s_unicode_to_utf8(res, value);
		return globalTamgu->EvaluateMap(res, idthread);
	}

	//we add the current value with a
	Tamgu* plus(Tamgu* a, bool itself) {
		wstring s = value + a->UString();
		return globalTamgu->Providewithustring(s);
	}

	//we remove a from the current value
	Tamgu* minus(Tamgu* a, bool itself) {
		wstring v = a->UString();
		size_t nb = v.size();
		size_t pos = value.find(v);
		if (pos == string::npos)
			return this;

		//we keep our string up to p
		v = value.substr(0, pos);
		//then we skip the nb characters matching the size of v
		pos += nb;
		//then we concatenate with the rest of the string
		v += value.substr(pos, value.size() - pos);
		return globalTamgu->Providewithustring(v);
	}


	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value < a->asUString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value > a->asUString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
        return booleantamgu[value == a->asUString()];
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aTRUE;
#endif
		if (value != a->asUString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value <= a->asUString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isString())
            return aFALSE;
#endif
		if (value >= a->asUString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		Tamguustring v(value);
		return v.Succ();
	}

	Tamgu* Pred() {
		Tamguustring v(value);
		return v.Pred();
	}

};
//--------------------------------------------------------------------

class TamguConstInt : public TamguBaseConst {
public:
	long value;

	TamguConstInt(long v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_int, g, parent) {
     investigate |= is_number;}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	bool Checkprecision(Tamgu* r) {
		if (idtype < r->Typenumber())
			return false;

		return true;
	}

    Tamgu* AtomNoConst() {
        return globalTamgu->Provideint(value);
    }

    Tamgu* Atomref() {
        TamguReference* r = globalTamgu->Provideint(value);
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_int;
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Provideint(value);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->Provideint(a->Integer());
	}

	short Typenumber() {
		return a_int;
	}

	

	bool isInteger() {
		return true;
	}

	long Size() {
		return value;
	}

	wstring UString() {
		
		return wconvertfromnumber(value);
		
	}

    string String() {
        return convertfromnumber(value);
    }

    string JSonString() {
        return convertfromnumber(value);
    }


    void Setstring(string& v, short idthread) {
        convertnumber(value,v);
    }

    void Setstring(wstring& v, short idthread) {
        convertnumber(value,v);
    }

	long Getinteger(short idthread) {
		return value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		
		return convertfromnumber(value);
		
	}

	wstring Getustring(short idthread) {
		
		return wconvertfromnumber(value);
		
	}

    bool Boolean() {
        return value;
    }

	long Integer() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	BLONG Long() {
		return value;
	}

	short Short() {
		return value;
	}

    unsigned char Byte() {
        return value;
    }

    unsigned char asByte() {
        return value;
    }

    long asInteger() {
        return value;
    }

    float asDecimal() {
        return (float)value;
    }

    double asFloat() {
        return value;
    }

    BLONG asLong() {
        return value;
    }

    short asShort() {
        return value;
    }

	Tamgu* Atom(bool forced = false) {
		return globalTamgu->Provideint(value);
	}

	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value & a->Integer());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value | a->Integer());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value ^ a->Integer());
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value + a->Integer());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value - a->Integer());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value * a->Integer());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);
		v = (double)value / v;
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value << a->Integer());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		return globalTamgu->Provideint(value >> a->Integer());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);

		v = value % v;
		return globalTamgu->ProvideConstint(v);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
        
		if (value < a->asInteger())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->asInteger())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        return booleantamgu[value == a->asInteger()];
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->asInteger())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->asInteger())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value >= a->asInteger())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		return globalTamgu->Provideint(value + 1);
	}

	Tamgu* Pred() {
		return globalTamgu->Provideint(value - 1);
	}

};
//--------------------------------------------------------------------

class TamguConstShort : public TamguBaseConst {
public:
	short value;

	TamguConstShort(short v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_short, g, parent) {
     investigate |= is_number;}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return globalTamgu->Provideint(value);
	}

    Tamgu* AtomNoConst() {
        return new Tamgushort(value);
    }

    Tamgu* Atomref() {
        TamguReference* r = new Tamgushort(value);
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_short;
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
        return new Tamgushort(a->Integer());
	}

	bool Checkprecision(Tamgu* r) {
		if (r->Type() <= a_short)
			return true;

		return false;
	}


	bool isShort() {
		return true;
	}

	short Typenumber() {
		return a_short;
	}

	

	bool isInteger() {
		return true;
	}

	long Size() {
		return value;
	}

	string String() {
		return convertfromnumber(value);
	}

    string JSonString() {
        return convertfromnumber(value);
    }


    void Setstring(string& v, short idthread) {
        v = convertfromnumber(value);
    }

    void Setstring(wstring& v, short idthread) {
        v = wconvertfromnumber(value);
    }

	wstring UString() {
		return wconvertfromnumber(value);
	}

	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		
		return convertfromnumber(value);
		
	}

	wstring Getustring(short idthread) {
		
		return wconvertfromnumber(value);
		
	}

    bool Boolean() {
        return value;
    }

    unsigned char Byte() {
        return value;
    }


    unsigned char asByte() {
        return value;
    }

    long asInteger() {
        return value;
    }

    float asDecimal() {
        return (float)value;
    }

    double asFloat() {
        return value;
    }

    BLONG asLong() {
        return value;
    }

    short asShort() {
        return value;
    }

	long Integer() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	BLONG Long() {
		return value;
	}

	short Short() {
		return value;
	}

	Tamgu* Atom(bool forced = false) {
        return globalTamgu->Provideint(value);
	}

	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
        if (a->Type() == a_short)
            return globalTamgu->Provideint(value & a->Short());
        return a->andset(this, itself);
	}

	Tamgu* orset(Tamgu* a, bool itself) {
        if (a->Type() == a_short)
            return globalTamgu->Provideint(value | a->Short());
        return a->orset(this, itself);
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
        if (a->Type() == a_short)
            return globalTamgu->Provideint(value ^ a->Short());
        return a->xorset(this, itself);
	}

	Tamgu* plus(Tamgu* a, bool itself) {
        if (a->Type() == a_short) {
            BLONG v = value;
            v += a->Long();
            return globalTamgu->Providelong(v);
        }
        return a->plus(this, itself);
	}

	Tamgu* minus(Tamgu* a, bool itself) {
        if (a->Type() == a_short) {
            BLONG v = value;
            v -= a->Long();
            return globalTamgu->Providelong(v);
        }
        a = a->minus(this, itself);
        a->multiply(aMINUSONE, true);
        return a;
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
        if (a->Type() == a_short) {
            BLONG v = value;
            v *= a->Long();
            return globalTamgu->Providelong(v);
        }
        return a->multiply(this, itself);
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);
		v = (double)value / v;
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
        BLONG v = value;
        v <<= a->Long();
        return globalTamgu->Providelong(v);
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
        BLONG v = value;
        v >>= a->Long();
        return globalTamgu->Providelong(v);
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		BLONG v = value;
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);

		v %= a->Long();
		return globalTamgu->Providelong(v);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value < a->asShort())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->asShort())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        return booleantamgu[value == a->asShort()];
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->asShort())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->asShort())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->asShort())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
        return globalTamgu->Provideint(value + 1);
	}

	Tamgu* Pred() {
        return globalTamgu->Provideint(value - 1);
	}

};
//--------------------------------------------------------------------
class TamguConstDecimal : public TamguBaseConst {
public:
	float value;

	TamguConstDecimal(double v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_decimal, g, parent) {
     investigate |= is_number;}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	bool Checkprecision(Tamgu* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}
	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgudecimal(value);
	}

    Tamgu* AtomNoConst() {
        return new Tamgudecimal(value);
    }

    Tamgu* Atomref() {
        TamguReference* r = new Tamgudecimal(value);
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_decimal;
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return new Tamgudecimal(a->Decimal());
	}

	short Typenumber() {
		return a_decimal;
	}

	

	bool isFloat() {
		return true;
	}

	string String() {
		
		return convertfromnumber(value);
		
	}

    string JSonString() {
        return convertfromnumber(value);
    }


	wstring UString() {
		
		return wconvertfromnumber(value);
		
	}

    void Setstring(string& v, short idthread) {
        v = convertfromnumber(value);
    }

    void Setstring(wstring& v, short idthread) {
        v = wconvertfromnumber(value);
    }


	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		
		return convertfromnumber(value);
		
	}

	wstring Getustring(short idthread) {
		
		return wconvertfromnumber(value);
		
	}

	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	float Decimal() {
		return value;
	}

	double Float() {
		return value;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
		return value;
	}

    bool Boolean() {
        return value;
    }

    unsigned char Byte() {
        return value;
    }


    unsigned char asByte() {
        return value;
    }

    long asInteger() {
        return value;
    }

    float asDecimal() {
        return (float)value;
    }

    double asFloat() {
        return value;
    }

    BLONG asLong() {
        return value;
    }

    short asShort() {
        return value;
    }


	Tamgu* Atom(bool forced = false) {
		return new Tamgudecimal(value);
	}


	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
		return new Tamgudecimal((long)value & a->Integer());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		return new Tamgudecimal((long)value | a->Integer());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		return new Tamgudecimal((long)value ^ a->Integer());
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		return new Tamgudecimal(value + a->Decimal());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		return new Tamgudecimal(value - a->Decimal());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		return new Tamgudecimal(value * a->Decimal());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		float v = a->Decimal();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);
		v = value / v;
		return new Tamgudecimal(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		v = pow(v, a->Decimal());
		return new Tamgudecimal(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		return new Tamgudecimal((long)value << a->Integer());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		return new Tamgudecimal((long)value >> a->Integer());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);

		v = (long)value % v;
		return new Tamgudecimal(v);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value < a->asDecimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->asDecimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        return booleantamgu[value == a->asDecimal()];
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->asDecimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->asDecimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->asDecimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		return new Tamgudecimal(value + 1);
	}

	Tamgu* Pred() {
		return new Tamgudecimal(value - 1);
	}

};

//--------------------------------------------------------------------
class TamguConstFloat : public TamguBaseConst {
public:
	double value;

	TamguConstFloat(double v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_float, g, parent) {
     investigate |= is_number;}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->ProvideConstfloat(value);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->ProvideConstfloat(a->Float());
	}

    Tamgu* AtomNoConst() {
        return globalTamgu->Providefloat(value);
    }
    
    Tamgu* Atomref() {
        TamguReference* r = globalTamgu->Providefloat(value);
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_float;
	}

	short Typenumber() {
		return a_float;
	}

	

	bool isFloat() {
		return true;
	}

	string String() {
		return convertfromnumber(value);
	}

    string JSonString() {
        return convertfromnumber(value);
    }

	wstring UString() {
		return wconvertfromnumber(value);
	}
 
    void Setstring(string& v, short idthread) {
        convertnumber(value,v);
    }

    void Setstring(wstring& v, short idthread) {
        convertnumber(value,v);
    }


	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return (BLONG)value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	string Getstring(short idthread) {
		
		return convertfromnumber(value);
		
	}

	wstring Getustring(short idthread) {
		
		return wconvertfromnumber(value);
		
	}


	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return value;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
		return value;
	}

    bool Boolean() {
        return value;
    }

    unsigned char Byte() {
        return value;
    }


    unsigned char asByte() {
        return value;
    }

    long asInteger() {
        return value;
    }

    float asDecimal() {
        return (float)value;
    }

    double asFloat() {
        return value;
    }

    BLONG asLong() {
        return value;
    }

    short asShort() {
        return value;
    }


	Tamgu* Atom(bool forced = false) {
		return globalTamgu->ProvideConstfloat(value);
	}

	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat((long)value & a->Integer());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat((long)value | a->Integer());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat((long)value ^ a->Integer());
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat(value + a->Float());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat(value - a->Float());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat(value * a->Float());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);
		v = value / v;
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat((long)value << a->Integer());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		return globalTamgu->ProvideConstfloat((long)value >> a->Integer());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);

		v = (long)value % v;
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value < a->asFloat())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->asFloat())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        return booleantamgu[value == a->asFloat()];
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->asFloat())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->asFloat())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
        if (value >= a->asFloat())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		return globalTamgu->ProvideConstfloat(value + 1);
	}

	Tamgu* Pred() {
		return globalTamgu->ProvideConstfloat(value - 1);
	}
};

//--------------------------------------------------------------------

class TamguConstLong : public TamguBaseConst {
public:
	BLONG value;

	TamguConstLong(BLONG v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_long, g, parent) {
     investigate |= is_number;}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Providelong(value);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->Providelong(a->Long());
	}

    
    Tamgu* AtomNoConst() {
        return globalTamgu->Providelong(value);
    }

    Tamgu* Atomref() {
        TamguReference* r = globalTamgu->Providelong(value);
        r->reference = 1;
        r->protect = false;
        return r;
    }

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_long;
	}

	bool Checkprecision(Tamgu* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}

	short Typenumber() {
		return a_long;
	}

	

	bool isInteger() {
		return true;
	}

	bool isLong() {
		return true;
	}

	string String() {
		return convertfromnumber(value);
	}

    string JSonString() {
        return convertfromnumber(value);
    }


	wstring UString() {
		return wconvertfromnumber(value);
	}
 
    void Setstring(string& v, short idthread) {
        v = convertfromnumber(value);
    }

    void Setstring(wstring& v, short idthread) {
        v = wconvertfromnumber(value);
    }


	long Getinteger(short idthread) {
		return (long)value;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	short Getshort(short idthread) {
		return (short)value;
	}

	float Getdecimal(short idthread) {
		return (float)value;
	}

	double Getfloat(short idthread) {
		return (double)value;
	}

	string Getstring(short idthread) {
		
		return convertfromnumber(value);
		
	}

	wstring Getustring(short idthread) {
		
		return wconvertfromnumber(value);
		
	}

	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	BLONG Long() {
		return value;
	}

	short Short() {
		return value;
	}

    bool Boolean() {
        return value;
    }

    unsigned char Byte() {
        return value;
    }


    unsigned char asByte() {
        return value;
    }

    long asInteger() {
        return value;
    }

    float asDecimal() {
        return (float)value;
    }

    double asFloat() {
        return value;
    }

    BLONG asLong() {
        return value;
    }

    short asShort() {
        return value;
    }


	Tamgu* Atom(bool forced = false) {
		return globalTamgu->Providelong(value);
	}


	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value & a->Long());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value | a->Long());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value ^ a->Long());
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value + a->Long());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value - a->Long());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value * a->Long());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);
		v = (double)value / v;
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = (double)value;
		v = pow(v, a->Float());
		return globalTamgu->ProvideConstfloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value << a->Long());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		return globalTamgu->Providelong(value >> a->Long());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		BLONG v = a->Long();
		if (v == 0)
			return globalTamgu->Returnerror(e_error_divided_by);

		v = value % v;
		return globalTamgu->Providelong(v);
	}

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value < a->asLong())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value > a->asLong())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
        return booleantamgu[value == a->asLong()];
	}

	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif
		if (value != a->asLong())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value <= a->asLong())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->asLong())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		return globalTamgu->Providelong(value + 1);
	}

	Tamgu* Pred() {
		return globalTamgu->Providelong(value - 1);
	}

};

//--------------------------------------------------------------------

class TamguConstBool : public TamguBaseConst {
public:
	bool value;

	TamguConstBool(bool v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_boolean, g, parent) {}

    Tamgu* AtomNoConst() {
        return new Tamguboolean(value);
    }

	bool isBoolean() {
		return true;
	}

    short checkTypePredicate() {
        return (value?a_true:a_false);
    }

	long Getinteger(short idthread) {
		return value;
	}

    Tamgu* Atomref() {
        return this;
    }

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_boolean;
	}

	BLONG Getlong(short idthread) {
		return value;
	}

	short Getshort(short idthread) {
		return value;
	}

	float Getdecimal(short idthread) {
		return value;
	}

	double Getfloat(short idthread) {
		return value;
	}

	string Getstring(short idthread) {
		if (value)
			return "true";
		return "false";
	}

	wstring Getustring(short idthread) {
		if (value)
			return L"true";
		return L"false";
	}

	string String() {
		if (value)
			return "true";
		return "false";
	}

	wstring UString() {
		if (value)
			return L"true";
		return L"false";
	}

    string JSonString() {
        if (value)
            return "true";
        return "false";
    }

    void Setstring(string& v, short idthread) {
        if (value)
            v = "true";
        else
            v = "false";
    }

    void Setstring(wstring& v, short idthread) {
        if (value)
            v = L"true";
        else
            v = L"false";
    }


	long Integer() {
		return (long)value;
	}

	long Size() {
		return (long)value;
	}

	bool Boolean() {
		return value;
	}

	float Decimal() {
		return (float)value;
	}

	double Float() {
		return (double)value;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
		return value;
	}

    unsigned char Byte() {
        return value;
    }


    unsigned char asByte() {
        return value;
    }

    long asInteger() {
        return value;
    }

    float asDecimal() {
        return (float)value;
    }

    double asFloat() {
        return value;
    }

    BLONG asLong() {
        return value;
    }

    short asShort() {
        return value;
    }


	Tamgu* same(Tamgu* a) {
		if (value == a->Boolean())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
		if (value != a->Boolean())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		if (a->Boolean() == true)
			return aTRUE;
		return aFALSE;
	}

	Tamgu* plus(Tamgu* a, bool itself) {
		if (a->Boolean() || value)
			return aTRUE;

		return aFALSE;
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		if (value) {
			if (a->Boolean())
				return aFALSE;
			return aTRUE;
		}

		if (a->Boolean())
			return aTRUE;

		return aFALSE;
	}

	Tamgu* Succ() {
		if (value)
			return aFALSE;
		return aTRUE;
	}

	Tamgu* Pred() {
		if (value)
			return aFALSE;
		return aTRUE;
	}
};

#endif







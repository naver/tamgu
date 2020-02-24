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
        investigate = is_const;
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

	virtual bool baseValue() {
		return true;
	}

    bool Candelete() {
        return false;
    }
};

//---------------------------------------------------------------------------------
class TamguConst : public TamguBaseConst {
public:
	short id;
	string name;
	long value;

	TamguConst(short i, string n, TamguGlobal* g) : id(i), name(n), value(0), TamguBaseConst(a_const, g)  {
        if (i == a_null || i == a_empty)
            investigate = is_null;
    }

	string String() {
		return "";
	}

	wstring UString() {
		return L"";
	}

	string JSonString() {
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
		return "";
	}

	wstring Getustring(short idthread) {
		return L"";
	}

	long Integer() { return value; }
	double Float() { return value; }
	float Decimal() { return value; }
	BLONG Long() { return value; }
	short Short() { return value; }

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}


	Tamgu* Atom(bool forced = false) {
		if (this == aNOELEMENT)
			return aNULL;
		return this;
	}
};

class TamguConstBreak : public TamguConst {
public:

	TamguConstBreak(short i, string n, TamguGlobal* g) : TamguConst(i, n, g) {
        investigate = is_break;
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

	TamguConstString(string v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_string, g, parent) {}

	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	bool isString() {
		return true;
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
                return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
            return aNOELEMENT;
        }
        
        if (res == 1)
            idx = globalTamgu->Providestring(c_char_get(USTR(value), ileft));
        else
            idx = globalTamgu->Providestring(value.substr(ileft, iright - ileft));
        return idx;
    }


	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Providestring(value);
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

	string Getstring(short idthread) {
		return value;
	}

	wstring Getustring(short idthread) {
		wstring res;
		s_utf8_to_unicode(res, USTR(value), value.size());
		return res;
	}

	long Integer() {
		return (long)conversionintegerhexa(STR(value));
	}

    long CommonSize() {
        return size_c(value);
    }

	long Size() {
		return (long)value.size();
	}

	float Decimal() {
		return convertfloat(STR(value));
	}

	double Float() {
		return convertfloat(STR(value));
	}

	bool Boolean() {
		if (value == "")
			return false;
		return true;
	}

	BLONG Long() {
		return conversionintegerhexa(STR(value));
	}

	short Short() {
		return (short)conversionintegerhexa(STR(value));
	}

	Tamgu* Atom(bool forced = false) {
		return globalTamgu->Providestring(value);
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
		return globalTamgu->Providestring(s);
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
		return globalTamgu->Providestring(v);
	}

	Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value < a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value > a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value == a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aTRUE;
#endif
		if (value != a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value <= a->String())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value >= a->String())
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

	TamguConstUString(wstring v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_ustring, g, parent) {}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	bool isString() {
		return true;
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
               return globalTamgu->Returnerror("Wrong key in a container or a string access", idthread);
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
		return globalTamgu->Provideustring(value);
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

#ifdef WSTRING_IS_UTF16
    long CommonSize() {
        return size_w(value);
    }
#else
    long CommonSize() {
        return value.size();
    }
#endif
    
	string Getstring(short idthread) {
		string res;
		s_unicode_to_utf8(res, value);
		return res;
	}

	wstring Getustring(short idthread) {
		return value;
	}

	long Integer() {
		return (long)conversionintegerhexa(STR(value));
	}

	long Size() {
		return (long)value.size();
	}

	float Decimal() {
		return convertfloat(STR(value));
	}

	double Float() {
		return convertfloat(STR(value));
	}

	bool Boolean() {
		if (value == L"")
			return false;
		return true;
	}

	BLONG Long() {
		return conversionintegerhexa(STR(value));
	}

	short Short() {
		return (short)conversionintegerhexa(STR(value));
	}

	Tamgu* Atom(bool forced = false) {
		return globalTamgu->Provideustring(value);
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
		return globalTamgu->Provideustring(s);
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
		return globalTamgu->Provideustring(v);
	}


	Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value < a->UString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value > a->UString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value == a->UString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aTRUE;
#endif
		if (value != a->UString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value <= a->UString())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isString())
            return aFALSE;
#endif
		if (value >= a->UString())
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

	TamguConstInt(long v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_int, g, parent) {}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	bool Checkprecision(Tamgu* r) {
		if (idtype < r->Typenumber())
			return false;

		return true;
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

	bool isNumber() {
		return true;
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

	long Integer() {
		return value;
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
			return globalTamgu->Returnerror("Error: Divided by 0");
		v = (double)value / v;
		return globalTamgu->Providefloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalTamgu->Providefloat(v);
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
			return globalTamgu->Returnerror("Error: Divided by 0");

		v = value % v;
		return globalTamgu->Provideint(v);
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

	TamguConstShort(short v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_short, g, parent) {}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return globalTamgu->Provideint(value);
	}

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_short;
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
        return globalTamgu->Provideint(a->Integer());
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

	bool isNumber() {
		return true;
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

	long Integer() {
		return value;
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
            return new Tamgulong(v);
        }
        return a->plus(this, itself);
	}

	Tamgu* minus(Tamgu* a, bool itself) {
        if (a->Type() == a_short) {
            BLONG v = value;
            v -= a->Long();
            return new Tamgulong(v);
        }
        a = a->minus(this, itself);
        a->multiply(aMINUSONE, true);
        return a;
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
        if (a->Type() == a_short) {
            BLONG v = value;
            v *= a->Long();
            return new Tamgulong(v);
        }
        return a->multiply(this, itself);
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		v = (double)value / v;
		return globalTamgu->Providefloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalTamgu->Providefloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
        BLONG v = value;
        v <<= a->Long();
        return new Tamgulong(v);
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
        BLONG v = value;
        v >>= a->Long();
        return new Tamgulong(v);
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		BLONG v = value;
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");

		v %= a->Long();
		return new Tamgulong(v);
	}

	Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value < a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value == a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->Short())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->Short())
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

	TamguConstDecimal(double v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_decimal, g, parent) {}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	bool Checkprecision(Tamgu* r) {
		if (r->Type() >= a_float)
			return false;
		return true;
	}
	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgudecimal(value);
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

	bool isNumber() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	string String() {
		
		return convertfromnumber(value);
		
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

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
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
			return globalTamgu->Returnerror("Error: Divided by 0");
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
			return globalTamgu->Returnerror("Error: Divided by 0");

		v = (long)value % v;
		return new Tamgudecimal(v);
	}

	Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value < a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value == a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->Decimal())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->Decimal())
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

	TamguConstFloat(double v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_float, g, parent) {}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return globalTamgu->Providefloat(value);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return globalTamgu->Providefloat(a->Float());
	}

    string Info(string n);
    void Methods(Tamgu* vs);

	short Typeinfered() {
		return a_float;
	}

	short Typenumber() {
		return a_float;
	}

	bool isNumber() {
		return true;
	}

	bool isFloat() {
		return true;
	}

	string String() {
		
		return convertfromnumber(value);
		
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

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return (BLONG)value;
	}

	short Short() {
		return value;
	}

	Tamgu* Atom(bool forced = false) {
		return globalTamgu->Providefloat(value);
	}

	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat((long)value & a->Integer());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat((long)value | a->Integer());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat((long)value ^ a->Integer());
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat(value + a->Float());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat(value - a->Float());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat(value * a->Float());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		v = value / v;
		return globalTamgu->Providefloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = value;
		v = pow(v, a->Float());
		return globalTamgu->Providefloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat((long)value << a->Integer());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		return globalTamgu->Providefloat((long)value >> a->Integer());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		long v = a->Integer();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");

		v = (long)value % v;
		return globalTamgu->Providefloat(v);
	}

	Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value < a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value > a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value == a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aTRUE;
#endif

        if (value != a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif

        if (value <= a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
        if (value >= a->Float())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		return globalTamgu->Providefloat(value + 1);
	}

	Tamgu* Pred() {
		return globalTamgu->Providefloat(value - 1);
	}
};

//--------------------------------------------------------------------

class TamguConstLong : public TamguBaseConst {
public:
	BLONG value;

	TamguConstLong(BLONG v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_long, g, parent) {}
	Exporting Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgulong(value);
	}

	Tamgu* Newvalue(Tamgu* a, short idthread) {
		return new Tamgulong(a->Long());
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

	bool isNumber() {
		return true;
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

	wstring UString() {
		
		return wconvertfromnumber(value);
		
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

	bool Boolean() {
		if (value == 0)
			return false;
		return true;
	}

	BLONG Long() {
		return value;
	}

	short Short() {
		return value;
	}

	Tamgu* Atom(bool forced = false) {
		return new Tamgulong(value);
	}


	//we add the current value with a
	Tamgu* andset(Tamgu* a, bool itself) {
		return new Tamgulong(value & a->Long());
	}

	Tamgu* orset(Tamgu* a, bool itself) {
		return new Tamgulong(value | a->Long());
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
		return new Tamgulong(value ^ a->Long());
	}


	Tamgu* plus(Tamgu* a, bool itself) {
		return new Tamgulong(value + a->Long());
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		return new Tamgulong(value - a->Long());
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		return new Tamgulong(value * a->Long());
	}

	Tamgu* divide(Tamgu* a, bool itself) {
		double v = a->Float();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");
		v = (double)value / v;
		return globalTamgu->Providefloat(v);
	}

	Tamgu* power(Tamgu* a, bool itself) {
		double v = (double)value;
		v = pow(v, a->Float());
		return globalTamgu->Providefloat(v);
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
		return new Tamgulong(value << a->Long());
	}

	Tamgu* shiftright(Tamgu* a, bool itself) {
		return new Tamgulong(value >> a->Long());
	}

	Tamgu* mod(Tamgu* a, bool itself) {
		BLONG v = a->Long();
		if (v == 0)
			return globalTamgu->Returnerror("Error: Divided by 0");

		v = value % v;
		return new Tamgulong(v);
	}

	Tamgu* less(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value < a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* more(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value > a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* same(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value == a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* different(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aTRUE;
#endif
		if (value != a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value <= a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
#ifndef TAMGULOOSECOMPATIBILITIES
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->Long())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* Succ() {
		return new Tamgulong(value + 1);
	}

	Tamgu* Pred() {
		return new Tamgulong(value - 1);
	}

};

//--------------------------------------------------------------------

class TamguConstBool : public TamguBaseConst {
public:
	bool value;

	TamguConstBool(bool v, TamguGlobal* g = NULL, Tamgu* parent = NULL) : value(v), TamguBaseConst(a_boolean, g, parent) {}

	bool isBoolean() {
		return true;
	}

	long Getinteger(short idthread) {
		return value;
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







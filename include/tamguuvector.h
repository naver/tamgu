/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguuvector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguuvector_h
#define tamguuvector_h

#include "tamguint.h"
#include "tamguustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguuvector;
//This typedef defines a type "uvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguuvector::*uvectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamguuvector : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<uvectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<wstring> values;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Tamguuvector(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
     investigate |= is_string;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    Tamguuvector() {
     investigate |= is_string;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    //----------------------------------------------------------------------------------------------------------------------

    void SetConst() {
        isconst = true;
    }

    Exporting Tamgu* Put(Tamgu* value, Tamgu* v, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign);
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    short Type() {
        return a_uvector;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "uvector";
    }

    

    bool isContainerClass() {
        return true;
    }

    bool isValueContainer() {
        return true;
    }

    bool isVectorValueContainer() {
        return true;
    }

    bool isVectorContainer() {
        return true;
    }

    bool isMapContainer() {
        return false;
    }

    bool duplicateForCall() {
        return isconst;
    }

    void Reserve(long d) {
        Locking _lock(this);
        if (d > values.size())
            values.reserve(d);
    }

    void storevalue(long l) {
        
        wstring s = wconvertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(double l) {
        
        wstring s = wconvertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(float l) {
        
        wstring s = wconvertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(short l) {
        
        wstring s = wconvertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(BLONG l) {
        
        wstring s = wconvertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    

    void Storevalue(string& s) {
        wstring v;
        sc_utf8_to_unicode(v, USTR(s), s.size());
        Locking* _lock = _getlock(this);
        values.push_back(v);
        _cleanlock(_lock);
    }
    
    void Storevalue(wstring& v) {
        Locking* _lock = _getlock(this);
        values.push_back(v);
        _cleanlock(_lock);
    }
    

    void storevalue(string s) {
        wstring v;
        sc_utf8_to_unicode(v, USTR(s), s.size());
        Locking* _lock = _getlock(this);
        values.push_back(v);
        _cleanlock(_lock);
    }

    void storevalue(wstring v) {
        Locking* _lock = _getlock(this);
        values.push_back(v);
        _cleanlock(_lock);
    }

    void storevalue(wchar_t v) {
        Locking _lock(this);
        wstring w;
        w = v;
        values.push_back(w);
    }

    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamguuvector* v = globalTamgu->Provideuvector();
            Locking _lock(this);
            v->values = values;
            return v;
        }
        return this;
    }

    Exporting Tamgu* getvalue(BLONG i);
    
    string getstring(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return "";
        }
        string v;
        s_unicode_to_utf8(v, values[i]);
        _cleanlock(_lock);
        return v;
    }

    wstring getustring(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return L"";
        return values[i];
    }
    

    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Long());
    }


    double getfloat(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return 0;
        }
        double v = convertfloat(values[i]);
        _cleanlock(_lock);
        return v;
    }
    
    long getinteger(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return 0;
        }
        long v = convertlong(values[i]);
        _cleanlock(_lock);
        return v;
    }
    
    float getdecimal(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return 0;
        }
        float v = convertfloat(values[i]);
        _cleanlock(_lock);
        return v;
    }
    
    BLONG getlong(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return 0;
        }
        BLONG v = convertlong(values[i]);
        _cleanlock(_lock);
        return v;
    }

    uchar getbyte(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i][0];
    }

    short getshort(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return 0;
        }
        short v = convertlong(values[i]);
        _cleanlock(_lock);
        return v;
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return globalTamgu->Provideuvector();
    }

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamguuvector* v = globalTamgu->Provideuvector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valueustring());
            }
            it->Release();
            return v;
        }
        wstring val=a->UString();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }

    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, uvectorMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);

    
    void Methods(Tamgu* v) {
        for (auto& it : infomethods)
            v->storevalue(it.first);
    }

    string Info(string n) {
        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodMin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m > values[i])
                m = values[i];
        }
        return globalTamgu->Provideustring(m);
    }
    
    Tamgu* MethodMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m < values[i])
                m = values[i];
        }
        return globalTamgu->Provideustring(m);
    }
    
    Tamgu* MethodMinMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring mn = values[0];
        wstring mx = mn;
        for (long i = 1; i < values.size(); i++) {
            if (mn > values[i])
                mn = values[i];
            if (mx < values[i])
                mx = values[i];
        }
        Tamguuvector* b = new Tamguuvector;
        b->values.push_back(mn);
        b->values.push_back(mx);
        return b;
    }
    

    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    
    Tamgu* MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodConvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring a = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
                return aTRUE;
            }
        }
        return aFALSE;
    }

    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->UString());
        return aTRUE;
    }

    Tamgu* MethodPoplast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Poplast();
    }
    

    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Tamgu* MethodNGrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring u;
        wstring sep;
        long num =callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        
        if (callfunc->Size() == 2)
            sep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
            
        Tamgu* kvect = Selectauvector(contextualpattern);
        long mx = values.size() - num + 1;
        for (long i = 0; i < mx; i++) {
            u = L"";
            for (long j = i; j < i + num; j++) {
                if (j != i)
                    u += sep;
                u += values[j];
            }
            kvect->storevalue(u);
        }
        return kvect;
    }

    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return globalTamgu->Provideustring(values.back());
    }

    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Thesum(0, values.size());

    }

    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }

    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        unsigned long dst = EditDistance(v);
        return globalTamgu->ProvideConstint(dst);
    }

    Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Tamgu* v = callfunc->Evaluate(1, contextualpattern, idthread);
        Insert(i, v);
        return aTRUE;
    }

    Tamgu* MethodUnique(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Unique();
    }

    Tamgu* MethodReserve(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        long sz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Reserve(sz);
        return aTRUE;
    }

    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        wstring sep = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        Locking _lock(this);
        bool beg = true;
        wstring res;
        for (size_t it = 0; it<values.size(); it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it];
        }

        return globalTamgu->Providewithustring(res);
    }

    Tamgu* MethodShuffle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Shuffle();
        return this;
    }

    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }

    Tamgu* MethodPermute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return booleantamgu[Permute()];
    }

    Tamgu* MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------
    Exporting void addstringto(string s, int i);
    Exporting void addstringto(wchar_t s, int i);
    Exporting void addustringto(wstring ws, int i);
    Exporting Tamgu* Push(Tamgu*);	Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        Locking _lock(this);
        values.push_back(a->UString());
        return this;
    }
    
    Exporting Tamgu* Pop(Tamgu*);
    
    Tamgu* Poplast() {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        wstring c = values.back();
        values.pop_back();
        return globalTamgu->Providewithustring(c);
    }

    Exporting Tamgu* Unique();
    Exporting void Sort(bool);
    Exporting void Shuffle();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    Exporting bool Permute();
    Exporting unsigned long EditDistance(Tamgu* e);

    void Insert(long idx, Tamgu* ke) {
        Locking _lock(this);
        if (idx<0)
            idx=0;
        if (idx>=values.size())
            values.push_back(ke->UString());
        else
            values.insert(values.begin() + idx, ke->UString());
    }

    Tamgu* Inverse() {
        Locking _lock(this);
        Tamguuvector* vect = globalTamgu->Provideuvector();
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        return vect;
    }

    Exporting Tamgu* Map(short idthread);Exporting Tamgu* Vector(short idthread);
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Tamgu* Thesum(long i, long j) {
        long sz = values.size();
        if (!sz)
            return aNULL;

        if (i < 0)
            i += sz;
        if (j < 0)
            j += sz;

        if (i < 0)
            i = 0;
        else
            if (i >= sz)
                i = sz - 1;

        if (j <= i)
            j = i + 1;
            
        j++;
        if (j>sz)
            j = sz;

        wstring v;
        for (; i < j; i++)
            v += values[i];

        return globalTamgu->Providewithustring(v);
    }

    Exporting void Clear();
    Exporting string JSonString();
    
    Exporting string String();
    Exporting wstring UString();
    Exporting void Setstring(string& v, short idthread);
    Exporting void Setstring(wstring&, short idthread);
    
    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();

    //Basic operations
    Exporting long Size();
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    Exporting Tamgu* andset(Tamgu* a, bool itself);Exporting Tamgu* orset(Tamgu* a, bool itself);Exporting Tamgu* xorset(Tamgu* a, bool itself);Exporting Tamgu* plus(Tamgu* a, bool itself);Exporting Tamgu* minus(Tamgu* a, bool itself);
    Exporting Tamgu* same(Tamgu* a);
};

class TamguIterationuvector : public TamguIteration {
    public:

    BLONG itx;
    Tamguuvector* ref;
    Locking _lock;

    TamguIterationuvector(Tamguuvector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Tamgu* Key() {
        return globalTamgu->ProvideConstint(itx);
    }

    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(ref->values[itx]);
    }

    Tamgu* Value() {
        return globalTamgu->Provideustring(ref->values[itx]);
    }

    string Keystring() {
        
        return convertfromnumber(itx);
        
    }

    wstring Valueustring() {
        return ref->values[itx];
    }

    string Valuestring() {
        string s;
        s_unicode_to_utf8(s, ref->values[itx]);
        return s;
    }

    long Keyinteger() {
        return itx;
    }

    long Valueinteger() {
        string v;
        sc_unicode_to_utf8(v, ref->values[itx]);
        return conversionintegerhexa(STR(v));
    }

    BLONG Keylong() {
        return itx;
    }

    BLONG Valuelong() {
        string v;
        sc_unicode_to_utf8(v, ref->values[itx]);
        return conversionintegerhexa(STR(v));
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        string v;
        sc_unicode_to_utf8(v, ref->values[itx]);
        return convertfloat(STR(v));
    }

    double Valuefloat() {
        string v;
        sc_unicode_to_utf8(v, ref->values[itx]);
        return convertfloat(STR(v));
    }

    void Next() {
        if (reverse)
            itx--;
        else
            itx++;
    }

    Tamgu* End() {
        if (reverse) {
            if (itx < 0)
                return aTRUE;
        }
        else {
            if (itx >= ref->values.size())
                return aTRUE;
        }
        return aFALSE;
    }

    Tamgu* Begin() {
        if (reverse) {
            itx = ref->values.size() - 1;
        }
        else
            itx = 0;
        return aTRUE;
    }


};

//---------------------------------------------------------------------------------------------------------------------
class Tamguuvectorbuff : public Tamguuvector {
    public:
    long idx;
    bool used;

    Tamguuvectorbuff(long i)  {
        //Do not forget your variable initialisation
        idx = i;
        used = false;
    }

    bool Candelete() {
        return false;
    }

    void Resetreference(short r) {
        r = reference - r;
        if (r <= 0) {
            reference.store(0);
            if (!protect) {
                protect = true;

                values.clear();
                used = false;
                if (!globalTamgu->threadMODE)
                    globalTamgu->uvectorempties.push_back(idx);
            }
        }
        else
            reference.store(r);
    }

};
//---------------------------------------------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------------------------------------------
    //We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
    //of how to declare a new method.
class Tamgua_uvector;
    //This typedef defines a type "a_uvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgua_uvector::*a_uvectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------


class Tamgua_uvector : public TamguContainer {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<a_uvectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
	atomic_wstring zero;
    atomic_value_vector<atomic_wstring> values;
    bool isconst;
        //---------------------------------------------------------------------------------------------------------------------
    Tamgua_uvector(TamguGlobal* g, Tamgu* parent = NULL) : values(zero), TamguContainer(g, parent) {
     investigate |= is_string;
            //Do not forget your variable initialisation
        isconst = false;
    }
    
    Tamgua_uvector() : values(zero) {
     investigate |= is_string;
            //Do not forget your variable initialisation
        isconst = false;
    }
    
        //----------------------------------------------------------------------------------------------------------------------
    
    void SetConst() {
        isconst = true;
    }
    
    void pushback(wstring s) {
        values.push_back(s);
    }
    
    Exporting virtual Tamgu* Put(Tamgu* i, Tamgu* value, short idthread);Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    
    short Type() {
        return Tamgua_uvector::idtype;
    }
    
	void Setidtype(TamguGlobal* global) {
		Tamgua_uvector::InitialisationModule(global, "");
	}

    string Typename() {
        return "a_uvector";
    }
    
    
    
    bool isContainerClass() {
        return true;
    }
    
    bool isValueContainer() {
        return true;
    }
    
    bool isVectorValueContainer() {
        return true;
    }
    
    bool isVectorContainer() {
        return true;
    }
    
    bool isMapContainer() {
        return false;
    }
    
    
    void storevalue(long l) {
        wstring s = wconvertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(double l) {
        wstring s = wconvertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(float l) {
        wstring s = wconvertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(short l) {
        wstring s = wconvertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(BLONG l) {
        wstring s = wconvertfromnumber(l);
        values.push_back(s);
    }
    
    void Storevalue(wstring& v) {
        values.push_back(v);
    }
    
    void Storevalue(string& v) {
        wstring s;
        s_utf8_to_unicode(s,USTR(v), v.size());
        values.push_back(s);
    }
    
    void storevalue(wstring v) {
        values.push_back(v);
    }
    
    void storevalue(string v) {
        wstring s;
        s_utf8_to_unicode(s,USTR(v), v.size());
        values.push_back(s);
    }
    
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgua_uvector* v = new Tamgua_uvector();
            v->values = values;
            return v;
        }
        return this;
    }
    
    Exporting Tamgu* getvalue(BLONG i);
    
    string getstring(long i) {
        if (i < 0 || i >= values.size())
            return "";
        string v;
        wstring w = values[i].value();
        s_unicode_to_utf8(v, w);
        return v;
    }
    
    wstring getustring(long i) {
        if (i < 0 || i >= values.size()) {
            return L"";
        }
        return values[i].value();
    }
    
    
    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Long());
    }
    
    
    double getfloat(long i) {
        if (i < 0 || i >= values.size()) {
            return 0;
        }
        return convertfloat(values[i].value());
    }
    
    long getinteger(long i) {
        if (i < 0 || i >= values.size()) {
            return 0;
        }
        return convertlong(values[i].value());
    }
    
    float getdecimal(long i) {
        if (i < 0 || i >= values.size()) {
            return 0;
        }
        return convertfloat(values[i].value());
    }
    
    BLONG getlong(long i) {
        if (i < 0 || i >= values.size()) {
            return 0;
        }
        return convertlong(values[i].value());
    }
    
    uchar getbyte(long i) {
        if (i < 0 || i >= values.size())
            return 0;
        return values[i].value()[0];
    }
    
    short getshort(long i) {
        if (i < 0 || i >= values.size()) {
            return 0;
        }
        return convertlong(values[i].value());
    }
    
        //---------------------------------------------------------------------------------------------------------------------
        //Declaration
        //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }
    
    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgua_uvector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgua_uvector* v = new Tamgua_uvector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            wstring s;
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                s = it->Valueustring();
                v->values.push_back(s);
            }
            it->Release();
            return v;
        }
        wstring val=a->UString();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }
    
    bool duplicateForCall() {
        return isconst;
    }
    
    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);
    
    static void AddMethod(TamguGlobal* g, string name, a_uvectorMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);
    
    
    void Methods(Tamgu* v) {
        for (auto& it : infomethods)
            v->storevalue(it.first);
    }

    string Info(string n) {
        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
        return "Unknown method";
    }
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring a = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        atomic_value_vector_iterator<atomic_wstring> it(values);
        for (; !it.end(); it.next()) {
            if (it.second.value() == a) {
                values.pop(it.first);
                return aTRUE;
            }
        }
        return aFALSE;
    }
    
    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring s;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            s = callfunc->Evaluate(i, contextualpattern, idthread)->UString();
            values.push_back(s);
        }
        return aTRUE;
    }
    
    Tamgu* MethodNGrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        wstring u;
        wstring sep;
        long num =callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        
        if (callfunc->Size() == 2)
            sep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();
        
        Tamgu* kvect = Selectauvector(contextualpattern);
        long mx = values.size() - num + 1;
        for (long i = 0; i < mx; i++) {
            u = L"";
            for (long j = i; j < i + num; j++) {
                if (j != i)
                    u += sep;
                u += values[j].value();
            }
            kvect->storevalue(u);
        }
        return kvect;
    }
    
    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (values.size() == 0)
            return aNOELEMENT;
        return globalTamgu->Provideustring(values.back().value());
    }
    
    Tamgu* MethodPoplast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Poplast();
    }
    
    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }
    
    Tamgu* MethodReserve(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //The separator between values
        long sz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Reserve(sz);
        return aTRUE;
    }
    
    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Thesum(0, values.size());
        
    }
    
    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //The separator between values
        wstring sep = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        bool beg = true;
        wstring res;
        atomic_value_vector_iterator<atomic_wstring> it(values);
        for (; !it.end(); it.next()) {
            if (beg == false)
                res += sep;
            beg = false;
            res += it.second.value();
        }
        
        return globalTamgu->Providewithustring(res);
    }
    
    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }
    
    Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Tamgu* v = callfunc->Evaluate(1, contextualpattern, idthread);
        Insert(i, v);
        return aTRUE;
    }
    
    Tamgu* MethodUnique(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Unique();
    }
    
    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }
    
        //---------------------------------------------------------------------------------------------------------------------
    Exporting void addstringto(string s, int i);
    Exporting void addstringto(wchar_t s, int i);
    Exporting void addustringto(wstring ws, int i);
    
    Exporting Tamgu* Push(Tamgu*);    Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        pushback(a->UString());
        return this;
    }
    
    
    Tamgu* Poplast() {
        if (values.size() == 0)
            return aNOELEMENT;
        return globalTamgu->Provideustring(values.remove_back().value());
    }
    

    Exporting Tamgu* Pop(Tamgu*);
    Exporting Tamgu* Unique();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    
    void Insert(long idx, Tamgu* ke) {
        if (idx<0)
            idx=0;
        if (idx>=values.size())
            pushback(ke->UString());
        else {
            wstring s = ke->UString();
            values.insert(idx, s);
        }
    }
    
    Tamgu* Inverse() {
        Tamgua_uvector* vect = new Tamgua_uvector();
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        return vect;
    }
    
    Exporting Tamgu* Map(short idthread);Exporting Tamgu* Vector(short idthread);
        //---------------------------------------------------------------------------------------------------------------------
    
        //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
        //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
            //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }
    
    Tamgu* Thesum(long i, long j) {
        long sz = values.size();
        if (!sz)
            return aNULL;
        
        if (i < 0)
            i += sz;
        if (j < 0)
            j += sz;
        
        if (i < 0)
            i = 0;
        else
            if (i >= sz)
                i = sz - 1;
        
        if (j <= i)
            j = i + 1;
        
        j++;
        if (j>sz)
            j = sz;
        
        wstring v;
        for (; i < j; i++)
            v += values[i].value();
        
        return globalTamgu->Providewithustring(v);
    }
    
    Exporting void Clear();
    Exporting string String();
    Exporting string JSonString();
    Exporting wstring UString();
    
    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();
    
        //Basic operations
    Exporting long Size();
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);Exporting Tamgu* andset(Tamgu* a, bool itself);Exporting Tamgu* orset(Tamgu* a, bool itself);Exporting Tamgu* xorset(Tamgu* a, bool itself);Exporting Tamgu* plus(Tamgu* a, bool itself);Exporting Tamgu* minus(Tamgu* a, bool itself);
    Exporting Tamgu* same(Tamgu* a);
};

class TamguIterationa_uvector : public TamguIteration {
public:
    
    
    Tamgua_uvector* ref;
    atomic_value_vector_iterator<atomic_wstring> it;
    
    TamguIterationa_uvector(Tamgua_uvector* v, bool d, TamguGlobal* g = NULL) : it(v->values), TamguIteration(d, g) {
        ref = v;
    }
    
    Tamgu* Key() {
        return globalTamgu->ProvideConstint(it.first);
    }
    
    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it.second.value());
    }
    
    Tamgu* Value() {
        return globalTamgu->Provideustring(it.second.value());
    }
    
    string Keystring() {
        return convertfromnumber(it.first);
        
    }
    
    string Valuestring() {
        string s;
        wstring w = it.second.value();
        s_unicode_to_utf8(s, w);
        return s;
    }
    
    wstring Valueustring() {
        return it.second.value();
    }
    
    long Keyinteger() {
        return it.first;
    }
    
    BLONG Keylong() {
        return it.first;
    }
    
    BLONG Valuelong() {
        return conversionintegerhexa(STR(it.second.value()));
    }
    
    long Valueinteger() {
        return (long)conversionintegerhexa(STR(it.second.value()));
    }
    
    double Keyfloat() {
        return it.first;
    }
    
    float Valuedecimal() {
        return convertfloat(STR(it.second.value()));
    }
    
    double Valuefloat() {
        return convertfloat(STR(it.second.value()));
    }
    
    void Next() {
        it.next();
    }
    
    Tamgu* End() {
        return booleantamgu[it.end()];
    }
    
    Tamgu* Begin() {
        it.begin(ref->values);
        return aTRUE;
    }
};


#endif

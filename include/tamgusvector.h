/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusvector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgusvector_h
#define tamgusvector_h

#include "tamguint.h"
#include "tamgustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgusvector;
//This typedef defines a type "svectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgusvector::*svectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Tamgusvector : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<svectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<string> values;
    bool isconst;
    //---------------------------------------------------------------------------------------------------------------------
    Tamgusvector(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
        //Do not forget your variable initialisation
        isconst = false; 
    }

    Tamgusvector()  {
        //Do not forget your variable initialisation
        isconst = false; 
    }

    //----------------------------------------------------------------------------------------------------------------------

    void SetConst() {
        isconst = true;
    }

    Exporting virtual Tamgu* Put(Tamgu* i, Tamgu* value, short idthread);Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

    short Type() {
        return Tamgusvector::idtype;
    }

    

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "svector";
    }

    bool isString() {
        return true;
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


    void Reserve(long d) {
        Locking _lock(this);
        if (d>values.size())
            values.reserve(d);
    }

    void storevalue(long l) {
        
        string s = convertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(double l) {
        
        string s = convertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(float l) {
        
        string s = convertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(short l) {
        
        string s = convertfromnumber(l);
        Locking _lock(this);
        values.push_back(s);
    }
    
    void storevalue(BLONG l) {
        
        string s = convertfromnumber(l);
        Locking* _lock = _getlock(this);
        values.push_back(s);
        _cleanlock(_lock);
    }
    
    void Storevalue(string& v) {
        Locking* _lock = _getlock(this);
        values.push_back(v);
        _cleanlock(_lock);
    }
    
    void Storevalue(wstring& v) {
        string s;
        sc_unicode_to_utf8(s,v);
        Locking* _lock = _getlock(this);
        values.push_back(s);
        _cleanlock(_lock);
    }

    void storevalue(string v) {
        Locking* _lock = _getlock(this);
        values.push_back(v);
        _cleanlock(_lock);
    }
    
    void storevalue(wstring v) {
        string s;
        sc_unicode_to_utf8(s,v);
        Locking* _lock = _getlock(this);
        values.push_back(s);
        _cleanlock(_lock);
    }
    

    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgusvector* v = globalTamgu->Providesvector();
            Locking _lock(this);
            v->values = values;
            return v;
        }
        return this;
    }

    Exporting Tamgu* getvalue(BLONG i);

    wstring getustring(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return L"";
        }
        wstring v;
        s_utf8_to_unicode(v, USTR(values[i]), values[i].size());
        _cleanlock(_lock);
        return v;
    }
    
    string getstring(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size()) {
            return "";
        }
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
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return 0;
        }
        uchar vv = values[i][0];
        _cleanlock(_lock);
        return vv;
    }

    short getshort(long i) {
        Locking* _lock = _getlock(this);
        if (i < 0 || i >= values.size()) {
            _cleanlock(_lock);
            return 0;
        }
        short vv = convertlong(values[i]);
        _cleanlock(_lock);
        return vv;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newpureinstance(short idthread) {
        return new Tamgusvector;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return globalTamgu->Providesvector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgusvector* v = globalTamgu->Providesvector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valuestring());
            }
            it->Release();
            return v;
        }
        string val=a->String();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }

    bool duplicateForCall() {
        return isconst;
    }

    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, svectorMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);

    
    void Methods(Tamgu* v) {
        string s;
        
        for (auto& it : infomethods) {
            s=it.first;
            v->storevalue(s);
        }
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
        string a = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
                return aTRUE;
            }
        }
        return aFALSE;
    }

    Tamgu* MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodPermute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return booleantamgu[Permute()];
    }

    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->String());
        return aTRUE;
    }

    Tamgu* MethodNGrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        string u;
        string sep;
        long num =callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        
        if (callfunc->Size() == 2)
            sep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
            
        Tamgu* kvect = Selectasvector(contextualpattern);
        long mx = values.size() - num + 1;
        for (long i = 0; i < mx; i++) {
            u = "";
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
        return globalTamgu->Providestring(values.back());
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
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        Locking _lock(this);
        bool beg = true;
        string res;
        for (size_t it = 0; it < values.size(); it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it];
        }

        return globalTamgu->Providestring(res);
    }

    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }

    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        unsigned long dst = EditDistance(v);
        return globalTamgu->Provideint(dst);
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

    Tamgu* MethodShuffle(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Shuffle();
        return this;
    }
    
    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }

    Tamgu* MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------
    Exporting void addstringto(string s, int i);
    Exporting void addstringto(wchar_t s, int i);
    Exporting void addustringto(wstring ws, int i);

    Exporting Tamgu* Push(Tamgu*);	Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        Locking _lock(this);
        values.push_back(a->String());
        return this;
    }

    Tamgu* Poplast() {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        string c = values.back();
        values.pop_back();
        return globalTamgu->Providestring(c);
    }
    


    Exporting Tamgu* Pop(Tamgu*);
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
            values.push_back(ke->String());
        else
            values.insert(values.begin() + idx, ke->String());
    }

    Tamgu* Inverse() {
        Tamgusvector* vect = globalTamgu->Providesvector();
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

        string v;
        for (; i < j; i++)
            v += values[i];

        return globalTamgu->Providestring(v);
    }

    Exporting void Clear();
    Exporting string String();Exporting string JSonString();
    //wstring UString() {}

    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();
    
    //Basic operations
    Exporting long Size();
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);Exporting Tamgu* andset(Tamgu* a, bool itself);Exporting Tamgu* orset(Tamgu* a, bool itself);Exporting Tamgu* xorset(Tamgu* a, bool itself);Exporting Tamgu* plus(Tamgu* a, bool itself);Exporting Tamgu* minus(Tamgu* a, bool itself);
    Exporting Tamgu* same(Tamgu* a);
};

class TamguIterationsvector : public TamguIteration {
    public:


    BLONG itx;
    Tamgusvector* ref;
    Locking _lock;

    TamguIterationsvector(Tamgusvector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Tamgu* Key() {
        return globalTamgu->Provideint(itx);
    }

    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(ref->values[itx]);
    }

    Tamgu* Value() {
        return globalTamgu->Providestring(ref->values[itx]);
    }

    string Keystring() {
        
        return convertfromnumber(itx);
        
    }

    string Valuestring() {
        return ref->values[itx];
    }

    long Keyinteger() {
        return itx;
    }

    BLONG Keylong() {
        return itx;
    }

    BLONG Valuelong() {
        return conversionintegerhexa(STR(ref->values[itx]));
    }

    long Valueinteger() {
        return (long)conversionintegerhexa(STR(ref->values[itx]));
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        return convertfloat(STR(ref->values[itx]));
    }

    double Valuefloat() {
        return convertfloat(STR(ref->values[itx]));
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

class TamguConstSVector : public Tamgusvector {
    public:

    TamguConstSVector() : Tamgusvector() {
        //Do not forget your variable initialisation
        investigate |= is_const;
    }
    
    void Setreference(short) {}
    void Setreference() {}
    virtual void Resetreference(short r = 1) {}
    virtual void Release() {}

    Tamgu* Put(Tamgu* context, Tamgu* value, short idthread) {
        return aTRUE;
    }

};
//---------------------------------------------------------------------------------------------------------------------
class Tamgusvectorbuff : public Tamgusvector {
    public:
    long idx;
    bool used;

    Tamgusvectorbuff(long i)  {
        //Do not forget your variable initialisation
        idx = i;
        used = false;
    }

    bool Candelete() {
        return false;
    }

    void Resetreference(short r) {
        reference -= r;
        if (reference <= 0) {
            if (!protect) {
                reference = 0;
                protect = true;
                
                values.clear();
                used = false;
                if (!globalTamgu->globalLOCK)
                    globalTamgu->svectorempties.push_back(idx);
            }
        }
    }
};
//---------------------------------------------------------------------------------------------------------------------
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgua_svector;
    //This typedef defines a type "a_svectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgua_svector::*a_svectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------


class Tamgua_svector : public TamguContainer {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<a_svectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
	atomic_string zero;
    atomic_value_vector<atomic_string> values;
    bool isconst;
        //---------------------------------------------------------------------------------------------------------------------
    Tamgua_svector(TamguGlobal* g, Tamgu* parent = NULL) : values(zero), TamguContainer(g, parent) {
            //Do not forget your variable initialisation
        isconst = false;
    }
    
    Tamgua_svector() : values(zero) {
            //Do not forget your variable initialisation
        isconst = false;
    }
    
        //----------------------------------------------------------------------------------------------------------------------
    
    void SetConst() {
        isconst = true;
    }
    
    void pushback(string v) {
        values.push_back(v);
    }
    
    Exporting virtual Tamgu* Put(Tamgu* i, Tamgu* value, short idthread);Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    
    short Type() {
        return Tamgua_svector::idtype;
    }
    
    string Typename() {
        return "a_svector";
    }
    
    bool isString() {
        return true;
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
        string s = convertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(double l) {
        string s = convertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(float l) {
        string s = convertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(short l) {
        string s = convertfromnumber(l);
        values.push_back(s);
    }
    
    void storevalue(BLONG l) {
        
        string s = convertfromnumber(l);
        values.push_back(s);
    }
    
    void Storevalue(string& v) {
        values.push_back(v);
    }
    
    void Storevalue(wstring& v) {
        string s;
        sc_unicode_to_utf8(s,v);
        values.push_back(s);
    }
    
    void storevalue(string v) {
        values.push_back(v);
    }
    
    void storevalue(wstring v) {
        string s;
        sc_unicode_to_utf8(s,v);
        values.push_back(s);
    }
    
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgua_svector* v = new Tamgua_svector();
            v->values = values;
            return v;
        }
        return this;
    }
    
    Exporting Tamgu* getvalue(BLONG i);
    
    wstring getustring(long i) {
        if (i < 0 || i >= values.size()) {
            return L"";
        }
        wstring v;
        s_utf8_to_unicode(v, USTR(values[i].value()), values[i].size());
        return v;
    }
    
    string getstring(long i) {
        if (i < 0 || i >= values.size()) {
            return "";
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
        if (i < 0 || i >= values.size()) {
            return 0;
        }
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
        return new Tamgua_svector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgua_svector* v = new Tamgua_svector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            string s;
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                s = it->Valuestring();
                v->values.push_back(s);
            }
            it->Release();
            return v;
        }
        string val=a->String();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }
    
    bool duplicateForCall() {
        return isconst;
    }
    
    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);
    
    static void AddMethod(TamguGlobal* g, string name, a_svectorMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);
    
    
    void Methods(Tamgu* v) {
        string s;
        
        for (auto& it : infomethods) {
            s=it.first;
            v->storevalue(s);
        }
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
        string a = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        atomic_value_vector_iterator<atomic_string> it(values);
        for (; !it.end(); it.next()) {
            if (it.second.value() == a) {
                values.pop(it.first);
                return aTRUE;
            }
        }
        return aFALSE;
    }
    
    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        string s;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            s = callfunc->Evaluate(i, contextualpattern, idthread)->String();
            values.push_back(s);
        }
        return aTRUE;
    }
    
    Tamgu* MethodNGrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        string u;
        string sep;
        long num =callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        
        if (callfunc->Size() == 2)
            sep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
        
        Tamgu* kvect = Selectasvector(contextualpattern);
        long mx = values.size() - num + 1;
        for (long i = 0; i < mx; i++) {
            u = "";
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
        return globalTamgu->Providestring(values.back().value());
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
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        bool beg = true;
        string res;
        atomic_value_vector_iterator<atomic_string> it(values);
        for (; !it.end(); it.next()) {
            if (beg == false)
                res += sep;
            beg = false;
            res += it.second.value();
        }
        
        return globalTamgu->Providestring(res);
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
        pushback(a->String());
        return this;
    }
    
    Tamgu* Poplast() {
        if (values.size() == 0)
            return aNOELEMENT;
        return globalTamgu->Providestring(values.remove_back().value());
    }
    

    Exporting Tamgu* Pop(Tamgu*);
    Exporting Tamgu* Unique();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    
    void Insert(long idx, Tamgu* ke) {
        if (idx<0)
            idx=0;
        if (idx>=values.size())
            pushback(ke->String());
        else {
            string s = ke->String();
            values.insert(idx, s);
        }
    }
    
    Tamgu* Inverse() {
        Tamgua_svector* vect = new Tamgua_svector();
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
        
        string v;
        for (; i < j; i++)
            v += values[i].value();
        
        return globalTamgu->Providestring(v);
    }
    
    Exporting void Clear();
    Exporting string String();Exporting string JSonString();
        //wstring UString() {}
    
    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();
    
        //Basic operations
    Exporting long Size();
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);Exporting Tamgu* andset(Tamgu* a, bool itself);Exporting Tamgu* orset(Tamgu* a, bool itself);Exporting Tamgu* xorset(Tamgu* a, bool itself);Exporting Tamgu* plus(Tamgu* a, bool itself);Exporting Tamgu* minus(Tamgu* a, bool itself);
    Exporting Tamgu* same(Tamgu* a);
};

class TamguIterationa_svector : public TamguIteration {
public:
    
    
    Tamgua_svector* ref;
    atomic_value_vector_iterator<atomic_string> it;
    
    TamguIterationa_svector(Tamgua_svector* v, bool d, TamguGlobal* g = NULL) : it(v->values), TamguIteration(d, g) {
        ref = v;
    }
    
    Tamgu* Key() {
        return globalTamgu->Provideint(it.first);
    }
    
    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it.second.value());
    }
    
    Tamgu* Value() {
        return globalTamgu->Providestring(it.second.value());
    }
    
    string Keystring() {
        
        return convertfromnumber(it.first);
        
    }
    
    string Valuestring() {
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

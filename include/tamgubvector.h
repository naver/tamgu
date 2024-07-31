/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubvector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgubvector_h
#define tamgubvector_h

#include "tamguint.h"
#include "tamgulong.h"
#include "tamgubyte.h"
#include "tamgustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgubvector;
//This typedef defines a type "bvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgubvector::*bvectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Tamgubvector : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<bvectorMethod> methods;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<unsigned char> values;
    bool isconst;
    //---------------------------------------------------------------------------------------------------------------------
    Tamgubvector(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
        //Do not forget your variable initialisation
        isconst = false; 
    }

    Tamgubvector() {
        //Do not forget your variable initialisation
        isconst = false; 
    }

    //----------------------------------------------------------------------------------------------------------------------
    void SetConst() {
        isconst = true;
    }

    Exporting Tamgu* Put(Tamgu* context, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign);
    
    short Type() {
        return a_bvector;
    }

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "bvector";
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
        locking();
        if (d>values.size())
            values.reserve(d);
        unlocking();
    }

    void storevalue(long v) {
        locking();
        values.push_back((unsigned char)v);
        unlocking();
    }

    void storevalue(short v) {
        locking();
        values.push_back((unsigned char)v);
        unlocking();
    }

    void storevalue(wchar_t v) {
        locking();
        values.push_back((unsigned char)v);
        unlocking();
    }

    void storevalue(BLONG v) {
        locking();
        values.push_back((unsigned char)v);
        unlocking();
    }

    void storevalue(unsigned char v) {
        locking();
        values.push_back(v);
        unlocking();
    }

    void storevalue(float v) {
        locking();
        values.push_back((unsigned char)v);
        unlocking();
    }

    void storevalue(double v) {
        locking();
        values.push_back((unsigned char)v);
        unlocking();
    }

    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgubvector* v = new Tamgubvector;
            locking();
            v->values = values;
            unlocking();
            return v;
        }
        return this;
    }

    Exporting Tamgu* getvalue(long i);

    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Long());
    }

    double getfloat(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        uchar c = values[i];
        unlocking();
        return c;
    }

    long getinteger(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        uchar c = values[i];
        unlocking();
        return c;
    }

    float getdecimal(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        uchar c = values[i];
        unlocking();
        return c;
    }

    BLONG getlong(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        uchar c = values[i];
        unlocking();
        return c;
    }

    uchar getbyte(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        uchar c = values[i];
        unlocking();
        return c;
    }
    
    short getshort(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        uchar c = values[i];
        unlocking();
        return c;
    }

    string getstring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return "";
        }
        string v;
        v=(char)values[i];
        unlocking();
        return v;
    }
    
    wstring getustring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return L"";
        }
        wstring w;
        w=(wchar_t)values[i];
        unlocking();
        return w;
    }
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgubvector();
    }

    bool duplicateForCall() {
        return isconst;
    }

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgubvector* v = new Tamgubvector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->IteratorValue()->Byte());
            }
            it->Release();
            return v;
        }
        
        uchar val=a->Byte();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }


    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, bvectorMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);

    
    void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[a_bvector])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[a_bvector].find(n) !=  globalTamgu->infomethods[a_bvector].end())
              return globalTamgu->infomethods[a_bvector][n];
             return "Unknown method";
    }
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodMin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        uchar m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m > values[i])
                m = values[i];
        }
        return new Tamgubyte(m);
    }
    
    Tamgu* MethodMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        uchar m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m < values[i])
                m = values[i];
        }
        return new Tamgubyte(m);
    }
    
    Tamgu* MethodMinMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        uchar mn = values[0];
        uchar mx = mn;
        for (long i = 1; i < values.size(); i++) {
            if (mn > values[i])
                mn = values[i];
            if (mx < values[i])
                mx = values[i];
        }
        Tamgubvector* b = new Tamgubvector;
        b->values.push_back(mn);
        b->values.push_back(mx);
        return b;
    }

    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Byte());
        
        unlocking();
        return aTRUE;
    }
    
    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Tamgu* MethodPoplast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Poplast();
    }

    Tamgu* MethodReserve(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        long sz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Reserve(sz);
        return aTRUE;
    }
    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        locking();
        bool beg = true;
        stringstream v;
        for (size_t it = 0; it < values.size(); it++) {
            if (beg == false)
                v << sep;
            beg = false;
            v << values[it];
        }
        unlocking();
        return globalTamgu->Providestring(v.str());
    }

    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        if (values.size() == 0) {
            unlocking();
            return aNOELEMENT;
        }
        contextualpattern = new Tamgubyte(values.back());
        unlocking();
        return contextualpattern;
    }

    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }

    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        locking();
        unsigned long dst = EditDistance(v);
        unlocking();
        return globalTamgu->ProvideConstint(dst);
    }

    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Sum();
        return globalTamgu->ProvideConstfloat(v);
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Product();
        return globalTamgu->ProvideConstfloat(v);
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

    Tamgu* MethodPermute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (Permute())
            return aTRUE;
        return aFALSE;
    }

    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }

    Tamgu* MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------

    Exporting Tamgu* Push(Tamgu*);
    Exporting Tamgu* Pop(Tamgu*);
    
    Tamgu* Poplast() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return aNOELEMENT;
        }
        char c = values.back();
        values.pop_back();
        unlocking();
        return new Tamgubyte(c);
    }
    
    Exporting Tamgu* Unique();
    Exporting void Sort(bool);
    Exporting void Shuffle();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    Exporting bool Permute();
    Exporting unsigned long EditDistance(Tamgu* e);

    Tamgu* Thesum(long i, long j) {
        long sz = values.size();
        if (!sz)
            return aZERO;

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

        BLONG v = 0;
        locking();
        for (; i < j; i++)
            v += values[i];
        unlocking();
        return globalTamgu->Providelong(v);
    }

    Tamgu* Theproduct(long i, long j) {
        long sz = values.size();
        if (!sz)
            return aZERO;

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

        BLONG v = 1;
        locking();
        for (; i < j; i++)
            v *= values[i];
        unlocking();
        return globalTamgu->Providelong(v);
    }


    double Sum() {
        locking();
        double v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i];
        unlocking();
        return v;
    }

    double Product() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return 0;
        }
        double v = values[0];
        for (int i = 1; i < values.size(); i++)
            v *= values[i];
        unlocking();
        return v;
    }

    void Insert(long idx, Tamgu* ke) {
        locking();
        if (idx<0)
            idx=0;
        if (idx>=values.size())
            values.push_back(ke->Byte());
        else
            values.insert(values.begin() + idx, ke->Byte());
        unlocking();
    }

    Tamgu* Inverse() {
        locking();
        Tamgubvector* vect = new Tamgubvector;
        for (long i = (long)values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        unlocking();
        return vect;
    }

    Exporting Tamgu* Map(short idthread);Exporting Tamgu* Vector(short idthread);
    
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*Tamgubvector::methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    Exporting string String();
    Exporting void Setstring(string& e, short idthread);

    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();

    //Basic operations
    Exporting long Size();
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    Exporting Tamgu* andset(Tamgu* a, bool itself);
    Exporting Tamgu* orset(Tamgu* a, bool itself);
    Exporting Tamgu* xorset(Tamgu* a, bool itself);
    Exporting Tamgu* plus(Tamgu* a, bool itself);
    Exporting Tamgu* minus(Tamgu* a, bool itself);
    Exporting Tamgu* multiply(Tamgu* a, bool itself);
    Exporting Tamgu* divide(Tamgu* a, bool itself);
    Exporting Tamgu* divideinteger(Tamgu* a, bool itself);
    Exporting Tamgu* power(Tamgu* a, bool itself);
    Exporting Tamgu* shiftleft(Tamgu* a, bool itself);
    Exporting Tamgu* shiftright(Tamgu* a, bool itself);
    Exporting Tamgu* mod(Tamgu* a, bool itself);
    Exporting Tamgu* same(Tamgu* a);
};


class TamguIterationbvector : public TamguIteration {
    public:
    BLONG itx;
    Tamgubvector* ref;
    Locking _lock;

    TamguIterationbvector(Tamgubvector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
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
        return new Tamgubyte(ref->values[itx]);
    }

    string Keystring() {
        
        return convertfromnumber(itx);
        
    }

    string Valuestring() {
        string s;
        s = ref->values[itx];
        return s;
    }

    long Keyinteger() {
        return itx;
    }

    BLONG Keylong() {
        return itx;
    }

    BLONG Valuelong() {
        return ref->values[itx];
    }

    long Valueinteger() {
        return ref->values[itx];
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        return ref->values[itx];
    }

    double Valuefloat() {
        return ref->values[itx];
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
            itx = ref->values.size();
            itx--;
        }
        else
            itx = 0;
        return aTRUE;
    }
};


//---------------------------------------------------------------------------------------------------------------------

#endif

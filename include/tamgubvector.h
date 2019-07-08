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
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

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
    Exporting Tamgu* Get(Tamgu* context, Tamgu* value, short idthread);
    short Type() {
        return Tamgubvector::idtype;
    }

    

    static void Setidtype(TamguGlobal* global);
    
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
        Locking _lock(this);
        if (d>values.size())
            values.reserve(d);
    }

    void storevalue(long v) {
        Locking _lock(this);
        values.push_back((unsigned char)v);
    }

    void storevalue(short v) {
        Locking _lock(this);
        values.push_back((unsigned char)v);
    }

    void storevalue(wchar_t v) {
        Locking _lock(this);
        values.push_back((unsigned char)v);
    }

    void storevalue(BLONG v) {
        Locking _lock(this);
        values.push_back((unsigned char)v);
    }

    void storevalue(unsigned char v) {
        Locking _lock(this);
        values.push_back(v);
    }

    void storevalue(float v) {
        Locking _lock(this);
        values.push_back((unsigned char)v);
    }

    void storevalue(double v) {
        Locking _lock(this);
        values.push_back((unsigned char)v);
    }

    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgubvector* v = new Tamgubvector;
            Locking _lock(this);
            v->values = values;
            return v;
        }
        return this;
    }

    Exporting Tamgu* getvalue(BLONG i);

    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Long());
    }

    double getfloat(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    long getinteger(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    float getdecimal(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    BLONG getlong(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    short getshort(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }

    string getstring(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return "";
        string v;
        v=(char)values[i];
        return v;
    }
    
    wstring getustring(long i) {
        Locking _lock(this);
        if (i < 0 || i >= values.size())
            return L"";
        wstring w;
        w=(wchar_t)values[i];
        return w;
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
    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Byte());
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
        Locking _lock(this);
        bool beg = true;
        stringstream v;
        for (size_t it = 0; it < values.size(); it++) {
            if (beg == false)
                v << sep;
            beg = false;
            v << values[it];
        }

        return globalTamgu->Providestring(v.str());
    }

    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        return new Tamgubyte(values.back());
    }

    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        return Merging(v);
    }

    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Locking _lock(this);
        unsigned long dst = EditDistance(v);
        return globalTamgu->Provideint(dst);
    }

    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Sum();
        return globalTamgu->Providefloat(v);
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Product();
        return globalTamgu->Providefloat(v);
    }

    Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long i = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Locking _lock(this);
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
        Locking _lock(this);
        if (values.size() == 0)
            return aNOELEMENT;
        char c = values.back();
        values.pop_back();
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
        for (; i < j; i++)
            v += values[i];

        return new Tamgulong(v);
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
        for (; i < j; i++)
            v *= values[i];

        return new Tamgulong(v);
    }


    double Sum() {
        Locking* _lock = _getlock(this);
        double v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i];
        _cleanlock(_lock);
        return v;
    }

    double Product() {
        Locking* _lock = _getlock(this);
        if (values.size() == 0)
            return 0;
        double v = values[0];
        for (int i = 1; i < values.size(); i++)
            v *= values[i];
        _cleanlock(_lock);
        return v;
    }

    void Insert(long idx, Tamgu* ke) {
        Locking _lock(this);
        if (idx<0)
            idx=0;
        if (idx>=values.size())
            values.push_back(ke->Byte());
        else
            values.insert(values.begin() + idx, ke->Byte());
    }

    Tamgu* Inverse() {
        Locking _lock(this);
        Tamgubvector* vect = new Tamgubvector;
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        return vect;
    }

    Exporting Tamgu* Map(short idthread);Exporting Tamgu* Vector(short idthread);	//---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    Exporting string String();
    //wstring UString() {}

    Exporting long Integer();Exporting double Float();Exporting BLONG Long();Exporting bool Boolean();

    //Basic operations
    Exporting long Size();
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);Exporting Tamgu* andset(Tamgu* a, bool itself);Exporting Tamgu* orset(Tamgu* a, bool itself);Exporting Tamgu* xorset(Tamgu* a, bool itself);Exporting Tamgu* plus(Tamgu* a, bool itself);Exporting Tamgu* minus(Tamgu* a, bool itself);Exporting Tamgu* multiply(Tamgu* a, bool itself);Exporting Tamgu* divide(Tamgu* a, bool itself);Exporting Tamgu* power(Tamgu* a, bool itself);Exporting Tamgu* shiftleft(Tamgu* a, bool itself);Exporting Tamgu* shiftright(Tamgu* a, bool itself);Exporting Tamgu* mod(Tamgu* a, bool itself);
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
        return globalTamgu->Provideint(itx);
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
            itx = ref->values.size() - 1;
        }
        else
            itx = 0;
        return aTRUE;
    }
};


//---------------------------------------------------------------------------------------------------------------------

#endif

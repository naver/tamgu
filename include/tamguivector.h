/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguivector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguivector_h
#define tamguivector_h

#include "tamguint.h"
#include "tamgustring.h"
#include "tamgulong.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguivector;
//This typedef defines a type "ivectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguivector::*ivectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Tamguivector : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<ivectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<long> values;
    bool isconst;
    //---------------------------------------------------------------------------------------------------------------------
    Tamguivector(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
     investigate |= is_number;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    Tamguivector() {
     investigate |= is_number;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    void SetConst() {
        isconst = true;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Put(Tamgu* idx, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

    short Type() {
        return a_ivector;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "ivector";
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
        if (d > values.size())
            values.reserve(d);
        unlocking();
    }

    void storevalue(long v) {
        locking();
        values.push_back(v);
        unlocking();
    }

    void storevalue(short v) {
        locking();
        values.push_back((long)v);
        unlocking();
    }

    void storevalue(wchar_t v) {
        locking();
        values.push_back((long)v);
        unlocking();
    }

    void storevalue(BLONG v) {
        locking();
        values.push_back((long)v);
        unlocking();
    }

    void storevalue(unsigned char v) {
        locking();
        values.push_back((long)v);
        unlocking();
    }

    void storevalue(float v) {
        locking();
        values.push_back((long)v);
        unlocking();
    }

    void storevalue(double v) {
        locking();
        values.push_back((long)v);
        unlocking();
    }

    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamguivector* v = globalTamgu->Provideivector();
            locking();
            v->values = values;
            unlocking();
            return v;
        }
        return this;
    }

    bool duplicateForCall() {
        return isconst;
    }


    Exporting Tamgu* getvalue(BLONG i);

    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Long());
    }

    double getfloat(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        long v = values[i];
        unlocking();
        return v;
    }

    long getinteger(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        long v = values[i];
        unlocking();
        return v;
    }

    float getdecimal(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        long v = values[i];
        unlocking();
        return v;
    }

    BLONG getlong(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        long v = values[i];
        unlocking();
        return v;
    }

    uchar getbyte(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        long v = values[i];
        unlocking();
        return v;
    }

    short getshort(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        long v = values[i];
        unlocking();
        return v;
    }

    string getstring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        string v = convertfromnumber(values[i]);
        unlocking();
        return v;
    }
    
    wstring getustring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        wstring v = wconvertfromnumber(values[i]);
        unlocking();
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
        return globalTamgu->Provideivector();
    }

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamguivector* v = globalTamgu->Provideivector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valueinteger());
            }
            it->Release();
            return v;
        }
        long val=a->Integer();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }


    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);

    Exporting TamguIteration* Newiteration(bool direction);


    static void AddMethod(TamguGlobal* g, string name, ivectorMethod func, unsigned long arity, string infos);

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
    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long a = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        locking();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
                unlocking();
                return aTRUE;
            }
        }
        unlocking();
        return aFALSE;
    }



    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }


    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread)->Integer();
            locking();
            values.push_back(v);
            unlocking();
        }
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

    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
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
        unlocking();
        if (values.size() == 0) {
            unlocking();
            return aNOELEMENT;
        }
        contextualpattern = globalTamgu->Provideint(values.back());
        unlocking();
        return contextualpattern;
    }

    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        locking();
        unsigned long dst = EditDistance(v);
        unlocking();
        return globalTamgu->Provideint(dst);
    }

    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long v = ISum();
        return globalTamgu->Provideint(v);
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long v = IProduct();
        return globalTamgu->Provideint(v);
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
        return booleantamgu[Permute()];
    }

    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }

    Tamgu* MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------

    Exporting Tamgu* Push(Tamgu*);
    Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        locking();
        values.push_back(a->Integer());
        unlocking();
        return this;
    }


    Exporting Tamgu* Pop(Tamgu*);
    Tamgu* Poplast() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return aNOELEMENT;
        }
        long c = values.back();
        values.pop_back();
        unlocking();
        return globalTamgu->Provideint(c);
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

    long ISum() {
        locking();
        long v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i];
        unlocking();
        return v;
    }

    long IProduct() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return 0;
        }
        long v = values[0];
        for (int i = 1; i < values.size(); i++)
            v *= values[i];
        unlocking();
        return v;
    }

    double Sum() {
        return ISum();
    }
    
    double Product() {
        return IProduct();
    }
    
    void Insert(long idx, Tamgu* ke) {
        locking();
        if (idx<0)
            idx=0;
        if (idx>=values.size())
            values.push_back(ke->Integer());
        else
            values.insert(values.begin() + idx, ke->Integer());
        unlocking();
    }

    Tamgu* Inverse() {
        locking();
        Tamguivector* vect = globalTamgu->Provideivector();
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        unlocking();
        return vect;
    }


    Exporting Tamgu* Map(short idthread);
    Exporting Tamgu* Vector(short idthread);
    //---------------------------------------------------------------------------------------------------------------------

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

    Exporting long Integer();
    Exporting short Short();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


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
    Exporting Tamgu* power(Tamgu* a, bool itself);
    Exporting Tamgu* shiftleft(Tamgu* a, bool itself);
    Exporting Tamgu* shiftright(Tamgu* a, bool itself);
    Exporting Tamgu* mod(Tamgu* a, bool itself);

    Exporting Tamgu* same(Tamgu* a);

};

class TamguIterationivector : public TamguIteration {
    public:

    BLONG itx;
    Tamguivector* ref;
    Locking _lock;

    TamguIterationivector(Tamguivector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
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
        return globalTamgu->Provideint(ref->values[itx]);
    }

    string Keystring() {
        
        return convertfromnumber(itx);
        
    }

    string Valuestring() {
        
        return convertfromnumber(ref->values[itx]);
        
    }

    BLONG Keylong() {
        return itx;
    }

    BLONG Valuelong() {
        return ref->values[itx];
    }

    long Keyinteger() {
        return itx;
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
class Tamguivectorbuff : public Tamguivector {
    public:
    long idx;
    bool used;

    Tamguivectorbuff(long i)  {
        //Do not forget your variable initialisation
        idx = i;
        used = false;
    }

    bool Candelete() {
        return false;
    }

    void Resetreference(short r) {
        if ((reference-=r) <= 0) {
            reference = 0;
            if (!protect) {
                protect = true;

                values.clear();
                used = false;
                if (!globalTamgu->threadMODE)
                    globalTamgu->ivectorempties.push_back(idx);
            }
        }
    }

};
//---------------------------------------------------------------------------------------------------------------------

    //We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
    //of how to declare a new method.
class Tamgua_ivector;
    //This typedef defines a type "a_ivectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgua_ivector::*a_ivectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------


class Tamgua_ivector : public TamguContainer {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<a_ivectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    atomic_value_vector<long> values;
    bool isconst;
        //---------------------------------------------------------------------------------------------------------------------
    Tamgua_ivector(TamguGlobal* g, Tamgu* parent = NULL) : values(0, false), TamguContainer(g, parent) {
     investigate |= is_number;
            //Do not forget your variable initialisation
        isconst = false;
    }
    
    Tamgua_ivector() : values(0, false) {
     investigate |= is_number;
            //Do not forget your variable initialisation
        isconst = false;
    }
    
    void SetConst() {
        isconst = true;
    }
    
        //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Put(Tamgu* idx, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    
    short Type() {
        return Tamgua_ivector::idtype;
    }
    
    string Typename() {
        return "a_ivector";
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
    
    
    void storevalue(long v) {
        values.push_back(v);
    }
    
    void storevalue(short v) {
        values.push_back((long)v);
    }
    
    void storevalue(wchar_t v) {
        values.push_back((long)v);
    }
    
    void storevalue(BLONG v) {
        values.push_back((long)v);
    }
    
    void storevalue(unsigned char v) {
        values.push_back((long)v);
    }
    
    void storevalue(float v) {
        values.push_back((long)v);
    }
    
    void storevalue(double v) {
        values.push_back((long)v);
    }
    
    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgua_ivector* v = new Tamgua_ivector();
            v->values = values;
            return v;
        }
        return this;
    }
    
    bool duplicateForCall() {
        return isconst;
    }
    
    
    Exporting Tamgu* getvalue(BLONG i);
    
    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Long());
    }
    
    double getfloat(long i) {
        if (i < 0 || i >= values.size())
            return 0;
        return (double)values[i];
    }
    
    long getinteger(long i) {
        if (i < 0 || i >= values.size())
            return 0;
        return values[i];
    }
    
    float getdecimal(long i) {
        if (i < 0 || i >= values.size())
            return 0;
        return (float)values[i];
    }
    
    BLONG getlong(long i) {
        if (i < 0 || i >= values.size())
            return 0;
        return (BLONG)values[i];
    }
    
    uchar getbyte(long i) {
        if (i < 0 || i >= values.size())
            return 0;
        return (uchar)values[i];
    }

    short getshort(long i) {
        if (i < 0 || i >= values.size())
            return 0;
        return (short)values[i];
    }
    
    string getstring(long i) {
        if (i < 0 || i >= values.size())
            return "";
        return convertfromnumber(values[i]);
    }
    
    wstring getustring(long i) {
        if (i < 0 || i >= values.size())
            return L"";
        return wconvertfromnumber(values[i]);
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
        return new Tamgua_ivector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgua_ivector* v = new Tamgua_ivector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valueinteger());
            }
            it->Release();
            return v;
        }
        long val=a->Integer();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }
    
    
    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    
    Exporting TamguIteration* Newiteration(bool direction);
    
    
    static void AddMethod(TamguGlobal* g, string name, a_ivectorMethod func, unsigned long arity, string infos);
    
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
    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long a = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        atomic_value_vector_iterator<long> it(values);
        for (; !it.end(); it.next()) {
            if (it.second == a) {
                values.pop(it.first);
                return aTRUE;
            }
        }
        return aFALSE;
    }
    
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    
    
    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread)->Integer();
            values.push_back(v);
        }
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
    
    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
    }
    
    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        bool beg = true;
        stringstream v;
        atomic_value_vector_iterator<long> it(values);
        for (; !it.end(); it.next()) {
            if (beg == false)
                v << sep;
            beg = false;
            v << it.second;
        }
        
        return globalTamgu->Providestring(v.str());
    }
    
    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (values.size() == 0)
            return aNOELEMENT;
        return globalTamgu->Provideint(values.back());
    }
    
    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        unsigned long dst = EditDistance(v);
        return globalTamgu->Provideint(dst);
    }
    
    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long v = ISum();
        return globalTamgu->Provideint(v);
    }
    
    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        long v = IProduct();
        return globalTamgu->Provideint(v);
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
    
    Exporting Tamgu* Push(Tamgu*);
    Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        values.push_back(a->Integer());
        return this;
    }
    
    
    Exporting Tamgu* Pop(Tamgu*);
    
    Tamgu* Poplast() {
        if (values.size() == 0)
            return aNOELEMENT;
        return globalTamgu->Provideint(values.remove_back());
    }

    Exporting Tamgu* Unique();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
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
    
    long ISum() {
        long v = 0;
        atomic_value_vector_iterator<long> it;
        for (; !it.end(); it.next())
            v += it.second;
        return v;
    }
    
    long IProduct() {
        if (!values.size())
            return 0;
        
        long v = 1;
        atomic_value_vector_iterator<long> it;
        for (; !it.end(); it.next())
            v *= it.second;
        return v;
    }
    
    double Sum() {
        return ISum();
    }
    
    double Product() {
        return IProduct();
    }

    void Insert(long idx, Tamgu* ke) {
        if (idx<0)
            idx=0;
        if (idx >= values.size())
            values.push_back(ke->Integer());
        else
            values.insert(idx, ke->Integer());
    }
    
    Tamgu* Inverse() {
        Tamgua_ivector* vect = new Tamgua_ivector();
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        return vect;
    }
    
    
    Exporting Tamgu* Map(short idthread);
    Exporting Tamgu* Vector(short idthread);
        //---------------------------------------------------------------------------------------------------------------------
    
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
    
    Exporting long Integer();
    Exporting short Short();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();
    
    
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
    Exporting Tamgu* power(Tamgu* a, bool itself);
    Exporting Tamgu* shiftleft(Tamgu* a, bool itself);
    Exporting Tamgu* shiftright(Tamgu* a, bool itself);
    Exporting Tamgu* mod(Tamgu* a, bool itself);
    
    Exporting Tamgu* same(Tamgu* a);
    
};

class TamguIterationa_ivector : public TamguIteration {
public:
    
    atomic_value_vector_iterator<long> it;
    Tamgua_ivector* av;
    
    TamguIterationa_ivector(Tamgua_ivector* v, bool d, TamguGlobal* g = NULL) : it(v->values), TamguIteration(d, g) {
        av = v;
    }
    
    Tamgu* Key() {
        return globalTamgu->Provideint(it.first);
    }
    
    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it.second);
    }
    
    Tamgu* Value() {
        return globalTamgu->Provideint(it.second);
    }
    
    string Keystring() {
        return convertfromnumber(it.first);
    }
    
    string Valuestring() {
        return convertfromnumber(it.second);
    }
    
    BLONG Keylong() {
        return it.first;
    }
    
    BLONG Valuelong() {
        return it.second;
    }
    
    long Keyinteger() {
        return it.first;
    }
    
    long Valueinteger() {
        return it.second;
    }
    
    double Keyfloat() {
        return it.first;
    }
    
    float Valuedecimal() {
        return it.second;
    }
    
    double Valuefloat() {
        return it.second;
    }
    
    void Next() {
        it.next();
    }
    
    Tamgu* End() {
        return booleantamgu[it.end()];
    }
    
    Tamgu* Begin() {
        it.begin(av->values);
        return aTRUE;
    }
};

#endif

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgulvector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgulvector_h
#define tamgulvector_h

#include "tamguint.h"
#include "tamgulong.h"
#include "tamgustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgulvector;
//This typedef defines a type "lvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgulvector::*lvectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------


class Tamgulvector : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<lvectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<BLONG> values;
    bool isconst;
    //---------------------------------------------------------------------------------------------------------------------
    Tamgulvector(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
     investigate |= is_number;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    Tamgulvector() {
     investigate |= is_number;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    //----------------------------------------------------------------------------------------------------------------------

    void SetConst() {
        isconst = true;
    }

    Exporting Tamgu* Put(Tamgu* idx, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Tamgu* EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign);
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    short Type() {
        return a_lvector;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "lvector";
    }

    bool isLong() {
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
        locking();
        if (d>values.size())
            values.reserve(d);
        unlocking();
    }

    void storevalue(long v) {
        locking();
        values.push_back((BLONG)v);
        unlocking();
    }

    void storevalue(short v) {
        locking();
        values.push_back((BLONG)v);
        unlocking();
    }

    void storevalue(wchar_t v) {
        locking();
        values.push_back((BLONG)v);
        unlocking();
    }

    void storevalue(BLONG v) {
        locking();
        values.push_back(v);
        unlocking();
    }

    void storevalue(unsigned char v) {
        locking();
        values.push_back((BLONG)v);
        unlocking();
    }

    void storevalue(float v) {
        locking();
        values.push_back((BLONG)v);
        unlocking();
    }

    void storevalue(double v) {
        locking();
        values.push_back((BLONG)v);
        unlocking();
    }

    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgulvector* v = new Tamgulvector;
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
        double res = values[i];
        unlocking();
        return res;
    }

    float getdecimal(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double res = values[i];
        unlocking();
        return res;
    }

    long getinteger(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double res = values[i];
        unlocking();
        return res;
    }

    BLONG getlong(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double res = values[i];
        unlocking();
        return res;
    }

    uchar getbyte(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double res = values[i];
        unlocking();
        return res;
    }
        
    short getshort(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double res = values[i];
        unlocking();
        return res;
    }

    string getstring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        string res = convertfromnumber(values[i]);
        unlocking();
        return res;
    }
    
    wstring getustring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        wstring res = wconvertfromnumber(values[i]);
        unlocking();
        return res;
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
        return new Tamgulvector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgulvector* v = new Tamgulvector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valuelong());
            }
            it->Release();
            return v;
        }
        
        BLONG val=a->Long();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }

    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, lvectorMethod func, unsigned long arity, string infos);
    
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
        BLONG a = callfunc->Evaluate(0, contextualpattern, idthread)->Long();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
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
        locking();
        for (size_t i = 0; i < callfunc->Size(); i++)
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Long());
        unlocking();
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
        locking();
        if (values.size() == 0) {
            locking();
            return aNOELEMENT;
        }
        contextualpattern = globalTamgu->ProvideConstlong(values.back());
        unlocking();
        return contextualpattern;
    }

    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        locking();
        unsigned long dst = EditDistance(v);
        unlocking();
        return globalTamgu->ProvideConstlong(dst);
    }

    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        BLONG v = LSum();
        return globalTamgu->Providelong(v);
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        BLONG v = LProduct();
        return globalTamgu->Providelong(v);
    }

    Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        BLONG i = callfunc->Evaluate(0, contextualpattern, idthread)->Long();
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

    Exporting Tamgu* Push(Tamgu*);	Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        locking();
        values.push_back(a->Long());
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
        BLONG c = values.back();
        values.pop_back();
        unlocking();
        return globalTamgu->Providelong(c);
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
        for (; i < j; i++)
            v *= values[i];

        return globalTamgu->Providelong(v);
    }

    BLONG LSum() {
        locking();
        BLONG v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i];
        unlocking();
        return v;
    }

    BLONG LProduct() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return 0;
        }
        BLONG v = values[0];
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
            values.push_back(ke->Long());
        else
            values.insert(values.begin() + idx, ke->Long());
        unlocking();
    }

    Tamgu* Inverse() {
        locking();
        Tamgulvector* vect = new Tamgulvector;
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->values.push_back(values[i]);
        }
        unlocking();
        return vect;
    }


    Exporting Tamgu* Map(short idthread);Exporting Tamgu* Vector(short idthread);    //---------------------------------------------------------------------------------------------------------------------

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
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);
    Exporting Tamgu* andset(Tamgu* a, bool itself);Exporting Tamgu* orset(Tamgu* a, bool itself);Exporting Tamgu* xorset(Tamgu* a, bool itself);Exporting Tamgu* plus(Tamgu* a, bool itself);Exporting Tamgu* minus(Tamgu* a, bool itself);Exporting Tamgu* multiply(Tamgu* a, bool itself);Exporting Tamgu* divide(Tamgu* a, bool itself);Exporting Tamgu* power(Tamgu* a, bool itself);Exporting Tamgu* shiftleft(Tamgu* a, bool itself);Exporting Tamgu* shiftright(Tamgu* a, bool itself);Exporting Tamgu* mod(Tamgu* a, bool itself);
    Exporting Tamgu* same(Tamgu* a);
};

class TamguIterationlvector : public TamguIteration {
    public:

    BLONG itx;
    Tamgulvector* ref;
    Locking _lock;

    TamguIterationlvector(Tamgulvector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Tamgu* Key() {
        return globalTamgu->ProvideConstlong(itx);
    }

    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(ref->values[itx]);
    }

    Tamgu* Value() {
        return globalTamgu->ProvideConstlong(ref->values[itx]);
    }

    string Keystring() {
        
        return convertfromnumber(itx);
        
    }

    string Valuestring() {
        
        return convertfromnumber(ref->values[itx]);
        
    }

    long Keyinteger() {
        return itx;
    }

    BLONG Keylong() {
        return itx;
    }

    long Valueinteger() {
        return ref->values[itx];
    }

    BLONG Valuelong() {
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

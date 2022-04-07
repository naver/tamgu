/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguhvector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef Tamguhvector_h
#define Tamguhvector_h

#include "tamguint.h"
#include "tamgushort.h"
#include "tamgustring.h"
#include "tamgudecimal.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguhvector;
//This typedef defines a type "hvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguhvector::*hvectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamguhvector : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<hvectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<short> values;
    vector<long> shape;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Tamguhvector(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
     investigate |= is_number;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    Tamguhvector() {
     investigate |= is_number;
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
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    short Type() {
        return a_hvector;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "dvector";
    }

    

    bool isShort() {
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
    
    void store(long k, Tamgu* v) {
        locking();
        if (k >= values.size())
            values.push_back(v->Short());
        else
            values[k] = v->Short();
        unlocking();
    }
    
    Exporting void storevalue(Tamgu* v, long beg, long end) {
        long sz = v->Size();
        for (;beg < end && beg < sz; beg++)
            values.push_back(((Tamguhvector*)v)->values[beg]);
    }

    void storevalue(long v) {
        locking();
        values.push_back((short)v);
        unlocking();
    }

    void storevalue(short v) {
        locking();
        values.push_back(v);
        unlocking();
    }

    void storevalue(wchar_t v) {
        locking();
        values.push_back((short)v);
        unlocking();
    }

    void storevalue(unsigned char v) {
        locking();
        values.push_back((short)v);
        unlocking();
    }

    void storevalue(BLONG v) {
        locking();
        values.push_back((short)v);
        unlocking();
    }

    void storevalue(float v) {
        locking();
        values.push_back((short)v);
        unlocking();
    }

    void storevalue(double v) {
        locking();
        values.push_back((short)v);
        unlocking();
    }


    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamguhvector* v = new Tamguhvector;
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

    string getstring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return "";
        }
        string r = convertfromnumber(values[i]);
        unlocking();
        return r;
    }
    
    wstring getustring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return L"";
        }
        wstring r = wconvertfromnumber(values[i]);
        unlocking();
        return r;
    }

    double getfloat(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double r = values[i];
        unlocking();
        return r;
    }
    
    long getinteger(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double r = values[i];
        unlocking();
        return r;
    }

    float getdecimal(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double r = values[i];
        unlocking();
        return r;
    }

    uchar getbyte(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double r = values[i];
        unlocking();
        return r;
    }

    short getshort(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double r = values[i];
        unlocking();
        return r;
    }

    BLONG getlong(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        double r = values[i];
        unlocking();
        return r;
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
        return new Tamguhvector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamguhvector* v = new Tamguhvector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valueshort());
            }
            it->Release();
            return v;
        }
        short val=a->Short();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }

    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, hvectorMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);

    
    void Methods(Tamgu* v) {

        for (const auto& it : infomethods)
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
        short m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m > values[i])
                m = values[i];
        }
        return new Tamgushort(m);
    }
    
    Tamgu* MethodMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        short m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m < values[i])
                m = values[i];
        }
        return new Tamgushort(m);
    }
    
    Tamgu* MethodMinMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        short mn = values[0];
        short mx = mn;
        for (long i = 1; i < values.size(); i++) {
            if (mn > values[i])
                mn = values[i];
            if (mx < values[i])
                mx = values[i];
        }
        Tamguhvector* b = new Tamguhvector;
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
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Short());
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
        contextualpattern = new Tamgudecimal(values.back());
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
        float v = DSum();
        return new Tamgudecimal(v);
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        float v = DProduct();
        return new Tamgudecimal(v);
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

    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        float a = callfunc->Evaluate(0, contextualpattern, idthread)->Short();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
                return aTRUE;
            }
        }
        return aFALSE;
    }


    Tamgu* MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------
    void Getshape(vector<long>& sh) {
        long nb = 1;
        for (long i = 0; i < shape.size(); i++)
            nb *= shape[i];
        if (nb <= values.size())
            sh = shape;
    }
    

    Tamgu* push(Tamgu* a) {
        values.push_back(a->Short());
        return this;
    }
    

    Exporting Tamgu* Push(Tamgu*);
    
    Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        locking();
        values.push_back(a->Short());
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
        short c = values.back();
        values.pop_back();
        unlocking();
        return new Tamgushort(c);
    }

    Exporting Tamgu* Unique();
    Exporting void Sort(bool);
    Exporting void Shuffle();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    Exporting bool Permute();
    Exporting unsigned long EditDistance(Tamgu* e);
    
    float DSum() {
        locking();
        float v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i];
        unlocking();
        return v;
    }

    float DProduct() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return 0;
        }
        float v = values[0];
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
            values.push_back(ke->Short());
        else
            values.insert(values.begin() + idx, ke->Short());
        unlocking();
    }

    Tamgu* Inverse() {
        locking();
        Tamguhvector* vect = new Tamguhvector;
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

    Exporting long Integer();
    Exporting short Short();
    Exporting float Decimal();
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

class TamguIterationhvector : public TamguIteration {
    public:
    BLONG itx;
    Tamguhvector* ref;
    Locking _lock;

    TamguIterationhvector(Tamguhvector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
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
        return new Tamgudecimal(ref->values[itx]);
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

    short Valueshort() {
        return ref->values[itx];
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


#endif

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufvector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgufvector_h
#define tamgufvector_h

#include "tamguint.h"
#include "tamgustring.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgufvector;
//This typedef defines a type "fvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgufvector::*fvectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamgufvector : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<fvectorMethod> methods;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<double> values;
    vector<long> shape;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgufvector(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
     investigate |= is_number;
        //Do not forget your variable initialisation
        isconst = false; 
    }

    Tamgufvector() {
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
        return a_fvector;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "fvector";
    }

    bool isFloat() {
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
        if (d > values.size())
            values.reserve(d);
        unlocking();
    }

    void store(long k, Tamgu* v) {
        locking();
        if (k >= values.size())
            values.push_back(v->Float());
        else
            values[k] = v->Float();
        unlocking();
    }
    
	Exporting void storevalue(Tamgu* v, long beg, long end) {
        long sz = v->Size();
        for (;beg < end && beg < sz; beg++)
            values.push_back(((Tamgufvector*)v)->values[beg]);
    }
    
    void storevalue(long v) {
        locking();
        values.push_back((double)v);
        unlocking();
    }

    void storevalue(short v) {
        locking();
        values.push_back((double)v);
        unlocking();
    }

    void storevalue(wchar_t v) {
        locking();
        values.push_back((double)v);
        unlocking();
    }

    void storevalue(BLONG v) {
        locking();
        values.push_back((double)v);
        unlocking();
    }

    void storevalue(unsigned char v) {
        locking();
        values.push_back((double)v);
        unlocking();
    }

    void storevalue(float v) {
        locking();
        values.push_back((double)v);
        unlocking();
    }

    void storevalue(double v) {
        locking();
        values.push_back(v);
        unlocking();
    }


    Tamgu* Atom(bool forced = false) {
        if (forced || !protect || reference) {
            Tamgufvector* v = globalTamgu->Providefvector();
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



    Tamgu* Value(Tamgu* a) {
        return getvalue(a->Long());
    }
    
    Tamgu* getvalue(long i) {
        if (globalTamgu->threadMODE) {
            locking();
            if (i<0 || i>=values.size()) {
                unlocking();
                return aNOELEMENT;
            }
            
            Tamgu* v = globalTamgu->ProvideConstfloat(values[i]);
            unlocking();
            return v;
        }
        
        if (i<0 || i>=values.size()) {
            unlocking();
            return aNOELEMENT;
        }
        return globalTamgu->ProvideConstfloat(values[i]);
    }
    
    double getfloat(long i) {
        if (globalTamgu->threadMODE) {
            locking();
            if (i<0 || i>=values.size()) {
                unlocking();
                return 0;
            }
            
            double v = values[i];
            unlocking();
            return v;
        }
        
        if (i<0 || i>=values.size()) {
            unlocking();
            return 0;
        }
        return values[i];
    }
    
    long getinteger(long i) {
        if (globalTamgu->threadMODE) {
            locking();
            if (i<0 || i>=values.size()) {
                unlocking();
                return 0;
            }
            
            double v = values[i];
            unlocking();
            return v;
        }
        
        if (i<0 || i>=values.size()) {
            unlocking();
            return 0;
        }
        return values[i];
    }
    
    float getdecimal(long i) {
        if (globalTamgu->threadMODE) {
            locking();
            if (i<0 || i>=values.size()) {
                unlocking();
                return 0;
            }
            
            double v = values[i];
            unlocking();
            return v;
        }
        
        if (i<0 || i>=values.size()) {
            unlocking();
            return 0;
        }
        return values[i];
    }
    
    BLONG getlong(long i) {
        if (globalTamgu->threadMODE) {
            locking();
            if (i<0 || i>=values.size()) {
                unlocking();
                return 0;
            }
            
            double v = values[i];
            unlocking();
            return v;
        }
        
        if (i<0 || i>=values.size()) {
            unlocking();
            return 0;
        }
        return values[i];
    }
    
    uchar getbyte(long i) {
        if (globalTamgu->threadMODE) {
            locking();
            if (i<0 || i>=values.size()) {
                unlocking();
                return 0;
            }
            
            double v = values[i];
            unlocking();
            return v;
        }
        
        if (i<0 || i>=values.size()) {
            unlocking();
            return 0;
        }
        return values[i];
    }

    short getshort(long i) {
        if (globalTamgu->threadMODE) {
            locking();
            if (i<0 || i>=values.size()) {
                unlocking();
                return 0;
            }
            
            double v = values[i];
            unlocking();
            return v;
        }
        
        if (i<0 || i>=values.size()) {
            unlocking();
            return 0;
        }
        return values[i];
    }

    string getstring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return "";
        }
        string v = convertfromnumber(values[i]);
        unlocking();
        return v;
    }
    
    wstring getustring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return L"";
        }
        wstring v = wconvertfromnumber(values[i]);
        unlocking();
        return v;
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return globalTamgu->Providefvector();
    }

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgufvector* v = globalTamgu->Providefvector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                v->values.push_back(it->Valuefloat());
            }
            it->Release();
            return v;
        }
        double val=a->Float();
        for (long i=0; i<values.size();i++)
            v->values.push_back(val);
        return v;
    }

    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting TamguIteration* Newiteration(bool direction);


    static void AddMethod(TamguGlobal* g, string name, fvectorMethod func, unsigned long arity, string infos);

    static bool InitialisationModule(TamguGlobal* g, string version);


    void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[a_fvector])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[a_fvector].find(n) !=  globalTamgu->infomethods[a_fvector].end())
              return globalTamgu->infomethods[a_fvector][n];
             return "Unknown method";
    }
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodMin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m > values[i])
                m = values[i];
        }
        return globalTamgu->Providefloat(m);
    }
    
    Tamgu* MethodMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m < values[i])
                m = values[i];
        }
        return globalTamgu->Providefloat(m);
    }
    
    Tamgu* MethodMinMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double mn = values[0];
        double mx = mn;
        for (long i = 1; i < values.size(); i++) {
            if (mn > values[i])
                mn = values[i];
            if (mx < values[i])
                mx = values[i];
        }
        Tamgufvector* b = new Tamgufvector;
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
            values.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Float());
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
        contextualpattern = globalTamgu->ProvideConstfloat(values.back());
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
        return booleantamgu[Permute()];
    }

    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }

    
    Tamgu* MethodShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == a) {
                values.erase(values.begin() + i);
                return aTRUE;
            }
        }
        return aFALSE;
    }


    Tamgu* MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------

    void Getshape(vector<long>& sh) {
        long nb = 1;
        for (long i = 0; i < shape.size(); i++)
            nb *= shape[i];
        if (nb <= values.size())
            sh = shape;
    }
    
    Tamgu* push(Tamgu* a) {
        values.push_back(a->Float());
        return this;
    }
    

    Exporting Tamgu* Push(Tamgu*);
    
    Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        locking();
        values.push_back(a->Float());
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
        double c = values.back();
        values.pop_back();
        unlocking();
        return globalTamgu->ProvideConstfloat(c);
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

        double v = 0;
        locking();
        for (; i < j; i++)
            v += values[i];
        unlocking();
        return globalTamgu->ProvideConstfloat(v);
    }

    Tamgu* Theproduct(long i, long j) {
        locking();
        long sz = values.size();
        if (!sz) {
            unlocking();
            return aZERO;
        }

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

        double v = 1;
        locking();
        for (; i < j; i++)
            v *= values[i];
        unlocking();
        return globalTamgu->ProvideConstfloat(v);
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
            values.push_back(ke->Float());
        else
            values.insert(values.begin() + idx, ke->Float());
        unlocking();
    }

    Tamgu* Inverse() {
        locking();
        Tamgufvector* vect = globalTamgu->Providefvector();
        for (long i = (long)values.size() - 1; i >= 0; i--) {
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

class TamguIterationfvector : public TamguIteration {
    public:
    BLONG itx;
    Tamgufvector* ref;
    Locking _lock;

    TamguIterationfvector(Tamgufvector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
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
        return globalTamgu->ProvideConstfloat(ref->values[itx]);
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
            itx = ref->values.size();
            itx--;
        }
        else
            itx = 0;
        return aTRUE;
    }
};


//---------------------------------------------------------------------------------------------------------------------
class Tamgufvectorbuff : public Tamgufvector {
    public:
    long idx;
    bool used;

    Tamgufvectorbuff(long i)  {
        //Do not forget your variable initialisation
        idx = i;
        used = false;
    }

    bool Candelete() {
        return false;
    }

    Tamgu* anInstance(long i) {
        return new Tamgufvectorbuff(i);
    }

    void Resetreference(short r) {
        r = reference - r;
        if (r <= 0) {
            reference.store(0);
            if (!protect) {
                protect = true;

                values.clear();
                if (!globalTamgu->threadMODE && used)
                    globalTamgu->fvectorempties.push_back(idx);
                used = false;
            }
        }
        else
            reference.store(r);
    }

};
//---------------------------------------------------------------------------------------------------------------------

    //We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
    //of how to declare a new method.
class Tamgua_fvector;
    //This typedef defines a type "a_fvectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgua_fvector::*a_fvectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------


class Tamgua_fvector : public TamguContainer {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<a_fvectorMethod> methods;
    
    
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    atomic_value_vector<double> values;
    bool isconst;
        //---------------------------------------------------------------------------------------------------------------------
    Tamgua_fvector(TamguGlobal* g, Tamgu* parent = NULL) : values(0, false), TamguContainer(g, parent) {
     investigate |= is_number;
            //Do not forget your variable initialisation
        isconst = false;
    }
    
    Tamgua_fvector() : values(0, false) {
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
        return Tamgua_fvector::idtype;
    }
    
    string Typename() {
        return "a_fvector";
    }
    
    
    
    bool isFloat() {
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
            Tamgua_fvector* v = new Tamgua_fvector();
            v->values = values;
            return v;
        }
        return this;
    }
    
    bool duplicateForCall() {
        return isconst;
    }
    
    
    Exporting Tamgu* getvalue(long i);
    
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
        return methods.check(n);
    }
    
    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgua_fvector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgua_fvector* v = new Tamgua_fvector();
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
    
    
    static void AddMethod(TamguGlobal* g, string name, a_fvectorMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);
    
    
    void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[idtype])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[idtype].find(n) !=  globalTamgu->infomethods[idtype].end())
              return globalTamgu->infomethods[idtype][n];
             return "Unknown method";
    }
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double a = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
        atomic_value_vector_iterator<double> it(values);
        for (; !it.end(); it.next()) {
            if (it.second == a) {
                values.pop(it.first);
                return aTRUE;
            }
        }
        return aFALSE;
    }
    
    Tamgu* MethodMin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double m = 0;
        bool first = true;
        atomic_value_vector_iterator<double> it(values);
        for (; !it.end(); it.next()) {
            if (first) {
                m = it.second;
                first = false;
            }
            else {
                if (m > it.second)
                    m = it.second;
            }
        }
        return globalTamgu->Providefloat(m);
    }
    
    Tamgu* MethodMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double m = 0;
        bool first = true;
        atomic_value_vector_iterator<double> it(values);
        for (; !it.end(); it.next()) {
            if (first) {
                m = it.second;
                first = false;
            }
            else {
                if (m < it.second)
                    m = it.second;
            }
        }
        return globalTamgu->Providefloat(m);
    }
    
    Tamgu* MethodMinMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double mn = 0;
        double mx = 0;
        bool first = true;
        atomic_value_vector_iterator<double> it(values);
        for (; !it.end(); it.next()) {
            if (first) {
                mn = it.second;
                mx = mn;
                first = false;
            }
            else {
                if (mn > it.second)
                    mn = it.second;
                if (mx < it.second)
                    mx = it.second;
            }
        }
        Tamgua_fvector* b = new Tamgua_fvector;
        b->values.push_back(mn);
        b->values.push_back(mx);
        return b;
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
        atomic_value_vector_iterator<double> it(values);
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
        return globalTamgu->ProvideConstfloat(values.back());
    }
    
    Tamgu* MethodEditDistance(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        unsigned long dst = EditDistance(v);
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
    
    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }
    
        //---------------------------------------------------------------------------------------------------------------------
    
    Exporting Tamgu* Push(Tamgu*);
    Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread) {
        values.push_back(a->Float());
        return this;
    }
    
    
    Exporting Tamgu* Pop(Tamgu*);

    Tamgu* Poplast() {
        if (values.size() == 0)
            return aNOELEMENT;
        return globalTamgu->ProvideConstfloat(values.remove_back());
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
        
        double v = 0;
        for (; i < j; i++)
            v += values[i];
        
        return globalTamgu->ProvideConstfloat(v);
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
        
        double v = 1;
        for (; i < j; i++)
            v *= values[i];
        
        return globalTamgu->ProvideConstfloat(v);
    }
    
    double Sum() {
        double v = 0;
        atomic_value_vector_iterator<long> it;
        for (; !it.end(); it.next())
            v += it.second;
        return v;
    }
    
    double Product() {
        if (!values.size())
            return 0;
        
        double v = 1;
        atomic_value_vector_iterator<long> it;
        for (; !it.end(); it.next())
            v *= it.second;
        
        return v;
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
        Tamgua_fvector* vect = new Tamgua_fvector();
        for (long i = (long)values.size() - 1; i >= 0; i--) {
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

class TamguIterationa_fvector : public TamguIteration {
public:
    
    atomic_value_vector_iterator<double> it;
    Tamgua_fvector* av;
    
    TamguIterationa_fvector(Tamgua_fvector* v, bool d, TamguGlobal* g = NULL) : it(v->values), TamguIteration(d, g) {
        av = v;
    }
    
    Tamgu* Key() {
        return globalTamgu->ProvideConstint(it.first);
    }
    
    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it.second);
    }
    
    Tamgu* Value() {
        return globalTamgu->ProvideConstint(it.second);
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

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutable.h
 Date       : 2017/09/01
 Purpose    : table implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgutable_h
#define tamgutable_h

#include "tamguint.h"
#include "tamguvector.h"
#include "tamgufloat.h"
#include "tamgustring.h"

class TamguCallFunctionTaskell;
class TamguFunctionLambda;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgutable;
//This typedef defines a type "tableMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgutable::*tableMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class TamguIterationtable;

//---------------------------------------------------------------------------------------------------------------------

class Tamgutable : public TamguObjectLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<tableMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    std::atomic<Tamgu*>* values;
    std::atomic<long> position;
    long size;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgutable(TamguGlobal* g, Tamgu* parent = NULL) : TamguObjectLockContainer(g, parent) {
        //Do not forget your variable initialisation
        values = NULL;
        size = 0;
        position = 0;
    }

    Tamgutable(long sz = 0) {
        //Do not forget your variable initialisation
        values = NULL;
        size = 0;
        position = 0;
        if (sz)
            Resize(sz);
    }

    ~Tamgutable() {
        if (values != NULL)
            delete[] values;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);


    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

    short Type() {
        return Tamgutable::idtype;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "table";
    }

    bool isObjectContainer() {
        return true;
    }

    bool isContainerClass() {
        return true;
    }

    bool Resize(long sz) {
        if (sz <= size)
            return true;

        long i;

        if (values == NULL) {
            values = new std::atomic<Tamgu*>[sz];
            for (i = 0; i < sz; i++)
                values[i] = aNOELEMENT;
            size = sz;
            return true;
        }

        //if it is in a thread, we cannot resize it...
        if (hasalsoLock())
            return false;

        std::atomic<Tamgu*>* v = new std::atomic<Tamgu*>[sz];
        Tamgu* e;
        for (i = 0; i < sz; i++) {
            if (i < size) {
                e = values[i];
                v[i] = e;
            }
            else
                v[i] = aNOELEMENT;
        }

        size = sz;
        delete[] values;
        values = v;
        return true;
    }

    Tamgu* Atom(bool forced = false) {
        if (forced) {
            Tamgutable* v = new Tamgutable;
            if (size) {
                v->Resize(size);
                Tamgu* a;
                for (size_t i = 0; i < size; i++) {
                    a = values[i];
                    a = a->Atom(true);
                    a->Setreference();
                    v->values[i] = a;
                }
            }
            return v;
        }
        return this;
    }

    bool duplicateForCall() {
        return false;
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
        return new Tamgutable();
    }

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgutable* v = new Tamgutable();
        if (a->isContainer()) {
            v->Resize(a->Size());
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next())
                v->Push(it->IteratorValue());
            it->Release();
            return v;
        }
        
        v->Resize(size);
        for (long i=0; i<size;i++)
            v->Push(a);
        return v;
    }


    Exporting TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, tableMethod func, unsigned long arity, string infos);
    
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
    Tamgu* getvalue(BLONG i) {
        
        if (i < 0 || i >= size)
            return aNOELEMENT;
        return values[i];
    }
    
    long getinteger(long i) {
        if (i < 0 || i >= size)
            return 0;
        Tamgu* e = values[i];
        return e->Integer();
    }
    
    BLONG getlong(long i) {
        if (i < 0 || i >= size)
            return 0;
        
        Tamgu* e = values[i];
        return e->Long();
    }
    
    short getshort(long i) {
        if (i < 0 || i >= size)
            return 0;
        
        Tamgu* e = values[i];
        return e->Short();
    }
    
    uchar getbyte(long i) {
        if (i < 0 || i >= size)
            return 0;
        Tamgu* e = values[i];
        return e->Byte();
    }
    
    float getdecimal(long i) {
        if (i < 0 || i >= size)
            return 0;
        Tamgu* e = values[i];
        return e->Decimal();
    }
    
    double getfloat(long i) {
        if (i < 0 || i >= size)
            return 0;
        Tamgu* e = values[i];
        return e->Float();
    }
    
    string getstring(long i) {
        if (i < 0 || i >= size)
            return "";
        Tamgu* e = values[i];
        return e->String();
    }
    
    wstring getustring(long i) {
        if (i < 0 || i >= size)
            return L"";
        Tamgu* e = values[i];
        return e->UString();
    }
    
    Exporting void Storevalue(string& u);
    Exporting void Storevalue(wstring& u);

    Exporting void storevalue(string u);
    Exporting void storevalue(float u);
    Exporting void storevalue(short u);
    Exporting void storevalue(wstring u);
    Exporting void storevalue(long u);
    Exporting void storevalue(BLONG u);
    Exporting void storevalue(double u);
    Exporting void storevalue(unsigned char u);
    Exporting void storevalue(wchar_t u);
    //---------------------------------------------------------------------------------------------------------------------
    Exporting unsigned long EditDistance(Tamgu* e);

    
    void unmark() {
        if (!lockingmark())
            return;
        
        usermark=false;

        Tamgu* e;
        for (long i = 0; i< size; i++) {
            e = values[i];
            return e->unmark();
        }
        
        unlockingmark();
    }

    Exporting void Cleanreference(short inc);

    Exporting void Setreference(short r);
    Exporting void Setreference();
    Exporting void Resetreference(short r = 1);

    void Setprotect(bool n) {
        if (!lockingmark())
            return;

        protect = n;
        
        Tamgu* e;
        for (size_t i = 0; i < size; i++) {
            e = values[i];
            return e->Setprotect(n);
        }
        
        unlockingmark();
    }

    void Protectcontainervalues() {
        if (!lockingmark())
            return;
        
        protect = true;
        Tamgu* e;
        for (size_t i = 0; i < size; i++) {
            e = values[i];
            return e->Setprotect(true);
        }
        unlockingmark();
    }
    
    void Popping() {
        if (!lockingmark())
            return;
        
        protect = false;
        if (Reference() <= 0)
            protect = true;
        Tamgu* e;
        for (size_t i = 0; i < size; i++) {
            e = values[i];
            return e->Popping();
        }
        unlockingmark();
    }

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }

    Tamgu* MethodFlatten(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* res = globalTamgu->Providevector();
        Flattening(res, this);
        return res;
    }

    Tamgu* MethodResize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return booleantamgu[Resize(callfunc->Evaluate(0, contextualpattern, idthread)->Integer())];
    }

    Tamgu* MethodPush(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v;
        for (size_t i = 0; i < callfunc->Size(); i++) {
            v = callfunc->Evaluate(i, contextualpattern, idthread);
            if (v->isError())
                return v;
            Push(v);
            v->Release();
        }
        return aTRUE;
    }

    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (size == 0)
            return aNOELEMENT;
        long i = 0;
        return Last(i);
    }

    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
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
        return booleantamgu[Resize(callfunc->Evaluate(0, contextualpattern, idthread)->Integer())];
    }

    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        
        bool beg = true;
        string res;
        Tamgu* e;
        for (size_t it = 0; it<size;it++) {
            if (beg == false)
                res += sep;
            beg = false;
            e = values[it];
            res += e->String();
        }

        return globalTamgu->Providewithstring(res);
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

    Tamgu* MethodRemove(Tamgu* e, short idthread, TamguCall* callfunc) {
        Tamgu* a = callfunc->Evaluate(0, e, idthread);
        size_t found = -1;
        
        for (size_t i = 0; i < size; i++) {
            e = values[i];
            if (e->same(a)->Boolean()) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            e = values[found].exchange(aNOELEMENT);
            e->Removereference(reference + 1);
            return aTRUE;
        }
        return aFALSE;
    }

    //---------------------------------------------------------------------------------------------------------

    //Raw push

    Tamgu* Last(long& idx) {
        if (size == 0)
            return aNOELEMENT;

        Tamgu* e;
        for (long i = size - 1; i >= 0; i--) {
            e = values[i];
            if (e != aNOELEMENT) {
                idx = i;
                return e;
            }
        }

        return aNOELEMENT;
    }
    
    Tamgu* push(Tamgu* v) {
        if (size == 0)
            return aNOELEMENT;

        Tamgu* e;
        for (; position < size; position++) {
            e = values[position];
            if (e == aNOELEMENT) {
                v = v->Atom();
                v->Addreference(investigate,reference + 1);
                e = values[position++].exchange(v);
                //In the case we are in a thread and a value has been pushed into it...
                e->Removereference(reference + 1);
                return this;
            }
        }

        position = 0;
        return aNOELEMENT;
    }

    bool Pushing(Tamgu* a) {
        if (position >= size)
            return false;
            
        a = a->Atom();
        a->Addreference(investigate,reference + 1);
        a = values[position++].exchange(a);
        a->Removereference(reference + 1);
        return true;
    }

    Exporting Tamgu* Push(Tamgu*);
    Exporting Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread);

    Exporting Tamgu* Pop(Tamgu*);
    Exporting Tamgu* Poplast();

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting Tamgu* Sort(TamguFunction* comp, short idthread, bool d);Exporting Tamgu* Sortstring(short idthread, bool d);Exporting Tamgu* Sortustring(short idthread, bool d);Exporting Tamgu* Sortint(short idthread, bool d);Exporting Tamgu* Sortfloat(short idthread, bool d);Exporting Tamgu* Unique();
    Tamgu* Inverse() {
        
        Tamgutable* vect = new Tamgutable;
        for (long i = size - 1; i >= 0; i--) {
            vect->Push(values[i]);
        }
        return vect;
    }

    Exporting void Shuffle();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    Exporting void Insert(long idx, Tamgu* ke);

    Exporting Tamgu* Map(short idthread);

    Tamgu* Vector(short idthread) {
        Tamguvector* vect = new Tamguvector;
        for (long i = 0; i < size; i++)
            vect->Push(values[i]);
        return vect;
    }

    Tamgu* Thesum(long i, long j) {
        long sz = size;
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
        else
            if (j>sz)
                j = sz;

        Tamgu* e = values[0];
        if (e->isNumber()) {
            double v = 0;
            for (; i < j; i++) {
                e = values[i];
                v += e->Float();
            }

            return new Tamgufloat(v);
        }

        string v;
        for (; i < j; i++) {
            e = values[i];
            v += e->String();
        }
        return new Tamgustring(v);
    }

    Tamgu* Theproduct(long i, long j) {
        long sz = size;
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
        else
            if (j>sz)
                j = sz;

        double v = 1;
        Tamgu* e;
        for (; i < j; i++) {
            e = values[i];
            v *= e->Float();
        }

        return new Tamgufloat(v);
    }

    double Sum() {
        
        double v = 0;
        Tamgu* e;
        for (int i = 0; i < size; i++) {
            e = values[i];
            v += e->Float();
        }
        return v;
    }

    double Product() {
        if (size == 0)
            return 0;
        Tamgu* e = values[0];
        double v = e->Float();
        for (int i = 1; i < size; i++) {
            e = values[i];
            v *= e->Float();
        }
        return v;
    }


    Exporting void Clear();
    

    Exporting string JSonString();
    Exporting string String();
    void Setstring(string& v, short idthread);

    //wstring UString() {}

    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


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
//---------------------------------------------------------------------------------
class TamguIterationtable : public TamguIteration {
    public:
    BLONG itx;
    Tamgutable* ref;

    TamguIterationtable(Tamgutable* v, bool d, TamguGlobal* g = NULL) : TamguIteration(d, g) {
        ref = v;
        itx = 0;
    }

    Tamgu* Key() {
        return globalTamgu->ProvideConstint(itx);
    }

    Tamgu* Value() {
        return ref->values[itx];
    }

    string Keystring() {
        
        return convertfromnumber(itx);
        
    }

    string Valuestring() {
        Tamgu* e = ref->values[itx];
        return e->String();
    }

    long Keyinteger() {
        return itx;
    }

    long Valueinteger() {
        Tamgu* e = ref->values[itx];
        return e->Integer();
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        Tamgu* e = ref->values[itx];
        return e->Decimal();
    }

    double Valuefloat() {
        Tamgu* e = ref->values[itx];
        return e->Float();
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
            if (itx >= ref->size)
                return aTRUE;
        }
        return aFALSE;
    }

    Tamgu* Begin() {
        if (reverse) {
            itx = ref->size - 1;
        }
        else
            itx = 0;
        return aTRUE;
    }

};

#endif

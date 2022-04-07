/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguvector.h
 Date       : 2017/09/01
 Purpose    : vector implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguvector_h
#define tamguvector_h

#include "vecte.h"
#include "tamguint.h"
#include "tamgufloat.h"
#include "tamgustring.h"

class TamguCallFunctionTaskell;
class TamguFunctionLambda;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguvector;
//This typedef defines a type "vectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguvector::*vectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class TamguIterationvector;

//---------------------------------------------------------------------------------------------------------------------

class Tamguvector : public TamguObjectLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<vectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    vector<Tamgu*> values;
    vector<long> shape;
    bool merge;

    //---------------------------------------------------------------------------------------------------------------------
    Tamguvector(TamguGlobal* g, Tamgu* parent = NULL) : TamguObjectLockContainer(g, parent) {
        //Do not forget your variable initialisation
        merge = false;
    }

    Tamguvector() {
        //Do not forget your variable initialisation
        merge = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting virtual Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting virtual Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Exporting Tamgu* EvalWithSimpleIndex(Tamgu* idx, short idthread, bool sgn);

    void SetConst() {
        investigate |= is_constante;
    }

    void Setmerge() {
        merge = true;
    }

    virtual bool isMerge() {
        return merge;
    }

    Exporting Tamgu* Getvalues(TamguDeclaration* dom, bool duplicate);
    Exporting bool isUnified(TamguDeclaration* dom);
    Exporting bool Unify(TamguDeclaration* dom, Tamgu* a);
    Exporting bool Insertvalue(Tamgu* dom, Tamgu* v, basebin_hash<Tamgu*>&);
    Exporting Tamgu* ExtractPredicateVariables(Tamgu* contextualpattern, TamguDeclaration* dom, Tamgu* c, Tamgu* e, short idthread, bool root);
    Exporting Tamgu* EvaluePredicateVariables(Tamgu* context, TamguDeclaration* dom, short idthread);

    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    
    virtual short Type() {
        return a_vector;
    }

    

    void Setidtype(TamguGlobal* global);
    
    virtual string Typename() {
        return "vector";
    }

    bool isObjectContainer() {
        return true;
    }

    bool isContainerClass() {
        return true;
    }

    bool isVectorContainer() {
        return true;
    }

    virtual Tamgu* Atom(bool forced = false) {
        if (forced) {
            Tamguvector* v = globalTamgu->Providevector();
            Tamgu* a;
            locking();
            for (size_t i = 0; i < values.size(); i++) {
                a = values[i]->Atom(true);
                a->Addreference(investigate,1);
                v->values.push_back(a);
            }
            unlocking();
            return v;
        }
        return this;
    }

    virtual bool duplicateForCall() {
        return isConst();
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    virtual Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return globalTamgu->Providevector();
    }

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamguvector* v = globalTamgu->Providevector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next())
                v->Push(it->IteratorValue());
            it->Release();
            return v;
        }
        
        for (long i=0; i<values.size();i++)
            v->Push(a);
        return v;
    }

    void AddInstruction(Tamgu* a) {
        a->Addreference(investigate,reference + 1);
        values.push_back(a);
    }

    Exporting TamguIteration* Newiteration(bool direction);


    static void AddMethod(TamguGlobal* g, string name, vectorMethod func, unsigned long arity, string infos);
    
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
    Tamgu* getvalue(BLONG i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return aNOELEMENT;
        }
        Tamgu* v = values[i];
        unlocking();
        return v;
    }

    string getstring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return "";
        }
        Tamgu* v = values[i];
        unlocking();
        return v->String();
    }
    
    wstring getustring(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return L"";
        }
        Tamgu* v = values[i];
        unlocking();
        return v->UString();
    }
    
    double getfloat(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        Tamgu* v = values[i];
        unlocking();
        return v->Float();
    }
    
    long getinteger(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        Tamgu* v = values[i];
        unlocking();
        return v->Integer();
    }
    
    float getdecimal(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        Tamgu* v = values[i];
        unlocking();
        return v->Decimal();
    }
    
    BLONG getlong(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        Tamgu* v = values[i];
        unlocking();
        return v->Long();
    }
    
    uchar getbyte(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        Tamgu* v = values[i];
        unlocking();
        return v->Byte();
    }
    
    short getshort(long i) {
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return 0;
        }
        Tamgu* v = values[i];
        unlocking();
        return v->Short();
    }
    
    Tamgu* Value(Tamgu* a) {
        long i = a->Integer();
        locking();
        if (i < 0 || i >= values.size()) {
            unlocking();
            return aNOELEMENT;
        }
        Tamgu* v = values[i];
        unlocking();
        return v;
    }

    void store(long k, Tamgu* v) {
        locking();
        if (k >= values.size())
            values.push_back(v);
        else {
            if (values[k] != NULL)
                values[k]->Removereference(reference + 1);
            values[k] = v;
        }
        v->Addreference(investigate, reference + 1);
        unlocking();
    }
    
	Exporting void storevalue(Tamgu* v, long beg, long end) {
        long sz = v->Size();
        for (;beg < end && beg < sz; beg++)
            Push(v->getvalue(beg));
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
    
    void unmark() {
        if (!lockingmark())
            return;
        
        usermark=false;

        for (long i = 0; i< values.size(); i++)
            values[i]->unmark();
       
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
        
        for (size_t i = 0; i<values.size(); i++)
            values[i]->Setprotect(n);
        
        unlockingmark();
    }

    void Protectcontainervalues() {
        if (!lockingmark())
            return;
        
        protect = true;
        
        for (size_t i = 0; i<values.size(); i++)
            values[i]->Setprotect(true);
        
        unlockingmark();
    }


    void Popping() {
        if (!lockingmark())
            return;
        
        protect = false;
        if (Reference() <= 0)
            protect = true;
        
        for (size_t i = 0; i < values.size(); i++)
            values[i]->Popping();
        
        unlockingmark();
    }
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

	Exporting Tamgu* MethodShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodMin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m->more(values[i])->Boolean())
                m = values[i];
        }
        return m;
    }
    
    Tamgu* MethodMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m->less(values[i])->Boolean())
                m = values[i];
        }
        return m;
    }
    
    Tamgu* MethodMinMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* mn = values[0];
        Tamgu* mx = mn;
        for (long i = 1; i < values.size(); i++) {
            if (mn->more(values[i])->Boolean())
                mn = values[i];
            if (mx->less(values[i])->Boolean())
                mx = values[i];
        }
        Tamguvector* b = new Tamguvector;
        b->values.push_back(mn);
        b->values.push_back(mx);
        return b;
    }   

    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    Tamgu* MethodFlatten(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* res = globalTamgu->Providevector();
        Flattening(res, this);
        return res;
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

    Tamgu* MethodNGrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        string u;
        string sep;
        long num =callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        
        if (callfunc->Size() == 2)
            sep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
            
        Tamgu* kvect = Selectasvector(contextualpattern);
        long mx = Size() - num + 1;
        for (int i = 0; i < mx; i++) {
            u = "";
            for (int j = i; j < i + num; j++) {
                if (j != i)
                    u += sep;
                u += getstring(j);
            }
            kvect->storevalue(u);
        }
        return kvect;
    }

    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
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

    Tamgu* MethodPoplast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Poplast();
    }
    

    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Tamgu* MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        bool d = false;
        Tamgu* comp = callfunc->Evaluate(0, contextualpattern, idthread);
        if (comp->isFunction()) {
            comp = comp->Body(idthread);
            if (comp == NULL || (comp->Size() != 1 && comp->Size() != 2))
                return globalTamgu->Returnerror("Expecting a comparison function with one or two parameters", idthread);
            if (callfunc->Size() == 2)
                d = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
        }
        else {
            if (comp->isFunctionParameter()) {
                if (comp->Size() > 2)
                    return globalTamgu->Returnerror("Expecting a comparison function with one parameter", idthread);
                if (callfunc->Size() == 2)
                    d = callfunc->Evaluate(1, contextualpattern, idthread)->Boolean();
            }
            else {
                d = comp->Boolean();
                comp = NULL;
            }
        }
        return Sort(comp, idthread, d);
    }

    Tamgu* MethodSortint(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortint(idthread, d);
    }

    Tamgu* MethodSortfloat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortfloat(idthread, d);
    }

    Tamgu* MethodSortstring(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortstring(idthread, d);
    }

    Tamgu* MethodSortustring(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        bool d = callfunc->Evaluate(0, contextualpattern, idthread)->Boolean();
        return Sortustring(idthread, d);
    }

    Tamgu* MethodReserve(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        long sz = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        Reserve(sz);
        return aTRUE;
    }

    Tamgu* MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        locking();
        bool beg = true;
        string res;
        for (size_t it = 0; it<values.size();it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it]->String();
        }
        unlocking();

        return globalTamgu->Providewithstring(res);
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
        Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
        size_t found = -1;
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i]->same(a)->Boolean()) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            values[found]->Removereference(reference + 1);
            values.erase(values.begin() + found);
            return aTRUE;
        }
        return aFALSE;
    }

    //---------------------------------------------------------------------------------------------------------

    void Getshape(vector<long>& sh) {
        long nb = 1;
        for (long i = 0; i < shape.size(); i++)
            nb *= shape[i];
        if (nb <= values.size())
            sh = shape;
    }

    //Raw push
    Tamgu* push(Tamgu* v) {
        values.push_back(v);
        v->Addreference(investigate, reference + 1);
        return this;
    }

    void pushatom(Tamgu* v) {
        v = v->Atom();
        values.push_back(v);
        v->Addreference(investigate, reference + 1);
    }

    inline void pushone(Tamgu* a) {
        a->Addreference(0,reference+1);
        values.push_back(a);
    }

    Exporting virtual Tamgu* Push(Tamgu*);
    Exporting Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread);

    Exporting virtual Tamgu* Pop(Tamgu*);

    void addstringto(string s, int i) {
        locking();
        if (values.size() == 0) {
            unlocking();
            return;
        }

        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addstringto(s);
        unlocking();
    }

    void addustringto(wstring s, int i) {
        locking();
        if (values.size() == 0) {
            unlocking();
            return;
        }

        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addustringto(s);
        unlocking();
    }

    void addstringto(wchar_t s, int i) {
        locking();
        if (values.size() == 0) {
            unlocking();
            return;
        }

        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addstringto(s);
        unlocking();
    }
    
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Tamgu* Last() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return aNOELEMENT;
        }
        Tamgu* r = values.back();
        unlocking();
        return r;
    }

    Exporting Tamgu* Sort(Tamgu* comp, short idthread, bool d);
    Exporting Tamgu* Sortstring(short idthread, bool d);
    Exporting Tamgu* Sortustring(short idthread, bool d);
    Exporting Tamgu* Sortint(short idthread, bool d);
    Exporting Tamgu* Sortfloat(short idthread, bool d);
    Exporting Tamgu* Unique();
    
    Tamgu* Poplast() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return aNOELEMENT;
        }
        Tamgu* c = values.back();
        values.pop_back();
        unlocking();
        c->Resetreference(reference);
        c->Protect();
        return c;
    }
    

    Tamgu* Inverse() {
        Tamguvector* vect = globalTamgu->Providevector();
        locking();
        for (long i = values.size() - 1; i >= 0; i--) {
            vect->Push(values[i]);
        }
        unlocking();
        return vect;
    }

    Exporting void Shuffle();
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    Exporting bool Permute();
    Exporting unsigned long EditDistance(Tamgu* e);
    Exporting void Insert(long idx, Tamgu* ke);

    Exporting Tamgu* Map(short idthread);
    Tamgu* Vector(short idthread) {
        return this;
    }

    Exporting Tamgu* Thesum(long i, long j);
    Exporting Tamgu* Theproduct(long i, long j);

    double Sum() {
        locking();
        double v = 0;
        for (int i = 0; i < values.size(); i++)
            v += values[i]->Float();
        unlocking();
       return v;
    }

    double Product() {
        locking();
        if (values.size() == 0) {
            unlocking();
            return 0;
        }
        double v = values[0]->Float();
        for (int i = 1; i < values.size(); i++)
            v *= values[i]->Float();
        unlocking();
        return v;
    }


    Exporting void Clear();
    

    Exporting string JSonString();
    Exporting virtual string String();
    Exporting virtual void Setstring(string& v, short idthread);


    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


    //Basic operations
    void Reserve(long d) {
        locking();
        if (d > values.size())
            values.reserve(d);
        unlocking();
    }

    Exporting long Size();

    virtual Tamgu* car(short idthread) {
        locking();
        if (!values.size()) {
            unlocking();
            return aNOELEMENT;
        }
        Tamgu* r = values[0]->Atom();
        unlocking();
        return r;
    }
    
    virtual Tamgu* cdr(short idthread);

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

    Exporting virtual Tamgu* same(Tamgu* a);

};
//---------------------------------------------------------------------------------
class TamguIterationvector : public TamguIteration {
    public:
    BLONG itx;
    Tamguvector* ref;
    Locking _lock;

    TamguIterationvector(Tamguvector* v, bool d, TamguGlobal* g = NULL) : _lock(v), TamguIteration(d, g) {
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
        return ref->values[itx]->String();
    }

    long Keyinteger() {
        return itx;
    }

    long Valueinteger() {
        return ref->values[itx]->Integer();
    }

    double Keyfloat() {
        return itx;
    }

    float Valuedecimal() {
        return ref->values[itx]->Decimal();
    }

    double Valuefloat() {
        return ref->values[itx]->Float();
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
class Tamguvectorbuff : public Tamguvector {
    public:
    long idx;
    bool used;

    Tamguvectorbuff(long i)  {
        //Do not forget your variable initialisation
        idx = i;
        used = false;
    }

    bool Candelete() {
        return false;
    }

    Exporting void Resetreference(short r);

};
//---------------------------------------------------------------------------------------------------------------------
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgua_vector;
    //This typedef defines a type "vectorMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgua_vector::*a_vectorMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    //---------------------------------------------------------------------------------------------------------------------
class TamguIterationa_vector;

    //---------------------------------------------------------------------------------------------------------------------

class Tamgua_vector : public TamguObjectContainer {
public:
        //We export the methods that will be exposed for our new object
        //this is a static object, which is common to everyone
        //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<a_vectorMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;
    
    static Exchanging short idtype;
    
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //Your personal variables here...
    atomic_vector<Tamgu*> values;
    
        //---------------------------------------------------------------------------------------------------------------------
    Tamgua_vector(TamguGlobal* g, Tamgu* parent = NULL) : values(NULL, true), TamguObjectContainer(g, parent) {
            //Do not forget your variable initialisation
    }
    
    Tamgua_vector() : values(NULL, true)  {
            //Do not forget your variable initialisation
    }
    
    bool lockingmark() {
        if (hasalsoLock())
            values._lock.lock();
        if (loopmark) {
            if (hasLock())
                values._lock.unlock();
            return false;
        }
        loopmark = true;
        return true;
    }
    
    void unlockingmark() {
        loopmark = false;
        if (hasalsoLock())
            values._lock.unlock();
    }
        //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    
    void SetConst() {
        investigate |= is_constante;
    }
    
    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);
    
    short Type() {
        return Tamgua_vector::idtype;
    }
    
	void Setidtype(TamguGlobal* global) {
		Tamgua_vector::InitialisationModule(global, "");
	}

    string Typename() {
        return "a_vector";
    }
    
    bool isObjectContainer() {
        return true;
    }
    
    bool isContainerClass() {
        return true;
    }
    
    bool isVectorContainer() {
        return true;
    }
    
    Tamgu* Atom(bool forced = false) {
        if (forced) {
            Tamgua_vector* v = new Tamgua_vector();
            Tamgu* a;
            for (size_t i = 0; i < values.size(); i++) {
                a = values[i]->Atom(true);
                a->Addreference(investigate,1);
                v->values.push_back(a);
            }
            
            return v;
        }
        return this;
    }
    
    bool duplicateForCall() {
        return isConst();
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
        return new Tamgua_vector();
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgua_vector* v = new Tamgua_vector();
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next())
                v->Push(it->IteratorValue());
            it->Release();
            return v;
        }

        atomic_vector_iterator<Tamgu*> it(values);
        for (; !it.end(); it.next())
            v->Push(a);
        return v;
    }
    
    void AddInstruction(Tamgu* a) {
        a->Addreference(investigate,reference + 1);
        values.push_back(a);
    }
    
    
    
    Exporting TamguIteration* Newiteration(bool direction);
    
    
    static void AddMethod(TamguGlobal* g, string name, a_vectorMethod func, unsigned long arity, string infos);
    
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
    Tamgu* getvalue(BLONG i) {
        if (i < 0)
            return aNOELEMENT;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return aNOELEMENT;
        return a;
    }
    
    string getstring(long i) {
        if (i < 0)
            return "";
        
        Tamgu* a = values[i];
        if (a == NULL)
            return "";
        return a->String();
    }
    
    wstring getustring(long i) {
        if (i < 0)
            return L"";
        
        Tamgu* a = values[i];
        if (a == NULL)
            return L"";
        return a->UString();
    }
    
    double getfloat(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Float();
    }
    
    long getinteger(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Integer();
    }
    
    float getdecimal(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Decimal();
    }
    
    BLONG getlong(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        return a->Long();
    }
    
    short getshort(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        
        return a->Short();
    }
    
    uchar getbyte(long i) {
        if (i < 0)
            return 0;
        
        Tamgu* a = values[i];
        if (a == NULL)
            return 0;
        
        return a->Byte();
    }
    
    Tamgu* Value(Tamgu* a) {
        long i = a->Integer();
        if (i < 0)
            return aNOELEMENT;
        
        a = values[i];
        if (a == NULL)
            return aNOELEMENT;
        
        return a;
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
    
    void unmark() {
        if (!lockingmark())
            return;
        
        usermark=false;

        atomic_vector_iterator<Tamgu*> it(values);
        
        while (!it.end()) {
            it.second->unmark();
            it.next();
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
        
        atomic_vector_iterator<Tamgu*> it(values);
        while (!it.end()) {
            it.second->Setprotect(n);
            it.next();
        }
        
        unlockingmark();
    }
    
    void Protectcontainervalues() {
        if (!lockingmark())
            return;

        protect = true;
        
        atomic_vector_iterator<Tamgu*> it(values);
        while (!it.end()) {
            it.second->Setprotect(true);
            it.next();
        }
        unlockingmark();
    }
    
    
    void Popping() {
        if (!lockingmark())
            return;
                
        protect = false;
        if (Reference() <= 0)
            protect = true;
        
        atomic_vector_iterator<Tamgu*> it(values);
        while (!it.end()) {
            it.second->Popping();
            it.next();
        }
        
        unlockingmark();
    }
        //---------------------------------------------------------------------------------------------------------------------
        //This SECTION is for your specific implementation...
        //This is an example of a function that could be implemented for your needs.
    
    Tamgu* MethodMin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m->more(values[i])->Boolean())
                m = values[i];
        }
        return m;
    }
    
    Tamgu* MethodMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* m = values[0];
        for (long i = 1; i < values.size(); i++) {
            if (m->less(values[i])->Boolean())
                m = values[i];
        }
        return m;
    }
    
    Tamgu* MethodMinMax(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* mn = values[0];
        Tamgu* mx = mn;
        for (long i = 1; i < values.size(); i++) {
            if (mn->more(values[i])->Boolean())
                mn = values[i];
            if (mx->less(values[i])->Boolean())
                mx = values[i];
        }
        Tamgua_vector* b = new Tamgua_vector;
        b->values.push_back(mn);
        b->values.push_back(mx);
        return b;
    }

    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    
    Tamgu* MethodFlatten(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* res = globalTamgu->Providevector();
        Flattening(res, this);
        return res;
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
    
    Tamgu* MethodNGrams(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        string u;
        string sep;
        long num =callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
        
        if (callfunc->Size() == 2)
            sep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
        
        Tamgu* kvect = Selectasvector(contextualpattern);
        long mx = values.size() - num + 1;
        for (int i = 0; i < mx; i++) {
            u = "";
            for (int j = i; j < i + num; j++) {
                if (j != i)
                    u += sep;
                u += getstring(j);
            }
            kvect->storevalue(u);
        }
        return kvect;
    }
    
    Tamgu* MethodLast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (values.size() == 0)
            return aNOELEMENT;
        return values.back();
    }
    
    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        return Merging(v);
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
    
    Tamgu* MethodPoplast(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Poplast();
    }
    
    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = aMINUSONE;
        if (callfunc->Size() == 1)
            pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }
    
    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //The separator between values
        string sep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        bool beg = true;
        string res;
        for (size_t it = 0; it < values.size();it++) {
            if (beg == false)
                res += sep;
            beg = false;
            res += values[it]->String();
        }
        
        return globalTamgu->Providewithstring(res);
    }
    
    Tamgu* MethodUnique(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Unique();
    }
    
    Tamgu* MethodReverse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return Inverse();
    }
    
    Tamgu* MethodRemove(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* a = callfunc->Evaluate(0, contextualpattern, idthread);
        size_t found = -1;
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i]->same(a)->Boolean()) {
                found = i;
                break;
            }
        }
        if (found != -1) {
            values[found]->Removereference(reference + 1);
            values.erase(found);
            return aTRUE;
        }
        return aFALSE;
    }
    
        //---------------------------------------------------------------------------------------------------------
    
        //Raw push
    Tamgu* push(Tamgu* v) {
        values.push_back(v);
        v->Addreference(investigate);
        return this;
    }
    
    Exporting Tamgu* Push(Tamgu*);
    Exporting Tamgu* Push(TamguGlobal* g, Tamgu* a, short idhtread);
    
    Exporting Tamgu* Pop(Tamgu*);
    
    void addstringto(string s, int i) {
        if (values.size() == 0)
            return;
        
        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            ke = values[i];
            if (ke == NULL)
                return;
        }
        ke->addstringto(s);
    }
    
    void addustringto(wstring s, int i) {
        if (values.size() == 0)
            return;
        
        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            ke = values[i];
            if (ke == NULL)
                return;
        }
        ke->addustringto(s);
    }
    
    void addstringto(wchar_t s, int i) {
        if (values.size() == 0)
            return;
        
        Tamgu* ke;
        if (i < 0) {
            ke = values.back();
        }
        else {
            if (i >= values.size())
                return;
            ke = values[i];
        }
        ke->addstringto(s);
    }
    
        //---------------------------------------------------------------------------------------------------------------------
    
        //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
        //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
            //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
            //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }
    
    Tamgu* Last() {
        if (values.size() == 0)
            return aNULL;
        return values.back();
    }
    
    Tamgu* Poplast() {
        if (values.size() == 0)
            return aNOELEMENT;
        Tamgu* c = values.remove_back();
        c->Resetreference(reference);
        c->Protect();
        return c;
    }

    Exporting Tamgu* Unique();
    
    Tamgu* Inverse() {
        Tamguvector* vect = globalTamgu->Providevector();
        atomic_vector_iterator<Tamgu*> it(values);
        while (!it.end()) {
            vect->values.insert(vect->values.begin(), it.second);
            it.next();
        }
        return vect;
    }
    
    Exporting Tamgu* Merging(Tamgu*);
    Exporting Tamgu* Combine(Tamgu*);
    
    Exporting Tamgu* Map(short idthread);
    Tamgu* Vector(short idthread) {
        Tamguvector* v = globalTamgu->Providevector();
        atomic_vector_iterator<Tamgu*> it(values);
        while (!it.end()) {
            values.push_back(it.second);
            it.next();
        }
        return v;
    }
    
    Exporting Tamgu* Thesum(long i, long j);
    Exporting Tamgu* Theproduct(long i, long j);
    
    double Sum() {
        double v = 0;
        atomic_vector_iterator<Tamgu*> it(values);
        while (!it.end()) {
            v += it.second->Float();
            it.next();
        }
        
        return v;
    }
    
    double Product() {
        if (values.size() == 0)
            return 0;

        atomic_vector_iterator<Tamgu*> it(values);
        double v = 1;
        while (!it.end()) {
            v *= it.second->Float();
            it.next();
        }
        return v;
    }
    
    
    Exporting void Clear();
    
    
    Exporting string JSonString();
    Exporting string String();
    Exporting void Setstring(string& v, short idthread);
    
    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();
    
    void Insert(long idx, Tamgu* ke) {
        ke = ke->Atom();
        values.insert(idx, ke);
        ke->Addreference(investigate,reference+1);
    }

    
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
    //---------------------------------------------------------------------------------
class TamguIterationa_vector : public TamguIteration {
public:
    atomic_vector_iterator<Tamgu*> ref;
    
    TamguIterationa_vector(Tamgua_vector* v, bool d, TamguGlobal* g = NULL) : ref(v->values), TamguIteration(d, g) {}
    
    Tamgu* Key() {
        return globalTamgu->ProvideConstint(ref.first);
    }
    
    Tamgu* Value() {
        return ref.second;
    }
    
    string Keystring() {
        
        return convertfromnumber(ref.first);
        
    }
    
    string Valuestring() {
        return ref.second->String();
    }
    
    long Keyinteger() {
        return ref.first;
    }
    
    long Valueinteger() {
        return ref.second->Integer();
    }
    
    double Keyfloat() {
        return ref.first;
    }
    
    float Valuedecimal() {
        return ref.second->Decimal();
    }
    
    double Valuefloat() {
        return ref.second->Float();
    }
    
    void Next() {
        ref.next();
    }
    
    Tamgu* End() {
        return booleantamgu[ref.end()];
    }
    
    Tamgu* Begin() {
        return aTRUE;
    }
    
};

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
class TamguConstvector : public Tamguvector {
    public:
    bool evaluate;

    TamguConstvector(TamguGlobal* g, Tamgu* parent = NULL) : Tamguvector(g, parent) {
        //Do not forget your variable initialisation
        evaluate = false;
        investigate |= is_constcontainer;
    }

    TamguConstvector() : Tamguvector() {
        //Do not forget your variable initialisation
        evaluate = false;
        investigate |= is_constcontainer;
    }

    bool isAssignable() {
        return true;
    }

    Exporting void Prepare(Tamgu* env, short idthread);

    bool baseValue() {
        if (evaluate)
            return false;
        return true;
    }

    short Type() {
        return a_constvector;
    }

    virtual Tamgu* Atom(bool forced = false) {
        Tamguvector* v = globalTamgu->Providevector();
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++)
            v->Push(values[i]);
        return v;
    }

    bool duplicateForCall() {
        return true;
    }

    Exporting bool Checkvariable();
    Exporting bool Setvalue(Tamgu* index, Tamgu* value, short idthread, bool strict = false);

    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* index, Tamgu* value, short idthread);
    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return this;
    }


    bool isEvaluate() {
        return evaluate;
    }

    void Setevaluate(bool v) {
        evaluate = v;
    }

    Exporting Tamgu* same(Tamgu* a);
    void Resetreference(short inc) {}
    void Release() {}
};

class TamguConstvectormerge : public TamguConstvector {
    public:

    Exporting Tamgu* ExtractPredicateVariables(Tamgu* contextualpattern, TamguDeclaration* dom, Tamgu* c, Tamgu* e, short idthread, bool root);    TamguConstvectormerge(TamguGlobal* g, Tamgu* parent = NULL) : TamguConstvector(g, parent) {}

    short Type() {
        return a_vectormerge;
    }

    Exporting string String();
    Tamgu* Atom(bool forced = false) {
        Tamguvector* v = globalTamgu->Providevector();
        v->merge = true;
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++)
            v->Push(values[i]);
        return v;
    }

    bool isMerge() {
        return true;
    }

};


//---------------------------------------------------------------------------------
class TamguIterationInfinitevector : public TamguIteration {
    public:
    long itx;
    long inc;

    TamguIterationInfinitevector(bool d, TamguGlobal* g = NULL) : TamguIteration(d, g) {}


    Tamgu* Key() {
        return globalTamgu->ProvideConstint(itx);
    }

    Tamgu* Value() {
        return globalTamgu->ProvideConstint(itx);
    }

    long Keyinteger() {
        return itx;
    }

    long Valueinteger() {
        return itx;
    }

    double Keyfloat() {
        return itx;
    }

    double Valuefloat() {
        return itx;
    }

    void Next() {
        itx += inc;
    }

    long IteratorKeyInteger() {
        return itx;
    }

    double IteratorKeyFloat() {
        return itx;
    }

    Tamgu* End() {
        return aFALSE;
    }

    Tamgu* Begin() {
        return aTRUE;
    }

};

//---------------------------------------------------------------------------------
class TamguIterationInfinitefloatvector : public TamguIteration {
    public:
    double itx;
    double inc;

    TamguIterationInfinitefloatvector(bool d, TamguGlobal* g = NULL) : TamguIteration(d, g) {}


    Tamgu* Key() {
        return globalTamgu->ProvideConstfloat(itx);
    }

    Tamgu* Value() {
        return globalTamgu->ProvideConstfloat(itx);
    }

    long Keyinteger() {
        return (long)itx;
    }

    long Valueinteger() {
        return (long)itx;
    }

    double Keyfloat() {
        return itx;
    }

    double Valuefloat() {
        return itx;
    }

    void Next() {
        itx += inc;
    }

    long IteratorKeyInteger() {
        return (long)itx;
    }

    double IteratorKeyFloat() {
        return itx;
    }

    Tamgu* End() {
        return aFALSE;
    }

    Tamgu* Begin() {
        return aTRUE;
    }

};

//---------------------------------------------------------------------------------
class TamguInfinitevector : public TamguTracked {
    public:

    Tamgu* seed;
    Tamgu* step;
    int direction;
    bool clean;
    bool compute;

    TamguInfinitevector(int d, TamguGlobal* global, Tamgu* parent = NULL) : TamguTracked(a_infinitive, global, parent) {
        clean = false;
        seed = aNULL;
        compute = false;
        if (d == 1)
            step = aONE;
        else
            step = aMINUSONE;
        direction = d;
    }

    bool isContainerClass() {
        return true;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return globalTamgu->Providevector();
    }

    Exporting Tamgu* Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* env, TamguFunctionLambda* bd, short idthread);
    Exporting Tamgu* Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct);

    Exporting TamguIteration* Newiteration(bool direction);
    void AddInstruction(Tamgu* s) {
        if (seed == aNULL)
            seed = s;
        else {
            step = s->Atom();
            step->Setreference();
            clean = true;
        }
    }

    Tamgu* Put(Tamgu* index, Tamgu* value, short idthread) {
        return this;
    }

    Exporting string String();
    long Size() {
        return 0;
    }

    bool isInfinite() {
        return true;
    }

    void Resetreference(short inc) {}
    void Release() {}

};

class TamguCycleVector : public TamguTracked {
    public:

    Tamgu* base;
    Tamgu* value;
    bool repeat;

    TamguCycleVector(Tamgu* v, bool r, TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_cycle, g, parent) {
        base = v;
        value = aNULL;
        repeat = r;
    }

    Exporting Tamgu* Newinstance(short idthread, Tamgu* f = NULL);
    Exporting TamguIteration* Newiteration(bool direction);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    bool isInfinite() {
        return true;
    }

    void Resetreference(short inc) {}
    void Release() {}

};

class TamguReplicateVector : public TamguTracked {
    public:

    Tamgu* base;
    Tamgu* nbbase;
    Tamgu* value;
    Tamgu* nb;

    TamguReplicateVector(Tamgu* v, TamguGlobal* g, Tamgu* parent = NULL) : TamguTracked(a_replicate, g, parent) {
        base = v;
        nbbase = aNULL;
        value = aNULL;
        nb = aNULL;
    }

    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);Exporting Tamgu* Newiterator(bool);
    Exporting Tamgu* Newinstance(short idthread, Tamgu* f = NULL);
    Exporting TamguIteration* Newiteration(bool direction);
    void AddInstruction(Tamgu* v) {
        nbbase = v;
    }

    bool isInfinite() {
        return true;
    }

    void Clear() {
        value->Resetreference();
        value = aNULL;
        nb->Resetreference();
        nb = aNULL;
    }

    void Resetreference(short inc) {}
    void Release() {}

};

class TamguIteratorCycleElement : public TamguIteration {
    public:
    TamguCycleVector* cycle;
    int itx;
    TamguGlobal* global;

    TamguIteratorCycleElement(TamguCycleVector* k, TamguGlobal* g = NULL) : TamguIteration(false, g) {
        cycle = k;
        itx = 0;
        global = g;
    }

    ~TamguIteratorCycleElement() {
        cycle->value->Resetreference();
        cycle->value = aNULL;
    }

    Tamgu* Key() {
        return globalTamgu->ProvideConstint(itx);
    }

    long Keyinteger() {
        return itx;
    }

    double Keyfloat() {
        return itx;
    }

    Tamgu* Value() {
        return cycle->value;
    }

    void Next() {
        itx++;
    }

    Tamgu* End() {
        return aFALSE;
    }

    Tamgu* Begin() {
        itx = 0;
        return aTRUE;
    }
};

class TamguIteratorCycleVector : public TamguIteration {
    public:
    TamguCycleVector* cycle;
    TamguIteration* iter;
    TamguGlobal* global;

    TamguIteratorCycleVector(TamguCycleVector* k, TamguGlobal* g = NULL) : TamguIteration(false, g) {
        cycle = k;
        global = g;
        iter = (TamguIteration*)cycle->value->Newiteration(false);
    }

    ~TamguIteratorCycleVector() {
        iter->Release();
        cycle->value->Resetreference();
        cycle->value = aNULL;
    }

    Tamgu* Key() {
        return iter->Key();
    }

    Tamgu* Value() {
        return iter->Value();
    }

    string Keystring() {
        return iter->Keystring();
    }

    string Valuestring() {
        return iter->Valuestring();
    }

    long Keyinteger() {
        return iter->Keyinteger();
    }

    long Valueinteger() {
        return iter->Valueinteger();
    }

    double Keyfloat() {
        return iter->Keyfloat();
    }

    double Valuefloat() {
        return iter->Valuefloat();
    }

    void Next() {
        return iter->Next();
    }

    Tamgu* End() {
        return iter->End();
    }

    Tamgu* Begin() {
        return iter->Begin();
    }
};

class TamguIteratorReplicate : public TamguIteration {
    public:
    TamguReplicateVector* replicate;
    long itx;
    long mx;
    TamguGlobal* global;

    TamguIteratorReplicate(TamguReplicateVector* k, TamguGlobal* g = NULL) : TamguIteration(false, g) {
        global = g;
        replicate = k;
        itx = 0;
        mx = k->nb->Integer();
    }

    ~TamguIteratorReplicate() {
        replicate->Clear();
    }

    Tamgu* Key() {
        return globalTamgu->ProvideConstint(itx);
    }

    long Keyinteger() {
        return itx;
    }

    BLONG Keylong() {
        return itx;
    }

    double Keyfloat() {
        return itx;
    }

    Tamgu* Value() {
        return replicate->value;
    }

    void Next() {
        itx++;
    }

    Tamgu* End() {
        if (itx >= mx)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* Begin() {
        itx = 0;
        return aTRUE;
    }
};

#endif

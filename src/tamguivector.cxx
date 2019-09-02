/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguivector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamguivector.h"
#include "tamguvector.h"
#include "tamgufvector.h"
#include "tamgumap.h"
#include "constobjects.h"
#include "instructions.h"


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


//We need to declare once again our local definitions.
Exporting basebin_hash<ivectorMethod>  Tamguivector::methods;
Exporting hmap<string, string> Tamguivector::infomethods;
Exporting bin_hash<unsigned long> Tamguivector::exported;

Exporting short Tamguivector::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguivector::AddMethod(TamguGlobal* global, string name, ivectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguivector::Setidtype(TamguGlobal* global) {
        Tamguivector::idtype = global->Getid("ivector");
    }

   bool Tamguivector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamguivector::idtype = global->Getid("ivector");

    Tamguivector::AddMethod(global, "clear", &Tamguivector::MethodClear, P_NONE, "clear(): clear the container.");

    Tamguivector::AddMethod(global, "remove", &Tamguivector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");

    Tamguivector::AddMethod(global, "reverse", &Tamguivector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamguivector::AddMethod(global, "reserve", &Tamguivector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamguivector::AddMethod(global, "unique", &Tamguivector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");

    Tamguivector::AddMethod(global, "last", &Tamguivector::MethodLast, P_NONE, "last(): return the last element.");
    Tamguivector::AddMethod(global, "join", &Tamguivector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamguivector::AddMethod(global, "permute", &Tamguivector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");

    Tamguivector::AddMethod(global, "shuffle", &Tamguivector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamguivector::AddMethod(global, "sort", &Tamguivector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Tamguivector::AddMethod(global, "sum", &Tamguivector::MethodSum, P_NONE, "sum(): return the sum of elements.");

    Tamguivector::AddMethod(global, "product", &Tamguivector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguivector::AddMethod(global, "push", &Tamguivector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamguivector::AddMethod(global, "pop", &Tamguivector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Tamguivector::AddMethod(global, "poplast", &Tamguivector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamguivector::AddMethod(global, "merge", &Tamguivector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamguivector::AddMethod(global, "editdistance", &Tamguivector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamguivector::AddMethod(global, "insert", &Tamguivector::MethodPermute, P_TWO, "insert(i,v): Insert v at position i.");

    global->newInstance[Tamguivector::idtype] = new Tamguivector(global);
    global->RecordMethods(Tamguivector::idtype, Tamguivector::exported);

    Tamgua_ivector::InitialisationModule(global, version);
    
    return true;
}

Exporting TamguIteration* Tamguivector::Newiteration(bool direction) {
    return new TamguIterationivector(this, direction);
}

Exporting Tamgu* Tamguivector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    long val = a->Integer();
     if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);

        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                v->values.push_back(i);
        }
        return v;
    }

   if (context->isNumber()) {
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                return globalTamgu->Provideint(i);
        }
        return aMINUSONE;
    }

    Locking _lock(this);
    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == val)
            return aTRUE;
    }

    return aFALSE;
}


Exporting Tamgu* Tamguivector::getvalue(BLONG i) {
    Locking _lock(this);
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalTamgu->Provideint(values[i]);
}


Exporting Tamgu* Tamguivector::Push(Tamgu* a) {
    locking();
    values.push_back(a->Integer());
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamguivector::Pop(Tamgu* idx) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;

    long v = idx->Integer();
    if (v == -1) {
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (long)values.size())
            return aFALSE;
        values.erase(values.begin() + v);
    }
    return aTRUE;
}

Exporting void Tamguivector::Clear() {
    //To set a variable back to empty
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamguivector::String() {
    size_t i;
    stringstream r;
    r << "[";
    bool first = true;
    locking();
    for (i = 0; i < values.size(); i++) {
        if (first)
            first = false;
        else
            r << ",";
        r << values[i];
    }
    r << "]";
    unlocking();
    return r.str();
}


Exporting long Tamguivector::Integer() {
    locking();
    long v = values.size();
    unlocking();
    return v;
}

Exporting short Tamguivector::Short() {
    locking();
    short v = values.size();
    unlocking();
    return v;
}

Exporting double Tamguivector::Float() {
    locking();
    double v = values.size();
    unlocking();
    return v;
}

Exporting BLONG Tamguivector::Long() {
    locking();
    BLONG v = values.size();
    unlocking();
    return v;
}

Exporting bool Tamguivector::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguivector::Size() {
    locking();
    long v = values.size();
    unlocking();
    return v;
}

Exporting Tamgu* Tamguivector::andset(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] &= itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    long v = b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] &= v;
    return ref;
}

Exporting Tamgu* Tamguivector::orset(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] |= itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    long v = b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] |= v;
    return ref;
}

Exporting Tamgu* Tamguivector::xorset(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;

    if (b->isVectorContainer()) {
        long size = Size();

        Tamguivector* ref = globalTamgu->Provideivector();
        hmap<long, bool> store;
        
        ((TamguObject*)b)->locking();
        for (it = 0; it < b->Size(); it++)
            store[b->getinteger(it)] = true;
        ((TamguObject*)b)->unlocking();

        locking();
        for (it = 0; it < size; it++) {
            if (store.find(values[it]) == store.end())
                store[values[it]] = true;
        }
        unlocking();
        
        for (auto& i : store)
            ref->values.push_back(i.first);

        return ref;
    }

    long v = b->Integer();
    
    if (ref == this)
        ref->locking();
    
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] ^= v;
    
    if (ref == this)
        ref->unlocking();
    
    return ref;
}


Exporting Tamgu* Tamguivector::plus(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;

    if (b->isContainer()) {
        Doublelocking _lock(ref, b);

        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    ref->locking();
    long v = b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] += v;
    ref->unlocking();
    
    return ref;
}

Exporting Tamgu* Tamguivector::minus(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;

    if (b->isContainer()) {
        Doublelocking _lock(ref, b);

        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] -= itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    long v = b->Integer();
    ref->locking();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] -= v;
    ref->unlocking();
    
    return ref;
}

Exporting Tamgu* Tamguivector::multiply(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;

    if (b->isContainer()) {
        Doublelocking _lock(ref, b);

        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] *= itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    long v = b->Integer();
    ref->locking();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] *= v;
    ref->unlocking();
    
    return ref;
}

Exporting Tamgu* Tamguivector::divide(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;
    long v;
    if (b->isContainer()) {
        Doublelocking _lock(ref, b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueinteger();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            ref->values[it] /= v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Integer();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }

    ref->locking();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] /= v;
    ref->unlocking();
    return ref;
}

Exporting Tamgu* Tamguivector::power(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;

    if (b->isContainer()) {
        Doublelocking _lock(ref, b);

        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = pow((double)ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    
    ref->locking();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = pow((double)ref->values[it], v);
    ref->unlocking();
    
    return ref;
}

Exporting Tamgu* Tamguivector::shiftleft(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;

    if (b->isContainer()) {
        Doublelocking _lock(ref, b);

        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] <<= itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    long v = b->Integer();
    ref->locking();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    ref->unlocking();
    return ref;
}

Exporting Tamgu* Tamguivector::shiftright(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;

    if (b->isContainer()) {

        Doublelocking _lock(ref, b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] >>= itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    long v = b->Integer();
    ref->locking();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    ref->unlocking();
    return ref;
}

Exporting Tamgu* Tamguivector::mod(Tamgu* b, bool itself) {
    Tamguivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguivector*)Atom(true);

    size_t it;
    long v;

    if (b->isContainer()) {
        Doublelocking _lock(ref, b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueinteger();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            ref->values[it] %= v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Integer();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }

    ref->locking();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] %= v;
    ref->unlocking();
    
    return ref;
}


Exporting unsigned long Tamguivector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    long* v1 = new long[s1len + 1];
    long* v2 = new long[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getinteger(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned int ed;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            if (v1[y - 1] == v2[x - 1])
                ed = 0;
            else
                ed = 1;
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
            lastdiag = olddiag;
        }
    }


    delete[] v1;
    delete[] v2;

    s2len = column[s1len];
    delete[] column;
    return s2len;

}

Exporting void Tamguivector::Shuffle() {
    Locking _lock(this);

    size_t sz = values.size();
    size_t i, f;
    long mx = sz;
    long v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}

Exporting Tamgu* Tamguivector::Unique() {
    Tamguivector* kvect = globalTamgu->Provideivector();

    Locking _lock(this);
    hmap<long, bool> inter;
    for (int i = 0; i < values.size(); i++) {
        if (inter.find(values[i]) == inter.end()) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    return kvect;
}

Exporting bool Tamguivector::Permute() {
    Locking _lock(this);

    return next_permutation(values.begin(), values.end());
}


Exporting Tamgu* Tamguivector::same(Tamgu* a) {
    if (!a->isVectorContainer())
        return aFALSE;

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            Doublelocking _lock(this, a);
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != a->getinteger(i))
                    return aFALSE;
            }
            return aTRUE;
        }        
        return aFALSE;
    }

    Tamguivector* m = (Tamguivector*)a;
    
    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguivector::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();

    Locking _lock(this);

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, globalTamgu->Provideint(values[it]));
    }
    return kmap;
}

Exporting Tamgu* Tamguivector::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();

    Locking _lock(this);
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalTamgu->Provideint(values[i]));
    return kvect;
}

Exporting Tamgu* Tamguivector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectaivector(contextualpattern);
            if (kv->isProtected()) {
                locking();
                ((Tamguivector*)kv)->values = values;
                unlocking();
                return kv;
            }
            
            Doublelocking _lock(this, kv);
            ((Tamguivector*)kv)->values = values;
            return kv;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }
        
        return this;
    }

    Tamgu* keyright = NULL;
    long ikey;
    TamguIndex* kind = (TamguIndex*)idx;
    ikey = kind->left->Getinteger(idthread);
    if (kind->interval == true)
        keyright = kind->right->Eval(aNULL, aNULL, idthread);

    Locking _lock(this);
    
    if (ikey < 0)
        ikey = values.size() + ikey;

    if (ikey < 0 || ikey >= values.size()) {
        if (ikey != values.size() || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    if (keyright == NULL)
        return globalTamgu->Provideint(values[ikey]);

    long iright = keyright->Integer();
    keyright->Release();
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    //In this case, we must create a new vector
    Tamguivector* kvect = globalTamgu->Provideivector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}

Exporting Tamgu* Tamguivector::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_ivector) {
            Doublelocking _lock(this, ke);
            values = ((Tamguivector*)ke)->values;
            return aTRUE;
        }

        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            if (sz) {
                values.reserve(sz);
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getinteger(i));
            }
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Keyinteger());
                itr->Release();
            }
            return aTRUE;
        }

        if (ke->isContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Valueinteger());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_int(sv, values))
                return globalTamgu->Returnerror("Cannot set this value", idthread);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);


        locking();
        sz = ke->Size();
        values.clear();
        if (sz) {
            values.reserve(sz);
            //We copy all values from ke to this
            for (long it = 0; it < ke->Size(); ++it)
                values.push_back(ke->getinteger(it));
        }
        unlocking();
        ke->Release();
        return aTRUE;
    }
    else {
        //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
            Doublelocking _lock(this, ke);

            //First we remove the values...
            long lkey = idx->Integer();
            if (lkey < 0)
                lkey = values.size() + lkey;

            long rkey = ((TamguIndex*)idx)->right->Integer();
            if (rkey < 0)
                rkey = values.size() + rkey;

            if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
                if (globalTamgu->erroronkey)
                    globalTamgu->Returnerror("Wrong index", idthread);
                return aTRUE;
            }

            if (rkey != lkey)
                rkey--;

            while (rkey >= lkey) {
                values.erase(values.begin() + rkey);
                rkey--;
            }
            if (ke->isVectorContainer()) {
                for (long i = ke->Size() - 1; i >= 0; i--) {
                    values.insert(values.begin() + lkey, ke->getinteger(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valueinteger());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->Integer());
        }
        else {
            locking();
            long ikey = idx->Integer();
            long sz = values.size();
            long nb;
            long n = ke->Integer();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0) {
                    unlocking();
                    return globalTamgu->Returnerror("Cannot set this value", idthread);
                }
            }

            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    values.push_back(0);
                values.push_back(n);
            }
            else
                values[ikey] = n;
            unlocking();
        }
    }
    return aTRUE;
}

static bool compInteger(long s1, long s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icompInteger(long s1, long s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Tamguivector::Sort(bool direction) {
    Locking _lock(this);
    if (direction == false)
        sort(values.begin(), values.end(), compInteger);
    else
        sort(values.begin(), values.end(), icompInteger);
}

Exporting Tamgu* Tamguivector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        locking();
        values.push_back(ke->Integer());
        unlocking();
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking* _dlock = _getlocks(this, ke);
    //Three cases:
    if (ke->isVectorContainer()) {
        Tamguivector* kvect = (Tamguivector*)ke;
        for (long i=0; i< kvect->values.size();i++)
            values.push_back(kvect->values[i]);
        _cleanlock(_dlock);
        return this;
    }

    if (ke->Type() == a_fvector) {
        Tamgufvector* kvect = (Tamgufvector*)ke;
        for (long i=0; i< kvect->values.size();i++)
            values.push_back(kvect->values[i]);
        _cleanlock(_dlock);
        return this;
    }

    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valueinteger());
    itr->Release();

    _cleanlock(_dlock);
    return this;
}

Exporting Tamgu* Tamguivector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    long i;
    
    if (!ke->isContainer()) {
        long v=ke->Integer();
        locking();
        for (i=0; i< values.size();i++)
            values[i]+=v;
        unlocking();
        return this;
    }
    
    
    Doublelocking _lock(this, ke);
    i = 0;
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (i == values.size()) {
            values.push_back(itr->Valueinteger());
            i++;
            continue;
        }
        
        values[i] += itr->Valueinteger();
        i++;
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamguivector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    Tamguint* it = globalTamgu->Provideint();
    it->reference++;

    uchar addvalue = 0;
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    recipient->Putvalue( it, idthread);

    for (size_t i = 0; i < values.size(); i++) {
        it->value = values[i];

        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;

            it->Resetreference();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }

        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }

    recipient->Forcedclean();
    it->Resetreference();
    return context;
}

Exporting Tamgu* Tamguivector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    Tamguint* key = globalTamgu->Provideint();
    key->Setreference();

    if (init != aNOELEMENT) {
        accu->Putvalue(init, idthread);
        if (kcont != NULL) {
            if (direct)
                kcont->Insert(0, init);
            else
                kcont->Push(init);
        }
    }
    else
        first = true; //we are dealing with a foldr1 or a foldl1

    for (size_t i = 0; i < values.size(); i++) {
        key->value = values[i];
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectEval(env, aNULL, idthread);

            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }

            if (globalTamgu->Error(idthread)) {
                key->Resetreference();
                var->Forcedclean();
                if (kcont != NULL)
                    kcont->Release();
                return globalTamgu->Errorobject(idthread);
            }
        }

        if (returnval != aNULL) {
            returnval = returnval->Returned(idthread);
            accu->Putvalue(returnval, idthread);
            if (kcont != NULL) {
                if (direct)
                    kcont->Insert(0, returnval);
                else
                    kcont->Push(returnval);
            }
            returnval->Release();
        }
    }

    key->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

class IComp {
    public:
    TamguCallFunction compare;
    short idthread;
    TamguConstInt p;
    TamguConstInt s;

    IComp(TamguFunction* comp, short idt) : compare(comp), p(0), s(0), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Eval(aNULL, aNULL, idthread)->Boolean();
    }
};

struct IComparison {
    public:
    IComp* compare;

    IComparison(IComp* c)  {
        compare = c;
    }

    bool operator() (long i, long j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Tamgu* Tamguivector::MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalTamgu->Returnerror("Expecting a comparison function with two parameters", idthread);
        IComp kcomp((TamguFunction*)comp->Body(idthread), idthread);
        IComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}



Exporting Tamgu* Tamguivector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    long sz = Size();
    Tamgu* a;
    for (long i = 0; i < sz; i++) {
        locking();
        var->storevalue(values[i]);
        unlocking();
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }

        a->Release();
    }

    return this;
}


    //We need to declare once again our local definitions.
Exporting basebin_hash<a_ivectorMethod>  Tamgua_ivector::methods;
Exporting hmap<string, string> Tamgua_ivector::infomethods;
Exporting bin_hash<unsigned long> Tamgua_ivector::exported;

Exporting short Tamgua_ivector::idtype = 0;


    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_ivector::AddMethod(TamguGlobal* global, string name, a_ivectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_ivector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgua_ivector::idtype = global->Getid("a_ivector");
    
    Tamgua_ivector::AddMethod(global, "clear", &Tamgua_ivector::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_ivector::AddMethod(global, "remove", &Tamgua_ivector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");
    
    Tamgua_ivector::AddMethod(global, "reverse", &Tamgua_ivector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgua_ivector::AddMethod(global, "unique", &Tamgua_ivector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    
    Tamgua_ivector::AddMethod(global, "last", &Tamgua_ivector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgua_ivector::AddMethod(global, "join", &Tamgua_ivector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgua_ivector::AddMethod(global, "sum", &Tamgua_ivector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    
    Tamgua_ivector::AddMethod(global, "product", &Tamgua_ivector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_ivector::AddMethod(global, "push", &Tamgua_ivector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgua_ivector::AddMethod(global, "pop", &Tamgua_ivector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Tamgua_ivector::AddMethod(global, "poplast", &Tamgua_ivector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgua_ivector::AddMethod(global, "merge", &Tamgua_ivector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgua_ivector::AddMethod(global, "editdistance", &Tamgua_ivector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamgua_ivector::AddMethod(global, "insert", &Tamgua_ivector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");
    
    global->newInstance[Tamgua_ivector::idtype] = new Tamgua_ivector(global);
    global->RecordMethods(Tamgua_ivector::idtype, Tamgua_ivector::exported);
    
    return true;
}

Exporting TamguIteration* Tamgua_ivector::Newiteration(bool direction) {
    return new TamguIterationa_ivector(this, direction);
}

Exporting Tamgu* Tamgua_ivector::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    long val = a->Integer();
    if (context->isNumber()) {
        atomic_value_vector_iterator<long> it(values);
        for (; !it.end(); it.next()) {
            if (it.second == val)
                return globalTamgu->Provideint(it.first);
        }
        return aMINUSONE;
    }
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        
        atomic_value_vector_iterator<long> it(values);
        for (; !it.end(); it.next()) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    atomic_value_vector_iterator<long> it(values);
    for (; !it.end(); it.next()) {
        if (it.second == val)
            return aTRUE;
    }
    
    return aFALSE;
}

Exporting Tamgu* Tamgua_ivector::getvalue(BLONG i) {
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalTamgu->Provideint(values[i]);
}


Exporting Tamgu* Tamgua_ivector::Push(Tamgu* a) {
    values.push_back(a->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_ivector::Pop(Tamgu* idx) {
    if (!values.size())
        return aFALSE;
    
    long v = idx->Integer();
    if (v == -1) {
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (long)values.size())
            return aFALSE;
        values.pop(v);
    }
    return aTRUE;
}

Exporting void Tamgua_ivector::Clear() {
        //To set a variable back to empty
    values.clear();
}



Exporting string Tamgua_ivector::String() {
    stringstream r;
    r << "[";
    bool first = true;
    atomic_value_vector_iterator<long> it(values);
    for (; !it.end(); it.next()) {
        if (first)
            first = false;
        else
            r << ",";
        r << it.second;
    }
    r << "]";
    return r.str();
}


Exporting long Tamgua_ivector::Integer() {
    return values.size();
}

Exporting short Tamgua_ivector::Short() {
    return values.size();
}

Exporting double Tamgua_ivector::Float() {
    return values.size();
}

Exporting BLONG Tamgua_ivector::Long() {
    return values.size();
}

Exporting bool Tamgua_ivector::Boolean() {
    if (values.size() == 0)
        return false;
    return true;
}


    //Basic operations
Exporting long Tamgua_ivector::Size() {
    return values.size();
}

Exporting Tamgu* Tamgua_ivector::andset(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second & itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second & v);
    return ref;
}

Exporting Tamgu* Tamgua_ivector::orset(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second | itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second | v);
    return ref;

}

Exporting Tamgu* Tamgua_ivector::xorset(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second ^ itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second ^ v);
    return ref;

}


Exporting Tamgu* Tamgua_ivector::plus(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second + itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second + v);
    return ref;
}

Exporting Tamgu* Tamgua_ivector::minus(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second - itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second - v);
    return ref;
}

Exporting Tamgu* Tamgua_ivector::multiply(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second * itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second * v);
    return ref;
}

Exporting Tamgu* Tamgua_ivector::divide(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    long v;
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueinteger();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            it.replace(it.second / itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    v = b->Integer();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second / v);
    return ref;
}

Exporting Tamgu* Tamgua_ivector::power(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(pow((double)it.second, itr->Valuefloat()));
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(pow((double)it.second, v));

    return ref;
}

Exporting Tamgu* Tamgua_ivector::shiftleft(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second << itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second << v);
    return ref;
}

Exporting Tamgu* Tamgua_ivector::shiftright(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second >> itr->Valueinteger());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    long v = b->Integer();
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second >> v);
    return ref;
}

Exporting Tamgu* Tamgua_ivector::mod(Tamgu* b, bool itself) {
    Tamgua_ivector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_ivector*)Atom(true);
    
    long v;
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<long> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueinteger();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            it.replace(it.second % v);
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    v = b->Integer();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    atomic_value_vector_iterator<long> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second % v);
    return ref;
}


Exporting unsigned long Tamgua_ivector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();
    
    long* v1 = new long[s1len + 1];
    long* v2 = new long[s2len + 1];
    
    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getinteger(x);
    }
    
    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    
    unsigned int ed;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            if (v1[y - 1] == v2[x - 1])
                ed = 0;
            else
                ed = 1;
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
            lastdiag = olddiag;
        }
    }
    
    
    delete[] v1;
    delete[] v2;
    
    s2len = column[s1len];
    delete[] column;
    return s2len;
    
}

Exporting Tamgu* Tamgua_ivector::Unique() {
    Tamgua_ivector* kvect = new Tamgua_ivector();
    
    hmap<long, bool> inter;
    atomic_value_vector_iterator<long> it(values);
    for (;!it.end();it.next()) {
        if (inter.find(it.second) == inter.end()) {
            inter[it.second] = true;
            kvect->values.push_back(it.second);
        }
    }
    return kvect;
}

Exporting Tamgu* Tamgua_ivector::same(Tamgu* a) {
    if (!a->isVectorContainer())
        return aFALSE;
    
    long i = 0;

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            Locking _lock(a);
            if (a->Size() != values.size())
                return aFALSE;
            atomic_value_vector_iterator<long> it(values);
            for (;!it.end();it.next()) {
                if (it.second != a->getinteger(i))
                    return aFALSE;
                i++;
            }
            return aTRUE;
        }
        return aFALSE;
    }
    
    Tamgua_ivector* m = (Tamgua_ivector*)a;
    
    Locking _lock(m);
    if (m->values.size() != values.size())
        return aFALSE;
    
    atomic_value_vector_iterator<long> it(values);
    for (;!it.end();it.next()) {
        if (m->values[i] != it.second)
            return aFALSE;
        i++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_ivector::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();
    
    char buff[100];
    atomic_value_vector_iterator<long> it(values);
    for (;!it.end();it.next()) {
        sprintf_s(buff, 100, "%ld", it.first);
        kmap->Push(buff, globalTamgu->Provideint(it.second));
    }
    return kmap;
}

Exporting Tamgu* Tamgua_ivector::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();
    
    kvect->values.reserve(values.size());
    atomic_value_vector_iterator<long> it(values);
    for (;!it.end();it.next())
        kvect->Push(globalTamgu->Provideint(it.second));
    return kvect;
}

Exporting Tamgu* Tamgua_ivector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        if (contextualpattern->isVectorContainer()) {
            Tamguivector* kv = (Tamguivector*)Selectaivector(contextualpattern);
            Locking _lock(kv);
            atomic_value_vector_iterator<long> it(values);
            for (;!it.end();it.next())
                kv->values.push_back(it.second);
            return kv;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }
        
        return this;
    }
    
    Tamgu* keyright = NULL;
    long ikey;
    TamguIndex* kind = (TamguIndex*)idx;
    ikey = kind->left->Getinteger(idthread);
    if (kind->interval == true)
        keyright = kind->right->Eval(aNULL, aNULL, idthread);
    
    if (ikey < 0)
        ikey = values.size() + ikey;
    
    if (ikey < 0 || ikey >= values.size()) {
        if (ikey != values.size() || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
    if (keyright == NULL)
        return globalTamgu->Provideint(values[ikey]);
    
    long iright = keyright->Integer();
    keyright->Release();
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
        //In this case, we must create a new vector
    Tamgua_ivector* kvect = new Tamgua_ivector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}

Exporting Tamgu* Tamgua_ivector::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgua_ivector::idtype) {
            Locking _lock(ke);
            values = ((Tamgua_ivector*)ke)->values;
            return aTRUE;
        }
        
        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            Locking _lock(ke);
            values.clear();
            if (sz) {
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getinteger(i));
            }
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Locking _lock(ke);
            values.clear();
            if (sz) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Keyinteger());
                itr->Release();
            }
            return aTRUE;
        }
        
        if (ke->isContainer()) {
            Locking _lock(ke);
            values.clear();
            if (sz) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Valueinteger());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            vector<long> vals;
            if (!v_comma_split_int(sv, vals))
                return globalTamgu->Returnerror("Cannot set this value", idthread);
            for (long i = 0; i < vals.size(); i++)
                values.push_back(vals[i]);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);
        
        
        sz = ke->Size();
        values.clear();
        if (sz) {
                //We copy all values from ke to this
            for (long it = 0; it < ke->Size(); ++it)
                values.push_back(ke->getinteger(it));
        }

        ke->Release();
        return aTRUE;
    }
    else {
            //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
            Locking _lock(ke);
            
                //First we remove the values...
            long lkey = idx->Integer();
            if (lkey < 0)
                lkey = values.size() + lkey;
            
            long rkey = ((TamguIndex*)idx)->right->Integer();
            if (rkey < 0)
                rkey = values.size() + rkey;
            
            if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
                if (globalTamgu->erroronkey)
                    globalTamgu->Returnerror("Wrong index", idthread);
                return aTRUE;
            }
            
            if (rkey != lkey)
                rkey--;
            
            while (rkey >= lkey) {
                values.pop(rkey);
                rkey--;
            }
            if (ke->isVectorContainer()) {
                for (long i = ke->Size() - 1; i >= 0; i--) {
                    values.insert(lkey, ke->getinteger(i));
                }
                return aTRUE;
            }
            
            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(lkey, itr->Valueinteger());
                itr->Release();
                return aTRUE;
            }
            
            values.insert(lkey, ke->Integer());
        }
        else {
            long ikey = idx->Integer();
            long sz = values.size();
            long nb;
            long n = ke->Integer();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalTamgu->Returnerror("Cannot set this value", idthread);
            }
            
            if (ikey >= sz) {
                for (nb = values.size(); nb < ikey; nb++)
                    values.push_back(0);
                values.push_back(n);
            }
            else
                values.set(ikey,n);
        }
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_ivector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        values.push_back(ke->Integer());
        return this;
    }
    
    if (ke->Size() == 0)
        return this;
    
    Locking* _dlock = _getlockif(ke);
    //Three cases:
    if (ke->isVectorContainer()) {
        Tamgua_ivector* kvect = (Tamgua_ivector*)ke;
        atomic_value_vector_iterator<long> it(kvect->values);
        for (; !it.end();it.next())
            values.push_back(it.second);
        _cleanlockif(_dlock);
        return this;
    }
    
    if (ke->Type() == a_fvector) {
        Tamgufvector* kvect = (Tamgufvector*)ke;
        for (long i=0; i< kvect->values.size();i++)
            values.push_back(kvect->values[i]);
        _cleanlockif(_dlock);
        return this;
    }
    
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valueinteger());
    itr->Release();
    
    _cleanlockif(_dlock);
    return this;
}

Exporting Tamgu* Tamgua_ivector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    
    if (!ke->isContainer()) {
        long v=ke->Integer();
        atomic_value_vector_iterator<long> it(values);
        for (; !it.end();it.next())
            it.replace(it.second+v);
        return this;
    }
    
    
    Locking _lock(ke);
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    atomic_value_vector_iterator<long> it(values);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (it.end()) {
            values.push_back(itr->Valueinteger());
            continue;
        }
        it.replace(it.second + itr->Valueinteger());
        it.next();
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgua_ivector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    Tamguint* it = globalTamgu->Provideint();
    it->reference++;
    
    uchar addvalue = 0;
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    recipient->Putvalue( it, idthread);
    
    atomic_value_vector_iterator<long> itx(values);
    for (; !itx.end(); itx.next()) {
        it->value = itx.second;
        
        a = bd->DirectEval(environment, aNULL, idthread);
        if (a->isNULL())
            continue;
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            
            it->Resetreference();
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }
    
    recipient->Forcedclean();
    it->Resetreference();
    return context;
}

Exporting Tamgu* Tamgua_ivector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    Tamguint* key = globalTamgu->Provideint();
    key->Setreference();
    
    if (init != aNOELEMENT) {
        accu->Putvalue(init, idthread);
        if (kcont != NULL) {
            if (direct)
                kcont->Insert(0, init);
            else
                kcont->Push(init);
        }
    }
    else
        first = true; //we are dealing with a foldr1 or a foldl1
    
    atomic_value_vector_iterator<long> itx(values);
    for (; !itx.end(); itx.next()) {
        key->value = itx.second;
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectEval(env, aNULL, idthread);
            
            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }
            
            if (globalTamgu->Error(idthread)) {
                key->Resetreference();
                var->Forcedclean();
                if (kcont != NULL)
                    kcont->Release();
                return globalTamgu->Errorobject(idthread);
            }
        }
        
        if (returnval != aNULL) {
            returnval = returnval->Returned(idthread);
            accu->Putvalue(returnval, idthread);
            if (kcont != NULL) {
                if (direct)
                    kcont->Insert(0, returnval);
                else
                    kcont->Push(returnval);
            }
            returnval->Release();
        }
    }
    
    key->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}


Exporting Tamgu* Tamgua_ivector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    Tamgu* a;
    atomic_value_vector_iterator<long> itx(values);
    for (; !itx.end(); itx.next()) {
        var->storevalue(itx.second);
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
            //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }
        
        a->Release();
    }
    
    return this;
}

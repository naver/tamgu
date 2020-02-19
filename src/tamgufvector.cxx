/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgufvector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamgufvector.h"
#include "tamguivector.h"
#include "tamguconstants.h"
#include "tamgumap.h"
#include "tamguvector.h"
#include "constobjects.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<fvectorMethod>  Tamgufvector::methods;
Exporting hmap<string, string> Tamgufvector::infomethods;
Exporting bin_hash<unsigned long> Tamgufvector::exported;

Exporting short Tamgufvector::idtype = 0;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
        #endif

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgufvector::AddMethod(TamguGlobal* global, string name, fvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgufvector::Setidtype(TamguGlobal* global) {
        Tamgufvector::idtype = global->Getid("fvector");
    }

   bool Tamgufvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();



    Tamgufvector::idtype = global->Getid("fvector");

    Tamgufvector::AddMethod(global, "reverse", &Tamgufvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgufvector::AddMethod(global, "reserve", &Tamgufvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamgufvector::AddMethod(global, "unique", &Tamgufvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");

    Tamgufvector::AddMethod(global, "remove", &Tamgufvector::MethodRemove, P_ONE, "remove(float e): remove 'e' from the vector.");

    Tamgufvector::AddMethod(global, "last", &Tamgufvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgufvector::AddMethod(global, "join", &Tamgufvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");

    Tamgufvector::AddMethod(global, "shuffle", &Tamgufvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamgufvector::AddMethod(global, "sort", &Tamgufvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Tamgufvector::AddMethod(global, "sum", &Tamgufvector::MethodSum, P_NONE, "sum(): return the sum of elements.");

    Tamgufvector::AddMethod(global, "product", &Tamgufvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgufvector::AddMethod(global, "push", &Tamgufvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgufvector::AddMethod(global, "pop", &Tamgufvector::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamgufvector::AddMethod(global, "merge", &Tamgufvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgufvector::AddMethod(global, "editdistance", &Tamgufvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamgufvector::AddMethod(global, "insert", &Tamgufvector::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");

    Tamgufvector::AddMethod(global, "permute", &Tamgufvector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");

    global->newInstance[Tamgufvector::idtype] = new Tamgufvector(global);
    global->RecordMethods(Tamgufvector::idtype,Tamgufvector::exported);

    Tamgua_fvector::InitialisationModule(global, version);
    
    return true;
}

Exporting TamguIteration* Tamgufvector::Newiteration(bool direction) {
    return new TamguIterationfvector(this, direction);
}


Exporting Tamgu* Tamgufvector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    double val = a->Float();
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

Exporting Tamgu* Tamgufvector::Push(Tamgu* a) {
    Locking _lock(this);
    values.push_back(a->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgufvector::Pop(Tamgu* idx) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;
        
    BLONG v = idx->Integer();
    if (v == -1) {
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size())
            return aFALSE;
        values.erase(values.begin() + v);
    }
    return aTRUE;
}

Exporting void Tamgufvector::Clear() {
    //To set a variable back to empty
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgufvector::String() {
    Locking _lock(this);
    size_t i;
    stringstream r;
    r<<"[";
    bool first=true;
    for (i=0;i<values.size();i++) {
        if (first)
            first=false;
        else
            r<<",";
        r<<values[i];
    }
    r<<"]";
    return r.str();
}


Exporting long Tamgufvector::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgufvector::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgufvector::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgufvector::Boolean() {
    Locking _lock(this);
    if (values.size()==0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgufvector::Size() {
    Locking _lock(this);
    return values.size();
}

static bool compFloat(double s1, double s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icompFloat(double s1, double s2) {
    if (s1>s2)
        return true;
    return false;
}
Exporting void Tamgufvector::Sort(bool direction) {
    Locking _lock(this);
    if (direction == false)
        sort(values.begin(), values.end(), compFloat);
    else
        sort(values.begin(), values.end(), icompFloat);
}

Exporting Tamgu* Tamgufvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        Locking _lock(this);
        values.push_back(ke->Float());
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Tamgufvector* kvect = (Tamgufvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    if (ke->Type() == a_ivector) {
        Tamguivector* kvect = (Tamguivector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valuefloat());
    itr->Release();

    return this;
}

Exporting Tamgu* Tamgufvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    long i;
    
    if (!ke->isContainer()) {
        double v=ke->Float();
        Locking _lock(this);
        for (i=0; i< values.size();i++)
            values[i]+=v;
        return this;
    }
    
    
    Doublelocking _lock(this, ke);
    i = 0;
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (i == values.size()) {
            values.push_back(itr->Valuefloat());
            i++;
            continue;
        }
        
        values[i] += itr->Valuefloat();
        i++;
    }
    itr->Release();
    return this;
}

Exporting unsigned long Tamgufvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    double* v1 = new double[s1len + 1];
    double* v2 = new double[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getfloat(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned long ed;
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

Exporting Tamgu* Tamgufvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    if (ke == this)
        return aTRUE;

    if (!idx->isIndex()) {
        Doublelocking _lock(this, ke);

        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_fvector) {
            //We copy all values from ke to this
            values = ((Tamgufvector*)ke)->values;
            return aTRUE;
        }

        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getfloat(i));
            }
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Keyfloat());
                itr->Release();
            }
            return aTRUE;
        }

        if (ke->isContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Valuefloat());
                itr->Release();
            }
            return aTRUE;
        }
        
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_float(sv, values))
                return globalTamgu->Returnerror("Cannot set this value", idthread);
            return aTRUE;
        }
        
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);

        sz = ke->Size();
        
        values.clear();
        if (sz) {
            values.reserve(sz);
            //We copy all values from ke to this
            for (long it = 0; it < ke->Size(); ++it)
                values.push_back(ke->getfloat(it));
        }
        ke->Release();
        return aTRUE;
    }
    else {
        //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
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
                for (int i = ke->Size() - 1; i >= 0; i--) {
                    values.insert(values.begin() + lkey, ke->getfloat(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valuefloat());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->Float());
        }
        else {
            long ikey = idx->Integer();
            long sz = values.size();
            long nb;
            double n = ke->Float();
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
                values[ikey] = n;
        }
    }
    return aTRUE;
}

Exporting bool Tamgufvector::Permute() {
    Locking _lock(this);

    return next_permutation(values.begin(), values.end());
}

Exporting void Tamgufvector::Shuffle() {
    Locking _lock(this);

    size_t sz = values.size();
    size_t i, f;
    long mx = sz;
    double v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}

Exporting Tamgu* Tamgufvector::Unique() {
    Locking _lock(this);
    Tamgufvector* kvect = globalTamgu->Providefvector();
    hmap<double, bool> inter;
    for (int i = 0; i < values.size(); i++) {
        if (inter.find(values[i]) == inter.end()) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    return kvect;
}


Exporting Tamgu* Tamgufvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    Locking _lock(this);

    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectafvector(contextualpattern);
            if (kv->Type() == a_fvector)
                ((Tamgufvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(globalTamgu->Providefloat(values[i]));
            return kv;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }

        return this;
    }

    long ikey;
    Tamgu* keyright = NULL;
    TamguIndex* kind = (TamguIndex*)idx;
    ikey = kind->left->Getinteger(idthread);
    if (kind->interval == true)
        keyright = kind->right->Eval(aNULL, aNULL, idthread);


    Tamgufvector* kvect;
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
        return globalTamgu->Providefloat(values[ikey]);

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
    kvect = globalTamgu->Providefvector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}

Exporting Tamgu* Tamgufvector::Map(short idthread) {
    Locking _lock(this);
    Tamgumap* kmap = globalTamgu->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, globalTamgu->Providefloat(values[it]));
    }
    return kmap;
}

Exporting Tamgu* Tamgufvector::Vector(short idthread) {
    Locking _lock(this);
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalTamgu->Providefloat(values[i]));
    return kvect;
}



Exporting Tamgu* Tamgufvector::same(Tamgu* a) {
    if (!a->isVectorContainer())
        return aFALSE;


    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != a->getfloat(i))
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Tamgufvector* m = (Tamgufvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}


Exporting Tamgu* Tamgufvector::andset(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = DoubleAnd(ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = DoubleAnd(ref->values[it], v);
    return ref;
}

Exporting Tamgu* Tamgufvector::orset(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);
    
    Doublelocking _lock(ref, b);
    size_t it;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = DoubleOr(ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = DoubleOr(ref->values[it], v);
    return ref;
}

Exporting Tamgu* Tamgufvector::xorset(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);
    
    Doublelocking _lock(ref, b);
    size_t it;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = DoubleXor(ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = DoubleXor(ref->values[it], v);
    return ref;
}

Exporting Tamgu* Tamgufvector::plus(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valuefloat();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] += v;
    return ref;
}

Exporting Tamgu* Tamgufvector::minus(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] -= itr->Valuefloat();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] -= v;
    return ref;
}

Exporting Tamgu* Tamgufvector::multiply(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] *= itr->Valuefloat();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] *= v;
    return ref;
}

Exporting Tamgu* Tamgufvector::divide(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    double v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valuefloat();
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

    v = b->Float();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] /= v;
    return ref;
}

Exporting Tamgu* Tamgufvector::power(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = pow(ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = pow(ref->values[it], v);

    return ref;
}

Exporting Tamgu* Tamgufvector::shiftleft(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);
    
    Doublelocking _lock(ref, b);
    size_t it;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = DoubleShiftleft(ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = DoubleShiftleft(ref->values[it], v);
    return ref;
}

Exporting Tamgu* Tamgufvector::shiftright(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);
    
    Doublelocking _lock(ref, b);
    size_t it;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = DoubleShiftright(ref->values[it], itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = DoubleShiftright(ref->values[it], v);
    return ref;

}

Exporting Tamgu* Tamgufvector::mod(Tamgu* b, bool itself) {
    Tamgufvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgufvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    long v;
    long vbis;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valuefloat();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            vbis = ref->values[it];
            vbis %= v;
            ref->values[it] = vbis;
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

    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis %= v;
        ref->values[it] = vbis;

    }
    return ref;
}

Exporting Tamgu* Tamgufvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    Tamgufloat* it = globalTamgu->Providefloat();
    it->Setreference();
    
    uchar addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    recipient->Putvalue(it, idthread);

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

    it->Resetreference();
    recipient->Forcedclean();
    return context;
}

Exporting Tamgu* Tamgufvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    
    Tamgufloat* key = globalTamgu->Providefloat();
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

class FComp {
    public:
    TamguCallFunction compare;
    short idthread;
    TamguConstFloat p;
    TamguConstFloat s;

    FComp(TamguFunction* comp, short idt) : compare(comp), p(0), s(0), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Eval(aNULL, aNULL, idthread)->Boolean();
    }
};

class FComparison {
    public:
    FComp* compare;

    FComparison(FComp* c)  {
        compare = c;
    }

    bool operator() (double i, double j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Tamgu* Tamgufvector::MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalTamgu->Returnerror("Expecting a comparison function with two parameters", idthread);

        FComp kcomp((TamguFunction*)comp->Body(idthread), idthread);
        FComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Tamgu* Tamgufvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    long sz = Size();
    Tamgu* a = aNULL;
    bool testcond = false;
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        var->storevalue(values[i]);
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;
}

//-------------------------------------------------------------------------------------------

//We need to declare once again our local definitions.
Exporting basebin_hash<a_fvectorMethod>  Tamgua_fvector::methods;
Exporting hmap<string, string> Tamgua_fvector::infomethods;
Exporting bin_hash<unsigned long> Tamgua_fvector::exported;

Exporting short Tamgua_fvector::idtype = 0;


    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_fvector::AddMethod(TamguGlobal* global, string name, a_fvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_fvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgua_fvector::idtype = global->Getid("a_fvector");
    
    Tamgua_fvector::AddMethod(global, "clear", &Tamgua_fvector::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_fvector::AddMethod(global, "remove", &Tamgua_fvector::MethodRemove, P_ONE, "remove(float e): remove 'e' from the vector.");
    
    Tamgua_fvector::AddMethod(global, "reverse", &Tamgua_fvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgua_fvector::AddMethod(global, "unique", &Tamgua_fvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    
    Tamgua_fvector::AddMethod(global, "last", &Tamgua_fvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgua_fvector::AddMethod(global, "join", &Tamgua_fvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgua_fvector::AddMethod(global, "sum", &Tamgua_fvector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    
    Tamgua_fvector::AddMethod(global, "product", &Tamgua_fvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_fvector::AddMethod(global, "push", &Tamgua_fvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgua_fvector::AddMethod(global, "pop", &Tamgua_fvector::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamgua_fvector::AddMethod(global, "poplast", &Tamgua_fvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgua_fvector::AddMethod(global, "merge", &Tamgua_fvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgua_fvector::AddMethod(global, "editdistance", &Tamgua_fvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamgua_fvector::AddMethod(global, "insert", &Tamgua_fvector::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");
    
    global->newInstance[Tamgua_fvector::idtype] = new Tamgua_fvector(global);
    global->RecordMethods(Tamgua_fvector::idtype, Tamgua_fvector::exported);
    
    return true;
}

Exporting TamguIteration* Tamgua_fvector::Newiteration(bool direction) {
    return new TamguIterationa_fvector(this, direction);
}

Exporting Tamgu* Tamgua_fvector::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    double val = a->Float();
    if (context->isNumber()) {
        atomic_value_vector_iterator<double> it(values);
        for (; !it.end(); it.next()) {
            if (it.second == val)
                return globalTamgu->Provideint(it.first);
        }
        return aMINUSONE;
    }
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        
        atomic_value_vector_iterator<double> it(values);
        for (; !it.end(); it.next()) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    atomic_value_vector_iterator<double> it(values);
    for (; !it.end(); it.next()) {
        if (it.second == val)
            return aTRUE;
    }
    
    return aFALSE;
}


Exporting Tamgu* Tamgua_fvector::getvalue(BLONG i) {
    if (i < 0 || i >= values.size())
        return aNOELEMENT;
    return globalTamgu->Providefloat(values[i]);
}


Exporting Tamgu* Tamgua_fvector::Push(Tamgu* a) {
    values.push_back(a->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgua_fvector::Pop(Tamgu* idx) {
    if (!values.size())
        return aFALSE;
    
    long v = idx->Integer();
    if (v == -1)
        values.pop_back();
    else {
        if (v < 0 || v >= (long)values.size())
            return aFALSE;
        values.pop(v);
    }
    return aTRUE;
}

Exporting void Tamgua_fvector::Clear() {
        //To set a variable back to empty
    values.clear();
}



Exporting string Tamgua_fvector::String() {
    stringstream r;
    r << "[";
    bool first = true;
    atomic_value_vector_iterator<double> it(values);
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


Exporting long Tamgua_fvector::Integer() {
    return values.size();
}

Exporting short Tamgua_fvector::Short() {
    return values.size();
}

Exporting double Tamgua_fvector::Float() {
    return values.size();
}

Exporting BLONG Tamgua_fvector::Long() {
    return values.size();
}

Exporting bool Tamgua_fvector::Boolean() {
    if (values.size() == 0)
        return false;
    return true;
}


    //Basic operations
Exporting long Tamgua_fvector::Size() {
    return values.size();
}

Exporting Tamgu* Tamgua_fvector::andset(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(DoubleAnd(it.second,itr->Valuefloat()));
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(DoubleAnd(it.second,v));
    return ref;
}

Exporting Tamgu* Tamgua_fvector::orset(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(DoubleOr(it.second,itr->Valuefloat()));
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(DoubleOr(it.second,v));
    return ref;
    
}

Exporting Tamgu* Tamgua_fvector::xorset(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(DoubleXor(it.second,itr->Valuefloat()));
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(DoubleXor(it.second,v));
    return ref;
    
}


Exporting Tamgu* Tamgua_fvector::plus(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second + itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second + v);
    return ref;
}

Exporting Tamgu* Tamgua_fvector::minus(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second - itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second - v);
    return ref;
}

Exporting Tamgu* Tamgua_fvector::multiply(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(it.second * itr->Valuefloat());
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second * v);
    return ref;
}

Exporting Tamgu* Tamgua_fvector::divide(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    long v;
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valuefloat();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            it.replace(it.second / itr->Valuefloat());
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
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(it.second / v);
    return ref;
}

Exporting Tamgu* Tamgua_fvector::power(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
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
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(pow((double)it.second, v));
    
    return ref;
}

Exporting Tamgu* Tamgua_fvector::shiftleft(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(DoubleShiftleft(it.second,itr->Valuefloat()));
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(DoubleShiftleft(it.second,v));
    return ref;
}

Exporting Tamgu* Tamgua_fvector::shiftright(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            it.replace(DoubleShiftright(it.second,itr->Valuefloat()));
            itr->Next();
        }
        itr->Release();
        return ref;
    }
    
    double v = b->Float();
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace(DoubleShiftright(it.second,v));
    return ref;
}

Exporting Tamgu* Tamgua_fvector::mod(Tamgu* b, bool itself) {
    Tamgua_fvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_fvector*)Atom(true);
    
    long v;
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_value_vector_iterator<double> it(ref->values);
        for (; !it.end(); it.next()) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueinteger();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            it.replace((BLONG)it.second % v);
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
    atomic_value_vector_iterator<double> it(ref->values);
    for (; !it.end(); it.next())
        it.replace((long)it.second % v);
    return ref;
}


Exporting unsigned long Tamgua_fvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();
    
    double* v1 = new double[s1len + 1];
    double* v2 = new double[s2len + 1];
    
    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getfloat(x);
    }
    
    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    
    unsigned long ed;
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

Exporting Tamgu* Tamgua_fvector::Unique() {
    Tamgua_fvector* kvect = new Tamgua_fvector();
    
    hmap<double, bool> inter;
    atomic_value_vector_iterator<double> it(values);
    for (;!it.end();it.next()) {
        if (inter.find(it.second) == inter.end()) {
            inter[it.second] = true;
            kvect->values.push_back(it.second);
        }
    }
    return kvect;
}

Exporting Tamgu* Tamgua_fvector::same(Tamgu* a) {
    if (!a->isVectorContainer())
        return aFALSE;
    
    long i = 0;
    
    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            Locking _lock(a);
            if (a->Size() != values.size())
                return aFALSE;
            atomic_value_vector_iterator<double> it(values);
            for (;!it.end();it.next()) {
                if (it.second != a->getfloat(i))
                    return aFALSE;
                i++;
            }
            return aTRUE;
        }
        return aFALSE;
    }
    
    Tamgua_fvector* m = (Tamgua_fvector*)a;
    
    Locking _lock(m);
    if (m->values.size() != values.size())
        return aFALSE;
    
    atomic_value_vector_iterator<double> it(values);
    for (;!it.end();it.next()) {
        if (m->values[i] != it.second)
            return aFALSE;
        i++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_fvector::Map(short idthread) {
    Tamgumap* kmap = globalTamgu->Providemap();
    
    char buff[100];
    atomic_value_vector_iterator<double> it(values);
    for (;!it.end();it.next()) {
        sprintf_s(buff, 100, "%ld", it.first);
        kmap->Push(buff, globalTamgu->Providefloat(it.second));
    }
    return kmap;
}

Exporting Tamgu* Tamgua_fvector::Vector(short idthread) {
    Tamguvector* kvect = globalTamgu->Providevector();
    
    kvect->values.reserve(values.size());
    atomic_value_vector_iterator<double> it(values);
    for (;!it.end();it.next())
        kvect->Push(globalTamgu->Providefloat(it.second));
    return kvect;
}

Exporting Tamgu* Tamgua_fvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgufvector* kv = (Tamgufvector*)Selectafvector(contextualpattern);
            Locking _lock(kv);
            atomic_value_vector_iterator<double> it(values);
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
        return globalTamgu->Providefloat(values[ikey]);
    
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
    Tamgua_fvector* kvect = new Tamgua_fvector();
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    return kvect;
}

Exporting Tamgu* Tamgua_fvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgua_fvector::idtype) {
            Locking _lock(ke);
            values = ((Tamgua_fvector*)ke)->values;
            return aTRUE;
        }
        
        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            Locking _lock(ke);
            values.clear();
            if (sz) {
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getfloat(i));
            }
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Locking _lock(ke);
            values.clear();
            if (sz) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Keyfloat());
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
                    values.push_back(itr->Valuefloat());
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
                values.push_back(ke->getfloat(it));
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
                    values.insert(lkey, ke->getfloat(i));
                }
                return aTRUE;
            }
            
            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(lkey, itr->Valuefloat());
                itr->Release();
                return aTRUE;
            }
            
            values.insert(lkey, ke->Integer());
        }
        else {
            long ikey = idx->Integer();
            long sz = values.size();
            long nb;
            double n = ke->Float();
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

Exporting Tamgu* Tamgua_fvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        values.push_back(ke->Float());
        return this;
    }
    
    if (ke->Size() == 0)
        return this;
    
    Locking* _dlock = _getlockif(ke);
        //Three cases:
    if (ke->isVectorContainer()) {
        Tamgua_fvector* kvect = (Tamgua_fvector*)ke;
        atomic_value_vector_iterator<double> it(kvect->values);
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
        values.push_back(itr->Valuefloat());
    itr->Release();
    
    _cleanlockif(_dlock);
    return this;
}

Exporting Tamgu* Tamgua_fvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    
    if (!ke->isContainer()) {
        long v=ke->Integer();
        atomic_value_vector_iterator<double> it(values);
        for (; !it.end();it.next())
            it.replace(it.second+v);
        return this;
    }
    
    
    Locking _lock(ke);
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    atomic_value_vector_iterator<double> it(values);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (it.end()) {
            values.push_back(itr->Valuefloat());
            continue;
        }
        it.replace(it.second + itr->Valuefloat());
        it.next();
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgua_fvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    Tamgufloat* it = globalTamgu->Providefloat();
    it->reference++;
    
    uchar addvalue = 0;
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    recipient->Putvalue( it, idthread);
    
    atomic_value_vector_iterator<double> itx(values);
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

Exporting Tamgu* Tamgua_fvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    Tamgufloat* key = globalTamgu->Providefloat();
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
    
    atomic_value_vector_iterator<double> itx(values);
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


Exporting Tamgu* Tamgua_fvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    Tamgu* a = aNULL;
    bool testcond = false;
    atomic_value_vector_iterator<double> itx(values);
    for (; !itx.end() && !testcond; itx.next()) {
        a->Releasenonconst();
        var->storevalue(itx.second);
        
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);
        
            //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }
    
    a->Releasenonconst();
    return this;
}

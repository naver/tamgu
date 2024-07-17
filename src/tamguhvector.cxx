/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguhvector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamguhvector.h"
#include "tamgufvector.h"
#include "tamguivector.h"
#include "tamgushort.h"
#include "tamguconstants.h"
#include "tamgumap.h"
#include "tamguvector.h"
#include "constobjects.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<hvectorMethod>  Tamguhvector::methods;
static ThreadLock classlock;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguhvector::AddMethod(TamguGlobal* global, string name, hvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(a_hvector) != global->infomethods.end() &&
            global->infomethods[a_hvector].find(name) != global->infomethods[a_hvector].end())
    return;

    global->infomethods[a_hvector][name] = infos;
    global->RecordArity(a_hvector, idname, arity);
}




void Tamguhvector::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamguhvector::methods.isEmpty())
    Tamguhvector::InitialisationModule(global,"");
}


bool Tamguhvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    Tamguhvector::AddMethod(global, "min", &Tamguhvector::MethodMin, P_NONE, "min(): returns the min in the vector.");
    Tamguhvector::AddMethod(global, "max", &Tamguhvector::MethodMax, P_NONE, "max(): returns the max in the vector.");
    Tamguhvector::AddMethod(global, "minmax", &Tamguhvector::MethodMinMax, P_NONE, "minmax(): returns the min and the max in the vector.");

    Tamguhvector::AddMethod(global, "reverse", &Tamguhvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamguhvector::AddMethod(global, "reserve", &Tamguhvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamguhvector::AddMethod(global, "unique", &Tamguhvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    
    Tamguhvector::AddMethod(global, "remove", &Tamguhvector::MethodRemove, P_ONE, "remove(short e): remove 'e' from the vector.");
    
    Tamguhvector::AddMethod(global, "last", &Tamguhvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamguhvector::AddMethod(global, "join", &Tamguhvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    
    Tamguhvector::AddMethod(global, "shuffle", &Tamguhvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamguhvector::AddMethod(global, "sort", &Tamguhvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Tamguhvector::AddMethod(global, "shape", &Tamguhvector::MethodShape, P_NONE|P_ATLEASTONE, "shape(int s1, int s2...): defines the vector shape.");
    Tamguhvector::AddMethod(global, "sum", &Tamguhvector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    
    Tamguhvector::AddMethod(global, "product", &Tamguhvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguhvector::AddMethod(global, "push", &Tamguhvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamguhvector::AddMethod(global, "pop", &Tamguhvector::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamguhvector::AddMethod(global, "poplast", &Tamguhvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamguhvector::AddMethod(global, "merge", &Tamguhvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamguhvector::AddMethod(global, "editdistance", &Tamguhvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamguhvector::AddMethod(global, "insert", &Tamguhvector::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");
    
    Tamguhvector::AddMethod(global, "permute", &Tamguhvector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");

    if (version != "") {
        global->returnindextypes[a_hvector] = a_short;
        global->minimal_indexes[a_hvector] = true;
        global->newInstance[a_hvector] = new Tamguhvector(global);
        global->RecordCompatibilities(a_hvector);
    }
    
    return true;
}

Exporting TamguIteration* Tamguhvector::Newiteration(bool direction) {
    return new TamguIterationhvector(this, direction);
}


Exporting Tamgu* Tamguhvector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    short val = a->Short();
     if (context->isVectorContainer()) {
        Tamguhvector* v = (Tamguhvector*)Selectahvector(context);
        Doublelocking _lock(this, v);

        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                v->values.push_back(i);
        }
        return v;
    }

    locking();
    if (context->isNumber()) {
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val) {
                unlocking();
                return globalTamgu->ProvideConstint(i);
            }
        }
        unlocking();
        return aMINUSONE;
    }
    
    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == val) {
            unlocking();
            return aTRUE;
        }
    }
    unlocking();
    return aFALSE;
}

Exporting Tamgu* Tamguhvector::getvalue(long i) {
    locking();
    if (i < 0 || i >= values.size()) {
        unlocking();
        return aNOELEMENT;
    }
    Tamgu* r = new Tamgushort(values[i]);
    unlocking();
    return r;
}

Exporting Tamgu* Tamguhvector::Push(Tamgu* a) {
    locking();
    values.push_back(a->Short());
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamguhvector::Pop(Tamgu* idx) {
    locking();
    if (!values.size()) {
        unlocking();
        return aFALSE;
    }
        
    long v = idx->Integer();
    if (v == -1) {
        v = (long)values.size() - 1;
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size()) {
            unlocking();
            return aFALSE;
        }
        values.erase(values.begin() + v);
    }
    unlocking();
    return aTRUE;
}

Exporting void Tamguhvector::Clear() {
    //To set a variable back to empty
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamguhvector::String() {
    locking();
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
    unlocking();
    r<<"]";
    return r.str();
}


Exporting long Tamguhvector::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting short Tamguhvector::Short() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting float Tamguhvector::Decimal() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamguhvector::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamguhvector::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamguhvector::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamguhvector::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

static bool compshort(short s1, short s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icompshort(short s1, short s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Tamguhvector::Sort(bool direction) {
    locking();
    if (direction == false)
        sort(values.begin(), values.end(), compshort);
    else
        sort(values.begin(), values.end(), icompshort);
    unlocking();
}

Exporting Tamgu* Tamguhvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        locking();
        values.push_back(ke->Short());
        unlocking();
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == a_hvector) {
        Tamguhvector* kvect = (Tamguhvector*)ke;
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
        values.push_back(itr->Valueshort());
    itr->Release();

    return this;
}

Exporting Tamgu* Tamguhvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    long i;
    
    if (!ke->isContainer()) {
        short v=ke->Short();
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
            values.push_back(itr->Valueshort());
            i++;
            continue;
        }
        
        values[i] += itr->Valueshort();
        i++;
    }
    itr->Release();
    return this;
}

Exporting unsigned long Tamguhvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    short* v1 = new short[s1len + 1];
    short* v2 = new short[s2len + 1];

    y =  maxlocal(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getshort(x);
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

Exporting Tamgu* Tamguhvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    if (ke == this)
        return aTRUE;

    if (!idx->isIndex()) {
        Doublelocking _lock(this, ke);

        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_dvector) {
            values = ((Tamguhvector*)ke)->values;
            return aTRUE;
        }

        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getshort(i));
            }
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.push_back(itr->Keyshort());
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
                    values.push_back(itr->Valueshort());
                itr->Release();
            }
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_short(sv, values))
                return globalTamgu->Returnerror(e_cannot_set_this, idthread);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror(e_cannot_set_this, idthread);

        values.clear();
        sz = ke->Size();
        if (sz) {
            values.reserve(sz);
            //We copy all values from ke to this
            for (long it = 0; it < ke->Size(); ++it)
                values.push_back(ke->getshort(it));
        }
        ke->Release();
        return aTRUE;
    }
    else {
        //In this specific case, we try to replace a bloc of values with a new bloc
        if (idx->isInterval()) {
            //First we remove the values...
            long lkey = idx->Getinteger(idthread);
            if (lkey < 0)
                lkey = values.size() + lkey;

            long rkey = ((TamguIndex*)idx)->right->Getinteger(idthread);
            if (rkey < 0)
                rkey = values.size() + rkey;

            if (rkey < lkey || rkey >= values.size() || lkey >= values.size()) {
                if (globalTamgu->erroronkey)
                    globalTamgu->Returnerror(e_wrong_index, idthread);
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
                    values.insert(values.begin() + lkey, ke->getshort(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valueshort());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->Short());
        }
        else {
            long ikey = idx->Getinteger(idthread);
            long sz = values.size();
            long nb;
            short n = ke->Short();
            if (ikey < 0) {
                ikey = sz + ikey;
                if (ikey < 0)
                    return globalTamgu->Returnerror(e_cannot_set_this, idthread);
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

Exporting bool Tamguhvector::Permute() {
    locking();

    bool v = next_permutation(values.begin(), values.end());
    unlocking();
    return v;
}

Exporting void Tamguhvector::Shuffle() {
    locking();

    size_t sz = values.size();
    size_t i, f;
    long mx = sz;
    short v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
    unlocking();
}

Exporting Tamgu* Tamguhvector::Unique() {
    Tamguhvector* kvect = new Tamguhvector;
    std::set<short> inter;
    locking();
    for (int i = 0; i < values.size(); i++) {
        if (inter.insert(values[i]).second)
            kvect->values.push_back(values[i]);
    }
    unlocking();
    return kvect;
}

Tamgu* Tamguhvector::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    long ikey;
    ikey = key->Getinteger(idthread);
    if (ikey < 0)
        ikey = values.size() + ikey;
    
    locking();
    if (ikey < 0 || ikey >= values.size()) {
        unlocking();
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;
    }

    key =  new Tamgushort(values[ikey]);
    unlocking();
    return key;
}

Exporting Tamgu* Tamguhvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    if (!idx->isIndex()) {
        Tamgu* kv = NULL;
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            kv = Selectadvector(contextualpattern);
            locking();
            if (kv->Type() == a_dvector)
                ((Tamguhvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(new Tamgushort(values[i]));
            unlocking();
            return kv;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_short)
            return globalTamgu->ProvideConstint(values.size());
        return this;
    }

    long ikey;
    Tamgu* keyright = NULL;

    ikey = idx->Getinteger(idthread);
    if (idx->isInterval())
        keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);


    Tamguhvector* kvect;
    if (ikey < 0)
        ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		if (globalTamgu->erroronkey)
			return globalTamgu->Returnerror(e_wrong_index, idthread);
		return aNOELEMENT;
	}

    if (keyright == NULL)
        return new Tamgushort(values[ikey]);

    long iright = keyright->Integer();
    
    if (keyright != ((TamguIndex*)idx)->right)
        keyright->Release();

    locking();
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror(e_wrong_index, idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror(e_wrong_index, idthread);
            return aNOELEMENT;
        }
    }
    //In this case, we must create a new vector
    kvect = new Tamguhvector;
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    
    unlocking();
    return kvect;
}

Exporting Tamgu* Tamguhvector::Map(short idthread) {
    locking();
    Tamgumap* kmap = globalTamgu->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, new Tamgushort(values[it]));
    }
    unlocking();
    return kmap;
}

Exporting Tamgu* Tamguhvector::Vector(short idthread) {
    locking();
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(new Tamgushort(values[i]));
    unlocking();
    return kvect;
}



Exporting Tamgu* Tamguhvector::same(Tamgu* a) {
    if (!a->isVectorContainer())
        return aFALSE;


    Doublelocking _lock(this, a);

    if (a->Type() != a_hvector) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != a->getshort(i))
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Tamguhvector* m = (Tamguhvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}


Exporting Tamgu* Tamguhvector::andset(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    short v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v &= itr->Valueshort();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Short();
    short vbis;
    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis &= v;
        ref->values[it] = vbis;
    }
    return ref;
}

Exporting Tamgu* Tamguhvector::orset(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    short v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v |= itr->Valueshort();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Short();
    short vbis;
    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis |= v;
        ref->values[it] = vbis;
    }
    return ref;
}

Exporting Tamgu* Tamguhvector::xorset(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    short v;
    if (b->isVectorContainer()) {
        long size = Size();

        Tamguhvector* ref = new Tamguhvector;
        locking();
        hmap<short, bool> store;
        for (it = 0; it < b->Size(); it++)
            store[b->getshort(it)] = true;

        for (it = 0; it < size; it++) {
            if (store.find(values[it]) == store.end())
                store[values[it]] = true;
        }

        for (const auto& i : store)
            ref->values.push_back(i.first);
        unlocking();
        return ref;
    }

    v = b->Short();
    short vbis;
    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis ^= v;
        ref->values[it] = vbis;
    }
    return ref;
}


Exporting Tamgu* Tamguhvector::plus(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valueshort();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    short v = b->Short();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] += v;
    return ref;
}

Exporting Tamgu* Tamguhvector::minus(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] -= itr->Valueshort();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    short v = b->Short();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] -= v;
    return ref;
}

Exporting Tamgu* Tamguhvector::multiply(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] *= itr->Valueshort();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    short v = b->Short();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] *= v;
    return ref;
}

Exporting Tamgu* Tamguhvector::divide(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    short v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueshort();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }
            ref->values[it] /= v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Short();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] /= v;
    return ref;
}

Exporting Tamgu* Tamguhvector::power(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] = pow(ref->values[it], itr->Valueshort());
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    short v = b->Short();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = pow(ref->values[it], v);

    return ref;
}

Exporting Tamgu* Tamguhvector::shiftleft(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    short v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v <<= itr->Valueshort();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Short();
    long vint;
    for (it = 0; it < ref->values.size(); it++) {
        vint = ref->values[it];
        vint <<= v;
        ref->values[it] = vint;
    }
    return ref;
}

Exporting Tamgu* Tamguhvector::shiftright(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    short v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = ref->values[it];
            v >>= itr->Valueshort();
            ref->values[it] = v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Short();
    long vint;
    for (it = 0; it < ref->values.size(); it++) {
        vint = ref->values[it];
        vint >>= v;
        ref->values[it] = vint;
    }
    return ref;
}

Exporting Tamgu* Tamguhvector::mod(Tamgu* b, bool itself) {
    Tamguhvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamguhvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    short v;
    short vbis;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valueshort();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }
            vbis = ref->values[it];
            vbis %= v;
            ref->values[it] = vbis;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Short();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }

    for (it = 0; it < ref->values.size(); it++) {
        vbis = ref->values[it];
        vbis %= v;
        ref->values[it] = vbis;

    }
    return ref;
}

Exporting Tamgu* Tamguhvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    Tamgushort* it = new Tamgushort(0);
    it->Setreference();
    
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

    it->Resetreference();
    recipient->Forcedclean();
    return context;
}

Exporting Tamgu* Tamguhvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    
    Tamgushort* key = new Tamgushort(0);
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

class HComp {
    public:
    TamguCallFunction2 compare;
    short idthread;
    TamguConstShort p;
    TamguConstShort s;

    HComp(TamguFunction* comp, short idt) : compare(comp), p(0), s(0), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Eval(aNULL, aNULL, idthread)->Boolean();
    }
};

class HComparison {
    public:
    HComp* compare;

    HComparison(HComp* c)  {
        compare = c;
    }

    bool operator() (short i, short j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Tamgu* Tamguhvector::MethodShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    long nb = callfunc->Size();
    if (!nb) {
        Tamguivector* ivector = globalTamgu->Provideivector();
        for (long i = 0; i < shape.size(); i++) {
            ivector->storevalue(shape[i]);
        }
        return ivector;
    }
    
    shape.clear();
    Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
    if (v->isVectorContainer()) {
        if (nb != 1)
            globalTamgu->Returnerror(e_wrong_shape_definition, idthread);
        nb = v->Size();
        for (long i = 0; i < nb; i++) {
            shape.push_back(v->getinteger(i));
        }
        return aTRUE;
    }
    
    shape.push_back(v->Integer());

    for (long i = 1; i < nb; i++)
        shape.push_back(callfunc->Evaluate(i, contextualpattern, idthread)->Integer());
    return aTRUE;
}

Tamgu* Tamguhvector::MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalTamgu->Returnerror(e_expecting_a_comparison02, idthread);

        HComp kcomp((TamguFunction*)comp->Body(idthread), idthread);
        HComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Tamgu* Tamguhvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    long sz = Size();
    Tamgu* a = aNULL;
    bool testcond = false;
    locking();
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        var->storevalue(values[i]);
        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }
    unlocking();
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;
}

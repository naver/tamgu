/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubvector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgubvector.h"
#include "tamgubyte.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgufvector.h"
#include "tamgumap.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<bvectorMethod>  Tamgubvector::methods;
Exporting hmap<string, string> Tamgubvector::infomethods;
Exporting basebin_hash<unsigned long> Tamgubvector::exported;

Exporting short Tamgubvector::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubvector::AddMethod(TamguGlobal* global, string name, bvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgubvector::Setidtype(TamguGlobal* global) {
    Tamgubvector::InitialisationModule(global,"");
}


bool Tamgubvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgubvector::idtype = global->Getid("bvector");
    
    Tamgubvector::AddMethod(global, "min", &Tamgubvector::MethodMin, P_NONE, "min(): returns the min in the vector.");
    Tamgubvector::AddMethod(global, "max", &Tamgubvector::MethodMax, P_NONE, "max(): returns the max in the vector.");
    Tamgubvector::AddMethod(global, "minmax", &Tamgubvector::MethodMinMax, P_NONE, "minmax(): returns the min and the max in the vector.");

    Tamgubvector::AddMethod(global, "clear", &Tamgubvector::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgubvector::AddMethod(global, "reverse", &Tamgubvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgubvector::AddMethod(global, "reserve", &Tamgubvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamgubvector::AddMethod(global, "unique", &Tamgubvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    
    Tamgubvector::AddMethod(global, "last", &Tamgubvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgubvector::AddMethod(global, "join", &Tamgubvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgubvector::AddMethod(global, "shuffle", &Tamgubvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamgubvector::AddMethod(global, "sort", &Tamgubvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Tamgubvector::AddMethod(global, "sum", &Tamgubvector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgubvector::AddMethod(global, "product", &Tamgubvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgubvector::AddMethod(global, "push", &Tamgubvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgubvector::AddMethod(global, "pop", &Tamgubvector::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamgubvector::AddMethod(global, "poplast", &Tamgubvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgubvector::AddMethod(global, "merge", &Tamgubvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgubvector::AddMethod(global, "editdistance", &Tamgubvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamgubvector::AddMethod(global, "insert", &Tamgubvector::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");
    Tamgubvector::AddMethod(global, "permute", &Tamgubvector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");
    
    global->minimal_indexes[Tamgubvector::idtype] = true;
    
    if (version != "") {
        global->newInstance[Tamgubvector::idtype] = new Tamgubvector(global);
        global->RecordMethods(Tamgubvector::idtype, Tamgubvector::exported);
    }
    
    return true;
}

Exporting Tamgu* Tamgubvector::getvalue(BLONG i) {
    locking();
    if (i<0 || i>values.size()) {
        unlocking();
        return aNOELEMENT;
    }
    Tamgu* r = new Tamgubyte(values[i]);
    unlocking();
    return r;
}

Exporting TamguIteration* Tamgubvector::Newiteration(bool direction) {
    return new TamguIterationbvector(this, direction);
}

Exporting Tamgu* Tamgubvector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    if (a->Size() == 0)
        return aNULL;

    uchar val = a->String()[0];
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
        locking();
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val) {
                unlocking();
                return globalTamgu->ProvideConstint(i);
            }
        }
       unlocking();
        return aMINUSONE;
    }

    locking();
    for (size_t i = 0; i < values.size(); i++) {
        if (values[i] == val) {
            unlocking();
            return aTRUE;
        }
    }
    unlocking();
    return aFALSE;
}


Exporting Tamgu* Tamgubvector::Push(Tamgu* a) {
    locking();
    string v = a->String();
    if (v.size() > 0)
        values.push_back(v[0]);
    else
        values.push_back(0);
    unlocking();
    return this;
}

Exporting Tamgu* Tamgubvector::Pop(Tamgu* idx) {
    locking();
    if (!values.size()) {
        unlocking();
        return aFALSE;
    }

    BLONG v = idx->Integer();
    if (v == -1) {
        v = values.size() - 1;
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

Exporting void Tamgubvector::Clear() {
    //To set a variable back to empty
    locking();
    values.clear();
    unlocking();
}

Exporting string Tamgubvector::String() {
    locking();
    size_t i;
    stringstream r;
    r << "[";
    bool first = true;
    for (i = 0; i < values.size(); i++) {
        if (first)
            first = false;
        else
            r << ",";
        if (values[i] == '"')
            r << "'" << values[i] << "'";
        else
            r << '"' << values[i] << '"';
    }
    unlocking();
    r << "]";
    return r.str();
}

Exporting void Tamgubvector::Setstring(string& r, short idthread) {
    locking();
    size_t i;
    r = "[";
    bool first = true;
    for (i = 0; i < values.size(); i++) {
        if (first)
            first = false;
        else
            r += ",";
        if (values[i] == '"') {
            r += "'";
            r += values[i];
            r += "'";
        }
        else {
            r += "\"";
            r += values[i];
            r += "\"";
        }
    }
    unlocking();
    r += "]";
}


Exporting long Tamgubvector::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgubvector::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgubvector::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgubvector::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgubvector::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting Tamgu* Tamgubvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        locking();
        values.push_back(ke->Byte());
        unlocking();
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Tamgubvector* kvect = (Tamgubvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    if (ke->Type() == a_ivector) {
        Tamguivector* kvect = (Tamguivector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    if (ke->Type() == a_fvector) {
        Tamgufvector* kvect = (Tamgufvector*)ke;
        values.insert(values.end(), kvect->values.begin(), kvect->values.end());
        return this;
    }

    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        values.push_back(itr->Valueshort());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgubvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    long i;
    
    if (!ke->isContainer()) {
        uchar v=ke->Byte();
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

static bool comp(uchar s1, uchar s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icomp(uchar s1, uchar s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Tamgubvector::Sort(bool direction) {
    locking();

    if (direction == false)
        sort(values.begin(), values.end(), comp);
    else
        sort(values.begin(), values.end(), icomp);
    unlocking();
}

Exporting Tamgu* Tamgubvector::same(Tamgu* a) {
    if (!a->isVectorContainer())
        return aFALSE;

    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != ((Tamguvector*)a)->values[i]->Byte())
                    return aFALSE;
            }
            return aTRUE;
        }
        if (a->Type() == a_ivector) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (((Tamguivector*)a)->values[i] != values[i])
                    return aFALSE;
            }
            return aTRUE;
        }
        if (a->Type() == a_fvector) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (((Tamgufvector*)a)->values[i] != values[i])
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Tamgubvector* m = (Tamgubvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}


Exporting bool Tamgubvector::Permute() {
    locking();
    bool r = next_permutation(values.begin(), values.end());
    unlocking();
    return r;
}

Exporting void Tamgubvector::Shuffle() {
    locking();

    size_t sz = values.size();
    size_t i, f;
    long mx = sz;
    uchar v;
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

Exporting Tamgu* Tamgubvector::Unique() {
    locking();
    Tamgubvector* kvect = new Tamgubvector;
    hmap<uchar, bool> inter;
    for (long i = 0; i < values.size(); i++) {
        try {
            inter.at(values[i]);
        }
        catch(const std::out_of_range& oor) {
            inter[values[i]] = true;
            kvect->values.push_back(values[i]);
        }
    }
    unlocking();
    return kvect;
}

Exporting unsigned long Tamgubvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    long* v1 = new long[s1len + 1];
    long* v2 = new long[s2len + 1];

    y =  maxlocal(s1len, s2len);
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


Exporting Tamgu*  Tamgubvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_bvector) {
            values = ((Tamgubvector*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            values.reserve(ke->Size());
            TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back((uchar)itr->Keyinteger());
            itr->Release();
            return aTRUE;
        }

        if (ke->isContainer()) {
            values.clear();
            values.reserve(ke->Size());
            TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back((uchar)itr->Valueinteger());
            itr->Release();
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_byte(sv, values))
                return globalTamgu->Returnerror("Cannot set this value", idthread);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);

        values.clear();
        values.reserve(ke->Size());
        //We copy all values from ke to this
        for (long it = 0; it < ke->Size(); ++it)
            values.push_back((uchar)ke->getinteger(it));
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
                    values.insert(values.begin() + lkey, (uchar)ke->getinteger(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, (uchar)itr->Valueinteger());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->Byte());
        }
        else {
            long ikey = idx->Getinteger(idthread);
            long sz = values.size();
            long nb;
            uchar n = ke->Byte();
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

Tamgu* Tamgubvector::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    long ikey;
    ikey = key->Getinteger(idthread);
    if (ikey < 0)
        ikey = values.size() + ikey;
    
    locking();
    if (ikey < 0 || ikey >= values.size()) {
        unlocking();
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }

    key =  new Tamgubyte(values[ikey]);
    unlocking();
    return key;
}

Exporting Tamgu* Tamgubvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectabvector(contextualpattern);
            locking();
            if (kv->Type() == a_bvector)
                ((Tamgubvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(new Tamgubyte(values[i]));
            unlocking();
            return kv;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->ProvideConstint(v);
        }
        return this;
    }

    long ikey;
    Tamgu* keyright = NULL;
    ikey = idx->Getinteger(idthread);
    if (idx->isInterval())
        keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);

    locking();
    if (ikey < 0)
        ikey = values.size() + ikey;

	if (ikey < 0 || ikey >= values.size()) {
		unlocking();
		if (globalTamgu->erroronkey)
			return globalTamgu->Returnerror("Wrong index", idthread);
		return aNOELEMENT;
	}

    if (keyright == NULL) {
        keyright = new Tamgubyte(values[ikey]);
        unlocking();
        return keyright;
    }

    long iright = keyright->Getinteger(idthread);
    Tamgubvector* kvect;
    if (keyright != ((TamguIndex*)idx)->right)
        keyright->Release();
    
    if (iright < 0 || keyright == aNULL) {
        iright = values.size() + iright;
        if (iright<ikey) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>values.size()) {
            unlocking();
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    //In this case, we must create a new vector
    kvect = new Tamgubvector;
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    unlocking();
    return kvect;
}


Exporting Tamgu* Tamgubvector::Map(short idthread) {
    locking();
    Tamgumap* kmap = globalTamgu->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, new Tamgubyte(values[it]));
    }
    unlocking();
    return kmap;
}

Exporting Tamgu* Tamgubvector::Vector(short idthread) {
    locking();
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(new Tamgubyte(values[i]));
    unlocking();
    return kvect;
}


Exporting Tamgu* Tamgubvector::andset(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] &= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] &= v;
    return ref;
}

Exporting Tamgu* Tamgubvector::orset(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] |= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] |= v;
    return ref;
}

Exporting Tamgu* Tamgubvector::xorset(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isVectorContainer()) {
        long size = Size();

        Tamgubvector* ref = new Tamgubvector;
        hmap<short, bool> store;
        for (it = 0; it < b->Size(); it++)
            store[b->getshort(it)] = true;

        for (it = 0; it < size; it++) {
            if (store.find(values[it]) == store.end())
                store[values[it]] = true;
        }

        for (const auto& i : store)
            ref->values.push_back((uchar)i.first);

        return ref;
    }

    uchar v = (uchar)b->Short();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] ^= v;
    return ref;
}


Exporting Tamgu* Tamgubvector::plus(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] += v;
    return ref;
}

Exporting Tamgu* Tamgubvector::minus(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] -= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] -= v;
    return ref;
}

Exporting Tamgu* Tamgubvector::multiply(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] *= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] *= v;
    return ref;
}

Exporting Tamgu* Tamgubvector::divide(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    uchar v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = (uchar)itr->Valueinteger();
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

    v = (uchar)b->Integer();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] /= v;
    return ref;
}

Exporting Tamgu* Tamgubvector::power(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
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
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] = pow((double)ref->values[it], v);

    return ref;
}

Exporting Tamgu* Tamgubvector::shiftleft(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] <<= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    return ref;
}

Exporting Tamgu* Tamgubvector::shiftright(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] >>= (uchar)itr->Valueinteger();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    uchar v = (uchar)b->Integer();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    return ref;
}

Exporting Tamgu* Tamgubvector::mod(Tamgu* b, bool itself) {
    Tamgubvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgubvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    uchar v;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = (uchar)itr->Valueinteger();
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

    v = (uchar)b->Integer();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] %= v;
    return ref;
}

class BComp {
    public:
    TamguCallFunction2 compare;
    short idthread;
    Tamgubyte p;
    Tamgubyte s;

    BComp(TamguFunction* comp, short idt) : compare(comp), p(0), s(0), idthread(idt) {
        p.Setreference();
        s.Setreference();
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Eval(aNULL, aNULL, idthread)->Boolean();
    }
};

class BComparison {
    public:
    BComp* compare;

    BComparison(BComp* c)  {
        compare = c;
    }

    bool operator() (uchar i, uchar j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Tamgu* Tamgubvector::MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* comp= callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalTamgu->Returnerror("Expecting a comparison function with two parameters", idthread);

        BComp kcomp((TamguFunction*)comp->Body(idthread), idthread);
        BComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}



Exporting Tamgu* Tamgubvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    Tamgu* a = aNULL;
    bool testcond = false;
  
    locking();
    for (long i = 0; i < values.size() && !testcond; i++) {
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

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgulvector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamguivector.h"
#include "tamgulvector.h"
#include "tamguvector.h"
#include "tamgufvector.h"
#include "tamgumap.h"
#include "constobjects.h"
#include "instructions.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<lvectorMethod>  Tamgulvector::methods;

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgulvector::AddMethod(TamguGlobal* global, string name, lvectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(a_lvector) != global->infomethods.end() &&
            global->infomethods[a_lvector].find(name) != global->infomethods[a_lvector].end())
    return;

    global->infomethods[a_lvector][name] = infos;
    global->RecordArity(a_lvector, idname, arity);
}




void Tamgulvector::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgulvector::InitialisationModule(global,"");
}


bool Tamgulvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    Tamgulvector::AddMethod(global, "min", &Tamgulvector::MethodMin, P_NONE, "min(): returns the min in the vector.");
    Tamgulvector::AddMethod(global, "max", &Tamgulvector::MethodMax, P_NONE, "max(): returns the max in the vector.");
    Tamgulvector::AddMethod(global, "minmax", &Tamgulvector::MethodMinMax, P_NONE, "minmax(): returns the min and the max in the vector.");

    Tamgulvector::AddMethod(global, "clear", &Tamgulvector::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgulvector::AddMethod(global, "remove", &Tamgulvector::MethodRemove, P_ONE, "remove(long e): remove 'e' from the vector.");
    
    Tamgulvector::AddMethod(global, "reverse", &Tamgulvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgulvector::AddMethod(global, "reserve", &Tamgulvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamgulvector::AddMethod(global, "unique", &Tamgulvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamgulvector::AddMethod(global, "permute", &Tamgulvector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");
    
    Tamgulvector::AddMethod(global, "last", &Tamgulvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgulvector::AddMethod(global, "join", &Tamgulvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    
    Tamgulvector::AddMethod(global, "shuffle", &Tamgulvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamgulvector::AddMethod(global, "sort", &Tamgulvector::MethodSort, P_ONE, "sort(bool reverse): sort the elements within.");
    Tamgulvector::AddMethod(global, "shape", &Tamgulvector::MethodShape, P_NONE|P_ATLEASTONE, "shape(int s1, int s2...): defines the vector shape.");
    Tamgulvector::AddMethod(global, "matrixproduct", &Tamgulvector::MethodMatrixproduct, P_ONE, "matrixproduct(lvector): Matrix multiplication. The two vectors must have shapes");
    Tamgulvector::AddMethod(global, "transpose", &Tamgulvector::MethodTranspose, P_NONE, "transpose(lvector): Matrix transposition");
    Tamgulvector::AddMethod(global, "sum", &Tamgulvector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    
    Tamgulvector::AddMethod(global, "product", &Tamgulvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgulvector::AddMethod(global, "push", &Tamgulvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgulvector::AddMethod(global, "pop", &Tamgulvector::MethodPop, P_NONE | P_ONE, "pop(int i): Erase an element from the vector");
    Tamgulvector::AddMethod(global, "poplast", &Tamgulvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgulvector::AddMethod(global, "merge", &Tamgulvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgulvector::AddMethod(global, "editdistance", &Tamgulvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamgulvector::AddMethod(global, "insert", &Tamgulvector::MethodInsert, P_TWO, "insert(int i,v): Insert v at position i.");
    
    if (version != "") {
        global->returnindextypes[a_lvector] = a_long;
        global->minimal_indexes[a_lvector] = true;
        
        global->newInstance[a_lvector] = new Tamgulvector(global);
        global->RecordCompatibilities(a_lvector);
    }
    
    
    return true;
}

Exporting TamguIteration* Tamgulvector::Newiteration(bool direction) {
    return new TamguIterationlvector(this, direction);
}

Exporting Tamgu* Tamgulvector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    BLONG val = a->Long();
     if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectaivector(context);
        Doublelocking _lock(this, v);

        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] == val)
                v->values.push_back(i);
        }
        return  v;
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


Exporting Tamgu* Tamgulvector::getvalue(long i) {
    locking();
    if (i < 0 || i >= values.size()) {
        unlocking();
        return aNOELEMENT;
    }
    Tamgu* r = globalTamgu->ProvideConstlong(values[i]);
    unlocking();
    return r;
}


Exporting Tamgu* Tamgulvector::Push(Tamgu* a) {
    locking();
    values.push_back(a->Long());
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgulvector::Pop(Tamgu* idx) {
    locking();
    if (!values.size()) {
        unlocking();
        return aFALSE;
    }

    BLONG v = idx->Long();
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

Exporting void Tamgulvector::Clear() {
    //To set a variable back to empty
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgulvector::String() {
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
        r << values[i];
    }
    unlocking();
    r << "]";
    return r.str();
}


Exporting long Tamgulvector::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgulvector::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgulvector::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgulvector::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgulvector::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting Tamgu* Tamgulvector::andset(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] &= itr->Valuelong();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] &= v;
    return ref;
}

Exporting Tamgu* Tamgulvector::orset(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] |= itr->Valuelong();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] |= v;
    return ref;
}

Exporting Tamgu* Tamgulvector::xorset(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isVectorContainer()) {
        long size = Size();

        Tamgulvector* ref = new Tamgulvector;
        hmap<BLONG, bool> store;
        for (it = 0; it < b->Size(); it++)
            store[b->getlong(it)] = true;

        for (it = 0; it < size; it++) {
            if (store.find(values[it]) == store.end())
                store[values[it]] = true;
        }

        for (const auto& i : store)
            ref->values.push_back(i.first);

        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] ^= v;
    return ref;
}


Exporting Tamgu* Tamgulvector::plus(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] += itr->Valuelong();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] += v;
    return ref;
}

Exporting Tamgu* Tamgulvector::minus(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] -= itr->Valuelong();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] -= v;
    return ref;
}

Exporting Tamgu* Tamgulvector::multiply(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] *= itr->Valuelong();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] *= v;
    return ref;
}

Exporting Tamgu* Tamgulvector::divide(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    BLONG v;
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valuelong();
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

    v = b->Long();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] /= v;
    return ref;
}

Exporting Tamgu* Tamgulvector::power(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

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

Exporting Tamgu* Tamgulvector::shiftleft(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] <<= itr->Valuelong();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    return ref;
}

Exporting Tamgu* Tamgulvector::shiftright(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            ref->values[it] >>= itr->Valuelong();
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    BLONG v = b->Long();
    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] <<= v;
    return ref;
}

Exporting Tamgu* Tamgulvector::mod(Tamgu* b, bool itself) {
    Tamgulvector* ref = this;
    if (itself)
        ref = this;
    else
        ref = (Tamgulvector*)Atom(true);

    Doublelocking _lock(ref, b);
    size_t it;
    BLONG v;

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < ref->values.size(); it++) {
            if (itr->End() == aTRUE)
                break;
            v = itr->Valuelong();
            if (v == 0) {
                ref->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }

            ref->values[it] %= v;
            itr->Next();
        }
        itr->Release();
        return ref;
    }

    v = b->Long();
    if (v == 0) {
        ref->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }

    for (it = 0; it < ref->values.size(); it++)
        ref->values[it] %= v;
    return ref;
}


Exporting unsigned long Tamgulvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    BLONG* v1 = new BLONG[s1len + 1];
    BLONG* v2 = new BLONG[s2len + 1];

    y =  maxlocal(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = values[x];
        if (x < s2len)
            v2[x] = e->getlong(x);
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

Exporting void Tamgulvector::Shuffle() {
    locking();

    size_t sz = values.size();
    size_t i, f;
    long mx = sz;
    BLONG v;
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

Exporting Tamgu* Tamgulvector::Unique() {
    Tamgulvector* kvect = new Tamgulvector;
    std::set<BLONG> inter;
    
    locking();
    for (int i = 0; i < values.size(); i++) {
        if (inter.insert(values[i]).second)
            kvect->values.push_back(values[i]);
    }
    unlocking();
    
    return kvect;
}

Exporting bool Tamgulvector::Permute() {
    locking();

    bool v = next_permutation(values.begin(), values.end());
    unlocking();
    return v;
}


Exporting Tamgu* Tamgulvector::same(Tamgu* a) {
    if (!a->isVectorContainer())
        return aFALSE;

    Doublelocking _lock(this, a);

    if (a->Type() != a_lvector) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] != a->getlong(i))
                    return aFALSE;
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Tamgulvector* m = (Tamgulvector*)a;
    if (m->values.size() != values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (m->values[i] != values[i])
            return aFALSE;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgulvector::Map(short idthread) {
    locking();
    Tamgumap* kmap = globalTamgu->Providemap();

    char buff[100];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(buff, 100, "%d", it);
        kmap->Push(buff, globalTamgu->Providelong(values[it]));
    }
    unlocking();
    return kmap;
}

Exporting Tamgu* Tamgulvector::Vector(short idthread) {
    locking();
    Tamguvector* kvect = globalTamgu->Providevector();
    kvect->values.reserve(values.size());
    for (int i = 0; i < values.size(); i++)
        kvect->Push(globalTamgu->ProvideConstlong(values[i]));
    unlocking();
    return kvect;
}

Tamgu* Tamgulvector::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
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

    key =  globalTamgu->Providelong(values[ikey]);
    unlocking();
    return key;
}

Exporting Tamgu* Tamgulvector::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer()) {
            Tamgu* kv = Selectalvector(contextualpattern);
            locking();
            if (kv->Type() == a_lvector)
                ((Tamgulvector*)kv)->values = values;
            else
                for (int i = 0; i < values.size(); i++)
                    kv->Push(globalTamgu->ProvideConstlong(values[i]));
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

    Tamgulvector* kvect;
    if (ikey < 0)
        ikey = values.size() + ikey;

    locking();

	if (ikey < 0 || ikey >= values.size()) {
		unlocking();
		if (globalTamgu->erroronkey)
			return globalTamgu->Returnerror(e_wrong_index, idthread);
		return aNOELEMENT;
	}

    if (keyright == NULL) {
        keyright = globalTamgu->Providelong(values[ikey]);
        unlocking();
        return keyright;
    }

    long iright = keyright->Integer();

    if (keyright != ((TamguIndex*)idx)->right)
        keyright->Release();
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
    kvect = new Tamgulvector;
    for (long i = ikey; i < iright; i++)
        kvect->values.push_back(values[i]);
    unlocking();
    return kvect;
}

Exporting Tamgu* Tamgulvector::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    Doublelocking _lock(this, ke);

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            values.clear();
            return aTRUE;
        }

        if (ke->Type() == a_lvector) {
            values = ((Tamgulvector*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            values.clear();
            values.reserve(ke->Size());
            TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Keyinteger());
            itr->Release();
            return aTRUE;
        }

        long sz = ke->Size();
        if (ke->isVectorContainer()) {
            values.clear();
            if (sz) {
                values.reserve(sz);
                for (long i=0; i< sz; i++)
                    values.push_back(ke->getlong(i));
            }
            return aTRUE;
        }

        if (ke->isContainer()) {
            values.clear();
            values.reserve(ke->Size());
            TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.push_back(itr->Valuelong());
            itr->Release();
            return aTRUE;
        }
        if (ke->isString()) {
            values.clear();
            string sv = ke->String();
            if (!v_comma_split_long(sv, values))
                return globalTamgu->Returnerror(e_cannot_set_this, idthread);
            return aTRUE;
        }
        ke = ke->Vector(idthread);
        if (!ke->isVectorContainer())
            return globalTamgu->Returnerror(e_cannot_set_this, idthread);

        values.clear();
        values.reserve(ke->Size());
        //We copy all values from ke to this
        for (long it = 0; it < ke->Size(); ++it)
            values.push_back(ke->getlong(it));
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
                    values.insert(values.begin() + lkey, ke->getlong(i));
                }
                return aTRUE;
            }

            if (ke->isContainer()) {
                TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    values.insert(values.begin() + lkey, itr->Valuelong());
                itr->Release();
                return aTRUE;
            }

            values.insert(values.begin() + lkey, ke->Long());
        }
        else {
            long ikey = idx->Getinteger(idthread);
            long sz = values.size();
            long nb;
            BLONG n = ke->Long();
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

static bool compInteger(BLONG s1, BLONG s2) {
    if (s1<s2)
        return true;
    return false;
}

static bool icompInteger(BLONG s1, BLONG s2) {
    if (s1>s2)
        return true;
    return false;
}

Exporting void Tamgulvector::Sort(bool direction) {
    locking();
    if (direction == false)
        sort(values.begin(), values.end(), compInteger);
    else
        sort(values.begin(), values.end(), icompInteger);
    unlocking();
}

Exporting Tamgu* Tamgulvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        locking();
        values.push_back(ke->Long());
        unlocking();
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == a_lvector) {
        Tamgulvector* kvect = (Tamgulvector*)ke;
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
        values.push_back(itr->Valuelong());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgulvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    long i;
    
    if (!ke->isContainer()) {
        BLONG v=ke->Long();
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
            values.push_back(itr->Valuelong());
            i++;
            continue;
        }
        
        values[i] += itr->Valuelong();
        i++;
    }
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgulvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    Tamgu* a;
    Tamgulong* it = globalTamgu->Providelong(0);
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

    recipient->Forcedclean();
    it->Resetreference();
    return context;
}

Exporting Tamgu* Tamgulvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    Tamgulong* key = globalTamgu->Providelong(0);
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

class LComp {
    public:
    TamguCallFunction2 compare;
    short idthread;
    TamguConstLong p;
    TamguConstLong s;

    LComp(TamguFunction* comp, short idt) : compare(comp), p(0), s(0), idthread(idt) {
        compare.arguments.push_back(&p);
        compare.arguments.push_back(&s);
    }

    bool get() {
        return compare.Eval(aNULL, aNULL, idthread)->Boolean();
    }
};

class LComparison {
    public:
    LComp* compare;

    LComparison(LComp* c)  {
        compare = c;
    }

    bool operator() (BLONG i, BLONG j) {
        compare->p.value = i;
        compare->s.value = j;
        return compare->get();
    }
};

Tamgu* Tamgulvector::MethodShape(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

Tamgu* Tamgulvector::MethodTranspose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (shape.size() != 2)
        return globalTamgu->Returnerror("Error: Can only transpose a two dimensions matrix", idthread);
    
    Tamgulvector* transpose = (Tamgulvector*)Selectalvector(contextualpattern);
    transpose->values.reserve(values.size());
    transpose->shape.push_back(shape[1]);
    transpose->shape.push_back(shape[0]);
    for (long i = 0; i < shape[1]; i++) {
        for (long j = 0; j < shape[0]; j++) {
            transpose->values.push_back(values[i+j*shape[1]]);
        }
    }
    return transpose;
}

Tamgu* Tamgulvector::MethodMatrixproduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (shape.size() != 2)
        return globalTamgu->Returnerror("Error: the shape should have two dimensions", idthread);
    
    Tamgu* vect = callfunc->Evaluate(0, contextualpattern, idthread);
    if (vect->Type() != a_lvector)
        return globalTamgu->Returnerror("Error: the argument must be another 'lvector'", idthread);
    
    Tamgulvector* fvect = (Tamgulvector*)vect;
    long sh = shape[1];
    long sh10 = shape[0];
    long sh21 = fvect->shape[1];
    
    if (fvect->shape.size() != 2 || fvect->shape[0] != sh)
        return globalTamgu->Returnerror("Error: Vector shapes are incompatible", idthread);
    
    Tamgulvector* result = (Tamgulvector*)Selectalvector(contextualpattern);
    result->values.reserve(sh10 * sh);
    result->shape.push_back(sh10);
    result->shape.push_back(sh);

    long sz = values.size();
    long szf = fvect->values.size();
    long i, j, k;
    vector<BLONG> transpose;
    BLONG v;

    transpose.reserve(szf);
    for (i = 0; i < sh21; i++) {
        for (j = 0; j < sh; j++) {
            transpose.push_back(fvect->values[i+j*sh21]);
        }
    }
    
    for (i = 0; i < sz; i += sh) {
        for (j = 0; j < szf; j += sh) {
            v = 0;
            for (k = 0; k < sh; k++) {
                v += values[k + i] * transpose[k + j];
            }
            result->values.push_back(v);
        }
    }

    return result;
}


Tamgu* Tamgulvector::MethodSort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    Tamgu* comp = callfunc->Evaluate(0, contextualpattern, idthread);
    if (comp->isFunction()) {
        if (comp->Size() != 2)
            return globalTamgu->Returnerror(e_expecting_a_comparison02, idthread);

        LComp kcomp((TamguFunction*)comp->Body(idthread), idthread);
        LComparison kfcomp(&kcomp);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }
    Sort(comp->Boolean());
    return this;
}


Exporting Tamgu* Tamgulvector::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
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

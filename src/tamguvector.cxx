/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguvector.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgumap.h"
#include "tamgulist.h"
#include "instructions.h"
#include "constobjects.h"
#include "tamguuvector.h"
#include "tamgufvector.h"
#include "tamgubyte.h"
#include "tamgufile.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<vectorMethod>  Tamguvector::methods;
Exporting hmap<string, string> Tamguvector::infomethods;
Exporting bin_hash<unsigned long> Tamguvector::exported;

Exporting short Tamguvector::idtype = 0;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
    #define min(a,b)            (((a) < (b)) ? (a) : (b))
        #endif

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguvector::AddMethod(TamguGlobal* global, string name, vectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguvector::Setidtype(TamguGlobal* global) {
        Tamguvector::idtype = global->Getid("vector");
    }

   bool Tamguvector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamguvector::idtype = global->Getid("vector");

    Tamguvector::AddMethod(global, "clear", &Tamguvector::MethodClear, P_NONE, "clear(): clear the container.");
    Tamguvector::AddMethod(global, "flatten", &Tamguvector::MethodFlatten, P_NONE, "flatten(): flatten a vector structure.");

    Tamguvector::AddMethod(global, "remove", &Tamguvector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");
    Tamguvector::AddMethod(global, "ngrams", &Tamguvector::MethodNGrams, P_ONE|P_TWO, "ngrams(int nb, int sep): produces a ngrams svector.");

    Tamguvector::AddMethod(global, "reverse", &Tamguvector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamguvector::AddMethod(global, "unique", &Tamguvector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamguvector::AddMethod(global, "reserve", &Tamguvector::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the vector.");
    Tamguvector::AddMethod(global, "join", &Tamguvector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamguvector::AddMethod(global, "shuffle", &Tamguvector::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the vector.");
    Tamguvector::AddMethod(global, "permute", &Tamguvector::MethodPermute, P_NONE, "permute(): permute the values in the vector after each call.");
    Tamguvector::AddMethod(global, "last", &Tamguvector::MethodLast, P_NONE, "last(): return the last element.");
    Tamguvector::AddMethod(global, "sort", &Tamguvector::MethodSort, P_ONE | P_TWO, "sort(bool reverse | function): sort the values in the vector.");
    Tamguvector::AddMethod(global, "sortint", &Tamguvector::MethodSortint, P_ONE, "sortint(bool reverse | function): sort the values in the vector as int.");
    Tamguvector::AddMethod(global, "sortustring", &Tamguvector::MethodSortustring, P_ONE, "sortustring(bool reverse | function): sort the values in the vector as ustring.");
    Tamguvector::AddMethod(global, "sortstring", &Tamguvector::MethodSortstring, P_ONE, "sortstring(bool reverse): sort the values in the vector as string.");
    Tamguvector::AddMethod(global, "sortfloat", &Tamguvector::MethodSortfloat, P_ONE, "sortfloat(bool reverse): sort the values in the vector as float.");
    Tamguvector::AddMethod(global, "sum", &Tamguvector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguvector::AddMethod(global, "product", &Tamguvector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguvector::AddMethod(global, "push", &Tamguvector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamguvector::AddMethod(global, "pop", &Tamguvector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Tamguvector::AddMethod(global, "poplast", &Tamguvector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamguvector::AddMethod(global, "merge", &Tamguvector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamguvector::AddMethod(global, "editdistance", &Tamguvector::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with vector 'v'.");
    Tamguvector::AddMethod(global, "insert", &Tamguvector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");
    Tamguvector::AddMethod(global, "read", &Tamguvector::MethodRead, P_ONE, "read(string path): Read the content of a file into the container.");

    global->newInstance[Tamguvector::idtype] = new Tamguvector(global);
    global->RecordMethods(Tamguvector::idtype, Tamguvector::exported);
    global->newInstance[a_constvector] = new TamguConstvector(global);
    global->RecordMethods(a_constvector, Tamguvector::exported);

    Tamgua_vector::InitialisationModule(global, version);
    
    return true;
}

Exporting TamguIteration* Tamguvector::Newiteration(bool direction) {
    return new TamguIterationvector(this, direction);
}

Exporting bool Tamguvector::Permute() {
    return next_permutation(values.begin(), values.end());
}

Exporting void Tamguvector::Insert(long idx, Tamgu* ke) {
    ke = ke->Atom();
    ke->Addreference(reference+1);
    if (idx<0)
        idx=0;

    if (idx>=values.size())
        values.push_back(ke);
    else
        values.insert(values.begin() + idx, ke);
}


Exporting void Tamguvector::Cleanreference(short inc) {
    Locking* _lock = _getlock(this);
    for (long i=0;i<values.size();i++)
        values[i]->Removecontainerreference(inc);
    _cleanlock(_lock);
}

Exporting void Tamguvector::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect=false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    for (long i = 0; i< values.size(); i++) {
        values[i]->Addreference(inc);
    }
    _cleanlock(_lock);
    loopmark=false;
}

Exporting void Tamguvector::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect=false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    for (long i = 0; i< values.size(); i++) {
        values[i]->Addreference(1);
    }
    _cleanlock(_lock);
    loopmark=false;
}


static void resetVector(Tamguvector* kvect, short inc) {
    kvect->reference -= inc;

    Locking* _lock = _getlock(kvect);
    vector<Tamgu*>& values = kvect->values;
    if (values.size() == 0) {
        _cleanlock(_lock);
        return;
    }

    for (int itx = 0; itx < values.size(); itx++)
        values[itx]->Removereference(inc);

    _cleanlock(_lock);
}

Exporting void Tamguvector::Resetreference(short inc) {
    if ((reference + containerreference - inc) > 0)
        resetVector(this, inc);
    else {
        resetVector(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}


Exporting void Tamguvectorbuff::Resetreference(short inc) {
    if ((reference + containerreference - inc) > 0)
        resetVector(this, inc);
    else {
        resetVector(this, inc + 1 - protect);
        if (!protect) {
            reference = 0;
            protect = true;

            values.clear();
            used = false;
            globalTamgu->vectorempties.push_back(idx);
        }
    }
}

Exporting Tamgu* Tamguvector::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (size_t i = 0; i < values.size(); i++) {
            if (a->same(values[i]) == aTRUE)
                v->values.push_back(i);
        }
        return v;
    }

    if (context->isNumber()) {
        Locking _lock(this);
        for (size_t i = 0; i < values.size(); i++) {
            if (a->same(values[i]) == aTRUE)
                return globalTamgu->Provideint(i);
        }
        return aMINUSONE;
    }
    
    Locking _lock(this);
    for (size_t i = 0; i < values.size(); i++) {
        if (a->same(values[i]) == aTRUE)
            return aTRUE;
    }

    return aFALSE;
}

Exporting Tamgu* Tamguvector::Push(Tamgu* a) {
    Locking _lock(this);
    a = a->Atom();
    a->Addreference(reference+1);
    values.push_back(a);
    return aTRUE;
}

Exporting Tamgu* Tamguvector::Push(TamguGlobal* g, Tamgu* a, short idhtread) {
    Locking _lock(this);
    a = a->Atom();
    a->Addreference(reference+1);
    values.push_back(a);
    return aTRUE;
}

Exporting Tamgu* Tamguvector::Pop(Tamgu* idx) {
    Locking _lock(this);
    if (!values.size())
        return aFALSE;

    BLONG v = idx->Integer();

    if (v == -1) {
        idx = values.back();
        values.pop_back();
    }
    else {
        if (v < 0 || v >= (BLONG)values.size())
            return aFALSE;
        idx = values[v];
        values.erase(values.begin() + v);
    }

    idx->Removereference(reference + 1);
    return aTRUE;
}



Exporting string Tamguvector::String() {
    Locking _lock(this);
    if (loopmark)
        return("[...]");
    TamguCircular _c(this);
    string res;
    int it;
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < values.size(); it++) {
        element = values[it];
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx == "")
                sx = "''";
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
            res += sx;
        }
        else {
            if (beg == false)
                res += ",";
            stringing(res, sx);
        }
        beg = false;

    }
    res += "]";
    return res;
}

Exporting Tamgu* Tamguvector::Map(short idthread) {
    Locking _lock(this);
    if (loopmark)
        return aNULL;
        
    loopmark=true;
    Tamgumap* kmap = globalTamgu->Providemap();
    char ch[20];
    for (int it = 0; it < values.size(); it++) {
        sprintf_s(ch, 20, "%d", it);
        kmap->Push(ch, values[it]);
    }
    loopmark=false;
    return kmap;
}

Exporting string Tamguvector::JSonString() {
    Locking _lock(this);
    if (loopmark)
        return("");
    TamguCircular _c(this);
    string res;
    int it;
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < values.size(); it++) {
        element = values[it];
        sx = element->JSonString();
        if (!element->isString() || element->isContainer()) {
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
        }
        else {
            if (beg == false)
                res += ",";
        }
        res += sx;
        beg = false;
    }
    res += "]";
    return res;
}

Exporting long Tamguvector::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamguvector::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamguvector::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamguvector::Boolean() {
    Locking _lock(this);
    if (values.size())
        return true;
    return false;
}

//Basic operations
Exporting long Tamguvector::Size() {
    Locking _lock(this);
    return values.size();
}

Exporting Tamgu* Tamguvector::andset(Tamgu *b, bool itself) {
    Tamguvector* ref;

    long size = Size();
    size_t it;
    Tamgu* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamguvector*)Atom(true);

        Locking _lock(ref);

        for (it = 0; it < size; it++) {
            ke = ref->values[it]->andset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    Locking _lock(this);

    ref = globalTamgu->Providevector();
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        for (it = 0; it < size; it++) {
            ke = itr->IteratorValue();
            if (values[it]->same(ke) == aTRUE) {
                ref->Push(ke);
                break;
            }
        }
    }
    itr->Release();
    return ref;

}

Exporting Tamgu* Tamguvector::orset(Tamgu *b, bool itself) {
    Tamguvector* ref;

    long size = Size();
    if (itself)
        ref = this;
    else
        ref = (Tamguvector*)Atom(true);

    if (!b->isContainer()) {
        Locking _lock(ref);

        for (long it = 0; it < size; it++) {
            ref->values[it]->orset(b, true);
        }
        return ref;
    }

    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamguvector::xorset(Tamgu *b, bool itself) {
    Tamguvector* ref;

    long size = Size();
    size_t it;
    Tamgu* ke;
    if (!b->isContainer()) {
        Locking* _lock = NULL;

        if (itself) {
            ref = this;
            _lock = _getlock(this);
        }
        else
            ref = (Tamguvector*)Atom(true);

        for (it = 0; it < size; it++) {
            ref->values[it]->xorset(b, true);
        }
        
        _cleanlockif(_lock);
        return ref;
    }

    Locking _lock(this);


    ref = globalTamgu->Providevector();
    bool found=false;
    vector<Tamgu*> vals;
    
    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        vals.push_back(itr->Value());
    itr->Release();

    long itv;
    for (it = 0; it < size; it++) {
        ke = values[it];
        found = false;
        for (itv = 0; itv < vals.size(); itv++)  {
            if (vals[itv] == NULL)
                continue;

            if (vals[itv]->same(ke) == aTRUE) {
                vals[itv]->Release();
                vals[itv] = NULL;
                found = true;
                break;
            }
        }
        if (!found)
            ref->Push(ke);
    }

    for (itv = 0; itv < vals.size(); itv++) {
        if (vals[itv] == NULL)
            continue;
        ref->Push(vals[itv]);
        vals[itv]->Release();
    }

    return ref;
}


Exporting Tamgu* Tamguvector::plus(Tamgu* b, bool itself) {
    Tamguvector* ref;
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    
    if (loopmark) {
        if (_lock != NULL)
            delete _lock;
        return aNULL;
    }

    TamguCircular _c(this);

    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->plus(kv, true);
            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ref->values[it]->plus(b, true);
    }

    _cleanlockif(_lock);
    return ref;
}

Exporting Tamgu* Tamguvector::minus(Tamgu *b, bool itself) {
    Tamguvector* ref;
    
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->minus(kv, true);
            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ref->values[it]->minus(b, true);
    }

    _cleanlockif(_lock);
    return ref;
}

Exporting Tamgu* Tamguvector::multiply(Tamgu *b, bool itself) {
    Tamguvector* ref;
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->multiply(kv, true);
            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ref->values[it]->multiply(b, true);
    }

    _cleanlockif(_lock);
    return ref;
}

Exporting Tamgu* Tamguvector::divide(Tamgu *b, bool itself) {
    Tamguvector* ref;
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->divide(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                _cleanlockif(_lock);
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->divide(b, true);
        if (ke->isError()) {
            ref->Release();
            _cleanlockif(_lock);
            return ke;
        }
    }

    _cleanlockif(_lock);
    return ref;
}

Exporting Tamgu* Tamguvector::power(Tamgu *b, bool itself) {
    Tamguvector* ref;
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;

    long size = values.size();

    if (b->isContainer()) {
        Tamgu* ke;
        Tamgu* kv;

        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->power(kv, true);
            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ref->values[it]->power(b, true);
    }

    _cleanlockif(_lock);
    return ref;
}

Exporting Tamgu* Tamguvector::shiftleft(Tamgu *b, bool itself) {
    Tamguvector* ref;
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->shiftleft(kv, true);
            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ref->values[it]->shiftleft(b, true);
    }

    _cleanlockif(_lock);
    return ref;
}

Exporting Tamgu* Tamguvector::shiftright(Tamgu *b, bool itself) {
    Tamguvector* ref;
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;
    Tamgu* kv;

    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ref->values[it]->shiftright(kv, true);
            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ref->values[it]->shiftright(b, true);
    }

    _cleanlockif(_lock);
   return ref;
}

Exporting Tamgu* Tamguvector::mod(Tamgu *b, bool itself) {
    Tamguvector* ref;
    Locking* _lock = NULL;
    if (itself) {
        ref = this;
        _lock = _getlock(this);
    }
    else
        ref = (Tamguvector*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    long size = values.size();

    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->mod(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                _cleanlockif(_lock);
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        _cleanlockif(_lock);
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->mod(b, true);
        if (ke->isError()) {
            ref->Release();
            _cleanlockif(_lock);
            return ke;
        }
    }

    _cleanlockif(_lock);
    return ref;
}

Tamgu* Tamguvector::MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //The separator between values
    string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
    Clear();
    Tamgufile file;
    
#ifdef WIN32
    fopen_s(&file.thefile, STR(filename), "rb");
#else
    file.thefile=fopen(STR(filename), "rb");
#endif
    
    if (file.thefile == NULL) {
        string msg="Cannot open the file:";
        msg += filename;
        return globalTamgu->Returnerror(msg, idthread);
    }
    
    Locking _lock(this);
    vector<string> v;
    file.readall(v);
    Tamgu* a;
    for (long i = 0; i < v.size(); i++) {
        a = globalTamgu->Providestring(v[i]);
        a->Addreference(reference+1);
        values.push_back(a);
    }
    fclose(file.thefile);
    
    return this;
}

//------------------------

Exporting TamguIteration* TamguInfinitevector::Newiteration(bool rev) {
    Tamgu* s = seed;
    Tamgu* st = step;

    if (!seed->isNumber())
        s = seed->Put(aNULL, aNULL, globalTamgu->GetThreadid());

    if (!step->isNumber())
        st = step->Put(aNULL, aNULL, globalTamgu->GetThreadid());

    if (compute) {//In this case, the step is a difference between the two values step-seed
        if (s->Type() == a_float || st->Type() == a_float) {
            double d = st->Float() - s->Float();
            TamguIterationInfinitefloatvector* iter = new TamguIterationInfinitefloatvector(false);
            iter->itx = s->Float();
            iter->inc = d;
            if (s != seed)
                s->Release();
            if (st != step)
                st->Release();
            return iter;
        }

        long d = st->Integer() - s->Integer();
        TamguIterationInfinitevector* iter = new TamguIterationInfinitevector(false);
        iter->itx = s->Integer();
        iter->inc = d;
        if (s != seed)
            s->Release();
        if (st != step)
            st->Release();
        return iter;
    }

    if (s->Type() == a_float || st->Type() == a_float) {
        TamguIterationInfinitefloatvector* iter = new TamguIterationInfinitefloatvector(false);
        iter->itx = s->Float();
        iter->inc = st->Float();
        if (s != seed)
            s->Release();
        if (st != step)
            st->Release();
        return iter;
    }

    TamguIterationInfinitevector* iter = new TamguIterationInfinitevector(false);
    iter->itx = s->Integer();
    iter->inc = st->Integer();
    if (s != seed)
        s->Release();
    if (st != step)
        st->Release();
    return iter;
}

Exporting Tamgu* TamguInfinitevector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    
    Tamgu* a;
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    Tamgu* number = seed;
    Tamgu* st = step;
    
    if (seed->isCallVariable())
        number = number->Get(aNULL, aNULL, idthread);
    else
        number = seed->Atom(true);
    
    number->Setreference();
    
    if (step->isCallVariable()) {
        st = step->Get(aNULL, aNULL, idthread);
        st->Setreference();
    }
    
    if (compute) {
        a = st->minus(number, false);
        if (st != step)
            st->Resetreference();
        st = a;
    }
    
    while (true) {
        recipient->Putvalue(number, idthread);
        
        a = bd->DirectGet(environment, aNULL, idthread);
        if (a->isNULL()) {
            number->plus(st, true);
            continue;
        }
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            
            recipient->Forcedclean();
            a->Release();
            context->Release();
            number->Resetreference();
            if (st != seed)
                st->Resetreference();
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        number->plus(st, true);
        a->Release();
    }
    
    number->Resetreference();
    if (st != seed)
        st->Resetreference();
    recipient->Forcedclean();
    return context;
}

Exporting Tamgu* TamguInfinitevector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    
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
    
    Tamgu* key = seed;
    Tamgu* st = step;
    
    if (seed->isCallVariable())
        key = key->Get(aNULL, aNULL, idthread);
    else
        key = seed->Atom(true);
    
    key->Setreference();
    
    if (step->isCallVariable()) {
        st = step->Get(aNULL, aNULL, idthread);
        st->Setreference();
    }

    if (compute) {
        returnval = st->minus(key, false);
        if (st != step)
            st->Resetreference();
        st = returnval;
    }

    while (true) {
        
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectGet(env, aNULL, idthread);
            
            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }
            
            if (globalTamgu->Error(idthread)) {
                var->Forcedclean();
                key->Resetreference();
                if (st != seed)
                    st->Resetreference();
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
        
        key->plus(step, true);
    }
    
    key->Resetreference();
    if (st != seed)
        st->Resetreference();
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

Exporting string TamguInfinitevector::String() {
    stringstream res;
    if (direction)
        res << "[.." << seed->String() << "]";
    else
        res << "[" << seed->String() << "..]";
    return res.str();
}

Exporting Tamgu*  Tamguvector::Put(Tamgu* idx, Tamgu* value, short idthread) {
    Doublelocking _lock(this, value);
    if (!idx->isIndex()) {
        if (value == this)
            return aTRUE;

        if (value == aNULL) {
            Clear();
            return aTRUE;
        }
        if (value->Type() == a_list) {
            Tamgulist* kvect = (Tamgulist*)value;
            //We copy all values from value to this
            Clear();
            Reserve(value->Size());
            {

                for (auto& it : kvect->values)
                    Push(it);
            }
            return aTRUE;
        }

        long sz = value->Size();
        if (value->isVectorContainer()) {
            //We copy all values from value to this
            Clear();
            if (sz) {
                Reserve(sz);
                for (int it = 0; it < sz; it++)
                    Push(value->getvalue(it));
            }
            return aTRUE;
        }
        //We gather all the keys from the MAP
        if (value->isMapContainer()) {
            Clear();
            if (sz) {
                Reserve(sz);
                TamguIteration* itr = value->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    Push(itr->Key());
                itr->Release();
            }
            return aTRUE;
        }

        value = value->Vector(idthread);
        if (!value->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);

        sz = value->Size();
        
        if (value != this) {
            Clear();
            if (sz) {
                Reserve(sz);
                //We copy all values from ke to this
                for (long it = 0; it < value->Size(); ++it)
                    Push(value->getvalue(it));
            }
            value->Releasenonconst();
        }
        return aTRUE;
    }
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
        Tamgu* krkey;
        while (rkey >= lkey) {
            krkey = values[rkey];
            values.erase(values.begin() + rkey);
            krkey->Removereference(reference + 1);
            rkey--;
        }
        if (value->isVectorContainer()) {
            for (long i = value->Size() - 1; i >= 0; i--) {
                krkey = value->getvalue(i);
                krkey = krkey->Atom();
                values.insert(values.begin() + lkey, krkey);
                krkey->Addreference(reference+1);
            }
            return aTRUE;
        }
        if (value->isContainer()) {
            TamguIteration* itr = value->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                krkey = itr->Value();
                if (krkey->Reference())
                    krkey = krkey->Atom();
                values.insert(values.begin() + lkey, krkey);
                krkey->Addreference(reference+1);
            }
            itr->Release();
            return aTRUE;
        }
        krkey = value->Atom();
        values.insert(values.begin() + lkey, krkey);
        krkey->Addreference(reference+1);
        return aTRUE;
    }
    long ikey = idx->Integer();
    long nb, mx;
    mx = values.size();
    if (ikey >= mx) {
        for (nb = values.size(); nb < ikey; nb++)
            values.push_back(aNULL);
        Push(value);
    }
    else {
        if (ikey < 0) {
            ikey = mx + ikey;
            if (ikey < 0)
                return globalTamgu->Returnerror("Cannot set this value", idthread);
        }
        value = value->Atom();
        if (values[ikey] != NULL)
            values[ikey]->Removereference(reference + 1);
        values[ikey] = value;
        value->Addreference(reference+1);
    }
    return aTRUE;
}


Exporting Tamgu* Tamguvector::Get(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    Locking _lock(this);

    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

        //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Tamgu* ke;
            Tamgu* map = Selectamap(contextualpattern);
            size_t i = 0;
            char ch[20];
            for (int it = 0; it < values.size(); it++) {
                sprintf_s(ch, 20, "%ld", i);
                ke = values[it];
                map->Push(ch, ke);
                i++;
            }
            return map;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }
        return this;
    }

    Tamgu* key;
    Tamgu* keyright = NULL;

    TamguIndex* kind = (TamguIndex*)idx;
    key = kind->left->Get(aNULL, aNULL, idthread);
    if (kind->interval == true)
        keyright = kind->right->Get(aNULL, aNULL, idthread);

    long ikey;
    bool stringkey = false;
    if (key->Type() == a_string) {
        string sf = key->String();
        stringkey = true;
        bool found = false;
        if (kind->signleft) {
            for (ikey = values.size() - 1; ikey >= 0; ikey--) {
                if (sf == values[ikey]->String()) {
                    found = true;
                    break;
                }
            }

        }
        else {
            for (ikey = 0; ikey < values.size(); ikey++) {
                if (sf == values[ikey]->String()) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else
        ikey = key->Integer();

    key->Release();
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
        return values[ikey];

    Tamguvector* kvect;
    long iright;
    if (keyright->Type() == a_string) {
        string sf = keyright->String();
        bool found = false;
        if (kind->signright) {
            for (iright = values.size() - 1; iright >= 0; iright--) {
                if (sf == values[iright]->String()) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        else {
            for (iright = 0; iright < values.size(); iright++) {
                if (sf == values[iright]->String()) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        if (!found) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (keyright == aNULL)
            iright = 0;
        else {
            iright = keyright->Integer();
            if (stringkey && iright >= 0)
                iright = ikey + iright + 1;
        }
    }

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
    kvect = globalTamgu->Providevector();
    for (long i = ikey; i < iright; i++)
        kvect->Push(values[i]);

    return kvect;
}

Exporting void Tamguvector::Shuffle() {
    size_t sz = values.size();
    size_t i, f;
    long mx = sz;
    Tamgu* v;
    for (i = 0; i < sz; i++) {
        f = localrandom(mx--);
        if (mx != f) {
            v = values[mx];
            values[mx] = values[f];
            values[f] = v;
        }
    }
}


Exporting Tamgu* Tamguvector::Unique() {
    Locking _lock(this);
    Tamguvector* kvect = globalTamgu->Providevector();
    map<string, Tamgu*> inter;
    string k;
    for (int i = 0; i < values.size(); i++) {
        k = values[i]->String();
        if (inter.find(k) != inter.end()) {
            if (inter[k]->same(values[i])->Boolean() == false)
                kvect->Push(values[i]);
        }
        else {
            inter[k] = values[i];
            kvect->Push(values[i]);
        }
    }
    return kvect;
}


Exporting void Tamguvector::Clear() {
    Locking _lock(this);
    for (int itx = 0; itx < values.size(); itx++)
        values[itx]->Removereference(reference + 1);
    values.clear();
}

Exporting Tamgu* Tamguvector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        Push(ke);
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Doublelocking _lock(this, ke);
    //Three cases:
    if (ke->Type() == idtype) {
        Tamguvector* kvect = (Tamguvector*)ke;
        for (long i = 0; i < kvect->values.size(); i++) {
            ke = kvect->values[i]->Atom();
            ke->Addreference(reference+1);
            values.push_back(ke);
        }
        return this;
    }

    if (ke->isVectorValueContainer()) {
        Tamgu* k;
        for (long i = 0; i < ke->Size(); i++) {
            k = ke->getvalue(i);
            k->Addreference(reference+1);
            values.push_back(k);
        }
        return this;
    }

    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        Push(itr->Value());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamguvector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    Tamguvector* vect = globalTamgu->Providevector();
    long i;
    Tamgu* val;
    
    if (!ke->isContainer()) {
        Locking _lock(this);
        for (i=0; i< values.size();i++) {
            val=new Tamguvector;
            val->Push(values[i]);
            val->Push(ke);
            vect->Push(val);
        }
        return vect;
    }
    
    
    Doublelocking _lock(this, ke);
    i = 0;
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (i == values.size()) {
            val=new Tamguvector;
            val->Push(itr->IteratorValue());
            vect->Push(val);
            continue;
        }
        
        val=new Tamguvector;
        val->Push(values[i]);
        val->Push(itr->IteratorValue());
        vect->Push(val);
        i++;
    }
    itr->Release();
    
    for (;i<values.size();i++) {
        val=new Tamguvector;
        val->Push(values[i]);
        vect->Push(val);
    }
    
    return vect;
}

Exporting Tamgu* Tamguvector::same(Tamgu* a) {
    Doublelocking _lock(this, a);

    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            Tamgu* v;
            for (int i = 0; i < values.size(); i++) {
                v = a->getvalue(i);
                if (!v->same(values[i])->Boolean()) {
                    v->Release();
                    return aFALSE;
                }
                v->Release();
            }
            return aTRUE;
        }
        return aFALSE;
    }

    Tamguvector* v = (Tamguvector*)a;

    if (values.size() != v->values.size())
        return aFALSE;

    for (int i = 0; i < values.size(); i++) {
        if (values[i]->same(v->values[i]) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}

Exporting unsigned long Tamguvector::EditDistance(Tamgu* e) {
    unsigned long s1len, s2len, x, y, lastdiag, olddiag;
    s1len = Size();
    s2len = e->Size();

    Tamgu** v1 = new Tamgu*[s1len + 1];
    Tamgu** v2 = new Tamgu*[s2len + 1];

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x] = getvalue(x);
        if (x < s2len)
            v2[x] = e->getvalue(x);
    }

    size_t* column = new size_t[s1len + 1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;

    unsigned long ed;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
            olddiag = column[y];
            ed = v1[y - 1]->EditDistance(v2[x - 1]);
            column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + ed);
            lastdiag = olddiag;
        }
    }

    y = max(s1len, s2len);
    for (x = 0; x < y; x++) {
        if (x < s1len)
            v1[x]->Release();
        if (x < s2len)
            v2[x]->Release();
    }

    delete[] v1;
    delete[] v2;

    s2len = column[s1len];
    delete[] column;
    return s2len;

}

//---------------------------------------------------------------------------------------

Exporting bool TamguConstvector::Checkvariable() {
    if (values.size() == 0)
        return false;

    for (size_t i = 0; i < values.size(); i++) {
        if (!values[i]->isCallVariable())
            return false;
    }

    return true;
}

Exporting bool TamguConstvector::Setvalue(Tamgu* index, Tamgu* value, short idthread, bool strict) {
    Locking _lock(this);
    Tamgu* a;
    Tamgu* v;
    for (size_t i = 0; i < values.size(); i++) {
        if (i >= value->Size()) {
            globalTamgu->Returnerror("Out of range affectation.", idthread);
            return false;
        }

        v = value->getvalue(i);
        a = values[i];
        a->Setaffectation(true);
        a = a->Get(aNULL, aNULL, idthread);
        a->Putvalue(v, idthread);
        v->Release();
    }

    return true;
}

Exporting Tamgu* TamguConstvector::same(Tamgu* value) {
    if (values.size() == 0) {
        if (value->Size() == 0)
            return aTRUE;
        return aFALSE;
    }

    size_t sz = value->Size();
    if (!isMerge()) {
        if (!values.back()->isMerge()) {
            if (values.size() != sz)
                return aFALSE;
        }
        else {
            if (!sz)
                return aFALSE;
        }
    }

    short idthread = globalTamgu->GetThreadid();
    Tamgu* a;
    Tamgu* v;
    bool clean = true;
    for (size_t i = 0; i < values.size(); i++) {
        a = values[i];

        if (a->isMerge()) {
            TamguIndex idx(true);
            idx.left = globalTamgu->Provideint(i);
            idx.right = aNULL;

            v = value->Get(aNULL, &idx, idthread);
            a = a->same(v);
            v->Release();
            return a;
        }

        if (isMerge()) {
            v = value;
            clean = false;
        }
        else {
            clean = true;
            if (i >= sz)
                return aFALSE;
            v = value->getvalue(i);
        }

        if (a->isCallVariable()) {
            a->Setaffectation(true);
            a = a->Get(aNULL, aNULL, idthread);
            a->Putvalue(v, idthread);
            if (clean)
                v->Release();
            continue;
        }

        //a comes from the const vector, while v comes from the value to match the const vector with...
        if (a->isVectorContainer()) {
            if (!v->isVectorContainer()) {
                if (clean)
                    v->Release();
                return globalTamgu->Returnerror("Mismatch assignment, expecting two vector containers.", idthread);
            }

            a->Putvalue(v, idthread);
            if (clean)
                v->Release();
            if (a->isError())
                return a;

            continue;
        }

        if (a->isMapContainer()) {
            if (!v->isMapContainer()) {
                if (clean)
                    v->Release();
                return globalTamgu->Returnerror("Mismatch assignment, expecting two map containers.", idthread);
            }

            a->Putvalue(v, idthread);
            if (clean)
                v->Release();
            if (a->isError())
                return a;
            continue;
        }

        a = a->Get(aNULL, aNULL, idthread);
        if (a != aNOELEMENT && a->same(v) == aFALSE) {
            if (clean)
                v->Release();
            return aFALSE;
        }

        if (clean)
            v->Release();
    }
    return aTRUE;
}

Exporting void TamguConstvector::Prepare(Tamgu* env, short idthread) {
    evaluate = true;
    Tamgu* v;
    for (size_t i = 0; i < values.size(); i++) {
        v = values[i];
        if (v->isCallVariable())
            v->Put(env, aNULL, idthread);
        else
            if (v->isVariable())
                v->Get(env, aNULL, idthread);
            else
                v->Prepare(env, idthread);
    }
}

Exporting Tamgu* TamguConstvector::Put(Tamgu* index, Tamgu* value, short idthread) {
    if (!evaluate)
        return this;

    Locking _lock(this);
    if (values.size() == 0) {
        if (value->Size() == 0)
            return aTRUE;
        if (index->isError())
            return index;
        return globalTamgu->Returnerror("Size mismatch.", idthread);
    }

    size_t sz = value->Size();
    if (!isMerge()) {
        if (!values.back()->isMerge()) {
            if (values.size() != sz) {
                if (index->isError())
                    return index;
                return globalTamgu->Returnerror("Size mismatch.", idthread);
            }
        }
        else {
            if (!sz) {
                if (index->isError())
                    return index;
                return globalTamgu->Returnerror("Size mismatch.", idthread);
            }
        }
    }

    Tamgu* a;
    Tamgu* v;
    bool clean = true;
    for (size_t i = 0; i < values.size(); i++) {
        a = values[i];
        if (a->isMerge()) {
            a->Setevaluate(true);
            TamguIndex idx(true);
            idx.left = globalTamgu->Provideint(i);
            idx.right = aNULL;

            v = value->Get(aNULL, &idx, idthread);
            return a->Put(index, v, idthread);
        }


        if (isMerge()) {
            v = value;
            clean = false;
        }
        else {
            clean = true;
            if (i >= sz) {
                if (index->isError())
                    return index;
                return globalTamgu->Returnerror("Out of range affectation.", idthread);
            }
            v = value->getvalue(i);
        }

        if (a->isCallVariable()) {
            a->Setaffectation(true);
            a = a->Get(aNULL, aNULL, idthread);
            a->Putvalue(v, idthread);
            if (clean)
                v->Release();
            continue;
        }

        if (a->isVariable()) {
            //Then it should have created before hand...
            a = globalTamgu->Getdeclaration(a->Name(), idthread);
            a->Putvalue(v, idthread);
            if (clean)
                v->Release();
            continue;
        }

        //a comes from the const vector, while v comes from the value to match the const vector with...
        if (a->isVectorContainer()) {
            if (!v->isVectorContainer()) {
                if (clean)
                    v->Release();
                if (index->isError())
                    return index;
                return globalTamgu->Returnerror("Mismatch assignment, expecting two vector containers.", idthread);
            }
            a = a->Put(index, v, idthread);
            if (clean)
                v->Release();
            if (a->isError())
                return a;

            continue;
        }

        if (a->isMapContainer()) {
            if (!v->isMapContainer()) {
                if (clean)
                    v->Release();
                if (index->isError())
                    return index;
                return globalTamgu->Returnerror("Mismatch assignment, expecting two map containers.", idthread);
            }
            a = a->Put(index, v, idthread);
            if (clean)
                v->Release();

            if (a->isError())
                return a;
            continue;
        }

        a = a->Get(aNULL, aNULL, idthread);
        if (a->same(v) == aFALSE) {
            if (clean)
                v->Release();
            if (index->isError())
                return index;
            return globalTamgu->Returnerror("Mismatch assignment.", idthread);
        }

        if (clean)
            v->Release();
    }
    return this;
}

Exporting Tamgu* TamguConstvector::Get(Tamgu* index, Tamgu* value, short idthread) {


    if (affectation && evaluate)
        return this;

    Tamguvector* kvect = globalTamgu->Providevector();

    Locking _lock(this);

    BLONG it;
    BLONG sz = values.size();
    if (isEvaluate()) {
        Tamgu* v;
        for (it = 0; it < sz; it++) {
            v = values[it]->Get(aNULL, aNULL, idthread);
            kvect->Push(v);
            v->Release();
        }
    }
    else {
        for (it = 0; it < sz; it++)
            kvect->Push(values[it]);
    }
    return kvect;
}

Exporting string TamguConstvectormerge::String() {
    Locking _lock(this);
    string res("|");
    int it;
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < values.size(); it++) {
        element = values[it];
        if (beg == false)
            res += ",";
        beg = false;
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer())
            res += sx;
        else
            stringing(res, sx);
    }
    return res;
}

class Comp {
    public:
    TamguCallFunction compare;
    short idthread;

    Comp(TamguFunction* comp, short idt) : compare(comp), idthread(idt) {
        compare.arguments.push_back(aNULL);
        compare.arguments.push_back(aNULL);
    }

    bool get(Tamgu* i, Tamgu* j) {
        compare.arguments.vecteur[0] = i;
        compare.arguments.vecteur[1] = j;
        return compare.Get(aNULL, aNULL, idthread)->Boolean();
    }
};

class Comparison {
    public:
    Comp* compare;

    Comparison(Comp* c)  {
        compare = c;
    }

    bool failcheck(Tamgu* i) {
        return compare->get(i,i);
    }
    
    bool operator() (Tamgu* i, Tamgu* j) {
        return compare->get(i, j);
    }
};

class Comparisonparam {
public:
    TamguParameterFunction* compare;
    short idthread;
    bool direction;
    
    Comparisonparam(TamguParameterFunction* c, short id, bool d)  {
        compare = c;
        direction = d;
        idthread = id;
    }
    
    bool operator() (Tamgu* i, Tamgu* j) {
        i = compare->Get(aNULL, i, idthread);
        j = compare->Get(aNULL, j, idthread);
        if (direction) {
            if (i->Integer() > j ->Integer()) {
                i->Releasenonconst();
                j->Releasenonconst();
                return true;
            }
            i->Releasenonconst();
            j->Releasenonconst();
            return false;
        }
        
        if (i->Integer() < j ->Integer()) {
            i->Releasenonconst();
            j->Releasenonconst();
            return true;
        }
        i->Releasenonconst();
        j->Releasenonconst();
        return false;
    }
    
};


class Comparisontaskell {
public:
    TamguParameterFunction* compare;
    TamguCallFunction* args;
    short idthread;
    bool direction;
    
    Comparisontaskell(TamguParameterFunction* c, TamguCallFunction* a, short id, bool d)  {
        args = a;
        compare = c;
        direction = d;
        idthread = id;
    }
    
    bool failcheck(Tamgu* i) {
        args->arguments.push_back(i);
        args->arguments.push_back(i);
        return compare->Execute(aNULL, args, idthread)->Boolean();
    }
    
    bool operator() (Tamgu* i, Tamgu* j) {
        args->arguments.vecteur[0] = i;
        args->arguments.vecteur[1] = j;
        return compare->Execute(aNULL, args, idthread)->Boolean();
    }
    
};

class Comparisonfunction {
public:
    TamguCallFunction* compare;
    short idthread;
    bool direction;
    
    Comparisonfunction(TamguFunction* c, TamguCallFunction* C, short id, bool d) {
        compare = C;
        idthread = id;
        direction = d;
        compare->arguments.push_back(aNULL);
    }
    
    bool operator() (Tamgu* i, Tamgu* j) {
        compare->arguments.vecteur[0] = i;
        i = compare->Get(aNULL, aNULL, idthread);
        compare->arguments.vecteur[0] = j;
        j = compare->Get(aNULL, aNULL, idthread);
        if (direction) {
            if (i->Integer() > j ->Integer()) {
                i->Releasenonconst();
                j->Releasenonconst();
                return true;
            }
            i->Releasenonconst();
            j->Releasenonconst();
            return false;
        }
        
        if (i->Integer() < j ->Integer()) {
            i->Releasenonconst();
            j->Releasenonconst();
            return true;
        }
        i->Releasenonconst();
        j->Releasenonconst();
        return false;
    }
    
};

static bool CompareInteger(Tamgu* e1, Tamgu* e2) {
    if (e1->Integer() < e2->Integer())
        return true;
    return false;
}

static bool CompareShort(Tamgu* e1, Tamgu* e2) {
    if (e1->Short() < e2->Short())
        return true;
    return false;
}

static bool CompareLong(Tamgu* e1, Tamgu* e2) {
    if (e1->Long() < e2->Long())
        return true;
    return false;
}

static bool CompareFloat(Tamgu* e1, Tamgu* e2) {
    if (e1->Float() < e2->Float())
        return true;
    return false;
}

static bool CompareDecimal(Tamgu* e1, Tamgu* e2) {
    if (e1->Decimal() < e2->Decimal())
        return true;
    return false;
}

static bool CompareString(Tamgu* e1, Tamgu* e2) {
    if (e1->String() < e2->String())
        return true;
    return false;
}


static bool CompareUString(Tamgu* e1, Tamgu* e2) {
    if (e1->UString() < e2->UString())
        return true;
    return false;
}


static bool InvCompareInteger(Tamgu* e1, Tamgu* e2) {
    if (e1->Integer()>e2->Integer())
        return true;
    return false;
}

static bool InvCompareShort(Tamgu* e1, Tamgu* e2) {
    if (e1->Short() > e2->Short())
        return true;
    return false;
}

static bool InvCompareLong(Tamgu* e1, Tamgu* e2) {
    if (e1->Long() > e2->Long())
        return true;
    return false;
}

static bool InvCompareDecimal(Tamgu* e1, Tamgu* e2) {
    if (e1->Decimal() > e2->Decimal())
        return true;
    return false;
}

static bool InvCompareFloat(Tamgu* e1, Tamgu* e2) {
    if (e1->Float() > e2->Float())
        return true;
    return false;
}

static bool InvCompareString(Tamgu* e1, Tamgu* e2) {
    if (e1->String() > e2->String())
        return true;
    return false;
}

static bool InvCompareUString(Tamgu* e1, Tamgu* e2) {
    if (e1->UString() > e2->UString())
        return true;
    return false;
}

Exporting Tamgu* Tamguvector::Sort(Tamgu* comp, short idthread, bool d) {
    //First parameter should be a vector
    if (Size() <= 1)
        return this;

    Locking _lock(this);

    if (comp != NULL) {
        if (comp->Size() == 2) {
            if (comp->isFunctionParameter()) {
                if (!comp->isTaskellFunction())
                    return globalTamgu->Returnerror("Wrong function call: expecting a 'taskell function'", idthread);

                TamguCallFunction cmp(NULL);
                Comparisontaskell kfcomp((TamguParameterFunction*)comp, &cmp, idthread, d);
                if (kfcomp.failcheck(values[0]))
                    return globalTamgu->Returnerror("The comparison function does not satisfy the strict weak ordering rule", idthread);

                sort(values.begin(), values.end(), kfcomp);
                return this;
            }
            
            Comp kcomp((TamguFunction*)comp, idthread);
            Comparison kfcomp(&kcomp);
            if (kfcomp.failcheck(values[0]))
                return globalTamgu->Returnerror("The comparison function does not satisfy the strict weak ordering rule", idthread);
            
            sort(values.begin(), values.end(), kfcomp);
            return this;
        }
        
        if (comp->isFunctionParameter()) {
            Comparisonparam kfcomp((TamguParameterFunction*)comp, idthread, d);
            sort(values.begin(), values.end(), kfcomp);
            return this;
        }
        
        TamguCallFunction cmp(comp);
        Comparisonfunction kfcomp((TamguFunction*)comp, &cmp, idthread, d);
        sort(values.begin(), values.end(), kfcomp);
        return this;
    }

    //we use the first element as a trigger
    Tamgu* kfirst = values[0];
    if (!d) {
        switch (kfirst->Type()) {
            case a_short:
            sort(values.begin(), values.end(), CompareShort);
            break;
            case a_int:
            sort(values.begin(), values.end(), CompareInteger);
            break;
            case a_long:
            sort(values.begin(), values.end(), CompareLong);
            break;
            case a_decimal:
            sort(values.begin(), values.end(), CompareDecimal);
            break;
            case a_float:
            sort(values.begin(), values.end(), CompareFloat);
            break;
            case a_string: //else we only use strings for other types
            sort(values.begin(), values.end(), CompareString);
            break;
            case a_ustring: //else we only use strings for other types
            sort(values.begin(), values.end(), CompareUString);
        }
        return this;
    }

    switch (kfirst->Type()) {
        case a_short:
        sort(values.begin(), values.end(), InvCompareShort);
        break;
        case a_int:
        sort(values.begin(), values.end(), InvCompareInteger);
        break;
        case a_long:
        sort(values.begin(), values.end(), InvCompareLong);
        break;
        case a_decimal:
        sort(values.begin(), values.end(), InvCompareDecimal);
        break;
        case a_float:
        sort(values.begin(), values.end(), InvCompareFloat);
        break;
        case a_string: //else we only use strings for other types
        sort(values.begin(), values.end(), InvCompareString);
        break;
        case a_ustring: //else we only use strings for other types
        sort(values.begin(), values.end(), InvCompareUString);
    }

    return this;
}

Exporting Tamgu* Tamguvector::Sortint(short idthread, bool d) {
    //First parameter should be a vector
    if (Size() <= 1)
        return this;

    Locking _lock(this);

    if (d)
        sort(values.begin(), values.end(), InvCompareInteger);
    else
        sort(values.begin(), values.end(), CompareInteger);
    return this;
}

Exporting Tamgu* Tamguvector::Sortstring(short idthread, bool d) {
    //First parameter should be a vector
    if (Size() <= 1)
        return this;

    Locking _lock(this);

    if (d)
        sort(values.begin(), values.end(), InvCompareString);
    else
        sort(values.begin(), values.end(), CompareString);
    return this;
}

Exporting Tamgu* Tamguvector::Sortustring(short idthread, bool d) {
    //First parameter should be a vector
    if (Size() <= 1)
        return this;

    Locking _lock(this);

    if (d)
        sort(values.begin(), values.end(), InvCompareUString);
    else
        sort(values.begin(), values.end(), CompareUString);
    return this;
}

Exporting Tamgu* Tamguvector::Sortfloat(short idthread, bool d) {
    //First parameter should be a vector
    if (Size() <= 1)
        return this;

    Locking _lock(this);

    if (d)
        sort(values.begin(), values.end(), InvCompareFloat);
    else
        sort(values.begin(), values.end(), CompareFloat);
    return this;
}


Exporting Tamgu* TamguCycleVector::Get(Tamgu* context, Tamgu* v, short idthread) {
    value = base->Get(context, aNULL, idthread);
    value->Setreference();
    return this;
}

Exporting Tamgu* TamguCycleVector::Newinstance(short idthread, Tamgu* f) {
    if (!repeat)
        return value->Newinstance(idthread);

    //Depends on the internal type of value...
    switch (value->Type()) {
        case a_int:
        return globalTamgu->Provideivector();
        case a_float:
        return globalTamgu->Providefvector();
        case a_string:
        return globalTamgu->Providesvector();
        case a_ustring:
        return globalTamgu->Provideuvector();
    }
    return globalTamgu->Providevector();
}

Exporting TamguIteration* TamguCycleVector::Newiteration(bool rev) {
    if (repeat)
        return new TamguIteratorCycleElement(this);

    return new TamguIteratorCycleVector(this);
}

Exporting Tamgu* TamguReplicateVector::Get(Tamgu* context, Tamgu* v, short idthread) {
    value = base->Get(context, aNULL, idthread);
    nb = nbbase->Get(context, aNULL, idthread);
    value->Setreference();
    nb->Setreference();
    return this;
}

Exporting Tamgu* TamguReplicateVector::Newinstance(short idthread, Tamgu* f) {
    //Depends on the internal type of value...
    switch (value->Type()) {
        case a_int:
        return globalTamgu->Provideivector();
        case a_float:
        return globalTamgu->Providefvector();
        case a_string:
        return globalTamgu->Providesvector();
        case a_ustring:
        return globalTamgu->Provideuvector();
    }
    return globalTamgu->Providevector();
}

Exporting TamguIteration* TamguReplicateVector::Newiteration(bool rev) {
    TamguIteratorReplicate* iter = new TamguIteratorReplicate(this);
    return iter;
}


Exporting Tamgu* Tamguvector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {

    Tamgu* a;
    uchar addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    for (size_t i = 0; i < values.size(); i++) {
        recipient->Putvalue(values[i], idthread);

        a = bd->DirectGet(environment, aNULL, idthread);
        if (a->isNULL())
            continue;

        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;

            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }

        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }

    recipient->Forcedclean();
    return context;
}

Exporting Tamgu* Tamguvector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

    Tamgu* returnval;

    bool first = false;
    Tamgu* key;

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
        key = values[i];
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectGet(env, aNULL, idthread);

            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }

            if (globalTamgu->Error(idthread)) {
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

    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

//----------------------------------------------------------------------------------
Exporting void Tamguvector::Storevalue(string& u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::Storevalue(wstring& u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(string u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(long u) {
    Tamgu* a = globalTamgu->Provideint(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(BLONG u) {
    Tamgu* a = new Tamgulong(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}

Exporting void Tamguvector::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Provideustring(w);
    a->Addreference(reference+1);
    Locking* _lock = _getlock(this);
    values.push_back(a);
    _cleanlock(_lock);
}


Exporting Tamgu* Tamguvector::Thesum(long i, long j) {
    long sz = values.size();
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

    if (j >= sz)
        j = sz - 1;

    if (values[0]->isContainer()) {
        Tamgufvector* sub = globalTamgu->Providefvector();
        Tamgu* res;
        for (; i <= j; i++) {
            res = values[i]->Thesum(0, (long)values[i]->Size());
            sub->values.push_back(res->Float());
            res->Release();
        }
        return sub;
    }

    if (values[0]->isNumber()) {
        double v = 0;
        for (; i <= j; i++)
            v += values[i]->Float();

        return globalTamgu->Providefloat(v);
    }

    string v;
    for (; i < j; i++)
        v += values[i]->String();
    return globalTamgu->Providestring(v);
}

Exporting Tamgu* Tamguvector::Theproduct(long i, long j) {
    long sz = values.size();
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

    if (j >= sz)
        j = sz - 1;

    if (values[0]->isContainer()) {
        Tamgufvector* sub = globalTamgu->Providefvector();
        Tamgu* res;
        for (; i <= j; i++) {
            res = values[i]->Theproduct(0, (long)values[i]->Size());
            sub->values.push_back(res->Float());
            res->Release();
        }
        return sub;
    }

    double v = 1;
    for (; i <= j; i++)
        v *= values[i]->Float();

    return globalTamgu->Providefloat(v);
}

//----------------------------------------------------------------------------------------------------------

    //We need to declare once again our local definitions.
Exporting basebin_hash<a_vectorMethod>  Tamgua_vector::methods;
Exporting hmap<string, string> Tamgua_vector::infomethods;
Exporting bin_hash<unsigned long> Tamgua_vector::exported;

Exporting short Tamgua_vector::idtype = 0;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_vector::AddMethod(TamguGlobal* global, string name, a_vectorMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_vector::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgua_vector::idtype = global->Getid("a_vector");
    
    Tamgua_vector::AddMethod(global, "clear", &Tamgua_vector::MethodClear, P_NONE, "clear(): clear the container.");
    Tamgua_vector::AddMethod(global, "flatten", &Tamgua_vector::MethodFlatten, P_NONE, "flatten(): flatten a vector structure.");
    
    Tamgua_vector::AddMethod(global, "remove", &Tamgua_vector::MethodRemove, P_ONE, "remove(e): remove 'e' from the vector.");
    Tamgua_vector::AddMethod(global, "ngrams", &Tamgua_vector::MethodNGrams, P_ONE|P_TWO, "ngrams(int nb, int sep): produces a ngrams svector.");
    
    Tamgua_vector::AddMethod(global, "reverse", &Tamgua_vector::MethodReverse, P_NONE, "reverse(): reverse a vector.");
    Tamgua_vector::AddMethod(global, "unique", &Tamgua_vector::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamgua_vector::AddMethod(global, "join", &Tamgua_vector::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgua_vector::AddMethod(global, "last", &Tamgua_vector::MethodLast, P_NONE, "last(): return the last element.");
    Tamgua_vector::AddMethod(global, "sum", &Tamgua_vector::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_vector::AddMethod(global, "product", &Tamgua_vector::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_vector::AddMethod(global, "push", &Tamgua_vector::MethodPush, P_ATLEASTONE, "push(v): Push a value into the vector.");
    Tamgua_vector::AddMethod(global, "pop", &Tamgua_vector::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the vector");
    Tamgua_vector::AddMethod(global, "poplast", &Tamgua_vector::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgua_vector::AddMethod(global, "merge", &Tamgua_vector::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    Tamgua_vector::AddMethod(global, "insert", &Tamgua_vector::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");

    global->newInstance[Tamgua_vector::idtype] = new Tamgua_vector(global);
    global->RecordMethods(Tamgua_vector::idtype, Tamgua_vector::exported);
    global->newInstance[a_constvector] = new TamguConstvector(global);
    global->RecordMethods(a_constvector, Tamgua_vector::exported);
    
    return true;
}

Exporting TamguIteration* Tamgua_vector::Newiteration(bool direction) {
    return new TamguIterationa_vector(this, direction);
}

Exporting void Tamgua_vector::Cleanreference(short inc) {
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Removecontainerreference(inc);
        it.next();
    }
}

Exporting void Tamgua_vector::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect=false;
    loopmark=true;

    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Addreference(inc);
        it.next();
    }

    loopmark=false;
}

Exporting void Tamgua_vector::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect=false;
    loopmark=true;

    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Addreference(1);
        it.next();
    }

    loopmark=false;
}


static void resetVector(Tamgua_vector* kvect, short inc) {
    kvect->reference -= inc;
    
    atomic_vector_iterator<Tamgu*> it(kvect->values);
    while (!it.end()) {
        it.second->Removereference(inc);
        it.next();
    }
}

Exporting void Tamgua_vector::Resetreference(short inc) {
    if ((reference + containerreference - inc) > 0)
        resetVector(this, inc);
    else {
        resetVector(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}


Exporting Tamgu* Tamgua_vector::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    atomic_vector_iterator<Tamgu*> it(values);

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        while (!it.end()) {
            if (a->same(it.second) == aTRUE)
                v->values.push_back(it.first);
            it.next();
        }
        return v;
    }
    
    if (context->isNumber()) {
        while (!it.end()) {
            if (a->same(it.second) == aTRUE)
                return globalTamgu->Provideint(it.first);
            it.next();
        }
        return aMINUSONE;
    }
    
    while (!it.end()) {
        if (a->same(it.second) == aTRUE)
            return aTRUE;
        it.next();
    }

    return aFALSE;
}

Exporting Tamgu* Tamgua_vector::Push(Tamgu* a) {
    a = a->Atom();
    a->Addreference(reference+1);
    values.push_back(a);
    return aTRUE;
}

Exporting Tamgu* Tamgua_vector::Push(TamguGlobal* g, Tamgu* a, short idhtread) {
    a = a->Atom();
    a->Addreference(reference+1);
    values.push_back(a);
    return aTRUE;
}

Exporting Tamgu* Tamgua_vector::Pop(Tamgu* idx) {
    BLONG v = idx->Integer();
    
    if (v == -1) {
        idx = values.remove_back();
    }
    else {
        if (v < -1)
            return aFALSE;
        idx = values.remove(v);
    }

    if (idx == NULL)
        return aFALSE;

    idx->Removereference(reference + 1);
    return aTRUE;
}



Exporting string Tamgua_vector::String() {
    if (loopmark)
        return("[...]");
    TamguCircular _c(this);
    string res;
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        element = it.second;
        sx = element->StringToDisplay();
        if (!element->isString() || element->isContainer()) {
            if (sx == "")
                sx = "''";
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
            res += sx;
        }
        else {
            if (beg == false)
                res += ",";
            stringing(res, sx);
        }
        beg = false;
        it.next();
    }
    res += "]";
    return res;
}

Exporting Tamgu* Tamgua_vector::Map(short idthread) {
    if (loopmark)
        return aNULL;
    
    loopmark=true;
    Tamgumap* kmap = globalTamgu->Providemap();
    char ch[20];
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        sprintf_s(ch, 20, "%ld", it.first);
        kmap->Push(ch, it.second);
        it.next();
    }
    loopmark=false;
    return kmap;
}

Exporting string Tamgua_vector::JSonString() {
    if (loopmark)
        return("");
    TamguCircular _c(this);
    string res;

    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        element = it.second;
        sx = element->JSonString();
        if (!element->isString() || element->isContainer()) {
            if (beg == false) {
                if (sx[0] != '|')
                    res += ",";
            }
        }
        else {
            if (beg == false)
                res += ",";
        }
        res += sx;
        beg = false;
        it.next();
    }
    res += "]";
    return res;
}

Exporting long Tamgua_vector::Integer() {
    return values.size();
}

Exporting double Tamgua_vector::Float() {
    return values.size();
}

Exporting BLONG Tamgua_vector::Long() {
    return values.size();
}

Exporting bool Tamgua_vector::Boolean() {
    if (values.size())
        return true;
    return false;
}

    //Basic operations
Exporting long Tamgua_vector::Size() {
    return values.size();
}

Exporting Tamgu* Tamgua_vector::andset(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamgua_vector*)Atom(true);
        
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            it.second->andset(b, true);
            it.next();
        }
        return ref;
    }
    
    ref = new Tamgua_vector();
    TamguIteration* itr = b->Newiteration(false);
    atomic_vector_iterator<Tamgu*> it;
    Tamgu* ke;

    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        it.begin(values);
        while (!it.end()) {
            ke = itr->IteratorValue();
            if (it.second->same(ke) == aTRUE) {
                ref->Push(ke);
                break;
            }
            it.next();
        }
    }
    itr->Release();
    
    return ref;
    
}

Exporting Tamgu* Tamgua_vector::orset(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    
    Tamgu* ke;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    if (!b->isContainer()) {
        
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            ke = it.second->orset(b, true);
            it.next();
        }
        return ref;
    }
    
    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamgua_vector::xorset(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamgua_vector*)Atom(true);
        
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            it.second->xorset(b, true);
            it.next();
        }
        return ref;
    }
    
    ref = new Tamgua_vector();
    bool found=false;
    vector<Tamgu*> vals;
    Tamgu* ke;

    TamguIteration* itr = b->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        vals.push_back(itr->Value());
    itr->Release();
    
    long itv;
    atomic_vector_iterator<Tamgu*> it(values);

    while (!it.end()) {
        ke = it.second;
        found = false;
        for (itv = 0; itv < vals.size(); itv++)  {
            if (vals[itv] == NULL)
                continue;
            
            if (vals[itv]->same(ke) == aTRUE) {
                vals[itv]->Release();
                vals[itv] = NULL;
                found = true;
                break;
            }
        }
        if (!found)
            ref->Push(ke);
        it.next();
    }
    
    for (itv = 0; itv < vals.size(); itv++) {
        if (vals[itv] == NULL)
            continue;
        ref->Push(vals[itv]);
        vals[itv]->Release();
    }
    
    return ref;
}


Exporting Tamgu* Tamgua_vector::plus(Tamgu* b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->plus(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->plus(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_vector::minus(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->minus(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->minus(b, true);
        it.next();
    }
    
    return ref;

}

Exporting Tamgu* Tamgua_vector::multiply(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->multiply(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->multiply(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_vector::divide(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* ke;
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = it.second->divide(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        ke = it.second->divide(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_vector::power(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->power(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->power(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_vector::shiftleft(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->shiftleft(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->shiftright(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_vector::shiftright(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            it.second->shiftright(kv, true);
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        it.second->shiftright(b, true);
        it.next();
    }
    
    return ref;
}

Exporting Tamgu* Tamgua_vector::mod(Tamgu *b, bool itself) {
    Tamgua_vector* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgua_vector*)Atom(true);
    
    Tamgu* ke;
    Tamgu* kv;
    if (loopmark)
        return aNULL;
    
    TamguCircular _c(this);
    
    if (b->isContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        atomic_vector_iterator<Tamgu*> it(ref->values);
        while (!it.end()) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = it.second->mod(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }
            itr->Next();
            it.next();
        }
        itr->Release();
        return ref;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(ref->values);
    while (!it.end()) {
        ke = it.second->mod(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
        it.next();
    }
    
    return ref;
}


Exporting Tamgu* Tamgua_vector::Unique() {
    Tamguvector* kvect = globalTamgu->Providevector();
    map<string, Tamgu*> inter;
    string k;
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        k = it.second->String();
        if (inter.find(k) != inter.end()) {
            if (inter[k]->same(it.second)->Boolean() == false)
                kvect->Push(it.second);
        }
        else {
            inter[k] = it.second;
            kvect->Push(it.second);
        }
        it.next();
    }
    return kvect;
}


Exporting Tamgu*  Tamgua_vector::Put(Tamgu* idx, Tamgu* value, short idthread) {
    if (!idx->isIndex()) {
        if (value == this)
            return aTRUE;
        
        if (value == aNULL) {
            Clear();
            return aTRUE;
        }
        
        if (value->Type() == Tamgua_vector::idtype) {
            //this is one of the rare cases when we need a lock...
            values._lock.lock();
            values = ((Tamgua_vector*)value)->values;
            atomic_vector_iterator<Tamgu*> it(values);
            for (;!it.end();it.next()) {
                if (!it.second->isConst()) {
                    idx = it.second->Atom();
                    it.replace(idx);
                    idx->Setreference(reference+1);
                }
            }
            values._lock.unlock();
            return aTRUE;
        }
        
        if (value->Type() == a_list) {
            Locking* _lock = _getlockif(value);
            
            Tamgulist* kvect = (Tamgulist*)value;
                //We copy all values from value to this
            Clear();
            {
                
                for (auto& it : kvect->values)
                    Push(it);
            }
            
            _cleanlockif(_lock);
            return aTRUE;
        }
        
        long sz = value->Size();
        if (value->isVectorContainer()) {
            Locking* _lock = _getlockif(value);
            

                //We copy all values from value to this
            Clear();
            if (sz) {
                for (int it = 0; it < sz; it++)
                    Push(value->getvalue(it));
            }
            
            _cleanlockif(_lock);
            return aTRUE;
        }
            //We gather all the keys from the MAP
        if (value->isMapContainer()) {
            Clear();
            if (sz) {
                TamguIteration* itr = value->Newiteration(false);
                for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                    Push(itr->Key());
                itr->Release();
            }
            return aTRUE;
        }
        
        value = value->Vector(idthread);
        if (!value->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);
        
        sz = value->Size();
        
        if (value != this) {
            Clear();
            if (sz) {
                //We copy all values from ke to this
                for (long it = 0; it < value->Size(); ++it)
                    Push(value->getvalue(it));
            }
            value->Releasenonconst();
        }
        return aTRUE;
    }

    long ikey = idx->Integer();
    long mx = values.size();
    if (ikey < 0) {
        ikey = mx + ikey;
        if (ikey < 0)
            return globalTamgu->Returnerror("Cannot set this value", idthread);
    }
    value = value->Atom();
    value->Addreference(reference+1);
    value = values.replace(ikey, value);
    if (value != NULL)
        value->Removereference(reference + 1);
    return aTRUE;
}


Exporting Tamgu* Tamgua_vector::Get(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    if (!idx->isIndex()) {
        if (contextualpattern->isLoop())
            return this;

            //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Tamgu* ke;
            Tamgu* map = Selectamap(contextualpattern);
            char ch[20];
            atomic_vector_iterator<Tamgu*> it(values);
            while (!it.end()) {
                sprintf_s(ch, 20, "%ld", it.first);
                ke = it.second;
                map->Push(ch, ke);
                it.next();
            }
            return map;
        }
        
        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }
        return this;
    }
    
    Tamgu* key;
    Tamgu* keyright = NULL;
    
    TamguIndex* kind = (TamguIndex*)idx;
    key = kind->left->Get(aNULL, aNULL, idthread);
    if (kind->interval == true)
        keyright = kind->right->Get(aNULL, aNULL, idthread);
    
    long mx = values.size();
    long ikey = key->Integer();
    key->Release();
    if (ikey < 0)
        ikey = mx + ikey;
    
    if (ikey >= 0)
        key = values[ikey];
    
    if (ikey < 0 || key == NULL) {
        if (ikey != mx || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
    if (keyright == NULL)
        return key;
    
    Tamguvector* kvect;
    long iright;
    
    if (keyright == aNULL)
        iright = 0;
    else
        iright = keyright->Integer();
    
    keyright->Release();
    
    if (iright < 0 || keyright == aNULL) {
        iright = mx + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>mx) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    
        //In this case, we must create a new vector
    kvect = globalTamgu->Providevector();
    
    atomic_vector_iterator<Tamgu*> it(values, ikey);
    while (!it.end() && it.first < iright) {
        kvect->Push(it.second);
        it.next();
    }
    
    return kvect;
}


Exporting void Tamgua_vector::Clear() {
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        it.second->Removereference(reference + 1);
        it.next();
    }
    values.clear();
}


Exporting void Tamgua_vector::Storevalue(string& u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::Storevalue(wstring& u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(string u) {
    Tamgu* a = globalTamgu->Providestring(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(long u) {
    Tamgu* a = globalTamgu->Provideint(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(BLONG u) {
    Tamgu* a = new Tamgulong(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    a->Addreference(reference+1);
    values.push_back(a);
}

Exporting void Tamgua_vector::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Provideustring(w);
    a->Addreference(reference+1);
    values.push_back(a);
}


Exporting Tamgu* Tamgua_vector::Thesum(long i, long j) {
    long sz = values.size();
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
    
    if (j >= sz)
        j = sz - 1;
    
    if (values[0]->isContainer()) {
        Tamgufvector* sub = globalTamgu->Providefvector();
        Tamgu* res;
        atomic_vector_iterator<Tamgu*> it(values, i);
        while (!it.end() && it.first <= j) {
            res = it.second->Thesum(0, (long)it.second->Size());
            sub->values.push_back(res->Float());
            res->Release();
            it.next();
        }
        return sub;
    }
    
    if (values[0]->isNumber()) {
        double v = 0;
        atomic_vector_iterator<Tamgu*> it(values, i);
        while (!it.end() && it.first <= j) {
            v += it.second->Float();
            it.next();
        }
        
        return globalTamgu->Providefloat(v);
    }
    
    string v;
    atomic_vector_iterator<Tamgu*> it(values, i);
    while (!it.end() && it.first < j) {
        v += it.second->String();
        it.next();
    }
    return globalTamgu->Providestring(v);
}

Exporting Tamgu* Tamgua_vector::Theproduct(long i, long j) {
    long sz = values.size();
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
    
    if (j >= sz)
        j = sz - 1;
    
    if (values[0]->isContainer()) {
        Tamgufvector* sub = globalTamgu->Providefvector();
        Tamgu* res;
        atomic_vector_iterator<Tamgu*> it(values, i);
        while (!it.end() && it.first <= j) {
            res = it.second->Theproduct(0, (long)it.second->Size());
            sub->values.push_back(res->Float());
            res->Release();
            it.next();
        }
        return sub;
    }
    
    double v = 1;
    atomic_vector_iterator<Tamgu*> it(values, i);
    while (!it.end() && it.first <= j) {
        v *= it.second->Float();
        it.next();
    }

    return globalTamgu->Providefloat(v);
}

Exporting Tamgu* Tamgua_vector::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {
    
    Tamgu* a;
    uchar addvalue = 0;
    
    if (context != aNOELEMENT)
        addvalue = Selecttype(context);
    
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        recipient->Putvalue(it.second, idthread);
        it.next();

        a = bd->DirectGet(environment, aNULL, idthread);
        if (a->isNULL())
            continue;
        
        if (globalTamgu->Error(idthread) || a->needInvestigate()) {
            if (a == aBREAK)
                break;
            
            recipient->Forcedclean();
            a->Release();
            context->Release();
            return globalTamgu->Errorobject(idthread);
        }
        
        context = Storealongtype(context, a, idthread, addvalue);
        a->Release();
    }
    
    recipient->Forcedclean();
    return context;
}

Exporting Tamgu* Tamgua_vector::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {
    
    Tamgu* returnval;
    
    bool first = false;
    Tamgu* key;
    
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
    
    atomic_vector_iterator<Tamgu*> it(values);
    while (!it.end()) {
        key = it.second;
        it.next();
        if (first) {
            returnval = key->Atom();//We use the first value of the list to seed our accumulator variable
            first = false;
        }
        else {
            var->Putvalue(key, idthread);
            returnval = bd->DirectGet(env, aNULL, idthread);
            
            if (returnval == aBREAK) {
                accu = returnval;
                break;
            }
            
            if (globalTamgu->Error(idthread)) {
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
    
    var->Forcedclean();
    if (kcont != NULL)
        return kcont;
    return accu->Value();
}

Exporting Tamgu* Tamgua_vector::same(Tamgu* a) {
    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != values.size())
                return aFALSE;
            Tamgu* v;
            atomic_vector_iterator<Tamgu*> it(values);
            for (; !it.end(); it.next()) {
                v = a->getvalue(it.first);
                if (!v->same(it.second)->Boolean()) {
                    v->Release();
                    return aFALSE;
                }
                v->Release();
            }
            return aTRUE;
        }
        return aFALSE;
    }
    
    Tamguvector* v = (Tamguvector*)a;
    Locking _lock(v);
    
    if (values.size() != v->values.size())
        return aFALSE;

    atomic_vector_iterator<Tamgu*> it(values);
    for (; !it.end(); it.next()) {
        if (it.second->same(v->values[it.first]) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgua_vector::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        Push(ke);
        return this;
    }
    
    if (ke->Size() == 0)
        return this;
    

    if (ke->isVectorValueContainer()) {
        Locking* _lock = _getlockif(ke);
        Tamgu* k;
        for (long i = 0; i < ke->Size(); i++) {
            k = ke->getvalue(i);
            k->Addreference(reference+1);
            values.push_back(k);
        }
        _cleanlockif(_lock);
        return this;
    }
    
    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        Push(itr->Value());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgua_vector::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    Tamguvector* vect = globalTamgu->Providevector();
    Tamgu* val;
    
    if (!ke->isContainer()) {
        atomic_vector_iterator<Tamgu*> it(values);
        for (; !it.end(); it.next()) {
            val=new Tamguvector;
            val->Push(it.second);
            val->Push(ke);
            vect->Push(val);
        }
        return vect;
    }
    
    
    atomic_vector_iterator<Tamgu*> it(values);
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (it.end()) {
            val=new Tamguvector;
            val->Push(itr->IteratorValue());
            vect->Push(val);
            continue;
        }
        
        val=new Tamguvector;
        val->Push(it.second);
        val->Push(itr->IteratorValue());
        vect->Push(val);
        it.next();
    }
    itr->Release();
    
    for (;!it.end();it.next()) {
        val=new Tamguvector;
        val->Push(it.second);
        vect->Push(val);
    }
    
    return vect;
}

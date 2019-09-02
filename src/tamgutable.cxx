/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutable.cxx
 Date       : 2017/09/01
 Purpose    : vector implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutaskell.h"
#include "tamgutable.h"
#include "tamguvector.h"
#include "tamguivector.h"
#include "tamgumap.h"
#include "tamgulist.h"
#include "instructions.h"
#include "constobjects.h"
#include "tamguuvector.h"
#include "tamgufvector.h"
#include "tamgubyte.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<tableMethod>  Tamgutable::methods;
Exporting hmap<string, string> Tamgutable::infomethods;
Exporting bin_hash<unsigned long> Tamgutable::exported;

Exporting short Tamgutable::idtype = 0;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutable::AddMethod(TamguGlobal* global, string name, tableMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutable::Setidtype(TamguGlobal* global) {
        Tamgutable::idtype = global->Getid("table");
    }

   bool Tamgutable::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgutable::idtype = global->Getid("table");

    Tamgutable::AddMethod(global, "clear", &Tamgutable::MethodClear, P_NONE, "clear(): clear the container.");
    Tamgutable::AddMethod(global, "flatten", &Tamgutable::MethodFlatten, P_NONE, "flatten(): flatten a table structure.");

    Tamgutable::AddMethod(global, "remove", &Tamgutable::MethodRemove, P_ONE, "remove(e): remove 'e' from the table.");

    Tamgutable::AddMethod(global, "reverse", &Tamgutable::MethodReverse, P_NONE, "reverse(): reverse a table.");
    Tamgutable::AddMethod(global, "unique", &Tamgutable::MethodUnique, P_NONE, "unique(): remove duplicate elements.");
    Tamgutable::AddMethod(global, "_initial", &Tamgutable::MethodReserve, P_ONE, "_initial(int sz): Reserve a size of 'sz' potential element in the table.");
    Tamgutable::AddMethod(global, "reserve", &Tamgutable::MethodReserve, P_ONE, "reserve(int sz): Reserve a size of 'sz' potential element in the table.");
    Tamgutable::AddMethod(global, "resize", &Tamgutable::MethodReserve, P_ONE, "resize(int sz): Resize a table.");
    Tamgutable::AddMethod(global, "join", &Tamgutable::MethodJoin, P_ONE, "join(string sep): Produce a string representation for the container.");
    Tamgutable::AddMethod(global, "shuffle", &Tamgutable::MethodShuffle, P_NONE, "shuffle(): shuffle the values in the table.");
    Tamgutable::AddMethod(global, "last", &Tamgutable::MethodLast, P_NONE, "last(): return the last element.");
    Tamgutable::AddMethod(global, "sum", &Tamgutable::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutable::AddMethod(global, "product", &Tamgutable::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutable::AddMethod(global, "push", &Tamgutable::MethodPush, P_ATLEASTONE, "push(v): Push a value into the table.");
    Tamgutable::AddMethod(global, "pop", &Tamgutable::MethodPop, P_NONE | P_ONE, "pop(i): Erase an element from the table");
    Tamgutable::AddMethod(global, "poplast", &Tamgutable::MethodPoplast, P_NONE, "poplast(): remove and return the last element from the vector");
    Tamgutable::AddMethod(global, "merge", &Tamgutable::MethodMerge, P_ONE, "merge(v): Merge v into the table.");
    Tamgutable::AddMethod(global, "editdistance", &Tamgutable::MethodEditDistance, P_ONE, "editdistance(v): Compute the edit distance with table 'v'.");
    Tamgutable::AddMethod(global, "insert", &Tamgutable::MethodInsert, P_TWO, "insert(i,v): Insert v at position i.");

    global->newInstance[Tamgutable::idtype] = new Tamgutable(global);
    global->RecordMethods(Tamgutable::idtype, Tamgutable::exported);

    return true;
}

Exporting TamguIteration* Tamgutable::Newiteration(bool direction) {
    return new TamguIterationtable(this, direction);
}

Exporting void Tamgutable::Insert(long idx, Tamgu* ke) {
    if (idx < 0 || (idx + 1) >= size) {
        globalTamgu->Returnerror("table is full or wrong index", globalTamgu->GetThreadid());
        return;
    }

    ke = ke->Atom();
    ke->Addreference(reference+1);
    if (values[idx] == aNOELEMENT) {
        values[idx] = ke;
        return;
    }

    for (long i = size - 1; i >= idx; i++)
        values[i] = values[i - 1];

    values[idx] = ke;
}


Exporting void Tamgutable::Cleanreference(short inc) {
    for (long i=0;i<size;i++)
        values[i]->Removecontainerreference(inc);
}

Exporting void Tamgutable::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect=false;
    loopmark=true;
    for (long i = 0; i< size; i++)
        values[i]->Addreference(inc);
    loopmark=false;
}

Exporting void Tamgutable::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect=false;
    loopmark=true;
    for (long i = 0; i< size; i++)
        values[i]->Addreference(1);
    loopmark=false;
}


static void resetTable(Tamgutable* kvect, short inc) {
    kvect->reference -= inc;
    if (kvect->size == 0)
        return;

    for (int itx = 0; itx < kvect->size; itx++)
        kvect->values[itx]->Removereference(inc);
}

Exporting void Tamgutable::Resetreference(short inc) {
    if ((reference + containerreference - inc) > 0)
        resetTable(this, inc);
    else {
        resetTable(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}


Exporting Tamgu* Tamgutable::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        for (size_t i = 0; i < size; i++) {
            if (a->same(values[i]) == aTRUE)
                v->values.push_back(i);
        }
        return v;
    }

    if (context->isNumber()) {
        for (size_t i = 0; i < size; i++) {
            if (a->same(values[i]) == aTRUE)
                return globalTamgu->Provideint(i);
        }
        return aMINUSONE;
    }
    

    for (size_t i = 0; i < size; i++) {
        if (a->same(values[i]) == aTRUE)
            return aTRUE;
    }

    return aFALSE;
}

Exporting Tamgu* Tamgutable::Push(Tamgu* a) {
    if (push(a) == aNOELEMENT)
        return globalTamgu->Returnerror("Could not push this value into the table", globalTamgu->GetThreadid());

    return aTRUE;
}

Exporting Tamgu* Tamgutable::Push(TamguGlobal* g, Tamgu* a, short idhtread) {
    if (push(a) == aNOELEMENT)
        return globalTamgu->Returnerror("Could not push this value into the table", globalTamgu->GetThreadid());
    return aTRUE;
}

Exporting Tamgu* Tamgutable::Pop(Tamgu* idx) {
    
    if (!size)
        return aFALSE;

    BLONG v = idx->Integer();

    long i;
    if (v == -1) {
        idx = Last(i);
        if (idx != aNOELEMENT)
            values[i] = aNOELEMENT;
        else
            return aFALSE;
    }
    else {
        if (v < 0 || v >= (BLONG)size)
            return aFALSE;
        idx = values[v];
        values[v] = aNOELEMENT;
    }

    idx->Removereference(reference + 1);
    return aTRUE;
}

Exporting Tamgu* Tamgutable::Poplast() {
    
    if (!size)
        return aFALSE;
    
    long i;
    Tamgu* idx = Last(i);
    if (idx != aNOELEMENT) {
        values[i] = aNOELEMENT;
        idx->Removereference(reference);
        idx->Protect();
    }
    return idx;
}



Exporting string Tamgutable::String() {
    
    string res;
    int it;
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < size; it++) {
        element = values[it];
        sx = element->StringToDisplay();
        if (loopmark)
            return("[...]");
        TamguCircular _c(this);
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

Exporting Tamgu* Tamgutable::Map(short idthread) {
    if (loopmark)
        return aNULL;
    loopmark=true;
    Tamgumap* kmap = globalTamgu->Providemap();
    char ch[20];
    for (int it = 0; it < size; it++) {
        sprintf_s(ch, 20, "%d", it);
        kmap->Push(ch, values[it]);
    }
    loopmark=false;
    return kmap;
}

Exporting string Tamgutable::JSonString() {
    
    string res;
    int it;
    res = "[";
    bool beg = true;
    string sx;
    Tamgu* element;
    for (it = 0; it < size; it++) {
        element = values[it];
        sx = element->JSonString();
        if (loopmark)
            return("");
        TamguCircular _c(this);
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

Exporting long Tamgutable::Integer() {
    
    return size;
}

Exporting double Tamgutable::Float() {
    
    return size;
}

Exporting BLONG Tamgutable::Long() {
    
    return size;
}

Exporting bool Tamgutable::Boolean() {
    if (!size)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgutable::Size() {
    
    return size;
}

Exporting Tamgu* Tamgutable::andset(Tamgu *b, bool itself) {
    Tamgutable* ref;

    long size = Size();
    size_t it;
    Tamgu* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamgutable*)Atom(true);

        

        for (it = 0; it < size; it++) {
            ke = ref->values[it]->andset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    ref = new Tamgutable(b->Size());
    Locking((TamguObject*)b);

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

Exporting Tamgu* Tamgutable::orset(Tamgu *b, bool itself) {
    Tamgutable* ref;

    long size = Size();
    size_t it;
    Tamgu* ke;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    if (!b->isContainer()) {
        

        for (it = 0; it < size; it++) {
            ke = ref->values[it]->orset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    ref->Merging(b);
    return ref;
}

Exporting Tamgu* Tamgutable::xorset(Tamgu *b, bool itself) {
    Tamgutable* ref;

    long size = Size();
    size_t it;
    Tamgu* ke;
    if (!b->isContainer()) {
        if (itself)
            ref = this;
        else
            ref = (Tamgutable*)Atom(true);

        for (it = 0; it < size; it++) {
            ke = ref->values[it]->xorset(b, true);
            if (ke->isError()) {
                ref->Release();
                return ke;
            }
        }
        return ref;
    }

    ref = new Tamgutable(b->Size());
    Locking((TamguObject*)b);

    TamguIteration* itr = b->Newiteration(false);
    bool found;
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        found = false;
        for (it = 0; it < size; it++) {
            ke = itr->IteratorValue();
            if (values[it]->same(ke) == aTRUE) {
                found = true;
                break;
            }
        }

        if (!found) {
            ke = itr->IteratorValue();
            ref->Push(ke);
        }
    }
    itr->Release();
    return ref;
}


Exporting Tamgu* Tamgutable::plus(Tamgu* b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;
    if (loopmark)
        return aNULL;

    TamguCircular _c(this);

    
    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->plus(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->plus(b, true);
        if (ke->isError()) {
            ref->Release();
           return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgutable::minus(Tamgu *b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    
    

    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->minus(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->minus(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgutable::multiply(Tamgu *b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    
    

    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->multiply(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->multiply(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgutable::divide(Tamgu *b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    
    

    if (b->isContainer()) {
        Locking _lock(b);
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
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->divide(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgutable::power(Tamgu *b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    
    

    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->power(kv, true);
            if (ke->isError()) {
                ref->Release();
                itr->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->power(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgutable::shiftleft(Tamgu *b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    
    

    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->shiftleft(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->shiftleft(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgutable::shiftright(Tamgu *b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* kv;

    Tamgu* ke;
    
    

    if (b->isContainer()) {
        Locking _lock(b);
        TamguIteration* itr = b->Newiteration(false);
        itr->Begin();
        for (it = 0; it < size; it++) {
            if (itr->End() == aTRUE)
                break;
            kv = itr->IteratorValue();
            ke = ref->values[it]->shiftright(kv, true);
            if (ke->isError()) {
                itr->Release();
                ref->Release();
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->shiftright(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu* Tamgutable::mod(Tamgu *b, bool itself) {
    Tamgutable* ref;
    if (itself)
        ref = this;
    else
        ref = (Tamgutable*)Atom(true);

    int it;
    Tamgu* ke;
    Tamgu* kv;

    if (b->isContainer()) {
        Locking _lock(b);
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
                return ke;
            }

            itr->Next();
        }
        itr->Release();
        return ref;
    }


    for (it = 0; it < size; it++) {
        ke = ref->values[it]->mod(b, true);
        if (ke->isError()) {
            ref->Release();
            return ke;
        }
    }

    return ref;
}

Exporting Tamgu*  Tamgutable::Put(Tamgu* idx, Tamgu* value, short idthread) {
    if (!idx->isIndex()) {
        if (value == this)
            return aTRUE;

        if (value == aNULL) {
            Clear();
            return aTRUE;
        }

        if (value->Type() == a_list) {
            Tamgulist* kvect = (Tamgulist*)value;
            Locking _lock(kvect);
            //We copy all values from value to this
            Clear();
            Resize(value->Size());
            {
                long i = 0;
                for (auto& it : kvect->values)
                    Pushing(it, i++);
            }
            return aTRUE;
        }
        if (value->isVectorContainer()) {
            //We copy all values from value to this
            Clear();
            long sz = value->Size();
            Resize(sz);
            for (long it = 0; it < sz; it++)
                Pushing(value->getvalue(it), it);
            return aTRUE;
        }
        //We gather all the keys from the MAP
        if (value->isMapContainer()) {
            Locking _lock(value);
            Clear();
            Resize(value->Size());
            long i = 0;
            TamguIteration* itr = value->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                if (!Pushing(itr->Key(), i++))
                    break;
            }
            itr->Release();
            return aTRUE;
        }
        
        value = value->Vector(idthread);
        if (!value->isVectorContainer())
            return globalTamgu->Returnerror("Cannot set this value", idthread);

        Clear();
        Resize(value->Size());
        //We copy all values from ke to this
        for (long it = 0; it < value->Size(); ++it) {
            if (!Pushing(value->getvalue(it), it)) {
                value->Releasenonconst();
                return globalTamgu->Returnerror("Maximum size of table reached", idthread);
            }
        }
        value->Releasenonconst();
        return aTRUE;
    }
    
    //In this specific case, we try to replace a bloc of values with a new bloc
    if (idx->isInterval()) {
        //First we remove the values...
        long lkey = idx->Integer();
        if (lkey < 0)
            lkey = size + lkey;
        long rkey = ((TamguIndex*)idx)->right->Integer();
        if (rkey < 0)
            rkey = size + rkey;
        if (rkey < lkey || rkey >= size || lkey >= size) {
            if (globalTamgu->erroronkey)
                globalTamgu->Returnerror("Wrong index", idthread);
            return aTRUE;
        }
        if (rkey != lkey)
            rkey--;
        Tamgu* krkey = aZERO;
        while (rkey >= lkey) {
            krkey = values[rkey];
            values[rkey] = aNOELEMENT;
            krkey->Removereference(reference + 1);
            rkey--;
        }
        if (value->isVectorContainer()) {
            for (long i = value->Size() - 1; i >= 0; i--) {
                krkey = value->getvalue(i);
                Insert(lkey, krkey);
            }
            return aTRUE;
        }
        if (value->isContainer()) {
            TamguIteration* itr = value->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                krkey = itr->Value();
                Insert(lkey, krkey);
            }
            itr->Release();
            return aTRUE;
        }

        Insert(lkey, krkey);
        return aTRUE;
    }
    long ikey = idx->Integer();
    if (ikey >= size)
        return globalTamgu->Returnerror("Table is full", idthread);

    if (ikey < 0) {
        ikey = size + ikey;
        if (ikey < 0)
            return globalTamgu->Returnerror("Cannot set this value", idthread);
    }
    value = value->Atom();
    if (values[ikey] != aNOELEMENT)
        values[ikey]->Removereference(reference + 1);
    values[ikey] = value;
    value->Addreference(reference+1);

    return aTRUE;
}


Exporting Tamgu* Tamgutable::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    

    if (!idx->isIndex()) {

        Tamgu* ke;
        //In this case, we copy the elements from the vector to the map, using the position as index
        if (contextualpattern->isMapContainer()) {
            Tamgu* map = Selectamap(contextualpattern);
            size_t i = 0;
            char ch[20];
            for (int it = 0; it < size; it++) {
                sprintf_s(ch, 20, "%ld", i);
                ke = values[it];
                map->Push(ch, ke);
                i++;
            }
            return map;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalTamgu->Provideint(size);
        return this;
    }

    Tamgu* key;
    Tamgu* keyright = NULL;

    TamguIndex* kind = (TamguIndex*)idx;
    key = kind->left->Eval(aNULL, aNULL, idthread);
    if (kind->interval == true)
        keyright = kind->right->Eval(aNULL, aNULL, idthread);

    long ikey;
    bool stringkey = false;
    if (key->Type() == a_string) {
        string sf = key->String();
        stringkey = true;
        bool found = false;
        if (kind->signleft) {
            for (ikey = size - 1; ikey >= 0; ikey--) {
                if (sf == values[ikey]->String()) {
                    found = true;
                    break;
                }
            }

        }
        else {
            for (ikey = 0; ikey < size; ikey++) {
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
        ikey = size + ikey;

    if (ikey < 0 || ikey >= size) {
        if (ikey != size || keyright == NULL) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    if (keyright == NULL)
        return values[ikey];

    Tamgutable* kvect;
    long iright;
    if (keyright->Type() == a_string) {
        string sf = keyright->String();
        bool found = false;
        if (kind->signright) {
            for (iright = size - 1; iright >= 0; iright--) {
                if (sf == values[iright]->String()) {
                    found = true;
                    iright++;
                    break;
                }
            }
        }
        else {
            for (iright = 0; iright < size; iright++) {
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
        iright = size + iright;
        if (iright<ikey) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }
    else {
        if (iright>size) {
            if (globalTamgu->erroronkey)
                return globalTamgu->Returnerror("Wrong index", idthread);
            return aNOELEMENT;
        }
    }

    //In this case, we must create a new vector
    kvect = new Tamgutable(size);
    for (long i = ikey; i < iright; i++)
        kvect->values[i] = values[i];

    return kvect;
}

Exporting void Tamgutable::Shuffle() {
    size_t sz = size;
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


Exporting Tamgu* Tamgutable::Unique() {
    
    Tamgutable* kvect = new Tamgutable(size);
    map<string, Tamgu*> inter;
    string k;
    for (int i = 0; i < size; i++) {
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


Exporting void Tamgutable::Clear() {
    
    for (int itx = 0; itx < size; itx++) {
        values[itx]->Removereference(reference + 1);
        values[itx] = aNOELEMENT;
    }
}

Exporting Tamgu* Tamgutable::Merging(Tamgu* ke) {
    if (!ke->isContainer()) {
        Push(ke);
        return this;
    }

    if (ke->Size() == 0)
        return this;

    Locking _lock((TamguObject*)ke);
    //Two cases:
    if (ke->Type() == idtype) {
        Tamgutable* kvect = (Tamgutable*)ke;
        for (long i = 0; i < kvect->size; i++)
            Push(kvect->values[i]);
        return this;
    }

    TamguIteration* itr = ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next())
        Push(itr->Value());
    itr->Release();
    return this;
}

Exporting Tamgu* Tamgutable::Combine(Tamgu* ke) {
    if (ke->Size() == 0)
        return this;
    
    Tamguvector* vect = globalTamgu->Providevector();
    long i;
    Tamgu* val;
    
    if (!ke->isContainer()) {
        for (i=0; i< size ;i++) {
            val=new Tamguvector;
            val->Push(values[i]);
            val->Push(ke);
            vect->Push(val);
        }
        return vect;
    }
    
    
    Locking _lock(ke);
    
    i = 0;
    TamguIteration* itr = (TamguIteration*)ke->Newiteration(false);
    for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
        if (i == size) {
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
    
    for (;i<size;i++) {
        val=new Tamguvector;
        val->Push(values[i]);
        vect->Push(val);
    }
    
    return vect;
}

Exporting Tamgu* Tamgutable::same(Tamgu* a) {
    
    if (a->Type() != idtype) {
        if (a->isVectorContainer()) {
            if (a->Size() != size)
                return aFALSE;
            Tamgu* v;
            for (int i = 0; i < size; i++) {
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

    Tamgutable* v = (Tamgutable*)a;

    if (size != v->size)
        return aFALSE;

    for (int i = 0; i < size; i++) {
        if (values[i]->same(v->values[i]) == aFALSE)
            return aFALSE;
    }
    return aTRUE;
}

Exporting unsigned long Tamgutable::EditDistance(Tamgu* e) {
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


Exporting Tamgu* Tamgutable::Looptaskell(Tamgu* recipient, Tamgu* context, Tamgu* environment, TamguFunctionLambda* bd, short idthread) {

    Tamgu* a;
    uchar addvalue = 0;

    if (context != aNOELEMENT)
        addvalue = Selecttype(context);

    for (size_t i = 0; i < size; i++) {
        if (values[i] == aNOELEMENT)
            continue;

        recipient->Putvalue(values[i], idthread);

        a = bd->DirectEval(environment, aNULL, idthread);
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

Exporting Tamgu* Tamgutable::Filter(short idthread, Tamgu* env, TamguFunctionLambda* bd, Tamgu* var, Tamgu* kcont, Tamgu* accu, Tamgu* init, bool direct) {

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

    for (size_t i = 0; i < size; i++) {
        key = values[i];
        if (key == aNOELEMENT)
            continue;

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
Exporting void Tamgutable::Storevalue(string& u) {
    Tamgu* a = globalTamgu->Providestring(u);
    
    
    push(a);
}

Exporting void Tamgutable::Storevalue(wstring& u) {
    Tamgu* a = globalTamgu->Provideustring(u);
    
    
    push(a);
}

Exporting void Tamgutable::storevalue(string u) {
    Tamgu* a = globalTamgu->Providestring(u);

    
    push(a);
}

Exporting void Tamgutable::storevalue(wstring u) {
    Tamgu* a = globalTamgu->Provideustring(u);

    
    push(a);
}

Exporting void Tamgutable::storevalue(long u) {
    Tamgu* a = globalTamgu->Provideint(u);
    
    push(a);
}

Exporting void Tamgutable::storevalue(short u) {
    Tamgu* a = new Tamgushort(u);

    
    push(a);
}

Exporting void Tamgutable::storevalue(BLONG u) {
    Tamgu* a = new Tamgulong(u);

    
    push(a);
}

Exporting void Tamgutable::storevalue(float u) {
    Tamgu* a = new Tamgudecimal(u);

    
    push(a);
}

Exporting void Tamgutable::storevalue(double u) {
    Tamgu* a = globalTamgu->Providefloat(u);

    
    push(a);
}

Exporting void Tamgutable::storevalue(unsigned char u) {
    Tamgu* a = new Tamgubyte(u);
    
    push(a);
}

Exporting void Tamgutable::storevalue(wchar_t u) {
    wstring w;
    w = u;
    Tamgu* a = globalTamgu->Provideustring(w);
    
    push(a);
}

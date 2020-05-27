/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutreemapu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapuMethod>  Tamgutreemapu::methods;
Exporting hmap<string, string> Tamgutreemapu::infomethods;
Exporting basebin_hash<unsigned long> Tamgutreemapu::exported;

Exporting short Tamgutreemapu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapu::AddMethod(TamguGlobal* global, string name, treemapuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgutreemapu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgutreemapu::idtype = global->Getid("treemapu");

    Tamgutreemapu::AddMethod(global, "clear", &Tamgutreemapu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapu::AddMethod(global, "invert", &Tamgutreemapu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapu::AddMethod(global, "find", &Tamgutreemapu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapu::AddMethod(global, "items", &Tamgutreemapu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapu::AddMethod(global, "join", &Tamgutreemapu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapu::AddMethod(global, "test", &Tamgutreemapu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapu::AddMethod(global, "keys", &Tamgutreemapu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapu::AddMethod(global, "values", &Tamgutreemapu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapu::AddMethod(global, "sum", &Tamgutreemapu::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemapu::AddMethod(global, "product", &Tamgutreemapu::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemapu::AddMethod(global, "pop", &Tamgutreemapu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapu::AddMethod(global, "merge", &Tamgutreemapu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgutreemapu::idtype] = new Tamgutreemapu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("utreemap")] = new Tamgutreemapu(global);

    global->RecordMethods(global->Getid("utreemap"), Tamgutreemapu::exported);
    #endif
    global->RecordMethods(Tamgutreemapu::idtype, Tamgutreemapu::exported);

    return true;
}


Exporting TamguIteration* Tamgutreemapu::Newiteration(bool direction) {
    return new TamguIterationtreemapu(this, direction);
}


Exporting Tamgu* Tamgutreemapu::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    wstring val = a->UString();
    
    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        try {
            values.at(val);
            v->values.push_back(val);
        }
        catch(const std::out_of_range& oor) {}
        
        return v;
    }
    
    if (context->isString()) {
        locking();
        try {
            values.at(val);
            unlocking();
            return globalTamgu->Providewithustring(val);
        }
        catch(const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }
    
    locking();
    try {
        values.at(val);
        unlocking();
        return aTRUE;
    }
    catch(const std::out_of_range& oor) {
        unlocking();
        return aFALSE;
    }
    
}


Exporting Tamgu* Tamgutreemapu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    Tamgu* a = callfunc->Evaluate(0,context,idthread);
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                v->values.push_back(it.first);
        }
        return v;
    }
    Locking _lock(this);
    for (auto& it : values) {
        if (it.second->same(a) == aTRUE)
            return globalTamgu->Provideustring(it.first);
    }
    return aNULL;
}



Exporting void Tamgutreemapu::Cleanreference(short inc) {
    locking();
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    
    unlocking();
}

Exporting void Tamgutreemapu::Setreference(short inc) {
    locking();

    reference += inc;
    protect = false;
    
    for (auto& it : values)
        it.second->Addreference(investigate,inc);

    unlocking();
}

Exporting void Tamgutreemapu::Setreference() {
    locking();

    ++reference;
    protect = false;
    
    for (auto& it : values)
        it.second->Addreference(investigate,1);

    unlocking();
}

static void resetMap(Tamgutreemapu* kmap, short inc) {
    kmap->reference -= inc;
    
    kmap->locking();
    
    map<wstring, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        kmap->unlocking();
        return;
    }
    
    for (auto& itx : values)
        itx.second->Removereference(inc);
    kmap->unlocking();
}

Exporting void Tamgutreemapu::Resetreference(short inc) {
    if ((reference + containerreference - inc) > 0)
        resetMap(this, inc);
    else {
        resetMap(this, inc + 1 - protect);
        if (!protect) {
            if (idtracker != -1)
                globalTamgu->RemoveFromTracker(idtracker);
            delete this;
        }
    }
}


Exporting Tamgu* Tamgutreemapu::Push(Tamgu* k, Tamgu* v) {
    locking();
    wstring s = k->UString();
    
    k = values[s];
    if (k != NULL)
        k->Removereference(reference + 1);

    v = v->Atom();
    values[s] = v;
    v->Addreference(investigate,reference+1);
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapu::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    locking();
    try {
        kkey = values.at(k);
        values.erase(k);
        unlocking();
        kkey->Removereference(reference + 1);
        return aTRUE;
    }
    catch (const std::out_of_range& oor) {
        unlocking();
        return aFALSE;
    }
}

Exporting void Tamgutreemapu::Clear() {
    Locking _lock(this);

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamgutreemapu::String() {
    if (!lockingmark())
        return("{...}");

    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it.first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << ":";
        sx = it.second->StringToDisplay();
        if (!it.second->isString() || it.second->isContainer())
            res << sx;
        else
            stringing(res, sx);
    }
    res << "}";
    unlockingmark();
    return res.str();
}

Exporting string Tamgutreemapu::JSonString() {
    if (!lockingmark())
        return("");

    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it.first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << sx << ":" << it.second->JSonString();
    }
    res << "}";
    unlockingmark();
    return res.str();
}


Exporting long Tamgutreemapu::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamgutreemapu::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamgutreemapu::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamgutreemapu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgutreemapu::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
    #endif


Exporting Tamgu*  Tamgutreemapu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            Clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                Push(itr->Keyustring(), itr->Value());
            itr->Release();
            return aTRUE;
        }
        wchar_t buff[20];
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            Clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                swprintf_s(buff, 20, L"%d", nb);
                Push(buff, ke->getvalue(it));
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Doublelocking _lock(this, ke);
            Tamgulist* kvect = (Tamgulist*)ke;
            Clear();
            long nb = 0;

            for (auto& it : kvect->values) {
                swprintf_s(buff, 20, L"%d", nb);
                Push(buff, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        Clear();
        if (ke->Type() == Tamgutreemapu::idtype) {
            Tamgutreemapu * kmap = (Tamgutreemapu *)ke;
            //We copy all values from ke to this

            for (auto& it : kmap->values)
                Push(it.first, it.second);
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            Tamgu* a;
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                a=itr->IteratorValue();
                a=a->Atom();
                values[itr->Keyustring()] = a;
                a->Addreference(investigate,reference+1);
            }
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    Push(idx->Getustring(idthread), ke);
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isMapContainer())
            return this;
        
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            locking();
            map<wstring, Tamgu* > ::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideustring(it->first));
            unlocking();
            return vect;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }

        return this;
    }

    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgutreemapu * kmap = new Tamgutreemapu;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        map<wstring, Tamgu* > ::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<wstring, Tamgu* > ::iterator itr = values.find(vright);
        if (itr == values.end() && keyright != aNULL) {
            keyright->Release();
            return kmap;
        }
        keyright->Release();
        for (; it != values.end(); it++) {
            kmap->Push(it->first, it->second);
            if (it == itr)
                return kmap;
        }

        if (itr != values.end())
            kmap->Clear();
        return kmap;

    }

    wstring skey = idx->Getustring(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgutreemapu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapu * m = (Tamgutreemapu *)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<wstring, Tamgu* > ::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapu::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgutreemapu * res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        hmap<wstring, Tamgu*> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->IteratorValue();
            else {
                if (values[v]->same(itr->IteratorValue()) == aTRUE)
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->Push(a.first,a.second);

        return res;


    }
    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu * res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemapu;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]->same(v) == aTRUE)
                res->Push(itr->Keyustring(), v);
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        Tamgu* r;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                res->values[k]->plus(v, true);
            }
            catch (const std::out_of_range& oor) {
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        Tamgu* r;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                res->values[k]->minus(v, true);
            }
            catch (const std::out_of_range& oor) {
            }
        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        Tamgu* r;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                res->values[k]->multiply(v, true);
            }
            catch (const std::out_of_range& oor) {
            }
        }

        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        wstring k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                inter = res->values[k]->divide(v, true);
                if (inter->isError()) {
                    res->Release();
                    itr->Release();
                    return inter;
                }
            }
            catch (const std::out_of_range& oor) {
            }
        }

        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemapu::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        wstring k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                inter = res->values[k]->mod(v, true);
                if (inter->isError()) {
                    res->Release();
                    itr->Release();
                    return inter;
                }
            }
            catch (const std::out_of_range& oor) {
            }
        }

        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemapu::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        Tamgu* r;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                res->values[k]->shiftright(v, true);
            }
            catch (const std::out_of_range& oor) {
                r = NULL;
            }
        }

        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        Tamgu* r;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                res->values[k]->shiftleft(v, true);
            }
            catch (const std::out_of_range& oor) {
            }
        }

        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        Tamgu* r;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            v = itr->IteratorValue();
            try {
                r = values.at(k);
                res->Push(k, r);
                res->values[k]->power(v, true);
            }
            catch (const std::out_of_range& oor) {
            }
        }

        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}



Exporting Tamgu* Tamgutreemapu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    map<wstring, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<wstring> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    a = aNULL;
    bool testcond = false;

    long sz = keys.size();
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        var->storevalue(keys[i]);

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


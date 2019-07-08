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
Exporting bin_hash<unsigned long> Tamgutreemapu::exported;

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
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isString()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Provideustring(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

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
    Locking* _lock = _getlock(this);
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    
    _cleanlock(_lock);
}

Exporting void Tamgutreemapu::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect = false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    for (auto& it : values)
        it.second->Addreference(inc);
    _cleanlock(_lock);
    
    loopmark=false;
}

Exporting void Tamgutreemapu::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect = false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    for (auto& it : values)
        it.second->Addreference(1);
    _cleanlock(_lock);
    
    loopmark=false;
}

static void resetMap(Tamgutreemapu* kmap, short inc) {
    kmap->reference -= inc;
    
    Locking* _lock = _getlock(kmap);
    
    map<wstring, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        _cleanlock(_lock);
        return;
    }
    
    for (auto& itx : values)
        itx.second->Removereference(inc);
    _cleanlock(_lock);
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
    Locking _lock(this);
    wstring s = k->UString();
    if (values.find(s) != values.end()) {
        Tamgu* kval = values[s];
        values.erase(s);
        kval->Removereference(reference + 1);
    }

    v = v->Atom();
    values[s] = v;
    v->Addreference(reference+1);
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapu::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    Locking _lk(this);
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Removereference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgutreemapu::Clear() {
    Locking _lock(this);

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamgutreemapu::String() {
    Locking _lock(this);
    if (loopmark)
        return("{...}");
    TamguCircular _c(this);
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
    return res.str();
}

Exporting string Tamgutreemapu::JSonString() {
    Locking _lock(this);
    if (loopmark)
        return("");
    TamguCircular _c(this);
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
    return res.str();
}


Exporting long Tamgutreemapu::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgutreemapu::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgutreemapu::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgutreemapu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgutreemapu::Size() {
    Locking _lock(this);
    return values.size();
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
        Locking* _lock = _getlock(this);
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
                a->Addreference(reference+1);
            }
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Push(idx->UString(), ke);
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapu::Get(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            map<wstring, Tamgu* > ::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideustring(it->first));
            return vect;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }

        return this;
    }

    Tamgu* key;
    if (idx->isInterval()) {
        Tamgutreemapu * kmap = new Tamgutreemapu;
        key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Get(aNULL, aNULL, idthread);
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

    key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
    
    if (key == aNULL) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }

    wstring skey = key->UString();
    key->Release();

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

    Tamgutreemapu * res;
    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->plus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu * res;
    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->minus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu * res;
    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->multiply(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu * res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                r = res->values[it->first]->divide(v, true);
                if (r->isError()) {
                    res->Release();

                    itr->Release();
                    return r;
                }
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

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

    Tamgutreemapu * res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                r = res->values[it->first]->mod(v, true);
                if (r->isError()) {
                    res->Release();

                    itr->Release();
                    return r;
                }
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

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

    Tamgutreemapu * res;
    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->shiftright(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu * res;
    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->shiftleft(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapu::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapu * res;
    if (b->isMapContainer()) {
        map<wstring, Tamgu* > ::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapu;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->Push(it->first, it->second);
                res->values[it->first]->power(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapu *)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}




Exporting Tamgu* Tamgutreemapu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    map<wstring, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<wstring> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    for (long i = 0; i < keys.size(); i++) {

        var->storevalue(keys[i]);

        a = ins->instructions.vecteur[1]->Get(context, aNULL, idthread);

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

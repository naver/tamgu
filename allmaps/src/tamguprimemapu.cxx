/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguprimemapu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapuMethod>  Tamguprimemapu::methods;
static ThreadLock classlock;

Exporting short Tamguprimemapu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapu::AddMethod(TamguGlobal* global, string name, primemapuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguprimemapu::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamguprimemapu::methods.isEmpty())
    Tamguprimemapu::InitialisationModule(global,"");
}

bool Tamguprimemapu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamguprimemapu::idtype = global->Getid("primemapu");

    Tamguprimemapu::AddMethod(global, "clear", &Tamguprimemapu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapu::AddMethod(global, "invert", &Tamguprimemapu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapu::AddMethod(global, "find", &Tamguprimemapu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamguprimemapu::AddMethod(global, "items", &Tamguprimemapu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamguprimemapu::AddMethod(global, "join", &Tamguprimemapu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapu::AddMethod(global, "test", &Tamguprimemapu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapu::AddMethod(global, "keys", &Tamguprimemapu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapu::AddMethod(global, "values", &Tamguprimemapu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapu::AddMethod(global, "sum", &Tamguprimemapu::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguprimemapu::AddMethod(global, "product", &Tamguprimemapu::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguprimemapu::AddMethod(global, "pop", &Tamguprimemapu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapu::AddMethod(global, "merge", &Tamguprimemapu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamguprimemapu::idtype] = new Tamguprimemapu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("uprimemap")] = new Tamguprimemapu(global);

    global->RecordCompatibilities(global->Getid("uprimemap"));
    #endif
    global->RecordCompatibilities(Tamguprimemapu::idtype);

    return true;
}


Exporting TamguIteration* Tamguprimemapu::Newiteration(bool direction) {
    return new TamguIterationprimemapu(this, direction);
}


Exporting Tamgu* Tamguprimemapu::in(Tamgu* context, Tamgu* a, short idthread) {
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
        locking();
        if (values.find(val)!=values.end()) {
            unlocking();
            return globalTamgu->Provideustring(val);
        }
        unlocking();
        return aNOELEMENT;;
    }
    
    locking();
    if (values.find(val)!=values.end()) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;

}


Exporting Tamgu* Tamguprimemapu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<wstring, Tamgu*>::iterator it;
    
    Tamgu* a = callfunc->Evaluate(0,context,idthread);

    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Doublelocking _lock(this, v);

        for (it = values.begin(); it != values.end(); it++) {
            if (it->second->same(a) == aTRUE)
                v->values.push_back(it->first);
        }
        return v;
    }

    Locking _lock(this);
    for (it = values.begin(); it != values.end(); it++) {
        if (it->second->same(a) == aTRUE)
            return globalTamgu->Provideustring(it->first);
    }

    return aNULL;

}



Exporting void Tamguprimemapu::Cleanreference(short inc) {
    locking();
    prime_hash<wstring, Tamgu*>::iterator it;
    for (it=values.begin();it!=values.end();it++)
        it->second->Removecontainerreference( inc);
    unlocking();
}

Exporting void Tamguprimemapu::Setreference(short inc) {
    locking();

    reference += inc;
    protect = false;
    
    prime_hash<wstring, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(investigate,inc);

    unlocking();
}

Exporting void Tamguprimemapu::Setreference() {
    locking();

    ++reference;
    protect = false;
    
    prime_hash<wstring, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(investigate,1);

    unlocking();
}

static void resetMap(Tamguprimemapu* kmap, short inc) {
    kmap->reference -= inc;
    
    kmap->locking();
    prime_hash<wstring, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        kmap->unlocking();
        return;
    }
    prime_hash<wstring, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(inc);
    kmap->unlocking();
}

Exporting void Tamguprimemapu::Resetreference(short inc) {
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


Exporting Tamgu* Tamguprimemapu::Push(Tamgu* k, Tamgu* v) {
    locking();
    wstring s = k->UString();
    if (values.find(s) != values.end())
        values[s]->Removereference(reference + 1);
    v = v->Atom();
    values[s] = v;
    v->Addreference(investigate,reference+1);
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapu::Pop(Tamgu* kkey) {
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

Exporting void Tamguprimemapu::Clear() {
    Locking _lock(this);
    prime_hash<wstring, Tamgu* > ::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamguprimemapu::String() {
    if (!lockingmark())
        return("{...}");

    stringstream res;
    prime_hash<wstring, Tamgu* > ::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it->first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << ":";
        sx = it->second->StringToDisplay();
        if (!it->second->isString())
            res << sx;
        else
            stringing(res, sx);
    }
    unlockingmark();
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapu::JSonString() {
    if (!lockingmark())
        return("");

    stringstream res;
    prime_hash<wstring, Tamgu* > ::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it->first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << sx << ":" << it->second->JSonString();
    }
    res << "}";
    unlockingmark();
    return res.str();
}


Exporting long Tamguprimemapu::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamguprimemapu::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamguprimemapu::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamguprimemapu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemapu::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
    #endif


Exporting Tamgu*  Tamguprimemapu::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                swprintf_s(buff, 20, L"%d", nb);
                Push(buff, *it);
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        Clear();
        if (ke->Type() == Tamguprimemapu::idtype) {
            Tamguprimemapu * kmap = (Tamguprimemapu *)ke;
            //We copy all values from ke to this
            prime_hash<wstring, Tamgu* > ::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                Push(it->first, it->second);
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


Exporting Tamgu* Tamguprimemapu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            prime_hash<wstring, Tamgu* > ::iterator it;
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
        Tamguprimemapu * kmap = new Tamguprimemapu;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        prime_hash<wstring, Tamgu* > ::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<wstring, Tamgu* > ::iterator itr = values.find(vright);
        if (itr == values.end() && keyright != aNULL) {
            if (keyright != ((TamguIndex*)idx)->right)
                keyright->Release();
            return kmap;
        }
        if (keyright != ((TamguIndex*)idx)->right)
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

Exporting Tamgu* Tamguprimemapu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemapu * m = (Tamguprimemapu *)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<wstring, Tamgu* > ::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapu::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamguprimemapu * res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
        hmap<wstring, Tamgu*> keys;

        prime_hash<wstring, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
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
        res = (Tamguprimemapu *)Atom(true);

    prime_hash<wstring, Tamgu* > ::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapu *)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    for (it = values.begin(); it != values.end(); it++)
        it->second->orset(b, true);
    return res;

}

Exporting Tamgu* Tamguprimemapu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapu::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapu::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamguprimemapu::divideinteger(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                r = res->values[it->first]->divideinteger(v, true);
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->divideinteger(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamguprimemapu::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamguprimemapu::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapu::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapu::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring, Tamgu* > ::iterator it;
    Tamguprimemapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapu;
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
        res = (Tamguprimemapu *)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->power(b, true);
    return res;
}




Exporting Tamgu* Tamguprimemapu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    prime_hash<wstring, Tamgu*>::iterator it;
    
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


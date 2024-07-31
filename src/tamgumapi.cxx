/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapi.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamgumapi.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapiMethod>  Tamgumapi::methods;
static ThreadLock classlock;

Exporting short Tamgumapi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapi::AddMethod(TamguGlobal* global, string name, mapiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamgumapi::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
    if (Tamgumapi::methods.isEmpty())
        Tamgumapi::InitialisationModule(global,"");
}


bool Tamgumapi::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    Tamgumapi::idtype = a_mapi;
    
    Tamgumapi::AddMethod(global, "clear", &Tamgumapi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapi::AddMethod(global, "invert", &Tamgumapi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapi::AddMethod(global, "find", &Tamgumapi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamgumapi::AddMethod(global, "items", &Tamgumapi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamgumapi::AddMethod(global, "join", &Tamgumapi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapi::AddMethod(global, "test", &Tamgumapi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapi::AddMethod(global, "keys", &Tamgumapi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapi::AddMethod(global, "values", &Tamgumapi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapi::AddMethod(global, "sum", &Tamgumapi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapi::AddMethod(global, "product", &Tamgumapi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapi::AddMethod(global, "pop", &Tamgumapi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapi::AddMethod(global, "merge", &Tamgumapi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    if (version != "") {
        global->minimal_indexes[a_mapi] = true;
        
        global->newInstance[a_mapi] = new Tamgumapi(global);
        
        global->RecordCompatibilities(a_mapi);
    }
    
    return true;
}


Exporting TamguIteration* Tamgumapi::Newiteration(bool direction) {
    return new TamguIterationmapi(this, direction);
}


Exporting Tamgu* Tamgumapi::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    long val = a->Integer();

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        try {
            values.at(val);
            v->values.push_back(val);
        }
        catch(const std::out_of_range& oor) {}

        return v;
    }

    if (context->isNumber()) {
        locking();
        try {
            values.at(val);
            unlocking();
            return globalTamgu->ProvideConstint(val);
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


Exporting Tamgu* Tamgumapi::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    Tamgu* a = callfunc->Evaluate(0,context,idthread);
    if (context->isBoolean()) {
        Locking _lock(this);
        for (const auto& it : values) {
            if (it.second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (const auto& it : values) {
            if (it.second->same(a) == aTRUE)
                v->values.push_back(it.first);
        }
        return v;
    }
    Locking _lock(this);
    for (const auto& it : values) {
        if (it.second->same(a) == aTRUE)
            return globalTamgu->ProvideConstint(it.first);
    }
    return aNULL;
}



Exporting void Tamgumapi::Cleanreference(short inc) {
    locking();
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    
    unlocking();
}

Exporting void Tamgumapi::Setreference(short inc) {
    locking();

    reference += inc;
    protect = false;
    
    for (const auto& it : values)
        it.second->Addreference(investigate,inc);
    
    unlocking();
}

Exporting void Tamgumapi::Setreference() {
    locking();

    ++reference;
    protect = false;
    
    for (const auto& it : values)
        it.second->Addreference(investigate,1);
    unlocking();
}

static void resetMap(Tamgumapi* kmap, short inc) {
    kmap->reference -= inc;
    
    kmap->locking();
    
    hmap<long, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        kmap->unlocking();
        return;
    }
    
    for (auto& itx : values)
        itx.second->Removereference(inc);
    kmap->unlocking();
}

Exporting void Tamgumapi::Resetreference(short inc) {
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


Exporting Tamgu* Tamgumapi::Push(Tamgu* k, Tamgu* v) {
    locking();
    long s = k->Integer();
    
    k = values[s];
    if (k != NULL) {
        if (k == v)
            return this;
        k->Removereference(reference + 1);
    }

    v = v->Atom();
    values[s] = v;
    unlocking();
    v->Addreference(investigate,reference+1);
    return aTRUE;
}

Exporting Tamgu* Tamgumapi::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
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

Exporting void Tamgumapi::Clear() {
    locking();

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
    unlocking();
}



Exporting string Tamgumapi::String() {
    if (!lockingmark())
        return("{...}");

    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        sx = it.second->StringToDisplay();
        if (!it.second->isString() || it.second->isContainer())
            res << sx;
        else
            stringing(res, sx);
    }

    unlockingmark();
    res << "}";
    return res.str();
}

Exporting string Tamgumapi::JSonString() {
    if (!lockingmark())
        return("");

    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":" << it.second->JSonString();
    }

    unlockingmark();
    res << "}";
    return res.str();
}


Exporting long Tamgumapi::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapi::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapi::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapi::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapi::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
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
                Push(itr->Keyinteger(), itr->Value());
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            Clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                Push(nb, ke->getvalue(it));
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Doublelocking _lock(this, ke);
            Tamgulist* kvect = (Tamgulist*)ke;
            Clear();
            long nb = 0;

            for (const auto& it : kvect->values) {
                Push(nb, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        locking();
        Clear();
        if (ke->Type() == a_mapi) {
            Tamgumapi* kmap = (Tamgumapi*)ke;
            //We copy all values from ke to this

            for (const auto& it : kmap->values)
                Push(it.first, it.second);
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            Tamgu* a;
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                a=itr->IteratorValue();
                a=a->Atom();
                values[itr->Keyinteger()] = a;
                a->Addreference(investigate,reference+1);
            }
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    Push(idx->Getinteger(idthread), ke);
    return aTRUE;
}



Tamgu* Tamgumapi::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    long skey = key->Getinteger(idthread);

    Tamgu* val;
    
    locking();
    try {
        val = values.at(skey);
    }
    catch(...) {
        unlocking();
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;
    }
    unlocking();
    return val;
}

Exporting Tamgu* Tamgumapi::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            hmap<long, Tamgu*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->ProvideConstint(it->first));
            unlocking();
            return vect;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->ProvideConstint(v);
        }

        return this;
    }

    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgumapi* kmap = new Tamgumapi;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        hmap<long, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<long, Tamgu*>::iterator itr = values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                if (keyright != ((TamguIndex*)idx)->right)
                    keyright->Release();
                return kmap;
            }
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

    long skey = idx->Getinteger(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumapi::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapi* m = (Tamgumapi*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<long, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapi::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgumapi* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        hmap<long, Tamgu*> keys;

        for (const auto& it : values)
            keys[it.first] = it.second;
            
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v]=itr->IteratorValue();
            else {
                if (values[v]->same(itr->IteratorValue()) == aTRUE)
                    keys.erase(v);
            }
        }
        itr->Release();

        for (const auto& a : keys)
            res->Push(a.first,a.second);

        return res;


    }
    if (itself)
        res = this;
    else
        res = (Tamgumapi*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapi*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapi;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keyinteger()) != values.end() && values[itr->Keyinteger()]->same(v) == aTRUE)
                res->Push(itr->Keyinteger(), v);
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapi*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        Tamgu* v;
        Tamgu* r;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        Tamgu* v;
        Tamgu* r;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        Tamgu* v;
        Tamgu* r;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        long k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapi::divideinteger(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        long k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
                res->Push(k, r);
                inter = res->values[k]->divideinteger(v, true);
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divideinteger(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapi::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        long k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapi::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        Tamgu* v;
        Tamgu* r;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
                res->Push(k, r);
                res->values[k]->shiftright(v, true);
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        Tamgu* v;
        Tamgu* r;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgumapi::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapi;
        Tamgu* v;
        Tamgu* r;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
            try {
                r = values.at(k);
                v = itr->IteratorValue();
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
        res = (Tamgumapi*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}


Exporting Tamgu* Tamgumapi::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    hmap<long, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<long> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    long sz = keys.size();
    a = aNULL;
    bool testcond = false;
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

Exporting Tamgu* Tamguframemapi::Push(Tamgu* k, Tamgu* v) {
    if (!check_frame(v)) {
        return globalTamgu->Returnerror(e_error_on_frame_map);
    }

    locking();
    long s = k->Integer();
    
    k = values[s];
    if (k != NULL) {
        if (k == v)
            return this;
        k->Removereference(reference + 1);
    }

    v = v->Atom();
    values[s] = v;
    unlocking();
    v->Addreference(investigate,reference+1);
    return aTRUE;
}

Exporting Tamgu*  Tamguframemapi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
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
                Push(itr->Keyinteger(), itr->Value());
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            Clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                Push(nb, ke->getvalue(it));
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Doublelocking _lock(this, ke);
            Tamgulist* kvect = (Tamgulist*)ke;
            Clear();
            long nb = 0;

            for (const auto& it : kvect->values) {
                Push(nb, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        locking();
        Clear();
        if (ke->Type() == a_framemapi) {
            Tamguframemapi* kmap = (Tamguframemapi*)ke;
            //We copy all values from ke to this

            for (const auto& it : kmap->values)
                Push(it.first, it.second);
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            Tamgu* a;
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                a=itr->IteratorValue();
                a=a->Atom();
                values[itr->Keyinteger()] = a;
                a->Addreference(investigate,reference+1);
            }
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    Push(idx->Getinteger(idthread), ke);
    return aTRUE;
}

/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapl.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamgumapl.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<maplMethod>  Tamgumapl::methods;
static ThreadLock classlock;

Exporting short Tamgumapl::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapl::AddMethod(TamguGlobal* global, string name, maplMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




    void Tamgumapl::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgumapl::methods.isEmpty())
    Tamgumapl::InitialisationModule(global,"");
}


   bool Tamgumapl::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgumapl::idtype = global->Getid("mapl");

    
    

    Tamgumapl::AddMethod(global, "clear", &Tamgumapl::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapl::AddMethod(global, "invert", &Tamgumapl::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapl::AddMethod(global, "find", &Tamgumapl::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapl::AddMethod(global, "items", &Tamgumapl::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapl::AddMethod(global, "join", &Tamgumapl::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapl::AddMethod(global, "test", &Tamgumapl::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapl::AddMethod(global, "keys", &Tamgumapl::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapl::AddMethod(global, "values", &Tamgumapl::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapl::AddMethod(global, "sum", &Tamgumapl::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapl::AddMethod(global, "product", &Tamgumapl::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapl::AddMethod(global, "pop", &Tamgumapl::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapl::AddMethod(global, "merge", &Tamgumapl::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {        
    global->minimal_indexes[Tamgumapl::idtype] = true;

        global->newInstance[Tamgumapl::idtype] = new Tamgumapl(global);
        
        global->RecordCompatibilities(Tamgumapl::idtype);
    }

    return true;
}


Exporting TamguIteration* Tamgumapl::Newiteration(bool direction) {
    return new TamguIterationmapl(this, direction);
}


Exporting Tamgu* Tamgumapl::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    BLONG val = a->Long();

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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
            return globalTamgu->Providelong(val);
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


Exporting Tamgu* Tamgumapl::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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
            return globalTamgu->Providelong(it.first);
    }
    return aNULL;
}

Exporting void Tamgumapl::Cleanreference(short inc) {
    locking();
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    unlocking();
}

Exporting void Tamgumapl::Setreference(short inc) {
    locking();

    reference += inc;
    protect = false;
    
    for (const auto& it : values)
        it.second->Addreference(investigate,inc);
    
    unlocking();
}

Exporting void Tamgumapl::Setreference() {
    locking();

    ++reference;
    protect = false;
    
    for (const auto& it : values)
        it.second->Addreference(investigate,1);
    
    unlocking();
}

static void resetMap(Tamgumapl* kmap, short inc) {
    kmap->reference -= inc;
    
    kmap->locking();
    
    hmap<BLONG, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        kmap->unlocking();
        return;
    }
    
    for (auto& itx : values)
        itx.second->Removereference(inc);
    kmap->unlocking();
}

Exporting void Tamgumapl::Resetreference(short inc) {
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


Exporting Tamgu* Tamgumapl::Push(Tamgu* k, Tamgu* v) {
    locking();
    BLONG s = k->Long();
    
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

Exporting Tamgu* Tamgumapl::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
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

Exporting void Tamgumapl::Clear() {
    locking();

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
    unlocking();
}



Exporting string Tamgumapl::String() {
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

Exporting string Tamgumapl::JSonString() {
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


Exporting long Tamgumapl::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapl::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapl::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapl::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapl::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapl::Put(Tamgu* idx, Tamgu* ke, short idthread) {
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
                Push(itr->Keylong(), itr->Value());
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
        if (ke->Type() == Tamgumapl::idtype) {
            Tamgumapl* kmap = (Tamgumapl*)ke;
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
                values[itr->Keylong()] = a;
                a->Addreference(investigate,reference+1);
            }
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    Push(idx->Long(), ke);
    return aTRUE;
}



Tamgu* Tamgumapl::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    BLONG skey = key->Getlong(idthread);

    if (globalTamgu->threadMODE) {
        locking();
        key = values[skey];
        if (key == NULL) {
            if (globalTamgu->erroronkey) {
                unlocking();
                return globalTamgu->Returnerror(e_wrong_index, idthread);
            }
            values.erase(skey);
            key = aNOELEMENT;
        }
        unlocking();
        return key;
    }
    
    key = values[skey];
    if (key == NULL) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        values.erase(skey);
        key = aNOELEMENT;
    }
    return key;
}

Exporting Tamgu* Tamgumapl::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            hmap<BLONG, Tamgu*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providelong(it->first));
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
        Tamgumapl* kmap = new Tamgumapl;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        hmap<BLONG, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<BLONG, Tamgu*>::iterator itr = values.end();
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

    BLONG skey = idx->Getlong(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumapl::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapl* m = (Tamgumapl*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<BLONG, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapl::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgumapl* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        hmap<BLONG, Tamgu*> keys;

        for (const auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapl*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapl;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]->same(v) == aTRUE)
                res->Push(itr->Keylong(), v);
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapl*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        Tamgu* v;
        Tamgu* r;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        Tamgu* v;
        Tamgu* r;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        Tamgu* v;
        Tamgu* r;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        BLONG k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapl::divideinteger(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        BLONG k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divideinteger(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapl::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        BLONG k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapl::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        Tamgu* v;
        Tamgu* r;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        Tamgu* v;
        Tamgu* r;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgumapl::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapl;
        Tamgu* v;
        Tamgu* r;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapl*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}





Exporting Tamgu* Tamgumapl::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    hmap<BLONG, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<BLONG> keys;

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


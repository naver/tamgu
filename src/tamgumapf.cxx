/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamgumapf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapfMethod>  Tamgumapf::methods;
Exporting hmap<string, string> Tamgumapf::infomethods;
Exporting bin_hash<unsigned long> Tamgumapf::exported;

Exporting short Tamgumapf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapf::AddMethod(TamguGlobal* global, string name, mapfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapf::Setidtype(TamguGlobal* global) {
    Tamgumapf::InitialisationModule(global,"");
}


   bool Tamgumapf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgumapf::idtype = global->Getid("mapf");

    Tamgumapf::AddMethod(global, "clear", &Tamgumapf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapf::AddMethod(global, "invert", &Tamgumapf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapf::AddMethod(global, "find", &Tamgumapf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapf::AddMethod(global, "items", &Tamgumapf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapf::AddMethod(global, "join", &Tamgumapf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapf::AddMethod(global, "test", &Tamgumapf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapf::AddMethod(global, "keys", &Tamgumapf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapf::AddMethod(global, "values", &Tamgumapf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapf::AddMethod(global, "sum", &Tamgumapf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapf::AddMethod(global, "product", &Tamgumapf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapf::AddMethod(global, "pop", &Tamgumapf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapf::AddMethod(global, "merge", &Tamgumapf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgumapf::idtype] = new Tamgumapf(global);
        
        global->RecordMethods(Tamgumapf::idtype, Tamgumapf::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgumapf::Newiteration(bool direction) {
    return new TamguIterationmapf(this, direction);
}


Exporting Tamgu* Tamgumapf::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    double val = a->Float();

    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
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
            return globalTamgu->Providefloat(val);
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


Exporting Tamgu* Tamgumapf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
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
            return globalTamgu->Providefloat(it.first);
    }
    return aNULL;
}



Exporting void Tamgumapf::Cleanreference(short inc) {
    locking();
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    unlocking();
}

Exporting void Tamgumapf::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect = false;
    loopmark=true;
    
    locking();
    for (auto& it : values)
        it.second->Addreference(inc);
    unlocking();
    
    loopmark=false;
}

Exporting void Tamgumapf::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect = false;
    loopmark=true;
    
    locking();
    for (auto& it : values)
        it.second->Addreference(1);
    unlocking();
    
    loopmark=false;
}

static void resetMap(Tamgumapf* kmap, short inc) {
    kmap->reference -= inc;

    kmap->locking();

    hmap<double, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        kmap->unlocking();
        return;
    }

    for (auto& itx : values)
        itx.second->Removereference(inc);
    kmap->unlocking();
}

Exporting void Tamgumapf::Resetreference(short inc) {
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


Exporting Tamgu* Tamgumapf::Push(Tamgu* k, Tamgu* v) {
    locking();
    double s = k->Float();
    
    k = values[s];
    if (k != NULL)
        k->Removereference(reference + 1);

    v = v->Atom();
    values[s] = v;
    unlocking();
    v->Addreference(reference+1);
    return aTRUE;
}

Exporting Tamgu* Tamgumapf::Pop(Tamgu* kkey) {
    double k = kkey->Float();
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

Exporting void Tamgumapf::Clear() {
    locking();

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
    unlocking();
}



Exporting string Tamgumapf::String() {
    locking();
    if (loopmark) {
        unlocking();
        return("{...}");
    }
    TamguCircular _c(this);
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
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgumapf::JSonString() {
    locking();
    if (loopmark) {
        unlocking();
        return("");
    }
    TamguCircular _c(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":" << it.second->JSonString();
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgumapf::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapf::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapf::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapf::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapf::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapf::Put(Tamgu* idx, Tamgu* ke, short idthread) {
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
                Push(itr->Keyfloat(), itr->Value());
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

            for (auto& it : kvect->values) {
                Push(nb, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        Clear();
        if (ke->Type() == Tamgumapf::idtype) {
            Tamgumapf* kmap = (Tamgumapf*)ke;
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
                values[itr->Keyfloat()] = a;
                a->Addreference(reference+1);
            }
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    Push(idx->Getfloat(idthread), ke);
    return aTRUE;
}


Exporting Tamgu* Tamgumapf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            hmap<double, Tamgu*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providefloat(it->first));
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
        Tamgumapf* kmap = new Tamgumapf;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        hmap<double, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<double, Tamgu*>::iterator itr = values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                keyright->Release();
                return kmap;
            }
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

    double skey = idx->Getfloat(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumapf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapf* m = (Tamgumapf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<double, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapf::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgumapf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        hmap<double, Tamgu*> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapf*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapf;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()]->same(v) == aTRUE)
                res->Push(itr->Keyfloat(), v);
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapf*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        Tamgu* v;
        Tamgu* r;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        Tamgu* v;
        Tamgu* r;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        Tamgu* v;
        Tamgu* r;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        double k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        double k;
        Tamgu* inter;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgumapf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        Tamgu* v;
        Tamgu* r;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        Tamgu* v;
        Tamgu* r;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapf* res;
    if (b->isMapContainer()) {

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapf;
        Tamgu* v;
        Tamgu* r;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgumapf*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}

Exporting Tamgu* Tamgumapf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    hmap<double, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<double> keys;

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


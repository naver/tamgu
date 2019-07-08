/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamgutreemapf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapfMethod>  Tamgutreemapf::methods;
Exporting hmap<string, string> Tamgutreemapf::infomethods;
Exporting bin_hash<unsigned long> Tamgutreemapf::exported;

Exporting short Tamgutreemapf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapf::AddMethod(TamguGlobal* global, string name, treemapfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgutreemapf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgutreemapf::idtype = global->Getid("treemapf");

    Tamgutreemapf::AddMethod(global, "clear", &Tamgutreemapf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapf::AddMethod(global, "invert", &Tamgutreemapf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapf::AddMethod(global, "find", &Tamgutreemapf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapf::AddMethod(global, "items", &Tamgutreemapf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapf::AddMethod(global, "join", &Tamgutreemapf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapf::AddMethod(global, "test", &Tamgutreemapf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapf::AddMethod(global, "keys", &Tamgutreemapf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapf::AddMethod(global, "values", &Tamgutreemapf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapf::AddMethod(global, "sum", &Tamgutreemapf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemapf::AddMethod(global, "product", &Tamgutreemapf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemapf::AddMethod(global, "pop", &Tamgutreemapf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapf::AddMethod(global, "merge", &Tamgutreemapf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgutreemapf::idtype] = new Tamgutreemapf(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ftreemap")] = new Tamgutreemapf(global);

    global->RecordMethods(global->Getid("ftreemap"), Tamgutreemapf::exported);
    #endif
    global->RecordMethods(Tamgutreemapf::idtype, Tamgutreemapf::exported);

    return true;
}


Exporting TamguIteration* Tamgutreemapf::Newiteration(bool direction) {
    return new TamguIterationtreemapf(this, direction);
}


Exporting Tamgu* Tamgutreemapf::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    double val = a->Float();

     if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isNumber()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Providefloat(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgutreemapf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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



Exporting void Tamgutreemapf::Cleanreference(short inc) {
    Locking* _lock = _getlock(this);
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    
    _cleanlock(_lock);
}

Exporting void Tamgutreemapf::Setreference(short inc) {
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

Exporting void Tamgutreemapf::Setreference() {
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

static void resetMap(Tamgutreemapf* kmap, short inc) {
    kmap->reference -= inc;
    
    Locking* _lock = _getlock(kmap);
    
    map<double, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        _cleanlock(_lock);
        return;
    }
    
    for (auto& itx : values)
        itx.second->Removereference(inc);
    _cleanlock(_lock);
}

Exporting void Tamgutreemapf::Resetreference(short inc) {
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


Exporting Tamgu* Tamgutreemapf::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    double s = k->Float();
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

Exporting Tamgu* Tamgutreemapf::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    Locking _lk(this);
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Removereference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgutreemapf::Clear() {
    Locking _lock(this);

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamgutreemapf::String() {
    Locking _lock(this);
    if (loopmark)
        return("{...}");
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
    res << "}";
    return res.str();
}

Exporting string Tamgutreemapf::JSonString() {
    Locking _lock(this);
    if (loopmark)
        return("");
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
    res << "}";
    return res.str();
}


Exporting long Tamgutreemapf::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgutreemapf::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgutreemapf::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgutreemapf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgutreemapf::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamgutreemapf::Put(Tamgu* idx, Tamgu* ke, short idthread) {
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
        Locking* _lock = _getlock(this);
        Clear();
        if (ke->Type() == Tamgutreemapf::idtype) {
            Tamgutreemapf* kmap = (Tamgutreemapf*)ke;
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
        _cleanlock(_lock);
        return aTRUE;
    }
    Push(idx->Float(), ke);
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapf::Get(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            map<double, Tamgu*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providefloat(it->first));
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
        Tamgutreemapf* kmap = new Tamgutreemapf;
        key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        map<double, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<double, Tamgu*>::iterator itr = values.end();
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

    key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
    
    if (key == aNULL) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }

    double skey = key->Float();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgutreemapf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapf* m = (Tamgutreemapf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<double, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapf::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgutreemapf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapf*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemapf;
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
        res = (Tamgutreemapf*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemapf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemapf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapf* res;
    if (b->isMapContainer()) {
        map<double, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapf;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgutreemapf*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}




Exporting Tamgu* Tamgutreemapf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    map<double, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<double> keys;

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

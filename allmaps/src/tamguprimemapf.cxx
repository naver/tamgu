/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamguprimemapf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapfMethod>  Tamguprimemapf::methods;
Exporting hmap<string, string> Tamguprimemapf::infomethods;
Exporting bin_hash<unsigned long> Tamguprimemapf::exported;

Exporting short Tamguprimemapf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapf::AddMethod(TamguGlobal* global, string name, primemapfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguprimemapf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamguprimemapf::idtype = global->Getid("primemapf");

    Tamguprimemapf::AddMethod(global, "clear", &Tamguprimemapf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapf::AddMethod(global, "invert", &Tamguprimemapf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapf::AddMethod(global, "find", &Tamguprimemapf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamguprimemapf::AddMethod(global, "items", &Tamguprimemapf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamguprimemapf::AddMethod(global, "join", &Tamguprimemapf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapf::AddMethod(global, "test", &Tamguprimemapf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapf::AddMethod(global, "keys", &Tamguprimemapf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapf::AddMethod(global, "values", &Tamguprimemapf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapf::AddMethod(global, "sum", &Tamguprimemapf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguprimemapf::AddMethod(global, "product", &Tamguprimemapf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguprimemapf::AddMethod(global, "pop", &Tamguprimemapf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapf::AddMethod(global, "merge", &Tamguprimemapf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamguprimemapf::idtype] = new Tamguprimemapf(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("fprimemap")] = new Tamguprimemapf(global);

    global->RecordMethods(global->Getid("fprimemap"), Tamguprimemapf::exported);
    #endif
    global->RecordMethods(Tamguprimemapf::idtype, Tamguprimemapf::exported);

    return true;
}


Exporting TamguIteration* Tamguprimemapf::Newiteration(bool direction) {
    return new TamguIterationprimemapf(this, direction);
}


Exporting Tamgu* Tamguprimemapf::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamguprimemapf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<double, Tamgu*>::iterator it;
    
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
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
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
            return globalTamgu->Providefloat(it->first);
    }

    return aNULL;

}



Exporting void Tamguprimemapf::Cleanreference(short inc) {
    Locking* _lock = _getlock(this);
    prime_hash<double, Tamgu*>::iterator it;
    for (it=values.begin();it!=values.end();it++)
        it->second->Removecontainerreference( inc);
    _cleanlock(_lock);
}

Exporting void Tamguprimemapf::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect = false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    prime_hash<double, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(inc);
    _cleanlock(_lock);
    
    loopmark=false;
}

Exporting void Tamguprimemapf::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect = false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    prime_hash<double, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(1);
    _cleanlock(_lock);
    
    loopmark=false;
}

static void resetMap(Tamguprimemapf* kmap, short inc) {
    kmap->reference -= inc;
    
    Locking* _lock = _getlock(kmap);
    prime_hash<double, Tamgu*>& values = kmap->values;
    if (values.size() == 0)
        return;
    
    prime_hash<double, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(inc);
    _cleanlock(_lock);
}

Exporting void Tamguprimemapf::Resetreference(short inc) {
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


Exporting Tamgu* Tamguprimemapf::Push(Tamgu* k, Tamgu* v) {
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

Exporting Tamgu* Tamguprimemapf::Pop(Tamgu* kkey) {
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

Exporting void Tamguprimemapf::Clear() {
    Locking _lock(this);
    prime_hash<double, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamguprimemapf::String() {
    Locking _lock(this);
    if (loopmark)
        return("{...}");
    TamguCircular _c(this);
    stringstream res;
    prime_hash<double, Tamgu*>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        sx = it->second->StringToDisplay();
        if (!it->second->isString())
            res << sx;
        else
            stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapf::JSonString() {
    Locking _lock(this);
    if (loopmark)
        return("");
    TamguCircular _c(this);
    stringstream res;
    prime_hash<double, Tamgu*>::iterator it;
    res << "{";
    bool beg = true;

    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":" << it->second->JSonString();
    }
    res << "}";
    return res.str();
}


Exporting long Tamguprimemapf::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamguprimemapf::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamguprimemapf::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamguprimemapf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemapf::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamguprimemapf::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                Push(nb, *it);
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        Locking* _lock = _getlock(this);
        Clear();
        if (ke->Type() == Tamguprimemapf::idtype) {
            Tamguprimemapf* kmap = (Tamguprimemapf*)ke;
            //We copy all values from ke to this
            prime_hash<double, Tamgu*>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                Push(it->first, it->second);
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


Exporting Tamgu* Tamguprimemapf::Get(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            prime_hash<double, Tamgu*>::iterator it;
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
        Tamguprimemapf* kmap = new Tamguprimemapf;
        key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        prime_hash<double, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<double, Tamgu*>::iterator itr = values.end();
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

Exporting Tamgu* Tamguprimemapf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemapf* m = (Tamguprimemapf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<double, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapf::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamguprimemapf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
        hmap<double, Tamgu*> keys;

        prime_hash<double, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
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
        res = (Tamguprimemapf*)Atom(true);

    prime_hash<double, Tamgu*>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapf*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    for (it = values.begin(); it != values.end(); it++)
        it->second->orset(b, true);
    return res;

}

Exporting Tamgu* Tamguprimemapf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamguprimemapf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamguprimemapf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemapf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<double, Tamgu*>::iterator it;
    Tamguprimemapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapf;
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
        res = (Tamguprimemapf*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->power(b, true);
    return res;
}




Exporting Tamgu* Tamguprimemapf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    prime_hash<double, Tamgu*>::iterator it;
    
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

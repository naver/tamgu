/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemap.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamguprimemap.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapMethod>  Tamguprimemap::methods;
Exporting hmap<string, string> Tamguprimemap::infomethods;
Exporting bin_hash<unsigned long> Tamguprimemap::exported;

Exporting short Tamguprimemap::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemap::AddMethod(TamguGlobal* global, string name, primemapMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamguprimemap::Setidtype(TamguGlobal* global) {
        Tamguprimemap::idtype = global->Getid("primemap");
    }

   bool Tamguprimemap::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamguprimemap::idtype = global->Getid("primemap");

    Tamguprimemap::AddMethod(global, "clear", &Tamguprimemap::MethodClear, P_NONE, "clear(): clear the container.");

    Tamguprimemap::AddMethod(global, "items", &Tamguprimemap::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    Tamguprimemap::AddMethod(global, "invert", &Tamguprimemap::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemap::AddMethod(global, "find", &Tamguprimemap::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");



    Tamguprimemap::AddMethod(global, "join", &Tamguprimemap::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemap::AddMethod(global, "test", &Tamguprimemap::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemap::AddMethod(global, "keys", &Tamguprimemap::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemap::AddMethod(global, "values", &Tamguprimemap::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemap::AddMethod(global, "sum", &Tamguprimemap::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguprimemap::AddMethod(global, "product", &Tamguprimemap::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguprimemap::AddMethod(global, "pop", &Tamguprimemap::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemap::AddMethod(global, "merge", &Tamguprimemap::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamguprimemap::idtype] = new Tamguprimemap(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("sprimemap")] = new Tamguprimemap(global);
    global->newInstance[global->Getid("primemaps")] = new Tamguprimemap(global);

    global->RecordMethods(global->Getid("primemaps"), Tamguprimemap::exported);
    global->RecordMethods(global->Getid("sprimemap"), Tamguprimemap::exported);
    #endif
    global->RecordMethods(Tamguprimemap::idtype, Tamguprimemap::exported);

    return true;
}


Exporting TamguIteration* Tamguprimemap::Newiteration(bool direction) {
    return new TamguIterationprimemap(this, direction);
}

Exporting Tamgu* Tamguprimemap::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<string, Tamgu*>::iterator it;

    Tamgu* a = callfunc->Evaluate(0, context, idthread);

    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
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
            return globalTamgu->Providestring(it->first);
    }

    return aNULL;

}

Exporting Tamgu* Tamguprimemap::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    string val = a->String();

     if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isString()) {
        Locking _lock(this);
        if (values.find(val) != values.end())
            return globalTamgu->Providestring(val);
        return aNOELEMENT;
    }

    Locking _lock(this);
    if (values.find(val) != values.end())
        return aTRUE;

    return aFALSE;

}

Exporting void Tamguprimemap::Cleanreference(short inc) {
    Locking* _lock = _getlock(this);
    prime_hash<string, Tamgu*>::iterator it;
    for (it=values.begin();it!=values.end();it++)
        it->second->Removecontainerreference( inc);
    _cleanlock(_lock);
}

Exporting void Tamguprimemap::Setreference(short inc) {
    if (loopmark)
        return;
    
    reference += inc;
    protect = false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    prime_hash<string, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(inc);
    _cleanlock(_lock);
    
    loopmark=false;
}

Exporting void Tamguprimemap::Setreference() {
    if (loopmark)
        return;
    
    ++reference;
    protect = false;
    loopmark=true;
    
    Locking* _lock = _getlock(this);
    prime_hash<string, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(1);
    _cleanlock(_lock);
    
    loopmark=false;
}

static void resetMap(Tamguprimemap* kmap, short inc) {
    kmap->reference -= inc;

    Locking* _lock = _getlock(kmap);
    prime_hash<string, Tamgu*>& values = kmap->values;
    if (values.size() == 0)
        return;

    prime_hash<string, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(inc);
    _cleanlock(_lock);
}

Exporting void Tamguprimemap::Resetreference(short inc) {
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


Exporting Tamgu* Tamguprimemap::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    string s = k->String();
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

Exporting Tamgu* Tamguprimemap::Pop(Tamgu* kkey) {
    string k = kkey->String();
    Locking _lk(this);
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Removereference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamguprimemap::Clear() {
    Locking _lock(this);
    prime_hash<string, Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamguprimemap::String() {
    Locking _lock(this);
    if (loopmark)
        return("{...}");
    TamguCircular _c(this);
    stringstream res;
    prime_hash<string, Tamgu*>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        stringing(res, sx);
        res << ":";
        sx = it->second->StringToDisplay();
        if (!it->second->isString())
            res << sx;
        else
            stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamguprimemap::JSonString() {
    Locking _lock(this);
    if (loopmark)
        return("");
    TamguCircular _c(this);
    stringstream res;
    prime_hash<string, Tamgu*>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        jstringing(res, sx);
        res << ":" << it->second->JSonString();
    }
    res << "}";
    return res.str();
}


Exporting long Tamguprimemap::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamguprimemap::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamguprimemap::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamguprimemap::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemap::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamguprimemap::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
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
                Push(itr->Keystring(), itr->Value());
            itr->Release();
            return aTRUE;
        }

        char ch[20];
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            Clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(ch, 20, "%ld", nb);
                Push(ch, ke->getvalue(it));
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
                sprintf_s(ch, 20, "%ld", nb);
                Push(ch, *it);
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        Locking* _lock = _getlock(this);
        Clear();
        if (ke->Type() == Tamguprimemap::idtype) {
            Tamguprimemap* kmap = (Tamguprimemap*)ke;
            //We copy all values from ke to this
            prime_hash<string, Tamgu*>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                Push(it->first, it->second);
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            Tamgu* a;
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                a=itr->IteratorValue();
                a=a->Atom();
                values[itr->Keystring()] = a;
                a->Addreference(reference+1);
            }
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }

    Push(idx->String(), ke);
    return aTRUE;
}


Exporting Tamgu* Tamguprimemap::Get(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            prime_hash<string, Tamgu*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providestring(it->first));
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
        Tamguprimemap* kmap = new Tamguprimemap;
        key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        prime_hash<string, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<string, Tamgu*>::iterator itr = values.end();
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

    string skey = key->String();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamguprimemap::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemap* m = (Tamguprimemap*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<string, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemap::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamguprimemap* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        hmap<string, Tamgu*> keys;

        prime_hash<string, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
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
        res = (Tamguprimemap*)Atom(true);

    prime_hash<string, Tamgu*>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemap::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemap*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    for (it = values.begin(); it != values.end(); it++)
        it->second->orset(b, true);
    return res;

}

Exporting Tamgu* Tamguprimemap::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]->same(v) == aTRUE)
                res->Push(itr->Keystring(), v);
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemap::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemap::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemap::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemap::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamguprimemap::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamguprimemap::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemap::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamguprimemap::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<string, Tamgu*>::iterator it;
    Tamguprimemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemap;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemap*)Atom(true);

    for (it = values.begin(); it != values.end(); it++)
        it->second->power(b, true);
    return res;
}




Exporting Tamgu* Tamguprimemap::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);

    prime_hash<string, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<string> keys;

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

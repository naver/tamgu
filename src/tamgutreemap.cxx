/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemap.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgutreemap.h"
#include "tamgulist.h"
#include "tamguivector.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<treemapMethod>  Tamgutreemap::methods;
Exporting hmap<string, string> Tamgutreemap::infomethods;
Exporting bin_hash<unsigned long> Tamgutreemap::exported;

Exporting short Tamgutreemap::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemap::AddMethod(TamguGlobal* global, string name, treemapMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutreemap::Setidtype(TamguGlobal* global) {
        Tamgutreemap::idtype = global->Getid("treemap");
    }

   bool Tamgutreemap::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();



    Tamgutreemap::idtype = global->Getid("treemap");

    Tamgutreemap::AddMethod(global, "items", &Tamgutreemap::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    Tamgutreemap::AddMethod(global, "invert", &Tamgutreemap::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemap::AddMethod(global, "find", &Tamgutreemap::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");



    Tamgutreemap::AddMethod(global, "join", &Tamgutreemap::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemap::AddMethod(global, "test", &Tamgutreemap::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemap::AddMethod(global, "keys", &Tamgutreemap::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemap::AddMethod(global, "values", &Tamgutreemap::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemap::AddMethod(global, "sum", &Tamgutreemap::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemap::AddMethod(global, "product", &Tamgutreemap::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemap::AddMethod(global, "pop", &Tamgutreemap::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemap::AddMethod(global, "merge", &Tamgutreemap::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    
    global->newInstance[Tamgutreemap::idtype] = new Tamgutreemap(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("streemap")] = new Tamgutreemap(global);
    global->newInstance[global->Getid("treemaps")] = new Tamgutreemap(global);

    global->RecordMethods(global->Getid("treemaps"),Tamgutreemap::exported);
    global->RecordMethods(global->Getid("streemap"),Tamgutreemap::exported);
    #endif
    global->RecordMethods(Tamgutreemap::idtype,Tamgutreemap::exported);

    return true;
}

Exporting TamguIteration* Tamgutreemap::Newiteration(bool direction) {
    return new TamguIterationtreemap(this, direction);
}

Exporting Tamgu* Tamgutreemap::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    Tamgu* a = callfunc->Evaluate(0, context, idthread);
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second->same(a) == aTRUE)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
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
            return globalTamgu->Providestring(it.first);
    }
    return aNULL;
}

Exporting Tamgu* Tamgutreemap::in(Tamgu* context, Tamgu* a, short idthread) {
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


Exporting void Tamgutreemap::Cleanreference(short inc) {
    Locking* _lock = _getlock(this);
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    
    _cleanlock(_lock);
}

Exporting void Tamgutreemap::Setreference(short inc) {
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

Exporting void Tamgutreemap::Setreference() {
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

static void resetMap(Tamgutreemap* kmap, short inc) {
    kmap->reference -= inc;
    
    Locking* _lock = _getlock(kmap);
    
    map<string, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        _cleanlock(_lock);
        return;
    }
    
    for (auto& itx : values)
        itx.second->Removereference(inc);
    _cleanlock(_lock);
}

Exporting void Tamgutreemap::Resetreference(short inc) {
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

Exporting Tamgu* Tamgutreemap::Push(Tamgu* k, Tamgu* v) {
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

Exporting Tamgu* Tamgutreemap::Pop(Tamgu* kkey) {
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

Exporting void Tamgutreemap::Clear() {
    Locking _lock(this);

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamgutreemap::String() {
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
        sx = it.first;
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

Exporting string Tamgutreemap::JSonString() {
    Locking _lock(this);
    if (loopmark)
        return("");
    TamguCircular _c(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        jstringing(res, sx);
        res << ":" << it.second->JSonString();
    }
    res << "}";
    return res.str();
}

//Basic operations
Exporting long Tamgutreemap::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting long Tamgutreemap::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgutreemap::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgutreemap::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgutreemap::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}



Exporting Tamgu*  Tamgutreemap::Put(Tamgu* idx, Tamgu* ke, short idthread) {
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

            for (auto& it : kvect->values) {
                sprintf_s(ch, 20, "%ld", nb);
                Push(ch, it);
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        Locking* _lock = _getlock(this);
        Clear();
        if (ke->Type() == Tamgutreemap::idtype) {
            Tamgutreemap* kmap = (Tamgutreemap*)ke;
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
                values[itr->Keystring()] = a;
                a->Addreference(reference+1);
            }
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Push(STR(idx->String()), ke);
    return aTRUE;
}


Exporting Tamgu* Tamgutreemap::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            map<string, Tamgu*>::iterator it;
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
        Tamgutreemap* kmap = new Tamgutreemap;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        map<string, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<string, Tamgu*>::iterator itr= values.end();
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

    key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
    
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

Exporting Tamgu* Tamgutreemap::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemap* m = (Tamgutreemap*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<string, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemap::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgutreemap* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
        hmap<string, Tamgu*> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemap*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemap::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemap::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemap::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemap* res;
    if (b->isMapContainer()) {
        map<string, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemap;
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
        res = (Tamgutreemap*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}


Exporting Tamgu* Tamgutreemap::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    map<string, Tamgu*>::iterator it;
    
    Tamgu* a;
    vector<string> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    for (long i = 0; i < keys.size(); i++) {

        var->storevalue(keys[i]);

        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

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

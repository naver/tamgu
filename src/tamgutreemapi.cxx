/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapi.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamgutreemapi.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapiMethod>  Tamgutreemapi::methods;
Exporting hmap<string, string> Tamgutreemapi::infomethods;
Exporting bin_hash<unsigned long> Tamgutreemapi::exported;

Exporting short Tamgutreemapi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapi::AddMethod(TamguGlobal* global, string name, treemapiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutreemapi::Setidtype(TamguGlobal* global) {
    Tamgutreemapi::InitialisationModule(global,"");
}


   bool Tamgutreemapi::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgutreemapi::idtype = global->Getid("treemapi");

    Tamgutreemapi::AddMethod(global, "clear", &Tamgutreemapi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapi::AddMethod(global, "invert", &Tamgutreemapi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapi::AddMethod(global, "find", &Tamgutreemapi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapi::AddMethod(global, "items", &Tamgutreemapi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapi::AddMethod(global, "join", &Tamgutreemapi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapi::AddMethod(global, "test", &Tamgutreemapi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapi::AddMethod(global, "keys", &Tamgutreemapi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapi::AddMethod(global, "values", &Tamgutreemapi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapi::AddMethod(global, "sum", &Tamgutreemapi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemapi::AddMethod(global, "product", &Tamgutreemapi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemapi::AddMethod(global, "pop", &Tamgutreemapi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapi::AddMethod(global, "merge", &Tamgutreemapi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgutreemapi::idtype] = new Tamgutreemapi(global);
        
        global->RecordMethods(Tamgutreemapi::idtype, Tamgutreemapi::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgutreemapi::Newiteration(bool direction) {
    return new TamguIterationtreemapi(this, direction);
}


Exporting Tamgu* Tamgutreemapi::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    long val = a->Integer();

     if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.find(val) != values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isNumber()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Provideint(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgutreemapi::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
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
            return globalTamgu->Provideint(it.first);
    }
    return aNULL;
}



Exporting void Tamgutreemapi::Cleanreference(short inc) {
    Locking* _lock = _getlock(this);
    for (auto& a : values)
        a.second->Removecontainerreference(inc);
    
    _cleanlock(_lock);
}

Exporting void Tamgutreemapi::Setreference(short inc) {
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

Exporting void Tamgutreemapi::Setreference() {
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

static void resetMap(Tamgutreemapi* kmap, short inc) {
    kmap->reference -= inc;
    
    Locking* _lock = _getlock(kmap);
    
    map<long, Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        _cleanlock(_lock);
        return;
    }
    
    for (auto& itx : values)
        itx.second->Removereference(inc);
    _cleanlock(_lock);
}

Exporting void Tamgutreemapi::Resetreference(short inc) {
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


Exporting Tamgu* Tamgutreemapi::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    long s = k->Integer();
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

Exporting Tamgu* Tamgutreemapi::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    Locking _lk(this);
    if (values.find(k) != values.end()) {
        kkey = values[k];
        values.erase(k);
        kkey->Removereference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgutreemapi::Clear() {
    Locking _lock(this);

    for (auto& itx : values)
        itx.second->Removereference(reference + 1);
    values.clear();
}



Exporting string Tamgutreemapi::String() {
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

Exporting string Tamgutreemapi::JSonString() {
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


Exporting long Tamgutreemapi::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgutreemapi::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgutreemapi::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgutreemapi::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgutreemapi::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamgutreemapi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
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
        if (ke->Type() == Tamgutreemapi::idtype) {
            Tamgutreemapi* kmap = (Tamgutreemapi*)ke;
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
                values[itr->Keyinteger()] = a;
                a->Addreference(reference+1);
            }
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Push(idx->Integer(), ke);
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapi::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            map<long, Tamgu*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideint(it->first));
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
        Tamgutreemapi* kmap = new Tamgutreemapi;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        map<long, Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<long, Tamgu*>::iterator itr = values.end();
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

    long skey = key->Integer();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgutreemapi::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapi* m = (Tamgutreemapi*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<long, Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second->same(values[it->first]) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapi::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgutreemapi* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        hmap<long, Tamgu*> keys;

        for (auto& it : values)
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

        for (auto& a : keys)
            res->Push(a.first,a.second);

        return res;


    }
    if (itself)
        res = this;
    else
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : values)
        it.second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapi*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    for (auto& it : res->values)
        it.second->orset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemapi;
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
        res = (Tamgutreemapi*)Atom(true);
    for (auto& it : res->values)
        it.second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values)
        it.second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values)
        it.second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values)
        it.second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemapi::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values) {
        r = it.second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgutreemapi::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values)
        it.second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgutreemapi::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapi* res;
    if (b->isMapContainer()) {
        map<long, Tamgu*>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapi;
        Tamgu* v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgutreemapi*)Atom(true);

    for (auto& it : res->values)
        it.second->power(b, true);
    return res;
}




Exporting Tamgu* Tamgutreemapi::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    map<long, Tamgu*>::iterator it;
    
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
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;

}

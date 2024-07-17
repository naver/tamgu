/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubinmap.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
//#prime
#include "tamgubinmap.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<binmapMethod>  Tamgubinmap::methods;
static ThreadLock classlock;

Exporting short Tamgubinmap::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubinmap::AddMethod(TamguGlobal* global, string name, binmapMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgubinmap::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgubinmap::methods.isEmpty())
    Tamgubinmap::InitialisationModule(global,"");
}

bool Tamgubinmap::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgubinmap::idtype = a_binmap;

    Tamgubinmap::AddMethod(global, "clear", &Tamgubinmap::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgubinmap::AddMethod(global, "invert", &Tamgubinmap::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgubinmap::AddMethod(global, "find", &Tamgubinmap::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgubinmap::AddMethod(global, "items", &Tamgubinmap::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgubinmap::AddMethod(global, "join", &Tamgubinmap::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgubinmap::AddMethod(global, "test", &Tamgubinmap::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgubinmap::AddMethod(global, "keys", &Tamgubinmap::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgubinmap::AddMethod(global, "values", &Tamgubinmap::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgubinmap::AddMethod(global, "sum", &Tamgubinmap::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgubinmap::AddMethod(global, "product", &Tamgubinmap::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgubinmap::AddMethod(global, "pop", &Tamgubinmap::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgubinmap::AddMethod(global, "merge", &Tamgubinmap::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgubinmap::idtype] = new Tamgubinmap(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("iprimemap")] = new Tamgubinmap(global);

    global->RecordCompatibilities(global->Getid("iprimemap"));
    #endif
    global->RecordCompatibilities(Tamgubinmap::idtype);

    return true;
}


Exporting TamguIteration* Tamgubinmap::Newiteration(bool direction) {
    return new TamguIterationbinmap(this, direction);
}


Exporting Tamgu* Tamgubinmap::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    ushort val = a->Short();

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.check(val))
            v->values.push_back(val);

        return v;
    }
    
    if (context->isNumber()) {
        locking();
        if (values.check(val)) {
            unlocking();
            return globalTamgu->Provideint(val);
        }
        unlocking();
        return aNOELEMENT;;
    }
    
    locking();
    if (values.check(val)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;

}


Exporting Tamgu* Tamgubinmap::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebin_hash<Tamgu*>::iterator it;
    
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
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
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
            return globalTamgu->Provideint(it->first);
    }

    return aNULL;

}

Exporting void Tamgubinmap::Cleanreference(short inc) {
    locking();
    basebin_hash<Tamgu*>::iterator it;
    for (it=values.begin();it!=values.end();it++)
        it->second->Removecontainerreference(inc);
    unlocking();
}

Exporting void Tamgubinmap::Setreference(short inc) {
    locking();

    reference += inc;
    protect = false;
    
    basebin_hash<Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(investigate,inc);

    unlocking();
}

Exporting void Tamgubinmap::Setreference() {
    locking();

    ++reference;
    protect = false;
    
    basebin_hash<Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Addreference(investigate,1);
    
    unlocking();
}


static void resetMap(Tamgubinmap* kmap, short inc) {
    kmap->reference -= inc;

    kmap->locking();
    basebin_hash<Tamgu*>& values = kmap->values;
    if (values.size() == 0) {
        kmap->unlocking();
        return;
    }

    basebin_hash<Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(inc);
    
    kmap->unlocking();
}

Exporting void Tamgubinmap::Resetreference(short inc) {
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


Exporting Tamgu* Tamgubinmap::Push(Tamgu* k, Tamgu* v) {
    locking();
    ushort s = k->Short();
    if (values.check(s)) {
        Tamgu* kval = values.get(s);
        values.erase(s);
        kval->Removereference(reference + 1);
    }

    v = v->Atom();
    values[s] = v;
    v->Addreference(investigate,reference+1);
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgubinmap::Pop(Tamgu* kkey) {
    ushort k = kkey->Short();
    Locking _lk(this);
    if (values.check(k)) {
        kkey = values.get(k);
        values.erase(k);
        kkey->Removereference(reference + 1);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgubinmap::Clear() {
    locking();
    basebin_hash<Tamgu*>::iterator itx;
    for (itx = values.begin(); itx != values.end(); itx++)
        itx->second->Removereference(reference + 1);
    values.clear();
    unlocking();
}



Exporting string Tamgubinmap::String() {
    if (!lockingmark())
        return("{...}");

    stringstream res;
    basebin_hash<Tamgu*>::iterator it;
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
    unlockingmark();
    return res.str();
}

Exporting string Tamgubinmap::JSonString() {
    if (!lockingmark())
        return("");

    stringstream res;
    basebin_hash<Tamgu*>::iterator it;
    res << "{";
    bool beg = true;

    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":" << it->second->JSonString();
    }
    res << "}";
    unlockingmark();
    return res.str();
}


Exporting long Tamgubinmap::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


short Tamgubinmap::Short() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


float Tamgubinmap::Decimal() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamgubinmap::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamgubinmap::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamgubinmap::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgubinmap::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamgubinmap::Put(Tamgu* idx, Tamgu* ke, short idthread) {


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
                Push((ushort)itr->Keyshort(), itr->Value());
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

        Clear();
        
        locking();
        if (ke->Type() == Tamgubinmap::idtype) {
            Tamgubinmap* kmap = (Tamgubinmap*)ke;
            //We copy all values from ke to this
            basebin_hash<Tamgu*>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                Push(it->first, it->second);
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            Tamgu* a;
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                a=itr->IteratorValue();
                a=a->Atom();
                values[itr->Keyshort()] = a;
                a->Addreference(investigate,reference+1);
            }
            itr->Release();
        }
        unlocking();
        ke->Release();
        return aTRUE;
    }

    Push(idx->Getshort(idthread), ke);
    return aTRUE;
}

Exporting Tamgu* Tamgubinmap::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            basebin_hash<Tamgu*>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideint(it->first));
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
        Tamgubinmap* kmap = new Tamgubinmap;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort vright = keyright->Short();
        basebin_hash<Tamgu*>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebin_hash<Tamgu*>::iterator itr = values.end();
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

    short skey = idx->Getshort(idthread);
    locking();
    if (!values.check(skey)) {
        unlocking();
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    
    contextualpattern = values.get(skey);
    unlocking();
    return contextualpattern;
}

Exporting Tamgu* Tamgubinmap::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgubinmap* m = (Tamgubinmap*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebin_hash<Tamgu*>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (!values.check(it->first))
            return aFALSE;
        if (it->second->same(values.get(it->first)) == aFALSE)
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgubinmap::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgubinmap* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        hmap<short, Tamgu*> keys;

        basebin_hash<Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyshort();
            if (keys.find(v) == keys.end())
                keys[v] = itr->IteratorValue();
            else {
                if (values[v]->same(itr->IteratorValue()) == aTRUE)
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->Push(a.first, a.second);

        return res;

    }


    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second->xorset(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++)
        it->second->orset(b, true);
    return res;

}

Exporting Tamgu* Tamgubinmap::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        Tamgu* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey=itr->Keyshort();
            if (values.check(ikey) && values.get(ikey)->same(v) == aTRUE)
                res->Push(ikey, v);
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++)
        it->second->andset(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        Tamgu* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                res->values[ikey]->plus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++)
        it->second->plus(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        Tamgu* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                res->values[ikey]->minus(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++)
        it->second->minus(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        Tamgu* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                res->values[ikey]->multiply(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++)
        it->second->multiply(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                r = res->values[ikey]->divide(v, true);
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
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->divide(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgubinmap::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    Tamgu* v;
    Tamgu* r;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                r = res->values[ikey]->mod(v, true);
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
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++) {
        r = it->second->mod(b, true);
        if (r->isError()) {
            res->Release();
            return r;
        }
    }

    return res;
}

Exporting Tamgu* Tamgubinmap::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        Tamgu* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                res->values[ikey]->shiftright(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftright(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        Tamgu* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                res->values[ikey]->shiftleft(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second->shiftleft(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmap* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmap;
        Tamgu* v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->IteratorValue();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->Push(ikey, values.get(ikey));
                res->values[ikey]->power(v, true);
            }

        }
        
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmap*)Atom(true);

    basebin_hash<Tamgu*>::iterator it;

    for (it = values.begin(); it != values.end(); it++)
        it->second->power(b, true);
    return res;
}

Exporting Tamgu* Tamgubinmap::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    locking();
    basebin_hash<Tamgu*>::iterator it;
    vector<short> keys;
    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);


    Tamgu* a;
    for (long i = 0; i < keys.size(); i++) {
        var->storevalue(keys[i]);

        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            unlocking();
            return a;
        }

        a->Release();
    }

    unlocking();
    return this;
}

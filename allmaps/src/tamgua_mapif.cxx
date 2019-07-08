/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapif.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapif.h"
#include "tamgulist.h"
#include "tamguivector.h"


    //We need to declare once again our local definitions.
Exporting basebin_hash<a_mapifMethod>  Tamgua_mapif::methods;
Exporting hmap<string, string> Tamgua_mapif::infomethods;
Exporting bin_hash<unsigned long> Tamgua_mapif::exported;

Exporting short Tamgua_mapif::idtype = 0;

    //-------------------------------------------------------------------------
    //MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapif::AddMethod(TamguGlobal* global, string name, a_mapifMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_mapif::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgua_mapif::idtype = global->Getid("a_mapif");
    
    Tamgua_mapif::AddMethod(global, "clear", &Tamgua_mapif::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapif::AddMethod(global, "invert", &Tamgua_mapif::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapif::AddMethod(global, "find", &Tamgua_mapif::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamgua_mapif::AddMethod(global, "items", &Tamgua_mapif::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamgua_mapif::AddMethod(global, "join", &Tamgua_mapif::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapif::AddMethod(global, "test", &Tamgua_mapif::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapif::AddMethod(global, "keys", &Tamgua_mapif::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapif::AddMethod(global, "values", &Tamgua_mapif::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapif::AddMethod(global, "sum", &Tamgua_mapif::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_mapif::AddMethod(global, "product", &Tamgua_mapif::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_mapif::AddMethod(global, "pop", &Tamgua_mapif::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapif::AddMethod(global, "merge", &Tamgua_mapif::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    Tamgua_mapif::AddMethod(global, "check", &Tamgua_mapif::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapif::AddMethod(global, "compact", &Tamgua_mapif::MethodCompact, P_NONE, "compact(): remove empty chunks.");
    
    global->newInstance[Tamgua_mapif::idtype] = new Tamgua_mapif(global);
    global->RecordMethods(Tamgua_mapif::idtype, Tamgua_mapif::exported);
    
    return true;
}


Exporting TamguIteration* Tamgua_mapif::Newiteration(bool direction) {
    return new TamguIterationa_mapif(this, direction);
}


Exporting Tamgu* Tamgua_mapif::in(Tamgu* context, Tamgu* a, short idthread) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    long val = a->Integer();
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        if (values.check(val))
            v->values.push_back(val);
        
        return v;
    }
    
    if (context->isNumber()) {
        if (values.check(val))
            return globalTamgu->Provideint(val);
        return aNOELEMENT;
    }
    
    if (values.check(val))
        return aTRUE;
    
    return aFALSE;
    
}

Exporting Tamgu* Tamgua_mapif::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
        //Three cases along the container type...
        //It is a Boolean, we expect false or true
        //It is an integer, we expect a position in v
        //It is a container, we are looking for all positions...
    
    TamguIterationa_mapif it(this, false);
    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second == val)
            return globalTamgu->Provideint(it.first);
    }
    
    return aNULL;
    
}

Exporting Tamgu* Tamgua_mapif::Push(Tamgu* k, Tamgu* v) {
    long s = k->Integer();
    values.set(s, v->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapif::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapif::Clear() {
    values.clear();
}

Exporting string Tamgua_mapif::String() {
    stringstream res;
    TamguIterationa_mapif it(this, false);
    res << "{";
    bool beg = true;
    
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first;
        res << ":";
        res << it.second;
    }
    res << "}";
    return res.str();
}

Exporting string Tamgua_mapif::JSonString() {
    stringstream res;
    TamguIterationa_mapif it(this, false);
    res << "{";
    bool beg = true;
    
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first;
        res << ":";
        res << it.second;
    }
    res << "}";
    return res.str();
}


Exporting long Tamgua_mapif::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapif::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapif::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapif::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


    //Basic operations
Exporting long Tamgua_mapif::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapif::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgua_mapif::idtype) {
            values = ((Tamgua_mapif*)ke)->values;
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.set(itr->Keyinteger(),  itr->Valuefloat());
            itr->Release();
            return aTRUE;
        }
        
        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values.set(nb, ke->getfloat(it));
                nb++;
            }
            return aTRUE;
        }
        
        if (ke->Type() == a_list) {
            Locking _lock((TamguObject*)ke);
            Tamgulist* kvect = (Tamgulist*)ke;
            values.clear();
            long nb = 0;
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                values.set(nb, (*it)->Float());
                nb++;
            }
            return aTRUE;
        }
        
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        
        if (ke->Type() == Tamgua_mapif::idtype) {
                //We copy all values from ke to this
            values = ((Tamgua_mapif*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values.set(itr->Keyinteger(),  itr->Valuefloat());
            itr->Release();
        }
        
        ke->Release();
        return aTRUE;
    }
    
    values.set(idx->Integer(),  ke->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapif::Get(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
            //particular case, the contextualpattern is a vector, which means that we expect a set of keys
            //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapif it(this, false);
            for (it.Begin(); it.End() == aFALSE; it.Next())
                vect->Push(globalTamgu->Provideint(it.first));
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
        TamguIterationa_mapif it(this, false);
        Tamgua_mapif* kmap = new Tamgua_mapif;
        key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Get(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapif itr(this, false);
        
        if (key == aNULL)
            itr.Begin();
        
        key->Release();
        itr.end = true;
        if (keyright != aNULL) {
            itr.find(vright);
            if (itr.End()) {
                keyright->Release();
                return kmap;
            }
        }
        keyright->Release();
        for (; itr.End() == aFALSE; itr.Next()) {
            kmap->values.set(it.first, it.second);
            if (it.compare(itr))
                return kmap;
        }
        
        if (!itr.end)
            kmap->values.clear();
        return kmap;
        
    }
    
    key = ((TamguIndex*)idx)->left->Get(aNULL, aNULL, idthread);
    
    if (key == aNULL) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    
    long skey = key->Integer();
    key->Release();
    
    if (!values.check(skey)) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    
    double vl;
    values.get(skey, vl);
    return globalTamgu->Providefloat(vl);
}

Exporting Tamgu* Tamgua_mapif::same(Tamgu* a) {
    
    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);
    
    Tamgua_mapif* m = (Tamgua_mapif*)a;
    
    Locking* _lock = _getlockif(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapif it(m, false);
    double vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        if (!values.check(it.first)) {
            _cleanlockif(_lock);
            return aFALSE;
        }
        values.get(it.first, vl);
        if (it.second != vl) {
            _cleanlockif(_lock);
            return aFALSE;
        }
    }
    _cleanlockif(_lock);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapif::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlockif(b);
    
    Tamgua_mapif* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        hmap<long, double> keys;
        
        TamguIterationa_mapif it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second;
        
        long v;
        double vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuefloat();
            else {
                if (values.check(v)) {
                    values.get(v, vl);
                    if (vl == itr->Valuefloat())
                        keys.erase(v);
                }
            }
        }
        itr->Release();
        
        for (auto& a : keys)
            res->values.set(a.first, a.second);
        
        
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    TamguIterationa_mapif it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleXor(it.second,v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlockif(_lock);
        return res;
    }
    
    TamguIterationa_mapif it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleOr(it.second,v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        double v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl == v)
                    res->values.set(ikey, v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    TamguIterationa_mapif it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleAnd(it.second,v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        double v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl + v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapif it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second + v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        double v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl - v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapif it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second - v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::multiply(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        double v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl * v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapif it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second * v);
    _cleanlockif(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapif::divide(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        double v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl / v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapif it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second / v);
    _cleanlockif(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapif::mod(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        long v;
        double vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, (BLONG)vl % v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapif it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  (BLONG)it.second % v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::shiftright(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    Tamgua_mapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        long v;
        double vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, DoubleShiftright(vl, v));
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    TamguIterationa_mapif it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleShiftright(it.second, v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::shiftleft(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    Tamgua_mapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        long v;
        double vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, DoubleShiftleft(vl, v));
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    TamguIterationa_mapif it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleShiftleft(it.second, v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapif::power(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapif;
        double v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, pow(vl, v));
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapif*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapif it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  pow(it.second, v));
    _cleanlockif(_lock);
    return res;
}



Exporting Tamgu* Tamgua_mapif::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Get(context, aNULL, idthread);
    
    vector<long> keys;
    
    TamguIterationa_mapif itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        keys.push_back(itr.first);
    
    
    Tamgu* a;
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

double Tamgua_mapif::Sum() {
    double v = 0;
    TamguIterationa_mapif itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v += itr.second;
    return v;
}

double Tamgua_mapif::Product() {
    double v = 1;
    TamguIterationa_mapif itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v *= itr.second;
    return v;
}

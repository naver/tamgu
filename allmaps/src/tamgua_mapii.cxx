/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapii.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapii.h"
#include "tamgulist.h"
#include "tamguivector.h"
#include "tamguivector.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapiiMethod>  Tamgua_mapii::methods;

Exporting short Tamgua_mapii::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapii::AddMethod(TamguGlobal* global, string name, a_mapiiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapii::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgua_mapii::InitialisationModule(global,"");
}

bool Tamgua_mapii::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapii::idtype = global->Getid("a_mapii");

    Tamgua_mapii::AddMethod(global, "clear", &Tamgua_mapii::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapii::AddMethod(global, "invert", &Tamgua_mapii::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapii::AddMethod(global, "find", &Tamgua_mapii::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapii::AddMethod(global, "items", &Tamgua_mapii::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapii::AddMethod(global, "join", &Tamgua_mapii::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapii::AddMethod(global, "test", &Tamgua_mapii::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapii::AddMethod(global, "keys", &Tamgua_mapii::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapii::AddMethod(global, "values", &Tamgua_mapii::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapii::AddMethod(global, "sum", &Tamgua_mapii::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_mapii::AddMethod(global, "product", &Tamgua_mapii::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_mapii::AddMethod(global, "pop", &Tamgua_mapii::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapii::AddMethod(global, "merge", &Tamgua_mapii::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapii::AddMethod(global, "check", &Tamgua_mapii::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapii::AddMethod(global, "compact", &Tamgua_mapii::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapii::idtype] = new Tamgua_mapii(global);
    global->RecordCompatibilities(Tamgua_mapii::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapii::Newiteration(bool direction) {
    return new TamguIterationa_mapii(this, direction);
}


Exporting Tamgu* Tamgua_mapii::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapii::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapii it(this, false);

    long val = callfunc->Evaluate(0, context, idthread)->Integer();

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

Exporting Tamgu* Tamgua_mapii::Push(Tamgu* k, Tamgu* v) {
    long s = k->Integer();
    values.set(s, v->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapii::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapii::Clear() {
    values.clear();
}

Exporting string Tamgua_mapii::String() {
    stringstream res;
    TamguIterationa_mapii it(this, false);
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

Exporting string Tamgua_mapii::JSonString() {
    stringstream res;
    TamguIterationa_mapii it(this, false);
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


Exporting long Tamgua_mapii::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapii::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapii::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapii::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapii::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapii::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapii::idtype) {
            values = ((Tamgua_mapii*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.set(itr->Keyinteger(),  itr->Valueinteger());
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values.set(nb, ke->getinteger(it));
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
                values.set(nb, (*it)->Integer());
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapii::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapii*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values.set(itr->Keyinteger(),  itr->Valueinteger());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    values.set(idx->Getinteger(idthread),  ke->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapii::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapii it(this, false);
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
        TamguIterationa_mapii it(this, false);
        Tamgua_mapii* kmap = new Tamgua_mapii;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapii itr(this, false);

        if (key == aNULL)
            itr.Begin();
        
        key->Release();
        itr.end = true;
        if (keyright != aNULL) {
            itr.find(vright);
            if (itr.End()) {
                if (keyright != ((TamguIndex*)idx)->right)
                    keyright->Release();
                return kmap;
            }
        }
        if (keyright != ((TamguIndex*)idx)->right)
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

    long skey = idx->Getinteger(idthread);

    if (!values.check(skey)) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    
    long vl;
    values.get(skey, vl);
    return globalTamgu->Provideint(vl);
}

Exporting Tamgu* Tamgua_mapii::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapii* m = (Tamgua_mapii*)a;

    Locking* _lock = _getlocktamgu(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapii it(m, false);
    long vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        if (!values.check(it.first)) {
            _cleanlocktamgu(_lock);
            return aFALSE;
        }
        values.get(it.first, vl);
        if (it.second != vl) {
            _cleanlocktamgu(_lock);
            return aFALSE;
        }
    }
    _cleanlocktamgu(_lock);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapii::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlocktamgu(b);

    Tamgua_mapii* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        hmap<long, long> keys;
        
        TamguIterationa_mapii it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second;
        
        long v, vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valueinteger();
            else {
                if (values.check(v)) {
                    values.get(v, vl);
                    if (vl == itr->Valueinteger())
                        keys.erase(v);
                }
            }
        }
        itr->Release();
        
        for (auto& a : keys)
            res->values.set(a.first, a.second);
        
        
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    TamguIterationa_mapii it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second ^ v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapii* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    TamguIterationa_mapii it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second | v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl == v)
                    res->values.set(ikey, v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    TamguIterationa_mapii it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second & v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl + v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapii it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second + v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl - v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapii it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second - v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::multiply(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl * v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapii it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second * v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapii::divide(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v, vl;
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
                res->values.set(ikey, vl / v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapii it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second / v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapii::mod(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v,vl;
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
                res->values.set(ikey, vl % v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapii it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second % v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::shiftright(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    Tamgua_mapii * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl >> v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    TamguIterationa_mapii it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second >> v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::shiftleft(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        long v, vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl << v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    TamguIterationa_mapii it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second << v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapii::power(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapii * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapii;
        double v;
        long vl;
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
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapii*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapii it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  pow(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}



Exporting Tamgu* Tamgua_mapii::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<long> keys;
    
    TamguIterationa_mapii itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        keys.push_back(itr.first);
    
    
    Tamgu* a;
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

double Tamgua_mapii::Sum() {
    double v = 0;
    TamguIterationa_mapii itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v += itr.second;
    return v;
}

double Tamgua_mapii::Product() {
    double v = 1;
    TamguIterationa_mapii itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v *= itr.second;
    return v;
}

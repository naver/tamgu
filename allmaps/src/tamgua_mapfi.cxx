/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapfi.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapfi.h"
#include "tamgulist.h"
#include "tamgufvector.h"
#include "tamguivector.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapfiMethod>  Tamgua_mapfi::methods;

Exporting short Tamgua_mapfi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapfi::AddMethod(TamguGlobal* global, string name, a_mapfiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapfi::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgua_mapfi::InitialisationModule(global,"");
}

bool Tamgua_mapfi::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapfi::idtype = global->Getid("a_mapfi");

    Tamgua_mapfi::AddMethod(global, "clear", &Tamgua_mapfi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapfi::AddMethod(global, "invert", &Tamgua_mapfi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapfi::AddMethod(global, "find", &Tamgua_mapfi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapfi::AddMethod(global, "items", &Tamgua_mapfi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapfi::AddMethod(global, "join", &Tamgua_mapfi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapfi::AddMethod(global, "test", &Tamgua_mapfi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapfi::AddMethod(global, "keys", &Tamgua_mapfi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapfi::AddMethod(global, "values", &Tamgua_mapfi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapfi::AddMethod(global, "sum", &Tamgua_mapfi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_mapfi::AddMethod(global, "product", &Tamgua_mapfi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_mapfi::AddMethod(global, "pop", &Tamgua_mapfi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapfi::AddMethod(global, "merge", &Tamgua_mapfi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapfi::AddMethod(global, "check", &Tamgua_mapfi::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapfi::AddMethod(global, "compact", &Tamgua_mapfi::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapfi::idtype] = new Tamgua_mapfi(global);
    global->RecordCompatibilities(Tamgua_mapfi::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapfi::Newiteration(bool direction) {
    return new TamguIterationa_mapfi(this, direction);
}


Exporting Tamgu* Tamgua_mapfi::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    double val = a->Float();

    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Locking _lock(v);
        if (values.check(val))
            v->values.push_back(val);

        return v;
    }

    if (context->isNumber()) {
        if (values.check(val))
            return globalTamgu->Providefloat(val);
        return aNOELEMENT;
    }
    
    if (values.check(val))
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgua_mapfi::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapfi it(this, false);

    long val = callfunc->Evaluate(0, context, idthread)->Integer();

    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }

    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second == val)
            return globalTamgu->Providefloat(it.first);
    }

    return aNULL;

}

Exporting Tamgu* Tamgua_mapfi::Push(Tamgu* k, Tamgu* v) {
    double s = k->Float();
    values.set(s, v->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapfi::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapfi::Clear() {
    values.clear();
}

Exporting string Tamgua_mapfi::String() {
    stringstream res;
    TamguIterationa_mapfi it(this, false);
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

Exporting string Tamgua_mapfi::JSonString() {
    stringstream res;
    TamguIterationa_mapfi it(this, false);
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


Exporting long Tamgua_mapfi::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapfi::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapfi::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapfi::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapfi::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapfi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapfi::idtype) {
            values = ((Tamgua_mapfi*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.set(itr->Keyfloat(),  itr->Valueinteger());
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            double nb = 0;
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
            double nb = 0;
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

        if (ke->Type() == Tamgua_mapfi::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapfi*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values.set(itr->Keyfloat(),  itr->Valueinteger());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    values.set(idx->Getfloat(idthread),  ke->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapfi::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapfi it(this, false);
            for (it.Begin(); it.End() == aFALSE; it.Next())
                vect->Push(globalTamgu->Providefloat(it.first));
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
        TamguIterationa_mapfi it(this, false);
        Tamgua_mapfi* kmap = new Tamgua_mapfi;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapfi itr(this, false);

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

    double skey = idx->Getfloat(idthread);

    if (!values.check(skey)) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    
    long vl;
    values.get(skey, vl);
    return globalTamgu->Provideint(vl);
}

Exporting Tamgu* Tamgua_mapfi::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapfi* m = (Tamgua_mapfi*)a;

    Locking* _lock = _getlocktamgu(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapfi it(m, false);
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

Exporting Tamgu* Tamgua_mapfi::xorset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    Tamgua_mapfi* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        hmap<double, long> keys;
        
        TamguIterationa_mapfi it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second;
        
        double v;
        long vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    TamguIterationa_mapfi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second ^ v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapfi*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    TamguIterationa_mapfi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second | v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    TamguIterationa_mapfi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second & v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapfi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second + v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapfi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second - v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::multiply(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapfi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second * v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapfi::divide(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapfi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second / v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapfi::mod(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v,vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapfi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second % v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::shiftright(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    Tamgua_mapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    TamguIterationa_mapfi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second >> v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::shiftleft(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        long v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    TamguIterationa_mapfi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second << v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfi::power(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfi;
        double v;
        long vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfi*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapfi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  pow(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}



Exporting Tamgu* Tamgua_mapfi::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<double> keys;
    
    TamguIterationa_mapfi itr(this, false);
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

double Tamgua_mapfi::Sum() {
    double v = 0;
    TamguIterationa_mapfi itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v += itr.second;
    return v;
}

double Tamgua_mapfi::Product() {
    double v = 1;
    TamguIterationa_mapfi itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v *= itr.second;
    return v;
}

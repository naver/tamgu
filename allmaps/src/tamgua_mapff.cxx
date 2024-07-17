/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapff.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapff.h"
#include "tamgulist.h"
#include "tamgufvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapffMethod>  Tamgua_mapff::methods;
static ThreadLock classlock;

Exporting short Tamgua_mapff::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapff::AddMethod(TamguGlobal* global, string name, a_mapffMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapff::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgua_mapff::methods.isEmpty())
    Tamgua_mapff::InitialisationModule(global,"");
}

bool Tamgua_mapff::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapff::idtype = global->Getid("a_mapff");

    Tamgua_mapff::AddMethod(global, "clear", &Tamgua_mapff::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapff::AddMethod(global, "invert", &Tamgua_mapff::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapff::AddMethod(global, "find", &Tamgua_mapff::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapff::AddMethod(global, "items", &Tamgua_mapff::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapff::AddMethod(global, "join", &Tamgua_mapff::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapff::AddMethod(global, "test", &Tamgua_mapff::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapff::AddMethod(global, "keys", &Tamgua_mapff::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapff::AddMethod(global, "values", &Tamgua_mapff::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapff::AddMethod(global, "sum", &Tamgua_mapff::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_mapff::AddMethod(global, "product", &Tamgua_mapff::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_mapff::AddMethod(global, "pop", &Tamgua_mapff::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapff::AddMethod(global, "merge", &Tamgua_mapff::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapff::AddMethod(global, "check", &Tamgua_mapff::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapff::AddMethod(global, "compact", &Tamgua_mapff::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapff::idtype] = new Tamgua_mapff(global);
    global->RecordCompatibilities(Tamgua_mapff::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapff::Newiteration(bool direction) {
    return new TamguIterationa_mapff(this, direction);
}


Exporting Tamgu* Tamgua_mapff::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapff::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapff it(this, false);

    double val = callfunc->Evaluate(0, context, idthread)->Float();

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

Exporting Tamgu* Tamgua_mapff::Push(Tamgu* k, Tamgu* v) {
    double s = k->Float();
    values.set(s, v->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapff::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapff::Clear() {
    values.clear();
}

Exporting string Tamgua_mapff::String() {
    stringstream res;
    TamguIterationa_mapff it(this, false);
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

Exporting string Tamgua_mapff::JSonString() {
    stringstream res;
    TamguIterationa_mapff it(this, false);
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


Exporting long Tamgua_mapff::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapff::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapff::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapff::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapff::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapff::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapff::idtype) {
            values = ((Tamgua_mapff*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.set(itr->Keyfloat(),  itr->Valuefloat());
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            double nb = 0;
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
            double nb = 0;
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

        if (ke->Type() == Tamgua_mapff::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapff*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values.set(itr->Keyfloat(),  itr->Valuefloat());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    values.set(idx->Getfloat(idthread),  ke->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapff::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapff it(this, false);
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
        TamguIterationa_mapff it(this, false);
        Tamgua_mapff* kmap = new Tamgua_mapff;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapff itr(this, false);

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
    double vl;
    values.get(skey, vl);
    return globalTamgu->Providefloat(vl);
}

Exporting Tamgu* Tamgua_mapff::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapff* m = (Tamgua_mapff*)a;

    Locking* _lock =  _getlocktamgu(m);

    if (m->values.size() != values.size())
        return aFALSE;
    
    double vl;
    TamguIterationa_mapff it(m, false);
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

Exporting Tamgu* Tamgua_mapff::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock =  _getlocktamgu(b);

    Tamgua_mapff* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        hmap<double, double> keys;
        
        TamguIterationa_mapff it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second;
        
        double v, vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
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
        
        
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapff*)Atom(true);
    
    TamguIterationa_mapff it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleXor(it.second,v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::orset(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapff* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapff*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    TamguIterationa_mapff it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleOr(it.second,v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::andset(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapff* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        double v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamgua_mapff*)Atom(true);
    
    TamguIterationa_mapff it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleAnd(it.second,v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::plus(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    Tamgua_mapff* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        double v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamgua_mapff*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapff it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second + v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::minus(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapff * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        double v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamgua_mapff*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapff it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second - v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::multiply(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapff * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        double v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamgua_mapff*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapff it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second * v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapff::divide(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapff * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        double v, vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamgua_mapff*)Atom(true);
    
    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapff it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second / v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapff::mod(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapff * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        long v;
        double vl;
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
                res->values.set(ikey, (BLONG)vl % v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapff*)Atom(true);
    
    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapff it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  (BLONG)it.second % v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::shiftright(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    Tamgua_mapff * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        long v;
        double ikey, vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, DoubleShiftright(vl, v));
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapff*)Atom(true);
    
    TamguIterationa_mapff it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleShiftright(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::shiftleft(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    Tamgua_mapff * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        long v;
        double ikey, vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyfloat();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, DoubleShiftleft(vl, v));
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapff*)Atom(true);
    
    TamguIterationa_mapff it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleShiftleft(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapff::power(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapff * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapff;
        double v, vl;
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
        res = (Tamgua_mapff*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapff it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  pow(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}



Exporting Tamgu* Tamgua_mapff::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<double> keys;
    
    TamguIterationa_mapff itr(this, false);
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

double Tamgua_mapff::Sum() {
    double v = 0;
    TamguIterationa_mapff itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v += itr.second;
    return v;
}

double Tamgua_mapff::Product() {
    double v = 1;
    TamguIterationa_mapff itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v *= itr.second;
    return v;
}

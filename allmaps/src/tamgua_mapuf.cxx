/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapuf.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapuf.h"
#include "tamgulist.h"
#include "tamguuvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapufMethod>  Tamgua_mapuf::methods;
static ThreadLock classlock;

Exporting short Tamgua_mapuf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapuf::AddMethod(TamguGlobal* global, string name, a_mapufMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapuf::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgua_mapuf::methods.isEmpty())
    Tamgua_mapuf::InitialisationModule(global,"");
}

bool Tamgua_mapuf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapuf::idtype = global->Getid("a_mapuf");

    Tamgua_mapuf::AddMethod(global, "clear", &Tamgua_mapuf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapuf::AddMethod(global, "invert", &Tamgua_mapuf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapuf::AddMethod(global, "find", &Tamgua_mapuf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapuf::AddMethod(global, "items", &Tamgua_mapuf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapuf::AddMethod(global, "join", &Tamgua_mapuf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapuf::AddMethod(global, "test", &Tamgua_mapuf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapuf::AddMethod(global, "keys", &Tamgua_mapuf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapuf::AddMethod(global, "values", &Tamgua_mapuf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapuf::AddMethod(global, "sum", &Tamgua_mapuf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_mapuf::AddMethod(global, "product", &Tamgua_mapuf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_mapuf::AddMethod(global, "pop", &Tamgua_mapuf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapuf::AddMethod(global, "merge", &Tamgua_mapuf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapuf::AddMethod(global, "check", &Tamgua_mapuf::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapuf::AddMethod(global, "compact", &Tamgua_mapuf::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapuf::idtype] = new Tamgua_mapuf(global);
    global->RecordCompatibilities(Tamgua_mapuf::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapuf::Newiteration(bool direction) {
    return new TamguIterationa_mapuf(this, direction);
}


Exporting Tamgu* Tamgua_mapuf::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    wstring val = a->UString();

    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Locking _lock(v);
        if (values.check(val))
            v->values.push_back(val);

        return v;
    }

    if (context->isString()) {
        if (values.check(val))
            return globalTamgu->Provideustring(val);
        return aNOELEMENT;
    }
    
    if (values.check(val))
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgua_mapuf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapuf it(this, false);

    double val = callfunc->Evaluate(0, context, idthread)->Float();

    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }

    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second == val)
            return globalTamgu->Provideustring(it.first);
    }

    return aNULL;

}

Exporting Tamgu* Tamgua_mapuf::Push(Tamgu* k, Tamgu* v) {
    wstring s = k->UString();
    values.set(s, v->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapuf::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapuf::Clear() {
    values.clear();
}

Exporting string Tamgua_mapuf::String() {
    stringstream res;
    TamguIterationa_mapuf it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sc_unicode_to_utf8(sx, it.first);stringing(res, sx);
        res << ":";
        res << it.second;
    }
    res << "}";
    return res.str();
}

Exporting wstring Tamgua_mapuf::UString() {
    std::wstringstream res;
    TamguIterationa_mapuf it(this, false);
    res << "{";
    bool beg = true;
    wstring sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;ustringing(res, sx);
        res << ":";
        res << it.second;
    }
    res << "}";
    return res.str();
}

Exporting string Tamgua_mapuf::JSonString() {
    stringstream res;
    TamguIterationa_mapuf it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sc_unicode_to_utf8(sx, it.first);jstringing(res, sx);
        res << ":";
        res << it.second;
    }
    res << "}";
    return res.str();
}


Exporting long Tamgua_mapuf::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapuf::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapuf::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapuf::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapuf::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapuf::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapuf::idtype) {
            values = ((Tamgua_mapuf*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.set(itr->Keyustring(),  itr->Valuefloat());
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            long nb = 0;
            wchar_t buff[20];
            for (long it = 0; it < ke->Size(); ++it) {
                swprintf_s(buff, 20, L"%ld", nb);
                values.set(buff, ke->getfloat(it));
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
            wchar_t buff[20];
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                swprintf_s(buff, 20, L"%ld", nb);
                values.set(buff, (*it)->Float());
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapuf::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapuf*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values.set(itr->Keyustring(),  itr->Valuefloat());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    values.set(idx->Getustring(idthread),  ke->Float());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapuf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapuf it(this, false);
            for (it.Begin(); it.End() == aFALSE; it.Next())
                vect->Push(globalTamgu->Provideustring(it.first));
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
        TamguIterationa_mapuf it(this, false);
        Tamgua_mapuf* kmap = new Tamgua_mapuf;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapuf itr(this, false);

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

    wstring skey = idx->Getustring(idthread);

    if (!values.check(skey)) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    
    double vl;
    values.get(skey, vl);
    return globalTamgu->Providefloat(vl);
}

Exporting Tamgu* Tamgua_mapuf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapuf* m = (Tamgua_mapuf*)a;

    Locking* _lock = _getlocktamgu(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapuf it(m, false);
    double vl;
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

Exporting Tamgu* Tamgua_mapuf::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapuf* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        hmap<wstring, double> keys;
        
        TamguIterationa_mapuf it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second;
        
        wstring v;
        double vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    TamguIterationa_mapuf it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleXor(it.second,v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapuf*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    TamguIterationa_mapuf it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleOr(it.second,v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        double v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    TamguIterationa_mapuf it(this, false);
    double v = b->Float();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleAnd(it.second,v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        double v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapuf it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second + v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        double v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapuf it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second - v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::multiply(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        double v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapuf it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second * v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapuf::divide(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        double v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapuf it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second / v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapuf::mod(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        long v;
        double vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapuf it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  (BLONG)it.second % v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::shiftright(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        long v;
        double vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    TamguIterationa_mapuf it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleShiftright(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::shiftleft(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        long v;
        double vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    TamguIterationa_mapuf it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  DoubleShiftleft(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuf::power(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuf;
        double v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapuf*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapuf it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  pow(it.second, v));
    _cleanlocktamgu(_lock);
    return res;
}



Exporting Tamgu* Tamgua_mapuf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<wstring> keys;
    
    TamguIterationa_mapuf itr(this, false);
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

double Tamgua_mapuf::Sum() {
    double v = 0;
    TamguIterationa_mapuf itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v += itr.second;
    return v;
}

double Tamgua_mapuf::Product() {
    double v = 1;
    TamguIterationa_mapuf itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v *= itr.second;
    return v;
}

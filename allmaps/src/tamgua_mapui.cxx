/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapui.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapui.h"
#include "tamgulist.h"
#include "tamguuvector.h"
#include "tamguivector.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapuiMethod>  Tamgua_mapui::methods;
Exporting hmap<string, string> Tamgua_mapui::infomethods;
Exporting basebin_hash<unsigned long> Tamgua_mapui::exported;

Exporting short Tamgua_mapui::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapui::AddMethod(TamguGlobal* global, string name, a_mapuiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_mapui::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgua_mapui::idtype = global->Getid("a_mapui");

    Tamgua_mapui::AddMethod(global, "clear", &Tamgua_mapui::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapui::AddMethod(global, "invert", &Tamgua_mapui::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapui::AddMethod(global, "find", &Tamgua_mapui::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapui::AddMethod(global, "items", &Tamgua_mapui::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapui::AddMethod(global, "join", &Tamgua_mapui::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapui::AddMethod(global, "test", &Tamgua_mapui::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapui::AddMethod(global, "keys", &Tamgua_mapui::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapui::AddMethod(global, "values", &Tamgua_mapui::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapui::AddMethod(global, "sum", &Tamgua_mapui::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_mapui::AddMethod(global, "product", &Tamgua_mapui::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_mapui::AddMethod(global, "pop", &Tamgua_mapui::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapui::AddMethod(global, "merge", &Tamgua_mapui::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapui::AddMethod(global, "check", &Tamgua_mapui::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapui::AddMethod(global, "compact", &Tamgua_mapui::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapui::idtype] = new Tamgua_mapui(global);
    global->RecordMethods(Tamgua_mapui::idtype, Tamgua_mapui::exported);

    return true;
}


Exporting TamguIteration* Tamgua_mapui::Newiteration(bool direction) {
    return new TamguIterationa_mapui(this, direction);
}


Exporting Tamgu* Tamgua_mapui::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapui::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapui it(this, false);

    long val = callfunc->Evaluate(0, context, idthread)->Integer();

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

Exporting Tamgu* Tamgua_mapui::Push(Tamgu* k, Tamgu* v) {
    wstring s = k->UString();
    values.set(s, v->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapui::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapui::Clear() {
    values.clear();
}

Exporting string Tamgua_mapui::String() {
    stringstream res;
    TamguIterationa_mapui it(this, false);
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

Exporting wstring Tamgua_mapui::UString() {
    std::wstringstream res;
    TamguIterationa_mapui it(this, false);
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


Exporting string Tamgua_mapui::JSonString() {
    stringstream res;
    TamguIterationa_mapui it(this, false);
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


Exporting long Tamgua_mapui::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapui::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapui::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapui::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapui::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapui::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapui::idtype) {
            values = ((Tamgua_mapui*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.set(itr->Keyustring(),  itr->Valueinteger());
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
                values.set(buff, ke->getinteger(it));
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
                values.set(buff, (*it)->Integer());
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapui::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapui*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values.set(itr->Keyustring(),  itr->Valueinteger());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    values.set(idx->Getustring(idthread),  ke->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapui::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapui it(this, false);
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
        TamguIterationa_mapui it(this, false);
        Tamgua_mapui* kmap = new Tamgua_mapui;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapui itr(this, false);

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

    wstring skey = idx->Getustring(idthread);

    if (!values.check(skey)) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    
    long vl;
    values.get(skey, vl);
    return globalTamgu->Provideint(vl);
}

Exporting Tamgu* Tamgua_mapui::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapui* m = (Tamgua_mapui*)a;

    Locking* _lock = _getlockif(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapui it(m, false);
    long vl;
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

Exporting Tamgu* Tamgua_mapui::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlockif(b);
    
    Tamgua_mapui* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        hmap<wstring, long> keys;
        
        TamguIterationa_mapui it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second;
        
        wstring v;
        long vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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
        
        
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapui*)Atom(true);
    
    TamguIterationa_mapui it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second ^ v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapui*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlockif(_lock);
        return res;
    }
    
    TamguIterationa_mapui it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second | v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapui*)Atom(true);
    
    TamguIterationa_mapui it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second & v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapui*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapui it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second + v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapui*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapui it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second - v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::multiply(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
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
        res = (Tamgua_mapui*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapui it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second * v);
    _cleanlockif(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapui::divide(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
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
        res = (Tamgua_mapui*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapui it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second / v);
    _cleanlockif(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapui::mod(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
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
                res->values.set(ikey, vl % v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapui*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapui it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second % v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::shiftright(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    Tamgua_mapui * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl >> v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapui*)Atom(true);
    
    TamguIterationa_mapui it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second >> v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::shiftleft(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        long v, vl;
        wstring ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyustring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl << v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapui*)Atom(true);
    
    TamguIterationa_mapui it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second << v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapui::power(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapui * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapui;
        double v;
        long vl;
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
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapui*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapui it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  pow(it.second, v));
    _cleanlockif(_lock);
    return res;
}



Exporting Tamgu* Tamgua_mapui::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<wstring> keys;
    
    TamguIterationa_mapui itr(this, false);
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

double Tamgua_mapui::Sum() {
    double v = 0;
    TamguIterationa_mapui itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v += itr.second;
    return v;
}

double Tamgua_mapui::Product() {
    double v = 1;
    TamguIterationa_mapui itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v *= itr.second;
    return v;
}

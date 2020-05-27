/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapfu.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapfu.h"
#include "tamgulist.h"
#include "tamgufvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapfuMethod>  Tamgua_mapfu::methods;
Exporting hmap<string, string> Tamgua_mapfu::infomethods;
Exporting basebin_hash<unsigned long> Tamgua_mapfu::exported;

Exporting short Tamgua_mapfu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapfu::AddMethod(TamguGlobal* global, string name, a_mapfuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_mapfu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgua_mapfu::idtype = global->Getid("a_mapfu");

    Tamgua_mapfu::AddMethod(global, "clear", &Tamgua_mapfu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapfu::AddMethod(global, "invert", &Tamgua_mapfu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapfu::AddMethod(global, "find", &Tamgua_mapfu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapfu::AddMethod(global, "items", &Tamgua_mapfu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapfu::AddMethod(global, "join", &Tamgua_mapfu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapfu::AddMethod(global, "test", &Tamgua_mapfu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapfu::AddMethod(global, "keys", &Tamgua_mapfu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapfu::AddMethod(global, "values", &Tamgua_mapfu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapfu::AddMethod(global, "pop", &Tamgua_mapfu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapfu::AddMethod(global, "merge", &Tamgua_mapfu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapfu::AddMethod(global, "check", &Tamgua_mapfu::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapfu::AddMethod(global, "compact", &Tamgua_mapfu::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapfu::idtype] = new Tamgua_mapfu(global);
    global->RecordMethods(Tamgua_mapfu::idtype, Tamgua_mapfu::exported);

    return true;
}


Exporting TamguIteration* Tamgua_mapfu::Newiteration(bool direction) {
    return new TamguIterationa_mapfu(this, direction);
}


Exporting Tamgu* Tamgua_mapfu::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapfu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapfu it(this, false);

    wstring val = callfunc->Evaluate(0, context, idthread)->UString();

    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                v->values.push_back(it.first);
        }
        return v;
    }

    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second.value() == val)
            return globalTamgu->Providefloat(it.first);
    }

    return aNULL;

}

Exporting Tamgu* Tamgua_mapfu::Push(Tamgu* k, Tamgu* v) {
    double s = k->Float();
    setvalue(s, v->UString());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapfu::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapfu::Clear() {
    values.clear();
}

Exporting string Tamgua_mapfu::String() {
    stringstream res;
    TamguIterationa_mapfu it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first;
        res << ":";
        w = it.second.value();
        sc_unicode_to_utf8(sx, w);stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting wstring Tamgua_mapfu::UString() {
    std::wstringstream res;
    TamguIterationa_mapfu it(this, false);
    res << "{";
    bool beg = true;
    wstring sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first;
        res << ":";
        sx = it.second.value();ustringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamgua_mapfu::JSonString() {
    stringstream res;
    TamguIterationa_mapfu it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first;
        res << ":";
        w = it.second.value();
        sc_unicode_to_utf8(sx, w);jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamgua_mapfu::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapfu::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapfu::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapfu::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapfu::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapfu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapfu::idtype) {
            values = ((Tamgua_mapfu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                setvalue(itr->Keyfloat(),  itr->Valueustring());
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            double nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                setvalue(nb, ke->getustring(it));
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
                setvalue(nb, (*it)->UString());
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapfu::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapfu*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                setvalue(itr->Keyfloat(),  itr->Valueustring());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    wstring vl = ke->UString();
    setvalue(idx->Getfloat(idthread),  vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapfu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapfu it(this, false);
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
        TamguIterationa_mapfu it(this, false);
        Tamgua_mapfu* kmap = new Tamgua_mapfu;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapfu itr(this, false);

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
            kmap->setvalue(it.first, it.second.value());
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
    atomic_wstring vl;
    values.get(skey, vl);
    return new Tamgua_ustring(vl);
}

Exporting Tamgu* Tamgua_mapfu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapfu* m = (Tamgua_mapfu*)a;

    Locking* _lock = _getlockif(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    atomic_wstring vl;
    TamguIterationa_mapfu it(m, false);
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

Exporting Tamgu* Tamgua_mapfu::plus(Tamgu* b, bool itself) {
Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapfu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfu;
        wstring v;
        double ikey;
        atomic_wstring vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyfloat();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->setvalue(ikey, vl.value() + v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapfu*)Atom(true);
    
    wstring v = b->UString();
    TamguIterationa_mapfu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first,  it.second.value() + v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfu::minus(Tamgu* b, bool itself) {
Locking* _lock = _getlockif(b);
    
    
    Tamgua_mapfu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfu;
        wstring v;
        atomic_wstring vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyfloat();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->setvalue(ikey, StringMinus(vl.value(),v));
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapfu*)Atom(true);
    
    wstring v = b->UString();
    TamguIterationa_mapfu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first, StringMinus(it.second.value(),v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfu::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlockif(b);
    
    Tamgua_mapfu* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfu;
        hmap<double, wstring> keys;
        
        TamguIterationa_mapfu it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second.value();
        
        double v;
        atomic_wstring vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valueustring();
            else {
                if (values.check(v)) {
                    values.get(v, vl);
                    if (vl.value() == itr->Valueustring())
                        keys.erase(v);
                }
            }
        }
        itr->Release();
        
        for (auto& a : keys)
            res->setvalue(a.first, a.second);
        
        
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapfu*)Atom(true);
    
    TamguIterationa_mapfu it(this, false);
    wstring v = b->UString();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first,  StringXor(it.second.value(),v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfu::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);
    
    Tamgua_mapfu* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapfu*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlockif(_lock);
        return res;
    }
    
    TamguIterationa_mapfu it(this, false);
    wstring v = b->UString();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first,  it.second.value() + v);
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfu::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);

    Tamgua_mapfu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfu;
        wstring v;
        atomic_wstring vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyfloat();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl.value() == v)
                    res->setvalue(ikey, v);
            }
        }
        itr->Release();
        _cleanlockif(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapfu*)Atom(true);
    
    TamguIterationa_mapfu it(this, false);
    wstring v = b->UString();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first, StringAnd(it.second.value(),v));
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<double> keys;
    
    TamguIterationa_mapfu itr(this, false);
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


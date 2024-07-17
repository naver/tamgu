/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapuu.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapuu.h"
#include "tamgulist.h"
#include "tamguuvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapuuMethod>  Tamgua_mapuu::methods;
static ThreadLock classlock;

Exporting short Tamgua_mapuu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapuu::AddMethod(TamguGlobal* global, string name, a_mapuuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapuu::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgua_mapuu::methods.isEmpty())
    Tamgua_mapuu::InitialisationModule(global,"");
}

bool Tamgua_mapuu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapuu::idtype = global->Getid("a_mapuu");

    Tamgua_mapuu::AddMethod(global, "clear", &Tamgua_mapuu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapuu::AddMethod(global, "invert", &Tamgua_mapuu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapuu::AddMethod(global, "find", &Tamgua_mapuu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapuu::AddMethod(global, "items", &Tamgua_mapuu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapuu::AddMethod(global, "join", &Tamgua_mapuu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapuu::AddMethod(global, "test", &Tamgua_mapuu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapuu::AddMethod(global, "keys", &Tamgua_mapuu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapuu::AddMethod(global, "values", &Tamgua_mapuu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapuu::AddMethod(global, "pop", &Tamgua_mapuu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapuu::AddMethod(global, "merge", &Tamgua_mapuu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapuu::AddMethod(global, "check", &Tamgua_mapuu::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapuu::AddMethod(global, "compact", &Tamgua_mapuu::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapuu::idtype] = new Tamgua_mapuu(global);
    global->RecordCompatibilities(Tamgua_mapuu::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapuu::Newiteration(bool direction) {
    return new TamguIterationa_mapuu(this, direction);
}


Exporting Tamgu* Tamgua_mapuu::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapuu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapuu it(this, false);

    wstring val = callfunc->Evaluate(0, context, idthread)->UString();

    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                v->values.push_back(it.first);
        }
        return v;
    }

    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second.value() == val)
            return globalTamgu->Provideustring(it.first);
    }

    return aNULL;

}

Exporting Tamgu* Tamgua_mapuu::Push(Tamgu* k, Tamgu* v) {
    wstring s = k->UString();
    setvalue(s, v->UString());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapuu::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapuu::Clear() {
    values.clear();
}

Exporting string Tamgua_mapuu::String() {
    stringstream res;
    TamguIterationa_mapuu it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sc_unicode_to_utf8(sx, it.first);stringing(res, sx);
        res << ":";
        w = it.second.value();
        sc_unicode_to_utf8(sx, w);stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting wstring Tamgua_mapuu::UString() {
    std::wstringstream res;
    TamguIterationa_mapuu it(this, false);
    res << "{";
    bool beg = true;
    wstring sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;ustringing(res, sx);
        res << ":";
        sx = it.second.value();ustringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamgua_mapuu::JSonString() {
    stringstream res;
    TamguIterationa_mapuu it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sc_unicode_to_utf8(sx, it.first);jstringing(res, sx);
        res << ":";
        w = it.second.value();
        sc_unicode_to_utf8(sx, w);jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamgua_mapuu::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapuu::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapuu::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapuu::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapuu::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapuu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapuu::idtype) {
            values = ((Tamgua_mapuu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                setvalue(itr->Keyustring(),  itr->Valueustring());
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
                setvalue(buff, ke->getustring(it));
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
                setvalue(buff, (*it)->UString());
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapuu::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapuu*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                setvalue(itr->Keyustring(),  itr->Valueustring());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    wstring vl = ke->UString();
    setvalue(idx->Getustring(idthread),  vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapuu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapuu it(this, false);
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
        TamguIterationa_mapuu it(this, false);
        Tamgua_mapuu* kmap = new Tamgua_mapuu;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapuu itr(this, false);

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
            kmap->setvalue(it.first, it.second.value());
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
    
    atomic_wstring vl;
    values.get(skey, vl);
    return new Tamgua_ustring(vl);
}

Exporting Tamgu* Tamgua_mapuu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapuu* m = (Tamgua_mapuu*)a;

    Locking* _lock = _getlocktamgu(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapuu it(m, false);
    atomic_wstring vl;
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

Exporting Tamgu* Tamgua_mapuu::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuu;
        wstring v;
        wstring ikey;
        atomic_wstring vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyustring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->setvalue(ikey, vl.value() + v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapuu*)Atom(true);
    
    wstring v = b->UString();
    TamguIterationa_mapuu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first,  it.second.value() + v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuu::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuu;
        wstring v;
        wstring ikey;
        atomic_wstring vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyustring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->setvalue(ikey, StringMinus(vl.value(),v));
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapuu*)Atom(true);
    
    wstring v = b->UString();
    TamguIterationa_mapuu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first, StringMinus(it.second.value(),v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuu::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapuu* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuu;
        hmap<wstring, wstring> keys;
        
        TamguIterationa_mapuu it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second.value();
        
        wstring v;
        atomic_wstring vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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
        
        
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapuu*)Atom(true);
    
    TamguIterationa_mapuu it(this, false);
    wstring v = b->UString();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first,  StringXor(it.second.value(),v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuu::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuu* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapuu*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    TamguIterationa_mapuu it(this, false);
    wstring v = b->UString();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first,  it.second.value() + v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuu::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapuu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapuu;
        wstring v;
        wstring ikey;
        atomic_wstring vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyustring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl.value() == v)
                    res->setvalue(ikey, v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapuu*)Atom(true);
    
    TamguIterationa_mapuu it(this, false);
    wstring v = b->UString();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        setvalue(it.first, StringAnd(it.second.value(),v));
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapuu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<wstring> keys;
    
    TamguIterationa_mapuu itr(this, false);
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


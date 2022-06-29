/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapiu.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapiu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapiuMethod>  Tamgua_mapiu::methods;

Exporting short Tamgua_mapiu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapiu::AddMethod(TamguGlobal* global, string name, a_mapiuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapiu::Setidtype(TamguGlobal* global) {
  if (methods.isEmpty())
    Tamgua_mapiu::InitialisationModule(global,"");
}

bool Tamgua_mapiu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapiu::idtype = global->Getid("a_mapiu");

    Tamgua_mapiu::AddMethod(global, "clear", &Tamgua_mapiu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapiu::AddMethod(global, "invert", &Tamgua_mapiu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapiu::AddMethod(global, "find", &Tamgua_mapiu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapiu::AddMethod(global, "items", &Tamgua_mapiu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapiu::AddMethod(global, "join", &Tamgua_mapiu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapiu::AddMethod(global, "test", &Tamgua_mapiu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapiu::AddMethod(global, "keys", &Tamgua_mapiu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapiu::AddMethod(global, "values", &Tamgua_mapiu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapiu::AddMethod(global, "pop", &Tamgua_mapiu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapiu::AddMethod(global, "merge", &Tamgua_mapiu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapiu::AddMethod(global, "check", &Tamgua_mapiu::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapiu::AddMethod(global, "compact", &Tamgua_mapiu::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapiu::idtype] = new Tamgua_mapiu(global);
    global->RecordCompatibilities(Tamgua_mapiu::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapiu::Newiteration(bool direction) {
    return new TamguIterationa_mapiu(this, direction);
}


Exporting Tamgu* Tamgua_mapiu::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapiu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapiu it(this, false);

    wstring val = callfunc->Evaluate(0, context, idthread)->UString();

    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                v->values.push_back(it.first);
        }
        return v;
    }

    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second.value() == val)
            return globalTamgu->Provideint(it.first);
    }

    return aNULL;

}

Exporting Tamgu* Tamgua_mapiu::Push(Tamgu* k, Tamgu* v) {
    long s = k->Integer();
    wstring w = v->UString();
    values.set(s, w);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapiu::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapiu::Clear() {
    values.clear();
}

Exporting string Tamgua_mapiu::String() {
    stringstream res;
    TamguIterationa_mapiu it(this, false);
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

Exporting wstring Tamgua_mapiu::UString() {
    std::wstringstream res;
    TamguIterationa_mapiu it(this, false);
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

Exporting string Tamgua_mapiu::JSonString() {
    stringstream res;
    TamguIterationa_mapiu it(this, false);
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


Exporting long Tamgua_mapiu::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapiu::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapiu::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapiu::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapiu::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapiu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    wstring w;
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapiu::idtype) {
            values = ((Tamgua_mapiu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                w = itr->Valueustring();
                values.set(itr->Keyinteger(),  w);
            }
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                w = ke->getustring(it);
                values.set(nb, w);
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
                w = (*it)->UString();
                values.set(nb, w);
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapiu::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapiu*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                w = itr->Valueustring();
                values.set(itr->Keyinteger(),  w);
            }
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    wstring vl = ke->UString();
    values.set(idx->Getinteger(idthread),  vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapiu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapiu it(this, false);
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
        TamguIterationa_mapiu it(this, false);
        Tamgua_mapiu* kmap = new Tamgua_mapiu;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapiu itr(this, false);

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
    
    atomic_wstring w;
    values.get(skey, w);
    return globalTamgu->Provideustring(w.value());
}

Exporting Tamgu* Tamgua_mapiu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapiu* m = (Tamgua_mapiu*)a;

    Locking* _lock = _getlocktamgu(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapiu it(m, false);
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

Exporting Tamgu* Tamgua_mapiu::plus(Tamgu* b, bool itself) {
   Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapiu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapiu;
        wstring v;
        atomic_wstring vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                v = vl.value() + v;
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
        res = (Tamgua_mapiu*)Atom(true);
    
    wstring v = b->UString();
    wstring vl;
    TamguIterationa_mapiu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first,  vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapiu::minus(Tamgu* b, bool itself) {
   Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapiu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapiu;
        wstring v;
        atomic_wstring vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                v = StringMinus(vl.value(), v);
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
        res = (Tamgua_mapiu*)Atom(true);
    
    wstring v = b->UString();
    wstring vl;
    TamguIterationa_mapiu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringMinus(it.second.value(), v);
        values.set(it.first,  vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapiu::xorset(Tamgu* b, bool itself) {
    
   Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapiu* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapiu;
        hmap<long, wstring> keys;
        
        TamguIterationa_mapiu it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second.value();
        
        long v;
        atomic_wstring vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
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
            res->values.set(a.first, a.second);
        
        
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapiu*)Atom(true);
    
    wstring v = b->UString();
    wstring vl;
    TamguIterationa_mapiu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringXor(it.second.value(), v);
        values.set(it.first,  vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapiu::orset(Tamgu* b, bool itself) {
   Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapiu* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapiu*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    wstring v = b->UString();
    wstring vl;
    TamguIterationa_mapiu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first,  vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapiu::andset(Tamgu* b, bool itself) {
   Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapiu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapiu;
        wstring v;
        atomic_wstring vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl.value() == v)
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
        res = (Tamgua_mapiu*)Atom(true);
    
    wstring v = b->UString();
    wstring vl;
    TamguIterationa_mapiu it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringAnd(it.second.value(), v);
        values.set(it.first,  vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapiu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<long> keys;
    
    TamguIterationa_mapiu itr(this, false);
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


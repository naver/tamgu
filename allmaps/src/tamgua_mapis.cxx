/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapis.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapis.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapisMethod>  Tamgua_mapis::methods;
Exporting hmap<string, string> Tamgua_mapis::infomethods;
Exporting bin_hash<unsigned long> Tamgua_mapis::exported;

Exporting short Tamgua_mapis::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapis::AddMethod(TamguGlobal* global, string name, a_mapisMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_mapis::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgua_mapis::idtype = global->Getid("a_mapis");

    Tamgua_mapis::AddMethod(global, "clear", &Tamgua_mapis::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapis::AddMethod(global, "invert", &Tamgua_mapis::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapis::AddMethod(global, "find", &Tamgua_mapis::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapis::AddMethod(global, "items", &Tamgua_mapis::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapis::AddMethod(global, "join", &Tamgua_mapis::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapis::AddMethod(global, "test", &Tamgua_mapis::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapis::AddMethod(global, "keys", &Tamgua_mapis::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapis::AddMethod(global, "values", &Tamgua_mapis::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapis::AddMethod(global, "pop", &Tamgua_mapis::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapis::AddMethod(global, "merge", &Tamgua_mapis::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapis::AddMethod(global, "check", &Tamgua_mapis::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapis::AddMethod(global, "compact", &Tamgua_mapis::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapis::idtype] = new Tamgua_mapis(global);
    global->RecordMethods(Tamgua_mapis::idtype, Tamgua_mapis::exported);

    return true;
}


Exporting TamguIteration* Tamgua_mapis::Newiteration(bool direction) {
    return new TamguIterationa_mapis(this, direction);
}


Exporting Tamgu* Tamgua_mapis::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapis::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapis it(this, false);

    string val = callfunc->Evaluate(0, context, idthread)->String();

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

Exporting Tamgu* Tamgua_mapis::Push(Tamgu* k, Tamgu* v) {
    long s = k->Integer();
    string vl = v->String();
    values.set(s, vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapis::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapis::Clear() {
    values.clear();
}

Exporting string Tamgua_mapis::String() {
    stringstream res;
    TamguIterationa_mapis it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first;
        res << ":";
        sx = it.second.value();stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamgua_mapis::JSonString() {
    stringstream res;
    TamguIterationa_mapis it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first;
        res << ":";
        sx = it.second.value();jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamgua_mapis::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapis::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapis::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapis::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapis::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapis::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    string vl;

    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapis::idtype) {
            values = ((Tamgua_mapis*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                vl = itr->Valuestring();
                values.set(itr->Keyinteger(),  vl);
            }
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                vl = ke->getstring(it);
                values.set(nb, vl);
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
                vl = (*it)->String();
                values.set(nb, vl);
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapis::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapis*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                vl = itr->Valuestring();
                values.set(itr->Keyinteger(),  vl);
            }
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    vl = ke->String();
    values.set(idx->Integer(),  vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapis::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapis it(this, false);
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
        TamguIterationa_mapis it(this, false);
        Tamgua_mapis* kmap = new Tamgua_mapis;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapis itr(this, false);

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

    key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
    
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
    
    atomic_string vl;
    values.get(skey, vl);
    return new Tamgua_string(vl);
}

Exporting Tamgu* Tamgua_mapis::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapis* m = (Tamgua_mapis*)a;

    Locking* _lock = _getlockif(m);

    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapis it(m, false);
    atomic_string vl;
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

Exporting Tamgu* Tamgua_mapis::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);

    
    Tamgua_mapis* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapis;
        string v;
        atomic_string vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                v = vl.value() + v;
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
        res = (Tamgua_mapis*)Atom(true);
    
    string v = b->String();
    string vl;
    TamguIterationa_mapis it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first,  vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapis::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);

    Tamgua_mapis * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapis;
        string v;
        atomic_string vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                v = StringMinus(vl.value(), v);
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
        res = (Tamgua_mapis*)Atom(true);
    
    string v = b->String();
    string vl;
    TamguIterationa_mapis it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringMinus(it.second.value(),v);
        values.set(it.first, vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapis::xorset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);

    Tamgua_mapis* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapis;
        hmap<long, string> keys;
        
        TamguIterationa_mapis it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second.value();
        
        long v;
        atomic_string vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuestring();
            else {
                if (values.check(v)) {
                    values.get(v, vl);
                    if (vl.value()  == itr->Valuestring())
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
        res = (Tamgua_mapis*)Atom(true);
    
    TamguIterationa_mapis it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringXor(it.second.value(),v);
        values.set(it.first,  vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapis::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);

    Tamgua_mapis* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapis*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlockif(_lock);
        return res;
    }
    
    TamguIterationa_mapis it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first,  vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapis::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlockif(b);

    Tamgua_mapis* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapis;
        string v;
        atomic_string vl;
        long ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keyinteger();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl.value() == v)
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
        res = (Tamgua_mapis*)Atom(true);
    
    TamguIterationa_mapis it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringAnd(it.second.value(),v);
        values.set(it.first, vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapis::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<long> keys;
    
    TamguIterationa_mapis itr(this, false);
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


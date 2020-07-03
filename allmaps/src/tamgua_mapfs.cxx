/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapfs.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapfs.h"
#include "tamgulist.h"
#include "tamgufvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapfsMethod>  Tamgua_mapfs::methods;
Exporting hmap<string, string> Tamgua_mapfs::infomethods;
Exporting basebin_hash<unsigned long> Tamgua_mapfs::exported;

Exporting short Tamgua_mapfs::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapfs::AddMethod(TamguGlobal* global, string name, a_mapfsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgua_mapfs::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgua_mapfs::idtype = global->Getid("a_mapfs");

    Tamgua_mapfs::AddMethod(global, "clear", &Tamgua_mapfs::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapfs::AddMethod(global, "invert", &Tamgua_mapfs::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapfs::AddMethod(global, "find", &Tamgua_mapfs::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapfs::AddMethod(global, "items", &Tamgua_mapfs::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapfs::AddMethod(global, "join", &Tamgua_mapfs::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapfs::AddMethod(global, "test", &Tamgua_mapfs::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapfs::AddMethod(global, "keys", &Tamgua_mapfs::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapfs::AddMethod(global, "values", &Tamgua_mapfs::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapfs::AddMethod(global, "pop", &Tamgua_mapfs::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapfs::AddMethod(global, "merge", &Tamgua_mapfs::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapfs::AddMethod(global, "check", &Tamgua_mapfs::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapfs::AddMethod(global, "compact", &Tamgua_mapfs::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapfs::idtype] = new Tamgua_mapfs(global);
    global->RecordMethods(Tamgua_mapfs::idtype, Tamgua_mapfs::exported);

    return true;
}


Exporting TamguIteration* Tamgua_mapfs::Newiteration(bool direction) {
    return new TamguIterationa_mapfs(this, direction);
}


Exporting Tamgu* Tamgua_mapfs::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapfs::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapfs it(this, false);

    string val = callfunc->Evaluate(0, context, idthread)->String();

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

Exporting Tamgu* Tamgua_mapfs::Push(Tamgu* k, Tamgu* v) {
    double s = k->Float();
    string vl = v->String();
    values.set(s, vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapfs::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapfs::Clear() {
    values.clear();
}

Exporting string Tamgua_mapfs::String() {
    stringstream res;
    TamguIterationa_mapfs it(this, false);
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

Exporting string Tamgua_mapfs::JSonString() {
    stringstream res;
    TamguIterationa_mapfs it(this, false);
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


Exporting long Tamgua_mapfs::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapfs::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapfs::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapfs::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapfs::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapfs::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    string vl;
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapfs::idtype) {
            values = ((Tamgua_mapfs*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                vl = itr->Valuestring();
                values.set(itr->Keyfloat(),  vl);
            }
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            double nb = 0;
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
            double nb = 0;
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

        if (ke->Type() == Tamgua_mapfs::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapfs*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                vl = itr->Valuestring();
                values.set(itr->Keyfloat(),  vl);
            }
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    vl = ke->String();
    values.set(idx->Getfloat(idthread),  vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapfs::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapfs it(this, false);
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
        TamguIterationa_mapfs it(this, false);
        Tamgua_mapfs* kmap = new Tamgua_mapfs;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapfs itr(this, false);

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
    
    atomic_string vl;
    values.get(skey, vl);
    return new Tamgua_string(vl);
}

Exporting Tamgu* Tamgua_mapfs::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapfs* m = (Tamgua_mapfs*)a;

    Locking* _lock = _getlock(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapfs it(m, false);
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

Exporting Tamgu* Tamgua_mapfs::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlock(b);

    
    Tamgua_mapfs* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfs;
        string v;
        atomic_string vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keyfloat();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                v = vl.value()+v;
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
        res = (Tamgua_mapfs*)Atom(true);
    
    string v = b->String();
    TamguIterationa_mapfs it(this, false);
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first,  vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfs::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlock(b);

    
    Tamgua_mapfs * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfs;
        string v;
        atomic_string vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfs*)Atom(true);
    
    string v = b->String();
    string vl;
    TamguIterationa_mapfs it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringMinus(it.second.value(), v);
        values.set(it.first, vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfs::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlock(b);

    Tamgua_mapfs* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfs;
        hmap<double, string> keys;
        
        TamguIterationa_mapfs it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second.value();
        
        double v;
        atomic_string vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuestring();
            else {
                if (values.check(v)) {
                    values.get(v, vl);
                    if (vl.value() == itr->Valuestring())
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
        res = (Tamgua_mapfs*)Atom(true);
    
    TamguIterationa_mapfs it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringXor(it.second.value(),v);
        values.set(it.first,  vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfs::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlock(b);

    
    Tamgua_mapfs* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapfs*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlockif(_lock);
        return res;
    }
    
    TamguIterationa_mapfs it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first,  vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfs::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlock(b);

    
    Tamgua_mapfs* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapfs;
        string v;
        atomic_string vl;
        double ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keyfloat();
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
        res = (Tamgua_mapfs*)Atom(true);
    
    TamguIterationa_mapfs it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringAnd(it.second.value(),v);
        values.set(it.first,  vl);
    }
    _cleanlockif(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapfs::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<double> keys;
    
    TamguIterationa_mapfs itr(this, false);
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


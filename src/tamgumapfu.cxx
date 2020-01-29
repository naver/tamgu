/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapfu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufvector.h"
#include "tamgufloat.h"
//#prime
#include "tamgumapfu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapfuMethod>  Tamgumapfu::methods;
Exporting hmap<string, string> Tamgumapfu::infomethods;
Exporting bin_hash<unsigned long> Tamgumapfu::exported;

Exporting short Tamgumapfu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapfu::AddMethod(TamguGlobal* global, string name, mapfuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapfu::Setidtype(TamguGlobal* global) {
        Tamgumapfu::idtype = global->Getid("mapfu");
    }

   bool Tamgumapfu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgumapfu::idtype = global->Getid("mapfu");

    Tamgumapfu::AddMethod(global, "clear", &Tamgumapfu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapfu::AddMethod(global, "invert", &Tamgumapfu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapfu::AddMethod(global, "find", &Tamgumapfu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapfu::AddMethod(global, "items", &Tamgumapfu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapfu::AddMethod(global, "join", &Tamgumapfu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapfu::AddMethod(global, "test", &Tamgumapfu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapfu::AddMethod(global, "keys", &Tamgumapfu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapfu::AddMethod(global, "values", &Tamgumapfu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapfu::AddMethod(global, "pop", &Tamgumapfu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapfu::AddMethod(global, "merge", &Tamgumapfu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgumapfu::idtype] = new Tamgumapfu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("fumap")] = new Tamgumapfu(global);

    global->RecordMethods(global->Getid("fumap"), Tamgumapfu::exported);
    #endif
    global->RecordMethods(Tamgumapfu::idtype, Tamgumapfu::exported);

    return true;
}


Exporting TamguIteration* Tamgumapfu::Newiteration(bool direction) {
    return new TamguIterationmapfu(this, direction);
}


Exporting Tamgu* Tamgumapfu::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    double val = a->Float();

     if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isNumber()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Providefloat(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgumapfu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    wstring val = callfunc->Evaluate(0, context, idthread)->UString();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Doublelocking _lock(this, v);
        for (auto& it : values) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (auto& it : values) {
        if (it.second == val)
            return globalTamgu->Providefloat(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapfu::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    double s = k->Float();
    values[s] = v->UString();
    return aTRUE;
}

Exporting Tamgu* Tamgumapfu::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgumapfu::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgumapfu::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamgumapfu::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamgumapfu::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgumapfu::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgumapfu::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgumapfu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgumapfu::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamgumapfu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapfu::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapfu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valueustring();
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getustring(it);
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Doublelocking _lock(this, ke);
            Tamgulist* kvect = (Tamgulist*)ke;
            values.clear();
            long nb = 0;

            for (auto& it : kvect->values) {
                values[nb] = it->UString();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        Locking* _lock = _getlock(this);
        values.clear();
        if (ke->Type() == Tamgumapfu::idtype)
            values = ((Tamgumapfu*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyfloat()] = itr->Valueustring();
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Locking* _lock = _getlock(this);
    values[idx->Float()] = ke->UString();
    _cleanlock(_lock);
    return aTRUE;
}


Exporting Tamgu* Tamgumapfu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            hmap<double, wstring>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providefloat(it->first));
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
        Tamgumapfu* kmap = new Tamgumapfu;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        hmap<double, wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<double, wstring>::iterator itr = values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                keyright->Release();
                return kmap;
            }
        }
        keyright->Release();
        for (; it != values.end(); it++) {
            kmap->values[it->first] = it->second;
            if (it == itr)
                return kmap;
        }

        if (itr != values.end())
            kmap->values.clear();
        return kmap;

    }

    key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
    
    if (key == aNULL) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }

    double skey = key->Float();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgumapfu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapfu* m = (Tamgumapfu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<double, wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapfu::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgumapfu* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfu;
        hmap<double, wstring> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valueustring();
            else {
                if (values[v]==itr->Valueustring())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first]= a.second;

        return res;


    }

    if (itself)
        res = this;
    else
        res = (Tamgumapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfu* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapfu*)Atom(true);

    if (b->isMapContainer()) {
        hmap<double, wstring>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfu* res;
    if (b->isMapContainer()) {
        hmap<double, wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()] == v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfu* res;
    if (b->isMapContainer()) {
        hmap<double, wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = it->second + v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapfu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfu * res;
    if (b->isMapContainer()) {
        hmap<double, wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = StringMinus(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringMinus(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<double, wstring>::iterator it;
    
    Tamgu* a;
    vector<double> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    long sz = keys.size();
    a = aNULL;
    bool testcond = false;
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        var->storevalue(keys[i]);

        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;

}

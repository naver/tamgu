/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapuu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguuvector.h"
#include "tamguustring.h"
//#prime
#include "tamgumapuu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapuuMethod>  Tamgumapuu::methods;
Exporting hmap<string, string> Tamgumapuu::infomethods;
Exporting bin_hash<unsigned long> Tamgumapuu::exported;

Exporting short Tamgumapuu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapuu::AddMethod(TamguGlobal* global, string name,mapuuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapuu::Setidtype(TamguGlobal* global) {
        Tamgumapuu::idtype = global->Getid("mapuu");
    }

   bool Tamgumapuu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgumapuu::idtype = global->Getid("mapuu");

    Tamgumapuu::AddMethod(global, "clear", &Tamgumapuu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapuu::AddMethod(global, "invert", &Tamgumapuu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapuu::AddMethod(global, "find", &Tamgumapuu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapuu::AddMethod(global, "items", &Tamgumapuu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapuu::AddMethod(global, "join", &Tamgumapuu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapuu::AddMethod(global, "test", &Tamgumapuu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapuu::AddMethod(global, "keys", &Tamgumapuu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapuu::AddMethod(global, "values", &Tamgumapuu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapuu::AddMethod(global, "pop", &Tamgumapuu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapuu::AddMethod(global, "merge", &Tamgumapuu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgumapuu::idtype] = new Tamgumapuu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("uumap")] = new Tamgumapuu(global);

    global->RecordMethods(global->Getid("uumap"), Tamgumapuu::exported);
    #endif
    global->RecordMethods(Tamgumapuu::idtype, Tamgumapuu::exported);

    return true;
}


Exporting TamguIteration* Tamgumapuu::Newiteration(bool direction) {
    return new TamguIterationmapuu(this, direction);
}


Exporting Tamgu* Tamgumapuu::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    wstring val = a->UString();

     if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isString()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Provideustring(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgumapuu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
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
            return globalTamgu->Provideustring(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapuu::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    wstring s = k->UString();
    values[s] = v->UString();
    return aTRUE;
}

Exporting Tamgu* Tamgumapuu::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgumapuu::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgumapuu::String() {
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
        w = it.first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamgumapuu::JSonString() {
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
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
        res << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamgumapuu::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgumapuu::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgumapuu::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgumapuu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgumapuu::Size() {
    Locking _lock(this);
    return values.size();
}

#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
    #endif


Exporting Tamgu*  Tamgumapuu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapuu::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapuu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyustring()] = itr->Valueustring();
            itr->Release();
            return aTRUE;
        }
        
        wchar_t buff[20];
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                swprintf_s(buff, 20, L"%d", nb);
                values[buff] = ke->getustring(it);
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
                swprintf_s(buff, 20, L"%d", nb);
                values[buff] = it->UString();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        Locking* _lock = _getlock(this);
        values.clear();
        if (ke->Type() == Tamgumapuu::idtype)
            values = ((Tamgumapuu*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyustring()] = itr->Valueustring();
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Locking* _lock = _getlock(this);
    values[idx->UString()] = ke->UString();
    _cleanlock(_lock);
    return aTRUE;
}


Exporting Tamgu* Tamgumapuu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            hmap<wstring,wstring>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideustring(it->first));
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
        Tamgumapuu* kmap = new Tamgumapuu;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        hmap<wstring,wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<wstring, wstring>::iterator itr= values.end();
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

    wstring skey = key->UString();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgumapuu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapuu* m = (Tamgumapuu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<wstring,wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapuu::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgumapuu* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuu;
        hmap<wstring, wstring> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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
        res = (Tamgumapuu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringXor(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapuu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapuu* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapuu*)Atom(true);

    if (b->isMapContainer()) {
        hmap<wstring,wstring>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapuu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringOr(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapuu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapuu* res;
    if (b->isMapContainer()) {
        hmap<wstring,wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]==v)
                res->values[itr->Keyustring()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapuu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringAnd(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapuu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapuu* res;
    if (b->isMapContainer()) {
        hmap<wstring,wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyustring());
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
        res = (Tamgumapuu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapuu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapuu * res;
    if (b->isMapContainer()) {
        hmap<wstring,wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyustring());
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
        res = (Tamgumapuu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapuu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<wstring, wstring>::iterator it;
    
    Tamgu* a;
    vector<wstring> keys;

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

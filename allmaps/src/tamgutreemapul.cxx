/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapul.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamguuvector.h"
#include "tamguustring.h"
//#prime
#include "tamgutreemapul.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapulMethod>  Tamgutreemapul::methods;
Exporting map<string, string> Tamgutreemapul::infomethods;
Exporting basebin_hash<unsigned long> Tamgutreemapul::exported;

Exporting short Tamgutreemapul::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapul::AddMethod(TamguGlobal* global, string name,treemapulMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



void Tamgutreemapul::Setidtype(TamguGlobal* global) {
    Tamgutreemapul::InitialisationModule(global,"");
}


bool Tamgutreemapul::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgutreemapul::idtype = global->Getid("treemapul");
    
    Tamgutreemapul::AddMethod(global, "clear", &Tamgutreemapul::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapul::AddMethod(global, "invert", &Tamgutreemapul::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapul::AddMethod(global, "find", &Tamgutreemapul::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamgutreemapul::AddMethod(global, "items", &Tamgutreemapul::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamgutreemapul::AddMethod(global, "join", &Tamgutreemapul::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapul::AddMethod(global, "test", &Tamgutreemapul::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapul::AddMethod(global, "keys", &Tamgutreemapul::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapul::AddMethod(global, "values", &Tamgutreemapul::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapul::AddMethod(global, "sum", &Tamgutreemapul::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemapul::AddMethod(global, "product", &Tamgutreemapul::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemapul::AddMethod(global, "pop", &Tamgutreemapul::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapul::AddMethod(global, "merge", &Tamgutreemapul::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    if (version != "") {
        global->newInstance[Tamgutreemapul::idtype] = new Tamgutreemapul(global);
        
        global->RecordMethods(Tamgutreemapul::idtype, Tamgutreemapul::exported);
    }
    
    return true;
}


Exporting TamguIteration* Tamgutreemapul::Newiteration(bool direction) {
    return new TamguIterationtreemapul(this, direction);
}


Exporting Tamgu* Tamgutreemapul::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    wstring val = a->UString();
    
    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        try {
            values.at(val);
            v->values.push_back(val);
        }
        catch(const std::out_of_range& oor) {}
        
        return v;
    }
    
    if (context->isString()) {
        locking();
        try {
            values.at(val);
            unlocking();
            return globalTamgu->Providewithustring(val);
        }
        catch(const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }
    
    locking();
    try {
        values.at(val);
        unlocking();
        return aTRUE;
    }
    catch(const std::out_of_range& oor) {
        unlocking();
        return aFALSE;
    }
    
}



Exporting Tamgu* Tamgutreemapul::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    
    BLONG val = callfunc->Evaluate(0, context, idthread)->Long();
    
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



Exporting Tamgu* Tamgutreemapul::Push(Tamgu* k, Tamgu* v) {
    locking();
    wstring s = k->UString();
    values[s] = v->Long();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapul::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgutreemapul::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgutreemapul::String() {
    locking();
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
        res << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgutreemapul::JSonString() {
    locking();
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
        jstringing(res, sx);
        res << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgutreemapul::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgutreemapul::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgutreemapul::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgutreemapul::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgutreemapul::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
#endif


Exporting Tamgu*  Tamgutreemapul::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgutreemapul::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgutreemapul*)ke)->values;
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyustring()] = itr->Valuelong();
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
                values[buff] = ke->getinteger(it);
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
                values[buff] = it->Long();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgutreemapul::idtype)
            values = ((Tamgutreemapul*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyustring()] = itr->Valuelong();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    wstring skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);

    locking();
    values[skey] = ke->Long();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapul::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isMapContainer())
            return this;
        
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            locking();
            map<wstring,BLONG>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideustring(it->first));
            unlocking();
            return vect;
        }
        
        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }
        
        return this;
    }
    
    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgutreemapul* kmap = new Tamgutreemapul;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        map<wstring,BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<wstring, BLONG>::iterator itr= values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                if (keyright != ((TamguIndex*)idx)->right)
                    keyright->Release();
                return kmap;
            }
        }
        if (keyright != ((TamguIndex*)idx)->right)
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
    
    wstring skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);
    
    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgutreemapul::same(Tamgu* a) {
    
    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);
    
    Tamgutreemapul* m = (Tamgutreemapul*)a;
    
    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<wstring,BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapul::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgutreemapul;
        map<wstring, BLONG> keys;
        
        for (auto& it : values)
            keys[it.first] = it.second;
        
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuelong();
            else {
                if (values[v]==itr->Valuelong())
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
        res = (Tamgutreemapul*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : values)
        it.second = it.second ^ v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second | v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemapul;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]==v)
                res->values[itr->Keyustring()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second & v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) + itr->Valuelong();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) + itr->Valuelong();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) * itr->Valuelong();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second *= v;
    return res;
    
}

Exporting Tamgu* Tamgutreemapul::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) * itr->Valuelong();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;
    
}

Exporting Tamgu* Tamgutreemapul::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k)  % itr->Valueinteger();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = it.second % v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) >> itr->Valueinteger();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second >> v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) << itr->Valueinteger();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second << v;
    return res;
}

Exporting Tamgu* Tamgutreemapul::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapul * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapul;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = pow(values.at(k), itr->Valuefloat());
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }

    
    if (itself)
        res = this;
    else
        res = (Tamgutreemapul*)Atom(true);
    
    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapul::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    
    map<wstring, BLONG>::iterator it;
    
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

    unlocking();
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }
    
    a->Releasenonconst();
    return this;
    
}


/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapuf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamguuvector.h"
#include "tamguustring.h"
//#prime
#include "tamgumapuf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapufMethod>  Tamgumapuf::methods;
Exporting hmap<string, string> Tamgumapuf::infomethods;
Exporting bin_hash<unsigned long> Tamgumapuf::exported;

Exporting short Tamgumapuf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapuf::AddMethod(TamguGlobal* global, string name,mapufMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



void Tamgumapuf::Setidtype(TamguGlobal* global) {
    Tamgumapuf::InitialisationModule(global,"");
}


bool Tamgumapuf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamgumapuf::idtype = global->Getid("mapuf");
    
    Tamgumapuf::AddMethod(global, "clear", &Tamgumapuf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapuf::AddMethod(global, "invert", &Tamgumapuf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapuf::AddMethod(global, "find", &Tamgumapuf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamgumapuf::AddMethod(global, "items", &Tamgumapuf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamgumapuf::AddMethod(global, "join", &Tamgumapuf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapuf::AddMethod(global, "test", &Tamgumapuf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapuf::AddMethod(global, "keys", &Tamgumapuf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapuf::AddMethod(global, "values", &Tamgumapuf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapuf::AddMethod(global, "sum", &Tamgumapuf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapuf::AddMethod(global, "product", &Tamgumapuf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapuf::AddMethod(global, "pop", &Tamgumapuf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapuf::AddMethod(global, "merge", &Tamgumapuf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    if (version != "") {
        global->newInstance[Tamgumapuf::idtype] = new Tamgumapuf(global);
        
        global->RecordMethods(Tamgumapuf::idtype, Tamgumapuf::exported);
    }
    
    return true;
}


Exporting TamguIteration* Tamgumapuf::Newiteration(bool direction) {
    return new TamguIterationmapuf(this, direction);
}


Exporting Tamgu* Tamgumapuf::in(Tamgu* context, Tamgu* a, short idthread) {
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



Exporting Tamgu* Tamgumapuf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
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



Exporting Tamgu* Tamgumapuf::Push(Tamgu* k, Tamgu* v) {
    locking();
    wstring s = k->UString();
    values[s] = v->Float();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgumapuf::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgumapuf::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgumapuf::String() {
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

Exporting string Tamgumapuf::JSonString() {
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


Exporting long Tamgumapuf::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapuf::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapuf::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapuf::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapuf::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
#endif


Exporting Tamgu*  Tamgumapuf::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapuf::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapuf*)ke)->values;
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyustring()] = itr->Valuefloat();
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
                values[buff] = ke->getfloat(it);
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
                values[buff] = it->Float();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgumapuf::idtype)
            values = ((Tamgumapuf*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyustring()] = itr->Valuefloat();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    wstring skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);

    locking();
    values[skey] = ke->Float();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgumapuf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    
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
            hmap<wstring,double>::iterator it;
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
        Tamgumapuf* kmap = new Tamgumapuf;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        hmap<wstring,double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<wstring, double>::iterator itr= values.end();
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

Exporting Tamgu* Tamgumapuf::same(Tamgu* a) {
    
    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);
    
    Tamgumapuf* m = (Tamgumapuf*)a;
    
    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<wstring,double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapuf::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgumapuf;
        hmap<wstring, double> keys;
        
        for (auto& it : values)
            keys[it.first] = it.second;
        
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuefloat();
            else {
                if (values[v]==itr->Valuefloat())
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
        res = (Tamgumapuf*)Atom(true);
    
    double v = b->Float();
    for (auto& it : values)
        it.second = DoubleXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapuf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapuf*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapuf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapuf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]==v)
                res->values[itr->Keyustring()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapuf*)Atom(true);
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapuf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) + itr->Valuefloat();
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
        res = (Tamgumapuf*)Atom(true);
    
    double v = b->Float();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapuf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) + itr->Valuefloat();
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
        res = (Tamgumapuf*)Atom(true);
    
    double v = b->Float();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgumapuf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) * itr->Valuefloat();
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
        res = (Tamgumapuf*)Atom(true);
    
    double v = b->Float();
    for (auto& it : res->values)
        it.second *= v;
    return res;
    
}

Exporting Tamgu* Tamgumapuf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = values.at(k) * itr->Valuefloat();
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
        res = (Tamgumapuf*)Atom(true);
    
    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;
    
}

Exporting Tamgu* Tamgumapuf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = (long)values.at(k)  % itr->Valueinteger();
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
        res = (Tamgumapuf*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Tamgu* Tamgumapuf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = DoubleShiftright(values.at(k), itr->Valueinteger());
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
        res = (Tamgumapuf*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftright(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapuf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
        wstring k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyustring();
            try {
                res->values[k] = DoubleShiftleft(values.at(k), itr->Valueinteger());
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
        res = (Tamgumapuf*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftleft(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapuf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapuf;
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
        res = (Tamgumapuf*)Atom(true);
    
    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapuf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    
    hmap<wstring, double>::iterator it;
    
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


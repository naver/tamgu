/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapsi.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamgusvector.h"
#include "tamgustring.h"
//#prime
#include "tamgumapsi.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapsiMethod>  Tamgumapsi::methods;
static ThreadLock classlock;

Exporting short Tamgumapsi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapsi::AddMethod(TamguGlobal* global, string name,mapsiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamgumapsi::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgumapsi::methods.isEmpty())
    Tamgumapsi::InitialisationModule(global,"");
}


bool Tamgumapsi::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    
    
    Tamgumapsi::idtype = global->Getid("mapsi");
    
    
    

    Tamgumapsi::AddMethod(global, "clear", &Tamgumapsi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapsi::AddMethod(global, "invert", &Tamgumapsi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapsi::AddMethod(global, "find", &Tamgumapsi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamgumapsi::AddMethod(global, "items", &Tamgumapsi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamgumapsi::AddMethod(global, "join", &Tamgumapsi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapsi::AddMethod(global, "test", &Tamgumapsi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapsi::AddMethod(global, "keys", &Tamgumapsi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapsi::AddMethod(global, "values", &Tamgumapsi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapsi::AddMethod(global, "sum", &Tamgumapsi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapsi::AddMethod(global, "product", &Tamgumapsi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapsi::AddMethod(global, "pop", &Tamgumapsi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapsi::AddMethod(global, "merge", &Tamgumapsi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    if (version != "") {
        global->returnindextypes[Tamgumapsi::idtype] = a_int;        
    global->minimal_indexes[Tamgumapsi::idtype] = true;

        global->newInstance[Tamgumapsi::idtype] = new Tamgumapsi(global);
        
        global->RecordCompatibilities(Tamgumapsi::idtype);
    }
    
    return true;
}


Exporting TamguIteration* Tamgumapsi::Newiteration(bool direction) {
    return new TamguIterationmapsi(this, direction);
}


Exporting Tamgu* Tamgumapsi::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    string val = a->String();
    
    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
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
            return globalTamgu->Providewithstring(val);
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



Exporting Tamgu* Tamgumapsi::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    
    long val = callfunc->Evaluate(0, context, idthread)->Integer();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (const auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        for (const auto& it : values) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (const auto& it : values) {
        if (it.second == val)
            return globalTamgu->Providestring(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapsi::Push(Tamgu* k, Tamgu* v) {
    locking();
    string s = k->String();
    values[s] = v->Integer();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgumapsi::Pop(Tamgu* kkey) {
    string k = kkey->String();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgumapsi::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgumapsi::String() {
    locking();
    stringstream res;
    
    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        stringing(res, sx);
        res << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgumapsi::JSonString() {
    locking();
    stringstream res;
    
    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        jstringing(res, sx);
        res << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgumapsi::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapsi::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapsi::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapsi::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapsi::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapsi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapsi::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapsi*)ke)->values;
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valueinteger();
            itr->Release();
            return aTRUE;
        }
        char buff[20];
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(buff, 20, "%ld", nb);
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
            
            for (const auto& it : kvect->values) {
                sprintf_s(buff, 20, "%ld", nb);
                values[buff] = it->Integer();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgumapsi::idtype)
            values = ((Tamgumapsi*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keystring()] = itr->Valueinteger();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    string skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);

    locking();
    values[skey] = ke->Integer();
    unlocking();
    return aTRUE;
}



Tamgu* Tamgumapsi::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    string skey;
    key->Setstring(skey, idthread);

    Tamgu* val = Value(skey);
    if (val == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return val;
}

Exporting Tamgu* Tamgumapsi::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    
    if (!idx->isIndex()) {
        if (contextualpattern->isMapContainer())
            return this;
        
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isMapContainer())
            return this;
        
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            locking();
            hmap<string,long>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providestring(it->first));
            unlocking();
            return vect;
        }
        
        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->ProvideConstint(v);
        }
        
        return this;
    }
    
    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgumapsi* kmap = new Tamgumapsi;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        hmap<string,long>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<string, long>::iterator itr= values.end();
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
    
    string skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);
    
    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumapsi::same(Tamgu* a) {
    
    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);
    
    Tamgumapsi* m = (Tamgumapsi*)a;
    
    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<string,long>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapsi::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgumapsi;
        hmap<string, long> keys;
        
        for (const auto& it : values)
            keys[it.first] = it.second;
        
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valueinteger();
            else {
                if (values[v]==itr->Valueinteger())
                    keys.erase(v);
            }
        }
        itr->Release();
        
        for (const auto& a : keys)
            res->values[a.first]= a.second;
        
        return res;
        
        
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapsi*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : values)
        it.second = it.second ^ v;
    return res;
}

Exporting Tamgu* Tamgumapsi::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapsi*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second | v;
    return res;
}

Exporting Tamgu* Tamgumapsi::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]==v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapsi*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second & v;
    return res;
}

Exporting Tamgu* Tamgumapsi::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
            try {
                res->values[k] = values.at(k) + itr->Valueinteger();
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
        res = (Tamgumapsi*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapsi::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
            try {
                res->values[k] = values.at(k) - itr->Valueinteger();
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
        res = (Tamgumapsi*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgumapsi::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
            try {
                res->values[k] = values.at(k) * itr->Valueinteger();
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
        res = (Tamgumapsi*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second *= v;
    return res;
    
}

Exporting Tamgu* Tamgumapsi::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi * res;
    long v;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                itr->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }
            try {
                res->values[k] = values.at(k) / v;
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
        res = (Tamgumapsi*)Atom(true);
    
    v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;
    
}

Exporting Tamgu* Tamgumapsi::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi * res;
    long v;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                itr->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }
            try {
                res->values[k] = values.at(k) % v;
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
        res = (Tamgumapsi*)Atom(true);
    
    v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }
    for (auto& it : res->values)
        it.second = it.second % v;
    return res;
}

Exporting Tamgu* Tamgumapsi::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgumapsi*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second >> v;
    return res;
}

Exporting Tamgu* Tamgumapsi::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgumapsi*)Atom(true);
    
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second << v;
    return res;
}

Exporting Tamgu* Tamgumapsi::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsi;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgumapsi*)Atom(true);
    
    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapsi::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    
    hmap<string, long>::iterator it;
    
    Tamgu* a;
    vector<string> keys;
    
    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);
    
    long sz = keys.size();
    a = aNULL;
    bool testcond = false;
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        var->Storevalue(keys[i]);
        
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


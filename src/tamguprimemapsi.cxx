/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapsi.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamgusvector.h"
#include "tamgustring.h"
//#prime
#include "tamguprimemapsi.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapsiMethod>  Tamguprimemapsi::methods;
Exporting hmap<string, string> Tamguprimemapsi::infomethods;
Exporting basebin_hash<unsigned long> Tamguprimemapsi::exported;

Exporting short Tamguprimemapsi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapsi::AddMethod(TamguGlobal* global, string name,primemapsiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



void Tamguprimemapsi::Setidtype(TamguGlobal* global) {
    Tamguprimemapsi::InitialisationModule(global,"");
}


bool Tamguprimemapsi::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    
    
    Tamguprimemapsi::idtype = global->Getid("primemapsi");
    
    Tamguprimemapsi::AddMethod(global, "clear", &Tamguprimemapsi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapsi::AddMethod(global, "invert", &Tamguprimemapsi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapsi::AddMethod(global, "find", &Tamguprimemapsi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamguprimemapsi::AddMethod(global, "items", &Tamguprimemapsi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamguprimemapsi::AddMethod(global, "join", &Tamguprimemapsi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapsi::AddMethod(global, "test", &Tamguprimemapsi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapsi::AddMethod(global, "keys", &Tamguprimemapsi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapsi::AddMethod(global, "values", &Tamguprimemapsi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapsi::AddMethod(global, "sum", &Tamguprimemapsi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguprimemapsi::AddMethod(global, "product", &Tamguprimemapsi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguprimemapsi::AddMethod(global, "pop", &Tamguprimemapsi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapsi::AddMethod(global, "merge", &Tamguprimemapsi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    if (version != "") {
        global->newInstance[Tamguprimemapsi::idtype] = new Tamguprimemapsi(global);
        
        global->RecordMethods(Tamguprimemapsi::idtype, Tamguprimemapsi::exported);
    }
    
    return true;
}


Exporting TamguIteration* Tamguprimemapsi::Newiteration(bool direction) {
    return new TamguIterationprimemapsi(this, direction);
}


Exporting Tamgu* Tamguprimemapsi::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    string val = a->String();
    
    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);
        
        return v;
    }
    
    if (context->isString()) {
        locking();
        if (values.find(val)!=values.end()) {
            unlocking();
            return globalTamgu->Providewithstring(val);
        }
        unlocking();
        return aNOELEMENT;
    }
    
    locking();
    if (values.find(val)!=values.end()) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
    
}



Exporting Tamgu* Tamguprimemapsi::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    prime_hash<string, long>::iterator it;
    
    long val = callfunc->Evaluate(0, context, idthread)->Integer();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    
    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                v->values.push_back(it->first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (it = values.begin(); it != values.end(); it++) {
        if (it->second == val)
            return globalTamgu->Providestring(it->first);
    }
    
    return aNULL;
    
}



Exporting Tamgu* Tamguprimemapsi::Push(Tamgu* k, Tamgu* v) {
    locking();
    string s = k->String();
    values[s] = v->Integer();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapsi::Pop(Tamgu* kkey) {
    string k = kkey->String();
    locking();
    if (values.find(k) != values.end()) {
        values.erase(k);
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamguprimemapsi::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamguprimemapsi::String() {
    locking();
    stringstream res;
    prime_hash<string,long>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        stringing(res, sx);
        res << ":" << it->second;
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapsi::JSonString() {
    locking();
    stringstream res;
    prime_hash<string,long>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        jstringing(res, sx);
        res << ":" << it->second;
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamguprimemapsi::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamguprimemapsi::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamguprimemapsi::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamguprimemapsi::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamguprimemapsi::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamguprimemapsi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        
        if (ke->Type() == Tamguprimemapsi::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemapsi*)ke)->values;
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                sprintf_s(buff, 20, "%ld", nb);
                values[buff] = (*it)->Integer();
                nb++;
            }
            return aTRUE;
        }
        
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        
        locking();
        values.clear();
        if (ke->Type() == Tamguprimemapsi::idtype) {
            Tamguprimemapsi* kmap = (Tamguprimemapsi*)ke;
            //We copy all values from ke to this
            prime_hash<string,long>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
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


Exporting Tamgu* Tamguprimemapsi::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    
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
            prime_hash<string,long>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providestring(it->first));
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
        Tamguprimemapsi* kmap = new Tamguprimemapsi;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        prime_hash<string,long>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<string, long>::iterator itr= values.end();
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
    
    string skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);
    
    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamguprimemapsi::same(Tamgu* a) {
    
    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);
    
    Tamguprimemapsi* m = (Tamguprimemapsi*)a;
    
    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<string,long>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapsi::xorset(Tamgu* b, bool itself) {
    
    Doublelocking _lock(this, b);
    
    
    Tamguprimemapsi* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        hmap<string, long> keys;
        
        prime_hash<string, long>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
        
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
        
        for (auto& a : keys)
            res->values[a.first]= a.second;
        
        return res;
        
        
    }
    
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    prime_hash<string,long>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second ^ v;
    return res;
}

Exporting Tamgu* Tamguprimemapsi::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second | v;
    return res;
    
}

Exporting Tamgu* Tamguprimemapsi::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
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
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second & v;
    return res;
}

Exporting Tamgu* Tamguprimemapsi::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemapsi::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = it->second - v;
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Tamgu* Tamguprimemapsi::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = it->second * v;
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;
    
}

Exporting Tamgu* Tamguprimemapsi::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = it->second / v;
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;
    
}

Exporting Tamgu* Tamguprimemapsi::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = it->second % v;
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second % v;
    return res;
}

Exporting Tamgu* Tamguprimemapsi::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = it->second >> v;
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second >> v;
    return res;
}

Exporting Tamgu* Tamguprimemapsi::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = it->second << v;
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second << v;
    return res;
}

Exporting Tamgu* Tamguprimemapsi::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,long>::iterator it;
    Tamguprimemapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsi;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = pow(it->second, v);
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsi*)Atom(true);
    
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapsi::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    
    prime_hash<string, long>::iterator it;
    
    Tamgu* a;
    vector<string> keys;
    
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


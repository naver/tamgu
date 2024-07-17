/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapsf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamgusvector.h"
#include "tamgustring.h"
//#prime
#include "tamguprimemapsf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapsfMethod>  Tamguprimemapsf::methods;
static ThreadLock classlock;

Exporting short Tamguprimemapsf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapsf::AddMethod(TamguGlobal* global, string name, primemapsfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamguprimemapsf::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamguprimemapsf::methods.isEmpty())
    Tamguprimemapsf::InitialisationModule(global,"");
}


bool Tamguprimemapsf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    
    
    Tamguprimemapsf::idtype = global->Getid("primemapsf");
    
    
    

    Tamguprimemapsf::AddMethod(global, "clear", &Tamguprimemapsf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapsf::AddMethod(global, "invert", &Tamguprimemapsf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapsf::AddMethod(global, "find", &Tamguprimemapsf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamguprimemapsf::AddMethod(global, "items", &Tamguprimemapsf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamguprimemapsf::AddMethod(global, "join", &Tamguprimemapsf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapsf::AddMethod(global, "test", &Tamguprimemapsf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapsf::AddMethod(global, "keys", &Tamguprimemapsf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapsf::AddMethod(global, "values", &Tamguprimemapsf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapsf::AddMethod(global, "sum", &Tamguprimemapsf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguprimemapsf::AddMethod(global, "product", &Tamguprimemapsf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguprimemapsf::AddMethod(global, "pop", &Tamguprimemapsf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapsf::AddMethod(global, "merge", &Tamguprimemapsf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    if (version != "") {
        global->returnindextypes[Tamguprimemapsf::idtype] = a_float;        
    global->minimal_indexes[Tamguprimemapsf::idtype] = true;

        global->newInstance[Tamguprimemapsf::idtype] = new Tamguprimemapsf(global);
        
        global->RecordCompatibilities(Tamguprimemapsf::idtype);
    }
    
    return true;
}


Exporting TamguIteration* Tamguprimemapsf::Newiteration(bool direction) {
    return new TamguIterationprimemapsf(this, direction);
}


Exporting Tamgu* Tamguprimemapsf::in(Tamgu* context, Tamgu* a, short idthread) {
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



Exporting Tamgu* Tamguprimemapsf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    prime_hash<string, double>::iterator it;
    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
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



Exporting Tamgu* Tamguprimemapsf::Push(Tamgu* k, Tamgu* v) {
    locking();
    string s = k->String();
    values[s] = v->Float();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapsf::Pop(Tamgu* kkey) {
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

Exporting void Tamguprimemapsf::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamguprimemapsf::String() {
    locking();
    stringstream res;
    prime_hash<string, double>::iterator it;
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

Exporting string Tamguprimemapsf::JSonString() {
    locking();
    stringstream res;
    prime_hash<string, double>::iterator it;
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


Exporting long Tamguprimemapsf::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamguprimemapsf::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamguprimemapsf::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamguprimemapsf::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamguprimemapsf::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamguprimemapsf::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        
        if (ke->Type() == Tamguprimemapsf::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemapsf*)ke)->values;
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valuefloat();
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                sprintf_s(buff, 20, "%ld", nb);
                values[buff] = (*it)->Float();
                nb++;
            }
            return aTRUE;
        }
        
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        
        locking();
        values.clear();
        if (ke->Type() == Tamguprimemapsf::idtype) {
            Tamguprimemapsf* kmap = (Tamguprimemapsf*)ke;
            //We copy all values from ke to this
            prime_hash<string, double>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keystring()] = itr->Valuefloat();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    
    string skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);

    locking();
    values[skey] = ke->Float();
    unlocking();
    return aTRUE;
}



Tamgu* Tamguprimemapsf::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
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

Exporting Tamgu* Tamguprimemapsf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    
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
            prime_hash<string, double>::iterator it;
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
        Tamguprimemapsf* kmap = new Tamguprimemapsf;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        prime_hash<string, double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<string, double>::iterator itr = values.end();
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

Exporting Tamgu* Tamguprimemapsf::same(Tamgu* a) {
    
    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);
    
    Tamguprimemapsf* m = (Tamguprimemapsf*)a;
    
    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<string, double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapsf::xorset(Tamgu* b, bool itself) {
    
    Doublelocking _lock(this, b);
    
    
    Tamguprimemapsf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        hmap<string, double> keys;
        
        prime_hash<string, double>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
        
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuefloat();
            else {
                if (values[v]==itr->Valuefloat())
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
        res = (Tamguprimemapsf*)Atom(true);
    
    prime_hash<string, double>::iterator it;
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleXor(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapsf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsf*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleOr(it->second, v);
    return res;
    
}

Exporting Tamgu* Tamguprimemapsf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()] == v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsf*)Atom(true);
    
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleAnd(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapsf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamguprimemapsf*)Atom(true);
    
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemapsf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamguprimemapsf*)Atom(true);
    
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Tamgu* Tamguprimemapsf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
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
        res = (Tamguprimemapsf*)Atom(true);
    
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;
    
}

Exporting Tamgu* Tamguprimemapsf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
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
        res = (Tamguprimemapsf*)Atom(true);
    
    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;
    
}

Exporting Tamgu* Tamguprimemapsf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }
            
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second % v;
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsf*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Tamgu* Tamguprimemapsf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = DoubleShiftright(it->second, v);
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsf*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftright(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapsf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
            if (it != values.end()) {
                res->values[it->first] = DoubleShiftleft(it->second, v);
            }
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapsf*)Atom(true);
    
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftleft(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapsf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string, double>::iterator it;
    Tamguprimemapsf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapsf;
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
        res = (Tamguprimemapsf*)Atom(true);
    
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapsf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    
    prime_hash<string, double>::iterator it;
    
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


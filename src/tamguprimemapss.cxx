/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapss.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#include "tamgu.h"
#include "tamgusvector.h"
#include "tamgustring.h"
#include "tamguprimemapss.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapssMethod>  Tamguprimemapss::methods;
static ThreadLock classlock;

Exporting short Tamguprimemapss::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapss::AddMethod(TamguGlobal* global, string name,primemapssMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




void Tamguprimemapss::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamguprimemapss::methods.isEmpty())
    Tamguprimemapss::InitialisationModule(global,"");
}


bool Tamguprimemapss::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    
    
    
    Tamguprimemapss::idtype = global->Getid("primemapss");
    
    
    

    Tamguprimemapss::AddMethod(global, "clear", &Tamguprimemapss::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapss::AddMethod(global, "invert", &Tamguprimemapss::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapss::AddMethod(global, "find", &Tamguprimemapss::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");
    
    
    Tamguprimemapss::AddMethod(global, "items", &Tamguprimemapss::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");
    
    
    Tamguprimemapss::AddMethod(global, "join", &Tamguprimemapss::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapss::AddMethod(global, "test", &Tamguprimemapss::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapss::AddMethod(global, "keys", &Tamguprimemapss::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapss::AddMethod(global, "values", &Tamguprimemapss::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapss::AddMethod(global, "pop", &Tamguprimemapss::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapss::AddMethod(global, "merge", &Tamguprimemapss::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");
    
    if (version != "") {
        global->returnindextypes[Tamguprimemapss::idtype] = a_string;        
    global->minimal_indexes[Tamguprimemapss::idtype] = true;

        global->newInstance[Tamguprimemapss::idtype] = new Tamguprimemapss(global);
        
        global->RecordCompatibilities(Tamguprimemapss::idtype);
    }
    
    return true;
}


Exporting TamguIteration* Tamguprimemapss::Newiteration(bool direction) {
    return new TamguIterationprimemapss(this, direction);
}


Exporting Tamgu* Tamguprimemapss::in(Tamgu* context, Tamgu* a, short idthread) {
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



Exporting Tamgu* Tamguprimemapss::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    prime_hash<string, string>::iterator it;
    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
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



Exporting Tamgu* Tamguprimemapss::Push(Tamgu* k, Tamgu* v) {
    locking();
    string s = k->String();
    values[s] = v->String();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapss::Pop(Tamgu* kkey) {
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

Exporting void Tamguprimemapss::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamguprimemapss::String() {
    locking();
    stringstream res;
    prime_hash<string,string>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        stringing(res, sx);
        res << ":";
        sx = it->second;
        stringing(res, sx);
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapss::JSonString() {
    locking();
    stringstream res;
    prime_hash<string,string>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it->first;
        jstringing(res, sx);
        res << ":";
        sx = it->second;
        jstringing(res, sx);
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamguprimemapss::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamguprimemapss::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamguprimemapss::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamguprimemapss::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamguprimemapss::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamguprimemapss::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    
    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;
        
        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        
        if (ke->Type() == Tamguprimemapss::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemapss*)ke)->values;
            return aTRUE;
        }
        
        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valuestring();
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
                values[buff] = ke->getstring(it);
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
                values[buff] = (*it)->String();
                nb++;
            }
            return aTRUE;
        }
        
        
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        
        locking();
        values.clear();
        if (ke->Type() == Tamguprimemapss::idtype) {
            Tamguprimemapss* kmap = (Tamguprimemapss*)ke;
            //We copy all values from ke to this
            prime_hash<string,string>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keystring()] = itr->Valuestring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    
    string skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);

    locking();
    values[skey] = ke->String();
    unlocking();
    return aTRUE;
}



Tamgu* Tamguprimemapss::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
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

Exporting Tamgu* Tamguprimemapss::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    
    
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
            prime_hash<string,string>::iterator it;
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
        Tamguprimemapss* kmap = new Tamguprimemapss;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        prime_hash<string,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<string, string>::iterator itr= values.end();
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

Exporting Tamgu* Tamguprimemapss::same(Tamgu* a) {
    
    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);
    
    Tamguprimemapss* m = (Tamguprimemapss*)a;
    
    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<string,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapss::xorset(Tamgu* b, bool itself) {
    
    Doublelocking _lock(this, b);
    
    
    Tamguprimemapss* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapss;
        hmap<string, string> keys;
        
        prime_hash<string, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
        
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuestring();
            else {
                if (values[v]==itr->Valuestring())
                    keys.erase(v);
            }
        }
        itr->Release();
        
        for (const auto& a : keys)
            res->values[a.first]= a.second;
        
        return res;
        
        
    }
    
    prime_hash<string, string>::iterator it;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapss*)Atom(true);
    
    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringXor(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapss::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,string>::iterator it;
    Tamguprimemapss* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapss*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapss*)Atom(true);
    
    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringOr(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapss::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,string>::iterator it;
    Tamguprimemapss* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapss;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]==v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamguprimemapss*)Atom(true);
    
    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringAnd(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapss::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,string>::iterator it;
    Tamguprimemapss* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapss;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
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
        res = (Tamguprimemapss*)Atom(true);
    
    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemapss::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    prime_hash<string,string>::iterator it;
    Tamguprimemapss * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamguprimemapss;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keystring());
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
        res = (Tamguprimemapss*)Atom(true);
    
    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapss::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    
    prime_hash<string, string>::iterator it;
    
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


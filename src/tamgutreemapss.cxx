/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapss.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgusvector.h"
#include "tamgustring.h"
#include "tamgutreemapss.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapssMethod>  Tamgutreemapss::methods;
Exporting hmap<string, string> Tamgutreemapss::infomethods;
Exporting bin_hash<unsigned long> Tamgutreemapss::exported;

Exporting short Tamgutreemapss::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapss::AddMethod(TamguGlobal* global, string name,treemapssMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutreemapss::Setidtype(TamguGlobal* global) {
    Tamgutreemapss::InitialisationModule(global,"");
}


   bool Tamgutreemapss::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgutreemapss::idtype = global->Getid("treemapss");

    Tamgutreemapss::AddMethod(global, "clear", &Tamgutreemapss::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapss::AddMethod(global, "invert", &Tamgutreemapss::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapss::AddMethod(global, "find", &Tamgutreemapss::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapss::AddMethod(global, "items", &Tamgutreemapss::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapss::AddMethod(global, "join", &Tamgutreemapss::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapss::AddMethod(global, "test", &Tamgutreemapss::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapss::AddMethod(global, "keys", &Tamgutreemapss::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapss::AddMethod(global, "values", &Tamgutreemapss::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapss::AddMethod(global, "pop", &Tamgutreemapss::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapss::AddMethod(global, "merge", &Tamgutreemapss::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgutreemapss::idtype] = new Tamgutreemapss(global);
        
        global->RecordMethods(Tamgutreemapss::idtype, Tamgutreemapss::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgutreemapss::Newiteration(bool direction) {
    return new TamguIterationtreemapss(this, direction);
}


Exporting Tamgu* Tamgutreemapss::in(Tamgu* context, Tamgu* a, short idthread) {
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



Exporting Tamgu* Tamgutreemapss::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
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
            return globalTamgu->Providestring(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgutreemapss::Push(Tamgu* k, Tamgu* v) {
    locking();
    string s = k->String();
    values[s] = v->String();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapss::Pop(Tamgu* kkey) {
    string k = kkey->String();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgutreemapss::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgutreemapss::String() {
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
        res << ":";
        sx = it.second;
        stringing(res, sx);
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgutreemapss::JSonString() {
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
        res << ":";
        sx = it.second;
        jstringing(res, sx);
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgutreemapss::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgutreemapss::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgutreemapss::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgutreemapss::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgutreemapss::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgutreemapss::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgutreemapss::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgutreemapss*)ke)->values;
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

            for (auto& it : kvect->values) {
                sprintf_s(buff, 20, "%ld", nb);
                values[buff] = it->String();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgutreemapss::idtype)
            values = ((Tamgutreemapss*)ke)->values;
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


Exporting Tamgu* Tamgutreemapss::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            map<string,string>::iterator it;
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
        Tamgutreemapss* kmap = new Tamgutreemapss;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        map<string,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<string, string>::iterator itr= values.end();
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

Exporting Tamgu* Tamgutreemapss::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapss* m = (Tamgutreemapss*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<string,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapss::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgutreemapss* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapss;
        hmap<string, string> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
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

        for (auto& a : keys)
            res->values[a.first]= a.second;

        return res;


    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapss*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringXor(it.second,v);
    return res;
}

Exporting Tamgu* Tamgutreemapss::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapss* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapss*)Atom(true);

    if (b->isMapContainer()) {
        map<string,string>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapss*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringOr(it.second,v);
    return res;
}

Exporting Tamgu* Tamgutreemapss::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapss* res;
    if (b->isMapContainer()) {
        map<string,string>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapss;
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
        res = (Tamgutreemapss*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringAnd(it.second,v);
    return res;
}

Exporting Tamgu* Tamgutreemapss::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapss* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapss;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
            try {
                res->values[k] = values.at(k) + itr->Valuestring();
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
        res = (Tamgutreemapss*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgutreemapss::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapss * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapss;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
            try {
                res->values[k] = StringMinus(values.at(k), itr->Valuestring());
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
        res = (Tamgutreemapss*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Tamgu* Tamgutreemapss::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    map<string, string>::iterator it;
    
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


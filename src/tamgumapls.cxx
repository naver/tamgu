/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapls.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgumapls.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<maplsMethod>  Tamgumapls::methods;
Exporting hmap<string, string> Tamgumapls::infomethods;
Exporting basebin_hash<unsigned long> Tamgumapls::exported;

Exporting short Tamgumapls::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapls::AddMethod(TamguGlobal* global, string name,maplsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapls::Setidtype(TamguGlobal* global) {
    Tamgumapls::InitialisationModule(global,"");
}


   bool Tamgumapls::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgumapls::idtype = global->Getid("mapls");

    
    global->minimal_indexes[Tamgumapls::idtype] = true;

    Tamgumapls::AddMethod(global, "clear", &Tamgumapls::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapls::AddMethod(global, "invert", &Tamgumapls::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapls::AddMethod(global, "find", &Tamgumapls::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapls::AddMethod(global, "items", &Tamgumapls::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapls::AddMethod(global, "join", &Tamgumapls::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapls::AddMethod(global, "test", &Tamgumapls::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapls::AddMethod(global, "keys", &Tamgumapls::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapls::AddMethod(global, "values", &Tamgumapls::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapls::AddMethod(global, "pop", &Tamgumapls::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapls::AddMethod(global, "merge", &Tamgumapls::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgumapls::idtype] = new Tamgumapls(global);
        
        global->RecordMethods(Tamgumapls::idtype, Tamgumapls::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgumapls::Newiteration(bool direction) {
    return new TamguIterationmapls(this, direction);
}


Exporting Tamgu* Tamgumapls::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases aBLONG the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    BLONG val = a->Long();

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
        Doublelocking _lock(this, v);
        try {
            values.at(val);
            v->values.push_back(val);
        }
        catch(const std::out_of_range& oor) {}

        return v;
    }

    if (context->isNumber()) {
        locking();
        try {
            values.at(val);
            unlocking();
            return globalTamgu->Providelong(val);
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


Exporting Tamgu* Tamgumapls::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases aBLONG the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (const auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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
            return globalTamgu->Providelong(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapls::Push(Tamgu* k, Tamgu* v) {
    locking();
    BLONG s = k->Long();
    values[s] = v->String();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgumapls::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgumapls::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgumapls::String() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        stringing(res, it.second);
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgumapls::JSonString() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":";
        jstringing(res, it.second);
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgumapls::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapls::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapls::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapls::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapls::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapls::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapls::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapls*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keylong()] = itr->Valuestring();
            itr->Release();
            return aTRUE;
        }
        
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getstring(it);
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
                values[nb] = it->String();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgumapls::idtype)
            values = ((Tamgumapls*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keylong()] = itr->Valuestring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getlong(idthread)] = ke->String();
    unlocking();
    return aTRUE;
}



Tamgu* Tamgumapls::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    BLONG skey = key->Getlong(idthread);

    Tamgu* val = Value(skey);
    if (val == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return val;
}

Exporting Tamgu* Tamgumapls::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            hmap<BLONG,string>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providelong(it->first));
            unlocking();
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalTamgu->Providelong(values.size());

        return this;
    }

    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgumapls* kmap = new Tamgumapls;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        hmap<BLONG,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<BLONG, string>::iterator itr= values.end();
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

    BLONG skey = idx->Getlong(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumapls::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapls* m = (Tamgumapls*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<BLONG,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapls::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgumapls* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapls;
        hmap<BLONG, string> keys;

        for (const auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
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

    if (itself)
        res = this;
    else
        res = (Tamgumapls*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringXor(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapls::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapls* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapls*)Atom(true);

    if (b->isMapContainer()) {
        hmap<BLONG,string>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapls*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringOr(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapls::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapls* res;
    if (b->isMapContainer()) {
        hmap<BLONG,string>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapls;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]==v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapls*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringAnd(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapls::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapls* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapls;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapls*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapls::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapls * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapls;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumapls*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapls::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<BLONG, string>::iterator it;
    
    Tamgu* a;
    vector<BLONG> keys;

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

